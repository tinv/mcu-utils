// Copyright 2022 TecInvent Electronics Ltd

#ifndef MU_GPIO_TARGET_H_
#define MU_GPIO_TARGET_H_

#include <stdbool.h>
#include <stdint.h>
#ifndef BUILD_UNIT_TESTS
#include <zephyr/drivers/gpio.h>
#endif
#include "util_preproc.h"

#ifdef BUILD_UNIT_TESTS

#define gpio_callback_handler_t
#define gpio_flags_t uint16_t
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define FILTER_TIMER_PERIOD_MSEC  1

struct mu_input_config
{
    int                       id;
    const char*               name;
    const char*               dsc;
#ifndef BUILD_UNIT_TESTS
    const struct gpio_dt_spec dt;
    gpio_callback_handler_t   handler;
    gpio_flags_t              flags;
#endif
    int                       filter_num_stable_val;
};

struct mu_output_config
{
    int                       id;
    const char*               name;
    const char*               dsc;
#ifndef BUILD_UNIT_TESTS
    const struct gpio_dt_spec dt;
#endif
};

struct mu_input_data
{
    int state;
    int old_state;
    int filtered_state;
#ifndef BUILD_UNIT_TESTS
    struct gpio_callback cb_data;
    gpio_flags_t flags;
#endif
};

struct mu_input_def
{
    const struct mu_input_config config;
    struct mu_input_data         data;
};

struct mu_output_data
{
    bool         running;
    unsigned int time_on_ms;
    unsigned int time_off_ms;
    unsigned int on;
    unsigned int off;
    int cycles;
};


struct mu_output_def
{
    const struct mu_output_config config;
    struct mu_output_data         data;
};


#define MU_GPIO_STATIC_INPUT_SET_CREATE( name, ... ) static struct mu_input_def name[] = { __VA_ARGS__ }

#define MU_GPIO_STATIC_OUTPUT_SET_CREATE( name, ... ) static struct mu_output_def name[] = { __VA_ARGS__ }

#define MU_GPIO_STATIC_GPIO_END { NULL }

#ifndef BUILD_UNIT_TESTS
#define MU_GPIO_STATIC_INPUT( ID, DTLABEL )                                                                            \
    {                                                                                                                  \
        .config =                                                                                                      \
            {                                                                                                          \
                .id      = ID,                                                                                         \
                .name    = xstr( ID ),                                                                                 \
                .dsc     = DT_PROP_OR( DT_NODELABEL( DTLABEL ), label, "N/A label" ),                                  \
                .dt      = GPIO_DT_SPEC_GET_OR( DT_NODELABEL( DTLABEL ), gpios, { 0 } ),                               \
                .handler = NULL,                                                                                       \
                .flags   = 0U,                                                                                         \
                .filter_num_stable_val = 0U                                                                            \
            },                                                                                                         \
        .data = {                                                                                                      \
            .state          = 0,                                                                                       \
            .old_state      = 0,                                                                                       \
            .filtered_state = 0                                                                                        \
        }                                                                                                              \
    }
#else
#define MU_GPIO_STATIC_INPUT( ID, DTLABEL )                                                                            \
    {                                                                                                                  \
        .config =                                                                                                      \
            {                                                                                                          \
                .id      = ID,                                                                                         \
                .name    = xstr( ID ),                                                                                 \
                .dsc     = DT_PROP_OR( DT_NODELABEL( DTLABEL ), label, "N/A label" ),                                  \
                .dt      = GPIO_DT_SPEC_GET_OR( DT_NODELABEL( DTLABEL ), gpios, { 0 } ),                               \
                .handler = NULL,                                                                                       \
                .flags   = 0U,                                                                                         \
                .filter_num_stable_val = 0U                                                                            \
            },                                                                                                         \
        .data = {                                                                                                      \
            .state          = 0,                                                                                       \
            .old_state      = 0,                                                                                       \
            .filtered_state = 0                                                                                        \
        }                                                                                                              \
    }
#endif


#define MU_GPIO_STATIC_INPUT_HANDLER( ID, DTLABEL, HANDLER, FLAGS )                                                    \
    {                                                                                                                  \
        .config =                                                                                                      \
            {                                                                                                          \
                .id      = ID,                                                                                         \
                .name    = xstr( ID ),                                                                                 \
                .dsc     = DT_PROP_OR( DT_NODELABEL( DTLABEL ), label, "N/A label" ),                                  \
                .dt      = GPIO_DT_SPEC_GET_OR( DT_NODELABEL( DTLABEL ), gpios, { 0 } ),                               \
                .handler = HANDLER,                                                                                    \
                .flags   = FLAGS,                                                                                      \
                .filter_num_stable_val = 0U                                                                            \
            },                                                                                                         \
        .data = {                                                                                                      \
            .state     = 0,                                                                                            \
            .old_state = 0,                                                                                            \
            .cb_data = { 0 },                                                                                          \
            .filtered_state = 0                                                                                        \
        }                                                                                                              \
    }

#define MU_GPIO_STATIC_INPUT_FILTERED( ID, DTLABEL, FILTER_MSEC )                                                      \
    {                                                                                                                  \
        .config =                                                                                                      \
            {                                                                                                          \
                .id      = ID,                                                                                         \
                .name    = xstr( ID ),                                                                                 \
                .dsc     = DT_PROP_OR( DT_NODELABEL( DTLABEL ), label, "N/A label" ),                                  \
                .dt      = GPIO_DT_SPEC_GET_OR( DT_NODELABEL( DTLABEL ), gpios, { 0 } ),                               \
                .handler = NULL,                                                                                       \
                .flags   = 0U,                                                                                         \
                .filter_num_stable_val = FILTER_MSEC / FILTER_TIMER_PERIOD_MSEC                                        \
            },                                                                                                         \
        .data = {                                                                                                      \
            .state     = 0,                                                                                            \
            .old_state = 0,                                                                                            \
            .filtered_state = 0                                                                                        \
        }                                                                                                              \
    }

#define MU_GPIO_STATIC_OUTPUT( ID, DTLABEL )                                                                           \
    {                                                                                                                  \
        .config =                                                                                                      \
            {                                                                                                          \
                .id   = ID,                                                                                            \
                .name = xstr( ID ),                                                                                    \
                .dsc  = (const char*)DT_PROP_OR( DT_NODELABEL( DTLABEL ), label, "N/A label" ),                        \
                .dt   = GPIO_DT_SPEC_GET_OR( DT_NODELABEL( DTLABEL ), gpios, { 0 } ),                                  \
            },                                                                                                         \
        .data = {                                                                                                      \
            .running     = false,                                                                                      \
            .time_on_ms  = 0,                                                                                          \
            .time_off_ms = 0,                                                                                          \
            .on          = 0,                                                                                          \
            .off         = 0,                                                                                          \
            .cycles      = 0,                                                                                          \
        }                                                                                                              \
    }

#define MU_GPIO_STATIC_SHELL_HELPER_EL( name ) xstr( name ) "\n"

#ifdef __cplusplus
}
#endif

#endif /* MU_GPIO_TARGET_H_ */
