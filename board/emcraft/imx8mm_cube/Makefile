#
# Copyright 2018 NXP
# Copyright 2020 Emcraft Systems
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += imx8mm_cube.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
ifdef CONFIG_IMX8M_2G_LPDDR4
obj-y += lpddr4_timing_2g.o
else
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing.o
obj-$(CONFIG_IMX8M_DDR4) += ddr4_timing.o
endif
endif
