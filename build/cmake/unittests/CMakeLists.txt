# #################################################################################################
#  CMakeLists.txt - ALib Unit Tests
#
#  Copyright 2015-2024 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  How to use:
#   1. Enter subdirectory 'debug'   and invoke:  `cmake ..`
#   2. Enter subdirectory 'release' and invoke:  `cmake -DCMAKE_BUILD_TYPE=Release ..`
#   3. Invoke `make` within any of these directories.
#   More information is found in ALib documentation.
# #################################################################################################

# -------------------------------------------------------------------------------------------------
# Prior to setting CMake project
# -------------------------------------------------------------------------------------------------
    cmake_minimum_required( VERSION 3.18.0 )

    ### uncomment to force cotire on clean cmake built
    # set( ALIB_CMAKE_COTIRE   "On" )

# -------------------------------------------------------------------------------------------------
# General
# -------------------------------------------------------------------------------------------------
    if( NOT APPLE )
        project("ALib_UnitTests")
    else()
        project("ALib_UnitTests" LANGUAGES CXX OBJCXX)
    endif()


    # enable modules in project path
    set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.")

## -------------------------------------------------------------------------------------------------
## include ALib base cmake file
## -------------------------------------------------------------------------------------------------

    #set(CMAKE_CXX_STANDARD 17)
    #set(CMAKE_CXX_STANDARD 20)
    #set(CMAKE_CXX_STANDARD 23)

    #set( ALIB_CMAKE_VERBOSE  "On" ) # uncomment for more information

    message(STATUS "ALib Base Directory:       ${ALIB_BASE_DIR}")
    include( ../../../build/cmake/ALib.cmake )

# --------------------------------------------------------------------------------------------------
# set variables
# --------------------------------------------------------------------------------------------------
    set( ALIB_GTEST                     "On"                                            CACHE STRING
         "If true, unit tests are compiled for gtest library."   )

    # set(ALIB_GDB_PP_SUPPRESS_CHILDREN  "Off" CACHE BOOL
    #     "<<< Overwritten by CLion CMAKE File of UnitTests" FORCE)

# -------------------------------------------------------------------------------------------------
# source code definitions
# -------------------------------------------------------------------------------------------------
    # ALib unit tests
    set (              ALIB_UTESTS_SRC_DIR  ${ALIB_BASE_DIR}/src/unittests       )
    file( GLOB_RECURSE ALIB_UTESTS_INCS     ${ALIB_UTESTS_SRC_DIR}/*.h*              )
    file( GLOB_RECURSE ALIB_UTESTS_SRCS     ${ALIB_UTESTS_SRC_DIR}/*.cpp             )

## -------------------------------------------------------------------------------------------------
## find libraries
## -------------------------------------------------------------------------------------------------

    if ( ${ALIB_GTEST} )
        #----- Old version: use a tool script to download GTest -------
        if (CMAKE_VERSION VERSION_LESS 3.20)
            # Include CMake download script of "crascit". (See license note in corresponding directory)
            include(${ALIB_BASE_DIR}/build/cmake/tools/crascit_download/DownloadProject.cmake)

            if (CMAKE_VERSION VERSION_LESS 3.2)
                set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
            else()
                set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
            endif()

            download_project(PROJ                googletest
                             GIT_REPOSITORY      https://github.com/google/googletest.git
                             GIT_TAG             release-1.12.1 # last to support C++11
                             ${UPDATE_DISCONNECTED_IF_AVAILABLE}                               )

            # Prevent GoogleTest from overriding our compiler/linker options
            # when building with Visual Studio
            set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

            add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

            # When using CMake 2.8.11 or later, header path dependencies
            # are automatically added to the gtest and gmock targets.
            # For earlier CMake versions, we have to explicitly add the
            # required directories to the header search path ourselves.
            if (CMAKE_VERSION VERSION_LESS 2.8.11)
                include_directories("${gtest_SOURCE_DIR}/include"
                                    "${gmock_SOURCE_DIR}/include")
            endif()

        #----- Actual version using new cmake built-in FetchContent() method -------
        else()
            # Fetch GoogleTest
            # We use a local tarball, fixed to version 1.12.1, the latest to support C++11.
            # To get a more current one, just switch comment of lines "URL" and "GIT_REPOSITORY
            include(FetchContent)
            #cmake_policy( SET CMP0135 NEW )
            FetchContent_Declare( googletest
                                  GIT_REPOSITORY https://github.com/google/googletest.git    GIT_TAG  release-1.12.1 # last to support C++11
                                  #URL            "${ALIB_BASE_DIR}/tools/gtest.tar.gz"
                                  DOWNLOAD_EXTRACT_TIMESTAMP true
                                                        )
            FetchContent_MakeAvailable(googletest)
            FetchContent_GetProperties(googletest)

            # Prevent GoogleTest from overriding our compiler/linker options
            # when building with Visual Studio
            set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        endif()

        # prevent warnings
        if( NOT MSVC)
            target_compile_options ( gtest    PRIVATE "-Wno-implicit-int-float-conversion" )
            target_compile_options ( gtest    PRIVATE "-Wno-psabi" )
            #target_compile_options ( gtest    PRIVATE "-Wno-restrict" )
        endif()

        # 32-bit compilations
        if("-m32" IN_LIST ALIB_COMPILER_OPTIONS  )
            target_compile_options ( gtest    PRIVATE "-m32" )
        endif()
    endif()

    # QT (only in 64bit compilations)
    if (ALIB_UT_NO_QT)
        if (NOT WIN32)
            if( NOT  "-m32" IN_LIST ALIB_COMPILER_OPTIONS  )
                find_package(Qt5Core)
                find_package(Qt5Widgets)
            endif()
        endif()
    endif()

    # multithreading
    if ( NOT ${ALIB_FEAT_THREADS} )
        if ( ${ALIB_GTEST} )

        find_package(Threads)

        # we hack this into the existing alib variable...
        set( ALIB_EXTERNAL_LIBS ${ALIB_EXTERNAL_LIBS}         ${CMAKE_THREAD_LIBS_INIT} )

        endif()
    endif()


# -------------------------------------------------------------------------------------------------
# Target
# -------------------------------------------------------------------------------------------------
    if( NOT DEFINED  ALIB_UT_USE_STATIC_LIB )
        set( ALIB_UT_USE_STATIC_LIB              "Off"                                  CACHE   BOOL
             "If 'On' compiles and links to static library, otherwise uses a shared (dynamic) one." )
    endif()

    if( ${ALIB_UT_LINKER_MOLD} )
        list( APPEND  ALIB_LINKER_OPTIONS    "-fuse-ld=mold"     )
    endif()

    if( ${ALIB_UT_USE_STATIC_LIB} )
        ALibAddStaticLibrary()
    else()
        ALibAddSharedLibrary()
    endif()


    add_executable             ( ALib_UT    ${ALIB_INCLUDE_FILES}
                                            ${ALIB_UTESTS_INCS}
                                            ${ALIB_UTESTS_SRCS}  )

    target_include_directories ( ALib_UT    PRIVATE   ${ALIB_UTESTS_SRC_DIR}           )
    ALibSetCompilerAndLinker   ( ALib_UT )

    target_compile_definitions(ALib_UT PRIVATE ALIB_BASE_DIR="${ALIB_BASE_DIR}")

    if( ${ALIB_UT_REDUCED_COMPILE_TIME} )
        target_compile_definitions(ALib_UT PRIVATE ALIB_UT_REDUCED_COMPILE_TIME)
    endif()

    if( ${ALIB_UT_MEASURE_COMPILE_TIME} )
        if ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"     )
            target_compile_options( ALib_UT PRIVATE   "-ftime-trace"    )

            add_custom_command( TARGET ALib_UT  POST_BUILD
                                COMMAND "/home/dev/tools/ClangBuildAnalyzer-1.5.0/build/ClangBuildAnalyzer"
                                ARGS --all ${CMAKE_BINARY_DIR} builttime.json )
            add_custom_command( TARGET ALib_UT  POST_BUILD
                                COMMAND "/home/dev/tools/ClangBuildAnalyzer-1.5.0/build/ClangBuildAnalyzer"
                                ARGS --analyze ${CMAKE_BINARY_DIR}/builttime.json )
        endif()
    endif()


    target_precompile_headers(ALib_SharedLib  PRIVATE   "${ALIB_BASE_DIR}/src/alib/alib_precompile.hpp" )
    target_precompile_headers(ALib_UT         PRIVATE   "${ALIB_BASE_DIR}/src/alib/alib_precompile.hpp" )


    # for GTEST, option available only up to gcc 7.2
    if     ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU"    )
      if   (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.2")
        target_compile_options ( ALib_UT    PRIVATE "-Wno-gnu-statement-expression" )
      endif()
    endif()

    if( NOT APPLE )
        target_link_libraries      ( ALib_UT   PRIVATE ${ALIB_EXTERNAL_LIBS}                    )
        if( NOT MSVC)
            target_link_libraries  ( ALib_UT   PRIVATE stdc++ stdc++fs                          )
        endif()
    endif()

    if( ${ALIB_UT_USE_STATIC_LIB} )
        if( ${ALIB_CMAKE_COTIRE} )
            target_link_libraries     ( ALib_UT   PRIVATE ALib_StaticLib_unity  )
        else()
            target_link_libraries     ( ALib_UT   PRIVATE ALib_StaticLib        )
        endif()
    else()
        if( ${ALIB_CMAKE_COTIRE} )
            target_link_libraries     ( ALib_UT   PRIVATE ALib_SharedLib_unity  )
        else()
            target_link_libraries     ( ALib_UT   PRIVATE ALib_SharedLib        )
        endif()
    endif()

    if ( ${ALIB_GTEST} )
        target_compile_definitions( ALib_UT PRIVATE  "ALIB_GTEST"                       )
        target_include_directories( ALib_UT PRIVATE   ${gtest_source_dir}/include       )
        target_link_libraries     ( ALib_UT PRIVATE  gtest                              )
    endif()


    if ( Qt5Core_FOUND )
        target_include_directories( ALib_UT PRIVATE      ${Qt5Core_INCLUDE_DIRS}       )
        target_compile_definitions( ALib_UT PRIVATE      ${Qt5Core_DEFINITIONS}        )
        target_link_libraries     ( ALib_UT PRIVATE      ${Qt5Core_QTMAIN_LIBRARIES}   )
    endif()

    if ( Qt5Widgets_FOUND )
        target_include_directories( ALib_UT PRIVATE      ${Qt5Widgets_INCLUDE_DIRS}    )
        target_compile_definitions( ALib_UT PRIVATE      ${Qt5Widgets_DEFINITIONS}     )
        target_link_libraries     ( ALib_UT PRIVATE      ${Qt5Widgets_LIBRARIES}       )
    endif()

# -------------------------------------------------------------------------------------------------
# Precompiled headers with 'cotire' CMake script.
# For more Info see 'ALib.cmake' which downloads the script and the cotire website at
# https://github.com/sakra/cotire/
# -------------------------------------------------------------------------------------------------
if ( ${ALIB_CMAKE_COTIRE} )

    # use multiple processor cores
    set_target_properties( ALib_UT    PROPERTIES COTIRE_UNITY_SOURCE_MAXIMUM_NUMBER_OF_INCLUDES "-j" )

    # header for precomp is "alib/alib_precompile.hpp"
    set_target_properties( ALib_UT    PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT  "${ALIB_SOURCE_DIR}/alib/alib_precompile.hpp" )

    #set some other cotire specific settings
    set_source_files_properties( ${ALIB_SOURCE_DIR}/unittests/alox/ut_alox_log_scopes.cpp      PROPERTIES COTIRE_EXCLUDED "1"  )
    set_source_files_properties( ${ALIB_SOURCE_DIR}/unittests/ut_alib_monomem_monomem.cpp      PROPERTIES COTIRE_EXCLUDED "1"  )
    set_source_files_properties( ${ALIB_SOURCE_DIR}/unittests/ut_alib_monomem_stringtree.cpp   PROPERTIES COTIRE_EXCLUDED "1"  )
    set_source_files_properties( ${ALIB_SOURCE_DIR}/unittests/ut_alib_strings.cpp              PROPERTIES COTIRE_EXCLUDED "1"  )

    set_target_properties      ( ALib_UT PROPERTIES   COTIRE_UNITY_SOURCE_POST_UNDEFS "LOX_LOX;TESTCLASSNAME;cout" )


    # add cotire to project
    if( ${ALIB_UT_USE_STATIC_LIB} )
        cotire( ALib_StaticLib )
    else()
        cotire( ALib_SharedLib )
    endif()
    cotire( ALib_UT )

#set_target_properties( ALib_StaticLib PROPERTIES EXCLUDE_FROM_ALL           1 )
#set_target_properties( ALib_StaticLib PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1 )



    #--- dump cotire properties ---
    # set ( _dumppname "ALib_UT" )
    # get_target_property( _unitySource        ${_dumppname}    COTIRE_CXX_UNITY_SOURCE      )
    # get_target_property( _prefixHeader       ${_dumppname}    COTIRE_CXX_PREFIX_HEADER     )
    # get_target_property( _precompiledHeader  ${_dumppname}    COTIRE_CXX_PRECOMPILED_HEADER)
    # get_target_property( _unityTargetName    ${_dumppname}    COTIRE_UNITY_TARGET_NAME     )
    #
    # message(STATUS "${_dumppname} unity source:       ${_unitySource}")
    # message(STATUS "${_dumppname} prefix header:      ${_prefixHeader}")
    # message(STATUS "${_dumppname} precompiled header: ${_precompiledHeader}")
    # message(STATUS "${_dumppname} unity target:       ${_unityTargetName}")

endif()

# -------------------------------------------------------------------------------------------------
# End
# -------------------------------------------------------------------------------------------------

# uncomment for debug
# ALibDumpCMakeVariables()
