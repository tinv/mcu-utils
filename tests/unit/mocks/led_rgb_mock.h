/*
* Copyright 2025 TecInvent Electronics Ltd
 */

#ifndef MU_LED_RGB_MOCK_H
#define MU_LED_RGB_MOCK_H

#include <gmock/gmock.h>
#include <leds/led_rgb.h>
#include <time/timer.h>
#include <timer_fake.h>
#include <zephyr/sys/util.h>

using namespace ::testing;

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
	virtual bool finishedAll() = 0;
};

class MuLedRgbMock : public MuLedRgbInterface
{
      public:

	MuLedRgbMock() : mTimerFake(nullptr) {};
	MuLedRgbMock(MuTimerFake* timerFake) : mTimerFake(timerFake)
	{
		mTimerFake->init(&tmFinished, MuLedRgbMock::internalCb, NULL);

		ON_CALL(*this, setSingle(_, _, _, _, _, _, _)).WillByDefault(
			[this](unsigned int num, uint8_t red, uint8_t green, uint8_t blue,
		    		uint8_t brightness, const int timeMs, led_rgb_finished_cb cb) {

					ARG_UNUSED(num);
					ARG_UNUSED(red);
					ARG_UNUSED(green);
					ARG_UNUSED(blue);
					ARG_UNUSED(brightness);

		    			userCb = cb;

		    			/* Trigger timer for user notification only if cb is set */
					if (cb) {
						mTimerFake->start(&tmFinished, timeMs, 0);
					} else {
						mTimerFake->stop(&tmFinished);
					}

				return 0;
		    	}
		);

		ON_CALL(*this, setAll(_, _, _, _, _, _)).WillByDefault(
			[this](uint8_t red, uint8_t green, uint8_t blue,
		    		uint8_t brightness, const int timeMs, led_rgb_finished_cb cb) {

					ARG_UNUSED(red);
					ARG_UNUSED(green);
					ARG_UNUSED(blue);
					ARG_UNUSED(brightness);

		    			userCb = cb;

		    			/* Trigger timer for user notification only if cb is set */
					if (cb) {
						mTimerFake->start(&tmFinished, timeMs, 0);
					} else {
						mTimerFake->stop(&tmFinished);
					}

				return 0;
		    	}
		);

	}

	virtual ~MuLedRgbMock() {};

	static void internalCb(void *user_data)
	{
		ARG_UNUSED(user_data);

		if (userCb) {
			userCb();
		}
	};

	MOCK_METHOD(int, init, (const struct mu_led_ctrl_if* led_ctrl,
				const struct mu_timer_if* muTimer), (override));
	MOCK_METHOD(int, setMap, (const struct mu_led_rgb_pos_map *map, size_t size), (override));
	MOCK_METHOD(int, setSingle, (unsigned int num, uint8_t red, uint8_t green, uint8_t blue,
				     uint8_t brightness, const int timeMs, led_rgb_finished_cb cb), (override));
	MOCK_METHOD(int, setAll, (uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness,
				  const int timeMs, led_rgb_finished_cb cb), (override));

	MOCK_METHOD(bool, finishedAll, (), (override));

	private:

	MuTimerFake *mTimerFake;
	timer_handle_t tmFinished;
	static led_rgb_finished_cb userCb;

};

extern const struct mu_led_rgb_if muLedRgbMock;
extern MuLedRgbMock* MuLedRgbMockObj;

#endif // MU_LED_RGB_MOCK_H
