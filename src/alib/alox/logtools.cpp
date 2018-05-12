// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

#if !defined (HPP_ALIB_LANG_EXCEPTION)
    #include "alib/lang/exception.hpp"
#endif
#if !defined (HPP_ALOX_LOGTOOLS)
    #include "alib/alox/logtools.hpp"
#endif


namespace aworx { namespace lib { namespace lox {


void LogTools::Exception( Lox&                lox,
                          aworx::Exception&   e,
                          Verbosity           verbosity,
                          const NString&      domainPrefix,
                          const String&       logPrefix
                        )
{
    Tokenizer tknzr;
    tknzr.Whitespaces= "\r";
    String256 buf;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( buf,  ReplaceExternalBuffer );

    size_t entryNo= 1;
    lox.Acquire( ALIB_CALLER_NULLED );

        if( domainPrefix.IsNotNull() ) lox.SetDomain( domainPrefix, Scope::ThreadOuter );
        if( logPrefix   .IsNotNull() ) lox.SetPrefix( logPrefix   , Scope::ThreadOuter );
        for ( auto entry= e.Entries.begin(); entry < e.Entries.end(); entry++ )
        {
            lox.Acquire( entry->File, entry->Line, entry->Func );
                Exception::Describe( buf.Clear(), *entry );
                tknzr.Set( buf, '\n' );
                bool firstLine= true;
                while( tknzr.HasNext() )
                {
                    auto& logables= lox.GetLogableContainer();
                    if( firstLine )
                        logables.Add( "{}{}: [{}]", (entry->Code.Value() >= 0 ? 'E' : 'I'), entryNo, entry->Code );
                    else
                        logables.Add( "    {}"         , tknzr.Next()                    );

                    lox.Entry( "", verbosity );
                    firstLine= false;
                }
            lox.Release();
            entryNo++;
        }
        if( domainPrefix.IsNotNull() ) lox.SetDomain( nullptr, Scope::ThreadOuter );
        if( logPrefix   .IsNotNull() ) lox.SetPrefix( nullptr, Scope::ThreadOuter );
    lox.Release();

}


}}}// namespace [aworx::lib::lox]
