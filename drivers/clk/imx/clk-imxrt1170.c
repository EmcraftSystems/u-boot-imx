// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022
 * Author(s): Jesse Taube <Mr.Bossman075@gmail.com>
 * Copyright (C) 2023 Emcraft Systems
 * Author(s): Vladimir Skvortsov <vskvortsov@emcraft.com>
 */

#include <common.h>
#include <clk.h>
#include <clk-uclass.h>
#include <dm.h>
#include <log.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <dt-bindings/clock/imxrt1170-clock.h>

#include "clk.h"

static ulong imxrt1170_clk_get_rate(struct clk *clk)
{
	struct clk *c;
	int ret;

	debug("%s(#%lu)\n", __func__, clk->id);

	ret = clk_get_by_id(clk->id, &c);
	if (ret)
		return ret;

	return clk_get_rate(c);
}

static ulong imxrt1170_clk_set_rate(struct clk *clk, ulong rate)
{
	struct clk *c;
	int ret;

	debug("%s(#%lu), rate: %lu\n", __func__, clk->id, rate);

	ret = clk_get_by_id(clk->id, &c);
	if (ret)
		return ret;

	return clk_set_rate(c, rate);
}

static int __imxrt1170_clk_enable(struct clk *clk, bool enable)
{
	struct clk *c;
	int ret;

	debug("%s(#%lu) en: %d\n", __func__, clk->id, enable);

	ret = clk_get_by_id(clk->id, &c);
	if (ret)
		return ret;

	if (enable)
		ret = clk_enable(c);
	else
		ret = clk_disable(c);

	return ret;
}

static int imxrt1170_clk_disable(struct clk *clk)
{
	return __imxrt1170_clk_enable(clk, 0);
}

static int imxrt1170_clk_enable(struct clk *clk)
{
	return __imxrt1170_clk_enable(clk, 1);
}

static int imxrt1170_clk_set_parent(struct clk *clk, struct clk *parent)
{
	struct clk *c, *cp;
	int ret;

	debug("%s(#%lu), parent: %lu\n", __func__, clk->id, parent->id);

	ret = clk_get_by_id(clk->id, &c);
	if (ret)
		return ret;

	ret = clk_get_by_id(parent->id, &cp);
	if (ret)
		return ret;

	return clk_set_parent(c, cp);
}

static struct clk_ops imxrt1170_clk_ops = {
	.set_rate = imxrt1170_clk_set_rate,
	.get_rate = imxrt1170_clk_get_rate,
	.enable = imxrt1170_clk_enable,
	.disable = imxrt1170_clk_disable,
	.set_parent = imxrt1170_clk_set_parent,
};

#define IMXRT1170_CLK_SRC_COMMON "rcosc48M_div2", "osc", "rcosc400M", "rcosc16M"

static const char * const m7_sels[] = {IMXRT1170_CLK_SRC_COMMON,
"pll_arm_out", "pll1_sys", "pll3_sys", "video_pll"};
static const char * const bus_sels[] = {IMXRT1170_CLK_SRC_COMMON,
"pll3_sys", "pll1_div5", "pll2_sys", "pll2_pfd3"};
static const char * const lpuart1_sels[] = {IMXRT1170_CLK_SRC_COMMON,
"pll3_div2", "pll1_div5", "pll2_sys", "pll2_pfd3"};
static const char * const gpt1_sels[] = {IMXRT1170_CLK_SRC_COMMON,
"pll3_div2", "pll1_div5", "pll3_pfd2", "pll3_pfd3"};
static const char * const usdhc1_sels[] = {IMXRT1170_CLK_SRC_COMMON,
"pll2_pfd2", "pll2_pfd0", "pll1_div5", "pll_arm"};
static const char * const semc_sels[] = {IMXRT1170_CLK_SRC_COMMON,
"pll1_div5", "pll2_sys", "pll2_pfd1", "pll3_pfd0"};
static const char * const enet1_sels[] = {IMXRT1170_CLK_SRC_COMMON,
"pll1_div2", "audio_pll", "pll1_div5", "pll2_pfd1"};


struct clk *imxrt1170_clk_composite(const char *name, const char * const *parent_names,
				    int num_parents, void __iomem *reg, unsigned long flags)
{
	struct clk *clk = ERR_PTR(-ENOMEM);
	struct clk_divider *div = NULL;
	struct clk_gate *gate = NULL;
	struct clk_mux *mux = NULL;

	mux = kzalloc(sizeof(*mux), GFP_KERNEL);
	if (!mux)
		goto fail;

	mux->reg = reg;
	mux->shift = 8;
	mux->mask = 3;
	mux->num_parents = num_parents;
	mux->flags = flags;
	mux->parent_names = parent_names;

	div = kzalloc(sizeof(*div), GFP_KERNEL);
	if (!div)
		goto fail;

	div->reg = reg;
	div->shift = 0;
	div->width = 8;
	div->flags = CLK_DIVIDER_ROUND_CLOSEST | flags;

	gate = kzalloc(sizeof(*gate), GFP_KERNEL);
	if (!gate)
		goto fail;

	gate->reg = reg;
	gate->bit_idx = 24;
	gate->flags = CLK_GATE_SET_TO_DISABLE | flags;

	clk = clk_register_composite(NULL, name, parent_names, num_parents,
				     &mux->clk, &clk_mux_ops,
				     &div->clk, &clk_divider_ops,
				     &gate->clk, &clk_gate_ops, flags);
	if (IS_ERR(clk))
		goto fail;

	return clk;

fail:
	kfree(gate);
	kfree(div);
	kfree(mux);
	return ERR_CAST(clk);
}

extern const struct clk_ops ccf_clk_fixed_factor_ops;

struct clk *imxrt1170_clk_pll_div_out_composite(const char *name, const char *parent_name,
						void __iomem *reg, int div_factor, int gate_bit, unsigned long flags)
{
	struct clk *clk = ERR_PTR(-ENOMEM);
	struct clk_fixed_factor *div = NULL;
	struct clk_gate *gate = NULL;

	div = kzalloc(sizeof(*div), GFP_KERNEL);
	if (!div)
		goto fail;

	div->mult = 1;
	div->div = div_factor;
	div->div = 2;

	gate = kzalloc(sizeof(*gate), GFP_KERNEL);
	if (!gate)
		goto fail;

	gate->reg = reg;
	gate->bit_idx = gate_bit;
	gate->flags = flags;

	clk = clk_register_composite(NULL, name, &parent_name, 1,
				     NULL, NULL,
				     &div->clk, &ccf_clk_fixed_factor_ops,
				     &gate->clk, &clk_gate_ops, flags);
	if (IS_ERR(clk))
		goto fail;

	return clk;

fail:
	kfree(gate);
	kfree(div);
	return ERR_CAST(clk);
}


struct imxrt1170_clk_root {
	u32 clk_id;
	char *name;
	const char * const *parent_names;
	u32 off;
	unsigned long flags;
};

static struct imxrt1170_clk_root clk_roots[] = {
	{ IMXRT1170_CLK_ROOT_M7, "m7_root", m7_sels, 0, CLK_IS_CRITICAL },
	{ IMXRT1170_CLK_ROOT_BUS, "bus_root", bus_sels, (2 * 0x80), },
        { IMXRT1170_CLK_ROOT_SEMC, "semc_root", semc_sels, (4 * 0x80), },
	{ IMXRT1170_CLK_ROOT_GPT1, "gpt1_root", gpt1_sels, (14 * 0x80), },
	{ IMXRT1170_CLK_ROOT_LPUART1, "lpuart1_root", lpuart1_sels, (25 * 0x80), },
	{ IMXRT1170_CLK_ROOT_ENET1, "enet1_root", enet1_sels, (51 * 0x80), },
	{ IMXRT1170_CLK_ROOT_USDHC1, "usdhc1_root", usdhc1_sels, (58 * 0x80), },
};

struct imxrt1170_clk_ccgr {
	u32 clk_id;
	char *name;
	char *parent_names;
	u32 off;
	unsigned long flags;
};

static struct imxrt1170_clk_ccgr clk_ccgrs[] = {
	{ IMXRT1170_CLK_M7, "m7", "m7_root", 0x6000, CLK_IS_CRITICAL },
        { IMXRT1170_CLK_SEMC, "semc", "semc_root", (0x6000 + (33 * 0x20)), },
        { IMXRT1170_CLK_GPT1, "gpt1", "gpt1_root", (0x6000 + (64 * 0x20)), },
	{ IMXRT1170_CLK_LPUART1, "lpuart1", "lpuart1_root", (0x6000 + (86 * 0x20)), },
	{ IMXRT1170_CLK_ENET1, "enet1", "enet1_root", (0x6000 + (112 * 0x20)), },
	{ IMXRT1170_CLK_USDHC1, "usdhc1", "usdhc1_root", (0x6000 + (117 * 0x20)), },
};


static int imxrt1170_clk_probe(struct udevice *dev)
{
	void *base;
	struct imxrt1170_clk_root *root;
	struct imxrt1170_clk_ccgr *ccgr;
	struct clk *clk;

	/* Anatop clocks */
	base = (void *)ofnode_get_addr(ofnode_by_compatible(ofnode_null(), "fsl,imxrt-anatop"));

	clk_dm(IMXRT1170_CLK_RCOSC_48M,
	       imx_clk_fixed_factor("rcosc48M", "rcosc16M", 3, 1));
	clk_dm(IMXRT1170_CLK_RCOSC_400M,
	       imx_clk_fixed_factor("rcosc400M",  "rcosc16M", 25, 1));
	clk_dm(IMXRT1170_CLK_RCOSC_48M_DIV2,
	       imx_clk_fixed_factor("rcosc48M_div2",  "rcosc48M", 1, 2));

	clk_dm(IMXRT1170_CLK_PLL_ARM,
	       imx_clk_pll_arm_rt1170("pll_arm", "osc", base + 0x200));
	clk_dm(IMXRT1170_CLK_PLL_ARM_OUT,
	       imx_clk_gate_dis("pll_arm_out", "pll_arm", base + 0x200, 30));

	clk_dm(IMXRT1170_CLK_PLL3,
	       imx_clk_pllv3(IMX_PLLV3_GENERICV2, "pll3_sys", "osc",
			     base + 0x210, 1));
	clk_dm(IMXRT1170_CLK_PLL2,
	       imx_clk_pllv3(IMX_PLLV3_GENERICV2, "pll2_sys", "osc",
			     base + 0x240, 1));
	clk_dm(IMXRT1170_CLK_PLL1,
	       imx_clk_pllv3(IMX_PLLV3_ENET_1G, "pll1_sys", "osc",
			     base + 0x2c0, 1));
	clk_dm(IMXRT1170_CLK_PLL1_OUT,
	       imx_clk_gate_dis("pll1_out", "pll1_sys", base + 0x2c0, 14));

	clk_dm(IMXRT1170_CLK_PLL3_PFD0,
	       imx_clk_pfd("pll3_pfd0", "pll3_sys", base + 0x230, 0));
	clk_dm(IMXRT1170_CLK_PLL3_PFD1,
	       imx_clk_pfd("pll3_pfd1", "pll3_sys", base + 0x230, 1));
	clk_dm(IMXRT1170_CLK_PLL3_PFD2,
	       imx_clk_pfd("pll3_pfd2", "pll3_sys", base + 0x230, 2));
	clk_dm(IMXRT1170_CLK_PLL3_PFD3,
	       imx_clk_pfd("pll3_pfd3", "pll3_sys", base + 0x230, 3));

	clk_dm(IMXRT1170_CLK_PLL2_PFD0,
	       imx_clk_pfd("pll2_pfd0", "pll2_sys", base + 0x270, 0));
	clk_dm(IMXRT1170_CLK_PLL2_PFD1,
	       imx_clk_pfd("pll2_pfd1", "pll2_sys", base + 0x270, 1));
	clk_dm(IMXRT1170_CLK_PLL2_PFD2,
	       imx_clk_pfd("pll2_pfd2", "pll2_sys", base + 0x270, 2));
	clk_dm(IMXRT1170_CLK_PLL2_PFD3,
	       imx_clk_pfd("pll2_pfd3", "pll2_sys", base + 0x270, 3));

	clk_dm(IMXRT1170_CLK_PLL3_DIV2,
	       imxrt1170_clk_pll_div_out_composite("pll3_div2", "pll3_sys", base + 0x210, 2, 3, 0));

	clk_dm(IMXRT1170_CLK_PLL1_DIV2,
	       imxrt1170_clk_pll_div_out_composite("pll1_div2", "pll1_out", base + 0x2c0, 2, 25, 0));
	clk_dm(IMXRT1170_CLK_PLL1_DIV5,
	       imxrt1170_clk_pll_div_out_composite("pll1_div5", "pll1_out", base + 0x2c0, 5, 26, 0));

	/* CCM clocks */
	base = dev_read_addr_ptr(dev);
	if (base == (void *)FDT_ADDR_T_NONE)
		return -EINVAL;


	for (int i = 0; i < ARRAY_SIZE(clk_roots); i++) {
		root = &clk_roots[i];
		clk = imxrt1170_clk_composite(root->name, root->parent_names, 8,
					      base + root->off, root->flags);
		clk_dm(root->clk_id, clk);
	}

	for (int i = 0; i < ARRAY_SIZE(clk_ccgrs); i++) {
		ccgr = &clk_ccgrs[i];
		clk = imx_clk_gate_flags(ccgr->name, ccgr->parent_names,
					 base + ccgr->off, 0, ccgr->flags);
		clk_dm(ccgr->clk_id, clk);
	}

	return 0;
}

static const struct udevice_id imxrt1170_clk_ids[] = {
	{ .compatible = "fsl,imxrt1170-ccm" },
	{ },
};

U_BOOT_DRIVER(imxrt1170_clk) = {
	.name = "clk_imxrt1170",
	.id = UCLASS_CLK,
	.of_match = imxrt1170_clk_ids,
	.ops = &imxrt1170_clk_ops,
	.probe = imxrt1170_clk_probe,
	.flags = DM_FLAG_PRE_RELOC,
};
