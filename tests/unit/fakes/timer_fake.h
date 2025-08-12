
#ifndef MU_TIMER_MOCK_H
#define MU_TIMER_MOCK_H

#include <gmock/gmock.h>
#include <stdint.h>
#include "time/timer.h"
#include "clock_fake.h"

class MuTimerInterface
{
      public:
	virtual ~MuTimerInterface() {}
	virtual int init(timer_handle_t *handle, timerCallback cb, void *user_data) = 0;
	virtual int start(timer_handle_t *handle, uint32_t timeout_ms, uint32_t periodic_ms) = 0;
	virtual int stop(timer_handle_t *handle) = 0;
	virtual uint32_t remaining_get(timer_handle_t *handle) = 0;
	virtual void tick() = 0;
	virtual void setClock(MuClockFake *clock) = 0;
};

class MuTimerFake : public MuTimerInterface
{
      public:
	MuTimerFake();
	virtual ~MuTimerFake() {}
	int init(timer_handle_t *handle, timerCallback cb, void *user_data) override;
	int start(timer_handle_t *handle, uint32_t timeout_ms, uint32_t periodic_ms) override;
	int stop(timer_handle_t *handle) override;
	uint32_t remaining_get(timer_handle_t *handle) override;
	void tick();
	void setClock(MuClockFake *clock);

      private:
	timer_handle_t *mHandle;
	MuClockFake *mClock;
};


extern MuTimerFake* MuTimerFakeObj;
extern struct mu_timer_if muTimerFake;

#endif // MU_TIMER_MOCK_H
