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
#ifdef KPI_L3
KpiWorker::KpiWorker(std::string workerName, UdpSocket* udpServerSocket)
: Thread(workerName)
{
    m_udpServerSocket = udpServerSocket;//new UdpSocket(UDP_SERVER_IP_RECV_L3_KPI, UDP_SERVER_PORT_RECV_L3);
#else
KpiWorker::KpiWorker(std::string workerName, Qmss* qmss)
: Thread(workerName)
{
    m_macQmss = qmss;
#endif
    m_writeOption = gWriteOption;
    m_period = gPeriod;

    m_udpSocket = 0;
    m_file = 0;

    if (m_writeOption == 2) {
        m_udpSocket = new UdpSocket();
        m_kpiServerAddress.port = gServerPort;
        Socket::getSockaddrByIpAndPort(&m_kpiServerAddress.addr, gServerIp, gServerPort);
    }

    if (m_writeOption == 1 || m_writeOption == 3) {
        // char currPath[256];
        // getcwd(currPath, 256);
        // LOG_DBG(KPI_LOGGER_NAME, "[%s], Current path: %s\n", __func__, currPath);
#ifndef GSM
        m_filename.append("/mnt/user2/lte_kpi_");
#else
        m_filename.append("/flash/appsys/common/gsm_kpi_");     
#endif   
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

#ifndef GSM 
    m_numKpiCounter = sizeof(LteCounter) / sizeof(UInt32);
    m_prevKpiArray = new UInt32[m_numKpiCounter];
    m_deltaKpiArray = new SInt32[m_numKpiCounter];
    memset((void*)m_prevKpiArray, 0, sizeof(LteCounter));
    memset((void*)m_deltaKpiArray, 0, sizeof(LteCounter));
#else 
    m_numKpiCounter = sizeof(GSMCounter) / sizeof(UInt32);
    m_prevKpiArray = new UInt32[m_numKpiCounter];
    m_deltaKpiArray = new SInt32[m_numKpiCounter];
    memset((void*)m_prevKpiArray, 0, sizeof(GSMCounter));
    memset((void*)m_deltaKpiArray, 0, sizeof(GSMCounter));
#endif

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

#ifdef KPI_L3
    delete m_udpServerSocket;
#endif
}

// ----------------------------------------
unsigned long KpiWorker::run() {
    LOG_DBG(KPI_LOGGER_NAME, "[%s], KpiWorker running\n", __func__);
    
    if ((m_udpSocket != 0) || (m_file != 0)) {
        std::string kpiCounterName;
        kpiCounterName.append("NO.; ");
#ifndef GSM
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
        kpiCounterName.append("RRCRecfg; ");
        kpiCounterName.append("RRCRecfgCompl; ");
        kpiCounterName.append("ULCCCH; ");
        kpiCounterName.append("DLCCCH; ");
        kpiCounterName.append("ULDCCH; ");
        kpiCounterName.append("DLDCCH; ");
#else 
        kpiCounterName.append("ChannReq; ");
        kpiCounterName.append("ImmediaAssign; ");
        kpiCounterName.append("SABM; ");
        kpiCounterName.append("RRSetupUA; ");
        kpiCounterName.append("imsiIDReq; ");
        kpiCounterName.append("imsiIDResp; ");
        kpiCounterName.append("imeiIDReq; ");
        kpiCounterName.append("imeiIDResp; ");
        kpiCounterName.append("LACReject; ");
        kpiCounterName.append("ChannRel; ");
        kpiCounterName.append("DISC; ");
        kpiCounterName.append("RRChannUA; ");      
#endif
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
#ifdef KPI_L3
        length = m_udpServerSocket->receive(m_recvBuffer, MAC_RECV_MSG_BUFFER_LENGTH, m_rrcClientAddress);
        if (length > 0) {
            handleMacKpiResponse(length);
        } else {
            LOG_ERROR(KPI_LOGGER_NAME, "[%s], receive UDP data error\n", __func__);
        }
#else 
        if ((length = m_macQmss->recv(m_recvBuffer)) > 0) {
            handleMacKpiResponse(length);
        } else {
            LOG_DBG(KPI_LOGGER_NAME, "[%s], Recv MAC msg length = %d\n", __func__, length);
            Thread::sleep(m_period);
        }
#endif
    }

    LOG_DBG(KPI_LOGGER_NAME, "[%s], KpiWorker exited\n", __func__);
    return 0; 
}

// ----------------------------------------
void KpiWorker::handleMacKpiResponse(UInt32 length) {
    LOG_DBG(KPI_LOGGER_NAME, "[%s], Recv KPI response length = %d\n", __func__, length);

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
        
        if (gWriteOption < 3) {
            //LteCounter* lteCounter = (LteCounter*)m_recvBuffer;
            displayCounter(m_recvBuffer);
        }
    } else {
        LOG_ERROR(KPI_LOGGER_NAME, "[%s], invalid kpi response\n", __func__);
    }
}

void KpiWorker::displayCounter(void* counter) {
    system("clear");

    time_t timep;   
    struct tm *p; 
  
    time(&timep);   
    p = localtime(&timep);

#ifndef GSM    
    LteCounter* accumulateCounter = (LteCounter*)m_prevKpiArray;
    LteCounter* deltaCounter = (LteCounter*)m_deltaKpiArray;
#else 
    GSMCounter* accumulateCounter = (GSMCounter*)m_prevKpiArray;
    GSMCounter* deltaCounter = (GSMCounter*)m_deltaKpiArray;
#endif
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

#ifndef GSM
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
    varLength = sprintf(dispChar + sumLength, "Identity Resp   %10d  %8d\n", accumulateCounter->identityResp, deltaCounter->identityResp);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "TAU Reject      %10d  %8d\n", accumulateCounter->tauReject, deltaCounter->tauReject);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "Attach Reject   %10d  %8d\n", accumulateCounter->attachReject, deltaCounter->attachReject);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RRC Release     %10d  %8d\n", accumulateCounter->rrcRelease, deltaCounter->rrcRelease);
    sumLength += varLength;

    printf("%s\n", dispChar);

    float setupDivReq = 0;
    float msg3DivRach = 0;
    float setupComplDivSetup = 0;
    float idRspDivSetupCompl = 0;
    float idRspDivIdReq = 0;

    if (accumulateCounter->rrcReq != 0) {
        setupDivReq = (accumulateCounter->rrcSetup * 100.0) / accumulateCounter->rrcReq;
    }
    if (accumulateCounter->rach != 0) {
        msg3DivRach = (accumulateCounter->msg3 * 100.0) / accumulateCounter->rach;
    }  
    if (accumulateCounter->rrcSetup != 0) {
        setupComplDivSetup = (accumulateCounter->rrcSetupComplete * 100.0) / accumulateCounter->rrcSetup;
    }  
    if (accumulateCounter->rrcSetupComplete != 0) {
        idRspDivSetupCompl = (accumulateCounter->identityResp * 100.0) / accumulateCounter->rrcSetupComplete;
    }  
    if (accumulateCounter->identityReq != 0) {
        idRspDivIdReq = (accumulateCounter->identityResp * 100.0) / accumulateCounter->identityReq;
    }  

    printf("RrcSetup/RrcReq:           %f\n", setupDivReq);
    printf("MSG3/RACH:                 %f\n", msg3DivRach);
    printf("RrcSetupCompl/RrcSetup:    %f\n", setupComplDivSetup);
    printf("IdentityRsp/RrcSetupCompl: %f\n", idRspDivSetupCompl);
    printf("IdentityRsp/IdentityReq:   %f\n", idRspDivIdReq);
#else 
    varLength = sprintf(dispChar + sumLength, "Channel Req     %10d  %8d\n", accumulateCounter->channelReq, deltaCounter->channelReq);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "Immedia Assign  %10d  %8d\n", accumulateCounter->immediateAssign, deltaCounter->immediateAssign);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "SABM            %10d  %8d\n", accumulateCounter->SABM, deltaCounter->SABM);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RR Setup UA     %10d  %8d\n", accumulateCounter->RRsetupUA, deltaCounter->RRsetupUA);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "IMSI ID Req     %10d  %8d\n", accumulateCounter->imsiIDReq, deltaCounter->imsiIDReq);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "IMSI ID Resp    %10d  %8d\n", accumulateCounter->imsiIDResp, deltaCounter->imsiIDResp);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "IMEI ID Req     %10d  %8d\n", accumulateCounter->imeiIDReq, deltaCounter->imeiIDReq);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "IMEI ID Resp    %10d  %8d\n", accumulateCounter->imeiIDResp, deltaCounter->imeiIDResp);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "LAC Reject      %10d  %8d\n", accumulateCounter->LacReject, deltaCounter->LacReject);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "Channel Rel     %10d  %8d\n", accumulateCounter->channelRelease, deltaCounter->channelRelease);
    sumLength += varLength;   
    varLength = sprintf(dispChar + sumLength, "DISC            %10d  %8d\n", accumulateCounter->DISC, deltaCounter->DISC);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RR Channel UA   %10d  %8d\n", accumulateCounter->RRchannelUA, deltaCounter->RRchannelUA);
    sumLength += varLength;   

    printf("%s\n", dispChar);

    float immediaADivChannReq = 0;
    float sabmDivImmediaA = 0;
    float rrSetupUADivSABM = 0;
    float imsiIdRspDivImsiIdReq = 0;
    float imeiIdRspDivImeiIdReq = 0;
    float channRelDivDisc = 0;
    float RRChannUADivDisc = 0;

    if (accumulateCounter->channelReq != 0) {
        immediaADivChannReq = (accumulateCounter->immediateAssign * 100.0) / accumulateCounter->channelReq;
    }
    if (accumulateCounter->immediateAssign != 0) {
        sabmDivImmediaA = (accumulateCounter->SABM * 100.0) / accumulateCounter->immediateAssign;
    }  
    if (accumulateCounter->SABM != 0) {
        rrSetupUADivSABM = (accumulateCounter->RRsetupUA * 100.0) / accumulateCounter->SABM;
    }  
    if (accumulateCounter->imsiIDReq != 0) {
        imsiIdRspDivImsiIdReq = (accumulateCounter->imsiIDResp * 100.0) / accumulateCounter->imsiIDReq;
    }  
    if (accumulateCounter->imeiIDReq != 0) {
        imeiIdRspDivImeiIdReq = (accumulateCounter->imeiIDResp * 100.0) / accumulateCounter->imeiIDReq;
    }  
    if (accumulateCounter->DISC != 0) {
        channRelDivDisc = (accumulateCounter->channelRelease * 100.0) / accumulateCounter->DISC;
        RRChannUADivDisc = (accumulateCounter->RRchannelUA * 100.0) / accumulateCounter->DISC;
    }  

    printf("ImmediaAss/ChannelReq:     %f\n", immediaADivChannReq);
    printf("SABM/ImmediaAss:           %f\n", sabmDivImmediaA);
    printf("RRSetup/SABM:              %f\n", rrSetupUADivSABM);
    printf("IMSIIdRsp/IMSIIDReq:       %f\n", imsiIdRspDivImsiIdReq);
    printf("IMEIIDRsp/IMEIIDRsp:       %f\n", imeiIdRspDivImeiIdReq); 
    printf("ChannelRel/DISC:           %f\n", channRelDivDisc);
    printf("RRChannelUA/DISC:          %f\n", RRChannUADivDisc);  
#endif
}
