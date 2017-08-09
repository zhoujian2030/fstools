/*
 * BeUdpService.h
 *
 *  Created on: July 10, 2017
 *      Author: j.zhou
 */

#ifndef BE_TCP_SERVICE_H
#define BE_TCP_SERVICE_H

#include "Service.h"
#include "SelectSocketSet.h"
#include "EventIndicator.h"
#include "MacInterface.h"
#include "CliWorker.h"
#include "Task.h"
#include "UdpSocket.h"

namespace cli {
    class BeUdpService : public cm::Service {
    public: 
        virtual ~BeUdpService();     

        static BeUdpService* getInstance();

        void addBeTask(cm::Task* theTask, int userData);

    private:
        BeUdpService();
        virtual unsigned long run();   

        static BeUdpService* m_theInstance;

        CliWorker* getBeWorker();

        enum {
            MAX_NUM_CLI_WORKERS = 5
        };

        CliWorker* getWorker();

        net::UdpSocket* m_udpSocket;
        char m_recvBuffer[IP_RECV_MSG_BUFFER_LENGTH];

        CliWorker** m_cliWorkerArray;
        int m_numOfCliWorkers;
    };
}

#endif
