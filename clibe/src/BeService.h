/*
 * BeService.h
 *
 *  Created on: June 14, 2017
 *      Author: j.zhou
 */

#ifndef BE_SERVICE_H
#define BE_SERVICE_H

#include "Service.h"
#include "TcpServerSocket.h"
#include "SelectSocketSet.h"
#include "EventIndicator.h"
#include "MacInterface.h"
#include "CliWorker.h"
#include "Task.h"

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

        CliWorker* getWorker();
        CliWorker* getBeWorker();

        enum {
            MAX_NUM_CLI_WORKERS = 5,
            MAC_NUM_CLI_BE_WORKERS = 3
        };

        net::TcpServerSocket* m_tcpServerSocket;
        net::Socket::InetAddressPort m_beAddress;

        CliWorker** m_cliWorkerArray;
        int m_numOfCliWorkers;

        CliWorker** m_cliBeWorkerArray;
        int m_numOfCliBeWorkers;
    };
}

#endif
