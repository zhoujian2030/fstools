/*
 * UdpSocket.cpp
 *
 *  Created on: Nov 01, 2016
 *      Author: j.zhou
 */

#include "UdpSocket.h"
#include "CLogger.h"
#include "IoException.h"
#include <string.h>

using namespace net;
using namespace std;
using namespace cm;

// ----------------------------------------------
UdpSocket::UdpSocket(std::string localIp, unsigned short localPort) 
: Socket(localIp, localPort, SOCK_DGRAM, 0, AF_INET),
  m_udpState(UDP_IDLE)//,
//   m_reactor(0), 
//   m_socketListener(0)
{
    m_lock = new MutexLock(true);
    bind();
    m_udpState = UDP_OPEN;
    LOG_DBG(NET_LOGGER_NAME, "Create UDP Server socket: %s:%d\n", localIp.c_str(), localPort);
}

// ----------------------------------------------
UdpSocket::UdpSocket()
: Socket("", 0, SOCK_DGRAM, 0, AF_INET),
  m_udpState(UDP_IDLE)//, 
//   m_reactor(0), 
//   m_socketListener(0)
{
    m_lock = new MutexLock(true);
    m_udpState = UDP_OPEN;
    LOG_DBG(NET_LOGGER_NAME, "Create UDP client socket");
}

// -----------------------------------------------
UdpSocket::~UdpSocket() {
    close();
}

// // -----------------------------------------------
// void UdpSocket::addSocketListener(UdpSocketListener* socketListener) {
//     if (socketListener != 0) {
//         if (m_socketListener == 0) {
//             // if previous listener is null, it means the socket is blocking mode currently
//             m_reactor = Reactor::getInstance();
//             makeNonBlocking();
//         }
//     } else {
//         if (m_socketListener != 0) {
//             makeBlocking();
//         }
//     }

//     m_socketListener = socketListener;

// }

// -----------------------------------------------
int UdpSocket::receive(char* theBuffer, int buffSize, InetAddressPort& theRemoteAddrPort) {
    // // async mode
    // if (m_socketListener != 0) {
    //     // TODO
    //     return 0;
    // } 
    // sync mode
    // else {
        int numOfByteRecved;
        int result = Socket::recvfrom(theBuffer, buffSize, numOfByteRecved, theRemoteAddrPort);
        if (result != SKT_ERR) {
            // in case no data received in non-blocking IO
            if (numOfByteRecved < 0) {
                LOG_ERROR(NET_LOGGER_NAME, "no data ready on the socket receive buffer");
                numOfByteRecved = 0;
            }
            return numOfByteRecved;
        }
        LOG_ERROR(NET_LOGGER_NAME, "error occurred on recvfrom()");
        return -1;
    // }
}

// ----------------------------------------------
int UdpSocket::send(const char* theBuffer, int numOfBytesToSend, InetAddressPort& theRemoteAddrPort) {
    // // async mode
    // if (m_socketListener != 0) {
    //     // TODO
    //     return 0;
    // } 
    // sync mode
    // else {
        int numberOfBytesSent;
        int result = Socket::sendto(theBuffer, numOfBytesToSend, numberOfBytesSent, theRemoteAddrPort);
        if (result == SKT_ERR) {
            LOG_ERROR(NET_LOGGER_NAME, "error occur on sendto() in sync mode, close the socket.");
            close(); // TODO
            return numberOfBytesSent;
        }
        
        // if success, numberOfBytesSent is set actual number of bytes sent, 
        // or it is set -1 in Socket::send

        // in case no data sent out due to send buffer full in non-blocking IO
        if (numOfBytesToSend < 0) {
            numOfBytesToSend = 0;
        }
        return numberOfBytesSent;
    // }    
}

// ----------------------------------------------
void UdpSocket::close() {
    LOG_DBG(NET_LOGGER_NAME, "UdpSocket::close(), fd = %d\n", this->getSocket());  
    if (this->getSocket() < 0) {
        return;
    }

    // if (m_socketListener != 0) {
    //     // async mode
    //     if (m_udpState != UDP_CLOSED) {
    //         // TODO
    //     }
        
    // } else {
        // sync mode
        if (m_udpState != UDP_CLOSED) {
            Socket::close();
            m_udpState = UDP_CLOSED;   
        }   
    // }
}

// // ----------------------------------------------
// void UdpSocket::addSocketHandlerForNonAsync(
//     UdpSocketListener* socketListener, 
//     char* theBuffer, 
//     int bufferSize)
// {
//     if (socketListener != 0) {
//         if (theBuffer == 0) {
//             return;
//         }
//         if (m_socketListener == 0) {
//             // if previous listener is null, it means the socket is blocking mode currently
//             makeNonBlocking();
//             m_recvBuffer = theBuffer;
//             m_recvBufferSize = bufferSize;
//         }
//     } else {
//         if (m_socketListener != 0) {
//             makeBlocking();
//         }
//     }

//     m_socketListener = socketListener;    
// }

// ----------------------------------------------
void UdpSocket::handleInput(Socket* theSocket) {
    // LOG4CPLUS_DEBUG(_NET_LOOGER_NAME_, "UdpSocket::handleInput(), fd = " << theSocket->getSocket());
    // int numOfByteRecved;
    // InetAddressPort remoteAddrPort;
    // int result = Socket::recvfrom(m_recvBuffer, m_recvBufferSize, numOfByteRecved, remoteAddrPort);
    // if (result != SKT_ERR) {
    //     if (numOfByteRecved > 0) {
    //         m_socketListener->handleRecvResult(this, numOfByteRecved);
    //     } else {
    //         LOG4CPLUS_DEBUG(_NET_LOOGER_NAME_, "No data receive");
    //     }
    // } else {
    //     LOG4CPLUS_ERROR(_NET_LOOGER_NAME_, "error occurred on recvfrom()");
    // }
}

// ----------------------------------------------
void UdpSocket::handleOutput(Socket* theSocket) {
    // TODO    
}
