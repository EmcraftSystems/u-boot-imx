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

#if defined(CONFIG_CMD_SF)
#define _CFG_EXTRA_SF_ENV_SETTINGS					\
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
	"kernel_sf_size=0x800000\0"					\
	"rootfs_sf_offset=0x880000\0"					\
	"rootfs_sf_size=0x780000\0"					\
	"sf_kernel_update=tftp ${tftpdir}${project}.uImage &&"		\
		" sf erase ${kernel_sf_offset} ${kernel_sf_size} &&"	\
		" sf write ${loadaddr} ${kernel_sf_offset} ${filesize}\0"\
	"sf_rootfs_update=tftp ${tftpdir}${project}.${fstype} &&"	\
		" sf erase ${rootfs_sf_offset} ${rootfs_sf_size} &&"	\
		" sf write ${loadaddr} ${rootfs_sf_offset} ${filesize}\0"\

#else
#define _CFG_EXTRA_SF_ENV_SETTINGS
#endif

#define CFG_EXTRA_ENV_SETTINGS						\
	"image=rootfs.uImage\0"						\
	"uboot=u-boot.img\0"						\
	"spl=SPL\0"							\
	"tftpdir=imxrt1170/\0"						\
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:"	\
		"${gatewayip}:${netmask}:${hostname}:eth0:off\0"	\
	"netboot=tftp ${tftpdir}${image} && run addip && bootm\0"	\
	"ethaddr=aa:bb:cc:dd:ee:f0\0"					\
	"serverip=172.17.0.1\0"						\
	"ipaddr=172.17.44.111\0"					\
	"netmask=255.255.0.0\0"						\
	"gatewayip=172.17.0.1\0"					\
	_CFG_EXTRA_SF_ENV_SETTINGS

#endif /* __MAAXBOARD_RT_H */
