/*
 * KpiTask.h
 *
 *  Created on: June 15, 2017
 *      Author: j.zhou
 */

#ifndef KPI_TASK_H
#define KPI_TASK_H

#include "Task.h"
#include "DataBuffer.h"
#include "CliCommon.h"

namespace cli {

    class KpiTask : public cm::Task {
    public:
        KpiTask(CliLteMacKpiReq kpiReq);
        virtual ~KpiTask();

        virtual int execute();

    private:
        CliLteMacKpiReq m_kpiReq;
        cm::DataBuffer* m_dataBuffer;
    };

}

#endif
