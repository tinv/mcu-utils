/*
* Copyright 2025 TecInvent Electronics Ltd
 */

#include "led_rgb_mock.h"

MuLedRgbMock* MuLedRgbMockObj = nullptr;
led_rgb_finished_cb MuLedRgbMock::userCb = NULL;
unsigned int MuLedRgbMock::mStartedLedNum = 0;

static int init(const struct mu_led_ctrl_if* led_ctrl)
{
	return MuLedRgbMockObj->init(led_ctrl);
}

static int setMap(const struct mu_led_rgb_pos_map *map, size_t size)
{
	return 	MuLedRgbMockObj->setMap(map, size);
}

static int setSingle(unsigned int num, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness,
	      const int timeMs, led_rgb_finished_cb cb)
{
	return MuLedRgbMockObj->setSingle(num, red, green, blue, brightness, timeMs, cb);
}

static int setAll(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness,
	   const int timeMs, led_rgb_finished_cb cb)
{
	return MuLedRgbMockObj->setAll(red, green, blue, brightness, timeMs, cb);
}

static int start()
{
	return MuLedRgbMockObj->start();
}

static bool finishedAll()
{
	return MuLedRgbMockObj->finishedAll();
}

const struct mu_led_rgb_if muLedRgbMock = {
	.init = init,
	.setMap = setMap,
	.setSingle = setSingle,
	.setAll = setAll,
	.start = start,
	.finishedAll = finishedAll
};
