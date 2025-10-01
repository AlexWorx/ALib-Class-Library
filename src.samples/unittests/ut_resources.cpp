// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_CAMP

#include "ALib.Camp.Base.H"
#include "ALib.Resources.H"


using namespace alib;

#include <iostream>

#define TESTCLASSNAME       UT_Resources
#include "aworx_unittests.hpp"

using namespace std;
using namespace alib;

namespace ut_aworx {

UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- Get Resource Stats
//--------------------------------------------------------------------------------------------------
#if ALIB_DEBUG_RESOURCES
UT_METHOD( ResourceStats )
{
    UT_INIT()

    auto* localPool= dynamic_cast<resources::LocalResourcePool*>( BASECAMP.GetResourcePool().Get() );
    UT_TRUE( localPool )

    auto categoryList= localPool->DbgGetCategories();
    integer sum= 0;
    for( auto& category : categoryList )
    {
        UT_PRINT( "Resource category {} has {} entries", category.first, category.second )
        sum+= category.second;
    }
    UT_PRINT( "This sums up to {} entries", sum )
}
#endif // ALIB_DEBUG_RESOURCES



#include "aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_CAMP
