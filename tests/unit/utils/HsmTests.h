/*
* hsm.h
 *
 *  Created on: Jul 18, 2025
 *      Author: Dina Gomes Dias
 */


#ifndef TESTS_UNIT_HSM_TEST_H_
#define TESTS_UNIT_HSM_TEST_H_

#include "utils/hsm.h"

#ifdef __cplusplus
extern "C" {
#endif

MU_HSM_DECL_ENTRY_LOOP_FN(startup);
MU_HSM_DECL_ALL_FN(stopped);
MU_HSM_DECL_ALL_FN(running);
MU_HSM_DECL_ALL_FN(paused);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_UNIT_HSM_TEST_H_ */
