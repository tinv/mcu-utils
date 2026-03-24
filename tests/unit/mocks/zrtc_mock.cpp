/*
 * Copyright (c) 2026 TecInvent Electronics Ltd
 * SPDX-License-Identifier: Apache-2.0
 */

#include "zrtc_mock.h"

RTCMock *RTCMockObj = nullptr;

int rtc_get_time(const struct device *dev, struct rtc_time *tm)
{
	return RTCMockObj->rtc_get_time(dev, tm);
}

int rtc_set_time(const struct device *dev, const struct rtc_time *tm)
{
	return RTCMockObj->rtc_set_time(dev, tm);
}
