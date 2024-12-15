// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/lang/format/formatterpythonstyle.hpp"
#   include "alib/lang/format/fmtexceptions.hpp"
#endif // !DOXYGEN


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

using namespace alib::strings;

namespace alib::lang::format {


FormatterPythonStyle::FormatterPythonStyle()
: FormatterStdImpl( A_CHAR( "FormatterPythonStyle" ) )
, Sizes(&SizesDefaultInstance)
{
    // set number format to python defaults
    DefaultNumberFormat.Flags-=  NumberFormatFlags::ForceDecimalPoint;
    DefaultNumberFormat.Flags+=  NumberFormatFlags::WriteExponentPlusSign;
}


SPFormatter   FormatterPythonStyle::Clone()
{
    // create a clone
    SPFormatter clone;
    clone.InsertDerived<FormatterPythonStyle>();

    // create a clone of #Next, in the case that next is derived from std base class
    if( Next )
        clone->Next= Next->Clone();

    // copy my settings, that's it
    clone->CloneSettings( *this );
    return clone;
}

void FormatterPythonStyle::resetPlaceholder()
{
    // first invoke parent's setting...
    FormatterStdImpl::resetPlaceholder();

    // ...then make some "python like" adjustments
                             placeholderPS.Conversion                    = nullptr;
                             placeholderPS.ConversionPos                 = -1;

    placeholder.NF.Flags                        -= NumberFormatFlags::ForceDecimalPoint;
    placeholder.NF.ExponentSeparator             = AlternativeNumberFormat.ExponentSeparator; // choose lower case as default
    placeholder.NF.INFLiteral                    = AlternativeNumberFormat.INFLiteral;        // choose lower case as default
    placeholder.NF.NANLiteral                    = AlternativeNumberFormat.NANLiteral;        // choose lower case as default
    placeholder.NF.Flags                        += NumberFormatFlags::OmitTrailingFractionalZeros;

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
    // read position
    if( isdigit( parser.CharAtStart() )  )
    {
        int argNo;
        parser.ConsumeDecDigits( argNo );
        setArgument( argNo );
    }

    // read conversion
    if( parser.CharAtStart() == '!' )
    {
        placeholderPS.ConversionPos  = static_cast<int>(   formatString.Length()
                                                         - parser.Length() - 1   );
        integer endConversion= parser.IndexOfAny<lang::Inclusion::Include>( A_CHAR( ":}" ) );
        if( endConversion < 0 )
            throw Exception(ALIB_CALLER_NULLED, FMTExceptions::MissingClosingBracket,
                            formatString, placeholderPS.ConversionPos );

        parser.ConsumeChars( endConversion, &placeholderPS.Conversion );
    }

    // read colon and format spec
    if( parser.CharAtStart() == ':'  )
    {
        parser.ConsumeChars(1);

        // find end of format spec: allow "\}", "\{" and nested "{..}" in format string
        Substring subParser= parser;
        int depth= 0;
        while( subParser.IsNotEmpty() )
        {
            if( subParser.CharAtStart() == '}' )
            {
                if(depth == 0 )
                    break;

                --depth;
                subParser.ConsumeChars<NC>(1);
                continue;
            }

            if( subParser.CharAtStart() == '\\' )   { subParser.ConsumeChars(2); continue; }
            if( subParser.CharAtStart() == '{' )    ++depth;
            subParser.ConsumeChars<NC>(1);
        }

        if ( subParser.IsEmpty() )
            throw Exception(ALIB_CALLER_NULLED, FMTExceptions::MissingClosingBracket,
                            formatString,
                            formatString.Length()  );

        // extract format spec to separate substring
        parser.ConsumeChars( parser.Length() - subParser.Length(), &(placeholder.FormatSpec) ) ;
    }

    // check for closing brace
    if( parser.CharAtStart() != '}'  )
        throw Exception( ALIB_CALLER_NULLED, FMTExceptions::MissingClosingBracket,
                         formatString,
                         formatString.Length() - parser.Length() );

    parser.ConsumeChars(1);
    return true;
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
        case '<': placeholder.ValueAlignment= lang::Alignment::Left;    break;
        case '>': placeholder.ValueAlignment= lang::Alignment::Right;   break;
        case '^': placeholder.ValueAlignment= lang::Alignment::Center;  break;
        case '=': placeholder.ValueAlignment= lang::Alignment::Right;
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
                throw Exception(ALIB_CALLER_NULLED, FMTExceptions::MissingPrecisionValuePS,
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
                throw Exception( ALIB_CALLER_NULLED, FMTExceptions::DuplicateTypeCode,
                                 actChar, placeholder.TypeCode,
                                 placeholder.Arg->TypeID(),
                                 formatString,
                                 formatString.Length() - parser.Length()
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

                case 'X':   placeholder.NF.Flags-= NumberFormatFlags::HexLowerCase; ALIB_FALLTHROUGH
                case 'x':   placeholder.Type= PHTypes::IntHex;                break;

                case 'H':   placeholder.NF.Flags-= NumberFormatFlags::HexLowerCase; ALIB_FALLTHROUGH
                case 'h':   placeholder.Type= PHTypes::HashCode;              break;
                case 'B':   placeholder.Type= PHTypes::Bool;                  break;

                // float types
                case 'E':
                case 'e':   placeholder.Type= PHTypes::Float;
                            placeholder.NF.Flags+= NumberFormatFlags::ForceScientific; break;

                case '%':   placeholder.IsPercentage= true;                  ALIB_FALLTHROUGH
                case 'f':
                case 'F':   placeholder.NF.Flags-= NumberFormatFlags::OmitTrailingFractionalZeros;
                            placeholder.Type= PHTypes::Float;                 break;

                case 'n':   placeholder.NF.DecimalPointChar=   AlternativeNumberFormat.DecimalPointChar;
                            placeholder.NF.ThousandsGroupChar= AlternativeNumberFormat.ThousandsGroupChar;
                                                                   ALIB_FALLTHROUGH
                case 'G':                                          ALIB_FALLTHROUGH
                case 'g':   placeholder.Type= PHTypes::Float;
                            placeholderPS.DefaultPrecision=   -1;
                break;

                default:    ALIB_ERROR( "FMT", "Unhandled character in choices string above" )
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
                        placeholder.NF.Flags+= NumberFormatFlags::ForceDecimalPoint;
                        placeholder.NF.Flags-= NumberFormatFlags::OmitTrailingFractionalZeros;
            break;

            // Enable grouping
            case ',':   placeholder.NF.Flags+= NumberFormatFlags::WriteGroupChars;
                        break;


            default:
                throw Exception( ALIB_CALLER_NULLED, FMTExceptions::UnknownTypeCode,
                                 actChar,
                                   formatString,
                                   formatString.Length()
                                 - parser.Length()
                                 - formatSpec.Length() - 1,
                                 placeholder.Arg->TypeID()
                                 );
        }

        formatSpec.ConsumeChars(1);
    }

    return true;
}


void    FormatterPythonStyle::writeStringPortion( integer length )
{
    if( length == 0)
        return;

    targetString->EnsureRemainingCapacity( length );
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    auto* src = parser.Buffer();
    auto* dest= targetString->VBuffer() + targetString->Length();
    parser.ConsumeChars( length );

    character c1;
    character c2= *src;
    while( length > 1 )
    {
        c1= c2;
        c2= *++src;

        if(  c1 == '\n' ) // reset auto-sizes when act ual character is '\n'
            Sizes->Restart();

        else if(     ( c1 == '{' && c2 =='{')
                 ||  ( c1 == '}' && c2 =='}')
                 ||    c1 == '\\'                     )
        {
            if(  c1 == '\\' )
                switch(c2)
                {
                    case 'r': c1= '\r' ; break;
                    case 'n': c1= '\n' ;
                              // reset auto-sizes with \n
                              targetStringStartLength=  dest - targetString->VBuffer() + 1;
                              Sizes->Restart();
                              break;
                    case 't': c1= '\t' ; break;
                    case 'a': c1= '\a' ; break;
                    case 'b': c1= '\b' ; break;
                    case 'v': c1= '\v' ; break;
                    case 'f': c1= '\f' ; break;
                    case '"': c1= '"'  ; break;
                    default:  c1= '?'  ; break;
                }

            c2= *++src;
            --length;
        }

        *dest++= c1;
        --length;
    }
    ALIB_WARNINGS_RESTORE

    // copy last character and adjust target string length:
    // Note: length usually is 1. Only if last character is an escape sequence, it is 0.
    if( length == 1)
    {
        *dest= *src;
        // reset auto-sizes when last character is '\n'
        if(  *dest == '\n' )
            Sizes->Restart();
    }
    targetString->SetLength( dest - targetString->VBuffer() + length);
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
            throw Exception(ALIB_CALLER_NULLED, FMTExceptions::ExclamationMarkExpected,
                            placeholder.Arg->TypeID(),
                            formatString,  placeholderPS.ConversionPos
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
                targetString->InsertAt<NC>( open, startIdx ) .Append<NC>(  close );
        }

        else if(  conversion.ConsumePartOf( A_CHAR( "Fill"      ) ) > 0 )
        {
            placeholder.Type= PHTypes::Fill;
            placeholder.FillChar=  conversion.ConsumeChar<lang::Case::Ignore>('C' ) && conversion.Length() > 0
                                                   ? conversion.ConsumeChar<NC>()
                                                   : ' ';

        }

        else if(  conversion.ConsumePartOf( A_CHAR( "Tab"       ) ) > 0 )
        {
            character tabChar= conversion.ConsumeChar<lang::Case::Ignore>('C') && conversion.Length() > 0
                                ? conversion.ConsumeChar<NC>()
                                : ' ';
            int tabSize;
            if( !conversion.ConsumeDecDigits<int>( tabSize ) )
                tabSize= 8;

            if( isPreProcess )
                targetString->_<NC>( strings::TFormat<character>::Tab( tabSize, -1, 1, tabChar ) );

        }

        else if( conversion.ConsumePartOf( A_CHAR( "ATab" ), 2 ) > 0 )
        {

            if( conversion.ConsumePartOf(  A_CHAR( "Reset")    ) > 0 )
            {
                if( isPreProcess )
                    Sizes->Reset();
            }
            else
            {
                character tabChar= conversion.ConsumeChar<lang::Case::Ignore>('C' ) && conversion.Length() > 0
                                ? conversion.ConsumeChar<NC>()
                                : ' ';

                int growth;
                if( !conversion.ConsumeDecDigits<int>( growth ) )
                    growth= 3;

                if( isPreProcess )
                {
                    integer actPos= targetString->Length() - targetStringStartLength;
                    integer tabStop= Sizes->Next( AutoSizes::Types::Tabstop, actPos , growth );
                    targetString->InsertChars<NC>( tabChar, tabStop - actPos );
                }
            }
        }

        else if( conversion.ConsumePartOf( A_CHAR( "AWidth" ), 2 ) > 0 )
        {
            if( conversion.ConsumePartOf(  A_CHAR( "Reset"  )    ) > 0 )
            {
                if( isPreProcess )
                    Sizes->Reset();
            }
            else
            {
                int extraPadding;
                conversion.ConsumeDecDigits<int>( extraPadding );

                if( isPreProcess )
                    placeholder.Width= int( Sizes->Actual( AutoSizes::Types::Field, 0, extraPadding ) );
                else if ( isPostProcess )
                    Sizes->Next( AutoSizes::Types::Field, targetString->Length() - startIdx, extraPadding );
            }
        }

        else if(    conversion.ConsumePartOf( A_CHAR( "Esc" ) ) > 0
                 || conversion.ConsumePartOf( A_CHAR( "A"   ) ) > 0 )
        {
            lang::Switch toESC= lang::Switch::On;
            conversion.ConsumeChar('<');
            if(conversion.ConsumeChar('>') )
                toESC=  lang::Switch::Off;

            if( isPostProcess )
                targetString->_<NC>( typename strings::TFormat<character>::Escape( toESC, startIdx ) );
        }

        else if( conversion.ConsumePartOf( A_CHAR( "Replace" ), 2 ) > 0 )
        {
            String search = conversion.ConsumeField( '<', '>' );
            String replace= conversion.ConsumeField( '<', '>' );
            if( search.IsNull() || replace.IsNull() )
                throw Exception( ALIB_CALLER_NULLED, FMTExceptions::MissingReplacementStrings,
                                 placeholder.Arg->TypeID(),
                                 formatString, placeholderPS.ConversionPos
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
            throw Exception( ALIB_CALLER_NULLED, FMTExceptions::UnknownConversionPS,
                             conversion, placeholder.Arg->TypeID(),
                             formatString,  placeholderPS.ConversionPos
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
        throw Exception(ALIB_CALLER_NULLED, FMTExceptions::PrecisionSpecificationWithInteger,
                        placeholder.Arg->TypeID(),
                        formatString, placeholderPS.PrecisionPos );
    return result;

}


} // namespace [alib::lang::format]

