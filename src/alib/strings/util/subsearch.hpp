/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_strings of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_STRINGS_UTIL_SUBSEARCH
#define HPP_ALIB_STRINGS_UTIL_SUBSEARCH 1

#if !defined (HPP_ALIB_STRINGS_ASTRING)
#   include "alib/strings/astring.hpp"
#endif


namespace alib {  namespace strings { namespace util  {

/** ************************************************************************************************
 * Implements "Knuth-Morris-Pratt" algorithm for searching a sub-string within a string.
 *
 * While the well known "Boyer-Moore-Algorithm" is even faster in the average case, for
 * uni-code characters its implementation would be efficient only with very long haystack strings.
 *
 * For convenience, the following alias type names are available:
 * - \ref alib::SubstringSearch,
 * - \ref alib::NSubstringSearch and
 * - \ref alib::WSubstringSearch.
 *
 * @tparam TChar        The character type of the haystack and needle strings.
 * @tparam TSensitivity The letter case sensitivity of the search..
 **************************************************************************************************/
template<typename TChar, lang::Case TSensitivity= lang::Case::Sensitive>
class TSubstringSearch
{
    protected:
        /** The needle set Knuth-Morris-Pratt prefix length table. */
        TAString<TChar>  needle;

        /** The Knuth-Morris-Pratt prefix length table. */
        integer*            kmpTable                                                      = nullptr;

        /** Length of #kmpTable. */
        integer             kmpTableLength                                                      = 0;

    public:
        /** ****************************************************************************************
         * Constructor. Passes the optional parameters to method #Compile.
         *
         * @param pNeedle The string to search.
         *                Defaults to \b NullString() to allow parameterless construction with later
         *                invocation of #Compile.
         ******************************************************************************************/
        TSubstringSearch( const TString<TChar>& pNeedle= nullptr )
        {
            Compile( pNeedle );
        }

        /** ****************************************************************************************
         * Destructor.
         ******************************************************************************************/
        ALIB_API
        ~TSubstringSearch();

    public:
        /** ****************************************************************************************
         * Resets this object to use the given string as the needle to search.
         * @param  needle   The needle to search.
         ******************************************************************************************/
        ALIB_API
        void Compile( const TString<TChar>& needle );

        /** ****************************************************************************************
         * Searches for the needle in \p{haystack} starting at \p{startIdx}.
         *
         * @param haystack  The string to search in.
         * @param startIdx  The start of the search.
         *                  Defaults to \c 0.
         * @return The index of the next occurrence of the needle in given \p{haystack}.
         *         \c -1 if not found.
         ******************************************************************************************/
        ALIB_API
        integer   Search( const TString<TChar>& haystack, integer startIdx= 0 );

}; // class TSubstringSearch

extern template ALIB_API         TSubstringSearch<nchar, lang::Case::Sensitive>::TSubstringSearch (const TString<nchar>&);
extern template ALIB_API         TSubstringSearch<nchar, lang::Case::Ignore   >::TSubstringSearch (const TString<nchar>&);
extern template ALIB_API         TSubstringSearch<wchar, lang::Case::Sensitive>::TSubstringSearch (const TString<wchar>&);
extern template ALIB_API         TSubstringSearch<wchar, lang::Case::Ignore   >::TSubstringSearch (const TString<wchar>&);
extern template ALIB_API         TSubstringSearch<nchar, lang::Case::Sensitive>::~TSubstringSearch();
extern template ALIB_API         TSubstringSearch<nchar, lang::Case::Ignore   >::~TSubstringSearch();
extern template ALIB_API         TSubstringSearch<wchar, lang::Case::Sensitive>::~TSubstringSearch();
extern template ALIB_API         TSubstringSearch<wchar, lang::Case::Ignore   >::~TSubstringSearch();
extern template ALIB_API void    TSubstringSearch<nchar, lang::Case::Sensitive>::Compile             (const TString<nchar>&);
extern template ALIB_API void    TSubstringSearch<nchar, lang::Case::Ignore   >::Compile             (const TString<nchar>&);
extern template ALIB_API void    TSubstringSearch<wchar, lang::Case::Sensitive>::Compile             (const TString<wchar>&);
extern template ALIB_API void    TSubstringSearch<wchar, lang::Case::Ignore   >::Compile             (const TString<wchar>&);
extern template ALIB_API integer TSubstringSearch<nchar, lang::Case::Sensitive>::Search              (const TString<nchar>&, integer);
extern template ALIB_API integer TSubstringSearch<nchar, lang::Case::Ignore   >::Search              (const TString<nchar>&, integer);
extern template ALIB_API integer TSubstringSearch<wchar, lang::Case::Sensitive>::Search              (const TString<wchar>&, integer);
extern template ALIB_API integer TSubstringSearch<wchar, lang::Case::Ignore   >::Search              (const TString<wchar>&, integer);

}} // namespace alib[::strings::util]


/// Type alias in namespace \b alib.
template<lang::Case TSensitivity>
using  NSubstringSearch =  strings::util::TSubstringSearch<nchar>;

/// Type alias in namespace \b alib.
template<lang::Case TSensitivity>
using  WSubstringSearch =  strings::util::TSubstringSearch<wchar>;

/// Type alias in namespace \b alib.
template<lang::Case TSensitivity>
using   SubstringSearch =  strings::util::TSubstringSearch<character>;


} // namespace alib


#endif // HPP_ALIB_STRINGS_UTIL_SUBSEARCH
