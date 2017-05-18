/*
 * KpiService.h
 *
 *  Created on: May 05, 2017
 *      Author: j.zhou
 */

#ifndef KPI_SERVICE_H
#define KPI_SERVICE_H

#include "Service.h"
#include "UdpSocket.h"
#include "SelectSocketSet.h"
#include "EventIndicator.h"
#include "MacInterface.h"

class Qmss;

namespace kpi {

    class KpiService : public cm::Service {
    public:
        KpiService(std::string serviceName);
        virtual ~KpiService();

        void postEvent();

    private:
        virtual unsigned long run();

        int sendMacKpiReq();

        Qmss* m_macQmss;
        char m_sendBuffer[MAC_SEND_MSG_BUFFER_LENGTH];
        unsigned int m_transactionId;

        UInt32 m_period;

        cm::EventIndicator m_stopEvent;
    };

    // ------------------------------------
    inline void KpiService::postEvent() {
        m_stopEvent.set();
    }

}

#endif
