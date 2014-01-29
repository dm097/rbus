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

#include <assert.h>
#include <stdio.h>
/*lint -e(451)*/
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>   
#include <sys/socket.h>
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
#include "log4c.h"
#include <glib.h>
#include <rdk_utils.h>


/// Debugging messages are enabled.  Default is enabled (1) and 0 for off.
static int g_debugEnabled = 1;

extern int global_count;

/**
 * Returns 1 if logging has been requested for the corresponding module (mod)
 * and level (lvl) combination. To be used in rdk_dbg_priv_* files ONLY.
 */
#define WANT_LOG(mod, lvl) (rdk_g_logControlTbl[(mod)] & (1 << (lvl)))


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

/** The default port if one is not supplied via the socket.host file. */
#define SOCKET_PORT 51400

#define SO_MAX_MSG_SIZE (64*1024)
#define MAX_COMPUTERNAME_LENGTH 80
#define INVALID_SOCKET -1

#define SOCKET_DGRAM_SIZE 4096

#define MAX_LOGLINE_LENGTH 4096


typedef struct sockaddr SOCKADDR_IN;
typedef int SOCKET;

static int initLogger(char *category);
static void socket_open_log(const char* ident, int option);
static void socket_close_log();
static void socket_append_msg(char* fmt, ...);
static void vsocket_append_msg(char* fmt, va_list ap);
static const char* set_socket_conf_dir(const char* dir);
static const char* dated_format_nocr(const log4c_layout_t* a_layout,
        const log4c_logging_event_t*a_event);
static const char* basic_format_nocr(const log4c_layout_t* a_layout,
        const log4c_logging_event_t*a_event);
static const char* comcast_dated_format_nocr(const log4c_layout_t* a_layout,
        const log4c_logging_event_t*a_event);
static int stream_env_overwrite_open(log4c_appender_t * appender);
static int stream_env_append_open(log4c_appender_t * appender);
static int stream_env_append(log4c_appender_t* appender, const log4c_logging_event_t* a_event);
static int stream_env_plus_stdout_append(log4c_appender_t* appender, const log4c_logging_event_t* a_event);
static int stream_env_close(log4c_appender_t * appender);
static int socket_env_open(log4c_appender_t * appender);
static int socket_env_append(log4c_appender_t* appender, const log4c_logging_event_t* a_event);
static int socket_env_close(log4c_appender_t * appender);

/* GLOBALS */

/** Define SOCKET_CONF_DIR, the default location where socket.host should exist. */
#ifndef SOCKET_CONF_DIR
static const char *g_socket_conf_dir = ".";
#else
static const char *g_socket_conf_dir = SOCKET_CONF_DIR;
#endif

static rdk_logger_Bool g_initialized = FALSE;
static struct sockaddr_in g_sa_logger;

static SOCKET g_socket;

static char g_local_hostname[MAX_COMPUTERNAME_LENGTH + 1];
static char g_datagramm[SOCKET_DGRAM_SIZE];
static size_t g_datagramm_size;

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

static const log4c_appender_type_t log4c_appender_type_socket_env =
{ "socket_env", socket_env_open, socket_env_append, socket_env_close, };

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
    while (i < ENUM_RDK_LOG_COUNT)
    {
        if (strcmp(name, rdk_logLevelStrings[i]) == 0)
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
    char logTypeName[128] =
    { 0 };
    int rc = RC_OK;

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

        memset(logTypeName, 0, sizeof(logTypeName));
        extractToken(&cfgStr, logTypeName);
        forceUpperCase(logTypeName);

        /* Handle special meta names. */

        if (strcmp(logTypeName, "ALL") == 0)
        {
            config |= LOG_ALL;
        }
        else if (strcmp(logTypeName, "NONE") == 0)
        {
            config = LOG_NONE;
        }
        else if (strcmp(logTypeName, "TRACE") == 0)
        {
            config |= LOG_TRACE;
        }
        else if (strcmp(logTypeName, "!TRACE") == 0)
        {
            config &= ~LOG_TRACE;
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

/*
 * Initialize udp logging support based on the configuration string
 * provided in "debug.ini".
 *
 * Returns FALSE if either the configuration is invalid or any system
 * call used to initialize the networking support fails.
 */
static rdk_logger_Bool createUDPSocket(const char *cfgStr)
{
    long port = 0;
    char hostaddr[HOSTADDR_STR_MAX + 1];
    const char *portStr = NULL;
    char *strEnd = NULL;
    int status = 0;

    portStr = strchr(cfgStr, ':');
    if (portStr == NULL)
    {
        /* no colon = no port number */
        printf("*** invalid configuration no colon!\n");
        return FALSE;
    }

    memset(hostaddr, 0, sizeof(hostaddr));
    strncpy(hostaddr, cfgStr, portStr - cfgStr);
    printf("*** hostaddr = '%s'\n", hostaddr);
    ++portStr; /**< skip past ':' */
    if (*portStr == 0)
    {
        /** there was a colon but no port specified */
        printf("*** no port specified\n");
        return FALSE;
    }

    port = strtol(portStr, &strEnd, 10);
    if (port <= 0 || *strEnd != 0 || port >= SHRT_MAX)
    {
        /** invalid number, invalid port or has trailing garbage */
        printf("*** invalid number specified for port or trailing garbage.\n");
        return FALSE;
    }

#if 0
    if (rdk_logger_socketInit() == FALSE)
    {
        printf("** socket layer could not be initialized\n");
        return FALSE;
    }
#endif

    dbg_udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (dbg_udpSocket == -1)
    {
        /** failed to create socket */
        printf("*** failed to create socket\n");
        return FALSE;
    }

    /** initialise the logging host address */

    (void) memset((uint8_t *) &dbg_logHostAddr, 0, sizeof(dbg_logHostAddr));
    dbg_logHostAddr.sin_family = AF_INET;
    status = inet_aton(hostaddr, &(dbg_logHostAddr.sin_addr));
    if (status == 0)
    {
        /** invalid or unknown host name */
        printf("*** invalid or unknown host\n");
        return FALSE;
    }

    printf("*** port number = %li\n", port);
    dbg_logHostAddr.sin_port = htons((uint16_t) port);

#if 0
    status = rdk_logger_socketConnect(dbg_udpSocket,
            (rdk_logger_SocketSockAddr*)&dbg_logHostAddr,
            sizeof(dbg_logHostAddr));
    if (status == 0 )
    {
        printf("*** error failed to connect socket");
        return FALSE;
    }
#endif

    printf("*** successfully setup logging socket\n");
    return TRUE;
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

    /* Configure UDP logging if desired. */
    envVarValue = rdk_logger_envGet("LOG.RDK.UDP");
    if (envVarValue != NULL && createUDPSocket(envVarValue) == TRUE)
    {
        printf("udp logging enabled!");
        dbg_logViaUDP = TRUE;
    }

    /** Pre-condition the control table to disable all logging.  This
     * means that if no logging control statements are present in the
     * debug.ini file all logging will be suppressed. */
    memset(rdk_g_logControlTbl, 0, sizeof(rdk_g_logControlTbl));

    /** Intialize to the default configuration for all modules. */
    strcpy(envVarName, "LOG.RDK.DEFAULT");
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

/** Function to check if a specific log level of a module is enabled.*/
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

    if ((pszModuleName != NULL) && (pszLogLevels != NULL))
    {
        /* Intialize to the default configuration for all modules. */
        if(0 == strcmp(pszModuleName, "LOG.RDK.DEFAULT"))
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
            if(0 == strcmp(pszModuleName, envVarName))
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

    assert(modName);
    assert(cfgStr);
    assert(cfgStrMaxLen > 32);

    cfgStrMaxLen -= 1; /**< Ensure there is space for NUL. */
    strcpy(cfgStr, "");

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
        strcpy(cfgStr, "NONE");
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

    for (level = 0; level <= ENUM_RDK_LOG_COUNT; level++)
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

            strcat(cfgStr, " "); /* Not efficient - rah rah. */
            strcat(cfgStr, rdk_logLevelStrings[level]);
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
    if (!g_debugEnabled || !WANT_LOG(module, level))
    {
        return;
    }
    char *parent_cat_name = (char *)log4c_category_get_name(stackCat);
    snprintf(cat_name, sizeof(cat_name)-1, "%s.%s", parent_cat_name == NULL ? "" : parent_cat_name, module_name); 

    if (cat_cache[module] == NULL) {
        /** Only doing a read here, lock not needed */
        cat_cache[module] = log4c_category_get(cat_name);
    }

    log4c_category_t* cat = cat_cache[module];

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

/*************************Copied from socket_client.c/socket_client.h******************/






/******************************************************************************
 * set_socket_conf_dir
 *
 * Set the configuration directory for the socket.host file.
 */
static const char* set_socket_conf_dir(const char* dir)
{
    const char *ret = g_socket_conf_dir;
    g_socket_conf_dir = dir;
    return ret;
}

/******************************************************************************
 * init_logger_addr
 *
 * Read configuration file socket.host. This file should contain host address
 * and, optionally, port. Initialize sa_logger. If the configuration file does
 * not exist, use localhost:51400.
 * Returns: 0 - ok, -1 - error.
 */
static void init_logger_addr()
{
#ifdef WIN32
    char pathname[FILENAME_MAX];
    char *p;
    FILE *fd;
    char host[256];
    struct hostent * phe;

    memset(&g_sa_logger, 0, sizeof(SOCKADDR_IN));
    g_sa_logger.sin_family = AF_INET;

    if ('\\' == g_socket_conf_dir[0] || '/' == g_socket_conf_dir[0] || ':' == g_socket_conf_dir[1])
    {
        /* Absolute path. */
        strcpy(pathname, g_socket_conf_dir);
    }
    else
    {
        /* Relative path. */
        char *q;

        strcpy(pathname, __argv[0]);
        if (NULL != (p = strrchr(pathname, '\\')))
        p++;
        else
        goto use_default;

        if (NULL != (q = strrchr(pathname, '/')))
        q++;
        else
        goto use_default;

        if (p < q)
        *q = 0;
        else if (p > q)
        *p = 0;
        else
        pathname[0] = 0;
        strcat(pathname, g_socket_conf_dir);
    }
    p = &pathname[strlen(pathname) - 1];
    if ('\\' != *p && '/' != *p)
    {
        p++; *p = '/';
    }
    strcpy(++p, "socket.host");

    /* Read destination host name. */
    fd = fopen(pathname, "r");
    if (! fd)
    goto use_default;

    if (NULL == fgets(host, sizeof(host), fd))
    host[0] = 0;
    else
    {
        p = strchr(host, '\n');
        if (p)
        *p = 0;
        p = strchr(host, '\r');
        if (p)
        *p = 0;
    }
    fclose(fd);

    p = strchr(host, ':');
    if (p)
    *p++ = 0;

    phe = gethostbyname(host);
    if (! phe)
    goto use_default;

    memcpy(&g_sa_logger.sin_addr.s_addr, phe->h_addr, phe->h_length);

    if (p)
    g_sa_logger.sin_port = htons((unsigned short) strtoul(p, NULL, 0));
    else
    g_sa_logger.sin_port = htons(SOCKET_PORT);
    return;

    use_default:
    g_sa_logger.sin_addr.S_un.S_addr = htonl(0x7F000001);
#else
    g_sa_logger.sin_family = AF_INET;
    g_sa_logger.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif
    g_sa_logger.sin_port = htons(SOCKET_PORT);
}

/******************************************************************************
 * socket_open_log
 *
 * Open connection to system logger.
 */
static void socket_open_log(const char* ident, int option)
{
    int failed = 0;
#ifdef RI_WIN32_SOCKETS
    int wsa_initialized = 0;
    WSADATA wsd;
    int size;
#endif
    struct sockaddr_in sa_local;
    int n;

    if (g_initialized)
        return;

    // Parse options here if there are any.

#ifdef RI_WIN32_SOCKETS
    // Initialize windows sockets
    if (WSAStartup(MAKEWORD(2, 2), &wsd))
    goto done;
    wsa_initialized = 1;
#endif

    // Get local host name
    n = sizeof(g_local_hostname);
    if (gethostname(g_local_hostname, n) == -1)
        goto done;

    g_socket = INVALID_SOCKET;

    init_logger_addr();

    for (n = 0;; n++)
    {
        g_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (INVALID_SOCKET == g_socket)
            goto done;

        memset(&sa_local, 0, sizeof(struct sockaddr_in));
        sa_local.sin_family = AF_INET;
        if (bind(g_socket, (struct sockaddr*) &sa_local,
                sizeof(struct sockaddr)) == 0)
            break;
#ifdef RI_WIN32_SOCKETS
        (void)closesocket(g_socket);
#else
        (void) close(g_socket);
#endif
        g_socket = INVALID_SOCKET;
        if (n == 100)
            goto done;
    }

    /* Get size of datagramm. */
#ifdef RI_WIN32_SOCKETS
    size = sizeof(g_datagramm_size);
    if (getsockopt(g_socket, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*) &g_datagramm_size, &size))
    goto done;
    if (g_datagramm_size - strlen(g_local_hostname) - (ident ? strlen(ident) : 0) < 64)
    goto done;
    if (g_datagramm_size > sizeof(g_datagramm))
    g_datagramm_size = sizeof(g_datagramm);
#else
    g_datagramm_size = sizeof(g_datagramm);
#endif

    failed = 0;

    done: if (failed)
    {
#ifdef RI_WIN32_SOCKETS
        if (g_socket != INVALID_SOCKET) (void)closesocket(g_socket);
        if (wsa_initialized) WSACleanup();
#else
        if (g_socket != INVALID_SOCKET)
            (void) close(g_socket);
#endif
    }
    g_initialized = !failed;
}

/******************************************************************************
 * socket_close_log
 *
 * Close descriptor used to write to system logger.
 */
static void socket_close_log()
{
    if (!g_initialized)
        return;
#ifdef RI_WIN32_SOCKETS
    (void)closesocket(g_socket);
    WSACleanup();
#else
    (void) close(g_socket);
#endif
    g_initialized = FALSE;
}

/******************************************************************************
 * socket_append_msg
 *
 * Generate a log message using FMT string and option arguments.
 */
static void socket_append_msg(char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsocket_append_msg(fmt, ap);
    va_end(ap);
}

/******************************************************************************
 * vsocket_append_msg
 *
 * Generate a log message using FMT and using arguments pointed to by AP.
 */
static void vsocket_append_msg(char* fmt, va_list ap)
{
    //char *p;
    int num_chars;
    ssize_t numSent;

    if (!g_initialized)
        return;

    num_chars = vsnprintf(g_datagramm, g_datagramm_size, fmt, ap);
    g_datagramm[num_chars + 1] = '\0';

    numSent = sendto(g_socket, g_datagramm, strlen(g_datagramm), 0,
            (struct sockaddr*) &g_sa_logger, sizeof(struct sockaddr));
    if (numSent < 0)
    {
        printf("***** Error occurred while sending log message. *****\n");
    }
}
/*************************End Copied from socket_client.c/socket_client.h******************/

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
    (void) log4c_appender_type_set(&log4c_appender_type_socket_env);
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
static const char* dated_format_nocr(const log4c_layout_t* a_layout,
        const log4c_logging_event_t*a_event)
{
#if 0 //__BUG_MEMORY_LEAK__.  This make it really impossible to use  the "basic".
    char *rendered_msg = g_try_malloc(MAX_LOGLINE_LENGTH+1);

    if (NULL == rendered_msg)
    {
        fprintf(stderr, "\nline %d of %s %s memory allocation of %d failure!\n",
                    __LINE__, __FILE__, __func__, MAX_LOGLINE_LENGTH+1);
        return "------- ERROR: couldn't allocate memory for log message!?\r\n";
    }
#endif

#ifndef _WIN32
    struct tm tm;
    //localtime_r(&a_event->evt_timestamp.tv_sec, &tm); /* Use the UTC Time for logging */
    gmtime_r(&a_event->evt_timestamp.tv_sec, &tm);
    (void) snprintf(a_event->evt_buffer.buf_data, a_event->evt_buffer.buf_size,
            "%04d%02d%02d %02d:%02d:%02d.%03ld %-8s %s- %s", tm.tm_year + 1900,
            tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
            a_event->evt_timestamp.tv_usec / 1000, log4c_priority_to_string(
                    a_event->evt_priority), a_event->evt_category,
            a_event->evt_msg);
#else
    SYSTEMTIME stime;
    FILETIME ftime;

    if ( FileTimeToLocalFileTime(&a_event->evt_timestamp, &ftime))
    {
        if ( FileTimeToSystemTime(&ftime, &stime))
        {

            (void)snprintf(a_event->evt_buffer.buf_data, a_event->evt_buffer.buf_size, "%04d%02d%02d %02d:%02d:%02d.%03d %-8s %s- %s",
                    stime.wYear, stime.wMonth , stime.wDay,
                    stime.wHour, stime.wMinute, stime.wSecond,
                    stime.wMilliseconds,
                    log4c_priority_to_string(a_event->evt_priority),
                    a_event->evt_category, a_event->evt_msg);
        }
    }
#endif
    if (a_event->evt_buffer.buf_size > 0 && a_event->evt_buffer.buf_data != NULL) 
    {
        a_event->evt_buffer.buf_data[a_event->evt_buffer.buf_size - 1] = 0;
    }
    return a_event->evt_buffer.buf_data;
}

/****************************************************************
 * Basic layout with no ending carriage return / line feed
 */
static const char* basic_format_nocr(const log4c_layout_t* a_layout,
        const log4c_logging_event_t* a_event)
{
#if 0 //__BUG_MEMORY_LEAK__.  This make it really impossible to use  the "basic".

    char *rendered_msg = g_try_malloc(MAX_LOGLINE_LENGTH+1);

    if (NULL == rendered_msg)
    {
        fprintf(stderr, "\nline %d of %s %s memory allocation of %d failure!\n",
                    __LINE__, __FILE__, __func__, MAX_LOGLINE_LENGTH+1);
        return "------- ERROR: couldn't allocate memory for log message!?\r\n";
    }
#endif

    (void) snprintf(a_event->evt_buffer.buf_data, a_event->evt_buffer.buf_size, "%-8s %s - %s",
            log4c_priority_to_string(a_event->evt_priority),
            a_event->evt_category, a_event->evt_msg);

    if (a_event->evt_buffer.buf_size > 0 && a_event->evt_buffer.buf_data != NULL) 
    {
        a_event->evt_buffer.buf_data[a_event->evt_buffer.buf_size - 1] = 0;
    }

    return a_event->evt_buffer.buf_data;
}

static const char* comcast_dated_format_nocr(const log4c_layout_t* a_layout,
        const log4c_logging_event_t*a_event)
{
#if 0 //__BUG_MEMORY_LEAK__.  This make it really impossible to use  the "basic".
    char *rendered_msg = g_try_malloc(MAX_LOGLINE_LENGTH+1);

    if (NULL == rendered_msg)
    {
        fprintf(stderr, "\nline %d of %s %s memory allocation of %d failure!\n",
                    __LINE__, __FILE__, __func__, MAX_LOGLINE_LENGTH+1);
        return "------- ERROR: couldn't allocate memory for log message!?\r\n";
    }
#endif

#ifndef _WIN32
    struct tm tm;
    //localtime_r(&a_event->evt_timestamp.tv_sec, &tm);  /* Use the UTC Time for logging */
    gmtime_r(&a_event->evt_timestamp.tv_sec, &tm);
    /** Get the last part of the cagetory as "module" */
    char *p= (char *)(a_event->evt_category);
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
            while (p != (char *)(a_event->evt_category) && *p != '.') p--;
            if (*p == '.') p+=1;

        }
        else 
        {
            p = (char*)"UNKNOWN";
        }
    }

    (void) snprintf(a_event->evt_buffer.buf_data, a_event->evt_buffer.buf_size,
            "%02d%02d%02d-%02d:%02d:%02d.%06ld [mod=%s, lvl=%s] [tid=%ld] %s", 
            tm.tm_year + 1900 - 2000, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, a_event->evt_timestamp.tv_usec, 
            p, log4c_priority_to_string(a_event->evt_priority), syscall(SYS_gettid),
            a_event->evt_msg);
#else
    SYSTEMTIME stime;
    FILETIME ftime;

    if ( FileTimeToLocalFileTime(&a_event->evt_timestamp, &ftime))
    {
        if ( FileTimeToSystemTime(&ftime, &stime))
        {

            (void)snprintf(a_event->evt_buffer.buf_data, a_event->evt_buffer.buf_size, "%04d%02d%02d %02d:%02d:%02d.%03d %-8s %s- %s",
                    stime.wYear, stime.wMonth , stime.wDay,
                    stime.wHour, stime.wMinute, stime.wSecond,
                    stime.wMilliseconds,
                    log4c_priority_to_string(a_event->evt_priority),
                    a_event->evt_category, a_event->evt_msg);
        }
    }
#endif
    if (a_event->evt_buffer.buf_size > 0 && a_event->evt_buffer.buf_data != NULL) 
    {
        a_event->evt_buffer.buf_data[a_event->evt_buffer.buf_size - 1] = 0;
    }
    return a_event->evt_buffer.buf_data;
}
/****************************************************************
 * Stream layout that will parse environment variables from the
 * stream name (env vars have a leading "$(" and end with )"
 */
static int stream_env_open(log4c_appender_t* appender, int append)
{
    FILE* fp = (FILE*)log4c_appender_get_udata(appender);
    char* name = g_strdup(log4c_appender_get_name(appender));
    int nameLen = strlen(name);
    char* temp = name;
    char *varBegin, *varEnd;
    char* envVar;
    const int MAX_VAR_LEN = 1024;
    char newName[MAX_VAR_LEN+1];
    int newNameLen = 0;

    if (fp)
    return 0;

    newName[0] = '\0';

    ///> Parse any environment variables
    while ((varBegin = strchr(temp,'$')) != NULL)
    {
        ///> Search for opening and closing parens
        if (((varBegin - name + 1) >= nameLen) || (*(varBegin+1) != '('))
        goto parse_error;

        ///> Append characters up to this point to the new name
        strncat(newName, temp, varBegin-temp);
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
        strncat(newName, envVar, strlen(envVar));
        newNameLen += strlen(envVar);
        if (newNameLen >MAX_VAR_LEN)
        goto length_error;

        temp = varEnd + 1;
    }

    ///> Append remaining characters
    strncat(newName, temp, (name + nameLen) - temp);
    newNameLen += (name + nameLen) - temp;
    if (newNameLen >MAX_VAR_LEN)
    goto length_error;

    g_free(name);

    if (!strcmp(newName,"stderr"))
    fp = stderr;
    else if (!strcmp(newName,"stdout"))
    fp = stdout;
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

    /**> unbuffered mode */
    setbuf(fp, NULL);

    (void)log4c_appender_set_udata(appender, fp);
    return 0;

    parse_error:
    fprintf(stderr, "*(*(*(*( log4c appender stream_env, %s -- Illegal env var name or format! %s\n",
            __FUNCTION__, name);
    (void)fflush(stderr);
    g_free(name);
    return -1;

    length_error:
    fprintf(stderr, "*(*(*(*( log4c appender stream_env, %s -- Path is too long! %s\n",
            __FUNCTION__, name);
    (void)fflush(stderr);
    g_free(name);
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

static int stream_env_append(log4c_appender_t* appender,
        const log4c_logging_event_t* a_event)
{
    int retval;
    FILE* fp = (FILE*)log4c_appender_get_udata(appender);

    retval = fprintf(fp, "%s", a_event->evt_rendered_msg);
    (void)fflush(fp);

    //g_free((void *)a_event->evt_rendered_msg);

    return retval;
}

static int stream_env_plus_stdout_append(log4c_appender_t* appender,
        const log4c_logging_event_t* a_event)
{
    int retval;
    FILE* fp = (FILE*)log4c_appender_get_udata(appender);

    retval = fprintf(fp, "%s", a_event->evt_rendered_msg);
    fprintf(stdout, "%s", a_event->evt_rendered_msg);

    (void)fflush(fp);
    (void)fflush(stdout);

    //g_free((void *)a_event->evt_rendered_msg);

    return retval;
}

static int stream_env_close(log4c_appender_t* appender)
{
    FILE* fp = (FILE*)log4c_appender_get_udata(appender);

    if (!fp || fp == stdout || fp == stderr)
    return 0;

    return fclose(fp);
}

static int socket_env_open(log4c_appender_t* appender)
{
    int retval = 0;
    char* name = g_strdup(log4c_appender_get_name(appender));
    int nameLen = strlen(name);
    char* temp = name;
    char *varBegin, *varEnd;
    char* envVar;
    const int MAX_VAR_LEN = 1024;
    char newName[MAX_VAR_LEN+1];
    int newNameLen = 0;

    newName[0] = '\0';

    ///> Parse any environment variables.
    while ((varBegin = strchr(temp,'$')) != NULL)
    {
        ///> Search for opening and closing parens.
        if (((varBegin - name + 1) >= nameLen) || (*(varBegin+1) != '('))
        goto parse_error;

        ///> Append characters up to this point to the new name.
        strncat(newName, temp, varBegin-temp);
        newNameLen += varBegin-temp;
        if (newNameLen > MAX_VAR_LEN)
        goto length_error;

        varBegin += 2; ///> Start of env var name.

        if ((varEnd = strchr(varBegin,')')) == NULL)
        goto parse_error;

        *varEnd = '\0';
        if ((envVar = getenv(varBegin)) == NULL)
        goto parse_error;

        ///> Append env var value to the new name.
        strncat(newName, envVar, strlen(envVar));
        newNameLen += strlen(envVar);
        if (newNameLen >MAX_VAR_LEN)
        goto length_error;

        temp = varEnd + 1;
    }

    ///> Append remaining characters.
    strncat(newName, temp, (name + nameLen) - temp);
    newNameLen += (name + nameLen) - temp;
    if (newNameLen >MAX_VAR_LEN)
    goto length_error;

    ///> Call the socket client implementation.
    if (strcmp(newName, "socket"))
    ///> Must be specifying the configuration directory for the socket.host file.
    (void)set_socket_conf_dir(newName);
    socket_open_log(name, 0);

    g_free(name);

    return retval;

    parse_error:
    fprintf(stderr, "*(*(*(*( log4c appender socket_env, %s -- Illegal env var name or format! %s\n",
            __FUNCTION__, name);
    (void)fflush(stderr);
    g_free(name);
    return -1;

    length_error:
    fprintf(stderr, "*(*(*(*( log4c appender socket_env, %s -- Path is too long! %s\n",
            __FUNCTION__, name);
    (void)fflush(stderr);
    g_free(name);
    return -1;
}

static int socket_env_append(log4c_appender_t* appender, const log4c_logging_event_t* a_event)
{
    int retval = 0;

    ///> Call the socket client implementation.
    socket_append_msg((char*)"%s", a_event->evt_rendered_msg);

    //g_free((void *)a_event->evt_rendered_msg);

    return retval;
}

static int socket_env_close(log4c_appender_t* appender)
{
    ///> Call the socket client implementation.
    socket_close_log();

    return 0;
}

/*************************End Copied from ri_log.c******************/

