#!/bin/bash
###########################################################
### use lcov to create coverage information for alox.lib
###########################################################

UNITTESTDIR="../../_builds_/cmake_qtc/unittests-Desktop-Debug/CMakeFiles/ALib_ALox_UT.dir/"
OUTPUTDIR="../../_builds_/cmake_qtc/unittests-Desktop-Debug"
OUTPUTFILE="ALib_ALox_UT.coverage.info"

if [ ! -e $UNITTESTDIR ] ; then
    echo "Directory '"$UNITTESTDIR"' not found."
    echo "The project has to be built first."
    echo "Exiting(1)"
    exit 1
fi

if [ ! -e $OUTPUTDIR ] ; then
    echo "Directory '"$OUTPUTDIR"' not found. Exiting(1)"
    echo "The project has to be built first."
    echo "Exiting(1)"
        exit 1
fi

lcov --capture --directory $UNITTESTDIR --output-file $OUTPUTDIR"/"$OUTPUTFILE
genhtml $OUTPUTDIR"/"$OUTPUTFILE --output-directory $OUTPUTDIR"/"$OUTPUTFILE"_COVERAGE_HTML"

echo

if [ -e $OUTPUTDIR"/"$OUTPUTFILE"_COVERAGE_HTML/index.html" ] ; then
    echo "Coverage html files generated in: "$OUTPUTDIR"/"$OUTPUTFILE"_COVERAGE_HTML"
else
    echo "Some error occured. Nothing generated :-("
    echo "Debug version has to be built with gcc and compiled and linked with '--coverage' flag set."
    echo "The software has to be executed prior to running this script."
fi
