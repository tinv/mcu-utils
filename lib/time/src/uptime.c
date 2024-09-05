// Copyright 2023 TecInvent Electronics Ltd

#include "time/uptime.h"
#include <zephyr/kernel.h>

int64_t uptime_get(void)
{
    return k_uptime_get();
}

struct mu_uptime_if muUptime =
{
    .uptime_get = uptime_get,
};
