#!/bin/bash
echo "Note: To successfully create the documentation, the following has to be done"
echo "      1. The CLI Sample application has to be compiled."
echo "      2. The unit tests have to be run with the right compilation options."
echo "      3. ALox Sample has to be run once (generates an INI-file in doxygen/generated folder."

### Cleaning before build When uncomment, also uncomment the echo message below ###
# echo "!!! Cleaning before build. This is for testing.  !!!"
#makeclean.sh

# prior used doxygen version
Doxygen="/opt/doxygen/current.git3/build/bin/doxygen"
Doxygen="doxygen"


echo "Doxygen version: "
$Doxygen --version
$Doxygen doxygen/doxyfile.ini
#$Doxygen doxygen/doxyfile.ini  2>&1 | grep -i ".hpp"

echo '*** Copying other additional resources ***'
cp resources/*   /tmp/alib_html/

echo '*** Patching menu.js ***'
sed -i 's/).smartmenus()/).smartmenus({markCurrentItem:true})/g' /tmp/alib_html/menu.js

#doxygen/dot postprocess /tmp/alib_html/
