# #################################################################################################
#  CMakeLists.txt - ALox Samples
#
#  Copyright 2015-2018 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  This CMake file just includes the main CMake file found in build/cmake.cpp/unittests.
#  The whole purpose of this excercise is to have a dedicated project folder for
#  the Jetbrains CLion IDE, which gets filled by CLion with (hidden) configuration option files.
#
#  More information is found in ALox documentation.
# #################################################################################################

cmake_minimum_required( VERSION 3.1.0 )

# -------------------------------------------------------------------------------------------------
# Settings prior to including the generic cmake CMake file
# -------------------------------------------------------------------------------------------------

    ### uncomment to force cotire on clean cmake built
    #set( ALIB_CMAKE_COTIRE   "On" )


# -------------------------------------------------------------------------------------------------
# Includine the generic cmake CMake file
# -------------------------------------------------------------------------------------------------

    include( ../../cmake.cpp/alox.samples/CMakeLists.txt )




