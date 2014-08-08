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

#if !defined(_RDK_LOGGER_TYPES_H_)
#define _RDK_LOGGER_TYPES_H_

#ifndef __ASSEMBLY__

#include <time.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h> /* uint32_t etc. */

#ifdef __cplusplus
extern "C"
{
#endif

typedef int32_t rdk_logger_Bool;

#ifndef TRUE
#define TRUE (1==1)
#endif


#ifndef FALSE
#define FALSE (1!=1)
#endif


#ifdef __cplusplus
}
#endif

#endif /* _RDK_LOGGER_TYPES_H_ */

#endif /*__ASSEMBLY__*/ /*FD-20090114*/
