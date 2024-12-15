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
    cmake_minimum_required( VERSION 3.20.0 )

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
        # Fetch GoogleTest
        include(FetchContent)
        #cmake_policy( SET CMP0135 NEW )
        FetchContent_Declare( googletest
                              GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG  main
                              #URL            "${ALIB_BASE_DIR}/tools/googletest-1.15.2.tar.gz"
                              DOWNLOAD_EXTRACT_TIMESTAMP true
                                                    )
        FetchContent_MakeAvailable(googletest)
        FetchContent_GetProperties(googletest)

        # Prevent GoogleTest from overriding our compiler/linker options
        # when building with Visual Studio
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

        # prevent warnings
        if( NOT MSVC)
            #target_compile_options ( gtest    PRIVATE "-Wno-implicit-int-float-conversion" )
            #target_compile_options ( gtest    PRIVATE "-Wno-psabi" )
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
# Create doxygen.ini file from .cmake.ini file
# -------------------------------------------------------------------------------------------------
if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    message( "Copying doxygen template (${ALIB_BASE_DIR}/docs/doxygen/doxyfile.cmake.ini)")
    configure_file( "${ALIB_BASE_DIR}/docs/doxygen/doxyfile.cmake.ini"
                    "${ALIB_BASE_DIR}/docs/doxygen/doxyfile.ini" @ONLY)
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
        target_link_libraries     ( ALib_UT   PRIVATE ALib_StaticLib        )
    else()
        target_link_libraries     ( ALib_UT   PRIVATE ALib_SharedLib        )
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
# End
# -------------------------------------------------------------------------------------------------

# uncomment for debug
# ALibDumpCMakeVariables()
