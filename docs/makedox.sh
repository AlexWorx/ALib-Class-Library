#!/bin/bash

echo "Note: To successfully create the documentation, the unit tests have to be run upfront to generate input files."

### Cleaning before build When uncomment, also uncomment the echo message below ###
# echo "!!! Cleaning before build. This is for testing.  !!!"
# makeclean.sh

doxygen doxygen/doxyfile.ini

#echo "*** Copying other additional resources ***"
cp resources/* ../html
