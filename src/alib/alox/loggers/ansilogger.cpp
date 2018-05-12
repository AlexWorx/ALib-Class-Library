// #################################################################################################
//  aworx::lib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined(HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif
#if !defined(HPP_ALOX_CORE_TEXTLOGGER_TEXTLOGGER)
    #include "alib/alox/loggers/ansilogger.hpp"
#endif
#if !defined(HPP_ALIB_COMPATIBILITY_STD_IOSTREAM)
    #include "alib/compatibility/std_iostream.hpp"
#endif
#if !defined(HPP_ALIB_STRINGS_UTIL_SPACES)
    #include "alib/strings/util/spaces.hpp"
#endif

using namespace aworx::lib::lox::core;

namespace aworx { namespace lib { namespace lox { namespace loggers {
// #################################################################################################
// ANSI Escape Codes
// #################################################################################################
 #if defined(_MSC_VER)
// MSC  (as of 12/2015):
// C4579: in-class initialization for type 'const aworx::SLiteral<10>'
// is not yet implemented; static member will remain uninitialized at runtime but
// use in constant-expressions is supported
          SLiteral<5>   AnsiLogger::ANSI_RED              { ASTR("\033[31m")       }; ///< Select red as foreground color
          SLiteral<5>   AnsiLogger::ANSI_GREEN            { ASTR("\033[32m")       }; ///< Select green as foreground color
          SLiteral<5>   AnsiLogger::ANSI_YELLOW           { ASTR("\033[33m")       }; ///< Select yellow as foreground color
          SLiteral<5>   AnsiLogger::ANSI_BLUE             { ASTR("\033[34m")       }; ///< Select blue as foreground color
          SLiteral<5>   AnsiLogger::ANSI_MAGENTA          { ASTR("\033[35m")       }; ///< Select magenta as foreground color
          SLiteral<5>   AnsiLogger::ANSI_CYAN             { ASTR("\033[36m")       }; ///< Select cyan as foreground color
          SLiteral<5>   AnsiLogger::ANSI_BLACK            { ASTR("\033[30m")       }; ///< Select black as foreground color
          SLiteral<10>  AnsiLogger::ANSI_WHITE            { ASTR("\033[38;5;15m")  }; ///< Select white as foreground color
          SLiteral<11>  AnsiLogger::ANSI_GRAY             { ASTR("\033[38;5;240m") }; ///< Select gray as foreground color
          SLiteral<5>   AnsiLogger::ANSI_STD_COL          { ASTR("\033[39m")       }; ///< Select standard foreground color

          SLiteral<5>   AnsiLogger::ANSI_BG_RED           { ASTR("\033[41m")       }; ///< Select red as background color
          SLiteral<5>   AnsiLogger::ANSI_BG_GREEN         { ASTR("\033[42m")       }; ///< Select green as background color
          SLiteral<5>   AnsiLogger::ANSI_BG_YELLOW        { ASTR("\033[43m")       }; ///< Select yellow as background color
          SLiteral<5>   AnsiLogger::ANSI_BG_BLUE          { ASTR("\033[44m")       }; ///< Select blue as background color
          SLiteral<5>   AnsiLogger::ANSI_BG_MAGENTA       { ASTR("\033[45m")       }; ///< Select magenta as background color
          SLiteral<5>   AnsiLogger::ANSI_BG_CYAN          { ASTR("\033[46m")       }; ///< Select cyan as background color
          SLiteral<5>   AnsiLogger::ANSI_BG_BLACK         { ASTR("\033[40m")       }; ///< Select black as background color
          SLiteral<10>  AnsiLogger::ANSI_BG_WHITE         { ASTR("\033[48;5;15m")  }; ///< Select white as background color
          SLiteral<11>  AnsiLogger::ANSI_BG_GRAY          { ASTR("\033[48;5;240m") }; ///< Select gray as background color
          SLiteral<5>   AnsiLogger::ANSI_BG_STD_COL       { ASTR("\033[49m")       }; ///< Select standard background color

          SLiteral<10>  AnsiLogger::ANSI_LIGHT_RED        { ASTR("\033[38;5;09m")  }; ///< Select light red as foreground color
          SLiteral<10>  AnsiLogger::ANSI_LIGHT_GREEN      { ASTR("\033[38;5;10m")  }; ///< Select light green as foreground color
          SLiteral<10>  AnsiLogger::ANSI_LIGHT_YELLOW     { ASTR("\033[38;5;11m")  }; ///< Select light yellow as foreground color
          SLiteral<10>  AnsiLogger::ANSI_LIGHT_BLUE       { ASTR("\033[38;5;12m")  }; ///< Select light blue as foreground color
          SLiteral<10>  AnsiLogger::ANSI_LIGHT_MAGENTA    { ASTR("\033[38;5;13m")  }; ///< Select light magenta as foreground color
          SLiteral<10>  AnsiLogger::ANSI_LIGHT_CYAN       { ASTR("\033[38;5;14m")  }; ///< Select light cyan as foreground color
          SLiteral<11>  AnsiLogger::ANSI_LIGHT_GRAY       { ASTR("\033[38;5;250m") }; ///< Select light gray as foreground color
          SLiteral<5>   AnsiLogger::ANSI_LIGHT_STD_COL    { ASTR("\033[39m")       }; ///< Select standard foreground color

          SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_RED     { ASTR("\033[48;5;09m")  }; ///< Select light red as background color
          SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_GREEN   { ASTR("\033[48;5;10m")  }; ///< Select light green as background color
          SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_YELLOW  { ASTR("\033[48;5;11m")  }; ///< Select light yellow as background color
          SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_BLUE    { ASTR("\033[48;5;12m")  }; ///< Select light blue as background color
          SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_MAGENTA { ASTR("\033[48;5;13m")  }; ///< Select light magenta as background color
          SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_CYAN    { ASTR("\033[48;5;14m")  }; ///< Select light cyan as background color
          SLiteral<11>  AnsiLogger::ANSI_BG_LIGHT_GRAY    { ASTR("\033[48;5;250m") }; ///< Select light gray as background color
          SLiteral<5>   AnsiLogger::ANSI_BG_LIGHT_STD_COL { ASTR("\033[49m")       }; ///< Select standard background color

          SLiteral<4>   AnsiLogger::ANSI_BOLD             { ASTR("\033[1m")        }; ///< Select bold font style
          SLiteral<4>   AnsiLogger::ANSI_ITALICS          { ASTR("\033[3m")        }; ///< Select italics font style
          SLiteral<4>   AnsiLogger::ANSI_STD_STYLE        { ASTR("\033[0m")        }; ///< Select standard font style
          SLiteral<4>   AnsiLogger::ANSI_RESET            { ASTR("\033[0m")        }; ///< Reset colors and font style

#else
constexpr SLiteral<5>   AnsiLogger::ANSI_RED             ;
constexpr SLiteral<5>   AnsiLogger::ANSI_GREEN           ;
constexpr SLiteral<5>   AnsiLogger::ANSI_YELLOW          ;
constexpr SLiteral<5>   AnsiLogger::ANSI_BLUE            ;
constexpr SLiteral<5>   AnsiLogger::ANSI_MAGENTA         ;
constexpr SLiteral<5>   AnsiLogger::ANSI_CYAN            ;
constexpr SLiteral<5>   AnsiLogger::ANSI_BLACK           ;
constexpr SLiteral<10>  AnsiLogger::ANSI_WHITE           ;
constexpr SLiteral<11>  AnsiLogger::ANSI_GRAY            ;
constexpr SLiteral<5>   AnsiLogger::ANSI_STD_COL         ;

constexpr SLiteral<5>   AnsiLogger::ANSI_BG_RED          ;
constexpr SLiteral<5>   AnsiLogger::ANSI_BG_GREEN        ;
constexpr SLiteral<5>   AnsiLogger::ANSI_BG_YELLOW       ;
constexpr SLiteral<5>   AnsiLogger::ANSI_BG_BLUE         ;
constexpr SLiteral<5>   AnsiLogger::ANSI_BG_MAGENTA      ;
constexpr SLiteral<5>   AnsiLogger::ANSI_BG_CYAN         ;
constexpr SLiteral<5>   AnsiLogger::ANSI_BG_BLACK        ;
constexpr SLiteral<10>  AnsiLogger::ANSI_BG_WHITE        ;
constexpr SLiteral<11>  AnsiLogger::ANSI_BG_GRAY         ;
constexpr SLiteral<5>   AnsiLogger::ANSI_BG_STD_COL      ;

constexpr SLiteral<10>  AnsiLogger::ANSI_LIGHT_RED       ;
constexpr SLiteral<10>  AnsiLogger::ANSI_LIGHT_GREEN     ;
constexpr SLiteral<10>  AnsiLogger::ANSI_LIGHT_YELLOW    ;
constexpr SLiteral<10>  AnsiLogger::ANSI_LIGHT_BLUE      ;
constexpr SLiteral<10>  AnsiLogger::ANSI_LIGHT_MAGENTA   ;
constexpr SLiteral<10>  AnsiLogger::ANSI_LIGHT_CYAN      ;
constexpr SLiteral<11>  AnsiLogger::ANSI_LIGHT_GRAY      ;
constexpr SLiteral<5>   AnsiLogger::ANSI_LIGHT_STD_COL   ;

constexpr SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_RED    ;
constexpr SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_GREEN  ;
constexpr SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_YELLOW ;
constexpr SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_BLUE   ;
constexpr SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_MAGENTA;
constexpr SLiteral<10>  AnsiLogger::ANSI_BG_LIGHT_CYAN   ;
constexpr SLiteral<11>  AnsiLogger::ANSI_BG_LIGHT_GRAY   ;
constexpr SLiteral<5>   AnsiLogger::ANSI_BG_LIGHT_STD_COL;

constexpr SLiteral<4>   AnsiLogger::ANSI_BOLD            ;
constexpr SLiteral<4>   AnsiLogger::ANSI_ITALICS         ;
constexpr SLiteral<4>   AnsiLogger::ANSI_STD_STYLE       ;
constexpr SLiteral<4>   AnsiLogger::ANSI_RESET           ;
#endif

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
    if ( ALOX.Config->Load( variable ) != Priorities::NONE && variable.Size() > 0)
    {
        Substring p= *variable.GetString();
        if(p.Trim().IsNotEmpty())
        {
            if( !p.ConsumeEnum<LightColorUsage>( UseLightColors ) )
            {
                ALIB_WARNING( ASTR("Unknown value specified in variable: {} = '{}'."),
                              variable.Fullname, variable.GetString() );
            }
        }
    }

    if( UseLightColors == LightColorUsage::Auto )
    {
        // default: dark background, hence use light color on foreground
        UseLightColors= LightColorUsage::Foreground;
    }

    // move verbosity information to the end to colorize the whole line
    ALIB_ASSERT_RESULT_NOT_EQUALS( MetaInfo->Format.SearchAndReplace( ASTR("]%V["), ASTR("][") ), 0);
    MetaInfo->Format._(ASTR("%V"));
    MetaInfo->VerbosityError           = ESC::RED;
    MetaInfo->VerbosityWarning         = ESC::BLUE;
    MetaInfo->VerbosityInfo            = ASTR("");
    MetaInfo->VerbosityVerbose         = ESC::GRAY;
}

AnsiLogger::~AnsiLogger()
{
}

// #################################################################################################
// logText
// #################################################################################################


void AnsiLogger::logText( core::Domain&      ,    Verbosity         ,
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
        msgParts.Next( Whitespaces::Keep );

        // check if end of actual msg part is an ANSI sequence. If yes, we extend the actual msg
        if ( rest.CharAtStart() == '[' )
        {
            // read the 'm'
            integer idx= rest.IndexOf( 'm' );
            if ( idx < 0 ) // unknown ANSI Code
            {
                ALIB_WARNING( ASTR("Unknown ANSI ESC Code ") );
                writer.Write( actual );
                continue;
            }

            integer wCharLength= actual.WStringLength();
            column+= wCharLength >= 0 ? wCharLength : actual.Length();

            actual.SetLength<false>( actual.Length() + idx + 2 );
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
            ALIB_ASSERT_WARNING( colNo >=0 && colNo <=9, ASTR("AnsiLogger: Unknown ESC-c code") );

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
                default: ALIB_ERROR(ASTR("Error in ANSI Code"))
                         ansiCol= ASTR("");
                         break;
            }

            writer.Write( ansiCol );
        }

        // Styles
        else if ( c == 's' )
        {
            c=  rest.ConsumeChar();
            writer.Write(  c== 'B' ? ANSI_BOLD
                         : c== 'I' ? ANSI_ITALICS
                         :           ANSI_RESET    );
        }

        // auto tab / end of meta
        else if ( c == 't' || c == 'A' )
        {
            c=  rest.ConsumeChar();
            int extraSpace=  c >= '0' && c <= '9' ? ( c - '0' )
                                                  : ( c - 'A' ) + 10;

            // tab stop (write spaces using a growing buffer)
            integer tabStop= AutoSizes.Next( column, extraSpace );
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
            ALIB_WARNING( ASTR("Unknown ESC code '{}'."), c )
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

}}}}// namespace [aworx::lib::lox::loggers]
