// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2019
 * Author(s): Giulio Benetti <giulio.benetti@benettiengineering.com>
 * Copyright (C) 2023 Emcraft Systems
 */

#include <common.h>
#include <dm.h>
#include <init.h>
#include <log.h>
#include <ram.h>
#include <spl.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/armv7m.h>
#include <serial.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_BOARD_EARLY_INIT_F)

#define IOMUXC_BASE (0x400e8000)

int board_early_init_f(void)
{
	/* Configure UART pins to enable output to the console at very beggining
	   before generic driver is initialized with the settings from DTS */
	struct {
		u32	mux_reg;
		u32	conf_reg;
		u32	mux_mode;
		u32	conf_val;
	} lpuart1_pins[] = {
		{ 0x16C, 0x3B0, 0x0, 0xf1 }, /* PAD_GPIO_AD_24_LPUART1_TXD */
		{ 0x170, 0x3B4, 0x0, 0xf1 }, /* PAD_GPIO_AD_25_LPUART1_RXD */
	};
	int i;

	/* Configure IOMUX */
	for (i = 0; i < ARRAY_SIZE(lpuart1_pins); i++) {
		writel(lpuart1_pins[i].mux_mode, IOMUXC_BASE + lpuart1_pins[i].mux_reg);
		writel(lpuart1_pins[i].conf_val, IOMUXC_BASE + lpuart1_pins[i].conf_reg);
	}
	return 0;
}

#endif


#if defined(CONFIG_BOARD_EARLY_INIT_R)

int board_early_init_r(void)
{
	int rv;
	struct udevice *dev;

	rv = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (rv) {
		debug("DRAM init failed: %d\n", rv);
	}
	return rv;
}


#endif

int dram_init(void)
{
#ifndef CONFIG_SUPPORT_SPL
	int rv;
	struct udevice *dev;

	rv = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (rv) {
		debug("DRAM init failed: %d\n", rv);
		return rv;
	}

#endif
	return fdtdec_setup_mem_size_base();
}

int dram_init_banksize(void)
{
	return fdtdec_setup_memory_banksize();
}

#ifdef CONFIG_SPL_BUILD
#ifdef CONFIG_SPL_OS_BOOT
int spl_start_uboot(void)
{
	debug("SPL: booting kernel\n");
	/* break into full u-boot on 'c' */
	return serial_tstc() && serial_getc() == 'c';
}
#endif

int spl_dram_init(void)
{
	struct udevice *dev;
	int rv;

	rv = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (rv)
		debug("DRAM init failed: %d\n", rv);
	return rv;
}

void spl_board_init(void)
{
	preloader_console_init();
	spl_dram_init();
	arch_cpu_init(); /* to configure mpu for sdram rw permissions */
}

u32 spl_boot_device(void)
{
	return BOOT_DEVICE_MMC1;
}
#endif

int board_init(void)
{
	gd->bd->bi_boot_params = gd->bd->bi_dram[0].start + 0x100;

	return 0;
}
