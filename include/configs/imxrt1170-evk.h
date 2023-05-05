/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2022
 * Author(s): Jesse Taube <Mr.Bossman075@gmail.com>
 * Giulio Benetti <giulio.benetti@benettiengineering.com>
 */

#ifndef __IMXRT1170_EVK_H
#define __IMXRT1170_EVK_H

#include <asm/arch/imx-regs.h>

#define CONFIG_SYS_INIT_SP_ADDR 0x20340000

#define ESDHCI_QUIRK_BROKEN_TIMEOUT_VALUE	1

/*
 * Configuration of the external SDRAM memory
 */

#define PHYS_SDRAM			0x80000000
#define PHYS_SDRAM_SIZE			(64 * 1024 * 1024)

#define DMAMEM_SZ_ALL			(1 * 1024 * 1024)
#define DMAMEM_BASE			(PHYS_SDRAM + PHYS_SDRAM_SIZE - \
					 DMAMEM_SZ_ALL)
/* For SPL */
#ifdef CONFIG_SUPPORT_SPL
#define CONFIG_SPL_STACK		CONFIG_SYS_INIT_SP_ADDR
#define CONFIG_SYS_SPL_LEN		0x00008000
#define CONFIG_SYS_UBOOT_START		0x202403FD
#endif
/* For SPL ends */

#endif /* __IMXRT1170_EVK_H */
