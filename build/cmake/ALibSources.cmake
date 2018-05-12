# #################################################################################################
#  ALibSources.cmake - CMake file for projects using ALib
#
#  Copyright 2015-2018 A-Worx GmbH, Germany
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
# #################################################################################################




if( NOT DEFINED  ALIB_SOURCE_DIR )
    set ( ALIB_SOURCE_DIR       ${ALIB_BASE_DIR}/src    )
endif()

# for speeding cmake up, "ALL" is captured with full globbing
if( modALL )
    file( GLOB_RECURSE ALIB_INCLUDE_FILES        ${ALIB_SOURCE_DIR}/alib/*.h*     )
    file( GLOB_RECURSE ALIB_SOURCE_FILES         ${ALIB_SOURCE_DIR}/alib/*.cpp    )
else()
    list( APPEND ALIB_INCLUDE_FILES  ${ALIB_SOURCE_DIR}/alib/alib.hpp             )

    if( modCORE )
        list( APPEND ALIB_INCLUDE_FILES  ${ALIB_SOURCE_DIR}/alib/alib.cpp         )

        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/debug/*.h*      )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/debug/*.cpp     )
        list( APPEND ALIB_SOURCE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/lang/*.h*       )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/lang/*.cpp      )
        list( APPEND ALIB_SOURCE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/strings/*.h*    )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/strings/*.cpp   )
        list( APPEND ALIB_SOURCE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/boxing/*.h*     )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/boxing/*.cpp    )
        list( APPEND ALIB_SOURCE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/compatibility/*.h*    )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/compatibility/*.cpp   )
        list( APPEND ALIB_SOURCE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/util/*.h*       )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/util/*.cpp      )
        list( APPEND ALIB_SOURCE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/time/*.h*       )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/time/*.cpp      )
        list( APPEND ALIB_SOURCE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/system/*.h*     )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/system/*.cpp    )
        list( APPEND ALIB_SOURCE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/threads/*.h*    )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/threads/*.cpp   )
        list( APPEND ALIB_SOURCE_FILES   ${tmp} )
        if ( ${APPLE} )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/system/directory.mm            )
        endif()


    else() # no core?
        if( modCONFIGURATION )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/threads/threadlocknr.hpp       )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/threads/threadlock.hpp         )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/threads/threadlock.cpp         )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/threads/threadlib.hpp          )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/threads/threadlib.cpp          )

            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/system/environment.hpp         )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/system/environment.cpp         )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/system/process.hpp             )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/system/process.cpp             )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/system/directory.hpp           )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/system/directory.cpp           )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/system/systemlib.hpp           )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/system/systemlib.cpp           )
            if ( ${APPLE} )
                list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/system/directory.mm        )
            endif()


            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/time/calendar.hpp              )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/time/calendar.cpp              )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/time/datetime.hpp              )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/time/ticks.hpp                 )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/time/ticksconverter.hpp        )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/time/timelib.hpp               )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/time/timelib.cpp               )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/time/timepointbase.hpp         )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/time/datetime.hpp              )


            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/util/plugincontainer.hpp       )

        endif()

        if( modSINGLETON )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/prepro.hpp                )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/rtti.hpp                  )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/lang/rtti.cpp                  )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/singleton.hpp             )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/lang/singleton.cpp             )
        endif()

        if( modBOXING OR modSTRINGS )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/alib.cpp                       )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/debug/assert.hpp               )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/debug/assert.cpp               )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/types.hpp                 )

            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/compatibility/std_iostream.hpp )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/compatibility/std_iostream.cpp )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/compatibility/std_string.hpp   )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/compatibility/std_vector.hpp   )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/compatibility/qt.hpp           )
        endif()

        if( modSTRINGS )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/enums.hpp                 )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/enumbitwise.hpp           )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/enumiterators.hpp         )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/enumarithmetical.hpp      )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/enummetadata.hpp          )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/lang/enummetadata.cpp          )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/langlib.hpp               )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/lang/langlib.cpp               )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/library.hpp               )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/lang/library.cpp               )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/resources.hpp             )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/lang/resources.cpp             )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/resourcedtupleloader.hpp  )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/util/memoryblocks.hpp          )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/util/memoryblocks.cpp          )

            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/applicables.hpp        )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/applicables.cpp        )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/astring.hpp            )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/astring.cpp            )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/numberformat.hpp       )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/numberformat.cpp       )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/preallocatedstring.hpp )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/resourcestring.hpp     )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/resourcestring.cpp     )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/cstring.hpp            )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/cstring.cpp            )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/string.hpp             )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/string.cpp             )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/stringlib.hpp          )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/stringlib.cpp          )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/stringliteral.hpp      )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/strings.hpp            )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/substring.hpp          )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/substring.cpp          )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/tstring.hpp            )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/tstring.cpp            )

            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/util/autosizes.hpp     )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/util/autosizes.cpp     )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/util/spaces.hpp        )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/util/spaces.cpp        )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/util/stringio.hpp      )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/util/stringmap.hpp     )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/util/stringtuples.hpp  )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/util/tokenizer.hpp     )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/util/tokenizer.cpp     )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/util/wildcardmatcher.hpp)
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/util/wildcardmatcher.cpp)
        endif()

        if( modBOXING )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/boxing/box.hpp                 )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/boxing/boxer.hpp               )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/boxing/boxes.hpp               )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/boxing/boxinglib.hpp           )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/boxing/boxinglib.cpp           )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/boxing/ftypes.hpp              )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/boxing/ftypes.cpp              )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/boxing/interface.hpp           )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/boxing/interface_builtin.hpp   )
        endif()

        if( modBOXING AND modSTRINGS )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/enum.hpp                  )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/enummetadataspec.hpp      )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/lang/exception.hpp             )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/lang/exception.cpp             )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/lang/owner.hpp                 )

            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/format/formatter.hpp           )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/format/formatter.cpp           )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/format/formatterjavastyle.hpp  )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/format/formatterjavastyle.cpp  )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/format/formatterpythonstyle.hpp)
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/format/formatterpythonstyle.cpp)
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/format/formatterstdimpl.hpp    )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/format/formatterstdimpl.cpp    )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/format/propertyformatter.hpp   )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/format/propertyformatter.cpp   )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/format/propertyformatters.hpp  )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/format/simpletext.hpp          )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/format/simpletext.cpp          )

            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/boxing/stringboxing.hpp        )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/boxing/stringboxing.cpp        )
            list( APPEND ALIB_INCLUDE_FILES ${ALIB_SOURCE_DIR}/alib/strings/boxing/debug.hpp               )
            list( APPEND ALIB_SOURCE_FILES  ${ALIB_SOURCE_DIR}/alib/strings/boxing/debug.cpp               )
        endif()
    endif()

    if( modCONFIGURATION )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/config/*.h*       )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/config/*.cpp      )
        list( APPEND ALIB_SOURCE_FILES    ${tmp} )
    endif()

    if( modALOX )
        list( APPEND ALIB_INCLUDE_FILES   ${ALIB_SOURCE_DIR}/alib/alox.hpp          )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/alox/*.h*         )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/alox/*.cpp        )
        list( APPEND ALIB_SOURCE_FILES    ${tmp} )
    endif()

    if( modCLI )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/cli/*.h*          )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/cli/*.cpp         )
        list( APPEND ALIB_SOURCE_FILES    ${tmp} )
    endif()

    if( modEXPRESSIONS )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/expressions/*.h*  )
        list( APPEND ALIB_INCLUDE_FILES   ${tmp} )
        file( GLOB_RECURSE tmp            ${ALIB_SOURCE_DIR}/alib/expressions/*.cpp )
        list( APPEND ALIB_SOURCE_FILES    ${tmp} )
    endif()

endif()




# Remove doubly defined sources (should never happen)
list(LENGTH ALIB_SOURCE_FILES  len1)
list(REMOVE_DUPLICATES ALIB_SOURCE_FILES)
list(LENGTH ALIB_SOURCE_FILES  len2)
if( NOT(len1 EQUAL len2))
    message(WARNING "ALib: Internal error in cmake script \"Doubly defined source file\". This should never happen.")
endif()
list(LENGTH ALIB_INCLUDE_FILES  len1)
list(REMOVE_DUPLICATES ALIB_INCLUDE_FILES)
list(LENGTH ALIB_INCLUDE_FILES  len2)
if( NOT(len1 EQUAL len2))
    message(WARNING "ALib: Internal error in cmake script \"Doubly defined header file\". This should never happen.")
endif()



# #################################################################################################
# Copy sources if requested
# #################################################################################################

if( ALIB_MODULE_SOURCE_COPY_DIR )

    if( NOT IS_DIRECTORY ${ALIB_MODULE_SOURCE_COPY_DIR})
        MESSAGE(FATAL_ERROR "Variable ALIB_MODULE_SOURCE_COPY_DIR=\"${ALIB_MODULE_SOURCE_COPY_DIR}\" is set but is not a valid directory. Stopping here!")
    endif()

    MESSAGE( STATUS "Copying source files of selected ALib modules to: ${ALIB_MODULE_SOURCE_COPY_DIR}")

    string(LENGTH ${ALIB_SOURCE_DIR} _srcDirStrlen)

    #### copy compilation units ####
    foreach( _origFilePathName ${ALIB_SOURCE_FILES})
        #MESSAGE("${_origFilePathName}")
        string(SUBSTRING ${_origFilePathName} ${_srcDirStrlen} -1 _FileRelativePathName)
        set(_newFilePathName "${ALIB_MODULE_SOURCE_COPY_DIR}${_FileRelativePathName}")
        get_filename_component(_newFilePath ${_newFilePathName} DIRECTORY )
        #MESSAGE("${_origFilePathName} ->${_newFilePath}")

        file( COPY ${_origFilePathName} DESTINATION ${_newFilePath} )
        list( APPEND _newlist   ${_newFilePathName} )
    endforeach()

    set( ALIB_SOURCE_FILES ${_newlist} )
    unset( _newlist )

    #### copy headers ####
    foreach( _origFilePathName ${ALIB_INCLUDE_FILES})
        #MESSAGE("${_origFilePathName}")
        string(SUBSTRING ${_origFilePathName} ${_srcDirStrlen} -1 _FileRelativePathName)
        set(_newFilePathName "${ALIB_MODULE_SOURCE_COPY_DIR}${_FileRelativePathName}")
        get_filename_component(_newFilePath ${_newFilePathName} DIRECTORY )
        #MESSAGE("${_origFilePathName} ->${_newFilePath}")

        file( COPY ${_origFilePathName} DESTINATION ${_newFilePath} )
        list( APPEND _newlist   ${_newFilePathName} )
    endforeach()

    set( ALIB_INCLUDE_FILES ${_newlist} )

    ### now also switch source base directories ###
    set( ALIB_SOURCE_DIR  ${ALIB_MODULE_SOURCE_COPY_DIR} )

endif()
