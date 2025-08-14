/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef MU_LED_RGB_H_
#define MU_LED_RGB_H_

#include <stdint.h>
#include <leds/led_ctrl.h>

#ifdef __cplusplus
extern "C" {
#endif

struct mu_led_rgb_pos_map {
	uint8_t dev_idx;
	uint8_t app_pos;
	uint8_t dt_pos;
};

struct mu_led_rgb_if {
	int (*init)(const struct mu_led_ctrl_if *led_ctrl);

	int (*setMap)(const struct mu_led_rgb_pos_map *map, size_t size);

	/**!
	 * Sets the specific RGB LED color and brightness
	 * @param num LED number
	 * @param red Red color value
	 * @param green Green color value
	 * @param blue Blue color value
	 * @param brightness Brightness level
	 * @retval Returns 0 on success
	 */
	int (*setSingle)(unsigned int num, uint8_t red, uint8_t green, uint8_t blue,
			 uint8_t brightness);
	int (*setAll)(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness);
};

extern const struct mu_led_rgb_if muLedRgb;

#ifdef __cplusplus
}
#endif

#endif /* MU_LED_RGB_H_ */

