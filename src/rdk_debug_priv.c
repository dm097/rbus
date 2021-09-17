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
 * @file rdk_debug_priv.c
 */


/**
* @defgroup rdk_logger
* @{
* @defgroup src
* @{
**/


#include <assert.h>
#include <stdio.h>
/*lint -e(451)*/
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>   
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "rdk_logger_types.h"
#include "rdk_error.h"
#define RDK_DEBUG_DEFINE_STRINGS
#include "rdk_debug_priv.h"
#include "rdk_dynamic_logger.h"
#include "log4c.h"
#include <rdk_utils.h>
#include "safec_library.h"

#ifdef SYSTEMD_JOURNAL
#include <systemd/sd-journal.h>
#endif //SYSTEMD_JOURNAL


#ifdef SYSTEMD_SYSLOG_HELPER
#include "syslog_helper_ifc.h"
#endif

/// Debugging messages are enabled.  Default is enabled (1) and 0 for off.
static int g_debugEnabled = 1;

extern int global_count;

/**
 * Returns 1 if logging has been requested for the corresponding module (mod)
 * and level (lvl) combination. To be used in rdk_dbg_priv_* files ONLY.
 */
#define WANT_LOG(mod, lvl) ( ( ((mod) >= 0) && ((mod) < RDK_MAX_MOD_COUNT) ) ? (rdk_g_logControlTbl[(mod)] & (1 << (lvl))) : 0 )


/** Skip whitespace in a c-style string. */
#define SKIPWHITE(cptr) while ((*cptr != '\0') && isspace(*cptr)) cptr++

/** Bit mask for trace (TRACE1 thru TRACE9) logging levels. */
#define LOG_TRACE ( (1 << RDK_LOG_TRACE1) | (1 << RDK_LOG_TRACE2) | (1 << RDK_LOG_TRACE3) \
 | (1 << RDK_LOG_TRACE4) | (1 << RDK_LOG_TRACE5) | (1 << RDK_LOG_TRACE6) \
 | (1 << RDK_LOG_TRACE7) | (1 << RDK_LOG_TRACE8) | (1 << RDK_LOG_TRACE9) )

/** Turns on FATAL, ERROR, WARN NOTICE & INFO. */
#define LOG_ALL (  (1 << RDK_LOG_FATAL) | (1 << RDK_LOG_ERROR) | \
                   (1 << RDK_LOG_WARN)  | (1 << RDK_LOG_INFO)  | \
                   (1 << RDK_LOG_NOTICE))

/** All logging 'off'. */
#define LOG_NONE 0

#define HOSTADDR_STR_MAX 255


#define MAX_LOGLINE_LENGTH 4096

static int initLogger(char *category);

static const char* dated_format_nocr(const log4c_layout_t* a_layout,
        const log4c_logging_event_t*a_event);
static const char* basic_format_nocr(const log4c_layout_t* a_layout,
        const log4c_logging_event_t*a_event);
static const char* comcast_dated_format_nocr(const log4c_layout_t* a_layout,
        const log4c_logging_event_t*a_event);
static int stream_env_overwrite_open(log4c_appender_t * appender);
static int stream_env_append_open(log4c_appender_t * appender);
static int stream_env_append(log4c_appender_t* appender, const log4c_logging_event_t* event);
static int stream_env_plus_stdout_append(log4c_appender_t* appender, const log4c_logging_event_t* event);
static int stream_env_close(log4c_appender_t * appender);

/* GLOBALS */

static rdk_logger_Bool g_initialized = FALSE;


/** Global variable used to control logging. */
uint32_t rdk_g_logControlTbl[RDK_MAX_MOD_COUNT];

/** UDP logging variables. */
rdk_logger_Bool dbg_logViaUDP = FALSE;
int dbg_udpSocket;
struct sockaddr_in dbg_logHostAddr;

log4c_category_t* stackCat = NULL;

enum
{
    /** Used as an array index. */
    ERR_INVALID_MOD_NAME = 0, ERR_INVALID_LOG_NAME
};

enum
{
    RC_ERROR, RC_OK
};

static const char *errorMsgs[] =
{ "Error: Invalid module name.", "Warning: Ignoring invalid log name(s)." };


/**
 * Initialize Debug API.
 */
static log4c_category_t* defaultCategory = NULL;
static log4c_category_t* glibCategory = NULL;

static const log4c_layout_type_t log4c_layout_type_dated_nocr =
{ "dated_nocr", dated_format_nocr, };

static const log4c_layout_type_t log4c_layout_type_basic_nocr =
{ "basic_nocr", basic_format_nocr, };

static const log4c_layout_type_t log4c_layout_type_comcast_dated_nocr =
{ "comcast_dated_nocr", comcast_dated_format_nocr, };

static const log4c_appender_type_t
        log4c_appender_type_stream_env =
        { "stream_env", stream_env_overwrite_open, stream_env_append,
                stream_env_close, };

static const log4c_appender_type_t log4c_appender_type_stream_env_append =
{ "stream_env_append", stream_env_append_open, stream_env_append,
        stream_env_close, };

static const log4c_appender_type_t log4c_appender_type_stream_env_plus_stdout =
{ "stream_env_plus_stdout", stream_env_overwrite_open,
        stream_env_plus_stdout_append, stream_env_close, };

static const log4c_appender_type_t log4c_appender_type_stream_env_append_plus_stdout =
{ "stream_env_append_plus_stdout", stream_env_append_open,
        stream_env_plus_stdout_append, stream_env_close, };

#define NUM_OF_LOGGER_TYPES (sizeof(log_type_logger_table)/sizeof(log_type_logger_table[0]))

enum LogType_Logger_e {
    ALL,
    NONE,
    TRACE,
    NOT_TRACE
};

typedef struct LogType_Pair_For_Logger{
  char                 *name;
  enum LogType_Logger_e   type;
} LOGTYPE_PAIR_FOR_LOGGER;

LOGTYPE_PAIR_FOR_LOGGER log_type_logger_table[] = {
  {"ALL",    ALL   },
  {"NONE",   NONE  },
  {"TRACE",  TRACE },
  {"!TRACE", NOT_TRACE  },
};

static int get_log_type_from_name(char *name, enum LogType_Logger_e *type_ptr)
{
  errno_t rc = -1;
  int ind = -1;
  int i = 0;
  size_t strsize = 0;

  if((name == NULL) || (type_ptr == NULL))
     return 0;

  strsize = strlen(name);

  for (i = 0 ; i < NUM_OF_LOGGER_TYPES ; ++i)
  {
      rc = strcmp_s(name, strsize, log_type_logger_table[i].name, &ind);
      ERR_CHK(rc);
      if((rc == EOK) && (!ind))
      {
          *type_ptr = log_type_logger_table[i].type;
          return 1;
      }
  }
  return 0;
}

void rdk_dbg_priv_Init()
{
    const char* envVar;

    if (initLogger("RI"))
    {
        fprintf(stderr, "%s -- initLogger failure?!\n", __FUNCTION__);
    }

    stackCat = log4c_category_get("RI.Stack");

    rdk_dbg_priv_LogControlInit();

    /* Try to get logging option. */
    envVar = rdk_logger_envGet("EnableMPELog");
    if (NULL != envVar)
    {
        g_debugEnabled = (strcasecmp(envVar, "TRUE") == 0);
    }
}

/**
 * Safely force a string to uppercase. I hate this mundane rubbish.
 *
 * @param token String to be forced to uppercase.
 */
static void forceUpperCase(char *token)
{
    while (*token)
    {
        if (islower(*token))
        {
            *token = (char) toupper(*token);
        }
        token++;
    }
}

/**
 * Convert a log level name to the correspodning log level enum value.
 *
 * @param name Log level name, which must be uppercase.
 * @param Corresponding enumeration value or -1 on error.
 */
static int logNameToEnum(const char *name)
{
    int i = 0;
    errno_t rc = -1;
    int ind = -1;

    if(name == NULL)
      return -1;

    int length = strlen(name);

    while (i < ENUM_RDK_LOG_COUNT)
    {
        rc = strcmp_s(name, length, rdk_logLevelStrings[i], &ind);
        ERR_CHK(rc);
        if((rc == EOK) && (!ind))
        {
            return i;
        }
        i++;
    }

    return -1;
}

/**
 * Extract a whitespace delimited token from a string.
 *
 * @param srcStr Pointer to the source string, this will be modified
 * to point to the first character after the token extracted.
 *
 * @param tokBuf This is a string that will be filled with the
 * token. Note: this buffer is assumed to be large enough to hold the
 * largest possible token.
 */
static void extractToken(const char **srcStr, char *tokBuf)
{
    const char *src = *srcStr;
    while (*src && !isspace(*src))
    {
        *tokBuf++ = *src++;
    }
    *tokBuf = '\0';
    *srcStr = src;
}

/**
 * Parse a whitespace delimited list of log types and log type meta names.
 *
 * @param cfgStr String containing one more log types. (Right hand
 * side of INI file entry.)
 *
 * @param defConfig Default configuration to base the return value on.
 *
 * @return Returns a bit mask that can be used as an entry in
 * #rdk_g_logControlTbl.
 */
static int parseLogConfig(const char *cfgStr, uint32_t *configEntry,
        const char **msg)
{
    uint32_t config = *configEntry;
    char logTypeName[128] = { 0 };
    int rc = RC_OK;
    errno_t safec_rc = -1;
    enum LogType_Logger_e   type;

    *msg = "";

    SKIPWHITE(cfgStr)
        ;
    if (*cfgStr == '\0')
    {
        *msg = "Warning: Empty log level confguration.";
        return RC_ERROR;
    }

    while (*cfgStr != '\0')
    {
        /* Extract and normalise log type name token. */

        safec_rc = memset_s(logTypeName,sizeof(logTypeName), 0, sizeof(logTypeName));
        ERR_CHK(safec_rc);
        extractToken(&cfgStr, logTypeName);
        forceUpperCase(logTypeName);

        /* Handle special meta names. */
        if(get_log_type_from_name(logTypeName, &type)) {

        if (type  == ALL)
        {
            config |= LOG_ALL;
        }
        else if (type == NONE )
        {
            config = LOG_NONE;
        }
        else if (type == TRACE )
        {
            config |= LOG_TRACE;
        }
        else if (type == NOT_TRACE)
        {
            config &= ~LOG_TRACE;
        }
       }
        else
        {
            /* Determine the corresponding bit for the log name. */
            int invert = 0;
            const char *name = logTypeName;
            int logLevel = -1;

            if (logTypeName[0] == '!')
            {
                invert = 1;
                name = &logTypeName[1];
            }

            logLevel = logNameToEnum(name);
            if (logLevel != -1)
            {
                if (invert)
                {
                    config &= ~(1 << logLevel);
                }
                else
                {
                    config |= (1 << logLevel);
                }
            }
            else
            {
                *msg = errorMsgs[ERR_INVALID_LOG_NAME];
                rc = RC_ERROR;
            }
        }

        SKIPWHITE(cfgStr)
            ;
    }

    *configEntry = config;
    return rc;
}

static void printTime(const struct tm *pTm, char *pBuff)
{
    errno_t rc = -1;
    /* Here pBuff size is 40 bytes from calling function */
    rc = sprintf_s(pBuff,40,"%02d%02d%02d-%02d:%02d:%02d",pTm->tm_year + 1900 - 2000, pTm->tm_mon + 1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
    if(rc < EOK)
    {
        ERR_CHK(rc);
    }
}

/*****************************************************************************
 *
 * EXPORTED FUNCTIONS
 *
 ****************************************************************************/

/**
 * Initialize the debug log control table. This should be called from
 * the initialization routine of the debug manager.
 */
void rdk_dbg_priv_LogControlInit(void)
{
    char envVarName[128] =
    { 0 };
    const char *envVarValue = NULL;
    uint32_t defaultConfig = 0;
    int mod = 0;
    const char *msg = "";
    errno_t rc = -1;

    /** Pre-condition the control table to disable all logging.  This
     * means that if no logging control statements are present in the
     * debug.ini file all logging will be suppressed. */
    rc = memset_s(rdk_g_logControlTbl,sizeof(rdk_g_logControlTbl), 0, sizeof(rdk_g_logControlTbl));
    ERR_CHK(rc);

    /** Intialize to the default configuration for all modules. */
    rc = strcpy_s(envVarName,sizeof(envVarName), "LOG.RDK.DEFAULT");
    ERR_CHK(rc);
    envVarValue = rdk_logger_envGet(envVarName);
    if ((envVarValue != NULL) && (envVarValue[0] != 0))
    {
        (void) parseLogConfig(envVarValue, &defaultConfig, &msg);
        for (mod = 1; mod <= global_count; mod++) 
        {
            rdk_g_logControlTbl[mod] = defaultConfig;
        }
    }

    /** Configure each module from the ini file. Note: It is not an
     * error to have no entry in the ini file for a module - we simply
     * leave it at the default logging. */
    for (mod = 1; mod <= global_count; mod++)
    {
       /** Get the logging level */
        envVarValue = rdk_logger_envGetValueFromNum(mod);
        if ((envVarValue != NULL) && (envVarValue[0] != '\0'))
        {
            (void) parseLogConfig(envVarValue, &rdk_g_logControlTbl[mod],
                    &msg);
        }
    }
}

/**
 * @brief Function to check if a specific log level of a module is enabled.
 *
 * @param[in] module The module name or category for for which the log level shall be checked (as mentioned in debug.ini).
 * @param[in] level The debug logging level.
 *
 * @return Returns true, if debug log level enabled successfully else returns false.
 */
rdk_logger_Bool rdk_dbg_enabled(const char *module, rdk_LogLevel level)
{
        int number = rdk_logger_envGetNum(module); 
    if (WANT_LOG(number, level))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 * Modify the debug log control table.
 */
void rdk_dbg_priv_SetLogLevelString(const char* pszModuleName, const char* pszLogLevels)
{
    const char *envVarName;
    uint32_t defaultConfig = 0;
    int mod = 0;
    const char *msg = "";
    errno_t rc = -1;
    int ind = -1;

    if ((pszModuleName != NULL) && (pszLogLevels != NULL))
    {
        int length = strlen(pszModuleName);
        /* Intialize to the default configuration for all modules. */
        rc = strcmp_s(pszModuleName, length, "LOG.RDK.DEFAULT", &ind);
        ERR_CHK(rc);
        if((rc == EOK) && (!ind))
        {
            (void) parseLogConfig(pszLogLevels, &defaultConfig, &msg);
            for (mod = 1; mod <= global_count; mod++)
            {
                rdk_g_logControlTbl[mod] = defaultConfig;
            }
            printf("rdk_dbg_priv_SetLogLevelString: Set Logging Level for '%s' to '%s'\n", pszModuleName, pszLogLevels);
            return;
        }

        /* Configure each module from the ini file. Note: It is not an
         * error to have no entry in the ini file for a module - we simply
         * leave it at the default logging. */
        for (mod = 1; mod <= global_count; mod++)
        {
            envVarName = rdk_logger_envGetModFromNum(mod);
            rc = strcmp_s(pszModuleName, length, envVarName, &ind);
            ERR_CHK(rc);
            if((rc == EOK) && (!ind))
            {
                if ((pszLogLevels != NULL) && (pszLogLevels[0] != '\0'))
                {
                    (void) parseLogConfig(pszLogLevels, &rdk_g_logControlTbl[mod], &msg);
                    printf("rdk_dbg_priv_SetLogLevelString: Set Logging Level for '%s' to '%s'\n", pszModuleName, pszLogLevels);
                    return;
                }
            }
        }
    }
    printf("rdk_dbg_priv_SetLogLevelString: Failed to set Logging Level for '%s' to '%s'\n", pszModuleName, pszLogLevels);
}



/**
 * Provides an interface to query the configuration of logging in a
 * specific module as a user readable string. Note: the mimimum
 * acceptable length of the supplied configuration buffer is 32 bytes.
 *
 * @param modName Name of the module.
 *
 * @param cfgStr The configuration strng: which should be space
 * separated module names.
 *
 * @param cfgStrMaxLen The maximum length of the configuration string
 * to be returned including the NUL character.
 *
 * @return A string containing a user readable error message if an
 * error occured; or "OK" upon success.
 */
const char * rdk_dbg_priv_LogQueryOpSysIntf(char *modName, char *cfgStr,
        int cfgStrMaxLen)
{
    char *name = modName;
    int mod = -1;
    uint32_t modCfg = 0;
    int level = -1;
    errno_t rc = -1;

    assert(modName);
    assert(cfgStr);
    assert(cfgStrMaxLen > 32);

    cfgStrMaxLen -= 1; /**< Ensure there is space for NUL. */
    cfgStr[0] = '\0';

    /** Get the module configuration. Note: DEFAULT is not valid as it
     * is an alias. 
     */
    forceUpperCase(name);
     mod = rdk_logger_envGetNum(modName); 
    if (mod < 0)
    {
        return "Unknown module specified.";
    }
    modCfg = rdk_g_logControlTbl[mod];

    /** Try and find a succinct way of describing the configuration. */

    if (modCfg == 0)
    {
        rc = strcpy_s(cfgStr, cfgStrMaxLen, "NONE");
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return "ERROR: strcpy_s failed";
        }

        return "OK"; /* This is a canonical response. */
    }

#if 0 /* BAT: print out the level details instead of just these abbreviations */

    /* Look for appropriate abberviations. */

    if ((modCfg & LOG_ALL) == LOG_ALL)
    {
        strcpy(cfgStr, " ALL");
        modCfg &= ~LOG_ALL;
    }

    if ((modCfg & LOG_TRACE) == LOG_TRACE)
    {
        strcat(cfgStr, " TRACE");
        modCfg &= ~LOG_TRACE;
    }
#endif /* BAT */

    /** Loop through the control word and print out the enabled levels. */

    for (level = 0; level < ENUM_RDK_LOG_COUNT; level++)
    {
        if (modCfg & (1 << level))
        {
            /** Stop building out the config string if it would exceed
             * the buffer length. 
             */
            if (strlen(cfgStr) + strlen(rdk_logLevelStrings[level])
                    > (size_t) cfgStrMaxLen)
            {
                return "Warning: Config string too long, config concatenated.";
            }

            /* Not efficient - rah rah. */
            rc = sprintf_s(cfgStr, cfgStrMaxLen, " %s", rdk_logLevelStrings[level]);
            if(rc < EOK)
            {
               ERR_CHK(rc);
               return "ERROR: sprintf_s failed";
            }
        }
    }

    return "OK";
}

void rdk_debug_priv_log_msg( rdk_LogLevel level,
        int module, const char *module_name, const char* format, va_list args)
{
    /** Get the category from module name */
    static log4c_category_t *cat_cache[RDK_MAX_MOD_COUNT] = {NULL};
    char cat_name[64] = {'\0'};
    log4c_category_t* cat = NULL;
    errno_t rc = -1;

    /* Handling process request here. This is not a blocking call and it shall return immediately */
    rdk_dyn_log_processPendingRequest();

    if (!g_debugEnabled || !WANT_LOG(module, level))
    {
        return;
    }

    char *parent_cat_name = (char *)log4c_category_get_name(stackCat);
    rc = sprintf_s(cat_name, sizeof(cat_name), "%s.%s", (parent_cat_name == NULL ? "" : parent_cat_name), module_name);
    if(rc < EOK)
    {
        ERR_CHK(rc);
    }

    if((module >= 0) && (module < RDK_MAX_MOD_COUNT))
    {
        if (cat_cache[module] == NULL) {
            /** Only doing a read here, lock not needed */
            cat_cache[module] = log4c_category_get(cat_name);
        }
    
        cat = cat_cache[module];
    }
    else
    {
        cat = log4c_category_get(cat_name);
    }

    switch (level)
    {
    case RDK_LOG_FATAL:
        log4c_category_vlog(cat, LOG4C_PRIORITY_FATAL, format, args);
        break;
    case RDK_LOG_ERROR:
        log4c_category_vlog(cat, LOG4C_PRIORITY_ERROR, format, args);
        break;
    case RDK_LOG_WARN:
        log4c_category_vlog(cat, LOG4C_PRIORITY_WARN, format, args);
        break;
    case RDK_LOG_NOTICE:
        log4c_category_vlog(cat, LOG4C_PRIORITY_NOTICE, format, args);
        break;
    case RDK_LOG_INFO:
        log4c_category_vlog(cat, LOG4C_PRIORITY_INFO, format, args);
        break;
    case RDK_LOG_DEBUG:
        log4c_category_vlog(cat, LOG4C_PRIORITY_DEBUG, format, args);
        break;
    case RDK_LOG_TRACE1:
    case RDK_LOG_TRACE2:
    case RDK_LOG_TRACE3:
    case RDK_LOG_TRACE4:
    case RDK_LOG_TRACE5:
    case RDK_LOG_TRACE6:
    case RDK_LOG_TRACE7:
    case RDK_LOG_TRACE8:
    case RDK_LOG_TRACE9:
        log4c_category_vlog(cat, LOG4C_PRIORITY_TRACE, format, args);
        break;
    default:
        log4c_category_vlog(cat, LOG4C_PRIORITY_DEBUG, format, args);
        break;
    }
}


void RDK_LOG_ControlCB(const char *moduleName, const char *subComponentName, const char *loggingLevel, int log_status)
{
    char logTypeName[20] = {0};

    if ((NULL == moduleName) || (NULL == loggingLevel))
    {
        return;
    }

    strncpy(logTypeName, loggingLevel, sizeof(logTypeName));
    if (logTypeName[0] == '~')
    {
        logTypeName[0] = '!';
    }

    rdk_dbg_priv_SetLogLevelString(moduleName, (const char *)logTypeName);
}


/*************************Copied from ri_log.c******************/

/**
 * Instantiate the logger...
 * @param cat The category string for this instance logging/filtering
 * @return the result of the instantiation
 */
static int initLogger(char *category)
{

    ///> These must be set before calling log4c_init so that the log4crc file
    ///> will configure them
    (void) log4c_appender_type_set(&log4c_appender_type_stream_env);
    (void) log4c_appender_type_set(&log4c_appender_type_stream_env_append);
    (void) log4c_appender_type_set(&log4c_appender_type_stream_env_plus_stdout);
    (void) log4c_appender_type_set(
            &log4c_appender_type_stream_env_append_plus_stdout);
    (void) log4c_layout_type_set(&log4c_layout_type_dated_nocr);
    (void) log4c_layout_type_set(&log4c_layout_type_basic_nocr);
    (void) log4c_layout_type_set(&log4c_layout_type_comcast_dated_nocr);

    if (log4c_init())
    {
        fprintf(stderr, "log4c_init() failed?!");
        return -1;
    }
    else
    {
        defaultCategory = log4c_category_get(category);
        glibCategory = log4c_category_get("RI.GLib");
    }

    return 0;
}


/****************************************************************
 * Dated layout with no ending carriage return / line feed
 */
static const char* dated_format_nocr(const log4c_layout_t* layout,
        const log4c_logging_event_t* event)
{
    struct tm tm;
    char timeBuff[40] = {0};
    errno_t rc = -1;
    //localtime_r(&event->evt_timestamp.tv_sec, &tm); /* Use the UTC Time for logging */
    gmtime_r(&event->evt_timestamp.tv_sec, &tm);

    printTime(&tm,timeBuff);

    rc = sprintf_s(event->evt_buffer.buf_data, event->evt_buffer.buf_size,
            "%s.%03ld %-8s %s- %s", timeBuff,
            event->evt_timestamp.tv_usec / 1000, log4c_priority_to_string(
                    event->evt_priority), event->evt_category,
            event->evt_msg);
    if(rc < EOK)
    {
        ERR_CHK(rc);
    }
    if (event->evt_buffer.buf_size > 0 && event->evt_buffer.buf_data != NULL)
    {
        event->evt_buffer.buf_data[event->evt_buffer.buf_size - 1] = 0;
    }
    return event->evt_buffer.buf_data;
}

/****************************************************************
 * Basic layout with no ending carriage return / line feed
 */
static const char* basic_format_nocr(const log4c_layout_t* layout,
        const log4c_logging_event_t* event)
{
    errno_t rc = -1;
    rc = sprintf_s(event->evt_buffer.buf_data, event->evt_buffer.buf_size, "%-8s %s - %s",
            log4c_priority_to_string(event->evt_priority),
            event->evt_category, event->evt_msg);
    if(rc < EOK)
    {
        ERR_CHK(rc);
    }

    if (event->evt_buffer.buf_size > 0 && event->evt_buffer.buf_data != NULL)
    {
        event->evt_buffer.buf_data[event->evt_buffer.buf_size - 1] = 0;
    }

    return event->evt_buffer.buf_data;
}

static const char* comcast_dated_format_nocr(const log4c_layout_t* layout,
        const log4c_logging_event_t*event)
{
    struct tm tm;
    char timeBuff[40] = {0};
    errno_t rc = -1;
    //localtime_r(&event->evt_timestamp.tv_sec, &tm);  /* Use the UTC Time for logging */
    gmtime_r(&event->evt_timestamp.tv_sec, &tm);

    printTime(&tm,timeBuff);

    /** Get the last part of the cagetory as "module" */
    char *p= (char *)(event->evt_category);
    if (NULL == p)
    {
        p = (char*)"UNKNOWN";
    }
    else
    {
        int len = strlen(p);
        if ( len > 0 && *p != '.' && *(p+len-1) !='.')
        {
            p = p + len - 1;
            while (p != (char *)(event->evt_category) && *p != '.') p--;
            if (*p == '.') p+=1;

        }
        else 
        {
            p = (char*)"UNKNOWN";
        }
    }

    rc = sprintf_s(event->evt_buffer.buf_data, event->evt_buffer.buf_size,
            "%s.%06ld [mod=%s, lvl=%s] [tid=%ld] %s",timeBuff,
            event->evt_timestamp.tv_usec,
            p, log4c_priority_to_string(event->evt_priority), syscall(SYS_gettid),
            event->evt_msg);
    if(rc < EOK)
    {
        ERR_CHK(rc);
    }
    if (event->evt_buffer.buf_size > 0 && event->evt_buffer.buf_data != NULL)
    {
        event->evt_buffer.buf_data[event->evt_buffer.buf_size - 1] = 0;
    }
    return event->evt_buffer.buf_data;
}
/****************************************************************
 * Stream layout that will parse environment variables from the
 * stream name (env vars have a leading "$(" and end with )"
 */
static int stream_env_open(log4c_appender_t* appender, int append)
{
    FILE* fp = (FILE*)log4c_appender_get_udata(appender);
    char* name = strdup(log4c_appender_get_name(appender));
    int nameLen = strlen(name);
    char* temp = name;
    char *varBegin, *varEnd;
    char* envVar;
    const int MAX_VAR_LEN = 1024;
    char newName[MAX_VAR_LEN+1];
    int newNameLen = 0;
    errno_t rc = -1;
    int ind = -1;

    if (fp)
    {
        free(name); /*RDKB-7467, CID-24968, free unused resources*/
        return 0;
    }

    newName[0] = '\0';

    ///> Parse any environment variables
    while ((varBegin = strchr(temp,'$')) != NULL)
    {
        ///> Search for opening and closing parens
        if (((varBegin - name + 1) >= nameLen) || (*(varBegin+1) != '('))
        goto parse_error;

        ///> Append characters up to this point to the new name
        rc = strncat_s(newName,sizeof(newName), temp, varBegin-temp);
        ERR_CHK(rc);
        newNameLen += varBegin-temp;
        if (newNameLen > MAX_VAR_LEN)
        goto length_error;

        varBegin += 2; ///> start of env var name

        if ((varEnd = strchr(varBegin,')')) == NULL)
        goto parse_error;

        *varEnd = '\0';
        if ((envVar = getenv(varBegin)) == NULL)
        goto parse_error;

        ///> Append env var value to the new name
        rc = strncat_s(newName,sizeof(newName), envVar, strlen(envVar));
        ERR_CHK(rc);
        newNameLen += strlen(envVar);
        if (newNameLen >MAX_VAR_LEN)
        goto length_error;

        temp = varEnd + 1;
    }

    ///> Append remaining characters
    rc = strncat_s(newName,sizeof(newName), temp, (name + nameLen) - temp);
    ERR_CHK(rc);
    newNameLen += (name + nameLen) - temp;
    if (newNameLen >MAX_VAR_LEN)
    goto length_error;

    free(name);

    rc = strcmp_s("stderr", strlen("stderr"), newName, &ind);
    ERR_CHK(rc);
    if((rc == EOK) && (!ind)){
    fp = stderr;
    }else {
    rc = strcmp_s("stdout", strlen("stdout"), newName, &ind);
    ERR_CHK(rc);
    if((rc == EOK) && (!ind)){
    fp = stdout;
    }
    else if (append)
    {
        printf("****Opening %s in append mode\n", newName);
        if ((fp = fopen(newName, "a")) == NULL)
        return -1;
    }
    else
    {
        printf("****Opening %s in write mode\n", newName);
        if ((fp = fopen(newName, "w")) == NULL)
        return -1;
    }
   }

    /**> unbuffered mode */
    setbuf(fp, NULL);

    (void)log4c_appender_set_udata(appender, fp);
    return 0;

    parse_error:
    fprintf(stderr, "*(*(*(*( log4c appender stream_env, %s -- Illegal env var name or format! %s\n",
            __FUNCTION__, name);
    (void)fflush(stderr);
    free(name);
    return -1;

    length_error:
    fprintf(stderr, "*(*(*(*( log4c appender stream_env, %s -- Path is too long! %s\n",
            __FUNCTION__, name);
    (void)fflush(stderr);
    free(name);
    return -1;
}

static int stream_env_overwrite_open(log4c_appender_t* appender)
{
    return stream_env_open(appender, 0);
}

static int stream_env_append_open(log4c_appender_t* appender)
{
    return stream_env_open(appender, 1);
}

#ifdef SYSTEMD_JOURNAL
static int stream_env_append_get_priority(int log4c_pr)
{
    int priority;
    switch(log4c_pr)
    {
    case LOG4C_PRIORITY_FATAL:
        priority = LOG_EMERG;
        break;
    case LOG4C_PRIORITY_ERROR:
        priority = LOG_ERR;
        break;
    case LOG4C_PRIORITY_WARN:
        priority = LOG_WARNING;
        break;
    case LOG4C_PRIORITY_NOTICE:
        priority = LOG_NOTICE;
        break;
    case LOG4C_PRIORITY_INFO:
        priority = LOG_INFO;
        break;
    case LOG4C_PRIORITY_DEBUG:
    case LOG4C_PRIORITY_TRACE:
    default:
        priority = LOG_DEBUG;
        break;
    }
    return priority;
}
#endif

static int stream_env_append(log4c_appender_t* appender,
        const log4c_logging_event_t* event)
{
    int retval=0;
    FILE* fp = (FILE*)log4c_appender_get_udata(appender);

#if defined(SYSTEMD_SYSLOG_HELPER)
    send_logs_to_syslog(event->evt_rendered_msg);
#elif defined(SYSTEMD_JOURNAL)
    if (fp == stdout || fp == stderr)
    {
        retval = sd_journal_print(stream_env_append_get_priority(event->evt_priority), "%s",event->evt_rendered_msg);
    }
    else
    {
        retval = fprintf(fp, "%s", event->evt_rendered_msg);
        (void)fflush(fp);
    }
#else
    retval = fprintf(fp, "%s", event->evt_rendered_msg);
    (void)fflush(fp);
#endif

    //free((void *)event->evt_rendered_msg);

    return retval;
}

static int stream_env_plus_stdout_append(log4c_appender_t* appender,
        const log4c_logging_event_t* event)
{
    int retval=0;
    FILE* fp = (FILE*)log4c_appender_get_udata(appender);

#if defined(SYSTEMD_SYSLOG_HELPER)
        send_logs_to_syslog(event->evt_rendered_msg);
#elif defined(SYSTEMD_JOURNAL)
    if (fp != stdout || fp != stderr)
    {
       retval = fprintf(fp, "%s", event->evt_rendered_msg);
    }
    else
    {
       retval = sd_journal_print(stream_env_append_get_priority(event->evt_priority), "%s",event->evt_rendered_msg);
    }
    (void)fflush(fp);
#else
    retval = fprintf(fp, "%s", event->evt_rendered_msg);
    fprintf(stdout, "%s", event->evt_rendered_msg);
    (void)fflush(fp);
    (void)fflush(stdout);
#endif
    //free((void *)event->evt_rendered_msg);

    return retval;
}


static int stream_env_close(log4c_appender_t* appender)
{
    FILE* fp = (FILE*)log4c_appender_get_udata(appender);

    if (!fp || fp == stdout || fp == stderr)
    return 0;

    return fclose(fp);
}


/*************************End Copied from ri_log.c******************/

