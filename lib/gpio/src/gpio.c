// Copyright 2022 TecInvent Electronics Ltd

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER( mu_gpio );

#include "gpio/gpio.h"
#include <string.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <stdbool.h>

#define BLINK_TIMER_PERIOD_MS 100
#define IO_EXISTS( io ) ( io->config.dt.port != NULL )

static void _filter_timer_handler( struct k_timer* dummy );
static void _seq_timer_handler( struct k_timer* dummy );

K_TIMER_DEFINE( filter_timer, _filter_timer_handler, NULL );
K_TIMER_DEFINE( seq_timer, _seq_timer_handler, NULL );

static struct mu_input_def*  _in  = NULL;
static struct mu_output_def* _out = NULL;

static int _not_found_id( int id )
{
    LOG_ERR( "io %d not found", id );
    return -1;
}

static int _not_found_name( const char* name )
{
    LOG_ERR( "io %s not found", name );
    return -1;
}

static void _filter_timer_handler( struct k_timer* dummy )
{
    struct mu_input_def* in = _in;

    while ( IO_EXISTS( in ) )
    {
        if ( in->config.filter_num_stable_val != 0 )
        {
            // Logical high
            if ( gpio_pin_get_dt( &in->config.dt ) )
            {
                in->data.filtered_state++;
            }
            else
            {
                in->data.filtered_state--;
            }

            // Saturate value
            if ( in->data.filtered_state < 0 )
            {
                in->data.filtered_state = 0;
            }
            else if ( in->data.filtered_state > in->config.filter_num_stable_val )
            {
                in->data.filtered_state = in->config.filter_num_stable_val;
            }
        }

        in++;
    }
}

static void _seq_timer_handler( struct k_timer* dummy )
{
    struct mu_output_def* out = _out;

    while ( IO_EXISTS( out ) )
    {
        if ( out->data.running )
        {
            LOG_DBG( "on: %d, off: %d", out->data.on, out->data.off );

            // Period ended
            if ( out->data.on == 0 && out->data.off == 0 )
            {
                // Reset values
                out->data.on  = out->data.time_on_ms;
                out->data.off = out->data.time_off_ms;
                // Switch back on
                gpio_pin_set_dt( &out->config.dt, true );
                // Decrement 'on' time and switch off when finished

                //Updates the cycles counter each time the data.on and data.off are re-initialized.
                if ( out->data.cycles > 0 ) {
                    if ( --out->data.cycles == 0 ) {
                        gpio_pin_set_dt(&out->config.dt, false);
                        out->data.running = false;
                    }
                }

            }
            else if ( out->data.on > 0 )
            {
                if ( --out->data.on == 0 )
                {
                    gpio_pin_set_dt( &out->config.dt, false );
                }
                // Decrement 'off' time only when 'on' time is expired
            }
            else if ( out->data.on == 0 )
            {
                out->data.off--;
            }
        }
        out++;
    }
}

/**
 * Iterates over the output table and fetches the output structure matching the provided id.
 * @param id Input id
 * @return Returns point to output structure or NULL if nothing matches id
 */
static const struct mu_input_def* _get_in_by_id( int id, const struct mu_input_def* inputs )
{
    while ( IO_EXISTS( inputs ) )
    {
        if ( id == inputs->config.id )
        {
            return inputs;
        }
        inputs++;
    }
    return NULL;
}

/**
 * Iterates over the input table and fetches the output structure matching the provided id.
 * @param name Input name
 * @param outputs Inputs array
 * @return Returns point to input structure or NULL if nothing matches name
 */
static const struct mu_input_def* _get_in_by_name( const char* name, const struct mu_input_def* inputs )
{
    while ( IO_EXISTS( inputs ) )
    {
        if ( strcmp( name, inputs->config.name ) == 0 )
        {
            return inputs;
        }
        inputs++;
    }
    return NULL;
}

/**
 * Iterates over the output table and fetches the output structure matching the provided id.
 * @param id Output id
 * @param outputs Outputs array
 * @return Returns point to output structure or NULL if nothing matches id
 */
static const struct mu_output_def* _get_out_by_id( int id, const struct mu_output_def* outputs )
{
    while ( IO_EXISTS( outputs ) )
    {
        if ( id == outputs->config.id )
        {
            return outputs;
        }
        outputs++;
    }
    return NULL;
}

/**
 * Iterates over the output table and fetches the output structure matching the provided name.
 * @param name Output name
 * @param outputs Outputs array
 * @return Returns point to output structure or NULL if nothing matches name
 */
static const struct mu_output_def* _get_out_by_name( const char* name, const struct mu_output_def* outputs )
{
    while ( IO_EXISTS( outputs ) )
    {
        if ( strcmp( name, outputs->config.name ) == 0 )
        {
            return outputs;
        }
        outputs++;
    }
    return NULL;
}

/**
 * Gets input state given input definition
 * @param in Pointer to input definition
 * @param value Location where to store logical state
 * @return Return 0 on success, 1 if state changed and a negative number if an error occurred.
 */
static int _get_in_state( const struct mu_input_def* in, int* value )
{
    __ASSERT( in, "In should not be NULL" );
    int                     ret  = 0;
    struct mu_input_data* data = (struct mu_input_data*)&in->data;

    if ( in )
    {
        // Filter is applied
        if ( in->config.filter_num_stable_val != 0 )
        {
            if ( data->filtered_state == in->config.filter_num_stable_val )
            {
                data->state = 1;
            }
            else if ( data->filtered_state == 0 )
            {
                data->state = 0;
            }
        }
        else
        {
            // Value less than 0 is an error
            if ( ( ret = gpio_pin_get_dt( &in->config.dt ) ) >= 0 )
            {
                data->state = ret;
            }
            else
            {
                return ret;
            }
        }

        // Changed
        if ( data->state != data->old_state )
        {
            LOG_INF( "'%s' -> %s", in->config.name, ( data->state > 0 ) ? "active" : "inactive" );
            data->old_state = data->state;
            ret             = 1;
        }
        else
        {
            ret = 0;
        }

        *value = data->state;
    }
    else
    {
        ret = _not_found_name( in->config.name );
    }

    return ret;
}

static void _set_out_state( const struct mu_output_def* out, const int active )
{
    __ASSERT( out, "Out shall not be NULL" );
    struct mu_output_data* data = (struct mu_output_data*)( &out->data );

    if ( ! active )
    {
        data->running = false;
    }
    LOG_DBG("Set %s to %s", out->config.name, active ? "Active" : "Inactive");
    gpio_pin_set_dt( &out->config.dt, active );
}

static int _get_out_state( const struct mu_output_def* out, int* active )
{
    __ASSERT( out, "Out shall not be NULL" );
    bool state = gpio_pin_get_dt( &out->config.dt );
    *active = ( state ) ? 1 : 0;
    return 0;
}

void _set_out_seq_cycles( const struct mu_output_def* out, const uint32_t dur_on_ms, const uint32_t dur_off_ms, const int cycles )
{
  __ASSERT( out, "Out shall not be NULL" );
  struct mu_output_data* data = (struct mu_output_data*)&out->data;

  data->running    = true;
  data->time_on_ms = data->on = dur_on_ms / BLINK_TIMER_PERIOD_MS;
  data->time_off_ms = data->off = dur_off_ms / BLINK_TIMER_PERIOD_MS;
  data->cycles = cycles;
  gpio_pin_set_dt( &out->config.dt, true );

  if ( k_timer_remaining_get( &seq_timer ) == 0 )
  {
    k_timer_start( &seq_timer, K_MSEC( BLINK_TIMER_PERIOD_MS ), K_MSEC( BLINK_TIMER_PERIOD_MS ) );
  }
}

static void _set_out_seq( const struct mu_output_def* out, const uint32_t dur_on_ms, const uint32_t dur_off_ms )
{
    _set_out_seq_cycles( out, dur_on_ms, dur_off_ms, -1 );
}

static void _eval_stop_seq_timer()
{
    bool                    running = false;
    struct mu_output_def* o       = _out;

    // If no one is running a sequence, then stop the timer
    if ( k_timer_remaining_get( &seq_timer ) != 0 )
    {
        while ( IO_EXISTS( o ) )
        {
            if ( o->data.running )
            {
                running = true;
                break;
            }
            o++;
        }

        if ( ! running )
        {
            k_timer_stop( &seq_timer );
        }
    }
}

static void _init_states()
{
    struct mu_input_def* i = _in;
    struct mu_output_def* o = _out;

    while ( IO_EXISTS( i ) )
    {
        i->data.state = i->data.old_state = gpio_pin_get_dt( &i->config.dt );
        LOG_DBG( "Init input: '%s' -> %s", i->config.name, ( i->data.state > 0 ) ? "Active" : "Inactive" );
        i++;
    }

    while ( IO_EXISTS( o ) )
    {
        LOG_DBG( "Init out: '%s' -> %s", o->config.name, ( gpio_pin_get_dt( &o->config.dt ) > 0 ) ? "Active" : "Inactive" );
        o++;
    }
}

/**
 * Initializes all GPIOs passed to this function and applies definitions found in device-tree
 * @param in Inputs
 * @param out Outputs
 * @return Return 0 on success, -EIO if something went wrong.
 */
static int gpio_init( struct mu_input_def* in, struct mu_output_def* out )
{
    bool trig_filter_tm = false;
    _in                 = in;
    _out                = out;
    int ret             = 0;

    struct mu_input_def*  i = in;
    struct mu_output_def* o = out;

    while ( IO_EXISTS( i ) )
    {
        if ( ! device_is_ready( i->config.dt.port ) )
        {
            LOG_ERR( "Error: gpio device %s is not ready (input: '%s')", i->config.dt.port->name, i->config.name );
            return -EIO;
        }

        if ( ( ( ret = gpio_pin_configure_dt( &i->config.dt, GPIO_INPUT ) ) != 0 ) )
        {
            LOG_ERR( "Error %d: failed to configure %s pin %d (input: '%s')", ret, i->config.dt.port->name,
                     i->config.dt.pin, i->config.name );
            return -EIO;
        }

        // Register callback if one was provided
        if ( i->config.handler != NULL )
        {
            if ( ( ret = gpio_pin_interrupt_configure_dt( &i->config.dt, i->config.flags ) ) != 0 )
            {
                LOG_ERR( "Error %d: failed to configure interrupt on %s pin %d (input: '%s')", ret,
                         i->config.dt.port->name, i->config.dt.pin, i->config.name );
                return -EIO;
            }

            gpio_init_callback( &i->data.cb_data, i->config.handler, BIT( i->config.dt.pin ) );
            gpio_add_callback( i->config.dt.port, &i->data.cb_data );
        }

        // Verify if input needs to be filtered
        if ( i->config.filter_num_stable_val != 0 )
        {
            trig_filter_tm = true;
        }

        LOG_DBG( "Input cfg: '%s' %s", i->config.name, (i->config.handler != NULL) ? "(with handler)" : "" );
        i++;
    }

    while ( IO_EXISTS( o ) )
    {
        if ( ! device_is_ready( o->config.dt.port ) )
        {
            LOG_ERR( "Error: gpio device %s is not ready (output: '%s')", o->config.dt.port->name, o->config.name );
            return -EIO;
        }

        if ( ( ret = gpio_pin_configure_dt( &o->config.dt, GPIO_OUTPUT_INACTIVE ) ) != 0 )
        {
            LOG_ERR( "Error %d: failed to configure %s pin %d (output: '%s')", ret, o->config.dt.port->name,
                     o->config.dt.pin, o->config.name );
            return -EIO;
        }

        LOG_DBG( "Output cfg: '%s'", o->config.name );
        o++;
    }

    _init_states();

    if ( trig_filter_tm )
    {
        k_timer_start( &filter_timer, K_MSEC( FILTER_TIMER_PERIOD_MSEC ), K_MSEC( FILTER_TIMER_PERIOD_MSEC ) );
    }

    return 0;
}

/**
 * Gets max id value for defined GPIOS
 * @return Max id
 */
static int gpio_get_max_id()
{
    const struct mu_input_def*  i = _in;
    const struct mu_output_def* o = _out;
    int max = 0;

    while ( IO_EXISTS( i ) )
    {
        max++;
        i++;
    }

    while ( IO_EXISTS( o ) )
    {
        max++;
        o++;
    }

    return max;
}

/**
 * Gets input name given @p id
 * @param id input
 * @return A point to the string, NULL if not found
 */
static const char* gpio_get_in_name( int id )
{
    const struct mu_input_def* in = _get_in_by_id( id, _in );

    if ( in )
    {
        return in->config.name;
    }
    return NULL;
}

/**
 * Gets output name given @p id
 * @param id output
 * @return A point to the string, NULL if not found
 */
static const char* gpio_get_out_name( int id )
{
    const struct mu_output_def* out = _get_out_by_id( id, _out );

    if ( out )
    {
        return out->config.name;
    }
    return NULL;
}

/**
 * Verifies if @p id is an input
 * @param id Input id
 * @return True if @p id is an input
 */
static bool gpio_is_in_by_id( int id )
{
    const struct mu_input_def* in = _get_in_by_id( id, _in );
    return ( in != NULL ) ? true : false;
}

/**
 * Verifies if @p name is an input
 * @param name Input name
 * @return True if @p name is an input
 */
static bool gpio_is_in_by_name( const char* name )
{
    const struct mu_input_def* in = _get_in_by_name( name, _in );
    return ( in != NULL ) ? true : false;
}

/**
 * Verifies if @p id is an output
 * @param id Input id
 * @return True if @p id is an input
 */
static bool gpio_is_out_by_id( int id )
{
    const struct mu_output_def* out = _get_out_by_id( id, _out );
    return ( out != NULL ) ? true : false;
}

/**
 * Verifies if @p name is an output
 * @param name Input name
 * @return True if @p name is an input
 */
static bool gpio_is_out_by_name( const char* name )
{
    const struct mu_output_def* out = _get_out_by_name( name, _out );
    return ( out != NULL ) ? true : false;
}

/**
 * Sets an output
 * @param id Output id according to enum defined in struct mu_output_def
 * @param active Desired logical state of output
 * @return Returns 0 if output was found and state has been applied
 */
static int gpio_set_out_state_by_id( int id, const int active )
{
    const struct mu_output_def* out = _get_out_by_id( id, _out );

    if ( out )
    {
        _set_out_state( out, active );
        if ( active == 0 )
        {
            _eval_stop_seq_timer();
        }
        return 0;
    }
    return _not_found_id( id );
}

/**
 * Sets an output
 * @param name Output name according to enum defined in struct mu_output_def
 * @param active Desired logical state of output
 * @return Returns 0 if output was found and state has been applied
 */
static int gpio_set_out_state_by_name( const char* name, const int active )
{
    const struct mu_output_def* out = _get_out_by_name( name, _out );

    if ( out )
    {
        _set_out_state( out, active );
        if ( active == 0 )
        {
            _eval_stop_seq_timer();
        }
        return 0;
    }
    return _not_found_name( name );
}

/**
 * Starts sequence for an output
 * @param id Output id according to enum defined in struct mu_led_def
 * @param dur_on_ms On duration defined in milli-seconds
 * @param dur_off_ms Off duration defined in milli-seconds
 * @return Returns 0 if output was found and state has been applied
 */
static int gpio_set_out_seq_by_id( int id, uint32_t dur_on_ms, uint32_t dur_off_ms )
{
    const struct mu_output_def* out = _get_out_by_id( id, _out );

    if ( out )
    {
        _set_out_seq( out, dur_on_ms, dur_off_ms );
        return 0;
    }
    return _not_found_id( id );
}

/**
 * Starts sequence for an output
 * @param name Output name according to enum defined in struct mu_led_def
 * @param dur_on_ms On duration defined in milli-seconds
 * @param dur_off_ms Off duration defined in milli-seconds
 * @return Returns 0 if output was found and state has been applied
 */
static int gpio_set_out_seq_by_name( const char* name, uint32_t dur_on_ms, uint32_t dur_off_ms )
{
    const struct mu_output_def* out = _get_out_by_name( name, _out );

    if ( out )
    {
        _set_out_seq( out, dur_on_ms, dur_off_ms );
        return 0;
    }
    return _not_found_name( name );
}

/**
 * Starts a cycle for an output
 * @param id Output id according to enum defined in struct mu_led_def
 * @param dur_on_ms On duration defined in milli-seconds
 * @param dur_off_ms Off duration defined in milli-seconds
 * @param cycles Number of cycles / repetitions
 * @return Returns 0 if output was found and state has been applied
 */
static int gpio_set_out_cycle_by_id( int id, uint32_t dur_on_ms, uint32_t dur_off_ms, int cycles )
{
    const struct mu_output_def* out = _get_out_by_id( id, _out );

    if ( out )
    {
        _set_out_seq_cycles( out, dur_on_ms, dur_off_ms, cycles );
        return 0;
    }
    return _not_found_id( id );
}

/**
 * Gets state of output by id
 * @param id Input id according to enum defined in struct mu_gpio_def
 * @param value Pointer where to store value of input
 * @return Returns 0 if successful
 */
static int gpio_get_out_state_by_id( int id, int* value )
{
    int                           val = 0;
    const struct mu_output_def* out = _get_out_by_id( id, _out );

    if ( out )
    {
        if ( _get_out_state( out, &val ) >= 0 )
        {
            *value = val;
            return 0;
        }
    }

    return _not_found_id( id );
}

/**
 * Gets state of output by name
 * @param name Input name according to enum defined in struct mu_gpio_def
 * @param value Pointer where to store actual value of input
 * @return Returns 0 if successful
 */
static int gpio_get_out_state_by_name( const char* name, int* value )
{
    int                           val = 0;
    const struct mu_output_def* out = _get_out_by_name( name, _out );

    if ( out )
    {
        if ( _get_out_state( out, &val ) >= 0 )
        {
            *value = val;
            return 0;
        }
    }

    return _not_found_name( name );
}

/**
 * Gets data_store of input
 * @param id Input id according to enum defined in struct mu_gpio_def
 * @param value Pointer where to store value of input
 * @return Returns 0 if successful
 */
static int gpio_get_in_state_by_id( int id, int* value )
{
    int                          val = 0;
    const struct mu_input_def* in  = _get_in_by_id( id, _in );

    if ( in )
    {
        if ( _get_in_state( in, &val ) >= 0 )
        {
            *value = val;
            return 0;
        }
    }

    return _not_found_id( id );
}

/**
 * Gets data_store of input
 * @param name Input name according to enum defined in struct mu_gpio_def
 * @param value Pointer where to store actual value of input
 * @return Returns 0 if successful
 */
static int gpio_get_in_state_by_name( const char* name, int* value )
{
    int                          val = 0;
    const struct mu_input_def* in  = _get_in_by_name( name, _in );

    if ( in )
    {
        if ( _get_in_state( in, &val ) >= 0 )
        {
            *value = val;
            return 0;
        }
    }
    return _not_found_name( name );
}

/**
 * Checks if a given input has changed state
 * @param id Input id
 * @param changed Address where to store changed flag
 * @param value Address where to store current value
 * @return Returns 0 on success, a negative number if an error occurred
 */
static int gpio_changed_in_state_by_id( int id, bool* changed, int* value )
{
    int                          ret, val = 0;
    const struct mu_input_def* in = _get_in_by_id( id, _in );

    if ( in )
    {
        if ( ( ret = _get_in_state( in, &val ) ) >= 0 )
        {
            *changed = ( ret == 1 ) ? true : false;
            *value   = val;
            return 0;
        }
        return ret;
    }
    return _not_found_id( id );
}

/**
 * Checks if a given input has changed state
 * @param name Input name
 * @param changed Address where to store changed flag
 * @param value Address where to store current value
 * @return Returns 0 true if is has changed, otherwise it returns false
 */
static int gpio_changed_in_state_by_name( const char* name, bool* changed, int* value )
{
    int                          ret, val = 0;
    const struct mu_input_def* in = _get_in_by_name( name, _in );

    if ( in )
    {
        if ( ( ret = _get_in_state( in, &val ) ) >= 0 )
        {
            *changed = ( ret == 1 ) ? true : false;
            *value   = val;
            return 0;
        }
        return ret;
    }
    return _not_found_name( name );
}

struct mu_gpio_if muGpio =
{
    .init                 = gpio_init,
    .getMaxId             = gpio_get_max_id,
    .getInName            = gpio_get_in_name,
    .getOutName           = gpio_get_out_name,
    .isInById             = gpio_is_in_by_id,
    .isOutById            = gpio_is_out_by_id,
    .isInByName           = gpio_is_in_by_name,
    .isOutByName          = gpio_is_out_by_name,
    .setOutStateById      = gpio_set_out_state_by_id,
    .setOutStateByName    = gpio_set_out_state_by_name,
    .setOutSeqById        = gpio_set_out_seq_by_id,
    .setOutSeqByName      = gpio_set_out_seq_by_name,
    .setOutCycleById      = gpio_set_out_cycle_by_id,
    .getOutStateById      = gpio_get_out_state_by_id,
    .getOutStateByName    = gpio_get_out_state_by_name,
    .getInStateById       = gpio_get_in_state_by_id,
    .getInStateByName     = gpio_get_in_state_by_name,
    .changedInStateById   = gpio_changed_in_state_by_id,
    .changedInStateByName = gpio_changed_in_state_by_name
};
