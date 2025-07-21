// Copyright 2022 TecInvent Electronics Ltd

#ifndef MU_HSM_MOCK_H
#define MU_HSM_MOCK_H

#include <gmock/gmock.h>

class MuHsmInterface
{
      public:
	virtual ~MuHsmInterface()
	{
	}
	virtual int init(mu_hsmCtx_t *ctx, int initialState, int maxState);
	virtual void handleStateTransition(mu_hsmCtx_t *ctx);
	virtual void handleStateLoop(mu_hsmCtx_t *ctx);
	virtual const char *getEventName(const mu_hsmCtx_t *ctx, int event);
};

class MuHsmMock : public MuHsmInterface
{
      public:
	virtual ~HsmMock()
	{
	}

	MOCK_METHOD(int, init, (mu_hsmCtx_t * ctx, int initialState, int maxState));
	MOCK_METHOD(void, handleStateTransition, (mu_hsmCtx_t * ctx));
	MOCK_METHOD(void, handleStateLoop, (mu_hsmCtx_t * ctx));
	MOCK_METHOD(const char *, getEventName, (const mu_hsmCtx_t *ctx, int event));
};

extern const struct mu_hsm_if muHsm;
extern MuHsmMock *MuHsmMockObj;

#endif // MU_HSM_MOCK_H
