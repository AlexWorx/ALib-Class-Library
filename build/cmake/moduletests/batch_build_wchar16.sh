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
echo "##########################       wchar16            ###########################"
echo "n#############################################################################"
echo "n#############################################################################"
echo ""


echo "n#############################################################################"
echo "############################     Single Modules    ###########################"
echo "n#############################################################################"

echo "\n\n############################  MONOMEM    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=MONOMEM"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=BITBUFFER"  ../../cmake/moduletests
make -j

echo "\n\n############################  SINGLETONS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=SINGLETONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CHARACTERS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=CHARACTERS"  ../../cmake/moduletests
make -j

echo "\n\n############################  ENUMS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=ENUMS"  ../../cmake/moduletests
make -j

echo "\n\n############################  TIME    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=TIME"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  STRINGS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=THREADS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CAMP    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=CAMP"  ../../cmake/moduletests
make -j

echo "\n\n############################  CONFIGURATION    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=CONFIGURATION"  ../../cmake/moduletests
make -j

echo "\n\n############################  ALOX    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=ALOX"  ../../cmake/moduletests
make -j

echo "\n\n############################  EXPRESSIONS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=EXPRESSIONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CLI    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=CLI"  ../../cmake/moduletests
make -j

#--------- wchar 4 ----------

echo "\n\n############################  ALOX    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=4"  "-DALIB_DISTRIBUTION=ALOX"  ../../cmake/moduletests
make -j

echo "\n\n############################  EXPRESSIONS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=4"  "-DALIB_DISTRIBUTION=EXPRESSIONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  CLI    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=4"  "-DALIB_DISTRIBUTION=CLI"  ../../cmake/moduletests
make -j

#--------- release wchar 2 ----------

echo "\n\n############################  ALOX    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=ALOX" "-DCMAKE_BUILD_TYPE=Release" ../../cmake/moduletests
make -j

echo "\n\n############################  EXPRESSIONS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=EXPRESSIONS" "-DCMAKE_BUILD_TYPE=Release" ../../cmake/moduletests
make -j

echo "\n\n############################  CLI    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=CLI" "-DCMAKE_BUILD_TYPE=Release" ../../cmake/moduletests
make -j

#--------- release wchar 4 ----------

echo "\n\n############################  ALOX    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=4"  "-DALIB_DISTRIBUTION=ALOX" "-DCMAKE_BUILD_TYPE=Release" ../../cmake/moduletests
make -j

echo "\n\n############################  EXPRESSIONS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=4"  "-DALIB_DISTRIBUTION=EXPRESSIONS" "-DCMAKE_BUILD_TYPE=Release" ../../cmake/moduletests
make -j

echo "\n\n############################  CLI    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=4"  "-DALIB_DISTRIBUTION=CLI" "-DCMAKE_BUILD_TYPE=Release" ../../cmake/moduletests
make -j




echo ""
echo ""
echo "n#############################################################################"
echo "############################     Mixed Modules    ############################"
echo "n#############################################################################"

echo "\n\n############################  SINGLETONS;TIME    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=SINGLETONS;TIME"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=MONOMEM;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;BOXING    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=MONOMEM;BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER;BOXING    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=BITBUFFER;BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=THREADS;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  THREADS;BOXING    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=THREADS;BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=MONOMEM;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  MONOMEM;BOXING    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=MONOMEM;BOXING"  ../../cmake/moduletests
make -j

echo "\n\n############################  BITBUFFER;SINGLETONS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=BITBUFFER;SINGLETONS"  ../../cmake/moduletests
make -j

echo "\n\n############################  BOXING;STRINGS    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=BOXING;STRINGS"  ../../cmake/moduletests
make -j

echo "\n\n############################  SINGLETONS;TIME    ############################"
rm -r -f *
cmake "-DALIB_CHARACTERS_WIDE=1" "-DALIB_CHARACTERS_SIZEOF_WCHAR=2"  "-DALIB_DISTRIBUTION=SINGLETONS;TIME"  ../../cmake/moduletests
make -j
