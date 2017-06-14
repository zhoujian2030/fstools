/*
 * CliCommon.h
 *
 *  Created on: June 13, 2017
 *      Author: j.zhou
 */

#ifndef CLI_COMMON_H
#define CLI_COMMON_H

#define CMD_TYPE_SET_NAME "SET"
#define CMD_TYPE_GET_NAME "GET"
typedef enum {
    SET = 1,
    GET = 2
} CliCmdType;

#define TGT_TYPE_L2_NAME "L2"
typedef enum {
    TGT_L2 = 1
} TARGET_TYPE;

#define SUB_TGT_TYPE_LOG_LVL_NAME "LOGLEVEL"
typedef enum {
    SUB_TGT_LOG_LEVEL = 1
} SUB_TARGET_TYPE;

#define TRACE_NAME "TRACE"
#define DEBUG_NAME "DEBUG"
#define INFO_NAME "INFO"
#define WARNINNG_NAME "WARNING"
#define ERROR_NAME "ERROR"
typedef enum {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4
} L2LogLevel;


#endif

