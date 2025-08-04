// Copyright 2022 TecInvent Electronics Ltd

#ifndef MU_ADC_MOCK_H_
#define MU_ADC_MOCK_H_

#include <gmock/gmock.h>
#include "mu_adc.h"

class MuAdcInterface
{
      public:
	virtual ~MuAdcInterface()
	{
	}
	virtual int init(const mu_adc_in_config_t *config, size_t configSize) = 0;
	virtual int measure(const int channel, uint32_t *value) = 0;
};

class MuAdcMock : public MuAdcInterface
{
      public:
	virtual ~MuAdcMock()
	{
	}
	MOCK_METHOD(int, init, (const mu_adc_in_config_t *config, size_t configSize));
	MOCK_METHOD(int, measure, (const int channel, uint32_t *value));
};

extern MuAdcMock *MuAdcMockObj;
extern const struct mu_adc_if muAdcMock;

#endif /* MU_ADC_MOCK_H_ */
