/*
 * KpiCommandParser.cpp
 *
 *  Created on: May 10, 2017
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

#include "KpiService.h"
#include "KpiCommandParser.h"

using namespace kpi;
using namespace std;

#define KPI_DEFAULT_WRITE_OPTION 4
int gWriteOption = KPI_DEFAULT_WRITE_OPTION; 
string gServerIp = "192.168.1.166";
unsigned short gServerPort = 50001;
int gPeriod = 10000;
string gKpiDirectory = "/OAM";

bool gShowAll = false;
bool gShowPhyKpi = false;

extern UInt8 gLogLevel;

// ----------------------------------------------
KpiCommandParser::KpiCommandParser() {

}

// ----------------------------------------------
KpiCommandParser::~KpiCommandParser() {

}

// ----------------------------------------------
#ifdef KPI_L3
bool KpiCommandParser::parseAndExecute(int argc, char* argv[])
#else
bool KpiCommandParser::parseAndExecute(Qmss* qmss, int argc, char* argv[])
#endif
{
    if (argc > 1) {
        for (int i=1; i<argc;) {
            string option(argv[i++]);

            if (option.compare("--debug") == 0) {
                gLogLevel = 1;
            } else if (option.compare("--all") == 0) {
                gShowAll = true;
            } else if (option.compare("--phy") == 0) {
                gShowPhyKpi = true;
            } else {
                if (i<argc) {
                    parseMinorOptions(option, string(argv[i++]));
                } else {
                    showMinorUsage();
                    return false;
                }
            }
        }
    } else {

    }

#ifdef KPI_L3
    KpiService* kpiService = new KpiService("KPI Service");
#else
    KpiService* kpiService = new KpiService("KPI Service", qmss);
#endif
    kpiService->wait();

    return true;
}

// ----------------------------------------------
void KpiCommandParser::parseMinorOptions(string option, string value) {
    if (option.compare("-w") == 0) {
        gWriteOption = Util::s2i(value);
        cout << "gWriteOption = " << gWriteOption << endl;
    } else if (option.compare("-t") == 0) {
        gPeriod = Util::s2i(value) * 1000;
        cout << "gPeriod = " << gPeriod << endl;
    } else if (option.compare("-d") == 0) {
        gKpiDirectory = value;
        cout << "gKpiDirectory = " << gKpiDirectory << endl;
    } else {
        cout << "Invalid option: " << option << endl;
        exit(0);
    }
}

// ----------------------------------------------
void KpiCommandParser::showUsage() {
    cout << "Usage: kpi [-w writeOption] [-i serverIp] [-p port] [-t period] [--all] [--debug]" << endl << endl;

    cout << "Options: " << endl;
    cout << "-w : Write option "<< endl;
    cout << "     0 -> only display on console" << endl;
    cout << "     1 -> write to file and console" << endl;
    cout << "     2 -> write to socket and console" << endl;
    cout << "     3 -> write to file only" << endl;
    cout << "     default: " << KPI_DEFAULT_WRITE_OPTION << endl;
    cout << "-i : Server IP to receive KPI data, default: " << gServerIp << endl;
    cout << "-p : Server Port, default: " << gServerPort << endl;
    cout << "-t : The period (in seconds) for sending/displaying KPI data, default: " << gPeriod/1000 << endl;
    cout << "-d : The directory for kpi file saved to, default is /OAM/" << endl;
    cout << "--debug : enable debug log for this kpi tool" << endl;
    cout << "--all :   show all kpi details if display on console is enabled" << endl;
    // cout << "--phy :   show phy kpi" << endl;
    cout << endl;

    cout << "Example: kpi -w 2 -i 192.168.1.166 -p 50001 -t 1" << endl;
}

// ----------------------------------------------
void KpiCommandParser::showMinorUsage() {
    cout << "Usage: kpi [-w writeOption] [-d directory] [-t period] [--all] [--debug]" << endl << endl;

    cout << "Options: " << endl;
    cout << "-w : Write option "<< endl;
    cout << "     0 -> only display on console" << endl;
    cout << "     1 -> write to file and console" << endl;
    cout << "     3 -> write to file only" << endl;
    cout << "     4 -> write to file only (with final result to kpi.txt)" << endl;
    cout << "     default: " << KPI_DEFAULT_WRITE_OPTION << endl;
    cout << "-t : The period (in seconds) for sending/displaying KPI data, default: " << gPeriod/1000 << endl;
    cout << "-d : The directory for kpi file saved to, default is /OAM/" << endl;
    cout << "--debug : enable debug log for this kpi tool" << endl;
    cout << "--all :   show all kpi details if display on console is enabled" << endl;
    // cout << "--phy :   show phy kpi" << endl;
    cout << endl;

    cout << "Example: kpi -w 0 -t 1 --all" << endl;
}

// ----------------------------------------------
void KpiCommandParser::parseOptions(string option, string value) {
    if (option.compare("-w") == 0) {
        gWriteOption = Util::s2i(value);
        cout << "gWriteOption = " << gWriteOption << endl;
    } else if (option.compare("-i") == 0) {
        gServerIp = value;
        cout << "gServerIp = " << gServerIp << endl;
    } else if (option.compare("-p") == 0) {
        gServerPort = Util::s2i(value);
        cout << "gServerPort = " << gServerPort << endl;
    } else if (option.compare("-t") == 0) {
        gPeriod = Util::s2i(value) * 1000;
        cout << "gPeriod = " << gPeriod << endl;
    } else if (option.compare("-d") == 0) {
        gKpiDirectory = value;
        cout << "gKpiDirectory = " << gKpiDirectory << endl;
    } else {
        cout << "Invalid option: " << option << endl;
        exit(0);
    }
}




