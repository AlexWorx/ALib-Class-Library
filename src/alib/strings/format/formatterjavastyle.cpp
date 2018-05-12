// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER_JAVASTYLE)
    #include "alib/strings/format/formatterjavastyle.hpp"
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
    template<typename TChar>    struct StringConstFJS {};
    template<>   struct StringConstFJS<char>
    {
        static const NTString Name         ;
        static const NTString sSbBhHgGcCtT ;
        static const NTString seg          ;
        static const NTString SBCT         ;
        static const NTString egf          ;
        static const NTString cCtTd        ;
        static const NTString per          ;
        static const NTString perper       ;
        static const NTString perN         ;
        static const NTString HH           ;
        static const NTString H            ;
        static const NTString KK           ;
        static const NTString K            ;
        static const NTString mm           ;
        static const NTString ss           ;
        static const NTString MMMM         ;
        static const NTString MMM          ;
        static const NTString dddd         ;
        static const NTString ddd          ;
        static const NTString yyyy         ;
        static const NTString yy           ;
        static const NTString MM           ;
        static const NTString dd           ;
        static const NTString d            ;
        static const NTString HH_mm        ;
        static const NTString HH_mm_ss     ;
        static const NTString MM_dd_yy     ;
        static const NTString yyyy_MM_dd   ;
        static const NTString HexLP        ;
        static const NTString OctLP        ;
        static const NTString NaN          ;
        static const NTString INF          ;
        static const NTString Inf          ;
    };
    template<>   struct StringConstFJS<wchar_t>
    {
        static const WTString Name         ;
        static const WTString sSbBhHgGcCtT ;
        static const WTString seg          ;
        static const WTString SBCT         ;
        static const WTString egf          ;
        static const WTString cCtTd        ;
        static const WTString per          ;
        static const WTString perper       ;
        static const WTString perN         ;
        static const WTString HH           ;
        static const WTString H            ;
        static const WTString KK           ;
        static const WTString K            ;
        static const WTString mm           ;
        static const WTString ss           ;
        static const WTString MMMM         ;
        static const WTString MMM          ;
        static const WTString dddd         ;
        static const WTString ddd          ;
        static const WTString yyyy         ;
        static const WTString yy           ;
        static const WTString MM           ;
        static const WTString dd           ;
        static const WTString d            ;
        static const WTString HH_mm        ;
        static const WTString HH_mm_ss     ;
        static const WTString MM_dd_yy     ;
        static const WTString yyyy_MM_dd   ;
        static const WTString HexLP        ;
        static const WTString OctLP        ;
        static const WTString NaN          ;
        static const WTString INF          ;
        static const WTString Inf          ;
    };
const NTString StringConstFJS<char   >::Name         =  "FormatterJavaStyle";
const NTString StringConstFJS<char   >::sSbBhHgGcCtT =  "sSbBhHgGcCtT"      ;
const NTString StringConstFJS<char   >::seg          =  "seg"               ;
const NTString StringConstFJS<char   >::SBCT         =  "SBCT"              ;
const NTString StringConstFJS<char   >::egf          =  "egf"               ;
const NTString StringConstFJS<char   >::cCtTd        =  "cCtTd"             ;
const NTString StringConstFJS<char   >::per          =  "%"                 ;
const NTString StringConstFJS<char   >::perper       =  "%%"                ;
const NTString StringConstFJS<char   >::perN         =  "%n"                ;
const NTString StringConstFJS<char   >::HH           = "HH"                 ;
const NTString StringConstFJS<char   >::H            = "H"                  ;
const NTString StringConstFJS<char   >::KK           = "KK"                 ;
const NTString StringConstFJS<char   >::K            = "K"                  ;
const NTString StringConstFJS<char   >::mm           = "mm"                 ;
const NTString StringConstFJS<char   >::ss           = "ss"                 ;
const NTString StringConstFJS<char   >::MMMM         = "MMMM"               ;
const NTString StringConstFJS<char   >::MMM          = "MMM"                ;
const NTString StringConstFJS<char   >::dddd         = "dddd"               ;
const NTString StringConstFJS<char   >::ddd          = "ddd"                ;
const NTString StringConstFJS<char   >::yyyy         = "yyyy"               ;
const NTString StringConstFJS<char   >::yy           = "yy"                 ;
const NTString StringConstFJS<char   >::MM           = "MM"                 ;
const NTString StringConstFJS<char   >::dd           = "dd"                 ;
const NTString StringConstFJS<char   >::d            = "d"                  ;
const NTString StringConstFJS<char   >::HH_mm        = "HH:mm"              ;
const NTString StringConstFJS<char   >::HH_mm_ss     = "HH:mm:ss"           ;
const NTString StringConstFJS<char   >::MM_dd_yy     = "MM/dd/yy"           ;
const NTString StringConstFJS<char   >::yyyy_MM_dd   = "yyyy-MM-dd"         ;
const NTString StringConstFJS<char   >::HexLP        = "0X";
const NTString StringConstFJS<char   >::OctLP        = "0";
const NTString StringConstFJS<char   >::NaN          = "NaN";
const NTString StringConstFJS<char   >::INF          = "INFINITY";
const NTString StringConstFJS<char   >::Inf          = "Infinity";



const WTString StringConstFJS<wchar_t>::Name         = L"FormatterJavaStyle";
const WTString StringConstFJS<wchar_t>::sSbBhHgGcCtT = L"sSbBhHgGcCtT"      ;
const WTString StringConstFJS<wchar_t>::seg          = L"seg"               ;
const WTString StringConstFJS<wchar_t>::SBCT         = L"SBCT"              ;
const WTString StringConstFJS<wchar_t>::egf          = L"egf"               ;
const WTString StringConstFJS<wchar_t>::cCtTd        = L"cCtTd"             ;
const WTString StringConstFJS<wchar_t>::per          = L"%"                 ;
const WTString StringConstFJS<wchar_t>::perper       = L"%%"                ;
const WTString StringConstFJS<wchar_t>::perN         = L"%n"                ;
const WTString StringConstFJS<wchar_t>::HH           = L"HH"                ;
const WTString StringConstFJS<wchar_t>::H            = L"H"                 ;
const WTString StringConstFJS<wchar_t>::KK           = L"KK"                ;
const WTString StringConstFJS<wchar_t>::K            = L"K"                 ;
const WTString StringConstFJS<wchar_t>::mm           = L"mm"                ;
const WTString StringConstFJS<wchar_t>::ss           = L"ss"                ;
const WTString StringConstFJS<wchar_t>::MMMM         = L"MMMM"              ;
const WTString StringConstFJS<wchar_t>::MMM          = L"MMM"               ;
const WTString StringConstFJS<wchar_t>::dddd         = L"dddd"              ;
const WTString StringConstFJS<wchar_t>::ddd          = L"ddd"               ;
const WTString StringConstFJS<wchar_t>::yyyy         = L"yyyy"              ;
const WTString StringConstFJS<wchar_t>::yy           = L"yy"                ;
const WTString StringConstFJS<wchar_t>::MM           = L"MM"                ;
const WTString StringConstFJS<wchar_t>::dd           = L"dd"                ;
const WTString StringConstFJS<wchar_t>::d            = L"d"                 ;
const WTString StringConstFJS<wchar_t>::HH_mm        = L"HH:mm"             ;
const WTString StringConstFJS<wchar_t>::HH_mm_ss     = L"HH:mm:ss"          ;
const WTString StringConstFJS<wchar_t>::MM_dd_yy     = L"MM/dd/yy"          ;
const WTString StringConstFJS<wchar_t>::yyyy_MM_dd   = L"yyyy-MM-dd"        ;
const WTString StringConstFJS<wchar_t>::HexLP        = L"0X";
const WTString StringConstFJS<wchar_t>::OctLP        = L"0";
const WTString StringConstFJS<wchar_t>::NaN          = L"NaN";
const WTString StringConstFJS<wchar_t>::INF          = L"INFINITY";
const WTString StringConstFJS<wchar_t>::Inf          = L"Infinity";

} // anonymous namespace
//! @endcond


ALIB_WARNINGS_RESTORE

template<typename TChar>
FormatterJavaStyleBase<TChar>::FormatterJavaStyleBase()
: FormatterStdImpl<TChar>( StringConstFJS<TChar>::Name )
{
    // arguments are counted starting with 1.
    FormatterStdImpl<TChar>::argumentCountStartsWith1 = true;

    // set number format to Java defaults
    FormatterBase<TChar>::DefaultNumberFormat.ForceDecimalPoint       = false;
    FormatterBase<TChar>::DefaultNumberFormat.WriteExponentPlusSign   = true;

    // set number format to Java defaults
    FormatterBase<TChar>::DefaultNumberFormat    .HexLiteralPrefix   = StringConstFJS<TChar>::HexLP;

    FormatterBase<TChar>::DefaultNumberFormat    .OctLiteralPrefix   =
    FormatterBase<TChar>::AlternativeNumberFormat.OctLiteralPrefix   = StringConstFJS<TChar>::OctLP;

    FormatterBase<TChar>::DefaultNumberFormat    .NANLiteral         =
    FormatterBase<TChar>::AlternativeNumberFormat.NANLiteral         = StringConstFJS<TChar>::NaN;
    FormatterBase<TChar>::DefaultNumberFormat    .INFLiteral         = StringConstFJS<TChar>::INF;
    FormatterBase<TChar>::AlternativeNumberFormat.INFLiteral         = StringConstFJS<TChar>::Inf;
}

template<typename TChar>
FormatterStdImpl<TChar>*   FormatterJavaStyleBase<TChar>::Clone()
{
    // create a clone
    FormatterJavaStyleBase<TChar>* clone= new FormatterJavaStyleBase<TChar>();

    // create a clone of #Next, in the case that next is derived from std base class
    if( FormatterBase<TChar>::Next )
        clone->FormatterBase<TChar>::Next= FormatterBase<TChar>::Next->Clone();

    // copy my settings, that's it
    clone->CloneSettings( *this );
    return clone;
}


template<typename TChar>
void FormatterJavaStyleBase<TChar>::resetPHAs()
{
    // first invoke parent's setting...
    FormatterStdImpl<TChar>::resetPHAs();

    // ...then make some "Java like" adjustments
    phaExtConversionUpper               = false;
    FormatterStdImpl<TChar>::phaAlignment                        = Alignment::Right;

    FormatterStdImpl<TChar>::phaNF.ForceDecimalPoint             = false;
    FormatterStdImpl<TChar>::phaNF.OmitTrailingFractionalZeros   = false;

    phaAlternateForm                    = false;

    phaExtPrecision                     = -1;
    phaExtDefaultPrecision              = 6;
}

template<typename TChar>
integer FormatterJavaStyleBase<TChar>::findPlaceholder()
{
    integer idx= 0;
    while(     (idx= FormatterStdImpl<TChar>::parser.IndexOf('%', idx )) >= 0
           &&  (    FormatterStdImpl<TChar>::parser.CharAt( idx + 1 ) == '%'
                 || FormatterStdImpl<TChar>::parser.CharAt( idx + 1 ) == 'n'      ) )
    {
        // double ESC found (escaped %% or %n)
        idx+= + 2;
    }
    return idx;
}

template<typename TChar>
void    FormatterJavaStyleBase<TChar>::replaceEscapeSequences( integer startIdx )
{
    FormatterStdImpl<TChar>::targetString->SearchAndReplace( StringConstFJS<TChar>::perper, StringConstFJS<TChar>::per,     startIdx );
    FormatterStdImpl<TChar>::targetString->SearchAndReplace( StringConstFJS<TChar>::perN, aworx::StringConstants<TChar>::NewLine, startIdx );
    FormatterStdImpl<TChar>::targetString->template _<false>( typename FormatBase<TChar>::Escape( Switch::Off, startIdx ) );
}


template<typename TChar>
bool FormatterJavaStyleBase<TChar>::parsePlaceholder()
{
    enum states
    {
        POSITION        =  1,
        FLAGS           =  2,
        WIDTH           =  3,
        PRECISION       =  4,
        TYPE            =  5,
        TYPE_SUFFIX     =  6,
        END             = 10,
    };

    states  state= POSITION;
    #define NEXTSTATE(s) { state= s; continue; }

    while( true )
    {
        // switch over state. With 'break' we consume one character (kind of success) while
        // with 'continue' we keep the current character (and go to another state)
        switch ( state )
        {
            case POSITION:
            {
                int argNo= -1;
                if ( FormatterStdImpl<TChar>::parser.ConsumeChar('<') )
                {
                    argNo= FormatterStdImpl<TChar>::phaPreviousArgumentIdx + 1;
                }
                else
                {
                    integer i= 0;
                    while(    i < FormatterStdImpl<TChar>::parser.Length()
                           && isdigit( FormatterStdImpl<TChar>::parser.template CharAt<false>(i) ) )
                         ++i;

                    if(  i > 0 &&  FormatterStdImpl<TChar>::parser.template CharAt<false>(i) == '$')
                    {
                        FormatterStdImpl<TChar>::parser.ConsumeDecDigits( argNo );
                        ALIB_ASSERT_RESULT_EQUALS( FormatterStdImpl<TChar>::parser.ConsumeChar('$'), true );
                    }
                }
                if( argNo >= 0 )
                    FormatterStdImpl<TChar>::setArgument( argNo );

                NEXTSTATE(FLAGS);
            }

            case FLAGS:
            {
                bool flagsDone= false;
                while(!flagsDone)
                {
                    switch ( FormatterStdImpl<TChar>::parser.CharAtStart() )
                    {
                        case '-':
                            FormatterStdImpl<TChar>::phaAlignment= Alignment::Left;
                            FormatterStdImpl<TChar>::phaAlignmentSpecified= true;
                            break;

                        case '^':
                            FormatterStdImpl<TChar>::phaAlignment= Alignment::Center;
                            FormatterStdImpl<TChar>::phaAlignmentSpecified= true;
                            break;

                        case '#':
                        {
                            phaAlternateForm= true;
                            FormatterStdImpl<TChar>::phaWriteBinOctHexPrefix=             true;
                            FormatterStdImpl<TChar>::phaNF.ForceDecimalPoint=             true;
                            FormatterStdImpl<TChar>::phaNF.OmitTrailingFractionalZeros=   false;
                            break;
                        }

                        case '+':
                        case ' ':
                            FormatterStdImpl<TChar>::phaNF.PlusSign= FormatterStdImpl<TChar>::parser.CharAtStart();
                            break;

                        case '0':
                            FormatterStdImpl<TChar>::phaSignPaddingMode= true;
                            break;

                        case ',':
                            FormatterStdImpl<TChar>::phaNF.WriteGroupChars= true;
                            break;

                        case '(':
                            throw Exception(ALIB_CALLER_NULLED, Exceptions::NegativeValuesInBracketsNotSupported,
                                            FormatterStdImpl<TChar>::formatString, FormatterStdImpl<TChar>::formatString.Length() - FormatterStdImpl<TChar>::parser.Length() );

                        default:
                            flagsDone= true;
                            break;
                    }

                    if( !flagsDone )
                        FormatterStdImpl<TChar>::parser.ConsumeChars( 1 );
                }
                NEXTSTATE(WIDTH);
            }


            case WIDTH:
            {
                if( isdigit( FormatterStdImpl<TChar>::parser.CharAtStart() ) )
                    FormatterStdImpl<TChar>::parser.ConsumeDecDigits( FormatterStdImpl<TChar>::phaWidth );

                NEXTSTATE(PRECISION);
            }

            case PRECISION:
            {
                if( FormatterStdImpl<TChar>::parser.ConsumeChar( '.' ) && !FormatterStdImpl<TChar>::parser.ConsumeDecDigits( phaExtPrecision ) )
                    throw Exception(ALIB_CALLER_NULLED, Exceptions::MissingPrecisionValueJS,
                                    FormatterStdImpl<TChar>::formatString, FormatterStdImpl<TChar>::formatString.Length() - FormatterStdImpl<TChar>::parser.Length() );

                NEXTSTATE(TYPE);
            }

            case TYPE:
            {
                FormatterStdImpl<TChar>::phaTypeCode=  FormatterStdImpl<TChar>::parser.CharAtStart();
                FormatterStdImpl<TChar>::parser.ConsumeChars( 1 );

                char typeCharLower= static_cast<char>( tolower( FormatterStdImpl<TChar>::phaTypeCode ) );

                if ( typeCharLower == 'a' )
                {
                    throw Exception(ALIB_CALLER_NULLED, Exceptions::HexadecimalFloatFormatNotSupported,
                                    FormatterStdImpl<TChar>::formatString, FormatterStdImpl<TChar>::formatString.Length() - FormatterStdImpl<TChar>::parser.Length() - 1 );
                }

                if( phaAlternateForm && StringConstFJS<TChar>::sSbBhHgGcCtT.IndexOf( FormatterStdImpl<TChar>::phaTypeCode ) >= 0 )
                    throw Exception(ALIB_CALLER_NULLED, Exceptions::NoAlternateFormOfConversion,
                                    FormatterStdImpl<TChar>::phaTypeCode,
                                    FormatterStdImpl<TChar>::formatString, FormatterStdImpl<TChar>::formatString.Length() - FormatterStdImpl<TChar>::parser.Length() - 1 );

                if( StringConstFJS<TChar>::seg.IndexOf( FormatterStdImpl<TChar>::phaTypeCode ) >= 0 )
                {
                    if( FormatterStdImpl<TChar>::phaTypeCode != 's' )
                        FormatterStdImpl<TChar>::phaNF.ExponentSeparator= FormatterBase<TChar>::AlternativeNumberFormat.ExponentSeparator;
                    FormatterStdImpl<TChar>::phaNF.INFLiteral=        FormatterBase<TChar>::AlternativeNumberFormat.INFLiteral;
                    FormatterStdImpl<TChar>::phaNF.NANLiteral=        FormatterBase<TChar>::AlternativeNumberFormat.NANLiteral;
                }

                if( StringConstFJS<TChar>::SBCT.IndexOf( FormatterStdImpl<TChar>::phaTypeCode ) >= 0 )
                    phaExtConversionUpper= true;

                if( StringConstFJS<TChar>::egf.IndexOf( typeCharLower ) < 0 )
                    FormatterStdImpl<TChar>::phaCutContent=  phaExtPrecision;

                if( phaExtPrecision >=0 && StringConstFJS<TChar>::cCtTd.IndexOf( FormatterStdImpl<TChar>::phaTypeCode ) >= 0 )
                    throw Exception(ALIB_CALLER_NULLED, Exceptions::NoPrecisionWithConversion,
                                    phaExtPrecision, FormatterStdImpl<TChar>::phaTypeCode,
                                    FormatterStdImpl<TChar>::formatString, FormatterStdImpl<TChar>::formatString.Length() - FormatterStdImpl<TChar>::parser.Length() - 1 );

                     if( FormatterStdImpl<TChar>::phaTypeCode == 'X' || FormatterStdImpl<TChar>::phaTypeCode == 'H' )   FormatterStdImpl<TChar>::phaNF.HexLowerCase= false;
                else if( FormatterStdImpl<TChar>::phaTypeCode == 'x' || FormatterStdImpl<TChar>::phaTypeCode == 'h' )   FormatterStdImpl<TChar>::phaNF.HexLiteralPrefix= FormatterBase<TChar>::AlternativeNumberFormat.HexLiteralPrefix;


                switch ( typeCharLower )
                {
                    case 's':   phaExtPrecision=   -1;
                                break;

                    case 'b':   FormatterStdImpl<TChar>::phaType=  FormatterStdImpl<TChar>::PHType::Bool;                  break;
                    case 'c':   FormatterStdImpl<TChar>::phaType=  FormatterStdImpl<TChar>::PHType::Character;             break;
                    case 'd':   FormatterStdImpl<TChar>::phaType=  FormatterStdImpl<TChar>::PHType::IntBase10;             break;
                    case 'o':   FormatterStdImpl<TChar>::phaType=  FormatterStdImpl<TChar>::PHType::IntOctal;              break;
                    case 'x':   FormatterStdImpl<TChar>::phaType=  FormatterStdImpl<TChar>::PHType::IntHex;                break;
                    case 'h':   FormatterStdImpl<TChar>::phaType=  FormatterStdImpl<TChar>::PHType::IntHex;                break;
                    case 'e':   FormatterStdImpl<TChar>::phaType=  FormatterStdImpl<TChar>::PHType::Float;
                                FormatterStdImpl<TChar>::phaNF.ForceScientific=  true;            break;
                    case 'g':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::Float;                  break;
                    case 'f':   FormatterStdImpl<TChar>::phaType= FormatterStdImpl<TChar>::PHType::Float;
                                FormatterStdImpl<TChar>::phaNF.IntegralPartMinimumWidth= 1;
                                phaExtDefaultPrecision=        -1;       break;

                    case 't':   phaExtDateTime= FormatterStdImpl<TChar>::parser.CharAtStart();
                                FormatterStdImpl<TChar>::parser.ConsumeChars( 1 );
                                switch( phaExtDateTime )
                                {
                                    case 'H': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::HH  ; break;
                                    case 'k': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::H   ; break;
                                    case 'I': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::KK  ; break;
                                    case 'l': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::K   ; break;
                                    case 'M': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::mm  ; break;
                                    case 'S': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::ss  ; break;
                                    // not supported: case 'L': ;
                                    // not supported: case 'N': ;
                                    // not supported: case 'p': ;
                                    // not supported: case 'z': ;
                                    // not supported: case 'Z': ;
                                    // not supported: case 's': ;
                                    // not supported: case 'Q': ;
                                    case 'B': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::MMMM;         break;
                                    case 'b': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::MMM;          break;
                                    case 'h':
                                    case 'A': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::dddd;         break;
                                    case 'a': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::ddd;          break;
                                    // not supported: case 'C': ;
                                    case 'Y': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::yyyy;         break;
                                    case 'y': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::yy;           break;
                                    // not supported: case 'j': ;
                                    case 'm': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::MM;           break;
                                    case 'd': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::dd;           break;
                                    case 'e': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::d;            break;

                                    case 'R': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::HH_mm;        break;
                                    case 'T': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::HH_mm_ss;     break;
                                    // not supported: case 'r': ;

                                    case 'D': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::MM_dd_yy;     break;
                                    case 'F': FormatterStdImpl<TChar>::phaFormatSpec= StringConstFJS<TChar>::yyyy_MM_dd;   break;
                                    // not supported: case 'c': ;

                                    default:
                                        throw Exception(ALIB_CALLER_NULLED, Exceptions::UnknownDateTimeConversionSuffix,
                                                        phaExtDateTime,
                                                        FormatterStdImpl<TChar>::formatString, FormatterStdImpl<TChar>::formatString.Length() - FormatterStdImpl<TChar>::parser.Length() - 1 );
                                }
                                break;

                    default:
                        throw Exception(ALIB_CALLER_NULLED, Exceptions::UnknownConversionJS,
                                        FormatterStdImpl<TChar>::phaTypeCode,
                                        FormatterStdImpl<TChar>::formatString, FormatterStdImpl<TChar>::formatString.Length() - FormatterStdImpl<TChar>::parser.Length() - 1 );
                }

                NEXTSTATE(TYPE_SUFFIX);
            }

            case TYPE_SUFFIX:
            {
                NEXTSTATE(END);
            }

            case END:
                //FormatterStdImpl<TChar>::parser.ConsumeChars(1);
                return true;

        } // state switch

    } // read loop

}

template<typename TChar>
bool    FormatterJavaStyleBase<TChar>::preAndPostProcess( integer startIdx, AStringBase<TChar>* target )
{
    if( startIdx >= 0 && phaExtConversionUpper && target == nullptr )
        FormatterStdImpl<TChar>::targetString->ToUpper( startIdx );
    return true;
}

template<typename TChar>
bool  FormatterJavaStyleBase<TChar>::checkStdFieldAgainstArgument()
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

    return result;
}

//! @cond NO_DOX

template                              FormatterJavaStyleBase<char   >::FormatterJavaStyleBase();
template  FormatterStdImpl<char   >*  FormatterJavaStyleBase<char   >::Clone();
template  void                        FormatterJavaStyleBase<char   >::resetPHAs();
template  integer                     FormatterJavaStyleBase<char   >::findPlaceholder();
template  bool                        FormatterJavaStyleBase<char   >::parsePlaceholder();
template  bool                        FormatterJavaStyleBase<char   >::parseStdFormatSpec();
template  void                        FormatterJavaStyleBase<char   >::replaceEscapeSequences(integer);
template  bool                        FormatterJavaStyleBase<char   >::preAndPostProcess(integer,AStringBase<char>*);
template  bool                        FormatterJavaStyleBase<char   >::checkStdFieldAgainstArgument();

template                              FormatterJavaStyleBase<wchar_t>::FormatterJavaStyleBase();
template  FormatterStdImpl<wchar_t>*  FormatterJavaStyleBase<wchar_t>::Clone();
template  void                        FormatterJavaStyleBase<wchar_t>::resetPHAs();
template  integer                     FormatterJavaStyleBase<wchar_t>::findPlaceholder();
template  bool                        FormatterJavaStyleBase<wchar_t>::parsePlaceholder();
template  bool                        FormatterJavaStyleBase<wchar_t>::parseStdFormatSpec();
template  void                        FormatterJavaStyleBase<wchar_t>::replaceEscapeSequences(integer);
template  bool                        FormatterJavaStyleBase<wchar_t>::preAndPostProcess(integer,AStringBase<wchar_t>*);
template  bool                        FormatterJavaStyleBase<wchar_t>::checkStdFieldAgainstArgument();

//! @endcond

}}}} // namespace [aworx::lib::strings::format]
