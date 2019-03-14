// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALOX_LOGTOOLS)
    #include "alib/alox/logtools.hpp"
#endif

#if !defined (HPP_ALIB_STRINGFORMAT_FORMATTER)
    #include "alib/stringformat/formatter.hpp"
#endif

namespace aworx { namespace lib { namespace lox {


void LogTools::Exception( Lox&                      lox,
                          const aworx::Exception&   e,
                          Verbosity                 verbosity,
                          const NString&            domainPrefix,
                          const String&             logPrefix
                        )
{
    Tokenizer tknzr;
    tknzr.TrimChars= "\r";
    String256 buf;
    buf.DbgDisableBufferReplacementWarning();
    SPFormatter formatter= GetDefaultFormatter();
    formatter->Acquire(ALIB_CALLER_PRUNED);

    size_t entryNo= 1;
    lox.Acquire( ALIB_CALLER_NULLED );

        if( domainPrefix.IsNotNull() ) lox.SetDomain( domainPrefix, Scope::ThreadOuter );
        if( logPrefix   .IsNotNull() ) lox.SetPrefix( logPrefix   , Scope::ThreadOuter );
        for ( auto entry :  e )
        {
            lox.Acquire( entry.File, entry.Line, entry.Func );

                formatter->FormatArgs( buf.Reset(), entry.Args );

                tknzr.Set( buf, '\n' );
                bool firstLine= true;
                while( tknzr.HasNext() )
                {
                    auto& logables= lox.GetLogableContainer();
                    if( firstLine )
                        logables.Add( "{}{}: [{}]", (entry.Type.Value() >= 0 ? 'E' : 'I'), entryNo, entry.Type );
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

    formatter->Release();

}


}}}// namespace [aworx::lib::lox]
