
#ifndef MU_LED_SINGLE_MOCK_H
#define MU_LED_SINGLE_MOCK_H

#include <gmock/gmock.h>
#include <leds/mu_led_single.h>

class MuLedSingleInterface
{
      public:
	virtual ~MuLedSingleInterface() {};
	virtual int init(const struct mu_led_ctrl_if* led_ctrl) = 0;
	virtual int setMap(const enum har_led_single_type type, const struct har_led_pos_map *map, size_t size ) = 0;
	virtual int setSingle(const enum har_led_single_type type, unsigned int num, uint8_t brightness) = 0;
	virtual int setAll(const enum har_led_single_type, uint8_t brightness) = 0;
};

class MuLedSingleMock : public MuLedSingleInterface
{
      public:
	virtual ~MuLedSingleMock() {};
	MOCK_METHOD(int, init, (const struct mu_led_ctrl_if* led_ctrl), (override));
	MOCK_METHOD(int, setMap, (const enum har_led_single_type type, const struct har_led_pos_map *map, size_t size ), (override));
	MOCK_METHOD(int, setSingle, (const enum har_led_single_type type, unsigned int num, uint8_t brightness), (override));
	MOCK_METHOD(int, setAll, (const enum har_led_single_type type, uint8_t brightness), (override));
};

extern const struct mu_led_single_if muLedSingleMock;
extern MuLedSingleMock* MuLedSingleMockObj;

#endif // MU_LED_SINGLE_MOCK_H
