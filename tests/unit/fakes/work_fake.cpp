// Copyright 2025 TecInvent Electronics Ltd

#include "work_fake.h"
#include <stddef.h>

MuWorkFake* MuWorkFakeObj = nullptr;

static int work_init(work_handle_t *handle, workCallback cb)
{
	return MuWorkFakeObj->init(handle, cb);
}

static int work_submit(work_handle_t *handle)
{
	return MuWorkFakeObj->submit(handle);
}

static int work_cancel(work_handle_t *handle)
{
	return MuWorkFakeObj->cancel(handle);
}


static int work_init_delayable(work_delayable_handle_t *handle, workCallback cb)
{
	return MuWorkFakeObj->initDelayable(handle, cb);
}

static int work_reschedule(work_delayable_handle_t *handle, int msec)
{
	return MuWorkFakeObj->reschedule(handle, msec);
}

static int work_cancel_delayable(work_delayable_handle_t *handle)
{
	return MuWorkFakeObj->cancelDelayable(handle);
}

MuWorkFake::MuWorkFake()
{

}

int MuWorkFake::init(work_handle_t *handle, workCallback cb)
{
	handle->cb = cb;
	mHandles.push_back(handle);
	return 0;
}

int MuWorkFake::submit(work_handle_t *handle)
{
	handle->cb();
	return 0;
}

int MuWorkFake::cancel(work_handle_t *handle)
{
	mHandles.remove(handle);
	return 0;
}


int MuWorkFake::initDelayable(work_delayable_handle_t *handle, workCallback cb)
{
	handle->cb = cb;
	mDelayableHandles.push_back(handle);
	return 0;

}

int MuWorkFake::reschedule(work_delayable_handle_t *handle, int msec)
{
	handle->expiry = mClock->nowMs() + msec;
	handle->active = true;
	return 0;
}

int MuWorkFake::cancelDelayable(work_delayable_handle_t *handle)
{
	mDelayableHandles.remove(handle);
	return 0;
}

void MuWorkFake::setClock(MuClockFake *clock)
{
	mClock = clock;
}

void MuWorkFake::tick()
{
	std::list<work_delayable_handle_t *>::iterator it;
	uint64_t now = mClock->nowMs();

	for (auto const it: mDelayableHandles) {

		if (!it->active || !it->cb) {
			continue;
		}

		if (now >= it->expiry) {

			it->active = false;
			it->cb();
		}
	}
}


struct mu_work_if muWorkFake = {
	.init = work_init,
	.submit = work_submit,
	.cancel = work_cancel,
	.initDelayable = work_init_delayable,
	.reschedule = work_reschedule,
	.cancelDelayable = work_cancel_delayable
};
