cd ../../

if [ ! -d _builds_ ]
then
    echo "cannot find _builds_ directory "
    exit -1
fi

cd _builds_
mkdir moduletests_batch
cd    moduletests_batch



echo "n#############################################################################"
echo "n#############################################################################"
echo "############################      With Threads     ###########################"
echo "n#############################################################################"
echo "n#############################################################################"
echo ""


echo "n#############################################################################"
echo "############################     Single Modules    ###########################"
echo "n#############################################################################"

echo "\n\n############################  MONOMEM    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=MONOMEM;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=BITBUFFER;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  SINGLETONS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=SINGLETONS;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CHARACTERS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=CHARACTERS;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  ENUMS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=ENUMS;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  TIME    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=TIME;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=BOXING;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=STRINGS;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=THREADS;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CAMP    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=CAMP;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CONFIGURATION    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=CONFIGURATION;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  ALOX    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=ALOX;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  EXPRESSIONS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=EXPRESSIONS;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CLI    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=CLI;THREADS"  ../../cmake/moduletests
make -j



echo ""
echo ""
echo "n#############################################################################"
echo "############################     Mixed Modules    ############################"
echo "n#############################################################################"

echo "\n\n############################  SINGLETONS;TIME    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=SINGLETONS;TIME;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=MONOMEM;STRINGS;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=MONOMEM;BOXING;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=BITBUFFER;BOXING;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=THREADS;STRINGS;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=THREADS;BOXING;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=BOXING;STRINGS;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;BOXING    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=MONOMEM;BOXING;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=BOXING;STRINGS;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER;SINGLETONS    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=BITBUFFER;SINGLETONS;THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  SINGLETONS;TIME    ############################"
rm -r -f *
cmake "-DALIB_FEAT_BOOST_REGEX=1" "-DALIB_DISTRIBUTION=SINGLETONS;TIME;THREADS"  ../../cmake/moduletests
make -j
