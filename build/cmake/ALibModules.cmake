# #################################################################################################
#  ALibModules.cmake - CMake file for projects using ALib
#
#  Copyright 2015-2018 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Notes:
#    Resolves dependencies between ALib modules and lists selected modules in variable ALIB_MODULES.
#
#    DO NOT INCLUDE THIS FILE MANUALLY !
#    This CMake file is inlcuded by "ALib.cmake" automatically. Its contents was separated into a
#    separated cmake file soley for clearity.
# #################################################################################################


# check for unknown module name
foreach (modName ${ALIB_MODULES})
    if(      NOT ( ${modName} STREQUAL "SINGLETON"      )
        AND  NOT ( ${modName} STREQUAL "BOXING"         )
        AND  NOT ( ${modName} STREQUAL "STRINGS"        )
        AND  NOT ( ${modName} STREQUAL "CORE"           )
        AND  NOT ( ${modName} STREQUAL "CONFIGURATION"  )
        AND  NOT ( ${modName} STREQUAL "ALOX"           )
        AND  NOT ( ${modName} STREQUAL "CLI"            )
        AND  NOT ( ${modName} STREQUAL "EXPRESSIONS"    )
        AND  NOT ( ${modName} STREQUAL "ALL"            )
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

list( FIND   ALIB_MODULES  "ALL"          idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "EXPRESSIONS" )
    list( APPEND  ALIB_MODULES  "CLI"         )
    list( APPEND  ALIB_MODULES  "ALOX"        )
endif()

list( FIND   ALIB_MODULES  "EXPRESSIONS"  idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "CORE"        )
endif()

list( FIND   ALIB_MODULES  "CLI"          idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "CONFIGURATION"      )
endif()

list( FIND   ALIB_MODULES  "ALOX"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "CONFIGURATION"      )
    list( APPEND  ALIB_MODULES  "CORE"        )
endif()

list( FIND   ALIB_MODULES  "CONFIGURATION"       idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "STRINGS"     )
    list( APPEND  ALIB_MODULES  "BOXING"      )
endif()

list( FIND   ALIB_MODULES  "CORE"         idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "STRINGS"     )
    list( APPEND  ALIB_MODULES  "BOXING"      )
endif()

list( FIND    ALIB_MODULES  "STRINGS"     idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "SINGLETON"   )
endif()

list( FIND    ALIB_MODULES  "BOXING"      idx )
if( NOT idx LESS 0 )
    list( APPEND  ALIB_MODULES  "SINGLETON" )
endif()

list( REMOVE_DUPLICATES  ALIB_MODULES )

# create cmake variable per module
list( FIND    ALIB_MODULES  "ALL"         idx)
if( NOT idx LESS 0 )
    set(modALL         "1")
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_MODULE_ALL"   )
endif()

list( FIND    ALIB_MODULES  "EXPRESSIONS" idx)
if( NOT idx LESS 0 )
    set(modEXPRESSIONS "1")
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_MODULE_EXPRESSIONS"   )
endif()
list( FIND    ALIB_MODULES  "CLI"         idx)
if( NOT idx LESS 0 )
    set(modCLI         "1")
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_MODULE_CLI"         )
endif()
list( FIND    ALIB_MODULES  "ALOX"        idx)
if( NOT idx LESS 0 )
    set(modALOX        "1")
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_MODULE_ALOX"        )
endif()
list( FIND    ALIB_MODULES  "CONFIGURATION" idx)
if( NOT idx LESS 0 )
    set(modCONFIGURATION      "1")
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_MODULE_CONFIGURATION"      )
endif()
list( FIND    ALIB_MODULES  "CORE"        idx)
if( NOT idx LESS 0 )
    set(modCORE        "1")
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_MODULE_CORE"        )
endif()
list( FIND    ALIB_MODULES  "STRINGS"     idx)
if( NOT idx LESS 0 )
    set(modSTRINGS     "1")
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_MODULE_STRINGS"     )
endif()
list( FIND    ALIB_MODULES  "BOXING"      idx)
if( NOT idx LESS 0 )
    set(modBOXING      "1")
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_MODULE_BOXING"      )
endif()
list( FIND    ALIB_MODULES  "SINGLETON"   idx)
if( NOT idx LESS 0 )
    set(modSINGLETON   "1")
    list( APPEND ALIB_COMPILATION_SYMBOLS    "ALIB_MODULE_SINGLETON"   )
endif()

