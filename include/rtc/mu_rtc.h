/*
 * Copyright (c) 2026 TecInvent Electronics Ltd
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MCU_RTC_H_
#define MCU_RTC_H_

#include <stdint.h>
#include "rtc/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mu_rtc_if {
	/**
	 * Initializes the RTC
	 * @return Returns 0 on success, otherwise it returns a negative value
	 */
	int (*init)(void);

	/**
	 * Sets the RTC time.
	 * @param rtc_tm Pointer to the rtc structure containing the values to set.
	 * @retval 0 on success.
	 * @retval Negative errno code on failure.
	 */
	int (*setDateTime)(struct rtc_time rtc_tm);
	/**
	 * Gets the RTC time.
	 * @param rtc_tm Pointer to the structure where the retrieved time will be stored.
	 * @retval 0 on success.
	 * @retval Negative errno code on failure.
	 */
	int (*getDateTime)(struct rtc_time *rtc_tm);
};

extern const struct mu_rtc_if muRtc;

#ifdef __cplusplus
}
#endif

#endif /* MCU_RTC_H_ */
