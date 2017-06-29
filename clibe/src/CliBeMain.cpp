/*
 * CliBeMain.cpp
 *
 *  Created on: June 14, 2017
 *      Author: j.zhou
 */
#include <stdio.h>
#include "CLogger.h"
#include "Util.h"

#include "BeService.h"

using namespace cli;
using namespace std;

UInt8 gLogLevel = 1;

int main(int argc, char* argv[]) {

    LOG_DBG(CLI_LOGGER_NAME, "[%s], Start CLI Backend\n", __func__);
    BeService::getInstance()->wait();

    return 0;
}


