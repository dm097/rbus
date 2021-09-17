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
 * @file rdk_dynamic_logger.c
 */

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include "rdk_dynamic_logger.h"
#include "safec_library.h"

#define DL_PORT 12035
#define DL_SIGNATURE "COMC"
#define DL_SIGNATURE_LEN 4

static int g_dl_socket = -1;
extern char *__progname;

static char * rdk_dyn_log_logLevelToString(unsigned char log_level)
{
    int negate = 0;
    unsigned char negate_mask = 0x80;

    if(negate_mask == (log_level & negate_mask)) {
        negate = 1;
        log_level = log_level & (~negate_mask) ;
    }

    switch(log_level){
        case RDK_LOG_FATAL:
            return (negate) ? "!FATAL":"FATAL";
        case RDK_LOG_ERROR:
            return (negate) ? "!ERROR":"ERROR";
        case RDK_LOG_WARN:
            return (negate) ? "!WARN":"WARN";
        case RDK_LOG_NOTICE:
            return (negate) ? "!NOTICE":"NOTICE";
        case RDK_LOG_INFO:
            return (negate) ? "!INFO":"INFO";
        case RDK_LOG_DEBUG:
            return (negate) ? "!DEBUG":"DEBUG";
        case RDK_LOG_TRACE1:
            return (negate) ? "!TRACE1":"TRACE1";
        case RDK_LOG_TRACE2:
            return (negate) ? "!TRACE2":"TRACE2";
        case RDK_LOG_TRACE3:
            return (negate) ? "!TRACE3":"TRACE3";
        case RDK_LOG_TRACE4:
            return (negate) ? "!TRACE4":"TRACE4";
        case RDK_LOG_TRACE5:
            return (negate) ? "!TRACE5":"TRACE5";
        case RDK_LOG_TRACE6:
            return (negate) ? "!TRACE6":"TRACE6";
        case RDK_LOG_TRACE7:
            return (negate) ? "!TRACE7":"TRACE7";
        case RDK_LOG_TRACE8:
            return (negate) ? "!TRACE8":"TRACE8";
        case RDK_LOG_TRACE9:
            return (negate) ? "!TRACE9":"TRACE9";
    }

    return NULL;
}

static void rdk_dyn_log_validateComponentName(const unsigned char *buf)
{
    unsigned char log_level = 0;
    int app_len, comp_len, i = DL_SIGNATURE_LEN;
    char *loggingLevel = NULL, comp_name[64] = {0};
    errno_t rc = -1;

    if(0 != memcmp(buf,DL_SIGNATURE,i)) {
        return;
    }

    log_level = buf[++i];
    app_len = buf[++i];

    if(0 != memcmp(buf+(++i),__progname,app_len)) {
        /* The received msg is not intended for this process */
        return;
    }

    i += app_len;
    comp_len = buf[i];

    loggingLevel = rdk_dyn_log_logLevelToString(log_level);
    if(NULL != loggingLevel) {
        ++i;
        rc = memcpy_s(comp_name,sizeof(comp_name),buf+i,comp_len);
        ERR_CHK(rc);
        RDK_LOG_ControlCB(comp_name, NULL, loggingLevel, 0);
        fprintf(stderr,"%s(): Set %s loglevel for the component %s of the process %s\n",__func__,loggingLevel,comp_name,__progname);
    }
}

void rdk_dyn_log_processPendingRequest()
{
    char buf[128] = {0};
    struct sockaddr_in sender_addr = {0};
    struct timeval tv;
    int numbytes, addr_len, ret, i = 0;
    fd_set rfds;
    errno_t rc = -1;
    int ind = -1;

    if(-1 == g_dl_socket)
        return;
	
	int length = strlen("127.0.0.1");

    while(1) {
        FD_ZERO(&rfds);
        FD_SET(g_dl_socket, &rfds);
        addr_len = sizeof(sender_addr);

        tv.tv_sec = 0;
        tv.tv_usec = 0;
        ret = select(g_dl_socket+1,&rfds,NULL,NULL,&tv);
        if(ret <= 0)
            break;

        if ((numbytes=recvfrom(g_dl_socket, buf, sizeof(buf), 0, (struct sockaddr *)&sender_addr, &addr_len)) == -1) {
            fprintf(stderr,"%s recvfrom failed %s\n",__func__,strerror(errno));
            return;
        }

        /*
         * msg_format {
         *    char    m_signature[4];
         *    uchar8  m_msgLength;
         *    uchar8  m_logLevel;
         *    uchar8  m_appLength;
         *    char    m_appname[m_appLength];
         *    uchar8  m_compNameLength;
         *    char    m_compName[m_compNameLength];
         * }
         *
         * Ensure that the we handle msgs only from localhost
         */
        rc = strcmp_s("127.0.0.1", length, inet_ntoa(sender_addr.sin_addr), &ind);
        ERR_CHK(rc);
        if(((!ind) && (rc == EOK)) &&
                (numbytes == buf[4]+DL_SIGNATURE_LEN+1)) {
            rdk_dyn_log_validateComponentName(buf);
        }
    }
}

void rdk_dyn_log_init()
{
    struct sockaddr_in my_addr = {0};
    int opt = 1;

    if ((g_dl_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        fprintf(stderr, "%s() Socket creation failed. %s\n",__func__,strerror(errno));
        return;
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DL_PORT);
    my_addr.sin_addr.s_addr = inet_addr("127.255.255.255");

    if (-1 == setsockopt(g_dl_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int))) {
        fprintf(stderr, "%s() setsockopt error %s\n",__func__,strerror(errno));
        close(g_dl_socket);
        g_dl_socket = -1;
        return;
    }

    if (-1 == bind(g_dl_socket, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))) {
        fprintf(stderr, "%s() bind error %s\n",__func__,strerror(errno));
        close(g_dl_socket);
        g_dl_socket = -1;
        return;
    }

    fprintf(stderr, "%sg_dl_socket = %d __progname = %s \n",__func__,g_dl_socket,__progname);
}

void rdk_dyn_log_deInit()
{
    close(g_dl_socket);
    g_dl_socket = -1;
}
