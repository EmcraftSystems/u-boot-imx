/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2022
 * Author(s): Jesse Taube <Mr.Bossman075@gmail.com>
 * Giulio Benetti <giulio.benetti@benettiengineering.com>
 */

#ifndef __IMXRT1170_EVK_H
#define __IMXRT1170_EVK_H

#include <asm/arch/imx-regs.h>

#define ESDHCI_QUIRK_BROKEN_TIMEOUT_VALUE	1

/*
 * Configuration of the external SDRAM memory
 */

#define PHYS_SDRAM			0x80000000
#define PHYS_SDRAM_SIZE			(32 * 1024 * 1024)

#define FB_RESERVED_SIZE		(4 * 1024 * 1024) /* should be 2^N, 5<=N<=32 */
#define FB_RESERVED_BASE		(PHYS_SDRAM + PHYS_SDRAM_SIZE - \
					 FB_RESERVED_SIZE) /* should be multiple to the region size i.e. FB_RESERVED_SIZE */

#define DMAMEM_SZ_ALL			(1 * 1024 * 1024)  /* should be 2^N */
#define DMAMEM_BASE			(FB_RESERVED_BASE - DMAMEM_SZ_ALL) /* should be multiple to DMAMEM_SZ_ALL */

#ifdef CONFIG_SPL_NOR_SUPPORT
#define CFG_SYS_UBOOT_BASE             0x30010000
#endif

#define CFG_EXTRA_ENV_SETTINGS						\
	"image=rootfs.uImage\0"						\
	"uboot=u-boot.img\0"						\
	"spl=SPL\0"							\
	"tftpdir=imxrt1170/\0"						\
	"mmc_update_spl=tftp ${tftpdir}${spl} &&"			\
	" setexpr tmp ${filesize} / 0x200 &&"				\
	" setexpr tmp ${tmp} + 1 &&"					\
	" mmc write ${loadaddr} 2 ${tmp}\0"				\
	"mmc_update_uboot=tftp ${tftpdir}${uboot} &&"			\
	" setexpr tmp ${filesize} / 0x200 &&"				\
	" setexpr tmp ${tmp} + 1 &&"					\
	" mmc write ${loadaddr} 0x100 ${tmp}\0"				\
	"mmc_update_kernel=tftp ${tftpdir}${image} &&"			\
	" fatwrite mmc 0 ${loadaddr} ${image} ${filesize}\0"		\
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:"	\
		"${gatewayip}:${netmask}:${hostname}:eth0:off\0"	\
	"mmcboot=fatload mmc 0 ${loadaddr} ${image} && run addip &&"	\
		" bootm ${loadaddr}\0"					\
	"netboot=tftp ${tftpdir}${image} && run addip && bootm\0"	\
	"ethaddr=aa:bb:cc:dd:ee:f0\0"					\
	"serverip=172.17.0.1\0"						\
	"ipaddr=172.17.44.111\0"					\
	"netmask=255.255.0.0\0"						\
	"gatewayip=172.17.0.1\0"					\

#endif /* __IMXRT1170_EVK_H */
