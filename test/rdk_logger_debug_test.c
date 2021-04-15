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

#include <stdio.h>
#include <stdlib.h>
#include "rdk_debug.h"
int rdk_logger_debug_test()
{
	RDK_LOG(RDK_LOG_FATAL, "LOG.RDK.GFX",
		"Fatal\n");

	RDK_LOG(RDK_LOG_ERROR, "LOG.RDK.GFX",
		"Error\n");

	RDK_LOG(RDK_LOG_WARN, "LOG.RDK.GFX",
		"Warning\n");

	RDK_LOG(RDK_LOG_INFO, "LOG.RDK.GFX",
		"Info\n");

	RDK_LOG(RDK_LOG_NOTICE, "LOG.RDK.GFX",
		"Notice\n");

	RDK_LOG(RDK_LOG_DEBUG, "LOG.RDK.GFX",
		"Debug\n");

	RDK_LOG(RDK_LOG_TRACE1, "LOG.RDK.GFX",
		"Trace1\n");

	RDK_LOG(RDK_LOG_FATAL, "LOG.RDK.SYS",
		"Fatal\n");

	RDK_LOG(RDK_LOG_ERROR, "LOG.RDK.SYS",
		"Error\n");

	RDK_LOG(RDK_LOG_WARN, "LOG.RDK.SYS",
		"Warning\n");

	RDK_LOG(RDK_LOG_INFO, "LOG.RDK.SYS",
		"Info\n");

	RDK_LOG(RDK_LOG_NOTICE, "LOG.RDK.SYS",
		"Notice\n");

	RDK_LOG(RDK_LOG_DEBUG, "LOG.RDK.SYS",
		"Debug\n");

	RDK_LOG(RDK_LOG_TRACE1, "LOG.RDK.SYS",
		"Trace1\n");

	RDK_LOG(RDK_LOG_FATAL, "LOG.RDK.QAMSRC",
		"Fatal\n");

	RDK_LOG(RDK_LOG_ERROR, "LOG.RDK.QAMSRC",
		"Error\n");

	RDK_LOG(RDK_LOG_WARN, "LOG.RDK.QAMSRC",
		"Warning\n");

	RDK_LOG(RDK_LOG_NOTICE, "LOG.RDK.QAMSRC",
		"Notice\n");

	RDK_LOG(RDK_LOG_INFO, "LOG.RDK.QAMSRC",
		"Info\n");

	RDK_LOG(RDK_LOG_DEBUG, "LOG.RDK.QAMSRC",
		"Debug\n");

	RDK_LOG(RDK_LOG_TRACE1, "LOG.RDK.QAMSRC",
		"Trace1\n");

	RDK_LOG(RDK_LOG_FATAL, "LOG.RDK.INBSI",
		"Fatal\n");

	RDK_LOG(RDK_LOG_ERROR, "LOG.RDK.INBSI",
		"Error\n");

	RDK_LOG(RDK_LOG_WARN, "LOG.RDK.INBSI",
		"Warning\n");

	RDK_LOG(RDK_LOG_NOTICE, "LOG.RDK.INBSI",
		"Notice\n");

	RDK_LOG(RDK_LOG_INFO, "LOG.RDK.INBSI",
		"Info\n");

	RDK_LOG(RDK_LOG_DEBUG, "LOG.RDK.INBSI",
		"Debug\n");

	RDK_LOG(RDK_LOG_TRACE1, "LOG.RDK.INBSI",
		"Trace1\n");

	if (TRUE == rdk_dbg_enabled( "LOG.RDK.QAMSRC", RDK_LOG_FATAL))
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_FATAL Enabled.\n");
	}
	else
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_FATAL Disabled.\n");
	}

	if (TRUE == rdk_dbg_enabled( "LOG.RDK.QAMSRC", RDK_LOG_ERROR))
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_ERROR Enabled.\n");
	}
	else
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_ERROR Disabled.\n");
	}

	if (TRUE == rdk_dbg_enabled( "LOG.RDK.QAMSRC", RDK_LOG_WARN))
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_WARN Enabled.\n");
	}
	else
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_WARN Disabled.\n");
	}

	if (TRUE == rdk_dbg_enabled( "LOG.RDK.QAMSRC", RDK_LOG_INFO))
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_INFO Enabled.\n");
	}
	else
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_INFO Disabled.\n");
	}

	if (TRUE == rdk_dbg_enabled( "LOG.RDK.QAMSRC", RDK_LOG_DEBUG))
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_DEBUG Enabled.\n");
	}
	else
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_DEBUG Disabled.\n");
	}

	if (TRUE == rdk_dbg_enabled( "LOG.RDK.QAMSRC", RDK_LOG_TRACE1))
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_TRACE1 Enabled.\n");
	}
	else
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_TRACE1 Disabled.\n");
	}

	if (TRUE == rdk_dbg_enabled( "LOG.RDK.QAMSRC", RDK_LOG_TRACE2))
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_TRACE2 Enabled.\n");
	}
	else
	{
		printf("LOG.RDK.QAMSRC  RDK_LOG_TRACE2 Disabled.\n");
	}

	printf ( "\n test complete\n");
	return 0;
}
