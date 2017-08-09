/*
 * BeTcpService.h
 *
 *  Created on: July 10, 2017
 *      Author: j.zhou
 */

#ifndef BE_TCP_SERVICE_H
#define BE_TCP_SERVICE_H

#include "Service.h"
#include "CliTcpServerSocket.h"
#include "SelectSocketSet.h"
#include "EventIndicator.h"
#include "MacInterface.h"
#include "CliWorker.h"
#include "Task.h"

namespace cli {
    class BeTcpService : public cm::Service {
    public: 
        virtual ~BeTcpService();     

        static BeTcpService* getInstance();

        void addBeTask(cm::Task* theTask, int userData);

    private:
        BeTcpService();
        virtual unsigned long run();   

        static BeTcpService* m_theInstance;

        CliWorker* getBeWorker();

        enum {
            MAX_NUM_CLI_WORKERS = 5,
            MAC_NUM_CLI_BE_WORKERS = 3
        };

        CliWorker* getWorker();

        CliTcpServerSocket* m_tcpServerSocket;
        net::Socket::InetAddressPort m_beAddress;        
        
        CliWorker** m_cliWorkerArray;
        int m_numOfCliWorkers;

        CliWorker** m_cliBeWorkerArray;
        int m_numOfCliBeWorkers;
    };
}

#endif
