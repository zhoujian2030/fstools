/*
 * Qmss.h
 *
 *  Created on: May 5, 2017
 *      Author: j.zhou
 */

#ifndef QMSS_H
#define QMSS_H

#ifdef TWO_CARRIER
#include "netcp_interface_new.h"
#else
#include "netcp_interface.h"
#endif

class Qmss {
public:
    Qmss(int sendQId, int recvQId);
    ~Qmss();

    // only need to init once
    static void initQmss();

    enum {
#ifdef TWO_CARRIER
#ifdef TDD
        QID_CLI_SEND_TO_L2      = 2,

        QID_CLI_RECV_FROM_L2    = 7, 

        QID_TST_SEND_START_TEST = 0, // todo
#else
        QID_CLI_SEND_TO_L2      = 22,

        QID_CLI_RECV_FROM_L2    = 27, 

        QID_TST_SEND_START_TEST = 0, // todo
#endif
#else
        QID_CLI_SEND_TO_L2 = OAM_SEND_L2_CFG_REQ,

        QID_CLI_RECV_FROM_L2 = CMAC_RECS_L2_UL_MAC_CE, // L3_RECS_CMAC_CFG_RSP, 
        
        QID_TST_SEND_START_TEST = CMAC_SEND_L2_HARQ_ACK,
#endif
        QID_INVALID_ID
    };

    void updateDstQmssId(int sendQId);

    int send(char* msgBuffer, int length);
    int recv(char* msgBuffer);
    int count();

private:    
    int m_sendQId;
    int m_recvQId;

    static bool m_isQmssInited;
};

#endif
