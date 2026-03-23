/*
 * Copyright (c) 2025 TecInvent Electronics Ltd
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MCU_RTC_H_
#define MCU_RTC_H_

#include <stdint.h>
#include <time.h>

/** Minimum valid Unix timestamp - 2026-03-17 06:22:39 GMT+0000 */
#define MIN_UNIX_TIME 1773728559LL
/** Maximum valid Unix timestamp (9999-12-31 23:59:59 GMT+0000). RTC allows max 4 digits for year.
 */
#define MAX_UNIX_TIME 253402300799LL
/**
 * RTC date and time structure.
 */
struct mu_rtc_dataTime {
	uint16_t year;       /**< Year [2026, 9999] */
	uint8_t month;       /**< Month [1, 12] */
	uint8_t day;         /**< Day [1, 31] */
	uint8_t hour;        /**< Hour [0, 23] */
	uint8_t minute;      /**< Minute [0, 59] */
	uint8_t second;      /**< Second [0, 59] */
	uint32_t nanosecond; /**< Nanosecond [0, 999999999] */
};

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
	 * gets the current RTC time as a Unix timestamp.
	 * @param unixTime Pointer to store the retrieved Unix timestamp.
	 * @retval 0 on success.
	 * @retval Negative errno code on failure.
	 */
	int (*getDateTimeUnix)(time_t *unixTime);

	/**
	 * Sets the RTC time.
	 * @param mu_rtc_dt Pointer to the date/time structure containing the values to set.
	 * @retval 0 on success.
	 * @retval Negative errno code on failure.
	 */
	int (*setDateTime)(struct mu_rtc_dataTime mu_rtc_dt);
	/**
	 * Gets the RTC time.
	 * @param mu_rtc_dt Pointer to the structure where the retrieved time will be stored.
	 * @retval 0 on success.
	 * @retval Negative errno code on failure.
	 */
	int (*getDateTime)(struct mu_rtc_dataTime *mu_rtc_dt);
};

extern const struct mu_rtc_if muRtc;

#ifdef __cplusplus
}
#endif

#endif /* MCU_RTC_H_ */
