// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_TEXT_FORMATTER_PYTHONSTYLE)
#       include "alib/text/formatterpythonstyle.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#       include "alib/strings/format.hpp"
#   endif
#   if !defined(HPP_ALIB_TEXT_TEXT)
#      include "alib/text/text.hpp"
#   endif
#endif // !defined(ALIB_DOX)


// For code compatibility with ALox Java/C++
// We have to use underscore as the start of the name and for this have to disable a compiler
// warning. But this is a local code (cpp file) anyhow.
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif


#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

using namespace aworx::lib::strings;

namespace aworx { namespace lib { namespace text {


FormatterPythonStyle::FormatterPythonStyle()
: FormatterStdImpl( A_CHAR( "FormatterPythonStyle" ) )
{
    // set number format to python defaults
    DefaultNumberFormat.ForceDecimalPoint       = false;
    DefaultNumberFormat.WriteExponentPlusSign   = true;
}


FormatterStdImpl*   FormatterPythonStyle::Clone()
{
    // create a clone
    FormatterPythonStyle* clone= new FormatterPythonStyle();

    // create a clone of #Next, in the case that next is derived from std base class
    if( Next )
        clone->Next.reset( Next->Clone() );

    // copy my settings, that's it
    clone->CloneSettings( *this );
    return clone;
}


void FormatterPythonStyle::reset()
{
    Sizes.Reset();
}


void FormatterPythonStyle::initializeFormat()
{
    Sizes.Start();
}



void FormatterPythonStyle::resetPlaceholder()
{
    // first invoke parent's setting...
    FormatterStdImpl::resetPlaceholder();

    // ...then make some "python like" adjustments
                             placeholderPS.Conversion                    = nullptr;
                             placeholderPS.ConversionPos                 = -1;

    placeholder.NF.ForceDecimalPoint             = false;
    placeholder.NF.ExponentSeparator             = AlternativeNumberFormat.ExponentSeparator; // choose lower case as default
    placeholder.NF.INFLiteral                    = AlternativeNumberFormat.INFLiteral;        // choose lower case as default
    placeholder.NF.NANLiteral                    = AlternativeNumberFormat.NANLiteral;        // choose lower case as default
    placeholder.NF.OmitTrailingFractionalZeros   = true;

                             placeholderPS.Precision                     = -1;
                             placeholderPS.PrecisionPos                  = -1;
                             placeholderPS.DefaultPrecision              = 6;
}


integer FormatterPythonStyle::findPlaceholder()
{
    integer idx= 0;
    while(     (idx= parser.IndexOf('{', idx )) >= 0
           &&  parser.CharAt( idx + 1 ) == '{'                   )
    {
        // double ESC found (escaped {{)
        idx+= + 2;
    }
    return idx;
}


bool FormatterPythonStyle::parsePlaceholder()
{
    enum states
    {
        POSITION                    = 1,
        CONVERSION                  = 2,
        COLON                       = 3,
        FORMAT_SPEC                 = 4,
        END                         = 10,
    };

    states  state= POSITION;
    #define NEXTSTATE(s) { state= s; continue; }

    while( true )
    {
        // switch over state. With 'break' we consume on character (kind of success) while
        // with 'continue' we keep the current character (and go to another state)
        switch ( state )
        {
            case POSITION:

                if( isdigit( parser.CharAtStart() )  )
                {
                    int argNo;
                    parser.ConsumeDecDigits( argNo );
                    setArgument( argNo );
                }
                NEXTSTATE(CONVERSION)


            case CONVERSION:
                if( parser.CharAtStart() == '!' )
                {
                    placeholderPS.ConversionPos  = static_cast<int>(   formatString.Length()
                                                                     - parser.Length() - 1   );
                    integer endConversion= parser.IndexOfAny<Inclusion::Include>( A_CHAR( ":}" ) );
                    if( endConversion < 0 )
                        throw Exception(ALIB_CALLER_NULLED, Exceptions::MissingClosingBracket,
                                        formatString, placeholderPS.ConversionPos );

                    parser.ConsumeChars( endConversion, &placeholderPS.Conversion );
                }
                NEXTSTATE(COLON)

            case COLON:
                if( parser.CharAtStart() != ':'  )
                {
                    state= END;
                    continue;
                }

                parser.ConsumeChars(1);
                NEXTSTATE(FORMAT_SPEC)

            case FORMAT_SPEC:
            {
                // find end of format spec (allow "\}" in format string)
                integer eoFormatSpec= -1;
                do
                {
                    eoFormatSpec= parser.IndexOf( '}', eoFormatSpec + 1 );
                }
                while(      eoFormatSpec > 0
                        &&  parser.CharAt( eoFormatSpec - 1) == '\\' );

                if ( eoFormatSpec < 0 )
                    throw Exception(ALIB_CALLER_NULLED, Exceptions::MissingClosingBracket,
                                    formatString,
                                    formatString.Length()  );

                // extract format spec to separate sub-string
                parser.ConsumeChars( eoFormatSpec, &(placeholder.FormatSpec) ) ;

                NEXTSTATE(END)
            }

            case END:
                if( parser.CharAtStart() != '}'  )
                    throw Exception( ALIB_CALLER_NULLED, Exceptions::MissingClosingBracket,
                                     formatString,
                                     formatString.Length() - parser.Length() );

                parser.ConsumeChars(1);
                return true;

        } // state switch

    } // read loop

}




bool FormatterPythonStyle::parseStdFormatSpec()
{
    // parse alignment and fill character. This is a bit tricky to shorten the code.
    // The alignment token <,>,^ or = can be on first or second position. If second, then the
    // first was a fill character. Therefore we loop from 0 to 1....
    integer charsToConsume= 1;
    placeholder.AlignmentSpecified= true;
    Substring& formatSpec= placeholder.FormatSpec;


    int cNo= 0;
    do switch ( formatSpec.CharAt( cNo++ ) )
    {
        case '<': placeholder.ValueAlignment= Alignment::Left;    break;
        case '>': placeholder.ValueAlignment= Alignment::Right;   break;
        case '^': placeholder.ValueAlignment= Alignment::Center;  break;
        case '=': placeholder.ValueAlignment= Alignment::Right;
                  placeholder.SignPaddingMode=  true;        break;

        default:
            if(cNo == 1 )
            {
                charsToConsume= 2;
                placeholder.FillChar= formatSpec.CharAtStart();
            }
            else
            {
                charsToConsume= 0;
                placeholder.FillChar= ' ';
                placeholder.AlignmentSpecified= false;
            }
        break;
    }
    while( cNo < 2 && charsToConsume != 1 );
    formatSpec.ConsumeChars( charsToConsume );

    // other things to parse
    character actChar;
    while( (actChar= formatSpec.CharAtStart()) != '\0' )
    {
        // width
        if( isdigit( actChar ) )
        {
            // Python doc says:
            //   When no explicit alignment is given, preceding the width field by a zero ('0') character
            //   enables sign-aware zero-padding for numeric types.
            //   This is equivalent to a fill character of '0' with an alignment type of '='.
            if ( actChar == '0' )
                placeholder.SignPaddingMode= true;
            formatSpec.ConsumeDecDigits( placeholder.Width );

            continue; // not consume below
        }

        // precision. Python doc says:
        // "The precision is a decimal number indicating how many digits should be displayed
        // after the decimal point for a floating point value formatted with 'f' and 'F', or
        // before and after the decimal point for a floating point value formatted with 'g' or 'G'.
        // For non-number types the field indicates the maximum field size - in other words,
        // how many characters will be used from the field content. The precision is not
        // allowed for integral values."
        else if( actChar == '.' )
        {
            placeholderPS.PrecisionPos= static_cast<int>(   formatString.Length()
                                                          - parser.Length()
                                                          - formatSpec.Length() -1                );
            formatSpec.ConsumeChars( 1 );
            if ( ! formatSpec.ConsumeDecDigits( placeholderPS.Precision ) )
                throw Exception(ALIB_CALLER_NULLED, Exceptions::MissingPrecisionValuePS,
                                formatString,
                                  formatString.Length()
                                - parser.Length()
                                - formatSpec.Length() - 1 );
            continue;
        }


        // ----------------------------- Types -------------------------------------
        else if (String(A_CHAR( "sdcboxXeEfFngGhHB%" )).IndexOf( actChar ) >= 0 )
        {
            if ( placeholder.TypeCode != '\0' )
                throw Exception(ALIB_CALLER_NULLED, Exceptions::DuplicateTypeCode,
                                actChar, placeholder.TypeCode,
                                  formatString,
                                  formatString.Length()
                                - parser.Length()
                                - formatSpec.Length() - 1 );

            placeholder.TypeCode= actChar;
            placeholder.TypeCodePosition= static_cast<int>(   formatString.Length()
                                                            - parser.Length()
                                                            - formatSpec.Length() - 1                 );

            if( String(A_CHAR( "EGF" )).IndexOf( actChar ) >= 0 )
            {
                placeholder.NF.ExponentSeparator= DefaultNumberFormat.ExponentSeparator;
                placeholder.NF.INFLiteral=        DefaultNumberFormat.INFLiteral;
                placeholder.NF.NANLiteral=        DefaultNumberFormat.NANLiteral;
            }

            switch( actChar )
            {
                case 's':   placeholder.Type= PHTypes::String;                break;
                case 'd':   placeholder.Type= PHTypes::IntBase10;             break;
                case 'c':   placeholder.Type= PHTypes::Character;             break;
                case 'b':   placeholder.Type= PHTypes::IntBinary;             break;
                case 'o':   placeholder.Type= PHTypes::IntOctal;              break;

                case 'X':   placeholder.NF.HexLowerCase= false;              ALIB_FALLTHROUGH
                case 'x':   placeholder.Type= PHTypes::IntHex;                break;

                case 'H':   placeholder.NF.HexLowerCase= false;              ALIB_FALLTHROUGH
                case 'h':   placeholder.Type= PHTypes::HashCode;              break;
                case 'B':   placeholder.Type= PHTypes::Bool;                  break;

                // float types
                case 'E':
                case 'e':   placeholder.Type= PHTypes::Float;
                            placeholder.NF.ForceScientific= true;            break;

                case '%':   placeholder.IsPercentage= true;                  ALIB_FALLTHROUGH
                case 'f':
                case 'F':   placeholder.NF.OmitTrailingFractionalZeros= false;
                            placeholder.Type= PHTypes::Float;                 break;

                case 'n':   placeholder.NF.DecimalPointChar=   AlternativeNumberFormat.DecimalPointChar;
                            placeholder.NF.ThousandsGroupChar= AlternativeNumberFormat.ThousandsGroupChar;
                                                                   ALIB_FALLTHROUGH
                case 'G':                                          ALIB_FALLTHROUGH
                case 'g':   placeholder.Type= PHTypes::Float;
                            placeholderPS.DefaultPrecision=   -1;
                break;

                default:    ALIB_ERROR( "Unhandled character in choices string above" )
                            return false;

            }//switch
        }


        // others
        else switch( actChar )
        {
            // sign
            case '+':   placeholder.NF.PlusSign= '+' ;  break;
            case '-':   placeholder.NF.PlusSign= '\0';  break;
            case ' ':   placeholder.NF.PlusSign= ' ' ;  break;

            // alternate version ('#')
            case '#':   placeholder.WriteBinOctHexPrefix=             true;
                        placeholder.NF.ForceDecimalPoint=             true;
                        placeholder.NF.OmitTrailingFractionalZeros=   false;
            break;

            // Enable grouping
            case ',':   placeholder.NF.WriteGroupChars= true;
                        break;


            default:
                throw Exception( ALIB_CALLER_NULLED, Exceptions::UnknownTypeCode,
                                 actChar,
                                   formatString,
                                   formatString.Length()
                                 - parser.Length()
                                 - formatSpec.Length() - 1 );
        }

        formatSpec.ConsumeChars(1);
    }

    return true;
}


void    FormatterPythonStyle::replaceEscapeSequences( integer startIdx )
{
    targetString->SearchAndReplace( A_CHAR( "{{" ), A_CHAR( "{" ), startIdx );
    targetString->SearchAndReplace( A_CHAR( "}}" ), A_CHAR( "}" ), startIdx );
    targetString->_<false>( strings::TFormat<character>::Escape( Switch::Off, startIdx ) );

    // search the last newline character in the just written portion of the target string.
    // If one is found, reset auto sizes and actual start of string.
    integer lastNewLine=-1;
    integer actNewLine = startIdx - 1;
    while( (actNewLine= targetString->IndexOf('\n', actNewLine + 1)) > 0 )
        lastNewLine= actNewLine;
    if( lastNewLine >= 0 )
    {
        targetStringStartLength= lastNewLine + 1;
        Sizes.Start();
    }
}


bool    FormatterPythonStyle::preAndPostProcess( integer startIdx, AString* target )
{
    bool isPreProcess=  startIdx < 0;
    bool isPostProcess= startIdx>= 0 && target == nullptr;
    Substring conversion= placeholderPS.Conversion;
    ++placeholderPS.ConversionPos;

    while( conversion.IsNotEmpty() )
    {
        if( !conversion.ConsumeChar('!') )
            throw Exception(ALIB_CALLER_NULLED, Exceptions::ExclamationMarkExpected,
                            formatString,
                              placeholderPS.ConversionPos
                            + placeholderPS.Conversion.Length()
                            - conversion.Length() );

             if(  conversion.ConsumePartOf( A_CHAR( "Xtinguish" ) ) > 0 )   { return false;                                                         }
             if(  conversion.ConsumePartOf( A_CHAR( "Upper"     ) ) > 0 )   { if (isPostProcess) targetString->ToUpper( startIdx );                 }
        else if(  conversion.ConsumePartOf( A_CHAR( "Lower"     ) ) > 0 )   { if (isPostProcess) targetString->ToLower( startIdx );                 }
        else if(  conversion.ConsumePartOf( A_CHAR( "str"       ) ) > 0
                ||conversion.ConsumePartOf( A_CHAR( "Quote"     ) ) > 0 )
        {
            String8 open = A_CHAR( '"' );
            String8 close= A_CHAR( '"' );
            if (conversion.IsNotEmpty() && conversion.CharAtStart() != A_CHAR('!'))
            {
                open  .Reset( conversion.ConsumeChar() );
                close.Reset(  conversion.IsNotEmpty() && conversion.CharAtStart() != A_CHAR('!')
                            ? conversion.ConsumeChar()
                            : open.CharAtStart() );
            }

            if (isPostProcess)
                targetString->InsertAt<false>( open, startIdx ) .Append  <false>(  close );
        }

        else if(  conversion.ConsumePartOf( A_CHAR( "Fill"      ) ) > 0 )
        {
            placeholder.Type= PHTypes::Fill;
            placeholder.FillChar=  conversion.ConsumeChar<Case::Ignore>('C' ) && conversion.Length() > 0
                                                   ? conversion.ConsumeChar<false>()
                                                   : ' ';

        }

        else if(  conversion.ConsumePartOf( A_CHAR( "Tab"       ) ) > 0 )
        {
            character tabChar= conversion.ConsumeChar<Case::Ignore>('C') && conversion.Length() > 0
                                ? conversion.ConsumeChar<false>()
                                : ' ';
            int tabSize;
            if( !conversion.ConsumeDecDigits<int>( tabSize ) )
                tabSize= 8;

            if( isPreProcess )
                targetString->_<false>( strings::TFormat<character>::Tab( tabSize, -1, 1, tabChar ) );

        }

        else if( conversion.ConsumePartOf( A_CHAR( "ATab" ), 2 ) > 0 )
        {

            if( conversion.ConsumePartOf(  A_CHAR( "Reset")    ) > 0 )
            {
                if( isPreProcess )
                    Sizes.Reset();
            }
            else
            {
                character tabChar= conversion.ConsumeChar<Case::Ignore>('C' ) && conversion.Length() > 0
                                ? conversion.ConsumeChar<false>()
                                : ' ';

                int growth;
                if( !conversion.ConsumeDecDigits<int>( growth ) )
                    growth= 3;

                if( isPreProcess )
                {
                    integer actPos= targetString->Length() - targetStringStartLength;
                    integer tabStop= Sizes.Next( AutoSizes::Types::Tabstop, actPos , growth );
                    targetString->InsertChars<false>( tabChar, tabStop - actPos );
                }
            }
        }

        else if( conversion.ConsumePartOf( A_CHAR( "AWidth" ), 2 ) > 0 )
        {
            if( conversion.ConsumePartOf(  A_CHAR( "Reset"  )    ) > 0 )
            {
                if( isPreProcess )
                    Sizes.Reset();
            }
            else
            {
                int extraPadding;
                conversion.ConsumeDecDigits<int>( extraPadding );

                if( isPreProcess )
                    placeholder.Width= static_cast<int>( Sizes.Actual( AutoSizes::Types::Field, 0, extraPadding ) );
                else if ( isPostProcess )
                    Sizes.Next( AutoSizes::Types::Field, targetString->Length() - startIdx, extraPadding );
            }
        }

        else if(    conversion.ConsumePartOf( A_CHAR( "Esc" ) ) > 0
                 || conversion.ConsumePartOf( A_CHAR( "A"   ) ) > 0 )
        {
            Switch toESC= Switch::On;
            conversion.ConsumeChar('<');
            if(conversion.ConsumeChar('>') )
                toESC=  Switch::Off;

            if( isPostProcess )
                targetString->_<false>( typename strings::TFormat<character>::Escape( toESC, startIdx ) );
        }

        else if( conversion.ConsumePartOf( A_CHAR( "Replace" ), 2 ) > 0 )
        {
            String search = conversion.ConsumeField( '<', '>' );
            String replace= conversion.ConsumeField( '<', '>' );
            if( search.IsNull() || replace.IsNull() )
                throw Exception( ALIB_CALLER_NULLED, Exceptions::MissingReplacementStrings,
                                 formatString,
                                   placeholderPS.ConversionPos
                                 + placeholderPS.Conversion.Length()
                                 - conversion.Length() );

            if( target != nullptr )
            {
                // special case: fill empty fields
                if( search.IsEmpty() && target->Length() - startIdx == 0 )
                {
                    *target << replace;
                }
                else
                    target->SearchAndReplace( search, replace, startIdx );
            }
        }

        // error (not recognized)
        else
            throw Exception( ALIB_CALLER_NULLED, Exceptions::UnknownConversionPS,
                             conversion,
                             formatString,
                               placeholderPS.ConversionPos
                             + placeholderPS.Conversion.Length()
                             - conversion.Length() );
    }

    return true;
}


bool  FormatterPythonStyle::checkStdFieldAgainstArgument()
{
    bool wasFloat= placeholder.Type == PHTypes::Float;
    if( wasFloat )
    {
             if ( placeholderPS.Precision >= 0 )
            placeholder.NF.FractionalPartWidth= static_cast<int8_t>( placeholderPS.Precision );
        else if( placeholder.NF.FractionalPartWidth < 0 )
            placeholder.NF.FractionalPartWidth= static_cast<int8_t>( placeholderPS.DefaultPrecision);
    }

    bool result= FormatterStdImpl::checkStdFieldAgainstArgument();

    if( !wasFloat && placeholder.Type == PHTypes::Float )
    {
        if ( placeholderPS.Precision >= 0 )
            placeholder.NF.FractionalPartWidth= static_cast<int8_t>( placeholderPS.Precision );
    }

    if(   placeholder.Type == PHTypes::String
       || placeholder.Type == PHTypes::Bool  )
        placeholder.CutContent= placeholderPS.Precision;
    else if (   placeholderPS.Precision >= 0
             && placeholder.Type != PHTypes::Float )
        throw Exception(ALIB_CALLER_NULLED, Exceptions::PrecisionSpecificationWithInteger,
                        formatString, placeholderPS.PrecisionPos );
    return result;

}


}}} // namespace [aworx::lib::text]
