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
 * @file rdk_debug.c
 * This source file contains the APIs for RDK debug.
 */

/**
* @defgroup rdk_logger
* @{
* @defgroup src
* @{
**/


#include <rdk_debug.h>
#include <rdk_debug_priv.h>

#include <string.h> // memset
#include <rdk_utils.h>
#include <stdarg.h>

extern int global_count;

/**
 * @brief Touch the file which can be used to check whether to log or not.
 *
 * @param[in] pszFile Character string representing name of the file to be created.
 * @return None.
 */
void TouchFile(const char * pszFile)
{
    if(NULL != pszFile)
    {
        FILE * fp = fopen(pszFile, "w");
        if(NULL != fp)
        {
            fclose(fp);
        }
    }
}

/**
 * @brief Dump the debug log. It will Dump all the current settings so that an analysis of a log
 * file will include what logging information to expect.
 *
 * @param[in] path Character string representing path of the temp file to be created.
 * @return None.
 */
void rdk_dbgDumpLog(const char* path)
{
    int mod, i;
    char config[128];
    const char *modptr = NULL;
    RDK_LOG(RDK_LOG_INFO, "LOG.RDK.OS", "\n");
    RDK_LOG(RDK_LOG_INFO, "LOG.RDK.OS",
            "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    RDK_LOG(RDK_LOG_INFO, "LOG.RDK.OS", "Stack level logging levels: \n");

    /**
     * Now just dump all the current settings so that an analysis of a log
     * file will include what logging information to expect
     */
    for (mod = 1; mod <= global_count; mod++) 
    {
        modptr = rdk_logger_envGetModFromNum(mod);
    
        memset(config, 0, sizeof(config));
        (void) rdk_dbg_priv_LogQueryOpSysIntf((char*) modptr, config, 127);
        RDK_LOG(RDK_LOG_INFO, "LOG.RDK.OS",
                "Initial Logging Level for %-10s: %s\n", modptr, config);
    }

    RDK_LOG(RDK_LOG_INFO, "LOG.RDK.OS",
            "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");
	TouchFile(path);
}

/**
 * @brief Initialize the underlying MPEOS debug support. This API must be called only once per boot cycle.
 * @return None.
 */
void rdk_dbgInit()
{
    static rdk_logger_Bool inited = FALSE;

    if (!inited)
    {
        rdk_dbg_priv_Init();
        inited = TRUE;
    }
}

/**
 * @brief Send a debugging message to the debugging window. It is appended to the log output based
 * on configurations set in the environment file.
 *
 * @param[in] level The debug logging level.
 * @param[in] module The name of the module for which this message belongs to, it is mentioned in debug.ini.
 * @param[in] format Printf style string containing the log message.
 */
void rdk_dbg_MsgRaw(rdk_LogLevel level, const char *module,
        const char *format, ...)
{
#if !defined(RDK_LOG_DISABLE)
    int num;
    va_list args;

    va_start(args, format);
    /** Get the registered value of module */
    num = rdk_logger_envGetNum(module);
    if(num < 0)
    {
		return;
	}
    rdk_debug_priv_log_msg( level, num, module, 
                format, args);
    va_end(args);
#endif /* RDK_LOG_DISABLE */
}

