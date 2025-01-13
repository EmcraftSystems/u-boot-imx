// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(C) 2023, Emcraft Systems
 * Author(s): Vladimir Skvortsov <vskvortsov@emcraft.com>
 */

#include <common.h>
#include <asm/io.h>
#include <malloc.h>
#include <clk-uclass.h>
#include <dm/device.h>
#include <dm/devres.h>
#include <linux/bitops.h>
#include <linux/clk-provider.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/iopoll.h>
#include <clk.h>
#include <div64.h>

#include "clk.h"

#define UBOOT_DM_CLK_IMX_PLLARM_RT1170 "imxrt1170_clk_pllarm"

#define DIV_SHIFT	0x0
#define DIV_MASK	0xff
#define PWRUP_MASK	BIT(13)
#define CLKE_MASK	BIT(14)
#define PDIV_SHIFT	15
#define PDIV_MASK	GENMASK(16, 15)
#define STABLE_MASK	BIT(29)

#define LOCK_TIMEOUT_US		50

static const unsigned int pdiv_table[] = { 2, 4, 8, 1 };

struct clk_pllarm {
	struct clk	clk;
	void __iomem	*base;
	u32		powerup_mask;
	u32		enable_mask;
	u32		stable_mask;
	u32		div_mask;
	u32		div_shift;
	u32		pdiv_mask;
	u32		pdiv_shift;
};

#define to_clk_pllarm(_clk) container_of(_clk, struct clk_pllarm, clk)

static ulong clk_pllarm_get_rate(struct clk *clk)
{
	struct clk_pllarm *pll = to_clk_pllarm(dev_get_clk_ptr(clk->dev));
	unsigned long parent_rate = clk_get_parent_rate(clk);
	u32 pll_ctrl = readl(pll->base);

	u32 div = (pll_ctrl & pll->div_mask) >> pll->div_shift;
	u32 pdiv_idx = (pll_ctrl & pll->pdiv_mask) >> pll->pdiv_shift;

	if (pdiv_idx >= ARRAY_SIZE(pdiv_table)) {
		return 0;
	}

	return (parent_rate * (div / 2)) / pdiv_table[pdiv_idx];
}

static ulong clk_pllarm_set_rate(struct clk *clk, ulong rate)
{
	struct clk_pllarm *pll = to_clk_pllarm(dev_get_clk_ptr(clk->dev));
	unsigned long parent_rate = clk_get_parent_rate(clk);
	u32 pll_ctrl = readl(pll->base);
	u32 div = (readl(pll->base) >> pll->div_shift) & pll->div_mask;
	int i;
	u32 val;

	if (pll_ctrl && pll->enable_mask) {
		/* do not update rate on the enabled PLL */
		return 0;
	}

	for (i = 0; i < ARRAY_SIZE(pdiv_table); i++) {
		val = parent_rate * (div / pdiv_table[i]);
		if (val == rate) {
			break;
		}
	}

	if (val == rate) {
		clrsetbits_le32(pll->base, pll->pdiv_mask, i << pll->pdiv_shift);
		return clk_pllarm_get_rate(clk);
	}

	return -EINVAL;
}

static int clk_pllarm_enable(struct clk *clk)
{
	struct clk_pllarm *pll = to_clk_pllarm(dev_get_clk_ptr(clk->dev));
	u32 pll_ctrl = readl(pll->base) & ~(STABLE_MASK);

	if (pll_ctrl & pll->enable_mask) {
		/* nothing to do */
		return 0;
	}

	pll_ctrl |= pll->powerup_mask;
	writel(pll_ctrl, pll->base);

	readl_poll_timeout(pll->base, pll_ctrl, pll_ctrl & STABLE_MASK, 0);

	pll_ctrl |= pll->enable_mask;
	writel(pll_ctrl, pll->base);

	return 0;
}

static int clk_pllarm_disable(struct clk *clk)
{
	struct clk_pllarm *pll = to_clk_pllarm(dev_get_clk_ptr(clk->dev));
	u32 pll_ctrl = readl(pll->base) & ~(STABLE_MASK);

	pll_ctrl &= ~(pll->enable_mask | pll->powerup_mask);
	writel(pll_ctrl, pll->base);

	return 0;
}

static const struct clk_ops clk_pllarm_ops = {
	.enable		= clk_pllarm_enable,
	.disable	= clk_pllarm_disable,
	.set_rate	= clk_pllarm_set_rate,
	.get_rate	= clk_pllarm_get_rate,
};

struct clk *imx_clk_pll_arm_rt1170(const char *name,
				   const char *parent_name, void __iomem *base)
{
	struct clk_pllarm *pll;
	struct clk *clk;
	int ret;

	pll = kzalloc(sizeof(*pll), GFP_KERNEL);
	if (!pll)
		return ERR_PTR(-ENOMEM);

	pll->powerup_mask = PWRUP_MASK;
	pll->enable_mask = CLKE_MASK;
	pll->stable_mask = STABLE_MASK;
	pll->div_mask = DIV_MASK;
	pll->div_shift = DIV_SHIFT;
	pll->pdiv_mask = PDIV_MASK;
	pll->pdiv_shift = PDIV_SHIFT;

	pll->base = base;
	clk = &pll->clk;

	ret = clk_register(clk, UBOOT_DM_CLK_IMX_PLLARM_RT1170, name, parent_name);
	if (ret) {
		kfree(pll);
		return ERR_PTR(ret);
	}

	return clk;
}

U_BOOT_DRIVER(clk_pllarm_imxrt1170) = {
	.name	= UBOOT_DM_CLK_IMX_PLLARM_RT1170,
	.id	= UCLASS_CLK,
	.ops	= &clk_pllarm_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
