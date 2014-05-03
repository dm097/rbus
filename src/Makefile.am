## Copyright (c) 2013 Comcast. All rights reserved.

SUBDIRS = 
AM_CPPFLAGS = -pthread -Wall
lib_LTLIBRARIES = librdkloggers.la
librdkloggers_la_SOURCES = rdk_debug.c rdk_debug_priv.c rdk_logger_init.c rdk_logger_util.c
librdkloggers_la_CFLAGS =  $(GLIB_CFLAGS) -I./include -I../include -I${RDK_PROJECT_ROOT_PATH}/opensource/include/log4c -I${RDK_PROJECT_ROOT_PATH}/opensource/include -DDEBUG_CONF_FILE="\"debug.ini\""
librdkloggers_la_LDFLAGS = $(GLIB_LIBS) 
librdkloggers_la_CPPFLAGS =  $(GLIB_CFLAGS) -I./include -I../include
