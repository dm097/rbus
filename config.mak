#Please make sure that 
# 1. PLATFORM_SDK,
# 2. RDK_DIR are exported properly before using this file

#Path of directory containing configuration files in STB. If commented, application expects config files to be present in working directory.
#CONFIG_DIR=/config
#Configuration flags.  Comment the definition for disabling and uncomment for enabling.


CFLAGS?= -g -Wno-format -Wunused

ifdef CONFIG_DIR
 CONFIG_PREFIX=$(CONFIG_DIR)/
endif


CFLAGS += -DDEBUG_CONF_FILE="\"$(CONFIG_PREFIX)debug.ini\""

ARFLAGS=rcs
BUILD_DIR=$(RDK_LOGGER_DIR)/build
LIBDIR=$(BUILD_DIR)/lib

LOGGER_INCL=$(RDK_LOGGER_DIR)/include

-include $(RDK_LOGGER_DIR)/soc/build/config.mak

INCLUDES=-I$(LOGGER_INCL)\
	-I$(RDK_LOGGER_DIR)/../core\
	-I$(PLATFORM_SDK)/include/ \
	-I$(PLATFORM_SDK)/include/gstreamer-0.10  \
	-I$(PLATFORM_SDK)/include/glib-2.0  \
	-I$(PLATFORM_SDK)/lib/glib-2.0/include  \
	-I$(PLATFORM_SDK)/usr/include/ \
	-I$(PLATFORM_SDK)/include/libxml2 \
	-I$(RDK_DIR)/opensource/include/libxml2 \
	-I$(RDK_DIR)/opensource/include/gstreamer-0.10 \
	-I$(RDK_DIR)/opensource/include/glib-2.0 \
	-I$(RDK_DIR)/opensource/include/ \
	-I$(RDK_DIR)/opensource/lib/glib-2.0/include  \
	-I$(PLATFORM_SDK)/include/linux_user
