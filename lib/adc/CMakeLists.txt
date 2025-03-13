# Copyright (c) 2025, TecInvent Electronics Ltd
# SPDX-License-Identifier: Apache-2.0

# find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

zephyr_library()
zephyr_library_sources(src/mu_adc.c)

# zephyr_include_directories_ifdef(CONFIG_FILE_SYSTEM_LITTLEFS ${ZEPHYR_BASE}/../modules/fs/littlefs)
# zephyr_include_directories_ifdef(CONFIG_FILE_SYSTEM_LITTLEFS ${ZEPHYR_BASE}/subsys/fs)
