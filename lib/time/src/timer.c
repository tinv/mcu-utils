// Copyright 2025 TecInvent Electronics Ltd

#include "time/timer.h"
#include <zephyr/kernel.h>
#include <string.h>

static void expiry_fn(struct k_timer *timer)
{
	timer_handle_t* handle = (timer_handle_t*)timer->user_data;

	if (handle) {
		handle->cb(handle->user_data);
	}
}

static int timer_init(timer_handle_t *handle, timerCallback cb, void *user_data)
{
	handle->cb = cb;
	handle->user_data = user_data;
	k_timer_init(&handle->timer, expiry_fn, NULL);
	k_timer_user_data_set(&handle->timer, handle);
	return 0;
}

static int timer_start(timer_handle_t *handle, uint32_t timeout_ms, uint32_t period_ms)
{
	k_timer_start(&handle->timer, K_MSEC(timeout_ms), K_MSEC(period_ms));
	return 0;
}

static int timer_stop(timer_handle_t *handle)
{
	k_timer_stop(&handle->timer);
	return 0;
}

static uint32_t timer_remaining_get(timer_handle_t *handle)
{
	return k_timer_remaining_get(&handle->timer);
}

const struct mu_timer_if muTimer = {
	.init = timer_init,
	.start = timer_start,
	.stop = timer_stop,
	.remaining_get = timer_remaining_get
};
