#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>

#include "Qmss.h"
#include "MacInterface.h"
#include "Util.h"

#include "KpiService.h"

using namespace kpi;
using namespace std;

int gWriteOption = 0;
string gServerIp = "192.168.1.166";
unsigned short gServerPort = 50001;
int gPeriod = 10000;

UInt8 gLogLevel = 2;

void showUsage() {
    cout << "Usage: kpi [-w writeOption] [-i serverIp] [-p port] [-t period] [--debug]" << endl << endl;

    cout << "Options: " << endl;
    cout << "-w : Write option "<< endl;
    cout << "     0 -> only display on console" << endl;
    cout << "     1 -> write to file and console" << endl;
    cout << "     2 -> write to socket and console" << endl;
    cout << "     3 -> write to file only" << endl;
    cout << "     default: " << gWriteOption << endl;
    cout << "-i : Server IP to receive KPI data, default: " << gServerIp << endl;
    cout << "-p : Server Port, default: " << gServerPort << endl;
    cout << "-t : The period (in seconds) for sending/displaying KPI data, default: " << gPeriod/1000 << endl;
    cout << "--debug : enable debug log for this kpi tool" << endl;
    cout << endl;

    cout << "Example: kpi -w 2 -i 192.168.1.166 -p 50001 -t 1" << endl;
}

void parseOptions(string option, string value) {
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
    } else {
        cout << "Invalid option: " << option << endl;
        exit(0);
    }
}


int main(int argc, char* argv[]) {
    if (argc > 1) {
        for (int i=1; i<argc;) {
            string option(argv[i++]);

            if (option.compare("--debug") == 0) {
                gLogLevel = 1;
            } else {
                if (i<argc) {
                    parseOptions(option, string(argv[i++]));
                } else {
                    showUsage();
                    return 0;
                }
            }
        }
    } else {
        
    }

    KpiService* kpiService = new KpiService("KPI Service");
    kpiService->wait();

    return 0;
}


