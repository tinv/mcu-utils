/*
 * Copyright (c) 2026 TecInvent Electronics Ltd
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RTC_MOCK_H
#define RTC_MOCK_H

#include <gmock/gmock.h>
#include <rtc/rtc.h>

class MuRtcInterface
{
      public:
	virtual ~MuRtcInterface(){};
	virtual int init() = 0;
	virtual int setDateTime(struct rtc_time rtc_tm) = 0;
	virtual int getDateTime(struct rtc_time *rtc_tm) = 0;
};

class MuRtcMock : public MuRtcInterface
{
      public:
	virtual ~MuRtcMock(){};
	MOCK_METHOD(int, init, (), (override));
	MOCK_METHOD(int, setDateTime, (struct rtc_time rtc_tm), (override));
	MOCK_METHOD(int, getDateTime, (struct rtc_time *rtc_tm), (override));
};

extern const struct mu_rtc_if muRtcMock;
extern MuRtcMock *MuRtcMockObj;

#endif // RTC_MOCK_H
