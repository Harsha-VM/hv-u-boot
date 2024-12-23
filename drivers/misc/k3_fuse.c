#include <errno.h>
#include <stdio.h>
#include <fuse.h>
#include <linux/arm-smccc.h>
#include <string.h>

#define K3_OTP_FUSE_WRITEBUFF_LEGACY     0xC2000000
#define K3_OTP_FUSE_WRITEBUFF            0xC2000001

struct fuse_writebuff_header {
    u32 version_info;
    u32 fuse_mode;
} __attribute__((packed));

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
    struct fuse_writebuff_header fuse_wb_hdr;
    /* Check if the vendor string is equal to k3 */
    if (strcmp(vendor, "k3") != 0) {
        printf("Error: Unsupported vendor '%s'\nSupported Vendor: k3\n", vendor);
        return -EINVAL;
    }

    /*
	 * The addr is decoded to a struct fuse_writebuff_header.
	 * To store u32 version_info and u32 fuse_mode and the addr is updated to addr + 64 bits,
     * from here on addr contains the payload.
     * The addr is decoded to the struct to make different ATF SMC calls based on fuse_mode.
	 */
    fuse_wb_hdr.version_info = *((u32 *)addr);
    addr += sizeof(u32);
    fuse_wb_hdr.fuse_mode = *((u32 *)addr);
    addr += sizeof(u32);
    
    /*
	 * If fuse_mode is equal to 0, call SMC K3_OTP_FUSE_WRITEBUFF_LEGACY 0xC2000000
	 * If fuse_mode is equal to 0, call SMC K3_OTP_FUSE_WRITEBUFF        0xC2000001
     * Else, Return -EINVAL
	 */
    if (fuse_wb_hdr.fuse_mode == 0) {
        arm_smccc_smc(K3_OTP_FUSE_WRITEBUFF_LEGACY, (unsigned long)fuse_wb_hdr.fuse_mode,
            (unsigned long)addr, fuse_wb_hdr.version_info, 0, 0, 0, 0, &res);
	} else if (fuse_wb_hdr.fuse_mode == 1) {
        arm_smccc_smc(K3_OTP_FUSE_WRITEBUFF, (unsigned long)fuse_wb_hdr.fuse_mode,
            (unsigned long)addr, fuse_wb_hdr.version_info, 0, 0, 0, 0, &res);
    } else {
        printf("Invalid fuse mode %u, ONLY fuse mode 0,1 is supported\n", fuse_wb_hdr.fuse_mode);
		return -EINVAL;
    }

    if (res.a0 != 0) {
        printf("SMC call failed: Error code %ld\n", res.a0);
        return -1; // Indicate failure
    }
	return res.a0;
}
