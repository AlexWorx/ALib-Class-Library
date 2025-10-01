# #################################################################################################
#  CMakeLists.txt - ALib Samples
#
#  Copyright 2013-2025 A-Worx GmbH, Germany
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


# -------------------------------------------------------------------------------------------------
# General
# -------------------------------------------------------------------------------------------------
    cmake_minimum_required(VERSION 3.20) # For C++ 20 module compilation, V. 3.28 is needed
    project("ALib.Boxing.Sample")


## -------------------------------------------------------------------------------------------------
## include ALib/ALox base cmake file
## -------------------------------------------------------------------------------------------------

    # select module(s)
    list( APPEND  ALIB_BUILD             "BOXING;STRINGS"     )

    # invoke main CMake file
    include( ${CMAKE_CURRENT_LIST_DIR}/../../ALib.cmake )

# -------------------------------------------------------------------------------------------------
# Targets
# -------------------------------------------------------------------------------------------------

    ALibAddSharedLibrary()


    add_executable            ( Sample   ${ALIB_BASE_DIR}/src.samples/Boxing/sample.cpp  )
    ALibSetCompilerAndLinker  ( Sample )
    target_link_libraries     ( Sample   PRIVATE ALib_SharedLib  )


# -------------------------------------------------------------------------------------------------
# End
# -------------------------------------------------------------------------------------------------

# Uncomment for debugging CMake
#ALibDumpCMakeVariables()

