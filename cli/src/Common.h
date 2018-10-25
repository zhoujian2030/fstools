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
#define TGT_TYPE_SIM_NAME "SIM"
#define TGT_TYPE_KPI_NAME "KPI"
typedef enum {
    TGT_L2 = 1,
    TGT_SIM = 2,
    TGT_KPI = 3
} TARGET_TYPE;

#define SUB_TGT_TYPE_LOG_LVL_NAME "LOGLEVEL"
#define SUB_TGT_TYPE_RAT2_TYPE_NAME "RAT2TYPE"
#define SUB_TGT_TYPE_RACH_THR_NAME "RACHTHR"
#define SUB_TGT_TYPE_MAX_UE_SCHED_NAME "MAXUESCHED"
#define SUB_TGT_TYPE_TARGET_UE_NAME "TARGETUE"
typedef enum {
    SUB_TGT_LOG_LEVEL = 1,
    SUB_TGT_RAT2_TYPE = 2,
    SUB_TGT_RACH_THR = 3,
    SUB_TGT_MAX_UE_SCHED = 4,
    SUB_TGT_TARGET_UE_ID = 5
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

#define DISTRIBUTED_NAME "DISTRIBUTED"
#define LOCALIZED_NAME "LOCALIZED"
typedef enum {
    DISTRIBUTED = 0,
    LOCALIZED = 1
} RAT2Type;


#endif

