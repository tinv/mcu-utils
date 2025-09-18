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

struct mu_work_if muWorkFake = {
	.init = work_init,
	.submit = work_submit,
};
