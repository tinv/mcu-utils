// Copyright 2025 TecInvent Electronics Ltd

#include "timer_fake.h"
#include <stddef.h>

MuTimerFake* MuTimerFakeObj = nullptr;

static int timer_init(timer_handle_t *handle, timerCallback cb, void *user_data)
{
	return MuTimerFakeObj->init(handle, cb, user_data);
}

static int timer_start(timer_handle_t *handle, uint32_t timeout_ms, uint32_t period_ms)
{
	return MuTimerFakeObj->start(handle, timeout_ms, period_ms);
}

static int timer_stop(timer_handle_t *handle)
{
	return MuTimerFakeObj->stop(handle);
}

uint32_t timer_remaining_get(timer_handle_t *handle)
{
	return MuTimerFakeObj->remaining_get(handle);
}

MuTimerFake::MuTimerFake()
{

}

int MuTimerFake::init(timer_handle_t *handle, timerCallback cb, void *user_data)
{
	handle->cb = cb;
	handle->user_data = user_data;
	mHandles.push_back(handle);
	return 0;
}

int MuTimerFake::start(timer_handle_t *handle, uint32_t timeout_ms, uint32_t periodic_ms)
{
	handle->timeout = timeout_ms;
	handle->period = periodic_ms;
	handle->expiry = mClock->nowMs() + timeout_ms;
	handle->active = true;
	return 0;
}

int MuTimerFake::stop(timer_handle_t *handle)
{
	handle->active = false;
	return 0;
}

uint32_t MuTimerFake::remaining_get(timer_handle_t *handle)
{
	if (!handle->active || mClock->nowMs() >= handle->expiry) {
		return 0;
	}

	return handle->expiry - mClock->nowMs();
}

void MuTimerFake::setClock(MuClockFake *clock)
{
	mClock = clock;
}

void MuTimerFake::tick()
{
	std::list<timer_handle_t *>::iterator it;
	uint64_t now = mClock->nowMs();

	for (auto const it: mHandles) {

		if (!it->active || !it->cb) {
			continue;
		}

		if (now >= it->expiry) {
			it->cb(it->user_data);

			if (it->period > 0) {
				it->expiry += it->period;
			} else {
				it->active = false;
			}
		}
	}
}

struct mu_timer_if muTimerFake = {
	.init = timer_init,
	.start = timer_start,
	.stop = timer_stop,
	.remaining_get = timer_remaining_get
};
