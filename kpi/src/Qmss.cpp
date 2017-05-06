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
    m_sendQId = (Qmss_HANDLE)sendQId;
    m_recvQId = (Qmss_HANDLE)recvQId;
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
int Qmss::send(char* msgBuffer, int length) {
    return Qmss_SendData(m_sendQId, (void*)msgBuffer, length);
}

// --------------------------------------
int Qmss::recv(char* msgBuffer) {
    if (count() > 0) {
        return Qmss_RecsData(m_recvQId, (void*)msgBuffer);
    }

    return 0;
}

// --------------------------------------
int Qmss::count() {
    return Qmss_GetQueueEntryCount(m_recvQId);
}
