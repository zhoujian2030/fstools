/*
 * KpiWorker.h
 *
 *  Created on: May 05, 2017
 *      Author: j.zhou
 */

#include "KpiWorker.h"
#include "CLogger.h"

using namespace cm;
using namespace kpi;

// ----------------------------------------
KpiWorker::KpiWorker(std::string workerName, Qmss* qmss)
: Thread(workerName)
{
    // CMLogger::initConsoleLog();
    m_macQmss = qmss;
}

// ----------------------------------------
KpiWorker::~KpiWorker() {

}

// ----------------------------------------
unsigned long KpiWorker::run() {
    LOG_DBG(KPI_LOGGER_NAME, "[%s], KpiWorker running\n", __func__);

    int length = 0;
    while (true) {
        if ((length = m_macQmss->recv(m_recvBuffer)) > 0) {
            handleMacKpiResponse(length);
        } else {
            LOG_DBG(KPI_LOGGER_NAME, "[%s], Recv MAC msg length = %d\n", __func__, length);
            Thread::sleep(1000);
        }
    }

    LOG_DBG(KPI_LOGGER_NAME, "[%s], KpiWorker exited\n", __func__);
    return 0; 
}

// ----------------------------------------
void KpiWorker::handleMacKpiResponse(unsigned short length) {
    LOG_DBG(KPI_LOGGER_NAME, "[%s], Recv MAC KPI response length = %d\n", __func__, length);
    LOG_BUFFER(m_recvBuffer, length);

    LteCounter* lteCounter = (LteCounter*)m_recvBuffer;
    displayCounter(lteCounter);
}

void KpiWorker::displayCounter(LteCounter* lteCounter) {
    char rrcCounterName[] = "|RRCReq   |RRCSetup |RRCCompl |IDReq    |IDResp   |AttRej   |TAURej   |RRCRel   |RRCRej   |ReestReq |ReestRej |UEInfReq |UEInfRsp |";
    char l2CounterName[] = "|ULCCCH   |DLCCCH   |ULDCCH   |DLDCCH   |";
    char macCounterName[] = "|ActiveUe |RACH     |RAR      |MSG3     |ContResl |MSG3Exp  |CrcValid |CrcError |";
    char rrcCounterVal[512];
    char l2CounterVal[128];
    char macCounterVal[512];
    
    sprintf(rrcCounterVal, "%s\n|%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |", 
        "+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+",
        lteCounter->rrcReq, lteCounter->rrcSetup, lteCounter->rrcSetupComplete, lteCounter->identityReq,
        lteCounter->idnetityResp, lteCounter->attachReject, lteCounter->tauReject, lteCounter->rrcRelease,
        lteCounter->rrcConnReject, lteCounter->rrcReestablishmentReq, lteCounter->rrcReestabReject,
        lteCounter->ueInfoReq, lteCounter->ueInfoRsp);
    
    sprintf(l2CounterVal, "%s\n|%8d |%8d |%8d |%8d |", 
        "+---------+---------+---------+---------+",
        lteCounter->ulCCCH, lteCounter->dlCCCH, lteCounter->ulDCCH, lteCounter->dlDCCH);

    sprintf(macCounterVal, "%s\n|%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |", 
        "+---------+---------+---------+---------+---------+---------+---------+---------+",
        lteCounter->activeUe, lteCounter->rach, lteCounter->rar, lteCounter->msg3,
        lteCounter->contResol, lteCounter->msg3Expired, lteCounter->crcValid, lteCounter->crcError);    

    printf("%s\n%s\n\n%s\n%s\n\n", rrcCounterName, rrcCounterVal, l2CounterName, l2CounterVal);
    printf("%s\n%s\n\n", macCounterName, macCounterVal);
}
