// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2021 NXP
 */
#include <common.h>
#include <command.h>
#include <dm.h>
#include <mapmem.h>
#include <asm/io.h>
#include <spi.h>
#include <spi_flash.h>
#include <dm/device-internal.h>
#include <asm/mach-imx/qspihdr.h>

static struct spi_flash *flash;

#if defined(CONFIG_MX6) || defined(CONFIG_MX7) || defined(CONFIG_ARCH_MX7ULP)
static struct qspi_config_parameter qspi_safe_config = {
	.cs_hold_time		= 3,
	.cs_setup_time		= 3,
	.sflash_A1_size		= 0x4000000,
	.sflash_B1_size		= 0x4000000,
	.sflash_type		= 1,
	.command_seq[0]		= 0x08180403,
	.command_seq[1]		= 0x24001c00,
	.tag			= 0xc0ffee01,
};

static struct header_config *safe_config = (struct header_config *)&qspi_safe_config;
#else
static struct fspi_config_parameter fspi_safe_config = {
	.tag			= 0x42464346,
	.version		= 0x56010000,
	.dataHoldTime		= 0x3,
	.dataSetupTime		= 0x3,
	.deviceType		= 0x1,
	.sflashPadType		= 0x1,
	.serialClkFreq		= 0x2,
	.sflashA1Size		= 0x10000000,
	.lookupTable[0]		= 0x0818040b,
	.lookupTable[1]		= 0x24043008,
};

static struct header_config *safe_config = (struct header_config *)&fspi_safe_config;
#endif

static int qspi_erase_update(struct spi_flash *flash, int off, int len, void *buf)
{
	int size;
	int ret;

	size = ROUND(len, flash->sector_size);
	ret = spi_flash_erase(flash, off, size);
	printf("Erase %#x bytes @ %#x %s\n",
	       size, off, ret ? "ERROR" : "OK");
	if (ret)
		return ret;

	ret = spi_flash_write(flash, off, len, buf);
	printf("Write %#x bytes @ %#x %s\n",
	       len, off, ret ? "ERROR" : "OK");

	return ret;
}

static int do_qspihdr_check(int argc, char * const argv[], int flag)
{
	u32 buf;
	unsigned long addr;
	char *endp;
	void *tmp;
	int ret;

#if defined(CONFIG_MX6) || defined(CONFIG_MX7) || defined(CONFIG_ARCH_MX7ULP)
	int off = QSPI_HDR_OFF + QSPI_HDR_TAG_OFF;
	int tag = QSPI_HDR_TAG;
#else
	int off = FSPI_HDR_OFF + FSPI_HDR_TAG_OFF;
	int tag = FSPI_HDR_TAG;
#endif

	if (argc == 3) {
		/* check data in memory */
		addr = simple_strtoul(argv[2], &endp, 16);

		tmp = map_physmem(addr + off, 4, MAP_WRBACK);
		if (!tmp) {
			printf("Failed to map physical memory\n");
			return 1;
		}

		if (*(u32 *)tmp == tag) {
			if (flag & FLAG_VERBOSE)
				printf("Found boot config header in memory\n");
			unmap_physmem(tmp, 4);
			return 0;
		} else {
			if (flag & FLAG_VERBOSE)
				printf("NO boot config header in memory\n");
			unmap_physmem(tmp, 4);
			return 1;
		}
	} else {
		ret = spi_flash_read(flash, off, 4, &buf);
		if (ret) {
			printf("flash read failed, ret: %d\n", ret);
			return -1;
		}

		if (buf == tag) {
			if (flag & FLAG_VERBOSE)
				printf("Found boot config header in Q(F)SPI\n");
			return 0;
		} else {
			if (flag & FLAG_VERBOSE)
				printf("NO boot config header in Q(F)SPI\n");
			return 1;
		}
	}
}

static void hdr_dump(void *data)
{
#if defined(CONFIG_MX6) || defined(CONFIG_MX7) || defined(CONFIG_ARCH_MX7ULP)
	struct qspi_config_parameter *hdr =
		(struct qspi_config_parameter *)data;
#else
	struct fspi_config_parameter *hdr =
		(struct fspi_config_parameter *)data;
#endif
	int i;

#define PH(mem, cnt) (						\
{								\
	if (cnt > 1) {						\
		int len = strlen(#mem);				\
		char *sub = strchr(#mem, '[');			\
		if (sub)					\
			*sub = '\0';				\
		for (i = 0; i < cnt; ++i)			\
			printf("  %s[%02d%-*s = %08x\n",	\
			       #mem, i, 25 - len, "]",		\
			       (u32)*(&hdr->mem + i));		\
	} else {						\
		printf("  %-25s = %0*x\n",			\
		       #mem, (int)sizeof(hdr->mem), hdr->mem);	\
		}						\
}								\
)

#if defined(CONFIG_MX6) || defined(CONFIG_MX7) || defined(CONFIG_ARCH_MX7ULP)
	PH(dqs_loopback, 1);
	PH(hold_delay, 1);
	PH(hsphs, 1);
	PH(hsdly, 1);
	PH(device_quad_mode_en, 1);
	PH(write_cmd_ipcr, 1);
	PH(write_enable_ipcr, 1);
	PH(cs_hold_time, 1);
	PH(cs_setup_time, 1);
	PH(sflash_A1_size, 1);
	PH(sflash_A2_size, 1);
	PH(sflash_B1_size, 1);
	PH(sflash_B2_size, 1);
	PH(sclk_freq, 1);
	PH(busy_bit_offset, 1);
	PH(busy_bit_polarity, 1);
	PH(sflash_type, 1);
	PH(sflash_port, 1);
	PH(ddr_mode_enable, 1);
	PH(dqs_enable, 1);
	PH(parallel_mode_enable, 1);
	PH(portA_cs1, 1);
	PH(portB_cs1, 1);
	PH(fsphs, 1);
	PH(fsdly, 1);
	PH(ddrsmp, 1);
	PH(command_seq[0], 64);
	PH(read_status_ipcr, 1);
	PH(enable_dqs_phase, 1);
	PH(config_cmds_en, 1);
	PH(config_cmds[0], 4);
	PH(config_cmds_args[0], 4);
	PH(dqs_pad_setting_override, 1);
	PH(sclk_pad_setting_override, 1);
	PH(data_pad_setting_override, 1);
	PH(cs_pad_setting_override, 1);
	PH(dqs_loopback_internal, 1);
	PH(dqs_phase_sel, 1);
	PH(dqs_fa_delay_chain_sel, 1);
	PH(dqs_fb_delay_chain_sel, 1);
	PH(sclk_fa_delay_chain_sel, 1);
	PH(sclk_fb_delay_chain_sel, 1);
	PH(misc_clock_enable, 1);
	PH(tag, 1);
#else
	PH(tag, 1);
	PH(version, 1);
	PH(readSampleClkSrc, 1);
	PH(dataHoldTime, 1);
	PH(dataSetupTime, 1);
	PH(columnAddressWidth, 1);
	PH(deviceModeCfgEnable, 1);
	PH(deviceModeSeq, 1);
	PH(deviceModeArg, 1);
	PH(configCmdEnable, 1);
	PH(configCmdSeqs[0], 4);
	PH(configCmdArgs[0], 4);
	PH(controllerMiscOption, 1);
	PH(deviceType, 1);
	PH(sflashPadType, 1);
	PH(serialClkFreq, 1);
	PH(lutCustomSeqEnable, 1);
	PH(sflashA1Size, 1);
	PH(sflashA2Size, 1);
	PH(sflashB1Size, 1);
	PH(sflashB2Size, 1);
	PH(csPadSettingOverride, 1);
	PH(sclkPadSettingOverride, 1);
	PH(dataPadSettingOverride, 1);
	PH(dqsPadSettingOverride, 1);
	PH(timeoutInMs, 1);
	PH(commandInterval, 1);
	PH(dataValidTime[0], 2);
	PH(busyOffset, 1);
	PH(busyBitPolarity, 1);
	PH(lookupTable[0], 64);
	PH(lutCustomSeq[0], 12);
	PH(pageSize, 1);
	PH(sectorSize, 1);
#endif
}

static int do_qspihdr_dump(int argc, char * const argv[])
{
	unsigned long addr;
	char *endp;
	void *tmp;
	void *buf;
	int ret;

#if defined(CONFIG_MX6) || defined(CONFIG_MX7) || defined(CONFIG_ARCH_MX7ULP)
	int off = QSPI_HDR_OFF;
#else
	int off = FSPI_HDR_OFF;
#endif

	if (argc == 3) {
		/* check data in memory */
		if (do_qspihdr_check(3, argv, FLAG_VERBOSE)) {
			/* return 0 in any cases */
			return 0;
		}

		addr = simple_strtoul(argv[2], &endp, 16);

		tmp = map_physmem(addr + off, HDR_LEN, MAP_WRBACK);
		if (!tmp) {
			printf("Failed to map physical memory\n");
			return 1;
		}

		hdr_dump(tmp);
		unmap_physmem(tmp, HDR_LEN);
	} else {
		/* check data in Q(F)SPI */
		buf = malloc(HDR_LEN);
		if (!buf) {
			printf("Failed to alloc memory\n");
			/* return 0 in any cases */
			return 0;
		}

		ret = spi_flash_read(flash, off, HDR_LEN, buf);
		if (ret) {
			printf("flash read failed, ret: %d\n", ret);
			return -1;
		}

		hdr_dump(buf);
		free(buf);
	}

	return 0;
}

static int do_qspihdr_init(int argc, char * const argv[])
{
	unsigned long addr, len;
	char *endp;
	int total_len;
	void *tmp;
	void *buf;
	bool hdr_flag = false;
	int ret;

#if defined(CONFIG_MX6) || defined(CONFIG_MX7) || defined(CONFIG_ARCH_MX7ULP)
	int hdr_off = QSPI_HDR_OFF;
	int data_off = QSPI_DATA_OFF;
#else
	int hdr_off = FSPI_HDR_OFF;
	int data_off = FSPI_DATA_OFF;

	safe_config->fspi_hdr_config.pageSize = flash->page_size;
	safe_config->fspi_hdr_config.sectorSize = flash->sector_size;
#endif

	addr = simple_strtoul(argv[2], &endp, 16);
	len = simple_strtoul(argv[3], &endp, 16);

	total_len = data_off + len;
	if (total_len > flash->size) {
		printf("Error: length %lx over flash size (%#x)\n",
		       len, flash->size);
		return 1;
	}

	/* check if header exists in this memory area*/
	if (do_qspihdr_check(3, argv, 0) == 0)
		hdr_flag = true;

	tmp = map_physmem(addr, len, MAP_WRBACK);
	if (!tmp) {
		printf("Failed to map physical memory\n");
		return 1;
	}

	if (hdr_flag)
		goto burn_image;

	buf = malloc(total_len);
	if (!buf) {
		printf("Failed to alloc memory\n");
		unmap_physmem(tmp, total_len);
		return 1;
	}

	memset(buf, 0xff, total_len);
	memcpy(buf + hdr_off, safe_config, HDR_LEN);
	memcpy(buf + data_off, tmp, len);

burn_image:
	if (hdr_flag) {
		ret = qspi_erase_update(flash, 0, len, tmp);
	} else {
		ret = qspi_erase_update(flash, 0, total_len, buf);
		free(buf);
	}

	unmap_physmem(tmp, total_len);
	return ret;
}

static int do_qspihdr_update(int argc, char * const argv[])
{
	int len;
	int size;
	void *buf;
	int ret;

#if defined(CONFIG_MX6) || defined(CONFIG_MX7) || defined(CONFIG_ARCH_MX7ULP)
	int hdr_off = QSPI_HDR_OFF;
#else
	int hdr_off = FSPI_HDR_OFF;
#endif

	len = hdr_off + HDR_LEN;
	size = ROUND(len, flash->sector_size);

	buf = malloc(size);
	if (!buf) {
		printf("Failed to alloc memory\n");
		return 1;
	}

	spi_flash_read(flash, 0, size, buf);
	memcpy(buf + hdr_off, safe_config, HDR_LEN);

	ret = qspi_erase_update(flash, 0, size, buf);
	free(buf);

	return ret;
}

static int do_qspihdr(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	char *cmd;
	unsigned int bus = CONFIG_SF_DEFAULT_BUS;
	unsigned int cs = CONFIG_SF_DEFAULT_CS;
	unsigned int speed = CONFIG_SF_DEFAULT_SPEED;
	unsigned int mode = CONFIG_SF_DEFAULT_MODE;
	int flags = 0;
	int ret;

	if (argc < 2)
		goto usage;

#ifdef CONFIG_DM_SPI_FLASH
	struct udevice *new, *bus_dev;

	ret = spi_find_bus_and_cs(bus, cs, &bus_dev, &new);
	if (!ret)
		device_remove(new, DM_REMOVE_NORMAL);
	flash = NULL;
	ret = spi_flash_probe_bus_cs(bus, cs, speed, mode, &new);
	if (ret) {
		printf("Failed to initialize SPI flash at %u:%u (error %d)\n",
		       bus, cs, ret);
		return 1;
	}
	flash = dev_get_uclass_priv(new);
#endif

	cmd = argv[1];

	if (strcmp(cmd, "check") == 0)
		return do_qspihdr_check(argc, argv, flags | FLAG_VERBOSE);

	if (strcmp(cmd, "dump") == 0)
		return do_qspihdr_dump(argc, argv);

	if (strcmp(cmd, "init") == 0) {
		if (argc < 5)
			goto usage;
		return do_qspihdr_init(argc, argv);
	}

	if (strcmp(cmd, "update") == 0) {
		if (argc < 3)
			goto usage;
		return do_qspihdr_update(argc, argv);
	}

	return 0;
usage:
	return CMD_RET_USAGE;
}

static char qspihdr_help_text[] =
	"check [addr] - check if boot config already exists, 0-yes, 1-no\n"
	"		with addr, it will check data in memory of this addr\n"
	"		without addr, it will check data in Q(F)SPI chip\n"
	"qspihdr dump [addr] - dump the header information, if exists\n"
	"		with addr, it will check data in memory of this addr\n"
	"		without addr, it will check data in Q(F)SPI chip\n"
	"qspihdr init addr len safe - burn data to Q(F)SPI with header\n"
	"		if data contains header, it will be used, otherwise,\n"
	"		safe: most common header, single line, sdr, low freq\n"
	"qspihdr update safe - only update the header in Q(F)SPI\n";

U_BOOT_CMD(qspihdr, 5, 1, do_qspihdr,
	"Q(F)SPI Boot Config sub-system",
	qspihdr_help_text
);
