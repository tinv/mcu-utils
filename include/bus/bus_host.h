/*
* Copyright (c) 2026 TecInvent Electronics Ltd
*/

#ifndef MU_BUS_HOST_H_
#define MU_BUS_HOST_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct zbus_channel;
struct zbus_observer;

typedef void (*zbus_cb_t)(const struct zbus_observer *obj);

struct zbus_observer_node {
	const struct zbus_channel *chan;
	const struct zbus_observer *obs;
};

struct zbus_channel {
	const char *name;
	void *message_buffer;
	size_t message_size;
};

struct zbus_observer {
	const char *name;
	zbus_cb_t callback;
};

// On target, this creates a complex kernel struct.
// On Host, it creates a simple struct with a buffer for the message.
#define ZBUS_CHAN_DEFINE(_name, _type, ...)		\
	_type _name##_msg_storage;			\
	const struct zbus_channel _name = {		\
		.name = #_name,				\
		.message_buffer = &_name##_msg_storage,	\
		.message_size = sizeof(_type)		\
	}

// Captures the static callback address
#define ZBUS_LISTENER_DEFINE(_name, _cb) \
	const struct zbus_observer _name = { .name = #_name, .callback = _cb }

#ifdef __cplusplus
}
#endif

#endif /* MU_BUS_HOST_H_ */

