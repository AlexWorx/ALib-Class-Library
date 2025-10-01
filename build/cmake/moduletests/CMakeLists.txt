# #################################################################################################
#  CMakeLists.txt - ALib Unit Tests
#
#  Copyright 2013-2025 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Note:
#   This project creates a source file for each header-file used in the selected ALib Build,
#   that includes the corresponding header.
#   Furthermore, ALib CMake feature ALIB_SOURCE_COPY_TARGET_DIR is used, to copy the library
#   source files to a temporary directory.
#   Finally, several batch files exists that invoke this script with various module combinations,
#   debug and release compilations, 32-bit compilations and C++17 and 20 versions.
#
#   This altogether ensures that
#   1. The CMake module dependencies are set right
#   2. The CMake source selections is correct.
#   3. The C++ module dependencies are set right
#   4. Every HPP file can be included as the first header.
#   5. All code compiles in various permutations of module selections (ALib Builds)
#   6. All code compiles with different C++ settings.
#
#  How to use:
#   Run "batch_build.sh". When this done (several minutes later) check "compilation_output.txt"
#   for compiler errors.
# #################################################################################################

# -------------------------------------------------------------------------------------------------
# Prior to setting CMake project
# -------------------------------------------------------------------------------------------------
    cmake_minimum_required(VERSION 3.20) # For C++ 20 module compilation, V. 3.28 is needed

    project("ALib_ModuleDepsAndHeaderTest")

# -------------------------------------------------------------------------------------------------
# Clean former runs
# -------------------------------------------------------------------------------------------------
    # enable modules in project path
    set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.")


    #------------------- Temp directory settings (delete and re-create) ------------------------

    set( ALIB_SOURCE_COPY_TARGET_DIR    "headertest" )


    #---------------------------------       Include ALib       ---------------------------------
    include( ../../../build/cmake/ALib.cmake )
    file(REMOVE_RECURSE                 "/tmp/headertest/")
    file(MAKE_DIRECTORY                 "/tmp/headertest/")
    set( ALIB_TEST_HEADERS_DIR          "/tmp/headertest/"  )


## -------------------------------------------------------------------------------------------------
## find libraries
## -------------------------------------------------------------------------------------------------

    # QT (only in 64bit compilations)
    if( NOT  "-m32" IN_LIST CMAKE_CXX_FLAGS )
        find_package(Qt5Core)
        find_package(Qt5Widgets)
    endif()

    # multithreading
    if ( ${ALIB_GTEST} )
        find_package(Threads)

        # we hack this into the existing alib variable...
        set( ALIB_EXTERNAL_LIBS ${ALIB_EXTERNAL_LIBS}         ${CMAKE_THREAD_LIBS_INIT} )
    endif()


# -------------------------------------------------------------------------------------------------
# Project to test if all headers can be used as the first to be included.
# -------------------------------------------------------------------------------------------------

#------------  Create cpp files that include exactly one header ------------
if( NOT IS_DIRECTORY ${ALIB_TEST_HEADERS_DIR})
    MESSAGE(FATAL_ERROR "Variable ALIB_TEST_HEADERS_DIR=\"${ALIB_TEST_HEADERS_DIR}\" "
                        "is set but is not a valid directory. Stopping here!"  )
endif()

foreach( fileName IN LISTS   ALIB_H )

    string(FIND ${fileName} ".inl" position REVERSE)
    IF( ${position} EQUAL -1 )
        string(FIND ${fileName} "lang/plugins.hpp" position REVERSE) # depends on enums
        IF( ${position} EQUAL -1 )
            STRING(REPLACE "${ALIB_SOURCE_DIR}/" "" fileName ${fileName})

            STRING(REPLACE "/" "__"  name  ${fileName}  )

            SET( destPath  "${ALIB_TEST_HEADERS_DIR}/${name}.cpp" )

            message( " filename   : ${fileName}" )
            # message( "    name    : ${name}" )
            # message( "    destPath: ${destPath}" )
            # message( "    contents: #include \"${fileName}\"\n" )
            file( WRITE ${destPath} "#include \"${fileName}\"\n" )
        ENDIF()
    ENDIF()
endforeach()


#------------  add project ------------
file( GLOB_RECURSE HCTL_SRCS            ${ALIB_TEST_HEADERS_DIR}/*.cpp         )


add_library( HeaderCompileTestLib       ${ALIB_HPP}
                                        ${HCTL_SRCS}  )

ALibSetCompilerAndLinker( HeaderCompileTestLib )

if ( Qt5Core_FOUND )
    target_include_directories( HeaderCompileTestLib PRIVATE      ${Qt5Core_INCLUDE_DIRS}       )
    target_compile_definitions( HeaderCompileTestLib PRIVATE      ${Qt5Core_DEFINITIONS}        )
else()
    target_compile_definitions( HeaderCompileTestLib PRIVATE      "ALIB_QT_LIB_NOT_FOUND"       )
endif()

if ( Qt5Widgets_FOUND )
    target_include_directories( HeaderCompileTestLib PRIVATE      ${Qt5Widgets_INCLUDE_DIRS}    )
    target_compile_definitions( HeaderCompileTestLib PRIVATE      ${Qt5Widgets_DEFINITIONS}     )
endif()



# -------------------------------------------------------------------------------------------------
# End
# -------------------------------------------------------------------------------------------------

# uncomment for debug
#ALibDumpCMakeVariables()
