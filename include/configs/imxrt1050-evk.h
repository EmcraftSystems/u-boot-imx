/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2019
 * Author(s): Giulio Benetti <giulio.benetti@benettiengineering.com>
 * Copyright (C) 2023-2025 Emcraft Systems
 * Author(s): Vladimir Skvortsov <vskvortsov@emcraft.com>
 */

#ifndef __IMXRT1050_EVK_H
#define __IMXRT1050_EVK_H

#include <asm/arch/imx-regs.h>

#define ESDHCI_QUIRK_BROKEN_TIMEOUT_VALUE	1

#define PHYS_SDRAM			0x80000000
#define PHYS_SDRAM_SIZE			(32 * 1024 * 1024)

#define FB_RESERVED_SIZE		(1024 * 1024) /*  should be 2^N, 5<=N<=32 */
#define FB_RESERVED_BASE		(PHYS_SDRAM + PHYS_SDRAM_SIZE - \
					 FB_RESERVED_SIZE) /* should be multiple to the region size i.e. FB_RESERVED_SIZE */

#define DMAMEM_SZ_ALL			(1024 * 1024) /* should be 2^N */
#define DMAMEM_BASE			(FB_RESERVED_BASE - DMAMEM_SZ_ALL)  /* should be multiple to DMAMEM_SZ_ALL */

#ifdef CONFIG_VIDEO
#define CONFIG_FB_ADDR			FB_RESERVED_BASE
#endif

/*
 * Address of U-Boot for SPI NOR boot
 */

#define CFG_SYS_UBOOT_BASE			0x60010000

#endif /* __IMXRT1050_EVK_H */
