// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_SYSTEM_ENVIRONMENT)
#   include "alib/system/environment.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif


#if defined (__GLIBCXX__)  || defined(__APPLE__)
    #include <unistd.h>
#elif   defined( _WIN32 )
    #include <direct.h>
#else
    #pragma message ("Unknown Platform in file: " __FILE__ )
#endif

namespace aworx { namespace lib { namespace system {

bool GetEnvironmentVariable( const CString&  name,
                             AString&        target,
                             CurrentData     targetData,
                             Case            sensitivity )
{
    if ( targetData == CurrentData::Clear )
        target.Reset();

    if ( name.IsEmpty() )
        return false;


    #if defined (_WIN32)
        (void) sensitivity;
        ALIB_ASSERT_WARNING( sensitivity == Case::Ignore,
           "Windows OS does not support case sensitive environment variables" )

        #if ALIB_CHARACTERS_ARE_NARROW
            nchar *env;
            errno_t err = _dupenv_s( &env, nullptr, name );
            if ( err  || env == nullptr )
                return false;
            target._( NString( env ) );
        #else
            wchar_t *env;
            errno_t err = _wdupenv_s( &env, nullptr, name );
            if ( err  || env == nullptr )
                return false;
            target._( strings::TString<wchar_t>( env ) );
        #endif
        free( env );
        return true;

    #elif defined(__APPLE__)
        ALIB_ASSERT_WARNING( sensitivity == Case::Sensitive,
           "Mac OS does not support case insensitive environment variables" )
        (void) sensitivity;
        nchar* env= getenv( name );
        if( env != nullptr )
        {
            target._( env );
            return true;
        }
        return false;

    #elif defined (__unix__)

        ALIB_STRINGS_TO_NARROW( name, nName, 512 );

        // case sensitive is easy (but not supported under Windows)
        if ( sensitivity == Case::Sensitive )
        {
            nchar* env= getenv( nName );
            if( env != nullptr )
            {
                target._( NString( env ) );
                return true;
            }
            return false;
        }

        // read directly from the global 'environ' variable, to enable case insensitive access
        nchar** envv=   environ;
        while ( *envv )
        {
            const nchar* actVar=   *envv;
            if (     NString( actVar, nName.Length() ).Equals<Case::Ignore>( nName )
                 &&  *(*envv + nName.Length()) == '='
               )
            {
                actVar+= nName.Length() + 1;
                target._( actVar );
                return true;
            }

            envv++;
        }

        return false;
    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
}


}}}// namespace [aworx::lib::system]

