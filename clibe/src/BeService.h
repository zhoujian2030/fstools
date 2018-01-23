/*
 * BeService.h
 *
 *  Created on: June 14, 2017
 *      Author: j.zhou
 */

#ifndef BE_SERVICE_H
#define BE_SERVICE_H

#include "Service.h"
#include "CliTcpServerSocket.h"
#include "SelectSocketSet.h"
#include "EventIndicator.h"
#include "CliCommon.h"
#include "CliWorker.h"
#include "Task.h"
#include "UdpSocket.h"

#define IP_RECV_MSG_BUFFER_LENGTH 1500

namespace cli {
    class BeService : public cm::Service {
    public: 
        virtual ~BeService();     

        static BeService* getInstance();

        void addBeTask(cm::Task* theTask, int userData);

    private:
        BeService();
        virtual unsigned long run();   

        static BeService* m_theInstance;

        CliWorker* getBeWorker();

        enum {
            MAX_NUM_CLI_WORKERS = 5,
            MAC_NUM_CLI_BE_WORKERS = 3
        };

#ifndef USING_UDP
        CliWorker* getWorker();

        CliTcpServerSocket* m_tcpServerSocket;
        net::Socket::InetAddressPort m_beAddress;        
        
        CliWorker** m_cliWorkerArray;
        int m_numOfCliWorkers;
#else 
        net::UdpSocket* m_udpSocket;
        char m_recvBuffer[IP_RECV_MSG_BUFFER_LENGTH];
#endif

        CliWorker** m_cliBeWorkerArray;
        int m_numOfCliBeWorkers;
    };
}

#endif
