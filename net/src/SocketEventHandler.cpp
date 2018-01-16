/*
 * SocketEventHandler.cpp
 *
 *  Created on: Apr 09, 2016
 *      Author: z.j
 */

#include "SocketEventHandler.h"
#include "Socket.h"
#include <iostream>

using namespace net;

// -------------------------------------------------
void SocketEventHandler::handleInput(Socket* theSocket) {
    // empty
}

// -------------------------------------------------
void SocketEventHandler::handleOutput(Socket* theSocket) {
    std::cout << "SocketEventHandler::handleOutput(), fd = " << theSocket->getSocket() << std::endl;
    // empty
}

// -------------------------------------------------
void SocketEventHandler::handleException(Socket* theSocket) {
    // empty
}
