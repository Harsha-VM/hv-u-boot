#include <errno.h>
#include <stdio.h>
#include <fuse.h>
#include <linux/arm-smccc.h>
#include <string.h>

#define K3_SIP_OTP_WRITEBUFF     0xC2000000

int fuse_read(u32 bank, u32 word, u32 *val)
{
    printf("fuse read to k3 in u-boot is forbidden\n");
    return -EPERM;
}

int fuse_sense(u32 bank, u32 word, u32 *val)
{
    printf("fuse sense to k3 in u-boot is forbidden\n");
    return -EPERM;
}

int fuse_prog(u32 bank, u32 word, u32 val)
{
    printf("fuse prog to k3 in u-boot is forbidden\n");
    return -EPERM;
}

int fuse_override(u32 bank, u32 word, u32 val)
{
    printf("fuse override to k3 in u-boot is forbidden\n");
    return -EPERM;
}

int fuse_writebuff(const char *vendor, ulong addr)
{
    struct arm_smccc_res res;

    /* Check if the vendor string is equal to k3 */
    if (strcmp(vendor, "k3") != 0) {
        printf("Error: Unsupported vendor '%s'\nSupported Vendor: k3\n", vendor);
        return -EINVAL;
    }
    /* Make SiP SMC call and send the addr in the parameter register */
    arm_smccc_smc(K3_SIP_OTP_WRITEBUFF, (unsigned long)addr,
            0, 0, 0, 0, 0, 0, &res);

    if (res.a0 != 0) {
        printf("SMC call failed: Error code %ld\n", res.a0);
        return -1; // Indicate failure
    }
    return res.a0;
}
