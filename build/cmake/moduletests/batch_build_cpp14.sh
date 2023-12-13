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
echo "##########################       C++ 14            ###########################"
echo "n#############################################################################"
echo "n#############################################################################"
echo ""


echo "n#############################################################################"
echo "############################     Single Modules    ###########################"
echo "n#############################################################################"

echo "\n\n############################  MONOMEM    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=MONOMEM"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=BITBUFFER"  ../../cmake/moduletests
make -j

echo "\n\n############################  SINGLETONS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=SINGLETONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CHARACTERS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=CHARACTERS"  ../../cmake/moduletests
make -j

echo "\n\n############################  ENUMS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=ENUMS"  ../../cmake/moduletests
make -j

echo "\n\n############################  TIME    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=TIME"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  STRINGS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  RESOURCES    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=RESOURCES"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  TEXT    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=TEXT"  ../../cmake/moduletests
make -j

echo "\n\n############################  RESULTS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=RESULTS"  ../../cmake/moduletests
make -j

echo "\n\n############################  SYSTEM    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=SYSTEM"  ../../cmake/moduletests
make -j

echo "\n\n############################  CONFIGURATION    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=CONFIGURATION"  ../../cmake/moduletests
make -j

echo "\n\n############################  ALOX    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=ALOX"  ../../cmake/moduletests
make -j

echo "\n\n############################  EXPRESSIONS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=EXPRESSIONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CLI    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=CLI"  ../../cmake/moduletests
make -j



echo ""
echo ""
echo "n#############################################################################"
echo "############################     Mixed Modules    ############################"
echo "n#############################################################################"

echo "\n\n############################  SINGLETONS;TIME    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=SINGLETONS;TIME"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=MONOMEM;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;BOXING    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=MONOMEM;BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER;BOXING    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=BITBUFFER;BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=THREADS;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS;BOXING    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=THREADS;BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=BOXING;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER;SINGLETONS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=BITBUFFER;SINGLETONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=BOXING;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  SINGLETONS;TIME    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=SINGLETONS;TIME"  ../../cmake/moduletests
make -j

echo "\n\n############################  RESOURCES;BOXING    ############################"
rm -r -f *
cmake "-DALIB_COMPILER_FEATURES=cxx_std_14"  "-DALIB_DISTRIBUTION=RESOURCES;BOXING"  ../../cmake/moduletests
make -j

