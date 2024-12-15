# #################################################################################################
#  CMakeLists.txt - ALox Tools: DoxygenGraphTemplateInstanceRemover
#
#  Copyright 2015-2024 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#   More information is found in ALib documentation.
# ##################################################################################################

# -------------------------------------------------------------------------------------------------
# Prior to setting CMake project
# -------------------------------------------------------------------------------------------------

# -------------------------------------------------------------------------------------------------
# General
# -------------------------------------------------------------------------------------------------

    cmake_minimum_required( VERSION 3.20.0 )
    project( "DoxygenGraphTemplateInstanceRemover" )

    # this defaults to a release version!
    if ( "${CMAKE_BUILD_TYPE}" STREQUAL ""    )
        MESSAGE( STATUS "Default build type 'Release' chosen" )
        set( CMAKE_BUILD_TYPE "Release" )
    endif()

## -------------------------------------------------------------------------------------------------
## find libraries
## -------------------------------------------------------------------------------------------------

    # Multithreading
    find_package(Threads REQUIRED)

## -------------------------------------------------------------------------------------------------
## include ALib/ALox base cmake file
## -------------------------------------------------------------------------------------------------

    list( APPEND  ALIB_DISTRIBUTION  "ALOX"         )
    list( APPEND  ALIB_DISTRIBUTION  "FILES"        )
    list( APPEND  ALIB_DISTRIBUTION  "EXPRESSIONS"  )
    include( ${CMAKE_CURRENT_LIST_DIR}/../../build/cmake/ALib.cmake )


# -------------------------------------------------------------------------------------------------
# sources
# -------------------------------------------------------------------------------------------------
aux_source_directory(. SRC_LIST)


# -------------------------------------------------------------------------------------------------
# The target
# -------------------------------------------------------------------------------------------------
    ALibAddStaticLibrary()

    add_executable( dot ${SRC_LIST})
    ALibSetCompilerAndLinker( dot )

    target_compile_definitions( dot   PUBLIC   ${ALIB_SYMBOLS} )

    target_link_libraries     ( dot   PRIVATE ALib_StaticLib        )

    set( EXECUTABLE_OUTPUT_PATH       ${ALIB_BASE_DIR}/docs/doxygen)

