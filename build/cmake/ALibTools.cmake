# #################################################################################################
#  ALibTools.cmake - CMake Tools for projects using ALib
#
#  Copyright 2015-2024 A-Worx GmbH, Germany
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
#       list( append MY_PROJECT_RESOURCE_FILES     ../../resources/res_*.img    )
#
# 2. After the executable (or library) is defined with cmake (add_executable or add_library),
#    insert the following invocation of this method:
#
#       AddResourceTarget( my_resources_target   my_target   ${MY_PROJECT_RESOURCE_FILES})
#
#    with the first parameter being an arbitrary name for the resource target to create.
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
    LIST( LENGTH  sourceList  cntSource )
    IF( ${cntSource} GREATER  0  )

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


# --------------------------------------------------------------------------------------------------
# ALibDumpCMakeVariables() Writes out all currently defined CMake variables
# --------------------------------------------------------------------------------------------------
FUNCTION (ALibDumpCMakeVariables)

    message(STATUS "------------------------------ Dumping CMake Variables: ------------------------------")
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        if(     "${_variableName}"  STREQUAL "temp"
            OR  "${_variableName}"  STREQUAL "_gccStdout"
            OR  "${_variableName}"  STREQUAL "_defineLines"
            OR  "${_variableName}"  STREQUAL "_dummy"
            OR  "${_variableName}"  STREQUAL "OUTPUT"
            OR  "${_variableName}"  STREQUAL "CMAKE_C_COMPILER_ID_TOOL_MATCH_REGEX"
            OR  "${_variableName}"  STREQUAL "CMAKE_CXX_COMPILER_ID_TOOL_MATCH_REGEX"
            OR  "${_variableName}"  STREQUAL "CMAKE_C_COMPILER_ID_PLATFORM_CONTENT"
            OR  "${_variableName}"  STREQUAL "CMAKE_CXX_COMPILER_ID_PLATFORM_CONTENT"
            OR  "${_variableName}"  STREQUAL "CMAKE_EXTRA_GENERATOR_C_SYSTEM_DEFINED_MACROS"
            OR  "${_variableName}"  STREQUAL "CMAKE_EXTRA_GENERATOR_CXX_SYSTEM_DEFINED_MACROS"
            OR  "${_variableName}"  STREQUAL "PTHREAD_C_CXX_TEST_SOURCE"
            OR  "${_variableName}"  STREQUAL "Qt5Gui_PLUGINS"                 )
            message(STATUS "${_variableName}=[...] (omitted)")
            continue()
        endif()

        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "------------------------------ Dumping CMake Variables (END) ------------------------------")

ENDFUNCTION()

