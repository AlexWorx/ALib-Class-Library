// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
#error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_STRINGS_UTIL_SUBSEARCH)
#error "Header already included"
#endif


// then, set include guard
#ifndef HPP_ALIB_STRINGS_UTIL_SUBSEARCH
//! @cond NO_DOX
#define HPP_ALIB_STRINGS_UTIL_SUBSEARCH 1
//! @endcond


namespace aworx { namespace lib { namespace strings { namespace util  {

/** ************************************************************************************************
 * Implements "Knuth-Morris-Pratt" algorithm for searching a substring within a string.
 *
 * While the well known "Boyer-Moore-Algorithm" is even faster in the average case, for
 * uni-code characters its implementation would be efficient only with very long haystack strings.
 *
 * For convenience, the following alias type names are available:
 * - ref aworx::SubstringSearch,
 * - ref aworx::NSubstringSearch and
 * - ref aworx::WSubstringSearch.
 *
 * @tparam TChar     The character type of the haystack and needle strings.
 * @tparam TSensitivity The letter case sensitivity of the search..
 **************************************************************************************************/
template<typename TChar, lang::Case TSensitivity= lang::Case::Sensitive>
class SubstringSearchBase
{
    protected:
        /// The needle set Knuth-Morris-Pratt prefix length table.
        AStringBase<TChar>  needle;

        /// The Knuth-Morris-Pratt prefix length table.
        integer*            kmpTable                                                      = nullptr;

        /// Length of #kmpTable.
        integer             kmpTableLength                                                      = 0;

    public:
        /** ****************************************************************************************
         * Constructor. Passes the optional parameters to method #Compile.
         *
         * @param pNeedle The string to search.
         *                Defaults to \b NullString to allow parameterless construction with later
         *                invocation of #Compile.
         ******************************************************************************************/
        inline
        SubstringSearchBase( const StringBase<TChar>& pNeedle= nullptr )
        {
            Compile( pNeedle );
        }

        /** ****************************************************************************************
         * Destructor.
         ******************************************************************************************/
        ALIB_API
        ~SubstringSearchBase();

    public:
        /** ****************************************************************************************
         * Resets this object to use the given string as the needle to search.
         * @param  needle   The needle to search.
         ******************************************************************************************/
        ALIB_API
        void Compile( const StringBase<TChar>& needle );

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
        integer   Search( const StringBase<TChar>& haystack, integer startIdx= 0 );

}; // class SubstringSearchBase

extern template ALIB_API         SubstringSearchBase<nchar, Case::Sensitive>::SubstringSearchBase (const StringBase<nchar>&);
extern template ALIB_API         SubstringSearchBase<nchar, Case::Ignore   >::SubstringSearchBase (const StringBase<nchar>&);
extern template ALIB_API         SubstringSearchBase<wchar, Case::Sensitive>::SubstringSearchBase (const StringBase<wchar>&);
extern template ALIB_API         SubstringSearchBase<wchar, Case::Ignore   >::SubstringSearchBase (const StringBase<wchar>&);
extern template ALIB_API         SubstringSearchBase<nchar, Case::Sensitive>::~SubstringSearchBase();
extern template ALIB_API         SubstringSearchBase<nchar, Case::Ignore   >::~SubstringSearchBase();
extern template ALIB_API         SubstringSearchBase<wchar, Case::Sensitive>::~SubstringSearchBase();
extern template ALIB_API         SubstringSearchBase<wchar, Case::Ignore   >::~SubstringSearchBase();
extern template ALIB_API void    SubstringSearchBase<nchar, Case::Sensitive>::Compile             (const StringBase<nchar>&);
extern template ALIB_API void    SubstringSearchBase<nchar, Case::Ignore   >::Compile             (const StringBase<nchar>&);
extern template ALIB_API void    SubstringSearchBase<wchar, Case::Sensitive>::Compile             (const StringBase<wchar>&);
extern template ALIB_API void    SubstringSearchBase<wchar, Case::Ignore   >::Compile             (const StringBase<wchar>&);
extern template ALIB_API integer SubstringSearchBase<nchar, Case::Sensitive>::Search              (const StringBase<nchar>&, integer);
extern template ALIB_API integer SubstringSearchBase<nchar, Case::Ignore   >::Search              (const StringBase<nchar>&, integer);
extern template ALIB_API integer SubstringSearchBase<wchar, Case::Sensitive>::Search              (const StringBase<wchar>&, integer);
extern template ALIB_API integer SubstringSearchBase<wchar, Case::Ignore   >::Search              (const StringBase<wchar>&, integer);

}}} // namespace aworx[::lib::strings::util]


/// Type alias in namespace #aworx.
template<lib::lang::Case TSensitivity>
using  NSubstringSearch =  aworx::lib::strings::util::SubstringSearchBase<nchar>;

/// Type alias in namespace #aworx.
template<lib::lang::Case TSensitivity>
using  WSubstringSearch =  aworx::lib::strings::util::SubstringSearchBase<wchar>;

/// Type alias in namespace #aworx.
template<lib::lang::Case TSensitivity>
using   SubstringSearch =  aworx::lib::strings::util::SubstringSearchBase<character>;


} // namespace aworx


#endif // HPP_ALIB_STRINGS_UTIL_SUBSEARCH
