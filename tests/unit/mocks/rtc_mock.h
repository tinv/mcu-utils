/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/


#ifndef RTC_MOCK_H
#define RTC_MOCK_H

#include <gmock/gmock.h>
#include "rtc/rtc.h"

class RTCInterface
{
public:
	virtual ~RTCInterface() {};
	virtual int rtc_get_time(const struct device *dev, struct rtc_time *tm) = 0;
	virtual int rtc_set_time(const struct device *dev, const struct rtc_time *tm) = 0;
};

class RTCMock : public RTCInterface
{
public:
	virtual ~RTCMock() {};
	MOCK_METHOD(int, rtc_get_time, ( const struct device *dev, struct rtc_time *tm), (override));
	MOCK_METHOD(int, rtc_set_time, ( const struct device *dev, const struct rtc_time *tm), (override));
};

#endif // RTC_MOCK_H
