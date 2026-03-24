/*
 * Copyright (c) 2026 TecInvent Electronics Ltd
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZRTC_H
#define ZRTC_H
#include <zephyr/device.h>

#ifndef BUILD_TEST
#include <zephyr/drivers/rtc.h>
#else


#ifdef __cplusplus
extern "C" {
#endif

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
	int tm_nsec;
};

int rtc_get_time(const struct device *dev, struct rtc_time *tm);
int rtc_set_time(const struct device *dev, const struct rtc_time *tm);

#ifdef __cplusplus
}
#endif

#endif

#endif // ZRTC_H
