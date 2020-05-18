// SPDX-License-Identifier: GPL-2.0+
/**
 * The JESD204 framework - finite state machine logic
 *
 * Copyright (c) 2019 Analog Devices Inc.
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/of.h>

#include "jesd204-priv.h"

#define JESD204_LINKS_ALL	((unsigned int)-1)

#define JESD204_FSM_BUSY	BIT(0)

extern struct list_head jesd204_topologies;

typedef int (*jesd204_fsm_cb)(struct jesd204_dev *jdev,
			      struct jesd204_link_opaque *ol,
			      struct jesd204_dev_con_out *con,
			      struct jesd204_fsm_data *data);

typedef int (*jesd204_fsm_done_cb)(struct jesd204_dev *jdev,
				   struct jesd204_link_opaque *ol,
				   void *data);
/**
 * struct jesd204_fsm_data - JESD204 device state change data
 * @jdev_top		top JESD204 for which this state change
 * @link_idx		JESD204 link index for this transition
 * @fsm_change_cb	callback to propagate during a state transition
 * @fsm_complete_cb	callback to run after a state transition has completed
 * @cb_data		callback data for @fsm_change_cb
 * @cur_state		current state from which this FSM is transitioning
 * @nxt_state		next state to which this FSM is transitioning
 * @inputs		true if this is running on the inputs
 */
struct jesd204_fsm_data {
	struct jesd204_dev_top		*jdev_top;
	unsigned int			link_idx;
	jesd204_fsm_cb			fsm_change_cb;
	jesd204_fsm_done_cb		fsm_complete_cb;
	void				*cb_data;
	enum jesd204_dev_state		cur_state;
	enum jesd204_dev_state		nxt_state;
	bool				inputs;
};

typedef int (*jesd204_propagated_cb)(struct jesd204_dev *jdev,
				     struct jesd204_dev_con_out *con,
				     struct jesd204_fsm_data *data);

/**
 * struct jesd204_fsm_table_entry - JESD204 link states table entry
 * @state		target JESD204 state
 * @op			callback ID associated with transitioning to @state
 * @last		marker for the last state in the transition series
 */
struct jesd204_fsm_table_entry {
	enum jesd204_dev_state	state;
	enum jesd204_dev_op	op;
	bool			last;
};

struct jesd204_fsm_table_entry_iter {
	const struct jesd204_fsm_table_entry	*table;
	unsigned int				link_idx;
};

#define _JESD204_STATE_OP(x, _last)	\
{					\
	.state = JESD204_STATE_##x,	\
	.op = JESD204_OP_##x,		\
	.last = _last			\
}
#define JESD204_STATE_OP(x)		_JESD204_STATE_OP(x, false)
#define JESD204_STATE_OP_LAST(x)	_JESD204_STATE_OP(x, true)

static int jesd204_fsm_table(struct jesd204_dev *jdev,
			     unsigned int link_idx,
			     enum jesd204_dev_state init_state,
			     const struct jesd204_fsm_table_entry *table);

/* States to transition to initialize a JESD204 link */
static const struct jesd204_fsm_table_entry jesd204_init_links_states[] = {
	JESD204_STATE_OP_LAST(LINK_INIT),
};

/* States to transition to start a JESD204 link */
static const struct jesd204_fsm_table_entry jesd204_start_links_states[] = {
	JESD204_STATE_OP(LINK_SUPPORTED),
	JESD204_STATE_OP(LINK_SETUP),
	JESD204_STATE_OP(CLOCKS_ENABLE),
	JESD204_STATE_OP(LINK_ENABLE),
	JESD204_STATE_OP_LAST(LINK_RUNNING),
};

/* States to transition when unregistering a device */
static const struct jesd204_fsm_table_entry jesd204_unreg_dev_states[] = {
	JESD204_STATE_OP(LINK_DISABLE),
	JESD204_STATE_OP(CLOCKS_DISABLE),
	JESD204_STATE_OP_LAST(LINK_UNINIT),
};

const char *jesd204_state_str(enum jesd204_dev_state state)
{
	switch (state) {
	case JESD204_STATE_ERROR:
		return "error";
	case JESD204_STATE_UNINIT:
		return "uninitialized";
	case JESD204_STATE_INITIALIZED:
		return "initialized";
	case JESD204_STATE_PROBED:
		return "probed";
	case JESD204_STATE_LINK_INIT:
		return "link_init";
	case JESD204_STATE_LINK_SUPPORTED:
		return "link_supported";
	case JESD204_STATE_LINK_SETUP:
		return "link_setup";
	case JESD204_STATE_CLOCKS_ENABLE:
		return "clocks_enable";
	case JESD204_STATE_CLOCKS_DISABLE:
		return "clocks_disable";
	case JESD204_STATE_LINK_ENABLE:
		return "link_enable";
	case JESD204_STATE_LINK_DISABLE:
		return "link_disable";
	case JESD204_STATE_LINK_RUNNING:
		return "link_running";
	case JESD204_STATE_LINK_UNINIT:
		return "link_uninit";
	case JESD204_STATE_DONT_CARE:
		return "dont_care";
	default:
		return "<unknown>";
	}
}

static int jesd204_con_link_idx_in_jdev_top(struct jesd204_dev_con_out *con,
					    struct jesd204_dev_top *jdev_top)
{
	int i;

	if (con->topo_id != jdev_top->topo_id)
		return -EINVAL;

	for (i = 0; i < jdev_top->num_links; i++) {
		if (jdev_top->link_ids[i] == con->link_id)
			return i;
	}

	return -EINVAL;
}

static int jesd204_dev_set_error(struct jesd204_link_opaque *ol,
				 struct jesd204_dev_con_out *con,
				 int err)
{
	/* FIXME: should we exit here? */
	if (err == 0)
		return 0;

	if (con) {
		con->error = err;
		con->state = JESD204_STATE_ERROR;
	}

	if (ol) {
		ol->error = err;
		ol->state = JESD204_STATE_ERROR;
	}

	return err;
}

static int jesd204_dev_propagate_cb_inputs(struct jesd204_dev *jdev,
					   jesd204_propagated_cb propagated_cb,
					   struct jesd204_fsm_data *data)
{
	struct jesd204_dev_con_out *con = NULL;
	unsigned int i;
	int ret = 0;

	for (i = 0; i < jdev->inputs_count; i++) {
		con = jdev->inputs[i];

		if (data->link_idx != JESD204_LINKS_ALL &&
		    data->link_idx != con->link_id)
			continue;

		ret = jesd204_dev_propagate_cb_inputs(con->owner,
						      propagated_cb, data);
		if (ret)
			break;
		ret = propagated_cb(con->owner, con, data);
		if (ret)
			break;
	}

	return ret;
}

static int jesd204_dev_propagate_cb_outputs(struct jesd204_dev *jdev,
					    jesd204_propagated_cb propagated_cb,
					    struct jesd204_fsm_data *data)
{
	struct jesd204_dev_con_out *con = NULL;
	struct jesd204_dev_list_entry *e;
	int ret = 0;

	list_for_each_entry(con, &jdev->outputs, entry) {
		list_for_each_entry(e, &con->dests, entry) {
			if (data->link_idx != JESD204_LINKS_ALL &&
			    data->link_idx != con->link_id)
				continue;

			ret = propagated_cb(e->jdev, con, data);
			if (ret)
				goto done;
			ret = jesd204_dev_propagate_cb_outputs(e->jdev,
							       propagated_cb,
							       data);
			if (ret)
				goto done;
		}
	}

done:
	return ret;
}

static int jesd204_dev_propagate_cb(struct jesd204_dev *jdev,
				    jesd204_propagated_cb propagated_cb,
				    struct jesd204_fsm_data *data)
{
	int ret;

	data->inputs = true;
	ret = jesd204_dev_propagate_cb_inputs(jdev, propagated_cb, data);
	if (ret)
		goto out;

	data->inputs = false;
	ret = jesd204_dev_propagate_cb_outputs(jdev, propagated_cb, data);
	if (ret)
		goto out;

	ret = propagated_cb(jdev, NULL, data);
	/* FIXME: error message here? */
out:
	return ret;
}

static void __jesd204_link_fsm_done_cb(struct kref *ref)
{
	struct jesd204_fsm_data *fsm_data;
	struct jesd204_link_opaque *ol;
	struct jesd204_dev *jdev;
	int ret;

	ol = container_of(ref, typeof(*ol), cb_ref);
	jdev = &ol->jdev_top->jdev;
	fsm_data = ol->fsm_data;

	/* clear out this transition information, to make way for the next */
	clear_bit(JESD204_FSM_BUSY, &ol->flags);
	ol->fsm_data = NULL;

	if (ol->error) {
		dev_err(jdev->parent, "jesd got error from topology %d\n",
			ol->error);
		return;
	}

	dev_info(jdev->parent, "JESD204 link[%u] transition %s -> %s\n",
		 ol->link_idx,
		 jesd204_state_str(fsm_data->cur_state),
		 jesd204_state_str(fsm_data->nxt_state));
	ol->state = fsm_data->nxt_state;

	if (!fsm_data->fsm_complete_cb)
		return;

	ret = fsm_data->fsm_complete_cb(jdev, ol, fsm_data->cb_data);
	if (jesd204_dev_set_error(ol, NULL, ret)) {
		dev_err(jdev->parent,
			"error from completion cb %d, state %s\n",
			ret,
			jesd204_state_str(ol->state));
	}
}

static void __jesd204_fsm_kref_link_put_get(struct jesd204_dev_top *jdev_top,
					    unsigned int link_idx,
					    bool put)
{
	struct jesd204_link_opaque *ol;

	if (link_idx != JESD204_LINKS_ALL) {
		ol = &jdev_top->active_links[link_idx];
		if (put)
			kref_put(&ol->cb_ref, __jesd204_link_fsm_done_cb);
		else
			kref_get(&ol->cb_ref);
		return;
	}

	for (link_idx = 0; link_idx < jdev_top->num_links; link_idx++) {
		ol = &jdev_top->active_links[link_idx];
		kref_put(&ol->cb_ref, __jesd204_link_fsm_done_cb);
		if (put)
			kref_put(&ol->cb_ref, __jesd204_link_fsm_done_cb);
		else
			kref_get(&ol->cb_ref);
	}
}

static void jesd204_fsm_kref_link_get(struct jesd204_dev_top *jdev_top,
				      unsigned int link_idx)
{
	__jesd204_fsm_kref_link_put_get(jdev_top, link_idx, false);
}

static void jesd204_fsm_kref_link_put(struct jesd204_dev_top *jdev_top,
				      unsigned int link_idx)
{
	__jesd204_fsm_kref_link_put_get(jdev_top, link_idx, true);
}

static int jesd204_dev_validate_cur_state(struct jesd204_dev *jdev,
					  struct jesd204_link_opaque *ol,
					  struct jesd204_dev_con_out *c,
					  enum jesd204_dev_state cur_state,
					  enum jesd204_dev_state nxt_state)
{
	if (cur_state == JESD204_STATE_DONT_CARE)
		return 0;

	if (cur_state != ol->state) {
		dev_warn(jdev->parent,
			 "JESD204 link[%d] invalid link state: %s, exp: %s, nxt: %s\n",
			 ol->link_idx,
			 jesd204_state_str(ol->state),
			 jesd204_state_str(cur_state),
			 jesd204_state_str(nxt_state));
		return jesd204_dev_set_error(ol, c, -EINVAL);
	}

	if (c && c->link_idx < 0) {
		dev_warn(jdev->parent,
			 "JESD204 link[%d] connection link index unitialized\n",
			 ol->link_idx);
		return jesd204_dev_set_error(ol, c, -EINVAL);
	}

	if (c && cur_state != c->state) {
		dev_warn(jdev->parent,
			 "JESD204 link[%d] invalid connection state: %s, exp: %s, nxt: %s\n",
			 ol->link_idx,
			 jesd204_state_str(c->state),
			 jesd204_state_str(cur_state),
			 jesd204_state_str(nxt_state));
		return jesd204_dev_set_error(ol, c, -EINVAL);
	}

	return 0;
}

static int jesd204_dev_update_con_state(struct jesd204_dev *jdev,
					struct jesd204_link_opaque *ol,
					struct jesd204_dev_con_out *c,
					struct jesd204_fsm_data *fsm_data)
{
	int ret;

	if (!c || c->state == fsm_data->nxt_state)
		return 0;

	ret = jesd204_dev_validate_cur_state(jdev, ol, c,
					     fsm_data->cur_state,
					     fsm_data->nxt_state);
	if (ret)
		return ret;

	c->state = fsm_data->nxt_state;

	return 0;
}

static int jesd204_fsm_change_one_link_cb(struct jesd204_dev *jdev,
					  unsigned int link_idx,
					  struct jesd204_dev_con_out *con,
					  struct jesd204_fsm_data *fsm_data)
{
	struct jesd204_dev_top *jdev_top = fsm_data->jdev_top;
	struct jesd204_link_opaque *ol = &jdev_top->active_links[link_idx];
	int ret;

	jesd204_fsm_kref_link_get(jdev_top, link_idx);

	ret = fsm_data->fsm_change_cb(jdev, ol, con, fsm_data);
	if (ret < 0) {
		dev_err(jdev->parent,
			"JESD204 link[%u] got error from cb: %d\n",
			link_idx, ret);
		return ret;
	}

	if (ret != JESD204_STATE_CHANGE_DONE)
		return ret;

	ret = jesd204_dev_update_con_state(jdev, ol, con, fsm_data);
	if (ret)
		return ret;

	jesd204_fsm_kref_link_put(jdev_top, link_idx);
	return 0;
}

static int jesd204_fsm_change_cb(struct jesd204_dev *jdev,
				 struct jesd204_dev_con_out *con,
				 struct jesd204_fsm_data *fsm_data)
{
	struct jesd204_dev_top *jdev_top;
	unsigned int link_idx;
	int ret;

	if (!fsm_data->fsm_change_cb)
		return 0;

	jdev_top = fsm_data->jdev_top;
	link_idx = fsm_data->link_idx;

	if (link_idx != JESD204_LINKS_ALL)
		return jesd204_fsm_change_one_link_cb(jdev, link_idx,
						      con, fsm_data);

	for (link_idx = 0; link_idx < jdev_top->num_links; link_idx++) {
		ret = jesd204_fsm_change_one_link_cb(jdev, link_idx,
						     con, fsm_data);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int jesd204_fsm_link_init(struct jesd204_dev_top *jdev_top,
				 struct jesd204_fsm_data *fsm_data,
				 unsigned int link_idx)
{
	struct jesd204_link_opaque *ol;

	if (link_idx != JESD204_LINKS_ALL) {
		ol = &jdev_top->active_links[link_idx];
		ol->fsm_data = fsm_data;
		kref_init(&ol->cb_ref);
		return 0;
	}

	for (link_idx = 0; link_idx < jdev_top->num_links; link_idx++) {
		ol = &jdev_top->active_links[link_idx];
		ol->fsm_data = fsm_data;
		kref_init(&ol->cb_ref);
	}

	return 0;
}

static int jesd204_fsm_test_and_set_busy(struct jesd204_dev_top *jdev_top,
					 enum jesd204_dev_state cur_state,
					 unsigned int link_idx)
{
	struct jesd204_dev *jdev = &jdev_top->jdev;
	struct jesd204_link_opaque *ol;

	/* ignore if the transition is busy */
	if (cur_state == JESD204_STATE_DONT_CARE)
		return 0;

	if (link_idx != JESD204_LINKS_ALL) {
		ol = &jdev_top->active_links[link_idx];
		if (test_and_set_bit(JESD204_FSM_BUSY, &ol->flags)) {
			dev_err(jdev->parent, "JESD204 link [%u]: FSM is busy\n",
				ol->link_idx);
			return -EBUSY;
		}
		return 0;
	}

	for (link_idx = 0; link_idx < jdev_top->num_links; link_idx++) {
		ol = &jdev_top->active_links[link_idx];
		if (test_and_set_bit(JESD204_FSM_BUSY, &ol->flags)) {
			dev_err(jdev->parent, "JESD204 link [%u]: FSM is busy\n",
				ol->link_idx);
			goto err_unwind_busy;
		}
	}

	return 0;

err_unwind_busy:
	for (; link_idx >= 0; link_idx--) {
		ol = &jdev_top->active_links[link_idx];
		clear_bit(JESD204_FSM_BUSY, &ol->flags);
	}

	return -EBUSY;
}

static void jesd204_fsm_clear_busy(struct jesd204_dev_top *jdev_top,
				   unsigned int link_idx)
{
	struct jesd204_link_opaque *ol;

	if (link_idx != JESD204_LINKS_ALL) {
		ol = &jdev_top->active_links[link_idx];
		clear_bit(JESD204_FSM_BUSY, &ol->flags);
		return;
	}

	for (link_idx = 0; link_idx < jdev_top->num_links; link_idx++) {
		ol = &jdev_top->active_links[link_idx];
		clear_bit(JESD204_FSM_BUSY, &ol->flags);
	}
}

static int jesd204_dev_validate_lnk_states(struct jesd204_dev_top *jdev_top,
					   unsigned int link_idx,
					   enum jesd204_dev_state cur_state,
					   enum jesd204_dev_state nxt_state)
{
	struct jesd204_dev *jdev = &jdev_top->jdev;
	struct jesd204_link_opaque *ol;
	int ret;

	if (link_idx != JESD204_LINKS_ALL) {
		ol = &jdev_top->active_links[link_idx];
		return jesd204_dev_validate_cur_state(jdev, ol, NULL,
						      cur_state, nxt_state);
	}

	for (link_idx = 0; link_idx < jdev_top->num_links; link_idx++) {
		ol = &jdev_top->active_links[link_idx];
		ret = jesd204_dev_validate_cur_state(jdev, ol, NULL,
						     cur_state, nxt_state);
		if (ret)
			return ret;
	}

	return 0;
}

static int __jesd204_fsm(struct jesd204_dev *jdev,
			 struct jesd204_dev_top *jdev_top,
			 unsigned int link_idx,
			 enum jesd204_dev_state cur_state,
			 enum jesd204_dev_state nxt_state,
			 jesd204_fsm_cb fsm_change_cb,
			 void *cb_data,
			 jesd204_fsm_done_cb fsm_complete_cb)
{
	struct jesd204_fsm_data data;
	int ret;

	ret = jesd204_fsm_test_and_set_busy(jdev_top, cur_state, link_idx);
	if (ret)
		return ret;

	ret = jesd204_dev_validate_lnk_states(jdev_top, link_idx,
					      cur_state, nxt_state);
	if (ret)
		goto out_clear_busy;

	memset(&data, 0, sizeof(data));
	data.jdev_top = jdev_top;
	data.cur_state = cur_state;
	data.nxt_state = nxt_state;
	data.fsm_change_cb = fsm_change_cb;
	data.fsm_complete_cb = fsm_complete_cb;
	data.cb_data = cb_data;
	data.link_idx = link_idx;

	ret = jesd204_fsm_link_init(jdev_top, &data, link_idx);
	if (ret)
		goto out_clear_busy;

	ret = jesd204_dev_propagate_cb(jdev,
				       jesd204_fsm_change_cb,
				       &data);

	jesd204_fsm_kref_link_put(jdev_top, link_idx);

out_clear_busy:
	jesd204_fsm_clear_busy(jdev_top, link_idx);

	return ret;
}

static int jesd204_fsm(struct jesd204_dev *jdev,
		       unsigned int link_idx,
		       enum jesd204_dev_state cur_state,
		       enum jesd204_dev_state nxt_state,
		       jesd204_fsm_cb fsm_change_cb,
		       void *cb_data,
		       jesd204_fsm_done_cb fsm_complete_cb)
{
	struct list_head *jesd204_topologies = jesd204_topologies_get();
	struct jesd204_dev_top *jdev_top = jesd204_dev_top_dev(jdev);
	int ret;

	if (jdev_top)
		return __jesd204_fsm(jdev, jdev_top, link_idx,
				     cur_state, nxt_state, fsm_change_cb,
				     cb_data, fsm_complete_cb);

	list_for_each_entry(jdev_top, jesd204_topologies, entry) {
		ret = __jesd204_fsm(jdev, jdev_top, link_idx,
				    cur_state, nxt_state, fsm_change_cb,
				    cb_data, fsm_complete_cb);
		if (ret)
			return ret;
	}

	return 0;
}

#if 0
/**
 * FIXME: paranoid FSM check; checks if a link id has the correct index.
 * remove it later maybe?
 */
static int jesd204_paranoid_cb_check(struct jesd204_dev *jdev,
				     struct jesd204_link_opaque *ol,
				     struct jesd204_dev_con_out *con,
				     struct jesd204_fsm_data *fsm_data)
{
	struct jesd204_dev_top *jdev_top = fsm_data->jdev_top;

	if (!jdev) {
		pr_err("jdev object is null\n");
		return -EFAULT;
	}

	if (fsm_data->link_idx >= jdev_top->num_links) {
		dev_err(&jdev->dev, "fsm_data->link_idx >= jdev_top->num_links %u\n",
			fsm_data->link_idx);
		return -EFAULT;
	}

	if (con->link_id != jdev_top->active_links[fsm_data->link_idx]) {
		dev_err(&jdev->dev, "ol->link.link_id != jdev_top->active_links[fsm_data->link_idx]\n");
		return -EFAULT;
	}

	/* FIXME: add more? */

	return 0;
}
#endif

static int jesd204_dev_initialize_cb(struct jesd204_dev *jdev,
				     struct jesd204_link_opaque *ol,
				     struct jesd204_dev_con_out *con,
				     struct jesd204_fsm_data *fsm_data)
{
	int ret;

	if (!con)
		return JESD204_STATE_CHANGE_DONE;

//	int ret = jesd204_paranoid_cb_check(jdev, ol, con, fsm_data);

//	if (ret)
//		return ret;

	ret = jesd204_con_link_idx_in_jdev_top(con, fsm_data->jdev_top);
	if (ret >= 0) {
		con->jdev_top = fsm_data->jdev_top;
		con->link_idx = ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

int jesd204_init_topology(struct jesd204_dev_top *jdev_top)
{
	if (!jdev_top)
		return -EINVAL;

	return jesd204_fsm(&jdev_top->jdev, JESD204_LINKS_ALL,
			   JESD204_STATE_UNINIT, JESD204_STATE_INITIALIZED,
			   jesd204_dev_initialize_cb, jdev_top, NULL);
}

static int jesd204_fsm_init_link(struct jesd204_dev *jdev,
				 unsigned int link_idx,
				 enum jesd204_dev_state init_state)
{
	int ret;

	ret = jesd204_fsm_table(jdev, link_idx,
				init_state, jesd204_init_links_states);
	if (ret)
		return ret;

	return jesd204_dev_init_link_data(jdev, link_idx);
}

static int jesd204_fsm_start_link(struct jesd204_dev *jdev,
				  unsigned int link_idx,
				  enum jesd204_dev_state init_state)
{
	return jesd204_fsm_table(jdev, link_idx,
				 init_state, jesd204_start_links_states);
}

static int jesd204_fsm_probed_cb(struct jesd204_dev *jdev,
				 struct jesd204_link_opaque *ol,
				 struct jesd204_dev_con_out *con,
				 struct jesd204_fsm_data *fsm_data)
{
	if (!jdev->parent)
		return JESD204_STATE_CHANGE_DEFER;
	return JESD204_STATE_CHANGE_DONE;
}

static int jesd204_fsm_probe_done(struct jesd204_dev *jdev,
				  struct jesd204_link_opaque *ol,
				  void *data)
{
	int ret;

	ret = jesd204_fsm_init_link(jdev, ol->link_idx, JESD204_STATE_PROBED);
	if (ret)
		return ret;

	return jesd204_fsm_start_link(jdev, ol->link_idx, JESD204_STATE_LINK_INIT);
}

int jesd204_fsm_probe(struct jesd204_dev *jdev)
{
	return jesd204_fsm(jdev, JESD204_LINKS_ALL,
			   JESD204_STATE_INITIALIZED, JESD204_STATE_PROBED,
			   jesd204_fsm_probed_cb, NULL, jesd204_fsm_probe_done);
}

static int jesd204_fsm_table_entry_cb(struct jesd204_dev *jdev,
				      struct jesd204_link_opaque *ol,
				      struct jesd204_dev_con_out *con,
				      struct jesd204_fsm_data *fsm_data)
{
	struct jesd204_fsm_table_entry_iter *it = fsm_data->cb_data;
	jesd204_link_cb link_op;

	if (!jdev->link_ops)
		return JESD204_STATE_CHANGE_DONE;

	link_op = jdev->link_ops[it->table[0].op];
	if (!link_op)
		return JESD204_STATE_CHANGE_DONE;

	return link_op(jdev, ol->link_idx, &ol->link);
}

static int jesd204_fsm_table_entry_done(struct jesd204_dev *jdev,
					struct jesd204_link_opaque *ol,
					void *data)
{
	struct jesd204_fsm_table_entry_iter *it = data;
	const struct jesd204_fsm_table_entry *table = it->table;

	if (table[0].last)
		return 0;

	return jesd204_fsm_table(jdev, it->link_idx, table[0].state, &table[1]);
}

static int jesd204_fsm_table(struct jesd204_dev *jdev,
			     unsigned int link_idx,
			     enum jesd204_dev_state init_state,
			     const struct jesd204_fsm_table_entry *table)
{
	struct jesd204_fsm_table_entry_iter it;

	it.link_idx = link_idx;
	it.table = table;

	return jesd204_fsm(jdev, link_idx,
			   init_state, table[0].state,
			   jesd204_fsm_table_entry_cb,
			   &it,
			   jesd204_fsm_table_entry_done);
}

void jesd204_fsm_unreg_device(struct jesd204_dev *jdev)
{
	jesd204_fsm_table(jdev, JESD204_LINKS_ALL,
			  JESD204_STATE_DONT_CARE, jesd204_unreg_dev_states);
}

int jesd204_fsm_link_change(struct jesd204_dev_top *jdev_top,
			    unsigned int link_idx)
{
	struct jesd204_link_opaque *oal, *osl;
	struct jesd204_link *al, *sl;
	int ret;

	if (link_idx >= jdev_top->num_links)
		return -EINVAL;

	oal = &jdev_top->active_links[link_idx];
	osl = &jdev_top->staged_links[link_idx];

	al = &oal->link;
	sl = &osl->link;

	/* If no links staged, there is nothing to do */
	if (memcmp(al, sl, sizeof(*al)) == 0)
		return 0;

	ret = jesd204_fsm_table(&jdev_top->jdev, link_idx,
				oal->state,
				jesd204_unreg_dev_states);
	if (ret)
		return ret;

	if (!sl->enabled)
		goto save_link_settings;

	ret = jesd204_fsm_table(&jdev_top->jdev, link_idx,
				oal->state,
				jesd204_start_links_states);
	if (ret)
		return ret;

save_link_settings:
	/* Save new active link settings */
	memcpy(al, sl, sizeof(*al));

	return 0;
}
