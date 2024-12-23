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


#include "alib/lang/callerinfo_functions.hpp"
namespace ut_alox
{

#if ALOX_DBG_LOG

    void Log_ScopeDomains_Helper2();
    void Log_ScopeDomains_Helper2()
    {
        Log_SetDomain( "H2FILE",     Scope::Filename  )
        Log_SetDomain( "H2METHOD",   Scope::Method      )
        Log_Info("")
    }

#endif

#if ALOX_REL_LOG

    void Lox_ScopeDomains_Helper2( Lox& lox );
    void Lox_ScopeDomains_Helper2( Lox& lox )
    {
        #define LOX_LOX lox
        Lox_SetDomain( "H2FILE",     Scope::Filename  )
        Lox_SetDomain( "H2METHOD",   Scope::Method      )
        Lox_Info("")
        #undef LOX_LOX
    }

#endif

} // namespace
#include "alib/lang/callerinfo_methods.hpp"


#endif // ALIB_UT_ALOX
