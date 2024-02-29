# #################################################################################################
#  CMakeLists.txt - ALox Unit Tests
#
#  Copyright 2015-2024 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  This CMake file just includes the main CMake file found in build/cmake.cpp/unittests.
#  The whole purpose of this exercise is to have a dedicated project folder for
#  the Jetbrains CLion IDE, which gets filled by CLion with (hidden) configuration option files.
#
#  More information is found in ALox documentation.
# #################################################################################################

cmake_minimum_required( VERSION 3.18.0 )

project("ALib_UnitTests")

# -------------------------------------------------------------------------------------------------
# Settings prior to including the generic cmake CMake file
# -------------------------------------------------------------------------------------------------

    # uncomment to force cotire on clean cmake built
    #set( ALIB_CMAKE_COTIRE                 "On" )
    #set( ALIB_PRECOMPILED_HEADER           "On" )

    # Allow valgrind runs.
    #set( ALIB_AVOID_ANALYZER_WARNINGS      "On" )

    # Uncomment to exclude huge loops, sleep times, thread races, etc
    #list( APPEND ALIB_SYMBOLS   "ALIB_UT_ROUGH_EXECUTION_SPEED_TEST"    )

    # Further compiler warnings
    #list( APPEND ALIB_COMPILER_WARNINGS    "-Wextra-semi" )

    # C++ Version selection

    #set(CMAKE_CXX_STANDARD 17)
    #set(CMAKE_CXX_STANDARD 20)
    #set(CMAKE_CXX_STANDARD 23)

    # 32-bit compilation
    #add_compile_options(-m32)

    #### Module CAMP is mandatory to build the unit tests ####
    #list( APPEND ALIB_DISTRIBUTION         "CAMP"  )

    #### These "micro modules" are optional: may be commented out. #####
    #list( APPEND ALIB_DISTRIBUTION         "TIME"    )
    #list( APPEND ALIB_DISTRIBUTION         "THREADS"  )  # includes TIME

    #### The "higher level" modules are optional to be included in the unit tests ####
    #list( APPEND ALIB_DISTRIBUTION         "ALL"           )
    #list( APPEND ALIB_DISTRIBUTION         "BITBUFFER"     )
    #list( APPEND ALIB_DISTRIBUTION         "CONFIGURATION" )
    #list( APPEND ALIB_DISTRIBUTION         "CLI"           )
    #list( APPEND ALIB_DISTRIBUTION         "EXPRESSIONS"   )
    #list( APPEND ALIB_DISTRIBUTION         "ALOX"          )


    #### ALib Features ####

    #set( ALIB_FEAT_SINGLETON_MAPPED        "On" )
    #set( ALIB_CHARACTERS_WIDE              "On" )
    #set( ALIB_CHARACTERS_SIZEOF_WCHAR      "2"  )

    # set( ALIB_FEAT_BOOST_REGEX            "On"  )

    #set( ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS     "On" )
    #set( ALIB_FEAT_BOXING_BIJECTIVE_FLOATS         "On" )
    #set( ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS      "On" )

    #set( ALIB_DEBUG_MONOMEM                "On" )
    #set( ALIB_DEBUG_ARRAY_COMPRESSION      "Off")
    #set( ALIB_DEBUG_STRINGS                "On" )
    #set( ALIB_DEBUG_RESOURCES              "On" )
    #set(  ALIB_DEBUG_BOXING                "On" ) # has to be switched on to create the documentation

# For UNKNOWN platforms: Try and set the integral sizes as follows.

if(MyPlatform)
    list( APPEND ALIB_SYMBOLS      "ALIB_SIZEOF_INTEGER=8"             )
    list( APPEND ALIB_SYMBOLS      "ALIB_SIZEOF_INTGAP=8"              )
    list( APPEND ALIB_SYMBOLS      "ALIB_INTGAP_TYPE=long"             )
    list( APPEND ALIB_SYMBOLS      "ALIB_SIZEOF_LONGDOUBLE_REPORTED=8" )
    list( APPEND ALIB_SYMBOLS      "ALIB_SIZEOF_LONGDOUBLE_WRITTEN=8"  )
endif ()




# -------------------------------------------------------------------------------------------------
# Including the generic cmake CMake file
# -------------------------------------------------------------------------------------------------
    #set( ALIB_CMAKE_VERBOSE  "On" ) # request details from the main ALib.cmake script

    include( ../../cmake/unittests/CMakeLists.txt )
