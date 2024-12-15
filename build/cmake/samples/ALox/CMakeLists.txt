# #################################################################################################
#  CMakeLists.txt - ALox Samples
#
#  Copyright 2015-2024 A-Worx GmbH, Germany
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

#! [DOXYGEN_CMAKE_INCLUDE_1]
    cmake_minimum_required( VERSION 3.18.0 )
    project("ALox.Samples")
#! [DOXYGEN_CMAKE_INCLUDE_1]


## -------------------------------------------------------------------------------------------------
## include ALib/ALox base cmake file
## -------------------------------------------------------------------------------------------------

#! [DOXYGEN_CMAKE_INCLUDE_2]
    list( APPEND  ALIB_DISTRIBUTION        "ALOX"       )
#! [DOXYGEN_CMAKE_INCLUDE_2]
#! [DOXYGEN_CMAKE_INCLUDE_3]
    list( APPEND  ALIB_COMPILER_FEATURES   "cxx_std_17" )
#! [DOXYGEN_CMAKE_INCLUDE_3]
#! [DOXYGEN_CMAKE_INCLUDE_4]
    include( ${CMAKE_CURRENT_LIST_DIR}/../../ALib.cmake )
#! [DOXYGEN_CMAKE_INCLUDE_4]
#! [DOXYGEN_CMAKE_INCLUDE_5]
    ALibAddSharedLibrary()
#! [DOXYGEN_CMAKE_INCLUDE_5]

# -------------------------------------------------------------------------------------------------
# Target
# -------------------------------------------------------------------------------------------------

#! [DOXYGEN_CMAKE_INCLUDE_6]
    add_executable            ( ALoxSamples   ${ALIB_BASE_DIR}/src/samples/ALox/sample.cpp  )
#! [DOXYGEN_CMAKE_INCLUDE_6]
#! [DOXYGEN_CMAKE_INCLUDE_7]
    ALibSetCompilerAndLinker  ( ALoxSamples )
#! [DOXYGEN_CMAKE_INCLUDE_7]
    target_compile_definitions(ALoxSamples PRIVATE ALIB_BASE_DIR="${ALIB_BASE_DIR}")
#! [DOXYGEN_CMAKE_INCLUDE_8]
    target_link_libraries     ( ALoxSamples   PRIVATE ALib_SharedLib  )
#! [DOXYGEN_CMAKE_INCLUDE_8]

# -------------------------------------------------------------------------------------------------
# End
# -------------------------------------------------------------------------------------------------

# uncomment for debug
#ALibDumpCMakeVariables()
