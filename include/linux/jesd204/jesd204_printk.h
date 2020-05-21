// SPDX-License-Identifier: GPL-2.0
/*
 * jesd204_printk.h - printk messages helpers for devices
 *
 * Copyright (c) 2020 Analog Devices Inc.
 */

#ifndef _JESD204_PRINTK_H_
#define _JESD204_PRINTK_H_

#include <linux/compiler.h>
#include <linux/types.h>
#include <linux/ratelimit.h>

#ifndef jesd204_fmt
#define jesd204_fmt(fmt) fmt
#endif

struct jesd204_dev;

#ifdef CONFIG_PRINTK

__printf(3, 0) __cold
int jesd204_vprintk_emit(int level, const struct jesd204_dev *jdev,
		     const char *fmt, va_list args);
__printf(3, 4) __cold
int jesd204_printk_emit(int level, const struct jesd204_dev *jdev, const char *fmt, ...);

__printf(3, 4) __cold
void jesd204_printk(const char *level, const struct jesd204_dev *jdev,
		const char *fmt, ...);
__printf(2, 3) __cold
void _jesd204_emerg(const struct jesd204_dev *jdev, const char *fmt, ...);
__printf(2, 3) __cold
void _jesd204_alert(const struct jesd204_dev *jdev, const char *fmt, ...);
__printf(2, 3) __cold
void _jesd204_crit(const struct jesd204_dev *jdev, const char *fmt, ...);
__printf(2, 3) __cold
void _jesd204_err(const struct jesd204_dev *jdev, const char *fmt, ...);
__printf(2, 3) __cold
void _jesd204_warn(const struct jesd204_dev *jdev, const char *fmt, ...);
__printf(2, 3) __cold
void _jesd204_notice(const struct jesd204_dev *jdev, const char *fmt, ...);
__printf(2, 3) __cold
void _jesd204_info(const struct jesd204_dev *jdev, const char *fmt, ...);

#else

static inline __printf(3, 0)
int jesd204_vprintk_emit(int level, const struct jesd204_dev *jdev,
		     const char *fmt, va_list args)
{ return 0; }
static inline __printf(3, 4)
int jesd204_printk_emit(int level, const struct jesd204_dev *jdev, const char *fmt, ...)
{ return 0; }

static inline void __jesd204_printk(const char *level, const struct jesd204_dev *jdev,
				struct va_format *vaf)
{}
static inline __printf(3, 4)
void jesd204_printk(const char *level, const struct jesd204_dev *jdev,
		 const char *fmt, ...)
{}

static inline __printf(2, 3)
void _jesd204_emerg(const struct jesd204_dev *jdev, const char *fmt, ...)
{}
static inline __printf(2, 3)
void _jesd204_crit(const struct jesd204_dev *jdev, const char *fmt, ...)
{}
static inline __printf(2, 3)
void _jesd204_alert(const struct jesd204_dev *jdev, const char *fmt, ...)
{}
static inline __printf(2, 3)
void _jesd204_err(const struct jesd204_dev *jdev, const char *fmt, ...)
{}
static inline __printf(2, 3)
void _jesd204_warn(const struct jesd204_dev *jdev, const char *fmt, ...)
{}
static inline __printf(2, 3)
void _jesd204_notice(const struct jesd204_dev *jdev, const char *fmt, ...)
{}
static inline __printf(2, 3)
void _jesd204_info(const struct jesd204_dev *jdev, const char *fmt, ...)
{}

#endif

/*
 * #defines for all the jesd204_<level> macros to prefix with whatever
 * possible use of #define jesd204_fmt(fmt) ...
 */

#define jesd204_emerg(jdev, fmt, ...)					\
	_jesd204_emerg(jdev, jesd204_fmt(fmt), ##__VA_ARGS__)
#define jesd204_crit(jdev, fmt, ...)						\
	_jesd204_crit(jdev, jesd204_fmt(fmt), ##__VA_ARGS__)
#define jesd204_alert(jdev, fmt, ...)					\
	_jesd204_alert(jdev, jesd204_fmt(fmt), ##__VA_ARGS__)
#define jesd204_err(jdev, fmt, ...)						\
	_jesd204_err(jdev, jesd204_fmt(fmt), ##__VA_ARGS__)
#define jesd204_warn(jdev, fmt, ...)						\
	_jesd204_warn(jdev, jesd204_fmt(fmt), ##__VA_ARGS__)
#define jesd204_notice(jdev, fmt, ...)					\
	_jesd204_notice(jdev, jesd204_fmt(fmt), ##__VA_ARGS__)
#define jesd204_info(jdev, fmt, ...)						\
	_jesd204_info(jdev, jesd204_fmt(fmt), ##__VA_ARGS__)

#if defined(CONFIG_DYNAMIC_DEBUG)
#define jesd204_dbg(jdev, fmt, ...)						\
	dynamic_jesd204_dbg(jdev, jesd204_fmt(fmt), ##__VA_ARGS__)
#elif defined(DEBUG)
#define jesd204_dbg(jdev, fmt, ...)						\
	jesd204_printk(KERN_DEBUG, jdev, jesd204_fmt(fmt), ##__VA_ARGS__)
#else
#define jesd204_dbg(jdev, fmt, ...)						\
({									\
	if (0)								\
		jesd204_printk(KERN_DEBUG, jdev, jesd204_fmt(fmt), ##__VA_ARGS__); \
})
#endif

#ifdef CONFIG_PRINTK
#define jesd204_level_once(jesd204_level, jdev, fmt, ...)			\
do {									\
	static bool __print_once __read_mostly;				\
									\
	if (!__print_once) {						\
		__print_once = true;					\
		jesd204_level(jdev, fmt, ##__VA_ARGS__);			\
	}								\
} while (0)
#else
#define jesd204_level_once(jesd204_level, jdev, fmt, ...)			\
do {									\
	if (0)								\
		jesd204_level(jdev, fmt, ##__VA_ARGS__);			\
} while (0)
#endif

#define jesd204_emerg_once(jdev, fmt, ...)					\
	jesd204_level_once(jesd204_emerg, jdev, fmt, ##__VA_ARGS__)
#define jesd204_alert_once(jdev, fmt, ...)					\
	jesd204_level_once(jesd204_alert, jdev, fmt, ##__VA_ARGS__)
#define jesd204_crit_once(jdev, fmt, ...)					\
	jesd204_level_once(jesd204_crit, jdev, fmt, ##__VA_ARGS__)
#define jesd204_err_once(jdev, fmt, ...)					\
	jesd204_level_once(jesd204_err, jdev, fmt, ##__VA_ARGS__)
#define jesd204_warn_once(jdev, fmt, ...)					\
	jesd204_level_once(jesd204_warn, jdev, fmt, ##__VA_ARGS__)
#define jesd204_notice_once(jdev, fmt, ...)					\
	jesd204_level_once(jesd204_notice, jdev, fmt, ##__VA_ARGS__)
#define jesd204_info_once(jdev, fmt, ...)					\
	jesd204_level_once(jesd204_info, jdev, fmt, ##__VA_ARGS__)
#define jesd204_dbg_once(jdev, fmt, ...)					\
	jesd204_level_once(jesd204_dbg, jdev, fmt, ##__VA_ARGS__)

#define jesd204_level_ratelimited(jesd204_level, jdev, fmt, ...)			\
do {									\
	static DEFINE_RATELIMIT_STATE(_rs,				\
				      DEFAULT_RATELIMIT_INTERVAL,	\
				      DEFAULT_RATELIMIT_BURST);		\
	if (__ratelimit(&_rs))						\
		jesd204_level(jdev, fmt, ##__VA_ARGS__);			\
} while (0)

#define jesd204_emerg_ratelimited(jdev, fmt, ...)				\
	jesd204_level_ratelimited(jesd204_emerg, jdev, fmt, ##__VA_ARGS__)
#define jesd204_alert_ratelimited(jdev, fmt, ...)				\
	jesd204_level_ratelimited(jesd204_alert, jdev, fmt, ##__VA_ARGS__)
#define jesd204_crit_ratelimited(jdev, fmt, ...)				\
	jesd204_level_ratelimited(jesd204_crit, jdev, fmt, ##__VA_ARGS__)
#define jesd204_err_ratelimited(jdev, fmt, ...)				\
	jesd204_level_ratelimited(jesd204_err, jdev, fmt, ##__VA_ARGS__)
#define jesd204_warn_ratelimited(jdev, fmt, ...)				\
	jesd204_level_ratelimited(jesd204_warn, jdev, fmt, ##__VA_ARGS__)
#define jesd204_notice_ratelimited(jdev, fmt, ...)				\
	jesd204_level_ratelimited(jesd204_notice, jdev, fmt, ##__VA_ARGS__)
#define jesd204_info_ratelimited(jdev, fmt, ...)				\
	jesd204_level_ratelimited(jesd204_info, jdev, fmt, ##__VA_ARGS__)
#if defined(CONFIG_DYNAMIC_DEBUG)
/* descriptor check is first to prevent flooding with "callbacks suppressed" */
#define jesd204_dbg_ratelimited(jdev, fmt, ...)				\
do {									\
	static DEFINE_RATELIMIT_STATE(_rs,				\
				      DEFAULT_RATELIMIT_INTERVAL,	\
				      DEFAULT_RATELIMIT_BURST);		\
	DEFINE_DYNAMIC_DEBUG_METADATA(descriptor, fmt);			\
	if (DYNAMIC_DEBUG_BRANCH(descriptor) &&				\
	    __ratelimit(&_rs))						\
		__dynamic_jesd204_dbg(&descriptor, jdev, jesd204_fmt(fmt),\
				  ##__VA_ARGS__);			\
} while (0)
#elif defined(DEBUG)
#define jesd204_dbg_ratelimited(jdev, fmt, ...)				\
do {									\
	static DEFINE_RATELIMIT_STATE(_rs,				\
				      DEFAULT_RATELIMIT_INTERVAL,	\
				      DEFAULT_RATELIMIT_BURST);		\
	if (__ratelimit(&_rs))						\
		jesd204_printk(KERN_DEBUG, jdev, jesd204_fmt(fmt), ##__VA_ARGS__); \
} while (0)
#else
#define jesd204_dbg_ratelimited(jdev, fmt, ...)				\
do {									\
	if (0)								\
		jesd204_printk(KERN_DEBUG, jdev, jesd204_fmt(fmt), ##__VA_ARGS__); \
} while (0)
#endif

#ifdef VERBOSE_DEBUG
#define jesd204_vdbg	jesd204_dbg
#else
#define jesd204_vdbg(jdev, fmt, ...)						\
({									\
	if (0)								\
		jesd204_printk(KERN_DEBUG, jdev, jesd204_fmt(fmt), ##__VA_ARGS__); \
})
#endif

/*
 * jesd204_WARN*() acts like jesd204_printk(), but with the key difference of
 * using WARN/WARN_ONCE to include file/line information and a backtrace.
 */
#define jesd204_WARN(jdev, format, arg...) \
	WARN(1, "%s %s: " format, jesd204_driver_string(jdev), jesd204_name(jdev), ## arg);

#define jesd204_WARN_ONCE(jdev, condition, format, arg...) \
	WARN_ONCE(condition, "%s %s: " format, \
			jesd204_driver_string(jdev), jesd204_name(jdev), ## arg)

#endif /* _JESD204_PRINTK_H_ */
