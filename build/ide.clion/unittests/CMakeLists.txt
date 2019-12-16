# #################################################################################################
#  CMakeLists.txt - ALox Unit Tests
#
#  Copyright 2015-2019 A-Worx GmbH, Germany
#  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
#
#  This CMake file just includes the main CMake file found in build/cmake.cpp/unittests.
#  The whole purpose of this exercise is to have a dedicated project folder for
#  the Jetbrains CLion IDE, which gets filled by CLion with (hidden) configuration option files.
#
#  More information is found in ALox documentation.
# #################################################################################################

cmake_minimum_required( VERSION 3.3.0 )

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
    #list( APPEND CMAKE_CXX_FLAGS           "-std=c++11"    )
    #list( APPEND CMAKE_CXX_FLAGS           "-std=c++14"    )
    list(  APPEND CMAKE_CXX_FLAGS           "-std=c++17"    )

    # 32-bit compilation
    #list( APPEND CMAKE_CXX_FLAGS           "-m32"    )

    #### Module TEXT is mandatory to build the unit tests ####
    #list( APPEND ALIB_DISTRIBUTION         "TEXT"  )

    #### These "micro modules" are optional: may be commented out. #####
    #list( APPEND ALIB_DISTRIBUTION         "TIME"    )
    #list( APPEND ALIB_DISTRIBUTION         "THREADS"  )  # includes TIME

    #### The "higher level" modules are optional to be included in the unit tests ####
    #list( APPEND ALIB_DISTRIBUTION         "ALL"           )
    #list( APPEND ALIB_DISTRIBUTION         "SYSTEM"        )
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
    #set( ALIB_DEBUG_STRINGS                "On" )
    #set( ALIB_DEBUG_RESOURCES              "On" )
    set(  ALIB_DEBUG_BOXING                 "On" ) # has to be switched on to create the documentation



# -------------------------------------------------------------------------------------------------
# Including the generic cmake CMake file
# -------------------------------------------------------------------------------------------------
    #set( ALIB_CMAKE_VERBOSE  "On" ) # request details from the main ALib.cmake script

    include( ../../cmake/unittests/CMakeLists.txt )




