#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "Qmss.h"
#include "MacInterface.h"

#include "KpiService.h"

using namespace kpi;

// unsigned short gTransactionId = 0;

// unsigned short buildMacMsgHeader(
//     char* buffer, 
//     unsigned short srcModuleId, 
//     unsigned short dstModuleId, 
//     unsigned short msgId, 
//     unsigned short length) {

//     gTransactionId++;
//     buffer[0] = (gTransactionId >> 8) & 0xff;
//     buffer[1] = gTransactionId & 0xff;
//     buffer[2] = (srcModuleId >> 8) & 0xff;
//     buffer[3] = srcModuleId & 0xff;
//     buffer[4] = (dstModuleId >> 8) & 0xff;
//     buffer[5] = dstModuleId & 0xff;
//     buffer[6] = (msgId >> 8) & 0xff;
//     buffer[7] = msgId & 0xff;
//     buffer[8] = (length >> 8) & 0xff;
//     buffer[9] = length & 0xff;

//     return gTransactionId;
// }

// UInt16 buildMacKpiReq(LteMacMsg* msg) {
//     msg->transactionId = htons(++gTransactionId);
//     msg->srcModuleId =  htons(KPI_MODULE_ID);
//     msg->dstModuleId =  htons(MAC_MODULE_ID);
//     msg->msgId = htons(MAC_KPI_REQ);
//     msg->length = htons(LTE_MSG_HEAD_LENGTH);

//     return msg->transactionId;
// }

// void displayCounter(LteCounter* lteCounter) {
//     char rrcCounterName[] = "|RRCReq   |RRCSetup |RRCCompl |IDReq    |IDResp   |AttRej   |TAURej   |RRCRel   |RRCRej   |ReestReq |ReestRej |UEInfReq |UEInfRsp |";
//     char l2CounterName[] = "|ULCCCH   |DLCCCH   |ULDCCH   |DLDCCH   |";
//     char macCounterName[] = "|ActiveUe |RACH     |RAR      |MSG3     |ContResl |MSG3Exp  |CrcValid |CrcError |";
//     char rrcCounterVal[512];
//     char l2CounterVal[128];
//     char macCounterVal[512];
    
//     sprintf(rrcCounterVal, "%s\n|%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |", 
//         "+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+",
//         lteCounter->rrcReq, lteCounter->rrcSetup, lteCounter->rrcSetupComplete, lteCounter->identityReq,
//         lteCounter->idnetityResp, lteCounter->attachReject, lteCounter->tauReject, lteCounter->rrcRelease,
//         lteCounter->rrcConnReject, lteCounter->rrcReestablishmentReq, lteCounter->rrcReestabReject,
//         lteCounter->ueInfoReq, lteCounter->ueInfoRsp);
    
//     sprintf(l2CounterVal, "%s\n|%8d |%8d |%8d |%8d |", 
//         "+---------+---------+---------+---------+",
//         lteCounter->ulCCCH, lteCounter->dlCCCH, lteCounter->ulDCCH, lteCounter->dlDCCH);

//     sprintf(macCounterVal, "%s\n|%8d |%8d |%8d |%8d |%8d |%8d |%8d |%8d |", 
//         "+---------+---------+---------+---------+---------+---------+---------+---------+",
//         lteCounter->activeUe, lteCounter->rach, lteCounter->rar, lteCounter->msg3,
//         lteCounter->contResol, lteCounter->msg3Expired, lteCounter->crcValid, lteCounter->crcError);    

//     printf("%s\n%s\n\n%s\n%s\n\n", rrcCounterName, rrcCounterVal, l2CounterName, l2CounterVal);
//     printf("%s\n%s\n\n", macCounterName, macCounterVal);
// }


int main(int argc, char* argv[]) {
    printf("main thread running\n");

    KpiService* kpiService = new KpiService("KPI Service");
    kpiService->wait();
    

    // Init_Netcp();

    // Qmss* pMacQmss = new Qmss(Qmss::QID_KPI_SEND_TO_MAC, Qmss::QID_KPI_RECV_FROM_MAC);

    // char sendMsgBuffer[256];
    // char recvMsgBuffer[2048];

    // LteMacMsg* macMsg = (LteMacMsg*)sendMsgBuffer;

    // LteCounter prevCounter, currCounter;
    // memset((void*)&prevCounter, 0, sizeof(LteCounter));
    // memset((void*)&currCounter, 0, sizeof(LteCounter));

    // LteCounter* lteCounter = (LteCounter*)recvMsgBuffer;

    // while (1) {
    //     buildMacKpiReq(macMsg);
    //     int length = ntohs(macMsg->length);
    //     printf("send msg to MAC, length = %d\n", length);
    //     pMacQmss->send(sendMsgBuffer, length);

    //     usleep(5000);

    //     length = pMacQmss->recv(recvMsgBuffer);
    //     printf("recv msg from MAC, length = %d\n", length);
    //     if (length > 0) {
    //         int i = 0;
    //         for(i=0; i<length; i++) {
    //             printf("%02x ", recvMsgBuffer[i]);
    //         }
    //         printf("\n");
    //     }

    //     displayCounter(lteCounter);


    //     usleep(1000000);
    // }

    return 0;
}


