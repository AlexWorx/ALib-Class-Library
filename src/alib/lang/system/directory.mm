// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  (c) 2013-2016 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#if   defined(__APPLE__)


#include "alib/lang/system/directory.hpp"

#import <Foundation/Foundation.h>

using namespace std;

namespace alib::lang::system::macos {

void ALIB_APPLE_OC_NSTemporaryDirectory( AString& target)
{
    target.Reset();
    NSString* tempDir = NSTemporaryDirectory();
    if( tempDir )
        target <<  [tempDir UTF8String];
}

void ALIB_APPLE_OC_NSHomeDirectory( AString& target)
{
    target.Reset();
    NSString* tempDir = NSHomeDirectory();
    if( tempDir )
        target <<  [tempDir UTF8String];
}

} // namespace [alib::lang::system::macos]

#endif
