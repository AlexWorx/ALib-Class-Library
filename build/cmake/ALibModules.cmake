# #################################################################################################
#  ALibModules.cmake - CMake file for projects using ALib
#
#  Copyright 2013-2025 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Notes:
#    This script resolves dependencies between ALib modules and lists selected plus necessary
#    modules in variable ALibBuild.
#    This CMake file is included by "ALib.cmake" automatically and is not intended for manual
#    inclusion. Its contents was separated into a separated cmake file solely for clarity.
# #################################################################################################

# List of available modules
SET( ALibKnownModules "" )
LIST( APPEND ALibKnownModules  "ALOX"           )
LIST( APPEND ALibKnownModules  "BITBUFFER"      )
LIST( APPEND ALibKnownModules  "BOXING"         )
LIST( APPEND ALibKnownModules  "CAMP"           )
LIST( APPEND ALibKnownModules  "CLI"            )
LIST( APPEND ALibKnownModules  "VARIABLES"  )
LIST( APPEND ALibKnownModules  "CONTAINERS"     )
LIST( APPEND ALibKnownModules  "ENUMRECORDS"    )
LIST( APPEND ALibKnownModules  "EXCEPTIONS"     )
LIST( APPEND ALibKnownModules  "EXPRESSIONS"    )
LIST( APPEND ALibKnownModules  "FILES"          )
LIST( APPEND ALibKnownModules  "FORMAT"         )
LIST( APPEND ALibKnownModules  "MONOMEM"        )
LIST( APPEND ALibKnownModules  "RESOURCES"      )
LIST( APPEND ALibKnownModules  "SINGLETONS"     )
LIST( APPEND ALibKnownModules  "STRINGS"        )
LIST( APPEND ALibKnownModules  "SYSTEM"         )
LIST( APPEND ALibKnownModules  "THREADMODEL"    )


# convert given list to upper case
SET( ALibBuild "" )
foreach(item ${ALIB_BUILD})
    string(TOUPPER "${item}" upperItem)
    list(APPEND ALibBuild ${upperItem} )
endforeach()

# Check for unknown module name
foreach (modName ${ALibBuild})
    if( NOT modName STREQUAL "ALL" )
        if( NOT modName IN_LIST ALibKnownModules )
            MESSAGE( FATAL_ERROR "ALib: Unknown module name \"${modName}\" given!")
            return()
        endif()
    endif()
endforeach()

# if nothing is given, ALL is chosen
if( NOT ALibBuild )
    MESSAGE( "ALib: No module specified with list variable \"ALibBuild\". Setting default \"ALL\".")
    list( APPEND ALibBuild "ALL" )
endif()

# resolve "mandatory" module dependencies
list( FIND   ALibBuild  "ALL"            idx )
if( NOT idx LESS 0 )
    LIST( REMOVE_AT ALibBuild  ${idx}        )
    list( APPEND  ALibBuild  "ALOX"          )
    list( APPEND  ALibBuild  "BITBUFFER"     )
    list( APPEND  ALibBuild  "CLI"           )
    list( APPEND  ALibBuild  "EXPRESSIONS"   )
    list( APPEND  ALibBuild  "FILES"         )

    if( NOT ALIB_SINGLE_THREADED )
        list( APPEND  ALibBuild  "THREADMODEL"   )
    endif()
elseif( ALIB_SINGLE_THREADED )
    list( FIND   ALibBuild  "THREADMODEL"            idx )
    if( NOT idx LESS 0 )
        message( FATAL_ERROR "ALib Module THREADMODEL requested, while symbol ALIB_SINGLE_THREADED is set" )
        return()
    endif()
endif()

#### camp modules ####
list( FIND   ALibBuild  "EXPRESSIONS"    idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "CAMP"   )
endif()

list( FIND   ALibBuild  "CLI"            idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "CAMP"       )
endif()

list( FIND   ALibBuild  "FILES"    idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "CAMP"   )
endif()

list( FIND   ALibBuild  "ALOX"           idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "CAMP" )
endif()

list( FIND    ALibBuild "CAMP"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "FORMAT"          )
    list( APPEND  ALibBuild  "VARIABLES"   )
endif()

list( FIND   ALibBuild  "VARIABLES"  idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "CONTAINERS"     )
    list( APPEND  ALibBuild  "MONOMEM"        )
    list( APPEND  ALibBuild  "SYSTEM"         )
endif()

list( FIND    ALibBuild "FORMAT"         idx  )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "EXCEPTIONS"     )
endif()

list( FIND    ALibBuild "EXCEPTIONS"      idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "BOXING"         )
    list( APPEND  ALibBuild  "ENUMRECORDS"    )
    list( APPEND  ALibBuild  "RESOURCES"      )
endif()

list( FIND    ALibBuild "RESOURCES"       idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "CONTAINERS"     )
    list( APPEND  ALibBuild  "MONOMEM"        )
    list( APPEND  ALibBuild  "STRINGS"        )
endif()

list( FIND    ALibBuild "SYSTEM"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "BOXING"        )
    list( APPEND  ALibBuild  "ENUMRECORDS"   )
endif()


### non-camp modules ###
list( FIND    ALibBuild "BITBUFFER"      idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "CONTAINERS"    )
    list( APPEND  ALibBuild  "MONOMEM"       )
endif()

list( FIND    ALibBuild "THREADMODEL"    idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "BOXING"        )
    list( APPEND  ALibBuild  "CONTAINERS"    )
    list( APPEND  ALibBuild  "MONOMEM"       )
endif()

list( FIND    ALibBuild "ENUMRECORDS"    idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "SINGLETONS"    )
    list( APPEND  ALibBuild  "STRINGS"       )
endif()

list( FIND    ALibBuild "BOXING"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALibBuild  "SINGLETONS"    )
endif()

### clean and sort module list
LIST( REMOVE_DUPLICATES  ALibBuild )
LIST( SORT               ALibBuild )

SET( ALibMaxModules 18 )
list( LENGTH   ALibBuild    length)
SET( ALibCtdModules ${length} )
if( ${ALibCtdModules} GREATER ${ALibMaxModules} )
    message( "More ALib modules than the known evaluated: ${ALibBuild}: " )
    message( FATAL_ERROR "  ${ALibCtdModules} > ${ALibMaxModules}. Internal ALib error (the CMake script is inconsistent!)" )
endif()
if( ALibCtdModules EQUAL ${ALibMaxModules} )
    SET( ALibAllModules True )
    SET( ALibOmittedModules "" )
else()
    # Create list of omitted modules
    SET( ALibOmittedModules "" )
    foreach(item ${ALibKnownModules})
        LIST( FIND ALibBuild ${item} idx)
        if( idx LESS 0 )
            list(APPEND ALibOmittedModules ${item} )
        endif()
    endforeach()
    LIST( REMOVE_DUPLICATES  ALibOmittedModules )
    LIST( SORT               ALibOmittedModules )
endif()

