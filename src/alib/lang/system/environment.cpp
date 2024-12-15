// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/lang/system/environment.hpp"
#   include "alib/strings/localstring.hpp"
#   if defined (__GLIBCXX__)  || defined(__APPLE__) || defined(__ANDROID_NDK__)
#      include <unistd.h>
#   elif   defined( _WIN32 )
#      include <direct.h>
#   else
#      pragma message ("Unknown Platform in file: " __FILE__ )
#   endif
#endif // !DOXYGEN

namespace alib {  namespace lang::system {


bool EnvironmentVariables::Get( const CString&      name,
                                AString&            target,
                                lang::CurrentData   targetData )
{
    if ( targetData == lang::CurrentData::Clear )
        target.Reset();

    if ( name.IsEmpty() )
        return false;

    ALIB_STRINGS_TO_NARROW( name, nName, 512 )
    ALIB_WARNINGS_ALLOW_UNSAFE_FUNCTION_OR_VARIABLE
    nchar* env= std::getenv( nName );
    ALIB_WARNINGS_RESTORE

    if( env != nullptr )
    {
        target._( NString( env ) );
        return true;
    }
    return false;
}

}} // namespace [alib::lang::system]
