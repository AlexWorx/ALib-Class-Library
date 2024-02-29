// #################################################################################################
//  alib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALOX_ANSI_LOGGER)
       #include "alib/alox/loggers/ansilogger.hpp"
#   endif

#   if !defined(HPP_ALIB_STRINGS_UTIL_SPACES)
       #include "alib/strings/util/spaces.hpp"
#   endif

#   if !defined (HPP_ALIB_ALOXMODULE)
#      include "alib/alox/aloxmodule.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#      include "alib/strings/util/tokenizer.hpp"
#   endif
#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif

#   if !defined(HPP_ALIB_CAMP_MESSAGE_REPORT)
#      include "alib/lang/message/report.hpp"
#   endif
#endif // !defined(ALIB_DOX)

using namespace alib::lox::detail;

namespace alib {  namespace lox { namespace loggers {

// #################################################################################################
// Constructor/Destructor
// #################################################################################################
AnsiLogger::AnsiLogger( const NString&  name, const NString&  typeName  )
:    TextLogger( name, typeName, true )
{
    construct();
}

AnsiLogger::AnsiLogger( std::ostream* pOStream,
                        const NString&  name, const NString&  typeName  )
:    TextLogger( name, typeName, false )
{
    writer.SetStream( pOStream );

    construct();
}

void AnsiLogger::construct()
{
    // set msg suffix to "reset"
    FmtMsgSuffix=   ANSI_RESET;

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
        UseLightColors= LightColorUsage::Foreground;
    }

    // move verbosity information to the end to colorize the whole line
    ALIB_ASSERT_RESULT_NOT_EQUALS( MetaInfo->Format.SearchAndReplace( A_CHAR("]%V["), A_CHAR("][") ), 0)
    MetaInfo->Format._("%V");
    MetaInfo->VerbosityError           = ESC::RED;
    MetaInfo->VerbosityWarning         = ESC::BLUE;
    MetaInfo->VerbosityInfo            = A_CHAR("");
    MetaInfo->VerbosityVerbose         = ESC::GRAY;
}

AnsiLogger::~AnsiLogger()
{
}

// #################################################################################################
// logText
// #################################################################################################


void AnsiLogger::logText( detail::Domain&      ,    Verbosity         ,
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
            rest.ConsumeChars<false>( idx  + 1 );

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
            if( UseLightColors != LightColorUsage::Never && ( (UseLightColors == LightColorUsage::Foreground) == isForeGround ) )
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
            integer tabStop= AutoSizes.Next( AutoSizes::Types::Tabstop, column, extraSpace );
            integer qtySpaces= tabStop - column;
            if( qtySpaces > 0 )
            {
                Spaces::Write( *writer.GetStream(), qtySpaces );
                column+= qtySpaces;
            }

        }

        // Link (we just colorize links here)
        else if ( c == 'l' )
            writer.Write( rest.ConsumeChar() == 'S'
                           ? ( UseLightColors == LightColorUsage::Foreground
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
