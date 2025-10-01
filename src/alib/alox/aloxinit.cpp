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
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.ALox.init;
    import   ALib.Lang;
    import   ALib.Containers.List;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Variables;
    import   ALib.Camp;
    import   ALib.Camp.Base;
#else
#   include "ALib.Lang.H"
#   include "ALib.Containers.List.H"
#   include "ALib.Strings.H"
#   include "ALib.Boxing.H"
#   include "ALib.ALox.H"
#endif
// ======================================   Implementation   =======================================

#if !DOXYGEN
ALIB_BOXING_VTABLE_DEFINE( alib::lox::Verbosity               , vt_lox_verbosity )
ALIB_BOXING_VTABLE_DEFINE( alib::lox::Scope                   , vt_lox_scope     )
#endif // !DOXYGEN

namespace alib::lox {

void ESC::ReplaceToReadable( AString& target, integer startIdx )
{
    while( (startIdx= target.IndexOf( '\033', startIdx ) ) >= 0 )
    {
        String32 val("{ESC::");
        character c=  target.CharAt( startIdx + 1 );
        character c2= target.CharAt( startIdx + 2 );

        const character* code= A_CHAR("ERROR");

        // colors
        if( c == 'c' || c == 'C' )
        {
            if ( c == 'C' )
                val._<NC>( A_CHAR("BG_") );
            switch( c2 - '0' )
            {
                case 0:  code= A_CHAR("RED")     ; break;
                case 1:  code= A_CHAR("GREEN")   ; break;
                case 2:  code= A_CHAR("YELLOW")  ; break;
                case 3:  code= A_CHAR("BLUE")    ; break;
                case 4:  code= A_CHAR("MAGENTA") ; break;
                case 5:  code= A_CHAR("CYAN")    ; break;
                case 6:  code= A_CHAR("BLACK")   ; break;
                case 7:  code= A_CHAR("WHITE")   ; break;
                case 8:  code= A_CHAR("GRAY")    ; break;
                case 9:  code= A_CHAR("RESET")   ; break;
                default: code= A_CHAR("COL_ERR"); break;
            }

        }

        // styles
        else if( c == 's' )
        {
            switch( c2 )
            {
                case 'B': code= A_CHAR("BOLD")         ; break;
                case 'I': code= A_CHAR("ITALICS")      ; break;
                case 'r': code= A_CHAR("STYLE_RESET")  ; break;
                case 'a': code= A_CHAR("RESET")        ; break;
                default:  code= A_CHAR("STYLE_ERR")    ; break;
            }
        }

        // styles
        else if( c == 'l' )
        {
            switch( c2 )
            {
                case 'S': code= A_CHAR("URL_START")    ; break;
                case 'E': code= A_CHAR("URL_END")      ; break;
                default:  code= A_CHAR("URL_ERR")      ; break;
            }
        }

        // others
        else if( c == 't' && c2 == '0' )    code= A_CHAR("TAB");
        else if( c == 'A' && c2 == '0' )    code= A_CHAR("EOMETA");

        // Replace
        val._<NC>(code)._('}');
        target.ReplaceSubstring<NC>( val, startIdx, 3 );
        startIdx+= 3;
    }
}

} // namespace [alib::lox]
