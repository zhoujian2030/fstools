/*
 * BeTcpService.cpp
 *
 *  Created on: July 10, 2017
 *      Author: j.zhou
 */


#include "BeTcpService.h"
#include "CLogger.h"
#include "TcpSocketTask.h"

using namespace cli;
using namespace net;
using namespace std;

#define SERVER_IP "0.0.0.0"
#define TCP_SERVER_PORT 54321

BeTcpService* BeTcpService::m_theInstance = 0;

// ------------------------------------------------
BeTcpService::BeTcpService() 
: Service("CliBeTcpService")
{
    init();

    m_tcpServerSocket = new CliTcpServerSocket(SERVER_IP, TCP_SERVER_PORT);

    m_numOfCliWorkers = MAX_NUM_CLI_WORKERS;
    m_cliWorkerArray = new CliWorker* [m_numOfCliWorkers];
    for (int i=0; i<m_numOfCliWorkers; i++) {
        m_cliWorkerArray[i] = new CliWorker(i);
    }

    m_numOfCliBeWorkers = MAC_NUM_CLI_BE_WORKERS;
    m_cliBeWorkerArray = new CliWorker* [m_numOfCliBeWorkers];
    for (int i=0; i<m_numOfCliBeWorkers; i++) {
        m_cliBeWorkerArray[i] = new CliWorker(i);
    }    
}

// ------------------------------------------------
BeTcpService::~BeTcpService() {
    delete m_tcpServerSocket;
    for (int i=0; i<m_numOfCliWorkers; i++) {
        delete m_cliWorkerArray[i];
    }
    delete m_cliWorkerArray;

    for (int i=0; i<m_numOfCliBeWorkers; i++) {
        delete m_numOfCliBeWorkers[i];
    }
    delete m_numOfCliBeWorkers;
}

// ------------------------------------------------
BeTcpService* BeTcpService::getInstance() {
    if (m_theInstance == 0) {
        m_theInstance = new BeTcpService();
    }
    return m_theInstance;
}

// ------------------------------------------------
void BeTcpService::addBeTask(cm::Task* theTask, int userData) {
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
CliWorker* BeTcpService::getWorker() {
    for (int i=0; i<m_numOfCliWorkers; i++) {
        if (m_cliWorkerArray[i]->isIdle()) {
            return m_cliWorkerArray[i];
        }
    }

    return 0;
}

// ------------------------------------------------
CliWorker* BeTcpService::getBeWorker() {
    for (int i=0; i<m_numOfCliBeWorkers; i++) {
        if (m_cliBeWorkerArray[i]->isIdle()) {
            return m_cliBeWorkerArray[i];
        }
    }

    return 0;
}

// ------------------------------------------------
unsigned long BeTcpService::run() {
    LOG_DBG(CLI_LOGGER_NAME, "[%s], Backend TCP service is running\n", __func__);

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

    return 0;
}
