// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALOX_LOGTOOLS)
    #include "alib/alox/logtools.hpp"
#endif

#if !defined (HPP_ALIB_TEXT_FORMATTER)
    #include "alib/text/formatter.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#   include "alib/strings/util/tokenizer.hpp"
#endif
#endif // !defined(ALIB_DOX)

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
    String1K buf;
    buf.DbgDisableBufferReplacementWarning();
    SPFormatter formatter= Formatter::AcquireDefault(ALIB_CALLER_PRUNED);
    try
    {
        size_t entryNo= 1;
        lox.Acquire( ALIB_CALLER_NULLED );

            if( domainPrefix.IsNotNull() ) lox.SetDomain( domainPrefix, Scope::ThreadOuter );
            if( logPrefix   .IsNotNull() ) lox.SetPrefix( logPrefix   , Scope::ThreadOuter );
            for ( auto& entry :  e )
            {
                formatter->FormatArgs( buf.Reset(), entry );

                lox.Acquire( entry.File, entry.Line, entry.Function );

                    tknzr.Set( buf, '\n' );
                    bool firstLine= true;
                    while( tknzr.HasNext() )
                    {
                        auto& logables= lox.GetLogableContainer();
                        if( firstLine )
                            logables.Add( "{}{}: {!Q[]}", (entry.Type.Integral() >= 0 ? 'E' : 'I'),
                                                          entryNo, entry.Type );
                        else
                            logables.Add( "    {}"         , tknzr.Next()                    );

                        lox.Entry( "", verbosity );
                        firstLine= false;
                    }
                lox.Release();
                ++entryNo;
            }
            if( domainPrefix.IsNotNull() ) lox.SetDomain( nullptr, Scope::ThreadOuter );
            if( logPrefix   .IsNotNull() ) lox.SetPrefix( nullptr, Scope::ThreadOuter );
        lox.Release();
    }
    catch(aworx::Exception& e)
    {
        lox.Error("Format exception caught while creating formatted output of another exception!\n"
                  "Format exception information follows:\n" );
        e.Format( buf.Reset() );
        lox.Error(buf);
    }

    formatter->Release();
}


}}}// namespace [aworx::lib::lox]
