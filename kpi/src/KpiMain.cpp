#include <stdio.h>
#include "CliCommon.h"
#include "Qmss.h"
#include "KpiCommandParser.h"

UInt8 gLogLevel = 2;

using namespace kpi;

int main(int argc, char* argv[]) {
    KpiCommandParser kpiCmdParser;
#ifndef KPI_L3
    Qmss::initQmss();
    Qmss* qmss = new Qmss(Qmss::QID_CLI_SEND_TO_L2, Qmss::QID_CLI_RECV_FROM_L2);
    system("clear");
    kpiCmdParser.parseAndExecute(qmss, argc, argv);
#else
    kpiCmdParser.parseAndExecute(argc, argv);
#endif 

    return 0;
}
