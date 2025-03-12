// Copyright 2022 TecInvent Electronics Ltd

#include "storage_mock.h"

MuStorageMock* MuStorageMockObj = nullptr;

static int init(void)
{
	return MuStorageMockObj->init();
}

static int mount(const mu_storage_config_t *config)
{
	return MuStorageMockObj->mount(config);
}

static int umount(const mu_storage_config_t *config)
{
	return MuStorageMockObj->umount(config);
}

static int file_write(const char *fname, const uint8_t *buf, const size_t size)
{
	return MuStorageMockObj->file_write(fname, buf, size);
}

static int file_read(const char *fname, uint8_t *buf, const size_t size)
{
	return MuStorageMockObj->file_read(fname, buf, size);
}

static int file_exists(const char *fname)
{
	return MuStorageMockObj->file_exists(fname);
}

const struct mu_storage_if muStorageMock =
{
	.init = init,
	.mount = mount,
	.umount = umount,
	.file_write = file_write,
	.file_read = file_read,
	.file_exists = file_exists
};