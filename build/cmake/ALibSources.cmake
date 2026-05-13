# ##################################################################################################
#  ALibSources.cmake - CMake file for projects using ALib
#
#  Copyright 2013-2026 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  Notes:
#    Selects source files in lists depending on the selected modules listed in variable
#    ALibBuild. The lists are:
#    -  ALIB_H  : C++ header file for external use with (both versions, C++20-Modules and legacy)
#    -  ALIB_MACROS: Header file with mostly preprocessor macros.
#    -  ALIB_MODIF: C++20-Module Interface Unit.
#    -  ALIB_HPP: Header-file included by .H and .IXX files.
#    -  ALIB_UNI: C++ source file compiled in legacy mode. Includes one or more CPP files.
#    -  ALIB_MOD: C++ source file compiled in C++20-Module mode. Includes one or more CPP files.
#    -  ALIB_CPP: C++ source files included by UNI and MOD files.
#    -  ALIB_MOD: C++20-Module implementation unit.
#    -  ALIB_RC:  ALib resource files.
#    If variable ALIB_SOURCE_COPY_TARGET_DIR is defined, then the sources are copied to the
#    directory specified.
#
#    This CMake file is included by "ALib.cmake" automatically and is not intended for manual
#    inclusion. Its contents was separated into a separated cmake file solely for clarity.
# ##################################################################################################
if( NOT DEFINED  ALIB_SOURCE_DIR )
    set ( ALIB_SOURCE_DIR       ${ALIB_BASE_DIR}/src    )
endif()

SET( ALIB_H       "" )
SET( ALIB_HPP     "" )
SET( ALIB_MODIF   "" )
SET( ALIB_UNI     "" )
SET( ALIB_MOD     "" )
SET( ALIB_CPP     "" )
SET( ALIB_MACROS  "" )
SET( ALIB_RC      "" )


# ---------- general files, always included -------

# alib
list( APPEND ALIB_H         ALib.Lang.CIFunctions.H        )
list( APPEND ALIB_H         ALib.Lang.CIMethods.H          )

list( APPEND ALIB_MACROS    alib.prepro.hpp                )
list( APPEND ALIB_MACROS    platform.prepro.hpp            )

list( APPEND ALIB_HPP       lang/assert.hpp                )
list( APPEND ALIB_UNI       lang/assert.uni.cpp            )
list( APPEND ALIB_MOD       lang/assert.mod.cpp            )
list( APPEND ALIB_CPP       lang/assert.cpp                )
list( APPEND ALIB_HPP       lang/mainargs.hpp              )
list( APPEND ALIB_CPP       lang/mainargs.cpp              )
list( APPEND ALIB_HPP       lang/chk_nc.hpp                )

#alib.Bootstrap
list( APPEND ALIB_H         ALib.Bootstrap.H               )
list( APPEND ALIB_MODIF     bootstrap/bootstrap.ixx        )
list( APPEND ALIB_HPP       bootstrap/bootstrap.hpp        )
list( APPEND ALIB_MACROS    bootstrap/bootstrap.prepro.hpp )
list( APPEND ALIB_UNI       bootstrap/bootstrap.uni.cpp    )
list( APPEND ALIB_MOD       bootstrap/bootstrap.mod.cpp    )
list( APPEND ALIB_CPP       bootstrap/bootstrap.cpp        )


# alib::lang
list( APPEND ALIB_H         ALib.Lang.H                    )
list( APPEND ALIB_MODIF     lang/lang.ixx                  )
list( APPEND ALIB_HPP       lang/allocation.hpp            )
list( APPEND ALIB_HPP       lang/bidilist.hpp              )
list( APPEND ALIB_HPP       lang/bits.hpp                  )
list( APPEND ALIB_HPP       lang/bitset.hpp                )
list( APPEND ALIB_HPP       lang/callerinfo.hpp            )
list( APPEND ALIB_HPP       lang/commonenumdefs.hpp        )
list( APPEND ALIB_HPP       lang/dbgcriticalsections.hpp   )
list( APPEND ALIB_HPP       lang/dbgtypedemangler.hpp      )
list( APPEND ALIB_CPP       lang/dbgcriticalsections.cpp   )
list( APPEND ALIB_CPP       lang/dbgtypedemangler.cpp      )
list( APPEND ALIB_UNI       lang/lang.uni.cpp              )
list( APPEND ALIB_MOD       lang/lang.mod.cpp              )
list( APPEND ALIB_HPP       lang/integers.hpp              )
list( APPEND ALIB_HPP       lang/owner.hpp                 )
list( APPEND ALIB_HPP       lang/placeholder.hpp           )
list( APPEND ALIB_HPP       lang/plugins.hpp               )
list( APPEND ALIB_HPP       lang/sidilist.hpp              )
list( APPEND ALIB_HPP       lang/stdtypeinfofunctors.hpp   )
list( APPEND ALIB_HPP       lang/tmp.hpp                   )

# alib::time
list( APPEND ALIB_H         ALib.Time.H                    )
list( APPEND ALIB_MOD       time/time.mod.cpp              )
list( APPEND ALIB_MODIF     time/time.ixx                  )
list( APPEND ALIB_HPP       time/datetime.hpp              )
list( APPEND ALIB_HPP       time/stopwatch.hpp             )
list( APPEND ALIB_HPP       time/tickconverter.hpp         )
list( APPEND ALIB_CPP       time/datetime.cpp              )
list( APPEND ALIB_CPP       time/tickconverter.cpp         )
list( APPEND ALIB_HPP       time/ticks.hpp                 )
list( APPEND ALIB_HPP       time/timepointbase.hpp         )
list( APPEND ALIB_CPP       time/time.cpp                  )

# alib::threads
list( APPEND ALIB_H         ALib.Threads.H                 )
list( APPEND ALIB_MODIF     threads/threads.ixx            )
list( APPEND ALIB_HPP       threads/thread.hpp             )
list( APPEND ALIB_HPP       threads/lock.hpp               )
list( APPEND ALIB_HPP       threads/recursivelock.hpp      )
list( APPEND ALIB_HPP       threads/recursivetimedlock.hpp )
list( APPEND ALIB_HPP       threads/sharedlock.hpp         )
list( APPEND ALIB_HPP       threads/sharedtimedlock.hpp    )
list( APPEND ALIB_HPP       threads/timedlock.hpp          )

if( NOT ALIB_SINGLE_THREADED )
  list( APPEND ALIB_MOD     threads/threads.mod.cpp        )
  list( APPEND ALIB_HPP     threads/dbgasserters.hpp       )
  list( APPEND ALIB_CPP     threads/dbgasserters.cpp       )
  list( APPEND ALIB_HPP     threads/condition.hpp          )
  list( APPEND ALIB_HPP     threads/promise.hpp            )
  list( APPEND ALIB_CPP     threads/promise.cpp            )

  list( APPEND ALIB_CPP     threads/thread.cpp             )
  list( APPEND ALIB_CPP     threads/locks.cpp              )
endif()

list( APPEND ALIB_HPP       characters/chartypes.hpp           )
list( APPEND ALIB_HPP       characters/chartraits.hpp          )
list( APPEND ALIB_HPP       characters/chartraits_stdtypes.hpp )
list( APPEND ALIB_HPP       characters/chartraits_qt.hpp       )

list( APPEND ALIB_H         ALib.Characters.Functions.H        )
list( APPEND ALIB_MOD       characters/functions.mod.cpp       )
list( APPEND ALIB_MODIF     characters/functions.ixx           )
list( APPEND ALIB_HPP       characters/functions.hpp           )
list( APPEND ALIB_CPP       characters/functions.cpp           )
list( APPEND ALIB_H         ALib.Compatibility.QTCharacters.H  )

list( APPEND ALIB_H         ALib.EnumOps.H                            )
list( APPEND ALIB_MODIF     enumops/enumops.ixx                       )
list( APPEND ALIB_MACROS    enumops/enumops.prepro.hpp                )
list( APPEND ALIB_HPP       enumops/underlyingintegral.hpp            )
list( APPEND ALIB_HPP       enumops/arithmetical.hpp                  )
list( APPEND ALIB_HPP       enumops/bitwise.hpp                       )
list( APPEND ALIB_HPP       enumops/iterable.hpp                      )
list( APPEND ALIB_HPP       enumops/iterablebitset.hpp                )
list( APPEND ALIB_HPP       enumops/bitwise_iterable_conversion.hpp   )


#################################           Normal Modules             #############################

if( "ALOX" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.ALox.H                               )
    list( APPEND ALIB_H        ALib.ALox.Impl.H                          )
    list( APPEND ALIB_MODIF    alox/alox_init.ixx                        )
    list( APPEND ALIB_HPP      alox/alox_init.hpp                        )
    list( APPEND ALIB_MODIF    alox/alox_impl.ixx                        )
    list( APPEND ALIB_MODIF    alox/alox.ixx                             )
    list( APPEND ALIB_MACROS   alox/alox.prepro.hpp                      )
    list( APPEND ALIB_HPP      alox/aloxcamp.hpp                         )
    list( APPEND ALIB_HPP      alox/log.hpp                              )
    list( APPEND ALIB_HPP      alox/logtools.hpp                         )
    list( APPEND ALIB_HPP      alox/lox.hpp                              )

    list( APPEND ALIB_MOD      alox/aloxcamp.mod.cpp                     )
    list( APPEND ALIB_CPP      alox/aloxcamp.cpp                         )
    list( APPEND ALIB_RC       alox/aloxcamp.alibrc                      )
    list( APPEND ALIB_MOD      alox/aloxinit.mod.cpp                     )
    list( APPEND ALIB_CPP      alox/aloxinit.cpp                         )
    list( APPEND ALIB_UNI      alox/alox.uni.cpp                         )
    list( APPEND ALIB_MOD      alox/alox.mod.cpp                         )
    list( APPEND ALIB_CPP      alox/alox.cpp                             )
    list( APPEND ALIB_MOD      alox/logtools.mod.cpp                     )
    list( APPEND ALIB_CPP      alox/logtools.cpp                         )
    list( APPEND ALIB_HPP      alox/detail/domain.hpp                    )
    list( APPEND ALIB_HPP      alox/detail/logger.hpp                    )
    list( APPEND ALIB_HPP      alox/detail/loxpimpl.hpp                  )
    list( APPEND ALIB_HPP      alox/detail/scopedump.hpp                 )
    list( APPEND ALIB_HPP      alox/detail/scopeinfo.hpp                 )
    list( APPEND ALIB_HPP      alox/detail/scopestore.hpp                )

    list( APPEND ALIB_MOD      alox/detail/domain.mod.cpp                )
    list( APPEND ALIB_CPP      alox/detail/domain.cpp                    )
    list( APPEND ALIB_UNI      alox/detail/loxpimpl.uni.cpp              )
    list( APPEND ALIB_MOD      alox/detail/loxpimpl.mod.cpp              )
    list( APPEND ALIB_CPP      alox/detail/loxpimpl.cpp                  )
    list( APPEND ALIB_MOD      alox/detail/scopedump.mod.cpp             )
    list( APPEND ALIB_CPP      alox/detail/scopedump.cpp                 )
    list( APPEND ALIB_UNI      alox/detail/scope_and_domain.uni.cpp      )
    list( APPEND ALIB_MOD      alox/detail/scopeinfo.mod.cpp             )
    list( APPEND ALIB_CPP      alox/detail/scopeinfo.cpp                 )
    list( APPEND ALIB_UNI      alox/detail/scopestore.uni.cpp            )
    list( APPEND ALIB_MOD      alox/detail/scopestore.mod.cpp            )
    list( APPEND ALIB_CPP      alox/detail/scopestore.cpp                )

    list( APPEND ALIB_HPP      alox/textlogger/plaintextlogger.hpp       )
    list( APPEND ALIB_HPP      alox/textlogger/textlogger.hpp            )
    list( APPEND ALIB_HPP      alox/textlogger/variables.hpp             )
    list( APPEND ALIB_MOD      alox/textlogger/plaintextlogger.mod.cpp   )
    list( APPEND ALIB_CPP      alox/textlogger/plaintextlogger.cpp       )
    list( APPEND ALIB_MOD      alox/textlogger/textlogger.mod.cpp        )
    list( APPEND ALIB_CPP      alox/textlogger/textlogger.cpp            )
    list( APPEND ALIB_MOD      alox/textlogger/variables.mod.cpp         )
    list( APPEND ALIB_CPP      alox/textlogger/variables.cpp             )

    list( APPEND ALIB_HPP      alox/loggers/ansilogger.hpp               )
    list( APPEND ALIB_HPP      alox/loggers/consolelogger.hpp            )
    list( APPEND ALIB_HPP      alox/loggers/memorylogger.hpp             )
    list( APPEND ALIB_HPP      alox/loggers/textfilelogger.hpp           )
    list( APPEND ALIB_HPP      alox/loggers/vstudiologger.hpp            )
    list( APPEND ALIB_HPP      alox/loggers/windowsconsolelogger.hpp     )
    list( APPEND ALIB_CPP      alox/loggers/ansilogger.cpp               )
    list( APPEND ALIB_CPP      alox/loggers/consolelogger.cpp            )
    list( APPEND ALIB_CPP      alox/loggers/textfilelogger.cpp           )
    list( APPEND ALIB_CPP      alox/loggers/vstudiologger.cpp            )
    list( APPEND ALIB_CPP      alox/loggers/windowsconsolelogger.cpp     )

    list( APPEND ALIB_MOD      alox/loggers/loggers.mod.cpp              )

endif()

if( "APP" IN_LIST ALibBuild )
    list( APPEND ALIB_H         ALib.App.H                               )
    list( APPEND ALIB_UNI       app/app.uni.cpp                          )
    list( APPEND ALIB_UNI       app/cli.uni.cpp                          )
    list( APPEND ALIB_MOD       app/app.mod.cpp                          )
    list( APPEND ALIB_MOD       app/cli.mod.cpp                          )
    list( APPEND ALIB_MOD       app/appcamp.mod.cpp                      )
    list( APPEND ALIB_MACROS    app/app.prepro.hpp                       )
    list( APPEND ALIB_MODIF     app/app.ixx                              )
    list( APPEND ALIB_HPP       app/app.hpp                              )
    list( APPEND ALIB_HPP       app/appcamp.hpp                          )
    list( APPEND ALIB_HPP       app/cli.hpp                              )
    list( APPEND ALIB_HPP       app/cliargtypes.hpp                      )
    list( APPEND ALIB_HPP       app/cliutil.hpp                          )
    list( APPEND ALIB_CPP       app/app.cpp                              )
    list( APPEND ALIB_CPP       app/appcamp.cpp                          )
    list( APPEND ALIB_RC        app/appcamp.alibrc                       )
    list( APPEND ALIB_CPP       app/cli.cpp                              )
    list( APPEND ALIB_CPP       app/cliargtypes.cpp                      )
    list( APPEND ALIB_CPP       app/cliutil.cpp                          )
endif()

if( "BITBUFFER" IN_LIST ALibBuild )
    list( APPEND ALIB_H         ALib.BitBuffer.H                         )
    list( APPEND ALIB_MODIF     bitbuffer/bitbuffer.ixx                  )
    list( APPEND ALIB_MACROS    bitbuffer/bitbuffer.prepro.hpp           )
    list( APPEND ALIB_HPP       bitbuffer/arraycompressor.hpp            )
    list( APPEND ALIB_HPP       bitbuffer/bitbuffer.hpp                  )
    list( APPEND ALIB_HPP       bitbuffer/ac_v1/acalgos.hpp.inl          )
    list( APPEND ALIB_HPP       bitbuffer/ac_v1/ac.hpp                   )
    list( APPEND ALIB_HPP       bitbuffer/ac_v1/huffman.hpp              )

    list( APPEND ALIB_UNI       bitbuffer/bitbuffer.uni.cpp              )
    list( APPEND ALIB_MOD       bitbuffer/bitbuffer.mod.cpp              )
    list( APPEND ALIB_CPP       bitbuffer/bitbuffer.cpp                  )
    list( APPEND ALIB_MOD       bitbuffer/ac_v1/ac.mod.cpp               )
    list( APPEND ALIB_CPP       bitbuffer/ac_v1/ac.cpp                   )
    list( APPEND ALIB_CPP       bitbuffer/ac_v1/huffman.cpp              )
endif()

if( "BOXING" IN_LIST ALibBuild )
    list( APPEND ALIB_H         ALib.Boxing.H                            )
    list( APPEND ALIB_MODIF     boxing/boxing.ixx                        )
    list( APPEND ALIB_MACROS    boxing/boxing.prepro.hpp                 )
    list( APPEND ALIB_HPP       boxing/box.hpp                           )
    list( APPEND ALIB_HPP       boxing/boxes.hpp                         )
    list( APPEND ALIB_HPP       boxing/boxstrap.hpp                      )
    list( APPEND ALIB_HPP       boxing/enum.hpp                          )
    list( APPEND ALIB_HPP       boxing/placeholder.hpp                   )
    list( APPEND ALIB_HPP       boxing/boxingtraits.hpp                  )
    list( APPEND ALIB_HPP       boxing/boxing_debug.hpp                  )
    list( APPEND ALIB_HPP       boxing/stdboxtraits.hpp                  )
    list( APPEND ALIB_MODIF     boxing/stdfunctors.ixx                   )
    list( APPEND ALIB_HPP       boxing/stdfunctors.hpp                   )
    list( APPEND ALIB_HPP       boxing/qtboxing.hpp                      )

    list( APPEND ALIB_HPP       boxing/boxingcustoms.hpp.inl             )
    list( APPEND ALIB_HPP       boxing/functiondecls.hpp                 )
    list( APPEND ALIB_HPP       boxing/functiondefs.hpp.inl              )
    list( APPEND ALIB_HPP       boxing/detail/vtable.hpp                 )

    list( APPEND ALIB_UNI       boxing/boxstrap.uni.cpp                  )
    list( APPEND ALIB_MOD       boxing/boxstrap.mod.cpp                  )
    list( APPEND ALIB_CPP       boxing/boxstrap.cpp                      )
    list( APPEND ALIB_MOD       boxing/boxes.mod.cpp                     )
    list( APPEND ALIB_CPP       boxing/boxes.cpp                         )
    list( APPEND ALIB_UNI       boxing/boxing.uni.cpp                    )
    list( APPEND ALIB_MOD       boxing/box.mod.cpp                       )
    list( APPEND ALIB_CPP       boxing/box.cpp                           )
    list( APPEND ALIB_MOD       boxing/detail/vtable.mod.cpp             )
    list( APPEND ALIB_CPP       boxing/detail/vtable.cpp                 )
    list( APPEND ALIB_CPP       boxing/detail/boxingcustoms.cpp          )
    list( APPEND ALIB_CPP       boxing/detail/dbgdoublelengthcheck.cpp   )
    list( APPEND ALIB_CPP       boxing/boxing_debug.cpp                  )

    list( APPEND ALIB_H         ALib.Boxing.StdFunctors.H                )
    list( APPEND ALIB_H         ALib.Compatibility.StdBoxtraits.H        )
    list( APPEND ALIB_H         ALib.Compatibility.QTBoxing.H            )

    if( "STRINGS" IN_LIST ALibBuild )
      list( APPEND ALIB_HPP     boxing/strings_appendables.hpp           )
      list( APPEND ALIB_HPP     boxing/extensions/strings_tfield.hpp     )
      list( APPEND ALIB_MOD     boxing/extensions/strings_tfield.mod.cpp )
      list( APPEND ALIB_CPP     boxing/extensions/strings_tfield.cpp     )

    endif()
endif()

if( "CAMP" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.Camp.H                      )
    list( APPEND ALIB_H        ALib.Camp.ResourceCompiler.H     )
    list( APPEND ALIB_MODIF    camp/camp.ixx                    )
    list( APPEND ALIB_MODIF    camp/resourcecompiler.ixx        )
    list( APPEND ALIB_MACROS   camp/camp.prepro.hpp             )
    list( APPEND ALIB_HPP      camp/basecamp.hpp                )
    list( APPEND ALIB_HPP      camp/camp.hpp                    )

    # additions from lower-level modules
    list( APPEND ALIB_HPP      camp/loadtokens.hpp              )
    list( APPEND ALIB_HPP      camp/resourcecompiler.hpp        )

    # Basecamp
    list( APPEND ALIB_H        ALib.Camp.Base.H                 )
    list( APPEND ALIB_MODIF    camp/basecamp.ixx                )
    list( APPEND ALIB_UNI      camp/basecamp.uni.cpp            )
    list( APPEND ALIB_MOD      camp/basecamp.mod.cpp            )
    list( APPEND ALIB_CPP      camp/basecamp.cpp                )
    list( APPEND ALIB_RC       camp/basecamp.alibrc             )
    list( APPEND ALIB_UNI      camp/resourcecompiler.uni.cpp    )
    list( APPEND ALIB_MOD      camp/resourcecompiler.mod.cpp    )
    list( APPEND ALIB_CPP      camp/resourcecompiler.cpp        )

endif()

if( "VARIABLES" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.Variables.H                    )
    list( APPEND ALIB_UNI      variables/configuration.uni.cpp     )
    list( APPEND ALIB_UNI      variables/variable.uni.cpp          )
    list( APPEND ALIB_UNI      variables/inifile.uni.cpp           )

    list( APPEND ALIB_MODIF    variables/variables.ixx             )
    list( APPEND ALIB_MACROS   variables/variables.prepro.hpp      )
    list( APPEND ALIB_HPP      variables/erpriority.hpp            )
    list( APPEND ALIB_HPP      variables/vmeta.hpp                 )
    list( APPEND ALIB_HPP      variables/declaration.hpp           )
    list( APPEND ALIB_HPP      variables/configuration.hpp         )
    list( APPEND ALIB_MOD      variables/configuration.mod.cpp     )
    list( APPEND ALIB_CPP      variables/configuration.cpp         )
    list( APPEND ALIB_MOD      variables/vmeta.mod.cpp             )
    list( APPEND ALIB_CPP      variables/vmeta.cpp                 )
    list( APPEND ALIB_MOD      variables/variable.mod.cpp          )
    list( APPEND ALIB_CPP      variables/variable.cpp              )

    list( APPEND ALIB_H        ALib.Variables.Plugins.H            )
    list( APPEND ALIB_MODIF    variables/plugins.ixx               )
    list( APPEND ALIB_MOD      variables/plugins.mod.cpp           )
    list( APPEND ALIB_CPP      variables/plugins.cpp               )
    list( APPEND ALIB_HPP      variables/plugins.hpp               )

    list( APPEND ALIB_H        ALib.Variables.IniFile.H            )
    list( APPEND ALIB_MODIF    variables/inifile.ixx               )
    list( APPEND ALIB_HPP      variables/inifile.hpp               )
    list( APPEND ALIB_HPP      variables/inifilefeeder.hpp         )
    list( APPEND ALIB_MOD      variables/inifile.mod.cpp           )
    list( APPEND ALIB_CPP      variables/inifile.cpp               )
    list( APPEND ALIB_MOD      variables/inifilefeeder.mod.cpp     )
    list( APPEND ALIB_CPP      variables/inifilefeeder.cpp         )

    if( "RESOURCES" IN_LIST ALibBuild )
      list( APPEND ALIB_H      ALib.Variables.ResourcePool.H       )
      list( APPEND ALIB_MODIF  variables/configresourcepool.ixx    )
      list( APPEND ALIB_UNI    variables/configresourcepool.uni.cpp)
      list( APPEND ALIB_MOD    variables/configresourcepool.mod.cpp)
      list( APPEND ALIB_CPP    variables/configresourcepool.cpp    )
      list( APPEND ALIB_HPP    variables/configresourcepool.hpp    )
    endif()
endif()

if( "CONTAINERS" IN_LIST ALibBuild )
    list( APPEND ALIB_MODIF    containers/containers_init.ixx            )
    list( APPEND ALIB_HPP      containers/recycling.hpp                  )
    list( APPEND ALIB_HPP      containers/valuedescriptor.hpp            )

    list( APPEND ALIB_H        ALib.Containers.FixedCapacityVector.H     )
    list( APPEND ALIB_MODIF    containers/fixedcapacityvector.ixx        )
    list( APPEND ALIB_HPP      containers/fixedcapacityvector.hpp        )

    list( APPEND ALIB_H        ALib.Containers.HashTable.H               )
    list( APPEND ALIB_MODIF    containers/hashtable.ixx                  )
    list( APPEND ALIB_HPP      containers/detail/hashtablebase.hpp       )
    list( APPEND ALIB_MOD      containers/detail/hashtablebase.mod.cpp   )
    list( APPEND ALIB_CPP      containers/detail/hashtablebase.cpp       )
    list( APPEND ALIB_HPP      containers/hashtable.hpp                  )
    list( APPEND ALIB_UNI      containers/detail/detail.uni.cpp          )

    list( APPEND ALIB_H        ALib.Containers.List.H                    )
    list( APPEND ALIB_MODIF    containers/list.ixx                       )
    list( APPEND ALIB_HPP      containers/list.hpp                       )

    list( APPEND ALIB_H        ALib.Containers.LRUCacheTable.H           )
    list( APPEND ALIB_MODIF    containers/lrucachetable.ixx              )
    list( APPEND ALIB_HPP      containers/lrucachetable.hpp              )

    list( APPEND ALIB_H        ALib.Containers.SharedPtr.H               )
    list( APPEND ALIB_MODIF    containers/sharedptr.ixx                  )
    list( APPEND ALIB_HPP      containers/sharedptr.hpp                  )

    list( APPEND ALIB_H        ALib.Containers.SharedVal.H               )
    list( APPEND ALIB_MODIF    containers/sharedval.ixx                  )
    list( APPEND ALIB_HPP      containers/sharedval.hpp                  )

    if( "STRINGS" IN_LIST ALibBuild )
      list( APPEND ALIB_H         ALib.Containers.StringTree.H            )
      list( APPEND ALIB_H         ALib.Containers.StringTreeIterator.H    )
      list( APPEND ALIB_MODIF     containers/stringtree.ixx               )
      list( APPEND ALIB_MODIF     containers/stringtreeiterator.ixx       )
      list( APPEND ALIB_HPP       containers/stringtree.hpp               )
      list( APPEND ALIB_HPP       containers/detail/stringtreebase.hpp    )
      list( APPEND ALIB_MOD       containers/detail/stringtreebase.mod.cpp)
      list( APPEND ALIB_CPP       containers/detail/stringtreebase.cpp    )
      list( APPEND ALIB_HPP       containers/stringtreeiterator.hpp       )
    endif()
endif()


if( "ENUMRECORDS" IN_LIST ALibBuild )
    list( APPEND ALIB_H         ALib.EnumRecords.H                            )
    list( APPEND ALIB_MODIF     enumrecords/enumrecords.ixx                   )
    list( APPEND ALIB_MACROS    enumrecords/enumrecords.prepro.hpp            )
    list( APPEND ALIB_HPP       enumrecords/detail/enumrecordmap.hpp          )
    list( APPEND ALIB_HPP       enumrecords/records.hpp                       )
    list( APPEND ALIB_HPP       enumrecords/serialization.hpp                 )
    list( APPEND ALIB_HPP       enumrecords/builtin.hpp                       )

    list( APPEND ALIB_H         ALib.EnumRecords.Bootstrap.H                  )
    list( APPEND ALIB_MODIF     enumrecords/bootstrap/recordbootstrap.ixx     )
    list( APPEND ALIB_HPP       enumrecords/bootstrap/recordparser.hpp        )
    list( APPEND ALIB_HPP       enumrecords/bootstrap/recordbootstrap.hpp     )
    list( APPEND ALIB_MOD       enumrecords/bootstrap/recordparser.mod.cpp    )
    list( APPEND ALIB_CPP       enumrecords/bootstrap/recordparser.cpp        )
    if( "BOXING" IN_LIST ALibBuild )
      list( APPEND ALIB_HPP     enumrecords/boxing_enums.hpp                  )
    endif()
    list( APPEND ALIB_UNI       enumrecords/enumrecords.uni.cpp               )
    list( APPEND ALIB_MOD       enumrecords/records.mod.cpp                   )
    list( APPEND ALIB_CPP       enumrecords/records.cpp                       )

endif()

if( "EXCEPTIONS" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.Exceptions.H                       )
    list( APPEND ALIB_MODIF    exceptions/exceptions.ixx               )
    list( APPEND ALIB_HPP      exceptions/message.hpp                  )
    list( APPEND ALIB_HPP      exceptions/exception.hpp                )
    list( APPEND ALIB_UNI      exceptions/exception.uni.cpp            )
    list( APPEND ALIB_MOD      exceptions/exception.mod.cpp            )
    list( APPEND ALIB_CPP      exceptions/exception.cpp                )
endif()

if( "EXPRESSIONS" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.Expressions.H                      )
    list( APPEND ALIB_H        ALib.Expressions.Impl.H                 )
    list( APPEND ALIB_UNI      expressions/expressions.uni.cpp         )
    list( APPEND ALIB_MOD      expressions/expression.mod.cpp          )
    list( APPEND ALIB_MODIF    expressions/expressions.ixx             )
    list( APPEND ALIB_MODIF    expressions/expressions_impl.ixx        )
    list( APPEND ALIB_HPP      expressions/detail/ast.hpp              )
    list( APPEND ALIB_HPP      expressions/parser.hpp                  )
    list( APPEND ALIB_MACROS   expressions/expressions.prepro.hpp      )
    list( APPEND ALIB_HPP      expressions/expressionscamp.hpp         )
    list( APPEND ALIB_HPP      expressions/compiler.hpp                )
    list( APPEND ALIB_HPP      expressions/compilerplugin.hpp          )
    list( APPEND ALIB_HPP      expressions/expression.hpp              )
    list( APPEND ALIB_HPP      expressions/scope.hpp                   )
    list( APPEND ALIB_HPP      expressions/standardrepository.hpp      )

    list( APPEND ALIB_HPP      expressions/detail/ast_impl.hpp         )
    list( APPEND ALIB_HPP      expressions/detail/parser_impl.hpp      )
    list( APPEND ALIB_HPP      expressions/detail/program.hpp          )
    list( APPEND ALIB_HPP      expressions/detail/virtualmachine.hpp   )

    list( APPEND ALIB_HPP      expressions/plugins/arithmetics.hpp     )
    list( APPEND ALIB_HPP      expressions/plugins/autocast.hpp        )
    list( APPEND ALIB_HPP      expressions/plugins/calculus.hpp        )
    list( APPEND ALIB_HPP      expressions/plugins/dateandtime.hpp     )
    list( APPEND ALIB_HPP      expressions/plugins/elvisoperator.hpp   )
    list( APPEND ALIB_HPP      expressions/plugins/math.hpp            )
    list( APPEND ALIB_HPP      expressions/plugins/strings.hpp         )

    list( APPEND ALIB_MOD      expressions/expressionscamp.mod.cpp     )
    list( APPEND ALIB_CPP      expressions/expressionscamp.cpp         )
    list( APPEND ALIB_RC       expressions/expressionscamp.alibrc      )
    list( APPEND ALIB_MOD      expressions/compiler.mod.cpp            )
    list( APPEND ALIB_CPP      expressions/compiler.cpp                )
    list( APPEND ALIB_CPP      expressions/expression.cpp              )
    list( APPEND ALIB_MOD      expressions/standardrepository.mod.cpp  )
    list( APPEND ALIB_CPP      expressions/standardrepository.cpp      )

    list( APPEND ALIB_MOD      expressions/parser.mod.cpp              )
    list( APPEND ALIB_CPP      expressions/parser.cpp                  )
    
    list( APPEND ALIB_UNI      expressions/detail/expr_detail.uni.cpp  )
    list( APPEND ALIB_MOD      expressions/detail/ast.mod.cpp          )
    list( APPEND ALIB_CPP      expressions/detail/ast.cpp              )
    list( APPEND ALIB_MOD      expressions/detail/parser_impl.mod.cpp  )
    list( APPEND ALIB_CPP      expressions/detail/parser_impl.cpp      )
    list( APPEND ALIB_MOD      expressions/detail/program.mod.cpp      )
    list( APPEND ALIB_CPP      expressions/detail/program.cpp          )
    list( APPEND ALIB_MOD      expressions/detail/virtualmachine.mod.cpp)
    list( APPEND ALIB_CPP      expressions/detail/virtualmachine.cpp   )

    list( APPEND ALIB_UNI      expressions/plugins/plugins.uni.cpp )
    list( APPEND ALIB_MOD      expressions/plugins/arithmetics.mod.cpp )
    list( APPEND ALIB_CPP      expressions/plugins/arithmetics.cpp     )
    list( APPEND ALIB_MOD      expressions/plugins/autocast.mod.cpp    )
    list( APPEND ALIB_CPP      expressions/plugins/autocast.cpp        )
    list( APPEND ALIB_MOD      expressions/plugins/calculus.mod.cpp    )
    list( APPEND ALIB_CPP      expressions/plugins/calculus.cpp        )
    list( APPEND ALIB_MOD      expressions/plugins/elvisoperator.mod.cpp)
    list( APPEND ALIB_CPP      expressions/plugins/elvisoperator.cpp   )
    list( APPEND ALIB_MOD      expressions/plugins/math.mod.cpp        )
    list( APPEND ALIB_CPP      expressions/plugins/math.cpp            )
    list( APPEND ALIB_MOD      expressions/plugins/strings.mod.cpp     )
    list( APPEND ALIB_CPP      expressions/plugins/strings.cpp         )
    list( APPEND ALIB_MOD      expressions/plugins/dateandtime.mod.cpp )
    list( APPEND ALIB_CPP      expressions/plugins/dateandtime.cpp     )

    list( APPEND ALIB_HPP      expressions/util/expressionformatter.hpp )
    list( APPEND ALIB_MOD      expressions/util/expressionformatter.mod.cpp)
    list( APPEND ALIB_CPP      expressions/util/expressionformatter.cpp )

 endif()

if( "FILETREE" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.FileTree.H                            )
    list( APPEND ALIB_UNI      filetree/filetree.uni.cpp                  )
    list( APPEND ALIB_MODIF    filetree/filetree.ixx                      )
    list( APPEND ALIB_MACROS   filetree/filetree.prepro.hpp               )
    list( APPEND ALIB_HPP      filetree/filetreecamp.hpp                  )
    list( APPEND ALIB_HPP      filetree/ffilter.hpp                       )
    list( APPEND ALIB_HPP      filetree/ftvalue.hpp                       )
    list( APPEND ALIB_HPP      filetree/fscanner.hpp                      )
    list( APPEND ALIB_HPP      filetree/ftree.hpp                         )

    list( APPEND ALIB_MOD      filetree/filetreecamp.mod.cpp              )
    list( APPEND ALIB_CPP      filetree/filetreecamp.cpp                  )
    list( APPEND ALIB_RC       filetree/filetreecamp.alibrc               )
    list( APPEND ALIB_MOD      filetree/ftfile.mod.cpp                    )
    list( APPEND ALIB_CPP      filetree/ftfile.cpp                        )
    list( APPEND ALIB_MOD      filetree/ftvalue.mod.cpp                   )
    list( APPEND ALIB_CPP      filetree/ftvalue.cpp                       )
    list( APPEND ALIB_UNI      filetree/fscanner.uni.cpp                  )
    list( APPEND ALIB_MOD      filetree/fscanner.mod.cpp                  )
    list( APPEND ALIB_CPP      filetree/fscanner.cpp                      )
    list( APPEND ALIB_MOD      filetree/ftree.mod.cpp                     )
    list( APPEND ALIB_CPP      filetree/ftree.cpp                         )

    if( "EXPRESSIONS" IN_LIST ALibBuild )
      list( APPEND ALIB_H       ALib.FileTree.Expressions.H               )
      list( APPEND ALIB_MODIF   filetree/expressions/fileexpressions.ixx  )
      list( APPEND ALIB_HPP     filetree/expressions/fileexpressions.hpp  )
      list( APPEND ALIB_UNI     filetree/expressions/fileexpressions.uni.cpp)
      list( APPEND ALIB_MOD     filetree/expressions/fileexpressions.mod.cpp)
      list( APPEND ALIB_CPP     filetree/expressions/fileexpressions.cpp  )
    endif()

endif()


if( "FORMAT" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.Format.H                               )
    list( APPEND ALIB_UNI      format/format.uni.cpp                       )
    list( APPEND ALIB_MOD      format/format.mod.cpp                       )
    list( APPEND ALIB_MODIF    format/format.ixx                           )
    list( APPEND ALIB_HPP      format/formatter.hpp                        )
    list( APPEND ALIB_HPP      format/fmtexceptions.hpp                    )
    list( APPEND ALIB_HPP      format/bytesize.hpp                         )
    list( APPEND ALIB_HPP      format/fmtcallerinfo.hpp                    )
    list( APPEND ALIB_HPP      format/fmtdatetime.hpp                      )
    list( APPEND ALIB_HPP      format/extensions/containers_hashtable.hpp  )
    list( APPEND ALIB_HPP      format/extensions/boxing_format_debug.hpp   )
    list( APPEND ALIB_HPP      format/extensions/resources_debug.hpp       )
    list( APPEND ALIB_CPP      format/formatter.cpp                        )
    list( APPEND ALIB_CPP      format/fmtvarious.cpp                       )
    list( APPEND ALIB_CPP      format/extensions/boxing_format_debug.cpp   )
    list( APPEND ALIB_CPP      format/extensions/resources_debug.cpp       )



    list( APPEND ALIB_H        ALib.Format.StdImplementation.H   )
    list( APPEND ALIB_MODIF    format/formatterstdimpl.ixx       )
    list( APPEND ALIB_HPP      format/formatterstdimpl.hpp       )
    list( APPEND ALIB_MOD      format/formatterstdimpl.mod.cpp   )
    list( APPEND ALIB_CPP      format/formatterstdimpl.cpp       )

    list( APPEND ALIB_H        ALib.Format.FormatterPythonStyle.H)
    list( APPEND ALIB_MODIF    format/formatterpythonstyle.ixx   )
    list( APPEND ALIB_HPP      format/formatterpythonstyle.hpp   )
    list( APPEND ALIB_MOD      format/formatterpythonstyle.mod.cpp)
    list( APPEND ALIB_CPP      format/formatterpythonstyle.cpp   )

    list( APPEND ALIB_H        ALib.Format.FormatterJavaStyle.H  )
    list( APPEND ALIB_MODIF    format/formatterjavastyle.ixx     )
    list( APPEND ALIB_HPP      format/formatterjavastyle.hpp     )
    list( APPEND ALIB_MOD      format/formatterjavastyle.mod.cpp )
    list( APPEND ALIB_CPP      format/formatterjavastyle.cpp     )

    list( APPEND ALIB_H        ALib.Format.Paragraphs.H          )
    list( APPEND ALIB_MODIF    format/paragraphs.ixx             )
    list( APPEND ALIB_HPP      format/paragraphs.hpp             )
    list( APPEND ALIB_MOD      format/paragraphs.mod.cpp         )
    list( APPEND ALIB_CPP      format/paragraphs.cpp             )

    list( APPEND ALIB_H        ALib.Format.PropertyFormatter.H   )
    list( APPEND ALIB_MODIF    format/propertyformatter.ixx      )
    list( APPEND ALIB_HPP      format/propertyformatter.hpp      )
    list( APPEND ALIB_MOD      format/propertyformatter.mod.cpp  )
    list( APPEND ALIB_CPP      format/propertyformatter.cpp      )

    if( "VARIABLES" IN_LIST ALibBuild )
      list( APPEND ALIB_H      ALib.Format.PropertyFormatters.H  )
      list( APPEND ALIB_MODIF  format/propertyformatters.ixx     )
      list( APPEND ALIB_HPP    format/propertyformatters.hpp     )
    endif()
endif()


if( "MONOMEM" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.Monomem.H                          )
    list( APPEND ALIB_MODIF    monomem/monomem.ixx                     )
    list( APPEND ALIB_MACROS   monomem/monomem.prepro.hpp              )
    list( APPEND ALIB_HPP      monomem/monoallocator.hpp               )
    list( APPEND ALIB_HPP      monomem/monoallocator.t.hpp             )
    list( APPEND ALIB_HPP      monomem/localallocator.hpp              )
    list( APPEND ALIB_HPP      monomem/poolallocator.hpp               )
    list( APPEND ALIB_HPP      monomem/poolallocator.t.hpp             )
    list( APPEND ALIB_MODIF    monomem/stdcontainers.ixx               )
    list( APPEND ALIB_HPP      monomem/stdcontainers.hpp               )
    list( APPEND ALIB_UNI      monomem/monomem.uni.cpp                 )
    list( APPEND ALIB_MOD      monomem/monomem.mod.cpp                 )
    list( APPEND ALIB_CPP      monomem/monomem.cpp                     )

    list( APPEND ALIB_H        ALib.Monomem.SharedMonoVal.H            )
    list( APPEND ALIB_MODIF    monomem/sharedmonoval.ixx               )
    list( APPEND ALIB_HPP      monomem/sharedmonoval.hpp               )

    list( APPEND ALIB_H        ALib.Monomem.StdContainers.H            )
endif()

if( "RESOURCES" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.Resources.H                             )
    list( APPEND ALIB_UNI      resources/resources.uni.cpp                  )
    list( APPEND ALIB_MODIF    resources/resources.ixx                      )
    list( APPEND ALIB_MACROS   resources/resources.prepro.hpp               )
    list( APPEND ALIB_HPP      resources/resources.hpp                      )
    list( APPEND ALIB_HPP      resources/detail/resourcemap.hpp             )
    list( APPEND ALIB_HPP      resources/localresourcepool.hpp              )

    list( APPEND ALIB_MOD      resources/localresourcepool.mod.cpp          )
    list( APPEND ALIB_CPP      resources/localresourcepool.cpp              )

    # additions from lower-level modules
    list( APPEND ALIB_HPP      resources/extensions/enums_serialization.hpp   )
    list( APPEND ALIB_HPP      resources/extensions/strings_loadtokens.hpp    )
    list( APPEND ALIB_MOD      resources/extensions/strings_loadtokens.mod.cpp)
    list( APPEND ALIB_CPP      resources/extensions/strings_loadtokens.cpp    )
endif()

if( "SINGLETONS" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.Singletons.H                       )
    list( APPEND ALIB_MODIF    singletons/singletons.ixx               )
    list( APPEND ALIB_MACROS   singletons/singletons.prepro.hpp        )
    list( APPEND ALIB_UNI      singletons/singletons.uni.cpp           )
    list( APPEND ALIB_MOD      singletons/singletons.mod.cpp           )
    list( APPEND ALIB_CPP      singletons/singletons.cpp               )
    list( APPEND ALIB_HPP      singletons/singletons.hpp               )
endif()

if( "STRINGS" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.Strings.H                          )
    list( APPEND ALIB_H        ALib.Strings.StdFormatter.H             )
    list( APPEND ALIB_UNI      strings/strings.uni.cpp                 )
    list( APPEND ALIB_MOD      strings/string.mod.cpp                  )
    list( APPEND ALIB_MOD      strings/astring.mod.cpp                 )

    list( APPEND ALIB_MODIF    strings/strings.ixx                     )
    list( APPEND ALIB_MACROS   strings/strings.prepro.hpp              )
    list( APPEND ALIB_HPP      strings/appendables.hpp                 )
    list( APPEND ALIB_HPP      strings/astring.hpp                     )
    list( APPEND ALIB_HPP      strings/cstring.hpp                     )
    list( APPEND ALIB_HPP      strings/format.hpp                      )
    list( APPEND ALIB_HPP      strings/numberformat.hpp                )
    list( APPEND ALIB_HPP      strings/localstring.hpp                 )
    list( APPEND ALIB_HPP      strings/string.hpp                      )
    list( APPEND ALIB_HPP      strings/stringnzt.hpp                   )
    list( APPEND ALIB_HPP      strings/substring.hpp                   )
    list( APPEND ALIB_MODIF    strings/stdfunctors.ixx                 )
    list( APPEND ALIB_HPP      strings/stdfunctors.hpp                 )
    list( APPEND ALIB_HPP      strings/stdformatter.hpp                )
    list( APPEND ALIB_HPP      strings/stdiostream.hpp                 )
    list( APPEND ALIB_HPP      strings/qtstrings.hpp                   )

    list( APPEND ALIB_HPP      strings/detail/tastring.hpp             )
    list( APPEND ALIB_HPP      strings/detail/tastringimpl.hpp         )
    list( APPEND ALIB_HPP      strings/detail/tastring_ca_decl.hpp.inl )
    list( APPEND ALIB_HPP      strings/detail/tastring_ca_impl.hpp.inl )
    list( APPEND ALIB_HPP      strings/detail/numberconversion.hpp     )

    list( APPEND ALIB_CPP      strings/astring.cpp                     )
    list( APPEND ALIB_CPP      strings/numberformat.cpp                )
    list( APPEND ALIB_CPP      strings/string.cpp                      )
    list( APPEND ALIB_CPP      strings/substring.cpp                   )
    list( APPEND ALIB_CPP      strings/detail/numberconversion.cpp     )

    list( APPEND ALIB_H        ALib.Compatibility.QTStrings.H          )
    list( APPEND ALIB_H        ALib.Strings.StdFunctors.H              )
    list( APPEND ALIB_H        ALib.Strings.StdIOStream.H              )
    list( APPEND ALIB_MODIF    strings/stdiostream.ixx                 )
    list( APPEND ALIB_MOD      strings/stdiostream.mod.cpp             )
    list( APPEND ALIB_CPP      strings/stdiostream.cpp                 )


    list( APPEND ALIB_H        ALib.Strings.Tokenizer.H                )
    list( APPEND ALIB_MODIF    strings/util/tokenizer.ixx              )
    list( APPEND ALIB_HPP      strings/util/tokenizer.hpp              )
    list( APPEND ALIB_MOD      strings/util/tokenizer.mod.cpp          )
    list( APPEND ALIB_CPP      strings/util/tokenizer.cpp              )
    list( APPEND ALIB_MODIF    strings/util/vector.ixx                 )
    list( APPEND ALIB_HPP      strings/util/vector.hpp                 )
    list( APPEND ALIB_UNI      strings/util/util.uni.cpp               )
    list( APPEND ALIB_CPP      strings/util/autosizes.cpp              )
    list( APPEND ALIB_CPP      strings/util/escaper.cpp                )
    list( APPEND ALIB_CPP      strings/util/regexmatcher.cpp           )
    list( APPEND ALIB_CPP      strings/util/wildcardmatcher.cpp        )

    list( APPEND ALIB_H        ALib.Strings.Escaper.H                  )
    list( APPEND ALIB_MODIF    strings/util/escaper.ixx                )
    list( APPEND ALIB_MOD      strings/util/escaper.mod.cpp           )
    list( APPEND ALIB_HPP      strings/util/escaper.hpp                )

    list( APPEND ALIB_H        ALib.Strings.Search.H                   )
    list( APPEND ALIB_MODIF    strings/util/search.ixx                 )
    list( APPEND ALIB_MOD      strings/util/search.mod.cpp             )
    list( APPEND ALIB_HPP      strings/util/search.hpp                 )
    list( APPEND ALIB_HPP      strings/util/regexmatcher.hpp           )
    list( APPEND ALIB_HPP      strings/util/wildcardmatcher.hpp        )
    list( APPEND ALIB_CPP      strings/util/search.cpp                 )

    list( APPEND ALIB_H        ALib.Strings.Calendar.H                 )
    list( APPEND ALIB_MODIF    strings/util/calendar.ixx               )
    list( APPEND ALIB_HPP      strings/util/calendar.hpp               )
    list( APPEND ALIB_MOD      strings/util/calendar.mod.cpp           )
    list( APPEND ALIB_CPP      strings/util/calendar.cpp               )

    list( APPEND ALIB_H        ALib.Strings.Token.H                    )
    list( APPEND ALIB_MODIF    strings/util/token.ixx                  )
    list( APPEND ALIB_HPP      strings/util/token.hpp                  )
    list( APPEND ALIB_MOD      strings/util/token.mod.cpp              )
    list( APPEND ALIB_CPP      strings/util/token.cpp                  )

    list( APPEND ALIB_H        ALib.Strings.AutoSizes.H                )
    list( APPEND ALIB_MODIF    strings/util/autosizes.ixx              )
    list( APPEND ALIB_MOD      strings/util/autosizes.mod.cpp          )
    list( APPEND ALIB_HPP      strings/util/autosizes.hpp              )

    list( APPEND ALIB_H        ALib.Strings.Vector.H                   )
    

  if( "MONOMEM" IN_LIST ALibBuild )
    list( APPEND ALIB_HPP      strings/monomem/strings_monomem.t.hpp   )
    list( APPEND ALIB_HPP      strings/monomem/strings_monomem.hpp     )
    list( APPEND ALIB_UNI      strings/monomem/strings_monomem.uni.cpp )
    list( APPEND ALIB_MOD      strings/monomem/strings_monomem.mod.cpp )
    list( APPEND ALIB_CPP      strings/monomem/strings_monomem.cpp     )
  endif()
endif()

if( "SYSTEM" IN_LIST ALibBuild )
    #### BASE/system ###
    list( APPEND ALIB_H        ALib.System.H                       )
    list( APPEND ALIB_H        ALib.System.TextFile.H              )
    list( APPEND ALIB_H        ALib.System.MappedFile.H            )

    list( APPEND ALIB_MODIF    system/system.ixx                   )
    list( APPEND ALIB_MODIF    system/mappedfile.ixx               )
    list( APPEND ALIB_MODIF    system/textfile.ixx                 )
    list( APPEND ALIB_MACROS   system/system.prepro.hpp            )
    list( APPEND ALIB_HPP      system/console.hpp                  )
    list( APPEND ALIB_HPP      system/environment.hpp              )
    list( APPEND ALIB_HPP      system/filestatus.hpp               )
    list( APPEND ALIB_HPP      system/mappedfile.hpp               )
    list( APPEND ALIB_HPP      system/path.hpp                     )
    list( APPEND ALIB_HPP      system/processinfo.hpp              )
    list( APPEND ALIB_HPP      system/shellcommand.hpp             )
    list( APPEND ALIB_HPP      system/textfile.hpp                 )
    list( APPEND ALIB_CPP      system/console.cpp                  )
    list( APPEND ALIB_CPP      system/environment.cpp              )
    list( APPEND ALIB_CPP      system/processinfo.cpp              )
    list( APPEND ALIB_CPP      system/shellcommand.cpp             )

    list( APPEND ALIB_UNI      system/system.uni.cpp               )
    list( APPEND ALIB_MOD      system/system.mod.cpp               )
    list( APPEND ALIB_MOD      system/filestatus.mod.cpp           )
    list( APPEND ALIB_CPP      system/filestatus.cpp               )
    list( APPEND ALIB_MOD      system/mappedfile.mod.cpp           )
    list( APPEND ALIB_CPP      system/mappedfile.cpp               )
    list( APPEND ALIB_MOD      system/path.mod.cpp                 )
    list( APPEND ALIB_CPP      system/path.cpp                     )
    if ( ${APPLE} )
      list( APPEND ALIB_UNI    system/path.mm                      )
    endif()
endif()

if( "THREADMODEL" IN_LIST ALibBuild )
    list( APPEND ALIB_H        ALib.ThreadModel.H                  )
    list( APPEND ALIB_UNI      threadmodel/threadmodel.uni.cpp     )
    list( APPEND ALIB_MODIF    threadmodel/threadmodel.ixx         )
    list( APPEND ALIB_HPP      threadmodel/dedicatedworker.hpp     )
    list( APPEND ALIB_HPP      threadmodel/jobs.hpp                )
    list( APPEND ALIB_HPP      threadmodel/threadpool.hpp          )
    list( APPEND ALIB_HPP      threadmodel/trigger.hpp             )

    list( APPEND ALIB_MOD      threadmodel/dedicatedworker.mod.cpp )
    list( APPEND ALIB_CPP      threadmodel/dedicatedworker.cpp     )
    list( APPEND ALIB_MOD      threadmodel/threadpool.mod.cpp      )
    list( APPEND ALIB_CPP      threadmodel/threadpool.cpp          )
    list( APPEND ALIB_MOD      threadmodel/trigger.mod.cpp         )
    list( APPEND ALIB_CPP      threadmodel/trigger.cpp             )
endif()

# ##################################################################################################
# Check for duplicates. This is a double-check to detect that this script needs a fix.
# ##################################################################################################

CollectAndRemoveDuplicates( "ALIB_MACROS"  "duplicates" )
LIST( LENGTH  duplicates  cntDuplicates )
IF( ${cntDuplicates} GREATER  0  )
    message( ERROR  "Internal script error ALIBSources.cmake: "
                     "DUPLICATE .prepro.hpp FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE HPP FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()

CollectAndRemoveDuplicates( "ALIB_HPP"  "duplicates" )
LIST( LENGTH  duplicates  cntDuplicates )
IF( ${cntDuplicates} GREATER  0  )
    message( ERROR  "Internal script error ALIBSources.cmake: "
                     "DUPLICATE .inl FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE INC FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()

CollectAndRemoveDuplicates( "ALIB_UNI"  "duplicates" )
LIST( LENGTH  duplicates  cntDuplicates )
IF( ${cntDuplicates} GREATER  0  )
    message( ERROR "Internal script error ALIBSources.cmake: "
                     "DUPLICATE .uni.cpp FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE CPP FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()
CollectAndRemoveDuplicates( "ALIB_MODIF"  "duplicates" )
LIST( LENGTH  duplicates  cntDuplicates )
IF( ${cntDuplicates} GREATER  0  )
    message( ERROR "Internal script error ALIBSources.cmake: "
                     "DUPLICATE .ixx FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE IXX FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()

CollectAndRemoveDuplicates( "ALIB_MOD"  "duplicates" )
LIST( LENGTH  duplicates  cntDuplicates )
IF( ${cntDuplicates} GREATER  0  )
    message( ERROR "Internal script error ALIBSources.cmake: "
                     "DUPLICATE .mod.cpp FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE MXX FILES:" )
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
                     "DUPLICATE .cpp FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE IMP FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()

CollectAndRemoveDuplicates( "ALIB_RC"  "duplicates" )
LIST( LENGTH  duplicates  cntDuplicates )
IF( ${cntDuplicates} GREATER  0  )
    message( ERROR "Internal script error ALIBSources.cmake: "
                     "DUPLICATE .alibrc FILES FOUND.\nSee list below! "
                     "(Processing continues with de-duplicated list)"          )
    message( STATUS  "LIST OF DUPLICATE IMP FILES:" )
    LIST(SORT duplicates)
    FOREACH( entry IN LISTS duplicates  )
        message( STATUS "    ${entry}" )
    ENDFOREACH()
    message( STATUS  "(end of list)" )
ENDIF()

# ##################################################################################################
# Check for lists containing the right file types
# ##################################################################################################
FOREACH( entry IN LISTS ALIB_H  )
    IF( NOT ${entry} MATCHES "([.]H$)"  )
        message( FATAL_ERROR  "File with wrong extension found in list ALIB_H: ${entry}" )
    ENDIF()
ENDFOREACH()

FOREACH( entry IN LISTS ALIB_HPP  )
    IF( NOT ${entry} MATCHES "([.]hpp$)|([.]inl$)"  )
        message( FATAL_ERROR  "File with wrong extension found in list ALIB_HPP: ${entry}" )
    ENDIF()
ENDFOREACH()

FOREACH( entry IN LISTS ALIB_MODIF  )
    IF( NOT ${entry} MATCHES "([.]ixx$)"  )
        message( FATAL_ERROR  "File with wrong extension found in list ALIB_MODIF: ${entry}" )
    ENDIF()
ENDFOREACH()

FOREACH( entry IN LISTS ALIB_UNI  )
    IF( NOT ${entry} MATCHES "([.]uni[.]cpp$)|([.]mm$)"  )
        message( FATAL_ERROR  "File with wrong extension found in list ALIB_UNI: ${entry}" )
    ENDIF()
ENDFOREACH()

FOREACH( entry IN LISTS ALIB_MOD  )
    IF( NOT ${entry} MATCHES "([.]mod[.]cpp$)"  )
        message( FATAL_ERROR  "File with wrong extension found in list ALIB_MOD: ${entry}" )
    ENDIF()
ENDFOREACH()

FOREACH( entry IN LISTS ALIB_CPP  )
    IF( NOT ${entry} MATCHES "([.]cpp$)"  )
        message( FATAL_ERROR  "File with wrong extension found in list ALIB_CPP: ${entry}" )
    ENDIF()
ENDFOREACH()

FOREACH( entry IN LISTS ALIB_MACROS  )
    IF( NOT ${entry} MATCHES "([.]prepro[.]hpp$)"  )
        message( FATAL_ERROR  "File with wrong extension found in list ALIB_MACROS: ${entry}" )
    ENDIF()
ENDFOREACH()

FOREACH( entry IN LISTS ALIB_RC  )
    IF( NOT ${entry} MATCHES "([.]alibrc$)"  )
        message( FATAL_ERROR  "File with wrong extension found in list ALIB_RC: ${entry}" )
    ENDIF()
ENDFOREACH()

# ##################################################################################################
# Copy sources if requested
# ##################################################################################################
if( ALIB_SOURCE_COPY_TARGET_DIR )

    # Control what to copy: by default copy only headers relevant to the current module selection.
    # Users can turn this OFF to copy all sources (including .uni.cppfiles) as before.
    option(ALIB_COPY_HEADERS_ONLY "When copying to temporary build folder, copy only headers (.H, .hpp, .hpp, .ixx) needed for the selected modules" ON)

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

    #### Create directories and copy public headers (ALib.*.H live at src root) ####
    foreach( fileName IN LISTS   ALIB_H  )
        SET( destPath  "${ALIB_SOURCE_COPY_TARGET_DIR}/ALib/${fileName}" )
        get_filename_component(destPath ${destPath} DIRECTORY )
        #MESSAGE("Copying file: ${ALIB_SOURCE_DIR}/ALib/${fileName}" )
        #MESSAGE("          to: ${destPath}" )
        file( MAKE_DIRECTORY "${destPath}")
        file( COPY   "${ALIB_SOURCE_DIR}/${fileName}"  DESTINATION  "${destPath}" )
    endforeach()

    # Build list of non-root headers to copy from src/alib tree
    set(_alib_copy_lists ${ALIB_MODIF} ${ALIB_HPP} ${ALIB_MACROS})

    # Optionally include .uni.cpp compilation units if headers-only copying is disabled
    if(NOT ALIB_COPY_HEADERS_ONLY)
        list(APPEND _alib_copy_lists ${ALIB_UNI} ${ALIB_MOD} ${ALIB_CPP} ${ALIB_RC})
    endif()

    foreach( fileName IN LISTS _alib_copy_lists )
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
    # Filter to only include files where the basename (before extension) has no dots
    set(relative_found_files "")
    foreach(file_path IN LISTS found_files)
        file(RELATIVE_PATH relative_file "${baseDir}" "${file_path}")

        # Get the filename without directory
        get_filename_component(filename "${relative_file}" NAME)

        # Extract everything before the final .extension
        # For "alox.mod.cpp" with extension "cpp", we want to check "alox.mod"
        # For "alox.cpp" with extension "cpp", we want to check "alox"
        string(REGEX REPLACE "\\.${extension}$" "" basename "${filename}")

        # Check if the basename contains a dot - if so, skip this file
        string(FIND "${basename}" "." dot_pos)
        if(${dot_pos} EQUAL -1)
            # No dot in basename, include this file
            list(APPEND relative_found_files "${relative_file}")
        endif()
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
    # Parse optional arguments
    set(options ALLOW_DOTS_IN_BASENAME)
    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    # Dereference the input variable names to get their values
    set(filenames ${${filenames_var}})
    set(extensions ${${extensions_var}})

    # Sort extensions by length (longest first) to match compound extensions first
    set(sorted_extensions ${extensions})
    list(SORT sorted_extensions COMPARE STRING ORDER DESCENDING)

    # Debug information
    #string(JOIN " " actual_filenames ${filenames})
    #string(JOIN " " allowed_extensions ${extensions})
    #message(STATUS "Validating files: ${actual_filenames}")
    #message(STATUS "Allowed extensions: ${allowed_extensions}")

    # Iterate over each filename
    foreach(filename IN LISTS filenames)
        # Get just the filename without directory
        get_filename_component(file_name_only "${filename}" NAME)

        # Try to match against each allowed extension (longest first)
        set(found_valid_extension FALSE)
        set(matched_extension "")

        foreach(ext IN LISTS sorted_extensions)
            # Check if filename ends with this extension
            string(REGEX MATCH "\\.${ext}$" match_result "${file_name_only}")
            if(match_result)
                set(found_valid_extension TRUE)
                set(matched_extension "${ext}")
                break()
            endif()
        endforeach()

        if(NOT found_valid_extension)
            # Extract what we think is the extension for error message
            string(REGEX MATCH "\\.[^.]*$" file_extension "${file_name_only}")
            string(REGEX REPLACE "\\." "" clean_extension "${file_extension}")
            message(FATAL_ERROR "File '${filename}' has an invalid extension: '${clean_extension}'. Allowed extensions are: ${extensions}")
        endif()

        # Additional check: ensure no dots in basename (before the extension) unless allowed
        if(NOT ARG_ALLOW_DOTS_IN_BASENAME)
            # Remove the matched extension to get the basename
            string(REGEX REPLACE "\\.${matched_extension}$" "" basename "${file_name_only}")

            # Check if basename contains a dot
            string(FIND "${basename}" "." dot_pos)
            if(NOT ${dot_pos} EQUAL -1)
                message(FATAL_ERROR "File '${filename}' has a dot in the basename before the extension. Only the extension dot is allowed.")
            endif()
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
    CheckFileInclusionForCompleteness( "${ALIB_H}"      "${ALIB_SOURCE_DIR}"      "H"   )
    CheckFileInclusionForCompleteness( "${ALIB_MODIF}"  "${ALIB_SOURCE_DIR}/alib" "ixx" )
    CheckFileInclusionForCompleteness( "${ALIB_HPP}"    "${ALIB_SOURCE_DIR}/alib" "hpp" )
    CheckFileInclusionForCompleteness( "${ALIB_UNI}"    "${ALIB_SOURCE_DIR}/alib" "uni.cpp" )
    if ( ${APPLE} )
      CheckFileInclusionForCompleteness( "${ALIB_UNI}"  "${ALIB_SOURCE_DIR}/alib" "mm" )
    endif()
    CheckFileInclusionForCompleteness( "${ALIB_MOD}"    "${ALIB_SOURCE_DIR}/alib" "mod.cpp" )
    CheckFileInclusionForCompleteness( "${ALIB_CPP}"    "${ALIB_SOURCE_DIR}/alib" "cpp" )
    CheckFileInclusionForCompleteness( "${ALIB_MACROS}" "${ALIB_SOURCE_DIR}/alib" "prepro.hpp" )
    CheckFileInclusionForCompleteness( "${ALIB_RC}"     "${ALIB_SOURCE_DIR}/alib" "alibrc" )
endif()
set(allowedExtensions "H" "hpp" )
                                     ValidateFileExtensions(ALIB_H   allowedExtensions ALLOW_DOTS_IN_BASENAME)
set(allowedExtensions "ixx" )
                                     ValidateFileExtensions(ALIB_MODIF allowedExtensions)
set(allowedExtensions "hpp" "hpp.inl" "t.hpp" )
                                     ValidateFileExtensions(ALIB_HPP allowedExtensions)
set(allowedExtensions "hpp" "prepro.hpp")
                                     ValidateFileExtensions(ALIB_MACROS allowedExtensions)
set(allowedExtensions "uni.cpp" "mm" )
                                     ValidateFileExtensions(ALIB_UNI allowedExtensions)
set(allowedExtensions "mod.cpp" )
                                     ValidateFileExtensions(ALIB_MOD allowedExtensions)
set(allowedExtensions "cpp" )
                                     ValidateFileExtensions(ALIB_CPP allowedExtensions)
set(allowedExtensions "alibrc" )
                                     ValidateFileExtensions(ALIB_RC allowedExtensions)
AddBaseDir("${ALIB_SOURCE_DIR}"      ALIB_H   )
AddBaseDir("${ALIB_SOURCE_DIR}/alib" ALIB_MODIF )
AddBaseDir("${ALIB_SOURCE_DIR}/alib" ALIB_HPP )
AddBaseDir("${ALIB_SOURCE_DIR}/alib" ALIB_MACROS )
AddBaseDir("${ALIB_SOURCE_DIR}/alib" ALIB_UNI )
AddBaseDir("${ALIB_SOURCE_DIR}/alib" ALIB_MOD )
AddBaseDir("${ALIB_SOURCE_DIR}/alib" ALIB_CPP )
AddBaseDir("${ALIB_SOURCE_DIR}/alib" ALIB_RC )
