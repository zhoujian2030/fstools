/*
 * CliCommon.h
 *
 *  Created on: June 15, 2017
 *      Author: j.zhou
 */

#ifndef CLI_COMMON_H
#define CLI_COMMON_H

#include "CliCommon.h"

#define IP_RECV_MSG_BUFFER_LENGTH 1500

#define CLI_MSG_HEADER_LENGTH 4

#define MAC_VAR_SIZE(x) 1
typedef struct {
    UInt16 msgId;
    UInt16 length;
    char msgBody[MAC_VAR_SIZE(length)];
} CliMsg;

typedef enum {
    LTE_MAC_KPI_REQ_START = 1,
    LTE_MAC_KPI_REQ_STOP = 2,
    LTE_L2_SET_LOGLEVEL_REQ = 3
} CliMsgId;

typedef struct {
    UInt16 interval;        // in milli second
    UInt8 writeToFile;      // 0: not write to file  
    UInt8 writeToSocket;    // 0: not write to socket
    UInt16 duration;        // in second
    UInt16 port;            // UDP server port
    char ip[32];            // UDP serer IP
} CliLteMacKpiReq;

typedef struct {
    UInt32 loglevel;
} CliLteL2SetLoglevelReq;

#endif
