//
// Created by dina on 18.07.25.
//

#ifndef MU_HSM_FN_MOCK_H
#define MU_HSM_FN_MOCK_H

#include <gmock/gmock.h>

#ifdef __cplusplus
extern "C" {
#endif

class HsmFnInterface
{
      public:
	virtual ~HsmFnInterface()
	{
	}
	virtual void startupEntry(void *dev) = 0;
	virtual int startupLoop(void *dev) = 0;
	virtual void stoppedEntry(void *dev) = 0;
	virtual int stoppedLoop(void *dev) = 0;
	virtual void stoppedExit(void *dev) = 0;
	virtual void runningEntry(void *dev) = 0;
	virtual int runningLoop(void *dev) = 0;
	virtual void runningExit(void *dev) = 0;
	virtual void pausedEntry(void *dev) = 0;
	virtual int pausedLoop(void *dev) = 0;
	virtual void pausedExit(void *dev) = 0;
};

class HsmFnMock : public HsmFnInterface
{
      public:
	virtual ~HsmFnMock()
	{
	}
	MOCK_METHOD(void, startupEntry, (void *dev));
	MOCK_METHOD(int, startupLoop, (void *dev));
	MOCK_METHOD(void, stoppedEntry, (void *dev));
	MOCK_METHOD(int, stoppedLoop, (void *dev));
	MOCK_METHOD(void, stoppedExit, (void *dev));
	MOCK_METHOD(void, runningEntry, (void *dev));
	MOCK_METHOD(int, runningLoop, (void *dev));
	MOCK_METHOD(void, runningExit, (void *dev));
	MOCK_METHOD(void, pausedEntry, (void *dev));
	MOCK_METHOD(int, pausedLoop, (void *dev));
	MOCK_METHOD(void, pausedExit, (void *dev));
};

#ifdef __cplusplus
}
#endif

#endif // MU_HSM_FN_MOCK_H
