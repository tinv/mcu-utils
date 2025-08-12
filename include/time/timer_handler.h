// Copyright 2023 TecInvent Electronics Ltd

#ifndef MU_TIMER_HANDLER_H
#define MU_TIMER_HANDLER_H

#include <zephyr/kernel.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*timerCallback)(void *user_data);

typedef struct {
	struct k_timer timer;
	timerCallback cb;
	void *user_data;
} timer_handle_t;

#ifdef __cplusplus
}
#endif

#endif // MU_TIMER_HANDLER_H
