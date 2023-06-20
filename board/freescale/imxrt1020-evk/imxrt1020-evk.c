// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020
 * Author(s): Giulio Benetti <giulio.benetti@benettiengineering.com>
 * Copyright (C) 2023 Emcraft Systems
 * Author(s): Vladimir Skvortsov <vskvortsov@emcraft.com>
 */

#include <dm.h>
#include <init.h>
#include <log.h>
#include <ram.h>
#include <spl.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/armv7m.h>
#include <serial.h>
#include <phy.h>

DECLARE_GLOBAL_DATA_PTR;

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

#ifdef CONFIG_XPL_BUILD
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

#if defined DMAMEM_BASE
ulong board_get_usable_ram_top(ulong total_size)
{
	/* relocate U-Boot before the uncached DMA area, which is reserved in the end of SDRAM */
	return DMAMEM_BASE;
}
#endif

#if CONFIG_IS_ENABLED(FEC_MXC)
#define MII_PHY_CTRL2	0x1f
#define MII_PHY_CTRL2_RMII_CLK_50MHZ	(1 << 7)
#define MII_PHY_CTRL2_RMII_LED_MODE	(1 << 4)

#define IOMUXC_GPR_BASE 0x400AC000
#define IOMUXC_GPR1_BASE (IOMUXC_GPR_BASE + 0x4)

/* IOMUXC GPR1 bits */
#define ENET_REF_CLK_DIR (1 << 17)

static void enable_enet_refclk_out(void)
{
	u32 reg;
	reg = readl(IOMUXC_GPR1_BASE);
	reg |= ENET_REF_CLK_DIR; /* 50M ENET_REF_CLOCK output to PHY. */
	writel(reg, IOMUXC_GPR1_BASE);
}

int board_phy_config(struct phy_device *phydev)
{
	enable_enet_refclk_out();
	phy_write(phydev, MDIO_DEVAD_NONE, MII_PHY_CTRL2,
		phy_read(phydev, MDIO_DEVAD_NONE, MII_PHY_CTRL2) |
			MII_PHY_CTRL2_RMII_CLK_50MHZ |
			MII_PHY_CTRL2_RMII_LED_MODE);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR,
		phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR) & ~BMCR_ISOLATE);
	return 0;
}

#endif /* FEC_MXC */
