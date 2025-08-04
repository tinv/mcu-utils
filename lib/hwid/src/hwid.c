/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#include "hwid/hwid.h"
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <errno.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER( mu_hwid );

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

struct gpio_hwid {
	struct gpio_dt_spec dt_hwid;
};


#define HWID_DEVS(node_id, prop, idx)								\
{                                     								\
        .dt_hwid = GPIO_DT_SPEC_GET(DT_PHANDLE_BY_IDX(node_id, prop, idx), gpios)               \
},

#if !DT_PROP_OR(ZEPHYR_USER_NODE, mu_hwid, 0)
static struct gpio_hwid hwid_devs[] = {
	DT_FOREACH_PROP_ELEM(ZEPHYR_USER_NODE, mu_hwid, HWID_DEVS)
};
#else
static struct gpio_hwid hwid_devs[] = {};
#endif

static int mu_hwid_init(void)
{
	int ret = 0;

	for (int i = 0; i < ARRAY_SIZE(hwid_devs); i++) {

		ret = gpio_pin_configure_dt(&hwid_devs[i].dt_hwid, GPIO_ACTIVE_HIGH + GPIO_INPUT);

		if (ret < 0) {
			LOG_ERR("Unable to configure hwid (gpio) index %d", i);
			return -EIO;
		}

	}

	return 0;
}

static int _get_gpio_code(void)
{
	uint32_t code = 0u;
	const size_t len = ARRAY_SIZE(hwid_devs);

	for (int i = 0; i < len; i++) {
		/* First element is MSB */
		const int val = gpio_pin_get_dt(&hwid_devs[i].dt_hwid);
		const int pos = len - 1 - i;
		LOG_DBG("Id %d: %d", pos, val);
		code += val << pos;
	}

	LOG_DBG("code: %d", code);
	return code;

}

static int mu_hwid_code(const enum mu_hwid_type type, int *code)
{

	switch (type) {

	case MU_HWID_TYPE_GPIO:
		*code = _get_gpio_code();
		break;

	case MU_HWID_TYPE_MEM:
	default:

		return -ENOTSUP;

	}

	return 0;

}

struct mu_hwid_if muHwId = {

	.init = mu_hwid_init,
	.code = mu_hwid_code

};

