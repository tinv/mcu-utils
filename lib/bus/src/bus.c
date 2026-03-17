/*
* Copyright (c) 2026 TecInvent Electronics Ltd
*/

#include "bus/bus.h"
#include <errno.h>
#include <zephyr/sys/util.h>
#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mu_bus);

static int muBus_init(void)
{
	return 0;
}

static int muBus_publish(const struct zbus_channel* chan, const void* msg)
{
	return zbus_chan_pub(chan, msg, K_MSEC(100));
}

static int muBus_addObserver(const struct zbus_channel *chan, const struct zbus_observer* obs,
	struct zbus_observer_node* node)
{
	int rc = 0;

	rc = zbus_chan_add_obs_with_node(chan, obs, node, K_MSEC(100));

	if (rc < 0) {
		LOG_ERR("unable to add obs (err: %d)", rc);
	}

	return rc;
}

static int muBus_removeObserver(const struct zbus_channel *chan, const struct zbus_observer *obs)
{
	int rc = 0;

	rc = zbus_chan_rm_obs(chan, obs, K_MSEC(100));

	if (rc < 0) {
		LOG_ERR("unable to remove obs (err: %d)", rc);
	}

	return rc;
}

static int muBus_read(const struct zbus_channel *chan, void *msg)
{
	return zbus_chan_read(chan, msg, K_MSEC(100));
}

static const void* muBus_constMessage(const struct zbus_channel *chan)
{
	return zbus_chan_const_msg(chan);
}

const struct mu_bus_if muBus = {
	.init = muBus_init,
	.publish = muBus_publish,
	.addObserver = muBus_addObserver,
	.removeObserver = muBus_removeObserver,
	.read = muBus_read,
	.constMessage = muBus_constMessage
};
