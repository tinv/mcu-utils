/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef MU_LED_CTRL_H_
#define MU_LED_CTRL_H_

#include <stdint.h>
#include <zephyr/sys/util.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MU_LED_CTRL_BRIGHTNESS_MAX    100
#define MU_LED_CTRL_COLOR_MAX         255

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

/**!
 *
 */
typedef void (*led_ctrl_finished_cb)(const uint8_t idx);

struct mu_led_ctrl_if
{
  int (*init)(led_ctrl_finished_cb finished_cb);
  int (*getDevQty)(void);
  int (*setBrightness)(const uint8_t idx, const uint8_t led_num, const uint8_t value);
  int (*setBrightnessAll)(const uint8_t idx, const uint8_t value);
  int (*setColor)(const uint8_t idx, const uint8_t led_num, const uint8_t red, const uint8_t green, const uint8_t blue);
  int (*setColorAll)(const uint8_t idx, const uint8_t red, const uint8_t green, const uint8_t blue);
  int (*update)(const uint8_t idx, const led_effect_t effect);
};

extern struct mu_led_ctrl_if muLedCtrl;

#ifdef __cplusplus
}
#endif

#endif /* MU_LED_CTRL_H_ */

