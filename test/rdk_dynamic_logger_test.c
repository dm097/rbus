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

#include<stdio.h>
#include "libIBus.h"
#include "rdk_log_reg.h"


/*
To build the test app, add test folder to the below lines
SUBDIRS = src include test
DIST_SUBDIRS = cfg src include test
To the file rdk_logger/generic/Makefile.am

How to test :

1. Run the test App dynamicLoggerTest in the box.
The binary can be copied to the box after compiling the test code by doing a bitbake for rdk-logger
with the above changes.
2. The test application has registered for dynamic logging updates with
Application Name :
 Application1
Module Name ------ SubModule Name ------ Registered Callback
ClosedCaptions1    NULL                  CC_LOG_ControlCB1
ClosedCaptions2    NULL                  CC_LOG_ControlCB2
ServiceManager     devicesettings        CC_LOG_ControlCB3
ServiceManager     hdmicec               CC_LOG_ControlCB4

Or

Application Name :
 Application2
Module Name ------ SubModule Name ------ Registered Callback
ClosedCaptions1    NULL                  CC_LOG_CallBack1
ClosedCaptions2    NULL                  CC_LOG_CallBack2
ServiceManager     devicesettings        CC_LOG_CallBack3
ServiceManager     hdmicec               CC_LOG_CallBack4

3. Execute the rdklogctrl tool with proper input parameters and check whether the
test application is showing correct print.

eg. If we execute the tool with following parameters
./rdklogctrl Application1 ClosedCaptions1 NULL DEBUG ON

The test application will print
TEST : CC_LOG_ControlCB1 called for ClosedCaptions1
TEST : ##############Data obtained ################
TEST : moduleName : ClosedCaptions1 subComponentName : NULL loggingLevel : DEBUG log_status : 1

Similarly different combinations of input parameters can be tested.

*/

void CC_LOG_ControlCB1(const char *moduleName, const char *subComponentName, const char *loggingLevel, int log_status)
{
    printf("TEST : CC_LOG_ControlCB1 called for ClosedCaptions1\n");
    printf("TEST : ##############Data obtained ################\n");
    printf("TEST : moduleName : %s subComponentName : %s loggingLevel : %s log_status : %d\n",moduleName,subComponentName,loggingLevel,log_status);
}
void CC_LOG_ControlCB2(const char *moduleName, const char *subComponentName, const char *loggingLevel, int log_status)
{
    printf("TEST : CC_LOG_ControlCB2 called for ClosedCaptions2\n");
    printf("TEST : ##############Data obtained ################\n");
    printf("TEST : moduleName : %s subComponentName : %s loggingLevel : %s log_status : %d\n",moduleName,subComponentName,loggingLevel,log_status);
}
void CC_LOG_ControlCB3(const char *moduleName, const char *subComponentName, const char *loggingLevel, int log_status)
{
    printf("TEST : CC_LOG_ControlCB3 called for ServiceManager devicesettings\n");
    printf("TEST : ##############Data obtained ################\n");
    printf("TEST : moduleName : %s subComponentName : %s loggingLevel : %s log_status : %d\n",moduleName,subComponentName,loggingLevel,log_status);
}
void CC_LOG_ControlCB4(const char *moduleName, const char *subComponentName, const char *loggingLevel, int log_status)
{
    printf("TEST : CC_LOG_ControlCB4 called for ServiceManager hdmicec\n");
    printf("TEST : ##############Data obtained ################\n");
    printf("TEST : moduleName : %s subComponentName : %s loggingLevel : %s log_status : %d\n",moduleName,subComponentName,loggingLevel,log_status);
}
void CC_LOG_CallBack1(const char *moduleName, const char *subComponentName, const char *loggingLevel, int log_status)
{
    printf("TEST : CC_LOG_CallBack1 called for ClosedCaptions1\n");
    printf("TEST : ##############Data obtained ################\n");
    printf("TEST : moduleName : %s subComponentName : %s loggingLevel : %s log_status : %d\n",moduleName,subComponentName,loggingLevel,log_status);
}
void CC_LOG_CallBack2(const char *moduleName, const char *subComponentName, const char *loggingLevel, int log_status)
{
    printf("TEST : CC_LOG_CallBack2 called for ClosedCaptions2\n");
    printf("TEST : ##############Data obtained ################\n");
    printf("TEST : moduleName : %s subComponentName : %s loggingLevel : %s log_status : %d\n",moduleName,subComponentName,loggingLevel,log_status);
}
void CC_LOG_CallBack3(const char *moduleName, const char *subComponentName, const char *loggingLevel, int log_status)
{
    printf("TEST : CC_LOG_CallBack3 called for ServiceManager devicesettings\n");
    printf("TEST : ##############Data obtained ################\n");
    printf("TEST : moduleName : %s subComponentName : %s loggingLevel : %s log_status : %d\n",moduleName,subComponentName,loggingLevel,log_status);
}
void CC_LOG_CallBack4(const char *moduleName, const char *subComponentName, const char *loggingLevel, int log_status)
{
    printf("TEST : CC_LOG_CallBack4 called for ServiceManager hdmicec\n");
    printf("TEST : ##############Data obtained ################\n");
    printf("TEST : moduleName : %s subComponentName : %s loggingLevel : %s log_status : %d\n",moduleName,subComponentName,loggingLevel,log_status);
}

#define IARM_BUS_DYNAMIC_LOGGER_TESTAPP "LOG_TESTER"

void main()
{
    IARM_Bus_Init(IARM_BUS_DYNAMIC_LOGGER_TESTAPP);
    IARM_Bus_Connect();
    rdk_logger_initialize();
    int i;

    printf("Thanks for using Dynamic Logger TestApp \n");
    printf("Please select the App name\n 1 - Application1   2 - Application2 \n");
    scanf("%d", &i);

    switch(i)
    {
    case 1:

        printf("Registering app Application1 \n");
        rdk_logger_setAppName("Application1");
        rdk_logger_registerLogCtrlComp("ClosedCaption1", NULL, CC_LOG_ControlCB1);
        rdk_logger_registerLogCtrlComp("ClosedCaption2", NULL, CC_LOG_ControlCB2);
        rdk_logger_registerLogCtrlComp("ServiceManager", "devicesettings", CC_LOG_ControlCB3);
        rdk_logger_registerLogCtrlComp("ServiceManager", "hdmicec", CC_LOG_ControlCB4);

    break;

    case 2 :

        printf("Registering app Application2 \n");
        rdk_logger_setAppName("Application2");
        rdk_logger_registerLogCtrlComp("ClosedCaption1", NULL, CC_LOG_CallBack1);
        rdk_logger_registerLogCtrlComp("ClosedCaption2", NULL, CC_LOG_CallBack2);
        rdk_logger_registerLogCtrlComp("ServiceManager", "devicesettings", CC_LOG_CallBack3);
        rdk_logger_registerLogCtrlComp("ServiceManager", "hdmicec", CC_LOG_CallBack4);

    break;

    default:
    break;
    }
    while(1)
    {
        sleep(10);
    }

    rdk_logger_unRegisterLogCtrlComp("ServiceManager", "hdmicec");
    rdk_logger_unRegisterLogCtrlComp("ClosedCaption2", NULL);
    rdk_logger_unRegisterLogCtrlComp("ClosedCaption1", NULL);
    rdk_logger_unRegisterLogCtrlComp("ServiceManager", "devicesettings");

    rdk_logger_unInitialize();
    IARM_Bus_Disconnect();
    IARM_Bus_Term();
    return;
}
