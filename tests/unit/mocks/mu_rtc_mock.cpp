// Copyright 2022 TecInvent Electronics Ltd
//
// Created by dina on 15.12.25.
//

#include "mu_rtc_mock.h"

MuRtcMock *MuRtcMockObj = nullptr;

static int init()
{
	return MuRtcMockObj->init();
}

static int getDateTimeUnix(time_t *unixTime)
{
	return MuRtcMockObj->getDateTimeUnix(unixTime);
}

static int setDateTime(struct mu_rtc_dataTime mu_rtc_dt)
{
	return MuRtcMockObj->setDateTime(mu_rtc_dt);
}

static int getDateTime(struct mu_rtc_dataTime *mu_rtc_dt)
{
	return MuRtcMockObj->getDateTime(mu_rtc_dt);
}

const struct mu_rtc_if muRtcMock = {
	.init = init,
	.getDateTimeUnix = getDateTimeUnix,
	.setDateTime = setDateTime,
	.getDateTime = getDateTime,
};