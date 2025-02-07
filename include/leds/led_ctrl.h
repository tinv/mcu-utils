/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef MU_LED_CTRL_H_
#define MU_LED_CTRL_H_

#include <stdint.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum led_effect {
  led_effect_none,
  led_effect_fade
} led_effect_t;

typedef struct led_buf {
  uint8_t *current;
  uint8_t *target;
  size_t size;
} led_buf_t;

typedef struct led_dev {
  const struct device *dev;
  led_buf_t brightness;
  led_buf_t color;
} led_dev_t;

struct mu_led_ctrl_if
{
  int (*setBrightness)(const led_dev_t *dev, const led_effect_t effect);
  int (*setColor)(const led_dev_t *dev, const led_effect_t effect);
};

extern struct mu_led_ctrl_if muLedCtrl;

#ifdef __cplusplus
}
#endif

#endif /* MU_LED_CTRL_H_ */

