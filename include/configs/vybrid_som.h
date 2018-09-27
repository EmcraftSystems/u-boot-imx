/*
 * Copyright 2018 Emcraft Systems
 *
 * Configuration settings for Vybrid-SOM Rev 3A and Linux 4.5
 *
 * Pavel Boldin <paboldin@emcraft.com>
 * Alexander Dyachenko <sasha_d@emcraft.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define tostring(s)			#s
#define stringify(s)			tostring(s)

/*#define DEBUG*/

#ifndef CONFIG_SYS_BOARD_REV
#define CONFIG_SYS_BOARD_REV            0x3A
#endif

#define CONFIG_SYS_CLOCK_FREQUENCY	498000000UL
/* #define CONFIG_SYS_CLOCK_FREQUENCY	396000000UL */

#define CONFIG_BOOT_MEDIA_NAND

/* #define CONFIG_SYS_ICACHE_OFF */
/* #define CONFIG_SYS_DCACHE_OFF */
/* #define CONFIG_SYS_ARM_CACHE_WRITETHROUGH */
/* #define CONFIG_SYS_L2CACHE_OFF */

#define CONFIG_MMC
#define CONFIG_MCFFEC

#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1_SIZE		(512 * 1024 * 1024)

#include "vybrid_som_common.h"

#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SIZE			(0x10000)
#define CONFIG_ENV_RANGE		(0x40000)
#define CONFIG_ENV_OFFSET		(0x80000)
#define CONFIG_ENV_OFFSET_REDUND	(0xc0000)

/* #define CONFIG_SPLASH_SCREEN */

#define CONFIG_LCD
#define CONFIG_VIDEO_MVF_DCU
#define LCD_TWR_RGB
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
#define CONFIG_SPLASH_SCREEN_ALIGN
#define CONFIG_SETUP_VIDEOLFB_TAG
#define CONFIG_SETUP_MTDSPLASHPART_TAG

#define CONFIG_BMP
#define CONFIG_CMD_BMP
#define CONFIG_BMP_24BPP
#define LCD_BPP	LCD_COLOR32

#define CONFIG_MTD_SPLASH_PART_START	0x120000
#define CONFIG_MTD_SPLASH_PART_LEN	0x180000

#include "vybrid_lcd_config.h"

#define DTB_PART_SIZE			0x20000
#define KERNEL_PART_SIZE		0xc00000
#define ROOTFS_PART_SIZE		0xa000000

# define SPLASH_FLASH_BASE		0x120000 /* (CONFIG_MTD_SPLASH_PART_START) */
# define DTB_FLASH_BASE			0x2a0000 /* (SPLASH_FLASH_BASE + CONFIG_MTD_SPLASH_PART_LEN) */
# define KERNEL_FLASH_BASE		0x2c0000 /* (DTB_FLASH_BASE + DTB_PART_SIZE) */
# define ROOTFS_FLASH_BASE		0xec0000 /* (KERNEL_FLASH_BASE + KERNEL_PART_SIZE) */

#define CONFIG_HOSTNAME		vf6-som
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"autoload=yes\0"						\
	"addip=setenv bootargs ${bootargs}"				\
	" ip=${ipaddr}:${serverip}:${gatewayip}:"			\
	"${netmask}:${hostname}:eth0:off "				\
	" fec_mac=${ethaddr}\0"						\
	"ethaddr=3C:FB:96:77:88:AB\0"					\
	"ipaddr=172.17.80.3\0"						\
	"serverip=172.17.0.1\0"						\
	"image=vf6-som/rootfs.uImage\0"					\
	"bootcmd=run reliableboot\0"					\
	"reliableboot=run nandboot\0"					\
	"netboot=tftp ${image} && run args addip && run boot_dtb\0"	\
	"nandboot=nboot ${loadaddr} 0 ${uImage_offset}"			\
	" && run args addip && run boot_dtb\0"				\
	"args=run args_verbose\0"					\
	"args_common=console=ttymxc0,115200\0"				\
	"args_quiet=setenv bootargs ${args_common} ${ubirfs}"		\
	" quiet=quiet\0"						\
	"args_verbose=setenv bootargs ${args_common} ${ubirfs}"		\
	" ignore_loglevel\0"						\
	"ubirfs=rootwait=1 rw ubi.mtd=4,2048 rootfstype=ubifs"		\
	" root=ubi0:rootfs ubi.fm_autoconvert=1\0"			\
	"args_nfs=setenv bootargs ${args_common} ignore_loglevel"	\
	" root=/dev/nfs"						\
	" nfsroot=${serverip}:/mnt/nfs/vf6-som/rootfs,v3,nolock\0"	\
	"verify=no\0"							\
	"bootdelay=1\0"							\
	"splashimage=0x80007fc0\0"					\
	"splashfile=vf6-som/boot_screen.bmp\0"				\
	"splashpos=m,m\0"						\
	"splashupdate=tftp ${splashfile} && nand erase.spread "		\
	"${splash_offset} ${filesize} && nand write ${loadaddr} "	\
	"${splash_offset} ${filesize}\0"				\
	"update=tftp ${image} && nand erase.spread "			\
	"${uImage_offset} ${filesize} && nand write ${loadaddr} "	\
	"${uImage_offset} ${filesize}\0"				\
	"rootfsimage=rootfs.ubi\0"					\
	"rootfsupdate=tftp ${rootfsimage} && nand erase.spread "	\
	"${rootfs_offset} " stringify(ROOTFS_PART_SIZE)			\
	" && nand write ${loadaddr} ${rootfs_offset} ${filesize}\0"	\
	"dtbimage=vf6-som/rootfs.dtb\0"					\
	"dtbupdate=tftp ${dtbimage} && nand erase.spread "		\
	"${dtb_offset} ${filesize} && nand write ${loadaddr} "		\
	"${dtb_offset} ${filesize}\0"					\
	"adjust_dtb=fdt addr ${dtb_addr}"				\
	" && fdt set /soc/aips-bus@40080000/nand@400e0000/nand@0"	\
	" nand-bus-width ${nand_bus_width}\0"				\
	"boot_dtb=nand read ${dtb_addr} ${dtb_offset} 0x7000"		\
	" && run adjust_dtb"						\
	" && bootm ${loadaddr} - ${dtb_addr}\0"				\
	"splash_offset=" stringify(SPLASH_FLASH_BASE) "\0"		\
	"dtb_offset=" stringify(DTB_FLASH_BASE) "\0"			\
	"uImage_offset=" stringify(KERNEL_FLASH_BASE) "\0"		\
	"rootfs_offset=" stringify(ROOTFS_FLASH_BASE) "\0"		\
	"dtb_addr=0x80000100\0"						\
	"uboot_image=vf6-som/u-boot.nand\0"				\
	"update_uboot=tftpboot ${uboot_image} && "			\
	" nand erase 0 0x60000 && "					\
	" nand write ${loadaddr} 0 ${filesize}\0"

/*
 * Enable support for booting with FDT
 */
#define CONFIG_OF_LIBFDT
/*
 * QSPI Flash is not supported with FDT yet
 */
#undef CONFIG_CMD_QSF

#endif /* __CONFIG_H */
