/*
 * KpiWorker.h
 *
 *  Created on: July 10, 2017
 *      Author: j.zhou
 */

#ifndef KPI_WORKER_H
#define KPI_WORKER_H

#include "Thread.h"
#include "EventIndicator.h"
#include "MutexLock.h"
#include "CliCommon.h"

namespace cli {

    class KpiWorker : public cm::Thread {
    public:
        KpiWorker();
        virtual ~KpiWorker();

        virtual unsigned long run();

    private:
        CliLteMacKpiReq m_kpiReq;
    };

}

#endif
