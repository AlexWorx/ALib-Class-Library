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
#include "alib/alib.inl"
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Format.FormatterJavaStyle;
    import   ALib.Lang;
    import   ALib.Characters.Functions;
    import   ALib.EnumOps;
    import   ALib.Exceptions;
#   if ALIB_CAMP
      import ALib.Camp.Base;
#   endif
#else
#   include "ALib.Format.FormatterJavaStyle.H"
#   include "ALib.Lang.H"
#   include "ALib.Exceptions.H"
#   include "ALib.Characters.Functions.H"
#   include "ALib.Camp.Base.H"
#endif
//========================================== Implementation ========================================
using namespace alib::strings;
namespace alib::format {


FormatterJavaStyle::FormatterJavaStyle()
: FormatterStdImpl( A_CHAR( "FormatterJavaStyle" ) ) {
    // arguments are counted starting with 1.
    argumentCountStartsWith1 = true;

    // set number format to Java defaults
    DefaultNumberFormat.Flags                  -= NumberFormatFlags::ForceDecimalPoint;
    DefaultNumberFormat.Flags                  += NumberFormatFlags::WriteExponentPlusSign;

    // set number format to Java defaults
    DefaultNumberFormat    .HexLiteralPrefix   = A_CHAR( "0X" );

    DefaultNumberFormat    .OctLiteralPrefix   =
    AlternativeNumberFormat.OctLiteralPrefix   = A_CHAR( "0"  );

    DefaultNumberFormat    .NANLiteral         =
    AlternativeNumberFormat.NANLiteral         = A_CHAR( "NaN" );
    DefaultNumberFormat    .INFLiteral         = A_CHAR( "INFINITY" );
    AlternativeNumberFormat.INFLiteral         = A_CHAR( "Infinity" );
}


SPFormatter   FormatterJavaStyle::Clone() {
    SPFormatter clone;
    clone.InsertDerived<FormatterJavaStyle>();
    
    // create a clone of #Next, in the case that next is derived from std base class
    if( Next )
        clone->Next= Next->Clone();

    // copy my settings, that's it
    clone->CloneSettings( *this );
    
    return clone;
}



void FormatterJavaStyle::resetPlaceholder() {
    // first invoke parent's setting...
    FormatterStdImpl::resetPlaceholder();

    // ...then make some "Java like" adjustments
    placeholderJS.ConversionUpper               = false;
    placeholder.ValueAlignment                  = lang::Alignment::Right;

    placeholder.NF.Flags                        -= NumberFormatFlags::ForceDecimalPoint;
    placeholder.NF.Flags                        -= NumberFormatFlags::OmitTrailingFractionalZeros;

    placeholderJS.AlternateForm                 = false;

    placeholderJS.Precision                     = -1;
    placeholderJS.DefaultPrecision              = 6;
}


integer FormatterJavaStyle::findPlaceholder() {
    integer idx= 0;
    while(     (idx= parser.IndexOf('%', idx )) >= 0
           &&  (    parser.CharAt( idx + 1 ) == '%'
                 || parser.CharAt( idx + 1 ) == 'n'      ) )
    {
        // double ESC found (escaped %% or %n)
        idx+= + 2;
    }
    return idx;
}


void    FormatterJavaStyle::writeStringPortion( integer length ) {
    if( length == 0)
        return;

    targetString->EnsureRemainingCapacity( length );
    auto* src = parser.Buffer();
    auto* dest= targetString->VBuffer() + targetString->Length();
    parser.ConsumeChars( length );

    character c1;
    character c2= *src;
    while( length > 1 ) {
        c1= c2;
        c2= *++src;

        if(     ( c1 == '%' && c2 =='%')
            ||    c1 == '\\'                     )
        {
            if(  c1 == '\\' )
                switch(c2) {
                    case 'r': c1= '\r' ; break;
                    case 'n': c1= '\n' ; break;
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
        else if( c1 == '%' && c2 =='n' ) {
            c1= '\n';
            ++src;
            --length;
        }

        *dest++= c1;
        --length;
    }

    // copy last character and adjust target string length:
    // Note: length usually is 1. Only if last character is an escape sequence, it is 0.
    if( length == 1)
        *dest= *src;
    targetString->SetLength( dest - targetString->VBuffer() + length);
}



bool FormatterJavaStyle::parsePlaceholder() {
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

    while( true ) {
        // switch over state. With 'break' we consume one character (kind of success) while
        // with 'continue' we keep the current character (and go to another state)
        switch ( state ) {
            case POSITION:
            {
                int argNo= -1;
                if ( parser.ConsumeChar('<') ) {
                    argNo= placeholder.PreviousArgIdx + 1;
                } else {
                    integer i= 0;
                    while(    i < parser.Length()
                           && isdigit( parser.CharAt<NC>(i) ) )
                         ++i;

                    if(  i > 0 &&  parser.CharAt<NC>(i) == '$') {
                        parser.ConsumeDecDigits( argNo );    ALIB_ASSERT_RESULT_EQUALS(
                        parser.ConsumeChar('$')                                        , true )
                }   }
                if( argNo >= 0 )
                    setArgument( argNo );

                NEXTSTATE(FLAGS)
            }

            case FLAGS:
            {
                bool flagsDone= false;
                while(!flagsDone) {
                    switch ( parser.CharAtStart() ) {
                        case '-':
                            placeholder.ValueAlignment= lang::Alignment::Left;
                            placeholder.AlignmentSpecified= true;
                            break;

                        case '^':
                            placeholder.ValueAlignment= lang::Alignment::Center;
                            placeholder.AlignmentSpecified= true;
                            break;

                        case '#':
                        {
                            placeholderJS.AlternateForm= true;
                            placeholder.WriteBinOctHexPrefix=             true;
                            placeholder.NF.Flags                          += NumberFormatFlags::ForceDecimalPoint;
                            placeholder.NF.Flags                          -= NumberFormatFlags::OmitTrailingFractionalZeros;
                            break;
                        }

                        case '+':
                        case ' ':
                            placeholder.NF.PlusSign= parser.CharAtStart();
                            break;

                        case '0':
                            placeholder.SignPaddingMode= true;
                            break;

                        case ',':
                            placeholder.NF.Flags += NumberFormatFlags::WriteGroupChars;
                            break;

                        case '(':
                            throw Exception(ALIB_CALLER_NULLED, FMTExceptions::NegativeValuesInBracketsNotSupported,
                                            formatString, formatString.Length() - parser.Length() );

                        default:
                            flagsDone= true;
                            break;
                    }

                    if( !flagsDone )
                        parser.ConsumeChars( 1 );
                }
                NEXTSTATE(WIDTH)
            }


            case WIDTH:
            {
                if( isdigit( parser.CharAtStart() ) )
                    parser.ConsumeDecDigits( placeholder.Width );

                NEXTSTATE(PRECISION)
            }

            case PRECISION:
            {
                if( parser.ConsumeChar( '.' ) && !parser.ConsumeDecDigits( placeholderJS.Precision ) )
                    throw Exception(ALIB_CALLER_NULLED, FMTExceptions::MissingPrecisionValueJS,
                                    formatString, formatString.Length() - parser.Length() );

                NEXTSTATE(TYPE)
            }

            case TYPE:
            {
                placeholder.TypeCode=  parser.CharAtStart();
                parser.ConsumeChars( 1 );

                character typeCharLower= characters::ToLower( placeholder.TypeCode );

                if ( typeCharLower == 'a' ) {
                    throw Exception(ALIB_CALLER_NULLED, FMTExceptions::HexadecimalFloatFormatNotSupported,
                                    formatString, formatString.Length() - parser.Length() - 1 );
                }

                if( placeholderJS.AlternateForm && String( A_CHAR( "sSbBhHgGcCtT" ) ).IndexOf( placeholder.TypeCode ) >= 0 )
                    throw Exception(ALIB_CALLER_NULLED, FMTExceptions::NoAlternateFormOfConversion,
                                    placeholder.TypeCode,
                                    formatString, formatString.Length() - parser.Length() - 1 );

                if( String(A_CHAR( "seg" )).IndexOf( placeholder.TypeCode ) >= 0 ) {
                    if( placeholder.TypeCode != 's' )
                        placeholder.NF.ExponentSeparator= AlternativeNumberFormat.ExponentSeparator;
                    placeholder.NF.INFLiteral=            AlternativeNumberFormat.INFLiteral;
                    placeholder.NF.NANLiteral=            AlternativeNumberFormat.NANLiteral;
                }

                if( String(A_CHAR( "SBCT" )).IndexOf( placeholder.TypeCode ) >= 0 )
                    placeholderJS.ConversionUpper= true;

                if( NCString("egf").IndexOf( static_cast<nchar>(typeCharLower) ) < 0 )
                    placeholder.CutContent=  placeholderJS.Precision;

                if( placeholderJS.Precision >=0 && String(A_CHAR( "cCtTd" )).IndexOf( placeholder.TypeCode ) >= 0 )
                    throw Exception(ALIB_CALLER_NULLED, FMTExceptions::NoPrecisionWithConversion,
                                    placeholderJS.Precision, placeholder.TypeCode,
                                    formatString, formatString.Length() - parser.Length() - 1 );

                     if( placeholder.TypeCode == 'X' || placeholder.TypeCode == 'H' )   placeholder.NF.Flags -= NumberFormatFlags::HexLowerCase;
                else if( placeholder.TypeCode == 'x' || placeholder.TypeCode == 'h' )   placeholder.NF.HexLiteralPrefix= AlternativeNumberFormat.HexLiteralPrefix;


                switch ( typeCharLower ) {
                    case 's':   placeholderJS.Precision=   -1;
                                break;

                    case 'b':   placeholder.Type= PHTypes::Bool;                  break;
                    case 'c':   placeholder.Type= PHTypes::Character;             break;
                    case 'd':   placeholder.Type= PHTypes::IntBase10;             break;
                    case 'o':   placeholder.Type= PHTypes::IntOctal;              break;
                    case 'x':   placeholder.Type= PHTypes::IntHex;                break;
                    case 'h':   placeholder.Type= PHTypes::IntHex;                break;
                    case 'e':   placeholder.Type= PHTypes::Float;
                                placeholder.NF.Flags+= NumberFormatFlags::ForceScientific;   break;
                    case 'g':   placeholder.Type= PHTypes::Float;                 break;
                    case 'f':   placeholder.Type= PHTypes::Float;
                                placeholder.NF.IntegralPartMinimumWidth=  1;
                                placeholderJS.DefaultPrecision         = -1;      break;

                    case 't':   placeholderJS.DateTime= parser.CharAtStart();
                                parser.ConsumeChars( 1 );
                                switch( placeholderJS.DateTime ) {
                                    case 'H': placeholder.FormatSpec= A_CHAR( "HH" )  ; break;
                                    case 'k': placeholder.FormatSpec= A_CHAR( "H"  )  ; break;
                                    case 'I': placeholder.FormatSpec= A_CHAR( "KK" )  ; break;
                                    case 'l': placeholder.FormatSpec= A_CHAR( "K"  )  ; break;
                                    case 'M': placeholder.FormatSpec= A_CHAR( "mm" )  ; break;
                                    case 'S': placeholder.FormatSpec= A_CHAR( "ss" )  ; break;
                                    // not supported: case 'L': ;
                                    // not supported: case 'N': ;
                                    // not supported: case 'p': ;
                                    // not supported: case 'z': ;
                                    // not supported: case 'Z': ;
                                    // not supported: case 's': ;
                                    // not supported: case 'Q': ;
                                    case 'B': placeholder.FormatSpec= A_CHAR( "MMMM" );       break;
                                    case 'b': placeholder.FormatSpec= A_CHAR( "MMM"  );       break;
                                    case 'h':
                                    case 'A': placeholder.FormatSpec= A_CHAR( "dddd" );       break;
                                    case 'a': placeholder.FormatSpec= A_CHAR( "ddd"  );       break;
                                    // not supported: case 'C': ;
                                    case 'Y': placeholder.FormatSpec= A_CHAR( "yyyy" );       break;
                                    case 'y': placeholder.FormatSpec= A_CHAR( "yy"   );       break;
                                    // not supported: case 'j': ;
                                    case 'm': placeholder.FormatSpec= A_CHAR( "MM"   );       break;
                                    case 'd': placeholder.FormatSpec= A_CHAR( "dd"   );       break;
                                    case 'e': placeholder.FormatSpec= A_CHAR( "d"    );       break;

                                    case 'R': placeholder.FormatSpec= A_CHAR( "HH:mm" );      break;
                                    case 'T': placeholder.FormatSpec= A_CHAR( "HH:mm:ss" );   break;
                                    // not supported: case 'r': ;

                                    case 'D': placeholder.FormatSpec= A_CHAR( "MM/dd/yy" );   break;
                                    case 'F': placeholder.FormatSpec= A_CHAR( "yyyy-MM-dd" ); break;
                                    // not supported: case 'c': ;

                                    default:
                                        throw Exception(ALIB_CALLER_NULLED, FMTExceptions::UnknownDateTimeConversionSuffix,
                                                        placeholderJS.DateTime,
                                                        formatString, formatString.Length() - parser.Length() - 1 );
                                }
                                break;

                    default:
                        throw Exception(ALIB_CALLER_NULLED, FMTExceptions::UnknownConversionJS,
                                        placeholder.TypeCode,
                                        formatString, formatString.Length() - parser.Length() - 1 );
                }

                NEXTSTATE(TYPE_SUFFIX)
            }

            case TYPE_SUFFIX:
            {
                NEXTSTATE(END)
            }

            case END:
                //parser.ConsumeChars(1);
                return true;

            default: ALIB_ERROR( "FORMAT", "Illegal switch state." ) break;
        } // state switch

    } // read loop

}


bool    FormatterJavaStyle::preAndPostProcess( integer startIdx, AString* target ) {
    if( startIdx >= 0 && placeholderJS.ConversionUpper && target == nullptr )
        targetString->ToUpper( startIdx );
    return true;
}


bool  FormatterJavaStyle::checkStdFieldAgainstArgument() {
    bool wasFloat= placeholder.Type == PHTypes::Float;
    if( wasFloat ) {
        if ( placeholderJS.Precision >= 0 )
            placeholder.NF.FractionalPartWidth= placeholderJS.Precision;
        else if( placeholder.NF.FractionalPartWidth < 0 )
            placeholder.NF.FractionalPartWidth= placeholderJS.DefaultPrecision;
    }

    bool result= FormatterStdImpl::checkStdFieldAgainstArgument();

    if( !wasFloat && placeholder.Type == PHTypes::Float ) {
        if ( placeholderJS.Precision >= 0 )
            placeholder.NF.FractionalPartWidth= placeholderJS.Precision;
    }

    return result;
}

} // namespace [alib::format]
