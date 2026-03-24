/*
 * Copyright (c) 2026 TecInvent Electronics Ltd
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mu_rtc_mock.h"

MuRtcMock *MuRtcMockObj = nullptr;

static int init()
{
	return MuRtcMockObj->init();
}

static int setDateTime(struct rtc_time rtc_tm)
{
	return MuRtcMockObj->setDateTime(rtc_tm);
}

static int getDateTime(struct rtc_time *rtc_tm)
{
	return MuRtcMockObj->getDateTime(rtc_tm);
}

const struct mu_rtc_if muRtcMock = {
	.init = init,
	.setDateTime = setDateTime,
	.getDateTime = getDateTime,
};