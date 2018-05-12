// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER_PYTHONSTYLE)
    #include "alib/strings/format/formatterpythonstyle.hpp"
#endif



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


namespace aworx { namespace lib { namespace strings { namespace format {

ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING
//! @cond NO_DOX
namespace {
    template<typename TChar>    struct StringConstFPS {};
    template<>   struct StringConstFPS<char>
    {
        static const NTString Name              ;
        static const NTString EGF               ;
        static const NTString sdcboxXeEfFngGhHB ;
        static const NTString Xtinguish         ;
        static const NTString Upper             ;
        static const NTString Lower             ;
        static const NTString str               ;
        static const NTString Quote             ;
        static const NTString Fill              ;
        static const NTString Tab               ;
        static const NTString ATab              ;
        static const NTString Reset             ;
        static const NTString AWidth            ;
        static const NTString Esc               ;
        static const NTString A                 ;
        static const NTString Replace           ;
        static const NTString brobro            ;
        static const NTString bro               ;
        static const NTString brcbrc            ;
        static const NTString brc               ;
        static const NTString colbra            ;
        static const NTString Q                 ;
    };
    template<>   struct StringConstFPS<wchar_t>
    {
        static const WTString Name              ;
        static const WTString EGF               ;
        static const WTString sdcboxXeEfFngGhHB ;
        static const WTString Xtinguish         ;
        static const WTString Upper             ;
        static const WTString Lower             ;
        static const WTString str               ;
        static const WTString Quote             ;
        static const WTString Fill              ;
        static const WTString Tab               ;
        static const WTString ATab              ;
        static const WTString Reset             ;
        static const WTString AWidth            ;
        static const WTString Esc               ;
        static const WTString A                 ;
        static const WTString Replace           ;
        static const WTString brobro            ;
        static const WTString bro               ;
        static const WTString brcbrc            ;
        static const WTString brc               ;
        static const WTString colbra            ;
        static const WTString Q                 ;
    };
const NTString StringConstFPS<char   >::Name              =  "FormatterPythonStyle";
const NTString StringConstFPS<char   >::EGF               =  "EGF"                 ;
const NTString StringConstFPS<char   >::sdcboxXeEfFngGhHB =  "sdcboxXeEfFngGhHB%"  ;
const NTString StringConstFPS<char   >::Xtinguish         =  "Xtinguish"           ;
const NTString StringConstFPS<char   >::Upper             =  "Upper"               ;
const NTString StringConstFPS<char   >::Lower             =  "Lower"               ;
const NTString StringConstFPS<char   >::str               =  "str"                 ;
const NTString StringConstFPS<char   >::Quote             =  "Quote"               ;
const NTString StringConstFPS<char   >::Fill              =  "Fill"                ;
const NTString StringConstFPS<char   >::Tab               =  "Tab"                 ;
const NTString StringConstFPS<char   >::ATab              =  "ATab"                ;
const NTString StringConstFPS<char   >::Reset             =  "Reset"               ;
const NTString StringConstFPS<char   >::AWidth            =  "AWidth"              ;
const NTString StringConstFPS<char   >::Esc               =  "Esc"                 ;
const NTString StringConstFPS<char   >::A                 =  "A"                   ;
const NTString StringConstFPS<char   >::Replace           =  "Replace"             ;
const NTString StringConstFPS<char   >::brobro            =  "{{"                  ;
const NTString StringConstFPS<char   >::bro               =  "{"                   ;
const NTString StringConstFPS<char   >::brcbrc            =  "}}"                  ;
const NTString StringConstFPS<char   >::brc               =  "}"                   ;
const NTString StringConstFPS<char   >::colbra            =  ":}"                  ;
const NTString StringConstFPS<char   >::Q                 =  "\""                  ;

const WTString StringConstFPS<wchar_t>::Name              = L"FormatterPythonStyle";
const WTString StringConstFPS<wchar_t>::EGF               = L"EGF"                 ;
const WTString StringConstFPS<wchar_t>::sdcboxXeEfFngGhHB = L"sdcboxXeEfFngGhHB%"  ;
const WTString StringConstFPS<wchar_t>::Xtinguish         = L"Xtinguish"           ;
const WTString StringConstFPS<wchar_t>::Upper             = L"Upper"               ;
const WTString StringConstFPS<wchar_t>::Lower             = L"Lower"               ;
const WTString StringConstFPS<wchar_t>::str               = L"str"                 ;
const WTString StringConstFPS<wchar_t>::Quote             = L"Quote"               ;
const WTString StringConstFPS<wchar_t>::Fill              = L"Fill"                ;
const WTString StringConstFPS<wchar_t>::Tab               = L"Tab"                 ;
const WTString StringConstFPS<wchar_t>::ATab              = L"ATab"                ;
const WTString StringConstFPS<wchar_t>::Reset             = L"Reset"               ;
const WTString StringConstFPS<wchar_t>::AWidth            = L"AWidth"              ;
const WTString StringConstFPS<wchar_t>::Esc               = L"Esc"                 ;
const WTString StringConstFPS<wchar_t>::A                 = L"A"                   ;
const WTString StringConstFPS<wchar_t>::Replace           = L"Replace"             ;
const WTString StringConstFPS<wchar_t>::brobro            = L"{{"                  ;
const WTString StringConstFPS<wchar_t>::bro               = L"{"                   ;
const WTString StringConstFPS<wchar_t>::brcbrc            = L"}}"                  ;
const WTString StringConstFPS<wchar_t>::brc               = L"}"                   ;
const WTString StringConstFPS<wchar_t>::colbra            = L":}"                  ;
const WTString StringConstFPS<wchar_t>::Q                 = L"\""                  ;

} // anonymous namespace
//! @endcond
ALIB_WARNINGS_RESTORE


template<typename TChar>
FormatterPythonStyleBase<TChar>::FormatterPythonStyleBase()
: FormatterStdImpl<TChar>( StringConstFPS<TChar>::Name )
{
    // set number format to python defaults
    FormatterBase<TChar>::DefaultNumberFormat.ForceDecimalPoint       = false;
    FormatterBase<TChar>::DefaultNumberFormat.WriteExponentPlusSign   = true;
}

template<typename TChar>
FormatterStdImpl<TChar>*   FormatterPythonStyleBase<TChar>::Clone()
{
    // create a clone
    FormatterPythonStyleBase<TChar>* clone= new FormatterPythonStyleBase<TChar>();

    // create a clone of #Next, in the case that next is derived from std base class
    if( FormatterBase<TChar>::Next )
        clone->FormatterBase<TChar>::Next= FormatterBase<TChar>::Next->Clone();

    // copy my settings, that's it
    clone->CloneSettings( *this );
    return clone;
}

template<typename TChar>
void FormatterPythonStyleBase<TChar>::reset()
{
    FormatterStdImpl<TChar>::reset();
    autoSizes.Reset();
}

template<typename TChar>
void FormatterPythonStyleBase<TChar>::initializeFormat()
{
    FormatterStdImpl<TChar>::initializeFormat();
    autoSizes.Start();
}


template<typename TChar>
void FormatterPythonStyleBase<TChar>::resetPHAs()
{
    // first invoke parent's setting...
    FormatterStdImpl<TChar>::resetPHAs();

    // ...then make some "python like" adjustments
                             phaExtConversion                    = nullptr;
                             phaExtConversionPos                 = -1;

    FormatterStdImpl<TChar>::phaNF.ForceDecimalPoint             = false;
    FormatterStdImpl<TChar>::phaNF.ExponentSeparator             = FormatterBase<TChar>::AlternativeNumberFormat.ExponentSeparator; // choose lower case as default
    FormatterStdImpl<TChar>::phaNF.INFLiteral                    = FormatterBase<TChar>::AlternativeNumberFormat.INFLiteral;        // choose lower case as default
    FormatterStdImpl<TChar>::phaNF.NANLiteral                    = FormatterBase<TChar>::AlternativeNumberFormat.NANLiteral;        // choose lower case as default
    FormatterStdImpl<TChar>::phaNF.OmitTrailingFractionalZeros   = true;

                             phaExtPrecision                     = -1;
                             phaExtPrecisionPos                  = -1;
                             phaExtDefaultPrecision              = 6;
}

template<typename TChar>
integer FormatterPythonStyleBase<TChar>::findPlaceholder()
{
    integer idx= 0;
    while(     (idx= FormatterStdImpl<TChar>::parser.IndexOf('{', idx )) >= 0
           &&  FormatterStdImpl<TChar>::parser.CharAt( idx + 1 ) == '{'                   )
    {
        // double ESC found (escaped {{)
        idx+= + 2;
    }
    return idx;
}

template<typename TChar>
bool FormatterPythonStyleBase<TChar>::parsePlaceholder()
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

                if( isdigit( FormatterStdImpl<TChar>::parser.CharAtStart() )  )
                {
                    int argNo;
                    FormatterStdImpl<TChar>::parser.ConsumeDecDigits( argNo );
                    FormatterStdImpl<TChar>::setArgument( argNo );
                }
                NEXTSTATE(CONVERSION);


            case CONVERSION:
                if( FormatterStdImpl<TChar>::parser.CharAtStart() == '!' )
                {
                    phaExtConversionPos  = FormatterStdImpl<TChar>::formatString.Length() - FormatterStdImpl<TChar>::parser.Length() - 1;
                    integer endConversion= FormatterStdImpl<TChar>::parser. template IndexOfAny<Inclusion::Include>(StringConstFPS<TChar>::colbra );
                    if( endConversion < 0 )
                        throw Exception(ALIB_CALLER_NULLED, Exceptions::MissingClosingBracket,
                                        FormatterStdImpl<TChar>::formatString, phaExtConversionPos );

                    FormatterStdImpl<TChar>::parser.ConsumeChars( endConversion, &phaExtConversion );
                }
                NEXTSTATE(COLON);

            case COLON:
                if( FormatterStdImpl<TChar>::parser.CharAtStart() != ':'  )
                {
                    state= END;
                    continue;
                }

                FormatterStdImpl<TChar>::parser.ConsumeChars(1);
                NEXTSTATE(FORMAT_SPEC)

            case FORMAT_SPEC:
            {
                // find end of format spec (allow "\}" in format string)
                integer eoFormatSpec= -1;
                do
                {
                    eoFormatSpec= FormatterStdImpl<TChar>::parser.IndexOf( '}', eoFormatSpec + 1 );
                }
                while(      eoFormatSpec > 0
                        &&  FormatterStdImpl<TChar>::parser.CharAt( eoFormatSpec - 1) == '\\' );

                if ( eoFormatSpec < 0 )
                    throw Exception(ALIB_CALLER_NULLED, Exceptions::MissingClosingBracket,
                                    FormatterStdImpl<TChar>::formatString,
                                    FormatterStdImpl<TChar>::formatString.Length()  );

                // extract format spec to separate substring
                FormatterStdImpl<TChar>::parser.ConsumeChars( eoFormatSpec, &(FormatterStdImpl<TChar>::phaFormatSpec) ) ;

                NEXTSTATE(END)
            }

//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum4]
case END:
    if( FormatterStdImpl<TChar>::parser.CharAtStart() != '}'  )
        throw Exception(ALIB_CALLER_NULLED, Exceptions::MissingClosingBracket,
                        FormatterStdImpl<TChar>::formatString,
                        FormatterStdImpl<TChar>::formatString.Length() - FormatterStdImpl<TChar>::parser.Length() );
//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum4]
                FormatterStdImpl<TChar>::parser.ConsumeChars(1);
                return true;

        } // state switch

    } // read loop

}



template<typename TChar>
bool FormatterPythonStyleBase<TChar>::parseStdFormatSpec()
{
    // parse alignment and fill character. This is a bit tricky to shorten the code.
    // The alignment token <,>,^ or = can be on first or second position. If second, then the
    // first was a fill character. Therefore we loop from 0 to 1....
    integer charsToConsume= 1;
    FormatterStdImpl<TChar>::phaAlignmentSpecified= true;
    SubstringBase<TChar>& formatSpec= FormatterStdImpl<TChar>::phaFormatSpec;


    int cNo= 0;
    do switch ( formatSpec.CharAt( cNo++ ) )
    {
        case '<': FormatterStdImpl<TChar>::phaAlignment= Alignment::Left;    break;
        case '>': FormatterStdImpl<TChar>::phaAlignment= Alignment::Right;   break;
        case '^': FormatterStdImpl<TChar>::phaAlignment= Alignment::Center;  break;
        case '=': FormatterStdImpl<TChar>::phaAlignment= Alignment::Right;
                  FormatterStdImpl<TChar>::phaSignPaddingMode=  true;        break;

        default:
            if(cNo == 1 )
            {
                charsToConsume= 2;
                FormatterStdImpl<TChar>::phaFillChar= formatSpec.CharAtStart();
            }
            else
            {
                charsToConsume= 0;
                FormatterStdImpl<TChar>::phaFillChar= ' ';
                FormatterStdImpl<TChar>::phaAlignmentSpecified= false;
            }
        break;
    }
    while( cNo < 2 && charsToConsume != 1 );
    formatSpec.ConsumeChars( charsToConsume );

    // other things to parse
    TChar actChar;
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
                FormatterStdImpl<TChar>::phaSignPaddingMode= true;
            formatSpec.ConsumeDecDigits( FormatterStdImpl<TChar>::phaWidth );

            continue; // not consume below
        }

        // precision. Python doc says:
        // "The precision is a decimal number indicating how many digits should be displayed
        // after the decimal point for a floating point value formatted with 'f' and 'F', or
        // before and after the decimal point for a floating point value formatted with 'g' or 'G'.
        // For non-number types the field indicates the maximum field size - in other words,
        // how many characters will be used from the field content. The precision is not
        // allowed for integer values."
        else if( actChar == '.' )
        {
            phaExtPrecisionPos=    FormatterStdImpl<TChar>::formatString.Length()
                                 - FormatterStdImpl<TChar>::parser.Length()
                                 - formatSpec.Length() -1;
            formatSpec.ConsumeChars( 1 );
            if ( ! formatSpec.ConsumeDecDigits( phaExtPrecision ) )
                throw Exception(ALIB_CALLER_NULLED, Exceptions::MissingPrecisionValuePS,
                                FormatterStdImpl<TChar>::formatString,
                                  FormatterStdImpl<TChar>::formatString.Length()
                                - FormatterStdImpl<TChar>::parser.Length()
                                - formatSpec.Length() - 1 );
            continue;
        }


        // ----------------------------- Types -------------------------------------
        else if (StringConstFPS<TChar>::sdcboxXeEfFngGhHB.IndexOf( actChar ) >= 0 )
        {
            if ( FormatterStdImpl<TChar>::phaTypeCode != '\0' )
                throw Exception(ALIB_CALLER_NULLED, Exceptions::DuplicateTypeCode,
                                actChar, FormatterStdImpl<TChar>::phaTypeCode,
                                  FormatterStdImpl<TChar>::formatString,
                                  FormatterStdImpl<TChar>::formatString.Length()
                                - FormatterStdImpl<TChar>::parser.Length()
                                - formatSpec.Length() - 1 );

            FormatterStdImpl<TChar>::phaTypeCode= actChar;
            FormatterStdImpl<TChar>::phaTypeCodePosition=   FormatterStdImpl<TChar>::formatString.Length()
                                                          - FormatterStdImpl<TChar>::parser.Length()
                                                          - formatSpec.Length() - 1;

            if( StringConstFPS<TChar>::EGF.IndexOf( actChar ) >= 0 )
            {
                FormatterStdImpl<TChar>::phaNF.ExponentSeparator= FormatterBase<TChar>::DefaultNumberFormat.ExponentSeparator;
                FormatterStdImpl<TChar>::phaNF.INFLiteral=        FormatterBase<TChar>::DefaultNumberFormat.INFLiteral;
                FormatterStdImpl<TChar>::phaNF.NANLiteral=        FormatterBase<TChar>::DefaultNumberFormat.NANLiteral;
            }

            switch( actChar )
            {
                case 's':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::String;                break;
                case 'd':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::IntBase10;             break;
                case 'c':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::Character;             break;
                case 'b':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::IntBinary;             break;
                case 'o':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::IntOctal;              break;

                case 'X':   FormatterStdImpl<TChar>::phaNF.HexLowerCase= false;              ALIB_FALLTHROUGH
                case 'x':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::IntHex;                break;

                case 'H':   FormatterStdImpl<TChar>::phaNF.HexLowerCase= false;              ALIB_FALLTHROUGH
                case 'h':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::HashCode;              break;
                case 'B':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::Bool;                  break;

                // float types
                case 'E':
                case 'e':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::Float;
                            FormatterStdImpl<TChar>::phaNF.ForceScientific= true;            break;

                case '%':   FormatterStdImpl<TChar>::phaIsPercentage= true;                  ALIB_FALLTHROUGH
                case 'f':
                case 'F':   FormatterStdImpl<TChar>::phaNF.OmitTrailingFractionalZeros= false;
                            FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::Float;                 break;

                case 'n':   FormatterStdImpl<TChar>::phaNF.DecimalPointChar=   FormatterBase<TChar>::AlternativeNumberFormat.DecimalPointChar;
                            FormatterStdImpl<TChar>::phaNF.ThousandsGroupChar= FormatterBase<TChar>::AlternativeNumberFormat.ThousandsGroupChar;
                                                                   ALIB_FALLTHROUGH
                case 'G':                                          ALIB_FALLTHROUGH
                case 'g':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::Float;
                            phaExtDefaultPrecision=   -1;
                break;

                default:    ALIB_ERROR(ASTR("Unhandled character in choices string above"));
                            return false;

            }//switch
        }


        // others
        else switch( actChar )
        {
            // sign
            case '+':   FormatterStdImpl<TChar>::phaNF.PlusSign= '+' ;  break;
            case '-':   FormatterStdImpl<TChar>::phaNF.PlusSign= '\0';  break;
            case ' ':   FormatterStdImpl<TChar>::phaNF.PlusSign= ' ' ;  break;

            // alternate version ('#')
            case '#':   FormatterStdImpl<TChar>::phaWriteBinOctHexPrefix=             true;
                        FormatterStdImpl<TChar>::phaNF.ForceDecimalPoint=             true;
                        FormatterStdImpl<TChar>::phaNF.OmitTrailingFractionalZeros=   false;
            break;

            // Enable grouping
            case ',':   FormatterStdImpl<TChar>::phaNF.WriteGroupChars= true;
                        break;


            default:
                throw Exception(ALIB_CALLER_NULLED, Exceptions::UnknownTypeCode,
                                actChar,
                                FormatterStdImpl<TChar>::formatString,
                                  FormatterStdImpl<TChar>::formatString.Length()
                                - FormatterStdImpl<TChar>::parser.Length()
                                - formatSpec.Length() - 1 );
        }

        formatSpec.ConsumeChars(1);
    }

    return true;
}

template<typename TChar>
void    FormatterPythonStyleBase<TChar>::replaceEscapeSequences( integer startIdx )
{
    FormatterStdImpl<TChar>::targetString->SearchAndReplace( StringConstFPS<TChar>::brobro, StringConstFPS<TChar>::bro , startIdx );
    FormatterStdImpl<TChar>::targetString->SearchAndReplace( StringConstFPS<TChar>::brcbrc, StringConstFPS<TChar>::brc , startIdx );
    FormatterStdImpl<TChar>::targetString->template _<false>( typename FormatBase<TChar>::Escape( Switch::Off, startIdx ) );

    // search the last newline character in the just written portion of the target string.
    // If one is found, reset autosizes and actual start of string.
    integer lastNewLine=-1;
    integer actNewLine = startIdx - 1;
    while( (actNewLine= FormatterStdImpl<TChar>::targetString->IndexOf('\n', actNewLine + 1)) > 0 )
        lastNewLine= actNewLine;
    if( lastNewLine >= 0 )
    {
        FormatterStdImpl<TChar>::targetStringStartLength= lastNewLine + 1;
        autoSizes.Start();
    }
}

template<typename TChar>
bool    FormatterPythonStyleBase<TChar>::preAndPostProcess( integer startIdx, AStringBase<TChar>* target )
{
    bool isPreProcess=  startIdx < 0;
    bool isPostProcess= startIdx>= 0 && target == nullptr;
    SubstringBase<TChar> conversion= phaExtConversion;
    phaExtConversionPos++;


    while( conversion.IsNotEmpty() )
    {
        if( !conversion.ConsumeChar('!') )
            throw Exception(ALIB_CALLER_NULLED, Exceptions::ExclamationMarkExpected,
                            FormatterStdImpl<TChar>::formatString,
                              phaExtConversionPos
                            + phaExtConversion.Length()
                            - conversion.Length() );

             if(  conversion.ConsumePartOf( StringConstFPS<TChar>::Xtinguish) > 0 )   { return false;                                                         }
             if(  conversion.ConsumePartOf( StringConstFPS<TChar>::Upper    ) > 0 )   { if (isPostProcess) FormatterStdImpl<TChar>::targetString->ToUpper( startIdx );                 }
        else if(  conversion.ConsumePartOf( StringConstFPS<TChar>::Lower    ) > 0 )   { if (isPostProcess) FormatterStdImpl<TChar>::targetString->ToLower( startIdx );                 }
        else if(  conversion.ConsumePartOf( StringConstFPS<TChar>::str      ) > 0
                ||conversion.ConsumePartOf( StringConstFPS<TChar>::Quote    ) > 0 )   { if (isPostProcess) FormatterStdImpl<TChar>::targetString->template InsertAt<false>(StringConstFPS<TChar>::Q, startIdx).template _<false>( StringConstFPS<TChar>::Q );  }

        else if(  conversion.ConsumePartOf( StringConstFPS<TChar>::Fill     ) > 0 )
        {
            FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::Fill;
            FormatterStdImpl<TChar>::phaFillChar=  conversion.template ConsumeChar<Case::Ignore>('C' ) && conversion.Length() > 0
                                                   ? conversion.template ConsumeChar<false>()
                                                   : ' ';

        }

        else if(  conversion.ConsumePartOf( StringConstFPS<TChar>::Tab      ) > 0 )
        {
            TChar tabChar= conversion.template ConsumeChar<Case::Ignore>('C') && conversion.Length() > 0
                                ? conversion.template ConsumeChar<false>()
                                : ' ';
            int tabSize;
            if( !conversion.template ConsumeDecDigits<int>( tabSize ) )
                tabSize= 8;

            if( isPreProcess )
                FormatterStdImpl<TChar>::targetString->template _<false>( typename FormatBase<TChar>::Tab( tabSize, -1, 1, tabChar ) );

        }

        else if( conversion.ConsumePartOf( StringConstFPS<TChar>::ATab  , 2 ) > 0 )
        {

            if( conversion.ConsumePartOf( StringConstFPS<TChar>::Reset      ) > 0 )
            {
                if( isPreProcess )
                    autoSizes.Reset();
            }
            else
            {
                TChar tabChar= conversion.template ConsumeChar<Case::Ignore>('C' ) && conversion.Length() > 0
                                ? conversion.template ConsumeChar<false>()
                                : ' ';

                int growth;
                if( !conversion.template ConsumeDecDigits<int>( growth ) )
                    growth= 3;

                if( isPreProcess )
                {
                    integer actPos= FormatterStdImpl<TChar>::targetString->Length() - FormatterStdImpl<TChar>::targetStringStartLength;
                    integer tabStop= autoSizes.Next( actPos , growth );
                    FormatterStdImpl<TChar>::targetString->template InsertChars<false>( tabChar, tabStop - actPos );
                }
            }
        }

        else if( conversion.ConsumePartOf( StringConstFPS<TChar>::AWidth  , 2 ) > 0 )
        {
            if( conversion.ConsumePartOf( StringConstFPS<TChar>::Reset      ) > 0 )
            {
                if( isPreProcess )
                    autoSizes.Reset();
            }
            else
            {
                int extraPadding;
                conversion.template ConsumeDecDigits<int>( extraPadding );

                if( isPreProcess )
                    FormatterStdImpl<TChar>::phaWidth= autoSizes.Actual( 0, extraPadding );
                else if ( isPostProcess )
                    autoSizes.Next( FormatterStdImpl<TChar>::targetString->Length() - startIdx, extraPadding );
            }
        }

        else if(    conversion.ConsumePartOf( StringConstFPS<TChar>::Esc ) > 0
                 || conversion.ConsumePartOf( StringConstFPS<TChar>::A   ) > 0 )
        {
            Switch toESC= Switch::On;
            conversion.ConsumeChar('<');
            if(conversion.ConsumeChar('>') )
                toESC=  Switch::Off;

            if( isPostProcess )
                FormatterStdImpl<TChar>::targetString->template _<false>( typename FormatBase<TChar>::Escape( toESC, startIdx ) );
        }

        else if( conversion.ConsumePartOf( StringConstFPS<TChar>::Replace  , 2 ) > 0 )
        {
            PreallocatedStringBase<TChar,64> search= conversion.ConsumeField( '<', '>' );
            StringBase<TChar>               replace= conversion.ConsumeField( '<', '>' );
            if( search.IsNull() || replace.IsNull() )
                throw Exception( ALIB_CALLER_NULLED, Exceptions::MissingReplacementStrings,
                                 FormatterStdImpl<TChar>::formatString,
                                   phaExtConversionPos
                                 + phaExtConversion.Length()
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
                             FormatterStdImpl<TChar>::formatString,
                               phaExtConversionPos
                             + phaExtConversion.Length()
                             - conversion.Length() );
    }

    return true;
}

template<typename TChar>
bool  FormatterPythonStyleBase<TChar>::checkStdFieldAgainstArgument()
{
    bool wasFloat= FormatterStdImpl<TChar>::phaType == FormatterStdImpl<TChar>::PHType::Float;
    if( wasFloat )
    {
        if ( phaExtPrecision >= 0 )
            FormatterStdImpl<TChar>::phaNF.FractionalPartWidth= phaExtPrecision;
        else if( FormatterStdImpl<TChar>::phaNF.FractionalPartWidth < 0 )
            FormatterStdImpl<TChar>::phaNF.FractionalPartWidth= phaExtDefaultPrecision;
    }

    bool result= FormatterStdImpl<TChar>::checkStdFieldAgainstArgument();

    if( !wasFloat && FormatterStdImpl<TChar>::phaType == FormatterStdImpl<TChar>::PHType::Float )
    {
        if ( phaExtPrecision >= 0 )
            FormatterStdImpl<TChar>::phaNF.FractionalPartWidth= phaExtPrecision;
    }

    if(   FormatterStdImpl<TChar>::phaType == FormatterStdImpl<TChar>::PHType::String
       || FormatterStdImpl<TChar>::phaType == FormatterStdImpl<TChar>::PHType::Bool  )
        FormatterStdImpl<TChar>::phaCutContent= phaExtPrecision;
    else if (   phaExtPrecision >= 0
             && FormatterStdImpl<TChar>::phaType != FormatterStdImpl<TChar>::PHType::Float )
        throw Exception(ALIB_CALLER_NULLED, Exceptions::PrecisionSpecificationWithInteger,
                        FormatterStdImpl<TChar>::formatString, phaExtPrecisionPos );
    return result;

}


template                              FormatterPythonStyleBase<char   >::FormatterPythonStyleBase();
template  FormatterStdImpl<char   >*  FormatterPythonStyleBase<char   >::Clone();
template  void                        FormatterPythonStyleBase<char   >::reset();
template  void                        FormatterPythonStyleBase<char   >::resetPHAs();
template  void                        FormatterPythonStyleBase<char   >::initializeFormat();
template  integer                     FormatterPythonStyleBase<char   >::findPlaceholder();
template  bool                        FormatterPythonStyleBase<char   >::parsePlaceholder();
template  bool                        FormatterPythonStyleBase<char   >::parseStdFormatSpec();
template  void                        FormatterPythonStyleBase<char   >::replaceEscapeSequences(integer);
template  bool                        FormatterPythonStyleBase<char   >::preAndPostProcess(integer,NAString*);
template  bool                        FormatterPythonStyleBase<char   >::checkStdFieldAgainstArgument();

template                              FormatterPythonStyleBase<wchar_t>::FormatterPythonStyleBase();
template  FormatterStdImpl<wchar_t>*  FormatterPythonStyleBase<wchar_t>::Clone();
template  void                        FormatterPythonStyleBase<wchar_t>::reset();
template  void                        FormatterPythonStyleBase<wchar_t>::resetPHAs();
template  void                        FormatterPythonStyleBase<wchar_t>::initializeFormat();
template  integer                     FormatterPythonStyleBase<wchar_t>::findPlaceholder();
template  bool                        FormatterPythonStyleBase<wchar_t>::parsePlaceholder();
template  bool                        FormatterPythonStyleBase<wchar_t>::parseStdFormatSpec();
template  void                        FormatterPythonStyleBase<wchar_t>::replaceEscapeSequences(integer);
template  bool                        FormatterPythonStyleBase<wchar_t>::preAndPostProcess(integer,WAString*);
template  bool                        FormatterPythonStyleBase<wchar_t>::checkStdFieldAgainstArgument();

}}}} // namespace [aworx::lib::strings::format]
