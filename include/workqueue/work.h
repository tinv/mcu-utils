// Copyright 2025 TecInvent Electronics Ltd

#ifndef MU_WORK_H
#define MU_WORK_H

#include <stdint.h>
#ifndef BUILD_TEST
#include "work_handler.h"
#else
#include "work_handler_fake.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct mu_work_if
{
	int (*init)(work_handle_t *handle, workCallback cb);
	int (*submit)(work_handle_t *handle);
};

extern const struct mu_work_if muWork;

#ifdef __cplusplus
}
#endif

#endif // MU_WORK_H
