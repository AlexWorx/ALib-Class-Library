// #################################################################################################
//  alib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if defined( _WIN32 )
#include "alib/alox/loggers/windowsconsolelogger.hpp"

#if !DOXYGEN
#   include "alib/alox/aloxcamp.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#   include "alib/lang/message/report.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#endif // !DOXYGEN

#include <iostream>
#include <string.h>

using namespace alib::lox::textlogger;

namespace alib::lox::loggers {

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
}

void WindowsConsoleLogger::AcknowledgeLox( detail::LoxImpl* lox, lang::ContainerOp op )
{
    textlogger::TextLogger::AcknowledgeLox( lox, op );
    if( op != lang::ContainerOp::Insert )
        return;


    // evaluate environment variable "ALOX_CONSOLE_LIGHT_COLORS"
    // If default, we assume dark background, hence use light color on foreground
    ALIB_LOCK_WITH(ALOX.GetConfigLock())
    Variable useLightColors(ALOX, Variables::CONSOLE_LIGHT_COLORS );
    (void) useLightColors.Define();
    CFP= useLightColors.Get<ColorfulLoggerParameters>();

    if( CFP.LCU == ColorfulLoggerParameters::LightColorUsage::Auto )
    {
        // default: dark background, hence use light color on foreground
        CFP.LCU=   ( originalConsoleAttributes & ~W32C_FOREGROUND_MASK )  < 7
                ?  ColorfulLoggerParameters::LightColorUsage::Background
                :  ColorfulLoggerParameters::LightColorUsage::Foreground;
    }

    // move verbosity information to the end to colorize the whole line
    auto& fmt= varFormatMetaInfo.Get<FormatMetaInfo>();
    fmt.VerbosityError  .Reset(ESC::RED);
    fmt.VerbosityWarning.Reset(ESC::BLUE);
    fmt.VerbosityInfo   .Reset(A_CHAR(""));
    fmt.VerbosityVerbose.Reset(ESC::GRAY);

    // evaluate config variable CODEPAGE
    Variable codePage(ALOX, Variables::CODEPAGE );
    if( codePage.IsDefined() )
        CodePage= UINT(codePage.GetInt());
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
        character c= rest.ConsumeChar<NC>();

        // Colors
        bool isForeGround=  true;
        if( c == 'C' || c == 'c' )
        {
            isForeGround=  c== 'c';

            c= rest.ConsumeChar();
            int colNo= c - '0';
            ALIB_ASSERT_WARNING( colNo >=0 && colNo <=9, "ALOX", "ConsoleLogger: Unknown ESC-c code" )

            WORD attr= 0;
            WORD light=      CFP.LCU!= ColorfulLoggerParameters::LightColorUsage::Never
                        && ((CFP.LCU== ColorfulLoggerParameters::LightColorUsage::Foreground)
                              == isForeGround )
                        ? FOREGROUND_INTENSITY
                        : 0;

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
            integer tabStop= varFormatAutoSizes.Get<FormatAutoSizes>().Main.Next(
                                                    AutoSizes::Types::Tabstop, column, extraSpace );
            integer qtySpaces= tabStop - column;
            if( qtySpaces > 0 )
            {
                column+= qtySpaces;
                characters::AlignedCharArray<nchar, 8*sizeof(void*)> spaces( ' ' );
                while (qtySpaces >= spaces.Length() ) {
                    WriteConsoleA( H, spaces.Buffer(), (DWORD) spaces.Length(), &ignore, NULL );
                    qtySpaces-= spaces.Length();
                }
                if (qtySpaces > 0)
                    WriteConsoleA( H, spaces.Buffer(), (DWORD) qtySpaces, &ignore, NULL );
            }
        }


        // Link (we just colorize links here)
        else if ( c == 'l' )
        {
            if ( rest.ConsumeChar() == 'S' )
                actualAttributes=      ( actualAttributes & W32C_FOREGROUND_MASK )
                                    |  W32C_BLUE
                                    |  ( CFP.LCU == ColorfulLoggerParameters::LightColorUsage::Foreground
                                          ? FOREGROUND_INTENSITY
                                          : 0  );
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

}  // namespace [alib::lox::loggers]

#endif // Win32

