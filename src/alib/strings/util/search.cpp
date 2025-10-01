// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/strings/strings.prepro.hpp"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Strings.Search;
    import   ALib.Characters.Functions;
#else
#   include "ALib.Characters.Functions.H"
#   include "ALib.Strings.Search.H"
#endif
// ======================================   Implementation   =======================================
namespace alib {  namespace strings { namespace util  {

template<typename TChar, lang::Case TSensitivity>
TStringSearch<TChar,TSensitivity>::~TStringSearch()      { if( kmpTable ) delete[] kmpTable; }

template<typename TChar, lang::Case TSensitivity>
void TStringSearch<TChar,TSensitivity>::Compile( const TString<TChar>& pNeedle )
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
}

template<typename TChar, lang::Case TSensitivity>
integer TStringSearch<TChar,TSensitivity>::Search( const TString<TChar>& haystack, integer haystackIdx )
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
        while ((needleIdx != -1) && !characters::Equal<TChar,TSensitivity>( haystack.Buffer()[haystackIdx],
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
template         TStringSearch<nchar, lang::Case::Sensitive>:: TStringSearch(const TString<nchar>&);
template         TStringSearch<nchar, lang::Case::Ignore   >:: TStringSearch(const TString<nchar>&);
template         TStringSearch<wchar, lang::Case::Sensitive>:: TStringSearch(const TString<wchar>&);
template         TStringSearch<wchar, lang::Case::Ignore   >:: TStringSearch(const TString<wchar>&);
template         TStringSearch<nchar, lang::Case::Sensitive>::~TStringSearch();
template         TStringSearch<nchar, lang::Case::Ignore   >::~TStringSearch();
template         TStringSearch<wchar, lang::Case::Sensitive>::~TStringSearch();
template         TStringSearch<wchar, lang::Case::Ignore   >::~TStringSearch();
template void    TStringSearch<nchar, lang::Case::Sensitive>::Compile          (const TString<nchar>&);
template void    TStringSearch<nchar, lang::Case::Ignore   >::Compile          (const TString<nchar>&);
template void    TStringSearch<wchar, lang::Case::Sensitive>::Compile          (const TString<wchar>&);
template void    TStringSearch<wchar, lang::Case::Ignore   >::Compile          (const TString<wchar>&);
template integer TStringSearch<nchar, lang::Case::Sensitive>::Search           (const TString<nchar>&, integer);
template integer TStringSearch<nchar, lang::Case::Ignore   >::Search           (const TString<nchar>&, integer);
template integer TStringSearch<wchar, lang::Case::Sensitive>::Search           (const TString<wchar>&, integer);
template integer TStringSearch<wchar, lang::Case::Ignore   >::Search           (const TString<wchar>&, integer);
//! @endcond


}}} // namespace [alib::strings::util]

