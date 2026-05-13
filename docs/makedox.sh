#!/bin/bash
echo "Note: To successfully create the documentation, the following has to be done"
echo "      1. The CLI Sample application has to be compiled."
echo "      2. The unit tests have to be run with the right compilation options."
echo "      3. ALox Sample has to be run once (generates an INI-file in doxygen/generated folder."

### Cleaning before build When uncomment, also uncomment the echo message below ###
# echo "!!! Cleaning before build. This is for testing.  !!!"
#makeclean.sh

##--------------------------  Doxygen  --------------------------
#Doxygen=/opt/doxygen/doxygen-1.14.0/bin/doxygen
#Doxygen=/opt/doxygen/doxygen-1.15.0/bin/doxygen
#Doxygen=/opt/doxygen/doxygen-1.16.0/bin/doxygen
#Doxygen=/opt/doxygen/doxygen-1.16.1/bin/doxygen
#Doxygen=/opt/doxygen/doxygen.git/_builds_/bin/doxygen
Doxygen="doxygen"
echo "Doxygen version: "
$Doxygen --version
$Doxygen /tmp/alib_doxyfile.ini

##--------------------------  DoxygenXLinks --------------------------
#DXL=/tmp/_builds_/doxygenxlinks_clang_release/DoxygenXLinks
#DXL=/tmp/_builds_/doxygenxlinks_clang_debug/DoxygenXLinks
DXL=/hub/projects/_MORE/tools/DoxygenXLinks.260423

$DXL   --xdryrun --xdoxyfy=../srccpy /tmp/alib_doxyfile.ini --STATISTICS=off --WORKING_DIR=$(dirname "$0")
#$DXL --xdryrun /tmp/alib_doxyfile.ini --STATISTICS=on --WORKING_DIR=$(dirname "$0")
#/usr/bin/time -v $DXL --xdryrun /tmp/alib_doxyfile.ini --STATISTICS=on --WORKING_DIR=$(dirname "$0")

##--------------------------  Final Steps  --------------------------
echo
echo '*** Copying other additional resources ***'
cp resources/*   /tmp/alib_html/

echo '*** Patching menu.js ***'
sed -i 's/).smartmenus()/).smartmenus({markCurrentItem:true})/g' /tmp/alib_html/menu.js

