# ##################################################################################################
#  ALibSources.cmake - CMake file for projects using ALib
#
#  Copyright 2015-2019 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Notes:
#    Selects source and header files in lists ALIB_SOURCE_FILES and ALIB_INCLUDE_FILES, depending
#    on the selected modules listed in ALIB_MODULES.
#
#    If variable ALIB_MODULE_SOURCE_COPY_DIR is defined, then the sources are copied to the
#    directory specified.
#
#    DO NOT INCLUDE THIS FILE MANUALLY !
#    This CMake file is inlcuded by "ALib.cmake" automatically. Its contents was separated into a
#    separated cmake file soley for clearity.
# ##################################################################################################
if( NOT DEFINED  ALIB_SOURCE_DIR )
    set ( ALIB_SOURCE_DIR       ${ALIB_BASE_DIR}/src    )
endif()

SET( ALIB_SOURCE_FILES  "" )
SET( ALIB_INCLUDE_FILES "" )

# always included!
    list( APPEND ALIB_INCLUDE_FILES     alib/alib_precompile.hpp            )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/predef_modules.hpp         )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/predef_compiler.hpp        )


# File-Sets
if( "PREDEF_PF" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/predef_platform.hpp        )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/predef_features.hpp        )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/predef_tools.hpp           )
endif()

if( "PREDEF_TMP" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/predef_tmp.hpp             )
endif()

if( "INTEGERS" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/integers.hpp               )
endif()

if( "COMMON_ENUMS" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/commonenums.hpp            )
endif()

if( "DEBUG" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/assert.hpp                 )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/typedemangler.hpp          )
    list( APPEND ALIB_SOURCE_FILES      alib/lib/assert.cpp                 )
    list( APPEND ALIB_SOURCE_FILES      alib/lib/typedemangler.cpp          )
endif()

if( "OWNER" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/owner.hpp                  )
endif()

if( "TYPEMAP" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/typemap.hpp                )
endif()

if( "MODULES" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/commonenumsresourced.hpp   )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/module.hpp                 )
    list( APPEND ALIB_SOURCE_FILES      alib/lib/module.cpp                 )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/alibmodules.hpp            )
    list( APPEND ALIB_SOURCE_FILES      alib/lib/alibmodules.cpp            )
endif()

if( "PLUGINS" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/plugincontainer.hpp        )
endif()


#--------- compatibility headers ---------
if( "CHARACTERS"  IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/std_characters.hpp        )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/qt_characters.hpp         )
endif()

if( "BOXING"  IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/std_boxing.hpp            )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/std_boxing_functional.hpp )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/qt_boxing.hpp             )
endif()

if( "STRINGS" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/std_strings_functional.hpp)
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/std_strings_iostream.hpp  )
    list( APPEND ALIB_SOURCE_FILES  alib/compatibility/std_strings_iostream.cpp  )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/qt_strings.hpp            )
endif()


#--------------- "Micro" modules ------------

if( "MEMORY" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES     alib/memory/memoryblocks.hpp              )
    list( APPEND ALIB_SOURCE_FILES      alib/memory/memoryblocks.cpp              )
endif()

if( "ENUMS" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES     alib/enums/enumvalue.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES     alib/enums/enumbitwise.hpp                )
    list( APPEND ALIB_INCLUDE_FILES     alib/enums/enumiterators.hpp              )
    list( APPEND ALIB_INCLUDE_FILES     alib/enums/enumarithmetical.hpp           )
endif()


if( "SINGLETONS" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES     alib/singletons/singletons_predef.hpp     )
    list( APPEND ALIB_INCLUDE_FILES     alib/singletons/singleton.hpp             )
    list( APPEND ALIB_SOURCE_FILES      alib/singletons/singleton.cpp             )
endif()

if( "CHARACTERS" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES     alib/characters/characters.hpp            )
    list( APPEND ALIB_INCLUDE_FILES     alib/characters/characters_predef.hpp     )
    list( APPEND ALIB_INCLUDE_FILES     alib/characters/chararray.hpp             )
    list( APPEND ALIB_SOURCE_FILES      alib/characters/chararray.cpp             )
endif()


if( "BOXING" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/box.inl                           )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/boxes.inl                         )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/boxing.hpp                        )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/boxing_predef.hpp                 )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/customizations.inl                )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/dbgboxing.hpp                     )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/enum.hpp                          )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/fwds.hpp                          )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/functions.inl                     )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/functiondefs.inl                  )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/placeholder.inl                   )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/typetraits.inl                    )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/detail/vtable.inl                 )

    list( APPEND ALIB_SOURCE_FILES  alib/boxing/boxing.cpp                        )
    list( APPEND ALIB_SOURCE_FILES  alib/boxing/dbgboxing.cpp                     )
    list( APPEND ALIB_SOURCE_FILES  alib/boxing/detail/vtable.cpp                 )
endif()


if( "STRINGS" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/appendables.inl                  )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/astring.hpp                      )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/chararraytraits.inl              )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/cstring.hpp                      )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/fwds.hpp                         )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/numberformat.hpp                 )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/localstring.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/string.hpp                       )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/stringnzt.hpp                    )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/substring.hpp                    )


    list( APPEND ALIB_SOURCE_FILES  alib/strings/appendables.cpp                  )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/astring.cpp                      )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/numberformat.cpp                 )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/string.cpp                       )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/substring.cpp                    )

    list( APPEND ALIB_INCLUDE_FILES alib/strings/detail/numberconversion.hpp      )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/detail/numberconversion.cpp      )


    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/autosizes.hpp               )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/regexmatcher.hpp            )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/spaces.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/stringtree.hpp              )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/stringtuples.hpp            )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/subsearch.hpp               )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/token.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/tokenizer.hpp               )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/wildcardmatcher.hpp         )

    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/autosizes.cpp               )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/regexmatcher.cpp            )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/spaces.cpp                  )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/subsearch.cpp               )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/token.cpp                   )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/tokenizer.cpp               )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/wildcardmatcher.cpp         )

endif()

#--------------- "Real" modules ------------
if( "RESOURCES" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES     alib/resources/resources.hpp              )
    list( APPEND ALIB_INCLUDE_FILES     alib/resources/resourcedtupleloader.hpp   )
    list( APPEND ALIB_INCLUDE_FILES     alib/resources/enummetadata.hpp           )
    list( APPEND ALIB_INCLUDE_FILES     alib/resources/enummetadataspec.hpp       )
    list( APPEND ALIB_SOURCE_FILES      alib/resources/resources.cpp              )
    list( APPEND ALIB_SOURCE_FILES      alib/resources/enummetadata.cpp           )
    if( "MODULES" IN_LIST ALIB_FILESETS )
        list( APPEND ALIB_INCLUDE_FILES alib/resources/resourcestring.hpp         )
        list( APPEND ALIB_SOURCE_FILES  alib/resources/resourcestring.cpp         )
    endif()

endif()

if( "STRINGFORMAT" IN_LIST ALIB_MODULES )

    list( APPEND ALIB_INCLUDE_FILES alib/stringformat/formatter.hpp               )
    list( APPEND ALIB_INCLUDE_FILES alib/stringformat/formatterjavastyle.hpp      )
    list( APPEND ALIB_INCLUDE_FILES alib/stringformat/formatterpythonstyle.hpp    )
    list( APPEND ALIB_INCLUDE_FILES alib/stringformat/formatterstdimpl.hpp        )
    list( APPEND ALIB_INCLUDE_FILES alib/stringformat/fwds.hpp                    )
    list( APPEND ALIB_INCLUDE_FILES alib/stringformat/stringformat.hpp            )
    list( APPEND ALIB_INCLUDE_FILES alib/stringformat/propertyformatter.hpp       )
    list( APPEND ALIB_INCLUDE_FILES alib/stringformat/propertyformatters.hpp      )
    list( APPEND ALIB_INCLUDE_FILES alib/stringformat/text.hpp                    )

    list( APPEND ALIB_SOURCE_FILES  alib/stringformat/formatter.cpp               )
    list( APPEND ALIB_SOURCE_FILES  alib/stringformat/formatterjavastyle.cpp      )
    list( APPEND ALIB_SOURCE_FILES  alib/stringformat/formatterpythonstyle.cpp    )
    list( APPEND ALIB_SOURCE_FILES  alib/stringformat/formatterstdimpl.cpp        )
    list( APPEND ALIB_SOURCE_FILES  alib/stringformat/stringformat.cpp            )
    list( APPEND ALIB_SOURCE_FILES  alib/stringformat/propertyformatter.cpp       )
    list( APPEND ALIB_SOURCE_FILES  alib/stringformat/text.cpp                    )

    if( "CONFIGURATION" IN_LIST ALIB_FILESETS )
        list( APPEND ALIB_INCLUDE_FILES alib/stringformat/propertyformatters.hpp  )
    endif()

endif()

if( "RESULTS" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES     alib/results/exception.hpp                )
    list( APPEND ALIB_INCLUDE_FILES     alib/results/iclonemessageargument.hpp    )
    list( APPEND ALIB_INCLUDE_FILES     alib/results/message.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES     alib/results/report.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES     alib/results/results.hpp                  )
    list( APPEND ALIB_SOURCE_FILES      alib/results/exception.cpp                )
    list( APPEND ALIB_SOURCE_FILES      alib/results/report.cpp                   )
    list( APPEND ALIB_SOURCE_FILES      alib/results/results.cpp                  )
endif()


if( "THREADS" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES alib/threads/smartlock.hpp                    )
    list( APPEND ALIB_INCLUDE_FILES alib/threads/thread.hpp                       )
    list( APPEND ALIB_INCLUDE_FILES alib/threads/threadlock.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES alib/threads/threadlocknr.hpp                 )

    list( APPEND ALIB_SOURCE_FILES  alib/threads/smartlock.cpp                    )
    list( APPEND ALIB_SOURCE_FILES  alib/threads/thread.cpp                       )
    list( APPEND ALIB_SOURCE_FILES  alib/threads/threadlock.cpp                   )
endif()


if( "SYSTEM" IN_LIST ALIB_MODULES )

    list( APPEND ALIB_INCLUDE_FILES alib/system/calendar.hpp                      )
    list( APPEND ALIB_INCLUDE_FILES alib/system/console.hpp                       )
    list( APPEND ALIB_INCLUDE_FILES alib/system/directory.hpp                     )
    list( APPEND ALIB_INCLUDE_FILES alib/system/environment.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES alib/system/processinfo.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES alib/system/system.hpp                        )

    list( APPEND ALIB_SOURCE_FILES  alib/system/calendar.cpp                      )
    list( APPEND ALIB_SOURCE_FILES  alib/system/console.cpp                       )
    list( APPEND ALIB_SOURCE_FILES  alib/system/directory.cpp                     )
    list( APPEND ALIB_SOURCE_FILES  alib/system/environment.cpp                   )
    list( APPEND ALIB_SOURCE_FILES  alib/system/processinfo.cpp                   )
    list( APPEND ALIB_SOURCE_FILES  alib/system/system.cpp                        )
    if ( ${APPLE} )
        list( APPEND ALIB_SOURCE_FILES  alib/system/directory.mm                  )
    endif()
endif()

if( "TIME" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES alib/time/datetime.hpp                        )
    list( APPEND ALIB_INCLUDE_FILES alib/time/stopwatch.hpp                       )
    list( APPEND ALIB_INCLUDE_FILES alib/time/tickconverter.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES alib/time/ticks.hpp                           )
    list( APPEND ALIB_INCLUDE_FILES alib/time/time.hpp                            )
    list( APPEND ALIB_INCLUDE_FILES alib/time/timepointbase.hpp                   )

    list( APPEND ALIB_SOURCE_FILES  alib/time/time.cpp                            )
endif()

if( "CONFIGURATION" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES alib/config/config.hpp                        )
    list( APPEND ALIB_INCLUDE_FILES alib/config/configuration.hpp                 )
    list( APPEND ALIB_INCLUDE_FILES alib/config/inifile.hpp                       )
    list( APPEND ALIB_INCLUDE_FILES alib/config/inmemoryplugin.hpp                )
    list( APPEND ALIB_INCLUDE_FILES alib/config/plugins.hpp                       )
    list( APPEND ALIB_INCLUDE_FILES alib/config/variable.hpp                      )

    list( APPEND ALIB_SOURCE_FILES  alib/config/config.cpp                        )
    list( APPEND ALIB_SOURCE_FILES  alib/config/configuration.cpp                 )
    list( APPEND ALIB_SOURCE_FILES  alib/config/inifile.cpp                       )
    list( APPEND ALIB_SOURCE_FILES  alib/config/inmemoryplugin.cpp                )
    list( APPEND ALIB_SOURCE_FILES  alib/config/plugins.cpp                       )
    list( APPEND ALIB_SOURCE_FILES  alib/config/variable.cpp                      )
endif()

if( "CLI" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES alib/cli/arguments.hpp                        )
    list( APPEND ALIB_INCLUDE_FILES alib/cli/cliapp.hpp                           )
    list( APPEND ALIB_INCLUDE_FILES alib/cli/cli.hpp                              )
    list( APPEND ALIB_INCLUDE_FILES alib/cli/cliutil.hpp                          )

    list( APPEND ALIB_SOURCE_FILES  alib/cli/arguments.cpp                        )
    list( APPEND ALIB_SOURCE_FILES  alib/cli/cliapp.cpp                           )
    list( APPEND ALIB_SOURCE_FILES  alib/cli/cli.cpp                              )
    list( APPEND ALIB_SOURCE_FILES  alib/cli/cliutil.cpp                          )
endif()

if( "EXPRESSIONS" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/compiler.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/compilerplugin.hpp            )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/expression.hpp                )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/expressions.hpp               )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/scope.hpp                     )

    list( APPEND ALIB_SOURCE_FILES  alib/expressions/compiler.cpp                  )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/expression.cpp                )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/expressions.cpp               )

    list( APPEND ALIB_INCLUDE_FILES alib/expressions/detail/ast.hpp                )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/detail/parser.hpp             )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/detail/parser_impl.hpp        )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/detail/program.hpp            )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/detail/spirit.hpp             )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/detail/virtualmachine.hpp     )

    list( APPEND ALIB_SOURCE_FILES  alib/expressions/detail/ast.cpp                )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/detail/parser.cpp             )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/detail/parser_impl.cpp        )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/detail/program.cpp            )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/detail/spirit.cpp             )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/detail/virtualmachine.cpp     )

    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/arithmetics.hpp       )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/autocast.hpp          )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/calculus.hpp          )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/elvisoperator.hpp     )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/math.hpp              )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/scopestring.hpp       )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/strings.hpp           )

    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/arithmetics.cpp       )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/autocast.cpp          )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/calculus.cpp          )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/elvisoperator.cpp     )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/math.cpp              )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/strings.cpp           )

    list( APPEND ALIB_INCLUDE_FILES alib/expressions/util/expressionformatter.hpp  )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/util/expressionformatter.cpp  )

    if( "SYSTEM" IN_LIST ALIB_MODULES )
        list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/dateandtime.hpp       )
        list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/dateandtime.cpp       )
    endif()

 endif()

if( "ALOX" IN_LIST ALIB_MODULES )
    list( APPEND ALIB_INCLUDE_FILES alib/alox.hpp                                 )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/alox.hpp                            )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/alox_predef.hpp                     )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/log.inl                             )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/logtools.hpp                        )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/lox.inl                             )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/macros.inl                          )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/reportwriter.hpp                    )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/variables.hpp                       )

    list( APPEND ALIB_SOURCE_FILES  alib/alox/alox.cpp                            )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/log.cpp                             )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/logtools.cpp                        )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/lox.cpp                             )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/lox_getstate.cpp                    )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/reportwriter.cpp                    )

    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/domain.inl                     )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/logger.inl                     )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/scopedump.inl                  )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/scopeinfo.inl                  )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/scopestore.inl                 )

    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/domain.cpp                     )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/scopedump.cpp                  )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/scopeinfo.cpp                  )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/scopestore.cpp                 )

    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/textlogger/plaintextlogger.hpp )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/textlogger/textlogger.hpp      )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/textlogger/plaintextlogger.cpp )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/textlogger/textlogger.cpp      )

    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/ansilogger.hpp              )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/consolelogger.hpp           )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/memorylogger.hpp            )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/textfilelogger.hpp          )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/vstudiologger.hpp           )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/windowsconsolelogger.hpp    )

    list( APPEND ALIB_SOURCE_FILES  alib/alox/loggers/ansilogger.cpp              )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/loggers/consolelogger.cpp           )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/loggers/textfilelogger.cpp          )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/loggers/vstudiologger.cpp           )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/loggers/windowsconsolelogger.cpp    )

endif()


# ##################################################################################################
# Check for duplicates. This is a pure double-check to detect that this script needs changes.
# ##################################################################################################

CollectAndRemoveDuplicates( "ALIB_INCLUDE_FILES"  "duplicates" )
LIST( LENGTH  duplicates  qtyDuplicates )



IF( ${qtyDuplicates} GREATER  0  )
    message( WARNING "Internal script error ALIBSources.cmake: "
                     "DUPLICATE HEADER FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE HEADER FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()

CollectAndRemoveDuplicates( "ALIB_SOURCE_FILES"  "duplicates" )
LIST( LENGTH  duplicates  qtyDuplicates )
IF( ${qtyDuplicates} GREATER  0  )
    message( WARNING "Internal script error ALIBSources.cmake: "
                     "DUPLICATE SOURCE FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE SOURCE FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()

# ##################################################################################################
# Check for ".hpp" in header files or ".cpp" in sources
# ##################################################################################################
FOREACH( entry IN LISTS ALIB_INCLUDE_FILES  )
    IF( NOT ${entry} MATCHES "([.]inl$)|([.]hpp$)"  )
        message( FATAL_ERROR  "Non-header file found in header file list: ${entry}" )
    ENDIF()
ENDFOREACH()

FOREACH( entry IN LISTS ALIB_SOURCE_FILES  )
    IF( NOT ${entry} MATCHES "([.]mm$)|([.]cpp$)"  )
        message( FATAL_ERROR  "Non-source file found in source file list: ${entry}" )
    ENDIF()
ENDFOREACH()

# ##################################################################################################
# Copy sources if requested
# ##################################################################################################
if( ALIB_MODULE_SOURCE_COPY_DIR )

    if( ${ALIB_MODULE_SOURCE_COPY_DIR} EQUAL ${ALIB_SOURCE_DIR})
        MESSAGE(FATAL_ERROR "Variable ALIB_MODULE_SOURCE_COPY_DIR=\"${ALIB_MODULE_SOURCE_COPY_DIR}\" "
                "is set and equals ALIB_SOURCE_DIR. This would delete origninal sources!")
    endif()

    # Add the build type to the directory name. Otherwise, if CMake was executed for different
    # configurations in parallel, a clash of file copy/delete operations may happen.
    if( NOT IS_DIRECTORY ${ALIB_MODULE_SOURCE_COPY_DIR})
        MESSAGE(FATAL_ERROR "Variable ALIB_MODULE_SOURCE_COPY_DIR=\"${ALIB_MODULE_SOURCE_COPY_DIR}\" "
                            "is set but is not a valid directory. Stopping here!"  )
    endif()

    SET( ALIB_MODULE_SOURCE_COPY_DIR  "${ALIB_MODULE_SOURCE_COPY_DIR}/${CMAKE_BUILD_TYPE}" )

    MESSAGE( STATUS "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    MESSAGE( STATUS "!!! ATTENTION: Copying sources.                             !!!")
    MESSAGE( STATUS "!!!            Destination directory will be deleted, hence !!!")
    MESSAGE( STATUS "!!!            CHANGES WILL BE LOST with next CMake run!    !!!")
    MESSAGE( STATUS "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    MESSAGE( STATUS "Copying source files of selected ALib modules to: ’${ALIB_MODULE_SOURCE_COPY_DIR}’")

    # delete destination directory
    file( REMOVE_RECURSE ${ALIB_MODULE_SOURCE_COPY_DIR} )

    #### copy compilation units ####
    foreach( fileName IN LISTS   ALIB_INCLUDE_FILES  ALIB_SOURCE_FILES  )
        SET( destPath  "${ALIB_MODULE_SOURCE_COPY_DIR}/${fileName}" )
        get_filename_component(destPath ${destPath} DIRECTORY )


        file( COPY   "${ALIB_SOURCE_DIR}/${fileName}"  DESTINATION  "${destPath}" )
    endforeach()

    ### now also switch source base directories ###
    set( ALIB_SOURCE_DIR  ${ALIB_MODULE_SOURCE_COPY_DIR} )
endif()

# ##################################################################################################
# Add base directory to files
# ##################################################################################################
SET( temp "" )
foreach( fileName IN LISTS   ALIB_INCLUDE_FILES )
    LIST( APPEND  temp "${ALIB_SOURCE_DIR}/${fileName}" )
endforeach()
SET( ALIB_INCLUDE_FILES ${temp} )

SET( temp "" )
foreach( fileName IN LISTS   ALIB_SOURCE_FILES )
    LIST( APPEND  temp "${ALIB_SOURCE_DIR}/${fileName}" )
endforeach()
SET( ALIB_SOURCE_FILES  ${temp} )

