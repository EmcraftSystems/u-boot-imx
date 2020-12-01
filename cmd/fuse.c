/*
 * (C) Copyright 2009-2013 ADVANSEE
 * Benoît Thébaudeau <benoit.thebaudeau@advansee.com>
 *
 * Based on the mpc512x iim code:
 * Copyright 2008 Silicon Turnkey Express, Inc.
 * Martha Marx <mmarx@silicontkx.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <console.h>
#include <fuse.h>
#include <linux/errno.h>

static int strtou32(const char *str, unsigned int base, u32 *result)
{
	char *ep;

	*result = simple_strtoul(str, &ep, base);
	if (ep == str || *ep != '\0')
		return -EINVAL;

	return 0;
}

static int confirm_prog(void)
{
	puts("Warning: Programming fuses is an irreversible operation!\n"
			"         This may brick your system.\n"
			"         Use this command only if you are sure of "
					"what you are doing!\n"
			"\nReally perform this fuse programming? <y/N>\n");

	if (confirm_yesno())
		return 1;

	puts("Fuse programming aborted\n");
	return 0;
}

static int do_fuse(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	const char *op = argc >= 2 ? argv[1] : NULL;
	int confirmed = argc >= 3 && !strcmp(argv[2], "-y");
	u32 bank, word, cnt, val;
	int ret, i;

	argc -= 2 + confirmed;
	argv += 2 + confirmed;

	if (argc < 2 || strtou32(argv[0], 0, &bank) ||
			strtou32(argv[1], 0, &word))
		return CMD_RET_USAGE;

	if (!strcmp(op, "read")) {
		if (argc == 2)
			cnt = 1;
		else if (argc != 3 || strtou32(argv[2], 0, &cnt))
			return CMD_RET_USAGE;

		printf("Reading bank %u:\n", bank);
		for (i = 0; i < cnt; i++, word++) {
			if (!(i % 4))
				printf("\nWord 0x%.8x:", word);

			ret = fuse_read(bank, word, &val);
			if (ret)
				goto err;

			printf(" %.8x", val);
		}
		putc('\n');
	} else if (!strcmp(op, "sense")) {
		if (argc == 2)
			cnt = 1;
		else if (argc != 3 || strtou32(argv[2], 0, &cnt))
			return CMD_RET_USAGE;

		printf("Sensing bank %u:\n", bank);
		for (i = 0; i < cnt; i++, word++) {
			if (!(i % 4))
				printf("\nWord 0x%.8x:", word);

			ret = fuse_sense(bank, word, &val);
			if (ret)
				goto err;

			printf(" %.8x", val);
		}
		putc('\n');
	} else if (!strcmp(op, "prog")) {
		if (argc < 3)
			return CMD_RET_USAGE;

		for (i = 2; i < argc; i++, word++) {
			if (strtou32(argv[i], 16, &val))
				return CMD_RET_USAGE;

			printf("Programming bank %u word 0x%.8x to 0x%.8x...\n",
					bank, word, val);
			if (!confirmed && !confirm_prog())
				return CMD_RET_FAILURE;
			ret = fuse_prog(bank, word, val);
			if (ret)
				goto err;
		}
	} else if (!strcmp(op, "override")) {
		if (argc < 3)
			return CMD_RET_USAGE;

		for (i = 2; i < argc; i++, word++) {
			if (strtou32(argv[i], 16, &val))
				return CMD_RET_USAGE;

			printf("Overriding bank %u word 0x%.8x with "
					"0x%.8x...\n", bank, word, val);
			ret = fuse_override(bank, word, val);
			if (ret)
				goto err;
		}
	} else {
		return CMD_RET_USAGE;
	}

	return 0;

err:
	puts("ERROR\n");
	return CMD_RET_FAILURE;
}

static int macstr_to_char(const char *str, unsigned char *mac)
{
	char octet[3], *ret;
	char *p = (char *)str;
	int i;

	if (strlen(str) != (6 * 3 -1)) {
		return -1;
	}

	octet[2] = 0;
	for (i = 0; (i < 6); i++, p+=3) {
		octet[0] = p[0];
		octet[1] = p[1];

		mac[i] = simple_strtoul(octet, &ret, 16);
		if (ret != &octet[2]) {
			return -1;
		}

		if (i < 5 && p[2] != ':') {
			return -1;
		}
	}

	return 0;
}

extern void imx_get_mac_from_fuse(int dev_id, unsigned char *mac);
extern int imx_save_mac_to_fuse(int dev_id, unsigned char *mac);

static int confirm_save(const char *macstr)
{
	printf("Warning: This command will save mac address %s in fuses\n"
	       "Programming fuses is an irreversible operation!\n"
	       "         Use this command only if you are sure of "
	       "what you are doing!\n"
	       "\nReally perform this fuse programming? <y/N>\n", macstr);

	if (confirm_yesno())
		return 1;

	puts("Fuse programming aborted\n");
	return 0;
}


static int do_mac_fuse(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	const char *op = argc >= 2 ? argv[1] : NULL;
	int confirmed = 0;
	const char *macstr;
	unsigned char mac[6];

	if (!strcmp(op, "read")) {
		if (argc != 2)
			return CMD_RET_USAGE;
		imx_get_mac_from_fuse(0, mac);
		printf("%02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	} else if (!strcmp(op, "save")) {
		if (argc != 3 && argc != 4)
			return CMD_RET_USAGE;
		if (argc == 4) {
			confirmed = !strcmp(argv[2], "-y");
			if (!confirmed)
				return CMD_RET_USAGE;
			macstr = argv[3];
		} else {
			macstr = argv[2];
		}
		if (macstr_to_char(macstr, mac) != 0)
			return CMD_RET_USAGE;
		if (confirmed || confirm_save(macstr)) {
			if (imx_save_mac_to_fuse(0, mac) != 0) {
				printf("Failed to save mac address\n");
				return CMD_RET_FAILURE;
			}
		}
	} else {
		return CMD_RET_USAGE;
	}

	return 0;
}

U_BOOT_CMD(
	fuse, CONFIG_SYS_MAXARGS, 0, do_fuse,
	"Fuse sub-system",
	     "read <bank> <word> [<cnt>] - read 1 or 'cnt' fuse words,\n"
	"    starting at 'word'\n"
	"fuse sense <bank> <word> [<cnt>] - sense 1 or 'cnt' fuse words,\n"
	"    starting at 'word'\n"
	"fuse prog [-y] <bank> <word> <hexval> [<hexval>...] - program 1 or\n"
	"    several fuse words, starting at 'word' (PERMANENT)\n"
	"fuse override <bank> <word> <hexval> [<hexval>...] - override 1 or\n"
	"    several fuse words, starting at 'word'"
);

U_BOOT_CMD(
	mac_fuse, CONFIG_SYS_MAXARGS, 0,  do_mac_fuse,
	"display and program MAC address in fuses",
		  "read\n"
	"    - read mac address from fuses\n"
	"mac_fuses save [-y] <mac>\n"
	"    - save mac address (XX:XX:XX:XX:XX:XX) to fuses\n"
);
