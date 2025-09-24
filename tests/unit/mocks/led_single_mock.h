
#ifndef MU_LED_SINGLE_MOCK_H
#define MU_LED_SINGLE_MOCK_H

#include <gmock/gmock.h>
#include <leds/led_single.h>
#include <time/timer.h>
#include <timer_fake.h>
#include <zephyr/sys/util.h>

using namespace ::testing;

class MuLedSingleInterface
{
      public:
	virtual ~MuLedSingleInterface() {};
	virtual int init(const struct mu_led_ctrl_if* led_ctrl,
			 const struct mu_timer_if* muTimer) = 0;
	virtual int setMap(const enum mu_led_single_type type, const struct mu_led_single_pos_map *map,
			   size_t size ) = 0;
	virtual int setSingle(const enum mu_led_single_type type, unsigned int num,
			      uint8_t brightness, const int timeMs, led_single_finished_cb cb) = 0;
	virtual int setAll(const enum mu_led_single_type, uint8_t brightness, const int timeMs,
			   led_single_finished_cb cb) = 0;

	virtual bool finishedAll() = 0;

	virtual bool finished(const enum mu_led_single_type type) = 0;

};

class MuLedSingleMock : public MuLedSingleInterface
{
      public:
	MuLedSingleMock() : mTimerFake(nullptr) {};
	MuLedSingleMock(MuTimerFake* timerFake) : mTimerFake(timerFake)
	{
		mTimerFake->init(&tmAmbFinished, MuLedSingleMock::internalCbAmb, NULL);
		mTimerFake->init(&tmRedFinished, MuLedSingleMock::internalCbRed, NULL);
		mTimerFake->init(&tmBlueFinished, MuLedSingleMock::internalCbBlue, NULL);
		mTimerFake->init(&tmGreenFinished, MuLedSingleMock::internalCbGreen, NULL);
		mTimerFake->init(&tmIrFinished, MuLedSingleMock::internalCbIr, NULL);

		ON_CALL(*this, setSingle(_, _, _, _, _)).WillByDefault(
			[this](const enum mu_led_single_type type, unsigned int num,
				uint8_t brightness, const int timeMs, led_single_finished_cb cb) {

				ARG_UNUSED(num);
				ARG_UNUSED(brightness);
				return assignCb(type, timeMs, cb);
			}
		);

		ON_CALL(*this, setAll(_, _, _, _)).WillByDefault(
			[this](const enum mu_led_single_type type, uint8_t brightness,
				const int timeMs, led_single_finished_cb cb) {

				ARG_UNUSED(brightness);
				return assignCb(type, timeMs, cb);
			}
		);

	}

	virtual ~MuLedSingleMock() {};

	int assignCb(const enum mu_led_single_type type, const int timeMs, led_single_finished_cb cb)
	{
		switch (type) {
		case MU_LED_RED:
			if (cb) {
				mTimerFake->start(&tmRedFinished, timeMs, 0);
			} else {
				mTimerFake->stop(&tmRedFinished);
			}
			break;
		case MU_LED_BLUE:
			if (cb) {
				mTimerFake->start(&tmBlueFinished, timeMs, 0);
			} else {
				mTimerFake->stop(&tmBlueFinished);
			}
			break;
		case MU_LED_AMBER:
			if (cb) {
				mTimerFake->start(&tmAmbFinished, timeMs, 0);
			} else {
				mTimerFake->stop(&tmAmbFinished);
			}
			break;
		case MU_LED_IR:
			if (cb) {
				mTimerFake->start(&tmIrFinished, timeMs, 0);
			} else {
				mTimerFake->stop(&tmIrFinished);
			}
			break;
		case MU_LED_GREEN:
		default:
			return -ENOTSUP;
		}

		return 0;


	}

	static void internalCbAmb(void *user_data)
	{
		ARG_UNUSED(user_data);

		if (userCbAmb) {
			userCbAmb(MU_LED_AMBER);
		}
	};

	static void internalCbRed(void *user_data)
	{
		ARG_UNUSED(user_data);

		if (userCbRed) {
			userCbRed(MU_LED_RED);
		}
	};

	static void internalCbBlue(void *user_data)
	{
		ARG_UNUSED(user_data);

		if (userCbBlue) {
			userCbBlue(MU_LED_BLUE);
		}
	};

	static void internalCbGreen(void *user_data)
	{
		ARG_UNUSED(user_data);

		if (userCbGreen) {
			userCbGreen(MU_LED_GREEN);
		}
	};

	static void internalCbIr(void *user_data)
	{
		ARG_UNUSED(user_data);

		if (userCbIr) {
			userCbIr(MU_LED_IR);
		}
	};

	MOCK_METHOD(int, init, (const struct mu_led_ctrl_if* led_ctrl,
				const struct mu_timer_if* muTimer), (override));
	MOCK_METHOD(int, setMap, (const enum mu_led_single_type type,
				  const struct mu_led_single_pos_map *map, size_t size ), (override));
	MOCK_METHOD(int, setSingle, (const enum mu_led_single_type type, unsigned int num,
				     uint8_t brightness, const int timeMs,
				     led_single_finished_cb cb), (override));
	MOCK_METHOD(int, setAll, (const enum mu_led_single_type type, uint8_t brightness,
				  const int timeMs, led_single_finished_cb cb), (override));

	MOCK_METHOD(bool, finishedAll, (), (override));

	MOCK_METHOD(bool, finished, (const enum mu_led_single_type type), (override));

private:
	MuTimerFake *mTimerFake;
	timer_handle_t tmAmbFinished;
	timer_handle_t tmRedFinished;
	timer_handle_t tmBlueFinished;
	timer_handle_t tmGreenFinished;
	timer_handle_t tmIrFinished;
	static led_single_finished_cb userCbAmb;
	static led_single_finished_cb userCbRed;
	static led_single_finished_cb userCbBlue;
	static led_single_finished_cb userCbGreen;
	static led_single_finished_cb userCbIr;
};

extern const struct mu_led_single_if muLedSingleMock;
extern MuLedSingleMock* MuLedSingleMockObj;

#endif // MU_LED_SINGLE_MOCK_H
