// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/strings/util/subsearch.hpp"
#   include "alib/strings/astring.hpp"
#endif // !DOXYGEN


namespace alib {  namespace strings { namespace util  {


template<typename TChar, lang::Case TSensitivity>
TSubstringSearch<TChar,TSensitivity>::~TSubstringSearch()
{
    if( kmpTable )
        delete[] kmpTable;
}

template<typename TChar, lang::Case TSensitivity>
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

        kmpTable= new integer[size_t( needle.Length() + 1 )];
        kmpTableLength= needle.Length();
    }
    integer needleIdx= 0;
    integer pfxLen= kmpTable[0]= -1;
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    for (;;)
    {
        while ((pfxLen != -1) && !characters::Equal<TChar,TSensitivity>(needle.Buffer()[needleIdx], needle.Buffer()[pfxLen]))
            pfxLen= kmpTable[pfxLen];
        ++needleIdx;
        if (needleIdx == needle.Length())
            break;

        ++pfxLen;
        kmpTable[needleIdx]=     characters::Equal<TChar,TSensitivity>(needle.Buffer()[needleIdx], needle.Buffer()[pfxLen]) ? kmpTable[pfxLen] : pfxLen;
    }
    kmpTable[needleIdx]=  pfxLen;
    ALIB_WARNINGS_RESTORE
}

template<typename TChar, lang::Case TSensitivity>
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
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        while ((needleIdx != -1) && !characters::Equal<TChar,TSensitivity>( haystack.Buffer()[haystackIdx],
                                                                            needle  .Buffer()[needleIdx] ) )
            needleIdx= kmpTable[needleIdx];
        ALIB_WARNINGS_RESTORE
        ++haystackIdx;
        ++needleIdx;
        if ( needleIdx >= needle.Length() )
            return (haystackIdx - needleIdx);
    }

  return -1;
}

//! @cond NO_DOX
template         TSubstringSearch<nchar, lang::Case::Sensitive>:: TSubstringSearch(const TString<nchar>&);
template         TSubstringSearch<nchar, lang::Case::Ignore   >:: TSubstringSearch(const TString<nchar>&);
template         TSubstringSearch<wchar, lang::Case::Sensitive>:: TSubstringSearch(const TString<wchar>&);
template         TSubstringSearch<wchar, lang::Case::Ignore   >:: TSubstringSearch(const TString<wchar>&);
template         TSubstringSearch<nchar, lang::Case::Sensitive>::~TSubstringSearch();
template         TSubstringSearch<nchar, lang::Case::Ignore   >::~TSubstringSearch();
template         TSubstringSearch<wchar, lang::Case::Sensitive>::~TSubstringSearch();
template         TSubstringSearch<wchar, lang::Case::Ignore   >::~TSubstringSearch();
template void    TSubstringSearch<nchar, lang::Case::Sensitive>::Compile          (const TString<nchar>&);
template void    TSubstringSearch<nchar, lang::Case::Ignore   >::Compile          (const TString<nchar>&);
template void    TSubstringSearch<wchar, lang::Case::Sensitive>::Compile          (const TString<wchar>&);
template void    TSubstringSearch<wchar, lang::Case::Ignore   >::Compile          (const TString<wchar>&);
template integer TSubstringSearch<nchar, lang::Case::Sensitive>::Search           (const TString<nchar>&, integer);
template integer TSubstringSearch<nchar, lang::Case::Ignore   >::Search           (const TString<nchar>&, integer);
template integer TSubstringSearch<wchar, lang::Case::Sensitive>::Search           (const TString<wchar>&, integer);
template integer TSubstringSearch<wchar, lang::Case::Ignore   >::Search           (const TString<wchar>&, integer);
//! @endcond


}}} // namespace [alib::strings::util]

