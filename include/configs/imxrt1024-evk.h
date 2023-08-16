/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2023 Emcraft Systems
 * Author(s): Vladimir Skvortsov <vskvortsov@emcraft.com>
 */

#ifndef __IMXRT1024_EVK_H
#define __IMXRT1024_EVK_H

#include <asm/arch/imx-regs.h>

#define CONFIG_SYS_INIT_SP_ADDR		0x20240000

#define ESDHCI_QUIRK_BROKEN_TIMEOUT_VALUE	1

#define PHYS_SDRAM			0x80000000
#define PHYS_SDRAM_SIZE			(32 * 1024 * 1024)

#define DMAMEM_SZ_ALL			(1 * 1024 * 1024)
#define DMAMEM_BASE			(PHYS_SDRAM + PHYS_SDRAM_SIZE - \
					 DMAMEM_SZ_ALL)
#define CONFIG_SYS_BOOTM_LEN		SZ_32M

/*
 * Configuration of the external SDRAM memory
 */

/* For SPL */
#ifdef CONFIG_SUPPORT_SPL
#define CONFIG_SPL_STACK		CONFIG_SYS_INIT_SP_ADDR
#define CONFIG_SYS_SPL_LEN		0x00008000
#define CONFIG_SYS_UBOOT_START		0x800023FD
#endif
/* For SPL ends */

#define CONFIG_EXTRA_ENV_SETTINGS \
	"image=rootfs.uImage\0" \
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:"	\
		"${gatewayip}:${netmask}:${hostname}:eth0:off\0"	\
	"mmcboot=fatload mmc 0 ${loadaddr} ${image} && run addip &&"	\
		" bootm ${loadaddr}\0" \
	"netboot=tftp ${image} && run addip && bootm\0"			\
	"ethaddr=aa:bb:cc:dd:ee:e0\0"					\
	"serverip=172.17.0.1\0"						\
	"gatewayip=172.17.0.1\0"					\
	"ipaddr=172.17.44.124\0"					\
	"netmask=255.255.0.0\0"						\

#endif /* __IMXRT1024_EVK_H */
