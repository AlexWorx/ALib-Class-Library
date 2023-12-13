# #################################################################################################
#  ALibModules.cmake - CMake file for projects using ALib
#
#  Copyright 2015-2023 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Notes:
#    This script resolves dependencies between ALib modules and lists selected plus necessary
#    modules in variable ALIB_DISTRIBUTION.
#    This CMake file is included by "ALib.cmake" automatically and is not intended for manual
#    inclusion. Its contents was separated into a separated cmake file solely for clarity.
# #################################################################################################

SET( ALIB_FILESETS "" )

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
        AND  NOT ( ${modName} STREQUAL "MONOMEM"        )
        AND  NOT ( ${modName} STREQUAL "RESOURCES"      )
        AND  NOT ( ${modName} STREQUAL "RESULTS"        )
        AND  NOT ( ${modName} STREQUAL "SINGLETONS"     )
        AND  NOT ( ${modName} STREQUAL "STRINGS"        )
        AND  NOT ( ${modName} STREQUAL "SYSTEM"         )
        AND  NOT ( ${modName} STREQUAL "TEXT"           )
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
    list( APPEND  ALIB_DISTRIBUTION  "EXPRESSIONS"   )
    list( APPEND  ALIB_DISTRIBUTION  "CLI"           )
    list( APPEND  ALIB_DISTRIBUTION  "THREADS"       )
    list( APPEND  ALIB_DISTRIBUTION  "BITBUFFER"     )
endif()

#### full modules ####
list( FIND   ALIB_DISTRIBUTION  "ALOX"           idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "CONFIGURATION" )
endif()

list( FIND   ALIB_DISTRIBUTION  "EXPRESSIONS"    idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "RESULTS"       )
    list( APPEND  ALIB_FILESETS "PLUGINS"       )
endif()

list( FIND   ALIB_DISTRIBUTION  "CONFIGURATION"  idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "SYSTEM"        )
    list( APPEND  ALIB_FILESETS      "PLUGINS"       )
endif()

list( FIND   ALIB_DISTRIBUTION  "SYSTEM"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "RESULTS"       )
    list( APPEND  ALIB_DISTRIBUTION  "TIME"          )
endif()

list( FIND   ALIB_DISTRIBUTION  "CLI"            idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "RESULTS"       )
endif()

list( FIND    ALIB_DISTRIBUTION "RESULTS"        idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "TEXT"          )
endif()

list( FIND    ALIB_DISTRIBUTION "TEXT"           idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "RESULTS"       )
    list( APPEND  ALIB_DISTRIBUTION  "BOXING"        )
    list( APPEND  ALIB_DISTRIBUTION  "RESOURCES"     )
    list( APPEND  ALIB_DISTRIBUTION  "ENUMS"         )
    list( APPEND  ALIB_FILESETS      "MODULES"       )
    list( APPEND  ALIB_FILESETS      "OWNER"         )
endif()

#### micro modules ####
list( FIND    ALIB_DISTRIBUTION "RESOURCES"      idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "STRINGS"       )
    list( APPEND  ALIB_DISTRIBUTION  "MONOMEM"       )
    list( APPEND  ALIB_DISTRIBUTION  "SINGLETONS"    )
endif()

list( FIND    ALIB_DISTRIBUTION "BITBUFFER"      idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "MONOMEM"       )
    list( APPEND  ALIB_DISTRIBUTION  "ENUMS"         )
endif()

list( FIND    ALIB_DISTRIBUTION "ENUMS"          idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "SINGLETONS"    )
    list( APPEND  ALIB_DISTRIBUTION  "STRINGS"       )
    list( APPEND  ALIB_DISTRIBUTION  "CHARACTERS"    )
    list( APPEND  ALIB_FILESETS      "COMMON_ENUMS"  )
endif()

list( FIND    ALIB_DISTRIBUTION "THREADS"        idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "STRINGS"       )
    list( APPEND  ALIB_FILESETS      "OWNER"         )
endif()

list( FIND    ALIB_DISTRIBUTION "STRINGS"        idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "CHARACTERS"    )
endif()

list( FIND    ALIB_DISTRIBUTION "BOXING"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_DISTRIBUTION  "SINGLETONS"    )
    list( APPEND  ALIB_DISTRIBUTION  "CHARACTERS"    )
    list( APPEND  ALIB_FILESETS "LISTS"         )
endif()

list( FIND    ALIB_DISTRIBUTION "CHARACTERS"     idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_FILESETS "COMMON_ENUMS"       )
endif()


list( FIND    ALIB_DISTRIBUTION "MONOMEM"        idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_FILESETS "COMMON_ENUMS"       )
    list( APPEND  ALIB_FILESETS "LISTS"              )
endif()

list( FIND    ALIB_DISTRIBUTION "TIME"           idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_FILESETS "COMMON_ENUMS"       )
endif()


### clean and sort module list
LIST( REMOVE_DUPLICATES  ALIB_DISTRIBUTION )
LIST( SORT               ALIB_DISTRIBUTION )

SET( maxModules 17 )
list( LENGTH   ALIB_DISTRIBUTION    length)
if( length GREATER ${maxModules} )
    message( FATAL_ERROR "More ALib modules than the known (${length}) > ${maxModules}. Script is inconsistent!" )
endif()
if( length EQUAL ${maxModules} )
    SET( allModules True )
endif()

### clean and sort fileset list
LIST( REMOVE_DUPLICATES  ALIB_FILESETS )
LIST( SORT               ALIB_FILESETS )

SET( maxFileSets 5 )
list( LENGTH   ALIB_FILESETS    length)
if( length GREATER ${maxFileSets} )
    message( FATAL_ERROR "More ALib Filesets sets than the known (${length}) > ${maxFileSets}. Script is inconsistent!" )
endif()
if( length EQUAL ${maxFileSets} )
    SET( allHeaders True )
endif()
