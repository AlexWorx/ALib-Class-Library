# #################################################################################################
#  CMakeLists.txt - ALox Tools: ResourceExport
#
#  Copyright 2015-2024 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Notes:
#   This tool exports ALib resources for externalization.
#   See section "3.5.3 Exporting Resources For Externalization" of the Programmer's Manual
#   of ALib for more information.
#
#   Starting with version 1903, the output of this tool for each version is found in folder
#   <b>./docs/pages/resource-exports/</b> of the ALib base directory.
# ##################################################################################################

# -------------------------------------------------------------------------------------------------
# General
# -------------------------------------------------------------------------------------------------
    cmake_minimum_required( VERSION 3.20.0 )
    project( "ResourceExport" )

## -------------------------------------------------------------------------------------------------
## include ALib/ALox base cmake file
## -------------------------------------------------------------------------------------------------

    include( ${CMAKE_CURRENT_LIST_DIR}/../../build/cmake/ALib.cmake )


# -------------------------------------------------------------------------------------------------
# sources
# -------------------------------------------------------------------------------------------------
    LIST( APPEND   SOURCES resourceexport.cpp)


# -------------------------------------------------------------------------------------------------
# The target
# -------------------------------------------------------------------------------------------------
    ALibAddStaticLibrary()

    add_executable(           resource-export ${SOURCES})
    ALibSetCompilerAndLinker( resource-export )

    target_link_libraries(    resource-export   PRIVATE ALib_StaticLib )
