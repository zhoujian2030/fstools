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

#define SERVER_IP "127.0.0.1"
#define TCP_SERVER_PORT 5432

BeService* BeService::m_theInstance = 0;

// ------------------------------------------------
BeService::BeService() 
: Service("CliBeService")
{
    init();
    //m_tcpServerSocket = new TcpServerSocket(SERVER_IP, TCP_SERVER_PORT);

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
BeService::~BeService() {
    delete m_tcpServerSocket;
    for (int i=0; i<m_numOfCliWorkers; i++) {
        delete m_cliWorkerArray[i];
    }
    delete m_cliWorkerArray;
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

// ------------------------------------------------
CliWorker* BeService::getWorker() {
    for (int i=0; i<m_numOfCliWorkers; i++) {
        if (m_cliWorkerArray[i]->isIdle()) {
            return m_cliWorkerArray[i];
        }
    }

    return 0;
}

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
    // TcpSocket* newSocket = 0;
    // while (true) {
    //     newSocket = m_tcpServerSocket->accept();
    //     if (newSocket != 0) {
    //         LOG_ERROR(CLI_LOGGER_NAME, "[%s], Error TCP connection\n", __func__);
    //         exit(0);
    //     }

    //     LOG_DBG(CLI_LOGGER_NAME, "[%s], accept new connection\n", __func__);

    //     CliWorker* worker = this->getWorker();
    //     if (worker != 0) {
    //         TcpSocketTask* task = new TcpSocketTask(newSocket);
    //         worker->setTask(task);
    //     } else {
    //         LOG_WARN(CLI_LOGGER_NAME, "[%s], no available worker now, ignore the new connection\n", __func__);
    //         delete newSocket;
    //     }

    //     newSocket = 0;
    // }

    return 0;
}
