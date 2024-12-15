# #################################################################################################
#  ALib.cmake - CMake file for projects using ALib
#
#  Copyright 2015-2024 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
# \file
# CMake file for projects using ALib
# #################################################################################################

# --------------------------------------------------------------------------------------------------
# checks
# --------------------------------------------------------------------------------------------------
    cmake_minimum_required( VERSION 3.20.0 )

    # check
    if (tmp_alib_included_marker)
            message( FATAL_ERROR "ALib.cmake: Already included (included twice!)" )
        return()
    endif()
    set(tmp_alib_included_marker "1")


    # Infer base directory from this script's location
    if( NOT DEFINED  ALIB_BASE_DIR )
        set( temp "${CMAKE_CURRENT_LIST_DIR}/../.." )
        get_filename_component(temp ${temp} ABSOLUTE)
        set( ALIB_BASE_DIR                  ${temp}                                     CACHE   PATH
             "The base path to ALib containing source code, project files, tools, docs, etc.")
    endif()


    # check
    if (NOT EXISTS "${ALIB_BASE_DIR}/src/alib" )
        message( FATAL_ERROR "ALib.cmake: Can't read sources in ALIB_BASE_DIR= ${ALIB_BASE_DIR}" )
        return()
    endif()


    # build type defaults to "Debug"
    if ( "${CMAKE_BUILD_TYPE}" STREQUAL ""    )
        set( CMAKE_BUILD_TYPE "Debug" )
    endif()

    MESSAGE( STATUS "Build type: ${CMAKE_BUILD_TYPE}" )


    # include tool functions
    include( ${CMAKE_CURRENT_LIST_DIR}/ALibTools.cmake )

# --------------------------------------------------------------------------------------------------
# ALib Module Dependency Resolution
# --------------------------------------------------------------------------------------------------

    include( ${CMAKE_CURRENT_LIST_DIR}/ALibModules.cmake )

# --------------------------------------------------------------------------------------------------
# ALib Cache Variables
# The variables are only set, if not already predefined prior to invoking this script.
# --------------------------------------------------------------------------------------------------

set( ALIB_VERSION                   "2412R0"                                            CACHE STRING
     "The ALib version. Not modifiable (will be overwritten on generation!)"        FORCE )

set( ALIB_VERSION_NO                "2412" )
set( ALIB_VERSION_REV               "0" )

if( NOT DEFINED  ALIB_DEBUG )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        set( defaultALIB_DEBUG   "On" )
    else()
        set( defaultALIB_DEBUG   "Off" )
    endif()
else()
        set( defaultALIB_DEBUG   ALIB_DEBUG )
endif()

set( ALIB_DEBUG                        ${defaultALIB_DEBUG}                             CACHE   BOOL
     "Enable/disable ALib debug code. Defaults to true with debug builds, otherwise to false." )


if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    if( NOT DEFINED  ALIB_DEBUG_GLIB )
        set( ALIB_DEBUG_GLIB                    "Off"                                   CACHE   BOOL
             "Defaults to false. If true, compiler symbols '_GLIBCXX_DEBUG', '_GLIBCXX_DEBUG_PEDANTIC' and '_GLIBCPP_CONCEPT_CHECKS' are set." )
    endif()


    if( NOT DEFINED  ALIB_COVERAGE_COMPILE )
        set( ALIB_COVERAGE_COMPILE              "Off"                                   CACHE   BOOL
             "Defaults to false. If true, option --coverage is added to GNU compiler command line.")
    endif()

    if ( $ENV{CLION_IDE} )
        set( tmpDefault   "On" )
    else()
        set( tmpDefault   "Off" )
    endif()

    set( ALIB_GDB_PP_SUPPRESS_CHILDREN     ${tmpDefault}                                CACHE   BOOL
         "Defaults to false except if CMake is run from within JetBrains CLion. If true, a corresponding symbol gets set with debug builds which is detected by GDB pretty printer scripts provided with ALib.")

    set( ALIB_GDB_PP_FIND_POINTER_TYPES    ${tmpDefault}                                CACHE   BOOL
         "Defaults to false. If true, a corresponding symbol gets set with debug builds which is detected by GDB pretty printer scripts provided with ALib.")
endif()

if( NOT DEFINED  ALIB_PRECOMPILED_HEADER )
    set( ALIB_PRECOMPILED_HEADER            "Off"                                       CACHE   BOOL
         "If on, header file ’alib/alib_precompile.hpp' will included some default headers, depending on the selected modules. Defaults to off." )
endif()


if( NOT DEFINED  ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH )
    set( ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH  "Off"                                       CACHE   BOOL
         "If true, no thread library is searched and bound to the target. If false, a thread library is searched and bound even if module Threads is not included. This is to allow debug assertions with multi-threaded use of a single-threaded ALib distribution." )
endif()


if( NOT DEFINED  ALIB_CMAKE_VERBOSE )
    set( ALIB_CMAKE_VERBOSE                 "Off"                                       CACHE   BOOL
         "If true, CMake generation runs will provide a detailed report." )
endif()


if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    if( NOT DEFINED  ALIB_DEBUG_ALLOCATIONS )
        set( ALIB_DEBUG_ALLOCATIONS             "Off"                                   CACHE   BOOL
             "Adds allocation debug mechanics with lang::HeapAllocator and ALib Monomem allocators. Defaults to false." )
    endif()
endif()


if( "SINGLETONS" IN_LIST ALIB_DISTRIBUTION )
    if ( ${WIN32} )
        set( platformDefaultFor_SINGLETON_MAPPED   "On" )
    else()
        set( platformDefaultFor_SINGLETON_MAPPED   "Off" )
    endif()
    if( NOT DEFINED  ALIB_FEAT_SINGLETON_MAPPED )
        set( ALIB_FEAT_SINGLETON_MAPPED    ${platformDefaultFor_SINGLETON_MAPPED}       CACHE   BOOL
             "Defaults to true on Windows OS, which then selects code to implement class Singleton to work with multiple data segments, as imposed by the use of Windows DLLs.")
    endif()
endif()

if( "BITBUFFER" IN_LIST ALIB_DISTRIBUTION )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if( NOT DEFINED  ALIB_DEBUG_ARRAY_COMPRESSION )
            set( ALIB_DEBUG_ARRAY_COMPRESSION   "On"                                    CACHE   BOOL
                 "If true, in debug compilations, compressed arrays are read back to check if result is same. Defaults to true." )
        endif()
    endif()
endif()

if( "CONTAINERS" IN_LIST ALIB_DISTRIBUTION )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if( NOT DEFINED  ALIB_DEBUG_CONTAINERS )
            set( ALIB_DEBUG_CONTAINERS             "Off"                                   CACHE   BOOL
                 "Adds debug features to module ALib Containers. Defaults to false." )
        endif()
    endif()
endif()

if( "MONOMEM" IN_LIST ALIB_DISTRIBUTION )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if( NOT DEFINED  ALIB_DEBUG_MONOMEM )
            set( ALIB_DEBUG_MONOMEM             "Off"                                   CACHE   BOOL
                 "Adds consistency checks and collection of statistics with module ALib Monomem. Defaults to false." )
        endif()
    endif()
endif()


if( "BOXING" IN_LIST ALIB_DISTRIBUTION )
    if( NOT DEFINED  ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS )
        set( ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS           "Off"                       CACHE   BOOL
             "If true, any C++ integral type will be boxed to its original type. If false (the default) all types are boxed to type 'integer', respectively / 'uinteger' and only this can be unboxed.")
    endif()

    if( NOT DEFINED  ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS )
        set( ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS          "Off"                       CACHE   BOOL
             "If true, any C++ character type will be boxed to its original type. If false (the default) all types are boxed to type 'character' and only this can be unboxed.")
    endif()

    if( NOT DEFINED  ALIB_FEAT_BOXING_BIJECTIVE_FLOATS )
        set( ALIB_FEAT_BOXING_BIJECTIVE_FLOATS              "Off"                       CACHE   BOOL
             "If true, type float will be boxed as float. If false (the default) float will be boxed as double and cannot be unboxed.")
    endif()

    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if( NOT DEFINED  ALIB_DEBUG_BOXING )
            set( ALIB_DEBUG_BOXING                          "Off"                       CACHE   BOOL
                 "Adds collection of statistics and debug methods to module ALib Boxing. Defaults to false." )
        endif()
    endif()
endif()

if( "CHARACTERS" IN_LIST ALIB_DISTRIBUTION )
    if ( ${WIN32} )
        set( defaultALIB_CHARACTERS_WIDE          "On"  )
        set( defaultALIB_CHARACTERS_SIZEOF_WCHAR  "2"   )
    else()
        set( defaultALIB_CHARACTERS_WIDE          "Off" )
        set( defaultALIB_CHARACTERS_SIZEOF_WCHAR  "4"   )
    endif()

    if( NOT DEFINED  ALIB_CHARACTERS_WIDE )
        set( ALIB_CHARACTERS_WIDE     ${defaultALIB_CHARACTERS_WIDE}                    CACHE   BOOL
             "If false, the type 'alib::character' is 1-byte wide, otherwise it has the width given with ALIB_CHARACTERS_SIZEOF_WCHAR. Default value depends on platform preference.")
    endif()

    if( NOT DEFINED  ALIB_CHARACTERS_SIZEOF_WCHAR )
        set( ALIB_CHARACTERS_SIZEOF_WCHAR   ${defaultALIB_CHARACTERS_SIZEOF_WCHAR}    CACHE   STRING
             "The width of wide characters, maybe 2 or 4. Default value depends on platform/compiler preference.")
    endif()
endif()

if( "STRINGS" IN_LIST ALIB_DISTRIBUTION )
    if( NOT DEFINED  ALIB_FEAT_BOOST_REGEX )
        set( ALIB_FEAT_BOOST_REGEX          "Off"                                       CACHE   BOOL
             "Defaults to false. If true, activates ALib classes that use boost regular expressions, for example strings::util::RegexMatcher. The corresponding boost library is searched and added to CMake variable ALIB_EXTERNAL_LIBS.")
    endif()

    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if( NOT DEFINED  ALIB_DEBUG_STRINGS )
            set( ALIB_DEBUG_STRINGS             "Off"                                   CACHE   BOOL
                 "Defaults to false. Adds consistency checks to ALib string classes. Useful when developing code to manipulate strings externally, i.e T_Append to specializations.")
        endif()
    endif()
endif()

if( "THREADS" IN_LIST ALIB_DISTRIBUTION )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if( NOT DEFINED  ALIB_DEBUG_CRITICAL_SECTIONS )
            set( ALIB_DEBUG_CRITICAL_SECTIONS       "Off"                               CACHE   BOOL
                 "Defaults to true. Activates lang::DbgCriticalSections with debug compilations.")
        endif()
    endif()
endif()


if( "CAMP" IN_LIST ALIB_DISTRIBUTION )
    if( NOT DEFINED  ALIB_RESOURCES_OMIT_DEFAULTS )
        set( ALIB_RESOURCES_OMIT_DEFAULTS       "Off"                                   CACHE   BOOL
             "If true, ALib modules do not add default versions of resource strings. See section 'Bootstrapping' of ALib Programmer's Manual for more information. Defaults to false.")
    endif()

    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if( NOT DEFINED  ALIB_DEBUG_RESOURCES )
            set( ALIB_DEBUG_RESOURCES           "Off"                                   CACHE   BOOL
                 "Adds collection of statistics and resource export with module ALib Resources. Defaults to false." )
        endif()
    endif()

endif()

if( "FILES" IN_LIST ALIB_DISTRIBUTION )
    if( NOT DEFINED  ALIB_FILES_FORCE_STD_SCANNER )
        set( ALIB_FILES_FORCE_STD_SCANNER      "Off"                                   CACHE   BOOL
             "If true, file scanning of ALib camp 'Files' falls back to a basic implementation using C++ library std::filesystem.")
    endif()
endif()

if( "ALOX" IN_LIST ALIB_DISTRIBUTION )

    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        set( defaultALOX_DBG_LOG   "On" )
    else()
        set( defaultALOX_DBG_LOG   "Off" )
    endif()
    if( NOT DEFINED  ALOX_DBG_LOG )
        set( ALOX_DBG_LOG                   ${defaultALOX_DBG_LOG}                      CACHE   BOOL
             "Enable/disable debug log statements. Defaults to true with debug builds, otherwise to false." )
    endif()

    if( NOT DEFINED  ALOX_DBG_LOG_CI )
        set( ALOX_DBG_LOG_CI                    "On"                                    CACHE   BOOL
             "Defaults to true. If set, caller information is used with debug log statements." )
    endif()

    if( NOT DEFINED  ALOX_REL_LOG )
        set( ALOX_REL_LOG                       "On"                                    CACHE   BOOL
             "Enable/disable release log statements. Defaults to true." )
    endif()

    if( NOT DEFINED  ALOX_REL_LOG_CI )
        set( ALOX_REL_LOG_CI                    "Off"                                   CACHE   BOOL
             "Defaults to false. If set, caller information is used even with release log statements (and even in release builds!)" )
    endif()
endif()

# --------------------------------------------------------------------------------------------------
# Compiler symbols
# --------------------------------------------------------------------------------------------------

# module selection
if( NOT allModules )
    SET( moduleList "" )
    LIST( APPEND moduleList    "EXPRESSIONS;CLI;ALOX" )
    LIST( APPEND moduleList    "CONFIGURATION;CAMP;CONTAINERS"   )
    LIST( APPEND moduleList    "THREADS;THREADMODEL;FILES"  )
    LIST( APPEND moduleList    "ENUMS;BOXING;STRINGS" )
    LIST( APPEND moduleList    "SINGLETONS;MONOMEM;BITBUFFER;CHARACTERS;TIME"   )
    FOREACH( module IN LISTS moduleList )
        IF( module IN_LIST  ALIB_DISTRIBUTION )
            list( APPEND  ALIB_SYMBOLS    "ALIB_${module}"  )
        ENDIF()
    ENDFOREACH()
endif()

# debug
if ( ${ALIB_DEBUG} )
    list( APPEND ALIB_SYMBOLS      "ALIB_DEBUG"                )

    if ( ${ALIB_DEBUG_GLIB} )
        list( APPEND ALIB_SYMBOLS         "_GLIBCXX_DEBUG"
                                          "_GLIBCXX_DEBUG_PEDANTIC"
                                          "_GLIBCPP_CONCEPT_CHECKS"   )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED  "_GLIBCXX_DEBUG"
                                          "_GLIBCXX_DEBUG_PEDANTIC"
                                          "_GLIBCPP_CONCEPT_CHECKS"   )
    endif()

    if ( ${ALIB_GDB_PP_SUPPRESS_CHILDREN} )
        list( APPEND ALIB_SYMBOLS           "ALIB_GDB_PP_SUPPRESS_CHILDREN"   )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_GDB_PP_SUPPRESS_CHILDREN"   )
    endif()
    if ( ${ALIB_GDB_PP_FIND_POINTER_TYPES} )
        list( APPEND ALIB_SYMBOLS           "ALIB_GDB_PP_FIND_POINTER_TYPES"  )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_GDB_PP_FIND_POINTER_TYPES"  )
    endif()

endif()

# precompiled header
if ( ${ALIB_PRECOMPILED_HEADER} )
    list( APPEND ALIB_SYMBOLS           "ALIB_PRECOMPILED_HEADER"  )
else()
    list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_PRECOMPILED_HEADER"  )
endif()

# ALib features
if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    if ( ALIB_DEBUG_ALLOCATIONS )
        list( APPEND ALIB_SYMBOLS           "ALIB_DEBUG_ALLOCATIONS"  )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_DEBUG_ALLOCATIONS"  )
    endif()
endif()


if( "SINGLETONS" IN_LIST ALIB_DISTRIBUTION )
    if (NOT platformDefaultFor_SINGLETON_MAPPED STREQUAL ALIB_FEAT_SINGLETON_MAPPED)
        if ( ALIB_FEAT_SINGLETON_MAPPED )
            list( APPEND ALIB_SYMBOLS    "ALIB_FEAT_SINGLETON_MAPPED"    )
        else()
            list( APPEND ALIB_SYMBOLS    "ALIB_FEAT_SINGLETON_MAPPED=0"  )
        endif()
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED "ALIB_FEAT_SINGLETON_MAPPED=0"  )
    endif()
endif()

if( "BITBUFFER" IN_LIST ALIB_DISTRIBUTION )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if ( ALIB_DEBUG_ARRAY_COMPRESSION )
            list( APPEND ALIB_SYMBOLS           "ALIB_DEBUG_ARRAY_COMPRESSION=1"  )
        else()
            list( APPEND ALIB_SYMBOLS           "ALIB_DEBUG_ARRAY_COMPRESSION=0"  )
        endif()
    endif()
endif()

if( "BOXING" IN_LIST ALIB_DISTRIBUTION )
    if ( ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS )
        list( APPEND ALIB_SYMBOLS           "ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS"  )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS"  )
    endif()
    if ( ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS )
        list( APPEND ALIB_SYMBOLS           "ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS"  )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS"  )
    endif()
    if ( ALIB_FEAT_BOXING_BIJECTIVE_FLOATS )
        list( APPEND ALIB_SYMBOLS           "ALIB_FEAT_BOXING_BIJECTIVE_FLOATS"  )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_FEAT_BOXING_BIJECTIVE_FLOATS"  )
    endif()
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if( ALIB_DEBUG_BOXING )
            list( APPEND ALIB_SYMBOLS           "ALIB_DEBUG_BOXING"   )
        else()
            list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_DEBUG_BOXING"   )
        endif()
    endif()
endif()

if( "CHARACTERS" IN_LIST ALIB_DISTRIBUTION )
    if (NOT (defaultALIB_CHARACTERS_WIDE STREQUAL ALIB_CHARACTERS_WIDE ))
        if ( ALIB_CHARACTERS_WIDE )
            list( APPEND ALIB_SYMBOLS       "ALIB_CHARACTERS_WIDE" )
        else()
            list( APPEND ALIB_SYMBOLS       "ALIB_CHARACTERS_WIDE=0" )
        endif()
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_CHARACTERS_WIDE" )
    endif()

    if (NOT defaultALIB_CHARACTERS_SIZEOF_WCHAR STREQUAL ALIB_CHARACTERS_SIZEOF_WCHAR)
        if ( NOT (ALIB_CHARACTERS_SIZEOF_WCHAR STREQUAL "2" OR  ALIB_CHARACTERS_SIZEOF_WCHAR STREQUAL "4") )
            message( FATAL_ERROR "Value of ALIB_CHARACTERS_SIZEOF_WCHAR must be 2 or 4" )
            return()
        endif()
        list( APPEND ALIB_SYMBOLS           "ALIB_CHARACTERS_SIZEOF_WCHAR=${ALIB_CHARACTERS_SIZEOF_WCHAR}" )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_CHARACTERS_SIZEOF_WCHAR=${ALIB_CHARACTERS_SIZEOF_WCHAR}" )
    endif()

endif()

if( "CONTAINERS" IN_LIST ALIB_DISTRIBUTION )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if ( ALIB_DEBUG_CONTAINERS )
            list( APPEND ALIB_SYMBOLS           "ALIB_DEBUG_CONTAINERS"  )
        else()
            list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_DEBUG_CONTAINERS"  )
        endif()
    endif()
endif()

if( "THREADS" IN_LIST ALIB_DISTRIBUTION )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if ( ALIB_DEBUG_CRITICAL_SECTIONS )
            list( APPEND ALIB_SYMBOLS           "ALIB_DEBUG_CRITICAL_SECTIONS"  )
        else()
            list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_DEBUG_CRITICAL_SECTIONS"  )
        endif()
    endif()
endif()

if( "MONOMEM" IN_LIST ALIB_DISTRIBUTION )
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if ( ALIB_DEBUG_MONOMEM )
            list( APPEND ALIB_SYMBOLS           "ALIB_DEBUG_MONOMEM"  )
        else()
            list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_DEBUG_MONOMEM"  )
        endif()
    endif()
endif()

if( "STRINGS" IN_LIST ALIB_DISTRIBUTION )
    if ( ALIB_FEAT_BOOST_REGEX )
        list( APPEND ALIB_SYMBOLS           "ALIB_FEAT_BOOST_REGEX"  )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_FEAT_BOOST_REGEX"  )
    endif()

    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if ( ALIB_DEBUG_STRINGS )
            list( APPEND ALIB_SYMBOLS           "ALIB_DEBUG_STRINGS"  )
        else()
            list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_DEBUG_STRINGS"  )
        endif()
    endif()
endif()

if( "CAMP" IN_LIST ALIB_DISTRIBUTION )

    if( ALIB_RESOURCES_OMIT_DEFAULTS )
        list( APPEND ALIB_SYMBOLS          "ALIB_RESOURCES_OMIT_DEFAULTS"   )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED   "ALIB_RESOURCES_OMIT_DEFAULTS"   )
    endif()

    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        if( ALIB_DEBUG_RESOURCES )
            list( APPEND ALIB_SYMBOLS          "ALIB_DEBUG_RESOURCES"   )
        else()
            list( APPEND ALIB_SYMBOLS_UNUSED   "ALIB_DEBUG_RESOURCES"   )
        endif()
    endif()
endif()

if( "FILES" IN_LIST ALIB_DISTRIBUTION )
    if ( ALIB_FILES_FORCE_STD_SCANNER )
        list( APPEND ALIB_SYMBOLS           "ALIB_FILES_FORCE_STD_SCANNER"  )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED    "ALIB_FILES_FORCE_STD_SCANNER"  )
    endif()
endif()

if( "ALOX" IN_LIST ALIB_DISTRIBUTION )

    if (NOT defaultALOX_DBG_LOG STREQUAL ALOX_DBG_LOG)
        if( NOT ALOX_DBG_LOG )
            list( APPEND ALIB_SYMBOLS           "ALOX_DBG_LOG"         )
        else()
            list( APPEND ALIB_SYMBOLS_UNUSED    "ALOX_DBG_LOG"         )
        endif()

        if ( NOT ${ALOX_DBG_LOG_CI} )
            list( APPEND ALIB_SYMBOLS           "ALOX_DBG_LOG_CI=0"    )
        else()
            list( APPEND ALIB_SYMBOLS_UNUSED    "ALOX_DBG_LOG_CI=0"    )
        endif()
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED        "ALOX_DBG_LOG"         )
        list( APPEND ALIB_SYMBOLS_UNUSED        "ALOX_DBG_LOG_CI"      )
    endif()

    if ( NOT ALOX_REL_LOG )
        list( APPEND ALIB_SYMBOLS               "ALOX_REL_LOG=0"       )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED        "ALOX_REL_LOG=0"       )
    endif()
    if ( ALOX_REL_LOG_CI )
        list( APPEND ALIB_SYMBOLS               "ALOX_REL_LOG_CI"      )
    else()
        list( APPEND ALIB_SYMBOLS_UNUSED        "ALOX_REL_LOG_CI"      )
    endif()
endif()



# --------------------------------------------------------------------------------------------------
# ALib Source File Definition
# --------------------------------------------------------------------------------------------------
include( ${CMAKE_CURRENT_LIST_DIR}/ALibSources.cmake )


# --------------------------------------------------------------------------------------------------
# External libraries
# --------------------------------------------------------------------------------------------------
if ( NOT ${ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH} )
    find_package(Threads)
    if(Threads_FOUND)
        list(     APPEND  ALIB_SYMBOLS  "ALIB_EXT_LIB_THREADS_AVAILABLE" )
        if(CMAKE_USE_PTHREADS_INIT)
            list( APPEND  ALIB_COMPILER_OPTIONS  "-pthread"       )
        endif()
    endif()

    list( APPEND  ALIB_EXTERNAL_LIBS  ${CMAKE_THREAD_LIBS_INIT} )
endif()

if ( ${ALIB_FEAT_BOOST_REGEX} )
    set(Boost_USE_STATIC_LIBS     "On"  CACHE  BOOL  "Link boost statically" )
    if( "THREADS" IN_LIST ALIB_DISTRIBUTION )
        set(Boost_USE_MULTITHREADED      "On"  CACHE   BOOL "Use multi-threaded version of boost")
    else()
        set(Boost_USE_MULTITHREADED      "Off" CACHE   BOOL "Use single-threaded version of boost")
    endif()

    find_package( Boost CONFIG REQUIRED COMPONENTS regex )

    if(Boost_FOUND)
        list( APPEND  ALIB_EXTERNAL_LIBS Boost::regex )
        if(${Boost_USE_STATIC_LIBS})
           message(STATUS "Found Boost version ${Boost_LIB_VERSION}, linking against boost static libraries")
        else()
           message(STATUS "Found Boost version ${Boost_LIB_VERSION}, linking against boost shared libraries")
        endif()
    else()
        MESSAGE("Attention: Boost::regex requested, but library not found!")
    endif()
endif()

if(APPLE)
    list( APPEND ALIB_EXTERNAL_LIBS  "-framework Foundation")
endif()

# lib math, needed with GCC (suddenly, we don't really know why, added 221205)
if(NOT MSVC)
    if(NOT APPLE)
        list( APPEND ALIB_EXTERNAL_LIBS  "m")
    endif(NOT APPLE)
endif(NOT MSVC)

# --------------------------------------------------------------------------------------------------
# A-Worx compiler features and flags
# --------------------------------------------------------------------------------------------------

# Set minimum required standard C++ 17
list( APPEND ALIB_COMPILER_FEATURES   "cxx_std_17"    )


# if "ALIB_SUPPRESS_COMPILER_WARNINGS" is set prior to invoking this script, this entry is removed
# and nothing is added.
if ("ALIB_SUPPRESS_COMPILER_WARNINGS" IN_LIST ALIB_COMPILER_WARNINGS)
    LIST( REMOVE_ITEM  ALIB_COMPILER_WARNINGS "ALIB_SUPPRESS_COMPILER_WARNINGS" )
else()
    if     ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU"    )
        # add -H to generate output "!/x" for use of precompiled header
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wall"          )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wextra"        )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Werror"        )
        #list( APPEND ALIB_COMPILER_WARNINGS   "-Weffc++"       )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-psabi"     )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-misleading-indentation"  )

        # this was "suddenly" needed with GCC 13.2.1 with release compilation
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-stringop-overread"  )

        # add coverage flags to GCC
        if( ${ALIB_COVERAGE_COMPILE} )
            list( APPEND  ALIB_COMPILER_OPTIONS  "--coverage"  )
            list( APPEND  ALIB_LINKER_OPTIONS    "--coverage"  )
        endif()

        # force unicode (needed for mingw)
        if(MINGW)
            list( APPEND  ALIB_COMPILER_OPTIONS   "-municode"  )
            list( APPEND  ALIB_COMPILER_OPTIONS   "-DUNICODE"  )
            list( APPEND  ALIB_COMPILER_OPTIONS   "-D_UNICODE" )
        endif()


    # Clang: We are using -Weverything, although this is not recommended. We think it should be
    #        recommended. ALib for example does not use old-style casts and explicitly cast each
    #        and every type change! The benefit for ALib users is that ALib code can be used in very
    #        strict build environments without using special warning flags.
    #        Of course, some very obvious things like C++98 compatibility warnings have to be
    #        removed explicitly:
    elseif ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"     )
        list( APPEND ALIB_COMPILER_WARNINGS   "-pedantic"                          )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Weffc++"                           )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Weverything"                       )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-c++98-compat"                  )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-c++98-compat-pedantic"         )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-global-constructors"           )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-exit-time-destructors"         )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-padded"                        )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-weak-vtables"                  )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-documentation-unknown-command" )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-misleading-indentation"        )
        list( APPEND ALIB_COMPILER_WARNINGS   "-Wno-covered-switch-default"        )

        if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
            list( APPEND ALIB_COMPILER_OPTIONS   "-fno-limit-debug-info"   )
        endif()

    # MSVC
    elseif ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC"   )
        list( APPEND ALIB_COMPILER_WARNINGS    "/W4"    )
        list( APPEND ALIB_COMPILER_WARNINGS    "/WX"    )
        list( APPEND ALIB_COMPILER_WARNINGS    "/EHsc"  )

    #! NEVER TESTED YET !
    elseif ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel"  )
        list( APPEND ALIB_COMPILER_WARNINGS "" )

    endif()
endif()

# --------------------------------------------------------------------------------------------------
# A-Worx linker features and flags
# --------------------------------------------------------------------------------------------------
if(APPLE)
    list( APPEND ALIB_LINKER_OPTIONS        "-lObjc"    )
else()
    list( APPEND ALIB_LINKER_OPTIONS        ""          )
endif()


# -------------------------------------------------------------------------------------------------
# Set filename of ALib library  (if not given in ALIB_LIBRARY_FILENAME)
# -------------------------------------------------------------------------------------------------
if ( NOT ALIB_LIBRARY_FILENAME )

    set( ALIB_LIBRARY_FILENAME  "alib_${ALIB_VERSION_NO}R${ALIB_VERSION_REV}" )

    if ( ${ALIB_DEBUG} )
        if ( (CMAKE_BUILD_TYPE STREQUAL "Debug") )
            set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}DBG                    )
        else()
            set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}RELDBG                 )
        endif()
        if ( NOT ${ALIB_DEBUG_CRITICAL_SECTIONS} )
            list( FIND   ALIB_DISTRIBUTION  "THREADS"           idx )
            if( NOT idx LESS 0 )
                set ( ALIB_LIBRARY_FILENAME  ${ALIB_LIBRARY_FILENAME}_NOCSDBG              )
            endif()
        endif()
        if ( ${ALIB_DEBUG_ALLOCATIONS} )
            set ( ALIB_LIBRARY_FILENAME    ${ALIB_LIBRARY_FILENAME}_DALLOC    )
        endif()
    else()
        if ( (CMAKE_BUILD_TYPE STREQUAL "Debug") )
            set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}DBGREL                    )
        endif()
    endif()

    # Add features
    if ( ${ALIB_FEAT_SINGLETON_MAPPED} )
        set ( ALIB_LIBRARY_FILENAME    ${ALIB_LIBRARY_FILENAME}_MS    )
    endif()
    if ( ${ALIB_CHARACTERS_WIDE} )
        if (  ALIB_CHARACTERS_SIZEOF_WCHAR STREQUAL  "2" )
            set ( ALIB_LIBRARY_FILENAME    ${ALIB_LIBRARY_FILENAME}_WC2    )
        else()
            set ( ALIB_LIBRARY_FILENAME    ${ALIB_LIBRARY_FILENAME}_WC4    )
        endif()
    endif()
    if ( ${ALIB_FEAT_BOOST_REGEX} )
        set ( ALIB_LIBRARY_FILENAME    ${ALIB_LIBRARY_FILENAME}_BREGEX    )
    endif()
    if ( ${ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS} )
        set ( ALIB_LIBRARY_FILENAME    ${ALIB_LIBRARY_FILENAME}_BIJCHARS    )
    endif()
    if ( ${ALIB_FEAT_BOXING_BIJECTIVE_FLOATS} )
        set ( ALIB_LIBRARY_FILENAME    ${ALIB_LIBRARY_FILENAME}_BIJFLOATS    )
    endif()
    if ( ${ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS} )
        set ( ALIB_LIBRARY_FILENAME    ${ALIB_LIBRARY_FILENAME}_BIJINTS    )
    endif()



    # Add all module's names, prefixed by "DBG" if special debug flag is set
    if( NOT allModules )
        FOREACH(modName IN LISTS   ALIB_DISTRIBUTION)

            # MONOMEM debug mode?
            IF(     modName STREQUAL "MONOMEM" )
                if ( ALIB_DEBUG_MONOMEM AND ${ALIB_DEBUG} )
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_DBGMONOMEM )
                else()
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_MONOMEM    )
                endif()

            # CONTAINERS debug mode?
            ELSEIF( modName STREQUAL "CONTAINERS" )
                if ( ALIB_DEBUG_CONTAINERS AND ${ALIB_DEBUG} )
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_DBGCONTAINERS )
                else()
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_CONTAINERS    )
                endif()

            # STRINGS debug mode?
            ELSEIF( modName STREQUAL "STRINGS" )
                if ( ALIB_DEBUG_STRINGS AND ${ALIB_DEBUG} )
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_DBGSTRINGS )
                else()
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_STRINGS    )
                endif()

            # RESOURCES debug mode?
            ELSEIF( modName STREQUAL "CAMP" )
                if ( ALIB_DEBUG_RESOURCES AND ${ALIB_DEBUG} )
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_DBGCAMP )
                else()
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_CAMP    )
                endif()

            # ALOX: add non-default feature s
            ELSEIF( modName STREQUAL "ALOX" )
                set ( ALIB_LIBRARY_FILENAME             ${ALIB_LIBRARY_FILENAME}_ALOX       )
                if ( (CMAKE_BUILD_TYPE STREQUAL "Debug")  AND   (NOT ALOX_DBG_LOG) )
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}NDL         )
                endif()

                if ( NOT ALOX_REL_LOG )
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}NRL         )
                endif()
            ELSE()
                set ( ALIB_LIBRARY_FILENAME            ${ALIB_LIBRARY_FILENAME}_${modName}  )
            ENDIF()

        ENDFOREACH()

        # Remove all dependent modules from the name, which are not added in a dbg version:
        list( FIND   ALIB_DISTRIBUTION  "BOXING"            idx )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_SINGLETONS" ""  ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_CHARACTERS" ""  ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

        list( FIND   ALIB_DISTRIBUTION  "STRINGS"           idx )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_CHARACTERS" "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

        list( FIND   ALIB_DISTRIBUTION  "THREADS"           idx )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_STRINGS"    "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_TIME"       "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

        list( FIND   ALIB_DISTRIBUTION  "ENUMS"            idx )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_SINGLETONS" "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_STRINGS"    "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

        list( FIND   ALIB_DISTRIBUTION  "BITBUFFER"         idx )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_MONOMEM"    "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_ENUMS"      "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

        list( FIND   ALIB_DISTRIBUTION  "THREADMODEL"       idx )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_BOXING"     "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_CONTAINERS" "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_ENUMS"      "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_MONOMEM"    "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_THREADS"    "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

        list( FIND   ALIB_DISTRIBUTION  "CAMP"      idx       )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_BOXING"     "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_ENUMS"      "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_CONTAINERS" "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_MONOMEM"    "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_SINGLETONS" "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_STRINGS"    "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
            STRING(REPLACE "_TIME"       "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

        list( FIND   ALIB_DISTRIBUTION  "CONFIGURATION"     idx )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_CAMP"       "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

        list( FIND   ALIB_DISTRIBUTION  "ALOX"              idx )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_CONFIGURATION"       "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

        list( FIND   ALIB_DISTRIBUTION  "EXPRESSIONS"       idx )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_CAMP"       "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

        list( FIND   ALIB_DISTRIBUTION  "FILES"             idx )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_CAMP"       "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

        list( FIND   ALIB_DISTRIBUTION  "CLI"               idx )
        if( NOT idx LESS 0 )
            STRING(REPLACE "_CAMP"       "" ALIB_LIBRARY_FILENAME "${ALIB_LIBRARY_FILENAME}")
        endif()

    endif() # not all modules

endif()

# -------------------------------------------------------------------------------------------------
# Display result summary
# -------------------------------------------------------------------------------------------------
message( "ALib CMake Configuration:"                             )
IF( NOT allModules )
    message( "  Module Selection : ${ALIB_DISTRIBUTION}"           )
ELSE()
    message( "  Module Selection : All (${ALIB_DISTRIBUTION})"     )
ENDIF()

    message( "  Library filename : ${ALIB_LIBRARY_FILENAME}"      )
    message( "  Compiler ID      : ${CMAKE_CXX_COMPILER_ID}")

IF( NOT ALIB_CMAKE_VERBOSE )
    message( "  (For further details enable CMake variable 'ALIB_CMAKE_VERBOSE')"      )
ELSE()

    message( "  Source folder:        ${ALIB_SOURCE_DIR}"            )
    SET( result "" )
    LIST( APPEND result ${ALIB_INCLUDE_FILES} )
    LIST( APPEND result ${ALIB_SOURCE_FILES}  )
    LIST( SORT   result  )
    LIST( LENGTH result  length)
    message( "\n  List of header and source files (${length} items):"  )
    FOREACH( entry IN LISTS result  )
        STRING(REPLACE "${ALIB_SOURCE_DIR}/" "" entry ${entry} )
        message( "      ${entry}"          )
    ENDFOREACH()

    SET( result  "" )
    LIST( APPEND result ${ALIB_SYMBOLS} )
    LIST( SORT   result  )
    LIST( LENGTH result  length)
    message( "\n  Compiler definitions (${length} items):"  )
    FOREACH( entry IN LISTS result  )
        message( "      ${entry}"          )
    ENDFOREACH()
    SET( result  "" )

    SET( result  "" )
    LIST( APPEND result ${ALIB_SYMBOLS_UNUSED} )
    LIST( SORT   result  )
    LIST( LENGTH result  length)
    message( "\n  Compiler definitions NOT given/omitted (${length} items):"  )
    FOREACH( entry IN LISTS result  )
        message( "      ${entry}"          )
    ENDFOREACH()
    SET( result  "" )

    LIST( APPEND result ${ALIB_COMPILER_WARNINGS} )
    LIST( SORT   result  )
    LIST( LENGTH result  length)
    message( "\n  Compiler warnings (${length} items):"  )
    FOREACH( entry IN LISTS result  )
        message( "      ${entry}"          )
    ENDFOREACH()
    SET( result  "" )

    LIST( APPEND result ${ALIB_COMPILER_OPTIONS} )
    LIST( SORT   result  )
    LIST( LENGTH result  length)
    message( "\n  Compiler flags (${length} items):"  )
    FOREACH( entry IN LISTS result  )
        message( "      ${entry}"          )
    ENDFOREACH()
    SET( result  "" )

    LIST( APPEND result ${ALIB_COMPILER_FEATURES} )
    LIST( SORT   result  )
    LIST( LENGTH result  length)
    message( "\n  Compiler features (${length} items):"  )
    FOREACH( entry IN LISTS result  )
        message( "      ${entry}"          )
    ENDFOREACH()
    SET( result  "" )

    LIST( APPEND result ${ALIB_EXTERNAL_LIBS} )
    LIST( SORT   result  )
    LIST( LENGTH result  length)
    message( "\n  External libraries (${length} items):"  )
    FOREACH( entry IN LISTS result  )
        message( "      ${entry}"          )
    ENDFOREACH()
    SET( result  "" )

    LIST( APPEND result ${ALIB_LINKER_OPTIONS} )
    LIST( SORT   result  )
    LIST( LENGTH result  length)
    message( "\n  Linker flags (${length} items):"  )
    FOREACH( entry IN LISTS result  )
        message( "      ${entry}"          )
    ENDFOREACH()
    SET( result  "" )

    message( "\n"  )
ENDIF()

# -------------------------------------------------------------------------------------------------
# ALibSetCompilerAndLinker(target)
#
# Simple CMake function that sets
# - ALIB_SYMBOLS
# - ALIB_COMPILER_FEATURES
# - ALIB_COMPILER_OPTIONS
# - ALIB_COMPILER_WARNINGS
# - ALIB_LINKER_OPTIONS
#
# In addition, postion independent compile (-fPic) is enabled (for static libraries its default
# is off with CMake).
# -------------------------------------------------------------------------------------------------
function( ALibSetCompilerAndLinker  target )

    # compiler flags
    target_compile_features   ( ${target}    PRIVATE         ${ALIB_COMPILER_FEATURES}       )
    target_compile_options    ( ${target}    PRIVATE         ${ALIB_COMPILER_OPTIONS}        )
    target_compile_options    ( ${target}    PRIVATE         ${ALIB_COMPILER_WARNINGS}       )
    set_property              ( TARGET ${target}   PROPERTY POSITION_INDEPENDENT_CODE ON     )
    target_include_directories( ${target}    PUBLIC          ${ALIB_SOURCE_DIR}              )

    #definitions
    target_compile_definitions( ${target}    PUBLIC          ${ALIB_SYMBOLS}     )

    # linker flags
    IF( NOT "${ALIB_LINKER_OPTIONS}"  STREQUAL "" )
        set_target_properties ( ${target}    PROPERTIES  LINK_FLAGS     ${ALIB_LINKER_OPTIONS} )
    ENDIF()
    IF( NOT "${ALIB_EXTERNAL_LIBS}"  STREQUAL "" )
        target_link_libraries ( ${target}   PRIVATE ${ALIB_EXTERNAL_LIBS}                      )
    ENDIF()

endfunction()

# -------------------------------------------------------------------------------------------------
# Targets
# -------------------------------------------------------------------------------------------------
function( ALibAddStaticLibrary )
    # sources
    add_library                ( ALib_StaticLib    STATIC   ${ALIB_INCLUDE_FILES}  ${ALIB_SOURCE_FILES} )
    ALibSetCompilerAndLinker   ( ALib_StaticLib )
    set_target_properties      ( ALib_StaticLib    PROPERTIES  ARCHIVE_OUTPUT_NAME  ${ALIB_LIBRARY_FILENAME}  )
endfunction()

function( ALibAddSharedLibrary )
    add_library                ( ALib_SharedLib    SHARED          ${ALIB_INCLUDE_FILES}  ${ALIB_SOURCE_FILES} )
    ALibSetCompilerAndLinker   ( ALib_SharedLib )
    set_target_properties      ( ALib_SharedLib    PROPERTIES  LIBRARY_OUTPUT_NAME  ${ALIB_LIBRARY_FILENAME}  )

    if(WIN32)
        target_compile_definitions ( ALib_SharedLib     PRIVATE         "ALIB_API_IS_DLL"          )
    endif()
endfunction()

# --------------------------------------------------------------------------------------------------
# CMake debugging  Uncomment a line to have CMake summarize information
# --------------------------------------------------------------------------------------------------
#set(CMAKE_DEBUG_TARGET_PROPERTIES  ${CMAKE_DEBUG_TARGET_PROPERTIES} INCLUDE_DIRECTORIES )
#set(CMAKE_DEBUG_TARGET_PROPERTIES  ${CMAKE_DEBUG_TARGET_PROPERTIES} SOURCES )
#set(CMAKE_DEBUG_TARGET_PROPERTIES  ${CMAKE_DEBUG_TARGET_PROPERTIES} COMPILE_DEFINITIONS )
#set(CMAKE_DEBUG_TARGET_PROPERTIES  ${CMAKE_DEBUG_TARGET_PROPERTIES} COMPILE_OPTIONS )
#set(CMAKE_DEBUG_TARGET_PROPERTIES  ${CMAKE_DEBUG_TARGET_PROPERTIES} COMPILE_FEATURES )
#set(CMAKE_DEBUG_TARGET_PROPERTIES  ${CMAKE_DEBUG_TARGET_PROPERTIES} AUTOUIC_OPTIONS )
#set(CMAKE_DEBUG_TARGET_PROPERTIES  ${CMAKE_DEBUG_TARGET_PROPERTIES} POSITION_INDEPENDENT_CODE )
#set(CMAKE_DEBUG_TARGET_PROPERTIES  ${CMAKE_DEBUG_TARGET_PROPERTIES} CONTAINER_SIZE_REQUIRED )
#set(CMAKE_DEBUG_TARGET_PROPERTIES  ${CMAKE_DEBUG_TARGET_PROPERTIES} LIB_VERSION )

#set(CMAKE_VERBOSE_MAKEFILE ON)
