// Copyright 2025 TecInvent Electronics Ltd

#ifndef MU_TIMER_HANDLER_FAKE_H
#define MU_TIMER_HANDLER_FAKE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*timerCallback)(void *user_data);

typedef struct {
	uint64_t expiry;
	uint64_t timeout;
	uint64_t period;
	bool active;
	timerCallback cb;
	void *user_data;
} timer_handle_t;

#ifdef __cplusplus
}
#endif

#endif // MU_TIMER_HANDLER_FAKE_H
