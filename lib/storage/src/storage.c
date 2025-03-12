/*
 * Copyright (c) 2025 TecInvent Electronics Ltd
 */

#include "storage/storage.h"
#include <errno.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/sys/util.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mu_storage);

#ifdef CONFIG_FILE_SYSTEM_LITTLEFS
#include <zephyr/fs/littlefs.h>
#endif


#include <autoconf.h>

#ifdef CONFIG_FILE_SYSTEM_LITTLEFS
#define PARTITION_NODE DT_NODELABEL(lfs1)
BUILD_ASSERT(DT_NODE_EXISTS(PARTITION_NODE), "No storage specified in DT");

struct fs_littlefs lfsfs;
FS_FSTAB_DECLARE_ENTRY(PARTITION_NODE);

static struct fs_mount_t mp_lfs_storage = {
    .type = FS_LITTLEFS,
    .fs_data = &lfsfs,
    .flags = FS_MOUNT_FLAG_USE_DISK_ACCESS,
};
struct fs_mount_t *mp_lfs = &FS_FSTAB_ENTRY(PARTITION_NODE);
#endif


static void _print_volume_info(struct fs_mount_t* mp)
{
	struct fs_statvfs sbuf;
	int rc = fs_statvfs( mp->mnt_point, &sbuf );
	if ( rc < 0 )
	{
		LOG_ERR( "FAIL: statvfs: %d", rc );
		return;
	}

	LOG_INF( "%s: bsize = %lu ; frsize = %lu ; blocks = %lu ; bfree = %lu",
		mp->mnt_point, sbuf.f_bsize, sbuf.f_frsize, sbuf.f_blocks, sbuf.f_bfree );
}



#ifdef CONFIG_FILE_SYSTEM_LITTLEFS
static int _littlefs_flash_erase( unsigned int id )
{
	const struct flash_area* pfa;
	int                      rc;

	rc = flash_area_open( id, &pfa );
	if ( rc < 0 )
	{
		LOG_ERR( "unable to find flash area %u: %d", id, rc );
		return rc;
	}

	LOG_PRINTK( "Area %u at 0x%x on %s for %u bytes\n", id, (unsigned int)pfa->fa_off, pfa->fa_dev->name,
		   (unsigned int)pfa->fa_size );

	/* Optional wipe flash contents */
	if ( IS_ENABLED( CONFIG_APP_WIPE_STORAGE ) )
	{
		rc = flash_area_erase( pfa, 0, pfa->fa_size );
		LOG_ERR( "erasing flash area ... %d", rc );
	}

	flash_area_close( pfa );
	return rc;
}
#endif

static int _mount(struct fs_mount_t* mp)
{
	int ret;

#ifdef CONFIG_FILE_SYSTEM_LITTLEFS
	ret = _littlefs_flash_erase( (uintptr_t)mp->storage_dev );
	if ( ret < 0 )
	{
		return ret;
	}
#endif

	if ( ( ret = fs_mount( mp ) ) != 0 )
	{
		LOG_ERR( "mounting error %d: %s", ret,  mp->mnt_point );
		return ret;
	}

	LOG_DBG( "mounting disk ok" );
	return 0;

}

static int _unmount(struct fs_mount_t* mp)
{
	int ret;

	if ( ( ret = fs_unmount( mp ) ) != 0 )
	{
		LOG_ERR( "unmounting error: %d", ret );
		return ret;
	}

	LOG_DBG( "unmounting disk ok" );
	return 0;
}

static int muStorage_init(void) {
	return 0;
}

static int muStorage_mount(const mu_storage_config_t *config)
{
	__ASSERT_NO_MSG(config);
	int ret = 0;
	struct fs_mount_t** mp = NULL;

	switch (config->fs_type) {
	case FS_FATFS:
		return -ENOTSUP;

	case FS_LITTLEFS:
#ifdef CONFIG_FILE_SYSTEM_LITTLEFS
		mp = &mp_lfs;
		break;
#else
		LOG_ERR( "LITTLEFS is not supported" );
		return -ENOTSUP;
#endif

	default:
		return -ENOTSUP;

	}

	if (mp) {

		(*mp)->mnt_point = config->mount_point;
		ret = _mount(*mp);
		_print_volume_info(*mp);
	}


	LOG_DBG( "mount ok: %s", config->mount_point );
	return ret;

}

static int muStorage_umount(const mu_storage_config_t *config)
{

	switch (config->fs_type) {

	case FS_LITTLEFS:
#ifdef CONFIG_FILE_SYSTEM_LITTLEFS
		return _unmount( mp_lfs );
#else
		LOG_ERR( "LITTLEFS is not supported" );
		return -ENOTSUP;
#endif

	default:

		return -ENOTSUP;

	}

	LOG_DBG( "unmount ok: %s", config->mount_point );
	return 0;
}

static int muStorage_file_write(const char *fname, const uint8_t *buf,
                                const size_t size)
{
	struct fs_file_t file;
	int ret, rc, wb;

	fs_file_t_init(&file);
	rc = fs_open(&file, fname, FS_O_CREATE | FS_O_WRITE);
	if (rc < 0) {
		LOG_ERR("open %s failed: %d", fname, rc);
		return rc;
	}

	wb = fs_write(&file, buf, size);
	if (wb < 0) {
		LOG_ERR("write %s failed: %d", fname, rc);
	}

	ret = fs_sync(&file);
	if (ret < 0) {
		LOG_ERR("sync %s: %d", fname, ret);
	}

	ret = fs_close(&file);
	if (ret < 0) {
		LOG_ERR("close %s: %d", fname, ret);
		return ret;
	}

	// Written bytes
	if (wb) {
		LOG_INF("written %dB to %s", wb, fname);
		return wb;
	}

	return ret;
}

static int muStorage_file_read(const char *fname, uint8_t *buf,
                               const size_t size)
{
	struct fs_file_t file;
	int ret, rc, rb;

	fs_file_t_init(&file);
	rc = fs_open(&file, fname, FS_O_CREATE | FS_O_READ);
	if (rc < 0) {
		LOG_ERR("open %s failed: %d", fname, rc);
		return rc;
	}

	rb = fs_read(&file, buf, size);
	if (rb < 0) {
		LOG_ERR("read %s failed: %d", fname, rc);
	}

	ret = fs_close(&file);
	if (ret < 0) {
		LOG_ERR("close %s: %d", fname, ret);
		return ret;
	}

	/* Read bytes */
	if (rb) {
		return rb;
	}

	return ret;
}

static int muStorage_file_exists(const char *fname)
{
	struct fs_dirent entry;
	return fs_stat(fname, &entry);
}

const struct mu_storage_if muStorage = {
	.init = muStorage_init,
	.mount = muStorage_mount,
	.umount = muStorage_umount,
	.file_write = muStorage_file_write,
	.file_read = muStorage_file_read,
	.file_exists = muStorage_file_exists
};
