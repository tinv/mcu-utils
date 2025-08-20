// Copyright 2022 TecInvent Electronics Ltd

#ifndef MU_STORAGE_MOCK_H_
#define MU_STORAGE_MOCK_H_

#include <gmock/gmock.h>
#include "storage/storage.h"

class MuStorageInterface {
public:
    virtual ~MuStorageInterface() {}
    virtual int init() = 0;
    virtual int mount(const mu_storage_config_t *config) = 0;
    virtual int umount(const mu_storage_config_t *config) = 0;
    virtual int file_write(const char *fname, const uint8_t *buf, const size_t size) = 0;
    virtual int file_read(const char *fname, uint8_t *buf, const size_t size) = 0;
    virtual int file_remove(const char *fname) = 0;
    virtual int file_exists(const char *fname) = 0;

};

class MuStorageMock : public MuStorageInterface {
public:
	virtual ~MuStorageMock() {};
	MOCK_METHOD(int, init, (), (override));
	MOCK_METHOD(int, mount, (const mu_storage_config_t *config), (override));
	MOCK_METHOD(int, umount, (const mu_storage_config_t *config), (override));
	MOCK_METHOD(int, file_write, (const char *fname, const uint8_t *buf, const size_t size),
		    (override));
	MOCK_METHOD(int, file_read, (const char *fname, uint8_t *buf, const size_t size),
		    (override));
	MOCK_METHOD(int, file_remove, (const char *fname), (override));
	MOCK_METHOD(int, file_exists, (const char *fname), (override));
};

extern const struct mu_storage_if muStorageMock;
extern MuStorageMock* MuStorageMockObj;

#endif //MU_STORAGE_MOCK_H_
