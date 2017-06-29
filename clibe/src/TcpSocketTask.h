/*
 * TcpSocketTask.h
 *
 *  Created on: June 14, 2017
 *      Author: j.zhou
 */

#ifndef TCP_SOCKET_TASK_H
#define TCP_SOCKET_TASK_H

#include "Task.h"
#include "TcpSocket.h"
#include "DataBuffer.h"

namespace cli {

    class TcpSocketTask : public cm::Task {
    public:
        TcpSocketTask(net::TcpSocket* tcpSocket);
        virtual ~TcpSocketTask();

        virtual int execute();

    private:
        net::TcpSocket* m_tcpSocket;
        cm::DataBuffer* m_tcpDataBuffer;
    };

}

#endif
