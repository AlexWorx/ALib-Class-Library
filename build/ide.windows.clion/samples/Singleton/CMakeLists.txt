# #################################################################################################
#  CMakeLists.txt - ALox Samples
#
#  Copyright 2015-2024 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  This CMake file just includes the main CMake file found in build/cmake.cpp/unittests.
#  The whole purpose of this excercise is to have a dedicated project folder for
#  the Jetbrains CLion IDE, which gets filled by CLion with (hidden) configuration option files.
#
#  More information is found in ALox documentation.
# #################################################################################################

cmake_minimum_required( VERSION 3.18.0 )
project("ALib.Singleton.Sample")


#
# For testing module distribution, create directory "/tmp/alib-module-test" and uncomment this:
#
#set( ALIB_SOURCE_COPY_TARGET_DIR      "/tmp/alib-module-test" )

#
# All we do here is to include the "generic" sample CMake file
#
include( ../../../cmake/samples/Singleton/CMakeLists.txt )
