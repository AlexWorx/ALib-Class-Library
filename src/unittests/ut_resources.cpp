// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_CAMP

#include "alib/lang/basecamp/basecamp.hpp"
#include "alib/lang/resources/localresourcepool.hpp"


using namespace alib;

#include <iostream>

#define TESTCLASSNAME       UT_Resources
#include "unittests/aworx_unittests.hpp"

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

    auto* localPool= dynamic_cast<lang::resources::LocalResourcePool*>( &BASECAMP.GetResourcePool() );
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



#include "unittests/aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_CAMP
