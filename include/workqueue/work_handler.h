// Copyright 2023 TecInvent Electronics Ltd

#ifndef MU_WORK_HANDLER_H
#define MU_WORK_HANDLER_H

#include <zephyr/kernel.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*workCallback)(void);

typedef struct {
	struct k_work work;
	workCallback cb;
} work_handle_t;

#ifdef __cplusplus
}
#endif

#endif // MU_WORK_HANDLER_H
