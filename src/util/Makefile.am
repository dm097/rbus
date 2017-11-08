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

SUBDIRS = 
AM_CPPFLAGS = -pthread -Wall
lib_LTLIBRARIES = libloggerutil.la
libloggerutil_la_SOURCES = rdk_dynamic_log_reg.c

INCLUDE_FILES = -I=$(includedir)/glib-2.0 \
                -I=$(libdir)/glib-2.0/include \
                -I=${includedir}/rdk/iarmbus \
                -I=${includedir}/rdk/rdklogctrl

libloggerutil_la_CFLAGS =  $(INCLUDE_FILES)
libloggerutil_la_LDFLAGS = $(GLIB_LIBS)  $(LOG4C_LIBS)
libloggerutil_la_CPPFLAGS =  $(GLIB_CFLAGS) -I$(top_srcdir)/src/include -I$(top_srcdir)/include
libloggerutil_la_LIBADD = -lIARMBus
