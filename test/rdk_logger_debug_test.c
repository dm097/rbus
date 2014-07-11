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
