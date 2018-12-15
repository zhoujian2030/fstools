#!/bin/sh

echo "build in platform " $PLATFORM

if [ $# == 1 ]; then
    libname="$1"
    echo "lib name is " $libname
else
    libname=libcm
fi

if [ "$PLATFORM" == "arm" ]; then
    echo "rename lib to" ${libname}_arm.a
    mv lib/${libname}.a lib/${libname}_arm.a
fi

