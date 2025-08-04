#include "hwid_mock.h"

MuHwIdMock* MuHwIdMockObj = nullptr;

int init()
{
	return MuHwIdMockObj->init();
}
int code(const enum mu_hwid_type type, int *code)
{
	return MuHwIdMockObj->code(type, code);
}

const struct mu_hwid_if muHwIdMock = {
	.init = init,
	.code = code,
};
