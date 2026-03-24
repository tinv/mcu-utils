/*
 * Copyright (c) 2026 TecInvent Electronics Ltd
 * SPDX-License-Identifier: Apache-2.0
 */

#include "rtc/mu_rtc.h"
#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <stdint.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mu_rtc);

static int muRtc_deviceGetDateTime(struct rtc_time *tm);
static int muRtc_deviceSetDateTime(struct rtc_time *tm);
static const struct device *dev;

static int muRtc_init(void)
{
#ifndef BUILD_TEST
	dev = DEVICE_DT_GET(DT_NODELABEL(rtc));
	if ((dev == NULL) || (!device_is_ready(dev))) {
		LOG_ERR("RTC device not ready");
		return -ENODEV;
	}
#endif
	return 0;
}

static int muRtc_deviceGetDateTime(struct rtc_time *tm)
{
	int ret = 0;

	ret = rtc_get_time(dev, tm);
	if (ret < 0) {
		LOG_ERR("Cannot get date time: %d", ret);
		return ret;
	}
	LOG_DBG("get: %04d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1,
		tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	return ret;
}

static int muRtc_deviceSetDateTime(struct rtc_time *tm)
{
	int ret = 0;

	LOG_DBG("set: %04d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1,
		tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	ret = rtc_set_time(dev, tm);
	if (ret < 0) {
		LOG_ERR("Cannot set date time: %d", ret);
		return ret;
	}
	return ret;
}

static int muRtc_getDateTime(struct rtc_time *rtc_tm)
{
	int ret = 0;

	ret = muRtc_deviceGetDateTime(rtc_tm);

	return ret;
}

static int muRtc_setDateTime(struct rtc_time rtc_tm)
{
	int ret = 0;
	if (rtc_tm.tm_year < 1900 || rtc_tm.tm_year > 9999) {
		LOG_ERR("year out of bounds");
		return -EINVAL;
	}
	if (rtc_tm.tm_mon < 0 || rtc_tm.tm_mon > 11) {
		LOG_ERR("month out of bounds");
		return -EINVAL;
	}
	if (rtc_tm.tm_mday < 1 || rtc_tm.tm_mday > 31) {
		LOG_ERR("day out of bounds");
		return -EINVAL;
	}
	if (rtc_tm.tm_hour < 0 || rtc_tm.tm_hour > 23) {
		LOG_ERR("hour out of bounds");
		return -EINVAL;
	}
	if (rtc_tm.tm_min < 0 || rtc_tm.tm_min > 59) {
		LOG_ERR("minute out of bounds");
		return -EINVAL;
	}
	if (rtc_tm.tm_sec < 0 || rtc_tm.tm_sec > 59) {
		LOG_ERR("second out of bounds");
		return -EINVAL;
	}
	if (rtc_tm.tm_nsec < 0 || rtc_tm.tm_nsec > 999999999) {
		LOG_ERR("nanosecond out of bounds");
		return -EINVAL;
	}

	ret = muRtc_deviceSetDateTime(&rtc_tm);

	return ret;
}

const struct mu_rtc_if muRtc = {
	.init = muRtc_init,
	.setDateTime = muRtc_setDateTime,
	.getDateTime = muRtc_getDateTime,

};
