// Copyright 2023 TecInvent Electronics Ltd

#ifndef MU_UPTIME_H
#define MU_UPTIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    struct mu_uptime_if
    {
        int64_t (*uptime_get)(void);
    };

    extern struct mu_uptime_if muUptime;

#ifdef __cplusplus
}
#endif

#endif //MU_UPTIME_H
