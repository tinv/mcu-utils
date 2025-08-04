// Copyright 2022 TecInvent Electronics Ltd

#include "hsm_mock.h"
#include <hsm.h>

MuHsmMock *MuHsmMockObj = nullptr;

static int init(mu_hsmCtx_t *ctx, int initialState, int maxState)
{
	return MuHsmMockObj->init(ctx, initialState, maxState);
}

static void handleStateTransition(mu_hsmCtx_t *ctx)
{
	return MuHsmMockObj->handleStateTransition(ctx);
}

static void handleStateLoop(mu_hsmCtx_t *ctx)
{
	return MuHsmMockObj->handleStateLoop(ctx);
}

static const char *getEventName(const mu_hsmCtx_t *ctx, int event)
{
	return MuHsmMockObj->getEventName(ctx, event);
}

const struct mu_hsm_if muHsmMock = {

	.init = init,
	.handleStateTransition = handleStateTransition,
	.handleStateLoop = handleStateLoop,
	.getEventName = getEventName

};