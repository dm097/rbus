/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

