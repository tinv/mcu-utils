// Copyright 2025 TecInvent Electronics Ltd

#include "workqueue/work.h"
#include <zephyr/kernel.h>

static void work_fn(struct k_work *work)
{
	work_handle_t* handle = CONTAINER_OF(work, work_handle_t, work);

	if (handle) {
		handle->cb();
	}
}

static int work_init(work_handle_t *handle, workCallback cb)
{
	handle->cb = cb;
	k_work_init(&handle->work, work_fn);
	return 0;
}

static int work_submit(work_handle_t *handle)
{
	return 	k_work_submit(&handle->work);
}

const struct mu_work_if muWork = {
	.init = work_init,
	.submit = work_submit,
};
