# #################################################################################################
#  CMakeLists.txt - ALox Samples
#
#  Copyright 2015-2019 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  How to use:
#   1. Enter subdirectory 'debug'   and invoke:  `cmake ..`
#   2. Enter subdirectory 'release' and invoke:  `cmake -DCMAKE_BUILD_TYPE=Release ..`
#   3. Invoke `make` within any of these directories.
#   More information is found in ALox documentation.
# #################################################################################################

# -------------------------------------------------------------------------------------------------
# Prior to setting CMake project
# -------------------------------------------------------------------------------------------------

    ### uncomment to force cotire on clean cmake built
    # set( ALIB_CMAKE_COTIRE  "On" )

    # list( APPEND  ALIB_COMPILER_FEATURES   "cxx_std_11" )


# -------------------------------------------------------------------------------------------------
# General
# -------------------------------------------------------------------------------------------------

    project("ALib.Configuration.Sample")

    cmake_minimum_required( VERSION 3.3.0 )


## -------------------------------------------------------------------------------------------------
## include ALib/ALox base cmake file
## -------------------------------------------------------------------------------------------------

    # select module(s)
    list( APPEND  ALIB_MODULES             "CONFIGURATION" )

    # invoke main CMake file
    include( ${CMAKE_CURRENT_LIST_DIR}/../../ALib.cmake )

# -------------------------------------------------------------------------------------------------
# Targets
# -------------------------------------------------------------------------------------------------

    ALibAddSharedLibrary()


    add_executable            ( Sample   ${ALIB_BASE_DIR}/src/samples/Configuration/sample.cpp  )
    ALibSetCompilerAndLinker  ( Sample )
    ALibSetCotire             ( Sample )
    target_link_libraries     ( Sample   ALib_SharedLib  )


# -------------------------------------------------------------------------------------------------
# End
# -------------------------------------------------------------------------------------------------

# uncomment for debug
#ALibDumpCMakeStatus()








