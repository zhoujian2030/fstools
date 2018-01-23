/*
 * CliMain.cpp
 *
 *  Created on: June 12, 2017
 *      Author: j.zhou
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>

#include "Qmss.h"
#include "CliCommon.h"
#include "Util.h"
#include "CliCommon.h"
#include "CommandParser.h"

using namespace cli;
using namespace std;

UInt8 gLogLevel = 2;

int main(int argc, char* argv[]) {
    Qmss::initQmss();
    Qmss* qmss = new Qmss(Qmss::QID_CLI_SEND_TO_L2, Qmss::QID_CLI_RECV_FROM_L2);
    system("clear");
    CommandParser commandParser;
    commandParser.parse(argc, argv);
    commandParser.execute(qmss, argc, argv);

    return 0;
}


