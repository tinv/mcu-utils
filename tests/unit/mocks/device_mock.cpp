
#include "device_mock.h"

DeviceMock* DevMockObj = nullptr;

static bool isReady(const struct device *dev)
{
	return DevMockObj->isReady(dev);
}


const struct device dev = {
    .name = "deviceMock",
    .config = NULL,
    .api = NULL,
    .state = NULL,
    .data = NULL,
#ifdef CONFIG_PM_DEVICE
    .pm = NULL
#endif
};

const struct device_if deviceMock =
{
    .isReady = isReady,
    .device = &dev
};

