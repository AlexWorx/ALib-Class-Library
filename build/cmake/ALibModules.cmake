# #################################################################################################
#  ALibModules.cmake - CMake file for projects using ALib
#
#  Copyright 2015-2024 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Notes:
#    This script resolves dependencies between ALib modules and lists selected plus necessary
#    modules in variable ALIB_DISTRIBUTION.
#    This CMake file is included by "ALib.cmake" automatically and is not intended for manual
#    inclusion. Its contents was separated into a separated cmake file solely for clarity.
# #################################################################################################

# check for unknown module name
foreach (modName ${ALIB_DISTRIBUTION})
    if(      NOT ( ${modName} STREQUAL "ALL"            )
        AND  NOT ( ${modName} STREQUAL "ALOX"           )
        AND  NOT ( ${modName} STREQUAL "BITBUFFER"      )
        AND  NOT ( ${modName} STREQUAL "BOXING"         )
        AND  NOT ( ${modName} STREQUAL "CHARACTERS"     )
        AND  NOT ( ${modName} STREQUAL "CLI"            )
        AND  NOT ( ${modName} STREQUAL "CONFIGURATION"  )
        AND  NOT ( ${modName} STREQUAL "ENUMS"          )
        AND  NOT ( ${modName} STREQUAL "EXPRESSIONS"    )
        AND  NOT ( ${modName} STREQUAL "FILES"          )
        AND  NOT ( ${modName} STREQUAL "MONOMEM"        )
        AND  NOT ( ${modName} STREQUAL "SINGLETONS"     )
        AND  NOT ( ${modName} STREQUAL "STRINGS"        )
        AND  NOT ( ${modName} STREQUAL "CAMP"         )
        AND  NOT ( ${modName} STREQUAL "THREADS"        )
        AND  NOT ( ${modName} STREQUAL "TIME"           )
      )
        MESSAGE( FATAL_ERROR "ALib: Unknown module name \"${modName}\" given!")
        return()
    endif()
endforeach()

# if nothing is given, ALL is chosen
if( NOT ALIB_DISTRIBUTION )
    MESSAGE( "ALib: No module specified with list variable \"ALIB_DISTRIBUTION\". Setting default \"ALL\".")
    list( APPEND ALIB_DISTRIBUTION "ALL" )
endif()

# resolve "mandatory" module dependencies
list( FIND   ALIB_DISTRIBUTION  "ALL"            idx )
if( NOT idx LESS 0 )
    LIST( REMOVE_AT ALIB_DISTRIBUTION  ${idx}        )
    list( APPEND  ALIB_DISTRIBUTION  "ALOX"          )
    list( APPEND  ALIB_DISTRIBUTION  "BITBUFFER"     )
    list( APPEND  ALIB_DISTRIBUTION  "CLI"           )
    list( APPEND  ALIB_DISTRIBUTION  "EXPRESSIONS"   )
    list( APPEND  ALIB_DISTRIBUTION  "FILES"         )
    list( APPEND  ALIB_DISTRIBUTION  "THREADS"       )
endif()

#### camp modules ####
list( FIND   ALIB_DISTRIBUTION  "ALOX"           idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "CONFIGURATION" )
endif()

list( FIND   ALIB_DISTRIBUTION  "EXPRESSIONS"    idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "CAMP"   )
endif()

list( FIND   ALIB_DISTRIBUTION  "CONFIGURATION"  idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "CAMP"        )
endif()

list( FIND   ALIB_DISTRIBUTION  "CLI"            idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "CAMP"       )
endif()

list( FIND   ALIB_DISTRIBUTION  "FILES"    idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "CAMP"   )
endif()

list( FIND    ALIB_DISTRIBUTION "CAMP"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "BOXING"        )
    list( APPEND  ALIB_DISTRIBUTION  "ENUMS"         )
    list( APPEND  ALIB_DISTRIBUTION  "MONOMEM"       )
    list( APPEND  ALIB_DISTRIBUTION  "STRINGS"       )
    list( APPEND  ALIB_DISTRIBUTION  "TIME"          )
endif()

### non-camp modules ###
list( FIND    ALIB_DISTRIBUTION "BITBUFFER"      idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "MONOMEM"       )
    list( APPEND  ALIB_DISTRIBUTION  "ENUMS"         )
endif()

list( FIND    ALIB_DISTRIBUTION "ENUMS"          idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "SINGLETONS"    )
    list( APPEND  ALIB_DISTRIBUTION  "STRINGS"       )
endif()

list( FIND    ALIB_DISTRIBUTION "THREADS"        idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "STRINGS"       )
endif()

list( FIND    ALIB_DISTRIBUTION "STRINGS"        idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "CHARACTERS"    )
endif()

list( FIND    ALIB_DISTRIBUTION "BOXING"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "SINGLETONS"    )
    list( APPEND  ALIB_DISTRIBUTION  "CHARACTERS"    )
endif()

### clean and sort module list
LIST( REMOVE_DUPLICATES  ALIB_DISTRIBUTION )
LIST( SORT               ALIB_DISTRIBUTION )

SET( maxModules 15 )
list( LENGTH   ALIB_DISTRIBUTION    length)
if( length GREATER ${maxModules} )
    message(             "More ALib modules than the known in:  ${ALIB_DISTRIBUTION}" )
    message( FATAL_ERROR "                                     ${length} > ${maxModules}. Script is inconsistent!" )
endif()
if( length EQUAL ${maxModules} )
    SET( allModules True )
endif()
