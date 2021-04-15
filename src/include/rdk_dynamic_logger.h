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

#if !defined(_RDK_DBG_PRIV_DYNLOG_H)
#define _RDK_DBG_PRIV_DYNLOG_H

#include <rdk_debug.h>

void rdk_dyn_log_processPendingRequest(void);

void rdk_dyn_log_init(void);

void rdk_dyn_log_deInit(void);

#endif /* _RDK_DBG_PRIV_DYNLOG_H */
