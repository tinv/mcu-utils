/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef MU_LED_SINGLE_H_
#define MU_LED_SINGLE_H_

#include <stdint.h>
#include <leds/led_ctrl.h>


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

struct mu_led_single_if {
	int (*init)(const struct mu_led_ctrl_if *led_ctrl);
	int (*setMap)(const enum mu_led_single_type type, const struct mu_led_single_pos_map *map, size_t size);
	int (*setSingle)(const enum mu_led_single_type type, unsigned int num, uint8_t brightness);
	int (*setAll)(const enum mu_led_single_type type, uint8_t brightness);
};

extern struct mu_led_single_if muLedSingle;

#ifdef __cplusplus
}
#endif

#endif /* MU_LED_SINGLE_H_ */

