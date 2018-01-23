#include <stdio.h>
#include "CliCommon.h"
#include "Qmss.h"

UInt8 gLogLevel = 2;

extern void kpiMain();

#ifdef KPI_L3
extern void parseAndExecuteKpiCmd(int argc, char* argv[]);
#else
extern void parseAndExecuteKpiCmd(Qmss* qmss, int argc, char* argv[]);
#endif

int main(int argc, char* argv[]) {
#ifndef KPI_L3
    Qmss::initQmss();
    Qmss* qmss = new Qmss(Qmss::QID_CLI_SEND_TO_L2, Qmss::QID_CLI_RECV_FROM_L2);
    system("clear");
    parseAndExecuteKpiCmd(qmss, argc, argv);
#else
    parseAndExecuteKpiCmd(argc, argv);
#endif 

    return 0;
}


