/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
 */

#include "led_single_mock.h"

MuLedSingleMock* MuLedSingleMockObj = nullptr;

static int muLedSingle_init(const struct mu_led_ctrl_if* led_ctrl, const struct mu_timer_if* muTimer)
{
	return MuLedSingleMockObj->init(led_ctrl, muTimer);
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

const struct mu_led_single_if muLedSingleMock = {
	.init = muLedSingle_init,
	.setMap = muLedSingle_setMap,
	.setSingle = muLedSingle_setSingle,
	.setAll = muLedSingle_setAll
};
