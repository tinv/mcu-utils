
#ifndef MU_WORK_FAKE_H
#define MU_WORK_FAKE_H

#include <gmock/gmock.h>
#include <list>
#include <stdint.h>
#include "workqueue/work.h"
#include "clock_fake.h"

class MuWorkInterface
{
      public:
	virtual ~MuWorkInterface() {}
	virtual int init(work_handle_t *handle, workCallback cb) = 0;
	virtual int submit(work_handle_t *handle) = 0;
	virtual int cancel(work_handle_t *handle) = 0;

	virtual int initDelayable(work_delayable_handle_t *handle, workCallback cb) = 0;
	virtual int reschedule(work_delayable_handle_t *handle, int msec) = 0;
	virtual int cancelDelayable(work_delayable_handle_t *handle) = 0;

	virtual void tick() = 0;
	virtual void setClock(MuClockFake *clock) = 0;
};

class MuWorkFake : public MuWorkInterface
{
      public:
	MuWorkFake();
	virtual ~MuWorkFake()
	{
		mHandles.clear();
	}

	int init(work_handle_t *handle, workCallback cb);
	int submit(work_handle_t *handle);
	int cancel(work_handle_t *handle);

	int initDelayable(work_delayable_handle_t *handle, workCallback cb);
	int reschedule(work_delayable_handle_t *handle, int msec);
	int cancelDelayable(work_delayable_handle_t *handle);

	void tick();
	void setClock(MuClockFake *clock);

      private:
	std::list<work_handle_t *> mHandles;
	std::list<work_delayable_handle_t *> mDelayableHandles;
	MuClockFake *mClock;
};


extern MuWorkFake* MuWorkFakeObj;
extern struct mu_work_if muWorkFake;

#endif // MU_WORK_MOCK_H
