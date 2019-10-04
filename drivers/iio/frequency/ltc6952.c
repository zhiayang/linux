// SPDX-License-Identifier: GPL-2.0

#include <linux/bitfield.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/gcd.h>
#include <linux/rational.h>
#include <linux/debugfs.h>

#include <linux/clk.h>
#include <linux/clk-provider.h>

#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>

/* Register address macro */
#define LTC6952_REG(x)		(x)

/* LTC6952_REG0 */
#define LTC6952_UNLOCK_MSK	BIT(6)
#define LTC6952_LOCK_MSK	BIT(4)
#define	LTC6952_VCOOK_MSK	BIT(2)
#define LTC6952_REFOK_MSK	BIT(0)

/* LTC6952_REG1 */
#define LTC6952_INVSTAT_MSK	BIT(7)
#define LTC6952_INVSTAT(x)	FIELD_PREP(LTC6952_INVSTAT_MSK, x)
#define LTC6952_STAT_OUT_MSK	GENMASK(6, 0)
#define	LTC6952_STAT_OUT(x)	FIELD_PREP(LTC6952_STAT_OUT_MSK, x)

/* LTC6952_REG2 */
#define LTC6952_PDALL_MSK	BIT(7)
#define LTC6952_PDALL(x)	FIELD_PREP(LTC6952_PDALL_MSK, x)
#define LTC6952_PDPLL_MSK	BIT(6)
#define	LTC6952_PDPLL(x)	FIELD_PREP(LTC6952_PDPLL_MSK, x)
#define LTC6952_PDVCOPK_MSK	BIT(5)
#define	LTC6952_PDVCOPK(x)	FIELD_PREP(LTC6952_PDVCOPK_MSK, x)
#define LTC6952_PDREFPK_MSK	BIT(4)
#define	LTC6952_PDREFPK(x)	FIELD_PREP(LTC6952_PDREFPK_MSK, x)
#define LTC6952_BST_MSK		BIT(3)
#define	LTC6952_BST(x)		FIELD_PREP(LTC6952_BST_MSK, x)
#define LTC6952_FILTR_MSK	BIT(2)
#define	LTC6952_FILTR(x)	FIELD_PREP(LTC6952_FILTR_MSK, x)
#define LTC6952_FILTV_MSK	BIT(1)
#define	LTC6952_FILTV(x)	FIELD_PREP(LTC6952_FILTV_MSK, x)
#define LTC6952_POR_MSK		BIT(0)
#define	LTC6952_POR(x)		FIELD_PREP(LTC6952_POR_MSK, x)

/* LTC6952_REG3 */
#define LTC6952_PD3_MSK		GENMASK(7, 6)
#define	LTC6952_PD3(x)		FIELD_PREP(LTC6952_PD3_MSK, x)
#define LTC6952_PD2_MSK		GENMASK(5, 4)
#define	LTC6952_PD2(x)		FIELD_PREP(LTC6952_PD2_MSK, x)
#define LTC6952_PD1_MSK		GENMASK(3, 2)
#define	LTC6952_PD1(x)		FIELD_PREP(LTC6952_PD1_MSK, x)
#define LTC6952_PD0_MSK		GENMASK(1, 0)
#define	LTC6952_PD0(x)		FIELD_PREP(LTC6952_PD0_MSK, x)

/* LTC6952_REG4 */
#define LTC6952_PD7_MSK		GENMASK(7, 6)
#define	LTC6952_PD7(x)		FIELD_PREP(LTC6952_PD7_MSK, x)
#define LTC6952_PD6_MSK		GENMASK(5, 4)
#define	LTC6952_PD6(x)		FIELD_PREP(LTC6952_PD6_MSK, x)
#define LTC6952_PD5_MSK		GENMASK(3, 2)
#define	LTC6952_PD5(x)		FIELD_PREP(LTC6952_PD5_MSK, x)
#define LTC6952_PD4_MSK		GENMASK(1, 0)
#define	LTC6952_PD4(x)		FIELD_PREP(LTC6952_PD4_MSK, x)

/* LTC6952_REG5 */
#define LTC6952_TEMPO_MSK	BIT(7)
#define LTC6952_TEMPO(x)	FIELD_PREP(LTC6952_TEMPO_MSK, x)
#define LTC6952_PD10_MSK	GENMASK(5, 4)
#define	LTC6952_PD10(x)		FIELD_PREP(LTC6952_PD10_MSK, x)
#define LTC6952_PD9_MSK		GENMASK(3, 2)
#define	LTC6952_PD9(x)		FIELD_PREP(LTC6952_PD9_MSK, x)
#define LTC6952_PD8_MSK		GENMASK(1, 0)
#define	LTC6952_PD8(x)		FIELD_PREP(LTC6952_PD8_MSK, x)

#define LTC6952_PD(ch, x)	((x) << ((ch) & 0x03))

/* LTC6952_REG6 */
#define LTC6952_RAO_MSK		BIT(7)
#define LTC6952_RAO(x)		FIELD_PREP(LTC6952_RAO_MSK, x)
#define LTC6952_PARSYNC_MSK	BIT(6)
#define LTC6952_PARSYNC(x)	FIELD_PREP(LTC6952_PARSYNC_MSK, x)
#define LTC6952_LKWIN_MSK	BIT(4)
#define LTC6952_LKWIN(x)	FIELD_PREP(LTC6952_LKWIN_MSK, x)
#define LTC6952_LKCT_MSK	GENMASK(3, 2)
#define	LTC6952_LKCT(x)		FIELD_PREP(LTC6952_LKCT_MSK, x)
#define LTC6952_RD_HIGH_MSK	GENMASK(1, 0)
#define	LTC6952_RD_HIGH(x)	FIELD_PREP(LTC6952_RD_HIGH_MSK, x)

/* LTC6952_REG7 */
#define LTC6952_RD_LOW_MSK	GENMASK(7, 0)
#define	LTC6952_RD_LOW(x)	FIELD_PREP(LTC6952_RD_LOW_MSK, x)

/* LTC6952_REG8 */
#define LTC6952_ND_HIGH_MSK	GENMASK(7, 0)
#define	LTC6952_ND_HIGH(x)	FIELD_PREP(LTC6952_ND_HIGH_MSK, x)

/* LTC6952_REG9 */
#define LTC6952_ND_LOW_MSK	GENMASK(7, 0)
#define	LTC6952_ND_LOW(x)	FIELD_PREP(LTC6952_ND_LOW_MSK, x)

/* LTC6952_REG10 */
#define LTC6952_CPRST_MSK	BIT(7)
#define	LTC6952_CPRST(x)	FIELD_PREP(LTC6952_CPRST_MSK, x)
#define LTC6952_CPUP_MSK	BIT(6)
#define	LTC6952_CPUP(x)		FIELD_PREP(LTC6952_CPUP_MSK, x)
#define LTC6952_CPDN_MSK	BIT(5)
#define	LTC6952_CPDN(x)		FIELD_PREP(LTC6952_CPDN_MSK, x)
#define LTC6952_CP_MSK		GENMASK(4, 0)
#define	LTC6952_CP(x)		FIELD_PREP(LTC6952_CP_MSK, x)

/* LTC6952_REG11 */
#define LTC6952_CPMID_MSK	BIT(7)
#define	LTC6952_CPMID(x)	FIELD_PREP(LTC6952_CPMID_MSK, x)
#define LTC6952_CPWIDE_MSK	BIT(6)
#define	LTC6952_CPWIDE(x)	FIELD_PREP(LTC6952_CPWIDE_MSK, x)
#define LTC6952_CPINV_MSK	BIT(5)
#define	LTC6952_CPINV(x)	FIELD_PREP(LTC6952_CPINV_MSK, x)
#define LTC6952_EZMD_MSK	BIT(4)
#define	LTC6952_EZMD(x)		FIELD_PREP(LTC6952_EZMD_MSK, x)
#define LTC6952_SRQMD_MSK	BIT(3)
#define	LTC6952_SRQMD(x)	FIELD_PREP(LTC6952_SRQMD_MSK, x)
#define LTC6952_SYSCT_MSK	GENMASK(2, 1)
#define	LTC6952_SYSCT(x)	FIELD_PREP(LTC6952_SYSCT_MSK, x)
#define LTC6952_SSRQ_MSK	BIT(0)
#define	LTC6952_SSRQ(x)		FIELD_PREP(LTC6952_SSRQ_MSK, x)

/* LTC6952_REG12 */
#define LTC6952_MP_MSK		GENMASK(7, 3)
#define	LTC6952_MP(x)		FIELD_PREP(LTC6952_MP_MSK, x)
#define LTC6952_MD_MSK		GENMASK(2, 0)
#define	LTC6952_MD(x)		FIELD_PREP(LTC6952_MD_MSK, x)

/* LTC6952_REG13 */
#define LTC6952_SRQEN_MSK	BIT(7)
#define	LTC6952_SRQEN(x)	FIELD_PREP(LTC6952_SRQEN_MSK, x)
#define LTC6952_MODE_MSK	GENMASK(6, 5)
#define	LTC6952_MODE(x)		FIELD_PREP(LTC6952_MODE_MSK, x)
#define LTC6952_OINV_MSK	BIT(4)
#define	LTC6952_OINV(x)		FIELD_PREP(LTC6952_OINV_MSK, x)
#define LTC6952_DDEL_HIGH_MSK	GENMASK(3, 0)
#define	LTC6952_DDEL_HIGH(x)	FIELD_PREP(LTC6952_DDEL_HIGH_MSK, x)

/* LTC6952_REG14 */
#define LTC6952_DDEL_LOW_MSK	GENMASK(7, 0)
#define	LTC6952_DDEL_LOW(x)	FIELD_PREP(LTC6952_DDEL_LOW_MSK, x)

/* LTC6952_REG15 */
#define LTC6952_ADEL_MSK	GENMASK(5, 0)
#define	LTC6952_ADEL(x)		FIELD_PREP(LTC6952_ADEL_MSK, x)

/* LTC6952_REG56 */
#define LTC6952_REV_MSK		GENMASK(7, 4)
#define LTC6952_PART_MSK	GENMASK(3, 0)

#define LTC6952_READ		0x1
#define LTC6952_WRITE		0x0
#define LTC6952_ADDR(x)		((x) & 0xFF << 1)

#define LTC6952_NUM_CHAN	11

#define LTC6952_N_MAX		65535
#define LTC6952_R_MAX		1023

#define LTC6952_PFD_FREQ_MAX	167000

#define LTC6952_OUT_DIV_MIN	1
#define LTC6952_OUT_DIV_MAX	1048576

struct ltc6952_output {
	unsigned int	address;
	struct clk_hw	hw;
	struct iio_dev	*indio_dev;
};

struct ltc6952_chan_spec {
	unsigned int		num;
	bool			digital_delay_en;
	bool			out_inversion_en;
	bool			srq_mode_en;
	unsigned int		out_divider;
	unsigned int		mp;
	unsigned int		md;
	unsigned int		digital_delay;
	unsigned int		analog_delay;
	unsigned int		sysref_mode;
	unsigned int		power_down_mode;
	const char		*extended_name;
};

struct ltc6952_state {
	struct spi_device		*spi;
	struct mutex			lock;
	u32				ref_freq;
	u32				vco_freq;
	u32				pfd_freq;
	bool				sysref_request;
	unsigned int			ref_divider;
	unsigned int			vco_divider;
	const char			*clk_out_names[LTC6952_NUM_CHAN];
	unsigned int			num_channels;
	struct ltc6952_chan_spec	*channels;
	struct iio_chan_spec		iio_channels[LTC6952_NUM_CHAN];
	struct ltc6952_output		outputs[LTC6952_NUM_CHAN];
	struct clk			*clks[LTC6952_NUM_CHAN];
	struct clk_onecell_data		clk_data;
};

#define to_output(_hw) container_of(_hw, struct ltc6952_output, hw)

static int ltc6952_write(struct iio_dev *indio_dev,
			 unsigned int reg,
			 unsigned int val)
{
	struct ltc6952_state *st = iio_priv(indio_dev);
	u8 buf[2];
	u8 cmd;

	cmd = LTC6952_WRITE | LTC6952_ADDR(reg);
	buf[0] = cmd;
	buf[1] = val;

	return spi_write(st->spi, buf, ARRAY_SIZE(buf));
}

static int ltc6952_read(struct iio_dev *indio_dev,
			unsigned int reg,
			unsigned int *val)
{
	struct ltc6952_state *st = iio_priv(indio_dev);
	u8 cmd;

	cmd = LTC6952_READ | LTC6952_ADDR(reg);

	return spi_write_then_read(st->spi, &cmd, 1, val, 1);
}

static unsigned int ltc6952_calc_out_div(unsigned long parent_rate,
					 unsigned long rate)
{
	unsigned int div;

	div = DIV_ROUND_CLOSEST(parent_rate, rate);

	div = clamp_t(unsigned int,
		      div,
		      LTC6952_OUT_DIV_MIN,
		      LTC6952_OUT_DIV_MAX);

	return div;
}

static void ltc6952_calculate_divider(struct ltc6952_chan_spec *chan)
{
	int i = 0, j = 0;

	/* M(x) = (MP(x) + 1)2^MD(x) */
	for (i = 0; i < 32; i++)
		for (j = 0; j < 8; j++)
			if (chan->out_divider <= (i + 1) << j)
				break;
	chan->mp = i;
	chan->md = j;
}

static struct attribute *ltc6952_attributes[] = {
	NULL,
};

static const struct attribute_group ltc6952_attribute_group = {
	.attrs = ltc6952_attributes,
};

static int ltc6952_read_raw(struct iio_dev *indio_dev,
			    struct iio_chan_spec const *chan,
			    int *val,
			    int *val2,
			    long mask)
{
	struct ltc6952_state *st = iio_priv(indio_dev);
	struct ltc6952_chan_spec *ch;
	unsigned int tmp1, tmp2, code;

	if (chan->address >= LTC6952_NUM_CHAN)
		return -EINVAL;

	ch = &st->channels[chan->address];

	switch (mask) {
	case IIO_CHAN_INFO_FREQUENCY:
		*val = st->vco_freq / ch->out_divider;
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_PHASE:
		ltc6952_read(indio_dev, 0x0D + 0x04 * chan->address,
			     &tmp1);
		ltc6952_read(indio_dev, 0x0E + 0x04 * chan->address,
			     &tmp2);

		tmp1 = (tmp1 & 0x0F) << 8;
		tmp2 &= 0xFF;
		code = DIV_ROUND_CLOSEST((tmp1 + tmp2) * 3141592,
					 ch->out_divider);
		*val = code / 1000000;
		*val2 = code % 1000000;
		return IIO_VAL_INT_PLUS_MICRO;
	default:
		return -EINVAL;
	}
};

static int ltc6952_write_raw(struct iio_dev *indio_dev,
			     struct iio_chan_spec const *chan,
			     int val,
			     int val2,
			     long mask)
{
	struct ltc6952_state *st = iio_priv(indio_dev);
	struct ltc6952_chan_spec *ch;
	unsigned int code, tmp;

	if (chan->address >= st->num_channels)
		return -EINVAL;

	ch = &st->channels[chan->address];

	switch (mask) {
	case IIO_CHAN_INFO_FREQUENCY:
		ch->out_divider = ltc6952_calc_out_div(st->vco_freq, val);
		ltc6952_calculate_divider(ch);
		mutex_lock(&st->lock);
		tmp = FIELD_PREP(LTC6952_MP_MSK, ch->mp) |
		      FIELD_PREP(LTC6952_MD_MSK, ch->md);
		ltc6952_write(indio_dev, 0x0D + 0x04 * chan->address,
			      tmp);
		mutex_unlock(&st->lock);
		break;
	case IIO_CHAN_INFO_PHASE:
		mutex_lock(&st->lock);
		code = val * 1000000 + val2 % 1000000;
		tmp = DIV_ROUND_CLOSEST(code * ch->out_divider, 3141592);
		tmp = clamp_t(unsigned int, tmp, 0, 4095);
		ltc6952_write(indio_dev, 0x0D + 0x04 * chan->address,
			      (tmp & 0xF00) >> 8);
		ltc6952_write(indio_dev, 0x0E + 0x04 * chan->address,
			      tmp & 0xFF);
		mutex_unlock(&st->lock);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int ltc6952_reg_access(struct iio_dev *indio_dev, unsigned int reg,
			      unsigned int writeval, unsigned int *readval)
{
	struct ltc6952_state *st = iio_priv(indio_dev);
	int ret;

	mutex_lock(&st->lock);
	if (readval)
		ret = ltc6952_read(indio_dev, reg, readval);
	else
		ret = ltc6952_write(indio_dev, reg, writeval);
	mutex_unlock(&st->lock);

	return ret;
}

static const struct iio_info ltc6952_iio_info = {
	.read_raw = &ltc6952_read_raw,
	.write_raw = &ltc6952_write_raw,
	.debugfs_reg_access = &ltc6952_reg_access,
	.attrs = &ltc6952_attribute_group,
	.driver_module = THIS_MODULE,
};

static long ltc6952_get_clk_attr(struct clk_hw *hw,
				 long mask)
{
	struct iio_dev *indio_dev = to_output(hw)->indio_dev;
	struct ltc6952_state *st = iio_priv(indio_dev);
	struct iio_chan_spec *chan;
	unsigned int address;
	int val, ret;

	address = to_output(hw)->address;
	if (address >= st->num_channels)
		return -EINVAL;

	chan = &st->iio_channels[address];

	ret = ltc6952_read_raw(indio_dev, chan, &val, NULL, mask);

	if (ret == IIO_VAL_INT)
		return val;

	return ret;
}

static long ltc6952_set_clk_attr(struct clk_hw *hw,
				 long mask,
				 unsigned long val)
{
	struct iio_dev *indio_dev = to_output(hw)->indio_dev;
	struct ltc6952_state *st = iio_priv(indio_dev);
	struct iio_chan_spec *chan;
	unsigned int address;

	address = to_output(hw)->address;
	if (address >= st->num_channels)
		return -EINVAL;

	chan = &st->iio_channels[address];

	return ltc6952_write_raw(indio_dev, chan, val, 0, mask);
}

static unsigned long ltc6952_clk_recalc_rate(struct clk_hw *hw,
					     unsigned long parent_rate)
{
	return ltc6952_get_clk_attr(hw, IIO_CHAN_INFO_FREQUENCY);
}

static long ltc6952_clk_round_rate(struct clk_hw *hw,
				   unsigned long rate,
				   unsigned long *parent_rate)
{
	struct ltc6952_output *out = to_output(hw);
	struct iio_dev *indio_dev = out->indio_dev;
	struct ltc6952_state *st = iio_priv(indio_dev);
	unsigned int div;

	div = ltc6952_calc_out_div(st->vco_freq, rate);

	return DIV_ROUND_CLOSEST(st->vco_freq, div);
}

static int ltc6952_clk_set_rate(struct clk_hw *hw,
				unsigned long rate,
				unsigned long parent_rate)
{
	return ltc6952_set_clk_attr(hw, IIO_CHAN_INFO_FREQUENCY, rate);
}

static const struct clk_ops ltc6952_clk_ops = {
	.recalc_rate = ltc6952_clk_recalc_rate,
	.round_rate = ltc6952_clk_round_rate,
	.set_rate = ltc6952_clk_set_rate,
};


static int ltc6952_clk_register(struct iio_dev *indio_dev,
				unsigned int num,
				unsigned int address)
{
	struct ltc6952_state *st = iio_priv(indio_dev);
	struct clk_init_data init;
	struct clk *clk;

	init.name = st->clk_out_names[num];
	init.ops = &ltc6952_clk_ops;
	init.flags = 0;
	init.num_parents = 0;

	st->outputs[num].hw.init = &init;
	st->outputs[num].indio_dev = indio_dev;
	st->outputs[num].address = address;

	clk = devm_clk_register(&st->spi->dev, &st->outputs[num].hw);
	if (IS_ERR(clk))
		return PTR_ERR(clk);

	st->clks[num] = clk;

	return 0;
}

static int ltc6952_setup(struct iio_dev *indio_dev)
{
	struct ltc6952_state *st = iio_priv(indio_dev);
	struct ltc6952_chan_spec *chan;
	unsigned long vco_freq, ref_freq;
	unsigned long pfd_freq;
	unsigned long n, r;
	unsigned int i, tmp;
	int ret;

	vco_freq = st->vco_freq / 1000;
	ref_freq = st->ref_freq / 1000;

	/* fVCO / N = fREF / R */
	rational_best_approximation(vco_freq, ref_freq,
				    LTC6952_N_MAX, LTC6952_R_MAX,
				    &n, &r);

	pfd_freq = vco_freq / n;
	while ((pfd_freq > LTC6952_PFD_FREQ_MAX) &&
	       (n <= LTC6952_N_MAX / 2) &&
	       (r <= LTC6952_R_MAX / 2)) {
		pfd_freq /= 2;
		n *= 2;
		r *= 2;
	}

	/* Resets all registers to default values */
	ret = ltc6952_write(indio_dev, 0x02, LTC6952_POR(1));
	if (ret < 0)
		return ret;

	/* Disable all channels */
	for (i = 0; i < 4; i++)
		ltc6952_write(indio_dev, 0x03, LTC6952_PD(i, 1));
	for (i = 4; i < 8; i++)
		ltc6952_write(indio_dev, 0x04, LTC6952_PD(i, 1));
	for (i = 8; i < LTC6952_NUM_CHAN; i++)
		ltc6952_write(indio_dev, 0x05, LTC6952_PD(i, 1));

	/* Program the dividers */
	ltc6952_write(indio_dev, 0x06, (r & 0x300) >> 8);
	ltc6952_write(indio_dev, 0x07, r & 0xFF);
	ltc6952_write(indio_dev, 0x08, (n & 0xFF00) >> 8);
	ltc6952_write(indio_dev, 0x06, n & 0xFF);

	/* Program the output channels */
	for (i = 0; i < st->num_channels; i++) {
		chan = &st->channels[i];

		if (chan->num >= LTC6952_NUM_CHAN)
			continue;

		ltc6952_calculate_divider(chan);
		/*tmp = FIELD_PREP(LTC6952_MP_MSK, chan->mp) |
			FIELD_PREP(LTC6952_MD_MSK, chan->md);
		Replace when upstreaming */

		tmp = (chan->mp & 0x1F) << 3 | (chan->md & 0x07);
		ltc6952_write(indio_dev, 0x0c + 0x04 * chan->num, tmp);

		st->iio_channels[i].type = IIO_ALTVOLTAGE;
		st->iio_channels[i].output = 1;
		st->iio_channels[i].indexed = 1;
		st->iio_channels[i].channel = chan->num;
		st->iio_channels[i].address = i;
		st->iio_channels[i].extend_name = chan->extended_name;
		st->iio_channels[i].info_mask_separate =
			BIT(IIO_CHAN_INFO_FREQUENCY) |
			BIT(IIO_CHAN_INFO_PHASE);
	}

	/* Enable channels */
	for (i = 0; i < 4; i++)
		ltc6952_write(indio_dev, 0x03, LTC6952_PD(i, 0));
	for (i = 4; i < 8; i++)
		ltc6952_write(indio_dev, 0x04, LTC6952_PD(i, 0));
	for (i = 8; i < LTC6952_NUM_CHAN; i++)
		ltc6952_write(indio_dev, 0x05, LTC6952_PD(i, 0));

	/* Configure clocks */
	for (i = 0; i < st->num_channels; i++) {
		chan = &st->channels[i];

		if (chan->num >= LTC6952_NUM_CHAN)
			continue;

		ret = ltc6952_clk_register(indio_dev, chan->num, i);
		if (ret)
			return ret;
	}

	st->clk_data.clks = st->clks;
	st->clk_data.clk_num = LTC6952_NUM_CHAN;

	return of_clk_add_provider(st->spi->dev.of_node,
				   of_clk_src_onecell_get,
				   &st->clk_data);
}

static int ltc6952_parse_dt(struct device *dev,
			    struct ltc6952_state *st)
{
	struct device_node *np = dev->of_node, *chan_np;
	unsigned int cnt = 0;
	int ret;

	ret = of_property_read_u32(np, "adi,ref-frequency-hz",
			     &st->ref_freq);
	if (ret)
		return ret;

	ret = of_property_read_u32(np, "adi,vco-frequency-hz",
				   &st->vco_freq);
	if (ret)
		return ret;

	for_each_child_of_node(np, chan_np)
		st->num_channels++;
	if (st->num_channels > LTC6952_NUM_CHAN)
		return -EINVAL;

	st->channels = devm_kzalloc(dev,
		sizeof(struct ltc6952_chan_spec) * st->num_channels,
				     GFP_KERNEL);
	if (!st->channels)
		return -ENOMEM;

	for_each_child_of_node(np, chan_np) {
		st->channels[cnt].num = cnt;
		of_property_read_u32(chan_np, "reg",
				     &st->channels[cnt].num);

		st->channels[cnt].out_divider = 4;
		of_property_read_u32(chan_np, "adi,divider",
				     &st->channels[cnt].out_divider);

		cnt++;
	}

	return 0;
}

static int ltc6952_status_show(struct seq_file *file, void *offset)
{
	struct iio_dev *indio_dev = spi_get_drvdata(file->private);
	int ret;
	u32 status;

	ret = ltc6952_read(indio_dev, 0x00, &status);
	if (ret < 0)
		return ret;

	seq_printf(file,
		   "SYSREF Status:\t%s\nSYNC Status:\t%s\nLock Status:\t%s\n",
		   status & LTC6952_REFOK_MSK ?
		   "Valid & Locked" : "Invalid",
		   status & LTC6952_VCOOK_MSK ?
		   "Unsynchronized" : "Synchronized",
		   status & LTC6952_LOCK_MSK ?
		   "VCO Locked" : "Unlocked");

	return 0;
}

static int ltc6952_probe(struct spi_device *spi)
{
	struct iio_dev *indio_dev;
	struct ltc6952_state *st;
	int ret;

	indio_dev = devm_iio_device_alloc(&spi->dev, sizeof(*st));
	if (!indio_dev)
		return -ENOMEM;

	st = iio_priv(indio_dev);

	mutex_init(&st->lock);

	spi_set_drvdata(spi, indio_dev);

	st->spi = spi;

	ret = ltc6952_parse_dt(&spi->dev, st);
	if (ret)
		return ret;

	indio_dev->dev.parent = &spi->dev;
	indio_dev->name = spi_get_device_id(spi)->name;
	indio_dev->info = &ltc6952_iio_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = st->iio_channels;
	indio_dev->num_channels = st->num_channels;

	if (spi->dev.of_node)
		indio_dev->name = spi->dev.of_node->name;
	else
		indio_dev->name = spi_get_device_id(spi)->name;

	ret = ltc6952_setup(indio_dev);
	if (ret)
		return ret;

	ret = iio_device_register(indio_dev);

	if (iio_get_debugfs_dentry(indio_dev)) {
		struct dentry *stats;

		stats = debugfs_create_devm_seqfile(&spi->dev, "status",
						    iio_get_debugfs_dentry(indio_dev),
						    ltc6952_status_show);
		if (PTR_ERR_OR_ZERO(stats))
			dev_err(&spi->dev,
				"Failed to create debugfs entry");
	}

	return ret;
}

static int ltc6952_remove(struct spi_device *spi)
{
	struct iio_dev *indio_dev = spi_get_drvdata(spi);

	iio_device_unregister(indio_dev);

	of_clk_del_provider(spi->dev.of_node);

	return 0;
}

static const struct spi_device_id ltc6952_id[] = {
	{"ltc6952", 0},
	{}
};
MODULE_DEVICE_TABLE(spi, ltc6952_id);

static const struct of_device_id ltc6952_of_match[] = {
	{ .compatible = "adi,ltc6952" },
	{ },
};
MODULE_DEVICE_TABLE(of, ltc6952_of_match);

static struct spi_driver ltc6952_driver = {
	.driver = {
		.name = "ltc6952",
		.of_match_table = ltc6952_of_match,
	},
	.probe = ltc6952_probe,
	.remove = ltc6952_remove,
	.id_table = ltc6952_id,
};
module_spi_driver(ltc6952_driver);

MODULE_AUTHOR("Mircea Caprioru <mircea.caprioru@analog.com>");
MODULE_DESCRIPTION("Analog Devices LTC6952 driver");
MODULE_LICENSE("GPL v2");
