/*
* Copyright 2025 TecInvent Electronics Ltd
 */

#include "led_rgb_mock.h"

MuLedRgbMock* MuLedRgbMockObj = nullptr;

int init(const struct mu_led_ctrl_if* led_ctrl)
{
	return MuLedRgbMockObj->init(led_ctrl);
}

int setSingle(unsigned int num, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{
	return MuLedRgbMockObj->setSingle(num, red, green, blue, brightness);
}

int setAll(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{
	return MuLedRgbMockObj->setAll(red, green, blue, brightness);
}

const struct mu_led_rgb_if muLedRgbMock = {
	.init = init,
	.setSingle = setSingle,
	.setAll = setAll
};
