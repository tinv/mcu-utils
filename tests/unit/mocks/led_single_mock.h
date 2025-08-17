
#ifndef MU_LED_SINGLE_MOCK_H
#define MU_LED_SINGLE_MOCK_H

#include <gmock/gmock.h>
#include <leds/led_single.h>

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
};

class MuLedSingleMock : public MuLedSingleInterface
{
      public:
	virtual ~MuLedSingleMock() {};
	MOCK_METHOD(int, init, (const struct mu_led_ctrl_if* led_ctrl,
				const struct mu_timer_if* muTimer), (override));
	MOCK_METHOD(int, setMap, (const enum mu_led_single_type type,
				  const struct mu_led_single_pos_map *map, size_t size ), (override));
	MOCK_METHOD(int, setSingle, (const enum mu_led_single_type type, unsigned int num,
				     uint8_t brightness, const int timeMs,
				     led_single_finished_cb cb), (override));
	MOCK_METHOD(int, setAll, (const enum mu_led_single_type type, uint8_t brightness,
				  const int timeMs, led_single_finished_cb cb), (override));
};

extern const struct mu_led_single_if muLedSingleMock;
extern MuLedSingleMock* MuLedSingleMockObj;

#endif // MU_LED_SINGLE_MOCK_H
