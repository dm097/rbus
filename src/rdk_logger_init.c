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
 * @file rdk_logger_init.c
 * This source file contains the APIs for RDK logger initializer.
 */

/**
* @defgroup rdk_logger
* @{
* @defgroup src
* @{
**/


#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "rdk_debug.h"
#include "rdk_error.h"
#include "rdk_debug_priv.h"
#include "rdk_utils.h"

#define BUF_LEN 256

/**
 * @brief Initialize the logger. Sets up the environment variable storage by parsing
 * debug configuration file then Initialize the debug support to the underlying platform.
 *
 * @note  Requests not to send SIGPIPE on errors on stream oriented sockets
 * when the other end breaks the connection. The EPIPE error is still returned.
 *
 * @param[in] debugConfigFile The character pointer variable of debug configuration file.
 *
 * @return Returns 0 if initialization of RDK logger module is successful, else it returns -1.
 */
rdk_Error rdk_logger_init(const char* debugConfigFile)
{
	rdk_Error ret;
    struct stat st;
    char buf[BUF_LEN] = {'\0'};

	if (NULL == debugConfigFile)
	{
		debugConfigFile = DEBUG_CONF_FILE;
	}

	ret = rdk_logger_env_add_conf_file(debugConfigFile); 
	if ( RDK_SUCCESS != ret)
	{
		printf("%s:%d Adding debug config file %s failed\n", __FUNCTION__, __LINE__, DEBUG_CONF_FILE);
		return ret;
	}

	rdk_dbgInit(); 

	snprintf(buf, BUF_LEN-1, "/tmp/%s", "debugConfigFile_read");
	buf[BUF_LEN-1] = '\0';

    if((0 == stat(buf, &st) && (0 != st.st_ino)))
    {
        printf("%s %s Already Stack Level Logging processed... not processing again.\n", __FUNCTION__, debugConfigFile);
    }
    else
    {
        rdk_dbgDumpLog(buf);
    }

        /**
         * Requests not to send SIGPIPE on errors on stream oriented
         * sockets when the other end breaks the connection. The EPIPE 
         * error is still returned. 
         */
         signal(SIGPIPE, SIG_IGN);

	return RDK_SUCCESS;
}

/**
 * @brief Cleanup the logger instantiation.
 *
 * @return Returns 0 if the call is successful else return -1.
 */
rdk_Error rdk_logger_deinit()
{
    log4c_fini();

    return RDK_SUCCESS;
}
