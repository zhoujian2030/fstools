/*
 * CommandParser.cpp
 *
 *  Created on: June 13, 2017
 *      Author: j.zhou
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "CLogger.h"
#include "CliCommandParser.h"
#include "Common.h"
#include "CliCommon.h"
#include "Qmss.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include "KpiCommandParser.h"

using namespace std;
using namespace cli;
using namespace kpi;

// ---------------------------------------
CliCommandParser::CliCommandParser() 
: m_isValid(false), m_cmdType(0), m_tgtType(0), m_subTgtType(0), m_msgId(0)
{
    memset((void*)m_cmdContent, 0, MAX_COMMAND_CONTENT_LENGTH);
    m_numUe = 1;
    m_numTestTime = 1;
}

// ---------------------------------------
CliCommandParser::~CliCommandParser() {

}

// ---------------------------------------
bool CliCommandParser::parse(int argc, char* argv[]) {
    if (argc < 3) {
        LOG_DBG(CLI_LOGGER_NAME, "[%s], mismatch number of command parameters, argc = %d\n", __func__, argc);
        //showUsage();
        return false;
    }

    int count = argc - 1;
    // parse from the second param of main (the 1st param is the name of the executive process)
    int index = 1; 

    while (count--) {
        string value(argv[index]);
        if (!parseParam(value, index)) {
            //showUsage();
            return false;
        }

        index++;
    }

    return true;
}

// ---------------------------------------
bool CliCommandParser::execute(Qmss* qmss, int argc, char* argv[]) {
    if (m_isValid) {
        if (m_tgtType == TGT_KPI) {
            if (m_cmdType == GET) {
                LOG_DBG(CLI_LOGGER_NAME, "[%s], call KPI main function\n", __func__);
                KpiCommandParser kpiCmdParser;
#ifdef KPI_L3
                kpiCmdParser.parseAndExecute(argc-2, &argv[2]);
#else 
                kpiCmdParser.parseAndExecute(qmss, argc-2, &argv[2]);
#endif
                return true;
            } else {
                showUsage();
                LOG_DBG(CLI_LOGGER_NAME, "[%s], NOT support SET kpi\n", __func__);
                return false;
            }
        }

        LteCliMsg* msg = (LteCliMsg*)m_sendBuffer;
        msg->transactionId = htons(1111);
        msg->srcModuleId =  htons(CLI_MODULE_ID);
        int length = LTE_CLI_MSG_HEAD_LENGTH;

        if (m_tgtType == TGT_L2) {
            msg->dstModuleId =  htons(MAC_MODULE_ID);
            if (m_subTgtType == SUB_TGT_LOG_LEVEL) {
                if ((m_cmdType == SET)) {
                    msg->msgId = htons(L2_CLI_SET_LOG_LEVEL_REQ);
                    length += sizeof(SetLogLevelReq);
                    msg->length = htons(length);
                    SetLogLevelReq* logLevelReq = (SetLogLevelReq*)msg->msgBody;
                    int* logLevel = (int*)m_cmdContent;
                    logLevelReq->logLevel = *logLevel;
                    cout << "Set L2 loglevel to " << *logLevel << endl; 
                } else {
                    showUsage();
                    LOG_DBG(CLI_LOGGER_NAME, "[%s], NOT support GET log level yet\n", __func__);
                    return false;
                }
            } else if (m_subTgtType == SUB_TGT_RAT2_TYPE) {
                if ((m_cmdType == SET)) {
                    msg->msgId = htons(L2_CLI_SET_COMM_CHAN_RAT2);
                    length += sizeof(SetRAT2Type);
                    msg->length = htons(length);
                    SetRAT2Type* rat2TypeReq = (SetRAT2Type*)msg->msgBody;
                    int* value = (int*)m_cmdContent;
                    rat2TypeReq->value = *value;
                    cout << "Set Common Channel RAT2 Type: " << *value << endl; 
                } else {
                    showUsage();
                    LOG_DBG(CLI_LOGGER_NAME, "[%s], NOT support GET RAT2 Type yet\n", __func__);
                    return false;
                }
            } else if (m_subTgtType == SUB_TGT_RACH_THR) {
                if ((m_cmdType == SET)) {
                    msg->msgId = htons(L2_CLI_SET_RACH_THRESTHOLD);
                    length += sizeof(UInt32);
                    msg->length = htons(length);
                    UInt32* rachThr = (UInt32*)msg->msgBody;
                    int* value = (int*)m_cmdContent;
                    *rachThr = *value;
                    cout << "Set Rach Thresthold: " << *value << endl; 
                } else {
                    showUsage();
                    LOG_DBG(CLI_LOGGER_NAME, "[%s], NOT support GET Rach Thresthold yet\n", __func__);
                    return false;
                }
            } else if (m_subTgtType == SUB_TGT_MAX_UE_SCHED) {
                if ((m_cmdType == SET)) {
                    msg->msgId = htons(L2_CLI_SET_MAX_UE_SCHEDULED);
                    length += sizeof(UInt32);
                    msg->length = htons(length);
                    UInt32* maxUeSched = (UInt32*)msg->msgBody;
                    int* value = (int*)m_cmdContent;
                    *maxUeSched = *value;
                    cout << "Set Max UE scheduled: " << *value << endl; 
                } else {
                    showUsage();
                    LOG_DBG(CLI_LOGGER_NAME, "[%s], NOT support GET Max UE scheduled yet\n", __func__);
                    return false;
                }
            } else if (m_subTgtType == SUB_TGT_TARGET_UE_ID) {
                if ((m_cmdType == SET)) {
                    msg->msgId = htons(L2_CLI_SET_TARGET_UE);
                    length += sizeof(UInt32); // for ueIndex
                    length += sizeof(UInt32); // for interval
                    msg->length = htons(length);
                    UInt32* targetUeId = (UInt32*)msg->msgBody;
                    int* value = (int*)m_cmdContent;
                    *targetUeId = *value;
                    UInt32* interval = targetUeId+1;
                    *interval = *(value+1);
                    cout << "Set Target UE ID: " << *targetUeId << ", interval = " << *interval << endl; 
                } else {
                    showUsage();
                    LOG_DBG(CLI_LOGGER_NAME, "[%s], NOT support GET Max UE scheduled yet\n", __func__);
                    return false;
                }
            } else {
                showUsage();
                LOG_DBG(CLI_LOGGER_NAME, "[%s], unsupported command parameters\n", __func__);
                return false;
            }    
            qmss->send(m_sendBuffer, length);
        } else if (m_tgtType == TGT_SIM) {
            msg->dstModuleId =  htons(SIM_MODULE_ID);
            msg->msgId = htons(SIM_CLI_SET_PARAM_REQ);
            length += sizeof(SetSIMParamReq);
            msg->length = htons(length);
            SetSIMParamReq* simParamReq = (SetSIMParamReq*)msg->msgBody;
            simParamReq->numUe = m_numUe;
            simParamReq->numTestTime = m_numTestTime;
            cout << "Set SIM UE number: " << m_numUe << ", Test Time: " << m_numTestTime << endl; 

            qmss->updateDstQmssId(Qmss::QID_TST_SEND_START_TEST);
            qmss->send(m_sendBuffer, length);
        }

        return true;
    } else {
        showUsage();
        LOG_DBG(CLI_LOGGER_NAME, "[%s], invalid command\n", __func__);
        return false;
    }
}

int CliCommandParser::s2i(string theString) {
    int result;
    stringstream ss;
    ss << theString;
    ss >> result;

    // TODO check if success, handle the exception

    return result;
}

// ---------------------------------------
bool CliCommandParser::parseParam(std::string option, int index) {
    transform(option.begin(), option.end(), option.begin(), ::toupper);
    LOG_DBG(CLI_LOGGER_NAME, "[%s], option  = %s, index = %d\n", __func__, option.c_str(), index);

    // if ((index > 2) && (m_tgtType == TGT_KPI)) {
    //     return false;
    // }

    if (index == 1) {
        if (option.compare(CMD_TYPE_SET_NAME) == 0) {
            m_cmdType = SET;
        } else if (option.compare(CMD_TYPE_GET_NAME) == 0) {
            m_cmdType = GET;
        } else {
            LOG_DBG(CLI_LOGGER_NAME, "[%s], invalid option = %s\n", __func__, option.c_str());
            return false;
        }
    } else if (index == 2) {
        if (option.compare(TGT_TYPE_L2_NAME) == 0) {
            m_tgtType = TGT_L2;
        } else if (option.compare(TGT_TYPE_SIM_NAME) == 0) {
            m_tgtType = TGT_SIM;
            m_isValid = true;
        } else if (option.compare(TGT_TYPE_KPI_NAME) == 0) {
            m_tgtType = TGT_KPI;
            m_isValid = true;
            return false;
        } else {
            LOG_DBG(CLI_LOGGER_NAME, "[%s], invalid option = %s\n", __func__, option.c_str());
            return false;
        }
    } else if (index == 3) {
        if (option.compare(SUB_TGT_TYPE_LOG_LVL_NAME) == 0) {
            m_subTgtType = SUB_TGT_LOG_LEVEL;
        } else if (option.compare(SUB_TGT_TYPE_RAT2_TYPE_NAME) == 0) {
            m_subTgtType = SUB_TGT_RAT2_TYPE;
        } else if (option.compare(SUB_TGT_TYPE_RACH_THR_NAME) == 0) {
            m_subTgtType = SUB_TGT_RACH_THR;
        } else if (option.compare(SUB_TGT_TYPE_MAX_UE_SCHED_NAME) == 0) {
            m_subTgtType = SUB_TGT_MAX_UE_SCHED;
        } else if (option.compare(SUB_TGT_TYPE_TARGET_UE_NAME) == 0) {
            m_subTgtType = SUB_TGT_TARGET_UE_ID;
        }  else {
            if (m_tgtType == TGT_SIM) {
                m_numUe = s2i(option);
            } else {
                LOG_DBG(CLI_LOGGER_NAME, "[%s], invalid option = %s\n", __func__, option.c_str());
                return false;
            }
        }
    } else if (index == 4) {
        if (m_subTgtType == SUB_TGT_LOG_LEVEL) {
            int* logLevel = (int*)m_cmdContent;
            if (option.compare(TRACE_NAME) == 0) {
                *logLevel = TRACE;
                m_isValid = true;
            } else if (option.compare(DEBUG_NAME) == 0) {
                *logLevel = DEBUG;
                m_isValid = true;
            } else if (option.compare(INFO_NAME) == 0) {
                *logLevel = INFO;
                m_isValid = true;
            } else if (option.compare(WARNINNG_NAME) == 0) {
                *logLevel = WARNING;
                m_isValid = true;
            } else if (option.compare(ERROR_NAME) == 0) {
                *logLevel = ERROR;
                m_isValid = true;
            } else {
                LOG_DBG(CLI_LOGGER_NAME, "[%s], invalid option = %s\n", __func__, option.c_str());
                return false;
            }
        } else if (m_subTgtType == SUB_TGT_RAT2_TYPE) {
            int* rat2Type = (int*)m_cmdContent;
            if (option.compare(DISTRIBUTED_NAME) == 0) {
                *rat2Type = DISTRIBUTED;
                m_isValid = true;
            } else if (option.compare(LOCALIZED_NAME) == 0) {
                *rat2Type = LOCALIZED;
                m_isValid = true; 
            } else {
                LOG_DBG(CLI_LOGGER_NAME, "[%s], invalid option = %s\n", __func__, option.c_str());
                return false;
            }
        } else if (m_subTgtType == SUB_TGT_RACH_THR) {
            int* rachThr = (int*)m_cmdContent;
            *rachThr = s2i(option);
            m_isValid = true;
        } else if (m_subTgtType == SUB_TGT_MAX_UE_SCHED) {
            int* maxUeSched = (int*)m_cmdContent;
            *maxUeSched = s2i(option);
            m_isValid = true;
        } else if (m_subTgtType == SUB_TGT_TARGET_UE_ID) {
            int* targetUeId = (int*)m_cmdContent;
            *targetUeId = s2i(option);
            int* interval = (int*)(m_cmdContent + 4);
            *interval = 1;
            m_isValid = true;
        } else if (m_tgtType == TGT_SIM) {
            m_numTestTime = s2i(option);
            m_isValid = true;
        }
    } else if (index == 5) {
        if (m_subTgtType == SUB_TGT_TARGET_UE_ID) {
            int* interval = (int*)(m_cmdContent + 4);
            *interval = s2i(option);
        }
    } else {
        return false;
    } 

    return true;   
}

// ---------------------------------------
void CliCommandParser::showUsage() {
    cout << "Usage: " << endl;
    cout << "  cli set [param1] [param2] [param3]" << endl;
    cout << "  cli get [param1] [param2] [param3]" << endl << endl;

    cout << "Options: " << endl;
    cout << "  set : set and change config" << endl;
    cout << "  get : get current config" << endl;
    cout << endl;

    cout << "Example: " << endl;
    cout << "  cli set l2 loglevel [trace/debug/info/warning/error]" << endl;    
    cout << "  cli set l2 rat2type [distributed/localized]" << endl; 
    cout << "  cli set l2 rachthr  [1, 2, 3, ...]" << endl; 
    cout << "  cli set l2 maxuesched  [1, 2, 3, 4]" << endl; 
    cout << "  cli set l2 targetue  [0, 1, 2, ..., 254] [1, 2, 3, ...]" << endl; 
    cout << "  cli get kpi [kpi options]" << endl; 
}
