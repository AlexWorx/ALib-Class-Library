// #################################################################################################
//  alib::lox::loggers - ALox Logging Library
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
#include <ostream>

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.ALox.Impl;
    import   ALib.Lang;
    import   ALib.Strings;
    import   ALib.Strings.Tokenizer;
    import   ALib.Camp;
#else
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.Strings.Tokenizer.H"
#   include "ALib.Camp.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"
#endif
// ======================================   Implementation   =======================================
using namespace alib::lox::detail;

namespace alib {  namespace lox { namespace loggers {

void AnsiLogger::AcknowledgeLox( detail::LoxImpl* lox, lang::ContainerOp op )
{
    TextLogger::AcknowledgeLox( lox, op );
    if( op != lang::ContainerOp::Insert )
        return;

    auto& fmt= GetFormatMetaInfo();

    // set msg suffix to "reset"
    fmt.MsgSuffix.Reset(ANSI_RESET);

    // evaluate environment variable "ALOX_CONSOLE_LIGHT_COLORS"
    // If default, we assume dark background, hence use light color on foreground
    Variable useLightColors= variables::CampVariable(ALOX, Variables::CONSOLE_LIGHT_COLORS );
    (void) useLightColors.Define();
    CFP= useLightColors.Get<textlogger::ColorfulLoggerParameters>();
    if( CFP.LCU == textlogger::ColorfulLoggerParameters::LightColorUsage::Auto )
        CFP.LCU=   textlogger::ColorfulLoggerParameters::LightColorUsage::Foreground;

    // If the meta-information came from the defaults (resources) we parse different ones
    // dedicated to ANSI consoles.
    if( varFormatMetaInfo.GetPriority() == Priority::DefaultValues )
    {
        // This moves the verbosity information to the end to colorize the whole line
        auto ansiLoggerDefaultFormat= ALOX.GetResource("Var_D21A");
        varFormatMetaInfo.Import(ansiLoggerDefaultFormat, Priority::DefaultValues );
    }
}

AnsiLogger::~AnsiLogger()
{
}

// #################################################################################################
// logText
// #################################################################################################


void AnsiLogger::logText( detail::Domain&    ,    Verbosity         ,
                          AString&        msg,
                          ScopeInfo&         ,    int                )
{
    // loop over message, print the parts between the escape sequences
    integer column= 0;
    Tokenizer msgParts( msg, '\x1B' );
    Substring& actual= msgParts.Actual;
    Substring& rest=   msgParts.Rest;
    for(;;)
    {
        msgParts.Next( lang::Whitespaces::Keep );

        // check if end of actual msg part is an ANSI sequence. If yes, we extend the actual msg
        if ( rest.CharAtStart() == '[' )
        {
            // read the 'm'
            integer idx= rest.IndexOf( 'm' );
            if ( idx < 0 ) // unknown ANSI Code
            {
                ALIB_WARNING( "ALOX", "Unknown ANSI ESC Code \"{}...\"", rest.Substring(0, 10 ) )
                writer.Write( actual );
                continue;
            }

            column+= actual.WStringLength();

            actual= Substring( actual.Buffer(), actual.Length() + idx + 2 );
            rest.ConsumeChars<NC>( idx  + 1 );

            writer.Write( actual );

            continue;
        }

        if ( actual.IsNotEmpty() )
        {
            writer.Write( actual );
            column+= actual.WStringLength();
        }

        // end of loop?
        if ( !msgParts.HasNext() )
            break;

        // found an ESC sequence
        character c= rest.ConsumeChar();

        // Colors
        if( c == 'C' || c == 'c' )
        {
            bool isForeGround=  ( c== 'c' );

            c= rest.ConsumeChar();
            int colNo= c - '0';
            ALIB_ASSERT_WARNING( colNo >=0 && colNo <=9, "ALOX", "AnsiLogger: Unknown ESC-c code" )

            // add bg
            colNo+=  isForeGround ? 0 : 10;

            // add light
            if(        CFP.LCU != textlogger::ColorfulLoggerParameters::LightColorUsage::Never
                && ( ( CFP.LCU == textlogger::ColorfulLoggerParameters::LightColorUsage::Foreground ) == isForeGround ) )
                colNo+= 20;

            String ansiCol;
            switch( colNo )
            {
                case  0: ansiCol= AnsiLogger::ANSI_RED              ; break;
                case  1: ansiCol= AnsiLogger::ANSI_GREEN            ; break;
                case  2: ansiCol= AnsiLogger::ANSI_YELLOW           ; break;
                case  3: ansiCol= AnsiLogger::ANSI_BLUE             ; break;
                case  4: ansiCol= AnsiLogger::ANSI_MAGENTA          ; break;
                case  5: ansiCol= AnsiLogger::ANSI_CYAN             ; break;
                case  6: ansiCol= AnsiLogger::ANSI_BLACK            ; break;
                case  7: ansiCol= AnsiLogger::ANSI_WHITE            ; break;
                case  8: ansiCol= AnsiLogger::ANSI_GRAY             ; break;
                case  9: ansiCol= AnsiLogger::ANSI_STD_COL          ; break;

                case 10: ansiCol= AnsiLogger::ANSI_BG_RED           ; break;
                case 11: ansiCol= AnsiLogger::ANSI_BG_GREEN         ; break;
                case 12: ansiCol= AnsiLogger::ANSI_BG_YELLOW        ; break;
                case 13: ansiCol= AnsiLogger::ANSI_BG_BLUE          ; break;
                case 14: ansiCol= AnsiLogger::ANSI_BG_MAGENTA       ; break;
                case 15: ansiCol= AnsiLogger::ANSI_BG_CYAN          ; break;
                case 16: ansiCol= AnsiLogger::ANSI_BG_BLACK         ; break;
                case 17: ansiCol= AnsiLogger::ANSI_BG_WHITE         ; break;
                case 18: ansiCol= AnsiLogger::ANSI_BG_GRAY          ; break;
                case 19: ansiCol= AnsiLogger::ANSI_BG_STD_COL       ; break;

                case 20: ansiCol= AnsiLogger::ANSI_LIGHT_RED        ; break;
                case 21: ansiCol= AnsiLogger::ANSI_LIGHT_GREEN      ; break;
                case 22: ansiCol= AnsiLogger::ANSI_LIGHT_YELLOW     ; break;
                case 23: ansiCol= AnsiLogger::ANSI_LIGHT_BLUE       ; break;
                case 24: ansiCol= AnsiLogger::ANSI_LIGHT_MAGENTA    ; break;
                case 25: ansiCol= AnsiLogger::ANSI_LIGHT_CYAN       ; break;
                case 26: ansiCol= AnsiLogger::ANSI_BG_BLACK         ; break;
                case 27: ansiCol= AnsiLogger::ANSI_BG_WHITE         ; break;
                case 28: ansiCol= AnsiLogger::ANSI_LIGHT_GRAY       ; break;
                case 29: ansiCol= AnsiLogger::ANSI_STD_COL          ; break;

                case 30: ansiCol= AnsiLogger::ANSI_BG_LIGHT_RED     ; break;
                case 31: ansiCol= AnsiLogger::ANSI_BG_LIGHT_GREEN   ; break;
                case 32: ansiCol= AnsiLogger::ANSI_BG_LIGHT_YELLOW  ; break;
                case 33: ansiCol= AnsiLogger::ANSI_BG_LIGHT_BLUE    ; break;
                case 34: ansiCol= AnsiLogger::ANSI_BG_LIGHT_MAGENTA ; break;
                case 35: ansiCol= AnsiLogger::ANSI_BG_LIGHT_CYAN    ; break;
                case 36: ansiCol= AnsiLogger::ANSI_BG_BLACK         ; break;
                case 37: ansiCol= AnsiLogger::ANSI_BG_WHITE         ; break;
                case 38: ansiCol= AnsiLogger::ANSI_BG_LIGHT_GRAY    ; break;
                case 39: ansiCol= AnsiLogger::ANSI_BG_STD_COL       ; break;
                default: ALIB_ERROR( "ALOX", "Error in ANSI Code" )
                         ansiCol= A_CHAR("");
                         break;
            }

            writer.Write( ansiCol );
        }

        // Styles
        else if ( c == 's' )
        {
            c=  rest.ConsumeChar();
            writer.Write(const_cast<const character*>(   c== 'B' ? ANSI_BOLD
                                                       : c== 'I' ? ANSI_ITALICS
                                                       :           ANSI_RESET    ) );
        }

        // auto tab / end of meta
        else if ( c == 't' || c == 'A' )
        {
            c=  rest.ConsumeChar();
            int extraSpace=  c >= '0' && c <= '9' ? ( c - '0' )
                                                  : ( c - 'A' ) + 10;

            // tab stop (write spaces using a growing buffer)
            integer tabStop= varFormatAutoSizes.Get<textlogger::FormatAutoSizes>().Main.Next(
                                                    AutoSizes::Types::Tabstop, column, extraSpace );
            integer qtySpaces= tabStop - column;
            writer.WriteChars(' ', qtySpaces );
            column+= qtySpaces;
        }

        // Link (we just colorize links here)
        else if ( c == 'l' )
            writer.Write( rest.ConsumeChar() == 'S'
                           ? ( CFP.LCU == textlogger::ColorfulLoggerParameters::LightColorUsage::Foreground
                               ? static_cast<const String&>( ANSI_LIGHT_BLUE )
                               : static_cast<const String&>( ANSI_BLUE       ) )
                           :     static_cast<const String&>( ANSI_STD_COL    )   );

        else
        {
            rest.ConsumeChar();
            ALIB_WARNING( "ALOX", "Unknown ESC code {!Q'}.", c )
        }

    } // write loop

    *writer.GetStream() << std::endl;
}


// #################################################################################################
// AnsiConsoleLogger
// #################################################################################################
AnsiConsoleLogger::AnsiConsoleLogger( const NString&  name )
: AnsiLogger( name, "ANSI_CONSOLE" )
{
}

AnsiConsoleLogger::~AnsiConsoleLogger()
{
}

}}} // namespace [alib::lox::loggers]

