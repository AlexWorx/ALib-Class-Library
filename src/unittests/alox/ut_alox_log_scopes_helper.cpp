// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_ALOX


#include "alib/alox.hpp"

using namespace std;
using namespace alib;


namespace ut_alox
{
// used with unit test Log_ScopeInfoCacheTest
void ScopeInfoCacheTest5();
void ScopeInfoCacheTest5() { Log_Info("Test Method 5") }

#if ALOX_DBG_LOG

    void Log_ScopeDomains_Helper();
    void Log_ScopeDomains_Helper()
    {
        Log_SetDomain( "HFILE",     Scope::Filename  )
        Log_SetDomain( "HMETHOD",   Scope::Method      )
        Log_Info("")
    }

#endif

#if ALOX_REL_LOG

    void Lox_ScopeDomains_Helper( Lox& lox );
    void Lox_ScopeDomains_Helper( Lox& lox )
    {
        #define LOX_LOX lox
        Lox_SetDomain( "HFILE",     Scope::Filename  )
        Lox_SetDomain( "HMETHOD",   Scope::Method      )
        Lox_Info("")
        #undef LOX_LOX
    }

#endif

} // namespace


#endif // ALIB_UT_ALOX
