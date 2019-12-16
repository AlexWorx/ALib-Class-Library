// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_FORMAT)
#   include "alib/strings/format.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_DETAIL_NUMBERCONVERSION)
#   include "alib/strings/detail/numberconversion.hpp"
#endif

#if ALIB_DEBUG && !defined (HPP_ALIB_FS_DEBUG_TYPEDEMANGLER)
#   include "alib/lib/fs_debug/typedemangler.hpp"
#endif
#endif // !defined(ALIB_DOX)


namespace aworx { namespace lib { namespace strings {

#if defined(ALIB_DOX)
/**
 * \attention
 *   This is a non-existing namespace! It is exclusively defined for the
 *   \https{documentation parser,www.stack.nl/~dimitri/doxygen}.
 *
 * In this <b>"documentation namespace"</b>, you will find specializations of functor
 * \alib{strings,T_Append} which in reality are implemented in parent
 * namespace \ref aworx::lib::strings (as required by C++ language syntax).<br>
 * The rational for tricking the documentation to this pseude-namespace, is to twofold:
 * On the one hand to keep namespace \b %aworx::lib::strings clean and on the other to have
 * an overview of all specializations in one place.
 */
namespace APPENDABLES {
#endif

// #################################################################################################
// Integrals
// #################################################################################################
template<typename TChar>
void T_Append<bool,TChar>::operator()( TAString<TChar>& target, bool b )
{
    target.template _<false>( b ? "true" : "false" );
}

template<typename TChar>
void T_Append<int64_t,TChar>::operator()( TAString<TChar>& target,  int64_t value )
{
    target.EnsureRemainingCapacity(28);//   20 digits, grouping symbol, sign and what have you
    integer length=      target.Length();
    length=  detail::WriteDecSigned ( value, target.VBuffer(), length, 0, TNumberFormat<TChar>::Computational );
    target.SetLength( length );
}

template<typename TChar>
void T_Append<uint64_t,TChar>::operator()( TAString<TChar>& target,  uint64_t value )
{
    target.EnsureRemainingCapacity(28);//   20 digits, grouping symbol, sign and what have you
    integer length=      target.Length();
    length=  detail::WriteDecUnsigned ( value, target.VBuffer(), length, 0, TNumberFormat<TChar>::Computational );
    target.SetLength( length );
}


template<typename TChar>
void T_Append<double,TChar>::operator()( TAString<TChar>& target,  double value )
{
    target.EnsureRemainingCapacity(48);  // float: 2x15 + '.' + ',' + sign + fear
    integer length=      target.Length();
    length=  detail::WriteFloat( value, target.VBuffer(), length, 0, TNumberFormat<TChar>::Computational );
    target.SetLength( length );
}

template void T_Append<bool     , nchar>::operator()( TAString<nchar>&, bool      );
template void T_Append<bool     , wchar>::operator()( TAString<wchar>&, bool      );
template void T_Append<bool     , xchar>::operator()( TAString<xchar>&, bool      );
template void T_Append<  int64_t, nchar>::operator()( TAString<nchar>&,   int64_t );
template void T_Append<  int64_t, wchar>::operator()( TAString<wchar>&,   int64_t );
template void T_Append<  int64_t, xchar>::operator()( TAString<xchar>&,   int64_t );
template void T_Append< uint64_t, nchar>::operator()( TAString<nchar>&,  uint64_t );
template void T_Append< uint64_t, wchar>::operator()( TAString<wchar>&,  uint64_t );
template void T_Append< uint64_t, xchar>::operator()( TAString<xchar>&,  uint64_t );
template void T_Append<float    , nchar>::operator()( TAString<nchar>&, float     );
template void T_Append<float    , wchar>::operator()( TAString<wchar>&, float     );
template void T_Append<float    , xchar>::operator()( TAString<xchar>&, float     );
template void T_Append<double   , nchar>::operator()( TAString<nchar>&, double    );
template void T_Append<double   , wchar>::operator()( TAString<wchar>&, double    );
template void T_Append<double   , xchar>::operator()( TAString<xchar>&, double    );


// #################################################################################################
// std::type_info
// #################################################################################################
#if ALIB_DEBUG
    template<typename TChar>
    void T_Append<std::type_info, TChar>::operator()( TAString<TChar>& target, const std::type_info& type )
    {
        target.template _<false>( DbgTypeDemangler( type ).Get() );
    }
    template void T_Append<std::type_info , nchar>::operator()( TAString<nchar>&, const std::type_info& );
    template void T_Append<std::type_info , wchar>::operator()( TAString<wchar>&, const std::type_info& );
    template void T_Append<std::type_info , xchar>::operator()( TAString<xchar>&, const std::type_info& );
#endif

// #################################################################################################
// TFormat::Tab()
// #################################################################################################
template<typename TChar>
void T_Append<typename TFormat<TChar>::Tab, TChar>::operator()( TAString<TChar>& target, const typename TFormat<TChar>::Tab& tab)
{
    integer reference= tab.reference;
    if (reference < 0 )
    {
        // search backwards
        reference= target.template LastIndexOfAny<Inclusion::Include>( TT_StringConstants<TChar>::NewLine(), target.Length() -1 );
        if ( reference < 0 )
            reference= 0;
        else
        {
            // if new line has more than one character (windows) we have to now search the first
            // character that is not in newline
            reference= target.template IndexOfAny<Inclusion::Exclude, false>( TT_StringConstants<TChar>::NewLine(), reference );
            if (reference < 0 )
                reference= target.Length();

        }
    }
    integer length=   target.Length();
    integer qtyChars= tab.minPad > 0 ? tab.minPad : 0;

    if ( tab.tabSize > 1 )
        qtyChars+= (tab.tabSize - ( (length + qtyChars - reference) % tab.tabSize ) ) % tab.tabSize;

    if ( qtyChars > 0 )
        target.template InsertChars<false>( tab.tabChar, qtyChars );
}


// #################################################################################################
// TFormat::Field()
// #################################################################################################
template<typename TChar>
void T_Append<typename TFormat<TChar>::Field, TChar>::operator()( TAString<TChar>& target, const typename TFormat<TChar>::Field& field)
{

#if ALIB_BOXING
    TString<TChar> theContent;

    // buffer used for conversion (if none string)
    TLocalString<TChar, 256> noneStringArgBuf;
    noneStringArgBuf.DbgDisableBufferReplacementWarning();

    // string type box given?
    if( !field.theContent.template IsType<void>() && field.theContent.template IsType<TString<TChar>>() )
        theContent= field.theContent.template Unbox<TString<TChar>>();
    else
    {
        // write box into local buffer
        noneStringArgBuf << field.theContent;
        theContent= noneStringArgBuf;
    }

#else
    TString<TChar> theContent= field.theContent;
#endif

    integer padSize=  field.fieldWidth
                    - theContent.WStringLength();

    // check pad field.width
    if (padSize <= 0 || field.alignment == Alignment::Left )
    {
                                target.template _          <false>( theContent );
        if (padSize > 0 )       target.template InsertChars<false>( field.padChar, padSize );
        return;
    }

    // align Right
    if ( field.alignment == Alignment::Right )
    {
        if( padSize > 0 )
            target.template InsertChars<false>( field.padChar, padSize );
        target.template Append<false>( theContent );
        return;
    }

    // align Center
    integer leftPadding= padSize / 2;
    if( leftPadding > 0 )
        target.template InsertChars<false> ( field.padChar, leftPadding  );
    target.template Append<false> ( theContent );
    if( padSize > leftPadding ) target.template InsertChars<false> ( field.padChar, padSize - leftPadding );
}

// #################################################################################################
// TFormat::Escape()
// #################################################################################################
template<typename TChar>
void T_Append<typename TFormat<TChar>::Escape, TChar>::operator()( TAString<TChar>& target, const typename TFormat<TChar>::Escape& escape)
{
    if( target.AdjustRegion( const_cast<typename TFormat<TChar>::Escape&>(escape).startIdx,
                             const_cast<typename TFormat<TChar>::Escape&>(escape).length   ) )
        return;

    integer regionEnd= escape.startIdx + escape.length;

    //
    // To escape sequences
    //
    if (escape.pSwitch == Switch::On)
    {
        for( integer idx= escape.startIdx; idx < regionEnd ; ++idx )
        {
            TChar c= target.CharAt(idx);

            TChar resultChar= '\0';
            switch(c)
            {
                case '\\' : resultChar= '\\'; break;
                case '\r' : resultChar= 'r' ; break;
                case '\n' : resultChar= 'n' ; break;
                case '\t' : resultChar= 't' ; break;
                case '\a' : resultChar= 'a' ; break;
                case '\b' : resultChar= 'b' ; break;
                case '\v' : resultChar= 'v' ; break;
                case '\f' : resultChar= 'f' ; break;
             // case '\e' : resultChar= 'e' ; break; Not C++ standard
                case '"'  : resultChar= '"' ; break;

                default   :                   break;
            }

            if( resultChar != '\0')
            {
                target.template InsertChars<false>('\\', 1, idx);
                target[++idx]= resultChar;
                ++regionEnd;
            }
        }
    }

    //
    // Un-escape escape sequences
    //
    else
    {
        --regionEnd; // we can go 1 over it!
        for( integer idx= escape.startIdx; idx < regionEnd ; ++idx )
        {
            TChar c= target.CharAt(idx);
            if( c != '\\' )
                continue;

            c= target.CharAt(idx + 1);

            TChar resultChar= '\0';
            switch(c)
            {
                case '\\' : resultChar= '\\'; break;
                case 'r'  : resultChar= '\r' ; break;
                case 'n'  : resultChar= '\n' ; break;
                case 't'  : resultChar= '\t' ; break;
                case 'a'  : resultChar= '\a' ; break;
                case 'b'  : resultChar= '\b' ; break;
                case 'v'  : resultChar= '\v' ; break;
                case 'f'  : resultChar= '\f' ; break;
             // case 'e'  : resultChar= '\e' ; break; Not C++ standard
                case '"'  : resultChar= '"' ; break;

                default   :                   break;
            }

            if( resultChar != '\0')
            {
                target.Delete( idx, 1);
                target[idx]= resultChar;
                --regionEnd;
            }
        }
    }
}

// #################################################################################################
// TFormat Integers
// #################################################################################################
template<typename TChar>
void T_Append<TFormat<TChar>, TChar>::operator()( TAString<TChar>& target, const TFormat<TChar>& fmt )
{
    const TNumberFormat<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &TNumberFormat<TChar>::Computational;

    target.EnsureRemainingCapacity( fmt.valueType== 3 ?  48  //float: 2x15 + '.' + ',' + sign + fear
                                                      :  28  //int:   20 digits, grouping symbol, sign and what have you
                                  );

    integer length=      target.Length();

    length=
    fmt.valueType == 1 ?  detail::WriteDecSigned  (                       fmt.v.value  , target.VBuffer(), length, fmt.width   , *nf  ) :
    fmt.valueType == 2 ?  detail::WriteDecUnsigned( static_cast<uint64_t>(fmt.v.value) , target.VBuffer(), length, fmt.width   , *nf  ) :
                          detail::WriteFloat      (                       fmt.v.fpValue, target.VBuffer(), length, fmt.width   , *nf  );

    target.SetLength( length );
}



template<typename TChar>
void T_Append<typename TFormat<TChar>::Bin, TChar>::operator()( TAString<TChar>& target,  const typename TFormat<TChar>::Bin& fmt )
{
    TNumberFormat<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &TNumberFormat<TChar>::Computational;

    target.EnsureRemainingCapacity( 80 );

    integer length=      target.Length();

    length= detail::WriteBin( fmt.theValue, target.VBuffer(), length,  fmt.theWidth, *nf );

    target.SetLength( length );
}

template<typename TChar>
void T_Append<typename TFormat<TChar>::Hex, TChar>::operator()( TAString<TChar>& target,  const typename TFormat<TChar>::Hex& fmt )
{
    TNumberFormat<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &TNumberFormat<TChar>::Computational;

    target.EnsureRemainingCapacity( 25 );

    integer length=      target.Length();

    length= detail::WriteHex( fmt.theValue, target.VBuffer(), length,  fmt.theWidth, *nf );

    target.SetLength( length );
}

template<typename TChar>
void T_Append<typename TFormat<TChar>::Oct, TChar>::operator()( TAString<TChar>& target,  const typename TFormat<TChar>::Oct& fmt )
{
    TNumberFormat<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &TNumberFormat<TChar>::Computational;

    target.EnsureRemainingCapacity( 30 );

    integer length=      target.Length();

    length= detail::WriteOct( fmt.theValue, target.VBuffer(), length,  fmt.theWidth, *nf );

    target.SetLength( length );
}


#if defined(ALIB_DOX)
} // APPENDABLES documentation fake namespace
#endif

#if !defined(ALIB_DOX)
// #################################################################################################
// NAString instantiations
// #################################################################################################
template void T_Append<TFormat<nchar>        , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>&         );
template void T_Append<TFormat<nchar>::Tab   , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Tab&    );
template void T_Append<TFormat<nchar>::Field , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Field&  );
template void T_Append<TFormat<nchar>::Escape, nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Escape& );
template void T_Append<TFormat<nchar>::Bin   , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Bin&    );
template void T_Append<TFormat<nchar>::Hex   , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Hex&    );
template void T_Append<TFormat<nchar>::Oct   , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Oct&    );


// #################################################################################################
// WAString instantiations
// #################################################################################################
template void T_Append<TFormat<wchar>        , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>&         );
template void T_Append<TFormat<wchar>::Tab   , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Tab&    );
template void T_Append<TFormat<wchar>::Field , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Field&  );
template void T_Append<TFormat<wchar>::Escape, wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Escape& );
template void T_Append<TFormat<wchar>::Bin   , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Bin&    );
template void T_Append<TFormat<wchar>::Hex   , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Hex&    );
template void T_Append<TFormat<wchar>::Oct   , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Oct&    );

// #################################################################################################
// XAString instantiations
// #################################################################################################
template void T_Append<TFormat<xchar>        , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>&         );
template void T_Append<TFormat<xchar>::Tab   , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Tab&    );
template void T_Append<TFormat<xchar>::Field , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Field&  );
template void T_Append<TFormat<xchar>::Escape, xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Escape& );
template void T_Append<TFormat<xchar>::Bin   , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Bin&    );
template void T_Append<TFormat<xchar>::Hex   , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Hex&    );
template void T_Append<TFormat<xchar>::Oct   , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Oct&    );

#endif // !defined(ALIB_DOX)


}}}// namespace [aworx::lib::strings]
