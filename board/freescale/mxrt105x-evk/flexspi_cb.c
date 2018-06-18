/*
 * (C) Copyright 2018 Emcraft Systems
 *
 * FlexSPI Configuration Block as per i.MX RT1050 Processor Reference Manual
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include "evkbimxrt1050_flexspi_nor_config.h"

const flexspi_nor_config_t qspiflash_config = {
	.memConfig =
	{
		.tag = FLEXSPI_CFG_BLK_TAG,
		.version = FLEXSPI_CFG_BLK_VERSION,
		.readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackFromDqsPad,
		.csHoldTime = 3u,
		.csSetupTime = 3u,
		.columnAddressWidth = 0u,
		.configCmdEnable = 0u,
		.controllerMiscOption = 0u,
		.deviceType = kFlexSpiDeviceType_SerialNOR,
		.sflashPadType = kSerialFlash_4Pads,
		.serialClkFreq = kFlexSpiSerialClk_133MHz,
		.lutCustomSeqEnable = 0u,
		.sflashA1Size = 0x00800000,
		.lookupTable =
		{
			// Read LUTs
			[0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEB, RADDR_SDR, FLEXSPI_4PAD, 0x18),
			[1] = FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, 0x06, READ_SDR, FLEXSPI_4PAD, 0x02),
			[2] = FLEXSPI_LUT_SEQ(STOP, 0, 0, STOP, 0, 0),
			[3] = FLEXSPI_LUT_SEQ(STOP, 0, 0, STOP, 0, 0),
		},

	},
};
