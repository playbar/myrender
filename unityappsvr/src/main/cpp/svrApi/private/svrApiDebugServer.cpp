//=============================================================================
// FILE: svrApiDebugServer.cpp
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#include <strings.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "svrUtil.h"
#include "svrConfig.h"

#include "private/svrApiCore.h"
#include "private/svrApiDebugServer.h"

#define MESSAGE_BUFFER_LEN          512
#define MAX_PROCEDURES              256
#define MAX_PROCEDURE_NAME_LENGTH   80

using namespace Svr;

VAR(int, gDebugServerPort, 8080, kVariableNonpersistent);       //Port for the debug server to listen for incoming requests on

// Code derived from jsonrpc-c project (https://github.com/hmng/jsonrpc-c) under MIT license

struct jrpc_procedure 
{
    char            name[MAX_PROCEDURE_NAME_LENGTH];
    svrDebugRpcFn   function;
    void*           data;
};

struct jrpc_server 
{
    int             procedureCount;
    jrpc_procedure  procedures[MAX_PROCEDURES];
};


jrpc_server*    gpRpcServer = NULL;
pthread_t       gDebugServerThread;
bool            gStopDebugServer = false;

//-----------------------------------------------------------------------------
int SetSvrVariable(cJSON *params, void* data)
//-----------------------------------------------------------------------------
{
    char varBuffer[256];
    int  retVal;

    int arrSize = cJSON_GetArraySize(params);

    cJSON* key = NULL;
    cJSON* value = NULL;

    if (arrSize != 2)
    {
        LOGE("SetSvrVariable: Invalid number of parameters, recieved %d expected 2", arrSize);
        return -1;
    }

    key = cJSON_GetArrayItem(params, 0);
    value = cJSON_GetArrayItem(params, 1);

    if (key && value)
    {
        snprintf(varBuffer, 256, "%s = %s", key->valuestring, value->valuestring);
        LoadVariable(varBuffer);
        LOGI("SetSvrVariable: %s", varBuffer);
        retVal = 0;
    }
    else
    {
        LOGE("SetSvrVariable: Unable to parse RPC data");
        LOGE("%s", cJSON_Print(params));
        retVal = -1;
    }

    return retVal;
}

//-----------------------------------------------------------------------------
int InvokeProcedure(char* name, cJSON* params)
//-----------------------------------------------------------------------------
{
    int ret;
    bool procedure_found = false;
  
    int i = gpRpcServer->procedureCount;
    while (i--) 
    {
        if (!strcmp(gpRpcServer->procedures[i].name, name)) 
        {
            procedure_found = true;
            ret = gpRpcServer->procedures[i].function(params, gpRpcServer->procedures[i].data);
            break;
        }
    }

    if (!procedure_found)
    {
        LOGE("svrDebugServer: failed to find procedure '%s'", name);
        ret = -1;
    }

    return ret;
}

//-----------------------------------------------------------------------------
int HandleMessage(cJSON *root)
//-----------------------------------------------------------------------------
{
    cJSON* method;
    cJSON* params;

    method = cJSON_GetObjectItem(root, "method");
    if (method != NULL && method->type == cJSON_String) 
    {
        params = cJSON_GetObjectItem(root, "params");
        if (params == NULL || params->type == cJSON_Array || params->type == cJSON_Object) 
        {
           LOGI("svrDebugServer: Method Invoked: %s\n", method->valuestring);
           return InvokeProcedure(method->valuestring, params);
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
void* DebugServerMain(void* arg)
//-----------------------------------------------------------------------------
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[MESSAGE_BUFFER_LEN];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero((char *)&serv_addr, sizeof(serv_addr));

    portno = gDebugServerPort;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    //Set the socket to non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) < 0)
    {
        LOGE("svrDebugServer: Failed to set O_NONBLOCK on server socket");
    }

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        LOGE("svrDebugServer: ERROR on binding");
    }

    if (listen(sockfd, 5) < 0)
    {
        LOGE("svrDebugServer: ERROR on listen");
    }
    else
    {
        LOGI("svrDebugServer: Listening for connections...");
    }

    clilen = sizeof(cli_addr);

    pollfd connectPollFd;
    memset(&connectPollFd, 0, sizeof(pollfd));

    connectPollFd.fd = sockfd;
    connectPollFd.events = POLLIN;

    while (1)
    {
        if (gStopDebugServer)
        {
            break;
        }

        //Poll once a second to incoming connections
        int pollRes = poll(&connectPollFd, 1, 1000);
        if (pollRes == 0)
        {
            continue;
        }

        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
        {
            LOGE("svrDebugServer: ERROR on accept");
        }
     
        bzero(buffer, MESSAGE_BUFFER_LEN);
        n = read(newsockfd, buffer, MESSAGE_BUFFER_LEN);
        if (n < 0)
        {
            LOGE("svrDebugServer: ERROR reading from socket");
        }
        
        cJSON* root = cJSON_Parse(buffer);
        if (root != NULL)
        {
            HandleMessage(root);
            cJSON_Delete(root);
        }
        else
        {
            LOGE("svrDebugServer: Failed to parse JSON RPC");
            LOGE("    Error before: [%s]\n", cJSON_GetErrorPtr());
            LOGE("%s", buffer);
        }
       
        close(newsockfd);
    }

    close(sockfd);

    return 0;
}

//-----------------------------------------------------------------------------
bool svrStartDebugServer()
//-----------------------------------------------------------------------------
{
    if (gpRpcServer != NULL)
    {
        LOGE("svrDebugServer: svrStartDebugServer called while server is already running");
        return true;
    }

    gpRpcServer = new jrpc_server();
    memset(gpRpcServer, 0, sizeof(jrpc_server));

    gStopDebugServer = false;

    int status = pthread_create(&gDebugServerThread, NULL, &DebugServerMain, NULL);
    if (status != 0)
    {
        LOGE("svrDebugServer: Failed to create server thread");
        delete gpRpcServer;
        gpRpcServer = NULL;
        return false;
    }

    pthread_setname_np(gDebugServerThread, "svrDebugServer");

    svrRegisterDebugFn(SetSvrVariable, "SetSvrVariable", NULL);

    return true;
}

//-----------------------------------------------------------------------------
bool svrStopDebugServer()
//-----------------------------------------------------------------------------
{
    if (gpRpcServer == NULL)
    {
        LOGE("svrDebugServer: svrStopDebugServer called without server running");
        return true;
    }

    LOGI("svrDebugServer: Requested Stop");
    gStopDebugServer = true;
    pthread_join(gDebugServerThread, NULL);
    LOGI("svrDebugServer: Stopped");

    delete gpRpcServer;
    gpRpcServer = NULL;

    return true;
}

//-----------------------------------------------------------------------------
int svrRegisterDebugFn(svrDebugRpcFn fnPtr, char const* name, void * data)
//-----------------------------------------------------------------------------
{
    int i = gpRpcServer->procedureCount++;
    if (i >= MAX_PROCEDURES)
    {
        LOGE("svrDebugServer: Procedure registration failed - too many procedures registered");
        return -1;
    }

    strncpy(&gpRpcServer->procedures[i].name[0], name, MAX_PROCEDURE_NAME_LENGTH);
    gpRpcServer->procedures[i].function = fnPtr;
    gpRpcServer->procedures[i].data = data;

    LOGI("svrDebugServer: Registered %s", name);

    return 0;
}