echo "Compiling all versions. Output is found in 'compilation_output.txt'"

date

./batch_build_reversed_flags.sh         >  compilation_output.txt  2>> compilation_output.txt
./batch_build_reversed_flags_release.sh >> compilation_output.txt  2>> compilation_output.txt
./batch_build_debug.sh                  >> compilation_output.txt  2>> compilation_output.txt
./batch_build_wchar16.sh                >> compilation_output.txt  2>> compilation_output.txt
./batch_build_debug.sh                  >> compilation_output.txt  2>> compilation_output.txt
./batch_build_release.sh                >> compilation_output.txt  2>> compilation_output.txt
./batch_build_cpp11.sh                  >> compilation_output.txt  2>> compilation_output.txt
./batch_build_cpp14.sh                  >> compilation_output.txt  2>> compilation_output.txt
./batch_build_32bit.sh                  >> compilation_output.txt  2>> compilation_output.txt
./batch_build_32bit_release.sh          >> compilation_output.txt  2>> compilation_output.txt

date
echo "Done! (Compiling all versions.)"


