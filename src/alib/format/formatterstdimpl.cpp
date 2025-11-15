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
#include <cmath>
#include "alib/alib.inl"
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Format.StdImplementation;
    import   ALib.Lang;
    import   ALib.EnumOps;
    import   ALib.Strings;
    import   ALib.Exceptions;
#   if ALIB_CAMP
      import ALib.Camp.Base;
#   endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.Exceptions.H"
#   include "ALib.Format.StdImplementation.H"
#   include "ALib.Camp.Base.H"
#endif
//========================================== Implementation ========================================
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

namespace alib::format {


FormatterStdImpl::FormatterStdImpl( const String& formatterClassName )
: formatterName( formatterClassName ) {
    Formatter::AlternativeNumberFormat.SetFromLocale();

    Formatter::AlternativeNumberFormat.ExponentSeparator=  A_CHAR( "e"   );
    Formatter::AlternativeNumberFormat.NANLiteral       =  A_CHAR( "nan" );
    Formatter::AlternativeNumberFormat.INFLiteral       =  A_CHAR( "inf" );

    argumentCountStartsWith1= false;
}

int  FormatterStdImpl::format( AString&        pTargetString,
                               const String&   pFormatString,
                               const BoxesMA&  pArguments,
                               int             pArgOffset        ) {
    // save parameters/init state
    targetString=             &pTargetString;
    targetStringStartLength=  pTargetString.Length();
    formatString=             pFormatString;
    parser      =             formatString;
    arguments   =             &pArguments;
    argOffset   =             pArgOffset;

    // initialize state info
    nextAutoIdx=        0;
    argsConsumed=       0;

    for(;;) {
        // find start of esc
        integer escStart= findPlaceholder();
        if ( escStart < 0 ) {
            // write rest of formatString string (only if we had consumed before)
            if( argsConsumed > 0)
                writeStringPortion( parser.Length() );

            return argsConsumed;
        }

        // write string before ESC code
        writeStringPortion( escStart );
        parser.template ConsumeChars<NC>(1);

        // get and clean field attributes
        resetPlaceholder();

        // invoke abstract method for parsing the attributes
        if ( !parsePlaceholder() )
            return argsConsumed;

        // If no position was set in the field format string, automatically use next parameter
        if ( placeholder.ArgIdx < 0 )
            if ( !setArgument( -1 ) )
                return argsConsumed;
        ALIB_ASSERT( placeholder.Arg != nullptr, "FORMAT" )



        // write field
        if( preAndPostProcess( -1 ) ) {
            integer actIdx= targetString->Length();
            if ( !writeCustomFormat() ) {
                // standard format
                if (    ( placeholder.FormatSpec.IsNotEmpty()  && !parseStdFormatSpec() )
                     || !checkStdFieldAgainstArgument() )
                    return argsConsumed;

                // write argument
                writeStdArgument();
            }
            preAndPostProcess( actIdx );
        }
    }// main loop searching next escape sequence
}


void FormatterStdImpl::resetPlaceholder() {
    placeholder.NF.Set( &(Formatter::DefaultNumberFormat) );
    placeholder.NF.Flags+= NumberFormatFlags::HexLowerCase;
    placeholder.Type                    = PHTypes::NotGiven;
    placeholder.PreviousArgIdx     = placeholder.ArgIdx;
    placeholder.Arg                     = nullptr;
    placeholder.Width                   =  0;
    placeholder.ArgIdx                  =
    placeholder.CutContent              = -1;

    placeholder.AlignmentSpecified      = false;
    placeholder.ValueAlignment          = lang::Alignment::Left;
    placeholder.SignPaddingMode         = false;
    placeholder.FillChar                = ' ';
    placeholder.WriteBinOctHexPrefix    = false;
    placeholder.IsPercentage            = false;
    placeholder.TypeCode                = '\0';
    placeholder.TypeCodePosition        = -1;
    placeholder.FormatSpec              = nullptr;
}


bool    FormatterStdImpl::setArgument( int pos ) {
    if( argumentCountStartsWith1 ) {
        if( pos == 0 )
        throw Exception(ALIB_CALLER_NULLED, FMTExceptions::ArgumentIndexIs0,
                        formatString, formatString.Length() - parser.Length() - 2 );

        if( pos > 0 )
            --pos;
    }

    // auto? If not, set auto to this pos + 1
    placeholder.ArgIdx= ( pos >= 0)  ?  pos : nextAutoIdx++;

    // store maximum value used
    if ( argsConsumed - 1 < placeholder.ArgIdx )
        argsConsumed= placeholder.ArgIdx + 1;

    // get corresponding argument
    int  argIdx= argOffset + placeholder.ArgIdx;
    if( argIdx >= arguments->Size() )
        throw Exception(ALIB_CALLER_NULLED, FMTExceptions::ArgumentIndexOutOfBounds,
                        argIdx - argOffset    + (argumentCountStartsWith1 ? 1 : 0),
                        arguments->Size() - argOffset,
                        formatString, formatString.Length() - parser.Length() - 1 );

    placeholder.Arg= &(*arguments)[size_t(argIdx)];
    return true;


}


bool    FormatterStdImpl::checkStdFieldAgainstArgument() {
    if( placeholder.TypeCodePosition < 0 )
        placeholder.TypeCodePosition= int( formatString.Length() - parser.Length() - 1 );


    // types bool and hashcode always works!
    if(   placeholder.Type == PHTypes::Bool
       || placeholder.Type == PHTypes::HashCode )
        return true;


    if ( placeholder.Arg->IsFloatingPoint() ) {
        if( placeholder.Type == PHTypes::NotGiven )
            placeholder.Type=   PHTypes::Float;

        if(  placeholder.Type == PHTypes::Float )
            return true;

        throw Exception( ALIB_CALLER_NULLED, FMTExceptions::IncompatibleTypeCode,
                         placeholder.ArgIdx + argumentCountStartsWith1,
                         placeholder.TypeCode, placeholder.Type,
                         "floating point",  placeholder.Arg->TypeID(),
                         formatString,      placeholder.TypeCodePosition                           );
    }

    if(    placeholder.Arg->IsSignedIntegral()
        || placeholder.Arg->IsUnsignedIntegral()
     #if ALIB_SIZEOF_INTEGER == 4
        || placeholder.Arg->IsType< int64_t>()
        || placeholder.Arg->IsType<uint64_t>()
     #endif
      )
    {
        if( placeholder.Type == PHTypes::NotGiven )
            placeholder.Type= PHTypes::IntBase10;
        if(     placeholder.Type == PHTypes::IntBase10
            ||  placeholder.Type == PHTypes::IntBinary
            ||  placeholder.Type == PHTypes::IntOctal
            ||  placeholder.Type == PHTypes::IntHex
            ||  placeholder.Type == PHTypes::Float
            ||  placeholder.Type == PHTypes::Character
            ||  placeholder.Type == PHTypes::Fill
            )
        {
            return true;
        }

        // not found
        throw Exception( ALIB_CALLER_NULLED, FMTExceptions::IncompatibleTypeCode,
                         placeholder.ArgIdx + argumentCountStartsWith1,
                         placeholder.TypeCode, placeholder.Type,
                         "integer",     placeholder.Arg->TypeID(),
                         formatString,  placeholder.TypeCodePosition               );
    }

    if(   placeholder.Arg->IsCharacter() ) {
        if( placeholder.Type == PHTypes::NotGiven )
            placeholder.Type=   PHTypes::Character;
        if( placeholder.Type == PHTypes::Character )
            return true;

        throw Exception( ALIB_CALLER_NULLED, FMTExceptions::IncompatibleTypeCode,
                         placeholder.ArgIdx + argumentCountStartsWith1,
                         placeholder.TypeCode, placeholder.Type,
                         "character",   placeholder.Arg->TypeID(),
                         formatString,  placeholder.TypeCodePosition                           );
    }

    if( placeholder.Type == PHTypes::NotGiven )
        placeholder.Type= PHTypes::String;

    return true;
}


void    FormatterStdImpl::writeStdArgument() {

    // write to temp buffer first, if we have a field width given
    AString* target;

    if ( placeholder.Width > 0 ) {
        target= &(fieldBuffer.Reset());

        // set default alignment
        if(     !placeholder.AlignmentSpecified
             &&  placeholder.Type != PHTypes::String
             &&  placeholder.Type != PHTypes::Character        )
             placeholder.ValueAlignment= lang::Alignment::Right;
    }
    else
        target= targetString;

    // store actual target length to fix some float exceptional cases (inf, nan)
    integer oldTargetLength= target->Length();


    integer fieldStartIdx= target->Length();

    // the main switch over the type
    switch( placeholder.Type ) {
        case PHTypes::NotGiven:
            ALIB_ERROR( "FORMAT",
             "Internal error: this should have been handled by method checkStdFieldAgainstArgument")
        break;

        case PHTypes::String:
            placeholder.Arg->Call<FAppend<character, lang::HeapAllocator>>( *target );
        break;

        case PHTypes::Bool:
            target->_<NC>( placeholder.Arg->Call<FIsTrue>() ? "true" : "false" );
        break;

        case PHTypes::Character:
        {
            wchar wc= 0;
                 if (placeholder.Arg->IsCharacter       ()) wc=                     placeholder.Arg->UnboxCharacter       ()  ;
            else if (placeholder.Arg->IsSignedIntegral  ()) wc= static_cast<wchar>( placeholder.Arg->UnboxSignedIntegral  () );
            else if (placeholder.Arg->IsUnsignedIntegral()) wc= static_cast<wchar>( placeholder.Arg->UnboxUnsignedIntegral() );

            if ( wc == 0)
                wc= L'?';
            target->_<NC>( wc );
        }
        break;

        case PHTypes::Fill:
        {
            integer qty;
                 if( placeholder.Arg->IsSignedIntegral()   ) qty=         placeholder.Arg->UnboxSignedIntegral  () ;
            else if( placeholder.Arg->IsUnsignedIntegral() ) qty= integer(placeholder.Arg->UnboxUnsignedIntegral());
            else throw Exception( ALIB_CALLER_NULLED, FMTExceptions::IncompatibleTypeCode,
                                 placeholder.ArgIdx + argumentCountStartsWith1,
                                 placeholder.TypeCode, placeholder.Type,
                                 "Fill",  placeholder.Arg->TypeID(),
                                 formatString,      placeholder.TypeCodePosition                           );
            target->InsertChars( placeholder.FillChar, qty );
        }
        break;


        case PHTypes::IntBase10:
        {
            if (placeholder.SignPaddingMode)
                placeholder.NF.DecMinimumFieldWidth= int8_t( placeholder.Width );

          #if ALIB_SIZEOF_INTEGER == 4
                 if( placeholder.Arg->IsType< int64_t>() )  target->_<NC>( strings::TDec<character>( placeholder.Arg->Unbox< int64_t>(), &placeholder.NF ) );
            else if( placeholder.Arg->IsType<uint64_t>() )  target->_<NC>( strings::TDec<character>( placeholder.Arg->Unbox<uint64_t>(), &placeholder.NF ) );
            else
          #endif
            if( placeholder.Arg->IsSignedIntegral())  target->_<NC>( strings::TDec<character>( placeholder.Arg->UnboxSignedIntegral()  , &placeholder.NF ) );
            else                                      target->_<NC>( strings::TDec<character>( placeholder.Arg->UnboxUnsignedIntegral(), &placeholder.NF ) );
        }
        break;


        case PHTypes::IntBinary:
        case PHTypes::HashCode:
        case PHTypes::IntHex:
        case PHTypes::IntOctal:
        {
            int digits= placeholder.Width;
            if( placeholder.WriteBinOctHexPrefix ) {
                target->_<NC>( placeholder.Type == PHTypes::IntOctal  ? placeholder.NF.OctLiteralPrefix :
                                  placeholder.Type == PHTypes::IntBinary ? placeholder.NF.BinLiteralPrefix :
                                                                           placeholder.NF.HexLiteralPrefix       );
                digits-= int((target->Length() - fieldStartIdx));
                if( placeholder.Width > 0 && digits <= 0 ) {
                    target->ShortenTo( fieldStartIdx + placeholder.Width );
                    break; // stop output, no space left
            }   }
            if ( digits <= 0 ) {
                if (placeholder.Type == PHTypes::HashCode )
                    digits= ALIB_SIZEOF_INTEGER * 2;
                else if ( placeholder.Arg->IsPointer() || placeholder.Arg->IsArray() )
                    digits= int( placeholder.Arg->GetPlaceholderUsageLength()
                                 * (  placeholder.Type == PHTypes::IntOctal  ? 3
                                    : placeholder.Type == PHTypes::IntBinary ? 8
                                    :                                          2 )  );
                else
                    digits= 0;
            }

            uint64_t value=   placeholder.Type == PHTypes::HashCode
                            ? uint64_t( placeholder.Arg->Hashcode()              )
                            :           placeholder.Arg->GetPlaceholderUsageLength() == 1
                            ?           placeholder.Arg->Data().Integrals.UInt8
                            :           placeholder.Arg->GetPlaceholderUsageLength() == 2
                            ?           placeholder.Arg->Data().Integrals.UInt16
                            :           placeholder.Arg->GetPlaceholderUsageLength() == 4
  #if    ALIB_SIZEOF_INTEGER == 8
                            ?           placeholder.Arg->Data().Integrals.UInt32
                            :           placeholder.Arg->Data().Integrals.UInt;
  #elif  ALIB_SIZEOF_INTEGER == 4
                            ?           placeholder.Arg->Data().Integrals.UInt
                            :           placeholder.Arg->Data().Integrals.UInt64;
  #endif

                 if( placeholder.Type == PHTypes::IntOctal)  target->_<NC>( TOct<character>( value, digits, &placeholder.NF ) );
            else if( placeholder.Type == PHTypes::IntBinary) target->_<NC>( TBin<character>( value, digits, &placeholder.NF ) );
            else if( placeholder.Type == PHTypes::HashCode ) target->_<NC>( THex<character>( value, digits, &placeholder.NF ) );
            else                                             target->_<NC>( THex<character>( value, digits, &placeholder.NF ) );
        }
        break;

        case PHTypes::Float:
        {
            // get value
            double value=    placeholder.Arg->IsFloatingPoint()   ?         placeholder.Arg->UnboxFloatingPoint()
                          :  placeholder.Arg->IsSignedIntegral()  ? double( placeholder.Arg->UnboxSignedIntegral()   )
                          :                                         double( placeholder.Arg->UnboxUnsignedIntegral() );
            if( placeholder.IsPercentage )
                value*= 100.0;

            if (placeholder.SignPaddingMode) {
                auto classification=  std::fpclassify(value);

                // write sign upfront and set fill character to 0
                if( classification != FP_NAN  ) {
                    bool negative= std::signbit(value);
                    if( classification == FP_ZERO && negative ) {
                        value= 0.0;
                        negative= false;
                    }

                    if( negative ) {
                        targetString->_<NC>( '-' );
                        --placeholder.Width;
                        value= -value;
                    }
                    else if( placeholder.NF.PlusSign != '\0' ) {
                        targetString->_<NC>( placeholder.NF.PlusSign );
                        --placeholder.Width;
                    }
                    placeholder.NF.PlusSign= '\0';

                    if ( !HasBits(placeholder.NF.Flags, NumberFormatFlags::WriteGroupChars) || placeholder.NF.ThousandsGroupChar == '\0')
                        placeholder.FillChar= '0';
                    else {
                        // calculate integral part width
                        if ( placeholder.Width > 0 && !HasBits(placeholder.NF.Flags, NumberFormatFlags::ForceScientific) ) {
                            placeholder.NF.IntegralPartMinimumWidth= int8_t( placeholder.Width - 1 ); // -1 == the dot

                            if( placeholder.NF.FractionalPartWidth >= 0 )
                                placeholder.NF.IntegralPartMinimumWidth-= placeholder.NF.FractionalPartWidth;

                            if( placeholder.IsPercentage )
                                --placeholder.NF.IntegralPartMinimumWidth;

                            // check
                            if( placeholder.NF.IntegralPartMinimumWidth <= 0)
                                placeholder.NF.IntegralPartMinimumWidth= 1;
                }   }   }

            }

            target->_<NC>( strings::TDec<character>( value, &placeholder.NF ) );

            if( placeholder.IsPercentage )
                target->_<NC>( '%' );


            // if nan or inf was written, we fill with spaces
            if (     placeholder.SignPaddingMode
                && (     target->IndexOf(placeholder.NF.NANLiteral, oldTargetLength) >= 0
                     ||  target->IndexOf(placeholder.NF.INFLiteral, oldTargetLength) >= 0 ) )
                placeholder.FillChar= ' ';

        }
        break;

        default: ALIB_ERROR("FORMAT", "Illegal switch state.") break;
    } // switch( placeholder.Type )

    // now do an 'intermediate post phase' processing
    preAndPostProcess( fieldStartIdx, target );

    // apply cutting
    if ( placeholder.CutContent >= 0 ) {
        // wchar compilation: we grant the length is the same as the number of wide characters
        // added, although this is still not true for some unicode character combinations
        if( std::is_same<character, wchar>::value ) {
            // too much added?
            if( target->Length() - oldTargetLength > placeholder.CutContent )
                target->ShortenTo( oldTargetLength + placeholder.CutContent );
        } else {
            integer qtyWCharsAdded= target->Substring<NC>( oldTargetLength, target->Length() - oldTargetLength ).WStringLength();

            // too much added?
            if( qtyWCharsAdded > placeholder.CutContent ) {
                // was not unicode?
                if( qtyWCharsAdded == target->Length() - oldTargetLength )
                    target->ShortenTo( oldTargetLength + placeholder.CutContent );

                // in the unicode case, it gets complicated: we have to convert to unicode and
                // then convert a part of it back!
                else if( qtyWCharsAdded < 256) {
                    WString256 wBuf;
                    wBuf.DbgDisableBufferReplacementWarning();
                    wBuf.Append<NC>( target->Buffer() + oldTargetLength, target->Length() - oldTargetLength );
                    target->ShortenTo( oldTargetLength );
                    target->Append<NC>( wBuf.Buffer(),placeholder.CutContent );
    }   }   }   }


    // if field mode, we have to append the field buffer as a field to the real target now
    if( target ==  &fieldBuffer )
        targetString->_<NC>( strings::TField<character>( fieldBuffer, placeholder.Width, placeholder.ValueAlignment, placeholder.FillChar ) );

}

bool    FormatterStdImpl::writeCustomFormat() {
    auto* func=  placeholder.Arg->GetFunction<FFormat>( lang::Reach::Local );
    if( !func )
        return false;

    placeholder.Arg->CallDirect<FFormat>( func, placeholder.FormatSpec, placeholder.NF, *targetString );
    return true;
}

} // namespace [alib::format]
