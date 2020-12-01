/*
 * Copyright 2017 NXP
 *
 * Peng Fan <peng.fan@nxp.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <asm/arch/imx-regs.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <errno.h>
#include <fuse.h>

struct imx_mac_fuse {
	u32 mac_addr0;
	u32 rsvd0[3];
	u32 mac_addr1;
	u32 rsvd1[3];
	u32 mac_addr2;
	u32 rsvd2[7];
};

#define MAC_FUSE_MX6_OFFSET	0x620
#define MAC_FUSE_MX7_OFFSET	0x640

void imx_get_mac_from_fuse(int dev_id, unsigned char *mac)
{
	struct imx_mac_fuse *fuse;
	u32 offset;
	bool has_second_mac;

	offset = is_mx6() ? MAC_FUSE_MX6_OFFSET : MAC_FUSE_MX7_OFFSET;
	fuse = (struct imx_mac_fuse *)(ulong)(OCOTP_BASE_ADDR + offset);
	has_second_mac = is_mx7() || is_mx6sx() || is_mx6ul() || is_mx6ull();

	if (has_second_mac && dev_id == 1) {
		u32 value = readl(&fuse->mac_addr2);

		mac[0] = value >> 24;
		mac[1] = value >> 16;
		mac[2] = value >> 8;
		mac[3] = value;

		value = readl(&fuse->mac_addr1);
		mac[4] = value >> 24;
		mac[5] = value >> 16;

	} else {
		u32 value = readl(&fuse->mac_addr1);

		mac[0] = value >> 8;
		mac[1] = value;

		value = readl(&fuse->mac_addr0);
		mac[2] = value >> 24;
		mac[3] = value >> 16;
		mac[4] = value >> 8;
		mac[5] = value;
	}
}

#ifdef CONFIG_CMD_FUSE
int imx_save_mac_to_fuse(int dev_id, unsigned char *mac)
{
	u32 value0, value1;

	if (dev_id != 0) {
		printf("Save mac to fuse for %d device is not supported\n", dev_id);
		return -1;
	}

	if (fuse_read(9, 1, &value0) || fuse_read(9, 0, &value1)) {
		printf("Failed to read mac from fuses\n");
		return -1;
	}

	if ((value0 & 0xffff) != 0 || value1 != 0) {
		printf("Fuses bank is not empty: %04x%08x\n", value0 & 0xffff, value1);
		return -1;
	}

	value0 |= mac[0] << 8 | mac[1];
	value1 = mac[2] << 24 | mac[3] << 16 | mac[4] << 8 | mac[5];

	if (fuse_prog(9, 1, value0) || fuse_prog(9, 0, value1)) {
		printf("Failed to write mac to fuses\n");
		return -1;
	}

	return 0;
}
#endif
