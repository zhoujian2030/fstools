/*
 * BeService.cpp
 *
 *  Created on: June 14, 2017
 *      Author: j.zhou
 */


#include "BeService.h"
#include "CLogger.h"
#include "TcpSocketTask.h"

using namespace cli;
using namespace net;
using namespace std;

#define SERVER_IP "0.0.0.0"
#define TCP_SERVER_PORT 54321
#define UDP_SERVER_PORT 54322

BeService* BeService::m_theInstance = 0;

// ------------------------------------------------
BeService::BeService() 
: Service("CliBeService")
{
    init();

#ifndef USING_UDP
    m_tcpServerSocket = new CliTcpServerSocket(SERVER_IP, TCP_SERVER_PORT);

    m_numOfCliWorkers = MAX_NUM_CLI_WORKERS;
    m_cliWorkerArray = new CliWorker* [m_numOfCliWorkers];
    for (int i=0; i<m_numOfCliWorkers; i++) {
        m_cliWorkerArray[i] = new CliWorker(i);
    }
#else 
    m_udpSocket = new UdpSocket(SERVER_IP, UDP_SERVER_PORT);
#endif

    m_numOfCliBeWorkers = MAC_NUM_CLI_BE_WORKERS;
    m_cliBeWorkerArray = new CliWorker* [m_numOfCliBeWorkers];
    for (int i=0; i<m_numOfCliBeWorkers; i++) {
        m_cliBeWorkerArray[i] = new CliWorker(i);
    }    
}

// ------------------------------------------------
BeService::~BeService() {
#ifndef USING_UDP
    delete m_tcpServerSocket;
    for (int i=0; i<m_numOfCliWorkers; i++) {
        delete m_cliWorkerArray[i];
    }
    delete m_cliWorkerArray;
#else 
    delete m_udpSocket;
#endif

    for (int i=0; i<m_numOfCliBeWorkers; i++) {
        delete m_numOfCliBeWorkers[i];
    }
    delete m_numOfCliBeWorkers;
}

// ------------------------------------------------
BeService* BeService::getInstance() {
    if (m_theInstance == 0) {
        m_theInstance = new BeService();
    }
    return m_theInstance;
}

// ------------------------------------------------
void BeService::addBeTask(cm::Task* theTask, int userData) {
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

#ifndef USING_UDP
// ------------------------------------------------
CliWorker* BeService::getWorker() {
    for (int i=0; i<m_numOfCliWorkers; i++) {
        if (m_cliWorkerArray[i]->isIdle()) {
            return m_cliWorkerArray[i];
        }
    }

    return 0;
}
#endif

// ------------------------------------------------
CliWorker* BeService::getBeWorker() {
    for (int i=0; i<m_numOfCliBeWorkers; i++) {
        if (m_cliBeWorkerArray[i]->isIdle()) {
            return m_cliBeWorkerArray[i];
        }
    }

    return 0;
}

// ------------------------------------------------
unsigned long BeService::run() {
    LOG_DBG(CLI_LOGGER_NAME, "[%s], Backend service is running\n", __func__);

#ifndef USING_UDP
    TcpSocket* newSocket = 0;
    while (true) {
        newSocket = m_tcpServerSocket->accept();
        if (newSocket == 0) {
            LOG_ERROR(CLI_LOGGER_NAME, "[%s], Error TCP connection\n", __func__);
            exit(0);
        } 

        LOG_DBG(CLI_LOGGER_NAME, "[%s], accept new connection\n", __func__);

        CliWorker* worker = this->getWorker();
        if (worker != 0) {
            TcpSocketTask* task = new TcpSocketTask(newSocket);
            worker->setTask(task);
        } else {
            LOG_WARN(CLI_LOGGER_NAME, "[%s], no available worker now, ignore the new connection\n", __func__);
            delete newSocket;
        }

        newSocket = 0;
    }

#else 

    UInt32 length = 0;
    Socket::InetAddressPort clientAddress;
    while (true) {
        length = m_udpSocket->receive(m_recvBuffer, IP_RECV_MSG_BUFFER_LENGTH, clientAddress);
        if (length > 0) {
            (length);
        } else {
            LOG_ERROR(KPI_LOGGER_NAME, "[%s], receive UDP data error\n", __func__);
        }
    }

#endif

    return 0;
}
