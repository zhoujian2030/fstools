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
#ifdef USE_UDP
using namespace net;
#endif
using namespace std;

#define VERSION         1003
#define VERSION_LEN     4

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

    m_file = 0;

    m_targetVersion = VERSION;

#ifdef USE_UDP
    m_udpSocket = 0;
    if (m_writeOption == 2) {
        m_udpSocket = new UdpSocket();
        m_kpiServerAddress.port = gServerPort;
        Socket::getSockaddrByIpAndPort(&m_kpiServerAddress.addr, gServerIp, gServerPort);
    }
    m_sendToServerFlag = false;
#endif

    if (m_writeOption == 1 || m_writeOption == 3 || m_writeOption == 4) {
        // char currPath[256];
        // getcwd(currPath, 256);
        // LOG_DBG(KPI_LOGGER_NAME, "[%s], Current path: %s\n", __func__, currPath);
#ifndef GSM
        m_filename = gKpiDirectory;
        m_filename.append("/lte_kpi_");
#else
        m_filename.append("/flash/appsys/common/gsm_kpi_");     
#endif   
        time_t timep;   
        struct tm *p;     
        time(&timep);   
        p = localtime(&timep);
        char dateStr[32];
        sprintf(dateStr, "%04d-%02d-%02d_%02d-%02d-%02d", (1900 + p->tm_year), ( 1 + p->tm_mon), p->tm_mday, (p->tm_hour), p->tm_min, p->tm_sec); 
        m_filename.append(dateStr);
        m_filename.append(".txt");

        m_file = new File(m_filename, FILE_CREATE, FILE_WRITE_ONLY);
        LOG_DBG(KPI_LOGGER_NAME, "[%s], Create file: %s\n", __func__, m_filename.c_str());
    } 
    
    if (m_writeOption == 4) {
        m_resultFilename = "/OAM/LTE_NODE/web/web_page/kpi.txt";
        m_resultFile = new File(m_resultFilename, FILE_CREATE, FILE_WRITE_ONLY);
        LOG_DBG(KPI_LOGGER_NAME, "[%s], Create file: %s\n", __func__, m_resultFilename.c_str());
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
}

// ----------------------------------------
KpiWorker::~KpiWorker() {
#ifdef USE_UDP
    if (m_writeOption == 2) {
        delete m_udpSocket;
    }
#endif
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
#ifdef USE_UDP
    if ((m_udpSocket != 0) || (m_file != 0)) {
#else 
    if (m_file != 0) {
#endif
        std::string kpiCounterName;
        kpiCounterName.append("NO.; ");
#ifndef GSM
        kpiCounterName.append("ActiveUe; ");
        kpiCounterName.append("RACH; ");
        kpiCounterName.append("MSG2; ");
        kpiCounterName.append("MSG3; ");
        kpiCounterName.append("ContResl; ");
        kpiCounterName.append("CrcValid; ");
        kpiCounterName.append("HarqAckRecv; ");
        kpiCounterName.append("MSG3Exp; ");
        kpiCounterName.append("CrcError; ");
        kpiCounterName.append("HarqNAckRecv; ");
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
        kpiCounterName.append("RRCReestabCompl; ");
        kpiCounterName.append("RRCReestab; ");
        kpiCounterName.append("ULDCCH; ");
        kpiCounterName.append("DLDCCH; ");
        kpiCounterName.append("HarqDTX; ");
        kpiCounterName.append("HarqOther; ");
        kpiCounterName.append("MaxActiveMacUe; ");
        kpiCounterName.append("ActiveRlcUe; ");
        kpiCounterName.append("MaxActiveRlcUe; ");
        kpiCounterName.append("ActivePdcpUe; ");
        kpiCounterName.append("MaxActivePdcpUe; ");
        kpiCounterName.append("HarqAckSent; ");
        kpiCounterName.append("HarqNackSent; ");
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

#ifdef USE_UDP
        if (m_udpSocket) {
            if (-1 == m_udpSocket->send(kpiCounterName.c_str(), kpiCounterName.size() + 1, m_kpiServerAddress)) {
                m_sendToServerFlag = false;
            } else {
                m_sendToServerFlag = true;
            }
        }
#endif 

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
    m_targetVersion = *kpiValArray;
    if (m_targetVersion < VERSION) {
        system("clear");
        if (m_targetVersion < 1000) {
            printf("ERROR: version[%d] not upported\n", VERSION);
        } else {
            printf("ERROR: version[%d] mismatch, please downgrade to version[%d]\n", VERSION, m_targetVersion);
        }        
        exit(0);
    }
    kpiValArray++;

    UInt32 numKpi = (length - VERSION_LEN) / sizeof(UInt32);
    if (numKpi >= m_numKpiCounter) {
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

#ifdef USE_UDP
        if (m_udpSocket) {
            // send to kpi receiver
            LOG_DBG(KPI_LOGGER_NAME, "[%s], Send KPI data to KPI server = %d\n", __func__, totalLen);
            m_udpSocket->send((const char*)kpiChar, totalLen, m_kpiServerAddress);   
        } 
#endif 

        if (m_file) {
            int writeBytes = 0;
            m_file->write((const char*)kpiChar, totalLen, writeBytes);
        }
        
        if ((gWriteOption < 3) || (gWriteOption == 4)) {
            //LteCounter* lteCounter = (LteCounter*)m_recvBuffer;
            displayCounter(m_recvBuffer);
        }
    } else {
        LOG_ERROR(KPI_LOGGER_NAME, "[%s], invalid kpi response\n", __func__);
#if 0
        Qmss* qmss = new Qmss(L1_SEND_CMAC_REPLY, Qmss::QID_CLI_RECV_FROM_L2);
        
        // For test
        S_L3MacMsgHead* msg = (S_L3MacMsgHead*)m_recvBuffer;
        LOG_DBG(KPI_LOGGER_NAME, "[%s], mNum = %u\n", __func__, msg->mNum);
        LOG_DBG(KPI_LOGGER_NAME, "[%s], tLen = %d\n", __func__, msg->tLen);
        LOG_DBG(KPI_LOGGER_NAME, "[%s], sno = %d\n", __func__, msg->sno);
        LOG_DBG(KPI_LOGGER_NAME, "[%s], attr = %d\n", __func__, msg->attr);
        LOG_DBG(KPI_LOGGER_NAME, "[%s], common = %d\n", __func__, msg->common);
        LOG_DBG(KPI_LOGGER_NAME, "[%s], opc = %d\n", __func__, msg->opc);

        msg->mNum = 0xDDCCBBAA;
        msg->sno = 2;
        // msg->opc = 6;
        msg->tLen = sizeof(S_L3MacMsgHead) + sizeof(S_L3MacCfgRsp);
        S_L3MacCfgRsp* pCfgRsp = (S_L3MacCfgRsp*)(m_recvBuffer+sizeof(S_L3MacMsgHead));
        pCfgRsp->errorCode = 0;
        LOG_DBG(KPI_LOGGER_NAME, "[%s], msg = %p, pCfgRsp = %p\n", __func__, msg, pCfgRsp);
        LOG_BUFFER(m_recvBuffer, msg->tLen);

        Thread::sleep(10000);        
        qmss->send(m_recvBuffer, msg->tLen);
#endif
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
    char dispChar[4096];
    int sumLength = 0;
    int varLength = 0;
    // memset((void*)dispChar, 32, 1000);

    varLength = sprintf(dispChar + sumLength, "Version: %d\n", VERSION);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "-------------\n");
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "Date: %04d-%02d-%02d %02d:%02d:%02d\n", (1900 + p->tm_year), ( 1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    sumLength += varLength;
#ifdef USE_UDP
    if (m_udpSocket) {
        varLength = sprintf(dispChar + sumLength, "KPI Server: %s:%d (Status: %s)\n", gServerIp.c_str(), gServerPort, m_sendToServerFlag ? "Active" : "Inactive");
        sumLength += varLength;
    }
#endif
    if (m_file) {
        varLength = sprintf(dispChar + sumLength, "File name: %s\n", m_filename.c_str());
        sumLength += varLength;
    }    
    varLength = sprintf(dispChar + sumLength, "Name            Accumulate  Delta(%ds)\n", m_period/1000);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "--------------------------------------\n");
    sumLength += varLength;

//     printf("Version: %d\n", VERSION);
//     printf("-------------\n");
//     printf("Date: %04d-%02d-%02d %02d:%02d:%02d\n", (1900 + p->tm_year), ( 1 + p->tm_mon), p->tm_mday,(p->tm_hour + 12), p->tm_min, p->tm_sec); 
// #ifdef USE_UDP
//     if (m_udpSocket) {
//         printf("KPI Server: %s:%d (Status: %s)\n", gServerIp.c_str(), gServerPort, m_sendToServerFlag ? "Active" : "Inactive");
//     }
// #endif
//     if (m_file) {
//         printf("File name: %s\n", m_filename.c_str());
//     }
//     printf("Name            Accumulate  Delta(%ds)\n", m_period/1000);
//     printf("--------------------------------------\n");

#ifndef GSM
    // varLength = sprintf(dispChar + sumLength, "Active UE       %10d  %8d\n", accumulateCounter->activeUe, deltaCounter->activeUe);
    // sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RACH IND        %10d  %8d\n", accumulateCounter->rach, deltaCounter->rach);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "MSG2            %10d  %8d\n", accumulateCounter->rar, deltaCounter->rar);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "MSG3            %10d  %8d\n", accumulateCounter->msg3, deltaCounter->msg3);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "MSG3 Expired    %10d  %8d\n", accumulateCounter->msg3Expired, deltaCounter->msg3Expired);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "Conten Resol    %10d  %8d\n", accumulateCounter->contResol, deltaCounter->contResol);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "CRC Correct     %10d  %8d\n", accumulateCounter->crcValid, deltaCounter->crcValid);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "CRC Error       %10d  %8d\n", accumulateCounter->crcError, deltaCounter->crcError);
    sumLength += varLength;    
    varLength = sprintf(dispChar + sumLength, "HARQ ACK Sent   %10d  %8d\n", accumulateCounter->harqAckSent, deltaCounter->harqAckSent);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "HARQ NACK Sent  %10d  %8d\n", accumulateCounter->harqNackSent, deltaCounter->harqNackSent);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "HARQ ACK Rcvd   %10d  %8d\n", accumulateCounter->harqAckRecvd, deltaCounter->harqAckRecvd);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "HARQ NACK Rcvd  %10d  %8d\n", accumulateCounter->harqNackRecvd, deltaCounter->harqNackRecvd);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "HARQ DTX        %10d  %8d\n", accumulateCounter->harqDtx, deltaCounter->harqDtx);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "HARQ Other      %10d  %8d\n", accumulateCounter->harqOther, deltaCounter->harqOther);
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

    // printf("%s\n", dispChar);

    float setupDivReq = 0;
    float msg3DivRach = 0;
    float setupComplDivSetup = 0;
    float setupComplDivContResol = 0;
    // float harqAckDivHarqReq = 0;
    float harqAckDivHarqInd = 0;
    float idRspDivIdReq = 0;

    if (accumulateCounter->rach != 0) {
        msg3DivRach = (accumulateCounter->msg3 * 100.0) / accumulateCounter->rach;
    }      
    // if (accumulateCounter->harqAckSent != 0) {
    //     harqAckDivHarqReq = (accumulateCounter->harqAckSent * 100.0) / (accumulateCounter->harqAckSent + accumulateCounter->harqNackSent);
    // }  
    if (accumulateCounter->harqAckRecvd != 0) {
        harqAckDivHarqInd = (accumulateCounter->harqAckRecvd * 100.0) / (accumulateCounter->harqAckRecvd + accumulateCounter->harqNackRecvd +  accumulateCounter->harqDtx);
    }  
    if (accumulateCounter->rrcReq != 0) {
        setupDivReq = (accumulateCounter->rrcSetup * 100.0) / accumulateCounter->rrcReq;
    }
    if (accumulateCounter->rrcSetup != 0) {
        setupComplDivSetup = (accumulateCounter->rrcSetupComplete * 100.0) / accumulateCounter->rrcSetup;
    }
    if (accumulateCounter->contResol != 0) {
        setupComplDivContResol = (accumulateCounter->rrcSetupComplete * 100.0) / accumulateCounter->contResol;
    }
    if (accumulateCounter->identityReq != 0) {
        idRspDivIdReq = (accumulateCounter->identityResp * 100.0) / accumulateCounter->identityReq;
    }  

    varLength = sprintf(dispChar + sumLength, "\n");
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "MSG3/RACH:                 %f\n", msg3DivRach);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "HarqAck/HarqInd:           %f\n", harqAckDivHarqInd);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RrcSetup/RrcReq:           %f\n", setupDivReq);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RrcSetupCompl/RrcSetup:    %f\n", setupComplDivSetup);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "RrcSetupCompl/ContResol:   %f\n", setupComplDivContResol);
    sumLength += varLength;
    varLength = sprintf(dispChar + sumLength, "IdentityRsp/IdentityReq:   %f\n", idRspDivIdReq);
    sumLength += varLength;

    // printf("MSG3/RACH:                 %f\n", msg3DivRach);
    // // printf("HarqAck/HarqReq:           %f\n", harqAckDivHarqReq);
    // printf("HarqAck/HarqInd:           %f\n", harqAckDivHarqInd);
    // printf("RrcSetup/RrcReq:           %f\n", setupDivReq);
    // printf("RrcSetupCompl/RrcSetup:    %f\n", setupComplDivSetup);
    // printf("RrcSetupCompl/ContResol:   %f\n", setupComplDivContResol);
    // printf("IdentityRsp/IdentityReq:   %f\n", idRspDivIdReq);    
       
    if (gShowAll || (m_writeOption == 4)) {
        static UInt8 count = 0;
        static UInt32 maxIdResp = 0;
        if (count > 2) {
            if (maxIdResp < deltaCounter->identityResp) {
                maxIdResp = deltaCounter->identityResp;
            }
            varLength = sprintf(dispChar + sumLength, "\n");
            sumLength += varLength;
            varLength = sprintf(dispChar + sumLength, "Max Collected IMSI (%ds):    %d\n", gPeriod/1000, maxIdResp);
            sumLength += varLength;
            // printf("\n");
            // printf("Max Collected IMSI (%ds):    %d\n", gPeriod/1000, maxIdResp);
        } else {
            count++;
        }

        varLength = sprintf(dispChar + sumLength, "\n");
        sumLength += varLength;
        varLength = sprintf(dispChar + sumLength, "ActiveMacUE/MaxActiveMacUE:     %03d/%03d\n", accumulateCounter->activeUe, accumulateCounter->maxActiveMacUe);
        sumLength += varLength;
        varLength = sprintf(dispChar + sumLength, "ActiveRlcUE/MaxActiveRlcUE:     %03d/%03d\n", accumulateCounter->activeRlcUe, accumulateCounter->maxActiveRlcUe);
        sumLength += varLength;
        varLength = sprintf(dispChar + sumLength, "ActivePdcpUE/MaxActivePdcpUE:   %03d/%03d\n", accumulateCounter->activePdcpUe, accumulateCounter->maxActivePdcpUe);
        sumLength += varLength;
        // printf("\n");
        // printf("ActiveMacUE/MaxActiveMacUE:     %03d/%03d\n", accumulateCounter->activeUe, accumulateCounter->maxActiveMacUe);
        // printf("ActiveRlcUE/MaxActiveRlcUE:     %03d/%03d\n", accumulateCounter->activeRlcUe, accumulateCounter->maxActiveRlcUe);
        // printf("ActivePdcpUE/MaxActivePdcpUE:   %03d/%03d\n", accumulateCounter->activePdcpUe, accumulateCounter->maxActivePdcpUe);
    }

    if (VERSION != m_targetVersion) {
        varLength = sprintf(dispChar + sumLength, "--------------------------------------\n");
        sumLength += varLength;
        varLength = sprintf(dispChar + sumLength, "NOTE: Version[%d] is too old, please upgrade to Version[%d]\n", VERSION, m_targetVersion);
        sumLength += varLength;
        // printf("--------------------------------------\n");
        // printf("NOTE: Version[%d] is too old, please upgrade to Version[%d]\n", VERSION, m_targetVersion);
    }

    if (m_writeOption == 4) {
        int writeBytes = 0;
        m_resultFile->seek(F_SEEK_BEGIN);
        m_resultFile->write(dispChar, sumLength, writeBytes);
    } else {
        printf("%s\n", dispChar);
    }

    if (gShowPhyKpi) {
        system("sh /mnt/user2/getPhyKpi.sh");
    }
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
