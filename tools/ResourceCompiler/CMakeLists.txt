# #################################################################################################
#  ALib Resource Compiler Tool, CMakeLists.txt
#  Copyright 2013-2026 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Note: This compiles resources defined in external ".alibrc"-files and inserts a code-block
#        into C++ compilation units. All information is given at:
#        https://alib.dev/alib_mod_camp.html#alib_camp_rc
# ##################################################################################################

# Project Definition
    cmake_minimum_required(VERSION 3.20)
    project( "ResourceCompiler" )

# Select ALib modules and set ALib Features
    list( APPEND  ALIB_BUILD         "APP"      )
    set( ALIB_SINGLE_THREADED              "On" )
    set( ALIB_CAMP_RESOURCE_COMPILATION    "On" )

# Run ALib.cmake
    include("../../build/cmake/ALib.cmake")

# Project sources
    LIST( APPEND   SOURCES   alibrc.cpp   alibrc.hpp )

# Add static ALib (CMake target name is 'ALib_StaticLib')
    ALibAddStaticLibrary()

# Add executable 'alibrc'
    add_executable(           alibrc ${SOURCES})
    ALibSetCompilerAndLinker( alibrc )
    target_link_libraries(    alibrc   PRIVATE ALib_StaticLib )
