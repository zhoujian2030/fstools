#!/bin/sh

export PLATFORM=arm
export PATH=$PATH:/omap/eldk/usr/bin/
export PLATFORM_CXX=arm-linux-g++
export PLATFORM_CC=arm-linux-gcc
export PLATFORM_AR=arm-linux-ar
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/include

export GTEST_SUPPORT=FALSE
export SCTP_SUPPORT=FALSE
export DB_SUPPORT=FALSE

# export NEW_QMSS_DRIVER=FALSE
# export NEW_QMSS_DRIVER=TDD_TRUE
#export NEW_QMSS_DRIVER=FDD_TRUE

export PROJBASE=$(cd `dirname "${BASH_SOURCE[0]}"` && pwd)
echo "Current directory: $PROJBASE"

COMMAND=$1

if [[ $COMMAND = "clean" ]]; then
    make clean
elif [[ $COMMAND = "install" ]]; then
    make install
else
    if [[ $COMMAND = "tdd" ]]; then
        export NEW_QMSS_DRIVER=TDD_TRUE
        make
        cp kpi/obj/kpi /mnt/hgfs/c/share/tddkpi
        cp cli/obj/cli /mnt/hgfs/c/share/tddcli
    elif [[ $COMMAND = "fdd" ]]; then
        export NEW_QMSS_DRIVER=FDD_TRUE
        make
        cp kpi/obj/kpi /mnt/hgfs/c/share/fddkpi
        cp cli/obj/cli /mnt/hgfs/c/share/fddcli
    else
        export NEW_QMSS_DRIVER=FALSE
        make
        cp kpi/obj/kpi /mnt/hgfs/c/share/kpi_mqbs
        # cp tst/obj/tst /mnt/hgfs/c/share/
        cp cli/obj/cli /mnt/hgfs/c/share/cli_mqbs
        #cp clibe/obj/cli-be /mnt/hgfs/c/share/
    fi

fi

