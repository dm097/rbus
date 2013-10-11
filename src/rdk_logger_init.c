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

#include <sys/socket.h>
#include <signal.h>
#include "rdk_debug.h"
#include "rdk_error.h"
#include "rdk_debug_priv.h"
#include "rdk_utils.h"


rdk_Error rdk_logger_init(const char* debugConfigFile)
{
	rdk_Error ret;

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

        /**
         * Requests not to send SIGPIPE on errors on stream oriented
         * sockets when the other end breaks the connection. The EPIPE 
         * error is still returned. 
         */
         sigignore(SIGPIPE);

	return RDK_SUCCESS;
}
