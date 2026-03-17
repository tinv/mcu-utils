/*
* Copyright (c) 2026 TecInvent Electronics Ltd
*/

#ifndef MU_BUS_H_
#define MU_BUS_H_

#include <stdint.h>
#include <stddef.h>

#ifndef BUILD_TEST
#include <zephyr/zbus/zbus.h>
#else
#include "bus/bus_host.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct mu_bus_if {

	/**!
	 * @retval 0 on success
	 * @retval For other values refer to errno
	 */
	int (*init)(void);

	int (*publish)(const struct zbus_channel *chan, const void* msg);
	int (*addObserver)(const struct zbus_channel *chan, const struct zbus_observer* obs,
		struct zbus_observer_node* node);
	int (*removeObserver)(const struct zbus_channel *chan, const struct zbus_observer *obs);
	int (*read)(const struct zbus_channel *chan, void *msg);
	const void* (*constMessage)(const struct zbus_channel *chan);

};

extern const struct mu_bus_if muBus;

#ifdef __cplusplus
}
#endif

#endif /* MU_BUS_H_ */

