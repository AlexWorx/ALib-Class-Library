# ##################################################################################################
#  ALibSources.cmake - CMake file for projects using ALib
#
#  Copyright 2013-2025 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Notes:
#    Selects source files in lists depending on the selected modules listed in variable
#    ALibBuild. The lists are:
#    -  ALIB_MPP: C++20 Module files (respectively aggregation headers with legacy builds)
#    -  ALIB_INL: C++ declarations
#    -  ALIB_CPP: Compilation units.
#    -  ALIB_HPP: Legacy Header files which are not exported, for example, "platformincludes.hpp"
#    -  ALIB_H:   Module includes. The only files a using code may include.
#    If variable ALIB_SOURCE_COPY_TARGET_DIR is defined, then the sources are copied to the
#    directory specified.
#
#    This CMake file is included by "ALib.cmake" automatically and is not intended for manual
#    inclusion. Its contents was separated into a separated cmake file solely for clarity.
# ##################################################################################################
if( NOT DEFINED  ALIB_SOURCE_DIR )
    set ( ALIB_SOURCE_DIR       ${ALIB_BASE_DIR}/src    )
endif()

SET( ALIB_H    "" )
SET( ALIB_INL  "" )
SET( ALIB_MPP  "" )
SET( ALIB_CPP  "" )
SET( ALIB_HPP  "" )

# ---------- general files, always included -------

# alib
list( APPEND ALIB_H     ALib.Lang.CIFunctions.H        )
list( APPEND ALIB_H     ALib.Lang.CIMethods.H          )

list( APPEND ALIB_INL   alib.inl                       )

list( APPEND ALIB_INL   assert.inl                     )
list( APPEND ALIB_CPP   assert.cpp                     )
list( APPEND ALIB_INL   mainargs.inl                   )
list( APPEND ALIB_CPP   mainargs.cpp                   )
list( APPEND ALIB_INL   chk_nc.inl                     )
list( APPEND ALIB_HPP   platformincludes.hpp           )

#alib.Bootstrap
list( APPEND ALIB_H     ALib.Bootstrap.H               )
list( APPEND ALIB_MPP   bootstrap/bootstrap.mpp        )
list( APPEND ALIB_HPP   bootstrap/bootstrap.prepro.hpp )
list( APPEND ALIB_CPP   bootstrap/bootstrap.cpp        )


# alib::lang
list( APPEND ALIB_H     ALib.Lang.H                    )
list( APPEND ALIB_MPP   lang/lang.mpp                  )
list( APPEND ALIB_INL   lang/allocation.inl            )
list( APPEND ALIB_INL   lang/bidilist.inl              )
list( APPEND ALIB_INL   lang/bits.inl                  )
list( APPEND ALIB_INL   lang/bitset.inl                )
list( APPEND ALIB_INL   lang/callerinfo.inl            )
list( APPEND ALIB_INL   lang/commonenumdefs.inl        )
list( APPEND ALIB_INL   lang/dbgcriticalsections.inl   )
list( APPEND ALIB_CPP   lang/dbgcriticalsections.cpp   )
list( APPEND ALIB_INL   lang/dbgtypedemangler.inl      )
list( APPEND ALIB_CPP   lang/dbgtypedemangler.cpp      )
list( APPEND ALIB_INL   lang/integers.inl              )
list( APPEND ALIB_INL   lang/owner.inl                 )
list( APPEND ALIB_INL   lang/placeholder.inl           )
list( APPEND ALIB_INL   lang/plugins.inl               )
list( APPEND ALIB_INL   lang/sidilist.inl              )
list( APPEND ALIB_INL   lang/stdtypeinfofunctors.inl   )
list( APPEND ALIB_INL   lang/tmp.inl                   )

# alib::time
list( APPEND ALIB_H     ALib.Time.H                    )
list( APPEND ALIB_MPP   time/time.mpp                  )
list( APPEND ALIB_INL   time/datetime.inl              )
list( APPEND ALIB_CPP   time/datetime.cpp              )
list( APPEND ALIB_INL   time/stopwatch.inl             )
list( APPEND ALIB_INL   time/tickconverter.inl         )
list( APPEND ALIB_CPP   time/tickconverter.cpp         )
list( APPEND ALIB_INL   time/ticks.inl                 )
list( APPEND ALIB_INL   time/timepointbase.inl         )
list( APPEND ALIB_CPP   time/time.cpp                  )

# alib::threads
list( APPEND ALIB_H     ALib.Threads.H                 )
list( APPEND ALIB_MPP   threads/threads.mpp            )
list( APPEND ALIB_INL   threads/thread.inl             )
list( APPEND ALIB_INL   threads/lock.inl               )
list( APPEND ALIB_INL   threads/recursivelock.inl      )
list( APPEND ALIB_INL   threads/recursivetimedlock.inl )
list( APPEND ALIB_INL   threads/sharedlock.inl         )
list( APPEND ALIB_INL   threads/sharedtimedlock.inl    )
list( APPEND ALIB_INL   threads/timedlock.inl          )

if( NOT ALIB_SINGLE_THREADED )
  list( APPEND ALIB_INL threads/dbgasserters.inl       )
  list( APPEND ALIB_CPP threads/dbgasserters.cpp       )
  list( APPEND ALIB_INL threads/condition.inl          )
  list( APPEND ALIB_INL threads/promise.inl            )
  list( APPEND ALIB_CPP threads/promise.cpp            )

  list( APPEND ALIB_CPP threads/thread.cpp             )
  list( APPEND ALIB_CPP threads/locks.cpp              )
endif()

list( APPEND ALIB_INL   characters/chartypes.inl       )
list( APPEND ALIB_INL   characters/chartraits.inl      )

list( APPEND ALIB_H     ALib.Characters.Functions.H    )
list( APPEND ALIB_MPP   characters/functions.mpp       )
list( APPEND ALIB_INL   characters/functions.inl       )
list( APPEND ALIB_CPP   characters/functions.cpp       )
list( APPEND ALIB_H     ALib.Compatibility.QTCharacters.H        )

list( APPEND ALIB_H     ALib.EnumOps.H                            )
list( APPEND ALIB_MPP   enumops/enumops.mpp                       )
list( APPEND ALIB_HPP   enumops/enumops.prepro.hpp                )
list( APPEND ALIB_INL   enumops/underlyingintegral.inl            )
list( APPEND ALIB_INL   enumops/arithmetical.inl                  )
list( APPEND ALIB_INL   enumops/bitwise.inl                       )
list( APPEND ALIB_INL   enumops/iterable.inl                      )
list( APPEND ALIB_INL   enumops/iterablebitset.inl                )
list( APPEND ALIB_INL   enumops/bitwise_iterable_conversion.inl   )


#################################           Normal Modules             #############################

if( "ALOX" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.ALox.H                               )
    list( APPEND ALIB_H    ALib.ALox.Impl.H                          )
    list( APPEND ALIB_MPP  alox/alox_init.mpp                        )
    list( APPEND ALIB_INL  alox/alox_init.inl                        )
    list( APPEND ALIB_MPP  alox/alox_impl.mpp                        )
    list( APPEND ALIB_MPP  alox/alox.mpp                             )
    list( APPEND ALIB_HPP  alox/alox.prepro.hpp                      )
    list( APPEND ALIB_INL  alox/aloxcamp.inl                         )
    list( APPEND ALIB_INL  alox/log.inl                              )
    list( APPEND ALIB_INL  alox/logtools.inl                         )
    list( APPEND ALIB_INL  alox/lox.inl                              )

    list( APPEND ALIB_CPP  alox/aloxcamp.cpp                         )
    list( APPEND ALIB_CPP  alox/aloxinit.cpp                         )
    list( APPEND ALIB_CPP  alox/alox.cpp                             )
    list( APPEND ALIB_CPP  alox/logtools.cpp                         )
#
    list( APPEND ALIB_INL  alox/detail/domain.inl                    )
    list( APPEND ALIB_INL  alox/detail/logger.inl                    )
    list( APPEND ALIB_INL  alox/detail/loxpimpl.inl                  )
    list( APPEND ALIB_INL  alox/detail/scopedump.inl                 )
    list( APPEND ALIB_INL  alox/detail/scopeinfo.inl                 )
    list( APPEND ALIB_INL  alox/detail/scopestore.inl                )

    list( APPEND ALIB_CPP  alox/detail/domain.cpp                    )
    list( APPEND ALIB_CPP  alox/detail/loxpimpl.cpp                  )
    list( APPEND ALIB_CPP  alox/detail/scopedump.cpp                 )
    list( APPEND ALIB_CPP  alox/detail/scopeinfo.cpp                 )
    list( APPEND ALIB_CPP  alox/detail/scopestore.cpp                )

    list( APPEND ALIB_INL  alox/textlogger/plaintextlogger.inl       )
    list( APPEND ALIB_INL  alox/textlogger/textlogger.inl            )
    list( APPEND ALIB_INL  alox/textlogger/variables.inl             )
    list( APPEND ALIB_CPP  alox/textlogger/plaintextlogger.cpp       )
    list( APPEND ALIB_CPP  alox/textlogger/textlogger.cpp            )
    list( APPEND ALIB_CPP  alox/textlogger/variables.cpp             )

    list( APPEND ALIB_INL  alox/loggers/ansilogger.inl               )
    list( APPEND ALIB_INL  alox/loggers/consolelogger.inl            )
    list( APPEND ALIB_INL  alox/loggers/memorylogger.inl             )
    list( APPEND ALIB_INL  alox/loggers/textfilelogger.inl           )
    list( APPEND ALIB_INL  alox/loggers/vstudiologger.inl            )
    list( APPEND ALIB_INL  alox/loggers/windowsconsolelogger.inl     )

    list( APPEND ALIB_CPP  alox/loggers/ansilogger.cpp               )
    list( APPEND ALIB_CPP  alox/loggers/consolelogger.cpp            )
    list( APPEND ALIB_CPP  alox/loggers/textfilelogger.cpp           )
    list( APPEND ALIB_CPP  alox/loggers/vstudiologger.cpp            )
    list( APPEND ALIB_CPP  alox/loggers/windowsconsolelogger.cpp     )

endif()

if( "BITBUFFER" IN_LIST ALibBuild )
    list( APPEND ALIB_H     ALib.BitBuffer.H                         )
    list( APPEND ALIB_MPP   bitbuffer/bitbuffer.mpp                  )
    list( APPEND ALIB_HPP   bitbuffer/bitbuffer.prepro.hpp           )
    list( APPEND ALIB_INL   bitbuffer/arraycompressor.inl            )
    list( APPEND ALIB_INL   bitbuffer/bitbuffer.inl                  )
    list( APPEND ALIB_INL   bitbuffer/ac_v1/acalgos.inl.inl          )
    list( APPEND ALIB_INL   bitbuffer/ac_v1/ac.inl                   )
    list( APPEND ALIB_INL   bitbuffer/ac_v1/huffman.inl              )

    list( APPEND ALIB_CPP   bitbuffer/bitbuffer.cpp                  )
    list( APPEND ALIB_CPP   bitbuffer/ac_v1/ac.cpp                   )
    list( APPEND ALIB_CPP   bitbuffer/ac_v1/huffman.cpp              )
endif()

if( "BOXING" IN_LIST ALibBuild )
    list( APPEND ALIB_H     ALib.Boxing.H                            )
    list( APPEND ALIB_MPP   boxing/boxing.mpp                        )
    list( APPEND ALIB_HPP   boxing/boxing.prepro.hpp                 )
    list( APPEND ALIB_INL   boxing/box.inl                           )
    list( APPEND ALIB_INL   boxing/boxes.inl                         )
    list( APPEND ALIB_INL   boxing/boxstrap.inl                      )
    list( APPEND ALIB_INL   boxing/enum.inl                          )
    list( APPEND ALIB_INL   boxing/placeholder.inl                   )
    list( APPEND ALIB_INL   boxing/boxingtraits.inl                  )
    list( APPEND ALIB_INL   boxing/boxing_debug.inl                  )

    list( APPEND ALIB_INL   boxing/boxingcustoms.inl.inl             )
    list( APPEND ALIB_INL   boxing/functiondecls.inl                 )
    list( APPEND ALIB_INL   boxing/functiondefs.inl.inl              )
    list( APPEND ALIB_INL   boxing/detail/vtable.inl                 )

    list( APPEND ALIB_CPP   boxing/boxstrap.cpp                      )
    list( APPEND ALIB_CPP   boxing/boxes.cpp                         )
    list( APPEND ALIB_CPP   boxing/box.cpp                           )
    list( APPEND ALIB_CPP   boxing/detail/vtable.cpp                 )
    list( APPEND ALIB_CPP   boxing/detail/boxingcustoms.cpp          )
    list( APPEND ALIB_CPP   boxing/detail/dbgdoublelengthcheck.cpp   )
    list( APPEND ALIB_CPP   boxing/boxing_debug.cpp                  )

    list( APPEND ALIB_H     ALib.Boxing.StdFunctors.H                )
    list( APPEND ALIB_H     ALib.Compatibility.StdBoxtraits.H        )
    list( APPEND ALIB_H     ALib.Compatibility.QTBoxing.H            )

    if( "STRINGS" IN_LIST ALibBuild )
      list( APPEND ALIB_INL boxing/strings_appendables.inl           )
      list( APPEND ALIB_INL boxing/extensions/strings_tfield.inl     )
      list( APPEND ALIB_CPP boxing/extensions/strings_tfield.cpp     )

    endif()
endif()

if( "CAMP" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.Camp.H                      )
    list( APPEND ALIB_MPP  camp/camp.mpp                    )
    list( APPEND ALIB_HPP  camp/camp.prepro.hpp             )
    list( APPEND ALIB_INL  camp/camp.inl                    )

    # additions from lower-level modules
    list( APPEND ALIB_INL  camp/loadtokens.inl              )

    # Basecamp
    list( APPEND ALIB_H    ALib.Camp.Base.H                 )
    list( APPEND ALIB_MPP  camp/basecamp.mpp                )
    list( APPEND ALIB_CPP  camp/basecamp.cpp                )
endif()

if( "CLI" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.CLI.H                       )
    list( APPEND ALIB_MPP  cli/cli.mpp                      )
    list( APPEND ALIB_INL  cli/arguments.inl                )
    list( APPEND ALIB_INL  cli/clicamp.inl                  )
    list( APPEND ALIB_INL  cli/cliutil.inl                  )
    list( APPEND ALIB_INL  cli/commandline.inl              )

    list( APPEND ALIB_CPP  cli/arguments.cpp                )
    list( APPEND ALIB_CPP  cli/clicamp.cpp                  )
    list( APPEND ALIB_CPP  cli/cliutil.cpp                  )
    list( APPEND ALIB_CPP  cli/commandline.cpp              )
endif()

if( "VARIABLES" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.Variables.H                    )
    list( APPEND ALIB_MPP  variables/variables.mpp             )
    list( APPEND ALIB_HPP  variables/variables.prepro.hpp      )
    list( APPEND ALIB_INL  variables/erpriority.inl            )
    list( APPEND ALIB_INL  variables/vmeta.inl                 )
    list( APPEND ALIB_INL  variables/declaration.inl           )
    list( APPEND ALIB_INL  variables/configuration.inl         )
    list( APPEND ALIB_CPP  variables/configuration.cpp         )
    list( APPEND ALIB_CPP  variables/vmeta.cpp                 )
    list( APPEND ALIB_CPP  variables/variable.cpp              )

    list( APPEND ALIB_H    ALib.Variables.Plugins.H            )
    list( APPEND ALIB_MPP  variables/plugins.mpp               )
    list( APPEND ALIB_CPP  variables/plugins.cpp               )

    list( APPEND ALIB_H    ALib.Variables.IniFile.H            )
    list( APPEND ALIB_MPP  variables/inifile.mpp               )
    list( APPEND ALIB_INL  variables/inifile.inl               )
    list( APPEND ALIB_INL  variables/inifilefeeder.inl         )
    list( APPEND ALIB_CPP  variables/inifile.cpp               )
    list( APPEND ALIB_CPP  variables/inifilefeeder.cpp         )

    list( APPEND ALIB_H    ALib.Variables.ResourcePool.H       )
    list( APPEND ALIB_MPP  variables/configresourcepool.mpp    )
    list( APPEND ALIB_CPP  variables/configresourcepool.cpp    )



endif()

if( "CONTAINERS" IN_LIST ALibBuild )
    list( APPEND ALIB_HPP  containers/containers.prepro.hpp          )
    list( APPEND ALIB_INL  containers/recycling.inl                  )
    list( APPEND ALIB_INL  containers/valuedescriptor.inl            )

    list( APPEND ALIB_H    ALib.Containers.FixedCapacityVector.H     )
    list( APPEND ALIB_MPP  containers/fixedcapacityvector.mpp        )
    list( APPEND ALIB_INL  containers/fixedcapacityvector.inl        )

    list( APPEND ALIB_H    ALib.Containers.HashTable.H               )
    list( APPEND ALIB_MPP  containers/hashtable.mpp                  )
    list( APPEND ALIB_INL  containers/detail/hashtablebase.inl       )
    list( APPEND ALIB_INL  containers/hashtable.inl                  )
    list( APPEND ALIB_CPP   containers/detail/hashtablebase.cpp      )

    list( APPEND ALIB_H    ALib.Containers.List.H                    )
    list( APPEND ALIB_MPP  containers/list.mpp                       )
    list( APPEND ALIB_INL  containers/list.inl                       )

    list( APPEND ALIB_H    ALib.Containers.LRUCacheTable.H           )
    list( APPEND ALIB_MPP  containers/lrucachetable.mpp              )
    list( APPEND ALIB_INL  containers/lrucachetable.inl              )

    list( APPEND ALIB_H    ALib.Containers.SharedPtr.H               )
    list( APPEND ALIB_MPP  containers/sharedptr.mpp                  )
    list( APPEND ALIB_INL  containers/sharedptr.inl                  )

    list( APPEND ALIB_H    ALib.Containers.SharedVal.H               )
    list( APPEND ALIB_MPP  containers/sharedval.mpp                  )
    list( APPEND ALIB_INL  containers/sharedval.inl                  )

    if( "STRINGS" IN_LIST ALibBuild )
      list( APPEND ALIB_H     ALib.Containers.StringTree.H            )
      list( APPEND ALIB_MPP   containers/stringtree.mpp               )
      list( APPEND ALIB_INL   containers/stringtree.inl               )
      list( APPEND ALIB_INL   containers/detail/stringtreebase.inl    )
      list( APPEND ALIB_CPP   containers/detail/stringtreebase.cpp    )
      list( APPEND ALIB_H     ALib.Containers.StringTreeIterator.H    )
      list( APPEND ALIB_MPP   containers/stringtreeiterator.mpp       )
      list( APPEND ALIB_INL   containers/stringtreeiterator.inl       )
    endif()
endif()


if( "ENUMRECORDS" IN_LIST ALibBuild )
    list( APPEND ALIB_H     ALib.EnumRecords.H                            )
    list( APPEND ALIB_MPP   enumrecords/enumrecords.mpp                   )
    list( APPEND ALIB_HPP   enumrecords/enumrecords.prepro.hpp            )
    list( APPEND ALIB_INL   enumrecords/detail/enumrecordmap.inl          )
    list( APPEND ALIB_INL   enumrecords/records.inl                       )
    list( APPEND ALIB_INL   enumrecords/serialization.inl                 )
    list( APPEND ALIB_INL   enumrecords/builtin.inl                       )

    list( APPEND ALIB_H     ALib.EnumRecords.Bootstrap.H                  )
    list( APPEND ALIB_MPP   enumrecords/bootstrap/recordbootstrap.mpp     )
    list( APPEND ALIB_INL   enumrecords/bootstrap/recordparser.inl        )
    list( APPEND ALIB_INL   enumrecords/bootstrap/recordbootstrap.inl     )
    list( APPEND ALIB_CPP   enumrecords/bootstrap/recordparser.cpp        )
    if( "BOXING" IN_LIST ALibBuild )
      list( APPEND ALIB_INL enumrecords/boxing_enums.inl                  )
    endif()
    list( APPEND ALIB_CPP   enumrecords/records.cpp                       )

endif()

if( "EXCEPTIONS" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.Exceptions.H                       )
    list( APPEND ALIB_MPP  exceptions/exceptions.mpp               )
    list( APPEND ALIB_INL  exceptions/message.inl                  )
    list( APPEND ALIB_INL  exceptions/exception.inl                )
    list( APPEND ALIB_CPP  exceptions/exception.cpp                )
endif()

if( "EXPRESSIONS" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.Expressions.H                      )
    list( APPEND ALIB_H    ALib.Expressions.Impl.H                 )
    list( APPEND ALIB_MPP  expressions/expressions.mpp             )
    list( APPEND ALIB_MPP  expressions/expressions_impl.mpp        )
    list( APPEND ALIB_INL  expressions/detail/ast.inl              )
    list( APPEND ALIB_INL  expressions/parser.inl                  )
    list( APPEND ALIB_HPP  expressions/expressions.prepro.hpp      )
    list( APPEND ALIB_INL  expressions/expressionscamp.inl         )
    list( APPEND ALIB_INL  expressions/compiler.inl                )
    list( APPEND ALIB_INL  expressions/compilerplugin.inl          )
    list( APPEND ALIB_INL  expressions/expression.inl              )
    list( APPEND ALIB_INL  expressions/scope.inl                   )
    list( APPEND ALIB_INL  expressions/standardrepository.inl      )

    list( APPEND ALIB_INL  expressions/detail/ast_impl.inl         )
    list( APPEND ALIB_INL  expressions/detail/parser_impl.inl      )
    list( APPEND ALIB_INL  expressions/detail/program.inl          )
    list( APPEND ALIB_INL  expressions/detail/virtualmachine.inl   )

    list( APPEND ALIB_INL  expressions/plugins/arithmetics.inl     )
    list( APPEND ALIB_INL  expressions/plugins/autocast.inl        )
    list( APPEND ALIB_INL  expressions/plugins/calculus.inl        )
    list( APPEND ALIB_INL  expressions/plugins/dateandtime.inl     )
    list( APPEND ALIB_INL  expressions/plugins/elvisoperator.inl   )
    list( APPEND ALIB_INL  expressions/plugins/math.inl            )
    list( APPEND ALIB_INL  expressions/plugins/strings.inl         )

    list( APPEND ALIB_CPP  expressions/expressionscamp.cpp         )
    list( APPEND ALIB_CPP  expressions/compiler.cpp                )
    list( APPEND ALIB_CPP  expressions/expression.cpp              )
    list( APPEND ALIB_CPP  expressions/standardrepository.cpp      )

    list( APPEND ALIB_CPP  expressions/parser.cpp           )
    
    list( APPEND ALIB_CPP  expressions/detail/ast.cpp              )
    list( APPEND ALIB_CPP  expressions/detail/parser_impl.cpp      )
    list( APPEND ALIB_CPP  expressions/detail/program.cpp          )
    list( APPEND ALIB_CPP  expressions/detail/virtualmachine.cpp   )

    list( APPEND ALIB_CPP  expressions/plugins/arithmetics.cpp     )
    list( APPEND ALIB_CPP  expressions/plugins/autocast.cpp        )
    list( APPEND ALIB_CPP  expressions/plugins/calculus.cpp        )
    list( APPEND ALIB_CPP  expressions/plugins/elvisoperator.cpp   )
    list( APPEND ALIB_CPP  expressions/plugins/math.cpp            )
    list( APPEND ALIB_CPP  expressions/plugins/strings.cpp         )
    list( APPEND ALIB_CPP  expressions/plugins/dateandtime.cpp     )

    list( APPEND ALIB_INL  expressions/util/expressionformatter.inl )
    list( APPEND ALIB_CPP  expressions/util/expressionformatter.cpp )

 endif()

if( "FORMAT" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.Format.H                               )
    list( APPEND ALIB_MPP  format/format.mpp                           )
    list( APPEND ALIB_INL  format/formatter.inl                        )
    list( APPEND ALIB_INL  format/fmtexceptions.inl                    )
    list( APPEND ALIB_INL  format/bytesize.inl                         )
    list( APPEND ALIB_INL  format/fmtcallerinfo.inl                    )
    list( APPEND ALIB_INL  format/fmtdatetime.inl                      )
    list( APPEND ALIB_INL  format/extensions/containers_hashtable.inl  )
    list( APPEND ALIB_INL  format/extensions/boxing_format_debug.inl   )
    list( APPEND ALIB_INL  format/extensions/resources_debug.inl       )
    list( APPEND ALIB_CPP  format/formatter.cpp                        )
    list( APPEND ALIB_CPP  format/fmtvarious.cpp                       )
    list( APPEND ALIB_CPP  format/extensions/boxing_format_debug.cpp   )
    list( APPEND ALIB_CPP  format/extensions/resources_debug.cpp       )



    list( APPEND ALIB_H    ALib.Format.StdImplementation.H   )
    list( APPEND ALIB_MPP  format/formatterstdimpl.mpp       )
    list( APPEND ALIB_INL  format/formatterstdimpl.inl       )
    list( APPEND ALIB_CPP  format/formatterstdimpl.cpp       )

    list( APPEND ALIB_H    ALib.Format.FormatterPythonStyle.H)
    list( APPEND ALIB_MPP  format/formatterpythonstyle.mpp   )
    list( APPEND ALIB_INL  format/formatterpythonstyle.inl   )
    list( APPEND ALIB_CPP  format/formatterpythonstyle.cpp   )

    list( APPEND ALIB_H    ALib.Format.FormatterJavaStyle.H  )
    list( APPEND ALIB_MPP  format/formatterjavastyle.mpp     )
    list( APPEND ALIB_INL  format/formatterjavastyle.inl     )
    list( APPEND ALIB_CPP  format/formatterjavastyle.cpp     )

    list( APPEND ALIB_H    ALib.Format.Paragraphs.H          )
    list( APPEND ALIB_MPP  format/paragraphs.mpp             )
    list( APPEND ALIB_INL  format/paragraphs.inl             )
    list( APPEND ALIB_CPP  format/paragraphs.cpp             )

    list( APPEND ALIB_H    ALib.Format.PropertyFormatter.H   )
    list( APPEND ALIB_MPP  format/propertyformatter.mpp      )
    list( APPEND ALIB_INL  format/propertyformatter.inl      )
    list( APPEND ALIB_CPP  format/propertyformatter.cpp      )

    list( APPEND ALIB_H    ALib.Format.PropertyFormatters.H  )
    list( APPEND ALIB_MPP  format/propertyformatters.mpp     )


endif()


if( "MONOMEM" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.Monomem.H                          )
    list( APPEND ALIB_MPP  monomem/monomem.mpp                     )
    list( APPEND ALIB_HPP  monomem/monomem.prepro.hpp              )
    list( APPEND ALIB_INL  monomem/monoallocator.inl               )
    list( APPEND ALIB_INL  monomem/monoallocator.t.inl             )
    list( APPEND ALIB_INL  monomem/localallocator.inl              )
    list( APPEND ALIB_INL  monomem/poolallocator.inl               )
    list( APPEND ALIB_INL  monomem/poolallocator.t.inl             )
    list( APPEND ALIB_CPP  monomem/monomem.cpp                     )

    list( APPEND ALIB_H    ALib.Monomem.SharedMonoVal.H            )
    list( APPEND ALIB_MPP  monomem/sharedmonoval.mpp               )
    list( APPEND ALIB_INL  monomem/sharedmonoval.inl               )

    list( APPEND ALIB_H    ALib.Monomem.StdContainers.H            )
endif()


if( "SINGLETONS" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.Singletons.H                       )
    list( APPEND ALIB_MPP  singletons/singletons.mpp               )
    list( APPEND ALIB_HPP  singletons/singletons.prepro.hpp        )
    list( APPEND ALIB_CPP  singletons/singletons.cpp               )
endif()

if( "STRINGS" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.Strings.H                          )
    list( APPEND ALIB_MPP  strings/strings.mpp                     )
    list( APPEND ALIB_HPP  strings/strings.prepro.hpp              )
    list( APPEND ALIB_INL  strings/appendables.inl                 )
    list( APPEND ALIB_INL  strings/astring.inl                     )
    list( APPEND ALIB_INL  strings/cstring.inl                     )
    list( APPEND ALIB_INL  strings/format.inl                      )
    list( APPEND ALIB_INL  strings/numberformat.inl                )
    list( APPEND ALIB_INL  strings/localstring.inl                 )
    list( APPEND ALIB_INL  strings/string.inl                      )
    list( APPEND ALIB_INL  strings/stringnzt.inl                   )
    list( APPEND ALIB_INL  strings/substring.inl                   )
    list( APPEND ALIB_INL  strings/detail/tastring.inl             )
    list( APPEND ALIB_INL  strings/detail/tastringimpl.inl         )

    list( APPEND ALIB_INL  strings/detail/tastring_ca_decl.inl.inl )
    list( APPEND ALIB_INL  strings/detail/tastring_ca_impl.inl.inl )
    list( APPEND ALIB_INL  strings/detail/numberconversion.inl     )

    list( APPEND ALIB_CPP  strings/astring.cpp                     )
    list( APPEND ALIB_CPP  strings/numberformat.cpp                )
    list( APPEND ALIB_CPP  strings/string.cpp                      )
    list( APPEND ALIB_CPP  strings/substring.cpp                   )
    list( APPEND ALIB_CPP  strings/detail/numberconversion.cpp     )

    list( APPEND ALIB_H    ALib.Compatibility.QTStrings.H          )
    list( APPEND ALIB_H    ALib.Strings.StdFunctors.H              )
    list( APPEND ALIB_H    ALib.Strings.StdIOStream.H              )
    list( APPEND ALIB_MPP  strings/stdiostream.mpp                 )
    list( APPEND ALIB_CPP  strings/stdiostream.cpp                 )

    list( APPEND ALIB_H    ALib.Compatibility.StdStrings.H         )

    list( APPEND ALIB_H    ALib.Strings.Tokenizer.H                )
    list( APPEND ALIB_MPP  strings/util/tokenizer.mpp              )
    list( APPEND ALIB_INL  strings/util/tokenizer.inl              )
    list( APPEND ALIB_CPP  strings/util/tokenizer.cpp              )

    list( APPEND ALIB_H    ALib.Strings.Escaper.H                  )
    list( APPEND ALIB_MPP  strings/util/escaper.mpp                )
    list( APPEND ALIB_INL  strings/util/escaper.inl                )
    list( APPEND ALIB_CPP  strings/util/escaper.cpp                )

    list( APPEND ALIB_H    ALib.Strings.Search.H                   )
    list( APPEND ALIB_MPP  strings/util/search.mpp                 )
    list( APPEND ALIB_INL  strings/util/search.inl                 )
    list( APPEND ALIB_INL  strings/util/regexmatcher.inl           )
    list( APPEND ALIB_INL  strings/util/wildcardmatcher.inl        )
    list( APPEND ALIB_CPP  strings/util/search.cpp                 )
    list( APPEND ALIB_CPP  strings/util/regexmatcher.cpp           )
    list( APPEND ALIB_CPP  strings/util/wildcardmatcher.cpp        )

    list( APPEND ALIB_H    ALib.Strings.Calendar.H                 )
    list( APPEND ALIB_MPP  strings/util/calendar.mpp               )
    list( APPEND ALIB_INL  strings/util/calendar.inl               )
    list( APPEND ALIB_CPP  strings/util/calendar.cpp               )

    list( APPEND ALIB_H    ALib.Strings.Token.H                    )
    list( APPEND ALIB_MPP  strings/util/token.mpp                  )
    list( APPEND ALIB_INL  strings/util/token.inl                  )
    list( APPEND ALIB_CPP  strings/util/token.cpp                  )

    list( APPEND ALIB_H    ALib.Strings.AutoSizes.H                )
    list( APPEND ALIB_MPP  strings/util/autosizes.mpp              )
    list( APPEND ALIB_INL  strings/util/autosizes.inl              )
    list( APPEND ALIB_CPP  strings/util/autosizes.cpp              )

    list( APPEND ALIB_H    ALib.Strings.Vector.H                   )
    

  if( "MONOMEM" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.Strings.Monomem.H                  )
    list( APPEND ALIB_MPP  strings/monomem/strings_monomem.mpp     )
    list( APPEND ALIB_INL  strings/monomem/strings_monomem.t.inl   )
    list( APPEND ALIB_CPP  strings/monomem/strings_monomem.cpp     )
  endif()
endif()

if( "RESOURCES" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.Resources.H                             )
    list( APPEND ALIB_MPP  resources/resources.mpp                      )
    list( APPEND ALIB_HPP  resources/resources.prepro.hpp               )
    list( APPEND ALIB_INL  resources/resources.inl                      )
    list( APPEND ALIB_INL  resources/detail/resourcemap.inl             )
    list( APPEND ALIB_INL  resources/localresourcepool.inl              )

    list( APPEND ALIB_CPP  resources/localresourcepool.cpp              )

    # additions from lower-level modules
    list( APPEND ALIB_INL  resources/extensions/enums_serialization.inl )
    list( APPEND ALIB_INL  resources/extensions/strings_loadtokens.inl  )
    list( APPEND ALIB_CPP  resources/extensions/strings_loadtokens.cpp  )
endif()

if( "SYSTEM" IN_LIST ALibBuild )

    #### BASE/system ###
    list( APPEND ALIB_H    ALib.System.H                       )
    list( APPEND ALIB_MPP  system/system.mpp                   )
    list( APPEND ALIB_HPP  system/system.prepro.hpp            )
    list( APPEND ALIB_INL  system/console.inl                  )
    list( APPEND ALIB_INL  system/environment.inl              )
    list( APPEND ALIB_INL  system/path.inl                     )
    list( APPEND ALIB_INL  system/processinfo.inl              )
    list( APPEND ALIB_INL  system/shellcommand.inl             )
    list( APPEND ALIB_INL  system/systemerrors.inl             )

    list( APPEND ALIB_CPP  system/console.cpp                  )
    list( APPEND ALIB_CPP  system/environment.cpp              )
    list( APPEND ALIB_CPP  system/path.cpp                     )
    list( APPEND ALIB_CPP  system/processinfo.cpp              )
    list( APPEND ALIB_CPP  system/shellcommand.cpp             )
    if ( ${APPLE} )
      list( APPEND ALIB_CPP system/path.mm                     )
    endif()
endif()

if( "THREADMODEL" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.ThreadModel.H                  )
    list( APPEND ALIB_MPP  threadmodel/threadmodel.mpp         )
    list( APPEND ALIB_INL  threadmodel/dedicatedworker.inl     )
    list( APPEND ALIB_INL  threadmodel/jobs.inl                )
    list( APPEND ALIB_INL  threadmodel/threadpool.inl          )
    list( APPEND ALIB_INL  threadmodel/trigger.inl             )

    list( APPEND ALIB_CPP  threadmodel/dedicatedworker.cpp     )
    list( APPEND ALIB_CPP  threadmodel/threadpool.cpp          )
    list( APPEND ALIB_CPP  threadmodel/trigger.cpp             )
endif()


if( "FILES" IN_LIST ALibBuild )
    list( APPEND ALIB_H    ALib.Files.H                            )
    list( APPEND ALIB_MPP  files/files.mpp                         )
    list( APPEND ALIB_HPP  files/files.prepro.hpp                  )
    list( APPEND ALIB_INL  files/filescamp.inl                     )
    list( APPEND ALIB_INL  files/ffilter.inl                       )
    list( APPEND ALIB_INL  files/finfo.inl                         )
    list( APPEND ALIB_INL  files/fscanner.inl                      )
    list( APPEND ALIB_INL  files/ftree.inl                         )

    list( APPEND ALIB_CPP  files/filescamp.cpp                     )
    list( APPEND ALIB_CPP  files/file.cpp                          )
    list( APPEND ALIB_CPP  files/finfo.cpp                         )
    list( APPEND ALIB_CPP  files/fscanner.cpp                      )
    list( APPEND ALIB_CPP  files/ftree.cpp                         )

    list( APPEND ALIB_H    ALib.Files.TextFile.H                   )

    if( "EXPRESSIONS" IN_LIST ALibBuild )
      list( APPEND ALIB_H   ALib.Files.Expressions.H               )
      list( APPEND ALIB_MPP files/expressions/fileexpressions.mpp  )
      list( APPEND ALIB_INL files/expressions/fileexpressions.inl  )
      list( APPEND ALIB_CPP files/expressions/fileexpressions.cpp  )
    endif()

endif()




# ##################################################################################################
# Check for duplicates. This is a double-check to detect that this script needs a fix.
# ##################################################################################################

CollectAndRemoveDuplicates( "ALIB_HPP"  "duplicates" )
LIST( LENGTH  duplicates  cntDuplicates )

IF( ${cntDuplicates} GREATER  0  )
    message( ERROR  "Internal script error ALIBSources.cmake: "
                     "DUPLICATE HPP FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE HPP FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()

CollectAndRemoveDuplicates( "ALIB_INL"  "duplicates" )
LIST( LENGTH  duplicates  cntDuplicates )
IF( ${cntDuplicates} GREATER  0  )
    message( ERROR  "Internal script error ALIBSources.cmake: "
                     "DUPLICATE INL FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE INL FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()

CollectAndRemoveDuplicates( "ALIB_CPP"  "duplicates" )
LIST( LENGTH  duplicates  cntDuplicates )
IF( ${cntDuplicates} GREATER  0  )
    message( ERROR "Internal script error ALIBSources.cmake: "
                     "DUPLICATE CPP FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE CPP FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()
CollectAndRemoveDuplicates( "ALIB_MPP"  "duplicates" )
LIST( LENGTH  duplicates  cntDuplicates )
IF( ${cntDuplicates} GREATER  0  )
    message( ERROR "Internal script error ALIBSources.cmake: "
                     "DUPLICATE MPP FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE MPP FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()
# ##################################################################################################
# Check for ".hpp" in header-files or ".cpp" in sources
# ##################################################################################################
FOREACH( entry IN LISTS ALIB_HPP  )
    IF( NOT ${entry} MATCHES "([.]hpp$)|([.]inl$)|([.]mpp$)"  )
        message( FATAL_ERROR  "Non-header-file (wrong extension) found in header-file list: ${entry}" )
    ENDIF()
ENDFOREACH()

FOREACH( entry IN LISTS ALIB_CPP  )
    IF( NOT ${entry} MATCHES "([.]cpp$)|([.]cppm$)|([.]mm$)"  )
        message( FATAL_ERROR  "File with wrong extension found in source file list: ${entry}" )
    ENDIF()
ENDFOREACH()

# ##################################################################################################
# Copy sources if requested
# ##################################################################################################

if( ALIB_SOURCE_COPY_TARGET_DIR )

    # check: must be single folder name (not a path)
    string(FIND "${ALIB_SOURCE_COPY_TARGET_DIR}" "/" separatorPosUnix)
    string(FIND "${ALIB_SOURCE_COPY_TARGET_DIR}" "\\" separatorPosWindows)

    if(separatorPosUnix GREATER -1 OR separatorPosWindows GREATER -1)
        MESSAGE(FATAL_ERROR "The given variable ALIB_SOURCE_COPY_TARGET_DIR=\"${ALIB_SOURCE_COPY_TARGET_DIR}\" "
                "must be a folder name and not contain folder separators!")
    endif()

    # Prepend build directory
    set(ALIB_SOURCE_COPY_TARGET_DIR "${CMAKE_BINARY_DIR}/${ALIB_SOURCE_COPY_TARGET_DIR}")

    MESSAGE( STATUS "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    MESSAGE( STATUS "!!! ATTENTION: Copying sources.                                 !!!")
    MESSAGE( STATUS "!!!            The destination directory will be deleted, hence !!!")
    MESSAGE( STATUS "!!!            CHANGES WILL BE LOST with next CMake run!        !!!")
    MESSAGE( STATUS "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    MESSAGE( STATUS "Copying source files of selected ALib modules to: ’${ALIB_SOURCE_COPY_TARGET_DIR}’")

    # delete destination directory
    MESSAGE( "Cleaning source-copy target directory ${ALIB_SOURCE_COPY_TARGET_DIR}" )
    file( REMOVE_RECURSE ${ALIB_SOURCE_COPY_TARGET_DIR} )

    #### Create directories units ####
    foreach( fileName IN LISTS   ALIB_H  )
        SET( destPath  "${ALIB_SOURCE_COPY_TARGET_DIR}/ALib/${fileName}" )
        get_filename_component(destPath ${destPath} DIRECTORY )
        #MESSAGE("Copying file: ${ALIB_SOURCE_DIR}/ALib/${fileName}" )
        #MESSAGE("          to: ${destPath}" )
        file( MAKE_DIRECTORY "${destPath}")
        file( COPY   "${ALIB_SOURCE_DIR}/${fileName}"  DESTINATION  "${destPath}" )
    endforeach()

    foreach( fileName IN LISTS   ALIB_MPP ALIB_INL ALIB_HPP  ALIB_CPP  )
        SET( destPath  "${ALIB_SOURCE_COPY_TARGET_DIR}/alib/${fileName}" )
        get_filename_component(destPath ${destPath} DIRECTORY )
        file( MAKE_DIRECTORY "${destPath}")
        file( COPY   "${ALIB_SOURCE_DIR}/alib/${fileName}"  DESTINATION  "${destPath}" )
    endforeach()


endif()


# ##################################################################################################
# Check if all given files exist and check all files are given
# ##################################################################################################
function(CheckFileInclusionForCompleteness fileList baseDir extension )
    # Ensure the baseDir is an absolute path
    if(NOT IS_ABSOLUTE "${baseDir}")
        message(FATAL_ERROR "The provided baseDir '${baseDir}' must be an absolute path.")
    endif()

    # Step 1: Loop over the given list and check for file existence
    foreach(file_path IN LISTS fileList)

        # Form the absolute path of the file relative to baseDir
        set(absolute_file_path "${baseDir}/${file_path}")

        # Check if the file exists
        if(NOT EXISTS "${absolute_file_path}")
            message(WARNING "File does not exist: ${absolute_file_path}")
        endif()

    endforeach()

    # Step 2: Glob the baseDir for all files with the given extension
    file(GLOB_RECURSE found_files "${baseDir}/*.${extension}")

    # Normalize the found files to be relative to baseDir, for easier comparison
    set(relative_found_files "")
    foreach(file_path IN LISTS found_files)
        file(RELATIVE_PATH relative_file "${baseDir}" "${file_path}")
        list(APPEND relative_found_files "${relative_file}")
    endforeach()

    # Step 3: Compare the globbed files with the provided file list
    foreach(found_file IN LISTS relative_found_files)
        # Check if the found file is in the list
        list(FIND fileList "${found_file}" index)

        if(${index} EQUAL -1)
            message(WARNING "Found an untracked file: ${found_file}")
        endif()
    endforeach()
endfunction()

# ##################################################################################################
# Check if all given files have one of the given extensions
# ##################################################################################################
function(ValidateFileExtensions filenames_var extensions_var)
    # Dereference the input variable names to get their values
    set(filenames ${${filenames_var}})
    set(extensions ${${extensions_var}})

    # Debug information
    #string(JOIN " " actual_filenames ${filenames})
    #string(JOIN " " allowed_extensions ${extensions})
    #message(STATUS "Validating files: ${actual_filenames}")
    #message(STATUS "Allowed extensions: ${allowed_extensions}")

    # Iterate over each filename
    foreach(filename IN LISTS filenames)
        # Extract the file extension using regex
        string(REGEX MATCH "\\.[^.]*$" file_extension "${filename}")

        # Remove the dot from the extension
        string(REGEX REPLACE "\\." "" clean_extension "${file_extension}")

        # Check if the extension is in the allowed list
        list(FIND extensions "${clean_extension}" index)

        if(${index} EQUAL -1) # -1 means the extension is invalid
            message(FATAL_ERROR "File '${filename}' has an invalid extension: '${clean_extension}'. Allowed extensions are: ${allowed_extensions}")
        endif()
    endforeach()

    # Success message if all extensions are valid
    #message(STATUS "All files have valid extensions.")
endfunction()

# ##################################################################################################
# Add base directory to files
# ##################################################################################################
function(AddBaseDir baseDir sourceListVar )
    SET(temp "")  # Temporary list to store valid files

    # Iterate over the files in the provided list
    foreach(fileName IN LISTS "${sourceListVar}")
        if(EXISTS "${baseDir}/${fileName}")
            LIST(APPEND temp "${baseDir}/${fileName}")
        else()
            message(FATAL_ERROR "Source file not found: ${baseDir}/${fileName}")
        endif()
    endforeach()

    # Set output variable with processed list
    SET("${sourceListVar}" "${temp}" PARENT_SCOPE)
endfunction()

if(ALibAllModules)
    CheckFileInclusionForCompleteness( "${ALIB_H}"   "${ALIB_SOURCE_DIR}"      "H"   )
    CheckFileInclusionForCompleteness( "${ALIB_MPP}" "${ALIB_SOURCE_DIR}/alib" "mpp" )
    CheckFileInclusionForCompleteness( "${ALIB_INL}" "${ALIB_SOURCE_DIR}/alib" "inl" )
    CheckFileInclusionForCompleteness( "${ALIB_CPP}" "${ALIB_SOURCE_DIR}/alib" "cpp" )
    CheckFileInclusionForCompleteness( "${ALIB_HPP}" "${ALIB_SOURCE_DIR}/alib" "hpp" )
endif()
set(allowedExtensions "H" "hpp" )
                                     ValidateFileExtensions(ALIB_H   allowedExtensions)
set(allowedExtensions "mpp" )
                                     ValidateFileExtensions(ALIB_MPP allowedExtensions)
set(allowedExtensions "inl" )
                                     ValidateFileExtensions(ALIB_INL allowedExtensions)
set(allowedExtensions "hpp" )
                                     ValidateFileExtensions(ALIB_HPP allowedExtensions)
set(allowedExtensions "cpp" "mm" )
                                     ValidateFileExtensions(ALIB_CPP allowedExtensions)
AddBaseDir("${ALIB_SOURCE_DIR}"      ALIB_H   )
AddBaseDir("${ALIB_SOURCE_DIR}/alib" ALIB_MPP )
AddBaseDir("${ALIB_SOURCE_DIR}/alib" ALIB_INL )
AddBaseDir("${ALIB_SOURCE_DIR}/alib" ALIB_HPP )
AddBaseDir("${ALIB_SOURCE_DIR}/alib" ALIB_CPP )

