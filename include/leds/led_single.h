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

typedef void (*led_single_finished_cb)(const enum mu_led_single_type, const int app_pos);

/* formatter:off */

struct mu_led_single_if {
	/**
	 *
	 * @param led_ctrl
	 * @return
	 */
	int (*init)(const struct mu_led_ctrl_if *led_ctrl);

	/**
	 *
	 * @param type
	 * @param map
	 * @param size
	 * @return
	 */
	int (*setMap)(const enum mu_led_single_type type, const struct mu_led_single_pos_map *map,
		      const size_t size);

	/**
	 * Sets a single LED
	 * @param type LED type
	 * @param num LED number
	 * @param brightness Brightness level
	 * @param timeMs Fade time
	 * @param cb User callback. This callback will be called when the animation finishes
	 * @return Returns 0 on success, otherwise it returns a negative number
	 */
	int (*setSingle)(const enum mu_led_single_type type, const unsigned int num,
			 const uint8_t brightness, const int timeMs, led_single_finished_cb cb);

	/**
	 * Sets all LEDS of type @p type to brightness @p brightness in time defined by @p timeMs
	 * @param type LED type
	 * @param brightness Brightness level
	 * @param timeMs Fade time
	 * @param cb User callback. This callback will be called when the animation finishes
	 * @return Returns 0 on success, otherwise it returns a negative number
	 */
	int (*setAll)(const enum mu_led_single_type type, const uint8_t brightness,
		      const int timeMs, led_single_finished_cb cb);

	/**
	 * Start fade
	 * @return Returns 0 on success, otherwise it returns a negative number
	 */
	int (*start)(void);

	/**
	 * Checks whether all LEDs have pending operations
	 * @return Returns true if all fade effects are finished, otherwise false
	 */
	bool (*finishedAll)(void);

	/**
	 * Checks whether LEDs of @p type have pending operations
	 * @param type  LED type
	 * @return Returns true if all fade effects are finished, otherwise false
	 */
	bool (*finished)(const enum mu_led_single_type type);
};

/* formatter:on */

extern const struct mu_led_single_if muLedSingle;

#ifdef __cplusplus
}
#endif

#endif /* MU_LED_SINGLE_H_ */

