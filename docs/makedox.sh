#!/bin/bash
echo "Note: To successfully create the documentation, the unit tests have to be run upfront to generate input files."
echo "      And prior to this, the CLI Sample application has to be compiled."
echo "Note: Most of the time, doxygen introduces new strange bugs with every new version. "
echo "      Thus, we have to choose the doxygen version carefully."

### Cleaning before build When uncomment, also uncomment the echo message below ###
# echo "!!! Cleaning before build. This is for testing.  !!!"
# makeclean.sh

/opt/doxygen/1.8.16.build/bin/doxygen doxygen/doxyfile.ini
#/opt/doxygen/1.8.17.build/bin/doxygen doxygen/doxyfile.ini
#/opt/doxygen/1.8.18.build/bin/doxygen doxygen/doxyfile.ini
#/opt/doxygen/1.8.20.build/bin/doxygen doxygen/doxyfile.ini
#/opt/doxygen/1.9.0.build/bin/doxygen doxygen/doxyfile.ini
#/opt/doxygen/1.9.1.build/bin/doxygen doxygen/doxyfile.ini
#/opt/doxygen/1.9.3.build/bin/doxygen doxygen/doxyfile.ini
#/opt/doxygen/1.9.5.build/bin/doxygen doxygen/doxyfile.ini
#/opt/doxygen/1.9.6.build/bin/doxygen doxygen/doxyfile.ini
#/opt/doxygen/1.9.8.build/bin/doxygen doxygen/doxyfile.ini
#/opt/doxygen/1.10.0.240303.git.build/bin/doxygen doxygen/doxyfile.ini


#doxygen doxygen/doxyfile.ini

# echo '*** Copying other additional resources ***'
cp resources/* ../html
