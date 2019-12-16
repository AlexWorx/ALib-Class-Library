// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_RESOURCES

#include "alib/lib/fs_modules/distribution.hpp"
#include "alib/resources/localresourcepool.hpp"


using namespace aworx;

#include <iostream>

#define TESTCLASSNAME       CPP_ALib_Resources
#include "unittests/aworx_unittests.hpp"

using namespace std;
using namespace aworx;

namespace ut_aworx {

UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- Get Resource Stats
//--------------------------------------------------------------------------------------------------
#if ALIB_DEBUG_RESOURCES
UT_METHOD( ResourceStats )
{
    UT_INIT()

    auto* localPool= dynamic_cast<lib::resources::LocalResourcePool*>( &ALIB.GetResourcePool() );
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

#endif // ALIB_UT_RESOURCES
