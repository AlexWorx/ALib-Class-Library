// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if !defined (HPP_ALIB_STRINGS_UTIL_SUBSEARCH)
    #include "alib/strings/util/subsearch.hpp"
#endif


namespace aworx { namespace lib { namespace strings { namespace util  {


//! @cond NO_DOX
namespace {
template<typename TChar, lang::Case>
                  bool CharsAreEqual( TChar lhs, TChar rhs );
template<> inline bool CharsAreEqual<nchar, lang::Case::Sensitive>( nchar lhs, nchar rhs )   {  return lhs == rhs;  }
template<> inline bool CharsAreEqual<wchar, lang::Case::Sensitive>( wchar lhs, wchar rhs )   {  return lhs == rhs;  }
template<> inline bool CharsAreEqual<nchar, lang::Case::Ignore   >( nchar lhs, nchar rhs )   {  return CString<nchar>::Toupper(lhs) == CString<nchar>::Toupper(rhs);  }
template<> inline bool CharsAreEqual<wchar, lang::Case::Ignore   >( wchar lhs, wchar rhs )   {  return CString<wchar>::Toupper(lhs) == CString<wchar>::Toupper(rhs);  }

}
//! @endcond


template<typename TChar, lang::Case TSensitivity>
SubstringSearchBase<TChar,TSensitivity>::~SubstringSearchBase()
{
    if( kmpTable )
        delete ( kmpTable );
}

template<typename TChar, lang::Case TSensitivity>
void SubstringSearchBase<TChar,TSensitivity>::Compile( const StringBase<TChar>& pNeedle )
{
    if (pNeedle.IsNull() )
    {
        needle.SetNull();
        return;
    }
    needle.Clear() << pNeedle;
    if( needle.Length() == 0 )
        return;

    if( kmpTableLength < needle.Length() )
    {
        if( kmpTable )
            delete kmpTable;

        kmpTable= new integer[static_cast<size_t>( needle.Length() + 1 )];
        kmpTableLength= needle.Length();
    }
    integer needleIdx= 0;
    integer pfxLen= kmpTable[0]= -1;
    while (needleIdx < needle.Length())
    {
        while ((pfxLen > -1) && !CharsAreEqual<TChar,TSensitivity>(needle.Buffer()[needleIdx], needle.Buffer()[pfxLen]))
	        pfxLen= kmpTable[pfxLen];
        needleIdx++;
        pfxLen++;
        kmpTable[needleIdx] = CharsAreEqual<TChar,TSensitivity>(needle.Buffer()[needleIdx], needle.Buffer()[pfxLen]) ? kmpTable[pfxLen] : pfxLen;
    }
}

template<typename TChar, lang::Case TSensitivity>
integer SubstringSearchBase<TChar,TSensitivity>::Search( const StringBase<TChar>& haystack, integer haystackIdx )
{
    if ( needle.IsNull()  )
        return  -1;
    if ( haystackIdx < 0                                   )    haystackIdx= 0;
    if ( haystackIdx + needle.Length() > haystack.Length() )    return -1;

    if( needle.Length() == 0 )
        return haystackIdx;

    integer needleIdx     = 0;
    while (haystackIdx < haystack.Length())
    {
        while ((needleIdx > -1) && !CharsAreEqual<TChar,TSensitivity>(haystack.Buffer()[haystackIdx],
                                                                      needle  .Buffer()[needleIdx]  ) )
	        needleIdx= kmpTable[needleIdx];
        haystackIdx++;
        needleIdx++;
        if ( needleIdx >= needle.Length() )
	        return (haystackIdx - needleIdx);
    }

  return -1;
}

//! @cond NO_DOX
template         SubstringSearchBase<nchar, Case::Sensitive>::SubstringSearchBase(const StringBase<nchar>&);
template         SubstringSearchBase<nchar, Case::Ignore   >::SubstringSearchBase(const StringBase<nchar>&);
template         SubstringSearchBase<wchar, Case::Sensitive>::SubstringSearchBase(const StringBase<wchar>&);
template         SubstringSearchBase<wchar, Case::Ignore   >::SubstringSearchBase(const StringBase<wchar>&);
template         SubstringSearchBase<nchar, Case::Sensitive>::~SubstringSearchBase();
template         SubstringSearchBase<nchar, Case::Ignore   >::~SubstringSearchBase();
template         SubstringSearchBase<wchar, Case::Sensitive>::~SubstringSearchBase();
template         SubstringSearchBase<wchar, Case::Ignore   >::~SubstringSearchBase();
template void    SubstringSearchBase<nchar, Case::Sensitive>::Compile            (const StringBase<nchar>&);
template void    SubstringSearchBase<nchar, Case::Ignore   >::Compile            (const StringBase<nchar>&);
template void    SubstringSearchBase<wchar, Case::Sensitive>::Compile            (const StringBase<wchar>&);
template void    SubstringSearchBase<wchar, Case::Ignore   >::Compile            (const StringBase<wchar>&);
template integer SubstringSearchBase<nchar, Case::Sensitive>::Search             (const StringBase<nchar>&, integer);
template integer SubstringSearchBase<nchar, Case::Ignore   >::Search             (const StringBase<nchar>&, integer);
template integer SubstringSearchBase<wchar, Case::Sensitive>::Search             (const StringBase<wchar>&, integer);
template integer SubstringSearchBase<wchar, Case::Ignore   >::Search             (const StringBase<wchar>&, integer);
//! @endcond


}}}} // namespace [aworx::lib::strings::util]


