echo "Compiling all versions. Output is found in'compilation_output.txt'"

date

./batch_build_cpp20.sh*                   >  compilation_output.txt  2>> compilation_output.txt
./batch_build_cpp20_release.sh*           >> compilation_output.txt  2>> compilation_output.txt
./batch_build_cpp23.sh*                   >> compilation_output.txt  2>> compilation_output.txt
./batch_build_32bit.sh*                   >> compilation_output.txt  2>> compilation_output.txt
./batch_build_32bit_release.sh*           >> compilation_output.txt  2>> compilation_output.txt
./batch_build_reversed_flags_release.sh*  >> compilation_output.txt  2>> compilation_output.txt
./batch_build_reversed_flags.sh*          >> compilation_output.txt  2>> compilation_output.txt
./batch_build_wchar16.sh*                 >> compilation_output.txt  2>> compilation_output.txt
./batch_build_singlethreaded_release.sh*  >> compilation_output.txt  2>> compilation_output.txt
./batch_build_singlethreaded.sh*          >> compilation_output.txt  2>> compilation_output.txt

date
echo "Done! (Compiling all versions.)"


