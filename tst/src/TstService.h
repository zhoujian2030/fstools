/*
 * TstService.h
 *
 *  Created on: May 10, 2017
 *      Author: j.zhou
 */

#ifndef TST_SERVICE_H
#define TST_SERVICE_H

#include "Service.h"
#include "EventIndicator.h"

class Qmss;

namespace tst {

    typedef unsigned int UInt32;
    typedef signed int SInt32;
    typedef unsigned short UInt16;
    typedef unsigned char UInt8;

    // module id
    #define TST_MODULE_ID            0x9801
    #define SIM_MODULE_ID            0x9802

    // msg id
    typedef enum {
        SIM_TST_START = 0x01,
        SIM_TST_STOP  = 0x02
    } TSTReqAPI;

    #define COLLECT_PERIOD_MS 1000

    #define LTE_CLI_MSG_HEAD_LENGTH 10
    #define MAC_VAR_SIZE(x) 1
    typedef struct {
        UInt16 transactionId;
        UInt16 srcModuleId;
        UInt16 dstModuleId;
        UInt16 msgId;
        UInt16 length;
        char msgBody[MAC_VAR_SIZE(length)];
    } LteTstMsg;

    class TstService : public cm::Service {
    public:
        TstService(std::string serviceName);
        virtual ~TstService();

        void postEvent();

    private:
        virtual unsigned long run();

        Qmss* m_simQmss;
        char m_sendBuffer[100];

        cm::EventIndicator m_stopEvent;
    };

    // ------------------------------------
    inline void TstService::postEvent() {
        m_stopEvent.set();
    }

}

#endif
