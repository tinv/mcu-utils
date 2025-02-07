/*
* Copyright (c) 2024 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#include "leds/led_ctrl.h"
#include <errno.h>
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mu_led_ctrl);

#define MAX_BRIGHTNESS	100
#define SLEEP_DELAY_MS	100
#define FADE_DELAY_MS	10

#define FADE_DELAY	K_MSEC(FADE_DELAY_MS)
#define SLEEP_DELAY	K_MSEC(SLEEP_DELAY_MS)

enum op_type {
  brightness = 0,
  color
};

/**!
 * Applies new values to LEDs specified within @p ldev
 * @param ldev LED device
 * @param effect Type of desired effect
 * @param type Brightness or color
 * @return
 */
static int _led_ctrl_set(const led_dev_t *ldev,
                         const led_effect_t effect,
                         const enum op_type type
                         )
{
  int err;

  uint8_t *current = (type == brightness) ? ldev->brightness.current : ldev->color.current;
  uint8_t *target = (type == brightness) ? ldev->brightness.target : ldev->color.target;
  size_t size = (type == brightness) ? ldev->brightness.size : ldev->color.size;

  while(memcmp(current, target, size) != 0) {

    for (int j = 0; j < size; j++) {

      if (effect == led_effect_fade) {

        if (current[j] < target[j]) {
          current[j]++;
        } else if (current[j] > target[j]) {
          current[j]--;
        } else {
          current[j] = target[j];
        }

      } else {
        current[j] = target[j];
      }

      switch (type) {

      case brightness:

        if ((err = led_set_brightness(ldev->dev, j, current[j])) < 0)
        {
          return err;
        }

        break;

      case color:

        if ((j % 3) == 0) {
          if ((err = led_set_color(ldev->dev, j / 3, 3, &current[j != 0 ? j-3 : 0])) < 0)
          {
            return err;
          }
        }

        break;
      }

    }

    if (effect == led_effect_fade) {
      k_sleep(FADE_DELAY);
    }

  }

  return 0;

}

/**!
 * Sets a new brightness to dev
 * @param dev Device
 * @param effect Type of effect to be applied
 * @return Returns 0 on success, otherwise it return a negative number
 */
static int led_ctrl_set_brightness(
    const led_dev_t *dev,
    const led_effect_t effect
    )
{
  return _led_ctrl_set(dev, effect, brightness);
}

/**!
 * Sets a new color to dev
 * @param dev Device
 * @param effect Type of effect to be applied
 * @return Returns 0 on success, otherwise it return a negative number
 */
static int led_ctrl_set_color(const led_dev_t *dev, const led_effect_t effect)
{
  return _led_ctrl_set(dev, effect, color);
}

struct mu_led_ctrl_if muLedCtrl =
    {
        .setBrightness = led_ctrl_set_brightness,
        .setColor = led_ctrl_set_color
};