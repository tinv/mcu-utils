//
// Created by dina on 18.07.25.
//

#include "hsm_fn_mock.h"
#include <utils/hsm.h>
#include "utils/HsmTests.h"

HsmFnMock *HsmFnMockObj = nullptr;

//******************************************************************************

MU_HSM_DEF_FUNC_ENTRY(startup)
{

	HsmFnMockObj->startupEntry(dev);
}

MU_HSM_DEF_FUNC_LOOP(startup)
{
	return HsmFnMockObj->startupLoop(&dev);
}

//******************************************************************************

MU_HSM_DEF_FUNC_ENTRY(stopped)
{
	HsmFnMockObj->stoppedEntry(dev);
}

MU_HSM_DEF_FUNC_LOOP(stopped)
{
	return HsmFnMockObj->stoppedLoop(dev);
}

MU_HSM_DEF_FUNC_EXIT(stopped)
{
	HsmFnMockObj->stoppedExit(dev);
}

//******************************************************************************

MU_HSM_DEF_FUNC_ENTRY(running)
{
	HsmFnMockObj->runningEntry(dev);
}

MU_HSM_DEF_FUNC_LOOP(running)
{
	return HsmFnMockObj->runningLoop(dev);
}

MU_HSM_DEF_FUNC_EXIT(running)
{
	HsmFnMockObj->runningExit(dev);
}

//******************************************************************************

MU_HSM_DEF_FUNC_ENTRY(paused)
{
	HsmFnMockObj->pausedEntry(dev);
}

MU_HSM_DEF_FUNC_LOOP(paused)
{
	return HsmFnMockObj->pausedLoop(dev);
}

MU_HSM_DEF_FUNC_EXIT(paused)
{
	HsmFnMockObj->pausedExit(dev);
}

//******************************************************************************