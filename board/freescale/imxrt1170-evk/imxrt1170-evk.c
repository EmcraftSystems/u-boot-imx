// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2019
 * Author(s): Giulio Benetti <giulio.benetti@benettiengineering.com>
 * Copyright (C) 2023 Emcraft Systems
 * Author(s): Vladimir Skvortsov <vskvortsov@emcraft.com>
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
#include <phy.h>
#include <linux/delay.h>
#include <clk.h>
#include <dt-bindings/clock/imxrt1170-clock.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_BOARD_EARLY_INIT_F)

#if CONFIG_IS_ENABLED(FEC_MXC)

#define MII_PHY_CTRL2	0x1f
#define MII_PHY_CTRL2_RMII_CLK_50MHZ	(1 << 7)
#define MII_PHY_CTRL2_RMII_LED_MODE	(1 << 4)

int board_phy_config(struct phy_device *phydev)
{
	phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR,
		  BMCR_RESET);

	phy_write(phydev, MDIO_DEVAD_NONE, MII_PHY_CTRL2,
		  phy_read(phydev, MDIO_DEVAD_NONE, MII_PHY_CTRL2) |
		  MII_PHY_CTRL2_RMII_CLK_50MHZ |
		  MII_PHY_CTRL2_RMII_LED_MODE);

	phy_write(phydev, MDIO_DEVAD_NONE, MII_ADVERTISE,
		  (ADVERTISE_100FULL | ADVERTISE_100HALF |
		   ADVERTISE_10FULL | ADVERTISE_10HALF | ADVERTISE_CSMA));

	phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR,
		  (BMCR_ANRESTART | BMCR_ANENABLE));

	return 0;
}

#define IOMUXC_GPR_BASE 0x400E4000
#define IOMUXC_GPR4_BASE (IOMUXC_GPR_BASE + 0x10)

/* IOMUXC GPR4 bits */
#define ENET_TX_CLK_SEL (1 << 0)
#define ENET_REF_CLK_DIR (1 << 1)

void enable_enet_refclk_out(void)
{
	u32 reg;
	reg = readl(IOMUXC_GPR4_BASE);
	reg |= ENET_REF_CLK_DIR; /* 50M ENET_REF_CLOCK output to PHY. */
	writel(reg, IOMUXC_GPR4_BASE);
}

#define CCM_MISC_BASE 0x40C84000

#define PLL_AI_CTRL0_REG	0x0
#define PLL_AI_CTRL0_SET_REG	0x4
#define PLL_AI_CTRL0_CLR_REG	0x8
#define PLL_AI_CTRL1_REG    	0x10
#define PLL_AI_CTRL1_SET_REG	0x14
#define PLL_AI_CTRL1_CLR_REG	0x18
#define PLL_AI_CTRL2_REG    	0x20
#define PLL_AI_CTRL2_SET_REG	0x24
#define PLL_AI_CTRL2_CLR_REG	0x28
#define PLL_AI_CTRL3_REG    	0x30
#define PLL_AI_CTRL3_SET_REG	0x34
#define PLL_AI_CTRL3_CLR_REG	0x38

#define PLL_AI_CTRL0_HOLD_RING_OFF_MASK (1 << 13)
#define PLL_AI_CTRL0_POWER_UP_MASK (1 << 14)
#define PLL_AI_CTRL0_ENABLE_MASK (1 << 15)
#define PLL_AI_CTRL0_PLL_REG_EN_MASK (1 << 22)

/* Analog IP */
#define VDDSOC2PLL_AI_1G_BASE (CCM_MISC_BASE + 0x850)
#define PLL1_AI_BASE VDDSOC2PLL_AI_1G_BASE

#define AI_CTRL_REG(base) (base)
#define AI_WDATA_REG(base) (base + 0x10)
#define AI_RDATA_REG(base) (base + 0x20)

#define AI_TOGGLE_BIT (1 << 8)
#define AI_RWB_BIT (1 << 20)
#define AI_TOGGLE_DONE_BIT (1 << 9)
#define AI_ADDR_MASK	0xff

static void ai_write(u32 base, u32 reg, u32 wdata)
{
	u32 ctrl = readl(AI_CTRL_REG(base));
	uint32_t pre_toggle_done, toggle_done;
	pre_toggle_done = ctrl & AI_TOGGLE_DONE_BIT;

	ctrl &= ~(AI_RWB_BIT | AI_ADDR_MASK);
	ctrl |= (reg & 0xff);
	writel(ctrl, AI_CTRL_REG(base));

	writel(wdata, AI_WDATA_REG(base));
	writel(ctrl ^ AI_TOGGLE_BIT, AI_CTRL_REG(base));
	do {
		ctrl = readl(AI_CTRL_REG(base));
		toggle_done = ctrl & AI_TOGGLE_DONE_BIT;
	} while (toggle_done == pre_toggle_done);
}

static u32 ai_read(u32 base, u32 reg)
{
	u32 ctrl = readl(AI_CTRL_REG(base));
	uint32_t pre_toggle_done, toggle_done;
	pre_toggle_done = ctrl & AI_TOGGLE_DONE_BIT;

	ctrl |= AI_RWB_BIT;
	ctrl &= ~AI_ADDR_MASK;
	ctrl |= (reg & 0xff);

	writel(ctrl ^ AI_TOGGLE_BIT, AI_CTRL_REG(base));

	do {
		ctrl = readl(AI_CTRL_REG(base));
		toggle_done = ctrl & AI_TOGGLE_DONE_BIT;
	} while (toggle_done == pre_toggle_done);

	return readl(AI_RDATA_REG(base));
}

#define PLL1_DIV 41
#define PLL1_NUMER 178956970
#define PLL1_DENOM 0x0FFFFFFF

#define PLL_REGS_BASE 0x40c84000
#define SYS_PLL1_CTRL (PLL_REGS_BASE + 0x2c0)
#define SYS_PLL1_STABLE (1 << 29)


static void imxrt1170_pll1_init(void)
{
    u32 reg;

    /* configure pll */
    ai_write(PLL1_AI_BASE, PLL_AI_CTRL0_CLR_REG, PLL_AI_CTRL0_POWER_UP_MASK);

    ai_write(PLL1_AI_BASE, PLL_AI_CTRL3_REG, PLL1_DENOM);
    ai_write(PLL1_AI_BASE, PLL_AI_CTRL2_REG, PLL1_NUMER);

    reg = ai_read(PLL1_AI_BASE, PLL_AI_CTRL0_REG);
    reg = (reg & (~0x7f)) | (PLL1_DIV & 0x7f);
    ai_write(PLL1_AI_BASE, PLL_AI_CTRL0_REG, reg);

    ai_write(PLL1_AI_BASE, PLL_AI_CTRL0_SET_REG, PLL_AI_CTRL0_PLL_REG_EN_MASK);

    udelay(100);

    ai_write(PLL1_AI_BASE, PLL_AI_CTRL0_SET_REG, PLL_AI_CTRL0_POWER_UP_MASK | PLL_AI_CTRL0_HOLD_RING_OFF_MASK);

    /* toggle hold ring off */
    ai_write(PLL1_AI_BASE, PLL_AI_CTRL0_SET_REG, PLL_AI_CTRL0_HOLD_RING_OFF_MASK);
    udelay(255);
    ai_write(PLL1_AI_BASE, PLL_AI_CTRL0_CLR_REG, PLL_AI_CTRL0_HOLD_RING_OFF_MASK);

    while ((readl( + SYS_PLL1_CTRL) & SYS_PLL1_STABLE) == 0) {
	    /* wait for PLL stable */
    }

    /* enabled clock */
    ai_write(PLL1_AI_BASE, PLL_AI_CTRL0_SET_REG, PLL_AI_CTRL0_ENABLE_MASK);
}

int board_eth_init(void)
{
	imxrt1170_pll1_init();

	enable_enet_refclk_out();

	return 0;
}

#endif /* CONFIG_IS_ENABLED(FEC_MXC) */

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
		{ 0x16C, 0x3B0, 0x0, 0x6 }, /* PAD_GPIO_AD_24_LPUART1_TXD */
		{ 0x170, 0x3B4, 0x0, 0x6 }, /* PAD_GPIO_AD_25_LPUART1_RXD */
	};
	int i;

	/* Configure IOMUX */
	for (i = 0; i < ARRAY_SIZE(lpuart1_pins); i++) {
		writel(lpuart1_pins[i].mux_mode, IOMUXC_BASE + lpuart1_pins[i].mux_reg);
		writel(lpuart1_pins[i].conf_val, IOMUXC_BASE + lpuart1_pins[i].conf_reg);
	}

#if CONFIG_IS_ENABLED(FEC_MXC)
	board_eth_init();
#endif

	return 0;
}

#endif


#if defined(CONFIG_BOARD_EARLY_INIT_R)

int board_early_init_r(void)
{
	/* Switch M7 CPU core to 986MHz from ARM_PLL */
	struct clk *clk, *clk1;

	clk_get_by_id(IMXRT1170_CLK_PLL_ARM_OUT, &clk);
	clk_enable(clk);
	clk_get_by_id(IMXRT1170_CLK_ROOT_M7, &clk1);
	clk_set_parent(clk1, clk);

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

void spl_board_init(void)
{
	preloader_console_init();
	arch_cpu_init(); /* to configure mpu for sdram rw permissions */
}

u32 spl_boot_device(void)
{
#ifdef CONFIG_FSPI_BOOT
	return BOOT_DEVICE_NOR;
#endif
	return BOOT_DEVICE_MMC1;
}
#endif

int board_init(void)
{
	gd->bd->bi_boot_params = gd->bd->bi_dram[0].start + 0x100;

	return 0;
}
