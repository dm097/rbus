/*
 * ============================================================================
 * COMCAST C O N F I D E N T I A L AND PROPRIETARY
 * ============================================================================
 * This file and its contents are the intellectual property of Comcast.  It may
 * not be used, copied, distributed or otherwise  disclosed in whole or in part
 * without the express written permission of Comcast.
 * ============================================================================
 * Copyright (c) 2013 Comcast. All rights reserved.
 * ============================================================================
 */

#if !defined(_RDK_DBG_PRIV_H)
#define _RDK_DBG_PRIV_H

#include <rdk_logger_types.h>
#include <rdk_debug.h>
#include <rdk_error.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** Maximum limit on the number of Modules logged*/

#define RDK_MAX_MOD_COUNT 512

/** This variable is described in rdk_dbg_priv_log.c - to be used by
 * rdk_debug_priv_.* files only.
 */
extern uint32_t rdk_g_logControlTbl[];
extern rdk_logger_Bool dbg_logViaUDP;

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
 * The <i>rdk_logger_env_add_conf_file()</i> function sets up the environment variable
 * storage by parsing configuration file.
 *
 * @param path Path of the file.
 * @return Returns relevant RDK_OSAL error code on failure, otherwise <i>RDK_SUCCESS</i>
 *          is returned.
 */
rdk_Error rdk_logger_env_add_conf_file(const char * path);

void 			rdk_dbg_priv_LogControlInit(void);
void 			rdk_dbg_priv_Init(void);
void rdk_dbg_priv_Shutdown();
const char * 	rdk_dbg_priv_LogControlOpSysIntf(char *logName, char *cfgStr);
const char * 	rdk_dbg_priv_LogQueryOpSysIntf(char *modName, char *cfgStr, int cfgStrMaxLen);
void rdk_debug_priv_log_msg( rdk_LogLevel level,
        int module, const char *module_name, const char* format, va_list args);

#ifdef __cplusplus
}
#endif

#endif /* _RDK_DBG_PRIV_H */

