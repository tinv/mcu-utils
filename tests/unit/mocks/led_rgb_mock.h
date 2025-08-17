/*
* Copyright 2025 TecInvent Electronics Ltd
 */

#ifndef MU_LED_RGB_MOCK_H
#define MU_LED_RGB_MOCK_H

#include <gmock/gmock.h>
#include <leds/led_rgb.h>

class MuLedRgbInterface
{
      public:
	virtual ~MuLedRgbInterface() {};
	virtual int init(const struct mu_led_ctrl_if* led_ctrl,
			 const struct mu_timer_if* muTimer) = 0;
	virtual int setMap(const struct mu_led_rgb_pos_map *map, size_t size) = 0;
	virtual int setSingle(unsigned int num, uint8_t red, uint8_t green, uint8_t blue,
			      uint8_t brightness, const int timeMs, led_rgb_finished_cb cb) = 0;
	virtual int setAll(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness,
			   const int timeMs, led_rgb_finished_cb cb) = 0;
};

class MuLedRgbMock : public MuLedRgbInterface
{
      public:
	virtual ~MuLedRgbMock() {};
	MOCK_METHOD(int, init, (const struct mu_led_ctrl_if* led_ctrl,
				const struct mu_timer_if* muTimer), (override));
	MOCK_METHOD(int, setMap, (const struct mu_led_rgb_pos_map *map, size_t size), (override));
	MOCK_METHOD(int, setSingle, (unsigned int num, uint8_t red, uint8_t green, uint8_t blue,
				     uint8_t brightness, const int timeMs, led_rgb_finished_cb cb), (override));
	MOCK_METHOD(int, setAll, (uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness,
				  const int timeMs, led_rgb_finished_cb cb), (override));
};

extern const struct mu_led_rgb_if muLedRgbMock;
extern MuLedRgbMock* MuLedRgbMockObj;

#endif // MU_LED_RGB_MOCK_H
