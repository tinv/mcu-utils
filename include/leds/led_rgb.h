/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef MU_LED_RGB_H_
#define MU_LED_RGB_H_

#include <stdint.h>
#include <leds/led_ctrl.h>
#include <time/timer.h>

#ifdef __cplusplus
extern "C" {
#endif

struct mu_led_rgb_pos_map {
	uint8_t dev_idx;
	uint8_t app_pos;
	uint8_t dt_pos;
};

typedef void (*led_rgb_finished_cb)(const uint8_t app_pos);

struct mu_led_rgb_if {
	/**
	 * Initilizes the RGB led sub-system
	 * @param led_ctrl LED controller interface
	 * @return Returns 0 on success, otherwise it returns a negative number
	 */
	int (*init)(const struct mu_led_ctrl_if *led_ctrl);

	/**
	 *
	 * @param map
	 * @param size
	 * @return
	 */
	int (*setMap)(const struct mu_led_rgb_pos_map *map, size_t size);

	/**!
	 * Sets the specific RGB LED color and brightness
	 * @param num LED number
	 * @param red Red color value
	 * @param green Green color value
	 * @param blue Blue color value
	 * @param brightness Brightness level
	 * @param timeMs
	 * @param cb User callback invoked once animation is completed
	 * @retval Returns 0 on success otherwise it returns a negative number
	 */
	int (*setSingle)(unsigned int num, uint8_t red, uint8_t green, uint8_t blue,
			 uint8_t brightness, const int timeMs, led_rgb_finished_cb cb);

	/**
	 * Sets all LEDs
	 * @param red Red channel value
	 * @param green Green channel value
	 * @param blue Blue channel value
	 * @param brightness Brightness
	 * @param timeMs
	 * @param cb User callback invoked once animation is completed
	 * @return Return 0 on success, otherwise it returns a negative number
	 */
	int (*setAll)(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness, const int timeMs,
		      led_rgb_finished_cb cb);

	/**
	 * Starts fade effect
	 */
	int (*start)(void);

	/**
	 * Checks whether all LEDs have pending operations
	 * @return Returns true if all fade effects are finished, otherwise false
	 */
	bool (*finishedAll)(void);
};

extern const struct mu_led_rgb_if muLedRgb;

#ifdef __cplusplus
}
#endif

#endif /* MU_LED_RGB_H_ */

