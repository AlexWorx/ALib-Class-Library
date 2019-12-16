// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_STRINGS_UTIL_SUBSEARCH)
#   include "alib/strings/util/subsearch.hpp"
#endif
#endif // !defined(ALIB_DOX)


namespace aworx { namespace lib { namespace strings { namespace util  {


template<typename TChar, Case TSensitivity>
TSubstringSearch<TChar,TSensitivity>::~TSubstringSearch()
{
    if( kmpTable )
        delete[] kmpTable;
}

template<typename TChar, Case TSensitivity>
void TSubstringSearch<TChar,TSensitivity>::Compile( const TString<TChar>& pNeedle )
{
    if (pNeedle.IsNull() )
    {
        needle.SetNull();
        return;
    }
    needle.Reset( pNeedle );
    if( needle.Length() == 0 )
        return;

    if( kmpTableLength < needle.Length() )
    {
        if( kmpTable )
            delete[] kmpTable;

        kmpTable= new integer[static_cast<size_t>( needle.Length() + 1 )];
        kmpTableLength= needle.Length();
    }
    integer needleIdx= 0;
    integer pfxLen= kmpTable[0]= -1;
    while (needleIdx != needle.Length())
    {
        while ((pfxLen != -1) && !characters::CharArray<TChar>::template Equal<TSensitivity>(needle.Buffer()[needleIdx], needle.Buffer()[pfxLen]))
	        pfxLen= kmpTable[pfxLen];
        ++needleIdx;
        ++pfxLen;
        kmpTable[needleIdx]=     characters::CharArray<TChar>::template Equal<TSensitivity>(needle.Buffer()[needleIdx], needle.Buffer()[pfxLen]) ? kmpTable[pfxLen] : pfxLen;
    }
}

template<typename TChar, Case TSensitivity>
integer TSubstringSearch<TChar,TSensitivity>::Search( const TString<TChar>& haystack, integer haystackIdx )
{
    if ( needle.IsNull()  )
        return  -1;
    if ( haystackIdx < 0                                   )    haystackIdx= 0;
    if ( haystackIdx + needle.Length() > haystack.Length() )    return -1;

    if( needle.Length() == 0 )
        return haystackIdx;

    integer needleIdx     = 0;
    while (haystackIdx != haystack.Length())
    {
        while ((needleIdx != -1) && !characters::CharArray<TChar>::template Equal<TSensitivity>(haystack.Buffer()[haystackIdx],
                                                                                                 needle  .Buffer()[needleIdx] ) )
	        needleIdx= kmpTable[needleIdx];
        ++haystackIdx;
        ++needleIdx;
        if ( needleIdx >= needle.Length() )
	        return (haystackIdx - needleIdx);
    }

  return -1;
}

//! @cond NO_DOX
template         TSubstringSearch<nchar, Case::Sensitive>::TSubstringSearch(const TString<nchar>&);
template         TSubstringSearch<nchar, Case::Ignore   >::TSubstringSearch(const TString<nchar>&);
template         TSubstringSearch<wchar, Case::Sensitive>::TSubstringSearch(const TString<wchar>&);
template         TSubstringSearch<wchar, Case::Ignore   >::TSubstringSearch(const TString<wchar>&);
template         TSubstringSearch<nchar, Case::Sensitive>::~TSubstringSearch();
template         TSubstringSearch<nchar, Case::Ignore   >::~TSubstringSearch();
template         TSubstringSearch<wchar, Case::Sensitive>::~TSubstringSearch();
template         TSubstringSearch<wchar, Case::Ignore   >::~TSubstringSearch();
template void    TSubstringSearch<nchar, Case::Sensitive>::Compile            (const TString<nchar>&);
template void    TSubstringSearch<nchar, Case::Ignore   >::Compile            (const TString<nchar>&);
template void    TSubstringSearch<wchar, Case::Sensitive>::Compile            (const TString<wchar>&);
template void    TSubstringSearch<wchar, Case::Ignore   >::Compile            (const TString<wchar>&);
template integer TSubstringSearch<nchar, Case::Sensitive>::Search             (const TString<nchar>&, integer);
template integer TSubstringSearch<nchar, Case::Ignore   >::Search             (const TString<nchar>&, integer);
template integer TSubstringSearch<wchar, Case::Sensitive>::Search             (const TString<wchar>&, integer);
template integer TSubstringSearch<wchar, Case::Ignore   >::Search             (const TString<wchar>&, integer);
//! @endcond


}}}} // namespace [aworx::lib::strings::util]


