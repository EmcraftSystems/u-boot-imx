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

#define DMAMEM_SZ_ALL			(2 * 1024 * 1024)
#define DMAMEM_BASE			(PHYS_SDRAM + PHYS_SDRAM_SIZE - \
					 DMAMEM_SZ_ALL)

#ifdef CONFIG_VIDEO
#define CONFIG_FB_ADDR			DMAMEM_BASE

#define _CONFIG_EXTRA_ENV_SETTINGS_VIDEO \
		"stdin=serial\0" \
		"stdout=serial,vidconsole\0" \
		"stderr=serial,vidconsole\0" \
		"splashsource=mmc_fs\0" \
		"splashimage=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0"
#else
#define _CONFIG_EXTRA_ENV_SETTINGS_VIDEO ""
#endif

#if CONFIG_IS_ENABLED(TARGET_IMXRT1050_EVK)
#define _CONFIG_TFTPDIR	"tftpdir=imxrt1050/\0"
#elif CONFIG_IS_ENABLED(TARGET_IMXRT1060_EVK)
#define _CONFIG_TFTPDIR	"tftpdir=imxrt1060/\0"
#else
#define _CONFIG_TFTPDIR	""
#endif

/*
 * Address of U-Boot for SPI NOR boot
 */

#define CFG_SYS_UBOOT_BASE			0x60010000

#define CFG_EXTRA_ENV_SETTINGS						\
	"image=rootfs.uImage\0"						\
	"uboot=u-boot.img\0"						\
	"spl=SPL\0"							\
	"mmc_update_spl=tftp ${tftpdir}${spl} &&"			\
                " setexpr tmp ${filesize} / 0x200 &&"			\
		" setexpr tmp ${tmp} + 1 &&"				\
                " mmc write ${loadaddr} 2 ${tmp}\0"			\
	"mmc_update_uboot=tftp ${tftpdir}${uboot} &&"			\
                " setexpr tmp ${filesize} / 0x200 &&"			\
		" setexpr tmp ${tmp} + 1 &&"				\
                " mmc write ${loadaddr} 0x100 ${tmp}\0"			\
        "mmc_update_kernel=tftp ${tftpdir}${image} &&"			\
                " fatwrite mmc 0 ${loadaddr} ${image} ${filesize}\0"	\
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:"	\
		"${gatewayip}:${netmask}:${hostname}:eth0:off\0"	\
	"mmcboot=fatload mmc 0 ${loadaddr} ${image} && run addip &&"	\
		" bootm ${loadaddr}\0"					\
	"netboot=tftp ${tftpdir}${image} && run addip && bootm\0"	\
	"ethaddr=aa:bb:cc:dd:ee:d0\0"					\
	"serverip=172.17.0.1\0"						\
	"gatewayip=172.17.0.1\0"					\
	"ipaddr=172.17.44.105\0"					\
	"netmask=255.255.0.0\0"						\
	_CONFIG_TFTPDIR							\
	_CONFIG_EXTRA_ENV_SETTINGS_VIDEO

#endif /* __IMXRT1050_EVK_H */
