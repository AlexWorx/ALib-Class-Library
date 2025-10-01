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

project("ALox.Samples")
cmake_minimum_required(VERSION 3.20) # For C++ 20 module compilation, V. 3.28 is needed

# -------------------------------------------------------------------------------------------------
# Settings prior to including the generic cmake CMake file
# -------------------------------------------------------------------------------------------------


# -------------------------------------------------------------------------------------------------
# Include the generic cmake CMake file
# -------------------------------------------------------------------------------------------------

include( ../../cmake/samples/ALox/CMakeLists.txt )
