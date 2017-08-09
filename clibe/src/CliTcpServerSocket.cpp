/*
 * CliTcpServerSocket.cpp
 *
 *  Created on: July 6, 2017
 *      Author: j.zhou
 */

#include "CliTcpServerSocket.h"
#include "IoException.h"
#include "CLogger.h"
#include <assert.h>

using namespace cli;
using namespace net;

// ------------------------------------------
CliTcpServerSocket::CliTcpServerSocket(
    std::string localIp, 
    unsigned short localPort,
    int backlog) 
: Socket(localIp, localPort, SOCK_STREAM, 0, AF_INET)
{
    bind();
    listen(backlog);
}

// ------------------------------------------
CliTcpServerSocket::~CliTcpServerSocket() {

}

// -------------------------------------------------------
void CliTcpServerSocket::close() {
    Socket::close();
}

// -------------------------------------------------------
TcpSocket* CliTcpServerSocket::accept() {
    int newSocket;
    Socket::InetAddressPort remoteAddrPort;
    try {
        // As the socket is blocking mode, can not be SKT_WAIT!
        assert(Socket::accept(newSocket, remoteAddrPort));
        LOG_DBG(CLI_LOGGER_NAME, "[%s], accept a new TCP connection, new fd = %d\n", __func__, newSocket);
        return new TcpSocket(newSocket, remoteAddrPort);
    } catch (cm::IoException& e) {
        LOG_DBG(CLI_LOGGER_NAME, "[%s], error occurred on Socket::accept()\n", __func__);
        close();
        throw e;
    }

}
