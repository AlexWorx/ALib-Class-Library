// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_STRINGS_DETAIL_NUMBERCONVERSION)
#   include "alib/strings/detail/numberconversion.hpp"
#endif

#if !defined(HPP_ALIB_LANG_BITS)
#   include "alib/lang/bits.hpp"
#endif


#if defined( _WIN32 ) || defined(__APPLE__)
#   include <clocale>
#endif

#if !defined (_GLIBCXX_CMATH) && !defined (_CMATH_)
#   include <cmath>
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif

#if defined(_WIN32) && !defined (__INTRIN_H_)
    #include <intrin.h>
#endif
#endif // !defined(ALIB_DOX)

namespace alib {  namespace strings {

/**
 * This is a detail namespace of module \alib_strings_nl.
 */
namespace detail {
#if !defined(ALIB_DOX)


namespace {
constexpr const uint64_t pow10_0to19[]=
{
                       1UL, // 10^ 0
                      10UL, // 10^ 1
                     100UL, // 10^ 2
                    1000UL, // 10^ 3
                   10000UL, // 10^ 4
                  100000UL, // 10^ 5
                 1000000UL, // 10^ 6
                10000000UL, // 10^ 7
               100000000UL, // 10^ 8
              1000000000UL, // 10^ 9
             10000000000UL, // 10^10
            100000000000UL, // 10^11
           1000000000000UL, // 10^12
          10000000000000UL, // 10^13
         100000000000000UL, // 10^14
        1000000000000000UL, // 10^15
       10000000000000000UL, // 10^16
      100000000000000000UL, // 10^17
     1000000000000000000UL, // 10^18
    10000000000000000000UL, // 10^19
};


constexpr const uint8_t binSizeToDecSize[]
{
    20, 19, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16, 16, 15,
    15, 15, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10,
    10, 10, 10,  9,  9,  9,  8,  8,  8,  7,  7,  7,  7,  6,  6,  6,
    05,  5,  5,  4,  4,  4,  4,  3,  3,  3,  2,  2,  2,  1,  1,  1
};

constexpr inline bool hasBits(NumberFormatFlags lhs, NumberFormatFlags rhs)
{ return ((int(lhs) & int(rhs)) != 0); }

}// anonymous namespace



template<typename TChar>
uint64_t ParseDecDigits( const TString<TChar>& src, integer& idx )
{
    uint64_t result=    0;

    // check
    integer length= src.Length();
    if( idx < 0  || idx >= length )
        return 0;

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    const TChar* buf=    src.Buffer();
    ALIB_WARNINGS_RESTORE

    // read number
    while ( idx < length )
    {
        const TChar c= buf[idx];
        if ( c < '0' || c > '9' )
            break;

        result= ( result * 10 ) + static_cast<uint64_t>( c - '0'  );
        ++idx;
    }

    // that's it
    return result;
}

// compilers can't know that the parse-methods above leave idx as it is or succeed.
// Therefore it would warn that result may not be initialized. But this will never happen.
ALIB_WARNINGS_UNINITIALIZED_OFF

template<typename TChar>
int64_t   ParseInt( const TString<TChar>& src, integer& startIdx, const TNumberFormat<TChar>& nf )
{

    integer srcLength= src.Length();

    if ( startIdx < 0 || startIdx >= srcLength )
        return 0;

    // get buffer and index behind whitespaces
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    const TChar* buffer= src.Buffer();
    ALIB_WARNINGS_RESTORE

    integer idx= src. template IndexOfAny<lang::Inclusion::Exclude, false>( nf.Whitespaces, startIdx );
    if ( idx < 0 )
        return 0;

    // read sign
    bool negative;
    if ( (negative= (buffer[idx] == '-')) == true || buffer[idx] == '+' )
    {
        if( (idx= src. template IndexOfAny<lang::Inclusion::Exclude, false>( nf.Whitespaces, idx + 1 ) ) == -1 )
            return 0;
    }

    int64_t result;
    integer oldIdx= idx;

    integer prefixLen;

    // try parsing hexadecimal
    if(     ( prefixLen= nf.HexLiteralPrefix.Length())  != 0
        &&  ( idx + prefixLen  < srcLength )
        &&  characters::CharArray<TChar>::Equal(buffer + idx, nf.HexLiteralPrefix.Buffer(), prefixLen )
        &&  nf.Whitespaces.IndexOf( buffer[ idx + prefixLen ] ) == -1     )
    {
        idx+= prefixLen;
        result=  static_cast<int64_t>( ParseHex( src, idx, nf ) );
        if( idx - prefixLen == oldIdx )
            idx-= prefixLen;
    }

    // try parsing binary
    else if(     (prefixLen= nf.BinLiteralPrefix.Length())  != 0
             &&  ( idx + prefixLen < srcLength )
             &&  characters::CharArray<TChar>::Equal(buffer + idx, nf.BinLiteralPrefix.Buffer(), prefixLen )
             &&  nf.Whitespaces.IndexOf( buffer[ idx + prefixLen ] ) == -1     )
    {
        idx+= prefixLen;
        result=  static_cast<int64_t>( ParseBin( src, idx, nf ) );
        if( idx - prefixLen == oldIdx )
            idx-= prefixLen;
    }

    // try parsing octal
    else if(     (prefixLen= nf.OctLiteralPrefix.Length()) != 0
             &&  ( idx + prefixLen  < srcLength )
             &&  characters::CharArray<TChar>::Equal(buffer + idx, nf.OctLiteralPrefix.Buffer(), prefixLen )
             &&  nf.Whitespaces.IndexOf( buffer[ idx + prefixLen ] ) == -1     )
    {
        idx+= prefixLen;
        result=  static_cast<int64_t>( ParseOct( src, idx, nf ) );
        if( idx - prefixLen == oldIdx )
            idx-= prefixLen;
    }


    // parse decimal
    if ( idx == oldIdx )
        result=  static_cast<int64_t>( ParseDec( src, idx, nf ) );

    // Any success? if yes, we move the given start pointer

    if ( idx != oldIdx )
        startIdx= idx;


    return negative ? static_cast<int64_t>(-result)
                    : static_cast<int64_t>( result);
}

ALIB_WARNINGS_RESTORE

template<typename TChar>
uint64_t ParseDec( const TString<TChar>& src, integer& startIdx, const TNumberFormat<TChar>& nf )
{
    uint64_t result=    0;

    // read whitespaces
    integer idx= src. template IndexOfAny<lang::Inclusion::Exclude>( nf.Whitespaces, startIdx );
    if ( idx < 0 )
        return 0;

    // read number
    bool         charFound=  false;
    integer      length=     src.Length();
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    const TChar* buf=        src.Buffer();
    ALIB_WARNINGS_RESTORE
    while ( idx < length )
    {
        const TChar c= buf[idx];
        if (    charFound
             && hasBits(nf.Flags, NumberFormatFlags::ReadGroupChars)
             && c != '\0'
             && c == nf.ThousandsGroupChar  )
        {
            ++idx;
            continue;
        }

        if ( c < '0' || c > '9' )
            break;

        charFound= true;
        result= ( result * 10 ) + static_cast<uint64_t>( c - '0' );

        ++idx;
    }

    if ( charFound )
        startIdx= idx;
    return result;
}


template<typename TChar>
uint64_t ParseBin( const TString<TChar>& src, integer& startIdx, const TNumberFormat<TChar>& nf )
{
    uint64_t result=    0;

    // read whitespaces
    integer idx= src. template IndexOfAny<lang::Inclusion::Exclude>( nf.Whitespaces, startIdx );
    if ( idx < 0 )
        return 0;

    // read number
    bool        charFound=  false;
    integer    length=     src.Length();
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    const TChar* buf=        src.Buffer();
    ALIB_WARNINGS_RESTORE
    while ( idx < length )
    {
        TChar c= buf[idx];
        if (    charFound
             && hasBits(nf.Flags, NumberFormatFlags::ReadGroupChars)
             && c != '\0'
             && (    c == nf.BinNibbleGroupChar
                  || c == nf.BinByteGroupChar
                  || c == nf.BinWordGroupChar
                  || c == nf.BinWord32GroupChar )    )
        {
            ++idx;
            continue;
        }

        if ( c == '0'  )
        {
            result<<= 1;
            ++idx;
            charFound= true;
            continue;
        }

        if ( c == '1'  )
        {
            result<<= 1;
            result|=1;
            ++idx;
            charFound= true;
            continue;
        }

        break;
    }

    if ( charFound )
        startIdx= idx;
    return result;
}

template<typename TChar>
uint64_t ParseHex( const TString<TChar>& src, integer& startIdx, const TNumberFormat<TChar>& nf )
{
    uint64_t result=    0;

    // read whitespaces
    integer idx= src. template IndexOfAny<lang::Inclusion::Exclude>( nf.Whitespaces, startIdx );
    if ( idx < 0 )
        return 0;

    // read number
    bool         charFound=  false;
    integer      length=     src.Length();
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    const TChar* buf=        src.Buffer();
    ALIB_WARNINGS_RESTORE
    while ( idx < length )
    {
        TChar c= buf[idx];
        if (    charFound
             && hasBits(nf.Flags, NumberFormatFlags::ReadGroupChars)
             && c != '\0'
             && (    c == nf.HexByteGroupChar
                  || c == nf.HexWordGroupChar
                  || c == nf.HexWord32GroupChar )    )
        {
            ++idx;
            continue;
        }

        if ( c >= '0' && c <= '9' )
        {
            result<<= 4;
            result|=  static_cast<uint64_t>(c - '0');
            ++idx;
            charFound= true;
            continue;
        }

        if ( c >= 'A' && c <= 'F' )
        {
            result<<= 4;
            result|=  static_cast<uint64_t>(c - 'A' + 10 );
            ++idx;
            charFound= true;
            continue;
        }

        if ( c >= 'a' && c <= 'f' )
        {
            result<<= 4;
            result|=  static_cast<uint64_t>(c - 'a' + 10 );
            ++idx;
            charFound= true;
            continue;
        }


        break;
    }

    if ( charFound )
        startIdx= idx;
    return result;
}

template<typename TChar>
uint64_t ParseOct( const TString<TChar>& src, integer& startIdx, const TNumberFormat<TChar>& nf )
{
    uint64_t result=    0;

    // read whitespaces
    integer idx= src. template IndexOfAny<lang::Inclusion::Exclude>( nf.Whitespaces, startIdx );
    if ( idx < 0 )
        return 0;

    // read number
    bool         charFound=  false;
    integer      length=     src.Length();
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    const TChar* buf=        src.Buffer();
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    while ( idx < length )
    {
        TChar c= buf[idx];
        if (     charFound
             && hasBits(nf.Flags, NumberFormatFlags::ReadGroupChars)
             && c != 0
             && c == nf.OctGroupChar )
        {
            ++idx;
            continue;
        }

        if ( c >= '0' && c <= '7' )
        {
            result<<= 3;
            result|=  static_cast<uint64_t>(c - '0');
            ++idx;
            charFound= true;
            continue;
        }

        break;
    }

    if ( charFound )
        startIdx= idx;
    return result;
}

template<typename TChar>
double ParseFloat( const TString<TChar>& src, integer& startIdx, const TNumberFormat<TChar>& nf )
{
    // checks
    if( startIdx < 0 || startIdx >= src.Length() )
        return 0.0;

    // get pointers
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    const TChar* buf=    src.Buffer() + startIdx;
    const TChar* bufEnd= src.Buffer() + src.Length();
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE

    // read whitespaces
    {
        integer skip= src. template IndexOfAny<lang::Inclusion::Exclude>( nf.Whitespaces, startIdx );
        if ( skip < 0 )
            return 0.0;
        buf+= skip - startIdx;
    }

    // +/- sign
    bool negative= (*buf == '-');
    if ( negative || *buf == '+' )
    {
        if( ++buf == bufEnd )
            return 0.0;

        integer skip= TString<TChar>(buf, bufEnd-buf ). template IndexOfAny<lang::Inclusion::Exclude, false>( nf.Whitespaces );
        if( skip < 0 )
            return 0.0;
        buf+= skip;
    }

    // NaN, Infinite
    if(     buf + nf.NANLiteral.Length() - 1 <= bufEnd
        &&  nf.NANLiteral. template CompareTo<true, lang::Case::Ignore>( TString<TChar>( buf, nf.NANLiteral.Length() ) ) == 0    )
    {
        startIdx= buf - src.Buffer()  + nf.NANLiteral.Length();
        return std::numeric_limits<double>::quiet_NaN();
    }

    if(     buf + nf.INFLiteral.Length() - 1 <= bufEnd
        &&  nf.INFLiteral. template CompareTo<true, lang::Case::Ignore>( TString<TChar>( buf, nf.INFLiteral.Length() ) ) == 0    )
    {
        startIdx= buf -  src.Buffer() + nf.INFLiteral.Length();
        return negative ? -std::numeric_limits<double>::infinity()
                        :  std::numeric_limits<double>::infinity();
    }

    double result= 0.0;

    // read number before dot?
    TChar c= *buf;
    bool integralPartFound= isdigit(c);
    if ( integralPartFound )
    {
        if (  c < '0' || c > '9' )
            return 0.0;

        {
            integer intIdx= 0;
            result= static_cast<double>( ParseDec( TString<TChar>(buf, bufEnd - buf), intIdx, nf ) );
            buf+= intIdx;
        }

        // end?
        ALIB_ASSERT_ERROR( buf <= bufEnd, "STRINGS", "Error in float parsing algorithm." )
        if ( buf == bufEnd )
        {
            startIdx= buf - src.Buffer();
            return negative ? -result : result;
        }
    }

    if( nf.DecimalPointChar == *buf  )
    {
        // consume dot
        ++buf;

        // read number after dot
        if (      buf <  bufEnd
             &&  *buf >= '0'  &&  *buf <= '9' )
        {
            integer intIdx= 0;
            double intValue= static_cast<double>( ParseDecDigits( TString<TChar>(buf, bufEnd - buf), intIdx) );
            buf+= intIdx;
            result+= ( intValue / pow( 10, static_cast<double>(intIdx) ) );
        }
    }
    else if( !integralPartFound )
        return 0.0; // index not moved, indicates failure.

    // read eNNN
    if ( buf <  bufEnd )
    {
        auto oldBuf= buf;
        bool eSepFound=  false;
        integer  sepLen= nf.ExponentSeparator.Length();
        if ( buf + sepLen < bufEnd )
        {
            integer pos= 0;
            while (     pos < sepLen
                    &&  nf.ExponentSeparator. template CharAt<false>(pos) == *(buf + pos) )
                ++pos;
            if ( (eSepFound= ( pos == sepLen ) ) == true )
                buf += pos;
        }
        if ( !eSepFound && ( *buf == 'e' || *buf == 'E' ) )
        {
            ++buf;
            eSepFound= true;
        }


        if (eSepFound && buf < bufEnd)
        {
            bool negativeE= false;
            if ( (negativeE= (*buf == '-') ) == true ||  *buf == '+' )
                ++buf;

            if( buf < bufEnd )
            {
                integer idx= 0;
                int exp= static_cast<int>( ParseDecDigits( TString<TChar>(buf, bufEnd - buf), idx ) );
                if( idx > 0 )
                {
                    buf+= idx;
                    result*= pow( 10, negativeE ? -exp : exp );
                }
                else
                {
                    // no number found behind e. restore buf and ignore.
                    buf= oldBuf;
                }
            }
        }
    }

    // adjust given idx
    startIdx= buf - src.Buffer();

    // that's it
    return negative ? -result : result;
}

template<typename TChar>
integer WriteDecUnsigned( uint64_t value, TChar* buffer, integer idx, int overrideWidth,
                          const TNumberFormat<TChar>& nf )
{
    int width= overrideWidth != 0 ? overrideWidth
                                  : nf.DecMinimumFieldWidth;

    if ( width < 1  )   width=  1;

    // get the number of dec digits in value
    int digitsInValue;
    {
        if( value < 10 )
            digitsInValue = 1;
        else
        {
            int leadingBinaryZeros= lang::CLZ(value);
            digitsInValue= binSizeToDecSize[leadingBinaryZeros];

            // could be one lower due to the rest after the most significant bit
            if( value < pow10_0to19[digitsInValue-1] )
                --digitsInValue;

            ALIB_ASSERT_ERROR(                            value >= pow10_0to19[ digitsInValue - 1 ]
                              && ( digitsInValue == 20 || value <  pow10_0to19[ digitsInValue     ] ),
                              "STRINGS", "Error in digitsInValue calculation"   )
        }
    }


    // calc printable digits and print leading group char replacement ' '
    int printDigits;
    {
        int requestedDigits;
        if ( hasBits(nf.Flags, NumberFormatFlags::WriteGroupChars) && nf.ThousandsGroupChar != '\0' )
        {
            if ( width > 26 )
                width= 26;
            requestedDigits=  width - width / 4;
        }
        else
        {
            if ( width > 20 )
                width= 20;
            requestedDigits= width;
        }

        printDigits= (std::max)( requestedDigits, digitsInValue );


        // starts with group character? -> write space instead
        ALIB_ASSERT_ERROR( width - 1 <= printDigits + (printDigits - 1) / 3 ,
                           "STRINGS", "Internal error, false assumption" )
        if( printDigits >1  && width > printDigits + (printDigits - 1) / 3 )
            buffer[idx++]= nf.LeadingGroupCharReplacement;
    }

    // main loop
    int  actDigit=      printDigits;
    while ( actDigit > 0 )
    {
        // print normal digit
        int digitValue=   static_cast<int>( ( value / pow10_0to19[actDigit-1] ) );
        ALIB_ASSERT( digitValue <= 9 )

        // write group character
        if(     hasBits(nf.Flags, NumberFormatFlags::WriteGroupChars) && nf.ThousandsGroupChar != '\0'
            &&  actDigit != printDigits
            &&  actDigit % 3 == 0
          )
            buffer[idx++]= nf.ThousandsGroupChar;

        // write character
        buffer[idx++]=  static_cast<TChar>( 48 + digitValue ); // 48= '0'

        // next
        value=    value % pow10_0to19[actDigit-1];

        --actDigit;
    }

    return idx;
}

template<typename TChar>
integer WriteDecSigned( int64_t value, TChar* buffer, integer idx, int overrideWidth,
                        const TNumberFormat<TChar>& nf )
{
    integer oldIdx= idx;

    // write sign and turn negative to positive
    uint64_t uValue;
    if ( value >= 0 )
    {
        uValue= static_cast<uint64_t>( value );
        if ( nf.PlusSign != '\0' )
            buffer[idx++]= nf.PlusSign;
    }
    else
    {
        uValue= static_cast<uint64_t>( -value );
        buffer[idx++]= '-';
    }

    int width= overrideWidth != 0 ? overrideWidth
                                  : nf.DecMinimumFieldWidth;
    if( idx != oldIdx && width > 1 )
        --width;

    return WriteDecUnsigned( uValue, buffer, idx, width, nf );
}


template<typename TChar>
integer WriteBin( uint64_t value, TChar* buffer, integer idx, int  overrideWidth,
                  const TNumberFormat<TChar>& nf )
{
    // how many digits in a grouping block?
    int groupWidth=   !hasBits(nf.Flags, NumberFormatFlags::WriteGroupChars)             ? 0
                     : nf.BinNibbleGroupChar  != '\0' ? 4
                     : nf.BinByteGroupChar    != '\0' ? 8
                     : nf.BinWordGroupChar    != '\0' ? 16
                     : nf.BinWord32GroupChar  != '\0' ? 32    :    0;

    // if the first "digit" is a separator, we will write a space instead.
    // (we do the best to keep the width of the output intact if given)
    int nextSeparator= 0; // 0: dont write, 1= space, 2 normal

    // adjust minDigits to 0..64 (if 0 demanded, 0 is given!)
    int digits=  overrideWidth != 0 ? overrideWidth  : nf.BinFieldWidth;
    if (digits > 0 )
    {
        if ( groupWidth != 0 )
        {
            nextSeparator=  digits<= groupWidth ? 0
                            :   (     digits >= groupWidth
                                  && (digits % (groupWidth + 1)) == 0 ) ? 1 : 2;

            digits-= digits / (groupWidth + 1); // subtract needed separators from digits
        }

        // still oversized?
        if ( digits > 64 )
               digits= 64;
    }

    // if negative value given, we calculate the needed digits
    if ( digits < 0  )
        digits=  value != 0 ? lang::MSB(value)
                            : 1;

    uint64_t testValue= static_cast<uint64_t>(1) << (digits - 1);
    while ( digits > 0)
    {
        // write the separator
        if( groupWidth != 0 && ( digits % groupWidth) == 0 )
        {
            if ( nextSeparator != 0 )
                buffer[idx++]=  nextSeparator == 1     ? nf.LeadingGroupCharReplacement :
                                ( digits % 32 == 0 )   ? nf.BinWord32GroupChar          :
                                ( digits % 16 == 0 )   ? nf.BinWordGroupChar            :
                                ( digits %  8 == 0 )   ? nf.BinByteGroupChar            : nf.BinNibbleGroupChar;
        }
        nextSeparator= 2; // from now on write separators

        // write digit
        buffer[idx++]= ( value & testValue ) == 0L ? '0' : '1';

        // next
        testValue >>= 1;
        --digits;
    }

    return idx;
}

template<typename TChar>
integer WriteHex( uint64_t value, TChar* buffer, integer idx, int overrideWidth,
                  const TNumberFormat<TChar>& nf )
{
    // how many digits in a grouping block?
    int groupWidth=  !hasBits(nf.Flags, NumberFormatFlags::WriteGroupChars)             ? 0
                    : nf.HexByteGroupChar    != '\0' ? 2
                    : nf.HexWordGroupChar    != '\0' ? 4
                    : nf.HexWord32GroupChar  != '\0' ? 8    :    0;

    // if the first "digit" is a separator, we will write a space instead.
    // (we do the best to keep the width of the output intact if given)
    int nextSeparator= 0; // 0: dont write, 1= space, 2 normal

    int digits= overrideWidth != 0 ? overrideWidth : nf.HexFieldWidth;

    // adjust minDigits to 0..64 (if 0 demanded, 0 is given!)
    if (digits > 0 )
    {
        if ( groupWidth != 0 )
        {
            nextSeparator=  digits<= groupWidth ? 0
                            :   (     digits >= groupWidth
                                  && (digits % (groupWidth + 1)) == 0 ) ? 1 : 2;

            digits-= digits / (groupWidth + 1); // subtract needed separators from digits
        }
    }

    // if negative value given, we calculate the needed digits
    if ( digits < 0  )
        digits=  value != 0 ? (lang::MSB(value)-1) / 4 + 1
                            : 1;

    // check now for oversize
    if ( digits > 16 )
           digits= 16;

    unsigned int characterA= static_cast<unsigned int>( hasBits(nf.Flags, NumberFormatFlags::HexLowerCase) ? 'a' : 'A' );
    int          shiftBits= (digits -1 ) * 4;
    uint64_t testMask=  static_cast<uint64_t>( 15 ) << shiftBits;
    while ( digits > 0)
    {
        // write the separator
        if( groupWidth != 0 && ( digits % groupWidth) == 0 )
        {
            if ( nextSeparator != 0 )
                buffer[idx++]=  nextSeparator == 1   ? nf.LeadingGroupCharReplacement :
                                ( digits % 8 == 0 )  ? nf.HexWord32GroupChar          :
                                ( digits % 4 == 0 )  ? nf.HexWordGroupChar            : nf.HexByteGroupChar;
        }
        nextSeparator= 2; // from now on write separators

        // write digit
        unsigned int nibble= static_cast<unsigned int>( ( value & testMask ) >> shiftBits );
        buffer[idx++]=   ( nibble < 10 )   ?  static_cast<TChar>('0' + nibble)
                                           :  static_cast<TChar>(characterA + (nibble - 10));

        // next
        testMask  >>= 4;
        shiftBits -=  4;
        --digits;
    }

    return idx;
}

template<typename TChar>
integer WriteOct( uint64_t value, TChar* buffer, integer idx, int overrideWidth,
                  const TNumberFormat<TChar>& nf )
{
    // how many digits in a grouping block?
    const int groupWidth=  hasBits(nf.Flags, NumberFormatFlags::WriteGroupChars) && nf.OctGroupChar != '\0' ? 3 : 0;

    // if the first "digit" is a separator, we will write a space instead.
    // (we do the best to keep the width of the output intact if given)
    int nextSeparator= 0; // 0: dont write, 1= space, 2 normal

    // adjust minDigits to 0..64 (if 0 demanded, 0 is given!)
    int digits= overrideWidth != 0 ? overrideWidth : nf.OctFieldWidth;
    if (digits > 0 )
    {
        if ( groupWidth != 0 )
        {
            nextSeparator=  digits<= groupWidth ? 0
                            :   (     digits >= groupWidth
                                  && (digits % (groupWidth + 1)) == 0 ) ? 1 : 2;

            digits-= digits / (groupWidth + 1); // subtract needed separators from digits
        }
    }

    // if negative value given, we calculate the needed digits
    if ( digits < 0  )
        digits=  value != 0 ? (lang::MSB(value)-1) / 3 + 1
                            : 1;

    // check now for oversize
    if ( digits > 22 )
           digits= 22;

    int           shiftBits= (digits -1 ) * 3;
    while ( digits > 0)
    {
        // write the separator
        if( groupWidth != 0 && ( digits % groupWidth) == 0 )
        {
            if ( nextSeparator != 0 )
                buffer[idx++]=  nextSeparator == 1    ? nf.LeadingGroupCharReplacement : nf.OctGroupChar;
        }
        nextSeparator= 2; // from now on write separators

        // write digit
        auto octet= ( value & (static_cast<uint64_t>(7) << shiftBits) ) >> shiftBits;
        buffer[idx++]= static_cast<TChar>('0' + octet);

        // next
        shiftBits -=  3;
        --digits;
    }

    return idx;
}

template<typename TChar>
integer WriteFloat( double value, TChar* buffer, integer idx, int overrideWidth,
                    const TNumberFormat<TChar>& nf )
{
    int integralWidth= overrideWidth != 0 ? overrideWidth : nf.IntegralPartMinimumWidth;


    // check for NaN, negative zero
    auto classification=  std::fpclassify(value);
    if( classification == FP_NAN )    {  return idx+= nf.NANLiteral.CopyTo(buffer + idx);   }
    bool isNegative= std::signbit(value);
    if ( isNegative )
    {
        if( classification == FP_ZERO )
        {
            isNegative= false;
            value     = 0.0;
        }
        else
            value= -value;
    }

    // +/-inf
    if( classification == FP_INFINITE )
    {
        if ( isNegative )
            buffer[idx++]= '-';
        else if ( nf.PlusSign != '\0' )
            buffer[idx++]= nf.PlusSign;
        return idx+= nf.INFLiteral.CopyTo(buffer + idx);
    }


    constexpr int MaxFloatSignificantDigits= 16;


    // calc dot position
    int exp10=      value != 0.0   ? static_cast<int>( floor(log10( value ) ) )
                                   : 0;

    // decide if we are using scientific format (with e) or not
    bool scientific=  (     hasBits(nf.Flags, NumberFormatFlags::ForceScientific)
                        || (  integralWidth < 0 && nf.FractionalPartWidth < 0  &&  ( exp10 > 6 || exp10 <= -5 )  )
                        || (  integralWidth          > 0 && exp10 != 0 && exp10 >= (MaxFloatSignificantDigits - integralWidth          - 1 ) )
                        || (  nf.FractionalPartWidth > 0 && exp10 != 0 && exp10 >= (MaxFloatSignificantDigits - nf.FractionalPartWidth - 1 ) )
                      );

    integralWidth=         (std::min) ( integralWidth,                             15 );
    int fractionalDigits=  (std::min) ( nf.FractionalPartWidth, static_cast<int8_t>(15) );



    // result variables used for output
    uint64_t intPart;
    uint64_t fractPart;
    int unusedFractDigits;
    int firstNonZero;

    // scientific output
    if ( scientific )
    {
        auto dotPos= MaxFloatSignificantDigits - exp10;
        intPart=     static_cast<uint64_t>(llrint( value * pow( 10, dotPos ) ));
        fractPart=   intPart %  pow10_0to19[ MaxFloatSignificantDigits ];
        intPart=     intPart /  pow10_0to19[ MaxFloatSignificantDigits ];

        // determine first non zero fract number
        firstNonZero= 0;
        if ( fractPart > 0 )
        {
            ALIB_ASSERT( MaxFloatSignificantDigits - firstNonZero < 20)
            while ( fractPart < pow10_0to19[ MaxFloatSignificantDigits - firstNonZero - 1 ] )
                ++firstNonZero;
            ALIB_ASSERT( MaxFloatSignificantDigits - firstNonZero > 0)
        }
        ++firstNonZero;

        unusedFractDigits= fractionalDigits >= 0 ?  MaxFloatSignificantDigits - fractionalDigits
                                                 :  1;
    }

     // normal output, number > 0
     else if (exp10 >= 0 )
     {
        int intPartSize= MaxFloatSignificantDigits - exp10;
        ALIB_ASSERT( intPartSize > 0  && intPartSize <= MaxFloatSignificantDigits )
        intPart=     static_cast<uint64_t>(llrint( value * pow( 10, intPartSize ) ));
        fractPart=   intPart %  pow10_0to19[ intPartSize ];
        intPart=     intPart /  pow10_0to19[ intPartSize ];

        // determine first non zero fract number
        firstNonZero= 0;
        if ( fractPart > 0 )
        {
            while ( fractPart < pow10_0to19[ intPartSize - firstNonZero - 1 ] )
                ++firstNonZero;
            ALIB_ASSERT( intPartSize - firstNonZero > 0 )
        }
        ++firstNonZero;

        unusedFractDigits= fractionalDigits >= 0 ?  intPartSize - fractionalDigits
                                                 :  1;
     }

    // normal output, number  < 1
    else
    {
        // just zeros? -> write them and return
        firstNonZero= -exp10;
        intPart=      0;
        fractPart=    static_cast<uint64_t>(llrint( value * pow( 10, MaxFloatSignificantDigits + firstNonZero) ));
        unusedFractDigits= fractionalDigits >= 0 ?  MaxFloatSignificantDigits - ( fractionalDigits - firstNonZero )
                                                 :  1;
     }

    // cut fract digits and round it up
    if (     (fractionalDigits < 0 || fractionalDigits  >= firstNonZero - 1 )
         &&  unusedFractDigits >  0
         &&  unusedFractDigits <= 18 )
    {

        uint64_t rest= fractPart % pow10_0to19[ unusedFractDigits ];
        fractPart    = fractPart / pow10_0to19[ unusedFractDigits ];
        if ( rest > pow10_0to19[ unusedFractDigits ] / 2 )
        {
            ++fractPart;
            int  overflowDigit= 0;
            bool overflow=      false;
            while (    (overflowDigit <= fractionalDigits || fractionalDigits < 0 )
                    && (overflow|= fractPart == pow10_0to19[ overflowDigit ]) == false
                    &&  fractPart > pow10_0to19[ overflowDigit ]
                   )
                ++overflowDigit;

            if ( overflow )
            {
                if ( overflowDigit == (fractionalDigits >= 0 ? fractionalDigits : 15) )
                {
                    fractPart= 0;
                    ++intPart;
                }
                else
                {
                    ALIB_ASSERT( firstNonZero > 1 )
                    --firstNonZero;
                }
            }
        }
    }

    // write sign. Do it only if this is not a 0 value after rounding.
    if ( isNegative )
    {
        if(    intPart
            || (    fractPart
                 && ( fractionalDigits < 0 || fractionalDigits > firstNonZero -1)
           ) )
            buffer[idx++]= '-';
    }
    else if ( nf.PlusSign != '\0' )
        buffer[idx++]= nf.PlusSign;

    // write int part
    if ( intPart != 0L || integralWidth != 0 )
        idx= WriteDecUnsigned( intPart, buffer, idx, integralWidth, nf );

    // write dot
    if ( fractionalDigits != 0 || hasBits(nf.Flags, NumberFormatFlags::ForceDecimalPoint) )
        buffer[idx++]=  nf.DecimalPointChar;

    // write fract part
    if (fractionalDigits != 0)
    {
        int fractZeros= firstNonZero - 1;
        if ( fractionalDigits > 0 && fractZeros > fractionalDigits )
            fractZeros= fractionalDigits;

        for ( int i= 0 ; i < fractZeros ; ++i )
            buffer[idx++]= '0';

        int  qtyDigits=        fractionalDigits - fractZeros;
        int  actDigit=         MaxFloatSignificantDigits + 1;
        int  cntOmittedZeros=  0;
        int  cntDigits=        0;
        bool printStarted=     false;
        while (    fractPart > 0
                && ( qtyDigits< 0 || cntDigits < qtyDigits )
              )
        {
            --actDigit;

            // get next d
            int digitValue=   static_cast<int>(  ( fractPart  / pow10_0to19[actDigit] ) );

            ALIB_ASSERT( digitValue <= 9 )

            // don't write, yet?
            if ( (printStarted|= (digitValue != 0)) == false )
                continue;
            ++cntDigits;

            // write digit unless its a '0'
            if ( digitValue == 0 )
                ++cntOmittedZeros;
            else
            {
                for ( int i= 0; i< cntOmittedZeros ; ++i )
                    buffer[idx++]= '0';
                cntOmittedZeros= 0;
                buffer[idx++]= static_cast<TChar>( 48 + digitValue ); // 48= '0'
            }

            // next
            fractPart=    fractPart % pow10_0to19[actDigit];
        }

        // assure that if -1 for fractDigits if given,at least 1 digit is printed
        if (fractionalDigits < 0 )
            qtyDigits= 1;

        // write omitted zeros
        if ( cntDigits < qtyDigits )
        {
            if ( hasBits(nf.Flags, NumberFormatFlags::OmitTrailingFractionalZeros) )
            {
                if( cntDigits == 0 )
                    buffer[idx++]= '0';
            }
            else
            {
                for ( int i= 0; i< cntOmittedZeros ; ++i )
                    buffer[idx++]= '0';
                cntDigits+= cntOmittedZeros;

                // write missing digits
                for ( int i= cntDigits; i< qtyDigits; ++i )
                    buffer[idx++]= '0';
            }
        }
    }

    // write eNN
    if ( scientific )
    {
        int p= 0;
        while( nf.ExponentSeparator[p] != '\0' )
            buffer[idx++]= nf.ExponentSeparator[p++];

        if ( exp10 < 0 )
            buffer[idx++]= '-';
        else if ( hasBits(nf.Flags, NumberFormatFlags::WriteExponentPlusSign) )
            buffer[idx++]= '+';

        idx= WriteDecUnsigned( static_cast<uint64_t>(exp10 >= 0 ? exp10 : -exp10), buffer, idx, 2, nf );
    }

    return idx;
}


template uint64_t ParseDecDigits  <nchar>( const TString<nchar>&, integer& );
template  int64_t ParseInt        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
template uint64_t ParseDec        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
template uint64_t ParseBin        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
template uint64_t ParseHex        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
template uint64_t ParseOct        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
template double   ParseFloat      <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
template integer  WriteDecUnsigned<nchar>( uint64_t, nchar*, integer, int , const TNumberFormat<nchar>& );
template integer  WriteDecSigned  <nchar>( int64_t , nchar*, integer, int , const TNumberFormat<nchar>& );
template integer  WriteBin        <nchar>( uint64_t, nchar*, integer, int , const TNumberFormat<nchar>& );
template integer  WriteHex        <nchar>( uint64_t, nchar*, integer, int , const TNumberFormat<nchar>& );
template integer  WriteOct        <nchar>( uint64_t, nchar*, integer, int , const TNumberFormat<nchar>& );
template integer  WriteFloat      <nchar>( double  , nchar*, integer, int , const TNumberFormat<nchar>& );

template uint64_t ParseDecDigits  <wchar>( const TString<wchar>&, integer& );
template  int64_t ParseInt        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
template uint64_t ParseDec        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
template uint64_t ParseBin        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
template uint64_t ParseHex        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
template uint64_t ParseOct        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
template double   ParseFloat      <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
template integer  WriteDecUnsigned<wchar>( uint64_t, wchar*, integer, int , const TNumberFormat<wchar>& );
template integer  WriteDecSigned  <wchar>( int64_t , wchar*, integer, int , const TNumberFormat<wchar>& );
template integer  WriteBin        <wchar>( uint64_t, wchar*, integer, int , const TNumberFormat<wchar>& );
template integer  WriteHex        <wchar>( uint64_t, wchar*, integer, int , const TNumberFormat<wchar>& );
template integer  WriteOct        <wchar>( uint64_t, wchar*, integer, int , const TNumberFormat<wchar>& );
template integer  WriteFloat      <wchar>( double  , wchar*, integer, int , const TNumberFormat<wchar>& );

template uint64_t ParseDecDigits  <xchar>( const TString<xchar>&, integer& );
template  int64_t ParseInt        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
template uint64_t ParseDec        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
template uint64_t ParseBin        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
template uint64_t ParseHex        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
template uint64_t ParseOct        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
template double   ParseFloat      <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
template integer  WriteDecUnsigned<xchar>( uint64_t, xchar*, integer, int , const TNumberFormat<xchar>& );
template integer  WriteDecSigned  <xchar>( int64_t , xchar*, integer, int , const TNumberFormat<xchar>& );
template integer  WriteBin        <xchar>( uint64_t, xchar*, integer, int , const TNumberFormat<xchar>& );
template integer  WriteHex        <xchar>( uint64_t, xchar*, integer, int , const TNumberFormat<xchar>& );
template integer  WriteOct        <xchar>( uint64_t, xchar*, integer, int , const TNumberFormat<xchar>& );
template integer  WriteFloat      <xchar>( double  , xchar*, integer, int , const TNumberFormat<xchar>& );


#endif // !defined(ALIB_DOX)
}}} // namespace [alib::strings::detail]
