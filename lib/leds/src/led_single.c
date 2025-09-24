/*
* Copyright (c) 2024 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#include <leds/led_single.h>
#include <errno.h>
#include <zephyr/sys/util.h>
#include <leds/led_ctrl.h>
#include <time/timer.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mu_led_single);

struct led_type_map {
	enum mu_led_single_type type;
	const struct mu_led_single_pos_map *map;
	size_t map_size;
	timer_handle_t* execTm;
	led_single_finished_cb finishedCb;
	bool finished;
};

static const struct mu_led_ctrl_if* ctrl;
static const struct mu_timer_if* timer;

static timer_handle_t tmRed;
static timer_handle_t tmGreen;
static timer_handle_t tmBlue;
static timer_handle_t tmAmber;
static timer_handle_t tmIr;

/* formatter:off */

static struct led_type_map led_map[] = {
	{
		.type = MU_LED_RED,
		.map = NULL,
		.map_size = 0,
		.execTm = &tmRed,
		.finishedCb = NULL,
		.finished = true
	},
	{
		.type = MU_LED_GREEN,
		.map = NULL,
		.map_size = 0,
		.execTm = &tmGreen,
		.finishedCb = NULL,
		.finished = true
	},
	{
		.type = MU_LED_BLUE,
		.map = NULL,
		.map_size = 0,
		.execTm = &tmBlue,
		.finishedCb = NULL,
		.finished = true
	},
	{
		.type = MU_LED_AMBER,
		.map = NULL,
		.map_size = 0,
		.execTm = &tmAmber,
		.finishedCb = NULL,
		.finished = true
	},
	{
		.type = MU_LED_IR,
		.map = NULL,
		.map_size = 0,
		.execTm = &tmIr,
		.finishedCb = NULL,
		.finished = true
	}
};

/* formatter:on */

static void internalCb(void *user_data)
{
	struct led_type_map* map = (struct led_type_map*)(user_data);

	if (map) {

		map->finished = true;

		if (map->finishedCb) {
			map->finishedCb(map->type);
		}
	}
}

static int initTimers(void)
{
	int ret = 0;
	for (int i = 0; i < ARRAY_SIZE(led_map); i++) {
		led_map[i].finished = true;
		ret += timer->init(led_map[i].execTm, internalCb, &led_map[i]);
	}
	return ret;
}

static void startTimer(const enum mu_led_single_type type, led_single_finished_cb cb, int timeoutMs)
{
	for (int i = 0; i < ARRAY_SIZE(led_map); i++) {
		if (led_map[i].type == type) {
			led_map[i].finishedCb = cb;
			led_map[i].finished = false;
			timer->start(led_map[i].execTm, timeoutMs, 0);
			return;
		}
	}
}

static void stopTimer(const enum mu_led_single_type type)
{
	for (int i = 0; i < ARRAY_SIZE(led_map); i++) {
		if (led_map[i].type == type) {
			led_map[i].finishedCb = NULL;
			led_map[i].finished = true;
			timer->stop(led_map[i].execTm);
			return;
		}
	}
}

/**!
 *
 * @param app_pos Application point of view position
 * @param dev_idx Device index where the LED il connected
 * @param dt_pos Physical position where the LED in connected
 * @retval Return 0 on success
 * @retval -EINVAL When Application position for @type is invalid
 */
static int app_to_dt_pos(const enum mu_led_single_type type, uint8_t app_pos, uint8_t *dev_idx,
			  uint8_t *dt_pos)
{
	for (int i = 0; i < ARRAY_SIZE(led_map); i++) {

		if (led_map[i].type == type) {

			for (int j = 0; j < led_map[i].map_size; j++) {

				if (led_map[i].map[j].app_pos == app_pos) {

					*dev_idx = led_map[i].map[j].dev_idx;
					*dt_pos = led_map[i].map[j].dt_pos;
					return 0;
				}
			}
		}
	}

	return -EINVAL;
}

/**!
 *
 * @param led_ctrl LED controller
 * @return
 */
static int mu_led_single_init(const struct mu_led_ctrl_if *led_ctrl, const struct mu_timer_if* muTimer)
{
	__ASSERT(led_ctrl, "led_ctrl shall not be NULL");
	ctrl = led_ctrl;
	timer = muTimer;

	if (initTimers() < 0) {
		LOG_ERR("Unable to init timers");
		return -EINVAL;
	}
	return 0;
}

static int mu_led_set_map(const enum mu_led_single_type type, const struct mu_led_single_pos_map *map,
			  const size_t size)
{

	for (int i = 0; i < ARRAY_SIZE(led_map); i++) {

		if (led_map[i].type == type) {

			led_map[i].map = map;
			led_map[i].map_size = size;
			break;
		}
	}

	return 0;
}

/**!
 * Applies new values to all single LEDs corresponding to type @p type
 * @param type LED type
 * @param brightness Brightness value
 * @param timeMs
 * @param cb User callback. It will be called when the animation finishes
 * @return Return 0 if success, otherwise it returns a negative value
 */
static int mu_led_set_single_all(const enum mu_led_single_type type, const uint8_t brightness,
				 const int timeMs, led_single_finished_cb cb)
{
	int ret = 0;

	for (int i = 0; i < ARRAY_SIZE(led_map); i++) {

		if (led_map[i].type == type) {

			for (int j = 0; j < led_map[i].map_size; j++) {

				ret += ctrl->setBrightness(led_map[i].map[j].dev_idx,
							   led_map[i].map[j].dt_pos, brightness,
							   timeMs);
			}
		}
	}

	if (ret < 0) {
		LOG_ERR("Unable to apply single brightness to all (err: %d)", ret);
		return ret;
	}

	startTimer(type, cb, timeMs);

	return ret;
}

/**!
 * Applies a new value to red LED @p num
 * @param type LED type
 * @param num LED number
 * @param brightness Brightness value
 * @param timeMs
 * @param cb User callback. It will be called when the animation finishes.
 * @return Return 0 if success, otherwise it returns a negative value
 */
static int mu_led_set_single(const enum mu_led_single_type type, const unsigned int num,
			     const uint8_t brightness, const int timeMs, led_single_finished_cb cb)
{
	uint8_t dev_idx = 0;
	uint8_t dt_pos = 0;
	int ret = app_to_dt_pos(type, num, &dev_idx, &dt_pos);

	if (ret < 0) {
		LOG_ERR("Unable to set single LED%d", num);
		return ret;
	}

	ret = ctrl->setBrightness(dev_idx, dt_pos, brightness, timeMs);

	if (ret < 0) {
		LOG_ERR("Unable to apply new single brightness to %d (err: %d)", num, ret);
		return ret;
	}

	startTimer(type, cb, timeMs);

	return ret;
}

static bool mu_led_single_finishedAll(void)
{
	for (int i = 0; i < ARRAY_SIZE(led_map); i++) {
		if (led_map[i].finished == false) {
			return false;
		}
	}

	return true;
}

static bool mu_led_single_finished(const enum mu_led_single_type type)
{
	for (int i = 0; i < ARRAY_SIZE(led_map); i++) {
		if (led_map[i].type == type) {
			return led_map[i].finished;
		}
	}

	return true;
}

const struct mu_led_single_if muLedSingle = {
	.init = mu_led_single_init,
	.setMap = mu_led_set_map,
	.setSingle = mu_led_set_single,
	.setAll = mu_led_set_single_all,
	.finishedAll = mu_led_single_finishedAll,
	.finished = mu_led_single_finished,
};
