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
 * @file rdk_logger_util.c
 * This source file contains the APIs for RDK logger util.
 */

/*
 * This file provides the rdk_logger_ utility APIs.
 */

/* Header Files */
#include <rdk_logger_types.h>      /* Resolve basic type references. */
#include "rdk_debug.h"      /* Resolved RDK_LOG support. */
#include "rdk_error.h"
#include "rdk_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/*Resolve reboot and related macros.*/
#include <unistd.h> 
#include <sys/reboot.h>
#include <linux/reboot.h>


typedef struct EnvVarNode
{
    int number;
    char* name;
    char* value;
    struct EnvVarNode *next;
} EnvVarNode;

/** Global count for the modules */
int global_count;

/* Env var cache */
static EnvVarNode *g_envCache = NULL;

static void trim(char *instr, char* outstr)
{
    char *ptr = instr;
    char *endptr = instr + strlen(instr)-1;
    int length;

    /* Advance pointer to first non-whitespace char */
    while (isspace(*ptr))
        ptr++;

    if (ptr > endptr)
    {
        /*
         * avoid breaking things when there are
         * no non-space characters in instr (JIRA OCORI-2028)
         */
        outstr[0] = '\0';
        return;
    }

    /* Move end pointer toward the front to first non-whitespace char */
    while (isspace(*endptr))
        endptr--;

    length = endptr + 1 - ptr;
    strncpy(outstr,ptr,length);
    outstr[length] = '\0';

}

/**
 * @brief This Function sets up the environment variable cache by parsing configuration file and adding
 * each name/value pairs to the list.
 *
 * @param[in] path Path of the configuration file.
 * @return Returns Returns RDK_SUCCESS if the setting of environment variable is successful else it returns -1.
 */
rdk_Error rdk_logger_env_add_conf_file( const char * path)
{
    const int line_buf_len = 256;
    static int number = 0;

    FILE* f;
    char lineBuffer[line_buf_len];

    /* Open the env file */
    if ((f = fopen( path,"r")) == NULL)
    {
        printf("***************************************************\n");
        printf("***************************************************\n");
        printf("**    ERROR!  Could not open configuration file!    **\n");
        printf("***************************************************\n");
        printf("***************************************************\n");
        printf("(Tried %s\n", path);
        return -1;
    }
    printf("Conf file %s open success\n", path);

    /* Read each line of the file */
    while (fgets(lineBuffer,line_buf_len,f) != NULL)
    {
        char name[line_buf_len];
        char value[line_buf_len];
        char trimname[line_buf_len];
        char trimvalue[line_buf_len];
        EnvVarNode *node;
        EnvVarNode *tmp_node;
        char *equals;
        int length;

        /* Ignore comment lines */
        if (lineBuffer[0] == '#')
            continue;

        /* Ignore lines that do not have an '=' char */
        if ((equals = strchr(lineBuffer,'=')) == NULL)
            continue;

        /* Read the property and store in the cache */
        length = equals - lineBuffer;
        strncpy( name,lineBuffer,length);
        name[ length] = '\0'; /* have to null-term */

        length = lineBuffer + strlen(lineBuffer) - equals + 1;
        strncpy( value,equals+1,length);
        value[ length] = '\0' ;

        /* Trim all whitespace from name and value strings */
        trim( name,trimname);
        trim( value,trimvalue);
        
        tmp_node = g_envCache;
        while(tmp_node)
        {
            if(strcmp(tmp_node->name, trimname) == 0)
            {
                break;
            }
            tmp_node = tmp_node->next;    
        }   
        
        if(!tmp_node)
        {
            node = ( EnvVarNode*)malloc(sizeof(EnvVarNode));
            node->name = strdup( trimname);
            node->value = strdup( trimvalue);
        }
        else
        {
            free(tmp_node->value);
            tmp_node->value = strdup( trimvalue);
            continue;
        }

        /** Update number only for the modules, not for environment variable */
        if ((strcmp("LOG.RDK.DEFAULT",node->name) != 0) && 
               (strcmp("EnableMPELog",node->name) != 0) && 
               (strcmp("SEPARATE.LOGFILE.SUPPORT",node->name) != 0))
        {
          number++; 
          node->number = number; 
        } else 
        node->number = 0;
	
        /* Insert at the front of the list */
        node->next = g_envCache;
        g_envCache = node;
    }

    global_count = number;

    fclose( f);
    return RDK_SUCCESS;
}

/**
 * @brief This function will get value of the specified environment variable.
 *
 * @param[in] name It is a pointer to the name of the target environment variable.
 *
 * @return Returns a pointer to the associated string value of the target environment
 * variable or return NULL if the variable can't be found.
 */
const char* rdk_logger_envGet(const char *name)
{
    EnvVarNode *node = g_envCache;

    while (node != NULL)
    {
        /* Env var name match */
        if (strcmp(name,node->name) == 0)
        {
            /* return the value */
            return node->value;
        }

        node = node->next;
    }

    /* Not found */
    return NULL;
}

/**
 * @brief This function is used to get the value of the specified environment variable based
 * on its registered number.
 *
 * @param[in] number Is a registered number of the target environment variable.
 * @return Returns a pointer to the associated string value of the target environment.
 * variable or return NULL in failure condition.
 */
const char* rdk_logger_envGetValueFromNum(int number)
{   
    EnvVarNode *node = g_envCache;
        
    while (node != NULL)
    {       
        /* Env var name match */
        if (number == node->number)
        {
            /* return the value */
            return node->value;
        }

        node = node->next;
    }

    /* Not found */
    return NULL;
}

/**
 * @brief Function will give the registered number of the specified environment variable.
 *
 * @param[in] mod It is a pointer to the name of the target environment variable.
 * @return Returns an integer value if the call is success otherwise returns -1.
 */
int rdk_logger_envGetNum(const char * mod)
{
    EnvVarNode *node = g_envCache;

    while (node != NULL)
    {
        /* Env var name match */
        if (strcmp(mod,node->name) == 0)
        {
            /* return the value */
            return node->number;
        }

        node = node->next;
    }

    /* Not found */
    return -1;
}

/**
 * @brief This function is used to get the name of the specified environment variable based
 * on its registered number.
 *
 * @param[in] Num Is a registered number of the target environment variable.
 * @return Returns a pointer to the associated string value of the target environment variable associated
 * for registered number. Return NULL if the environment variable can't be found for registered number.
 */
const char* rdk_logger_envGetModFromNum(int Num)
{
    EnvVarNode *node = g_envCache;

    while (node != NULL)
    {
        /* Env var name match */
        if (Num == node->number)
        {
            /* return the value */
            return node->name;
        }

        node = node->next;
    }

    /* Not found */
    return NULL;
}

