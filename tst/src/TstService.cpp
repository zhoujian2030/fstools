/*
 * TstService.cpp
 *
 *  Created on: May 10, 2017
 *      Author: j.zhou
 */

#include <arpa/inet.h>

#include "TstService.h"
#include <iostream>
#include "CLogger.h"
#include "Qmss.h"

using namespace tst;
using namespace cm;

UInt8 gLogLevel = 1;

// ------------------------------------------------
TstService::TstService(std::string serviceName) 
: Service(serviceName)
{
    init();
}

// ------------------------------------------------
TstService::~TstService() {

}

// ------------------------------------------------
unsigned long TstService::run() {
    Qmss::initQmss();
    m_simQmss = new Qmss(Qmss::QID_TST_SEND_START_TEST, Qmss::QID_INVALID_ID);

    LteTstMsg* msg = (LteTstMsg*)m_sendBuffer;
    int length = LTE_MSG_HEAD_LENGTH;

    msg->transactionId = 0;
    msg->srcModuleId =  htons(TST_MODULE_ID);
    msg->dstModuleId =  htons(SIM_MODULE_ID);
    msg->msgId = htons(SIM_TST_START);
    msg->length = htons(length);

    
    LOG_DBG(KPI_LOGGER_NAME, "[%s], send Start Test Req to SIM, msg length = %d\n", __func__, length);
    m_simQmss->send(m_sendBuffer, length);    

    m_stopEvent.wait();
}
