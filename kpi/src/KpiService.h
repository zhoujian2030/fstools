/*
 * KpiService.h
 *
 *  Created on: May 05, 2017
 *      Author: j.zhou
 */

#ifndef KPI_SERVICE_H
#define KPI_SERVICE_H

#include "Service.h"
#if (defined USE_UDP) || (defined KPI_L3)
#include "UdpSocket.h"
// #include "SelectSocketSet.h"
#endif
#include "EventIndicator.h"
#include "CliCommon.h"

class Qmss;

namespace kpi {

    class KpiService : public cm::Service {
    public:
#ifdef KPI_L3
        KpiService(std::string serviceName);
#else
        KpiService(std::string serviceName, Qmss* qmss, UInt32 msgId);
#endif
        virtual ~KpiService();

        void postEvent();

    private:
        virtual unsigned long run();

        int sendKpiReq();

#ifdef KPI_L3
        net::UdpSocket* m_udpServerSocket;
        net::Socket::InetAddressPort m_rrcAddress;
#else
        Qmss* m_macQmss;
#endif
        char m_sendBuffer[MAC_SEND_MSG_BUFFER_LENGTH];
        unsigned int m_transactionId;

        UInt32 m_period;
        UInt32 m_msgId;

        cm::EventIndicator m_stopEvent;
    };

    // ------------------------------------
    inline void KpiService::postEvent() {
        m_stopEvent.set();
    }

}

#endif
