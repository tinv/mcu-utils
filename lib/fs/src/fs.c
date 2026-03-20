/*
 * Copyright (c) 2026 TecInvent Electronics Ltd
 */

#include "fs/fs.h"
#include <zephyr/fs/storage.h>
#include <zephyr/kernel.h>
#include <errno.h>
#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mu_fs);

K_MUTEX_DEFINE(fs_mutex);

static int muFs_open(struct fs_file_t *zfp, const char *file_name, fs_mode_t flags)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_open(zfp, file_name, flags);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_close(struct fs_file_t *zfp)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_close(zfp);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_unlink(const char *path)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_unlink(path);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_rename(const char *from, const char *to)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_rename(from, to);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static ssize_t muFs_read(struct fs_file_t *zfp, void *ptr, size_t size)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_read(zfp, ptr, size);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static ssize_t muFs_write(struct fs_file_t *zfp, const void *ptr, size_t size)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_write(zfp, ptr, size);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_seek(struct fs_file_t *zfp, off_t offset, int whence)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_seek(zfp, offset, whence);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static off_t muFs_tell(struct fs_file_t *zfp)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_tell(zfp);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_truncate(struct fs_file_t *zfp, off_t length)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_truncate(zfp, length);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_sync(struct fs_file_t *zfp)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_sync(zfp);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_mkdir(const char *path)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_mkdir(path);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_opendir(struct fs_dir_t *zdp, const char *path)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_opendir(zdp, path);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_readdir(struct fs_dir_t *zdp, struct fs_dirent *entry)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_readdir(zdp, entry);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_closedir(struct fs_dir_t *zdp)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_closedir(zdp);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_mount(struct fs_mount_t *mp)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_mount(mp);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_unmount(struct fs_mount_t *mp)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_unmount(mp);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_readmount(int *index, const char **name)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_readmount(index, name);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_stat(const char *path, struct fs_dirent *entry)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_stat(path, entry);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_statvfs(const char *path, struct fs_statvfs *stat)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_statvfs(path, stat);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_mkfs(int fs_type, uintptr_t dev_id, void *cfg, int flags)
{
#if defined(CONFIG_FILE_SYSTEM_MKFS)
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_mkfs(fs_type, dev_id, cfg, flags);
	k_mutex_unlock(&fs_mutex);
	return rc;
#else
	return -ENOTSUP;
#endif
}

static int muFs_registerFs(int type, const struct fs_file_system_t *fs)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_register(type, fs);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_unregisterFs(int type, const struct fs_file_system_t *fs)
{
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_unregister(type, fs);
	k_mutex_unlock(&fs_mutex);
	return rc;
}

static int muFs_gc(struct fs_mount_t *mp)
{
#if defined(CONFIG_FILE_SYSTEM_GC)
	k_mutex_lock(&fs_mutex, K_FOREVER);
	const int rc = fs_gc(mp);
	k_mutex_unlock(&fs_mutex);
	return rc;
#else
	return -ENOTSUP;
#endif
}


const struct mu_fs_if muFs = {
	.open = muFs_open,
	.close = muFs_close,
	.unlink = muFs_unlink,
	.rename = muFs_rename,
	.read = muFs_read,
	.write = muFs_write,
	.seek = muFs_seek,
	.tell = muFs_tell,
	.truncate = muFs_truncate,
	.sync = muFs_sync,
	.mkdir = muFs_mkdir,
	.opendir = muFs_opendir,
	.readdir = muFs_readdir,
	.closedir = muFs_closedir,
	.mount = muFs_mount,
	.unmount = muFs_unmount,
	.readmount = muFs_readmount,
	.stat = muFs_stat,
	.statvfs = muFs_statvfs,
	.mkfs = muFs_mkfs,
	.registerFs = muFs_registerFs,
	.unregisterFs = muFs_unregisterFs,
	.gc = muFs_gc,
};
