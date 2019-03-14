// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/stringformat/formatterjavastyle.hpp"



using namespace aworx::lib::strings;
namespace aworx { namespace lib { namespace stringformat {


FormatterJavaStyle::FormatterJavaStyle()
: FormatterStdImpl( A_CHAR( "FormatterJavaStyle" ) )
{
    // arguments are counted starting with 1.
    argumentCountStartsWith1 = true;

    // set number format to Java defaults
    DefaultNumberFormat.ForceDecimalPoint       = false;
    DefaultNumberFormat.WriteExponentPlusSign   = true;

    // set number format to Java defaults
    DefaultNumberFormat    .HexLiteralPrefix   = A_CHAR( "0X" );

    DefaultNumberFormat    .OctLiteralPrefix   =
    AlternativeNumberFormat.OctLiteralPrefix   = A_CHAR( "0"  );

    DefaultNumberFormat    .NANLiteral         =
    AlternativeNumberFormat.NANLiteral         = A_CHAR( "NaN" );
    DefaultNumberFormat    .INFLiteral         = A_CHAR( "INFINITY" );
    AlternativeNumberFormat.INFLiteral         = A_CHAR( "Infinity" );
}


FormatterStdImpl*   FormatterJavaStyle::Clone()
{
    // create a clone
    FormatterJavaStyle* clone= new FormatterJavaStyle();

    // create a clone of #Next, in the case that next is derived from std base class
    if( Next )
        clone->Next.reset(Next->Clone());

    // copy my settings, that's it
    clone->CloneSettings( *this );
    return clone;
}



void FormatterJavaStyle::resetPlaceholder()
{
    // first invoke parent's setting...
    FormatterStdImpl::resetPlaceholder();

    // ...then make some "Java like" adjustments
    placeholderJS.ConversionUpper               = false;
    placeholder.ValueAlignment                  = Alignment::Right;

    placeholder.NF.ForceDecimalPoint            = false;
    placeholder.NF.OmitTrailingFractionalZeros  = false;

    placeholderJS.AlternateForm                 = false;

    placeholderJS.Precision                     = -1;
    placeholderJS.DefaultPrecision              = 6;
}


integer FormatterJavaStyle::findPlaceholder()
{
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


void    FormatterJavaStyle::replaceEscapeSequences( integer startIdx )
{
    targetString->SearchAndReplace( A_CHAR( "%%" ), A_CHAR( "%" ), startIdx );
    targetString->SearchAndReplace( A_CHAR( "%n" ), NewLine()    , startIdx );
    targetString->_<false>( strings::TFormat<character>::Escape( Switch::Off, startIdx ) );
}



bool FormatterJavaStyle::parsePlaceholder()
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
                if ( parser.ConsumeChar('<') )
                {
                    argNo= placeholder.PreviousArgIdx + 1;
                }
                else
                {
                    integer i= 0;
                    while(    i < parser.Length()
                           && isdigit( parser.CharAt<false>(i) ) )
                         ++i;

                    if(  i > 0 &&  parser.CharAt<false>(i) == '$')
                    {
                        parser.ConsumeDecDigits( argNo );
                        ALIB_ASSERT_RESULT_EQUALS( parser.ConsumeChar('$'), true );
                    }
                }
                if( argNo >= 0 )
                    setArgument( argNo );

                NEXTSTATE(FLAGS);
            }

            case FLAGS:
            {
                bool flagsDone= false;
                while(!flagsDone)
                {
                    switch ( parser.CharAtStart() )
                    {
                        case '-':
                            placeholder.ValueAlignment= Alignment::Left;
                            placeholder.AlignmentSpecified= true;
                            break;

                        case '^':
                            placeholder.ValueAlignment= Alignment::Center;
                            placeholder.AlignmentSpecified= true;
                            break;

                        case '#':
                        {
                            placeholderJS.AlternateForm= true;
                            placeholder.WriteBinOctHexPrefix=             true;
                            placeholder.NF.ForceDecimalPoint=             true;
                            placeholder.NF.OmitTrailingFractionalZeros=   false;
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
                            placeholder.NF.WriteGroupChars= true;
                            break;

                        case '(':
                            throw Exception(ALIB_CALLER_NULLED, Exceptions::NegativeValuesInBracketsNotSupported,
                                            formatString, formatString.Length() - parser.Length() );

                        default:
                            flagsDone= true;
                            break;
                    }

                    if( !flagsDone )
                        parser.ConsumeChars( 1 );
                }
                NEXTSTATE(WIDTH);
            }


            case WIDTH:
            {
                if( isdigit( parser.CharAtStart() ) )
                    parser.ConsumeDecDigits( placeholder.Width );

                NEXTSTATE(PRECISION);
            }

            case PRECISION:
            {
                if( parser.ConsumeChar( '.' ) && !parser.ConsumeDecDigits( placeholderJS.Precision ) )
                    throw Exception(ALIB_CALLER_NULLED, Exceptions::MissingPrecisionValueJS,
                                    formatString, formatString.Length() - parser.Length() );

                NEXTSTATE(TYPE);
            }

            case TYPE:
            {
                placeholder.TypeCode=  parser.CharAtStart();
                parser.ConsumeChars( 1 );

                nchar typeCharLower= static_cast<nchar>( tolower( placeholder.TypeCode ) );

                if ( typeCharLower == 'a' )
                {
                    throw Exception(ALIB_CALLER_NULLED, Exceptions::HexadecimalFloatFormatNotSupported,
                                    formatString, formatString.Length() - parser.Length() - 1 );
                }

                if( placeholderJS.AlternateForm && String( A_CHAR( "sSbBhHgGcCtT" ) ).IndexOf( placeholder.TypeCode ) >= 0 )
                    throw Exception(ALIB_CALLER_NULLED, Exceptions::NoAlternateFormOfConversion,
                                    placeholder.TypeCode,
                                    formatString, formatString.Length() - parser.Length() - 1 );

                if( String(A_CHAR( "seg" )).IndexOf( placeholder.TypeCode ) >= 0 )
                {
                    if( placeholder.TypeCode != 's' )
                        placeholder.NF.ExponentSeparator= AlternativeNumberFormat.ExponentSeparator;
                    placeholder.NF.INFLiteral=            AlternativeNumberFormat.INFLiteral;
                    placeholder.NF.NANLiteral=            AlternativeNumberFormat.NANLiteral;
                }

                if( String(A_CHAR( "SBCT" )).IndexOf( placeholder.TypeCode ) >= 0 )
                    placeholderJS.ConversionUpper= true;

                if( NCString("egf").IndexOf( typeCharLower ) < 0 )
                    placeholder.CutContent=  placeholderJS.Precision;

                if( placeholderJS.Precision >=0 && String(A_CHAR( "cCtTd" )).IndexOf( placeholder.TypeCode ) >= 0 )
                    throw Exception(ALIB_CALLER_NULLED, Exceptions::NoPrecisionWithConversion,
                                    placeholderJS.Precision, placeholder.TypeCode,
                                    formatString, formatString.Length() - parser.Length() - 1 );

                     if( placeholder.TypeCode == 'X' || placeholder.TypeCode == 'H' )   placeholder.NF.HexLowerCase= false;
                else if( placeholder.TypeCode == 'x' || placeholder.TypeCode == 'h' )   placeholder.NF.HexLiteralPrefix= AlternativeNumberFormat.HexLiteralPrefix;


                switch ( typeCharLower )
                {
                    case 's':   placeholderJS.Precision=   -1;
                                break;

                    case 'b':   placeholder.Type= PHTypes::Bool;                  break;
                    case 'c':   placeholder.Type= PHTypes::Character;             break;
                    case 'd':   placeholder.Type= PHTypes::IntBase10;             break;
                    case 'o':   placeholder.Type= PHTypes::IntOctal;              break;
                    case 'x':   placeholder.Type= PHTypes::IntHex;                break;
                    case 'h':   placeholder.Type= PHTypes::IntHex;                break;
                    case 'e':   placeholder.Type= PHTypes::Float;
                                placeholder.NF.ForceScientific=  true;            break;
                    case 'g':   placeholder.Type= PHTypes::Float;                 break;
                    case 'f':   placeholder.Type= PHTypes::Float;
                                placeholder.NF.IntegralPartMinimumWidth=  1;
                                placeholderJS.DefaultPrecision         = -1;      break;

                    case 't':   placeholderJS.DateTime= parser.CharAtStart();
                                parser.ConsumeChars( 1 );
                                switch( placeholderJS.DateTime )
                                {
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
                                        throw Exception(ALIB_CALLER_NULLED, Exceptions::UnknownDateTimeConversionSuffix,
                                                        placeholderJS.DateTime,
                                                        formatString, formatString.Length() - parser.Length() - 1 );
                                }
                                break;

                    default:
                        throw Exception(ALIB_CALLER_NULLED, Exceptions::UnknownConversionJS,
                                        placeholder.TypeCode,
                                        formatString, formatString.Length() - parser.Length() - 1 );
                }

                NEXTSTATE(TYPE_SUFFIX);
            }

            case TYPE_SUFFIX:
            {
                NEXTSTATE(END);
            }

            case END:
                //parser.ConsumeChars(1);
                return true;

        } // state switch

    } // read loop

}


bool    FormatterJavaStyle::preAndPostProcess( integer startIdx, AString* target )
{
    if( startIdx >= 0 && placeholderJS.ConversionUpper && target == nullptr )
        targetString->ToUpper( startIdx );
    return true;
}


bool  FormatterJavaStyle::checkStdFieldAgainstArgument()
{
    bool wasFloat= placeholder.Type == PHTypes::Float;
    if( wasFloat )
    {
        if ( placeholderJS.Precision >= 0 )
            placeholder.NF.FractionalPartWidth= placeholderJS.Precision;
        else if( placeholder.NF.FractionalPartWidth < 0 )
            placeholder.NF.FractionalPartWidth= placeholderJS.DefaultPrecision;
    }

    bool result= FormatterStdImpl::checkStdFieldAgainstArgument();

    if( !wasFloat && placeholder.Type == PHTypes::Float )
    {
        if ( placeholderJS.Precision >= 0 )
            placeholder.NF.FractionalPartWidth= placeholderJS.Precision;
    }

    return result;
}

}}} // namespace [aworx::lib::stringformat]
