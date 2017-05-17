#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "Qmss.h"
#include "TstService.h"

using namespace tst;

int main(int argc, char* argv[]) {
    printf("main thread running\n");

    TstService* tstService = new TstService("TST Service");
    tstService->wait();

    return 0;
}


