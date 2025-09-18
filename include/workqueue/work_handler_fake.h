// Copyright 2025 TecInvent Electronics Ltd

#ifndef MU_WORK_HANDLER_FAKE_H
#define MU_WORK_HANDLER_FAKE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*workCallback)(void);

typedef struct {
	workCallback cb;
} work_handle_t;

#ifdef __cplusplus
}
#endif

#endif // MU_WORK_HANDLER_FAKE_H
