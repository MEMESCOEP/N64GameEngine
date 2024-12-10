#!/bin/bash
echo Compiling...
make clean
make V=1

if [ $? -ne 0 ]; then
    echo
    echo Compilation failed!
    exit
fi

echo
echo Running Ares...
$PWD/../Utilities/Ares/ares --system Nintendo 64 $PWD/EngineTest.z64
