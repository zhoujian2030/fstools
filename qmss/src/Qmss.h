/*
 * Qmss.h
 *
 *  Created on: May 5, 2017
 *      Author: j.zhou
 */

#ifndef QMSS_H
#define QMSS_H

#include "netcp_interface.h"

class Qmss {
public:
    Qmss(int sendQId, int recvQId);
    ~Qmss();

    // only need to init once
    static void initQmss();

    enum {
        QID_CLI_SEND_TO_L2 = OAM_SEND_L2_CFG_REQ,

        QID_CLI_RECV_FROM_L2 = L3_RECS_CMAC_CFG_RSP,
        
        QID_TST_SEND_START_TEST = CMAC_SEND_L2_HARQ_ACK,

        QID_INVALID_ID
    };

    void updateDstQmssId(int sendQId);

    int send(char* msgBuffer, int length);
    int recv(char* msgBuffer);
    int count();

private:    
    Qmss_HANDLE m_sendQId;
    Qmss_HANDLE m_recvQId;

    static bool m_isQmssInited;
};

#endif
