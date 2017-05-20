/*
 * KpiWorker.h
 *
 *  Created on: May 05, 2017
 *      Author: j.zhou
 */

#ifndef KPI_WORKER_H
#define KPI_WORKER_H

#include "Thread.h"
#include "Qmss.h"
#include "MacInterface.h"
#include "UdpSocket.h"
#include "File.h"
#include <string>


namespace kpi {

    class KpiWorker : public cm::Thread {
    public:
        KpiWorker(std::string workerName, Qmss* qmss);
        virtual ~KpiWorker();

        virtual unsigned long run();

    private:
        void handleMacKpiResponse(UInt32 length);
        void displayCounter(LteCounter* lteCounter);
        char m_recvBuffer[MAC_RECV_MSG_BUFFER_LENGTH];
        UInt32* m_prevKpiArray;
        SInt32* m_deltaKpiArray;
        UInt32 m_numKpiCounter;

        UInt32 m_index;

        Qmss* m_macQmss;
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
