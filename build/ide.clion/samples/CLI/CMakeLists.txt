# #################################################################################################
#  CMakeLists.txt - ALib Samples
#
#  Copyright 2013-2026 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  This CMake file just includes the generic CMake file for this sample project.
#  The whole purpose of this exercise is to have a dedicated project folder for
#  the Jetbrains CLion IDE, which gets filled by CLion with (hidden) configuration option files.
# #################################################################################################

cmake_minimum_required(VERSION 3.20)
project("ALib.CLI.Sample")


#
# All we do here is to include the "generic" sample CMake file
#
include( ../../../cmake/samples/CLI/CMakeLists.txt )

# Uncomment for debugging CMake
#ALibDumpCMakeVariables()
