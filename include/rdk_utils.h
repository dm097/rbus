/*
 * ============================================================================
 * COMCAST C O N F I D E N T I A L AND PROPRIETARY
 * ============================================================================
 * This file (and its contents) are the intellectual property of Comcast.  It may
 * not be used, copied, distributed or otherwise  disclosed in whole or in part
 * without the express written permission of Comcast.
 * ============================================================================
 * Copyright (c) 2013 Comcast. All rights reserved.
 * ============================================================================
 */

/**
 * @file rdk_utils.h
 * The header file provides RDK Utils APIs.
 */


/**
* @defgroup rdk_logger
* @{
* @defgroup include
* @{
**/


#ifndef _RDK_UTILS_H_
#define _RDK_UTILS_H_

#include <stdio.h>
#include <rdk_error.h>
#include <rdk_utils.h>
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @ingroup RDKLOGGER_UTILS_API
 * @{
 */

const char* rdk_logger_envGet(const char *name);

int rdk_logger_envGetNum(const char * mod);

const char* rdk_logger_envGetValueFromNum(int number);

const char* rdk_logger_envGetModFromNum(int Num);

rdk_Error rdk_logger_env_add_conf_file(const char * path);

/** @} */ //end of Doxygen tag RDKLOGGER_UTILS_API

#ifdef __cplusplus
}
#endif

#endif /* _RDK_DEBUG_H_ */

