/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2025 Emcraft Systems
 */

#include <asm/mach-imx/qspihdr.h>


const struct fspi_config_parameter qspiflash_config = {
	.tag              = FSPI_HDR_TAG,
	.version          = FSPI_HDR_VERSION_V1_4_0,
	.readSampleClkSrc = kFlexSPIReadSampleClk_ExternalInputFromDqsPad,
	.dataHoldTime       = 3u,
	.dataSetupTime      = 3u,
	.columnAddressWidth = 3u,
	// Enable DDR mode, Wordaddassable, Safe configuration, Differential clock
	.controllerMiscOption =
                (1u << kFlexSpiMiscOffset_DdrModeEnable) | (1u << kFlexSpiMiscOffset_WordAddressableEnable) |
                (1u << kFlexSpiMiscOffset_SafeConfigFreqEnable) | (1u << kFlexSpiMiscOffset_DiffClkEnable),
	.deviceType           = kFlexSpiDeviceType_SerialNOR,
	.sflashPadType        = kSerialFlash_8Pads,
	.serialClkFreq        = kFlexSpiSerialClk_133MHz,
	.sflashA1Size         = 32u * 1024u * 1024u,
	.dataValidTime = {16u, 16u},
	.lookupTableF =
                {
                    // Read LUTs
			FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0xEB, RADDR_DDR, FLEXSPI_8PAD, 0x18),
			FLEXSPI_LUT_SEQ(CADDR_DDR, FLEXSPI_8PAD, 0x10, DUMMY_DDR, FLEXSPI_8PAD, 0x06),
			FLEXSPI_LUT_SEQ(READ_DDR, FLEXSPI_8PAD, 0x04, STOP, FLEXSPI_1PAD, 0),
                },
	.pageSize           = 512u,
	.sectorSize         = 256u * 1024u,
	.ipcmdSerialClkFreq = 0x1,
	.blockSize          = 256u * 1024u,
	.isUniformBlockSize = true,
};
