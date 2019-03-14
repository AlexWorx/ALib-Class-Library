cd ../../

if [ ! -d _builds_ ]
then
    echo "can not find _builds_ directory "
    exit -1
fi

cd _builds_
mkdir moduletests_batch
cd    moduletests_batch


echo "n#############################################################################"
echo "n#############################################################################"
echo "############################          DEBUG        ###########################"
echo "n#############################################################################"
echo "n#############################################################################"
echo ""


echo "n#############################################################################"
echo "############################     Single Modules    ###########################"
echo "n#############################################################################"

echo "\n\n############################  MEMORY    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=MEMORY"  ../../cmake/moduletests
make -j

echo "\n\n############################  SINGLETONS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=SINGLETONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CHARACTERS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=CHARACTERS"  ../../cmake/moduletests
make -j

echo "\n\n############################  ENUMS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=ENUMS"  ../../cmake/moduletests
make -j

echo "\n\n############################  TIME    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=TIME"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  RESOURCES    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=RESOURCES"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  STRINGFORMAT    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=STRINGFORMAT"  ../../cmake/moduletests
make -j

echo "\n\n############################  RESULTS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=RESULTS"  ../../cmake/moduletests
make -j

echo "\n\n############################  SYSTEM    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=SYSTEM"  ../../cmake/moduletests
make -j

echo "\n\n############################  CONFIGURATION    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=CONFIGURATION"  ../../cmake/moduletests
make -j

echo "\n\n############################  ALOX    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=ALOX"  ../../cmake/moduletests
make -j

echo "\n\n############################  EXPRESSIONS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=EXPRESSIONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CLI    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=CLI"  ../../cmake/moduletests
make -j



echo ""
echo ""
echo "n#############################################################################"
echo "############################     Mixed Modules    ############################"
echo "n#############################################################################"

echo "\n\n############################  SINGLETONS;TIME    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=SINGLETONS;TIME"  ../../cmake/moduletests
make -j

echo "\n\n############################  MEMORY;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=MEMORY;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  MEMORY;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=MEMORY;BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=THREADS;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=THREADS;BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  MEMORY;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=BOXING;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  MEMORY;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=BOXING;STRINGS"  ../../cmake/moduletests
make -j


echo "\n\n############################  BOXING;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=BOXING;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  SINGLETONS;TIME    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=SINGLETONS;TIME"  ../../cmake/moduletests
make -j

echo "\n\n############################  RESOURCES;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX_ON=1" "-DALIB_MODULES=RESOURCES;BOXING"  ../../cmake/moduletests
make -j

