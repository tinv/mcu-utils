#ifndef FS_MOCK_H
#define FS_MOCK_H

#include <gmock/gmock.h>
#include "fs/fs.h"

class MuFsInterface
{
public:
	virtual ~MuFsInterface() {};
	virtual int open(struct fs_file_t *zfp, const char *file_name, fs_mode_t flags) = 0;
	virtual int close(struct fs_file_t *zfp) = 0;
	virtual int unlink(const char *path) = 0;
	virtual int rename(const char *from, const char *to) = 0;
	virtual ssize_t read(struct fs_file_t *zfp, void *ptr, size_t size) = 0;
	virtual ssize_t write(struct fs_file_t *zfp, const void *ptr, size_t size) = 0;
	virtual int seek(struct fs_file_t *zfp, off_t offset, int whence) = 0;
	virtual off_t tell(struct fs_file_t *zfp) = 0;
	virtual int truncate(struct fs_file_t *zfp, off_t length) = 0;
	virtual int sync(struct fs_file_t *zfp) = 0;
	virtual int mkdir(const char *path) = 0;
	virtual int opendir(struct fs_dir_t *zdp, const char *path) = 0;
	virtual int readdir(struct fs_dir_t *zdp, struct fs_dirent *entry) = 0;
	virtual int closedir(struct fs_dir_t *zdp) = 0;
	virtual int mount(struct fs_mount_t *mp) = 0;
	virtual int unmount(struct fs_mount_t *mp) = 0;
	virtual int readmount(int *index, const char **name) = 0;
	virtual int stat(const char *path, struct fs_dirent *entry) = 0;
	virtual int statvfs(const char *path, struct fs_statvfs *stat) = 0;
	virtual int mkfs(int fs_type, uintptr_t dev_id, void *cfg, int flags) = 0;
	virtual int registerFs(int type, const struct fs_file_system_t *fs) = 0;
	virtual int unregisterFs(int type, const struct fs_file_system_t *fs) = 0;
	virtual int gc(struct fs_mount_t *mp) = 0;
};

class MuFsMock : public MuFsInterface
{
public:
	virtual ~MuFsMock() {};
	MOCK_METHOD(int, open, (struct fs_file_t *zfp, const char *file_name, fs_mode_t flags), (override));
	MOCK_METHOD(int, close, (struct fs_file_t *zfp), (override));
	MOCK_METHOD(int, unlink, (const char *path), (override));
	MOCK_METHOD(int, rename, (const char *from, const char *to), (override));
	MOCK_METHOD(ssize_t, read, (struct fs_file_t *zfp, void *ptr, size_t size), (override));
	MOCK_METHOD(ssize_t, write, (struct fs_file_t *zfp, const void *ptr, size_t size), (override));
	MOCK_METHOD(int, seek, (struct fs_file_t *zfp, off_t offset, int whence), (override));
	MOCK_METHOD(off_t, tell, (struct fs_file_t *zfp), (override));
	MOCK_METHOD(int, truncate, (struct fs_file_t *zfp, off_t length), (override));
	MOCK_METHOD(int, sync, (struct fs_file_t *zfp), (override));
	MOCK_METHOD(int, mkdir, (const char *path), (override));
	MOCK_METHOD(int, opendir, (struct fs_dir_t *zdp, const char *path), (override));
	MOCK_METHOD(int, readdir, (struct fs_dir_t *zdp, struct fs_dirent *entry), (override));
	MOCK_METHOD(int, closedir, (struct fs_dir_t *zdp), (override));
	MOCK_METHOD(int, mount, (struct fs_mount_t *mp), (override));
	MOCK_METHOD(int, unmount, (struct fs_mount_t *mp), (override));
	MOCK_METHOD(int, readmount, (int *index, const char **name), (override));
	MOCK_METHOD(int, stat, (const char *path, struct fs_dirent *entry), (override));
	MOCK_METHOD(int, statvfs, (const char *path, struct fs_statvfs *stat), (override));
	MOCK_METHOD(int, mkfs, (int fs_type, uintptr_t dev_id, void *cfg, int flags), (override));
	MOCK_METHOD(int, registerFs, (int type, const struct fs_file_system_t *fs), (override));
	MOCK_METHOD(int, unregisterFs, (int type, const struct fs_file_system_t *fs), (override));
	MOCK_METHOD(int, gc, (struct fs_mount_t *mp), (override));

};

extern const struct mu_fs_if muFsMock;
extern MuFsMock* MuFsMockObj;

#endif // FS_MOCK_H