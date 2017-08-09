/*
 * KpiTask.cpp
 *
 *  Created on: June 15, 2017
 *      Author: j.zhou
 */

#include "KpiTask.h"
#include "CLogger.h"

using namespace cli;
using namespace cm;

// ------------------------------------
KpiTask::KpiTask(CliLteMacKpiReq kpiReq) {
    m_kpiReq = kpiReq;
}

// ------------------------------------
KpiTask::~KpiTask() {

}

// ------------------------------------
int KpiTask::execute() {
    LOG_DBG(CLI_LOGGER_NAME, "[%s], Entry\n", __func__);
    if (m_kpiReq.writeToFile || m_kpiReq.writeToSocket) {
        
        

    }
}