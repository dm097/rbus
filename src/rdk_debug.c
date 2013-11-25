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

#include <rdk_debug.h>
#include <rdk_debug_priv.h>

#include <string.h> // memset
#include <rdk_utils.h>
#include <stdarg.h>

extern int global_count;

/**
 * Initialize the underlying MPEOS debug support.
 */
void rdk_dbgInit()
{
    int mod, i;
    char config[128];
    const char *modptr = NULL;
    static rdk_logger_Bool inited = FALSE;

    if (!inited)
    {
        rdk_dbg_priv_Init();
        inited = TRUE;
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
    }
}

/**
 * Send a debugging message to the debugging window.
 *
 * @param level The debug logging level.
 * @param module The module name or category for debug output (as mentioned in debug.ini).
 * @param format The <i>printf</i> string describing the message.  This can be followed
 *               by 0 or more arguments to the <i>printf</i> format string.
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

    rdk_debug_priv_log_msg( level, num, module, 
                format, args);
    va_end(args);
#endif /* RDK_LOG_DISABLE */
}

