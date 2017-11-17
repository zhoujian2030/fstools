/*
 * Service.cpp
 *
 *  Created on: June 7, 2016
 *      Author: z.j
 */

#include "Service.h"

using namespace cm;

// ------------------------------------------
Service::Service(std::string serviceName) 
: Thread(serviceName.c_str()) {

}

// ------------------------------------------
Service::~Service() {

}

// ------------------------------------------
void Service::init() {
    this->start(true);
}
