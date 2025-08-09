
#ifndef MU_GPIO_MOCK_H
#define MU_GPIO_MOCK_H

#include <gmock/gmock.h>
#include <stdint.h>
#include "gpio.h"

class MuGpioInterface
{
public:
    virtual ~MuGpioInterface() {}
    virtual int init(struct mu_input_def* in, struct mu_output_def* out) = 0;
    virtual int getMaxId() = 0;
    virtual const char* getInName( int id ) = 0;
    virtual const char* getOutName( int id ) = 0;
    virtual bool isInById( int id ) = 0;
    virtual bool isOutById( int id ) = 0;
    virtual bool isInByName( const char* name ) = 0;
    virtual bool isOutByName( const char* name ) = 0;
    virtual int setOutStateById( int id, const int state ) = 0;
    virtual int setOutStateByName( const char* name, const int state ) = 0;
    virtual int setOutSeqById( int id, uint32_t dur_on_ms, uint32_t dur_off_ms ) = 0;
    virtual int setOutSeqByName( const char* name, uint32_t dur_on_ms, uint32_t dur_off_ms ) = 0;
    virtual int setOutCycleById( int id, uint32_t dur_on_ms, uint32_t dur_off_ms, int cycles ) = 0;
    virtual int getOutStateById( int id, int* value ) = 0;
    virtual int getOutStateByName( const char* name, int* value ) = 0;
    virtual int getInStateById( int id, int* value ) = 0;
    virtual int getInStateByName( const char* name, int* value ) = 0;
    virtual int changedInStateById( int id, bool* changed, int* value ) = 0;
    virtual int changedInStateByName( const char* name, bool* changed, int* value ) = 0;
};

class MuGpioMock : public MuGpioInterface
{
public:
    virtual ~MuGpioMock() {}

    MOCK_METHOD( int, init, ( struct mu_input_def* in, struct mu_output_def* out ));
    MOCK_METHOD( int, getMaxId, ());
    MOCK_METHOD( const char*, getInName, ( int id ));
    MOCK_METHOD( const char*, getOutName, ( int id ));
    MOCK_METHOD( bool, isInById, ( int id ));
    MOCK_METHOD( bool, isOutById, ( int id ));
    MOCK_METHOD( bool, isInByName, ( const char* name ));
    MOCK_METHOD( bool, isOutByName, ( const char* name ));
    MOCK_METHOD( int, setOutStateById, ( int id, const int state ));
    MOCK_METHOD( int, setOutStateByName, ( const char* name, const int state ));
    MOCK_METHOD( int, setOutSeqById, ( int id, uint32_t dur_on_ms, uint32_t dur_off_ms ));
    MOCK_METHOD( int, setOutSeqByName, ( const char* name, uint32_t dur_on_ms, uint32_t dur_off_ms ));
    MOCK_METHOD( int, setOutCycleById, ( int id, uint32_t dur_on_ms, uint32_t dur_off_ms, int cycles ));
    MOCK_METHOD( int, getOutStateById, ( int id, int* value ));
    MOCK_METHOD( int, getOutStateByName, ( const char* name, int* value ));
    MOCK_METHOD( int, getInStateById, ( int id, int* value ));
    MOCK_METHOD( int, getInStateByName, ( const char* name, int* value ));
    MOCK_METHOD( int, changedInStateById, ( int id, bool* changed, int* value ));
    MOCK_METHOD( int, changedInStateByName, ( const char* name, bool* changed, int* value ));
};

extern const struct mu_gpio_if muGpioMock;
extern MuGpioMock* MuGpioMockObj;

#endif //MU_GPIO_MOCK_H
