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
#include "CommandParser.h"
#include "CliCommon.h"
#include "MacInterface.h"
#include "Qmss.h"
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace cli;

// ---------------------------------------
CommandParser::CommandParser() 
: m_isValid(false), m_cmdType(0), m_tgtType(0), m_subTgtType(0)
{
    memset((void*)m_cmdContent, 0, MAX_COMMAND_CONTENT_LENGTH);
}

// ---------------------------------------
CommandParser::~CommandParser() {

}

// ---------------------------------------
bool CommandParser::parse(int argc, char* argv[]) {
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
bool CommandParser::send(Qmss* qmss) {
    if (m_isValid) {
        LteMacMsg* msg = (LteMacMsg*)m_sendBuffer;
        msg->transactionId = htons(1111);
        msg->srcModuleId =  htons(CLI_MODULE_ID);
        msg->dstModuleId =  htons(MAC_MODULE_ID);
        int length = LTE_MSG_HEAD_LENGTH;

        if ((m_tgtType == TGT_L2) && (m_subTgtType == SUB_TGT_LOG_LEVEL)) {
            if ((m_cmdType == SET)) {
                msg->msgId = htons(MAC_CLI_SET_LOG_LEVEL_REQ);
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
        } else if ((m_tgtType == TGT_L2) && (m_subTgtType == SUB_TGT_RAT2_TYPE)) {
            if ((m_cmdType == SET)) {
                msg->msgId = htons(MAC_CLI_SET_COMM_CHAN_RAT2);
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
        } else if ((m_tgtType == TGT_L2) && (m_subTgtType == SUB_TGT_RACH_THR)) {
            if ((m_cmdType == SET)) {
                msg->msgId = htons(MAC_CLI_SET_RACH_THRESTHOLD);
                length += sizeof(UInt32);
                msg->length = htons(length);
                UInt32* rachThr = (UInt32*)msg->msgBody;
                int* value = (int*)m_cmdContent;
                *rachThr = *value;
                cout << "Set Rach Thresthold: " << *value << endl; 
            } else {
                showUsage();
                LOG_DBG(CLI_LOGGER_NAME, "[%s], NOT support Rach Thresthold yet\n", __func__);
                return false;
            }
        } else {
            showUsage();
            LOG_DBG(CLI_LOGGER_NAME, "[%s], unsupported command parameters\n", __func__);
            return false;
        }    

        qmss->send(m_sendBuffer, length);
        return true;
    } else {
        showUsage();
        LOG_DBG(CLI_LOGGER_NAME, "[%s], invalid command\n", __func__);
        return false;
    }
}

int CommandParser::s2i(string theString) {
    int result;
    stringstream ss;
    ss << theString;
    ss >> result;

    // TODO check if success, handle the exception

    return result;
}

// ---------------------------------------
bool CommandParser::parseParam(std::string option, int index) {
    transform(option.begin(), option.end(), option.begin(), ::toupper);
    LOG_DBG(CLI_LOGGER_NAME, "[%s], option  = %s, index = %d\n", __func__, option.c_str(), index);

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
        } else {
            LOG_DBG(CLI_LOGGER_NAME, "[%s], invalid option = %s\n", __func__, option.c_str());
            return false;
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
        }
    } else {
        return false;
    } 

    return true;   
}

// ---------------------------------------
void CommandParser::showUsage() {
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
}
