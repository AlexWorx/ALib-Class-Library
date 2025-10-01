// #################################################################################################
//  alib::lox::detail - ALox Logging Library
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
#include "alib/alox/alox.prepro.hpp"

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.ALox.Impl;
    import   ALib.Lang;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Variables;
    import   ALib.Camp;
    import   ALib.Camp.Base;
#else
#   include "ALib.Lang.H"
#   include "ALib.Characters.Functions.H"
#   include "ALib.Strings.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Variables.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"
#endif
// ======================================   Implementation   =======================================
using namespace alib;
using namespace alib::lox;
using namespace alib::lox::detail;
using namespace alib::lox::textlogger;

void PlainTextLogger::logText( Domain&          ,   Verbosity  ,
                               AString&      msg,
                               ScopeInfo&       ,   int           )
{
    if ( !notifyLogOp( lang::Phase::Begin ) )
        return;

    // loop over message, print the parts between the escape sequences
    integer         msgLength=  msg.Length();
    integer         start=      0;
    integer         end;
    integer         column=     0;
    while( start < msgLength )
    {
        bool foundESC=  true;
        end=    msg.String::IndexOf<NC>( '\x1B', start );
        if( end < 0 )
        {
            foundESC= false;
            end=      msgLength ;
        }

        if( end > start )
        {
            integer logWidth= logSubstring( msg,  start, end - start );
            if ( logWidth < 0 )
                return;
            column+= logWidth;
        }

        // interpret escape sequence
        if ( foundESC )
        {
            character c= msg[++end];

            // auto tab or end of meta info part
            if ( c == 't' || c == 'A')
            {
                ++end;
                c=  msg[end++];
                int extraSpace=  c >= '0' && c <= '9' ?   c - '0'
                                                      :   c - 'A'  + 10;

                integer tabStop= varFormatAutoSizes.Get<FormatAutoSizes>().Main.Next(
                                                    AutoSizes::Types::Tabstop, column, extraSpace );

                if ( tabStop > column )
                {
                    AlignedCharArray spaces( ' ' );
                    integer remaining= tabStop - column;
                    while ( remaining > 0 )
                    {
                        integer size= remaining < spaces.Length() ? remaining
                                                                  : spaces.Length();
                        if( logSubstring( String(spaces.Buffer(), spaces.Length()), 0, size ) < 0 )
                            return;
                        remaining-= size;
                    }
                    column= tabStop;
                }
            }

            // prune or ignore all others
            else
            {
                if ( !PruneESCSequences )
                    if( logSubstring( msg, end - 1, 3 ) < 0)
                        return;
                end+= 2;
            }
        }

        // next
        start= end;

    } // write loop

    ALIB_ASSERT_WARNING( start == msgLength, "ALOX", "Loop error when pruning ESC codes" )
    notifyLogOp( lang::Phase::End );
}

