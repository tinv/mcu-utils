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
	uint8_t *stage;
	const size_t size;
	int time;
	float *step;
	float *accu;
	bool ongoing;
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
	 	.color = {									   \
			.current = color_current_##led_node_id,                                    \
			.target = color_target_##led_node_id,                                      \
			.stage = color_stage_##led_node_id,                                        \
			.size = MIN(ARRAY_SIZE(color_current_##led_node_id),                       \
				      ARRAY_SIZE(color_target_##led_node_id)),                     \
			.time = 0,                                                                 \
			.step = color_step_##led_node_id,                                          \
			.accu = color_accu_##led_node_id,                                          \
			.ongoing = false                                                             \
		 },                                                                                \
		.brightness = {                                                                    \
			.current = brightness_current_##led_node_id,                               \
			.target = brightness_target_##led_node_id,                                 \
			.size = MIN(ARRAY_SIZE(brightness_current_##led_node_id),                  \
					ARRAY_SIZE(brightness_target_##led_node_id)),              \
			.stage = brightness_stage_##led_node_id,                                   \
			.time = 0,                                                                 \
                        .step = brightness_step_##led_node_id,                                     \
                        .accu = brightness_accu_##led_node_id,                                     \
                        .ongoing = false                                                             \
		},                                                                                 \
	},

#define LED_CTRL_MODULE_BUF(led_node_id, idx)                                                      \
	static uint8_t color_current_##led_node_id[DT_PROP_LEN_OR(led_node_id, color_mapping, 1)] = {0};  \
	static uint8_t color_target_##led_node_id[DT_PROP_LEN_OR(led_node_id, color_mapping, 1)] = {0};   \
	static uint8_t color_stage_##led_node_id[DT_PROP_LEN_OR(led_node_id, color_mapping, 1)] = {0};   \
        static float color_step_##led_node_id[DT_PROP_LEN_OR(led_node_id, color_mapping, 1)] = {1};\
	static float color_accu_##led_node_id[DT_PROP_LEN_OR(led_node_id, color_mapping, 1)] = {1};\
	static uint8_t brightness_current_##led_node_id[1] = {0};                                  \
	static uint8_t brightness_target_##led_node_id[1] = {0};                                   \
	static uint8_t brightness_stage_##led_node_id[1] = {0};                                    \
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

static led_ctrl_finished_cb finished_cb_color = NULL;
static led_ctrl_finished_cb finished_cb_brightness = NULL;

static bool is_color_fade_finished(const uint8_t idx)
{
	bool finished = true;
	struct led_ctrl_data *data = &led_devs[idx].data;

	if (data->color_update_pending) {
		for (int i = 0; i < data->module_size; i++) {

			if (memcmp(data->module[i].color.current, data->module[i].color.target,
				   data->module[i].color.size) != 0) {
				finished = false;
				break;
			}

			if (data->module[i].color.ongoing) {
				finished = false;
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

			if (data->module[i].brightness.ongoing) {
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
	data->module[led_num].brightness.ongoing = true;
	data->module[led_num].brightness.stage[0] =
		value <= MU_LED_CTRL_BRIGHTNESS_MAX ? value : MU_LED_CTRL_BRIGHTNESS_MAX;
	data->module[led_num].brightness.time = timeMs;
	k_mutex_unlock(&data->lock);
	return 0;
}

static int led_ctrl_set_brightness_all(const uint8_t idx, const uint8_t value, const int timeMs)
{
	__ASSERT(idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len");
	struct led_ctrl_data *data = &led_devs[idx].data;

	for (int i = 0; i < data->module_size; i++) {
		led_ctrl_set_brightness(idx, i, value, timeMs);
	}

	return 0;
}

static int led_ctrl_start_brightness(void)
{
	bool pending = false;

	for (int i = 0; i < ARRAY_SIZE(led_devs); i++) {

		struct led_ctrl_data *data = &led_devs[i].data;

		k_mutex_lock(&data->lock, K_FOREVER);

		for (int j = 0; j < data->module_size; j++) {

			if (data->module[j].brightness.target[0] !=
			    data->module[j].brightness.stage[0]) {
				pending = true;
				data->module[j].brightness.target[0] =
					data->module[j].brightness.stage[0];
				calc_progression(data->module[j].brightness.current,
						 data->module[j].brightness.target,
						 data->module[j].brightness.step,
						 data->module[j].brightness.accu,
						 data->module[j].brightness.time);
			} else {

				if (data->module[j].brightness.ongoing) {
					pending = true;
				}
			}
		}

		data->brightness_update_pending = pending;

		k_mutex_unlock(&data->lock);

	}

	return 0;
}

static bool led_ctrl_is_brightness_finished(const uint8_t idx, const uint8_t led_num)
{
	struct led_ctrl_data *data = &led_devs[idx].data;

	return data->module[led_num].brightness.target[0] ==
		       data->module[led_num].brightness.stage[0] &&
	       data->module[led_num].brightness.target[0] ==
		       data->module[led_num].brightness.current[0];
}

static int led_ctrl_set_color(const uint8_t idx, const uint8_t led_num, const uint8_t red,
			      const uint8_t green, const uint8_t blue, const int timeMs)
{
	__ASSERT(idx >= ARRAY_SIZE(led_devs), "idx is greater than led_devs len");
	struct led_ctrl_data *data = &led_devs[idx].data;

	k_mutex_lock(&data->lock, K_FOREVER);

	data->module[led_num].color.time = timeMs;
	data->module[led_num].color.ongoing = true;

	switch (data->module[led_num].color.size) {
	case 3:
		data->module[led_num].color.stage[2] =
			blue <= MU_LED_CTRL_COLOR_MAX ? blue : MU_LED_CTRL_COLOR_MAX;
	case 2:
		data->module[led_num].color.stage[1] =
			green <= MU_LED_CTRL_COLOR_MAX ? green : MU_LED_CTRL_COLOR_MAX;
	case 1:
		data->module[led_num].color.stage[0] =
			red <= MU_LED_CTRL_COLOR_MAX ? red : MU_LED_CTRL_COLOR_MAX;
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

	for (int i = 0; i < data->module_size; i++) {
		led_ctrl_set_color(idx, i, red, green, blue, timeMs);
	}

	return 0;
}


static bool start_color(struct led_ctrl_data *data, int moduleIdx, int colorIdx)
{
	bool pending = false;

	if (data->module[moduleIdx].color.target[colorIdx] != data->module[moduleIdx].color.stage[colorIdx]) {
		pending = true;
		data->module[moduleIdx].color.target[colorIdx] = data->module[moduleIdx].color.stage[colorIdx];
		calc_progression(&data->module[moduleIdx].color.current[colorIdx], &data->module[moduleIdx].color.target[colorIdx],
				 &data->module[moduleIdx].color.step[colorIdx], &data->module[moduleIdx].color.accu[colorIdx],
				 data->module[moduleIdx].color.time);
	}

	return pending;
}

static int led_ctrl_start_color(void)
{
	bool pending = false;

	for (int i = 0; i < ARRAY_SIZE(led_devs); i++) {

		struct led_ctrl_data *data = &led_devs[i].data;

		k_mutex_lock(&data->lock, K_FOREVER);

		for (int j = 0; j < data->module_size; j++) {

			if (data->module[j].color.size == 3) {

				switch (data->module[j].color.size) {
				case 3:
					if (start_color(data, j, 2)) {
						pending = true;
					}
				case 2:
					if (start_color(data, j, 1)) {
						pending = true;
					}
				case 1:
					if (start_color(data, j, 0)) {
						pending = true;
					}
				default:
					break;
				}

				if (pending == false) {

					if (data->module[j].color.ongoing) {
						pending = true;
					}
				}
			}
		}

		data->color_update_pending = pending;

		k_mutex_unlock(&data->lock);
	}

	return 0;
}

static bool led_ctrl_is_color_finished(const uint8_t idx, const uint8_t led_num)
{
	struct led_ctrl_data *data = &led_devs[idx].data;

	return (memcmp(data->module[led_num].color.target, data->module[led_num].color.stage,
		data->module[led_num].color.size) == 0) &&
		(memcmp(data->module[led_num].color.target, data->module[led_num].color.current,
			data->module[led_num].color.size) == 0);
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

				if (data->module[i].color.size == 3 && update) {
					if ((ret = led_set_color(
						     config->dev, i, data->module[i].color.size,
						     data->module[i].color.current)) <
					    0) {
						k_mutex_unlock(&data->lock);
						LOG_ERR("Unable to set color err %d", ret);
						break;
					}
				}

				if ((update || data->module[i].color.ongoing) &&
				    memcmp(data->module[i].color.current,
					   data->module[i].color.target,
					   data->module[i].color.size) == 0) {

					data->module[i].color.ongoing = false;
					finished_cb_color(idx, i);
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

				if ((update || data->module[i].brightness.ongoing) &&
				    data->module[i].brightness.current[j] ==
				    data->module[i].brightness.target[j]) {

					data->module[i].brightness.ongoing = false;
					finished_cb_brightness(idx, i);
				}

				k_mutex_unlock(&data->lock);
			}
		}
	}

	k_sleep(K_MSEC(REFRESH_MS));

	return ret;
}

static int led_ctrl_set_color_cb(led_ctrl_finished_cb cb)
{
	finished_cb_color = cb;
	return 0;
}

static int led_ctrl_set_brightness_cb(led_ctrl_finished_cb cb)
{
	finished_cb_brightness = cb;
	return 0;
}

static int led_ctrl_get_dev_qty(void)
{
	return ARRAY_SIZE(led_devs);
}

static int led_ctrl_init(void)
{
	__ASSERT(cb, "led_ctrl_finished_cb cannot be null")
	return 0;
}

/* @formatter:off */

struct mu_led_ctrl_if muLedCtrl = {
	.init = led_ctrl_init,
	.getDevQty = led_ctrl_get_dev_qty,
	.setBrightnessCb = led_ctrl_set_brightness_cb,
	.setBrightness = led_ctrl_set_brightness,
	.setBrightnessAll = led_ctrl_set_brightness_all,
	.startBrightness = led_ctrl_start_brightness,
	.isBrightnessFinished = led_ctrl_is_brightness_finished,
	.setColorCb = led_ctrl_set_color_cb,
	.setColor = led_ctrl_set_color,
	.setColorAll = led_ctrl_set_color_all,
	.startColor = led_ctrl_start_color,
	.isColorFinished = led_ctrl_is_color_finished,
	.update = led_ctrl_update
};

/* @formatter:on */