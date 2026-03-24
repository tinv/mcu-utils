/*
 * Copyright (c) 2026 TecInvent Electronics Ltd
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RTC_H
#define RTC_H
#include <zephyr/device.h>

#ifndef BUILD_TEST
#include <zephyr/drivers/rtc.h>
#else

#include "rtc_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

int rtc_get_time(const struct device *dev, struct rtc_time *tm);
int rtc_set_time(const struct device *dev, const struct rtc_time *tm);

#ifdef __cplusplus
}
#endif

#endif

#endif // RTC_H
