// SPDX-License-Identifier: GPL-2.0+
/*
 * Fuse commands
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

#define K3_SIP_OTP_WRITEBUFF     0xC2000000

static int strtou32(const char *str, unsigned int base, u32 *result)
{
	char *ep;

	*result = simple_strtoul(str, &ep, base);
	if (ep == str || *ep != '\0')
		return -EINVAL;

	return 0;
}

static int strtoul(const char *str, unsigned int base, ulong *result)
{
	char *ep;

	*result = simple_strtoul(str, &ep, base);
	if (ep == str || *ep != '\0')
		return -EINVAL;

	return 0;
}

static int confirm_prog(void)
{
	puts("Warning: Programming fuses is an irreversible operation!\n"
			"         This may brick your system.\n"
			"         Use this command only if you are sure of "
					"what you are doing!\n"
			"\nReally perform this fuse programming? <y/N>\n");

	if (confirm_yesno())
		return 1;

	puts("Fuse programming aborted\n");
	return 0;
}

static int fuse_read(u32 bank, u32 word, u32 *val)
{
    // TODO: fuse read will be implemented in the second phase of driver udpate.
    return -EPERM;
}

static int fuse_sense(u32 bank, u32 word, u32 *val)
{
    // TODO: fuse sense will be implemented in the second phase of driver udpate.
    return -EPERM;
}

static int fuse_prog(u32 bank, u32 word, u32 val)
{
    // TODO: fuse prog will be implemented in the second phase of driver udpate.
    return -EPERM;
}

static int fuse_override(u32 bank, u32 word, u32 val)
{
    // TODO: fuse override will be implemented in the second phase of driver udpate.
    return -EPERM;
}

/**
 * Writes the buffer at the specified address to the OTP (One-Time Programmable)
 * efuses using a secure monitor call (SMC).
 *
 * @param addr The memory address of the buffer to write.
 *
 * @return Returns the result of the SMC call. If the call is successful,
 *         the return value is the same as the value of res.a0. If the
 *         call fails, the return value is 1.
 */
static int fuse_writebuff(ulong addr)
{
    struct arm_smccc_res res;

    /* Make SiP SMC call and send the addr in the parameter register */
    arm_smccc_smc(K3_SIP_OTP_WRITEBUFF, (unsigned long)addr,
            0, 0, 0, 0, 0, 0, &res);

    if (res.a0 != 0) {
        printf("SMC call failed: Error code %ld\n", res.a0);
        return CMD_RET_FAILURE;
    }
    return res.a0;
}

static int do_fuse(struct cmd_tbl *cmdtp, int flag, int argc,
		   char *const argv[])
{
	const char *op = cmd_arg1(argc, argv);
	int confirmed = argc >= 3 && !strcmp(argv[2], "-y");
	u32 bank, word, cnt, val, cmp;
	ulong addr;
	void *buf, *start;
	int ret, i;

	argc -= 2 + confirmed;
	argv += 2 + confirmed;
	if (!strcmp(op, "writebuff")) {
		if (argc != 1 || strtoul(argv[0], 16, &addr))
		return CMD_RET_USAGE;
	} else {
		if (argc < 2 || strtou32(argv[0], 0, &bank) ||
			strtou32(argv[1], 0, &word))
		return CMD_RET_USAGE;
	}

	if (!strcmp(op, "read")) {
		if (argc == 2)
			cnt = 1;
		else if (argc != 3 || strtou32(argv[2], 0, &cnt))
			return CMD_RET_USAGE;

		printf("Reading bank %u:\n", bank);
		for (i = 0; i < cnt; i++, word++) {
			if (!(i % 4))
				printf("\nWord 0x%.8x:", word);

			ret = fuse_read(bank, word, &val);
			if (ret)
				goto err;

			printf(" %.8x", val);
		}
		putc('\n');
	} else if (!strcmp(op, "readm")) {
		if (argc == 3)
			cnt = 1;
		else if (argc != 4 || strtou32(argv[3], 0, &cnt))
			return CMD_RET_USAGE;

		addr = simple_strtoul(argv[2], NULL, 16);

		start = map_sysmem(addr, 4);
		buf = start;

		printf("Reading bank %u len %u to 0x%lx\n", bank, cnt, addr);
		for (i = 0; i < cnt; i++, word++) {
			ret = fuse_read(bank, word, &val);
			if (ret)
				goto err;

			*((u32 *)buf) = val;
			buf += 4;
		}

		unmap_sysmem(start);
	} else if (!strcmp(op, "cmp")) {
		if (argc != 3 || strtou32(argv[2], 0, &cmp))
			return CMD_RET_USAGE;

		printf("Comparing bank %u:\n", bank);
		printf("\nWord 0x%.8x:", word);
		printf("\nValue 0x%.8x:", cmp);

		ret = fuse_read(bank, word, &val);
		if (ret)
			goto err;

		printf("0x%.8x\n", val);
		if (val != cmp) {
			printf("failed\n");
			return CMD_RET_FAILURE;
		}
		printf("passed\n");
	} else if (!strcmp(op, "sense")) {
		if (argc == 2)
			cnt = 1;
		else if (argc != 3 || strtou32(argv[2], 0, &cnt))
			return CMD_RET_USAGE;

		printf("Sensing bank %u:\n", bank);
		for (i = 0; i < cnt; i++, word++) {
			if (!(i % 4))
				printf("\nWord 0x%.8x:", word);

			ret = fuse_sense(bank, word, &val);
			if (ret)
				goto err;

			printf(" %.8x", val);
		}
		putc('\n');
	} else if (!strcmp(op, "prog")) {
		if (argc < 3)
			return CMD_RET_USAGE;

		for (i = 2; i < argc; i++, word++) {
			if (strtou32(argv[i], 16, &val))
				return CMD_RET_USAGE;

			printf("Programming bank %u word 0x%.8x to 0x%.8x...\n",
					bank, word, val);
			if (!confirmed && !confirm_prog())
				return CMD_RET_FAILURE;
			ret = fuse_prog(bank, word, val);
			if (ret)
				goto err;
		}
	} else if (!strcmp(op, "override")) {
		if (argc < 3)
			return CMD_RET_USAGE;

		for (i = 2; i < argc; i++, word++) {
			if (strtou32(argv[i], 16, &val))
				return CMD_RET_USAGE;

			printf("Overriding bank %u word 0x%.8x with "
					"0x%.8x...\n", bank, word, val);
			ret = fuse_override(bank, word, val);
			if (ret)
				goto err;
		}
	} else if (!strcmp(op, "writebuff")) {
		printf("Programming fuses with buffer at addr 0x%.8lx\n",
				addr);
		if (!confirmed && !confirm_prog())
			return CMD_RET_FAILURE;
		ret = fuse_writebuff(addr);
		if (ret)
			goto err;
	} else {
		return CMD_RET_USAGE;
	}

	return 0;

err:
	puts("ERROR\n");
	return CMD_RET_FAILURE;
}

U_BOOT_CMD(
	fuse, CONFIG_SYS_MAXARGS, 0, do_fuse,
	"Fuse sub-system",
	     "read <bank> <word> [<cnt>] - read 1 or 'cnt' fuse words,\n"
	"    starting at 'word'\n"
	"fuse cmp <bank> <word> <hexval> - compare 'hexval' to fuse\n"
	"    at 'word'\n"
	"fuse readm <bank> <word> <addr> [<cnt>] - read 1 or 'cnt' fuse words,\n"
	"    starting at 'word' into memory at 'addr'\n"
	"fuse sense <bank> <word> [<cnt>] - sense 1 or 'cnt' fuse words,\n"
	"    starting at 'word'\n"
	"fuse prog [-y] <bank> <word> <hexval> [<hexval>...] - program 1 or\n"
	"    several fuse words, starting at 'word' (PERMANENT)\n"
	"fuse override <bank> <word> <hexval> [<hexval>...] - override 1 or\n"
	"    several fuse words, starting at 'word'\n"
	"fuse writebuff [-y] <addr> - program fuse data\n"
	"    using a structured buffer in memory starting at 'addr'"
);
