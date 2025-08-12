
#include "clock_fake.h"

MuClockFake::MuClockFake()
{

}

void MuClockFake::init()
{
	mCurrentTime = 0;
}

uint64_t MuClockFake::nowMs() const
{
	return mCurrentTime;
}

void MuClockFake::advance(uint64_t ms)
{
	mCurrentTime += ms;
}
