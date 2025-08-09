#include "gpio_mock.h"

MuGpioMock *MuGpioMockObj = nullptr;

int init(struct mu_input_def *in, struct mu_output_def *out)
{
	return MuGpioMockObj->init(in, out);
}
int getMaxId()
{
	return MuGpioMockObj->getMaxId();
}
const char *getInName(int id)
{
	return MuGpioMockObj->getInName(id);
}
const char *getOutName(int id)
{
	return MuGpioMockObj->getOutName(id);
}
bool isInById(int id)
{
	return MuGpioMockObj->isInById(id);
}
bool isOutById(int id)
{
	return MuGpioMockObj->isOutById(id);
}
bool isInByName(const char *name)
{
	return MuGpioMockObj->isInByName(name);
}
bool isOutByName(const char *name)
{
	return MuGpioMockObj->isOutByName(name);
}
int setOutStateById(int id, const int state)
{
	return MuGpioMockObj->setOutStateById(id, state);
}
int setOutStateByName(const char *name, const int state)
{
	return MuGpioMockObj->setOutStateByName(name, state);
}
int setOutSeqById(int id, uint32_t dur_on_ms, uint32_t dur_off_ms)
{
	return MuGpioMockObj->setOutSeqById(id, dur_on_ms, dur_off_ms);
}
int setOutSeqByName(const char *name, uint32_t dur_on_ms, uint32_t dur_off_ms)
{
	return MuGpioMockObj->setOutSeqByName(name, dur_on_ms, dur_off_ms);
}
int setOutCycleById(int id, uint32_t dur_on_ms, uint32_t dur_off_ms, int cycles)
{
	return MuGpioMockObj->setOutCycleById(id, dur_on_ms, dur_off_ms, cycles);
}
int getOutStateById(int id, int *value)
{
	return MuGpioMockObj->getOutStateById(id, value);
}
int getOutStateByName(const char *name, int *value)
{
	return MuGpioMockObj->getOutStateByName(name, value);
}
int getInStateById(int id, int *value)
{
	return MuGpioMockObj->getInStateById(id, value);
}
int getInStateByName(const char *name, int *value)
{
	return MuGpioMockObj->getInStateByName(name, value);
}
int changedInStateById(int id, bool *changed, int *value)
{
	return MuGpioMockObj->changedInStateById(id, changed, value);
}
int changedInStateByName(const char *name, bool *changed, int *value)
{
	return MuGpioMockObj->changedInStateByName(name, changed, value);
}

const struct mu_gpio_if muGpioMock = {.init = init,
				      .getMaxId = getMaxId,
				      .getInName = getInName,
				      .getOutName = getOutName,
				      .isInById = isInById,
				      .isOutById = isOutById,
				      .isInByName = isInByName,
				      .isOutByName = isOutByName,
				      .setOutStateById = setOutStateById,
				      .setOutStateByName = setOutStateByName,
				      .setOutSeqById = setOutSeqById,
				      .setOutSeqByName = setOutSeqByName,
				      .setOutCycleById = setOutCycleById,
				      .getOutStateById = getOutStateById,
				      .getOutStateByName = getOutStateByName,
				      .getInStateById = getInStateById,
				      .getInStateByName = getInStateByName,
				      .changedInStateById = changedInStateById,
				      .changedInStateByName = changedInStateByName};
