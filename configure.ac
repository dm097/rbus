dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ([2.69])
AC_INIT(rdklogger, 1.0, balaji_punnuru@cable.comcast.com,rmfwebkitifc)
AC_CONFIG_SRCDIR([src/rdk_logger_util.c])
AM_CONFIG_HEADER(cfg/config.h)
AC_CONFIG_MACRO_DIR([cfg])
AC_CONFIG_AUX_DIR([cfg])
AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-bzip2 1.9])
LT_INIT

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
                 include/Makefile])
AC_OUTPUT
