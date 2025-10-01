// #################################################################################################
//  ALib C++ Library
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
#include "alib/strings/strings.prepro.hpp"
#include "ALib.Strings.Vector.H"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Strings.Escaper;
    import   ALib.Strings.Tokenizer;
#else
#   include "ALib.Strings.Escaper.H"
#   include "ALib.Strings.Tokenizer.H"
#endif

// ======================================   Implementation   =======================================
namespace alib::strings::util {

// =================================================================================================
// ==== StringEscaper
// =================================================================================================
#if ALIB_MONOMEM
int StringEscaper::EscapeTokens( StringVectorMA& result, const String& value, const String& delimiters  ) const
{
    const integer oldSize= result.Size();
    Tokenizer tknzr( value, delimiters.CharAtStart() );
    tknzr.TrimChars.Reset();
    while(tknzr.HasNext())
        result.Add( tknzr.Next() );
    
    return int(result.Size() - oldSize);
}
#endif // if ALIB_MONOMEM

// =================================================================================================
// ==== StringEscaperStandard
// =================================================================================================
AString& StringEscaperStandard::Unescape( const String& src, AString& dest )                   const
{
    Substring parser(src);
    parser.Trim();
    if( parser.Length() > 1 && parser.CharAtStart() == '"' && parser.CharAtEnd() == '"')
    {
        parser.ConsumeChar       <NC>();
        parser.ConsumeCharFromEnd<NC>();
    }
    bool lastWasSlash= false;

    while( parser.IsNotEmpty() )
    {
        character c= parser.ConsumeChar<NC>();

        if( lastWasSlash )
        {
            lastWasSlash= false;
            character escChr= c == '\\' ? '\\' :
                              c == '"'  ? '"'  :
                              c == 'n'  ? '\n' :
                              c == 'r'  ? '\r' :
                              c == 't'  ? '\t' :
                              c == 'a'  ? '\a' :
                              c == 'b'  ? '\b' :
                              c == 'v'  ? '\v' :
                              c == 'f'  ? '\f' :
                              c == 'e'  ? '\033' :
                              c;

            dest._<NC>(escChr);
            continue;
        }

        if( c== '\\' )
        {
            lastWasSlash= true;
            continue;
        }

        dest._<NC>(c);
    }

    return dest;
}

AString& StringEscaperStandard::Escape( const String& src, AString& dest, const String& delimiters )    const
{
    Substring parser(src);
    bool needsQuotes=    parser.CharAtStart() == ' '
                      || parser.CharAtStart() == '\t'
                      || parser.CharAtEnd()   == ' '
                      || parser.CharAtEnd()   == '\t';
    if (!needsQuotes)
        for( character c : delimiters )
            if( parser.IndexOf(c) >= 0 )
            {
                needsQuotes= true;
                break;
            }
                      
    if ( needsQuotes )
        dest._<NC>('"');

    while( parser.IsNotEmpty() )
    {
        character c= parser.ConsumeChar();

        switch(c)
        {
            case '"'    : dest._<NC>(needsQuotes ? "\\\"" : "\"");
                          break;
            case '\\'   : dest._<NC>("\\\\"); break;
            case '\r'   : dest._<NC>("\\r" ); break;
            case '\n'   : dest._<NC>("\\n" ); break;
            case '\t'   : dest._<NC>("\\t" ); break;
            case '\a'   : dest._<NC>("\\a" ); break;
            case '\b'   : dest._<NC>("\\b" ); break;
            case '\v'   : dest._<NC>("\\v" ); break;
            case '\f'   : dest._<NC>("\\f" ); break;
            case '\033' : dest._<NC>("\\e" ); break;
            default     : dest._<NC>(c);      break;
        }
    }

    if ( needsQuotes )
        dest._('"');

    return dest;
}


#if ALIB_MONOMEM
int StringEscaperStandard::EscapeTokens( StringVectorMA& result, const String& src, const String& delimiters )  const
{
    String2K buf;
    const integer oldSize= result.Size();
    Tokenizer tknzr( src, delimiters.CharAtStart() );
    tknzr.TrimChars.Reset();
    while(tknzr.HasNext())
    {
        Escape( tknzr.Next(), buf.Reset(), delimiters );
        result.Add( buf );
    }
    return int(result.Size() - oldSize);
}

int StringEscaperStandard::UnescapeTokens( StringVectorMA& result, const String& value, const String& delimiters  ) const
{
    const integer oldSize= result.Size();
    String512 tempBuf; tempBuf.DbgDisableBufferReplacementWarning();
    Substring src( value );

    // tokenize
    bool inQuote=      false;
    bool lastWasSlash= false;
    integer idx=          0;
    while( idx < src.Length()  )
    {
        character c= src.CharAt<NC>( idx++ );

        if( lastWasSlash )
        {
            lastWasSlash= false;
            continue;
        }

        if( c== '\\' )
        {
            lastWasSlash= true;
            continue;
        }

        if( c== '"' && (idx==1 || inQuote) )
        {
            inQuote= !inQuote;
            continue;
        }

        if( !inQuote && delimiters.IndexOf(c) >= 0 )
        {
            Substring tok= src.Substring<NC>( 0, idx - 1 );
            Unescape( tok, tempBuf );
            result.Add( tempBuf );
            tempBuf.Reset();
            src.ConsumeChars( idx );
            src.TrimStart();
            idx= 0;
        }
    }

    if ( src.IsNotEmpty() )
    {
        Unescape( src, tempBuf );
        result.Add( tempBuf );
    }

    return int (result.Size() - oldSize);
}
#endif // if ALIB_MONOMEM



} // namespace [alib::util::strings]
