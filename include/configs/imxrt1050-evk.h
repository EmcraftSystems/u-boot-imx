/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2019
 * Author(s): Giulio Benetti <giulio.benetti@benettiengineering.com>
 * Copyright (C) 2023 Emcraft Systems
 * Author(s): Vladimir Skvortsov <vskvortsov@emcraft.com>
 */

#ifndef __IMXRT1050_EVK_H
#define __IMXRT1050_EVK_H

#include <asm/arch/imx-regs.h>

#define ESDHCI_QUIRK_BROKEN_TIMEOUT_VALUE	1

#define PHYS_SDRAM			0x80000000
#define PHYS_SDRAM_SIZE			(32 * 1024 * 1024)

#define DMAMEM_SZ_ALL			(1 * 1024 * 1024)
#define DMAMEM_BASE			(PHYS_SDRAM + PHYS_SDRAM_SIZE - \
					 DMAMEM_SZ_ALL)
#define CONFIG_SYS_BOOTM_LEN		SZ_32M

#ifdef CONFIG_DM_VIDEO
#define CONFIG_VIDEO_BMP_LOGO

#define _CONFIG_EXTRA_ENV_SETTINGS_VIDEO \
		"stdin=serial\0" \
		"stdout=serial,vidconsole\0" \
		"stderr=serial,vidconsole\0"
#else
#define _CONFIG_EXTRA_ENV_SETTINGS_VIDEO ""
#endif

/*
 * Address of U-Boot for SPI NOR boot
 */

#define CFG_SYS_UBOOT_BASE			0x60010000

#define CONFIG_EXTRA_ENV_SETTINGS \
	"image=rootfs.uImage\0" \
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:"	\
		"${gatewayip}:${netmask}:${hostname}:eth0:off\0"	\
	"mmcboot=fatload mmc 0 ${loadaddr} ${image} && run addip &&"	\
		" bootm ${loadaddr}\0" \
	"netboot=tftp ${image} && run addip && bootm\0"			\
	"ethaddr=aa:bb:cc:dd:ee:d0\0"					\
	"serverip=172.17.0.1\0"						\
	"gatewayip=172.17.0.1\0"					\
	"ipaddr=172.17.44.105\0"					\
	"netmask=255.255.0.0\0"						\
	_CONFIG_EXTRA_ENV_SETTINGS_VIDEO

#endif /* __IMXRT1050_EVK_H */
