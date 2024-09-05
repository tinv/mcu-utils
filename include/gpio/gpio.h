// Copyright 2022 TecInvent Electronics Ltd

#ifndef MU_GPIO_H_
#define MU_GPIO_H_

#ifndef BUILD_UNIT_TESTS
#include "gpio_target.h"
#else
#include "gpio_host.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct mu_gpio_if
{
    int (*init)(struct mu_input_def* in, struct mu_output_def* out);
    int (*getMaxId)();
    const char* (*getInName)( int id );
    const char* (*getOutName)( int id );
    bool (*isInById)( int id );
    bool (*isOutById)( int id );
    bool (*isInByName)( const char* name );
    bool (*isOutByName)( const char* name );
    int (*setOutStateById)( int id, const int state );
    int (*setOutStateByName)( const char* name, const int state );
    int (*setOutSeqById)( int id, uint32_t dur_on_ms, uint32_t dur_off_ms );
    int (*setOutSeqByName)( const char* name, uint32_t dur_on_ms, uint32_t dur_off_ms );
    int (*setOutCycleById)( int id, uint32_t dur_on_ms, uint32_t dur_off_ms, int cycles );
    int (*getOutStateById)( int id, int* value );
    int (*getOutStateByName)( const char* name, int* value );
    int (*getInStateById)( int id, int* value );
    int (*getInStateByName)( const char* name, int* value );
    int (*changedInStateById)( int id, bool* changed, int* value );
    int (*changedInStateByName)( const char* name, bool* changed, int* value );
};

extern struct mu_gpio_if muGpio;

#ifdef __cplusplus
}
#endif

#endif /* MU_GPIO_H_ */
