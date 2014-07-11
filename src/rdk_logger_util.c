/*
 * ============================================================================
 * COMCAST C O N F I D E N T I A L AND PROPRIETARY
 * ============================================================================
 * This file and its contents are the intellectual property of Comcast.  It may
 * not be used, copied, distributed or otherwise  disclosed in whole or in part
 * without the express written permission of Comcast.
 * ============================================================================
 * Copyright (c) 2014 Comcast. All rights reserved.
 * ============================================================================
 */


/*
 * This file provides the CableLabs Reference Implementation of the rdk_logger_ utility APIs.
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


/* Node for storing env vars in a linked list cache */
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
pthread_mutex_t g_cacheMutex = PTHREAD_MUTEX_INITIALIZER;

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
 * The <i>rdk_logger_env_add_conf_file()</i> function sets up the environment variable
 * storage by parsing configuration file.
 *
 * @param path Path of the file.
 * @return Returns <i>-1<i> on failure, otherwise <i>RDK_SUCCESS</i>
 *          is returned.
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

     pthread_mutex_lock(&g_cacheMutex);
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
    pthread_mutex_unlock( &g_cacheMutex);

    fclose( f);
    return RDK_SUCCESS;
}

/**
 * The <i>rdk_logger_envGet()</i> function will get the value of the specified
 * environment variable.
 *
 * @param name is a pointer to the name of the target environment variable.
 * @return NULL if the create fails, otherwise <i>value<i/>
 *          is returned.
 */
const char* rdk_logger_envGet(const char *name)
{
    EnvVarNode *node = g_envCache;

    pthread_mutex_lock(&g_cacheMutex);

    while (node != NULL)
    {
        /* Env var name match */
        if (strcmp(name,node->name) == 0)
        {
            /* return the value */
            pthread_mutex_unlock(&g_cacheMutex);
            return node->value;
        }

        node = node->next;
    }

    /* Not found */
    pthread_mutex_unlock(&g_cacheMutex);
    return NULL;
}

/**
 * The <i>rdk_logger_envGetValueFromNum()</i> function will get the value of the specified
 * environment variable based on its registered number.
 *
 * @param number is a registered number of the target environment variable.
 * @return NULL if the create fails, otherwise <i>value<i/>
 *          is returned.
 */
const char* rdk_logger_envGetValueFromNum(int number)
{   
    EnvVarNode *node = g_envCache;
        
    pthread_mutex_lock(&g_cacheMutex);
        
    while (node != NULL)
    {       
        /* Env var name match */
        if (number == node->number)
        {
            /* return the value */
            pthread_mutex_unlock(&g_cacheMutex);
            return node->value;
        }

        node = node->next;
    }

    /* Not found */
    pthread_mutex_unlock(&g_cacheMutex);
    return NULL;
}
/**
 * The <i>rdk_logger_envGetNum()</i> function will give the registered number 
 * of the specified environment variable.
 *
 * @param mod is a pointer to the name of the target environment variable.
 * @return -1 if fails, otherwise <i>number<i/>
 *          is returned.
 */

int rdk_logger_envGetNum(const char * mod)
{
    EnvVarNode *node = g_envCache;

    pthread_mutex_lock(&g_cacheMutex);

    while (node != NULL)
    {
        /* Env var name match */
        if (strcmp(mod,node->name) == 0)
        {
            /* return the value */
            pthread_mutex_unlock(&g_cacheMutex);
            return node->number;
        }

        node = node->next;
    }

    /* Not found */
    pthread_mutex_unlock(&g_cacheMutex);
    return -1;
}

/**
 * The <i>rdk_logger_envGetModFromNum()</i> function will get the name of the specified
 * environment variable based on its registered number.
 *
 * @param number is a registered number of the target environment variable.
 * @return NULL if it fails, otherwise <i>name<i/>
 *          is returned.
 */
const char* rdk_logger_envGetModFromNum(int Num)
{
    EnvVarNode *node = g_envCache;

    pthread_mutex_lock(&g_cacheMutex);

    while (node != NULL)
    {
        /* Env var name match */
        if (Num == node->number)
        {
            /* return the value */
            pthread_mutex_unlock(&g_cacheMutex);
            return node->name;
        }

        node = node->next;
    }

    /* Not found */
    pthread_mutex_unlock(&g_cacheMutex);
    return NULL;
}

