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

string gServerIp = "192.168.1.166";
unsigned short gServerPort = 50001;
int gPeriod = 10000;

void showUsage() {
    cout << "Usage: kpi [-i serverIp] [-p port] [-t period]" << endl << endl;

    cout << "Options: " << endl;
    cout << "-i : Server IP to receive KPI data, default: " << gServerIp << endl;
    cout << "-p : Server Port, default: " << gServerPort << endl;
    cout << "-t : The period (in seconds) for sending/displaying KPI data, default: " << gPeriod/1000 << endl;
    cout << endl;

    cout << "Example: kpi -i 192.168.1.166 -p 50001 -t 10" << endl;
}

void parseOptions(string option, string value) {
    if (option.compare("-i") == 0) {
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
            if (i<argc) {
                parseOptions(option, string(argv[i++]));
            } else {
                showUsage();
                return 0;
            }
        }
    }

    KpiService* kpiService = new KpiService("KPI Service");
    kpiService->wait();

    return 0;
}


