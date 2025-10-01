//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace strings { namespace util  {

//==================================================================================================
/// Implements "Knuth-Morris-Pratt" algorithm for searching a substring within a string.
///
/// While the well known "Boyer-Moore-Algorithm" is even faster in the average case, for
/// uni-code characters its implementation would be efficient only with very long haystack strings.
///
/// For convenience, the following alias type names are available:
/// - \ref alib::StringSearch,
/// - \ref alib::NStringSearch and
/// - \ref alib::WSubstringSearch.
///
/// @tparam TChar        The character type of the haystack and needle strings.
/// @tparam TSensitivity The letter case sensitivity of the search..
//==================================================================================================
template<typename TChar, lang::Case TSensitivity= lang::Case::Sensitive>
class TStringSearch
{
    protected:
        /// The needle set Knuth-Morris-Pratt prefix length table.
        TAString<TChar, lang::HeapAllocator>  needle;

        /// The Knuth-Morris-Pratt prefix length table.
        integer*                    kmpTable                                              = nullptr;

        /// Length of #kmpTable.
        integer                     kmpTableLength                                              = 0;

    public:
        //==========================================================================================
        /// Constructor. Passes the optional parameters to method #Compile.
        ///
        /// @param pNeedle The string to search.
        ///                Defaults to \b NULL_STRING to allow parameterless construction with later
        ///                invocation of #Compile.
        //==========================================================================================
        TStringSearch( const TString<TChar>& pNeedle= nullptr )
        {
            Compile( pNeedle );
        }

        //==========================================================================================
        /// Destructor.
        //==========================================================================================
        ALIB_DLL
        ~TStringSearch();

    public:
        //==========================================================================================
        /// Resets this object to use the given string as the needle to search.
        /// @param  needle   The needle to search.
        //==========================================================================================
        ALIB_DLL
        void Compile( const TString<TChar>& needle );

        //==========================================================================================
        /// Searches for the needle in \p{haystack} starting at \p{startIdx}.
        ///
        /// @param haystack  The string to search in.
        /// @param startIdx  The start of the search.
        ///                  Defaults to \c 0.
        /// @return The index of the next occurrence of the needle in given \p{haystack}.
        ///         \c -1 if not found.
        //==========================================================================================
        ALIB_DLL
        integer   Search( const TString<TChar>& haystack, integer startIdx= 0 );

}; // class TStringSearch

extern template ALIB_DLL         TStringSearch<nchar, lang::Case::Sensitive>::TStringSearch (const TString<nchar>&);
extern template ALIB_DLL         TStringSearch<nchar, lang::Case::Ignore   >::TStringSearch (const TString<nchar>&);
extern template ALIB_DLL         TStringSearch<wchar, lang::Case::Sensitive>::TStringSearch (const TString<wchar>&);
extern template ALIB_DLL         TStringSearch<wchar, lang::Case::Ignore   >::TStringSearch (const TString<wchar>&);
extern template ALIB_DLL         TStringSearch<nchar, lang::Case::Sensitive>::~TStringSearch();
extern template ALIB_DLL         TStringSearch<nchar, lang::Case::Ignore   >::~TStringSearch();
extern template ALIB_DLL         TStringSearch<wchar, lang::Case::Sensitive>::~TStringSearch();
extern template ALIB_DLL         TStringSearch<wchar, lang::Case::Ignore   >::~TStringSearch();
extern template ALIB_DLL void    TStringSearch<nchar, lang::Case::Sensitive>::Compile             (const TString<nchar>&);
extern template ALIB_DLL void    TStringSearch<nchar, lang::Case::Ignore   >::Compile             (const TString<nchar>&);
extern template ALIB_DLL void    TStringSearch<wchar, lang::Case::Sensitive>::Compile             (const TString<wchar>&);
extern template ALIB_DLL void    TStringSearch<wchar, lang::Case::Ignore   >::Compile             (const TString<wchar>&);
extern template ALIB_DLL integer TStringSearch<nchar, lang::Case::Sensitive>::Search              (const TString<nchar>&, integer);
extern template ALIB_DLL integer TStringSearch<nchar, lang::Case::Ignore   >::Search              (const TString<nchar>&, integer);
extern template ALIB_DLL integer TStringSearch<wchar, lang::Case::Sensitive>::Search              (const TString<wchar>&, integer);
extern template ALIB_DLL integer TStringSearch<wchar, lang::Case::Ignore   >::Search              (const TString<wchar>&, integer);

}} // namespace alib[::strings::util]

/// Type alias in namespace \b alib.
template<lang::Case TSensitivity>
using   StringSearch =  strings::util::TStringSearch<character>;


/// Type alias in namespace \b alib.
template<lang::Case TSensitivity>
using  NStringSearch =  strings::util::TStringSearch<nchar>;

/// Type alias in namespace \b alib.
template<lang::Case TSensitivity>
using  WSubstringSearch =  strings::util::TStringSearch<wchar>;


} // namespace alib

