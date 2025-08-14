/*
* Copyright 2025 TecInvent Electronics Ltd
 */

#ifndef MU_LED_RGB_MOCK_H
#define MU_LED_RGB_MOCK_H

#include <gmock/gmock.h>
#include <leds/mu_led_rgb.h>

class MuLedRgbInterface
{
      public:
	virtual ~MuLedRgbInterface() {};
	virtual int init(const struct mu_led_ctrl_if* led_ctrl) = 0;
	virtual int setSingle(unsigned int num, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness) = 0;
	virtual int setAll(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness) = 0;
};

class MuLedRgbMock : public MuLedRgbInterface
{
      public:
	virtual ~MuLedRgbMock() {};
	MOCK_METHOD(int, init, (const struct mu_led_ctrl_if* led_ctrl), (override));
	MOCK_METHOD(int, setSingle, (unsigned int num, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness), (override));
	MOCK_METHOD(int, setAll, (uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness), (override));
};

extern const struct mu_led_rgb_if muLedRgbMock;
extern MuLedRgbMock* MuLedRgbMockObj;

#endif // MU_LED_RGB_MOCK_H
