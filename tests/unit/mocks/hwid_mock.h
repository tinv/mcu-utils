
#ifndef MU_HWID_MOCK_H
#define MU_HWID_MOCK_H

#include <gmock/gmock.h>
#include <stdint.h>
#include "hwid/hwid.h"

class MuHwIdInterface
{
public:
    virtual ~MuHwIdInterface() {}
    virtual int init() = 0;
    virtual int code(const enum mu_hwid_type type, int *code) = 0;

};

class MuHwIdMock : public MuHwIdInterface
{
public:
    virtual ~MuHwIdMock() {}
    MOCK_METHOD( int, init, ());
    MOCK_METHOD( int, code, (const enum mu_hwid_type type, int *code));
};

extern const struct mu_hwid_if muHwIdMock;
extern MuHwIdMock* MuHwIdMockObj;

#endif //MU_HWID_MOCK_H
