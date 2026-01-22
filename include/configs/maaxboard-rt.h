/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2025 Emcraft Systems
 */

#ifndef __MAAXBOARD_RT_H
#define __MAAXBOARD_RT_H

#include <asm/arch/imx-regs.h>

/*
 * Configuration of the external SDRAM memory
 */

#define PHYS_SDRAM			0x80000000
#define PHYS_SDRAM_SIZE			(32 * 1024 * 1024)

#define FB_RESERVED_SIZE		(1 * 1024 * 1024) /* should be 2^N, 5<=N<=32 */
#define FB_RESERVED_BASE		(PHYS_SDRAM + PHYS_SDRAM_SIZE - \
					 FB_RESERVED_SIZE) /* should be multiple to the region size i.e. FB_RESERVED_SIZE */

#define DMAMEM_SZ_ALL			(1 * 1024 * 1024)  /* should be 2^N */
#define DMAMEM_BASE			(FB_RESERVED_BASE - DMAMEM_SZ_ALL) /* should be multiple to DMAMEM_SZ_ALL */

#ifdef CONFIG_SPL_NOR_SUPPORT
#define CFG_SYS_UBOOT_BASE             0x30010000
#endif

#define CFG_SYS_FLASH_BASE             0x30000000

#endif /* __MAAXBOARD_RT_H */
