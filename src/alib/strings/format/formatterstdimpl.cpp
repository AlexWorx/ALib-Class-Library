// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER_STD)
    #include "alib/strings/format/formatterstdimpl.hpp"
#endif

#if !defined (_GLIBCXX_CMATH) && !defined (_CMATH_)
    #include <cmath>
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
    template<typename TChar>    struct StringConstFSI {};
    template<>   struct StringConstFSI<char>
    {
        static const NTString e  ;
        static const NTString nan;
        static const NTString inf;
    };
    template<>   struct StringConstFSI<wchar_t>
    {
        static const WTString e  ;
        static const WTString nan;
        static const WTString inf;
    };
const NTString StringConstFSI<char   >::e     =  "e";
const NTString StringConstFSI<char   >::nan   =  "nan";
const NTString StringConstFSI<char   >::inf   =  "inf";

const WTString StringConstFSI<wchar_t>::e     = L"e";
const WTString StringConstFSI<wchar_t>::nan   = L"nan";
const WTString StringConstFSI<wchar_t>::inf   = L"inf";

} // anonymous namespace
//! @endcond
ALIB_WARNINGS_RESTORE

template<typename TChar>
FormatterStdImpl<TChar>::FormatterStdImpl( const StringBase<TChar>& formatterClassName )
: formatterName( formatterClassName )
{
    FormatterBase<TChar>::AlternativeNumberFormat.SetFromLocale();

    FormatterBase<TChar>::AlternativeNumberFormat.ExponentSeparator=  StringConstFSI<TChar>::e;
    FormatterBase<TChar>::AlternativeNumberFormat.NANLiteral       =  StringConstFSI<TChar>::nan;
    FormatterBase<TChar>::AlternativeNumberFormat.INFLiteral       =  StringConstFSI<TChar>::inf;
}

template<typename TChar>
int  FormatterStdImpl<TChar>::format( AStringBase<TChar>&        pTargetString,
                                      const StringBase<TChar>&   pFormatString,
                                      const Boxes&               pArguments,
                                      int                        pArgOffset        )
{
    // check for newline. This indicates that this is not a formatString string
    if( pFormatString.IndexOf('\n') >= 0 )
        return 0;

    // save parameters/init state
    targetString=             &pTargetString;
    targetStringStartLength=  pTargetString.Length();
    formatString=             &pFormatString;
    arguments=                &pArguments;
    argOffset=                 pArgOffset;

    // initialize state info
    nextAutoIdx=        0;
    argsConsumed=       0;

    parser=             formatString;

    for(;;)
    {
        integer actLength= pTargetString.Length();

        // find start of esc
        integer escStart= findPlaceholder();
        if ( escStart < 0 )
        {
            // write rest of formatString string (only if we had consumed before)
            if( argsConsumed > 0 && parser.Length() > 0 )
            {
                pTargetString.template _<false>( parser );
                replaceEscapeSequences( actLength );
            }
            return argsConsumed;
        }

        // write string before ESC code
        if ( escStart == 0 )
            parser.template ConsumeChars<false>(1);
        else
        {
            parser.template ConsumeChars<false, CurrentData::Keep >( escStart, pTargetString, 1 );
            replaceEscapeSequences( actLength );
        }

        // get and clean field attributes
        resetPHAs();

        // invoke abstract method for parsing the attributes
        if ( !parsePlaceholder() )
            return argsConsumed;

        // If no position was set in the field format string, automatically use next parameter
        if ( phaArgumentIdx < 0 )
            if ( !setArgument( -1 ) )
                return argsConsumed;
        ALIB_ASSERT( phaArgument != nullptr );



        // write field
        if( preAndPostProcess( -1 ) )
        {
            integer actIdx= targetString->Length();
            if ( !writeCustomFormat() )
            {
                // standard format
                if (    ( phaFormatSpec.IsNotEmpty()  && !parseStdFormatSpec() )
                     || !checkStdFieldAgainstArgument() )
                    return argsConsumed;

                // write argument
                writeStdArgument();
            }
            preAndPostProcess( actIdx );
        }
    }// main loop searching next escape sequence
}

template<typename TChar>
void FormatterStdImpl<TChar>::resetPHAs()
{
    phaNF.Set( &(FormatterBase<TChar>::DefaultNumberFormat) );
    phaNF.HexLowerCase          = true;
    phaType=                    PHType::NotGiven;
    phaPreviousArgumentIdx=     phaArgumentIdx;
    phaArgument=                nullptr;
    phaWidth=                    0;
    phaArgumentIdx=
    phaCutContent=              -1;

    phaAlignmentSpecified=      false;
    phaAlignment=               Alignment::Left;
    phaSignPaddingMode=         false;
    phaFillChar=                 ' ';
    phaWriteBinOctHexPrefix=    false;
    phaIsPercentage=            false;
    phaTypeCode=                '\0';
    phaTypeCodePosition=        -1;
    phaFormatSpec=              nullptr;
}

template<typename TChar>
bool    FormatterStdImpl<TChar>::setArgument( int pos )
{
    if( argumentCountStartsWith1 )
    {
        if( pos == 0 )
        throw Exception(ALIB_CALLER_NULLED, Exceptions::ArgumentIndexIs0,
                        formatString, formatString.Length() - parser.Length() - 2 );

        if( pos > 0 )
            pos--;
    }

    // auto? If not, set auto to this pos + 1
    phaArgumentIdx= ( pos >= 0)  ?  pos : nextAutoIdx++;

    // store maximum value used
    if ( argsConsumed - 1 < phaArgumentIdx )
        argsConsumed= phaArgumentIdx + 1;

    // get corresponding argument
    size_t  argIdx= static_cast<size_t>( argOffset + phaArgumentIdx );
    if( argIdx >= arguments->size() )
        throw Exception(ALIB_CALLER_NULLED, Exceptions::ArgumentIndexOutOfBounds,
                        argIdx    + argumentCountStartsWith1,
                        arguments->size() - static_cast<size_t>( argOffset ),
                        formatString, formatString.Length() - parser.Length() - 1 );

    phaArgument= &(*arguments)[argIdx];
    return true;


}

template<typename TChar>
bool    FormatterStdImpl<TChar>::checkStdFieldAgainstArgument()
{
    const Box* arg= phaArgument;

    if( phaTypeCodePosition < 0 )
        phaTypeCodePosition= formatString.Length() - parser.Length() - 1;


    // types bool and hashcode always works!
    if(   phaType == PHType::Bool
       || phaType == PHType::HashCode )
        return true;


    if ( arg->IsType<double>() )
    {
        if( phaType == PHType::NotGiven )
            phaType= PHType::Float;

        if(  phaType == PHType::Float )
            return true;

        throw Exception( ALIB_CALLER_NULLED, Exceptions::IncompatibleTypeCode,
                         phaTypeCode, phaArgumentIdx + argumentCountStartsWith1,
                         "floating point",
                         formatString, phaTypeCodePosition                           );
    }

    if(    arg->IsType<boxed_int >()
        || arg->IsType<boxed_uint>() )
    {
        if( phaType == PHType::NotGiven )
            phaType= PHType::IntBase10;
        if(     phaType == PHType::IntBase10
            ||  phaType == PHType::IntBinary
            ||  phaType == PHType::IntOctal
            ||  phaType == PHType::IntHex
            ||  phaType == PHType::Float
            ||  phaType == PHType::Character
            ||  phaType == PHType::Fill
            )
        {
            return true;
        }
        throw Exception(ALIB_CALLER_NULLED, Exceptions::IncompatibleTypeCode,
                        phaTypeCode, phaArgumentIdx + argumentCountStartsWith1,
                        "integer",
                         formatString, phaTypeCodePosition                           );

    }

    if(   arg->IsType<char   >()
       || arg->IsType<wchar_t>() )
    {
        if( phaType == PHType::NotGiven )
            phaType=   PHType::Character;
        if( phaType == PHType::Character )
            return true;

        throw Exception(ALIB_CALLER_NULLED, Exceptions::IncompatibleTypeCode,
                        phaTypeCode, phaArgumentIdx + argumentCountStartsWith1,
                        "character",
                         formatString, phaTypeCodePosition                           );
    }

    if( phaType == PHType::NotGiven )
        phaType= PHType::String;

    return true;
}

template<typename TChar>
void    FormatterStdImpl<TChar>::writeStdArgument()
{
    // store actual target length to fix some float exceptional cases (inf, nan)
    integer oldTargetLength= targetString->Length();

    // write to temp buffer first, if we have a field width given
    AStringBase<TChar>* target;

    if ( phaWidth > 0 )
    {
        target= &(fieldBuffer._());

        // set default alignment
        if(     !phaAlignmentSpecified
             &&  phaType != PHType::String
             &&  phaType != PHType::Character        )
             phaAlignment= Alignment::Right;
    }
    else
        target= targetString;

    integer fieldStartIdx= target->Length();


    // the main switch over the type
    switch( phaType )
    {
        case PHType::NotGiven:
            ALIB_ERROR( ASTR("Internal error: this should have been handled by method checkStdFieldAgainstArgument") );
        break;

        case PHType::String:
            phaArgument->Invoke<boxing::IApply<TChar>>( *target );
        break;

        case PHType::Bool:
            target->template _<false>( phaArgument->Invoke<IIsTrue, bool>() ? "true" : "false" );
        break;

        case PHType::Character:
        {
            wchar_t wc= 0;
                 if (phaArgument->IsType<char      >()) wc= static_cast<wchar_t>( phaArgument->Unbox<char      >() );
            else if (phaArgument->IsType<wchar_t   >()) wc=                       phaArgument->Unbox<wchar_t   >()  ;
            else if (phaArgument->IsType<boxed_int >()) wc= static_cast<wchar_t>( phaArgument->Unbox<boxed_int >() );
            else if (phaArgument->IsType<boxed_uint>()) wc= static_cast<wchar_t>( phaArgument->Unbox<boxed_uint>() );

            if ( wc == 0)
                wc= L'?';
            target->template _<false>( wc );
        }
        break;

        case PHType::Fill:
        {
            integer qty=  phaArgument->IsType<boxed_int>() ? static_cast<integer>( phaArgument->Unbox<boxed_int >() )
                                                           : static_cast<integer>( phaArgument->Unbox<boxed_uint>() );

            target->InsertChars( phaFillChar, qty );
        }
        break;


        case PHType::IntBase10:
        {
            if (phaSignPaddingMode)
                phaNF.DecMinimumFieldWidth= phaWidth;

            if( phaArgument->IsType<boxed_int>())
                target->template _<false>( strings::FormatBase<TChar>( phaArgument->Unbox<boxed_int>(),   &phaNF ) );
            else
                target->template _<false>( strings::FormatBase<TChar>( phaArgument->Unbox<boxed_uint>(),  &phaNF ) );
        }
        break;


        case PHType::IntBinary:
        case PHType::HashCode:
        case PHType::IntHex:
        case PHType::IntOctal:
        {
            int digits= phaWidth;
            if( phaWriteBinOctHexPrefix )
            {
                target->template _<false>( phaType == PHType::IntOctal  ? phaNF.OctLiteralPrefix :
                             phaType == PHType::IntBinary ? phaNF.BinLiteralPrefix :
                                                            phaNF.HexLiteralPrefix       );
                digits-= static_cast<int>((target->Length() - fieldStartIdx));
                if( phaWidth > 0 && digits <= 0 )
                {
                    target->SetLength( fieldStartIdx + phaWidth );
                    break; // stop output, no space left
                }
            }
            if ( digits < 0 )
                digits= 0;
                 if( phaType == PHType::IntOctal)  target->template _<false>( typename FormatBase<TChar>::Oct(  phaArgument->PeekRawValue(), digits, &phaNF ) );
            else if( phaType == PHType::IntBinary) target->template _<false>( typename FormatBase<TChar>::Bin(  phaArgument->PeekRawValue(), digits, &phaNF ) );
            else                                   target->template _<false>( typename FormatBase<TChar>::Hex(  phaArgument->PeekRawValue(), digits, &phaNF ) );



        }
        break;

        case PHType::Float:
        {
            // get value
            double value=    phaArgument->IsType<double>()    ?                      phaArgument->Unbox<double>    ()
                          :  phaArgument->IsType<boxed_int>() ? static_cast<double>( phaArgument->Unbox<boxed_int> () )
                          :                                     static_cast<double>( phaArgument->Unbox<boxed_uint>() );
            if( phaIsPercentage )
                value*= 100.0;

            if (phaSignPaddingMode)
            {
                auto classification=  std::fpclassify(value);

                // write sign upfront and set fill character to 0
                if( classification != FP_NAN  )
                {
                    bool negative= std::signbit(value);
                    if( classification == FP_ZERO && negative )
                    {
                        value= 0.0;
                        negative= false;
                    }

                    if( negative )
                    {
                        targetString->template _<false>( '-' );
                        phaWidth--;
                        value= -value;
                    }
                    else if( phaNF.PlusSign != '\0' )
                    {
                        targetString->template _<false>( phaNF.PlusSign );
                        phaWidth--;
                    }
                    phaNF.PlusSign= '\0';

                    if ( !phaNF.WriteGroupChars || phaNF.ThousandsGroupChar == '\0')
                        phaFillChar= '0';
                    else
                    {
                        // calculate integral part width
                        if ( phaWidth > 0 && !phaNF.ForceScientific)
                        {
                            phaNF.IntegralPartMinimumWidth= phaWidth - 1; // -1 == the dot

                            if( phaNF.FractionalPartWidth >= 0 )
                                phaNF.IntegralPartMinimumWidth-= phaNF.FractionalPartWidth;

                            if( phaIsPercentage )
                                phaNF.IntegralPartMinimumWidth--;

                            // check
                            if( phaNF.IntegralPartMinimumWidth <= 0)
                                phaNF.IntegralPartMinimumWidth= 1;
                        }
                    }
                }

            }

            target->template _<false>( strings::FormatBase<TChar>( value, &phaNF ) );

            if( phaIsPercentage )
                target->template _<false>( '%' );


            // if nan or inf was written, we fill with spaces
            if (     phaSignPaddingMode
                && (     target->IndexOf(phaNF.NANLiteral, oldTargetLength) >= 0
                     ||  target->IndexOf(phaNF.INFLiteral, oldTargetLength) >= 0 ) )
                phaFillChar= ' ';

        }
        break;
    }

    // now do an 'intermediate post phase' processing
    preAndPostProcess( fieldStartIdx, target );

    // apply cutting
    if ( phaCutContent >= 0 )
    {
        // wchar compilation: we grant the length is the same as the number of wide characters
        // added, although this is still not true for some unicode character combinations
        if( std::is_same<TChar, wchar_t>::value )
        {
            // too much added?
            if( target->Length() - oldTargetLength > phaCutContent )
                target->SetLength( oldTargetLength + phaCutContent );
        }
        else
        {
            integer qtyWCharsAdded= target->StringBase<TChar>::template Substring<false>( oldTargetLength, target->Length() - oldTargetLength ).WStringLength();

            // too much added?
            if( qtyWCharsAdded > phaCutContent )
            {
                // was not unicode?
                if( qtyWCharsAdded == target->Length() - oldTargetLength )
                    target->SetLength( oldTargetLength + phaCutContent );

                // in the unicode case, it gets complicated: we have to convert to unicode and
                // then convert a part of it back!
                else if( qtyWCharsAdded < 256)
                {
                    WString256 wBuf;
                    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( wBuf,  ReplaceExternalBuffer);
                    wBuf.template Append<false>( target->Buffer() + oldTargetLength, target->Length() - oldTargetLength );
                    target->SetLength( oldTargetLength );
                    target->template Append<false>( wBuf.Buffer(),phaCutContent );
                }
            }
        }
    }


    // if field mode, we have to apply the field buffer as a field to the real target now
    if( target ==  &fieldBuffer )
        targetString->template _<false>( typename FormatBase<TChar>::Field( fieldBuffer, phaWidth, phaAlignment, phaFillChar ) );

}

template<typename TChar>
bool    FormatterStdImpl<TChar>::writeCustomFormat()
{
    if( phaFormatSpec.IsEmpty() ||  !phaArgument->HasInterface<boxing::IFormat<TChar>>() )
        return false;

    phaArgument->Invoke<boxing::IFormat<TChar>>( phaFormatSpec, *targetString );
    return true;
}

//! @cond NO_DOX
template          FormatterStdImpl<nchar>::FormatterStdImpl( const NString&);
template int      FormatterStdImpl<nchar>::format( NAString&, const NString&, const Boxes&, int);
template void     FormatterStdImpl<nchar>::resetPHAs();
template bool     FormatterStdImpl<nchar>::setArgument(int);
template bool     FormatterStdImpl<nchar>::checkStdFieldAgainstArgument();
template void     FormatterStdImpl<nchar>::writeStdArgument();
template bool     FormatterStdImpl<nchar>::writeCustomFormat();

template          FormatterStdImpl<wchar>::FormatterStdImpl( const WString&);
template int      FormatterStdImpl<wchar>::format( WAString&, const WString&, const Boxes&, int);
template void     FormatterStdImpl<wchar>::resetPHAs();
template bool     FormatterStdImpl<wchar>::setArgument(int);
template bool     FormatterStdImpl<wchar>::checkStdFieldAgainstArgument();
template void     FormatterStdImpl<wchar>::writeStdArgument();
template bool     FormatterStdImpl<wchar>::writeCustomFormat();
//! @endcond



}}}} // namespace [aworx::lib::strings::format]
