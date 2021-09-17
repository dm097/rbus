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
#include "rdk_dynamic_logger.h"
#include "rdk_utils.h"
#include "safec_library.h"

#define BUF_LEN 256
static int isLogInited = 0;
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

    if (0 == isLogInited)
    {
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
        rdk_dyn_log_init();

        char *filename = "/tmp/debugConfigFile_read";

        if((0 == stat(filename, &st) && (0 != st.st_ino)))
        {
            printf("%s %s Already Stack Level Logging processed... not processing again.\n", __FUNCTION__, debugConfigFile);
        }
        else
        {
            rdk_dbgDumpLog(filename);
        }

        /**
         * Requests not to send SIGPIPE on errors on stream oriented
         * sockets when the other end breaks the connection. The EPIPE
         * error is still returned.
         */
        signal(SIGPIPE, SIG_IGN);
        isLogInited = 1;
    }
    return RDK_SUCCESS;
}

/**
 * @brief Cleanup the logger instantiation.
 *
 * @return Returns 0 if the call is successful else return -1.
 */
rdk_Error rdk_logger_deinit()
{
    if(isLogInited)
    {
        rdk_dyn_log_deInit();
        log4c_fini();
    }

    return RDK_SUCCESS;
}
