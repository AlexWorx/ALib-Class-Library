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
echo "##########################       C++20            ###########################"
echo "n#############################################################################"
echo "n#############################################################################"
echo ""


echo "n#############################################################################"
echo "############################     Single Modules    ###########################"
echo "n#############################################################################"

echo "\n\n############################  ALOX    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=ALOX"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=BITBUFFER"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  CAMP    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=CAMP"  ../../cmake/moduletests
make -j

echo "\n\n############################  CONTAINERS    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=CONTAINERS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CLI    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=CLI"  ../../cmake/moduletests
make -j

echo "\n\n############################  ENUMRECORDS    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=ENUMRECORDS"  ../../cmake/moduletests
make -j

echo "\n\n############################  EXCEPTIONS    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=EXCEPTIONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  EXPRESSIONS    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=EXPRESSIONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  FILES    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=FILES"  ../../cmake/moduletests
make -j

echo "\n\n############################  FORMAT    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=FORMAT"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=MONOMEM"  ../../cmake/moduletests
make -j

echo "\n\n############################  RESOURCES    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=RESOURCES"  ../../cmake/moduletests
make -j

echo "\n\n############################  SINGLETONS    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=SINGLETONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  STRINGS    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  SYSTEM    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=SYSTEM"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADMODEL    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=THREADMODEL"  ../../cmake/moduletests
make -j

echo "\n\n############################  VARIABLES    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=VARIABLES"  ../../cmake/moduletests
make -j


echo ""
echo ""
echo "n#############################################################################"
echo "############################     Mixed Modules    ############################"
echo "n#############################################################################"

echo "\n\n############################  BITBUFFER;SINGLETONS    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=BITBUFFER;SINGLETONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER;BOXING    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=BITBUFFER;BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER;FILES    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=BITBUFFER;FILES"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING;MONOMEM    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=BOXING;MONOMEM"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING;STRINGS    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=BOXING;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  EXCEPTIONS;VARIABLES    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=EXCEPTIONS;VARIABLES"  ../../cmake/moduletests
make -j

echo "\n\n############################  FILES;THREADMODEL    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=FILES;THREADMODEL"  ../../cmake/moduletests
make -j

echo "\n\n############################  FORMAT;THREADMODEL    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=FORMAT;THREADMODEL"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;CONTAINERS    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=MONOMEM;CONTAINERS"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;STRINGS    ############################"
rm -r -f *
cmake "-DCMAKE_BUILD_TYPE=Release" "-DALIB_FEAT_BOOST_REGEX=1"  "-DALIB_BUILD=MONOMEM;STRINGS"  ../../cmake/moduletests
make -j

