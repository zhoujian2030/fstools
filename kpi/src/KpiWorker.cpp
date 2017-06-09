/*
 * KpiWorker.h
 *
 *  Created on: May 05, 2017
 *      Author: j.zhou
 */

#include "KpiWorker.h"
#include "CLogger.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace cm;
using namespace kpi;
using namespace net;
using namespace std;

// ----------------------------------------
KpiWorker::KpiWorker(std::string workerName, Qmss* qmss)
: Thread(workerName)
{
    // CMLogger::initConsoleLog();
    m_macQmss = qmss;
    m_writeOption = gWriteOption;
    m_period = gPeriod;

    m_udpSocket = 0;
    m_file = 0;

    if (m_writeOption == 2) {
        m_udpSocket = new UdpSocket();
        m_kpiServerAddress.port = gServerPort;
        Socket::getSockaddrByIpAndPort(&m_kpiServerAddress.addr, gServerIp, gServerPort);
    }

    if (m_writeOption == 1) {
        // char currPath[256];
        // getcwd(currPath, 256);
        // LOG_DBG(KPI_LOGGER_NAME, "[%s], Current path: %s\n", __func__, currPath);
        m_filename.append("/mnt/user2/lte_kpi_");
        time_t timep;   
        struct tm *p;     
        time(&timep);   
        p = localtime(&timep);
        char dateStr[32];
        sprintf(dateStr, "%04d-%02d-%02d_%02d-%02d-%02d", (1900 + p->tm_year), ( 1 + p->tm_mon), p->tm_mday,(p->tm_hour + 12), p->tm_min, p->tm_sec); 
        m_filename.append(dateStr);
        m_filename.append(".txt");

        m_file = new File(m_filename, FILE_CREATE, FILE_WRITE_ONLY);
        LOG_DBG(KPI_LOGGER_NAME, "[%s], Create file: %s\n", __func__, m_filename.c_str());
    }

    m_numKpiCounter = sizeof(LteCounter) / sizeof(UInt32);
    m_prevKpiArray = new UInt32[m_numKpiCounter];
    m_deltaKpiArray = new SInt32[m_numKpiCounter];
    memset((void*)m_prevKpiArray, 0, sizeof(LteCounter));
    memset((void*)m_deltaKpiArray, 0, sizeof(LteCounter));

    m_index = 0;

    m_sendToServerFlag = false;
}

// ----------------------------------------
KpiWorker::~KpiWorker() {
    if (m_writeOption == 2) {
        delete m_udpSocket;
    }

    if (m_writeOption == 1) {
        delete m_file;
    }
}

// ----------------------------------------
unsigned long KpiWorker::run() {
    LOG_DBG(KPI_LOGGER_NAME, "[%s], KpiWorker running\n", __func__);
    
    if ((m_udpSocket != 0) || (m_file != 0)) {
        std::string kpiCounterName;
        kpiCounterName.append("NO.; ");
        // kpiCounterName.append("ActiveUe, ActiveUe delt; ");
        // kpiCounterName.append("RACH, RACH delt; ");
        // kpiCounterName.append("RAR, RAR delt; ");
        // kpiCounterName.append("MSG3, MSG3 delt; ");
        // kpiCounterName.append("ContResl, ContResl delt; ");
        // kpiCounterName.append("CrcValid, CrcValid delt; ");
        // kpiCounterName.append("HarqAck, HarqAck delt; ");
        // kpiCounterName.append("MSG3Exp, MSG3Exp delt; ");
        // kpiCounterName.append("CrcError, CrcError delt; ");
        // kpiCounterName.append("HarqNAck, HarqNAck delt; ");
        // kpiCounterName.append("ContNack, ContNack delt; ");
        // kpiCounterName.append("RRCReq, RRCReq delt; ");
        // kpiCounterName.append("RRCSetup, RRCSetup delt; ");
        // kpiCounterName.append("RRCCompl, RRCCompl delt; ");
        // kpiCounterName.append("IDReq, IDReq delt; ");
        // kpiCounterName.append("IDResp, IDResp delt; ");
        // kpiCounterName.append("AttRej, AttRej delt; ");
        // kpiCounterName.append("TAURej, TAURej delt; ");
        // kpiCounterName.append("RRCRel, RRCRel delt; ");
        // kpiCounterName.append("ReestReq, ReestReq delt; ");
        // kpiCounterName.append("RRCRej, RRCRej delt; ");
        // kpiCounterName.append("ReestRej, ReestRej delt; ");
        // kpiCounterName.append("UEInfReq, UEInfReq delt; ");
        // kpiCounterName.append("UEInfRsp, UEInfRsp delt; ");
        // kpiCounterName.append("ULCCCH, ULCCCH delt; ");
        // kpiCounterName.append("DLCCCH, DLCCCH delt; ");
        // kpiCounterName.append("ULDCCH, ULDCCH delt; ");
        // kpiCounterName.append("DLDCCH, DLDCCH delt; ");
        kpiCounterName.append("ActiveUe; ");
        kpiCounterName.append("RACH; ");
        kpiCounterName.append("RAR; ");
        kpiCounterName.append("MSG3; ");
        kpiCounterName.append("ContResl; ");
        kpiCounterName.append("CrcValid; ");
        kpiCounterName.append("HarqAck; ");
        kpiCounterName.append("MSG3Exp; ");
        kpiCounterName.append("CrcError; ");
        kpiCounterName.append("HarqNAck; ");
        kpiCounterName.append("ContNack; ");
        kpiCounterName.append("RRCReq; ");
        kpiCounterName.append("RRCSetup; ");
        kpiCounterName.append("RRCCompl; ");
        kpiCounterName.append("IDReq; ");
        kpiCounterName.append("IDResp; ");
        kpiCounterName.append("AttRej; ");
        kpiCounterName.append("TAURej; ");
        kpiCounterName.append("RRCRel; ");
        kpiCounterName.append("ReestReq; ");
        kpiCounterName.append("RRCRej; ");
        kpiCounterName.append("ReestRej; ");
        kpiCounterName.append("UEInfReq; ");
        kpiCounterName.append("UEInfRsp; ");
        kpiCounterName.append("RRCRecfgReq; ");
        kpiCounterName.append("RRCRecfgCompl; ");
        kpiCounterName.append("ULCCCH; ");
        kpiCounterName.append("DLCCCH; ");
        kpiCounterName.append("ULDCCH; ");
        kpiCounterName.append("DLDCCH; ");
        kpiCounterName.append("\n");

        if (m_udpSocket) {
            if (-1 == m_udpSocket->send(kpiCounterName.c_str(), kpiCounterName.size() + 1, m_kpiServerAddress)) {
                m_sendToServerFlag = false;
            } else {
                m_sendToServerFlag = true;
            }
        }

        if (m_file) {
            int writeBytes = 0;
            m_file->write(kpiCounterName.c_str(), kpiCounterName.size(), writeBytes);
        }
    }

    UInt32 length = 0;
    while (true) {
        if ((length = m_macQmss->recv(m_recvBuffer)) > 0) {
            handleMacKpiResponse(length);
        } else {
            LOG_DBG(KPI_LOGGER_NAME, "[%s], Recv MAC msg length = %d\n", __func__, length);
            Thread::sleep(m_period);
        }
    }

    LOG_DBG(KPI_LOGGER_NAME, "[%s], KpiWorker exited\n", __func__);
    return 0; 
}

// ----------------------------------------
void KpiWorker::handleMacKpiResponse(UInt32 length) {
    LOG_DBG(KPI_LOGGER_NAME, "[%s], Recv MAC KPI response length = %d\n", __func__, length);

    char kpiChar[1500];
    int totalLen = 0;
    int singleLen = 0;

    UInt32* kpiValArray = (UInt32*)m_recvBuffer;
    UInt32 numKpi = length / sizeof(UInt32);
    if (numKpi == m_numKpiCounter) {
        m_index++;
        singleLen = sprintf(kpiChar + totalLen, "%6d; ", m_index);
        totalLen += singleLen;

        for (int i=0; i<m_numKpiCounter; i++) {
            m_deltaKpiArray[i] = kpiValArray[i] - m_prevKpiArray[i];
            m_prevKpiArray[i] = kpiValArray[i];

            // singleLen = sprintf(kpiChar + totalLen, "%8d, %4d; ", kpiValArray[i], m_deltaKpiArray[i]);
            singleLen = sprintf(kpiChar + totalLen, "%8d; ", kpiValArray[i]);
            totalLen += singleLen;
            if ((totalLen + 30) > sizeof(kpiChar)) {
                break;
            }
        }
        singleLen = sprintf(kpiChar + totalLen, "\n");
        totalLen += singleLen;

        if (m_udpSocket) {
            // send to kpi receiver
            LOG_DBG(KPI_LOGGER_NAME, "[%s], Send KPI data to KPI server = %d\n", __func__, totalLen);
            m_udpSocket->send((const char*)kpiChar, totalLen, m_kpiServerAddress);   
        } 

        if (m_file) {
            int writeBytes = 0;
            m_file->write((const char*)kpiChar, totalLen, writeBytes);
        }
        
        LteCounter* lteCounter = (LteCounter*)m_recvBuffer;
        displayCounter(lteCounter);
    } else {
        LOG_ERROR(KPI_LOGGER_NAME, "[%s], invalid mac kpi response\n", __func__);
    }
}

void KpiWorker::displayCounter(LteCounter* lteCounter) {
    system("clear");

    time_t timep;   
    struct tm *p; 
  
    time(&timep);   
    p = localtime(&timep);

    LteCounter* accumulateCounter = (LteCounter*)m_prevKpiArray;
    LteCounter* deltaCounter = (LteCounter*)m_deltaKpiArray;
    char dispChar[2048];
    int sumLength = 0;
    int varLength = 0;
    // memset((void*)dispChar, 32, 1000);

    printf("Date: %04d-%02d-%02d %02d:%02d:%02d\n", (1900 + p->tm_year), ( 1 + p->tm_mon), p->tm_mday,(p->tm_hour + 12), p->tm_min, p->tm_sec); 
    if (m_udpSocket) {
        printf("KPI Server: %s:%d (Status: %s)\n", gServerIp.c_str(), gServerPort, m_sendToServerFlag ? "Active" : "Inactive");
    }
    if (m_file) {
        printf("File name: %s\n", m_filename.c_str());
    }
    printf("Name            Accumulate  Delta(%ds)\n", m_period/1000);
    printf("--------------------------------------\n");

    varLength = sprintf(dispChar + sumLength, "Active UE       %10d  %8d\n", accumulateCounter->activeUe, deltaCounter->activeUe);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RACH IND        %10d  %8d\n", accumulateCounter->rach, deltaCounter->rach);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "MSG3            %10d  %8d\n", accumulateCounter->msg3, deltaCounter->msg3);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "MSG3 Expired    %10d  %8d\n", accumulateCounter->msg3Expired, deltaCounter->msg3Expired);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "CRC Correct     %10d  %8d\n", accumulateCounter->crcValid, deltaCounter->crcValid);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "CRC Error       %10d  %8d\n", accumulateCounter->crcError, deltaCounter->crcError);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "HARQ ACK        %10d  %8d\n", accumulateCounter->harqAck, deltaCounter->harqAck);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "HARQ NACK       %10d  %8d\n", accumulateCounter->harqNack, deltaCounter->harqNack);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RRC Request     %10d  %8d\n", accumulateCounter->rrcReq, deltaCounter->rrcReq);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RRC Setup       %10d  %8d\n", accumulateCounter->rrcSetup, deltaCounter->rrcSetup);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RRC Reject      %10d  %8d\n", accumulateCounter->rrcConnReject, deltaCounter->rrcConnReject);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RRC Setup Compl %10d  %8d\n", accumulateCounter->rrcSetupComplete, deltaCounter->rrcSetupComplete);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "Identity Req    %10d  %8d\n", accumulateCounter->identityReq, deltaCounter->identityReq);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "Identity Resp   %10d  %8d\n", accumulateCounter->idnetityResp, deltaCounter->idnetityResp);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "TAU Reject      %10d  %8d\n", accumulateCounter->tauReject, deltaCounter->tauReject);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "Attach Reject   %10d  %8d\n", accumulateCounter->attachReject, deltaCounter->attachReject);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RRC Release     %10d  %8d\n", accumulateCounter->rrcRelease, deltaCounter->rrcRelease);
    sumLength += varLength;

    printf("%s", dispChar);
}
