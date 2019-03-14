# #################################################################################################
#  ALibModules.cmake - CMake file for projects using ALib
#
#  Copyright 2015-2019 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Notes:
#    This script resolves dependencies between ALib modules and lists selected plus necessary
#    modules in variable ALIB_MODULES.
#    This CMake file is inlcuded by "ALib.cmake" automatically and is not intended for manual
#    inclusion. Its contents was separated into a separated cmake file soley for clearity.
# #################################################################################################

SET( ALIB_FILESETS "" )

# check for unknown module name
foreach (modName ${ALIB_MODULES})
    if(      NOT ( ${modName} STREQUAL "ALL"            )
        AND  NOT ( ${modName} STREQUAL "ALOX"           )
        AND  NOT ( ${modName} STREQUAL "EXPRESSIONS"    )
        AND  NOT ( ${modName} STREQUAL "CONFIGURATION"  )
        AND  NOT ( ${modName} STREQUAL "CLI"            )
        AND  NOT ( ${modName} STREQUAL "SYSTEM"         )
        AND  NOT ( ${modName} STREQUAL "RESULTS"       )
        AND  NOT ( ${modName} STREQUAL "STRINGFORMAT"   )
        AND  NOT ( ${modName} STREQUAL "RESOURCES"      )
        AND  NOT ( ${modName} STREQUAL "THREADS"        )
        AND  NOT ( ${modName} STREQUAL "STRINGS"        )
        AND  NOT ( ${modName} STREQUAL "BOXING"         )
        AND  NOT ( ${modName} STREQUAL "TIME"           )
        AND  NOT ( ${modName} STREQUAL "CHARACTERS"     )
        AND  NOT ( ${modName} STREQUAL "ENUMS"          )
        AND  NOT ( ${modName} STREQUAL "SINGLETONS"     )
        AND  NOT ( ${modName} STREQUAL "MEMORY"         )

      )
        MESSAGE( FATAL_ERROR "ALib: Unknown module name \"${modName}\" given!")
    endif()
endforeach()

# if nothing is given, ALL is chosen
if( NOT ALIB_MODULES )
    MESSAGE( "ALib: No module specified with list variable \"ALIB_MODULES\". Setting default \"ALL\".")
    list( APPEND ALIB_MODULES "ALL" )
endif()

# resolve module dependencies
list( FIND   ALIB_MODULES  "ALL"            idx )
if( NOT idx LESS 0 )
    LIST( REMOVE_AT ALIB_MODULES  ${idx}        )
    list( APPEND  ALIB_MODULES  "ALOX"          )
    list( APPEND  ALIB_MODULES  "EXPRESSIONS"   )
    list( APPEND  ALIB_MODULES  "CLI"           )
endif()

#### full modules ####
list( FIND   ALIB_MODULES  "ALOX"           idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "CONFIGURATION" )
    list( APPEND  ALIB_MODULES  "THREADS"       )
endif()

list( FIND   ALIB_MODULES  "EXPRESSIONS"    idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "RESULTS"       )
    list( APPEND  ALIB_FILESETS "PLUGINS"       )
endif()

list( FIND   ALIB_MODULES  "CONFIGURATION"  idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "SYSTEM"        )
    list( APPEND  ALIB_FILESETS "PLUGINS"       )
endif()

list( FIND   ALIB_MODULES  "SYSTEM"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "RESULTS"      )
    list( APPEND  ALIB_MODULES  "TIME"          )
endif()

list( FIND   ALIB_MODULES  "CLI"            idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "RESULTS"      )
endif()

list( FIND    ALIB_MODULES "RESULTS"        idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "STRINGFORMAT"  )
endif()

list( FIND    ALIB_MODULES "STRINGFORMAT" idx   )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "RESULTS"      )
    list( APPEND  ALIB_MODULES  "STRINGS"       )
    list( APPEND  ALIB_MODULES  "BOXING"        )
    list( APPEND  ALIB_MODULES  "RESOURCES"     )
    list( APPEND  ALIB_MODULES  "ENUMS"         )
    list( APPEND  ALIB_FILESETS "MODULES"       )
    list( APPEND  ALIB_FILESETS "OWNER"         )
endif()

#### micro modules ####
list( FIND    ALIB_MODULES "RESOURCES"      idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "STRINGS"       )
    list( APPEND  ALIB_MODULES  "MEMORY"        )
    list( APPEND  ALIB_MODULES  "SINGLETONS"    )
endif()

list( FIND    ALIB_MODULES "THREADS"        idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "STRINGS"       )
    list( APPEND  ALIB_MODULES  "TIME"          )
    list( APPEND  ALIB_FILESETS "OWNER"         )
endif()

list( FIND    ALIB_MODULES "STRINGS"        idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "CHARACTERS"    )
    list( APPEND  ALIB_FILESETS "COMMON_ENUMS"  )
endif()

list( FIND    ALIB_MODULES "BOXING"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "SINGLETONS"    )
    list( APPEND  ALIB_MODULES  "CHARACTERS"    )
endif()


list( FIND    ALIB_MODULES "CHARACTERS"     idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_FILESETS "PREDEF_PF"     )
    list( APPEND  ALIB_FILESETS "PREDEF_TMP"    )
    list( APPEND  ALIB_FILESETS "DEBUG"         )
    list( APPEND  ALIB_FILESETS "INTEGERS"      )
    list( APPEND  ALIB_FILESETS "COMMON_ENUMS"  )
endif()


list( FIND    ALIB_MODULES "SINGLETONS"     idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_FILESETS "TYPEMAP"       )
endif()


list( FIND    ALIB_MODULES "MEMORY"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_FILESETS "PREDEF_PF"     )
    list( APPEND  ALIB_FILESETS "DEBUG"         )
    list( APPEND  ALIB_FILESETS "INTEGERS"      )
endif()


list( FIND    ALIB_MODULES "TIME"     idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_FILESETS "INTEGERS"      )
    list( APPEND  ALIB_FILESETS "COMMON_ENUMS"  )
endif()

list( FIND    ALIB_MODULES "ENUMS"   idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_FILESETS "PREDEF_TMP"    )
endif()




### clean and sort module list
LIST( REMOVE_DUPLICATES  ALIB_MODULES )
LIST( SORT               ALIB_MODULES )

SET( maxModules 16 )
list( LENGTH   ALIB_MODULES    length)
if( length GREATER ${maxModules} )
    message( FATAL_ERROR "More ALib modules (${length}) than the known ${maxModules}. Script is inconsistent!" )
endif()
if( length EQUAL ${maxModules} )
    SET( allModules True )
endif()

### clean and single library header list
LIST( REMOVE_DUPLICATES  ALIB_FILESETS )
LIST( SORT               ALIB_FILESETS )

SET( maxFileSets 9 )
list( LENGTH   ALIB_FILESETS    length)
if( length GREATER ${maxFileSets} )
    message( FATAL_ERROR "More ALib support headers (${length}) than the known ${maxFileSets}. Script is inconsistent!" )
endif()
if( length EQUAL ${maxFileSets} )
    SET( allHeaders True )
endif()




