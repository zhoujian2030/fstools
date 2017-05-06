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
#include <string>


namespace kpi {

    class KpiWorker : public cm::Thread {
    public:
        KpiWorker(std::string workerName, Qmss* qmss);
        virtual ~KpiWorker();

        virtual unsigned long run();

    private:
        void handleMacKpiResponse(unsigned short length);
        void displayCounter(LteCounter* lteCounter);
        char m_recvBuffer[MAC_RECV_MSG_BUFFER_LENGTH];

        Qmss* m_macQmss;
    };
}

#endif
