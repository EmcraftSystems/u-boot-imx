/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright(C) 2019
 * Author(s): Giulio Benetti <giulio.benetti@benettiengineering.com>
 */

#ifndef __ASM_ARCH_IMX_REGS_H__
#define __ASM_ARCH_IMX_REGS_H__

#define ARCH_MXC

#define GPIO1_BASE_ADDR		0x401B8000
#define GPIO2_BASE_ADDR		0x401BC000
#define GPIO3_BASE_ADDR		0x401C0000
#define GPIO4_BASE_ADDR		0x401C4000
#define GPIO5_BASE_ADDR		0x400C0000

#define MXS_LCDIF_BASE		0x402b8000

#define FEC_QUIRK_ENET_MAC

#if !(defined(__KERNEL_STRICT_NAMES) || defined(__ASSEMBLY__))
#include <asm/mach-imx/regs-lcdif.h>
#endif

#define USB_BASE_ADDR		0x402E0000
#define USB_PHY0_BASE_ADDR	0x400D9000
#define USB_PHY1_BASE_ADDR	0x400DA000

#if !(defined(__KERNEL_STRICT_NAMES) || defined(__ASSEMBLY__))
#include <asm/types.h>
#include <stdbool.h>

#ifdef CONFIG_IMXRT1170
struct usbphy_regs {
	u32	usbphy_pwd;			/* 0x000 */
	u32	usbphy_pwd_set;			/* 0x004 */
	u32	usbphy_pwd_clr;			/* 0x008 */
	u32	usbphy_pwd_tog;			/* 0x00c */
	u32	usbphy_tx;			/* 0x010 */
	u32	usbphy_tx_set;			/* 0x014 */
	u32	usbphy_tx_clr;			/* 0x018 */
	u32	usbphy_tx_tog;			/* 0x01c */
	u32	usbphy_rx;			/* 0x020 */
	u32	usbphy_rx_set;			/* 0x024 */
	u32	usbphy_rx_clr;			/* 0x028 */
	u32	usbphy_rx_tog;			/* 0x02c */
	u32	usbphy_ctrl;			/* 0x030 */
	u32	usbphy_ctrl_set;		/* 0x034 */
	u32	usbphy_ctrl_clr;		/* 0x038 */
	u32	usbphy_ctrl_tog;		/* 0x03c */
	u32	usbphy_status;			/* 0x040 */
	u32	reserved0[3];
	u32	usbphy_debug0;			/* 0x050 */
	u32	usbphy_debug0_set;		/* 0x054 */
	u32	usbphy_debug0_clr;		/* 0x058 */
	u32	usbphy_debug0_tog;		/* 0x05c */
	u32	reserved1[4];
	u32	usbphy_debug1;			/* 0x070 */
	u32	usbphy_debug1_set;		/* 0x074 */
	u32	usbphy_debug1_clr;		/* 0x078 */
	u32	usbphy_debug1_tog;		/* 0x07c */
	u32	usbphy_version;			/* 0x080 */
	u32	reserved2[7];
	u32	usb1_pll_480_ctrl;		/* 0x0a0 */
	u32	usb1_pll_480_ctrl_set;		/* 0x0a4 */
	u32	usb1_pll_480_ctrl_clr;		/* 0x0a8 */
	u32	usb1_pll_480_ctrl_tog;		/* 0x0ac */
	u32	reserved3[4];
	u32	usb1_vbus_detect;		/* 0xc0 */
	u32	usb1_vbus_detect_set;		/* 0xc4 */
	u32	usb1_vbus_detect_clr;		/* 0xc8 */
	u32	usb1_vbus_detect_tog;		/* 0xcc */
	u32	usb1_vbus_det_stat;		/* 0xd0 */
	u32	reserved4[3];
	u32	usb1_chrg_detect;		/* 0xe0 */
	u32	usb1_chrg_detect_set;		/* 0xe4 */
	u32	usb1_chrg_detect_clr;		/* 0xe8 */
	u32	usb1_chrg_detect_tog;		/* 0xec */
	u32	usb1_chrg_det_stat;		/* 0xf0 */
	u32	reserved5[3];
	u32	usbphy_anactrl;			/* 0x100 */
	u32	usbphy_anactrl_set;		/* 0x104 */
	u32	usbphy_anactrl_clr;		/* 0x108 */
	u32	usbphy_anactrl_tog;		/* 0x10c */
	u32	usb1_loopback;			/* 0x110 */
	u32	usb1_loopback_set;		/* 0x114 */
	u32	usb1_loopback_clr;		/* 0x118 */
	u32	usb1_loopback_tog;		/* 0x11c */
	u32	usb1_loopback_hsfscnt;		/* 0x120 */
	u32	usb1_loopback_hsfscnt_set;	/* 0x124 */
	u32	usb1_loopback_hsfscnt_clr;	/* 0x128 */
	u32	usb1_loopback_hsfscnt_tog;	/* 0x12c */
	u32	usphy_trim_override_en;		/* 0x130 */
	u32	usphy_trim_override_en_set;	/* 0x134 */
	u32	usphy_trim_override_en_clr;	/* 0x138 */
	u32	usphy_trim_override_en_tog;	/* 0x13c */
	u32	usb1_pfda_ctrl1;		/* 0x140 */
	u32	usb1_pfda_ctrl1_set;		/* 0x144 */
	u32	usb1_pfda_ctrl1_clr;		/* 0x148 */
	u32	usb1_pfda_ctrl1_tog;		/* 0x14c */
};
#endif /* CINFIG_IMXRT1170 */
#endif

#endif /* __ASM_ARCH_IMX_REGS_H__ */
