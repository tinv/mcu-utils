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
#include <math.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mu_led_ctrl);

#define REFRESH_MS		2

enum op_type {
	brightness = 0,
	color
};

struct led_ctrl_config {
	const struct device *dev;
	const k_tid_t *thread_id;
};

struct led_ctrl_data_buf {
	uint8_t *current;
	uint8_t *target;
	float *step;
	float *accu;
	const size_t size;
};

struct led_ctrl_data_module {
	const uint8_t index;
	struct led_ctrl_data_buf color;
	struct led_ctrl_data_buf brightness;
};

struct led_ctrl_data {
	struct led_ctrl_data_module *module;
	const size_t module_size;
	bool color_update_pending;
	bool brightness_update_pending;
	bool call_cb;
	struct k_mutex lock;
};

struct led_ctrl_dev {
	const struct led_ctrl_config config;
	struct led_ctrl_data data;
};

static void thread_entry_point(const uint8_t dev_idx, void *dummy0, void *dummy1);

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

#define PHANDLE_TO_DEVICE(node_id, prop, idx) DEVICE_DT_GET(DT_PHANDLE_BY_IDX(node_id, prop, idx)),

/* @formatter:off */

#define LED_CTRL_DATA_MODULE(led_node_id)                                                          \
	{                                                                                          \
		.index = DT_PROP_OR(led_node_id, index, DT_NODE_CHILD_IDX(led_node_id)),           \
	 	.color = {                                                                         \
			 .current = color_current_##led_node_id,                                   \
			 .target = color_target_##led_node_id,                                     \
                         .step = color_step_##led_node_id,                                         \
                         .accu = color_accu_##led_node_id,                                         \
			 .size = MIN(ARRAY_SIZE(color_current_##led_node_id),                      \
				     ARRAY_SIZE(color_target_##led_node_id)),                      \
		 },                                                                                \
		.brightness = {                                                                    \
			.current = brightness_current_##led_node_id,                               \
			.target = brightness_target_##led_node_id,                                 \
                        .step = brightness_step_##led_node_id,                                     \
                        .accu = brightness_accu_##led_node_id,                                     \
			.size = MIN(ARRAY_SIZE(brightness_current_##led_node_id),                  \
				ARRAY_SIZE(brightness_target_##led_node_id)),                      \
		},                                                                                 \
	},

#define LED_CTRL_MODULE_BUF(led_node_id, idx)                                                      \
	static uint8_t color_current_##led_node_id[DT_PROP_LEN_OR(led_node_id, color_mapping, 1)] = {0};  \
	static uint8_t color_target_##led_node_id[DT_PROP_LEN_OR(led_node_id, color_mapping, 1)] = {0};   \
        static float color_step_##led_node_id[DT_PROP_LEN_OR(led_node_id, color_mapping, 1)] = {1};\
	static float color_accu_##led_node_id[DT_PROP_LEN_OR(led_node_id, color_mapping, 1)] = {1};\
	static uint8_t brightness_current_##led_node_id[1] = {0};                                  \
	static uint8_t brightness_target_##led_node_id[1] = {0};                                   \
	static float brightness_step_##led_node_id[1] = {1};					   \
	static float brightness_accu_##led_node_id[1] = {1};


#define LED_CTRL_DATA(node_id, prop, idx)                                                          \
	DT_FOREACH_CHILD_VARGS(DT_PHANDLE_BY_IDX(node_id, prop, idx), LED_CTRL_MODULE_BUF, idx)    \
	static struct led_ctrl_data_module led_data_module_##idx[] = {                             \
		DT_FOREACH_CHILD(DT_PHANDLE_BY_IDX(node_id, prop, idx), LED_CTRL_DATA_MODULE)      \
	};

#define LED_DEVS_CTX(node_id, prop, idx, stacksize, priority)                                      \
	K_THREAD_DEFINE(led_dev_thread_##idx, stacksize, thread_entry_point, idx, NULL, NULL,      \
			priority, 0, 0);

#define LED_DEVS(node_id, prop, idx)                                                               \
	{                                                                                          \
		.config = {                                                                        \
			.dev = DEVICE_DT_GET(DT_PHANDLE_BY_IDX(node_id, prop, idx)),               \
			.thread_id = &led_dev_thread_##idx},                                       \
	 	.data = {                                                                          \
			.module = led_data_module_##idx,                                           \
		  	.module_size = ARRAY_SIZE(led_data_module_##idx),                          \
		  	.color_update_pending = false,                                             \
		  	.brightness_update_pending = false,                                        \
		  	.call_cb = false                                                           \
		}                                                                                  \
	},

/* @formatter:on */

#if !DT_PROP_OR(ZEPHYR_USER_NODE, mu_leds, 0)
DT_FOREACH_PROP_ELEM(ZEPHYR_USER_NODE, mu_leds, LED_CTRL_DATA)
#endif

#define STACKSIZE 1024
#define PRIORITY  7

#if !DT_PROP_OR(ZEPHYR_USER_NODE, mu_leds, 0)
DT_FOREACH_PROP_ELEM_VARGS(ZEPHYR_USER_NODE, mu_leds, LED_DEVS_CTX, STACKSIZE, PRIORITY)
static struct led_ctrl_dev led_devs[] = {DT_FOREACH_PROP_ELEM(ZEPHYR_USER_NODE, mu_leds, LED_DEVS)};
#endif

static led_ctrl_finished_cb finished_cb = NULL;

static bool is_color_fade_finished(const uint8_t idx)
{
	bool finished = true;
	struct led_ctrl_data *data = &led_devs[idx].data;

	if (data->color_update_pending) {
		for (int i = 0; i < data->module_size; i++) {

			if (memcmp(data->module[i].color.current, data->module[i].color.target,
				   data->module[i].color.size) != 0) {
				finished = false;
				data->call_cb = true;
				break;
			}
		}
	}

	if (finished) {
		data->color_update_pending = false;
	}

	return finished;
}

static bool is_brightness_fade_finished(const uint8_t idx)
{
	bool finished = true;
	struct led_ctrl_data *data = &led_devs[idx].data;

	if (data->brightness_update_pending) {
		for (int i = 0; i < data->module_size; i++) {

			if (memcmp(data->module[i].brightness.current,
				   data->module[i].brightness.target,
				   data->module[i].brightness.size) != 0) {
				finished = false;
				break;
			}
		}
	}

	if (finished) {
		data->brightness_update_pending = false;
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
	const struct led_ctrl_config *config = &led_devs[dev_idx].config;
	struct led_ctrl_data *data = &led_devs[dev_idx].data;
	int ret = 0;
	bool cf = true;
	bool bf = true;

	k_mutex_init(&data->lock);

	while (1) {

		if (data->brightness_update_pending || data->color_update_pending) {
			ret = muLedCtrl.update(dev_idx);
		}

		if (ret < 0) {
			LOG_ERR("Unable to set color / brightness on '%s' (err %d)",
				config->dev->name, ret);
			ret = 0;
		}

		cf = is_color_fade_finished(dev_idx);
		bf = is_brightness_fade_finished(dev_idx);

		/* Notify that target reached on device dev_idx */
		if (data->call_cb && cf && bf) {
			data->call_cb = false;
			if (finished_cb != NULL) {
				finished_cb(dev_idx);
			}
		}

		if (cf && bf) {
			k_sleep(K_MSEC(10));
		}
	}
}

static void thread_entry_point(const uint8_t dev_idx, void *dummy0, void *dummy1)
{
	ARG_UNUSED(dummy0);
	ARG_UNUSED(dummy1);
	thread_loop(dev_idx);
}

static void calc_progression(const uint8_t* current, const uint8_t* target, float* step,
			      float* accu, const int timeMs)
{
	float calcStep;

	/* One-cycle update to reach target in case timeMs is zero */
	if (timeMs == 0) {
		calcStep = *target - *current;
	} else {
		calcStep = (*target - *current) / (timeMs / REFRESH_MS * 1.0);
	}

	*step = calcStep;
	*accu = 0;
}

static bool adjust_current_val(uint8_t* current, uint8_t* target, float* step,
			       float* accu)
{
	if (*current == *target) {
		return false;
	}

	if (*step > 0.0 && *current >= *target) {
		*current = *target;
		return true;
	} else if (*step < 0.0 && *current <= *target) {
		*current = *target;
		return true;
	} else {
		int calcStep = 0;

		if (*step >= 1.0 || *step <= -1.0) {
			calcStep = roundf(*step);
			*current += calcStep;
		} else {
			*accu += *step;
			if (*accu >= 1.0 || *accu <= -1.0) {
				calcStep = roundf(*accu);
				*current += calcStep;
				*accu = 0.0;
				return true;
			}
		}
	}

	return false;
}

static int led_ctrl_set_brightness(const uint8_t idx, const uint8_t led_num, const uint8_t value,
				   const int timeMs)
{
	__ASSERT(idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len");
	struct led_ctrl_data *data = &led_devs[idx].data;

	k_mutex_lock(&data->lock, K_FOREVER);
	data->brightness_update_pending = true;
	data->module[led_num].brightness.target[0] =
		value <= MU_LED_CTRL_BRIGHTNESS_MAX ? value : MU_LED_CTRL_BRIGHTNESS_MAX;
	calc_progression(data->module[led_num].brightness.current,
			 data->module[led_num].brightness.target,
			 data->module[led_num].brightness.step,
			 data->module[led_num].brightness.accu, timeMs);
	k_mutex_unlock(&data->lock);
	return 0;
}

static int led_ctrl_set_brightness_all(const uint8_t idx, const uint8_t value, const int timeMs)
{
	__ASSERT(idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len");
	struct led_ctrl_data *data = &led_devs[idx].data;

	k_mutex_lock(&data->lock, K_FOREVER);

	data->brightness_update_pending = true;

	for (int i = 0; i < data->module_size; i++) {
		data->module[i].brightness.target[0] =
			value <= MU_LED_CTRL_BRIGHTNESS_MAX ? value : MU_LED_CTRL_BRIGHTNESS_MAX;
		calc_progression(data->module[i].brightness.current,
				 data->module[i].brightness.target, data->module[i].brightness.step,
				 data->module[i].brightness.accu, timeMs);
	}

	k_mutex_unlock(&data->lock);

	return 0;
}

static int led_ctrl_set_color(const uint8_t idx, const uint8_t led_num, const uint8_t red,
			      const uint8_t green, const uint8_t blue, const int timeMs)
{
	__ASSERT(idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len");
	struct led_ctrl_data *data = &led_devs[idx].data;

	k_mutex_lock(&data->lock, K_FOREVER);
	data->color_update_pending = true;

	switch (data->module[led_num].color.size) {
	case 3:
		data->module[led_num].color.target[2] =
			blue <= MU_LED_CTRL_COLOR_MAX ? blue : MU_LED_CTRL_COLOR_MAX;
		calc_progression(&data->module[led_num].color.current[2],
				 &data->module[led_num].color.target[2],
				 &data->module[led_num].color.step[2],
				 &data->module[led_num].color.accu[2], timeMs);
	case 2:
		data->module[led_num].color.target[1] =
			green <= MU_LED_CTRL_COLOR_MAX ? green : MU_LED_CTRL_COLOR_MAX;
		calc_progression(&data->module[led_num].color.current[1],
				 &data->module[led_num].color.target[1],
				 &data->module[led_num].color.step[1],
				 &data->module[led_num].color.accu[1], timeMs);
	case 1:
		data->module[led_num].color.target[0] =
			red <= MU_LED_CTRL_COLOR_MAX ? red : MU_LED_CTRL_COLOR_MAX;
		calc_progression(&data->module[led_num].color.current[0],
				 &data->module[led_num].color.target[0],
				 &data->module[led_num].color.step[0],
				 &data->module[led_num].color.accu[0], timeMs);
	default:
		break;
	}

	k_mutex_unlock(&data->lock);
	return 0;
}

static int led_ctrl_set_color_all(const uint8_t idx, const uint8_t red, const uint8_t green,
				  const uint8_t blue, const int timeMs)
{
	__ASSERT(idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len");
	struct led_ctrl_data *data = &led_devs[idx].data;

	k_mutex_lock(&data->lock, K_FOREVER);

	data->color_update_pending = true;

	for (int i = 0; i < data->module_size; i++) {
		switch (data->module[i].color.size) {
		case 3:
			data->module[i].color.target[2] =
				blue <= MU_LED_CTRL_COLOR_MAX ? blue : MU_LED_CTRL_COLOR_MAX;
			calc_progression(&data->module[i].color.current[2],
					 &data->module[i].color.target[2],
					 &data->module[i].color.step[2],
					 &data->module[i].color.accu[2], timeMs);
		case 2:
			data->module[i].color.target[1] =
				green <= MU_LED_CTRL_COLOR_MAX ? green : MU_LED_CTRL_COLOR_MAX;
			calc_progression(&data->module[i].color.current[1],
					 &data->module[i].color.target[1],
					 &data->module[i].color.step[1],
					 &data->module[i].color.accu[1], timeMs);
		case 1:
			data->module[i].color.target[0] =
				red <= MU_LED_CTRL_COLOR_MAX ? red : MU_LED_CTRL_COLOR_MAX;
			calc_progression(&data->module[i].color.current[0],
					 &data->module[i].color.target[0],
					 &data->module[i].color.step[0],
					 &data->module[i].color.accu[0], timeMs);
		default:
			break;
		}
	}
	k_mutex_unlock(&data->lock);

	return 0;
}

static int led_ctrl_update(const uint8_t idx)
{
	__ASSERT(idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len");
	int ret = 0;
	const struct led_ctrl_config *config = &led_devs[idx].config;
	struct led_ctrl_data *data = &led_devs[idx].data;
	bool update = false;

	for (int i = 0; i < data->module_size; i++) {

		if (data->color_update_pending) {

			/* Update colors */
			for (int j = 0; j < data->module[i].color.size; j++) {

				k_mutex_lock(&data->lock, K_FOREVER);

				update = adjust_current_val(&data->module[i].color.current[j],
						   &data->module[i].color.target[j],
						   &data->module[i].color.step[j],
						   &data->module[i].color.accu[j]);

				if ((j % 3) == 0 && update) {
					if ((ret = led_set_color(
						     config->dev, i, data->module[i].color.size,
						     &data->module[i]
							      .color.current[j != 0 ? j - 3 : 0])) <
					    0) {
						k_mutex_unlock(&data->lock);
						LOG_ERR("Unable to set color err %d", ret);
						break;
					}
				}

				k_mutex_unlock(&data->lock);
			}
		}

		if (data->brightness_update_pending) {

			/* Update brightness */
			for (int j = 0; j < data->module[i].brightness.size; j++) {

				k_mutex_lock(&data->lock, K_FOREVER);

				update = adjust_current_val(&data->module[i].brightness.current[j],
						   &data->module[i].brightness.target[j],
						   &data->module[i].brightness.step[j],
						   &data->module[i].brightness.accu[j]);

				if (update && (ret = led_set_brightness(
					     config->dev, i,
					     data->module[i].brightness.current[j])) < 0) {
					k_mutex_unlock(&data->lock);
					LOG_ERR("Unable to set brightness err %d", ret);
					break;
				}

				k_mutex_unlock(&data->lock);
			}
		}
	}

	k_sleep(K_MSEC(REFRESH_MS));

	return ret;
}

static int led_ctrl_get_dev_qty(void)
{
	return ARRAY_SIZE(led_devs);
}

static int led_ctrl_init(led_ctrl_finished_cb cb)
{
	__ASSERT(cb, "led_ctrl_finished_cb cannot be null")
	finished_cb = cb;
	return 0;
}

/* @formatter:off */

struct mu_led_ctrl_if muLedCtrl = {
	.init = led_ctrl_init,
	.getDevQty = led_ctrl_get_dev_qty,
	.setBrightness = led_ctrl_set_brightness,
	.setBrightnessAll = led_ctrl_set_brightness_all,
	.setColor = led_ctrl_set_color,
	.setColorAll = led_ctrl_set_color_all,
	.update = led_ctrl_update
};

/* @formatter:on */