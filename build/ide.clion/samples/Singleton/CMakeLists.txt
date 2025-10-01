# #################################################################################################
#  CMakeLists.txt - ALib Samples
#
#  Copyright 2013-2025 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  This CMake file just includes the generic CMake file for this sample project.
#  The whole purpose of this exercise is to have a dedicated project folder for
#  the Jetbrains CLion IDE, which gets filled by CLion with (hidden) configuration option files.
# #################################################################################################

cmake_minimum_required(VERSION 3.20) # For C++ 20 module compilation, V. 3.28 is needed
project("ALib.Singleton.Sample")


# For testing module permutations, create directory "/tmp/alib-module-test" and uncomment this:
#
#set( ALIB_SOURCE_COPY_TARGET_DIR      "/tmp/alib-module-test" )

#
# All we do here is to include the "generic" sample CMake file
#
include( ../../../cmake/samples/Singleton/CMakeLists.txt )

# Uncomment for debugging CMake
#ALibDumpCMakeVariables()
