/*
 * KpiService.cpp
 *
 *  Created on: May 05, 2017
 *      Author: j.zhou
 */

#include <arpa/inet.h>

#include "KpiService.h"
#include <iostream>
#include "CLogger.h"
#include "KpiWorker.h"
#include "Qmss.h"

using namespace kpi;
using namespace net;
using namespace cm;

UInt8 gLogLevel = 2;

// ------------------------------------------------
KpiService::KpiService(std::string serviceName) 
: Service(serviceName)
{
    init();
    m_transactionId = 0;
}

// ------------------------------------------------
KpiService::~KpiService() {

}

// ------------------------------------------------
unsigned long KpiService::run() {
    Qmss::initQmss();
    m_macQmss = new Qmss(Qmss::QID_KPI_SEND_TO_MAC, Qmss::QID_KPI_RECV_FROM_MAC);
    KpiWorker* pKpiWorker = new KpiWorker("KpiWorker", m_macQmss);

    pKpiWorker->start();

    while (true) {
        sendMacKpiReq();
        Thread::sleep(COLLECT_PERIOD_MS);
    }

    m_stopEvent.wait();

    return 0;
}

// ------------------------------------------------
int KpiService::sendMacKpiReq() {

    LteMacMsg* msg = (LteMacMsg*)m_sendBuffer;
    int length = LTE_MSG_HEAD_LENGTH;

    msg->transactionId = htons(++m_transactionId);
    msg->srcModuleId =  htons(KPI_MODULE_ID);
    msg->dstModuleId =  htons(MAC_MODULE_ID);
    msg->msgId = htons(MAC_KPI_REQ);
    msg->length = htons(length);

    
    LOG_DBG(KPI_LOGGER_NAME, "[%s], send KPI Req to MAC, msg length = %d\n", __func__, length);
    return m_macQmss->send(m_sendBuffer, length);
}
