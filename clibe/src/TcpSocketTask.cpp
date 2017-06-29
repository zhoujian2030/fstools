/*
 * TcpSocketTask.cpp
 *
 *  Created on: June 14, 2017
 *      Author: j.zhou
 */

#include "TcpSocketTask.h"
#include "CLogger.h"
#include "BeService.h"
#include "KpiTask.h"
#include "CliCommon.h"

using namespace cli;
using namespace net;
using namespace cm;

// ----------------------------------------
TcpSocketTask::TcpSocketTask(TcpSocket* tcpSocket) 
: m_tcpSocket(tcpSocket)
{
    m_tcpDataBuffer = new DataBuffer(2048);
}

// ----------------------------------------
TcpSocketTask::~TcpSocketTask() {
    if (m_tcpSocket) {
        delete m_tcpSocket;
    }
    delete m_tcpDataBuffer;
}

// ----------------------------------------
int TcpSocketTask::execute() {
    int length = 0;
    while (true) {
        if (!m_tcpSocket) {
            return TRC_END;
        }

        int length = m_tcpSocket->receive(m_tcpDataBuffer->getEndOfDataPointer(), m_tcpDataBuffer->getRemainBufferSize());
        if (length <= 0) {
            LOG_DBG(CLI_LOGGER_NAME, "[%s], connection is closed or error happened\n", __func__);
            return TRC_END;
        }

        CliMsg* cliMsg = (CliMsg*)m_tcpDataBuffer->getStartOfDataPointer();
        switch (cliMsg->msgId) {
            case LTE_MAC_KPI_REQ_START:
            {
                LOG_DBG(CLI_LOGGER_NAME, "[%s], recv LTE_MAC_KPI_REQ_START\n", __func__);
                CliLteMacKpiReq* kpiReq = (CliLteMacKpiReq*)cliMsg->msgBody;
                KpiTask* task = new KpiTask(*kpiReq);
                BeService::getInstance()->addBeTask(task, cliMsg->msgId);
                break;
            }

            case LTE_MAC_KPI_REQ_STOP:
            {

                break;
            }

            case LTE_L2_SET_LOGLEVEL_REQ:
            {

                break;
            }

            default:
            {
                LOG_WARN(CLI_LOGGER_NAME, "[%s], unsupported msgId = %d\n", __func__, cliMsg->msgId);
                break;
            }
        }
    }
}

