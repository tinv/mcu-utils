// Copyright 2022 TecInvent Electronics Ltd

#ifndef MU_DEVICE_H
#define MU_DEVICE_H

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct device_if
{
    bool (*isReady)(const struct device *dev);
    const struct device* device;
};

#ifdef __cplusplus
}
#endif

#endif // MU_DEVICE_H
