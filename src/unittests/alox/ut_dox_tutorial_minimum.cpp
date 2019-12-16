// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_DOCS



void HelloALox();

//! [Tut_Minimum_Hello]
#include "alib/alox.hpp"

void HelloALox()
{
    Log_Info ( "Hello ALox!" )
}
//! [Tut_Minimum_Hello]


#endif // ALIB_UT_ALOX
