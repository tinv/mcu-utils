#include "tinv_uptime_mock.h"

TinvUptimeMock* TinvUptimeMockObj = nullptr;

int64_t uptime_get()
{
    return TinvUptimeMockObj->uptime_get();
}

const struct tinv_uptime_if tinvUptimeMock =
{
    .uptime_get = uptime_get
};
