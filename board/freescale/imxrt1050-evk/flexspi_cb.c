/*
 * (C) Copyright 2018-2023 Emcraft Systems
 *
 * FlexSPI Configuration Block as per i.MX RT1050 Processor Reference Manual
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <asm/mach-imx/qspihdr.h>

/* const flexspi_nor_config_t qspiflash_config = { */
const struct fspi_config_parameter qspiflash_config = {
	.tag = FSPI_HDR_TAG,
	.version = FSPI_HDR_VERSION_V1_4_0,
	.readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackFromDqsPad,
	.dataHoldTime       = 3u,
	.dataSetupTime      = 3u,
	.columnAddressWidth = 0u,
	.configCmdEnable = 0u,
	.controllerMiscOption = 0u,
	.deviceType = kFlexSpiDeviceType_SerialNOR,
	.sflashPadType = kSerialFlash_4Pads,
	.serialClkFreq = kFlexSpiSerialClk_100MHz,
	.lutCustomSeqEnable = 0u,
	.sflashA1Size = 0x00800000,
	.lookupTableF =
	{
		// Read LUTs
		[0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEB, RADDR_SDR, FLEXSPI_4PAD, 0x18),
		[1] = FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, 0x06, READ_SDR, FLEXSPI_4PAD, 0x02),
		[2] = FLEXSPI_LUT_SEQ(STOP, 0, 0, STOP, 0, 0),
		[3] = FLEXSPI_LUT_SEQ(STOP, 0, 0, STOP, 0, 0),
	},

};
