/*
 * KpiWorker.h
 *
 *  Created on: May 05, 2017
 *      Author: j.zhou
 */

#ifndef KPI_WORKER_H
#define KPI_WORKER_H

#include "Thread.h"
#ifndef KPI_L3
#include "Qmss.h"
#endif
#include "MacInterface.h"
#include "UdpSocket.h"
#include "File.h"
#include <string>


namespace kpi {

    class KpiWorker : public cm::Thread {
    public:
#ifdef KPI_L3
        KpiWorker(std::string workerName, net::UdpSocket* udpServerSocket);
#else
        KpiWorker(std::string workerName, Qmss* qmss);
#endif
        virtual ~KpiWorker();

        virtual unsigned long run();
        void displayCounter(void* counter);

    private:
        void handleMacKpiResponse(UInt32 length);
        char m_recvBuffer[MAC_RECV_MSG_BUFFER_LENGTH];

        UInt32 m_targetVersion;

        UInt32* m_prevKpiArray;
        SInt32* m_deltaKpiArray;
        UInt32 m_numKpiCounter;

        UInt32 m_index;

#ifdef KPI_L3
        net::UdpSocket* m_udpServerSocket;
        net::Socket::InetAddressPort m_rrcClientAddress;
#else 
        Qmss* m_macQmss;
#endif
        UInt32 m_period;
        int m_writeOption;

        net::UdpSocket* m_udpSocket;
        net::Socket::InetAddressPort m_kpiServerAddress;

        cm::File* m_file;
        std::string m_filename;

        bool m_sendToServerFlag;
    };
}

#endif
