// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
    #include "alib/strings/numberformat.hpp"
#endif

#if defined( _WIN32 ) || defined(__APPLE__)
    #include <clocale>
#endif
#include <cmath>
#include <algorithm>

#if defined(_MSC_VER)
    #include <intrin.h>
#endif

namespace aworx { namespace lib { namespace strings {

//! @cond NO_DOX

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


#if defined(__GNUC__) || defined(_WIN64)
constexpr const uint8_t binSizeToDecSize[]
{
    20, 19, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16, 16, 15,
    15, 15, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10,
    10, 10, 10,  9,  9,  9,  8,  8,  8,  7,  7,  7,  7,  6,  6,  6,
    05,  5,  5,  4,  4,  4,  4,  3,  3,  3,  2,  2,  2,  1,  1,  1
};
#endif

//! @endcond

// #################################################################################################
// static fields
// #################################################################################################
template ALIB_API NumberFormatBase<char>    NumberFormatBase<char>::Global;
template ALIB_API NumberFormatBase<char>    NumberFormatBase<char>::Computational;

// #################################################################################################
//  Set methods
// #################################################################################################

template<typename TChar>
void NumberFormatBase<TChar>::SetFromLocale()
{
    struct lconv * lc=localeconv();

    DecimalPointChar=       *(lc->decimal_point);
    ThousandsGroupChar=     *(lc->thousands_sep);
}

template<typename TChar>
void NumberFormatBase<TChar>::Set( NumberFormatBase* other )
{
    if ( other == nullptr )
        other= &Global;

    // let C++ do the job
    *this= *other;
}


//! @cond NO_DOX
template<>
void NumberFormatBase<char>::SetComputational()
{
    DecimalPointChar=               '.';
    ForceDecimalPoint=              true;
    ReadGroupChars=
    WriteGroupChars=
    OmitTrailingFractionalZeros=
    WriteExponentPlusSign=
    HexLowerCase=
    ForceScientific=                false;
    Whitespaces=                    NDefaultWhitespaces;
    PlusSign=                       '\0';

    // literals
    ExponentSeparator               = "E";
    INFLiteral                      = "INF";
    NANLiteral                      = "NAN";
    BinLiteralPrefix                = "0b";
    HexLiteralPrefix                = "0x";
    OctLiteralPrefix                = "0o";


    // automatic field width (->minimum size of maximum accuracy)
    DecMinimumFieldWidth=
    BinFieldWidth=
    HexFieldWidth=
    OctFieldWidth=
    IntegralPartMinimumWidth=
    FractionalPartWidth=            -1;

    // group characters
    LeadingGroupCharReplacement=    ' ';

    ThousandsGroupChar=             ',';

    BinNibbleGroupChar=
    HexWordGroupChar=
    HexWord32GroupChar=
    OctGroupChar=                   '\'';

    HexByteGroupChar=               '\0';
    BinByteGroupChar=               '-';
    BinWordGroupChar=               '=';
    BinWord32GroupChar=             '#';
}

template<>
void NumberFormatBase<wchar_t >::SetComputational()
{
    DecimalPointChar=               '.';
    ForceDecimalPoint=              true;
    ReadGroupChars=
    WriteGroupChars=
    OmitTrailingFractionalZeros=
    WriteExponentPlusSign=
    HexLowerCase=
    ForceScientific=                false;
    Whitespaces=                    WDefaultWhitespaces;
    PlusSign=                       '\0';

    // literals
    ExponentSeparator               = L"E";
    INFLiteral                      = L"INF";
    NANLiteral                      = L"NAN";
    BinLiteralPrefix                = L"0b";
    HexLiteralPrefix                = L"0x";
    OctLiteralPrefix                = L"0o";


    // automatic field width (->minimum size of maximum accuracy)
    DecMinimumFieldWidth=
    BinFieldWidth=
    HexFieldWidth=
    OctFieldWidth=
    IntegralPartMinimumWidth=
    FractionalPartWidth=            -1;

    // group characters
    LeadingGroupCharReplacement=    ' ';

    ThousandsGroupChar=             ',';

    BinNibbleGroupChar=
    HexWordGroupChar=
    HexWord32GroupChar=
    OctGroupChar=                   '\'';

    HexByteGroupChar=               '\0';
    BinByteGroupChar=               '-';
    BinWordGroupChar=               '=';
    BinWord32GroupChar=             '#';
}
//! @endcond

// #################################################################################################
//  Integer conversion
// #################################################################################################

template<typename TChar>
uint64_t NumberFormatBase<TChar>::ParseDecDigits( const StringBase<TChar>& src, integer& idx ) //static
{
    uint64_t result=    0;

    // check
    integer length= src.Length();
    if( idx >= length )
        return 0;

    const TChar* buf=    src.Buffer();

    // read number
    while ( idx < length )
    {
        const TChar c= buf[idx];
        if ( c < '0' || c > '9' )
            break;

        result= ( result * 10 ) + static_cast<uint64_t>( c - '0'  );
        idx++;
    }

    // that's it
    return result;
}

// compilers can't know that the parse-methods above leave idx as it is or succeed.
// Therefore it would warn that result may not be initialized. But this will never happen.
ALIB_WARNINGS_UNINITIALIZED_OFF

template<typename TChar>
int64_t   NumberFormatBase<TChar>::ParseInt( const StringBase<TChar>& src, integer& startIdx ) const
{

    integer srcLength= src.Length();

    if ( startIdx < 0 || startIdx >= srcLength )
        return 0;

    // get buffer and index behind whitespaces
    const TChar* buffer= src.Buffer();


    integer idx= src. template IndexOfAny<Inclusion::Exclude, false>( Whitespaces, startIdx );
    if ( idx < 0 )
        return 0;

    // read sign
    bool negative;
    if ( (negative= (buffer[idx] == '-')) == true || buffer[idx] == '+' )
    {
        if( (idx= src. template IndexOfAny<Inclusion::Exclude, false>( Whitespaces, idx + 1 ) ) == -1 )
            return 0;
    }

    int64_t result;
    integer oldIdx= idx;

    integer prefixLen;

    // try parsing hexadecimal
    if(     ( prefixLen= HexLiteralPrefix.Length())  > 0
        &&  ( idx + prefixLen  < srcLength )
        &&  CString<TChar>::Equal(HexLiteralPrefix.Buffer(), buffer + idx, prefixLen )
        &&  Whitespaces.IndexOf( buffer[ idx + prefixLen ] ) < 0     )
    {
        idx+= prefixLen;
        result=  static_cast<int64_t>( ParseHex( src, idx ) );
        if( idx - prefixLen == oldIdx )
            idx-= prefixLen;
    }

    // try parsing binary
    else if(     (prefixLen= BinLiteralPrefix.Length())  > 0
             &&  ( idx + prefixLen < srcLength )
             &&  CString<TChar>::Equal(BinLiteralPrefix.Buffer(), buffer + idx, prefixLen )
             &&  Whitespaces.IndexOf( buffer[ idx + prefixLen ] ) < 0     )
    {
        idx+= prefixLen;
        result=  static_cast<int64_t>( ParseBin( src, idx ) );
        if( idx - prefixLen == oldIdx )
            idx-= prefixLen;
    }

    // try parsing octal
    else if(     (prefixLen= OctLiteralPrefix.Length()) > 0
             &&  ( idx + prefixLen  < srcLength )
             &&  CString<TChar>::Equal(OctLiteralPrefix.Buffer(), buffer + idx, prefixLen )
             &&  Whitespaces.IndexOf( buffer[ idx + prefixLen ] ) < 0     )
    {
        idx+= prefixLen;
        result=  static_cast<int64_t>( ParseOct( src, idx ) );
        if( idx - prefixLen == oldIdx )
            idx-= prefixLen;
    }


    // parse decimal
    if ( idx == oldIdx )
        result=  static_cast<int64_t>( ParseDec( src, idx ) );

    // Any success? if yes, we move the given start pointer

    if ( idx != oldIdx )
        startIdx= idx;


    return negative ? static_cast<int64_t>(-result)
                    : static_cast<int64_t>( result);
}

ALIB_WARNINGS_RESTORE

template<typename TChar>
uint64_t NumberFormatBase<TChar>::ParseDec( const StringBase<TChar>& src, integer& startIdx )  const
{
    uint64_t result=    0;

    // read whitespaces
    integer idx= src. template IndexOfAny<Inclusion::Exclude>( Whitespaces, startIdx );
    if ( idx < 0 )
        return 0;

    // read number
    bool        charFound=  false;
    integer    length=     src.Length();
    const TChar* buf=        src.Buffer();
    while ( idx < length )
    {
        const TChar c= buf[idx];
        if ( charFound &&ReadGroupChars && c!= '\0' && c == ThousandsGroupChar  )
        {
            idx++;
            continue;
        }

        if ( c < '0' || c > '9' )
            break;

        charFound= true;
        result= ( result * 10 ) + static_cast<uint64_t>( c - '0' );

        idx++;
    }

    if ( charFound )
        startIdx= idx;
    return result;
}


template<typename TChar>
uint64_t NumberFormatBase<TChar>::ParseBin( const StringBase<TChar>& src, integer& startIdx )  const
{
    uint64_t result=    0;

    // read whitespaces
    integer idx= src. template IndexOfAny<Inclusion::Exclude>( Whitespaces, startIdx );
    if ( idx < 0 )
        return 0;

    // read number
    bool        charFound=  false;
    integer    length=     src.Length();
    const TChar* buf=        src.Buffer();
    while ( idx < length )
    {
        TChar c= buf[idx];
        if (    charFound
             && ReadGroupChars
             && c != '\0'
             && (    c == BinNibbleGroupChar
                  || c == BinByteGroupChar
                  || c == BinWordGroupChar
                  || c == BinWord32GroupChar )    )
        {
            idx++;
            continue;
        }

        if ( c == '0'  )
        {
            result<<= 1;
            idx++;
            charFound= true;
            continue;
        }

        if ( c == '1'  )
        {
            result<<= 1;
            result|=1;
            idx++;
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
uint64_t NumberFormatBase<TChar>::ParseHex( const StringBase<TChar>& src, integer& startIdx )  const
{
    uint64_t result=    0;

    // read whitespaces
    integer idx= src. template IndexOfAny<Inclusion::Exclude>( Whitespaces, startIdx );
    if ( idx < 0 )
        return 0;

    // read number
    bool        charFound=  false;
    integer    length=     src.Length();
    const TChar* buf=        src.Buffer();
    while ( idx < length )
    {
        TChar c= buf[idx];
        if (    charFound
             && ReadGroupChars
             && c != '\0'
             && (    c == HexByteGroupChar
                  || c == HexWordGroupChar
                  || c == HexWord32GroupChar )    )
        {
            idx++;
            continue;
        }

        if ( c >= '0' && c <= '9' )
        {
            result<<= 4;
            result|=  static_cast<uint64_t>(c - '0');
            idx++;
            charFound= true;
            continue;
        }

        if ( c >= 'A' && c <= 'F' )
        {
            result<<= 4;
            result|=  static_cast<uint64_t>(c - 'A' + 10 );
            idx++;
            charFound= true;
            continue;
        }

        if ( c >= 'a' && c <= 'f' )
        {
            result<<= 4;
            result|=  static_cast<uint64_t>(c - 'a' + 10 );
            idx++;
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
uint64_t NumberFormatBase<TChar>::ParseOct( const StringBase<TChar>& src, integer& startIdx )  const
{
    uint64_t result=    0;

    // read whitespaces
    integer idx= src. template IndexOfAny<Inclusion::Exclude>( Whitespaces, startIdx );
    if ( idx < 0 )
        return 0;

    // read number
    bool        charFound=  false;
    integer    length=     src.Length();
    const TChar* buf=        src.Buffer();
    while ( idx < length )
    {
        TChar c= buf[idx];
        if (     charFound
             && ReadGroupChars
             && c != 0
             && c == OctGroupChar )
        {
            idx++;
            continue;
        }

        if ( c >= '0' && c <= '7' )
        {
            result<<= 3;
            result|=  static_cast<uint64_t>(c - '0');
            idx++;
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
double NumberFormatBase<TChar>::ParseFloat( const StringBase<TChar>& src, integer& startIdx )  const
{
    // checks
    if( startIdx < 0 || startIdx >= src.Length() )
        return 0.0;

    // get pointers
    const TChar* buf=    src.Buffer() + startIdx;
    const TChar* bufEnd= src.Buffer() + src.Length();

    // read whitespaces
    {
        integer skip= src. template IndexOfAny<Inclusion::Exclude>( Whitespaces, startIdx );
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

        integer skip= StringBase<TChar>(buf, bufEnd-buf ). template IndexOfAny<Inclusion::Exclude, false>( Whitespaces );
        if( skip < 0 )
            return 0.0;
        buf+= skip;
    }

    // NaN, Infinite
    if(     buf + NANLiteral.Length() - 1 <= bufEnd
        &&  NANLiteral. template CompareTo<true, Case::Ignore>( StringBase<TChar>( buf, NANLiteral.Length() ) ) == 0    )
    {
        startIdx= buf - src.Buffer()  + NANLiteral.Length();
        return std::numeric_limits<double>::quiet_NaN();
    }

    if(     buf + INFLiteral.Length() - 1 <= bufEnd
        &&  INFLiteral. template CompareTo<true, Case::Ignore>( StringBase<TChar>( buf, INFLiteral.Length() ) ) == 0    )
    {
        startIdx= buf -  src.Buffer() + INFLiteral.Length();
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
            result= static_cast<double>( ParseDec( StringBase<TChar>(buf, bufEnd - buf), intIdx ) );
            buf+= intIdx;
        }

        // end?
        ALIB_ASSERT_ERROR( buf <= bufEnd, ASTR("Error in float parsing algorithm.") )
        if ( buf == bufEnd )
        {
            startIdx= buf - src.Buffer();
            return negative ? -result : result;
        }
    }

    if( DecimalPointChar == *buf  )
    {
        // consume dot
        buf++;

        // read number after dot
        if (      buf <  bufEnd
             &&  *buf >= '0'  &&  *buf <= '9' )
        {
            integer intIdx= 0;
            double intValue= static_cast<double>( ParseDecDigits( StringBase<TChar>(buf, bufEnd - buf), intIdx ) );
            buf+= intIdx;
            result+= ( intValue / pow( 10, intIdx ) );
        }
    }
    else if( !integralPartFound )
        return 0.0; // index not moved, indicates failure.

    // read eNNN
    if ( buf <  bufEnd )
    {
        auto oldBuf= buf;
        bool eSepFound=  false;
        integer  sepLen= ExponentSeparator.Length();
        if ( buf + sepLen < bufEnd )
        {
            integer pos= 0;
            while (     pos < sepLen
                    &&  ExponentSeparator. template CharAt<false>(pos) == *(buf + pos) )
                pos++;
            if ( (eSepFound= ( pos == sepLen ) ) == true )
                buf += pos;
        }
        if ( !eSepFound && ( *buf == 'e' || *buf == 'E' ) )
        {
            buf++;
            eSepFound= true;
        }


        if (eSepFound && buf < bufEnd)
        {
            bool negativeE= false;
            if ( (negativeE= (*buf == '-') ) == true ||  *buf == '+' )
                buf++;

            if( buf < bufEnd )
            {
                integer idx= 0;
                int exp= static_cast<int>( ParseDecDigits( StringBase<TChar>(buf, bufEnd - buf), idx ) );
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
integer NumberFormatBase<TChar>::WriteDecSigned( int64_t value, TChar* buffer,
                                                 integer idx, int overrideWidth )              const
{
    integer oldIdx= idx;

    // write sign and turn negative to positive
    uint64_t uValue;
    if ( value >= 0 )
    {
        uValue= static_cast<uint64_t>( value );
        if ( PlusSign != '\0' )
            buffer[idx++]= PlusSign;
    }
    else
    {
        uValue= static_cast<uint64_t>( -value );
        buffer[idx++]= '-';
    }

    int width= overrideWidth != 0 ? overrideWidth
                                  : DecMinimumFieldWidth;
    if( idx != oldIdx && width > 1 )
    {
        width--;
    }

    return WriteDecUnsigned( uValue, buffer, idx, width );
}

template<typename TChar>
integer NumberFormatBase<TChar>::writeDecUnsigned( uint64_t value, TChar* buffer,
                                                   integer idx, int width          )           const
{
    if ( width < 1  )   width=  1;

    // get the number of dec digits in value
    int digitsInValue;
    {
        if( value == 0 )
            digitsInValue = 1;
        else
        {
            #if defined(__GNUC__) || defined(_WIN64)

                int leadingBinaryZeros;
                #if defined(__GNUC__)
                {
                         if( sizeof(uint64_t) == sizeof( unsigned int       ) )  leadingBinaryZeros= __builtin_clz  ( value );
                    else if( sizeof(uint64_t) == sizeof( unsigned long      ) )  leadingBinaryZeros= __builtin_clzl ( value );
                    else if( sizeof(uint64_t) == sizeof( unsigned long long ) )  leadingBinaryZeros= __builtin_clzll( value );
                }
                #else
                {
                    #include <intrin.h>
                    if( value != 0 )
                    {
                        unsigned long bitScanReverse;
                        _BitScanReverse64( &bitScanReverse, value );
                        leadingBinaryZeros= 63 - bitScanReverse;
                    }
                    else
                        leadingBinaryZeros= 0;
                }
                #endif

                digitsInValue= binSizeToDecSize[leadingBinaryZeros];
                // could be one lower due to the rest after the most significant bit
                if( value < pow10_0to19[digitsInValue-1] )
                    digitsInValue--;

            #else
                // we have to treat the 0, so let's treat 0 to 9 then
                if( value < 10 )
                    digitsInValue= 1;

                // binary search
                else
                {
                    int lower=  1;
                    int upper= 19;
                    digitsInValue=    4;
                    while( lower < upper  )
                    {
                        if( value < pow10_0to19[digitsInValue] )
                            upper= digitsInValue -1;
                        else
                            lower= digitsInValue;
                        digitsInValue= ( lower + upper ) / 2 + 1;
                    }
                }
            #endif
            ALIB_ASSERT_ERROR(                                  value >= pow10_0to19[ digitsInValue - 1 ]
                                    && ( digitsInValue == 20 || value <  pow10_0to19[ digitsInValue     ] )
                                ,   ASTR("Error in digitsInValue calculation"));
        }
    }


    // calc printable digits and print leading group char replacement ' '
    int printDigits;
    {
        int requestedDigits;
        if ( WriteGroupChars && ThousandsGroupChar != '\0' )
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
        ALIB_ASSERT_ERROR( width - 1 <= printDigits + (printDigits - 1) / 3 , ASTR("Invariant broken") );
        if( printDigits >1  && width > printDigits + (printDigits - 1) / 3 )
            buffer[idx++]= LeadingGroupCharReplacement;
    }

    // main loop
    int  actDigit=      printDigits;
    while ( actDigit > 0 )
    {
        // print normal digit
        int digitValue=   static_cast<int>( ( value / pow10_0to19[actDigit-1] ) );
        ALIB_ASSERT( digitValue <= 9 );

        // write group character
        if(     WriteGroupChars && ThousandsGroupChar != '\0'
            &&  actDigit != printDigits
            &&  actDigit % 3 == 0
          )
            buffer[idx++]= ThousandsGroupChar;

        // write character
        buffer[idx++]=  static_cast<TChar>( 48 + digitValue ); // 48= '0'

        // next
        value=    value % pow10_0to19[actDigit-1];

        actDigit--;
    }

    return idx;
}

template<typename TChar>
integer NumberFormatBase<TChar>::WriteBin(  uint64_t value, TChar* buffer,
                                            integer idx, int overrideWidth    )                const
{
    // how many digits in a grouping block?
    int groupWidth=    !WriteGroupChars            ? 0
                     : BinNibbleGroupChar  != '\0' ? 4
                     : BinByteGroupChar    != '\0' ? 8
                     : BinWordGroupChar    != '\0' ? 16
                     : BinWord32GroupChar  != '\0' ? 32    :    0;

    // if the first "digit" is a separator, we will write a space instead.
    // (we do the best to keep the width of the output intact if given)
    int nextSeparator= 0; // 0: dont write, 1= space, 2 normal

    // adjust minDigits to 0..64 (if 0 demanded, 0 is given!)
    int digits=  overrideWidth != 0 ? overrideWidth  : BinFieldWidth;
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
    {
        // zero value?
        if (value == 0)
            digits= 1;

        // calc
        else
        {
            #if defined(__GNUC__) || defined(_WIN64)

                #if defined(__GNUC__)
                {
                         if( sizeof(uint64_t) == sizeof( unsigned int       ) )  digits= 64 - __builtin_clz  ( value );
                    else if( sizeof(uint64_t) == sizeof( unsigned long      ) )  digits= 64 - __builtin_clzl ( value );
                    else if( sizeof(uint64_t) == sizeof( unsigned long long ) )  digits= 64 - __builtin_clzll( value );
                }
                #else
                {
                    #include <intrin.h>
                    unsigned long Index;
                    _BitScanReverse64( &Index, value );
                    digits= Index + 1;
                }
                #endif

            #else
                // rough
                digits =    value < (((uint64_t)1) <<  8) ? 8
                          : value < (((uint64_t)1) << 16) ? 16
                          : value < (((uint64_t)1) << 32) ? 32
                          : value < (((uint64_t)1) << 48) ? 48  : 64;

                // fine with loop
                uint64_t actSize=  ((uint64_t)1) << (digits - 1);
                while( value < actSize )
                {
                    digits--;
                    actSize >>= 1;
                }
            #endif
        }
    }

    // check now for oversize
    if ( digits > 64 )
           digits= 64;

    uint64_t testValue= static_cast<uint64_t>(1) << (digits - 1);
    while ( digits > 0)
    {
        // write the separator
        if( groupWidth != 0 && ( digits % groupWidth) == 0 )
        {
            if ( nextSeparator != 0 )
                buffer[idx++]=  nextSeparator == 1     ? LeadingGroupCharReplacement :
                                ( digits % 32 == 0 )   ? BinWord32GroupChar       :
                                ( digits % 16 == 0 )   ? BinWordGroupChar         :
                                ( digits %  8 == 0 )   ? BinByteGroupChar         : BinNibbleGroupChar;
        }
        nextSeparator= 2; // from now on write separators

        // write digit
        buffer[idx++]= ( value & testValue ) == 0L ? '0' : '1';

        // next
        testValue >>= 1;
        digits--;
    }

    return idx;
}

template<typename TChar>
integer NumberFormatBase<TChar>::WriteHex(  uint64_t value, TChar* buffer,
                                            integer idx, int overrideWidth )                   const
{
    // how many digits in a grouping block?
    int groupWidth=   !WriteGroupChars            ? 0
                    : HexByteGroupChar    != '\0' ? 2
                    : HexWordGroupChar    != '\0' ? 4
                    : HexWord32GroupChar  != '\0' ? 8    :    0;

    // if the first "digit" is a separator, we will write a space instead.
    // (we do the best to keep the width of the output intact if given)
    int nextSeparator= 0; // 0: dont write, 1= space, 2 normal

    int digits= overrideWidth != 0 ? overrideWidth : HexFieldWidth;

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
    {
        // zero value?
        if (value == 0)
            digits= 1;

        // calc
        else
        {
            #if defined(__GNUC__) || defined(_WIN64)

                int binDigits;
                #if defined(__GNUC__)
                {
                         if( sizeof(uint64_t) == sizeof( unsigned int       ) )  binDigits= 64 - __builtin_clz  ( value );
                    else if( sizeof(uint64_t) == sizeof( unsigned long      ) )  binDigits= 64 - __builtin_clzl ( value );
                    else if( sizeof(uint64_t) == sizeof( unsigned long long ) )  binDigits= 64 - __builtin_clzll( value );
                }
                #else
                {
                    #include <intrin.h>
                    unsigned long Index;
                    _BitScanReverse64( &Index, value );
                    binDigits= Index + 1;
                }
                #endif

                digits= (binDigits-1)/4 + 1;

            #else
                if ( value >= 0x8000000000000000 )
                    digits= 16;
                else
                {
                    // loop
                    digits= 1;

                    uint64_t testValue= 0xF;
                    while( value > testValue )
                    {
                        digits++;
                        testValue= (testValue << 4 ) | 0xF;
                    }
                }
            #endif
        }
    }

    // check now for oversize
    if ( digits > 16 )
           digits= 16;

    unsigned int characterA= static_cast<unsigned int>( HexLowerCase ? 'a' : 'A' );
    int          shiftBits= (digits -1 ) * 4;
    uint64_t testMask=  static_cast<uint64_t>( 15 ) << shiftBits;
    while ( digits > 0)
    {
        // write the separator
        if( groupWidth != 0 && ( digits % groupWidth) == 0 )
        {
            if ( nextSeparator != 0 )
                buffer[idx++]=  nextSeparator == 1    ? LeadingGroupCharReplacement   :
                                ( digits % 8 == 0 )   ? HexWord32GroupChar         :
                                ( digits % 4 == 0 )   ? HexWordGroupChar           : HexByteGroupChar;
        }
        nextSeparator= 2; // from now on write separators

        // write digit
        unsigned int nibble= static_cast<unsigned int>( ( value & testMask ) >> shiftBits );
        buffer[idx++]=   ( nibble < 10 )   ?  static_cast<TChar>('0' + nibble)
                                           :  static_cast<TChar>(characterA + (nibble - 10));

        // next
        testMask  >>= 4;
        shiftBits -=  4;
        digits--;
    }

    return idx;
}

template<typename TChar>
integer NumberFormatBase<TChar>::WriteOct(  uint64_t value, TChar* buffer,
                                            integer idx, int overrideWidth )                   const
{
    // how many digits in a grouping block?
    const int groupWidth=  WriteGroupChars && OctGroupChar != '\0' ? 3 : 0;

    // if the first "digit" is a separator, we will write a space instead.
    // (we do the best to keep the width of the output intact if given)
    int nextSeparator= 0; // 0: dont write, 1= space, 2 normal

    // adjust minDigits to 0..64 (if 0 demanded, 0 is given!)
    int digits= overrideWidth != 0 ? overrideWidth : OctFieldWidth;
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
    {
        // zero value?
        if (value == 0)
            digits= 1;

        // calc
        else
        {
            #if defined(__GNUC__) || defined(_WIN64)

                int binDigits;
                #if defined(__GNUC__)
                {
                         if( sizeof(uint64_t) == sizeof( unsigned int       ) )  binDigits= 64 - __builtin_clz  ( value );
                    else if( sizeof(uint64_t) == sizeof( unsigned long      ) )  binDigits= 64 - __builtin_clzl ( value );
                    else if( sizeof(uint64_t) == sizeof( unsigned long long ) )  binDigits= 64 - __builtin_clzll( value );
                }
                #else
                {
                    #include <intrin.h>
                    unsigned long Index;
                    _BitScanReverse64( &Index, value );
                    binDigits= Index + 1;
                }
                #endif

                digits= (binDigits-1)/3 + 1;

            #else
                if ( value >= 01000000000000000000000 )
                    digits= 22;
                else
                {
                    //  loop
                    digits= 1;
                    uint64_t testValue= 0x7;
                    while( value > testValue )
                    {
                        digits++;
                        testValue= (testValue << 3 ) | 0x7;
                    }
               }
            #endif
        }
    }

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
                buffer[idx++]=  nextSeparator == 1    ? LeadingGroupCharReplacement : OctGroupChar;
        }
        nextSeparator= 2; // from now on write separators

        // write digit
        auto octet= ( value & (static_cast<uint64_t>(7) << shiftBits) ) >> shiftBits;
        buffer[idx++]= static_cast<TChar>('0' + octet);

        // next
        shiftBits -=  3;
        digits--;
    }

    return idx;
}

template<typename TChar>
integer NumberFormatBase<TChar>::WriteFloat( double value,  TChar* buffer,
                                            integer idx, int overrideWidth )                   const
{
    int integralWidth= overrideWidth != 0 ? overrideWidth : IntegralPartMinimumWidth;


    // check for NaN, negative zero
    auto classification=  std::fpclassify(value);
    if( classification == FP_NAN )    {  return idx+= NANLiteral.CopyTo(buffer + idx);   }
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
        else if ( PlusSign != '\0' )
            buffer[idx++]= PlusSign;
        return idx+= INFLiteral.CopyTo(buffer + idx);
    }


    constexpr int MaxFloatSignificantDigits= 16;


    // calc dot position
    int exp10=      value != 0.0   ? static_cast<int>( floor(log10( value ) ) )
                                   : 0;

    // decide if we are using scientific format (with e) or not
    bool scientific=  (     ForceScientific
                        || (  integralWidth < 0 && FractionalPartWidth < 0  &&  ( exp10 > 6 || exp10 <= -5 )  )
                        || (  integralWidth       > 0 && exp10 != 0 && exp10 >= (MaxFloatSignificantDigits - integralWidth       - 1 ) )
                        || (  FractionalPartWidth > 0 && exp10 != 0 && exp10 >= (MaxFloatSignificantDigits - FractionalPartWidth - 1 ) )
                      );

    integralWidth=         (std::min) ( integralWidth,      15 ); //Note: the brackets around (std::min) are
    int fractionalDigits=  (std::min) ( FractionalPartWidth,15 ); //      to avoid conflicts with Windows 'min' macro



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
            ALIB_ASSERT( MaxFloatSignificantDigits - firstNonZero < 20);
            while ( fractPart < pow10_0to19[ MaxFloatSignificantDigits - firstNonZero - 1 ] )
                firstNonZero++;
            ALIB_ASSERT( MaxFloatSignificantDigits - firstNonZero > 0);
        }
        firstNonZero++;

        unusedFractDigits= fractionalDigits >= 0 ?  MaxFloatSignificantDigits - fractionalDigits
                                                 :  1;
    }

     // normal output, number > 0
     else if (exp10 >= 0 )
     {
        int intPartSize= MaxFloatSignificantDigits - exp10;
        ALIB_ASSERT( intPartSize > 0  && intPartSize <= MaxFloatSignificantDigits );
        intPart=     static_cast<uint64_t>(llrint( value * pow( 10, intPartSize ) ));
        fractPart=   intPart %  pow10_0to19[ intPartSize ];
        intPart=     intPart /  pow10_0to19[ intPartSize ];

        // determine first non zero fract number
        firstNonZero= 0;
        if ( fractPart > 0 )
        {
            ALIB_ASSERT( intPartSize - firstNonZero < 20 );
            while ( fractPart < pow10_0to19[ intPartSize - firstNonZero - 1 ] )
                firstNonZero++;
            ALIB_ASSERT( intPartSize - firstNonZero > 0 );
        }
        firstNonZero++;

        unusedFractDigits= fractionalDigits >= 0 ?  intPartSize - fractionalDigits
                                                 :  1;
     }

    // normal output, number  < 0
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
            fractPart++;
            int  overflowDigit= 0;
            bool overflow=      false;
            while (    (overflowDigit <= fractionalDigits || fractionalDigits < 0 )
                    && (overflow|= fractPart == pow10_0to19[ overflowDigit ]) == false
                    &&  fractPart > pow10_0to19[ overflowDigit ]
                   )
                overflowDigit++;

            if ( overflow )
            {
                if ( overflowDigit == fractionalDigits )
                {
                    fractPart= 0;
                    intPart++;
                }
                else
                {
                    ALIB_ASSERT( firstNonZero > 1 );
                    firstNonZero--;
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
    else if ( PlusSign != '\0' )
        buffer[idx++]= PlusSign;

    // write int part
    if ( intPart != 0L || integralWidth != 0 )
        idx= WriteDecUnsigned( intPart, buffer, idx, integralWidth );

    // write dot
    if ( fractionalDigits != 0 || ForceDecimalPoint )
        buffer[idx++]=  DecimalPointChar;

    // write fract part
    if (fractionalDigits != 0)
    {
        int fractZeros= firstNonZero - 1;
        if ( fractionalDigits > 0 && fractZeros > fractionalDigits )
            fractZeros= fractionalDigits;

        for ( int i= 0 ; i < fractZeros ; i++ )
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
            actDigit--;

            // get next d
            int digitValue=   static_cast<int>(  ( fractPart  / pow10_0to19[actDigit] ) );

            ALIB_ASSERT( digitValue <= 9 );

            // don't write, yet?
            if ( (printStarted|= (digitValue != 0)) == false )
                continue;
            cntDigits++;

            // write digit unless its a '0'
            if ( digitValue == 0 )
                cntOmittedZeros++;
            else
            {
                for ( int i= 0; i< cntOmittedZeros ; i++ )
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
            if ( OmitTrailingFractionalZeros )
            {
                if( cntDigits == 0 )
                    buffer[idx++]= '0';
            }
            else
            {
                for ( int i= 0; i< cntOmittedZeros ; i++ )
                    buffer[idx++]= '0';
                cntDigits+= cntOmittedZeros;

                // write missing digits
                for ( int i= cntDigits; i< qtyDigits; i++ )
                    buffer[idx++]= '0';
            }
        }
    }

    // write eNN
    if ( scientific )
    {
        int p= 0;
        while( ExponentSeparator[p] != '\0' )
            buffer[idx++]= ExponentSeparator[p++];

        if ( exp10 < 0 )
            buffer[idx++]= '-';
        else if ( WriteExponentPlusSign )
            buffer[idx++]= '+';

        idx= WriteDecUnsigned( static_cast<uint64_t>(exp10 >= 0 ? exp10 : -exp10), buffer, idx, 2 );
    }

    return idx;
}

template void     NumberFormatBase<char   >::Set             ( NumberFormatBase*);
template void     NumberFormatBase<char   >::SetFromLocale   ( );
template uint64_t NumberFormatBase<char   >::ParseDecDigits  ( const StringBase<char>&, integer& );
template  int64_t NumberFormatBase<char   >::ParseInt        ( const StringBase<char>&, integer& ) const;
template uint64_t NumberFormatBase<char   >::ParseDec        ( const StringBase<char>&, integer& ) const;
template uint64_t NumberFormatBase<char   >::ParseBin        ( const StringBase<char>&, integer& ) const;
template uint64_t NumberFormatBase<char   >::ParseHex        ( const StringBase<char>&, integer& ) const;
template uint64_t NumberFormatBase<char   >::ParseOct        ( const StringBase<char>&, integer& ) const;
template double   NumberFormatBase<char   >::ParseFloat      ( const StringBase<char>&, integer& ) const;
template integer  NumberFormatBase<char   >::WriteDecSigned  ( int64_t , char*, integer, int     ) const;
template integer  NumberFormatBase<char   >::WriteBin        ( uint64_t, char*, integer, int     ) const;
template integer  NumberFormatBase<char   >::WriteHex        ( uint64_t, char*, integer, int     ) const;
template integer  NumberFormatBase<char   >::WriteOct        ( uint64_t, char*, integer, int     ) const;
template integer  NumberFormatBase<char   >::WriteFloat      ( double  , char*, integer, int     ) const;
template integer  NumberFormatBase<char   >::writeDecUnsigned( uint64_t, char*, integer, int     ) const;

template void     NumberFormatBase<wchar_t>::Set             ( NumberFormatBase*);
template void     NumberFormatBase<wchar_t>::SetFromLocale   ( );
template uint64_t NumberFormatBase<wchar_t>::ParseDecDigits  ( const StringBase<wchar_t>&, integer& );
template  int64_t NumberFormatBase<wchar_t>::ParseInt        ( const StringBase<wchar_t>&, integer& ) const;
template uint64_t NumberFormatBase<wchar_t>::ParseDec        ( const StringBase<wchar_t>&, integer& ) const;
template uint64_t NumberFormatBase<wchar_t>::ParseBin        ( const StringBase<wchar_t>&, integer& ) const;
template uint64_t NumberFormatBase<wchar_t>::ParseHex        ( const StringBase<wchar_t>&, integer& ) const;
template uint64_t NumberFormatBase<wchar_t>::ParseOct        ( const StringBase<wchar_t>&, integer& ) const;
template double   NumberFormatBase<wchar_t>::ParseFloat      ( const StringBase<wchar_t>&, integer& ) const;
template integer  NumberFormatBase<wchar_t>::WriteDecSigned  ( int64_t , wchar_t*, integer, int     ) const;
template integer  NumberFormatBase<wchar_t>::WriteBin        ( uint64_t, wchar_t*, integer, int     ) const;
template integer  NumberFormatBase<wchar_t>::WriteHex        ( uint64_t, wchar_t*, integer, int     ) const;
template integer  NumberFormatBase<wchar_t>::WriteOct        ( uint64_t, wchar_t*, integer, int     ) const;
template integer  NumberFormatBase<wchar_t>::WriteFloat      ( double  , wchar_t*, integer, int     ) const;
template integer  NumberFormatBase<wchar_t>::writeDecUnsigned( uint64_t, wchar_t*, integer, int     ) const;


}}}// namespace [aworx::lib::strings]
