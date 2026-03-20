// Copyright 2026 TecInvent Electronics Ltd

#ifndef MU_FS_H_
#define MU_FS_H_

#include <zephyr/fs/fs.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct mu_fs_if
{

	/**
	 * @brief Open or create file
	 *
	 * Opens or possibly creates a file and associates a stream with it.
	 * Successfully opened file, when no longer in use, should be closed
	 * with fs_close().
	 *
	 * @details
	 * @p flags can be 0 or a binary combination of one or more of the following
	 * identifiers:
	 *   - @c FS_O_READ open for read
	 *   - @c FS_O_WRITE open for write
	 *   - @c FS_O_RDWR open for read/write (<tt>FS_O_READ | FS_O_WRITE</tt>)
	 *   - @c FS_O_CREATE create file if it does not exist
	 *   - @c FS_O_APPEND move to end of file before each write
	 *   - @c FS_O_TRUNC truncate the file
	 *
	 * @warning If @p flags are set to 0 the function will open file, if it exists
	 *          and is accessible, but you will have no read/write access to it.
	 *
	 * @param zfp Pointer to an @b initialized file object
	 * @param file_name The name of a file to open
	 * @param flags The mode flags
	 *
	 * @retval 0 on success;
	 * @retval -EBUSY when zfp is already used;
	 * @retval -EINVAL when a bad file name is given;
	 * @retval -EROFS when opening read-only file for write, or attempting to
	 *	   create a file on a system that has been mounted with the
	 *	   FS_MOUNT_FLAG_READ_ONLY flag;
	 * @retval -ENOENT when the file does not exist at the path;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval -EACCES when trying to truncate a file without opening it for write.
	 * @retval <0 an other negative errno code, depending on a file system back-end.
	 */
	int (*open)(struct fs_file_t *zfp, const char *file_name, fs_mode_t flags);

	/**
	 * @brief Close file
	 *
	 * Flushes the associated stream and closes the file.
	 *
	 * @param zfp Pointer to the file object
	 *
	 * @retval 0 on success;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 a negative errno code on error.
	 */
	int (*close)(struct fs_file_t *zfp);

	/**
	 * @brief Unlink file
	 *
	 * Deletes the specified file or directory
	 *
	 * @param path Path to the file or directory to delete
	 *
	 * @retval 0 on success;
	 * @retval -EINVAL when a bad file name is given;
	 * @retval -EROFS if file is read-only, or when file system has been mounted
	 *	   with the FS_MOUNT_FLAG_READ_ONLY flag;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 an other negative errno code on error.
	 */
	int (*unlink)(const char *path);

	/**
	 * @brief Rename file or directory
	 *
	 * Performs a rename and / or move of the specified source path to the
	 * specified destination.  The source path can refer to either a file or a
	 * directory.  All intermediate directories in the destination path must
	 * already exist.  If the source path refers to a file, the destination path
	 * must contain a full filename path, rather than just the new parent
	 * directory.  If an object already exists at the specified destination path,
	 * this function causes it to be unlinked prior to the rename (i.e., the
	 * destination gets clobbered).
	 * @note Current implementation does not allow moving files between mount
	 * points.
	 *
	 * @param from The source path
	 * @param to The destination path
	 *
	 * @retval 0 on success;
	 * @retval -EINVAL when a bad file name is given, or when rename would cause move
	 *	   between mount points;
	 * @retval -EROFS if file is read-only, or when file system has been mounted
	 *	   with the FS_MOUNT_FLAG_READ_ONLY flag;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 an other negative errno code on error.
	 */
	int (*rename)(const char *from, const char *to);

	/**
	 * @brief Read file
	 *
	 * Reads up to @p size bytes of data to @p ptr pointed buffer, returns number
	 * of bytes read.  A returned value may be lower than @p size if there were
	 * fewer bytes available than requested.
	 *
	 * @param zfp Pointer to the file object
	 * @param ptr Pointer to the data buffer
	 * @param size Number of bytes to be read
	 *
	 * @retval >=0 a number of bytes read, on success;
	 * @retval -EBADF when invoked on zfp that represents unopened/closed file;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 a negative errno code on error.
	 */
	ssize_t (*read)(struct fs_file_t *zfp, void *ptr, size_t size);

	/**
	 * @brief Write file
	 *
	 * Attempts to write @p size number of bytes to the specified file.
	 * If a negative value is returned from the function, the file pointer has not
	 * been advanced.
	 * If the function returns a non-negative number that is lower than @p size,
	 * the global @c errno variable should be checked for an error code,
	 * as the device may have no free space for data.
	 *
	 * @param zfp Pointer to the file object
	 * @param ptr Pointer to the data buffer
	 * @param size Number of bytes to be written
	 *
	 * @retval >=0 a number of bytes written, on success;
	 * @retval -EBADF when invoked on zfp that represents unopened/closed file;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 an other negative errno code on error.
	 */
	ssize_t (*write)(struct fs_file_t *zfp, const void *ptr, size_t size);

	/**
	 * @brief Seek file
	 *
	 * Moves the file position to a new location in the file. The @p offset is added
	 * to file position based on the @p whence parameter.
	 *
	 * @param zfp Pointer to the file object
	 * @param offset Relative location to move the file pointer to
	 * @param whence Relative location from where offset is to be calculated.
	 * - @c FS_SEEK_SET for the beginning of the file;
	 * - @c FS_SEEK_CUR for the current position;
	 * - @c FS_SEEK_END for the end of the file.
	 *
	 * @retval 0 on success;
	 * @retval -EBADF when invoked on zfp that represents unopened/closed file;
	 * @retval -ENOTSUP if not supported by underlying file system driver;
	 * @retval <0 an other negative errno code on error.
	 */
	int (*seek)(struct fs_file_t *zfp, off_t offset, int whence);

	/**
	 * @brief Get current file position.
	 *
	 * Retrieves and returns the current position in the file stream.
	 *
	 * @param zfp Pointer to the file object
	 *
	 * @retval >= 0 a current position in file;
	 * @retval -EBADF when invoked on zfp that represents unopened/closed file;
	 * @retval -ENOTSUP if not supported by underlying file system driver;
	 * @retval <0 an other negative errno code on error.
	 *
	 * The current revision does not validate the file object.
	 */
	off_t (*tell)(struct fs_file_t *zfp);

	/**
	 * @brief Truncate or extend an open file to a given size
	 *
	 * Truncates the file to the new length if it is shorter than the current
	 * size of the file. Expands the file if the new length is greater than the
	 * current size of the file. The expanded region would be filled with zeroes.
	 *
	 * @note In the case of expansion, if the volume got full during the
	 * expansion process, the function will expand to the maximum possible length
	 * and return success.  Caller should check if the expanded size matches the
	 * requested length.
	 *
	 * @param zfp Pointer to the file object
	 * @param length New size of the file in bytes
	 *
	 * @retval 0 on success;
	 * @retval -EBADF when invoked on zfp that represents unopened/closed file;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 an other negative errno code on error.
	 */
	int (*truncate)(struct fs_file_t *zfp, off_t length);

	/**
	 * @brief Flush cached write data buffers of an open file
	 *
	 * The function flushes the cache of an open file; it can be invoked to ensure
	 * data gets written to the storage media immediately, e.g. to avoid data loss
	 * in case if power is removed unexpectedly.
	 * @note Closing a file will cause caches to be flushed correctly so the
	 * function need not be called when the file is being closed.
	 *
	 * @param zfp Pointer to the file object
	 *
	 * @retval 0 on success;
	 * @retval -EBADF when invoked on zfp that represents unopened/closed file;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 a negative errno code on error.
	 */
	int (*sync)(struct fs_file_t *zfp);

	/**
	 * @brief Directory create
	 *
	 * Creates a new directory using specified path.
	 *
	 * @param path Path to the directory to create
	 *
	 * @retval 0 on success;
	 * @retval -EEXIST if entry of given name exists;
	 * @retval -EROFS if @p path is within read-only directory, or when
	 *         file system has been mounted with the FS_MOUNT_FLAG_READ_ONLY flag;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 an other negative errno code on error
	 */
	int (*mkdir)(const char *path);

	/**
	 * @brief Directory open
	 *
	 * Opens an existing directory specified by the path.
	 *
	 * @param zdp Pointer to the @b initialized directory object
	 * @param path Path to the directory to open
	 *
	 * @retval 0 on success;
	 * @retval -EINVAL when a bad directory path is given;
	 * @retval -EBUSY when zdp is already used;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 a negative errno code on error.
	 */
	int (*opendir)(struct fs_dir_t *zdp, const char *path);

	/**
	 * @brief Directory read entry
	 *
	 * Reads directory entries of an open directory. In end-of-dir condition,
	 * the function will return 0 and set the <tt>entry->name[0]</tt> to 0.
	 *
	 * @note: Most existing underlying file systems do not generate POSIX
	 * special directory entries "." or "..".  For consistency the
	 * abstraction layer will remove these from lower layer results so
	 * higher layers see consistent results.
	 *
	 * @param zdp Pointer to the directory object
	 * @param entry Pointer to zfs_dirent structure to read the entry into
	 *
	 * @retval 0 on success or end-of-dir;
	 * @retval -ENOENT when no such directory found;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 a negative errno code on error.
	 */
	int (*readdir)(struct fs_dir_t *zdp, struct fs_dirent *entry);

	/**
	 * @brief Directory close
	 *
	 * Closes an open directory.
	 *
	 * @param zdp Pointer to the directory object
	 *
	 * @retval 0 on success;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 a negative errno code on error.
	 */
	int (*closedir)(struct fs_dir_t *zdp);

	/**
	 * @brief Mount filesystem
	 *
	 * Perform steps needed for mounting a file system like
	 * calling the file system specific mount function and adding
	 * the mount point to mounted file system list.
	 *
	 * @note Current implementation of ELM FAT driver allows only following mount
	 * points: "/RAM:","/NAND:","/CF:","/SD:","/SD2:","/USB:","/USB2:","/USB3:"
	 * or mount points that consist of single digit, e.g: "/0:", "/1:" and so forth.
	 *
	 * @param mp Pointer to the fs_mount_t structure.  Referenced object
	 *	     is not changed if the mount operation failed.
	 *	     A reference is captured in the fs infrastructure if the
	 *	     mount operation succeeds, and the application must not
	 *	     mutate the structure contents until fs_unmount is
	 *	     successfully invoked on the same pointer.
	 *
	 * @retval 0 on success;
	 * @retval -ENOENT when file system type has not been registered;
	 * @retval -ENOTSUP when not supported by underlying file system driver;
	 *         when @c FS_MOUNT_FLAG_USE_DISK_ACCESS is set but driver does not
	 *         support it.
	 * @retval -EROFS if system requires formatting but @c FS_MOUNT_FLAG_READ_ONLY
	 *	   has been set;
	 * @retval <0 an other negative errno code on error.
	 */
	int (*mount)(struct fs_mount_t *mp);

	/**
	 * @brief Unmount filesystem
	 *
	 * Perform steps needed to unmount a file system like
	 * calling the file system specific unmount function and removing
	 * the mount point from mounted file system list.
	 *
	 * @param mp Pointer to the fs_mount_t structure
	 *
	 * @retval 0 on success;
	 * @retval -EINVAL if no system has been mounted at given mount point;
	 * @retval -ENOTSUP when not supported by underlying file system driver;
	 * @retval <0 an other negative errno code on error.
	 */
	int (*unmount)(struct fs_mount_t *mp);

	/**
	 * @brief Get path of mount point at index
	 *
	 * This function iterates through the list of mount points and returns
	 * the directory name of the mount point at the given @p index.
	 * On success @p index is incremented and @p name is set to the mount directory
	 * name.  If a mount point with the given @p index does not exist, @p name will
	 * be set to @c NULL.
	 *
	 * @param index Pointer to mount point index
	 * @param name Pointer to pointer to path name
	 *
	 * @retval 0 on success;
	 * @retval -ENOENT if there is no mount point with given index.
	 */
	int (*readmount)(int *index, const char **name);

	/**
	 * @brief File or directory status
	 *
	 * Checks the status of a file or directory specified by the @p path.
	 * @note The file on a storage device may not be updated until it is closed.
	 *
	 * @param path Path to the file or directory
	 * @param entry Pointer to the zfs_dirent structure to fill if the file or
	 * directory exists.
	 *
	 * @retval 0 on success;
	 * @retval -EINVAL when a bad directory or file name is given;
	 * @retval -ENOENT when no such directory or file is found;
	 * @retval -ENOTSUP when not supported by underlying file system driver;
	 * @retval <0 negative errno code on error.
	 */
	int (*stat)(const char *path, struct fs_dirent *entry);

	/**
	 * @brief Retrieves statistics of the file system volume
	 *
	 * Returns the total and available space in the file system volume.
	 *
	 * @param path Path to the mounted directory
	 * @param stat Pointer to the zfs_statvfs structure to receive the fs
	 * statistics.
	 *
	 * @retval 0 on success;
	 * @retval -EINVAL when a bad path to a directory, or a file, is given;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 an other negative errno code on error.
	 */
	int (*statvfs)(const char *path, struct fs_statvfs *stat);

	/**
	 * @brief Create fresh file system
	 *
	 * @param fs_type Type of file system to create.
	 * @param dev_id Id of storage device.
	 * @param cfg Backend dependent init object. If NULL then default configuration is used.
	 * @param flags Additional flags for file system implementation.
	 *
	 * @retval 0 on success;
	 * @retval <0 negative errno code on error.
	 */
	int (*mkfs)(int fs_type, uintptr_t dev_id, void *cfg, int flags);

	/**
	 * @brief Register a file system
	 *
	 * Register file system with virtual file system.
	 * Number of allowed file system types to be registered is controlled with the
	 * CONFIG_FILE_SYSTEM_MAX_TYPES Kconfig option.
	 *
	 * @param type Type of file system (ex: @c FS_FATFS)
	 * @param fs Pointer to File system
	 *
	 * @retval 0 on success;
	 * @retval -EALREADY when a file system of a given type has already been registered;
	 * @retval -ENOSCP when there is no space left, in file system registry, to add
	 *	   this file system type.
	 */
	int (*registerFs)(int type, const struct fs_file_system_t *fs);

	/**
	 * @brief Unregister a file system
	 *
	 * Unregister file system from virtual file system.
	 *
	 * @param type Type of file system (ex: @c FS_FATFS)
	 * @param fs Pointer to File system
	 *
	 * @retval 0 on success;
	 * @retval -EINVAL when file system of a given type has not been registered.
	 */
	int (*unregisterFs)(int type, const struct fs_file_system_t *fs);

	/**
	 * @brief Attempt to proactively clean file system
	 *
	 * Returns a negative error code on failure.
	 * Ignored cleaning request is not a failure.
	 *
	 * @param mp Pointer to the mounted fs_mount_t structure.
	 *
	 * @retval 0 on success;
	 * @retval -EINVAL when a bad path to a directory, or a file, is given;
	 * @retval -ENOTSUP when not implemented by underlying file system driver;
	 * @retval <0 an other negative errno code on error.
	 */
	int (*gc)(struct fs_mount_t *mp);


};

extern const struct mu_fs_if muFs;

#ifdef __cplusplus
}
#endif

#endif /* MU_FS_H_ */
