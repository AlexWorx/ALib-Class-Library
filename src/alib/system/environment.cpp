// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_SYSTEM_ENVIRONMENT)
#   include "alib/system/environment.hpp"
#endif



#if defined (__GLIBCXX__)  || defined(__APPLE__)
    #include <unistd.h>
#elif   defined( _WIN32 )
    #include <direct.h>
#else
    #pragma message ("Unknown Platform in file: " __FILE__ )
#endif

namespace aworx { namespace lib { namespace system {

bool GetEnvironmentVariable( const TString&  name,
                             AString&        target,
                             CurrentData     targetData,
                             Case            sensitivity )
{
    if ( targetData == CurrentData::Clear )
        target.Clear();

    if ( name.IsEmpty() )
        return false;


    #if defined (_WIN32)
        (void) sensitivity;
        ALIB_ASSERT_WARNING( sensitivity == Case::Ignore,
           ASTR("Windows OS does not support case sensitive environment variables") );

        #if ALIB_NARROW_STRINGS
            nchar *env;
            errno_t err = _dupenv_s( &env, nullptr, name.ToCString() );
        #else
            wchar *env;
            errno_t err = _wdupenv_s( &env, nullptr, name.ToCString() );
        #endif
        if ( err  || env == nullptr )
            return false;
        target._( env );
        free( env );
        return true;

    #elif defined(__APPLE__)
        ALIB_ASSERT_WARNING( sensitivity == Case::Sensitive,
           ASTR("Mac OS does not support case insensitive environment variables") );
        (void) sensitivity;
        char* env= getenv( name.ToCString() );
        if( env != nullptr )
        {
            target._( env );
            return true;
        }
        return false;

    #elif defined (__unix__)

        ALIB_STD_TO_NARROW_TSTRING( name, nName );

        // case sensitive is easy (but not supported under Windows)
        if ( sensitivity == Case::Sensitive )
        {
            char* env= getenv( nName.ToCString() );
            if( env != nullptr )
            {
                target._( env );
                return true;
            }
            return false;
        }

        // read directly from the global 'environ' variable, to enable case insensitive access
        char** envv=   environ;
        while ( *envv )
        {
            const char* actVar=   *envv;
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

