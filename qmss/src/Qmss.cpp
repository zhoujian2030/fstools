/*
 * Qmss.cpp
 *
 *  Created on: May 5, 2017
 *      Author: j.zhou
 */

#include "Qmss.h"

bool Qmss::m_isQmssInited = false;

// --------------------------------------
Qmss::Qmss(int sendQId, int recvQId) 
{
    m_sendQId = sendQId;
    m_recvQId = recvQId;
}

// --------------------------------------
Qmss::~Qmss() {
 
}

// --------------------------------------
void Qmss::initQmss() {
    if (!m_isQmssInited) {
        Init_Netcp();
        m_isQmssInited = true;
    }
}

// --------------------------------------
void Qmss::updateDstQmssId(int sendQId) {
    m_sendQId = sendQId;
}

// --------------------------------------
int Qmss::send(char* msgBuffer, int length) {
#ifndef TWO_CARRIER
    return Qmss_SendData((Qmss_HANDLE)m_sendQId, (void*)msgBuffer, length);
#else
    return Qmss_SendData(m_sendQId, (void*)msgBuffer, length);
#endif
}

// --------------------------------------
int Qmss::recv(char* msgBuffer) {
    if (count() > 0) {
#ifndef TWO_CARRIER
        return Qmss_RecsData((Qmss_HANDLE)m_recvQId, (void*)msgBuffer);
#else
        return Qmss_RecsData(m_recvQId, (void*)msgBuffer);
#endif
    }

    return 0;
}

// --------------------------------------
int Qmss::count() {
#ifndef TWO_CARRIER
    return Qmss_GetQueueEntryCount((Qmss_HANDLE)m_recvQId);
#else
    return Qmss_GetQueueEntryCount(m_recvQId);
#endif
}
