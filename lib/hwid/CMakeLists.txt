# Copyright (c) 2024, TecInvent Electronics Ltd
# SPDX-License-Identifier: Apache-2.0

target_sources_ifdef(CONFIG_SHELL app PRIVATE src/hwid_shell.c)

zephyr_library()
zephyr_library_sources(
        src/hwid.c
)
