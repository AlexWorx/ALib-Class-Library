# #################################################################################################
#  CMakeLists.txt - ALox Tools: DoxygenGraphTemplateInstanceRemover
#
#  Copyright 2015-2019 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  How to use:
#   1. Enter subdirectory 'debug'   and invoke:  `cmake ..`
#   2. Enter subdirectory 'release' and invoke:  `cmake -DCMAKE_BUILD_TYPE=Release ..`
#   3. Invoke `make` within any of these directories.
#   More information is found in ALox documentation.
# ##################################################################################################

# -------------------------------------------------------------------------------------------------
# General
# -------------------------------------------------------------------------------------------------

    project( "ResourceExport" )
    cmake_minimum_required( VERSION 3.3.0 )

## -------------------------------------------------------------------------------------------------
## include ALib/ALox base cmake file
## -------------------------------------------------------------------------------------------------

    include( ${CMAKE_CURRENT_LIST_DIR}/../../build/cmake/ALib.cmake )


# -------------------------------------------------------------------------------------------------
# sources
# -------------------------------------------------------------------------------------------------
    LIST( APPEND   SOURCES      main.cpp   )


# -------------------------------------------------------------------------------------------------
# The target
# -------------------------------------------------------------------------------------------------
    ALibAddStaticLibrary()

    add_executable(           resource-export ${SOURCES})
    ALibSetCompilerAndLinker( resource-export )

    target_link_libraries(    resource-export   ALib_StaticLib )



