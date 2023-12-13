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
echo "############################ With Threads  Release ###########################"
echo "n#############################################################################"
echo "n#############################################################################"
echo ""


echo "n#############################################################################"
echo "############################     Single Modules    ###########################"
echo "n#############################################################################"

echo "\n\n############################  MONOMEM    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=MONOMEM;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=BITBUFFER;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  SINGLETONS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=SINGLETONS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  CHARACTERS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=CHARACTERS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  ENUMS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=ENUMS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  TIME    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=TIME;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=BOXING;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=STRINGS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  RESOURCES    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=RESOURCES;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=THREADS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  TEXT    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=TEXT;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  RESULTS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=RESULTS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  SYSTEM    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=SYSTEM;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  CONFIGURATION    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=CONFIGURATION;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  ALOX    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=ALOX;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  EXPRESSIONS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=EXPRESSIONS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  CLI    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=CLI;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j



echo ""
echo ""
echo "n#############################################################################"
echo "############################     Mixed Modules    ############################"
echo "n#############################################################################"

echo "\n\n############################  SINGLETONS;TIME    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=SINGLETONS;TIME;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=MONOMEM;STRINGS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=MONOMEM;BOXING;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=BITBUFFER;BOXING;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=THREADS;STRINGS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=THREADS;BOXING;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=MONOMEM;STRINGS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=MONOMEM;BOXING;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER;SINGLETONS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=BITBUFFER;SINGLETONS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=BOXING;STRINGS;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  SINGLETONS;TIME    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=SINGLETONS;TIME;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

echo "\n\n############################  RESOURCES;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=RESOURCES;BOXING;THREADS" "-DCMAKE_BUILD_TYPE=Release"  ../../cmake/moduletests
make -j

