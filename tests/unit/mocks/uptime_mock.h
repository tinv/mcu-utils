
#ifndef MU_UPTIME_MOCK_H
#define MU_UPTIME_MOCK_H

#include <gmock/gmock.h>
#include <stdint.h>
#include "time/uptime.h"

class MuUptimeInterface
{
public:
    virtual ~MuUptimeInterface() {}
    virtual int64_t uptime_get() = 0;

};

class MuUptimeMock : public MuUptimeInterface
{
public:
    virtual ~MuUptimeMock() {}

    MOCK_METHOD( int64_t, uptime_get, () );

};

extern MuUptimeMock* MuUptimeMockObj;
extern const struct mu_uptime_if muUptimeMock;

#endif // MU_UPTIME_MOCK_H
