/*
* Copyright (c) 2024 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#include "leds/led_ctrl.h"
#include <errno.h>
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>

// #define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#define LOG_LEVEL LOG_LEVEL_INF
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

struct led_ctrl_dev {
  const struct device* dev;
  struct led_ctrl_data *data;
  const size_t data_size;
  struct k_mutex lock;
  const k_tid_t* thread_id;
};

struct led_ctrl_data_buf {
  uint8_t *current;
  uint8_t *target;
  const size_t size;
  bool change;
};

struct led_ctrl_data {
  const uint8_t index;
  struct led_ctrl_data_buf color;
  struct led_ctrl_data_buf brightness;
};

static void thread_entry_point(const uint8_t dev_idx, void *dummy0, void *dummy1);

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

#define PHANDLE_TO_DEVICE(node_id, prop, idx) \
  DEVICE_DT_GET(DT_PHANDLE_BY_IDX(node_id, prop, idx)),

#define LED_CTRL_NODE_DATA(led_node_id)                                        \
  {                                                                            \
    .index = DT_PROP_OR(led_node_id, index, DT_NODE_CHILD_IDX(led_node_id)),   \
    .color = {                                                                 \
      .current = color_current_##led_node_id,                                  \
      .target = color_target_##led_node_id,                                    \
      .size = MIN(ARRAY_SIZE(color_current_##led_node_id),                     \
                  ARRAY_SIZE(color_target_##led_node_id)),                     \
      .change = false                                                          \
    },                                                                         \
    .brightness = {                                                            \
      .current = brightness_current_##led_node_id,                             \
      .target = brightness_target_##led_node_id,                               \
      .size = MIN(ARRAY_SIZE(brightness_current_##led_node_id),                \
                         ARRAY_SIZE(brightness_target_##led_node_id)),         \
      .change = false                                                          \
    }                                                                          \
  },

#define LED_CTRL_BUF(led_node_id, idx) \
  static uint8_t color_current_##led_node_id[DT_PROP_LEN_OR(led_node_id, color_mapping, 1)] = { 0 }; \
  static uint8_t color_target_##led_node_id[DT_PROP_LEN_OR(led_node_id, color_mapping, 1)] = { 0 };  \
  static uint8_t brightness_current_##led_node_id[1] = { 0 }; \
  static uint8_t brightness_target_##led_node_id[1] = { 0 };


#define LED_CTRL_DATA(node_id, prop, idx)                                      \
  DT_FOREACH_CHILD_VARGS(DT_PHANDLE_BY_IDX(node_id, prop, idx), LED_CTRL_BUF, idx)  \
  static struct led_ctrl_data led_data_##idx[] = {                       \
    DT_FOREACH_CHILD(DT_PHANDLE_BY_IDX(node_id, prop, idx), LED_CTRL_NODE_DATA) \
  };


#define LED_DEVS_CTX(node_id, prop, idx, stacksize, priority)                  \
  K_THREAD_DEFINE(led_dev_thread_##idx, stacksize, thread_entry_point, idx, NULL, NULL, priority, 0, 0);

#define LED_DEVS(node_id, prop, idx)                                           \
{                                                                              \
    .dev = DEVICE_DT_GET(DT_PHANDLE_BY_IDX(node_id, prop, idx)),               \
    .data = led_data_##idx,                                                    \
    .data_size = ARRAY_SIZE( led_data_##idx ),                                 \
    .thread_id = &led_dev_thread_##idx                                         \
},


#if !DT_PROP_OR(ZEPHYR_USER_NODE, mu_leds, 0)
  DT_FOREACH_PROP_ELEM(ZEPHYR_USER_NODE, mu_leds, LED_CTRL_DATA )
#endif

#define STACKSIZE 1024
#define PRIORITY  7

#if !DT_PROP_OR(ZEPHYR_USER_NODE, mu_leds, 0)
  DT_FOREACH_PROP_ELEM_VARGS(ZEPHYR_USER_NODE, mu_leds, LED_DEVS_CTX, STACKSIZE, PRIORITY )
  static struct led_ctrl_dev led_devs[] = {
      DT_FOREACH_PROP_ELEM(ZEPHYR_USER_NODE, mu_leds, LED_DEVS)
  };
#endif



static bool is_color_fade_finished(const uint8_t idx)
{
  bool finished = true;
  const struct led_ctrl_dev *ldev = &led_devs[idx];

  if (ldev->data->color.change) {
    for(int i = 0; i < ldev->data_size; i++) {

      if (memcmp(ldev->data[i].color.current,
                 ldev->data[i].color.target,
                 ldev->data[i].color.size) != 0) {
        finished = false;
        break;
      }
    }
  }

  if (finished) {
    ldev->data->color.change = false;
  }

  return finished;
}

static bool is_brightness_fade_finished(const uint8_t idx)
{
  bool finished = true;
  const struct led_ctrl_dev *ldev = &led_devs[idx];

  if (ldev->data->brightness.change) {
    for (int i = 0; i < ldev->data_size; i++) {

      if (memcmp(ldev->data[i].brightness.current,
                 ldev->data[i].brightness.target,
                 ldev->data[i].brightness.size) != 0) {
        finished = false;
        break;
      }
    }
  }

  if (finished) {
    ldev->data->brightness.change = false;
  }

  return finished;
}


/**!
* Thread loop used by all
* @param led_dev
* @param mux
 */
void thread_loop(const uint8_t dev_idx)
{
  struct led_ctrl_dev *ldev = &led_devs[dev_idx];
  int ret = 0;

  k_mutex_init(&ldev->lock);

  while (1) {

    if (ldev->data->brightness.change || ldev->data->color.change) {
      ret = muLedCtrl.update(dev_idx, led_effect_fade);
    }

    if (ret < 0) {
      LOG_ERR("Unable to set color / brightness on '%s' (err %d)", ldev->dev->name, ret);
    }

    is_color_fade_finished(dev_idx);
    is_brightness_fade_finished(dev_idx);

    k_sleep(K_MSEC(10));
  }

}

static void thread_entry_point(const uint8_t dev_idx, void *dummy0, void *dummy1)
{
    ARG_UNUSED(dummy0);
    ARG_UNUSED(dummy1);
    thread_loop(dev_idx);
}

static int led_ctrl_set_brightness(const uint8_t idx, const uint8_t led_num, const uint8_t value)
{
  __ASSERT( idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len" );
  struct led_ctrl_dev *ldev = &led_devs[idx];

  for (int i = 0; i < ldev->data_size; i++) {
    if (ldev->data[i].index == led_num) {
      k_mutex_lock(&ldev->lock, K_FOREVER);
      ldev->data->brightness.change = true;
      ldev->data[i].brightness.target[0] = value <= MU_LED_CTRL_BRIGHTNESS_MAX ? value : MU_LED_CTRL_BRIGHTNESS_MAX;
      k_mutex_unlock(&ldev->lock);
      return 0;
    }
  }

  return -EINVAL;
}

static int led_ctrl_set_brightness_all(const uint8_t idx, const uint8_t value)
{
  __ASSERT( idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len" );
  struct led_ctrl_dev *ldev = &led_devs[idx];

  k_mutex_lock(&ldev->lock, K_FOREVER);

  ldev->data->brightness.change = true;

  for (int i = 0; i < ldev->data_size; i++) {
    ldev->data[i].brightness.target[0] = value <= MU_LED_CTRL_BRIGHTNESS_MAX ? value : MU_LED_CTRL_BRIGHTNESS_MAX;
  }
  k_mutex_unlock(&ldev->lock);

  return 0;
}

static int led_ctrl_set_color(const uint8_t idx, const uint8_t led_num, const uint8_t red, const uint8_t green, const uint8_t blue)
{
  __ASSERT( idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len" );
  struct led_ctrl_dev *ldev = &led_devs[idx];

  for (int i = 0; i < ldev->data_size; i++) {
    if (ldev->data[i].index == led_num) {
      k_mutex_lock(&ldev->lock, K_FOREVER);
      ldev->data->color.change = true;

      switch (ldev->data[i].color.size) {
      case 3:
        ldev->data[i].color.target[2] = blue <= MU_LED_CTRL_COLOR_MAX ? blue : MU_LED_CTRL_COLOR_MAX;
      case 2:
        ldev->data[i].color.target[1] = green <= MU_LED_CTRL_COLOR_MAX ? green : MU_LED_CTRL_COLOR_MAX;
      case 1:
        ldev->data[i].color.target[0] = red <= MU_LED_CTRL_COLOR_MAX ? red : MU_LED_CTRL_COLOR_MAX;
      default:
        break;
      }
      k_mutex_unlock(&ldev->lock);
      return 0;
    }
  }

  return -EINVAL;
}

static int led_ctrl_set_color_all(const uint8_t idx, const uint8_t red, const uint8_t green, const uint8_t blue)
{
  __ASSERT( idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len" );
  struct led_ctrl_dev *ldev = &led_devs[idx];

  k_mutex_lock(&ldev->lock, K_FOREVER);

  ldev->data->color.change = true;

  for (int i = 0; i < ldev->data_size; i++) {
    switch (ldev->data[i].color.size) {
    case 3:
      ldev->data[i].color.target[2] = blue <= MU_LED_CTRL_COLOR_MAX ? blue : MU_LED_CTRL_COLOR_MAX;
    case 2:
      ldev->data[i].color.target[1] = green <= MU_LED_CTRL_COLOR_MAX ? green : MU_LED_CTRL_COLOR_MAX;
    case 1:
      ldev->data[i].color.target[0] = red <= MU_LED_CTRL_COLOR_MAX ? red : MU_LED_CTRL_COLOR_MAX;
    default:
      break;
    }
  }
  k_mutex_unlock(&ldev->lock);

  return 0;
}

static int led_ctrl_update(const uint8_t idx, const led_effect_t effect)
{
  __ASSERT( idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len" );
  int ret = 0;
  struct led_ctrl_dev *ldev = &led_devs[idx];

  for(int i = 0; i < ldev->data_size; i++) {

    /* Update colors */
    for (int j = 0; j < ldev->data[i].color.size; j++) {

      k_mutex_lock(&ldev->lock, K_FOREVER);

      if (effect == led_effect_fade) {

        if (ldev->data[i].color.current[j] < ldev->data[i].color.target[j]) {
          ldev->data[i].color.current[j]++;
        } else if (ldev->data[i].color.current[j] > ldev->data[i].color.target[j]) {
          ldev->data[i].color.current[j]--;
        } else {
          ldev->data[i].color.current[j] = ldev->data[i].color.target[j];
        }

      } else {
        ldev->data[i].color.current[j] = ldev->data[i].color.target[j];
      }

      if ((j % 3) == 0) {
        if ((ret = led_set_color(ldev->dev,
                                 ldev->data[i].index,
                                 ldev->data[i].color.size,
                                 &ldev->data[i].color.current[j != 0 ? j-3 : 0])) < 0)
        {
          k_mutex_unlock(&ldev->lock);
          break;
        }

      }

      k_mutex_unlock(&ldev->lock);

    }

    /* Update brightness */
    for (int j = 0; j < ldev->data[i].brightness.size; j++) {

      k_mutex_lock(&ldev->lock, K_FOREVER);

      if (effect == led_effect_fade) {

        if (ldev->data[i].brightness.current[j] < ldev->data[i].brightness.target[j]) {
          ldev->data[i].brightness.current[j]++;
        } else if (ldev->data[i].brightness.current[j] > ldev->data[i].brightness.target[j]) {
          ldev->data[i].brightness.current[j]--;
        } else {
          ldev->data[i].brightness.current[j] = ldev->data[i].brightness.target[j];
        }

      } else {
        ldev->data[i].brightness.current[j] = ldev->data[i].brightness.target[j];
      }

      if ((ret = led_set_brightness(ldev->dev, ldev->data[i].index,
                                    ldev->data[i].brightness.current[j])) <
          0) {
        k_mutex_unlock(&ldev->lock);
        break;
      }

      k_mutex_unlock(&ldev->lock);

    }

    k_sleep(K_MSEC(10));

  }

  return ret;
}

int led_ctrl_get_dev_qty(void)
{
  return ARRAY_SIZE(led_devs);
}

struct mu_led_ctrl_if muLedCtrl =
{
  .getDevQty = led_ctrl_get_dev_qty,
  .setBrightness = led_ctrl_set_brightness,
  .setBrightnessAll = led_ctrl_set_brightness_all,
  .setColor = led_ctrl_set_color,
  .setColorAll = led_ctrl_set_color_all,
  .update = led_ctrl_update
};