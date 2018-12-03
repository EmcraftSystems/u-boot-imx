/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "evkmimxrt1060_flexspi_nor_config.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

const flexspi_nor_config_t qspiflash_config = {
    .memConfig =
        {
            .tag = FLEXSPI_CFG_BLK_TAG,
            .version = FLEXSPI_CFG_BLK_VERSION,
            .readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackFromDqsPad,
            .csHoldTime = 3u,
            .csSetupTime = 3u,
            // Enable DDR mode, Wordaddassable, Safe configuration, Differential clock
            .sflashPadType = kSerialFlash_4Pads,
	    .serialClkFreq = kFlexSpiSerialClk_133MHz,
//            .serialClkFreq = kFlexSpiSerialClk_100MHz,
            .sflashA1Size = 8u * 1024u * 1024u,
            .lookupTable =
                {
                    // Read LUTs
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEB, RADDR_SDR, FLEXSPI_4PAD, 0x18),
                    FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, 0x06, READ_SDR, FLEXSPI_4PAD, 0x04),
                },
        },
};
