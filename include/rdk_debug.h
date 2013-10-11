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

#ifndef _RDK_DEBUG_H_
#define _RDK_DEBUG_H_

#include <stdio.h>
#include "rdk_logger_types.h"
#include "rdk_error.h"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * These values represent the logging 'levels' or 'types', they are each
 * independent.
 */
typedef enum
{
    ENUM_RDK_LOG_BEGIN = 0, /**< Used as array index. */

    RDK_LOG_FATAL = ENUM_RDK_LOG_BEGIN,
    RDK_LOG_ERROR,
    RDK_LOG_WARN,
    RDK_LOG_NOTICE,
    RDK_LOG_INFO,
    RDK_LOG_DEBUG,

    RDK_LOG_TRACE1,
    RDK_LOG_TRACE2,
    RDK_LOG_TRACE3,
    RDK_LOG_TRACE4,
    RDK_LOG_TRACE5,
    RDK_LOG_TRACE6,
    RDK_LOG_TRACE7,
    RDK_LOG_TRACE8,
    RDK_LOG_TRACE9,

    ENUM_RDK_LOG_COUNT
} rdk_LogLevel;

/**
 * String names that correspond to the various logging types.
 * Note: This array *must* match the RDK_LOG_* enum.
 */
#ifdef RDK_DEBUG_DEFINE_STRINGS
const char *rdk_logLevelStrings[ENUM_RDK_LOG_COUNT] =
{
    "FATAL",
    "ERROR",
    "WARNING",
    "NOTICE",
    "INFO",
    "DEBUG",

    "TRACE1",
    "TRACE2",
    "TRACE3",
    "TRACE4",
    "TRACE5",
    "TRACE6",
    "TRACE7",
    "TRACE8",
    "TRACE9",
};
#else
extern const char *rdk_logLevelStrings[ENUM_RDK_LOG_COUNT];
#endif /* RDK_DEBUG_DEFINE_STRINGS */

/**
 * <i>rdk_dbgInit()</i> initializes the RDK debug manager module.  This API must
 * be called only once per boot cycle.
 */
void rdk_dbgInit();

rdk_Error rdk_logger_init(const char* debugConfigFile);

/**
 * <i>rdk_dbg_MsgRaw()</i> adds a log message. It is appended to the log output based
 *  on configurations set in the environment file.
 *
 * @param level Log level of the log message
 * @param module Module in which this message belongs to.
 * @param format Printf style string containing the log message.
 */
void rdk_dbg_MsgRaw(rdk_LogLevel level, const char *module,
        const char *format, ...);

/**
 * <i>rdk_dbg_enabled()</i> checks if a particular log is enabled.
 *
 * @param module Module of which log level shall be checked.
 * @param level Log level to be checked.
 */
rdk_logger_Bool rdk_dbg_enabled(const char *module, rdk_LogLevel level);

/**
 * Use RDK_LOG debug message as.
 * RDK_LOG (rdk_LogLevel level, const char *module, const char *format,...)
 * @param level Log level of the log message
 * @param module Module in which this message belongs to (Use module name same as mentioned in debug.ini)
 * @param format Printf style string containing the log message.
 *
 */
/**
 * @details Default log level entries for each modules are present in the debug.ini
 *  These entries are read at startup and can be modifiy/add as per the requirement.
 * @details Bydefault logs are redirected to /opt/logs/ocapri_log.txt. 
 * But these can be configure to capture logs for each component in separate files under 
 * /opt/logs/ by setting configuration parameter SEPARATE.LOG.SUPPORT as TRUE in 
 * debug.ini
 * @details Following logs files generated if SEPARATE.LOG.SUPPORT=TRUE
 *
 * For POD: pod_log.txt
 *
 * For CANH Daemon: canh_log.txt
 *
 * For RMFStreamer: rmfstr_log.txt
 *
 * For VOD client application: vod_log.txt
 *
 * For IARM: uimgr_log.txt
 */ 

#define RDK_LOG rdk_dbg_MsgRaw


#ifdef __cplusplus
}
#endif

#endif /* _RDK_DEBUG_H_ */
