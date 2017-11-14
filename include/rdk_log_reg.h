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

#if !defined(_RDK_LOG_REG_H)
#define _RDK_LOG_REG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*Callback function type which need to be registered by the components to get
log level and log status set dynamically by the user.*/

typedef void (*rdk_logger_logCtrlCallback_t) (const char *moduleName, const char *subComponentName, const char *loggingLevel, int log_status);

/**
 * The rdk_logger_initialize function will initialize the log registration utility.
 *
 * @param none
 * @return none
 *
 */
void rdk_logger_initialize();

/**
 * The rdk_logger_unInitialize function will unInitialize the log registration utility.
 *
 * @param none
 * @return none
 *
 */
void rdk_logger_unInitialize();

/**
 * The rdk_logger_setAppName function can be used to
 * register the Application name which is intends to use the
 * dynamic logging feature for its components.Upon initialization,
 * each application should register its unique name.
 *
 * @param appName Application name.
 * The appName should try to match the executable name of the application.
 * E.g. "Receiver", "CECDevMgr"
 *
 * @return none
 *
 */
void rdk_logger_setAppName(const char *appName);

/**
 * The rdk_logger_registerLogCtrlComp function can be used to
 * register the component and sub-component name.
 *
 * @param componentName Name of the component
 * @param subComponentName Name of the sub component
 * @param CB is function pointer which is registered as callback function
 * from the component.
 *
 * @return none
 *
 */
void rdk_logger_registerLogCtrlComp (const char *componentName, const char *subComponentName, rdk_logger_logCtrlCallback_t CB);

/**
 * The rdk_logger_unRegisterLogCtrlComp function can be used to
 * unRegister the component and/or sub-component name.
 *
 * @param componentName Name of the component
 * @param subComponentName Name of the sub component
 *
 * @return none
 *
 */
void rdk_logger_unRegisterLogCtrlComp(const char *module, const char *subModule);

/**
 * The rdk_logger_registerLogCtrlCallback function can be used to
 * register the callback function from different components.
 *
 * @param cb is function pointer which is registered as callback function
 * from the component.
 *
 * @return none
 *
 */
//void rdk_logger_registerLogCtrlCallback (rdk_logger_logCtrlCallback_t cb);



#ifdef __cplusplus
}
#endif

#endif /* _RDK_LOG_REG_H */

