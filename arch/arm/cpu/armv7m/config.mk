# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2015
# Kamil Lulko, <kamil.lulko@gmail.com>

PLATFORM_CPPFLAGS += -mno-unaligned-access

# Suppress FDPIC / PIE code-gen so U-Boot links cleanly with an FDPIC-default
# toolchain (e.g. buildroot's arm-buildroot-uclinuxfdpiceabi-gcc).
# Guarded with cc-option so legacy bare-metal toolchains (arm-none-eabi-)
# that lack -mno-fdpic ignore the flag rather than error out.
PLATFORM_CPPFLAGS += $(call cc-option,-mno-fdpic)
PLATFORM_CPPFLAGS += $(call cc-option,-fno-pic)
PLATFORM_CPPFLAGS += $(call cc-option,-no-pie)
