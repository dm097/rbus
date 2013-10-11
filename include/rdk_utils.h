/*
 * ============================================================================
 * COMCAST CONFIDENTIAL AND PROPRIETARY
 * ============================================================================
 * This file and its contents are the intellectual property of Comcast. It may
 * not be used, copied, distributed or otherwise disclosed in whole or in part
 * without the express written permission of Comcast.
 * ============================================================================
 * Copyright (c) 2013 Comcast. All rights reserved.
 * ============================================================================
 */

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
 * The <i>rdk_logger_envGet()</i> function will get the value of the specified
 * environment variable.
 *
 * @param name is a pointer to the name of the target environment variable.
 *
 * @return a pointer to the associated string value of the target environment
 * variable or NULL if the variable can't be found.
 */
const char* rdk_logger_envGet(const char *name);

/**
 * The <i>rdk_logger_envGetNum()</i> function will give the registered number 
 * of the specified environment variable.
 *
 * @param mod is a pointer to the name of the target environment variable.
 * @return -1 if fails, otherwise <i>number<i/>
 *          is returned.
 */
int rdk_logger_envGetNum(const char * mod);

/**
 * The <i>rdk_logger_envGetValueFromNum()</i> function will get the value of the specified
 * environment variable based on its registered number.
 *
 * @param number is a registered number of the target environment variable.
 * @return NULL if the create fails, otherwise <i>value<i/>
 *          is returned.
 */
const char* rdk_logger_envGetValueFromNum(int number);

/**
 * The <i>rdk_logger_envGetModFromNum()</i> function will get the name of the specified
 * environment variable based on its registered number.
 *
 * @param number is a registered number of the target environment variable.
 * @return NULL if it fails, otherwise <i>name<i/>
 *          is returned.
 */
const char* rdk_logger_envGetModFromNum(int Num);

/**
 * The <i>rdk_logger_env_add_conf_file()</i> function sets up the environment variable
 * storage by parsing configuration file.
 *
 * @param path Path of the file.
 * @return Returns relevant <i>-1<i> error code on failure, otherwise <i>RDK_SUCCESS</i>
 *          is returned.
 */
rdk_Error rdk_logger_env_add_conf_file(const char * path);

#ifdef __cplusplus
}
#endif

#endif /* _RDK_DEBUG_H_ */

