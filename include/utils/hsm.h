// Copyright 2022 TecInvent Electronics Ltd

#ifndef MU_HSM_H
#define MU_HSM_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NO_EVENT        -1
#define ANY_EVENT       -2
#define NO_STATE        -3
#define ENOLOOPFN       1
#define ENOENTRYFN      2
#define ENOEXITFN       3
#define ENOTR           4
#define ENOSTATE         5

typedef struct mu_hsmState mu_hsmState_t;
typedef struct mu_hsmTransition mu_hsmTransition_t;
typedef struct mu_hsmEvent mu_hsmEvent_t;
typedef struct mu_hsmCtrl mu_hsmCtrl_t;
typedef struct mu_hsmCtx mu_hsmCtx_t;

struct mu_hsmState
{
    int state;
    const char* name;
    int parent;
    void (*entry)(void* iface);
    int (*loop)(void* iface);
    void (*exit)(void* iface);
};

struct mu_hsmTransition
{
    int state;
    int event;
    int nextState;
} ;

struct mu_hsmEvent
{
    int event;
    const char* name;
};

struct mu_hsmCtrl
{
    int currState;      ///< HSM current state
    int nextState;          ///< HSM next state
    int maxState;			///< Max states (just to avoid overflows)
    int currEvent;
    int nextEvent;
    bool isLoop;			///< indicates if the current state is the Loop execution phase
};

struct mu_hsmCtx
{
  const mu_hsmState_t *states;
  int statesSize;
  const mu_hsmEvent_t *events;
  int eventsSize;
  const mu_hsmTransition_t *transitions;
  int transitionsSize;
  int initialState;
  int maxState;
  mu_hsmCtrl_t ctrl; ///< Hsm context
};

#define MU_HSM_STATE_ALL_FN(STATE, NAME, PARENT) \
    {                           \
        .state  = STATE,        \
        .name   = #STATE,        \
        .parent = PARENT,       \
        .entry  = NAME##Entry,  \
        .loop   = NAME##Loop,   \
        .exit   = NAME##Exit    \
    }

#define MU_HSM_STATE_ENTRY_LOOP_FN(STATE, NAME, PARENT) \
    {                           \
        .state  = STATE,        \
        .name   = #STATE,       \
        .parent = PARENT,       \
        .entry  = NAME##Entry,  \
        .loop   = NAME##Loop,   \
        .exit   = NULL          \
    }


#define MU_HSM_EVENT(EVENT) \
    {                    \
        .event = EVENT,   \
        .name = #EVENT   \
    }


#define MU_HSM_DECL_ALL_FN(NAME)   \
    void NAME##Entry( void* dev );         \
    int NAME##Loop( void* dev );           \
    void NAME##Exit( void* dev );

#define MU_HSM_DECL_ENTRY_LOOP_FN(NAME)    \
    void NAME##Entry( void* dev );    \
    int NAME##Loop( void* dev );      \

#define MU_HSM_DEF_FUNC_ENTRY(NAME)        \
    void NAME##Entry( void* dev )

#define MU_HSM_DEF_FUNC_LOOP(NAME)         \
    int NAME##Loop( void* dev )

#define MU_HSM_DEF_FUNC_EXIT(NAME)         \
    void NAME##Exit( void* dev )

#define MU_HSM_TRANSITION(STATE, EVENT, NEXTSTATE) \
    {                                    \
    .state = STATE,                      \
    .event = EVENT,                      \
    .nextState = NEXTSTATE               \
    }


struct mu_hsm_if
{
  int (*init)(mu_hsmCtx_t* ctx, int initialState, int maxState, void* iface);
  void (*handleStateTransition)(mu_hsmCtx_t* ctx);
  void (*handleStateLoop)( mu_hsmCtx_t* ctx );
  const char* (*getEventName)( const mu_hsmCtx_t* ctx, int event );
};

extern struct mu_hsm_if muHsm;

#ifdef __cplusplus
}
#endif

#endif //MU_HSM_H
