//==================================================================================================
/// \file
/// This header file is part of sub-namespace #alib::lang::system of module \alib_basecamp of
/// the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if   defined(__APPLE__)

#include "alib/lang/system/path.hpp"
#import <Foundation/Foundation.h>

using namespace std;

namespace alib::lang::system::macos {

void ALIB_APPLE_OC_NSTemporaryDirectory( Path& target)
{
    target.Reset();
    NSString* tempDir = NSTemporaryDirectory();
    if( tempDir )
        target <<  [tempDir UTF8String];
}

void ALIB_APPLE_OC_NSHomeDirectory( Path& target)
{
    target.Reset();
    NSString* tempDir = NSHomeDirectory();
    if( tempDir )
        target <<  [tempDir UTF8String];
}

} // namespace [alib::lang::system::macos]

#endif

