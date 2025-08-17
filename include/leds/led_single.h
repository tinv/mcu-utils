/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef MU_LED_SINGLE_H_
#define MU_LED_SINGLE_H_

#include <stdint.h>
#include <leds/led_ctrl.h>
#include <time/timer.h>

#ifdef __cplusplus
extern "C" {
#endif

enum mu_led_single_type {
	MU_LED_RED = 0,
	MU_LED_GREEN = 1,
	MU_LED_BLUE = 2,
	MU_LED_AMBER = 3,
	MU_LED_IR = 4
};

struct mu_led_single_pos_map {
	uint8_t dev_idx;
	uint8_t app_pos;
	/* Device tree led position within the array */
	uint8_t dt_pos;
};

typedef void (*led_single_finished_cb)(const enum mu_led_single_type);

/* formatter:off */

struct mu_led_single_if {
	/**
	 *
	 * @param led_ctrl
	 * @return
	 */
	int (*init)(const struct mu_led_ctrl_if *led_ctrl, const struct mu_timer_if* muTimer);

	/**
	 *
	 * @param type
	 * @param map
	 * @param size
	 * @return
	 */
	int (*setMap)(const enum mu_led_single_type type, const struct mu_led_single_pos_map *map, const size_t size);

	/**
	 *
	 * @param type
	 * @param num
	 * @param brightness
	 * @param timeMs
	 * @param cb
	 * @return
	 */
	int (*setSingle)(const enum mu_led_single_type type, const unsigned int num,
			 const uint8_t brightness, const int timeMs, led_single_finished_cb cb);

	/**
	 *
	 * @param type
	 * @param brightness
	 * @param timeMs
	 * @param cb
	 * @return
	 */
	int (*setAll)(const enum mu_led_single_type type, const uint8_t brightness,
		      const int timeMs, led_single_finished_cb cb);
};

/* formatter:on */

extern const struct mu_led_single_if muLedSingle;

#ifdef __cplusplus
}
#endif

#endif /* MU_LED_SINGLE_H_ */

