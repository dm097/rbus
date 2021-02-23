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
#include <string.h>
#include <string>
#include <time.h>
#include <sys/time.h>
#include "rdk_logger_milestone.h"

#ifdef LOGMILESTONE
#define MILESTONE_LOG_FILENAME "/opt/logs/rdk_milestones.log"
#else
#define MILESTONE_LOG_FILENAME "/rdklogs/logs/rdk_milestones.log"
#endif

unsigned long long getUptimeMS(void)
{
    struct timespec uptime;
    unsigned long long uptimems;

    //Using clock_gettime to get uptime in ms precision
    clock_gettime(CLOCK_MONOTONIC_RAW, &uptime);
    uptimems = (unsigned long long)uptime.tv_sec * 1000 + uptime.tv_nsec / 1000000;

    return uptimems;
}

void logMilestone(const char *msg_code)
{
    FILE *fp = NULL;
    fp = fopen(MILESTONE_LOG_FILENAME, "a+");
    if (fp != NULL)
    {
      fprintf(fp, "%s:%llu\n", msg_code, getUptimeMS());
      fclose(fp);
    }
}
