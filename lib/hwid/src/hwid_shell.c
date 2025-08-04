/*
* Copyright (c) 2025 TecInvent Electronics Ltd
* SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr/shell/shell.h>
#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <stdlib.h>
#include "hwid/hwid.h"

static int cmd_test_read( const struct shell* shell, size_t argc, char** argv );

SHELL_STATIC_SUBCMD_SET_CREATE( sub_test_read,
                               SHELL_CMD_ARG( read, NULL, "Tests hardware id", cmd_test_read, 1, 0 ),
                               SHELL_SUBCMD_SET_END /* Array terminated. */
);

SHELL_CMD_REGISTER( hwid, &sub_test_read, "Test hardware id", NULL );

static int cmd_test_read( const struct shell* shell, size_t argc, char** argv )
{
  int ret = 0;
  int code = -127;

  shell_print( shell, "Starting hwid" );

  ret = muHwId.code(MU_HWID_TYPE_GPIO, &code);

  if (ret < 0) {
	  shell_error(shell, "Unable to read code");
  } else {
	  shell_print(shell, "Code: '%d'", code);
  }

  return ret;

}
