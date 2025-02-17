// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2025 Texas Instruments Incorporated, <www.ti.com>
 */

#include <errno.h>
#include <stdio.h>
#include <fuse.h>
#include <linux/arm-smccc.h>
#include <string.h>

#define K3_SIP_OTP_WRITEBUFF     0xC2000000

int fuse_read(u32 bank, u32 word, u32 *val)
{
    // TODO: fuse read will be implemented in the second phase of driver udpate.
    return -EPERM;
}

int fuse_sense(u32 bank, u32 word, u32 *val)
{
    // TODO: fuse sense will be implemented in the second phase of driver udpate.
    return -EPERM;
}

int fuse_prog(u32 bank, u32 word, u32 val)
{
    // TODO: fuse prog will be implemented in the second phase of driver udpate.
    return -EPERM;
}

int fuse_override(u32 bank, u32 word, u32 val)
{
    // TODO: fuse override will be implemented in the second phase of driver udpate.
    return -EPERM;
}

int fuse_writebuff(ulong addr)
{
    struct arm_smccc_res res;

    /* Make SiP SMC call and send the addr in the parameter register */
    arm_smccc_smc(K3_SIP_OTP_WRITEBUFF, (unsigned long)addr,
            0, 0, 0, 0, 0, 0, &res);

    if (res.a0 != 0) {
        printf("SMC call failed: Error code %ld\n", res.a0);
        return -1; // Indicate failure
    }
    return res.a0;
}
