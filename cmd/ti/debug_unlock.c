// SPDX-License-Identifier: GPL-2.0+
/*
 * Debug unlock commands
 *
 * Copyright (C) 2025 Texas Instruments Incorporated, <www.ti.com>
 */

#include <command.h>
#include <console.h>
#include <mapmem.h>
#include <vsprintf.h>
#include <linux/errno.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <linux/arm-smccc.h>
#include <string.h>

#define K3_SIP_DEBUG_UNLOCK 0xC2000003

static int do_debug_unlock(struct cmd_tbl *cmdtp, int flag, int argc,
		   char *const argv[])
{
	u64 addr;

	argc -= 1;
	argv += 1;
	printf("Do debug unlock\n");
	if (argc == 1)
		addr = simple_strtoul(argv[0], NULL, 16);
	else
		return CMD_RET_USAGE;

	struct arm_smccc_res res;

	/* Make SiP SMC call and send the addr in the parameter register */
	arm_smccc_smc(K3_SIP_DEBUG_UNLOCK, (u64)addr,
		      0, 0, 0, 0, 0, 0, &res);

	if (res.a0 != 0) {
		printf("SMC call failed: Error code %lu\n", res.a0);
		goto err;
	}

	return 0;

err:
	puts("ERROR\n");
	return CMD_RET_FAILURE;
}

U_BOOT_CMD(
	debug_unlock, 2, 0, do_debug_unlock,
	"Debug unlock",
	"<addr> - debug unlock\n"
);
