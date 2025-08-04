#include "CustomGtestMacros.h"

#include <utils/hsm.h>
#include "hsm_fn_mock.h"
#include "HsmTests.h"

#include <zephyr/sys/util.h>

typedef enum {
	stStartup,
	stStopped,
	stRunning,
	stPaused,
	stMax = stPaused
} state_t;

typedef enum {
	evFailed,
	evStop,
	evRun,
	evPause
} event_t;

struct myUserData_t {
	int data1;
	int data2;
};

extern HsmFnMock *HsmFnMockObj;

class HsmFixture : public ::testing::Test
{
      protected:
	mu_hsmCtx ctx;

	myUserData_t myUserData = {0, 1};

	HsmFixture()
	{
		HsmFnMockObj = new ::testing::NiceMock<HsmFnMock>();
	}

	void SetUp()
	{
		ctx.states = states;
		ctx.statesSize = ARRAY_SIZE(states);
		ctx.events = events;
		ctx.eventsSize = ARRAY_SIZE(events);
		ctx.transitions = transitions;
		ctx.transitionsSize = ARRAY_SIZE(transitions);
		ctx.userData = &myUserData;
		muHsm.init(&ctx, stStartup, stMax);
	}

	void TearDown()
	{
		delete HsmFnMockObj;
		HsmFnMockObj = nullptr;
	}

	void triggerTransictionLoop(int event)
	{
		setNextEvent(event);
		muHsm.handleStateTransition(&ctx);
		muHsm.handleStateLoop(&ctx);
	}

	int getState()
	{
		return ctx.ctrl.currState;
	}

	void setNextEvent(int event)
	{
		ctx.ctrl.nextEvent = event;
	}

	const mu_hsmEvent_t events[4] = {MU_HSM_EVENT(evFailed), MU_HSM_EVENT(evStop),
					 MU_HSM_EVENT(evRun), MU_HSM_EVENT(evPause)};

	const mu_hsmState_t states[4] = {MU_HSM_STATE_ENTRY_LOOP_FN(stStartup, startup, NO_STATE),
					 MU_HSM_STATE_ALL_FN(stStopped, stopped, NO_STATE),
					 MU_HSM_STATE_ALL_FN(stRunning, running, NO_STATE),
					 MU_HSM_STATE_ALL_FN(stPaused, paused, NO_STATE)};

	const mu_hsmTransition_t transitions[8] = {
		MU_HSM_TRANSITION(stStartup, evRun, stRunning),
		MU_HSM_TRANSITION(stStartup, evFailed, stStopped),
		MU_HSM_TRANSITION(stRunning, evStop, stStopped),
		MU_HSM_TRANSITION(stRunning, evPause, stPaused),
		MU_HSM_TRANSITION(stRunning, evFailed, stStopped),
		MU_HSM_TRANSITION(stStopped, evRun, stRunning),
		MU_HSM_TRANSITION(stPaused, evRun, stRunning),
		MU_HSM_TRANSITION(stPaused, evStop, stStopped)};
};

TEST_F(HsmFixture, eventStringAssignedOnDefinitionUsingMacro)
{
	static const int Ev_0 = 0;
	mu_hsmEvent_t event = MU_HSM_EVENT(Ev_0);
	EXPECT_ARRAY_EQ(char, "Ev_0", event.name, 4);
}

TEST_F(HsmFixture, shouldTransitionFromStopToRunning)
{

	EXPECT_CALL(*HsmFnMockObj, stoppedEntry(testing::_));
	muHsm.init(&ctx, stStopped, stMax);
	EXPECT_EQ(getState(), stStopped);

	EXPECT_CALL(*HsmFnMockObj, stoppedExit(testing::_));
	EXPECT_CALL(*HsmFnMockObj, runningEntry(testing::_));
	EXPECT_CALL(*HsmFnMockObj, runningLoop(testing::_));
	triggerTransictionLoop(evRun);
	EXPECT_EQ(getState(), stRunning);
}

TEST_F(HsmFixture, shouldTransitionFromStopToRunningAndPaused)
{

	EXPECT_CALL(*HsmFnMockObj, stoppedEntry(testing::_));
	muHsm.init(&ctx, stStopped, stMax);

	EXPECT_CALL(*HsmFnMockObj, stoppedExit(testing::_));
	EXPECT_CALL(*HsmFnMockObj, runningEntry(testing::_));
	EXPECT_CALL(*HsmFnMockObj, runningLoop(testing::_));
	triggerTransictionLoop(evRun);
	EXPECT_EQ(getState(), stRunning);

	EXPECT_CALL(*HsmFnMockObj, runningExit(testing::_));
	EXPECT_CALL(*HsmFnMockObj, pausedEntry(testing::_));
	EXPECT_CALL(*HsmFnMockObj, pausedLoop(testing::_));
	triggerTransictionLoop(evPause);
	EXPECT_EQ(getState(), stPaused);
}

TEST_F(HsmFixture, shouldtriggerOnlyTransictionEntry)
{
	muHsm.init(&ctx, stStartup, stMax);
	setNextEvent(evRun);

	EXPECT_CALL(*HsmFnMockObj, runningEntry(testing::_));
	muHsm.handleStateTransition(&ctx);
}

TEST_F(HsmFixture, StartupLoop_ShouldNotBeCalled)
{

	EXPECT_CALL(*HsmFnMockObj, startupEntry(testing::_)).Times(1);
	muHsm.init(&ctx, stStartup, stMax);

	EXPECT_CALL(*HsmFnMockObj, startupLoop(testing::_)).Times(0);
	triggerTransictionLoop(evRun);
}

TEST_F(HsmFixture, NoEventDuringInit)
{

	EXPECT_CALL(*HsmFnMockObj, startupEntry(testing::_)).Times(1);

	EXPECT_EQ(ctx.ctrl.currEvent, NO_EVENT);

	muHsm.init(&ctx, stStartup, stMax);
}

TEST_F(HsmFixture, DuringPausedLoopIsLoopActive)
{

	muHsm.init(&ctx, stStartup, stMax);
	triggerTransictionLoop(evRun);

	EXPECT_CALL(*HsmFnMockObj, pausedLoop(testing::_));
	triggerTransictionLoop(evPause);

	EXPECT_EQ(ctx.ctrl.isLoop, true);
}
