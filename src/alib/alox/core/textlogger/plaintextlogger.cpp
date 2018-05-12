// #################################################################################################
//  aworx::lib::lox::core - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"
#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
    #include "alib/strings/util/tokenizer.hpp"
#endif
#if !defined (HPP_ALIB_SYSTEM_DIRECTORY)
    #include "alib/system/directory.hpp"
#endif


#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

#include "alib/alox/core/textlogger/plaintextlogger.hpp"
#include "alib/strings/util/spaces.hpp"


using namespace std;
using namespace aworx;
using namespace aworx::lib::lox::core;
using namespace aworx::lib::lox::core::textlogger;

void PlainTextLogger::logText( Domain&          ,   Verbosity  ,
                               AString&      msg,
                               ScopeInfo&       ,   int           )
{
    if ( !notifyLogOp( Phase::Begin ) )
        return;

    // loop over message, print the parts between the escape sequences
    integer         msgLength=  msg.Length();
    integer         start=      0;
    integer         end;
    integer         column=     0;
    while( start < msgLength )
    {
        bool foundESC=  true;
        end=    msg.String::IndexOf<false>( '\x1B', start );
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
                end++;
                c=  msg[end++];
                int extraSpace=  c >= '0' && c <= '9' ?   c - '0'
                                                      :   c - 'A'  + 10;

                int tabStop= AutoSizes.Next( column, extraSpace );

                if ( tabStop > column )
                {
                    auto& spaces= Spaces::Get();
                    integer spacesLength= spaces.Length();
                    integer qty= tabStop - column;
                    while ( qty > 0 )
                    {
                        integer size= qty < spacesLength ? qty : spacesLength;
                        if( logSubstring( spaces, 0, size ) < 0 )
                            return;
                        qty-= size;
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

    ALIB_ASSERT_WARNING( start == msgLength, "Loop error when pruning ESC codes" );
    notifyLogOp( Phase::End );
}


