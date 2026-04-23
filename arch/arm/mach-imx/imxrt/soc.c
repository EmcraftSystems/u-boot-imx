// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2019
 * Author(s): Giulio Benetti <giulio.benetti@benettiengineering.com>
 */

#include <init.h>
#include <asm/io.h>
#include <asm/armv7_mpu.h>
#include <asm/mach-imx/sys_proto.h>
#include <dm.h>
#include <dm/uclass.h>
#include <dm/device.h>
#include <dm/device-internal.h>
#include <linux/bitops.h>

int arch_cpu_init(void)
{
	int i;

	struct mpu_region_config imxrt_region_config[] = {
		{ 0x00000000, REGION_0, XN_DIS, PRIV_RW_USR_RW,
		  STRONG_ORDER, REGION_4GB },
		{ PHYS_SDRAM, REGION_1, XN_DIS, PRIV_RW_USR_RW,
		  O_I_WB_RD_WR_ALLOC, (ffs(PHYS_SDRAM_SIZE) - 2) },
		{ DMAMEM_BASE,
		  REGION_2, XN_DIS, PRIV_RW_USR_RW,
		  STRONG_ORDER, (ffs(DMAMEM_SZ_ALL) - 2) },
#if defined(FB_RESERVED_BASE)
		{ FB_RESERVED_BASE,
		  REGION_3, XN_DIS, PRIV_RW_USR_RW,
		  STRONG_ORDER, (ffs(FB_RESERVED_SIZE) - 2) },
#endif
	};

	/*
	 * Configure the memory protection unit (MPU) to allow full access to
	 * the whole 4GB address space.
	 */
	disable_mpu();
	for (i = 0; i < ARRAY_SIZE(imxrt_region_config); i++)
		mpu_config(&imxrt_region_config[i]);
	enable_mpu();

	return 0;
}

u32 get_cpu_rev(void)
{
#if defined(CONFIG_IMXRT1020)
	return MXC_CPU_IMXRT1020 << 12;
#elif defined(CONFIG_IMXRT1050)
	return MXC_CPU_IMXRT1050 << 12;
#elif defined(CONFIG_IMXRT1170)
	return MXC_CPU_IMXRT1170 << 12;
#else
#error This IMXRT SoC is not supported
#endif
}

void imx_get_mac_from_fuse(int dev_id, unsigned char *mac)
{
	/* TBD */
	memset(mac, 0, 6);
}

/*
 * Issue the flash's own soft-reset (via spi_nor_remove -> 66h/99h) on
 * every SPI-NOR before CPU reset.  The Winbond w25q512nwq on the
 * 1170-EVKB is left in a non-default mode after probe that the boot
 * ROM cannot read on warm reset unless the flash is returned to
 * power-on defaults first.  Not needed in SPL - do_reset / reset_misc
 * are U-Boot only.
 */
#ifndef CONFIG_XPL_BUILD
void reset_misc(void)
{
	struct uclass *uc;
	struct udevice *dev, *tmp;

	if (uclass_get(UCLASS_SPI_FLASH, &uc))
		return;

	uclass_foreach_dev_safe(dev, tmp, uc)
		device_remove(dev, DM_REMOVE_NORMAL);
}
#endif
