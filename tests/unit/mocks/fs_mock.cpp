#include "fs_mock.h"

MuFsMock* MuFsMockObj = nullptr;

static int muFsMock_open(struct fs_file_t *zfp, const char *file_name, fs_mode_t flags)
{
	return MuFsMockObj->open(zfp, file_name, flags);
}

static int muFsMock_close(struct fs_file_t *zfp)
{
	return MuFsMockObj->close(zfp);
}

static int muFsMock_unlink(const char *path)
{
	return MuFsMockObj->unlink(path);
}

static int muFsMock_rename(const char *from, const char *to)
{
	return MuFsMockObj->rename(from, to);
}

static ssize_t muFsMock_read(struct fs_file_t *zfp, void *ptr, size_t size)
{
	return MuFsMockObj->read(zfp, ptr, size);
}

static ssize_t muFsMock_write(struct fs_file_t *zfp, const void *ptr, size_t size)
{
	return MuFsMockObj->write(zfp, ptr, size);
}

static int muFsMock_seek(struct fs_file_t *zfp, off_t offset, int whence)
{
	return MuFsMockObj->seek(zfp, offset, whence);
}

static off_t muFsMock_tell(struct fs_file_t *zfp)
{
	return MuFsMockObj->tell(zfp);
}

static int muFsMock_truncate(struct fs_file_t *zfp, off_t length)
{
	return MuFsMockObj->truncate(zfp, length);
}

static int muFsMock_sync(struct fs_file_t *zfp)
{
	return MuFsMockObj->sync(zfp);
}

static int muFsMock_mkdir(const char *path)
{
	return MuFsMockObj->mkdir(path);
}

static int muFsMock_opendir(struct fs_dir_t *zdp, const char *path)
{
	return MuFsMockObj->opendir(zdp, path);
}

static int muFsMock_readdir(struct fs_dir_t *zdp, struct fs_dirent *entry)
{
	return MuFsMockObj->readdir(zdp, entry);
}

static int muFsMock_closedir(struct fs_dir_t *zdp)
{
	return MuFsMockObj->closedir(zdp);
}

static int muFsMock_mount(struct fs_mount_t *mp)
{
	return MuFsMockObj->mount(mp);
}

static int muFsMock_unmount(struct fs_mount_t *mp)
{
	return MuFsMockObj->unmount(mp);
}

static int muFsMock_readmount(int *index, const char **name)
{
	return MuFsMockObj->readmount(index, name);
}

static int muFsMock_stat(const char *path, struct fs_dirent *entry)
{
	return MuFsMockObj->stat(path, entry);
}

static int muFsMock_statvfs(const char *path, struct fs_statvfs *stat)
{
	return MuFsMockObj->statvfs(path, stat);
}

static int muFsMock_mkfs(int fs_type, uintptr_t dev_id, void *cfg, int flags)
{
	return MuFsMockObj->mkfs(fs_type, dev_id, cfg, flags);
}

static int muFsMock_registerFs(int type, const struct fs_file_system_t *fs)
{
	return MuFsMockObj->registerFs(type, fs);
}

static int muFsMock_unregisterFs(int type, const struct fs_file_system_t *fs)
{
	return MuFsMockObj->unregisterFs(type, fs);
}

static int muFsMock_gc(struct fs_mount_t *mp)
{
	return MuFsMockObj->gc(mp);
}

const struct mu_fs_if muFsMock = {
	.open = muFsMock_open,
	.close = muFsMock_close,
	.unlink = muFsMock_unlink,
	.rename = muFsMock_rename,
	.read = muFsMock_read,
	.write = muFsMock_write,
	.seek = muFsMock_seek,
	.tell = muFsMock_tell,
	.truncate = muFsMock_truncate,
	.sync = muFsMock_sync,
	.mkdir = muFsMock_mkdir,
	.opendir = muFsMock_opendir,
	.readdir = muFsMock_readdir,
	.closedir = muFsMock_closedir,
	.mount = muFsMock_mount,
	.unmount = muFsMock_unmount,
	.readmount = muFsMock_readmount,
	.stat = muFsMock_stat,
	.statvfs = muFsMock_statvfs,
	.mkfs = muFsMock_mkfs,
	.registerFs = muFsMock_registerFs,
	.unregisterFs = muFsMock_unregisterFs,
	.gc = muFsMock_gc,
};

