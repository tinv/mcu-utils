// Copyright 2025 TecInvent Electronics Ltd

#ifndef MU_TIMER_H
#define MU_TIMER_H

#include <stdint.h>
#ifndef BUILD_TEST
#include "timer_handler.h"
#else
#include "timer_handler_fake.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct mu_timer_if
{
	int (*init)(timer_handle_t *handle, timerCallback cb, void *user_data);
	int (*start)(timer_handle_t *handle, uint32_t timeout_ms, uint32_t periodic_ms);
	int (*stop)(timer_handle_t *handle);
	uint32_t (*remaining_get)(timer_handle_t *handle);
};

extern struct mu_timer_if muTimer;

#ifdef __cplusplus
}
#endif

#endif // MU_TIMER_H
