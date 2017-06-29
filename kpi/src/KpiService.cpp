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

#ifdef KPI_L3
#define RRC_UDP_SERVER_IP "127.0.0.1"
#define RRC_UDP_SERVER_PORT 60000
#define KPI_UDP_SERVER_IP "127.0.0.1"
#define KPI_UDP_SERVER_PORT 60001
#endif

// ------------------------------------------------
KpiService::KpiService(std::string serviceName) 
: Service(serviceName)
{
    init();
    m_transactionId = 0;
    m_period = gPeriod;
}

// ------------------------------------------------
KpiService::~KpiService() {
#ifdef KPI_L3
    delete m_udpServerSocket;
#endif
}

// ------------------------------------------------
unsigned long KpiService::run() {
#ifdef KPI_L3
    m_udpServerSocket = new UdpSocket(KPI_UDP_SERVER_IP, KPI_UDP_SERVER_PORT);
    m_rrcAddress.port = RRC_UDP_SERVER_PORT;
    Socket::getSockaddrByIpAndPort(&m_rrcAddress.addr, RRC_UDP_SERVER_IP, RRC_UDP_SERVER_PORT);
    KpiWorker* pKpiWorker = new KpiWorker("KpiWorker", m_udpServerSocket);
#else 
    Qmss::initQmss();
    m_macQmss = new Qmss(Qmss::QID_CLI_SEND_TO_L2, Qmss::QID_CLI_RECV_FROM_L2);
    KpiWorker* pKpiWorker = new KpiWorker("KpiWorker", m_macQmss);
#endif

#ifndef GSM
    LteCounter lteCounter;
    memset((void*)&lteCounter, 0, sizeof(LteCounter));
    pKpiWorker->displayCounter((void*)&lteCounter);
#else 
    GSMCounter gsmCounter;
    memset((void*)&gsmCounter, 0, sizeof(GSMCounter));
    pKpiWorker->displayCounter((void*)&gsmCounter);
#endif
    pKpiWorker->start();

    while (true) {
        sendMacKpiReq();
        Thread::sleep(m_period);
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
    msg->length = htons(length);

#ifdef KPI_L3
    msg->dstModuleId =  htons(RRC_MODULE_ID);
    msg->msgId = htons(RRC_KPI_REQ);
    LOG_DBG(KPI_LOGGER_NAME, "[%s], send KPI Req to RRC, msg length = %d\n", __func__, length);
    m_udpServerSocket->send(m_sendBuffer, length, m_rrcAddress);
#else
    msg->dstModuleId =  htons(MAC_MODULE_ID);
    msg->msgId = htons(MAC_KPI_REQ);
    LOG_DBG(KPI_LOGGER_NAME, "[%s], send KPI Req to MAC, msg length = %d\n", __func__, length);
    return m_macQmss->send(m_sendBuffer, length);
#endif
}
