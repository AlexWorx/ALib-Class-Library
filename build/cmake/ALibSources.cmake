# ##################################################################################################
#  ALibSources.cmake - CMake file for projects using ALib
#
#  Copyright 2015-2023 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Notes:
#    Selects source and header files in lists ALIB_SOURCE_FILES and ALIB_INCLUDE_FILES, depending
#    on the selected modules listed in ALIB_DISTRIBUTION.
#
#    If variable ALIB_SOURCE_COPY_TARGET_DIR is defined, then the sources are copied to the
#    directory specified.
#
#    This CMake file is included by "ALib.cmake" automatically and is not intended for manual
#    inclusion. Its contents was separated into a separated cmake file solely for clarity.
# ##################################################################################################
if( NOT DEFINED  ALIB_SOURCE_DIR )
    set ( ALIB_SOURCE_DIR       ${ALIB_BASE_DIR}/src    )
endif()

SET( ALIB_SOURCE_FILES  "" )
SET( ALIB_INCLUDE_FILES "" )

# always included!
list( APPEND ALIB_INCLUDE_FILES     alib/alib_precompile.hpp     )
list( APPEND ALIB_INCLUDE_FILES     alib/distribution.hpp        )
list( APPEND ALIB_SOURCE_FILES      alib/lib/alib.cpp            )

list( APPEND ALIB_INCLUDE_FILES     alib/alox.hpp                )

list( APPEND ALIB_INCLUDE_FILES     alib/lib/modules.hpp         )
list( APPEND ALIB_INCLUDE_FILES     alib/lib/compilers.hpp       )
list( APPEND ALIB_INCLUDE_FILES     alib/lib/platforms.hpp       )
list( APPEND ALIB_INCLUDE_FILES     alib/lib/features.hpp        )
list( APPEND ALIB_INCLUDE_FILES     alib/lib/tools.hpp           )
list( APPEND ALIB_INCLUDE_FILES     alib/lib/tmp.hpp             )
list( APPEND ALIB_INCLUDE_FILES     alib/lib/integers.hpp        )
list( APPEND ALIB_INCLUDE_FILES     alib/lib/bits.hpp            )



# File-Sets
if( "COMMON_ENUMS" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/fs_commonenums/commonenumdefs.hpp )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/fs_commonenums/commonenums.hpp    )
endif()

if( "OWNER" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/fs_owner/owner.hpp                )
endif()

if( "MODULES" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/fs_modules/module.hpp             )
    list( APPEND ALIB_SOURCE_FILES      alib/lib/fs_modules/module.cpp             )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/fs_modules/distribution.hpp       )
    list( APPEND ALIB_SOURCE_FILES      alib/lib/fs_modules/distribution.cpp       )
endif()

if( "PLUGINS" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/fs_plugins/plugins.hpp            )
endif()

if( "LISTS" IN_LIST ALIB_FILESETS )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/fs_lists/bidilist.hpp             )
    list( APPEND ALIB_INCLUDE_FILES     alib/lib/fs_lists/sidilist.hpp             )
endif()


#--------- compatibility headers ---------
if( "CHARACTERS"  IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/std_characters.hpp            )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/qt_characters.hpp             )
endif()

if( "BOXING"  IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/std_boxing.hpp                )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/std_boxing_functional.hpp     )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/qt_boxing.hpp                 )
endif()

if(     "BOXING"      IN_LIST ALIB_DISTRIBUTION
    OR  "SINGLETONS"  IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/std_typeinfo.hpp              )
endif()


if( "STRINGS" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/std_strings_functional.hpp    )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/std_strings_iostream.hpp      )
    list( APPEND ALIB_SOURCE_FILES  alib/compatibility/std_strings_iostream.cpp      )
    list( APPEND ALIB_INCLUDE_FILES alib/compatibility/qt_strings.hpp                )
endif()




#################################            Micro Modules             #############################

if( "MONOMEM" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/detail/hashtablebase.inl    )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/detail/recycler.inl         )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/fwds.hpp                    )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/hashmap.hpp                 )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/hashset.hpp                 )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/hashtable.hpp               )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/list.hpp                    )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/mastring.hpp                )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/monomem.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/monoallocator.hpp           )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/selfcontained.hpp           )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/stdcontainerma.hpp          )
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/util/fixedcapacityvector.hpp)
    list( APPEND ALIB_INCLUDE_FILES     alib/monomem/util/rttrallocator.hpp      )

    list( APPEND ALIB_SOURCE_FILES      alib/monomem/detail/hashtablebase.cpp    )
    list( APPEND ALIB_SOURCE_FILES      alib/monomem/monoallocator.cpp           )

    if( "STRINGS" IN_LIST ALIB_DISTRIBUTION )
        list( APPEND ALIB_INCLUDE_FILES alib/monomem/stringtree.hpp              )
        list( APPEND ALIB_INCLUDE_FILES alib/monomem/detail/stringtreebase.inl   )
    endif()
endif()

if( "BITBUFFER" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES     alib/bitbuffer/arraycompressor.hpp         )
    list( APPEND ALIB_INCLUDE_FILES     alib/bitbuffer/bitbuffer.hpp               )
    list( APPEND ALIB_INCLUDE_FILES     alib/bitbuffer/ac_v1/acalgos.inl           )
    list( APPEND ALIB_INCLUDE_FILES     alib/bitbuffer/ac_v1/ac.hpp                )
    list( APPEND ALIB_INCLUDE_FILES     alib/bitbuffer/ac_v1/huffman.hpp           )

    list( APPEND ALIB_SOURCE_FILES      alib/bitbuffer/bitbuffer.cpp               )
    list( APPEND ALIB_SOURCE_FILES      alib/bitbuffer/ac_v1/ac.cpp                )
    list( APPEND ALIB_SOURCE_FILES      alib/bitbuffer/ac_v1/huffman.cpp           )
endif()

if( "ENUMS" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES     alib/enums/arithmetical.hpp              )
    list( APPEND ALIB_INCLUDE_FILES     alib/enums/bitwise.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES     alib/enums/iterable.hpp                )
    list( APPEND ALIB_INCLUDE_FILES     alib/enums/records.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES     alib/enums/recordbootstrap.hpp           )
    list( APPEND ALIB_INCLUDE_FILES     alib/enums/underlyingintegral.hpp        )
    list( APPEND ALIB_INCLUDE_FILES     alib/enums/detail/enumrecords.inl        )

    list( APPEND ALIB_SOURCE_FILES      alib/enums/records.cpp                   )

    if( "STRINGS" IN_LIST ALIB_DISTRIBUTION )
        list( APPEND ALIB_INCLUDE_FILES alib/enums/recordparser.hpp              )
        list( APPEND ALIB_INCLUDE_FILES alib/enums/serialization.hpp             )
    endif()
endif()

if( "SINGLETONS" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES     alib/singletons/dbgsingletons.hpp        )
    list( APPEND ALIB_INCLUDE_FILES     alib/singletons/singleton.hpp            )
    list( APPEND ALIB_SOURCE_FILES      alib/singletons/singleton.cpp            )
endif()

if( "CHARACTERS" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES     alib/characters/characters.hpp           )
    list( APPEND ALIB_INCLUDE_FILES     alib/characters/chararray.hpp            )
    list( APPEND ALIB_SOURCE_FILES      alib/characters/chararray.cpp            )
endif()


if( "BOXING" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/box.inl                          )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/boxes.inl                        )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/boxing.hpp                       )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/customizations.inl               )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/dbgboxing.hpp                    )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/enum.hpp                         )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/fwds.hpp                         )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/functions.inl                    )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/functiondefs.inl                 )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/placeholder.inl                  )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/typetraits.inl                   )
    list( APPEND ALIB_INCLUDE_FILES alib/boxing/detail/vtable.inl                )

    list( APPEND ALIB_SOURCE_FILES  alib/boxing/boxes.cpp                        )
    list( APPEND ALIB_SOURCE_FILES  alib/boxing/boxing.cpp                       )
    list( APPEND ALIB_SOURCE_FILES  alib/boxing/dbgboxing.cpp                    )
    list( APPEND ALIB_SOURCE_FILES  alib/boxing/detail/vtable.cpp                )

endif()


if( "STRINGS" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/appendables.inl                 )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/astring.hpp                     )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/chararraytraits.inl             )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/cstring.hpp                     )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/format.hpp                        )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/fwds.hpp                        )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/numberformat.hpp                )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/localstring.hpp                 )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/string.hpp                      )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/stringnzt.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/substring.hpp                   )


    list( APPEND ALIB_SOURCE_FILES  alib/strings/appendables.cpp                 )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/astring.cpp                     )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/numberformat.cpp                )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/string.cpp                      )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/substring.cpp                   )

    list( APPEND ALIB_INCLUDE_FILES alib/strings/detail/numberconversion.hpp     )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/detail/numberconversion.cpp     )


    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/autosizes.hpp              )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/regexmatcher.hpp           )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/spaces.hpp                 )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/subsearch.hpp              )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/token.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/tokenizer.hpp              )
    list( APPEND ALIB_INCLUDE_FILES alib/strings/util/wildcardmatcher.hpp        )

    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/autosizes.cpp              )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/regexmatcher.cpp           )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/spaces.cpp                 )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/subsearch.cpp              )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/token.cpp                  )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/tokenizer.cpp              )
    list( APPEND ALIB_SOURCE_FILES  alib/strings/util/wildcardmatcher.cpp        )

endif()

if( "THREADS" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/threads/detail/threadmap.hpp            )
    list( APPEND ALIB_INCLUDE_FILES alib/threads/smartlock.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES alib/threads/thread.hpp                      )
    list( APPEND ALIB_INCLUDE_FILES alib/threads/threadlock.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES alib/threads/threadlocknr.hpp                )
    list( APPEND ALIB_INCLUDE_FILES alib/threads/sleeper.hpp                     )

    list( APPEND ALIB_SOURCE_FILES  alib/threads/smartlock.cpp                   )
    list( APPEND ALIB_SOURCE_FILES  alib/threads/thread.cpp                      )
    list( APPEND ALIB_SOURCE_FILES  alib/threads/threadlock.cpp                  )
endif()


#################################             Full Modules             #############################
if( "RESOURCES" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES     alib/resources/localresourcepool.hpp  )
    list( APPEND ALIB_INCLUDE_FILES     alib/resources/resources.hpp          )
    list( APPEND ALIB_INCLUDE_FILES     alib/resources/detail/resourcemap.hpp )

    list( APPEND ALIB_SOURCE_FILES      alib/resources/localresourcepool.cpp  )


    if( "CONFIGURATION" IN_LIST ALIB_DISTRIBUTION )
        list( APPEND ALIB_INCLUDE_FILES alib/resources/configresourcepool.hpp )
        list( APPEND ALIB_SOURCE_FILES  alib/resources/configresourcepool.cpp )
    endif()

endif()

if( "TEXT" IN_LIST ALIB_DISTRIBUTION )

    list( APPEND ALIB_INCLUDE_FILES alib/text/formatter.hpp              )
    list( APPEND ALIB_INCLUDE_FILES alib/text/formatterjavastyle.hpp     )
    list( APPEND ALIB_INCLUDE_FILES alib/text/formatterpythonstyle.hpp   )
    list( APPEND ALIB_INCLUDE_FILES alib/text/formatterstdimpl.hpp       )
    list( APPEND ALIB_INCLUDE_FILES alib/text/fwds.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES alib/text/propertyformatter.hpp      )
    list( APPEND ALIB_INCLUDE_FILES alib/text/propertyformatters.hpp     )
    list( APPEND ALIB_INCLUDE_FILES alib/text/paragraphs.hpp             )
    list( APPEND ALIB_INCLUDE_FILES alib/text/text.hpp                   )

    list( APPEND ALIB_SOURCE_FILES  alib/text/formatter.cpp              )
    list( APPEND ALIB_SOURCE_FILES  alib/text/formatterjavastyle.cpp     )
    list( APPEND ALIB_SOURCE_FILES  alib/text/formatterpythonstyle.cpp   )
    list( APPEND ALIB_SOURCE_FILES  alib/text/formatterstdimpl.cpp       )
    list( APPEND ALIB_SOURCE_FILES  alib/text/propertyformatter.cpp      )
    list( APPEND ALIB_SOURCE_FILES  alib/text/paragraphs.cpp             )
    list( APPEND ALIB_SOURCE_FILES  alib/text/text.cpp                   )

    if( "CONFIGURATION" IN_LIST ALIB_FILESETS )
        list( APPEND ALIB_INCLUDE_FILES alib/text/propertyformatters.hpp )
    endif()

endif()

if( "RESULTS" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES     alib/results/exception.hpp               )
    list( APPEND ALIB_INCLUDE_FILES     alib/results/iclonemessageargument.hpp   )
    list( APPEND ALIB_INCLUDE_FILES     alib/results/message.hpp                 )
    list( APPEND ALIB_INCLUDE_FILES     alib/results/report.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES     alib/results/results.hpp                 )
    list( APPEND ALIB_SOURCE_FILES      alib/results/exception.cpp               )
    list( APPEND ALIB_SOURCE_FILES      alib/results/report.cpp                  )
    list( APPEND ALIB_SOURCE_FILES      alib/results/results.cpp                 )
endif()


if( "SYSTEM" IN_LIST ALIB_DISTRIBUTION )

    list( APPEND ALIB_INCLUDE_FILES alib/system/calendar.hpp                     )
    list( APPEND ALIB_INCLUDE_FILES alib/system/console.hpp                      )
    list( APPEND ALIB_INCLUDE_FILES alib/system/directory.hpp                    )
    list( APPEND ALIB_INCLUDE_FILES alib/system/environment.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES alib/system/processinfo.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES alib/system/system.hpp                       )
    list( APPEND ALIB_INCLUDE_FILES alib/system/systemerrors.hpp                 )

    list( APPEND ALIB_SOURCE_FILES  alib/system/calendar.cpp                     )
    list( APPEND ALIB_SOURCE_FILES  alib/system/console.cpp                      )
    list( APPEND ALIB_SOURCE_FILES  alib/system/directory.cpp                    )
    list( APPEND ALIB_SOURCE_FILES  alib/system/environment.cpp                  )
    list( APPEND ALIB_SOURCE_FILES  alib/system/processinfo.cpp                  )
    list( APPEND ALIB_SOURCE_FILES  alib/system/system.cpp                       )
    if ( ${APPLE} )
        list( APPEND ALIB_SOURCE_FILES  alib/system/directory.mm                 )
    endif()
endif()

if( "TIME" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/time/datetime.hpp                       )
    list( APPEND ALIB_INCLUDE_FILES alib/time/stopwatch.hpp                      )
    list( APPEND ALIB_INCLUDE_FILES alib/time/tickconverter.hpp                  )
    list( APPEND ALIB_INCLUDE_FILES alib/time/ticks.hpp                          )
    list( APPEND ALIB_INCLUDE_FILES alib/time/time.hpp                           )
    list( APPEND ALIB_INCLUDE_FILES alib/time/timepointbase.hpp                  )

    list( APPEND ALIB_SOURCE_FILES  alib/time/time.cpp                           )
endif()

if( "CONFIGURATION" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/config/config.hpp                       )
    list( APPEND ALIB_INCLUDE_FILES alib/config/configuration.hpp                )
    list( APPEND ALIB_INCLUDE_FILES alib/config/inifile.hpp                      )
    list( APPEND ALIB_INCLUDE_FILES alib/config/inmemoryplugin.hpp               )
    list( APPEND ALIB_INCLUDE_FILES alib/config/plugins.hpp                      )
    list( APPEND ALIB_INCLUDE_FILES alib/config/priorities.hpp                   )
    list( APPEND ALIB_INCLUDE_FILES alib/config/variable.hpp                     )
    list( APPEND ALIB_INCLUDE_FILES alib/config/variabledecl.hpp                     )

    list( APPEND ALIB_SOURCE_FILES  alib/config/config.cpp                       )
    list( APPEND ALIB_SOURCE_FILES  alib/config/configuration.cpp                )
    list( APPEND ALIB_SOURCE_FILES  alib/config/inifile.cpp                      )
    list( APPEND ALIB_SOURCE_FILES  alib/config/inmemoryplugin.cpp               )
    list( APPEND ALIB_SOURCE_FILES  alib/config/plugins.cpp                      )
    list( APPEND ALIB_SOURCE_FILES  alib/config/variable.cpp                     )
endif()

if( "CLI" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/cli/arguments.hpp                       )
    list( APPEND ALIB_INCLUDE_FILES alib/cli/cli.hpp                             )
    list( APPEND ALIB_INCLUDE_FILES alib/cli/cliutil.hpp                         )
    list( APPEND ALIB_INCLUDE_FILES alib/cli/commandline.hpp                     )

    list( APPEND ALIB_SOURCE_FILES  alib/cli/arguments.cpp                       )
    list( APPEND ALIB_SOURCE_FILES  alib/cli/cli.cpp                             )
    list( APPEND ALIB_SOURCE_FILES  alib/cli/cliutil.cpp                         )
    list( APPEND ALIB_SOURCE_FILES  alib/cli/commandline.cpp                     )
endif()

if( "EXPRESSIONS" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/compiler.hpp                )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/compilerplugin.hpp          )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/expression.hpp              )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/expressions.hpp             )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/scope.hpp                   )

    list( APPEND ALIB_SOURCE_FILES  alib/expressions/compiler.cpp                )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/expression.cpp              )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/expressions.cpp             )

    list( APPEND ALIB_INCLUDE_FILES alib/expressions/detail/ast.hpp              )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/detail/parser.hpp           )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/detail/parser_impl.hpp      )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/detail/program.hpp          )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/detail/virtualmachine.hpp   )

    list( APPEND ALIB_SOURCE_FILES  alib/expressions/detail/ast.cpp              )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/detail/parser.cpp           )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/detail/parser_impl.cpp      )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/detail/program.cpp          )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/detail/virtualmachine.cpp   )

    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/arithmetics.hpp     )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/autocast.hpp        )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/calculus.hpp        )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/elvisoperator.hpp   )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/math.hpp            )
    list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/strings.hpp         )

    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/arithmetics.cpp     )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/autocast.cpp        )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/calculus.cpp        )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/elvisoperator.cpp   )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/math.cpp            )
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/strings.cpp         )

    list( APPEND ALIB_INCLUDE_FILES alib/expressions/util/expressionformatter.hpp)
    list( APPEND ALIB_SOURCE_FILES  alib/expressions/util/expressionformatter.cpp)

    if( "SYSTEM" IN_LIST ALIB_DISTRIBUTION )
        list( APPEND ALIB_INCLUDE_FILES alib/expressions/plugins/dateandtime.hpp )
        list( APPEND ALIB_SOURCE_FILES  alib/expressions/plugins/dateandtime.cpp )
    endif()
    if( "CONFIGURATION" IN_LIST ALIB_DISTRIBUTION )
        list( APPEND ALIB_INCLUDE_FILES alib/expressions/standardrepository.hpp           )
        list( APPEND ALIB_SOURCE_FILES  alib/expressions/standardrepository.cpp           )
    endif()

 endif()

if( "ALOX" IN_LIST ALIB_DISTRIBUTION )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/alox.hpp                              )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/aloxmodule.hpp                        )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/log.inl                               )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/logtools.hpp                          )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/lox.inl                               )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/macros.inl                            )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/reportwriter.hpp                      )

    list( APPEND ALIB_SOURCE_FILES  alib/alox/aloxmodule.cpp                        )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/log.cpp                               )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/logtools.cpp                          )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/reportwriter.cpp                      )

    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/domain.inl                     )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/logger.hpp                     )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/loxpimpl.inl                   )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/scopedump.inl                  )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/scopeinfo.inl                  )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/scopestore.inl                 )

    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/domain.cpp                     )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/loxpimpl.cpp                   )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/scopedump.cpp                  )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/scopeinfo.cpp                  )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/scopestore.cpp                 )

    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/textlogger/plaintextlogger.hpp )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/detail/textlogger/textlogger.hpp      )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/textlogger/plaintextlogger.cpp )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/detail/textlogger/textlogger.cpp      )

    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/ansilogger.hpp                )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/consolelogger.hpp             )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/memorylogger.hpp              )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/textfilelogger.hpp            )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/vstudiologger.hpp             )
    list( APPEND ALIB_INCLUDE_FILES alib/alox/loggers/windowsconsolelogger.hpp      )

    list( APPEND ALIB_SOURCE_FILES  alib/alox/loggers/ansilogger.cpp                )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/loggers/consolelogger.cpp             )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/loggers/textfilelogger.cpp            )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/loggers/vstudiologger.cpp             )
    list( APPEND ALIB_SOURCE_FILES  alib/alox/loggers/windowsconsolelogger.cpp      )

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
        message( FATAL_ERROR  "Non-header file (wrong extension) found in header file list: ${entry}" )
    ENDIF()
ENDFOREACH()

FOREACH( entry IN LISTS ALIB_SOURCE_FILES  )
    IF( NOT ${entry} MATCHES "([.]mm$)|([.]cpp$)"  )
        message( FATAL_ERROR  "File with wrong extension found in source file list: ${entry}" )
    ENDIF()
ENDFOREACH()

# ##################################################################################################
# Copy sources if requested
# ##################################################################################################
if( ALIB_SOURCE_COPY_TARGET_DIR )

    if( ${ALIB_SOURCE_COPY_TARGET_DIR} EQUAL ${ALIB_SOURCE_DIR})
        MESSAGE(FATAL_ERROR "Variable ALIB_SOURCE_COPY_TARGET_DIR=\"${ALIB_SOURCE_COPY_TARGET_DIR}\" "
                "is set and equals ALIB_SOURCE_DIR. This would delete origninal sources!")
    endif()

    # Add the build type to the directory name. Otherwise, if CMake was executed for different
    # configurations in parallel, a clash of file copy/delete operations may happen.
    if( NOT IS_DIRECTORY ${ALIB_SOURCE_COPY_TARGET_DIR})
        MESSAGE(FATAL_ERROR "Variable ALIB_SOURCE_COPY_TARGET_DIR=\"${ALIB_SOURCE_COPY_TARGET_DIR}\" "
                            "is set but is not a valid directory. Stopping here!"  )
    endif()

    SET( ALIB_SOURCE_COPY_TARGET_DIR  "${ALIB_SOURCE_COPY_TARGET_DIR}/${CMAKE_BUILD_TYPE}" )

    MESSAGE( STATUS "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    MESSAGE( STATUS "!!! ATTENTION: Copying sources.                             !!!")
    MESSAGE( STATUS "!!!            Destination directory will be deleted, hence !!!")
    MESSAGE( STATUS "!!!            CHANGES WILL BE LOST with next CMake run!    !!!")
    MESSAGE( STATUS "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    MESSAGE( STATUS "Copying source files of selected ALib modules to: ’${ALIB_SOURCE_COPY_TARGET_DIR}’")

    # delete destination directory
    file( REMOVE_RECURSE ${ALIB_SOURCE_COPY_TARGET_DIR} )

    #### copy compilation units ####
    foreach( fileName IN LISTS   ALIB_INCLUDE_FILES  ALIB_SOURCE_FILES  )
        SET( destPath  "${ALIB_SOURCE_COPY_TARGET_DIR}/${fileName}" )
        get_filename_component(destPath ${destPath} DIRECTORY )


        file( COPY   "${ALIB_SOURCE_DIR}/${fileName}"  DESTINATION  "${destPath}" )
    endforeach()

    ### now also switch source base directories ###
    set( ALIB_SOURCE_DIR  ${ALIB_SOURCE_COPY_TARGET_DIR} )
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
