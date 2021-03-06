# #################################################################################################
#  ALibTools.cmake - CMake Tools for projects using ALib
#
#  Copyright 2015-2019 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Note:
#    This CMake file is included by "ALib.cmake" automatically and is not intended for manual
#    inclusion. Its contents was separated into a separated cmake file solely for clarity.
# #################################################################################################

# -------------------------------------------------------------------------------------------------
# Collects resource files and add copy commands to a new target.
# Then sets a dependency of the main target to this new resource target.
#
# This methods may be used when resource files like texts, json data or images have to be copied
# to the target build directory. The use is simple and done as follows:
#
# 1. Create a LIST variable that holds one or more directory names with file wildcards, used for
#    "globbing" the resource files, for example:
#
#       list( append MY_PROJECT_RESOURCE_FILES     ../../resources/*    )
#
# 2. After the executable (or library) is defined with cmake (add_executable or add_library),
#    insert the following invocation of this method:
#
#       AddResourceTarget( my_resources_target   my_target   ${MY_PROJECT_RESOURCE_FILES})
#
#    with the first parameter being an arbitrary name for the resource target to create.
#
# 3. In case of using "cotire", the unity projects need to receive the information of the added
#    project, because cotire today can't detect inter-project dependencies:
#
#       add_dependencies   ( my_target_unity          my_resources_target            )
# -------------------------------------------------------------------------------------------------
FUNCTION( AddResourceTarget  resourceProjectName  targetName resourceFileFilters)

    #message(STATUS "### AddResourceTarget() ###")
    #message(STATUS "resourceProjectName   : ${resourceProjectName}")
    #message(STATUS "targetName            : ${targetName}")
    #message(STATUS "resourceFilter        : ${resourceFilter}")

    FOREACH(_resourceFileFilter IN LISTS resourceFileFilters)
        file( GLOB_RECURSE   resourceList    ${_resourceFileFilter}    )
    ENDFOREACH()
    #message(STATUS "resourceList          : ${resourceList}")

    FOREACH(_resourceFile IN LISTS resourceList)

        #get filename and create destination file
        get_filename_component( _destFile   ${_resourceFile} NAME)
        set                   ( _destFile   ${CMAKE_CURRENT_BINARY_DIR}/${_destFile}  )
        list ( APPEND   _destFileList  ${_destFile} )

        #add copy command
        add_custom_command( OUTPUT "${_destFile}"
                COMMAND ${CMAKE_COMMAND} -E copy "${_resourceFile}" "${_destFile}"
                COMMENT "Copying resource to ${_destFile}"                             )
    ENDFOREACH()

    # create the copy target
    add_custom_target(${resourceProjectName} ALL DEPENDS "${_destFileList}")

    # add a dependency to the main target
    add_dependencies ( ${targetName}  ${resourceProjectName}  )

ENDFUNCTION(AddResourceTarget)

# -------------------------------------------------------------------------------------------------
# Removes duplicates from a given LIST and collects those found in second list. The list of
# found duplicates may contain duplicates as well, if entries had been trippled before.
# In other words, the sum of entries of the modified input list and entries in the result list
# is the same as the number of original entries.
#
# Note that both input parameters have to give the name of variables:
# - "sourceListName" denotes the name of an existing variable in the calling scope which is
#   modified.
# - "duplicateListName" denotes the name of an new variable that is created in the calling scope.
#   In the case it previously existed already, it will be overwritten.
#
# ===============================================================================================
# Recipe for modifying existing variables of the, and creating new ones in the parent scope
# -------------------------------------------------------------------------------------------------
# CMake is not easy to understand for humans. Therefore, two recepies should be given here, as
# it took us a while to understand that (or rather: to make it work).
#
# 1. How to create a variable in the parent scope:
#    - Have a function parameter that specifies the name (!) of the variable.
#    - Use an own local variable to create the result value.
#    - At the end, before the function returns, assign the local variable into the gloabl
#      scope using parameter  PARENT_SCOPE of the SET command.
#
#    Here is a sample:
#
#             FUNCTION( SetHello  resultVarName )
#                   SET( resultVar  "Hello World" )
#                   SET( ${resultVarName}  ${resultVar}   PARENT_SCOPE)
#             ENDFUNCTION( SetHello )
#
# 2. How to modify an existing variable from the parent scope:
#    - Have a function parameter that specifies the name (!) of the existing variable.
#    - Copy the contents of the existing variable into a local one. This has to be done with
#      a nested ${} operator! (Yes, when you think about it...)
#    - Do modifications only with the local copy (e.g. add or remove list entries)
#    - At the end, before the function returns, assign the local variable into the gloabl
#      scope using parameter  PARENT_SCOPE of the SET command.
#
#    Here is a sample:
#
#             FUNCTION( AddHello  existingVarName )
#                   SET( localVar ${${existingVarName}} )
#                   SET( localVar  "Hello World")
#                   SET( ${existingVarName}  ${localVar}   PARENT_SCOPE)
#             ENDFUNCTION( AddHello )
#
# -------------------------------------------------------------------------------------------------
FUNCTION( CollectAndRemoveDuplicates  sourceListVariableName  duplicateListVariableName )
    #message(STATUS "******************  CollectAndRemoveDuplicates START ***********************" )

    # copy source list to local list
    SET( sourceList ${${sourceListVariableName}} )

    # copy list to duplicates
    SET( duplicates  ${sourceList} )

    # not empty?
    LIST( LENGTH  sourceList  qtySource )
    IF( ${qtySource} GREATER  0  )

        # remove duplicates from source
        LIST( REMOVE_DUPLICATES  sourceList )

        # remove each in source from duplicates
        FOREACH( entry IN LISTS sourceList  )
            LIST(FIND duplicates ${entry} idx )
            IF( ${idx} EQUAL -1  )
                message( ERROR " Internal script error in CollectAndRemoveDuplicates: Item not found in original list." )
            ENDIF()
                LIST(REMOVE_AT duplicates ${idx})
        ENDFOREACH()
    ENDIF()

    # Transfer local variables back to parent scope
    SET( ${sourceListVariableName}     ${sourceList}   PARENT_SCOPE)
    SET( ${duplicateListVariableName}  ${duplicates}   PARENT_SCOPE)

    #message(STATUS "******************  CollectAndRemoveDuplicates END  ************************" )
ENDFUNCTION(CollectAndRemoveDuplicates)


FUNCTION( DumpList listVariableName  )

    LIST( LENGTH  ${listVariableName}  length )
    message(STATUS "Entries in list '${listVariableName}': ${length}" )

    MATH(EXPR maxIdx "${length} - 1" )
    FOREACH( idx RANGE 0 ${maxIdx}   )
        list(GET ${listVariableName} ${idx}  entry )
        message( STATUS "   ${idx}:  ${entry}" )
    ENDFOREACH()

ENDFUNCTION( DumpList )


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
FUNCTION (CheckDownloadCotire)
    set( _cotireFileName ${CMAKE_CURRENT_LIST_DIR}/cotire.cmake )
    set( _cotireFileUrl  "http://raw.githubusercontent.com/sakra/cotire/master/CMake/cotire.cmake" )

    if( NOT EXISTS  ${_cotireFileName} )
        message( STATUS "Cotire not found. Trying to download ${_cotireFileName} from ${_cotireFileUrl}..." )
        file( DOWNLOAD  ${_cotireFileUrl}  ${_cotireFileName} STATUS _status)
        message( STATUS "Download status: ${_status}" )

        if( NOT EXISTS  ${_cotireFileName} )
            message( "...Error: Download of 'cotire' failed. Continuing without cotire." )
            set( ALIB_CMAKE_COTIRE "Off" )
        else()
            message( STATUS "... cotire downloaded successfully." )
        endif()
    endif()

ENDFUNCTION()


# --------------------------------------------------------------------------------------------------
# ALibDumpCMakeStatus() Writes out important CMake variables
# Taken from:  http://www.cmake.org/Wiki/CMake_Useful_Variables/Logging_Useful_Variables
# --------------------------------------------------------------------------------------------------
FUNCTION (ALibDumpCMakeStatus)

    # /*	C++ comment style not allowed	*/
    MESSAGE( STATUS "\nALibDumpCMakeStatus:" )


    # if you are building in-source, this is the same as CMAKE_SOURCE_DIR, otherwise
    # this is the top level directory of your build tree
    MESSAGE( STATUS "  CMAKE_BINARY_DIR:                  " ${CMAKE_BINARY_DIR} )

    # if you are building in-source, this is the same as CMAKE_CURRENT_SOURCE_DIR, otherwise this
    # is the directory where the compiled or generated files from the current CMakeLists.txt will go to
    MESSAGE( STATUS "  CMAKE_CURRENT_BINARY_DIR:          " ${CMAKE_CURRENT_BINARY_DIR} )

    # this is the directory, from which cmake was started, i.e. the top level source directory
    MESSAGE( STATUS "  CMAKE_SOURCE_DIR:                  " ${CMAKE_SOURCE_DIR} )

    # this is the directory where the currently processed CMakeLists.txt is located in
    MESSAGE( STATUS "  CMAKE_CURRENT_SOURCE_DIR:          " tools)

    # contains the full path to the top level directory of your build tree
    MESSAGE( STATUS "  PROJECT_BINARY_DIR:                " ${PROJECT_BINARY_DIR} )

    # contains the full path to the root of your project source directory,
    # i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command
    MESSAGE( STATUS "  PROJECT_SOURCE_DIR:                " ../../.private/build/cmake/alib_devtest)

    # set this variable to specify a common place where CMake should put all executable files
    # (instead of CMAKE_CURRENT_BINARY_DIR)
    MESSAGE( STATUS "  EXECUTABLE_OUTPUT_PATH:            " ${EXECUTABLE_OUTPUT_PATH} )

    # set this variable to specify a common place where CMake should put all libraries
    # (instead of CMAKE_CURRENT_BINARY_DIR)
    MESSAGE( STATUS "  LIBRARY_OUTPUT_PATH:               " ${LIBRARY_OUTPUT_PATH} )

    # tell CMake to search first in directories listed in CMAKE_MODULE_PATH
    # when you use FIND_PACKAGE() or INCLUDE()
    MESSAGE( STATUS "  CMAKE_MODULE_PATH:                 " ${CMAKE_MODULE_PATH} )

    # this is the complete path of the cmake which runs currently (e.g. /usr/local/bin/cmake)
    MESSAGE( STATUS "  CMAKE_COMMAND:                     " ${CMAKE_COMMAND} )

    # this is the CMake installation directory
    MESSAGE( STATUS "  CMAKE_ROOT:                        " ${CMAKE_ROOT} )

    # this is the filename including the complete path of the file where this variable is used.
    MESSAGE( STATUS "  CMAKE_CURRENT_LIST_FILE:           " ALibCMakeTools.cmake)

    # this is linenumber where the variable is used
    MESSAGE( STATUS "  CMAKE_CURRENT_LIST_LINE:           " ${CMAKE_CURRENT_LIST_LINE} )

    # this is used when searching for include files e.g. using the FIND_PATH() command.
    MESSAGE( STATUS "  CMAKE_INCLUDE_PATH:                " ${CMAKE_INCLUDE_PATH} )

    # this is used when searching for libraries e.g. using the FIND_LIBRARY() command.
    MESSAGE( STATUS "  CMAKE_LIBRARY_PATH:                " ${CMAKE_LIBRARY_PATH} )

    # the complete system name, e.g. "Linux-2.4.22", "FreeBSD-5.4-RELEASE" or "Windows 5.1"
    MESSAGE( STATUS "  CMAKE_SYSTEM:                      " ${CMAKE_SYSTEM} )

    # the short system name, e.g. "Linux", "FreeBSD" or "Windows"
    MESSAGE( STATUS "  CMAKE_SYSTEM_NAME:                 " ${CMAKE_SYSTEM_NAME} )

    # only the version part of CMAKE_SYSTEM
    MESSAGE( STATUS "  CMAKE_SYSTEM_VERSION:              " ${CMAKE_SYSTEM_VERSION} )

    # the processor name (e.g. "Intel(R) Pentium(R) M processor 2.00GHz")
    MESSAGE( STATUS "  CMAKE_SYSTEM_PROCESSOR:            " ${CMAKE_SYSTEM_PROCESSOR} )

    # is TRUE on all UNIX-like OS's, including Apple OS X and CygWin
    MESSAGE( STATUS "  UNIX:                              " ${UNIX} )

    # is TRUE on Windows, including CygWin
    MESSAGE( STATUS "  WIN32:                             " ${WIN32} )

    # is TRUE on Apple OS X
    MESSAGE( STATUS "  APPLE:                             " ${APPLE} )

    # is TRUE when using the MinGW compiler in Windows
    MESSAGE( STATUS "  MINGW:                             " ${MINGW} )

    # is TRUE on Windows when using the CygWin version of cmake
    MESSAGE( STATUS "  CYGWIN:                            " ${CYGWIN} )

    # is TRUE on Windows when using a Borland compiler
    MESSAGE( STATUS "  BORLAND:                           " ${BORLAND} )

    # Microsoft compiler
    MESSAGE( STATUS "  MSVC:                              " ${MSVC} )
    MESSAGE( STATUS "  MSVC_IDE:                          " ${MSVC_IDE} )
    MESSAGE( STATUS "  MSVC60:                            " ${MSVC60} )
    MESSAGE( STATUS "  MSVC70:                            " ${MSVC70} )
    MESSAGE( STATUS "  MSVC71:                            " ${MSVC71} )
    MESSAGE( STATUS "  MSVC80:                            " ${MSVC80} )
    MESSAGE( STATUS "  CMAKE_COMPILER_2005:               " ${CMAKE_COMPILER_2005} )


    # set this to true if you don't want to rebuild the object files if the rules have changed,
    # but not the actual source files or headers (e.g. if you changed the some compiler switches)
    MESSAGE( STATUS "  CMAKE_SKIP_RULE_DEPENDENCY:        " ${CMAKE_SKIP_RULE_DEPENDENCY} )

    # since CMake 2.1 the install rule depends on all, i.e. everything will be built before installing.
    # If you don't like this, set this one to true.
    MESSAGE( STATUS "  CMAKE_SKIP_INSTALL_ALL_DEPENDENCY: " ${CMAKE_SKIP_INSTALL_ALL_DEPENDENCY} )

    # If set, runtime paths are not added when using shared libraries. Default it is set to OFF
    MESSAGE( STATUS "  CMAKE_SKIP_RPATH:                  " ${CMAKE_SKIP_RPATH} )

    # set this to true if you are using makefiles and want to see the full compile and link
    # commands instead of only the shortened ones
    MESSAGE( STATUS "  CMAKE_VERBOSE_MAKEFILE:            " ${CMAKE_VERBOSE_MAKEFILE} )

    # this will cause CMake to not put in the rules that re-run CMake. This might be useful if
    # you want to use the generated build files on another machine.
    MESSAGE( STATUS "  CMAKE_SUPPRESS_REGENERATION:       " ${CMAKE_SUPPRESS_REGENERATION} )


    # A simple way to get switches to the compiler is to use ADD_DEFINITIONS().
    # But there are also two variables exactly for this purpose:

    # the compiler flags for compiling C sources
    MESSAGE( STATUS "  CMAKE_C_FLAGS:                     " ${CMAKE_C_FLAGS} )

    # the compiler flags for compiling C++ sources
    MESSAGE( STATUS "  CMAKE_CXX_FLAGS:                   " ${CMAKE_CXX_FLAGS} )

    # Choose the type of build.  Example: SET(CMAKE_BUILD_TYPE Debug)
    MESSAGE( STATUS "  CMAKE_BUILD_TYPE:                  " ${CMAKE_BUILD_TYPE} )

    # if this is set to ON, then all libraries are built as shared libraries by default.
    MESSAGE( STATUS "  BUILD_SHARED_LIBS:                 " ${BUILD_SHARED_LIBS} )

    # the compiler used for C files
    MESSAGE( STATUS "  CMAKE_C_COMPILER:                  " ${CMAKE_C_COMPILER} )

    # the compiler used for C++ files
    MESSAGE( STATUS "  CMAKE_CXX_COMPILER:                " ${CMAKE_CXX_COMPILER} )

    # if the compiler is a variant of gcc, this should be set to 1
    MESSAGE( STATUS "  CMAKE_COMPILER_IS_GNUCC:           " ${CMAKE_COMPILER_IS_GNUCC} )

    # if the compiler is a variant of g++, this should be set to 1
    MESSAGE( STATUS "  CMAKE_COMPILER_IS_GNUCXX :         " ${CMAKE_COMPILER_IS_GNUCXX} )

    # the tools for creating libraries
    MESSAGE( STATUS "  CMAKE_AR:                          " ${CMAKE_AR} )
    MESSAGE( STATUS "  CMAKE_RANLIB:                      " ${CMAKE_RANLIB} )

    #
    MESSAGE( STATUS "  : " ${} )
ENDFUNCTION()

