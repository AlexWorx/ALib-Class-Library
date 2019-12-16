// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
#   include "alib/strings/numberformat.hpp"
#endif
#endif // !defined(ALIB_DOX)

#if defined( _WIN32 ) || defined(__APPLE__)
#   include <clocale>
#endif

namespace aworx { namespace lib { namespace strings {

#if !defined(ALIB_DOX)

// #################################################################################################
//  Set methods
// #################################################################################################

template<typename TChar>
void TNumberFormat<TChar>::SetFromLocale()
{
    struct lconv * lc=localeconv();

    DecimalPointChar=       static_cast<TChar>(  *(lc->decimal_point)  );
    ThousandsGroupChar=     static_cast<TChar>(  *(lc->thousands_sep)  );
}

template<typename TChar>
void TNumberFormat<TChar>::Set( TNumberFormat* other )
{
    if ( other == nullptr )
        other= &Global;

    // let C++ do the job
    *this= *other;
}

namespace {
    template<typename TChar>
    void setComputational( TNumberFormat<TChar>& nf )
    {
        nf.DecimalPointChar=               '.';
        nf.ForceDecimalPoint=              true;
        nf.ReadGroupChars=
        nf.WriteGroupChars=
        nf.OmitTrailingFractionalZeros=
        nf.WriteExponentPlusSign=
        nf.HexLowerCase=
        nf.ForceScientific=                false;
        nf.Whitespaces=                    TT_StringConstants<TChar>::DefaultWhitespaces();
        nf.PlusSign=                       '\0';

        // automatic field width (->minimum size of maximum accuracy)
        nf.DecMinimumFieldWidth=
        nf.BinFieldWidth=
        nf.HexFieldWidth=
        nf.OctFieldWidth=
        nf.IntegralPartMinimumWidth=
        nf.FractionalPartWidth=            -1;

        // group characters
        nf.LeadingGroupCharReplacement=    ' ';

        nf.ThousandsGroupChar=             ',';

        nf.BinNibbleGroupChar=
        nf.HexWordGroupChar=
        nf.HexWord32GroupChar=
        nf.OctGroupChar=                   '\'';

        nf.HexByteGroupChar=               '\0';
        nf.BinByteGroupChar=               '-';
        nf.BinWordGroupChar=               '=';
        nf.BinWord32GroupChar=             '#';

    }
}

template<>
void TNumberFormat<nchar>::SetComputational()
{
    setComputational( *this );

    ExponentSeparator       =          "E"     ;
    INFLiteral              =          "INF"   ;
    NANLiteral              =          "NAN"   ;
    BinLiteralPrefix        =          "0b"    ;
    HexLiteralPrefix        =          "0x"    ;
    OctLiteralPrefix        =          "0o"    ;
}

template<>
void TNumberFormat<wchar>::SetComputational()
{
    setComputational( *this );

    ExponentSeparator       = A_WCHAR( "E"   ) ;
    INFLiteral              = A_WCHAR( "INF" ) ;
    NANLiteral              = A_WCHAR( "NAN" ) ;
    BinLiteralPrefix        = A_WCHAR( "0b"  ) ;
    HexLiteralPrefix        = A_WCHAR( "0x"  ) ;
    OctLiteralPrefix        = A_WCHAR( "0o"  ) ;
}

template<>
void TNumberFormat<xchar>::SetComputational()
{
    setComputational( *this );

    ExponentSeparator       = A_XCHAR( "E"   ) ;
    INFLiteral              = A_XCHAR( "INF" ) ;
    NANLiteral              = A_XCHAR( "NAN" ) ;
    BinLiteralPrefix        = A_XCHAR( "0b"  ) ;
    HexLiteralPrefix        = A_XCHAR( "0x"  ) ;
    OctLiteralPrefix        = A_XCHAR( "0o"  ) ;
}


template void     TNumberFormat<nchar>::Set             ( TNumberFormat*);
template void     TNumberFormat<nchar>::SetFromLocale   ();
template void     TNumberFormat<wchar>::Set             ( TNumberFormat*);
template void     TNumberFormat<wchar>::SetFromLocale   ( );
template void     TNumberFormat<xchar>::Set             ( TNumberFormat*);
template void     TNumberFormat<xchar>::SetFromLocale   ( );

#endif // HPP_ALIB_STRINGS_DETAIL_NUMBERCONVERSION


// #################################################################################################
// static fields
// #################################################################################################
//template ALIB_API TNumberFormat<nchar>    TNumberFormat<nchar>::Global;
//template ALIB_API TNumberFormat<nchar>    TNumberFormat<nchar>::Computational;
//template ALIB_API TNumberFormat<wchar>    TNumberFormat<wchar>::Global;
//template ALIB_API TNumberFormat<wchar>    TNumberFormat<wchar>::Computational;
//template ALIB_API TNumberFormat<xchar>    TNumberFormat<xchar>::Global;
//template ALIB_API TNumberFormat<xchar>    TNumberFormat<xchar>::Computational;




}}}// namespace [aworx::lib::strings]
