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

dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ([2.69])
AC_INIT(rdklogger, 1.0, balaji_punnuru@cable.comcast.com,rmfwebkitifc)
AC_CONFIG_SRCDIR([src])
AM_CONFIG_HEADER(cfg/config.h)
AC_CONFIG_MACRO_DIR([cfg])
AC_CONFIG_AUX_DIR([cfg])
AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-bzip2 1.9])
LT_INIT
IS_DYNAMIC_ENABLED=""

dnl subdirectories.

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_CPP
AC_PROG_CXXCPP

dnl use pretty build output with automake >= 1.11
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],
  [AM_DEFAULT_VERBOSITY=1
   AC_SUBST(AM_DEFAULT_VERBOSITY)])




dnl **********************************
dnl checks for dependencies
dnl **********************************
SYSLOG_HELPER_CFLAGS=" "
SYSLOG_HELPER_LDFLAGS=" "
UTILS=""
AC_ARG_ENABLE([dynamic],
        AS_HELP_STRING([--enable-dynamic],[enable dynamic (default is no)]),
        [
          case "${enableval}" in
           yes) IS_DYNAMIC_ENABLED=true
                UTILS="util";;
           no)  IS_DYNAMIC_ENABLED=false ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-dynamic]) ;;
           esac
           ],
        [echo "dynamic logging is disabled"])
AM_CONDITIONAL([IS_DYNAMIC_ENABLED], [test x$IS_DYNAMIC_ENABLED = xtrue])



AC_ARG_ENABLE([systemd-syslog-helper],
        AS_HELP_STRING([--enable-systemd-syslog-helper],[enable systemd syslog logging]),
        [
                case "${enableval}" in
                yes) SYSLOG_HELPER_CFLAGS="-DSYSTEMD_SYSLOG_HELPER"
                     SYSLOG_HELPER_LDFLAGS="-lsysloghelper" ;;
                no) AC_MSG_ERROR([systemd syslog logging  is disabled]) ;;
                *) AC_MSG_ERROR([bad value ${enableval} for --enable-systemd-syslog-helper ]) ;;
                esac
                ],
        [echo "systemd syslog logging  is disable"])

AC_SUBST(SYSLOG_HELPER_CFLAGS)
AC_SUBST(SYSLOG_HELPER_LDFLAGS)
AC_SUBST(UTILS)

AC_HEADER_STDC

dnl Checks for header files.
AC_CHECK_HEADERS([limits.h memory.h stdlib.h string.h sys/socket.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
dnl AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT8_T

dnl Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strdup strerror])

PKG_CHECK_MODULES([LOG4C], [log4c >= 1.2.3])

AC_CONFIG_FILES([Makefile
		 cfg/Makefile
                 src/Makefile
                 src/util/Makefile
                 test/Makefile
                 include/Makefile])
AC_OUTPUT
