# #################################################################################################
#  CMakeLists.txt - ALox Samples
#
#  Copyright 2015-2023 A-Worx GmbH, Germany
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

#! [DOXYGEN_CMAKE_INCLUDE_8]
    target_link_libraries     ( ALoxSamples   PRIVATE ALib_SharedLib  )
#! [DOXYGEN_CMAKE_INCLUDE_8]

# -------------------------------------------------------------------------------------------------
# Precompiled headers with 'cotire' CMake script.
# For more Info see 'ALox.cmake' which downloads the script and the cotire website at
# https://github.com/sakra/cotire/
# -------------------------------------------------------------------------------------------------
    if( ${ALIB_CMAKE_COTIRE} )
        set_target_properties( ALoxSamples     PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT   "${ALIB_SOURCE_DIR}/alib/alib_precompile.hpp" )
        set_target_properties( ALoxSamples     PROPERTIES COTIRE_UNITY_SOURCE_MAXIMUM_NUMBER_OF_INCLUDES "-j" )

        cotire( ALoxSamples )

        #--- dump cotire properties ---
        #set ( _dumppname "ALoxSamples" )
        #get_target_property( _unitySource        ${_dumppname}    COTIRE_CXX_UNITY_SOURCE      )
        #get_target_property( _prefixHeader       ${_dumppname}    COTIRE_CXX_PREFIX_HEADER     )
        #get_target_property( _precompiledHeader  ${_dumppname}    COTIRE_CXX_PRECOMPILED_HEADER)
        #get_target_property( _unityTargetName    ${_dumppname}    COTIRE_UNITY_TARGET_NAME     )

        # message(STATUS "${_dumppname} unity source:       ${_unitySource}")
        # message(STATUS "${_dumppname} prefix header:      ${_prefixHeader}")
        # message(STATUS "${_dumppname} precompiled header: ${_precompiledHeader}")
        # message(STATUS "${_dumppname} unity target:       ${_unityTargetName}")

    endif()

# -------------------------------------------------------------------------------------------------
# End
# -------------------------------------------------------------------------------------------------

# uncomment for debug
#ALibDumpCMakeVariables()
