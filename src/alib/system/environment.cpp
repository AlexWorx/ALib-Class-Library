// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/strings/strings.prepro.hpp"
#include "alib/system/system.prepro.hpp"

#if !DOXYGEN
#   if defined (__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__) || defined(__ANDROID_NDK__)
#      include <unistd.h>
#   elif   defined( _WIN32 )
#      include <direct.h>
#   else
#      pragma message ("Unknown Platform in file: " __FILE__ )
#   endif
#endif // !DOXYGEN
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.System;
#  if ALIB_STRINGS
    import   ALib.Strings;
#  endif
#else
#   include "ALib.Strings.H"
#   include "ALib.System.H"
#endif
// ======================================   Implementation   =======================================
namespace alib {  namespace system {


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

}} // namespace [alib::system]
