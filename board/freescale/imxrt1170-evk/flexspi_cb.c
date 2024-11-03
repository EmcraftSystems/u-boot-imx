/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2023 Emcraft Systems
 */

#include <asm/mach-imx/qspihdr.h>


const struct fspi_config_parameter qspiflash_config = {
	.tag              = FSPI_HDR_TAG,
	.version          = FSPI_HDR_VERSION_V1_4_0,
	.readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackFromDqsPad,
	.dataHoldTime       = 3u,
	.dataSetupTime      = 3u,
	// Enable DDR mode, Wordaddassable, Safe configuration, Differential clock
	.controllerMiscOption = 0x10,
	.deviceType           = kFlexSpiDeviceType_SerialNOR,
	.sflashPadType        = kSerialFlash_4Pads,
	.serialClkFreq        = kFlexSpiSerialClk_133MHz,
	.sflashA1Size         = 16u * 1024u * 1024u,
	.lookupTableF =
                {
                    // Read LUTs
			FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEB, RADDR_SDR, FLEXSPI_4PAD, 0x18),
			FLEXSPI_LUT_SEQ(MODE8_SDR, FLEXSPI_4PAD, 0x00, DUMMY_SDR, FLEXSPI_4PAD, 0x04),
			FLEXSPI_LUT_SEQ(READ_SDR, FLEXSPI_4PAD, 0x04, 0, 0, 0),
                },
	.pageSize           = 256u,
	.sectorSize         = 4u * 1024u,
	.ipcmdSerialClkFreq = 0x1,
	.blockSize          = 256u * 1024u,
	.isUniformBlockSize = false,
};
