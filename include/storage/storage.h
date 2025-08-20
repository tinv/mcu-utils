/*
* Copyright (c) 2025 TecInvent Electronics Ltd
*/

#ifndef MU_STORAGE_H_
#define MU_STORAGE_H_

#include <stdint.h>
#include <stddef.h>
#include <zephyr/fs/fs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mu_storage_config {
	const char *name;
	const char *mount_point;
	int fs_type;
} mu_storage_config_t;

struct mu_storage_if {

	/**!
	 * Inizializes Concrete storage
	 * @retval 0 on success
	 * @retval For other values refert to errno
	 */
	int (*init)(void);

	/**!
	 *
	 * @param config
	 * @return
	 */
	int (*mount)(const mu_storage_config_t *config);

	/**!
	 *
	 * @param config
	 * @return
	 */
	int (*umount)(const mu_storage_config_t *config);

	/**!
	 * Writes to a file @p fname data pointed by @p buf (of size @p size)
	 * @param fname File path
	 * @param buf Buffer
	 * @param size Size
	 * @retval 0 on success
	 * @retval Negative number see errno
	 */
	int (*file_write)(const char *fname, const uint8_t *buf, const size_t size);

	/**!
	 * Reads contents of a file @p fname
	 * @param fname
	 * @param buf
	 * @param size
	 * @return
	 */
	int (*file_read)(const char *fname, uint8_t *buf, const size_t size);

	/**
	 * Remove a file
	 * @param fname
	 * @return
	 */
	int (*file_remove)(const char *fname);

	/**!
	 *
	 * @param fname
	 * @return
	 */
	int (*file_exists)(const char *fname);
};

extern const struct mu_storage_if muStorage;

#ifdef __cplusplus
}
#endif

#endif /* MU_STORAGE_H_ */

