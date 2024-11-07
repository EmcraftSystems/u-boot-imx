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

#define FB_RESERVED_SIZE		(1024 * 1024) /*  should be 2^N, 5<=N<=32 */
#define FB_RESERVED_BASE		(PHYS_SDRAM + PHYS_SDRAM_SIZE - \
					 FB_RESERVED_SIZE) /* should be multiple to the region size i.e. FB_RESERVED_SIZE */

#define DMAMEM_SZ_ALL			(1024 * 1024) /* should be 2^N */
#define DMAMEM_BASE			(FB_RESERVED_BASE - DMAMEM_SZ_ALL)  /* should be multiple to DMAMEM_SZ_ALL */

#ifdef CONFIG_VIDEO
#define CONFIG_FB_ADDR			FB_RESERVED_BASE

#define _CONFIG_EXTRA_ENV_SETTINGS_VIDEO \
		"splashsource=mmc_fs\0"
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

#if defined(CONFIG_CMD_SF)
#define _CONFIG_EXTRA_SF_ENV_SETTINGS					\
	"sfboot=sf probe 0 && sf read ${loadaddr}"			\
		" ${kernel_sf_offset} ${kernel_sf_size} &&"		\
		" run addip && run addrootfs && bootm ${loadaddr}\0"	\
	"addrootfs=if test ${fstype} = \"ubi\"; then"			\
			" setenv bootargs ${bootargs} ${argsubi};"	\
		" elif test ${fstype} = \"jffs2\"; then"		\
			" setenv bootargs ${bootargs} ${argsjffs2};"	\
		" fi;\0"						\
	"project=rootfs_flash\0"					\
	"fstype=jffs2\0"						\
	"argsubi=ubi.mtd=3 rootfstype=ubifs root=ubi0:rootfs rw\0"	\
	"argsjffs2=rootfstype=jffs2 root=/dev/mtdblock3 rw\0"		\
	"uboot_sf_offset=0x0\0"						\
	"uboot_sf_size=0x60000\0"					\
	"kernel_sf_offset=0x80000\0"					\
	"kernel_sf_size=0x400000\0"					\
	"rootfs_sf_offset=0x480000\0"					\
	"rootfs_sf_size=0x380000\0"					\
	"sf_kernel_update=tftp ${tftpdir}${project}.uImage &&"		\
		" sf erase ${kernel_sf_offset} ${kernel_sf_size} &&"	\
		" sf write ${loadaddr} ${kernel_sf_offset} ${filesize}\0"\
	"sf_rootfs_update=tftp ${tftpdir}${project}.${fstype} &&"	\
		" sf erase ${rootfs_sf_offset} ${rootfs_sf_size} &&"	\
		" sf write ${loadaddr} ${rootfs_sf_offset} ${filesize}\0"\

#else
#define _CONFIG_EXTRA_SF_ENV_SETTINGS
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
	_CONFIG_EXTRA_SF_ENV_SETTINGS					\
	_CONFIG_EXTRA_ENV_SETTINGS_VIDEO

#endif /* __IMXRT1050_EVK_H */
