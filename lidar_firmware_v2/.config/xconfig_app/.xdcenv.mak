#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/bios_6_83_00_18/packages;C:/Users/myles/workspace_v10/lidar_firmware_v2/.config
override XDCROOT = C:/ti/ccs1040/xdctools_3_62_01_15_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/bios_6_83_00_18/packages;C:/Users/myles/workspace_v10/lidar_firmware_v2/.config;C:/ti/ccs1040/xdctools_3_62_01_15_core/packages;..
HOSTOS = Windows
endif
