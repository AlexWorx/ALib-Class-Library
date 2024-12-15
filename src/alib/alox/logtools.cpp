// #################################################################################################
//  alib::lox - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/alox/logtools.hpp"
#   include "alib/lang/format/formatter.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#endif // !DOXYGEN


namespace alib {  namespace lox {

#include "alib/lang/callerinfo_functions.hpp"

void LogTools::Exception( Lox&                      lox,
                          const alib::Exception&    e,
                          Verbosity                 verbosity,
                          const NString&            domainPrefix,
                          const String&             logPrefix
                        )
{
    Tokenizer tknzr;
    tknzr.TrimChars= "\r";
    String1K buf;
    buf.DbgDisableBufferReplacementWarning();
    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    Formatter& formatter= *Formatter::Default;
    formatter.Reset();
    try
    {
        size_t entryNo= 1;
        lox.Acquire( ALIB_CALLER_NULLED );

            if( domainPrefix.IsNotNull() ) lox.SetDomain( domainPrefix, Scope::ThreadOuter );
            if( logPrefix   .IsNotNull() ) lox.SetPrefix( logPrefix   , Scope::ThreadOuter );
            for ( auto& entry :  e )
            {
                formatter.FormatArgs( buf.Reset(), entry );

                lox.Acquire( entry.CI );

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
    catch(alib::Exception& e)
    {
        lox.Error("Format exception caught while creating formatted output of another exception!\n"
                  "Format exception information follows:\n" );
        e.Format( buf.Reset() );
        lox.Error(buf);
    }
}
#include "alib/lang/callerinfo_methods.hpp"


}}// namespace [alib::lox]

