/*
 * Copyright (c) 2025 TecInvent Electronics Ltd
 * SPDX-License-Identifier: Apache-2.0
 */

#include "rtc/mu_rtc.h"
#include "rtc/rtc.h"
#include <errno.h>
#include <time.h>
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

/* to do: move funzion to mcu-utils*/
static int days_from_epoch(int y, int m, int d);
static time_t timegm(struct tm const *t);

/*see https://howardhinnant.github.io/date_algorithms.html */
static int days_from_epoch(int y, int m, int d)
{
	y -= m <= 2;
	int era = y / 400;
	int yoe = y - era * 400;                                  // [0, 399]
	int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1; // [0, 365]
	int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;          // [0, 146096]
	return era * 146097 + doe - 719468;
}

static time_t timegm(struct tm const *t)
{
	int year = t->tm_year + 1900;
	int month =
		t->tm_mon; // [0-11]
			   // if month overflows, increase the year and calculate the correct month
	if (month > 11) {
		year += month / 12;
		month %= 12;
	}
	// if month underflows, decrease the year and calculate the correct month
	else if (month < 0) {
		int years_diff = (11 - month) / 12;
		year -= years_diff;
		month += 12 * years_diff;
	}
	// get total days since unix epoch, passing month as 1-12
	int days_since_epoch = days_from_epoch(year, month + 1, t->tm_mday);
	// convert days, hours, minutes, and seconds into total seconds
	return 60 * (60 * (24L * days_since_epoch + t->tm_hour) + t->tm_min) + t->tm_sec;
}

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

static int muRtc_getDateTimeUnix(time_t *unixTime)
{
	int ret = 0;
	struct rtc_time rtc_tm;
	ret = muRtc_deviceGetDateTime(&rtc_tm);

	struct tm tmp;
	tmp.tm_year = rtc_tm.tm_year;
	tmp.tm_mon = rtc_tm.tm_mon;
	tmp.tm_mday = rtc_tm.tm_mday;
	tmp.tm_hour = rtc_tm.tm_hour;
	tmp.tm_min = rtc_tm.tm_min;
	tmp.tm_sec = rtc_tm.tm_sec;

	*unixTime = timegm(&tmp); // from tm to time_t

	return ret;
}

static int muRtc_getDateTime(struct mu_rtc_dataTime *mu_rtc_dt)
{
	int ret = 0;
	struct rtc_time rtc_tm;

	ret = muRtc_deviceGetDateTime(&rtc_tm);

	mu_rtc_dt->year = rtc_tm.tm_year + 1900;
	mu_rtc_dt->month = rtc_tm.tm_mon + 1;
	mu_rtc_dt->day = rtc_tm.tm_mday;
	mu_rtc_dt->hour = rtc_tm.tm_hour;
	mu_rtc_dt->minute = rtc_tm.tm_min;
	mu_rtc_dt->second = rtc_tm.tm_sec;
	mu_rtc_dt->nanosecond = rtc_tm.tm_nsec;

	return ret;
}

static int muRtc_setDateTime(struct mu_rtc_dataTime mu_rtc_dt)
{
	int ret = 0;
	if (mu_rtc_dt.year < 2026 || mu_rtc_dt.year > 9999) {
		LOG_ERR("year out of bounds");
		return -EINVAL;
	}
	if (mu_rtc_dt.month < 1 || mu_rtc_dt.month > 12) {
		LOG_ERR("month out of bounds");
		return -EINVAL;
	}
	if (mu_rtc_dt.day < 1 || mu_rtc_dt.day > 31) {
		LOG_ERR("day out of bounds");
		return -EINVAL;
	}
	if (mu_rtc_dt.hour < 0 || mu_rtc_dt.hour > 23) {
		LOG_ERR("hour out of bounds");
		return -EINVAL;
	}
	if (mu_rtc_dt.minute < 0 || mu_rtc_dt.minute > 59) {
		LOG_ERR("minute out of bounds");
		return -EINVAL;
	}
	if (mu_rtc_dt.second < 0 || mu_rtc_dt.second > 59) {
		LOG_ERR("second out of bounds");
		return -EINVAL;
	}
	if (mu_rtc_dt.nanosecond < 0 || mu_rtc_dt.nanosecond > 999999999) {
		LOG_ERR("nanosecond out of bounds");
		return -EINVAL;
	}

	struct rtc_time rtc_tm;

	rtc_tm.tm_year = mu_rtc_dt.year - 1900;
	rtc_tm.tm_mon = mu_rtc_dt.month - 1;
	rtc_tm.tm_mday = mu_rtc_dt.day;
	rtc_tm.tm_hour = mu_rtc_dt.hour;
	rtc_tm.tm_min = mu_rtc_dt.minute;
	rtc_tm.tm_sec = mu_rtc_dt.second;
	rtc_tm.tm_nsec = (int32_t)mu_rtc_dt.nanosecond;

	ret = muRtc_deviceSetDateTime(&rtc_tm);

	return ret;
}

const struct mu_rtc_if muRtc = {
	.init = muRtc_init,
	.getDateTimeUnix = muRtc_getDateTimeUnix,
	.setDateTime = muRtc_setDateTime,
	.getDateTime = muRtc_getDateTime,

};

