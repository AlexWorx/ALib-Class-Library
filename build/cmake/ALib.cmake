# #################################################################################################
#  ALib.cmake - CMake file for projects using ALib
#
#  Copyright 2015-2019 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
# #################################################################################################


# --------------------------------------------------------------------------------------------------
# checks
# --------------------------------------------------------------------------------------------------
    cmake_minimum_required( VERSION 3.3.0 )

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
# The variables are only set, if not alreay predefined prior to invoking this script.
# --------------------------------------------------------------------------------------------------

set( ALIB_VERSION                   "1903R0"                                            CACHE STRING
     "The ALib version. Not modifiable (will be overwritten on generation!)"        FORCE )

set( ALIB_VERSION_NO                "1903" )
set( ALIB_VERSION_REV               "0" )

if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set( defaultALIB_DEBUG   "On" )
else()
    set( defaultALIB_DEBUG   "Off" )
endif()


if( NOT DEFINED  ALIB_DEBUG )
    set( ALIB_DEBUG                     ${defaultALIB_DEBUG}                            CACHE   BOOL
         "Enable/disable ALib debug code. Defaults to true in debug compilations, otherwise to false." )
endif()

if( NOT DEFINED  ALIB_DEBUG_GLIB )
    set( ALIB_DEBUG_GLIB                "Off"                                           CACHE   BOOL
         "Defaults to false. If true, compiler symbols '_GLIBCXX_DEBUG', '_GLIBCXX_DEBUG_PEDANTIC' and '_GLIBCPP_CONCEPT_CHECKS' are set." )
endif()

if( NOT DEFINED  ALIB_AVOID_ANALYZER_WARNINGS )
    set( ALIB_AVOID_ANALYZER_WARNINGS   "Off"                                           CACHE   BOOL
         "Defaults to false. If true, minor code modifications are made to avoid unnecessary warnings with tools like 'valgrind'.")
endif()

if( NOT DEFINED  ALIB_COVERAGE_COMPILE )
    set( ALIB_COVERAGE_COMPILE         "Off"                                            CACHE   BOOL
         "Defaults to false. If true, option --coverag is added to GNU compiler command line.")
endif()

if ( $ENV{CLION_IDE} )
    set( tmp_default_val   "On" )
else()
    set( tmp_default_val   "Off" )
endif()
if( NOT DEFINED  ALIB_GDB_PP_SUPPRESS_CHILDREN )
    set( ALIB_GDB_PP_SUPPRESS_CHILDREN    ${tmp_default_val}                            CACHE   BOOL
         "Defaults to false except if CMake is run from within JetBrains CLion. If true, a corresponding symbol gets set in debug compilations which is detected by GDB pretty printer scripts provided with ALib.")
endif()
if( NOT DEFINED  ALIB_GDB_PP_FIND_POINTER_TYPES )
    set( ALIB_GDB_PP_FIND_POINTER_TYPES ${tmp_default_val}                              CACHE   BOOL
         "Defaults to false. If true, a corresponding symbol gets set in debug compilations which is detected by GDB pretty printer scripts provided with ALib.")
endif()

if( NOT DEFINED  ALIB_CMAKE_COTIRE )
    set( ALIB_CMAKE_COTIRE              "Off"                                           CACHE   BOOL
         "If true, CMake compilation tool 'cotire' (https://github.com/sakra/cotire/) is downloaded and may be used to speedup builds." )
endif()

if( NOT DEFINED  ALIB_PRECOMPILED_HEADER_DISABLED )
    set( ALIB_PRECOMPILED_HEADER        "Off"                                           CACHE   BOOL
         "If on, header file ’alib/alib_precompile.hpp' does not include any header files." )
endif()

if( NOT DEFINED  ALIB_CMAKE_VERBOSE )
    set( ALIB_CMAKE_VERBOSE              "Off"                                          CACHE   BOOL
         "If true, CMake generation runs will provide a detailed report." )
endif()


if( "SINGLETONS" IN_LIST ALIB_MODULES )
    if ( ${WIN32} )
        set( platformDefaultFor_SINGLETON_MAPPED   "On" )
    else()
        set( platformDefaultFor_SINGLETON_MAPPED   "Off" )
    endif()
    if( NOT DEFINED  ALIB_FEAT_SINGLETON_MAPPED )
        set( ALIB_FEAT_SINGLETON_MAPPED     ${platformDefaultFor_SINGLETON_MAPPED}      CACHE   BOOL
             "Defaults to true on Windows OS, which then selects code to implement class Singleton to work with multiple data segments, as imposed by the use of Windows DLLs.")
    endif()
endif()

if( "MEMORY" IN_LIST ALIB_MODULES )
    if( NOT DEFINED  ALIB_MEMORY_DEBUG )
        set( ALIB_MEMORY_DEBUG              "Off"                                       CACHE   BOOL
             "Defaults to false. Adds consistency checks and collection of statistics with ALib memory classes." )
    endif()
endif()


if( "BOXING" IN_LIST ALIB_MODULES )
    if( NOT DEFINED  ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS )
        set( ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS   "On"                            CACHE   BOOL
             "Defaults to true. If true, any C++ integral will be boxed to 'integer', respectively / 'uinteger' and only those can be unboxed.")
    endif()
    if( NOT DEFINED  ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS )
        set( ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS   "On"                           CACHE   BOOL
             "Defaults to true. If true, any C++ character type will be boxed to type 'character' and only those can be unboxed.")
    endif()
    if( NOT DEFINED  ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS )
        set( ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS      "On"                            CACHE   BOOL
             "Defaults to true. If true, type float will be boxed as double and can not be unboxed.")
    endif()
endif()

if( "STRINGS" IN_LIST ALIB_MODULES )
    if( NOT DEFINED  ALIB_STRINGS_DEBUG )
        set( ALIB_STRINGS_DEBUG             "Off"                                       CACHE   BOOL
             "Defaults to false. Adds consistency checks to ALib string classes. Useful when developing code to manipulate strings externally, i.e T_Append to specializations.")
    endif()

    if (NOT DEFINED ALIB_FEAT_BOOST_REGEX)
        set( ALIB_FEAT_BOOST_REGEX          "Off"                                       CACHE   BOOL
             "Defaults to false. If true, activates ALib classes that use boost regular expressions, for example strings::util::RegexMatcher. The corresponding boost library is searched and added to CMake variable ALIB_EXTERNAL_LIBS.")
    endif()

    if (NOT DEFINED ALIB_CHARACTERS_FORCE_NARROW)
        set( ALIB_CHARACTERS_FORCE_NARROW     "Off"                                     CACHE   BOOL
             "Defaults to false. If true, forces 1-byte characters to be the standard type for strings, independent from the platform preference.")
    endif()

    if (NOT DEFINED ALIB_CHARACTERS_FORCE_WIDE)
        set( ALIB_CHARACTERS_FORCE_WIDE     "Off"                                       CACHE   BOOL
             "Defaults to false. If true, forces wide characters to be the standard type for strings, independent from the platform preference. Still the character size (16 or 32 bit) is up to the platform preference.")
    endif()

    if (NOT DEFINED ALIB_CHARACTERS_FORCE_WIDE_2_BYTES)
        set( ALIB_CHARACTERS_FORCE_WIDE_2_BYTES    "Off"                                CACHE   BOOL
             "Defaults to false. If true, forces wide characters to be 2 bytes wide, instead of what the compiler proposes with built-in type definition 'wchar_t'.")
    endif()

    if (NOT DEFINED ALIB_CHARACTERS_FORCE_WIDE_4_BYTES)
        set( ALIB_CHARACTERS_FORCE_WIDE_4_BYTES    "Off"                                CACHE   BOOL
             "Defaults to false. If true, forces wide characters to be 4 bytes wide, instead of what the compiler proposes with built-in type definition 'wchar_t'.")
    endif()

endif()

if( "ALOX" IN_LIST ALIB_MODULES )

    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        set( defaultALOX_DBG_LOG   "On" )
    else()
        set( defaultALOX_DBG_LOG   "Off" )
    endif()

    set( ALOX_DBG_LOG                   ${defaultALOX_DBG_LOG}                          CACHE   BOOL
         "Enable/disable debug log statements. Defaults to true in debug compilations, otherwise to false." )

    set( ALOX_DBG_LOG_CI                "On"                                            CACHE   BOOL
         "Defaults to true. If set, caller information is used with debug log statements." )

    set( ALOX_REL_LOG                   "On"                                            CACHE   BOOL
         "Enable/disable release log statements. Defaults to true." )

    set( ALOX_REL_LOG_CI                "Off"                                           CACHE   BOOL
         "Defaults to false. If set, caller information is used even with release log statements (and even in release builds!)" )
endif()

# --------------------------------------------------------------------------------------------------
# compilation symbols
# --------------------------------------------------------------------------------------------------

# module selection
if( NOT allModules )
    SET( moduleList "" )
    LIST( APPEND moduleList    "ALOX;EXPRESSIONS;CONFIGURATION;CLI;SYSTEM" )
    LIST( APPEND moduleList    "REPORTS;EXCEPTIONS;STRINGFORMAT"           )
    LIST( APPEND moduleList    "RESOURCES;THREADS;STRINGS;BOXING"          )
    LIST( APPEND moduleList    "TIME;CHARACTERS;ENUMS;SINGLETONS;MEMORY"   )
    FOREACH( module IN LISTS moduleList )
        IF( module IN_LIST  ALIB_MODULES )
            list( APPEND  ALIB_COMPILATION_SYMBOLS    "ALIB_MODULE_${module}_ON"  )
        ENDIF()
    ENDFOREACH()
endif()



# debug
if( defaultALIB_DEBUG )
    if ( NOT ${ALIB_DEBUG} )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_DEBUG_OFF"         )
    endif()
else()
    if ( ${ALIB_DEBUG} )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_DEBUG_ON"          )
    endif()
endif()



if ( ${ALIB_DEBUG_GLIB} )
    list( APPEND ALIB_COMPILATION_SYMBOLS    "_GLIBCXX_DEBUG"
                                              "_GLIBCXX_DEBUG_PEDANTIC"
                                              "_GLIBCPP_CONCEPT_CHECKS"   )
endif()

if ( ${ALIB_AVOID_ANALYZER_WARNINGS} )
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_AVOID_ANALYZER_WARNINGS_ON" )
endif()

if ( ${ALIB_GDB_PP_SUPPRESS_CHILDREN} )
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_GDB_PP_SUPPRESS_CHILDREN"   )
endif()
if ( ${ALIB_GDB_PP_FIND_POINTER_TYPES} )
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_GDB_PP_FIND_POINTER_TYPES"  )
endif()


# precompiled header
if ( ${ALIB_PRECOMPILED_HEADER_DISABLED} )
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_PRECOMPILED_HEADER_DISABLED"  )
endif()

# ALib features
if( "SINGLETONS" IN_LIST ALIB_MODULES )
    if (platformDefaultFor_SINGLETON_MAPPED)
        if ( NOT ALIB_FEAT_SINGLETON_MAPPED )
            list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_SINGLETON_MAPPED_OFF"  )
        endif()
    else()
        if (     ALIB_FEAT_SINGLETON_MAPPED )
            list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_SINGLETON_MAPPED_ON"  )
        endif()
    endif()
endif()

if( "MEMORY" IN_LIST ALIB_MODULES )
    if ( ALIB_MEMORY_DEBUG )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_MEMORY_DEBUG_ON"  )
    endif()
endif()

if( "BOXING" IN_LIST ALIB_MODULES )
    if ( NOT ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS_OFF"  )
    endif()
    if ( NOT ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS_OFF"  )
    endif()
    if ( NOT ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS_OFF"  )
    endif()
endif()

if( "STRINGS" IN_LIST ALIB_MODULES )
    if ( ALIB_STRINGS_DEBUG )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_STRINGS_DEBUG_ON"  )
    endif()

    if ( ALIB_FEAT_BOOST_REGEX )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_BOOST_REGEX_ON"  )
    endif()

    if ( ALIB_CHARACTERS_FORCE_NARROW )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_CHARACTERS_FORCE_NARROW_ON" )
    endif()
    if ( ALIB_CHARACTERS_FORCE_WIDE )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_CHARACTERS_FORCE_WIDE_ON"   )
    endif()
    if ( ALIB_CHARACTERS_FORCE_WIDE_2_BYTES )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_CHARACTERS_FORCE_WIDE_2_BYTES_ON"   )
    endif()
    if ( ALIB_CHARACTERS_FORCE_WIDE_4_BYTES )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_CHARACTERS_FORCE_WIDE_4_BYTES_ON"   )
    endif()

endif()

if( "ALOX" IN_LIST ALIB_MODULES )

    if ( ${ALOX_DBG_LOG} )

        if( NOT defaultALOX_DBG_LOG )
            list( APPEND ALIB_COMPILATION_SYMBOLS        "ALOX_DBG_LOG_ON"      )
        endif()

        if ( NOT ${ALOX_DBG_LOG_CI} )
            list( APPEND ALIB_COMPILATION_SYMBOLS    "ALOX_DBG_LOG_CI_OFF"  )
        endif()

    else()

        if( defaultALOX_DBG_LOG )
            list( APPEND ALIB_COMPILATION_SYMBOLS        "ALOX_DBG_LOG_OFF"     )
        endif()

    endif()

    if ( ${ALOX_REL_LOG} )
        if ( ${ALOX_REL_LOG_CI} )
            list( APPEND ALIB_COMPILATION_SYMBOLS    "ALOX_REL_LOG_CI_ON"   )
        endif()
    else()
        list( APPEND ALIB_COMPILATION_SYMBOLS        "ALOX_REL_LOG_OFF"     )
    endif()
endif()


# --------------------------------------------------------------------------------------------------
# ALib Source File Definition
# --------------------------------------------------------------------------------------------------
include( ${CMAKE_CURRENT_LIST_DIR}/ALibSources.cmake )


# --------------------------------------------------------------------------------------------------
# External libraries
# --------------------------------------------------------------------------------------------------
if (  "THREADS"  IN_LIST ALIB_MODULES )
    find_package(Threads)
    list( APPEND  ALIB_EXTERNAL_LIBS  ${CMAKE_THREAD_LIBS_INIT} )
endif()

if ( ${ALIB_FEAT_BOOST_REGEX} )
    find_package( Boost REQUIRED COMPONENTS regex )

    if(Boost_FOUND)
        #recommended but does not work currently: "Boost::regex"
        list( APPEND  ALIB_EXTERNAL_LIBS  boost_regex )
    else()
        MESSAGE("Attention: Boost::regex requested, but library not found!")
    endif()

endif()

if(APPLE)
    list( APPEND ALIB_EXTERNAL_LIBS  "-framework Foundation")
endif()


# --------------------------------------------------------------------------------------------------
# A-Worx compiler features and flags
# --------------------------------------------------------------------------------------------------

if     ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU"    )

    # add -H to generate output "!/x" for use of precompiled header
    list( APPEND ALIB_COMPILER_WARNINGS   "-Wall"          )
    list( APPEND ALIB_COMPILER_WARNINGS   "-Wextra"        )
    list( APPEND ALIB_COMPILER_WARNINGS   "-Werror"        )
    #list( APPEND ALIB_COMPILER_WARNINGS   "-Weffc++"       )

    # add coverage flags to GCC
    if( ${ALIB_COVERAGE_COMPILE} )
        list( APPEND  AWORX_COMPILER_FLAGS "--coverage"     )
        list( APPEND  ALIB_LINKER_FLAGS   "--coverage"     )
    endif()

# Clang: We are using -Weverything, although this is not recommended. We think it should be
#        recommmended. ALib for example does not use old-style casts and explicitly cast each
#        and every type change! The benefit for ALib users is that ALib code can be used in very
#        strict build environmnets without using special warning flags.
#        Of-course, some very obvious things like C++98 compatibility warnings have to be
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

    if ( ${ALIB_CMAKE_COTIRE} )
        # needs to be off of due to "unity builds" of cotire
        list( APPEND ALIB_COMPILER_WARNINGS "-Wno-header-hygiene"      )
    endif()

    if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        list( APPEND ALIB_COMPILE_FLAGS   "-fno-limit-debug-info"      )
    endif()



#! NEVER TESTED YET !
elseif ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel"  )
    list( APPEND ALIB_COMPILER_WARNINGS "" )

#! NEVER TESTED YET !
elseif ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC"   )
    list( APPEND ALIB_COMPILER_WARNINGS   "/W4"    )
    list( APPEND ALIB_COMPILER_WARNINGS   "/WX"    )
    list( APPEND ALIB_COMPILER_WARNINGS   "/EHsc"  )
endif()

# --------------------------------------------------------------------------------------------------
# A-Worx linker features and flags
# --------------------------------------------------------------------------------------------------
if(APPLE)
    list( APPEND ALIB_LINKER_FLAGS        "-lObjc"     )
else()
    list( APPEND ALIB_LINKER_FLAGS        ""           )
endif()


# -------------------------------------------------------------------------------------------------
# Set filename of ALib library  (if not given in ALIB_LIBRARY_FILENAME)
# -------------------------------------------------------------------------------------------------
if ( NOT ALIB_LIBRARY_FILENAME )

    set( ALIB_LIBRARY_FILENAME  "alib_${ALIB_VERSION_NO}R${ALIB_VERSION_REV}" )

    if ( ${ALIB_DEBUG} )
        set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_DBG                    )
    endif()

    if( NOT allModules )

        # create copy of module list and remove all dependent modules
        SET( modules    ${ALIB_MODULES} )

        list( FIND   modules  "BOXING"            idx )
        if( NOT idx LESS 0 )
            LIST( REMOVE_ITEM  modules  "SINGLETONS"  )
        endif()

        list( FIND   modules  "STRINGS"           idx )
        if( NOT idx LESS 0 )
            LIST( REMOVE_ITEM  modules  "CHARACTERS"  )
        endif()

        list( FIND   modules  "THREADS"           idx )
        if( NOT idx LESS 0 )
            LIST( REMOVE_ITEM  modules  "STRINGS"     )
            LIST( REMOVE_ITEM  modules  "TIME"     )
        endif()

        list( FIND   modules  "RESOURCES"         idx )
        if( NOT idx LESS 0 )
            LIST( REMOVE_ITEM  modules  "STRINGS"     )
            LIST( REMOVE_ITEM  modules  "SINGLETONS"  )
            LIST( REMOVE_ITEM  modules  "MEMORY"  )
        endif()

        list( FIND   modules  "STRINGFORMAT"      idx )
        if( NOT idx LESS 0 )
            LIST( REMOVE_ITEM  modules  "RESOURCES"   )
            LIST( REMOVE_ITEM  modules  "BOXING"      )
            LIST( REMOVE_ITEM  modules  "ENUMS"       )
        endif()

        list( FIND   modules  "RESULTS"          idx )
        if( NOT idx LESS 0 )
            LIST( REMOVE_ITEM  modules  "STRINGFORMAT")
        endif()

        list( FIND   modules  "SYSTEM"            idx )
        if( NOT idx LESS 0 )
            LIST( REMOVE_ITEM  modules  "RESULTS"    )
        endif()

        list( FIND   modules  "CONFIGURATION"     idx )
        if( NOT idx LESS 0 )
            LIST( REMOVE_ITEM  modules  "SYSTEM"      )
        endif()

        list( FIND   modules  "ALOX"              idx )
        if( NOT idx LESS 0 )
            LIST( REMOVE_ITEM  modules  "CONFIGURATION")
            LIST( REMOVE_ITEM  modules  "THREADS"     )
        endif()

        list( FIND   modules  "EXPRESSIONS"       idx )
        if( NOT idx LESS 0 )
            LIST( REMOVE_ITEM  modules  "RESULTS"    )
        endif()

        list( FIND   modules  "CLI"               idx )
        if( NOT idx LESS 0 )
            LIST( REMOVE_ITEM  modules  "RESULTS"    )
        endif()



        FOREACH(modName IN LISTS   modules)

            # STRINGS: debug mode?
            IF(     modName STREQUAL "STRINGS" )
                if ( ALIB_STRINGS_DEBUG )
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_DBGSTRINGS )
                else()
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_STRINGS    )
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
    endif()

endif()

# -------------------------------------------------------------------------------------------------
# Display result summary
# -------------------------------------------------------------------------------------------------
message( "ALib CMake Configuration:"                             )
IF( NOT allModules )
    message( "  Module Selection:     ${ALIB_MODULES}"           )
ELSE()
    message( "  Module Selection:     All (${ALIB_MODULES})"     )
ENDIF()

message( "  Lbrary filename:      ${ALIB_LIBRARY_FILENAME}"      )
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
        string(REPLACE "${ALIB_SOURCE_DIR}/" "" entry ${entry} )
        message( "      ${entry}"          )
    ENDFOREACH()
    SET( result  "" )
    LIST( APPEND result ${ALIB_COMPILATION_SYMBOLS} )
    LIST( SORT   result  )
    LIST( LENGTH result  length)
    message( "\n  Compiler definitions (${length} items):"  )
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

    LIST( APPEND result ${ALIB_COMPILE_FLAGS} )
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

    LIST( APPEND result ${ALIB_LINKER_FLAGS} )
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
# Simple CMake functin that sets
# - ALIB_COMPILATION_SYMBOLS
# - ALIB_COMPILER_FEATURES
# - ALIB_COMPILE_FLAGS
# - ALIB_COMPILER_WARNINGS
# - ALIB_LINKER_FLAGS
#
# In addition, postion independent compile (-fPic) is enabled (for static libraries its default
# is off with CMake).
# -------------------------------------------------------------------------------------------------
function( ALibSetCompilerAndLinker  target )

    # compiler flags
    target_compile_features   ( ${target}    PRIVATE         ${ALIB_COMPILER_FEATURES}       )
    target_compile_options    ( ${target}    PRIVATE         ${ALIB_COMPILE_FLAGS}           )
    target_compile_options    ( ${target}    PRIVATE         ${ALIB_COMPILER_WARNINGS}       )
    set_property              ( TARGET ${target}   PROPERTY POSITION_INDEPENDENT_CODE ON     )

    target_include_directories( ${target}    PUBLIC          ${ALIB_SOURCE_DIR}              )

    #definitions
    target_compile_definitions( ${target}    PUBLIC          ${ALIB_COMPILATION_SYMBOLS}     )

    # linker flags

    IF( NOT "${ALIB_LINKER_FLAGS}"  STREQUAL "" )
        set_target_properties     ( ${target}    PROPERTIES  LINK_FLAGS     ${ALIB_LINKER_FLAGS} )
    ENDIF()
    IF( NOT "${ALIB_EXTERNAL_LIBS}"  STREQUAL "" )
        target_link_libraries     ( ${target}    ${ALIB_EXTERNAL_LIBS}                           )
    ENDIF()

endfunction()

# -------------------------------------------------------------------------------------------------
# ALibSetCotire(target)
#
# If global variable \b ALIB_CMAKE_COTIRE is set and \ctrue, then the given project is setup to use
# cotire.
# -------------------------------------------------------------------------------------------------
function( ALibSetCotire  target )

    if ( ${ALIB_CMAKE_COTIRE} )

        # use multiple processor cores
        set_target_properties( ${target}  PROPERTIES COTIRE_UNITY_SOURCE_MAXIMUM_NUMBER_OF_INCLUDES "-j" )

        # header for precomp is "alib/alib_precompile.hpp"
        set_target_properties( ${target}  PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT  "${ALIB_SOURCE_DIR}/alib/alib_precompile.hpp" )

        # add cotire to projects
        cotire               ( ${target} )

    endif()
endfunction()


# -------------------------------------------------------------------------------------------------
# Targets
# -------------------------------------------------------------------------------------------------

function( ALibAddStaticLibrary )
    # sources
    add_library                ( ALib_StaticLib                    ${ALIB_INCLUDE_FILES}  ${ALIB_SOURCE_FILES} )
    ALibSetCompilerAndLinker   ( ALib_StaticLib )
    set_target_properties      ( ALib_StaticLib    PROPERTIES  ARCHIVE_OUTPUT_NAME  ${ALIB_LIBRARY_FILENAME}  )
    ALibSetCotire              ( ALib_StaticLib )
endfunction()

function( ALibAddSharedLibrary )
    add_library                ( ALib_SharedLib    SHARED          ${ALIB_INCLUDE_FILES}  ${ALIB_SOURCE_FILES} )
    ALibSetCompilerAndLinker   ( ALib_SharedLib )
    set_target_properties      ( ALib_SharedLib    PROPERTIES  LIBRARY_OUTPUT_NAME  ${ALIB_LIBRARY_FILENAME}  )

    if(WIN32)
        target_compile_definitions ( ALib_SharedLib     PRIVATE         "ALIB_API_IS_DLL"          )
        target_compile_definitions ( ALib_SharedLib     PRIVATE         "UNICODE"          )
    endif()

    ALibSetCotire              ( ALib_SharedLib )
endfunction()

# -------------------------------------------------------------------------------------------------
# Precompiled headers and unity build with 'cotire' CMake script.
#    More Info at: https://github.com/sakra/cotire/
#
# Note:
#   To enable/disable change CMake cache variable ALIB_CMAKE_COTIRE.
#   To change the variable permanently (on clean cmake builds), set the variable prior
#   to invoking this script.
# -------------------------------------------------------------------------------------------------

# download cotire (once)
if ( ${ALIB_CMAKE_COTIRE} )
    CheckDownloadCotire()
    # the include itself has to remain on this "source/scope level"
    # (aka: must not be done in the function/script above)
    include(${CMAKE_CURRENT_LIST_DIR}/cotire.cmake)
endif()



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


