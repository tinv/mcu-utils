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

static int file_remove(const char *fname)
{
	return MuStorageMockObj->file_remove(fname);
}

static int file_exists(const char *fname)
{
	return MuStorageMockObj->file_exists(fname);
}

static size_t file_size(const char *fname)
{
	return MuStorageMockObj->file_size(fname);
}

static int directory_exists(const char *path)
{
	return MuStorageMockObj->directory_exists(path);
}

static int directory_create(const char *path)
{

	return MuStorageMockObj->directory_create(path);
}

const struct mu_storage_if muStorageMock =
{
	.init = init,
	.mount = mount,
	.umount = umount,
	.file_write = file_write,
	.file_read = file_read,
	.file_remove = file_remove,
	.file_exists = file_exists,
	.file_size = file_size,
	.directory_exists = directory_exists,
	.directory_create = directory_create,
};
