// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_TEXT_PARAGRAPHS)
#   include "alib/text/paragraphs.hpp"
#endif
#   if !defined(HPP_ALIB_TEXT_TEXT)
#      include "alib/text/text.hpp"
#   endif

#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#   include "alib/strings/util/tokenizer.hpp"
#endif

#if !defined (HPP_ALIB_RESULTS_EXCEPTION)
#   include "alib/results/exception.hpp"
#endif
#endif // !defined(ALIB_DOX)

using namespace aworx::lib::strings;
namespace aworx { namespace lib { namespace text  {


// #################################################################################################
// Non-static methods (if used with instance).
// #################################################################################################
Paragraphs::Paragraphs()
: Buffer(text)
, Formatter( Formatter::GetDefault()  )
{
    text.SetBuffer(2048);
}

Paragraphs::Paragraphs(AString& externalBuffer)
: Buffer(externalBuffer)
, Formatter( Formatter::GetDefault()  )
{}



Paragraphs& Paragraphs::PushIndent( const String& indentFirstLine,
                                        const String& pIndentOtherLines )
{
    String indentOtherLines= pIndentOtherLines.IsNull()  ? indentFirstLine
                                                                 : pIndentOtherLines;

    IndentFirstLine ._( indentFirstLine );
    IndentOtherLines._( indentOtherLines );
    IndentSizesFirstLine .push( indentFirstLine.Length()  );
    IndentSizesOtherLines.push( indentOtherLines.Length() );
    return *this;
}


Paragraphs& Paragraphs::PushIndent( uinteger qty, character fillChar )
{
    IndentFirstLine .InsertChars( fillChar, static_cast<integer>( qty ) );
    IndentOtherLines.InsertChars( fillChar, static_cast<integer>( qty ) );
    IndentSizesFirstLine .push( static_cast<integer>( qty ) );
    IndentSizesOtherLines.push( static_cast<integer>( qty ) );
    return *this;
}


Paragraphs& Paragraphs::PopIndent()
{
    ALIB_ASSERT_ERROR( !IndentSizesFirstLine.empty(), "Paragraphs: PopIndent without prior push." )
    IndentFirstLine.DeleteEnd( static_cast<integer>( IndentSizesFirstLine.top() ) );
    IndentSizesFirstLine.pop();

    ALIB_ASSERT_ERROR( !IndentSizesOtherLines.empty(),"Paragraphs: PopIndent without prior push." )
    IndentOtherLines.DeleteEnd( static_cast<integer>( IndentSizesOtherLines.top() ) );
    IndentSizesOtherLines.pop();
    return *this;
}


Paragraphs&     Paragraphs::Clear()
{
    Buffer.Reset();
    while( IndentSizesFirstLine .size() ) IndentSizesFirstLine .pop();
    while( IndentSizesOtherLines.size() ) IndentSizesOtherLines.pop();
    IndentFirstLine .Reset();
    IndentOtherLines.Reset();
    markerBulletLevel= 0;
    DetectedMaxLineWidth= 0;
    return *this;
}



void   Paragraphs::Add( Boxes&  args )
{
    integer startIdx= Buffer.Length();
    Formatter->Acquire(ALIB_CALLER_PRUNED);
    try
    {
        Formatter->FormatArgs( Buffer, args );
    }
    catch(Exception& )
    {
        Formatter->Release();
        throw;
    }
    Formatter->Release();

    integer maxLineWidth;
    Paragraphs::Format( Buffer, startIdx, LineWidth, JustifyChar, maxLineWidth, IndentFirstLine, IndentOtherLines );
    DetectedMaxLineWidth= (std::max)(DetectedMaxLineWidth, maxLineWidth );

    if ( Buffer.IsNotEmpty() && !Buffer.EndsWith( NewLine() ) )
    {
        #if defined( _WIN32 )
            if( Buffer.CharAtEnd() == '\n' )
                Buffer.DeleteEnd(1);
        #endif
        Buffer << NewLine();
    }
}

//! @cond NO_DOX
namespace
{
    ALIB_NO_RETURN
    void throwMarkerException( Exceptions eType, String& markedBuffer, integer errPos )
    {
        String64 actText;
        integer excerptPos= 25;
        integer excerptStart= errPos - 25;
        if( excerptStart <= 0 )
        {
            excerptPos+= excerptStart;
            excerptStart= 0;
        }
        else
        {
            actText._( A_CHAR("[...]") );
            excerptPos+= 5;
        }


        actText._( markedBuffer, excerptStart, 50 );
        if( markedBuffer.Length() > excerptStart + 50 )
            actText._( A_CHAR("[...]") );
                     actText.SearchAndReplace( A_CHAR("\r"), A_CHAR("\\r"), excerptPos );
                     actText.SearchAndReplace( A_CHAR("\n"), A_CHAR("\\n"), excerptPos );
        excerptPos+= actText.SearchAndReplace( A_CHAR("\r"), A_CHAR("\\r")             );
        excerptPos+= actText.SearchAndReplace( A_CHAR("\n"), A_CHAR("\\n")             );

        throw Exception( ALIB_CALLER_NULLED, eType, errPos, actText, excerptPos );
    }

}
//! @endcond



void   Paragraphs::AddMarked( Boxes&  args )
{
    character searchCharBuf[2];
              searchCharBuf[0]= MarkerChar;
              searchCharBuf[1]= '\n';
    String searchChars(searchCharBuf, 2);

    Formatter->Acquire(ALIB_CALLER_PRUNED);
    try
    {
        Formatter->FormatArgs( markedBuffer.Reset(), args );
    }
    catch(Exception& )
    {
        Formatter->Release();
        throw;
    }
    Formatter->Release();



    Substring parser       =  markedBuffer;
    integer   lastTextStart=  Buffer.Length();

    while( parser.IsNotEmpty() )
    {
        integer pos= parser.template IndexOfAny<Inclusion::Include, false>( searchChars );

        // not found
        if( pos < 0 )
        {
            Buffer << parser;
            break;
        }

        // new line
        if( parser.CharAt( pos ) == '\n' )
        {
            parser.template ConsumeChars<false, CurrentData::Keep>( pos, Buffer, 1 );
            parser.ConsumeChar( '\r' );
            Buffer << NewLine();
            integer maxLineWidth;
            Paragraphs::Format( Buffer, lastTextStart, LineWidth, JustifyChar,
                                                    maxLineWidth, IndentFirstLine, IndentOtherLines );
            DetectedMaxLineWidth= (std::max)(DetectedMaxLineWidth, maxLineWidth );
            lastTextStart=  Buffer.Length();
            continue;
        }

        parser.template ConsumeChars<false, CurrentData::Keep>( pos, Buffer, 1 );

        // double marker: insert one symbol
        if( parser.ConsumeChar( MarkerChar ) )
            Buffer << MarkerChar;

        // Indent
        else if( parser.ConsumeString(A_CHAR( ">'" )) )
            PushIndent(  parser.ConsumeToken( '\'' ) );

        else if( parser.ConsumeString(A_CHAR( ">>" )) )
            PushIndent( A_CHAR( "  " ) );

        else if( parser.ConsumeString(A_CHAR( "<<" )) )
        {
            if( IndentSizesFirstLine.empty() )
                throwMarkerException( Exceptions::EndmarkerWithoutStart, markedBuffer,
                                      markedBuffer.Length() - parser.Length() - 3 );
            PopIndent();
        }

        // bullets
        else if( parser.ConsumeString(A_CHAR( "*>" )) )
        {
            if( markerBulletLevel > 0 )
            {
                IndentFirstLine .DeleteEnd( 2 )._( A_CHAR( "  " ) );
                IndentOtherLines.DeleteEnd( 2 )._( A_CHAR( "  " ) );
            }
            IndentFirstLine ._( MarkerBullets[markerBulletLevel % MarkerBullets.size()] )._(' ');
            IndentOtherLines._( "  " );
            ++markerBulletLevel;
        }
        else if( parser.ConsumeString(A_CHAR( "<*" )) )
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
                IndentOtherLines._( A_CHAR( "  " ) );
            }
        }

        else if( parser.ConsumeChar('p' ) || parser.ConsumeChar('P') )
            Buffer << NewLine();


        // horizontal line
        else if( parser.ConsumeString(A_CHAR( "HL" )) )
        {
            Buffer.InsertChars( parser.ConsumeChar() , LineWidth - IndentFirstLine.Length() )
                ._( NewLine() );
        }

        // not recognized
        else
        {
            throwMarkerException( Exceptions::UnknownMarker, markedBuffer,
                                  markedBuffer.Length() - parser.Length() - 1 );
        }
    }

    if( lastTextStart < Buffer.Length() )
    {
        integer maxLineWidth;
        Paragraphs::Format( Buffer, lastTextStart, LineWidth, JustifyChar, maxLineWidth,
                                                   IndentFirstLine, IndentOtherLines );
        DetectedMaxLineWidth= (std::max)(DetectedMaxLineWidth, maxLineWidth );
    }

    if ( Buffer.IsNotEmpty() && !Buffer.EndsWith( NewLine() ) )
        Buffer << NewLine();
}


// #################################################################################################
// The static formatter method
// #################################################################################################
void  Paragraphs::Format( AString&      text,
                          integer       startIdx,        integer  lineWidth,
                          character     justifyChar,     integer& maxLineWidth,
                          const String& pIndentFirstLine,
                          const String& pIndentOtherLines                         )
{
    maxLineWidth= 0;
    String indentFirstLines= pIndentFirstLine .IsNotNull()  ? pIndentFirstLine  : EmptyString();
    String indentOtherLines= pIndentOtherLines.IsNotNull()  ? pIndentOtherLines : pIndentFirstLine;

    bool isFirstLine= true;

    String indent             = nullptr;
    bool   indentAreJustSpaces= false;

    // loop over lines
    integer maxLineWidthDetectionStartIdx= startIdx;
    bool    hasNL= false;
    for(;;)
    {
        maxLineWidth= (std::max)( maxLineWidth, startIdx - maxLineWidthDetectionStartIdx
                      - ( !hasNL ? 0 :
                                        #if defined( _WIN32 )
                                          2
                                        #else
                                          1
                                        #endif
                        )
                      );
        if ( startIdx == text.Length() )
            break;
        maxLineWidthDetectionStartIdx= startIdx;
        hasNL= false;

        // skip lines beginning with newline characters, unless indent has non-space characters
        int isWinNL=  text[ startIdx ] == '\r' ? 1 : 0;
        if ( text[ startIdx + isWinNL ] == '\n' )
        {
            hasNL= true;

            // set indent and check if its just spaces
            if( indent.IsNull() )
            {
                indent = isFirstLine ? indentFirstLines : indentOtherLines;
                indentAreJustSpaces= (indent.template IndexOfAny<Inclusion::Exclude>( A_CHAR( " " ) ) < 0 );
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
            indentAreJustSpaces= (indent.template IndexOfAny<Inclusion::Exclude>( A_CHAR( " " ) ) < 0 );
        }
        text.InsertAt( indent, startIdx );

        if( isFirstLine )
        {
            isFirstLine= false;
            indent=      nullptr;
        }


        // find next end of line. Remember last space in line
        integer lastSpaceInLine = 0;
        bool    isLastLine      = true;
        bool    exceeds         = false;
        integer idx             = startIdx + indent.Length() - 1;
        while (++idx < text.Length() )
        {
            character c= text[idx];
            if ( c == '\n' )
            {
                hasNL= true;
                ++idx;
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
                ++idx;
            }
        #else
            if( text[idx-1] == '\n' && text[idx-2] == '\r' )
            {
                text.template Delete<false>((idx-2), 1);
                --idx;
            }
        #endif

        // wrap line.
        if( exceeds && ( lastSpaceInLine || !isLastLine ) )
        {
            integer wrapPos= lastSpaceInLine > 0 ? lastSpaceInLine : idx;
            text.template ReplaceSubstring<false>( NewLine(), wrapPos, 1 );
            idx= wrapPos + NewLine().Length();
            hasNL= true;

            // block justification
            if( justifyChar != '\0' )
            {
                integer qtyInserts= lineWidth - (wrapPos - startIdx );
                if( qtyInserts > 0 )
                {
                    // search first non-space after indent.
                    integer leftInsertBoundary= startIdx + indent.Length();
                    while ( leftInsertBoundary < idx && text[leftInsertBoundary] == ' ' )
                        ++leftInsertBoundary;

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
                                ++idx;
                                --qtyInserts;
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
}


}}} // namespace [aworx::lib::text]

