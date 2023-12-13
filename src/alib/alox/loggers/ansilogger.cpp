// #################################################################################################
//  aworx::lib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
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

#   if !defined(HPP_ALIB_RESULTS_REPORT)
#      include "alib/results/report.hpp"
#   endif
#endif // !defined(ALIB_DOX)

using namespace aworx::lib::lox::detail;

namespace aworx { namespace lib { namespace lox { namespace loggers {

// #################################################################################################
// ANSI Escape Codes
// #################################################################################################
#if defined(_MSC_VER)
    // MSC  (as of 12/2015):
    // C4579: in-class initialization for type 'const aworx::character[11]'
    // is not yet implemented; static member will remain uninitialized at run-time but
    // use in constant-expressions is supported
    character  AnsiLogger::ANSI_RED             [6]  { A_CHAR("\033[31m")       }; ///< Select red as foreground color
    character  AnsiLogger::ANSI_GREEN           [6]  { A_CHAR("\033[32m")       }; ///< Select green as foreground color
    character  AnsiLogger::ANSI_YELLOW          [6]  { A_CHAR("\033[33m")       }; ///< Select yellow as foreground color
    character  AnsiLogger::ANSI_BLUE            [6]  { A_CHAR("\033[34m")       }; ///< Select blue as foreground color
    character  AnsiLogger::ANSI_MAGENTA         [6]  { A_CHAR("\033[35m")       }; ///< Select magenta as foreground color
    character  AnsiLogger::ANSI_CYAN            [6]  { A_CHAR("\033[36m")       }; ///< Select cyan as foreground color
    character  AnsiLogger::ANSI_BLACK           [6]  { A_CHAR("\033[30m")       }; ///< Select black as foreground color
    character  AnsiLogger::ANSI_WHITE           [11] { A_CHAR("\033[38;5;15m")  }; ///< Select white as foreground color
    character  AnsiLogger::ANSI_GRAY            [12] { A_CHAR("\033[38;5;240m") }; ///< Select gray as foreground color
    character  AnsiLogger::ANSI_STD_COL         [6]  { A_CHAR("\033[39m")       }; ///< Select standard foreground color

    character  AnsiLogger::ANSI_BG_RED          [6]  { A_CHAR("\033[41m")       }; ///< Select red as background color
    character  AnsiLogger::ANSI_BG_GREEN        [6]  { A_CHAR("\033[42m")       }; ///< Select green as background color
    character  AnsiLogger::ANSI_BG_YELLOW       [6]  { A_CHAR("\033[43m")       }; ///< Select yellow as background color
    character  AnsiLogger::ANSI_BG_BLUE         [6]  { A_CHAR("\033[44m")       }; ///< Select blue as background color
    character  AnsiLogger::ANSI_BG_MAGENTA      [6]  { A_CHAR("\033[45m")       }; ///< Select magenta as background color
    character  AnsiLogger::ANSI_BG_CYAN         [6]  { A_CHAR("\033[46m")       }; ///< Select cyan as background color
    character  AnsiLogger::ANSI_BG_BLACK        [6]  { A_CHAR("\033[40m")       }; ///< Select black as background color
    character  AnsiLogger::ANSI_BG_WHITE        [11] { A_CHAR("\033[48;5;15m")  }; ///< Select white as background color
    character  AnsiLogger::ANSI_BG_GRAY         [12] { A_CHAR("\033[48;5;240m") }; ///< Select gray as background color
    character  AnsiLogger::ANSI_BG_STD_COL      [6]  { A_CHAR("\033[49m")       }; ///< Select standard background color

    character  AnsiLogger::ANSI_LIGHT_RED       [11] { A_CHAR("\033[38;5;09m")  }; ///< Select light red as foreground color
    character  AnsiLogger::ANSI_LIGHT_GREEN     [11] { A_CHAR("\033[38;5;10m")  }; ///< Select light green as foreground color
    character  AnsiLogger::ANSI_LIGHT_YELLOW    [11] { A_CHAR("\033[38;5;11m")  }; ///< Select light yellow as foreground color
    character  AnsiLogger::ANSI_LIGHT_BLUE      [11] { A_CHAR("\033[38;5;12m")  }; ///< Select light blue as foreground color
    character  AnsiLogger::ANSI_LIGHT_MAGENTA   [11] { A_CHAR("\033[38;5;13m")  }; ///< Select light magenta as foreground color
    character  AnsiLogger::ANSI_LIGHT_CYAN      [11] { A_CHAR("\033[38;5;14m")  }; ///< Select light cyan as foreground color
    character  AnsiLogger::ANSI_LIGHT_GRAY      [12] { A_CHAR("\033[38;5;250m") }; ///< Select light gray as foreground color
    character  AnsiLogger::ANSI_LIGHT_STD_COL   [6]  { A_CHAR("\033[39m")       }; ///< Select standard foreground color

    character  AnsiLogger::ANSI_BG_LIGHT_RED    [11] { A_CHAR("\033[48;5;09m")  }; ///< Select light red as background color
    character  AnsiLogger::ANSI_BG_LIGHT_GREEN  [11] { A_CHAR("\033[48;5;10m")  }; ///< Select light green as background color
    character  AnsiLogger::ANSI_BG_LIGHT_YELLOW [11] { A_CHAR("\033[48;5;11m")  }; ///< Select light yellow as background color
    character  AnsiLogger::ANSI_BG_LIGHT_BLUE   [11] { A_CHAR("\033[48;5;12m")  }; ///< Select light blue as background color
    character  AnsiLogger::ANSI_BG_LIGHT_MAGENTA[11] { A_CHAR("\033[48;5;13m")  }; ///< Select light magenta as background color
    character  AnsiLogger::ANSI_BG_LIGHT_CYAN   [11] { A_CHAR("\033[48;5;14m")  }; ///< Select light cyan as background color
    character  AnsiLogger::ANSI_BG_LIGHT_GRAY   [12] { A_CHAR("\033[48;5;250m") }; ///< Select light gray as background color
    character  AnsiLogger::ANSI_BG_LIGHT_STD_COL[6]  { A_CHAR("\033[49m")       }; ///< Select standard background color

    character  AnsiLogger::ANSI_BOLD            [5]  { A_CHAR("\033[1m")        }; ///< Select bold font style
    character  AnsiLogger::ANSI_ITALICS         [5]  { A_CHAR("\033[3m")        }; ///< Select italics font style
    character  AnsiLogger::ANSI_STD_STYLE       [5]  { A_CHAR("\033[0m")        }; ///< Select standard font style
    character  AnsiLogger::ANSI_RESET           [5]  { A_CHAR("\033[0m")        }; ///< Reset colors and font style

#elif ALIB_CPPVER < 17
    constexpr character  AnsiLogger::ANSI_RED             [6] ;
    constexpr character  AnsiLogger::ANSI_GREEN           [6] ;
    constexpr character  AnsiLogger::ANSI_YELLOW          [6] ;
    constexpr character  AnsiLogger::ANSI_BLUE            [6] ;
    constexpr character  AnsiLogger::ANSI_MAGENTA         [6] ;
    constexpr character  AnsiLogger::ANSI_CYAN            [6] ;
    constexpr character  AnsiLogger::ANSI_BLACK           [6] ;
    constexpr character  AnsiLogger::ANSI_WHITE           [11];
    constexpr character  AnsiLogger::ANSI_GRAY            [12];
    constexpr character  AnsiLogger::ANSI_STD_COL         [6] ;

    constexpr character  AnsiLogger::ANSI_BG_RED          [6] ;
    constexpr character  AnsiLogger::ANSI_BG_GREEN        [6] ;
    constexpr character  AnsiLogger::ANSI_BG_YELLOW       [6] ;
    constexpr character  AnsiLogger::ANSI_BG_BLUE         [6] ;
    constexpr character  AnsiLogger::ANSI_BG_MAGENTA      [6] ;
    constexpr character  AnsiLogger::ANSI_BG_CYAN         [6] ;
    constexpr character  AnsiLogger::ANSI_BG_BLACK        [6] ;
    constexpr character  AnsiLogger::ANSI_BG_WHITE        [11];
    constexpr character  AnsiLogger::ANSI_BG_GRAY         [12];
    constexpr character  AnsiLogger::ANSI_BG_STD_COL      [6] ;

    constexpr character  AnsiLogger::ANSI_LIGHT_RED       [11];
    constexpr character  AnsiLogger::ANSI_LIGHT_GREEN     [11];
    constexpr character  AnsiLogger::ANSI_LIGHT_YELLOW    [11];
    constexpr character  AnsiLogger::ANSI_LIGHT_BLUE      [11];
    constexpr character  AnsiLogger::ANSI_LIGHT_MAGENTA   [11];
    constexpr character  AnsiLogger::ANSI_LIGHT_CYAN      [11];
    constexpr character  AnsiLogger::ANSI_LIGHT_GRAY      [12];
    constexpr character  AnsiLogger::ANSI_LIGHT_STD_COL   [6] ;

    constexpr character  AnsiLogger::ANSI_BG_LIGHT_RED    [11];
    constexpr character  AnsiLogger::ANSI_BG_LIGHT_GREEN  [11];
    constexpr character  AnsiLogger::ANSI_BG_LIGHT_YELLOW [11];
    constexpr character  AnsiLogger::ANSI_BG_LIGHT_BLUE   [11];
    constexpr character  AnsiLogger::ANSI_BG_LIGHT_MAGENTA[11];
    constexpr character  AnsiLogger::ANSI_BG_LIGHT_CYAN   [11];
    constexpr character  AnsiLogger::ANSI_BG_LIGHT_GRAY   [12];
    constexpr character  AnsiLogger::ANSI_BG_LIGHT_STD_COL[6] ;

    constexpr character  AnsiLogger::ANSI_BOLD            [5] ;
    constexpr character  AnsiLogger::ANSI_ITALICS         [5] ;
    constexpr character  AnsiLogger::ANSI_STD_STYLE       [5] ;
    constexpr character  AnsiLogger::ANSI_RESET           [5] ;
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
        msgParts.Next( Whitespaces::Keep );

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

}}}}// namespace [aworx::lib::lox::loggers]
