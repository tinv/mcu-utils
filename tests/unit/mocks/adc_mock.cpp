#include "adc_mock.h"

MuAdcMock *MuAdcMockObj = nullptr;

int init(const mu_adc_in_config_t *config, size_t configSize)
{
	return MuAdcMockObj->init(config, configSize);
}
int measure(const int channel, uint32_t *value)
{
	return MuAdcMockObj->measure(channel, value);
}

const struct mu_adc_if muAdcMock = {

	.init = init, .measure = measure

};
