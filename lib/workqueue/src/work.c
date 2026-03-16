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

static void work_delayable_fn(struct k_work *work)
{

	struct k_work_delayable *dwork = CONTAINER_OF(work, struct k_work_delayable, work);
	work_delayable_handle_t* handle = CONTAINER_OF(dwork, work_delayable_handle_t, work);

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

static int work_cancel(work_handle_t *handle)
{
	return k_work_cancel(&handle->work);
}

static int work_initDelayable(work_delayable_handle_t *handle, workCallback cb)
{
	handle->cb = cb;
	k_work_init_delayable(&handle->work, work_delayable_fn);
	return 0;
}

static int work_reschedule(work_delayable_handle_t *handle, int msec)
{
	return k_work_reschedule(&handle->work, K_MSEC(msec));
}

static int work_cancelDelayable(work_delayable_handle_t *handle)
{
	return k_work_cancel_delayable(&handle->work);
}

const struct mu_work_if muWork = {
	.init = work_init,
	.submit = work_submit,
	.cancel = work_cancel,
	.initDelayable = work_initDelayable,
	.reschedule = work_reschedule,
	.cancelDelayable = work_cancelDelayable,
};
