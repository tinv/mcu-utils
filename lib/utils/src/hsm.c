// Copyright 2022 TecInvent Electronics Ltd

#include "utils/hsm.h"
#include <autoconf.h>
#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mu_hsm);
#include <string.h>

static int _updateNextTransition(mu_hsmCtx_t* ctx);
static int _execEntry(mu_hsmCtx_t* ctx);
static int _execExit(mu_hsmCtx_t* ctx);
static int _execLoop(mu_hsmCtx_t* ctx);
static const mu_hsmState_t* _getState(const mu_hsmCtx_t* ctx, int state);

int hsm_init(mu_hsmCtx_t* ctx, int initialState, int maxState)
{
    int ret = 0;
    memset(&ctx->ctrl, 0, sizeof(mu_hsmCtrl_t));
    ctx->ctrl.currState = initialState;
    ctx->ctrl.maxState = maxState;
    ctx->ctrl.currEvent = ctx->ctrl.nextEvent = NO_EVENT;

    // Execute very first entry function
   if ((ret = _execEntry(ctx)) >= 0)
   {
       // Switch to the loop function for the initial state
       ctx->ctrl.isLoop = true;
   }

    return ret;
}

void hsm_handleStateTransition(mu_hsmCtx_t* ctx)
{

    if (ctx->ctrl.nextEvent >= 0 || ctx->ctrl.nextEvent == ANY_EVENT)
    {
        ctx->ctrl.currEvent = ctx->ctrl.nextEvent;
        ctx->ctrl.nextEvent = NO_EVENT;

        if (_updateNextTransition(ctx) >= 0)
        {
            if (ctx->ctrl.nextState != ctx->ctrl.currState)
            {
                // Stop loop
                ctx->ctrl.isLoop = false;

                // Execute current state exit function before executing next entry state
                _execExit(ctx);

                if (ctx->ctrl.nextState <= ctx->ctrl.maxState)
                {
                    // Update current state
                    ctx->ctrl.currState = ctx->ctrl.nextState;
                    _execEntry(ctx);
                }

                ctx->ctrl.isLoop = true;
            }
        }
    }
}

void hsm_handleStateLoop( mu_hsmCtx_t* ctx )
{
    if ( ctx->ctrl.isLoop )
    {
        _execLoop( ctx );
    }
}

const char* hsm_getEventName( const mu_hsmCtx_t* ctx, int event )
{
    __ASSERT_NO_MSG( ctx );

    for ( int i = 0; i < ctx->eventsSize; i++ )
    {
        if ( event == ctx->events[i].event )
        {
            return ctx->events[i].name;
        }
    }
    return NULL;
}

static int _updateNextTransition( mu_hsmCtx_t* ctx )
{
    __ASSERT_NO_MSG( ctx );
    const mu_hsmState_t* st = _getState( ctx, ctx->ctrl.currState );
    bool switchedToParent = false;

    do
    {
        // Iterate over transition table
        for (int i = 0; i < ctx->transitionsSize; i++)
        {
            // Match state
            if (st->state == ctx->transitions[i].state)
            {
                // Evaluate ANY_EVENT condition only if not switched to parent
                if (!switchedToParent && ctx->transitions[i].event == ANY_EVENT)
                {
                    LOG_DBG("Switching to %s because of ANY_EVENT condition", _getState(ctx, ctx->transitions[i].nextState)->name);
                    ctx->ctrl.nextState = ctx->transitions[i].nextState;
                    return 0;
                }
                // Match current event to find out which is the next stage
                else if (ctx->ctrl.currEvent == ctx->transitions[i].event)
                {
                    ctx->ctrl.nextState = ctx->transitions[i].nextState;
                    return 0;
                }
            }
        }
        // Try with parent state
        st = _getState(ctx, st->parent);
        switchedToParent = true;
    }
    while(st);

    return -ENOTR;
}

static int _execEntry(mu_hsmCtx_t* ctx)
{
    for (int i = 0; i < ctx->statesSize; i++)
    {
        if (ctx->ctrl.currState == ctx->states[i].state  && ctx->states[i].entry != 0)
        {
            LOG_INF("st %s", ctx->states[i].name);
            ctx->states[i].entry( ctx->userData );
            return 0;
        }
    }
    return -ENOENTRYFN;
}

static int _execExit(mu_hsmCtx_t* ctx)
{
    for (int i = 0; i < ctx->statesSize; i++)
    {
        if (ctx->ctrl.currState == ctx->states[i].state && ctx->states[i].exit != 0) {
            LOG_INF( "st %s exit", ctx->states[i].name );
            ctx->states[i].exit( ctx->userData, ctx->ctrl.nextState);
            return 0;
        }
    }
    return -ENOEXITFN;
}

static int _execLoop(mu_hsmCtx_t* ctx)
{
    __ASSERT_NO_MSG(ctx);

    int newEvent = NO_EVENT;

    for (int i = 0; i < ctx->statesSize; i++)
    {

        if (ctx->ctrl.currState == ctx->states[i].state)
        {
            if (ctx->states[i].loop != 0)
            {
                // Execute and evaluate new event
                newEvent = ctx->states[i].loop( ctx->userData );
                if (newEvent != NO_EVENT)
                {
                    if (newEvent != ANY_EVENT) {
                    	const char* name = hsm_getEventName( ctx, newEvent );
                    	LOG_INF( "ev %s", name != NULL ? name : "name not found" );
                    } else {
                    	LOG_DBG("ev ANY_EVENT");
                    }
                    ctx->ctrl.nextEvent = newEvent;
                }
                return 0;
            }
            else
            {
                LOG_WRN("No loop function found");
                ctx->ctrl.isLoop = false;
                return -ENOLOOPFN;
            }
        }
    }

    LOG_WRN("No matching state");
    return -ENOSTATE;
}

static const mu_hsmState_t* _getState(const mu_hsmCtx_t* ctx, int state)
{
    for (int i = 0; i < ctx->statesSize; i++)
    {
        if (state == ctx->states[i].state)
        {
            return &ctx->states[i];
        }
    }
    return NULL;
}

struct mu_hsm_if muHsm =
{
  .init                   = hsm_init,
  .handleStateTransition  = hsm_handleStateTransition,
  .handleStateLoop        = hsm_handleStateLoop,
  .getEventName           = hsm_getEventName
};

