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
#include "rdk_debug.h"

int rdk_logger_debug_test();

int main()
{
	rdk_logger_init(NULL);
	rdk_logger_debug_test();
	return 0;
}
