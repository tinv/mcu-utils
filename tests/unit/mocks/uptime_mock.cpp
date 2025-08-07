#include "uptime_mock.h"

MuUptimeMock* MuUptimeMockObj = nullptr;

int64_t uptime_get()
{
    return MuUptimeMockObj->uptime_get();
}

const struct mu_uptime_if muUptimeMock =
{
    .uptime_get = uptime_get
};
