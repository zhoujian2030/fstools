/*
 * CliTcpServerSocket.h
 *
 *  Created on: July 6, 2017
 *      Author: j.zhou
 */

#ifndef CLI_TCP_SERVER_SOCKET_H
#define CLI_TCP_SERVER_SOCKET_H

#include "Socket.h"
#include "TcpSocket.h"
#include "MutexLock.h"

namespace cli {

    class CliTcpServerSocket : public net::Socket {
    public:
        CliTcpServerSocket(
            std::string localIp, 
            unsigned short localPort,
            int backlog = 5);
        
        virtual ~CliTcpServerSocket();

        net::TcpSocket* accept();

        void close();
    };

}

#endif

