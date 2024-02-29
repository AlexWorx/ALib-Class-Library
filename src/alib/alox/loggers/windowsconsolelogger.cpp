// #################################################################################################
//  alib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if  !defined (HPP_ALOX_WINDOWS_CONSOLE_LOGGER)
#   include "alib/alox/loggers/windowsconsolelogger.hpp"
#endif
#endif // !defined(ALIB_DOX)


#if defined( _WIN32 )

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_ALOXMODULE)
#      include "alib/alox/aloxmodule.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#      include "alib/strings/util/tokenizer.hpp"
#   endif
#   if !defined(HPP_ALIB_STRINGS_UTIL_SPACES)
#      include "alib/strings/util/spaces.hpp"
#   endif

#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#   if !defined(HPP_ALIB_CAMP_MESSAGE_REPORT)
#      include "alib/lang/message/report.hpp"
#   endif
#   if !defined (_GLIBCXX_IOSTREAM) && !defined(_IOSTREAM_)
#      include <iostream>
#   endif
#   if !defined (_STRING_H) && !defined(_INC_STRING)
#      include <string.h>
#   endif
#endif // !defined(ALIB_DOX)


namespace alib {  namespace lox {

using namespace detail;

// #################################################################################################
// Windows Console Colors
// #################################################################################################
 #define W32C_FOREGROUND_MASK   0xFFF0
 #define W32C_BACKGROUND_MASK   0xFF0F

 #define W32C_BLACK          0
 #define W32C_RED            FOREGROUND_RED
 #define W32C_GREEN          FOREGROUND_GREEN
 #define W32C_BLUE           FOREGROUND_BLUE
 #define W32C_CYAN           FOREGROUND_GREEN | FOREGROUND_BLUE
 #define W32C_MAGENTA        FOREGROUND_RED   | FOREGROUND_BLUE
 #define W32C_YELLOW         FOREGROUND_RED   | FOREGROUND_GREEN
 #define W32C_WHITE          FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY

 #define W32C_GRAY_LIGHT     FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE
 #define W32C_GRAY_DARK      FOREGROUND_INTENSITY

 //! @cond NO_DOX
 WORD win32Cols[]=
 {
    W32C_RED,
    W32C_GREEN,
    W32C_YELLOW,
    W32C_BLUE,
    W32C_MAGENTA,
    W32C_CYAN,
 };
 //! @endcond

// #################################################################################################
// Constructor/Destructor
// #################################################################################################
WindowsConsoleLogger::WindowsConsoleLogger( const NString&  name )
:    TextLogger( name, "WINDOWS_CONSOLE",  true)
{
    // get actual console attributes
    WORD actualAttributes;
    {
        HANDLE H=    GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo( H, &info );
        actualAttributes= info.wAttributes;
        originalConsoleAttributes= actualAttributes;
    }

    // evaluate environment variable "ALOX_CONSOLE_LIGHT_COLORS"
    UseLightColors= LightColorUsage::Auto;
    Variable variable( Variables::CONSOLE_LIGHT_COLORS );
    if ( ALOX.GetConfig().Load( variable ) != Priorities::NONE && variable.Size() > 0)
    {
        Substring p= variable.GetString();
        if(p.Trim().IsNotEmpty())
        {
            if( !enums::Parse<LightColorUsage>( p, UseLightColors ) )
            {
                ALIB_WARNING( "ALOX", "Unknown value specified in variable: {} = {!Q'}.",
                              variable.Fullname(), variable.GetString() )
            }
        }
    }

    if( UseLightColors == LightColorUsage::Auto )
    {
        // default: dark background, hence use light color on foreground
        UseLightColors=   ( originalConsoleAttributes & ~W32C_FOREGROUND_MASK )  < 7 ?  LightColorUsage::Background
                                                                                     :  LightColorUsage::Foreground;
    }

    // move verbosity information to the end to colorize the whole line
    ALIB_ASSERT_RESULT_NOT_EQUALS( MetaInfo->Format.SearchAndReplace( A_CHAR("]%V["), A_CHAR("][")), 0);
    MetaInfo->Format._(A_CHAR("%V"));
    MetaInfo->VerbosityError           = ESC::RED;
    MetaInfo->VerbosityWarning         = ESC::BLUE;
    MetaInfo->VerbosityInfo            = A_CHAR("");
    MetaInfo->VerbosityVerbose         = ESC::GRAY;

    // evaluate config variable CODE_PAGE
    if ( ALOX.GetConfig().Load( variable.Declare( Variables::CODEPAGE ) ) > 0 )
        CodePage= (UINT) variable.GetInteger();
}

WindowsConsoleLogger::~WindowsConsoleLogger()
{
}

// #################################################################################################
// logText
// #################################################################################################
void WindowsConsoleLogger::logText( Domain&        ,    Verbosity  ,
                                    AString&    msg,
                                    ScopeInfo&     ,    int                   )
{
    HANDLE H=       GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD ignore;

    // get actual console attributes
    WORD actualAttributes;
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo( H, &info );
        actualAttributes= info.wAttributes;
    }
    WORD previousAttributes= actualAttributes;

    // set codepage;
    if ( CodePage != 0 )
        SetConsoleOutputCP( CodePage );


    // loop over message, print the parts between the escape sequences
    integer column= 0;
    Tokenizer msgParts( msg, '\x1B' );
    Substring& actual= msgParts.Actual;
    Substring& rest=   msgParts.Rest;
    for(;;)
    {
        if ( msgParts.Next(lang::Whitespaces::Keep ).IsNotEmpty() )
        {
            #if !ALIB_CHARACTERS_WIDE
                WriteConsoleA( H, actual.Buffer(), (DWORD) actual.Length(), &ignore, NULL );
            #else
                WriteConsoleW( H, actual.Buffer(), (DWORD) actual.Length(), &ignore, NULL );
            #endif
            column+= actual.WStringLength();
        }

        if ( !msgParts.HasNext() )
            break;

        // found a delimiter: process ESC sequence
        character c= rest.ConsumeChar<false>();

        // Colors
        bool isForeGround=  true;
        if( c == 'C' || c == 'c' )
        {
            isForeGround=  c== 'c';

            c= rest.ConsumeChar();
            int colNo= c - '0';
            ALIB_ASSERT_WARNING( colNo >=0 && colNo <=9, "ALOX", "ConsoleLogger: Unknown ESC-c code" )

            WORD attr= 0;
            WORD light=  UseLightColors != LightColorUsage::Never && ((UseLightColors== LightColorUsage::Foreground) == isForeGround )  ? FOREGROUND_INTENSITY : 0;

            // 0..5 (red, green, yellow, blue, magenta, cyan)
            if ( colNo >= 0 && colNo < 6)  attr= (win32Cols[colNo] | light);

            else if ( colNo == 6 )         attr= W32C_BLACK;
            else if ( colNo == 7 )         attr= W32C_WHITE;
            else if ( colNo == 8 )         attr=  light ? W32C_GRAY_LIGHT : W32C_GRAY_DARK;
            else if ( colNo == 9 )         attr= isForeGround  ?    originalConsoleAttributes & ~W32C_FOREGROUND_MASK
                                                               :  ( originalConsoleAttributes & ~W32C_BACKGROUND_MASK ) >> 4;
            else
            {
                ALIB_WARNING( "ALOX", "Unknown ESC- code \"C{}{}...\"", c, rest.Substring(0,10) );
            }

            actualAttributes=  isForeGround  ?  ( actualAttributes & W32C_FOREGROUND_MASK ) |   attr
                                             :  ( actualAttributes & W32C_BACKGROUND_MASK ) |  (attr << 4);
        }

        // Styles
        else if ( c == 's' )
        {
            // bold/italics style not supported in Windows console

            // reset all
            if ( rest.ConsumeChar() == 'a' )
                actualAttributes= originalConsoleAttributes;
        }

        // auto tab / end of meta
        else if ( c == 't'  || c == 'A' )
        {
            c=  rest.ConsumeChar();
            int extraSpace=  c >= '0' && c <= '9' ? (int) ( c - '0' )
                                                  : (int) ( c - 'A' ) + 10;

            // tab stop (write spaces using a growing buffer)
            integer tabStop= AutoSizes.Next( AutoSizes::Types::Tabstop, column, extraSpace );
            integer qtySpaces= tabStop - column;
            if( qtySpaces > 0 )
            {
                column+= qtySpaces;
                String& spaces= strings::util::Spaces::Get();
                while ( qtySpaces > 0 )
                {
                    integer nextQty= qtySpaces < spaces.Length() ? qtySpaces
                                                                 : spaces.Length();
                    #if !ALIB_CHARACTERS_WIDE
                        WriteConsoleA( H, spaces.Buffer(), (DWORD) nextQty, &ignore, NULL );
                    #else
                        WriteConsoleW( H, spaces.Buffer(), (DWORD) nextQty, &ignore, NULL );
                    #endif

                    qtySpaces-= nextQty;
                }
            }
        }


        // Link (we just colorize links here)
        else if ( c == 'l' )
        {
            if ( rest.ConsumeChar() == 'S' )
                actualAttributes=  ( actualAttributes & W32C_FOREGROUND_MASK ) |  W32C_BLUE | ( UseLightColors == LightColorUsage::Foreground ? FOREGROUND_INTENSITY : 0 );
            else
                actualAttributes=  ( actualAttributes & W32C_FOREGROUND_MASK ) |  ( originalConsoleAttributes & ~W32C_FOREGROUND_MASK );
        }


        else
        {
            ALIB_WARNING( "ALOX", "WindowsConsoleLogger: Unknown ESC code" );
        }

        // set the attributes
        ALIB_ASSERT_RESULT_NOT_EQUALS( SetConsoleTextAttribute( H, actualAttributes ), 0 );


    } // write loop

    ALIB_ASSERT_RESULT_NOT_EQUALS( SetConsoleTextAttribute( H, previousAttributes ), 0 );

    std::cout << std::endl;


}

}}  // namespace [alib::lox]

#endif // Win32
