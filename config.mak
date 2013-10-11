#Please make sure that 
# 1. PLATFORM_SDK,
# 2. RDK_DIR are exported properly before using this file

#Path of directory containing configuration files in STB. If commented, application expects config files to be present in working directory.
#CONFIG_DIR=/config
#Configuration flags.  Comment the definition for disabling and uncomment for enabling.

configMode ?= HEADED_GW


ifndef PLATFORM_SDK 
  TOOLCHAIN_DIR=/opt/toolchain/staging_dir
else
  TOOLCHAIN_DIR=$(PLATFORM_SDK)
endif

ifndef GENERATE_SI_CACHE_UTILITY
  CC=$(TOOLCHAIN_DIR)/bin/i686-cm-linux-g++
  CXX=$(TOOLCHAIN_DIR)/bin/i686-cm-linux-g++
  AR=$(TOOLCHAIN_DIR)/bin/i686-cm-linux-ar
  LD=$(TOOLCHAIN_DIR)/bin/i686-cm-linux-ld
else
  CC=g++
  AR=ar
endif

CFLAGS?= -g -Wno-format -Wunused -DUSE_CC_GETTIMEOFDAY

ifdef CONFIG_DIR
 CONFIG_PREFIX=$(CONFIG_DIR)/
endif


CFLAGS += -DDEBUG_CONF_FILE="\"$(CONFIG_PREFIX)debug.ini\""

ARFLAGS=rcs
BUILD_DIR=$(RDK_LOGGER_DIR)/build
LIBDIR=$(BUILD_DIR)/lib

LOGGER_INCL=$(RDK_LOGGER_DIR)/include

INCLUDES=-I$(LOGGER_INCL)\
	-I$(RDK_LOGGER_DIR)/../core\
	-I$(TOOLCHAIN_DIR)/include/ \
	-I$(TOOLCHAIN_DIR)/include/gstreamer-0.10  \
	-I$(TOOLCHAIN_DIR)/include/glib-2.0  \
	-I$(TOOLCHAIN_DIR)/lib/glib-2.0/include  \
	-I$(TOOLCHAIN_DIR)/usr/include/ \
	-I$(TOOLCHAIN_DIR)/include/libxml2 \
	-I$(RDK_DIR)/opensource/include/libxml2 \
	-I$(RDK_DIR)/opensource/include/gstreamer-0.10 \
	-I$(RDK_DIR)/opensource/include/glib-2.0 \
	-I$(RDK_DIR)/opensource/include/ \
	-I$(RDK_DIR)/opensource/lib/glib-2.0/include  \
	-I$(TOOLCHAIN_DIR)/include/linux_user
