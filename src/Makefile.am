##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################

AM_CPPFLAGS = -pthread -Wall
lib_LTLIBRARIES = librdkloggers.la
librdkloggers_la_SOURCES = rdk_debug.c rdk_debug_priv.c rdk_dynamic_logger.c rdk_logger_init.c rdk_logger_util.c rdk_logger_milestone.cpp
if IS_ONBOARDLOG_ENABLED
librdkloggers_la_SOURCES += rdk_logger_onboard.c
endif
librdkloggers_la_CFLAGS =  $(LOG4C_CFLAGS) $(GLIB_CFLAGS) $(SYSLOG_HELPER_CFLAGS) $(SYSTEMD_CLFAGS) -I./include -I../include -I${RDK_PROJECT_ROOT_PATH}/opensource/include/log4c -I${RDK_PROJECT_ROOT_PATH}/opensource/include -DDEBUG_CONF_FILE="\"debug.ini\"" $(SAFEC_CFLAGS)
librdkloggers_la_LDFLAGS = $(GLIB_LIBS)  $(LOG4C_LIBS) $(SYSLOG_HELPER_LDFLAGS) $(SYSTEMD_LDFLAGS)
librdkloggers_la_CPPFLAGS =  $(GLIB_CFLAGS) -I$(top_srcdir)/src/include -I$(top_srcdir)/include
