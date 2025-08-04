/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
 */

#ifndef MU_HWID_H
#define MU_HWID_H

#ifdef __cplusplus
extern "C" {
#endif

enum mu_hwid_type {

	MU_HWID_TYPE_GPIO = 0,
	MU_HWID_TYPE_MEM

};


struct mu_hwid_if {

	int (*init)(void);
	int (*code)(const enum mu_hwid_type type, int *code);

};

extern struct mu_hwid_if muHwId;

#ifdef __cplusplus
}
#endif

#endif /* MU_HWID_H */
