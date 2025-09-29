/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
 */

#include "led_single_mock.h"

MuLedSingleMock* MuLedSingleMockObj = nullptr;
led_single_finished_cb MuLedSingleMock::userCbAmb = NULL;
led_single_finished_cb MuLedSingleMock::userCbRed = NULL;
led_single_finished_cb MuLedSingleMock::userCbBlue = NULL;
led_single_finished_cb MuLedSingleMock::userCbGreen = NULL;
led_single_finished_cb MuLedSingleMock::userCbIr = NULL;

unsigned int MuLedSingleMock::mStartedLedNumAmb = 0;
unsigned int MuLedSingleMock::mStartedLedNumRed = 0;
unsigned int MuLedSingleMock::mStartedLedNumBlue = 0;
unsigned int MuLedSingleMock::mStartedLedNumGreen = 0;
unsigned int MuLedSingleMock::mStartedLedNumIr = 0;

static int muLedSingle_init(const struct mu_led_ctrl_if* led_ctrl)
{
	return MuLedSingleMockObj->init(led_ctrl);
}

static int muLedSingle_setMap(const enum mu_led_single_type type,
			      const struct mu_led_single_pos_map *map, size_t size)
{
	return MuLedSingleMockObj->setMap(type, map, size);
}

static int muLedSingle_setSingle(const enum mu_led_single_type type, unsigned int num,
				 uint8_t brightness, const int timeMs, led_single_finished_cb cb)
{
	return MuLedSingleMockObj->setSingle(type, num, brightness, timeMs, cb);
}

static int muLedSingle_setAll(const enum mu_led_single_type type, uint8_t brightness,
			      const int timeMs, led_single_finished_cb cb)
{
	return MuLedSingleMockObj->setAll(type, brightness, timeMs, cb);
}

static int muLedSingle_start()
{
	return MuLedSingleMockObj->start();
}

static bool muLedSingle_finishedAll()
{
	return MuLedSingleMockObj->finishedAll();
}

static bool muLedSingle_finished(const enum mu_led_single_type type)
{
	return MuLedSingleMockObj->finished(type);
}

const struct mu_led_single_if muLedSingleMock = {
	.init = muLedSingle_init,
	.setMap = muLedSingle_setMap,
	.setSingle = muLedSingle_setSingle,
	.setAll = muLedSingle_setAll,
	.start = muLedSingle_start,
	.finishedAll = muLedSingle_finishedAll,
	.finished = muLedSingle_finished,
};
