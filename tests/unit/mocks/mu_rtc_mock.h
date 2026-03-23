// Copyright 2022 TecInvent Electronics Ltd
//
// Created by dina on 15.12.25.
//

#ifndef MU_RTC_MOCK_H
#define MU_RTC_MOCK_H

#include <gmock/gmock.h>
#include <rtc/mu_rtc.h>

class MuRtcInterface
{
      public:
	virtual ~MuRtcInterface() {};
	virtual int init() = 0;
	virtual int getDateTimeUnix(time_t *unixTime) = 0;
	virtual int setDateTime(struct mu_rtc_dataTime mu_rtc_dt) = 0;
	virtual int getDateTime(struct mu_rtc_dataTime *mu_rtc_dt) = 0;
};

class MuRtcMock: public MuRtcInterface
{
      public:
	virtual ~MuRtcMock() {};
	MOCK_METHOD(int, init, (), (override));
	MOCK_METHOD(int, getDateTimeUnix, (time_t *unixTime), (override));
	MOCK_METHOD(int, setDateTime, (struct mu_rtc_dataTime mu_rtc_dt), (override));
	MOCK_METHOD(int, getDateTime, (struct mu_rtc_dataTime *mu_rtc_dt), (override));
};

extern const struct mu_rtc_if muRtcMock;
extern MuRtcMock *MuRtcMockObj;

#endif // MU_RTC_MOCK_H
