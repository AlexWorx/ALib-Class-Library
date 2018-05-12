# -------------------------------------------------------------------------------------------------
# Collect resources to copy and add copy commands to a new target project.
#
# This methods may be used when resource files like texts, json data or images have to be copied
# to the target build directory. The use is simple and done as follows:
#
# 1. Create a variable that holds the parameter for "globbing" the the resource files, for example:
#
#       set ( MY_PROJECT_RESOURCE_FILTER    ../../resources/*    )
#
# 2. After the executable (or library) is defined with cmake (add_executable or add_library),
#    insert the following invocation of this method:
#
#       AddResourcesToCopy( my_project_name_resources   my_project_name  ${MY_PROJECT_RESOURCE_FILTER})
#
# 3. In case of using "cotire", the unity projects need to receive the information of the added
#    project, because cotire today can't detect inter-project dependencies:
#
#       add_dependencies   ( my_project_unity          my_project_name_resources            )
# -------------------------------------------------------------------------------------------------
function( AddResourcesToCopy  resourceProjectName  targetName resourceFilter)

    #message(STATUS "### AddResourcesToCopy() ###")
    #message(STATUS "resourceProjectName   : ${resourceProjectName}")
    #message(STATUS "targetName            : ${targetName}")
    #message(STATUS "resourceFilter        : ${resourceFilter}")

    file( GLOB_RECURSE   resourceList    ${resourceFilter}    )
    #message(STATUS "resourceList          : ${resourceList}")

    foreach(_resourceFile IN LISTS resourceList)

        #get filename and create destination file
        get_filename_component( _destFile   ${_resourceFile} NAME)
        set                   ( _destFile   ${CMAKE_CURRENT_BINARY_DIR}/${_destFile}  )
        list ( APPEND   _destFileList  ${_destFile} )

        #add copy command
        add_custom_command( OUTPUT "${_destFile}"
                COMMAND ${CMAKE_COMMAND} -E copy "${_resourceFile}" "${_destFile}"
                COMMENT "Copying resource to ${_destFile}"                             )
    endforeach()

    # create the copy target
    add_custom_target(${resourceProjectName} ALL DEPENDS "${_destFileList}")

    # add the target the main target
    add_dependencies ( ${targetName}  ${resourceProjectName}  )

endfunction(AddResourcesToCopy)


