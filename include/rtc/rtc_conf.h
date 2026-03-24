/*
 * Copyright (c) 2026 TecInvent Electronics Ltd
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RTC_CONF_H
#define RTC_CONF_H

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
#ifdef __cplusplus
}
#endif

#endif // RTC_CONF_H
