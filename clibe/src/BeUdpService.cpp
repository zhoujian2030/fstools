/*
 * BeUdpService.cpp
 *
 *  Created on: June 14, 2017
 *      Author: j.zhou
 */


#include "BeUdpService.h"
#include "CLogger.h"

using namespace cli;
using namespace net;
using namespace std;

#define SERVER_IP "0.0.0.0"
#define UDP_SERVER_PORT 54322

BeUdpService* BeUdpService::m_theInstance = 0;

// ------------------------------------------------
BeUdpService::BeUdpService() 
: Service("CliBeUdpService")
{
    init();

    m_numOfCliWorkers = MAX_NUM_CLI_WORKERS;
    m_cliWorkerArray = new CliWorker* [m_numOfCliWorkers];
    for (int i=0; i<m_numOfCliWorkers; i++) {
        m_cliWorkerArray[i] = new CliWorker(i);
    }

    m_udpSocket = new UdpSocket(SERVER_IP, UDP_SERVER_PORT);
}

// ------------------------------------------------
BeUdpService::~BeUdpService() {
    for (int i=0; i<m_numOfCliWorkers; i++) {
        delete m_cliWorkerArray[i];
    }
    delete m_cliWorkerArray;

    delete m_udpSocket;

}

// ------------------------------------------------
BeUdpService* BeUdpService::getInstance() {
    if (m_theInstance == 0) {
        m_theInstance = new BeUdpService();
    }
    return m_theInstance;
}

// ------------------------------------------------
void BeUdpService::addBeTask(cm::Task* theTask, int userData) {
    if (theTask != 0) {
        CliWorker* worker = this->getBeWorker();
        if (worker != 0) {
            worker->setTask(theTask);
            worker->setUserData(userData);
        } else {
            delete theTask;
        }
    }
}

// ------------------------------------------------
CliWorker* BeUdpService::getWorker() {
    for (int i=0; i<m_numOfCliWorkers; i++) {
        if (m_cliWorkerArray[i]->isIdle()) {
            return m_cliWorkerArray[i];
        }
    }

    return 0;
}

// ------------------------------------------------
unsigned long BeUdpService::run() {
    LOG_DBG(CLI_LOGGER_NAME, "[%s], Backend UDP service is running\n", __func__);

    UInt32 length = 0;
    Socket::InetAddressPort clientAddress;
    while (true) {
        length = m_udpSocket->receive(m_recvBuffer, IP_RECV_MSG_BUFFER_LENGTH, clientAddress);
        if (length > 0) {
           
        } else {
            LOG_ERROR(KPI_LOGGER_NAME, "[%s], receive UDP data error\n", __func__);
        }
    }

    return 0;
}
