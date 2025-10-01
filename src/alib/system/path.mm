//==================================================================================================
/// \file
/// This header-file is part of module \alib_system of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if   defined(__APPLE__)

#include "ALib.System.H"
#import <Foundation/Foundation.h>

using namespace std;

namespace alib::system::macos {

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

} // namespace [alib::system::macos]

#endif

