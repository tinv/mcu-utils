
#ifndef MU_WORK_FAKE_H
#define MU_WORK_FAKE_H

#include <gmock/gmock.h>
#include <list>
#include <stdint.h>
#include "workqueue/work.h"

class MuWorkInterface
{
      public:
	virtual ~MuWorkInterface() {}
	virtual int init(work_handle_t *handle, workCallback cb) = 0;
	virtual int submit(work_handle_t *handle) = 0;
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

      private:
	std::list<work_handle_t *> mHandles;
};


extern MuWorkFake* MuWorkFakeObj;
extern struct mu_work_if muWorkFake;

#endif // MU_WORK_MOCK_H
