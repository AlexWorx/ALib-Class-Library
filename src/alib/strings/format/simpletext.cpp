// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_STRINGS_FORMAT_SIMPLETEXT)
    #include "alib/strings/format/simpletext.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_STRINGTUPLES)
    #include "alib/strings/util/stringtuples.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
    #include "alib/strings/util/tokenizer.hpp"
#endif


#if !defined (HPP_ALIB_LANG_EXCEPTION)
    #include "alib/lang/exception.hpp"
#endif

#include <algorithm>

using namespace std;

namespace aworx { namespace lib { namespace strings { namespace format  {

//! @cond NO_DOX
namespace {
    template<typename TChar>    struct StringConstants {};
    template<>   struct StringConstants<nchar>
    {
        static const NTString spc    ;
        static const NTString spcpsc ;
        static const NTString gtq    ;
        static const NTString gtgt   ;
        static const NTString smsm   ;
        static const NTString smast  ;
        static const NTString astgt  ;
        static const NTString HL     ;
        static const NTString escr   ;
        static const NTString exc1   ;
        static const NTString exc2   ;
    };
    template<>   struct StringConstants<wchar>
    {
        static const WTString spc    ;
        static const WTString spcpsc ;
        static const WTString gtq    ;
        static const WTString gtgt   ;
        static const WTString smsm   ;
        static const WTString smast  ;
        static const WTString astgt  ;
        static const WTString HL     ;
        static const WTString escr   ;
        static const WTString exc1   ;
        static const WTString exc2   ;
    };

const NTString StringConstants<nchar   >::spc            =  " "   ;
const NTString StringConstants<nchar>::spcpsc         =  "  "  ;
const NTString StringConstants<nchar>::gtq            =  ">'"  ;
const NTString StringConstants<nchar>::gtgt           =  ">>"  ;
const NTString StringConstants<nchar>::smsm           =  "<<"  ;
const NTString StringConstants<nchar>::smast          =  "<*"  ;
const NTString StringConstants<nchar>::astgt          =  "*>"  ;
const NTString StringConstants<nchar>::HL             =  "HL"  ;
const NTString StringConstants<nchar>::escr           =  "\r"  ;
const NTString StringConstants<nchar>::exc1           =  "E{}: <{}>"  ;
const NTString StringConstants<nchar>::exc2           =  "    ";
const WTString StringConstants<wchar>::spc            = L" "   ;
const WTString StringConstants<wchar>::spcpsc         = L"  "  ;
const WTString StringConstants<wchar>::gtq            = L">'"  ;
const WTString StringConstants<wchar>::gtgt           = L">>"  ;
const WTString StringConstants<wchar>::smsm           = L"<<"  ;
const WTString StringConstants<wchar>::smast          = L"<*"  ;
const WTString StringConstants<wchar>::astgt          = L"*>"  ;
const WTString StringConstants<wchar>::HL             = L"HL"  ;
const WTString StringConstants<wchar>::escr           = L"\r"  ;
const WTString StringConstants<wchar>::exc1           = L"E{}: <{}>"  ;
const WTString StringConstants<wchar>::exc2           = L"    ";

} // anonymous namespace
//! @endcond



#if ALIB_NARROW_STRINGS
template<> SimpleTextBase<nchar>::SimpleTextBase() : Text(2048), Formatter(lib::STRINGS.GetDefaultFormatter()                                                ) {}
template<> SimpleTextBase<wchar>::SimpleTextBase() : Text(2048), Formatter(std::shared_ptr<FormatterBase<wchar>>( new FormatterPythonStyleBase<wchar>()) ) {}
#else
template<> SimpleTextBase<nchar>::SimpleTextBase() : Text(2048), Formatter(std::shared_ptr<FormatterBase<nchar>>( new FormatterPythonStyleBase<nchar>()) ) {}
template<> SimpleTextBase<wchar>::SimpleTextBase() : Text(2048), Formatter(lib::STRINGS.GetDefaultFormatter()                                                ) {}
#endif


// #################################################################################################
// Non-static methods (if used with instance).
// #################################################################################################

template<typename TChar>
SimpleTextBase<TChar>& SimpleTextBase<TChar>::PushIndent( const StringBase<TChar>& indentFirstLine,
                                                          const StringBase<TChar>& pIndentOtherLines )
{
    StringBase<TChar> indentOtherLines= pIndentOtherLines.IsNull()  ? indentFirstLine
                                                                    : pIndentOtherLines;

    IndentFirstLine ._( indentFirstLine );
    IndentOtherLines._( indentOtherLines );
    IndentSizesFirstLine .push( indentFirstLine.Length()  );
    IndentSizesOtherLines.push( indentOtherLines.Length() );
    return *this;
}

template<typename TChar>
SimpleTextBase<TChar>& SimpleTextBase<TChar>::PushIndent( uinteger qty, TChar fillChar )
{
    IndentFirstLine .InsertChars( fillChar, static_cast<integer>( qty ) );
    IndentOtherLines.InsertChars( fillChar, static_cast<integer>( qty ) );
    IndentSizesFirstLine .push( static_cast<integer>( qty ) );
    IndentSizesOtherLines.push( static_cast<integer>( qty ) );
    return *this;
}

template<typename TChar>
SimpleTextBase<TChar>& SimpleTextBase<TChar>::PopIndent()
{
    ALIB_ASSERT_ERROR( !IndentSizesFirstLine.empty(), ASTR("SimpleText: PopIndent without prior push.") )
    IndentFirstLine.DeleteEnd( static_cast<integer>( IndentSizesFirstLine.top() ) );
    IndentSizesFirstLine.pop();

    ALIB_ASSERT_ERROR( !IndentSizesOtherLines.empty(), ASTR("SimpleText: PopIndent without prior push.") )
    IndentOtherLines.DeleteEnd( static_cast<integer>( IndentSizesOtherLines.top() ) );
    IndentSizesOtherLines.pop();
    return *this;
}

template<typename TChar>
SimpleTextBase<TChar>&     SimpleTextBase<TChar>::Clear()
{
    Text.Clear();
    while( IndentSizesFirstLine .size() ) IndentSizesFirstLine .pop();
    while( IndentSizesOtherLines.size() ) IndentSizesOtherLines.pop();
    IndentFirstLine .Clear();
    IndentOtherLines.Clear();
    markerBulletLevel= 0;
    return *this;
}


template<typename TChar>
void   SimpleTextBase<TChar>::Add( Boxes&  args )
{
    integer startIdx= Text.Length();
    Formatter->Format( Text, args );
    integer maxLineWidth;
    SimpleTextBase<TChar>::FormatParagraph( Text, startIdx, LineWidth, JustifyChar, maxLineWidth, IndentFirstLine, IndentOtherLines );
    DetectedMaxLineWidth= (std::max)(DetectedMaxLineWidth, maxLineWidth );

    if ( Text.IsNotEmpty() && !Text.EndsWith( aworx::StringConstants<TChar>::NewLine ) )
    {
        #if defined( _WIN32 )
            if( Text.CharAtEnd() == '\n' )
                Text.DeleteEnd(1);
        #endif
        Text << aworx::StringConstants<TChar>::NewLine;
    }
}

//! @cond NO_DOX
namespace
{
    template<typename TChar>
    ALIB_NO_RETURN
    void throwMarkerException( Exceptions eType, StringBase<TChar>& markedBuffer, integer errPos );

    template<>
    void throwMarkerException( Exceptions eType, StringBase<nchar>& markedBuffer, integer errPos )
    {
        NString64 actText;
        integer excerptPos= 25;
        integer excerptStart= errPos - 25;
        if( excerptStart <= 0 )
        {
            excerptPos+= excerptStart;
            excerptStart= 0;
        }
        else
        {
            actText._( "[...]" );
            excerptPos+= 5;
        }


        actText._( markedBuffer, excerptStart, 50 );
        if( markedBuffer.Length() > excerptStart + 50 )
            actText._( "[...]" );
                     actText.SearchAndReplace( "\r", "\\r", excerptPos );
                     actText.SearchAndReplace( "\n", "\\n", excerptPos );
        excerptPos+= actText.SearchAndReplace( "\r", "\\r"             );
        excerptPos+= actText.SearchAndReplace( "\n", "\\n"             );

        throw Exception( ALIB_CALLER_NULLED, eType, errPos, actText, excerptPos );
    }

    template<>
    void throwMarkerException( Exceptions eType, StringBase<wchar>& markedBuffer, integer errPos )
    {
        WString64 actText;
        integer excerptPos= 25;
        integer excerptStart= errPos - 25;
        if( excerptStart <= 0 )
        {
            excerptPos+= excerptStart;
            excerptStart= 0;
        }
        else
        {
            actText._( L"[...]" );
            excerptPos+= 5;
        }


        actText._( markedBuffer, excerptStart, 50 );
        if( markedBuffer.Length() > excerptStart + 50 )
            actText._( L"[...]" );
                     actText.SearchAndReplace( L"\r", L"\\r", excerptPos );
                     actText.SearchAndReplace( L"\n", L"\\n", excerptPos );
        excerptPos+= actText.SearchAndReplace( L"\r", L"\\r"             );
        excerptPos+= actText.SearchAndReplace( L"\n", L"\\n"             );

        throw Exception( ALIB_CALLER_NULLED, eType, errPos, actText, excerptPos );
    }
}
//! @endcond


template<typename TChar>
void   SimpleTextBase<TChar>::AddMarked( Boxes&  args )
{
    TChar searchCharBuf[3];
          searchCharBuf[0]= MarkerChar;
          searchCharBuf[1]= '\n';
    StringBase<TChar> searchChars(searchCharBuf, 2);

    Formatter->Format( markedBuffer.Clear(), args );
    SubstringBase<TChar> parser=  markedBuffer;
    integer lastTextStart=  Text.Length();

    while( parser.IsNotEmpty() )
    {
        integer pos= parser.template IndexOfAny<Inclusion::Include, false>( searchChars );

        // not found
        if( pos < 0 )
        {
            Text << parser;
            break;
        }

        // new line
        if( parser.CharAt( pos ) == '\n' )
        {
            parser.template ConsumeChars<false, lang::CurrentData::Keep>( pos, Text, 1 );
            parser.ConsumeChar( '\r' );
            Text << aworx::StringConstants<TChar>::NewLine;
            integer maxLineWidth;
            SimpleTextBase<TChar>::FormatParagraph( Text, lastTextStart, LineWidth, JustifyChar,
                                         maxLineWidth, IndentFirstLine, IndentOtherLines );
            DetectedMaxLineWidth= (std::max)(DetectedMaxLineWidth, maxLineWidth );
            lastTextStart=  Text.Length();
            continue;
        }

        parser.template ConsumeChars<false, lang::CurrentData::Keep>( pos, Text, 1 );

        // double marker: insert one symbol
             if( parser.ConsumeChar( MarkerChar ) )
            Text << MarkerChar;

        // Indent
        else if( parser.ConsumeString(StringConstants<TChar>::gtq) )
            PushIndent(  parser.ConsumeToken( '\'' ) );

        else if( parser.ConsumeString(StringConstants<TChar>::gtgt) )
            PushIndent( StringConstants<TChar>::spcpsc );

        else if( parser.ConsumeString(StringConstants<TChar>::smsm) )
        {
            if( IndentSizesFirstLine.empty() )
                throwMarkerException( Exceptions::EndmarkerWithoutStart, markedBuffer,
                                      markedBuffer.Length() - parser.Length() - 3 );
            PopIndent();
        }

        // bullets
        else if( parser.ConsumeString(StringConstants<TChar>::astgt) )
        {
            if( markerBulletLevel > 0 )
            {
                IndentFirstLine .DeleteEnd( 2 )._( StringConstants<TChar>::spcpsc );
                IndentOtherLines.DeleteEnd( 2 )._( StringConstants<TChar>::spcpsc );
            }
            IndentFirstLine ._( MarkerBullets[markerBulletLevel] )._(' ');
            IndentOtherLines._( "  " );
            markerBulletLevel++;
        }
        else if( parser.ConsumeString(StringConstants<TChar>::smast) )
        {
            if( markerBulletLevel == 0 )
                throwMarkerException( Exceptions::EndmarkerWithoutStart, markedBuffer,
                                      markedBuffer.Length() - parser.Length() - 3 );

            int deIndentCnt= markerBulletLevel > 1 ? 4 : 2;
            IndentFirstLine .DeleteEnd( deIndentCnt );
            IndentOtherLines.DeleteEnd( deIndentCnt );
            if( --markerBulletLevel > 0 )
            {
                IndentFirstLine ._( MarkerBullets[markerBulletLevel - 1] )._(' ');
                IndentOtherLines._( StringConstants<TChar>::spcpsc );
            }
        }

        else if( parser.ConsumeChar('p' ) || parser.ConsumeChar('P') )
            Text << aworx::StringConstants<TChar>::NewLine;


        // horizontal line
        else if( parser.ConsumeString(StringConstants<TChar>::HL) )
        {
            Text.InsertChars( parser.ConsumeChar() , LineWidth - IndentFirstLine.Length() )
                ._( aworx::StringConstants<TChar>::NewLine );
        }

        // not recognized
        else
        {
            throwMarkerException( Exceptions::UnknownMarker, markedBuffer,
                                  markedBuffer.Length() - parser.Length() - 1 );
        }
    }

    if( lastTextStart < Text.Length() )
    {
        integer maxLineWidth;
        SimpleTextBase<TChar>::FormatParagraph( Text, lastTextStart, LineWidth, JustifyChar, maxLineWidth,
                                     IndentFirstLine, IndentOtherLines );
        DetectedMaxLineWidth= (std::max)(DetectedMaxLineWidth, maxLineWidth );
    }

    if ( Text.IsNotEmpty() && !Text.EndsWith( aworx::StringConstants<TChar>::NewLine ) )
        Text << aworx::StringConstants<TChar>::NewLine;
}


// #################################################################################################
// The static formatter method
// #################################################################################################
template<typename TChar>
void  SimpleTextBase<TChar>::FormatParagraph( AStringBase<TChar>& text,
                                              integer startIdx, integer lineWidth,
                                              TChar justifyChar, integer& maxLineWidth,
                                              const StringBase<TChar>& pIndentFirstLine,
                                              const StringBase<TChar>& pIndentOtherLines                         )
{
    maxLineWidth= 0;
    StringBase<TChar> indentFirstLines= pIndentFirstLine .IsNotNull()  ? pIndentFirstLine  : aworx::StringConstants<TChar>::EmptyString;
    StringBase<TChar> indentOtherLines= pIndentOtherLines.IsNotNull()  ? pIndentOtherLines : pIndentFirstLine;

    bool isFirstLine= true;

    StringBase<TChar> indent= nullptr;
    bool   indentAreJustSpaces= false;

    integer maxLineWidthDetectionStartIdx= startIdx;
    while ( startIdx < text.Length() )
    {
        maxLineWidth= (std::max)( maxLineWidth, startIdx - maxLineWidthDetectionStartIdx -1 );
        maxLineWidthDetectionStartIdx= startIdx;

        // skip lines beginning with newline characters, unless indent has non-space characters
        int isWinNL=  text[ startIdx ] == '\r' ? 1 : 0;
        if ( text[ startIdx + isWinNL ] == '\n' )
        {
            // set indent and check if its just spaces
            if( indent.IsNull() )
            {
                indent = isFirstLine ? indentFirstLines : indentOtherLines;
                indentAreJustSpaces= (indent.template IndexOfAny<Inclusion::Exclude>( StringConstants<TChar>::spc ) < 0 );
            }

            // insert indent if not just spaces
            if ( !indentAreJustSpaces )
            {
                text.InsertAt( indent, startIdx );
                startIdx+= indent.Length();
            }

            #if defined( _WIN32 )
                if( !isWinNL )
                {
                    text.template InsertChars<false>('\r', 1, startIdx );
                    isWinNL= true;
                }
            #else
                if( isWinNL )
                {
                    text.template Delete<false>(startIdx, 1);
                    isWinNL= false;
                }
            #endif


            startIdx+= 1 + isWinNL;
            if( isFirstLine )
            {
                isFirstLine= false;
                indent=      nullptr;
            }

            continue;
        }

        // insert indent
        if( indent.IsNull() )
        {
            indent = isFirstLine ? indentFirstLines : indentOtherLines;
            indentAreJustSpaces= (indent.template IndexOfAny<Inclusion::Exclude>( StringConstants<TChar>::spc ) < 0 );
        }
        text.InsertAt( indent, startIdx );

        if( isFirstLine )
        {
            isFirstLine= false;
            indent=      nullptr;
        }


        // find next end of line. Remember last space in line
        integer lastSpaceInLine=  0;
        bool    isLastLine=       true;
        bool    exceeds=          false;
        integer idx= startIdx + indent.Length() - 1;
        while (++idx < text.Length() )
        {
            TChar c= text[idx];
            if ( c == '\n' )
            {
                idx++;
                break;
            }
            exceeds= lineWidth > 0 && idx - startIdx >= lineWidth;

            if( c == ' ' )
            {
                if(idx - startIdx <= lineWidth )
                    lastSpaceInLine= idx;

                if( exceeds )
                {
                    isLastLine= false;
                    break;
                }
            }
        }

        // correct newline
        #if defined( _WIN32 )
            if( text[idx-1] == '\n' && text[idx-2] != '\r' )
            {
                text.template InsertChars<false>('\r', 1, idx-1 );
                idx++;
            }
        #else
            if( text[idx-1] == '\n' && text[idx-2] == '\r' )
            {
                text.template Delete<false>((idx-2), 1);
                idx--;
            }
        #endif

        // wrap line.
        if( exceeds && ( lastSpaceInLine || !isLastLine ) )
        {
            integer wrapPos= lastSpaceInLine > 0 ? lastSpaceInLine : idx;
            text.ReplaceSubstring( aworx::StringConstants<TChar>::NewLine, wrapPos, 1 );
            idx= wrapPos + aworx::StringConstants<TChar>::NewLine.Length();

            // block justification
            if( justifyChar != '\0' )
            {
                integer qtyInserts= lineWidth - (wrapPos - startIdx );
                if( qtyInserts > 0 )
                {
                    // search first non-space after indent.
                    integer leftInsertBoundary= startIdx + indent.Length();
                    while ( leftInsertBoundary < idx && text[leftInsertBoundary] == ' ' )
                        leftInsertBoundary++;

                    if( leftInsertBoundary < idx )
                    {
                        while( qtyInserts > 0 )
                        {
                            integer actPos= idx - 1;
                            bool foundOne= false;
                            while( qtyInserts > 0 )
                            {
                                actPos= text.LastIndexOf( ' ', actPos );
                                if( actPos < leftInsertBoundary )
                                    break;
                                foundOne= true;
                                text.InsertChars( justifyChar, 1, actPos );
                                idx++;
                                qtyInserts--;
                                while( --actPos > leftInsertBoundary && text[actPos] == ' ' )
                                    ;
                            }

                            if( !foundOne )
                                break;
                        }
                    }
                }
            }
        }

        startIdx= idx;
    }

    maxLineWidth= (std::max)( maxLineWidth, startIdx - maxLineWidthDetectionStartIdx -1 );
}


template<typename TChar>
SimpleTextBase<TChar>& SimpleTextBase<TChar>::AddException( Exception&  e )
{
    Tokenizer tknzr;
    tknzr.Whitespaces= StringConstants<TChar>::escr;
    String256 buf;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( buf,  ReplaceExternalBuffer );

    size_t entryNo= 1;
    for ( auto entry= e.Entries.begin(); entry < e.Entries.end(); entry++ )
    {
        Add( StringConstants<TChar>::exc1, entryNo, entry->Code );
        PushIndent( StringConstants<TChar>::exc2 );
        Exception::Describe( buf.Clear(), *entry );
        tknzr.Set( buf, '\n' );
        while( tknzr.HasNext() )
            Add( tknzr.Next()  );

        PopIndent();
        entryNo++;
    }

    return *this;
}

//! @cond NO_DOX

template SimpleTextBase<nchar>&  SimpleTextBase<nchar>::PushIndent     (const NString&,const NString&);
template SimpleTextBase<nchar>&  SimpleTextBase<nchar>::PushIndent     (uinteger qty, nchar);
template SimpleTextBase<nchar>&  SimpleTextBase<nchar>::PopIndent      ();
template SimpleTextBase<nchar>&  SimpleTextBase<nchar>::Clear          ();
template void                    SimpleTextBase<nchar>::Add            (Boxes&);
template void                    SimpleTextBase<nchar>::AddMarked      (Boxes&);
template void                    SimpleTextBase<nchar>::FormatParagraph(NAString&,integer,integer,nchar,integer&,const NString&,const NString&);
template SimpleTextBase<nchar>&  SimpleTextBase<nchar>::AddException   (Exception&);

template SimpleTextBase<wchar>&  SimpleTextBase<wchar>::PushIndent     (const WString&,const WString&);
template SimpleTextBase<wchar>&  SimpleTextBase<wchar>::PushIndent     (uinteger qty, wchar);
template SimpleTextBase<wchar>&  SimpleTextBase<wchar>::PopIndent      ();
template SimpleTextBase<wchar>&  SimpleTextBase<wchar>::Clear          ();
template void                    SimpleTextBase<wchar>::Add            (Boxes&);
template void                    SimpleTextBase<wchar>::AddMarked      (Boxes&);
template void                    SimpleTextBase<wchar>::FormatParagraph(WAString&,integer,integer,wchar,integer&,const WString&,const WString&);
template SimpleTextBase<wchar>&  SimpleTextBase<wchar>::AddException   (Exception&);
//! @endcond

}}}} // namespace [aworx::lib::strings::util]

