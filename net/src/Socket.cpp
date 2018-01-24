/*
 * Socket.cpp
 *
 *  Created on: Apr 05, 2016
 *      Author: z.j
 */

#include "Socket.h"
#include "CLogger.h"
#include "IoException.h"

#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <assert.h>

using namespace std;
using namespace net;
using namespace cm;

// ------------------------------------------------
Socket::Socket(
    std::string localIp, 
    unsigned short localPort, 
    int socketType, 
    int protocol,
    int saFamily)
: m_socket(-1), m_socketType(socketType), m_localIp(localIp), 
  m_localPort(localPort), m_state(CLOSED)
{
    // create socket
    m_socket = ::socket(saFamily, socketType, protocol);
    if (m_socket == -1) {
        LOG_ERROR(NET_LOGGER_NAME, "fail to create socket. errno = %d - %s\n", errno, strerror(errno));
        throw IoException();
    }

    // set the socket reuse address by default to avoid the port is locked after a system crash
    int option = 1;
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&option, sizeof(option)) != 0) {
        LOG_ERROR(NET_LOGGER_NAME, "fail to set SO_REUSEADDR. errno = %d - %s\n", errno, strerror(errno));
    }

    // for client socket, m_localPort is set to 0, m_localIp is unspecified ?
    if (m_localPort != 0) {
        // check if the ip is valid
        assert(INADDR_NONE != inet_addr(m_localIp.c_str()));

        memset(&m_localSa, 0, sizeof(struct sockaddr_in));
        m_localSa.sin_family = saFamily;
        m_localSa.sin_addr.s_addr = inet_addr(m_localIp.c_str());
        m_localSa.sin_port = htons(m_localPort);
        
        m_role = SERVER_SOCKET;
    } else {
        m_role = CLIENT_SOCKET;
    }

    m_state = CREATED;

    LOG_DBG(NET_LOGGER_NAME, "Success to create socket: %d, address = %s:%d\n", m_socket, m_localIp.c_str(), m_localPort);
}

// ------------------------------------------------
Socket::Socket(int socket, int socketType)
: m_socket(socket), m_socketType(socketType), m_localIp("null"), 
  m_localPort(0), m_state(CONNECTED)
{
    socklen_t length = sizeof(struct sockaddr);

    if (::getsockname(m_socket, (struct sockaddr*)&m_localSa, &length) == 0) {
        m_localPort = ntohs(m_localSa.sin_port);
        m_localIp = Socket::getHostAddress((struct sockaddr*)&m_localSa);
    }
    
    m_role = CONNECT_SOCKET;

    LOG_DBG(NET_LOGGER_NAME, "initialize the connected socket, fd = %d, local address = %s:%d\n", m_socket, m_localIp.c_str(), m_localPort);
}

// ------------------------------------------------
Socket::~Socket() {
    close();
}

// ------------------------------------------------
void Socket::bind() {
    if (m_state == BINDED || m_localPort == 0) {
        LOG_ERROR(NET_LOGGER_NAME, "no need to bind socket: %d\n", m_socket);
        return;
    }
    assert(m_state == CREATED);

    int result = ::bind(m_socket, (struct sockaddr *)&m_localSa, sizeof(struct sockaddr_in));
    if (result == -1) {
        LOG_ERROR(NET_LOGGER_NAME, "fail to bind socket, fd = %d, address = %s:%d, errno = %d - %s\n", m_socket, m_localIp.c_str(), m_localPort, errno, strerror(errno));
        close();
        throw IoException();
    }
    m_state = BINDED;
    LOG_DBG(NET_LOGGER_NAME, "success to bind socket: %d\n", m_socket);
}

// ------------------------------------------------
void Socket::listen(int backlog) {
    if (m_state == LISTENING) {
        LOG_DBG(NET_LOGGER_NAME, "socket is already listening: %d\n", m_socket);
        return;
    }
    assert(m_state == BINDED);

    int result = ::listen(m_socket, backlog);
    if (result == -1) {
        LOG_ERROR(NET_LOGGER_NAME, "fail to listen socket, fd = %d, errno = %d - %s\n", m_socket, errno, strerror(errno));
        close();
        throw IoException();
    }
    m_state = LISTENING;
    LOG_DBG(NET_LOGGER_NAME, "success to listen socket: %d\n", m_socket);
}

// ------------------------------------------------
bool Socket::accept(int& theSocket, InetAddressPort& theRemoteAddrPort) {
    LOG_DBG(NET_LOGGER_NAME, "Socket::accept(), fd = %d\n", m_socket);
    assert(m_state == LISTENING);

    struct sockaddr_in remoteAddr;
    socklen_t length = sizeof(remoteAddr);
    int newFd = ::accept(m_socket, (struct sockaddr*)&remoteAddr, &length);
    if (newFd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // For non-blocking socket, it would return EAGAIN or EWOULDBLOCK 
            // when no data read from socket
            LOG_DBG(NET_LOGGER_NAME, "no new connection coming now, fd = %d\n", m_socket);
            return false;
        } else {
            LOG_ERROR(NET_LOGGER_NAME, "fail to accept for socket: %d. errno = %d - %s\n", m_socket, errno, strerror(errno));
            throw IoException();
        }
    }

    memcpy(&theRemoteAddrPort.addr, &remoteAddr, sizeof(theRemoteAddrPort.addr));
    theRemoteAddrPort.port = ntohs(remoteAddr.sin_port);
    theSocket = newFd;

    LOG_DBG(NET_LOGGER_NAME, "accept new connection, fd = %d, remote address = %s:%d\n", 
        theSocket, (Socket::getHostAddress((struct sockaddr*)&theRemoteAddrPort.addr)).c_str(), theRemoteAddrPort.port);

    return true;
}

// ------------------------------------------------
bool Socket::connect(const InetAddressPort& theRemoteAddrPort) {
    LOG_DBG(NET_LOGGER_NAME, "Socket::connect(), fd = %d\n", m_socket);

    int result = ::connect(m_socket, (struct sockaddr*)&theRemoteAddrPort.addr, sizeof(theRemoteAddrPort.addr));
    if (result == -1) {
        if (errno == EINPROGRESS) {
            LOG_DBG(NET_LOGGER_NAME, "connecting, fd = %d, %s\n", m_socket, strerror(errno));
            return false;
        } else {
            LOG_ERROR(NET_LOGGER_NAME, "fail to connect on socket: %d. errno = %d - %s\n", m_socket, errno, strerror(errno));
            throw IoException();
        }
    }

    socklen_t length = sizeof(struct sockaddr);
    if (::getsockname(m_socket, (struct sockaddr*)&m_localSa, &length) == 0) {
        m_localPort = ntohs(m_localSa.sin_port);
        m_localIp = Socket::getHostAddress((struct sockaddr*)&m_localSa);

        LOG_DBG(NET_LOGGER_NAME, "connect success, local address = %s:%d, fd = %d\n", m_localIp.c_str(), m_localPort, m_socket);

        m_state = CONNECTED;
        return true;
    } else {
        LOG_ERROR(NET_LOGGER_NAME, "fail to getsockname on socket: %d. errno = %d - %s\n", m_socket, errno, strerror(errno));
        throw IoException();
    }
}

// ------------------------------------------------
void Socket::close() {
    m_state = CLOSED;
    if (m_socket != -1) {
        ::close(m_socket);
        m_socket = -1;
    }
}

// -------------------------------------------------
//  only used in connected socket (TCP socket)
// -------------------------------------------------
int Socket::recv(char* theBuffer, int buffSize, int& numOfBytesReceived, int flags) {
    if (theBuffer == 0) {
        throw std::invalid_argument("theBuffer is a null pointer!");
    }
    LOG_DBG(NET_LOGGER_NAME, "Socket::recv(), fd = %d\n", m_socket);

    int result = ::recv(m_socket, theBuffer, buffSize, flags);
    if (result == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // For non-blocking socket, it would return EAGAIN or EWOULDBLOCK 
            // when no data read from socket
            LOG_DBG(NET_LOGGER_NAME, "no data received from the socket now, fd = %d, %s\n", m_socket, strerror(errno));
            return SKT_WAIT;
        } else {
            LOG_ERROR(NET_LOGGER_NAME, "fail to recv data from socket: %d, errno = %d - %s\n", m_socket, errno, strerror(errno));
            // TODO throw io exception ??
            return SKT_ERR;
        }
    }

    numOfBytesReceived = result;
    return SKT_SUCC;
}

// -------------------------------------------------
int Socket::read(char* theBuffer, int buffSize, int& numOfBytesReceived) {
    if (theBuffer == 0) {
        throw std::invalid_argument("theBuffer is a null pointer!");
    }

    LOG_DBG(NET_LOGGER_NAME, "Socket::read(), fd = %d\n", m_socket);    

    int result = ::read(m_socket, theBuffer, buffSize);

    if (result == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // For non-blocking socket, it would return EAGAIN or EWOULDBLOCK 
            // when no data read from socket
            LOG_DBG(NET_LOGGER_NAME, "no data read from the socket now, fd = %d, %s", m_socket, strerror(errno));
            return SKT_WAIT;
        } else {
            LOG_ERROR(NET_LOGGER_NAME, "fail to read data from socket: %d, errno = %d - %s\n", m_socket, errno, strerror(errno));
            // TODO throw io exception ??
            return SKT_ERR;
        }
    }

    numOfBytesReceived = result;
    return SKT_SUCC;    
}

// -------------------------------------------------
int Socket::recvfrom(
    char* theBuffer, 
    int buffSize, 
    int& numOfBytesReceived, 
    InetAddressPort& theRemoteAddrPort) 
{
    if (theBuffer == 0) {
        throw std::invalid_argument("theBuffer is a null pointer!");
    }
    LOG_DBG(NET_LOGGER_NAME, "Socket::recvfrom(), fd = %d\n", m_socket);

    struct sockaddr_in remoteAddr;
    socklen_t length = sizeof(remoteAddr);
    int result = ::recvfrom(m_socket, theBuffer, buffSize, 0, (struct sockaddr*)&remoteAddr, &length);
    numOfBytesReceived = result;
    if (result == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // For non-blocking socket, it would return EAGAIN or EWOULDBLOCK 
            // when no data read from socket
            LOG_DBG(NET_LOGGER_NAME, "no data received from the socket now, fd = %d, %s\n", m_socket, strerror(errno));           
            return SKT_WAIT;
        } else {
            LOG_ERROR(NET_LOGGER_NAME, "fail to recv data from socket: %d, errno = %d - %s\n", m_socket, errno, strerror(errno));
            // TODO throw io exception ??
            return SKT_ERR;
        }
    }
    
    memcpy(&theRemoteAddrPort.addr, &remoteAddr, sizeof(theRemoteAddrPort.addr));
    theRemoteAddrPort.port = ntohs(remoteAddr.sin_port);
    return SKT_SUCC;
}

// -------------------------------------------------
int Socket::send(const char* theBuffer, int numOfBytesToSend, int& numberOfBytesSent) {
    if (theBuffer == 0) {
        throw std::invalid_argument("theBuffer is a null pointer!");
    }

    LOG_DBG(NET_LOGGER_NAME, "Socket::send(), fd = %d\n", m_socket);

    numberOfBytesSent = ::send(m_socket, theBuffer, numOfBytesToSend, 0);
    if (numberOfBytesSent == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // For non-blocking socket, it would return EAGAIN or EWOULDBLOCK 
            // when send buffer is full
            LOG_DBG(NET_LOGGER_NAME, "no data sent to the socket now, fd = %d\n", m_socket);
            return SKT_WAIT;
        } else {
            LOG_ERROR(NET_LOGGER_NAME, "fail to send data to socket: %d, errno = %d - %s\n", m_socket, errno, strerror(errno));    
            return SKT_ERR;        
        }
    }

    return SKT_SUCC;
}

// -------------------------------------------------
int Socket::write(const char* theBuffer, int numOfBytesToSend, int& numberOfBytesSent) {
    // TODO
    return false;
}

// -------------------------------------------------
int Socket::sendto(
    const char* theBuffer, 
    int numOfBytesToSend, 
    int& numberOfBytesSent, 
    InetAddressPort& theRemoteAddrPort)
{
    if (theBuffer == 0) {
        throw std::invalid_argument("theBuffer is a null pointer!");
    }

    LOG_DBG(NET_LOGGER_NAME, "Socket::sendto(), fd = %d\n", m_socket);

    socklen_t length = sizeof(theRemoteAddrPort.addr);
    numberOfBytesSent = ::sendto(m_socket, theBuffer, numOfBytesToSend, 
        0, (struct sockaddr*)&theRemoteAddrPort.addr, length);

    if (numberOfBytesSent == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // For non-blocking socket, it would return EAGAIN or EWOULDBLOCK 
            // when send buffer is full
            LOG_DBG(NET_LOGGER_NAME, "no data send to the socket now, fd = %d\n", m_socket);
            return SKT_WAIT;
        } else {
            LOG_ERROR(NET_LOGGER_NAME, "fail to send data to socket: %d. errno = %d - %s\n", m_socket, errno, strerror(errno));    
            return SKT_ERR;        
        }
    }

    return SKT_SUCC;
}

// -------------------------------------------------
void Socket::makeNonBlocking() {
    int option = 1;
    if (ioctl(m_socket, FIONBIO, &option) == -1) {
        LOG_ERROR(NET_LOGGER_NAME, "fail to set non-blocking by ioctl. errno = %d - %s\n", errno, strerror(errno));
    }
}

// -------------------------------------------------
void Socket::makeBlocking() {
    int flags = fcntl(m_socket, F_GETFL, 0);
    if (fcntl(m_socket, F_SETFL, flags & (~O_NONBLOCK)) == -1) {
        LOG_ERROR(NET_LOGGER_NAME, "fail to set blocking by fcntl. errno = %d - %s\n", errno, strerror(errno));
    }
}

// -------------------------------------------------
void Socket::setSendBufferSize(int size) {
    if (setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(size)) != 0) {
        LOG_ERROR(NET_LOGGER_NAME, "fail to set SO_SNDBUF. errno = %d - %s\n", errno, strerror(errno));
    }    
}

// -------------------------------------------------
int Socket::getSendBufferSize() {
    int bufferSize = 0;
    socklen_t size = sizeof(int);
    if (getsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSize, &size) != 0) {
        LOG_ERROR(NET_LOGGER_NAME, "fail to get SO_SNDBUF. errno = %d - %s\n", errno, strerror(errno));
        bufferSize = -1;
    }    

    return bufferSize;
}

// -------------------------------------------------
void Socket::setRecvBufferSize(int size) {
    if (setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(size)) != 0) {
        LOG_ERROR(NET_LOGGER_NAME, "fail to set SO_RCVBUF. errno = %d - %s\n", errno, strerror(errno));
    }    
}

// -------------------------------------------------
int Socket::getRecvBufferSize() {
    int bufferSize = 0;
    socklen_t size = sizeof(int);
    if (getsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, &size) != 0) {
        LOG_ERROR(NET_LOGGER_NAME, "fail to get SO_SNDBUF. errno = %d - %s\n", errno, strerror(errno));
        bufferSize = -1;
    }    

    return bufferSize;
}

// ------------------------------------------------
std::string Socket::getHostAddress(struct sockaddr* sockaddr) {
    if (sockaddr == 0) {
        return "null";
    }

    char tempAddr[NI_MAXHOST];
    socklen_t length = sizeof(struct sockaddr_in);

    int result = getnameinfo(sockaddr, length, tempAddr, sizeof(tempAddr), 0, 0, NI_NUMERICHOST);

    if (result != 0) {
        return "null"; 
    }

    std::string hostAddr(tempAddr);

    return hostAddr;
}

// ------------------------------------------------
void Socket::getSockaddrByIpAndPort(struct sockaddr_in* sockaddr, std::string ip, unsigned short port) {
    if (sockaddr == 0) {
        return;
    }

    memset(sockaddr, 0, sizeof(struct sockaddr_in));
    sockaddr->sin_family = AF_INET;
    sockaddr->sin_addr.s_addr = inet_addr(ip.c_str());
    sockaddr->sin_port = htons(port);

}
