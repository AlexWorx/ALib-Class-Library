//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "ALib.Monomem.StdContainers.H"
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Format.Paragraphs;
    import   ALib.Lang;
    import   ALib.Strings;
    import   ALib.Exceptions;
#   if ALIB_CAMP
      import ALib.Camp.Base;
#   endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.Exceptions.H"
#   include "ALib.Format.Paragraphs.H"
#   include "ALib.Camp.Base.H"
#endif
//========================================== Implementation ========================================
using namespace alib::strings;
namespace alib::format {


//##################################################################################################
// Non-static methods (if used with instance).
//##################################################################################################
Paragraphs::Paragraphs()
: allocator             (ALIB_DBG("Paragraphs",) 16)
, Buffer                (text)
, Formatter             ( Formatter::Default  )
, MarkerBullets         (allocator)
, IndentFirstLine       (allocator)
, IndentOtherLines      (allocator)
, IndentSizesFirstLine  (StdDequeMA<integer>(allocator))
, IndentSizesOtherLines (StdDequeMA<integer>(allocator))
, boxes                 (allocator) {
    text.SetBuffer(2048);
    MarkerBullets= {'*', '-', '*', '-', '*', '-' };
    IndentFirstLine .SetBuffer(20);
    IndentOtherLines.SetBuffer(20);
}

Paragraphs::Paragraphs(AString& externalBuffer)
: allocator(ALIB_DBG("Paragraphs",) 16)
, Buffer(externalBuffer)
, Formatter( Formatter::Default  )
, MarkerBullets (allocator)
, IndentFirstLine       (allocator)
, IndentOtherLines      (allocator)
, IndentSizesFirstLine  (StdDequeMA<integer>(allocator))
, IndentSizesOtherLines (StdDequeMA<integer>(allocator))
, boxes(allocator) {
    MarkerBullets= {'*', '-', '*', '-', '*', '-' };
    IndentFirstLine .SetBuffer(20);
    IndentOtherLines.SetBuffer(20);
}



Paragraphs& Paragraphs::PushIndent( const String& indentFirstLine,
                                    const String& pIndentOtherLines ) {
    String indentOtherLines= pIndentOtherLines.IsNull()  ? indentFirstLine
                                                         : pIndentOtherLines;

    IndentFirstLine ._( indentFirstLine );
    IndentOtherLines._( indentOtherLines );
    IndentSizesFirstLine .push( indentFirstLine.Length()  );
    
    IndentSizesOtherLines.push( indentOtherLines.Length() );
    return *this;
}


Paragraphs& Paragraphs::PushIndent( uinteger qty, character fillChar ) {
    IndentFirstLine .InsertChars( fillChar, integer( qty ) );
    IndentOtherLines.InsertChars( fillChar, integer( qty ) );
    IndentSizesFirstLine .push( integer( qty ) );
    IndentSizesOtherLines.push( integer( qty ) );
    return *this;
}


Paragraphs& Paragraphs::PopIndent() {
    ALIB_ASSERT_ERROR( !IndentSizesFirstLine.empty(),  "FORMAT",
                                                       "Paragraphs: PopIndent without prior push." )
    IndentFirstLine.DeleteEnd( integer( IndentSizesFirstLine.top() ) );
    IndentSizesFirstLine.pop();

    ALIB_ASSERT_ERROR( !IndentSizesOtherLines.empty(), "FORMAT",
                                                       "Paragraphs: PopIndent without prior push." )
    IndentOtherLines.DeleteEnd( integer( IndentSizesOtherLines.top() ) );
    IndentSizesOtherLines.pop();
    return *this;
}


Paragraphs&     Paragraphs::Clear() {
    Buffer.Reset();
    while( IndentSizesFirstLine .size() ) IndentSizesFirstLine .pop();
    while( IndentSizesOtherLines.size() ) IndentSizesOtherLines.pop();
    IndentFirstLine .Reset();
    IndentOtherLines.Reset();
    markerBulletLevel= 0;
    DetectedMaxLineWidth= 0;
    return *this;
}


//! @cond NO_DOX

template<>
void   Paragraphs::Add( boxing::TBoxes<MonoAllocator>&  args ) {
    integer startIdx= Buffer.Length();
    Formatter->FormatArgs( Buffer, args ); // may throw!

    integer maxLineWidth;
    Paragraphs::Format( Buffer, startIdx, LineWidth, JustifyChar, maxLineWidth, IndentFirstLine, IndentOtherLines );
    DetectedMaxLineWidth= (std::max)(DetectedMaxLineWidth, maxLineWidth );

    if ( Buffer.IsNotEmpty() && !Buffer.EndsWith( NEW_LINE ) ) {
        #if defined( _WIN32 )
            if( Buffer.CharAtEnd() == '\n' )
                Buffer.DeleteEnd(1);
        #endif
        Buffer.NewLine();
}   }

#   include "ALib.Lang.CIFunctions.H"
namespace {
    [[ noreturn ]]
void throwMarkerException( FMTExceptions eType, String& markedBuffer, integer errPos ) {
    String64 actText;
    integer exceptPos= 25;
    integer exceptStart= errPos - 25;
    if( exceptStart <= 0 ) {
        exceptPos+= exceptStart;
        exceptStart= 0;
    } else {
        actText._( A_CHAR("[...]") );
        exceptPos+= 5;
    }


    actText._( markedBuffer, exceptStart, 50 );
    if( markedBuffer.Length() > exceptStart + 50 )
        actText._( A_CHAR("[...]") );
                actText.SearchAndReplace( A_CHAR("\r"), A_CHAR("\\r"), exceptPos );
                actText.SearchAndReplace( A_CHAR("\n"), A_CHAR("\\n"), exceptPos );
    exceptPos+= actText.SearchAndReplace( A_CHAR( "\r"), A_CHAR( "\\r")             );
    exceptPos+= actText.SearchAndReplace( A_CHAR( "\n"), A_CHAR( "\\n")             );

    throw Exception( ALIB_CALLER_NULLED, eType, errPos, actText, exceptPos );
}
}
#   include "ALib.Lang.CIMethods.H"
    
template<> void   Paragraphs::AddMarked( boxing::TBoxes<MonoAllocator>&  args ) {
    character searchCharBuf[2];
              searchCharBuf[0]= MarkerChar;
              searchCharBuf[1]= '\n';
    String searchChars(searchCharBuf, 2);

    Formatter->FormatArgs( markedBuffer.Reset(), args ); // may throw

    Substring parser       =  markedBuffer;
    integer   lastTextStart=  Buffer.Length();

    while( parser.IsNotEmpty() ) {
        integer pos= parser.template IndexOfAny<lang::Inclusion::Include, NC>( searchChars );

        // not found
        if( pos < 0 ) {
            Buffer << parser;
            break;
        }

        // new line
        if( parser.CharAt( pos ) == '\n' ) {
            parser.template ConsumeChars<NC, lang::CurrentData::Keep>( pos, Buffer, 1 );
            if (Buffer.CharAtEnd<NC>() == '\r')
                Buffer.DeleteEnd<NC>(1);
            Buffer.NewLine();
            integer maxLineWidth;
            Paragraphs::Format( Buffer, lastTextStart, LineWidth, JustifyChar,
                                                    maxLineWidth, IndentFirstLine, IndentOtherLines );
            DetectedMaxLineWidth= (std::max)(DetectedMaxLineWidth, maxLineWidth );
            lastTextStart=  Buffer.Length();
            continue;
        }

        parser.template ConsumeChars<NC, lang::CurrentData::Keep>( pos, Buffer, 1 );

        // double marker: insert one symbol
        if( parser.ConsumeChar( MarkerChar ) )
            Buffer << MarkerChar;

        // Indent
        else if( parser.ConsumeString(A_CHAR( ">'" )) )
            PushIndent(  parser.ConsumeToken( '\'' ) );

        else if( parser.ConsumeString(A_CHAR( ">>" )) )
            PushIndent( A_CHAR( "  " ) );

        else if( parser.ConsumeString(A_CHAR( "<<" )) ) {
            if( IndentSizesFirstLine.empty() )
                throwMarkerException( FMTExceptions::EndmarkerWithoutStart, markedBuffer,
                                      markedBuffer.Length() - parser.Length() - 3 );
            PopIndent();
        }

        // bullets
        else if( parser.ConsumeString(A_CHAR( "*>" )) ) {
            if( markerBulletLevel > 0 ) {
                IndentFirstLine .DeleteEnd( 2 )._( A_CHAR( "  " ) );
                IndentOtherLines.DeleteEnd( 2 )._( A_CHAR( "  " ) );
            }
            IndentFirstLine ._( MarkerBullets[markerBulletLevel % MarkerBullets.size()] )._(' ');
            IndentOtherLines._( "  " );
            ++markerBulletLevel;
        }
        else if( parser.ConsumeString(A_CHAR( "<*" )) ) {
            if( markerBulletLevel == 0 )
                throwMarkerException( FMTExceptions::EndmarkerWithoutStart, markedBuffer,
                                      markedBuffer.Length() - parser.Length() - 3 );

            int deIndentCnt= markerBulletLevel > 1 ? 4 : 2;
            IndentFirstLine .DeleteEnd( deIndentCnt );
            IndentOtherLines.DeleteEnd( deIndentCnt );
            if( --markerBulletLevel > 0 ) {
                IndentFirstLine ._( MarkerBullets[markerBulletLevel - 1] )._(' ');
                IndentOtherLines._( A_CHAR( "  " ) );
        }   }

        else if( parser.ConsumeChar('p' ) || parser.ConsumeChar('P') )
            Buffer.NewLine();


        // horizontal line
        else if( parser.ConsumeString(A_CHAR( "HL" )) ) {
            Buffer.InsertChars( parser.ConsumeChar() , LineWidth - IndentFirstLine.Length() )
                  .NewLine();
        }

        // not recognized
        else {
            throwMarkerException( FMTExceptions::UnknownMarker, markedBuffer,
                                  markedBuffer.Length() - parser.Length() - 1 );
    }   }

    if( lastTextStart < Buffer.Length() ) {
        integer maxLineWidth;
        Paragraphs::Format( Buffer, lastTextStart, LineWidth, JustifyChar, maxLineWidth,
                                                   IndentFirstLine, IndentOtherLines );
        DetectedMaxLineWidth= (std::max)(DetectedMaxLineWidth, maxLineWidth );
    }

    if ( Buffer.IsNotEmpty() && !Buffer.EndsWith( NEW_LINE ) )
        Buffer.NewLine();
}

//! @endcond

//##################################################################################################
// The static formatter method
//##################################################################################################
void  Paragraphs::Format( AString&      text,
                          integer       startIdx,        integer  lineWidth,
                          character     justifyChar,     integer& maxLineWidth,
                          const String& pIndentFirstLine,
                          const String& pIndentOtherLines                         ) {
    maxLineWidth= 0;
    String indentFirstLines= pIndentFirstLine .IsNotNull()  ? pIndentFirstLine  : EMPTY_STRING;
    String indentOtherLines= pIndentOtherLines.IsNotNull()  ? pIndentOtherLines : pIndentFirstLine;

    bool isFirstLine= true;

    String indent             = nullptr;
    bool   indentAreJustSpaces= false;

    // loop over lines
    integer maxLineWidthDetectionStartIdx= startIdx;
    bool    hasNL= false;
    for(;;) {
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
        if ( text[ startIdx + isWinNL ] == '\n' ) {
            hasNL= true;

            // set indent and check if its just spaces
            if( indent.IsNull() ) {
                indent = isFirstLine ? indentFirstLines : indentOtherLines;
                indentAreJustSpaces= (indent.template IndexOfAny<lang::Inclusion::Exclude>( A_CHAR( " " ) ) < 0 );
            }

            // insert indent if not just spaces
            if ( !indentAreJustSpaces ) {
                text.InsertAt( indent, startIdx );
                startIdx+= indent.Length();
            }

            #if defined( _WIN32 )
                if( !isWinNL ) {
                    text.template InsertChars<NC>('\r', 1, startIdx );
                    isWinNL= true;
                }
            #else
                if( isWinNL ) {
                    text.template Delete<NC>(startIdx, 1);
                    isWinNL= false;
                }
            #endif


            startIdx+= 1 + isWinNL;
            if( isFirstLine ) {
                isFirstLine= false;
                indent=      nullptr;
            }

            continue;
        }

        // insert indent
        if( indent.IsNull() ) {
            indent = isFirstLine ? indentFirstLines : indentOtherLines;
            indentAreJustSpaces= (indent.template IndexOfAny<lang::Inclusion::Exclude>( A_CHAR( " " ) ) < 0 );
        }
        text.InsertAt( indent, startIdx );

        integer idx = startIdx + indent.Length() - 1;

        if( isFirstLine ) {
            isFirstLine= false;
            indent=      nullptr;
        }

        // find next end of line. Remember last space in line
        integer lastSpaceInLine = 0;
        bool    isLastLine      = true;
        bool    exceeds         = false;
        while (++idx < text.Length() ) {
            character c= text[idx];
            if ( c == '\n' ) {
                hasNL= true;
                ++idx;
                break;
            }
            exceeds= lineWidth > 0 && idx - startIdx >= lineWidth;

            if( c == ' ' ) {
                if(idx - startIdx <= lineWidth )
                    lastSpaceInLine= idx;

                if( exceeds ) {
                    isLastLine= false;
                    break;
        }   }   }

        // correct newline
        #if defined( _WIN32 )
            if( text[idx-1] == '\n' && text[idx-2] != '\r' ) {
                text.template InsertChars<NC>('\r', 1, idx-1 );
                ++idx;
            }
        #else
            if( text[idx-1] == '\n' && text[idx-2] == '\r' ) {
                text.template Delete<NC>((idx-2), 1);
                --idx;
            }
        #endif

        // wrap line.
        if( exceeds && ( lastSpaceInLine || !isLastLine ) ) {
            integer wrapPos= lastSpaceInLine > 0 ? lastSpaceInLine : idx;
            text.template ReplaceSubstring<NC>( NEW_LINE, wrapPos, 1 );
            idx= wrapPos + NEW_LINE.Length();
            hasNL= true;

            // block justification
            if( justifyChar != '\0' ) {
                integer qtyInserts= lineWidth - (wrapPos - startIdx );
                if( qtyInserts > 0 ) {
                    // search first non-space after indent.
                    integer leftInsertBoundary= startIdx + indent.Length();
                    while ( leftInsertBoundary < idx && text[leftInsertBoundary] == ' ' )
                        ++leftInsertBoundary;

                    if( leftInsertBoundary < idx ) {
                        while( qtyInserts > 0 ) {
                            integer actPos= idx - 1;
                            bool foundOne= false;
                            while( qtyInserts > 0 ) {
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
        }   }   }   }   }

        startIdx= idx;
}   }


#if !DOXYGEN
template<> void   Paragraphs::Add( boxing::TBoxes<lang::HeapAllocator>&  args ) {
    boxes.clear();
    boxes.Add(args);
    Add(boxes);
}

template<> void   Paragraphs::Add( boxing::TBoxes<PoolAllocator>&  args ) {
    boxes.clear();
    boxes.Add(args);
    Add(boxes);
}
template<> void   Paragraphs::AddMarked( boxing::TBoxes<lang::HeapAllocator>&  args ) {
    boxes.clear();
    boxes.Add(args);
    AddMarked(boxes);
}

template<> void   Paragraphs::AddMarked( boxing::TBoxes<PoolAllocator>&  args ) {
    boxes.clear();
    boxes.Add(args);
    AddMarked(boxes);
}
#endif // !DOXYGEN


} // namespace [alib::format]
