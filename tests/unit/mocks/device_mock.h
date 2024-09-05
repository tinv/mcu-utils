// Copyright 2022 TecInvent Electronics Ltd

#ifndef TESTS_UNIT_MOCKS_DEVICE_MOCK_H_
#define TESTS_UNIT_MOCKS_DEVICE_MOCK_H_

#include <gmock/gmock.h>
#include "device.h"

class DeviceInterface {
public:
   virtual ~DeviceInterface() {}
   virtual bool isReady(const struct device *dev) = 0;
};

class DeviceMock : public DeviceInterface {
public:
   virtual ~DeviceMock() {}
   MOCK_METHOD(bool, isReady, (const struct device *dev));

};

extern DeviceMock* DevMockObj;
extern const struct device_if deviceMock;

#endif /* TESTS_UNIT_MOCKS_DEVICE_MOCK_H_ */
