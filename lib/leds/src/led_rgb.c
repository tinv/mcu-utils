/*
* Copyright (c) 2024 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#include <leds/led_rgb.h>
#include <errno.h>
#include <zephyr/sys/util.h>
#include <leds/led_ctrl.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mu_led_rgb);

static const struct mu_led_ctrl_if* ctrl;
const struct mu_led_rgb_pos_map *rgbMap = NULL;
size_t rgbMapSize;
static const struct mu_timer_if* timer;
static timer_handle_t tmFinished;
static led_rgb_finished_cb userCb = NULL;
static bool finished = true;

static void internalCb(void *user_data)
{
	finished = true;

	if (userCb) {
		userCb();
	}
}

/**
 *
 * @param app_pos Application point of view position
 * @param dev_idx Device index where the LED il connected
 * @param dt_pos Physical position where the LED in connected
 * @retval Return 0 on success
 * @retval -EINVAL When Application position for @type is invalid
 */
static int app_to_dt_pos(uint8_t app_pos, uint8_t *dev_idx, uint8_t *dt_pos)
{
	for (int i = 0; i < rgbMapSize; i++) {
		if (rgbMap[i].app_pos == app_pos) {
			*dev_idx = rgbMap[i].dev_idx;
			*dt_pos = rgbMap[i].dt_pos;
			return 0;
		}
	}

	return -EINVAL;
}

/**!
 *
 * @param led_ctrl LED controller
 * @return
 */
static int mu_led_rgb_init(const struct mu_led_ctrl_if *led_ctrl,
			   const struct mu_timer_if* muTimer)
{
	__ASSERT(led_ctrl, "led_ctrl shall not be NULL");
	ctrl = led_ctrl;
	timer = muTimer;
	finished = true;
	timer->init(&tmFinished, internalCb, NULL);
	return 0;
}

/**
 *
 * @param map
 * @param size
 * @return
 */
static int mu_led_set_map(const struct mu_led_rgb_pos_map *map, size_t size)
{
	rgbMap = map;
	rgbMapSize = size;
	return 0;
}

/**!
 * Applies a new value to RGB LED @p num
 * @param num LED number
 * @param red Red value
 * @param green Green value
 * @param blue Blue value
 * @param brightness Brightness value
 * @param timeMs
 * @return Return 0 if success, otherwise it returns a negative value
 */
static int mu_led_set_rgb(unsigned int num, uint8_t red, uint8_t green, uint8_t blue,
			  uint8_t brightness, const int timeMs, led_rgb_finished_cb cb)
{
	uint8_t dev_idx = 0;
	uint8_t dt_pos = 0;
	int ret = app_to_dt_pos(num, &dev_idx, &dt_pos);

	if (ret < 0) {
		LOG_ERR("Unable to set RGB LED%d", num);
		return ret;
	}

	ret = ctrl->setColor(dev_idx, dt_pos, red, green, blue, timeMs);

	if (ret < 0) {
		LOG_ERR("Unable to apply new RGB color to %d (err: %d)", num, ret);
		return ret;
	}

	ret = ctrl->setBrightness(dev_idx, dt_pos, brightness, timeMs);

	if (ret < 0) {
		LOG_ERR("Unable to apply new RGB brightness to %d (err: %d)", num, ret);
		return ret;
	}

	userCb = cb;
	finished = false;
	timer->start(&tmFinished, timeMs, 0);

	return ret;
}

/**!
 * Applies new values to all RGB LEDs
 * @param red Red value
 * @param green Green value
 * @param blue Blue value
 * @param brightness Brightness value
 * @param timeMs
 * @return Return 0 if success, otherwise it returns a negative value
 */
static int mu_led_set_rgb_all(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness,
			      const int timeMs, led_rgb_finished_cb cb)
{
	int ret = 0;

	for (int i = 0; i < rgbMapSize; i++) {
		ret += mu_led_set_rgb(rgbMap[i].app_pos, red, green, blue, brightness, timeMs, cb);
	}

	return ret;
}

static bool mu_led_rgb_finishedAll(void)
{
	return finished;
}

const struct mu_led_rgb_if muLedRgb = {
	.init = mu_led_rgb_init,
	.setMap = mu_led_set_map,
	.setSingle = mu_led_set_rgb,
	.setAll = mu_led_set_rgb_all,
	.finishedAll = mu_led_rgb_finishedAll
};
