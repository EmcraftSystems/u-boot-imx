/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2021 NXP
 * Copyright (C) 2023 Emcraft Systems
 */

#ifndef __MACH_QSPI_HDR_H__
#define __MACH_QSPI_HDR_H__

#include <linux/types.h>

#define QSPI_HDR_TAG		0xc0ffee01 /* c0ffee01 */
#define QSPI_HDR_TAG_OFF	0x1fc
#define FSPI_HDR_TAG		0x42464346/* FCFB, bigendian */
#define FSPI_HDR_TAG_OFF	0x0
#define FSPI_HDR_VERSION_V1_4_0	(0x56010400UL) // V1.4.0
#define FSPI_HDR_SIZE    (512)

#ifdef CONFIG_MX94
#define HDR_LEN			0x300
#else
#define HDR_LEN			0x200
#endif

#ifdef CONFIG_MX7
#define QSPI_HDR_OFF	0x0
#define QSPI_DATA_OFF	0x400
#else
#define QSPI_HDR_OFF	0x400
#define QSPI_DATA_OFF	0x1000
#endif

#ifdef CONFIG_IMX8MM
#define FSPI_HDR_OFF	0x0
#define FSPI_DATA_OFF	0x1000
#else
#define FSPI_HDR_OFF	0x400
#define FSPI_DATA_OFF	0x1000
#endif

#define FLAG_VERBOSE		1

struct qspi_config_parameter {
	u32 dqs_loopback;			/* Sets DQS LoopBack Mode to enable Dummy Pad MCR[24] */
	u32 hold_delay;				/* No needed on ULT1 */
	u32 hsphs;				/* Half Speed Phase Shift */
	u32 hsdly;				/* Half Speed Delay Selection */
	u32 device_quad_mode_en;		/* Write Command to Device */
	u32 device_cmd;				/* Cmd to xfer to device */
	u32 write_cmd_ipcr;			/* IPCR value of Write Cmd */
	u32 write_enable_ipcr;			/* IPCR value of Write enable */
	u32 cs_hold_time;			/* CS hold time in terms of serial clock.(for example 1 serial clock cyle) */
	u32 cs_setup_time;			/* CS setup time in terms of serial clock.(for example 1 serial clock cyle) */
	u32 sflash_A1_size;			/* interms of Bytes */
	u32 sflash_A2_size;			/* interms of Bytes */
	u32 sflash_B1_size;			/* interms of Bytes */
	u32 sflash_B2_size;			/* interms of Bytes */
	u32 sclk_freq;				/* 0 - 18MHz, 1 - 49MHz, 2 - 55MHz, 3 - 60MHz, 4 - 66Mhz, 5 - 76MHz, 6 - 99MHz (only for SDR Mode) */
	u16 busy_bit_offset;			/* Flash device busy bit offset in status register */
	u16 busy_bit_polarity;			/* Polarity of busy bit, 0 means the busy bit is 1 while busy and vice versa. */
	u32 sflash_type;			/* 1 - Single, 2 - Dual, 4 - Quad */
	u32 sflash_port;			/* 0 - Only Port-A, 1 - Both PortA and PortB */
	u32 ddr_mode_enable;			/* Enable DDR mode if set to TRUE */
	u32 dqs_enable;				/* Enable DQS mode if set to TRUE. Bit 0 represents DQS_EN, bit 1 represents DQS_LAT_EN */
	u32 parallel_mode_enable;		/* Enable Individual or parrallel mode. */
	u32 portA_cs1;				/* Enable Port A CS1 */
	u32 portB_cs1;				/* Enable Port B CS1 */
	u32 fsphs;				/* Full Speed Phase Selection */
	u32 fsdly;				/* Full Speed Phase Selection */
	u32 ddrsmp;				/* Select the sampling point for incoming data when serial flash is in DDR mode. */
	u32 command_seq[64];			/* Set of seq to perform optimum read on SFLASH as as per vendor SFLASH */
	u32 read_status_ipcr;			/* IPCR value of Read Status Reg */
	u32 enable_dqs_phase;			/* Enable DQS phase */
	u32 config_cmds_en;			/* Enable config commands */
	u32 config_cmds[4];			/* config commands, used to configure nor flash */
	u32 config_cmds_args[4];		/* config commands argu */
	u32 dqs_pad_setting_override;		/* DQS pin pad setting override */
	u32 sclk_pad_setting_override;		/* SCLK pin pad setting override */
	u32 data_pad_setting_override;		/* DATA pins pad setting override */
	u32 cs_pad_setting_override;		/* CS pins pad setting override */
	u32 dqs_loopback_internal;		/* 0: dqs loopback from pad, 1: dqs loopback internally */
	u32 dqs_phase_sel;			/* dqs phase sel */
	u32 dqs_fa_delay_chain_sel;		/* dqs fa delay chain selection */
	u32 dqs_fb_delay_chain_sel;		/* dqs fb delay chain selection */
	u32 sclk_fa_delay_chain_sel;		/* sclk fa delay chain selection */
	u32 sclk_fb_delay_chain_sel;		/* sclk fb delay chain selection */
	u32 misc_clock_enable;			/* Misc clock enable, bit 0 means differential clock enable, bit 1 means CK2 clock enable. */
	u32 reserve[15];			/* Reserved area, the total size of configuration structure should be 512 bytes */
	u32 tag;				/* QSPI configuration TAG, should be 0xc0ffee01 */
};

struct fspi_config_parameter {
	u32 tag;			/* tag, 0x46434642 ascii 'FCFB' */
	u32 version;			/* 0x00000156 ascii bugfix | minor | major | 'V' */
	u16 reserved;
	u8  reserved0[2];
	u8  readSampleClkSrc;		/* 0 - internal loopback, 1 - loopback from DQS pad, 2 - loopback from SCK pad, 3 - Flash provided DQS */
	u8  dataHoldTime;		/* CS hold time */
	u8  dataSetupTime;		/* CS setup time */
	u8  columnAddressWidth;		/* 3 - for HyperFlash, 0 - other devices */
	u8  deviceModeCfgEnable;	/* device mode configuration enable feature, 0 - disable, 1- enable */
	union {
		u8  reserved1[3];
		struct {
			u8	deviceModeType;		/* 0 -Generic, 1 - QuadEnable, 2 - Spi2Xpi, 3 - Xpi2Spi */
			u8	waitTimeCfgCommands[2]; /* wait time for device mode configuration commands in unit of 100us */
		};
	};
	u32 deviceModeSeq;		/* sequence parameter for device mode configuration */
	u32 deviceModeArg;		/* device mode argument, effective only when deviceModeCfgEnable = 1 */
	u8  configCmdEnable;		/* config command enable feature, 0 - disable, 1 - enable */
	union {
		u8  reserved2[3];
		u8  configModeType[3];	/* config mode type, similar to deviceModeType */
	};
	union {
		u32 configCmdSeqsF[4];		/* sequences for config command, allow 4 separate configuration command sequences */
		struct {
			u32 configCmdSeqsX[3];		/* sequences for config command, allow 3 command sequences */
			u8  ipedMode;			/* IPED mode, 0 - disable, 1 - enable */
			u8  x16DllaSlvFineOffset;	/* Fine offset Delay Elements in incoming DQS1 */
			u8  maxCsLowInterval;		/* Configure psram tcsm, unit 0.1 us */
			u8 ahbAlignment;		/* AHB alignment, 00b - no limit, 01b - 256B, 10b - 512B, 11b - 1KB */
		};
	};
	union {
		u32 configCmdArgsF[4];		/* arguments for each separate configuration command sequence */
		struct {
			u32 configCmdArgsX[3];	/* arguments for each separate configuration command sequence */
			u8  ahbSplitEn;		/* Enable AHB transaction split for PSRAM */
			u8  reservedArgs0[3];
		};
	};
	u32 controllerMiscOption;
					/*
					 *
					 * +--------+----------------------------------------------------------+
					 * | offset | description					       |
					 * +--------+----------------------------------------------------------+
					 * |	    | differential clock enable				       |
					 * |   0    |							       |
					 * |	    | 0 - differential clock is not supported		       |
					 * |	    | 1 - differential clock is supported		       |
					 * +--------+----------------------------------------------------------+
					 * |	    | CK2 enable					       |
					 * |   1    |							       |
					 * |	    | must set 0 for this silicon			       |
					 * |	    |							       |
					 * +--------+----------------------------------------------------------+
					 * |	    | parallel mode enable				       |
					 * |   2    |							       |
					 * |	    | must set 0 for this silicon			       |
					 * |	    |							       |
					 * +--------+----------------------------------------------------------+
					 * |	    | word addressable enable				       |
					 * |   3    |							       |
					 * |	    | 0 - device is not word addressable		       |
					 * |	    | 1 - device is word addressable			       |
					 * +--------+----------------------------------------------------------+
					 * |	    | safe configuration frequency enable		       |
					 * |   4    |							       |
					 * |	    | 0 - configure external device using specified frequency  |
					 * |	    | 1 - configure external device using 30MHz		       |
					 * +--------+----------------------------------------------------------+
					 * |   5    | reserved						       |
					 * +--------+----------------------------------------------------------+
					 * |	    | ddr mode enable					       |
					 * |   6    |							       |
					 * |	    | 0 - external device works using SDR commands	       |
					 * |	    | 1 - external device works using DDR commands	       |
					 * +--------+----------------------------------------------------------+
					 */
	u8  deviceType;			/* 1 - serial NOR */
	u8  sflashPadType;		/* 1 - single pad, 2 - dual pads, 4 - quad pads, 8 - octal pads */
	u8  serialClkFreq;		/* 1 - 20MHz, 2 - 50MHz, 3 - 60MHz, 4 - 80MHz, 5 - 100MHz, 6 - 133MHz, 7 - 166MHz, other values - 20MHz*/
	u8  lutCustomSeqEnable;		/* 0 - use pre-defined LUT sequence index and number, 1 - use LUT sequence parameters provided in this block */
	u32 reserved3[2];
	u32 sflashA1Size;		/* For SPI NOR, need to fill with actual size, in terms of bytes */
	u32 sflashA2Size;		/* same as above */
	u32 sflashB1Size;		/* same as above */
	u32 sflashB2Size;		/* same as above */
	u32 csPadSettingOverride;	/* set to 0 if it is not supported */
	u32 sclkPadSettingOverride;	/* set to 0 if it is not supported */
	u32 dataPadSettingOverride;	/* set to 0 if it is not supported */
	u32 dqsPadSettingOverride;	/* set to 0 if it is not supported */
	u32 timeoutInMs;		/* maximum wait time during dread busy status, not used in ROM */
	u32 commandInterval;		/* interval of CS deselected period, set to 0 */
	u16 dataValidTime[2];		/* time from clock edge to data valid edge */
					/* This field is used when the FlexSPI root clock is less than 100MHz and the read sample */
					/* clock source is device provided DQS signal without CK2 support. */
					/* [31:16] - data valid time for DLLB in terms of 0.1ns */
					/* [15:0]  - data valid time for DLLA in terms of 0.1ns */
	u16 busyOffset;			/* busy bit offset, valid range: 0 - 31 */
	u16 busyBitPolarity;		/* 0 - busy bit is 1 if device is busy, 1 - busy bit is 0 if device is busy */
	union {
		struct {
			u32 lookupTableF[64];	/* lookup table */
			u32 lutCustomSeqF[12];	/* customized LUT sequence */
			u32 reserved4[4];
			u32 pageSize;		/* page size of serial NOR flash, not used in ROM */
			u32 sectorSize;		/* sector size of serial NOR flash, not used in ROM */
			u8  ipcmdSerialClkFreq;	/* clock frequency for IP command */
			u8  isUniformBlockSize;	/* setor/bock size is the same */
			u8  reserved5[2];
			u8  serialNorType;	/* serial NOR Flash type: 0/1/2/3 */
			u8  needExitNoCmdMode;	/* need to exit NoCmd mode before other IP command */
			u8  halfClkForNonReadCmd; /* half the Serial Clock for non-read command: true/false */
			u8  needRestoreNoCmdMode; /* need to restore NoCmd mode after IP commmand execution */
			u32 blockSize;		/* block size */
			u32 reserve6[11];
		};
		struct {
			u32 lookupTableX[80];	/* lookup table */
			u32 lutCustomSeqX[12];	/* customized LUT sequence */
			u32 dllCraSdrVal;	/* customized DLLCRA for SDR setting */
			u32 smprSdrVal;		/* customized SMPR for SDR setting */
			u32 dllCraDdrVal;	/* customized DLLCRA for DDR setting */
			u32 smprDdrVal;		/* customized SMPR for DDR setting */
		};
	};
};

struct xspi_config_parameter {
	struct fspi_config_parameter mem_config;
	u32 pageSize;			/* page size of serial NOR flash */
	u32 sectorSize;			/* sector size of serial NOR flash */
	u8  ipcmdSerialClkFreq;		/* clock frequency for IP commands */
	u8  isUniformBlockSize;		/* sector/block size is the same */
	u8  isDataOrderSwapped;		/* data order (D0, D1, D2, D3) is swapped (D1, D0, D3, D2) */
	u8  reserved0;
	u8  serialNorType;		/* serial NOR flash type */
	u8  needExitNoCmdMode;		/* need to exit NoCmd mode before other IP commands */
	u8  halfClkForNonReadCmd;	/* half clock for non-read commands */
	u8  needRestoreNoCmdMode;	/* need to restore NoCmd mode after IP commands */
	u32 blockSize;			/* block size of serial NOR flash */
	u32 flashStateCtx;		/* flash state context */
	u32 reserved1[58];
};

struct header_config {
	union {
		struct qspi_config_parameter qspi_hdr_config;
		struct fspi_config_parameter fspi_hdr_config;
		struct xspi_config_parameter xspi_hdr_config;
	};
};

/* FlexSPI Read Sample Clock Source definition */
enum {
    kFlexSPIReadSampleClk_LoopbackInternally      = 0,
    kFlexSPIReadSampleClk_LoopbackFromDqsPad      = 1,
    kFlexSPIReadSampleClk_LoopbackFromSckPad      = 2,
    kFlexSPIReadSampleClk_ExternalInputFromDqsPad = 3,
};

/* Misc feature bit definitions */
enum
{
    kFlexSpiMiscOffset_DiffClkEnable            = 0, //!< Bit for Differential clock enable
    kFlexSpiMiscOffset_Ck2Enable                = 1, //!< Bit for CK2 enable
    kFlexSpiMiscOffset_ParallelEnable           = 2, //!< Bit for Parallel mode enable
    kFlexSpiMiscOffset_WordAddressableEnable    = 3, //!< Bit for Word Addressable enable
    kFlexSpiMiscOffset_SafeConfigFreqEnable     = 4, //!< Bit for Safe Configuration Frequency enable
    kFlexSpiMiscOffset_PadSettingOverrideEnable = 5, //!< Bit for Pad setting override enable
    kFlexSpiMiscOffset_DdrModeEnable            = 6, //!< Bit for DDR clock confiuration indication.
};

/* Flash Type definition */
enum {
    kFlexSpiDeviceType_SerialNOR    = 1,	/* Flash devices are Serial NOR */
    kFlexSpiDeviceType_SerialNAND   = 2,	/* Flash devices are Serial NAND */
    kFlexSpiDeviceType_SerialRAM    = 3,	/* Flash devices are Serial RAM/HyperFLASH */
    kFlexSpiDeviceType_MCP_NOR_NAND = 0x12,	/* Flash device is MCP device, A1 is Serial NOR, A2 is Serial NAND */
    kFlexSpiDeviceType_MCP_NOR_RAM  = 0x13,	/* Flash deivce is MCP device, A1 is Serial NOR, A2 is Serial RAMs */
};

/* Flash Pad definitions */
enum {
    kSerialFlash_1Pad  = 1,
    kSerialFlash_2Pads = 2,
    kSerialFlash_4Pads = 4,
    kSerialFlash_8Pads = 8,
};

/* Definitions for FlexSPI Serial Clock Frequency */
enum {
    kFlexSpiSerialClk_30MHz  = 1,
    kFlexSpiSerialClk_50MHz  = 2,
    kFlexSpiSerialClk_60MHz  = 3,
    kFlexSpiSerialClk_80MHz  = 4,
    kFlexSpiSerialClk_100MHz = 5,
    kFlexSpiSerialClk_120MHz = 6,
    kFlexSpiSerialClk_133MHz = 7,
    kFlexSpiSerialClk_166MHz = 8,
    kFlexSpiSerialClk_200MHz = 9,
};

/* Lookup table related defintions */
#define CMD_INDEX_READ        0
#define CMD_INDEX_READSTATUS  1
#define CMD_INDEX_WRITEENABLE 2
#define CMD_INDEX_WRITE       4

#define CMD_LUT_SEQ_IDX_READ        0
#define CMD_LUT_SEQ_IDX_READSTATUS  1
#define CMD_LUT_SEQ_IDX_WRITEENABLE 3
#define CMD_LUT_SEQ_IDX_WRITE       9

#define CMD_SDR        0x01
#define CMD_DDR        0x21
#define RADDR_SDR      0x02
#define RADDR_DDR      0x22
#define CADDR_SDR      0x03
#define CADDR_DDR      0x23
#define MODE1_SDR      0x04
#define MODE1_DDR      0x24
#define MODE2_SDR      0x05
#define MODE2_DDR      0x25
#define MODE4_SDR      0x06
#define MODE4_DDR      0x26
#define MODE8_SDR      0x07
#define MODE8_DDR      0x27
#define WRITE_SDR      0x08
#define WRITE_DDR      0x28
#define READ_SDR       0x09
#define READ_DDR       0x29
#define LEARN_SDR      0x0A
#define LEARN_DDR      0x2A
#define DATSZ_SDR      0x0B
#define DATSZ_DDR      0x2B
#define DUMMY_SDR      0x0C
#define DUMMY_DDR      0x2C
#define DUMMY_RWDS_SDR 0x0D
#define DUMMY_RWDS_DDR 0x2D
#define JMP_ON_CS      0x1F
#define STOP           0

#define FLEXSPI_1PAD 0
#define FLEXSPI_2PAD 1
#define FLEXSPI_4PAD 2
#define FLEXSPI_8PAD 3

#define FLEXSPI_LUT_SEQ(cmd0, pad0, op0, cmd1, pad1, op1)                                                              \
    (FLEXSPI_LUT_OPERAND0(op0) | FLEXSPI_LUT_NUM_PADS0(pad0) | FLEXSPI_LUT_OPCODE0(cmd0) | FLEXSPI_LUT_OPERAND1(op1) | \
     FLEXSPI_LUT_NUM_PADS1(pad1) | FLEXSPI_LUT_OPCODE1(cmd1))


#define FLEXSPI_LUT_OPERAND0_MASK                (0xFFU)
#define FLEXSPI_LUT_OPERAND0_SHIFT               (0U)
#define FLEXSPI_LUT_OPERAND0(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXSPI_LUT_OPERAND0_SHIFT)) & FLEXSPI_LUT_OPERAND0_MASK)

#define FLEXSPI_LUT_NUM_PADS0_MASK               (0x300U)
#define FLEXSPI_LUT_NUM_PADS0_SHIFT              (8U)
#define FLEXSPI_LUT_NUM_PADS0(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXSPI_LUT_NUM_PADS0_SHIFT)) & FLEXSPI_LUT_NUM_PADS0_MASK)

#define FLEXSPI_LUT_OPCODE0_MASK                 (0xFC00U)
#define FLEXSPI_LUT_OPCODE0_SHIFT                (10U)
#define FLEXSPI_LUT_OPCODE0(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXSPI_LUT_OPCODE0_SHIFT)) & FLEXSPI_LUT_OPCODE0_MASK)

#define FLEXSPI_LUT_OPERAND1_MASK                (0xFF0000U)
#define FLEXSPI_LUT_OPERAND1_SHIFT               (16U)
#define FLEXSPI_LUT_OPERAND1(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXSPI_LUT_OPERAND1_SHIFT)) & FLEXSPI_LUT_OPERAND1_MASK)

#define FLEXSPI_LUT_NUM_PADS1_MASK               (0x3000000U)
#define FLEXSPI_LUT_NUM_PADS1_SHIFT              (24U)
#define FLEXSPI_LUT_NUM_PADS1(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXSPI_LUT_NUM_PADS1_SHIFT)) & FLEXSPI_LUT_NUM_PADS1_MASK)

#define FLEXSPI_LUT_OPCODE1_MASK                 (0xFC000000U)
#define FLEXSPI_LUT_OPCODE1_SHIFT                (26U)
#define FLEXSPI_LUT_OPCODE1(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXSPI_LUT_OPCODE1_SHIFT)) & FLEXSPI_LUT_OPCODE1_MASK)

#endif	/* __MACH_QSPI_HDR_H__*/
