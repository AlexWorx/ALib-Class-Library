# #################################################################################################
#  ALib.cmake - CMake file for projects using ALib
#
#  Copyright 2015-2018 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
# #################################################################################################

    include( ${CMAKE_CURRENT_LIST_DIR}/tools/ALibCMakeTools.cmake )

# --------------------------------------------------------------------------------------------------
# checks
# --------------------------------------------------------------------------------------------------

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
        set( ALIB_BASE_DIR                  ${temp}                                         CACHE   PATH
             "The base path to ALib containing source code, project files, tools, docs, etc.")
    endif()


    # check
    if (NOT EXISTS "${ALIB_BASE_DIR}/src/alib/lang" )
        message( FATAL_ERROR "ALib.cmake: Can't read sources in ALIB_BASE_DIR= ${ALIB_BASE_DIR}" )
        return()
    endif()


    # build type defaults to "Debug"
    if ( "${CMAKE_BUILD_TYPE}" STREQUAL ""    )
        set( CMAKE_BUILD_TYPE "Debug" )
    endif()

    MESSAGE( STATUS "Build type: ${CMAKE_BUILD_TYPE}" )



# --------------------------------------------------------------------------------------------------
# ALib Module Resolution and source file definition
# --------------------------------------------------------------------------------------------------

    include( ${CMAKE_CURRENT_LIST_DIR}/ALibModules.cmake )
    include( ${CMAKE_CURRENT_LIST_DIR}/ALibSources.cmake )


# --------------------------------------------------------------------------------------------------
# set cache variables
# The variables are only set, if not alreay predefined prior to invoking this script.
# --------------------------------------------------------------------------------------------------

set( ALIB_VERSION                   "1805R0"                                        CACHE STRING
     "The ALib version. Not modifiable (will be overwritten on generation!)"        FORCE )

set( ALIB_VERSION_NO                "1805" )
set( ALIB_VERSION_REV               "0" )

if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set( tmp_default_val   "On" )
else()
    set( tmp_default_val   "Off" )
endif()


if( NOT DEFINED  ALIB_DEBUG )
    set( ALIB_DEBUG                     ${tmp_default_val}                          CACHE   BOOL
         "Enable/disable ALib debug code. Defaults to true in debug compilations, otherwise to false." )
endif()

if( NOT DEFINED  ALIB_DEBUG_GLIB )
    set( ALIB_DEBUG_GLIB                "Off"                                       CACHE   BOOL
         "Defaults to false. If true, compiler symbols '_GLIBCXX_DEBUG', '_GLIBCXX_DEBUG_PEDANTIC' and '_GLIBCPP_CONCEPT_CHECKS' are set." )
endif()

if( NOT DEFINED  ALIB_AVOID_ANALYZER_WARNINGS )
    set( ALIB_AVOID_ANALYZER_WARNINGS   "Off"                                       CACHE   BOOL
         "Defaults to false. If true, minor code modifications are made to avoid unnecessary warnings with tools like 'valgrind'.")
endif()

if( NOT DEFINED  ALIB_COVERAGE_COMPILE )
    set( ALIB_COVERAGE_COMPILE         "Off"                                       CACHE   BOOL
         "Defaults to false. If true, option --coverag is added to GNU compiler command line.")
endif()

if ( $ENV{CLION_IDE} )
    set( tmp_default_val   "On" )
else()
    set( tmp_default_val   "Off" )
endif()
if( NOT DEFINED  ALIB_GDB_PP_SUPPRESS_CHILDREN )
    set( ALIB_GDB_PP_SUPPRESS_CHILDREN    ${tmp_default_val}                        CACHE   BOOL
         "Defaults to false. If true, a corresponding symbol gets set in debug compilations which is detected by GDB pretty printer scripts provided with ALib.")
endif()
if( NOT DEFINED  ALIB_GDB_PP_FIND_POINTER_TYPES )
    set( ALIB_GDB_PP_FIND_POINTER_TYPES ${tmp_default_val}                          CACHE   BOOL
         "Defaults to false. If true, a corresponding symbol gets set in debug compilations which is detected by GDB pretty printer scripts provided with ALib.")
endif()

if( NOT DEFINED  ALIB_CMAKE_COTIRE )
    set( ALIB_CMAKE_COTIRE              "Off"                                       CACHE   BOOL
         "If true, CMake compilation tool 'cotire' (https://github.com/sakra/cotire/) is downloaded and may be used to speedup builds." )
endif()


if( modSINGLETON )
    if ( ${WIN32} )
        set( tmp_default_val   "On" )
    else()
        set( tmp_default_val   "Off" )
    endif()
    if( NOT DEFINED  ALIB_FEAT_SINGLETON_MAPPED )
        set( ALIB_FEAT_SINGLETON_MAPPED     ${tmp_default_val}                          CACHE   BOOL
             "Defaults to true on Windows OS, which then selects code to implement class Singleton to work with multiple data segments, as imposed by the use of Windows DLLs.")
    endif()
endif()

if( modBOXING )
    if( NOT DEFINED  ALIB_FEAT_BOXING_FTYPES )
        set( ALIB_FEAT_BOXING_FTYPES        "On"                                        CACHE   BOOL
             "Defaults to true. If false, built-in boxing of fundamental types is switched off. Allowed to be switched off only in 'ALib Boxing' module distribution!")
    endif()
    if( NOT DEFINED  ALIB_FEAT_BOXING_STD_VECTOR )
        set( ALIB_FEAT_BOXING_STD_VECTOR    "On"                                        CACHE   BOOL
             "Defaults to true. If false, built-in boxing of std::vector objects (to array types) is switched off. ")
    endif()
endif()

if( modSTRINGS )
    if( NOT DEFINED  ALIB_DEBUG_STRINGS )
        set( ALIB_DEBUG_STRINGS             "Off"                                       CACHE   BOOL
             "Defaults to false. Adds consistency checks to ALib string classes. Useful when developing code to manipulate strings externally, i.e T_Apply to specializations.")
    endif()

    if (NOT DEFINED ALIB_FEAT_BOOST_REGEX)
        set( ALIB_FEAT_BOOST_REGEX          "Off"                                       CACHE   BOOL
             "Defaults to false. If true, activates ALib classes that use boost regular expressions, for example strings::util::RegexMatcher. The corresponding boost library is searched and added to CMake variable ALIB_EXTERNAL_LIBS.")
    endif()

    if (NOT DEFINED ALIB_FORCE_NARROW_STRINGS)
        set( ALIB_FORCE_NARROW_STRINGS     "Off"                                       CACHE   BOOL
             "Defaults to false. If true, forces narrow strings to be the standard, independent from the platform preference.")
    endif()

    if (NOT DEFINED ALIB_FORCE_WIDE_STRINGS)
        set( ALIB_FORCE_WIDE_STRINGS     "Off"                                         CACHE   BOOL
             "Defaults to false. If true, forces wide strings to be the standard, independent from the platform preference.")
    endif()
endif()

if( modCORE OR modCONFIGURATION)
    if( NOT DEFINED  ALIB_FEAT_THREADS )
        set( ALIB_FEAT_THREADS              "On"                                        CACHE   BOOL
             "Defaults to true. If false, multi-threading support is removed from ALib. Thread/Lock classes still exist and compile but are not effective.")
    endif()
endif()


if( modALOX )

    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        set( tmp_default_val   "On" )
    else()
        set( tmp_default_val   "Off" )
    endif()

    set( ALOX_DBG_LOG                   ${tmp_default_val}                              CACHE   BOOL
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

if ( ${ALIB_DEBUG} )
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_DEBUG_ON"          )
else()
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_DEBUG_OFF"         )
endif()

if ( ${ALIB_DEBUG_GLIB} )
    list( APPEND ALIB_COMPILATION_SYMBOLS    "_GLIBCXX_DEBUG"
                                              "_GLIBCXX_DEBUG_PEDANTIC"
                                              "_GLIBCPP_CONCEPT_CHECKS" )
endif()

if ( ${ALIB_AVOID_ANALYZER_WARNINGS} )
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_AVOID_ANALYZER_WARNINGS_ON"  )
else()
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_AVOID_ANALYZER_WARNINGS_OFF" )
endif()

if ( ${ALIB_GDB_PP_SUPPRESS_CHILDREN} )
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_GDB_PP_SUPPRESS_CHILDREN"  )
endif()
if ( ${ALIB_GDB_PP_FIND_POINTER_TYPES} )
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_GDB_PP_FIND_POINTER_TYPES"  )
endif()



if( modSINGLETON )
    if ( ${ALIB_FEAT_SINGLETON_MAPPED} )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_SINGLETON_MAPPED_ON"   )
    else()
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_SINGLETON_MAPPED_OFF"  )
    endif()
endif()

if( modBOXING )
    if ( ${ALIB_FEAT_BOXING_FTYPES} )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_BOXING_FTYPES_ON"   )
    else()
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_BOXING_FTYPES_OFF"  )
    endif()
    if ( ${ALIB_FEAT_BOXING_STD_VECTOR} )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_BOXING_STD_VECTOR_ON"   )
    else()
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_BOXING_STD_VECTOR_OFF"  )
    endif()
endif()

if( modSTRINGS )
    if ( ${ALIB_DEBUG_STRINGS} )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_DEBUG_STRINGS_ON"  )
    else()
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_DEBUG_STRINGS_OFF" )
    endif()

    if ( ${ALIB_FEAT_BOOST_REGEX} )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_BOOST_REGEX_ON"   )
    else()
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_BOOST_REGEX_OFF"  )
    endif()

    if ( ${ALIB_FORCE_NARROW_STRINGS} )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_NARROW_STRINGS_ON"    )
    endif()
    if ( ${ALIB_FORCE_WIDE_STRINGS} )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_NARROW_STRINGS_OFF"   )
    endif()

endif()

if( modCORE OR modCONFIGURATION )
    if ( ${ALIB_FEAT_THREADS} )
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_THREADS_ON"   )
    else()
        list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_FEAT_THREADS_OFF"  )
    endif()
endif()


if( modALOX )
    if ( ${ALOX_DBG_LOG} )
        list( APPEND ALIB_COMPILATION_SYMBOLS        "ALOX_DBG_LOG_ON"      )
        if ( ${ALOX_DBG_LOG_CI} )
            list( APPEND ALIB_COMPILATION_SYMBOLS    "ALOX_DBG_LOG_CI_ON"   )
        else()
            list( APPEND ALIB_COMPILATION_SYMBOLS    "ALOX_DBG_LOG_CI_OFF"  )
        endif()
    else()
        list( APPEND ALIB_COMPILATION_SYMBOLS        "ALOX_DBG_LOG_OFF"     )
    endif()

    if ( ${ALOX_REL_LOG} )
        list( APPEND ALIB_COMPILATION_SYMBOLS        "ALOX_REL_LOG_ON"      )
        if ( ${ALOX_REL_LOG_CI} )
            list( APPEND ALIB_COMPILATION_SYMBOLS    "ALOX_REL_LOG_CI_ON"   )
        else()
            list( APPEND ALIB_COMPILATION_SYMBOLS    "ALOX_REL_LOG_CI_OFF"  )
        endif()
    else()
        list( APPEND ALIB_COMPILATION_SYMBOLS        "ALOX_REL_LOG_OFF"     )
    endif()
endif()


# --------------------------------------------------------------------------------------------------
# External libraries
# --------------------------------------------------------------------------------------------------
if ( ${ALIB_FEAT_THREADS} )

#! [DOXYGEN_CMAKE_FIND_THREADS]
    find_package(Threads)
    list( APPEND  ALIB_EXTERNAL_LIBS  ${CMAKE_THREAD_LIBS_INIT} )
#! [DOXYGEN_CMAKE_FIND_THREADS]

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
    list( APPEND ALIB_LINKER_FLAGS        " "          )
endif()


# -------------------------------------------------------------------------------------------------
# Set filename of ALib library  (if not given in ALIB_LIBRARY_FILENAME)
# -------------------------------------------------------------------------------------------------
if ( NOT ALIB_LIBRARY_FILENAME )
    set( ALIB_LIBRARY_FILENAME  "alib_${ALIB_VERSION_NO}R${ALIB_VERSION_REV}" )

    if ( ${ALIB_DEBUG} )
        set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_DBG        )
    endif()

    if( NOT modALL )
        set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_          )
        set ( tempMainModuleSet  False)

        if( modEXPRESSIONS )
            set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}Expr  )
            set ( tempMainModuleSet  True)
        endif()

        if( modCLI )
            set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}Cli   )
            set ( tempMainModuleSet  True)
        endif()

        if( modALOX )
            set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}Alox  )
            set ( tempMainModuleSet  True)
        endif()


        if( NOT tempMainModuleSet)
            if( modCONFIGURATION )
                set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}Cfg   )
                set ( tempMainModuleSet  True)
            endif()
            if( modCORE )
                set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}Core  )
                set ( tempMainModuleSet  True)
            endif()

            if( NOT tempMainModuleSet)
                if( modSTRINGS )
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}Str   )
                endif()
                if( modBOXING )
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}Box   )
                endif()
                if( modSINGLETON )
                    set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}Sng   )
                endif()
            endif()
        endif()
    endif()



    if( modSTRINGS )
        if ( ${ALIB_DEBUG_STRINGS} )
            set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_STRDBG     )
        endif()
    endif()

    if( modCORE OR modCONFIGURATION )
        if ( NOT ${ALIB_FEAT_THREADS} )
            set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_NOTHRD     )
        endif()
    endif()

    if( modALOX )
        if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
            if ( NOT ${ALIB_DBG_LOG} )
                set ( ALIB_LIBRARY_FILENAME     ${ALIB_LIBRARY_FILENAME}_NODBGLOG   )
            endif()
        endif()

        if ( NOT ${ALIB_REL_LOG} )
            set ( ALIB_LIBRARY_FILENAME         ${ALIB_LIBRARY_FILENAME}_NORELLOG   )
        endif()
    endif()


    #message("ALib library file name: ${ALIB_LIBRARY_FILENAME}")
    #ALibDumpCMakeStatus()
endif()

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
    set_target_properties     ( ${target}    PROPERTIES  LINK_FLAGS     ${ALIB_LINKER_FLAGS} )
    target_link_libraries     ( ${target}    ${ALIB_EXTERNAL_LIBS}                           )

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

        # header for precomp is "alib/alib.hpp"
        set_target_properties( ${target}  PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT  "${ALIB_SOURCE_DIR}/alib/alib.hpp" )

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


