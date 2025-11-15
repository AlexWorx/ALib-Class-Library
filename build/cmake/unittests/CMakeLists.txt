# #################################################################################################
#  CMakeLists.txt - ALib Unit Tests
#
#  Copyright 2013-2025 A-Worx GmbH, Germany
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
    cmake_minimum_required(VERSION 3.20) # For C++ 20 module compilation, V. 3.28 is needed

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

# -------------------------------------------------------------------------------------------------
# source code definitions
# -------------------------------------------------------------------------------------------------
    # ALib unit tests
    set (              ALIB_UTESTS_SRC_DIR  ${ALIB_BASE_DIR}/src.samples/unittests       )
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
                              URL            "file://${ALIB_BASE_DIR}/tools/googletest-1.15.2.tar.gz"
                              URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
                              DOWNLOAD_EXTRACT_TIMESTAMP true
                                                    )
        FetchContent_MakeAvailable(googletest)
        FetchContent_GetProperties(googletest)

        # Prevent GoogleTest from overriding our compiler/linker options
        # when building with Visual Studio
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

        # apply some ALib options
        if("-m32" IN_LIST ALIB_COMPILER_OPTIONS  )
            target_compile_options ( gtest    PRIVATE "-m32" )
        endif()

        if ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"     )
            target_compile_options ( gtest    PRIVATE "-Wno-character-conversion" )
        endif()

        if ( ALIB_CLANG_USE_LIBCPP )
            target_compile_options ( gtest    PRIVATE "-Wno-character-conversion" )
            target_compile_options ( gtest    PRIVATE -stdlib=libc++)
            target_link_libraries(   gtest    PRIVATE c++)
        endif()



    endif()

    # QT (only in 64bit compilations)
    if (ALIB_UT_QT)
        if (NOT WIN32)
            if( NOT  "-m32" IN_LIST ALIB_COMPILER_OPTIONS  )
                find_package(Qt5Core)
                find_package(Qt5Widgets)
            endif()
        endif()
    endif()

    # multithreading
    if ( ${ALIB_GTEST} )
        find_package(Threads)

        # we hack this into the existing alib variable...
        set( ALIB_EXTERNAL_LIBS ${ALIB_EXTERNAL_LIBS}         ${CMAKE_THREAD_LIBS_INIT} )
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

    # check for <format>
    check_cxx_source_compiles([=[
      #include <format>
      int main() { auto s = std::format("{}", 42); }
    ]=] HAVE_STD_FORMAT)

    if (NOT HAVE_STD_FORMAT)
      find_package(fmt REQUIRED)              # needs libfmt-dev installed
    endif()



    add_executable             ( ALib_UT    ${ALIB_HPP}
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
#        target_include_directories( ALib_UT PRIVATE   ${gtest_source_dir}/include       )
        target_link_libraries     ( ALib_UT PRIVATE  gtest                              )
    endif()

    if (NOT HAVE_STD_FORMAT)
      target_link_libraries(        ALib_UT PRIVATE fmt::fmt)
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
# Create doxygen.ini file from .cmake.ini file
# -------------------------------------------------------------------------------------------------
if(CMAKE_HOST_WIN32)
    set(TEMP_DIR $ENV{TEMP})
else()
    set(TEMP_DIR "/tmp")
endif()
if(EXISTS "${TEMP_DIR}")
    # if locks fail, obviously parallel runs of this CMake script are in place (which is normal
    # with IDEs that perform different configurations in parallel). Thus we ignore it, as the
    # doxygen config has to be configured only once.
    file(LOCK "${TEMP_DIR}/alib_doxyfile.lock" TIMEOUT 0 RESULT_VARIABLE LOCK_RESULT)
    if("${LOCK_RESULT}" STREQUAL "0")
        message( "Configuring doxygen template (${ALIB_BASE_DIR}/docs/doxygen/doxyfile.cmake.ini)")
        configure_file( "${ALIB_BASE_DIR}/docs/doxygen/doxyfile.cmake.ini"
                        "${TEMP_DIR}/alib_doxyfile.ini" @ONLY)
        file(LOCK "${TEMP_DIR}/alib_doxyfile_config.lock" RELEASE)
    endif()
 else()
    message(WARNING "The system's temporary directory (${SYSTEM_TEMP_DIR}) was not found. Doxygen cannot be configured and documentation not be built on this machine.")
endif()

# -------------------------------------------------------------------------------------------------
# End
# -------------------------------------------------------------------------------------------------

# uncomment for debug
# ALibDumpCMakeVariables()
