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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_COMPATIBILITY_STD_VECTOR)
    #error "Header already included"
#endif

#ifndef HPP_ALIB_COMPATIBILITY_STD_VECTOR
//! @cond NO_DOX
#define HPP_ALIB_COMPATIBILITY_STD_VECTOR 1
//! @endcond
#include <string>

// #################################################################################################
// ALib String / std::string compatibility support
// #################################################################################################
#if ALIB_MODULE_STRINGS

namespace aworx { namespace lib { namespace strings
{
// We are faking template specializations of namespace string for doxygen into sub namespace
// thirdparty::std to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
namespace thirdparty { namespace std {
#endif
    // ####################### String construction from std::vector<char>  #########################

    /** ********************************************************************************************
     * Specialization of struct
     * \ref aworx::lib::strings::T_String "T_String" for type \c std::vector<nchar>.
     **********************************************************************************************/
    template<>   struct      T_String<std::vector<nchar>,nchar>   : public std::true_type
    {
        /** ****************************************************************************************
         * Returns the  result of \c std::vector::data.
         *
         * @param  src    The source string.
         * @return The buffer to the contents of \p{src}.
         ******************************************************************************************/
        static inline
        const nchar* Buffer( const std::vector<nchar>& src )
        {
            return  src.data();
        }

        /** ****************************************************************************************
         * Returns the  result of \c std::vector::size.
         *
         * @param  src    The source string.
         * @return The length of \p{src}.
         ******************************************************************************************/
        static inline
        integer Length( const std::vector<nchar>& src )
        {
            return static_cast<integer>( src.size() );
        }
    };

    /** ********************************************************************************************
     * Specialization of struct
     * \ref aworx::lib::strings::T_String "T_String" for type \c std::vector<wchar>.
     **********************************************************************************************/
    template<>   struct      T_String<std::vector<wchar>,wchar>   : public std::true_type
    {
        /** ****************************************************************************************
         * Returns the  result of \c std::vector::data.
         *
         * @param  src    The source string.
         * @return The buffer to the contents of \p{src}.
         ******************************************************************************************/
        static inline
        const wchar* Buffer( const std::vector<wchar>& src )
        {
            return  src.data();
        }

        /** ****************************************************************************************
         * Returns the  result of \c std::vector::size.
         *
         * @param  src    The source string.
         * @return The length of \p{src}.
         ******************************************************************************************/
        static inline
        integer Length( const std::vector<wchar>& src )
        {
            return static_cast<integer>( src.size() );
        }
    };

    // ################### ApplyTo for vectors of different character types  #######################

    /// Specialization of template struct \ref T_Apply for type \c std::vector<wchar>.
    template<>   struct T_Apply<std::vector<wchar>,nchar>   : public std::true_type
    {

        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source vector of characters.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<char>& target, const std::vector<wchar>& src )
        {
            target.Append( src.data(),  static_cast<integer>( src.size()) );
            return static_cast<integer>( src.size() );
        }
    };

    /// Specialization of template struct \ref T_Apply for type \c std::vector<strangeChar>.
    template<>   struct T_Apply<std::vector<strangeChar>,char>  : public std::true_type
    {
        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source vector of characters.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<char>& target, const std::vector<strangeChar>& src )
        {
            target.Append( src.data(),  static_cast<integer>( src.size()) );
            return static_cast<integer>( src.size() );
        }
    };

    /// Specialization of template struct \ref T_Apply for type \c std::vector<wchar>.
    template<>   struct T_Apply<std::vector<nchar>,wchar>   : public std::true_type
    {

        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source vector of characters.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<wchar>& target, const std::vector<nchar>& src )
        {
            target.Append( src.data(),  static_cast<integer>( src.size()) );
            return static_cast<integer>( src.size() );
        }
    };

    /// Specialization of template struct \ref T_Apply for type \c std::vector<strangeChar>.
    template<>   struct T_Apply<std::vector<strangeChar>,wchar_t>   : public std::true_type
    {
        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source vector of characters.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<wchar>& target, const std::vector<strangeChar>& src )
        {
            target.Append( src.data(),  static_cast<integer>( src.size()) );
            return static_cast<integer>( src.size() );
        }
    };

    // #############################  ToStdString (from std::vector<char>)  ##############################

    /** ********************************************************************************************
     *  Copies or appends a region of a given \alib{strings,StringBase,String} into
     *  a given object of type \c std::vector<char>.
     *  The region is adjusted to the length of the %String.
     *
     * @param src           The \alib{strings,StringBase,String} to convert into a std::vector<char>.
     * @param target        A result \c std::vector<char> to copy the specified region into.
     * @param regionStart   The start index of the region to be copied.
     * @param regionLength  The maximum length of the region to be copied.
     *                      Defaults to \b MaxLen.
     * @param targetData    If \c CurrentData::Keep, the target \c std::vector<char> is not cleared before
     *                      the result is written. Defaults to \c CurrentData::Clear.
     *
     * @return The (modified) \c std::vector<char>& result that was provided (for concatenation of calls).
     **********************************************************************************************/
    inline
    std::vector<char>& ToStdVector( const NString&        src,
                                    std::vector<char>&    target,
                                    integer               regionStart,
                                    integer               regionLength    = strings::MaxLen,
                                    lang::CurrentData     targetData      = lang::CurrentData::Clear )
    {

        if ( targetData == lang::CurrentData::Clear )
            target.clear();

        // adjust range, if empty return empty vector<char>
        if ( src.AdjustRegion( regionStart, regionLength ) )
            return target;

        // copy our buffer into target
        target.reserve( static_cast<size_t>( regionLength ) );
        const char* ps= src.Buffer() + regionStart;
        const char* pe= ps     + regionLength;
        while(ps < pe )
            target.push_back( *ps++ );
        return  target;
    }

    /** ********************************************************************************************
     * Copies a region of a given \alib{strings,StringBase,String} into an
     * object of type \c std::vector<char> which is created.
     * The given region is adjusted to match the length of the %String.
     *
     * @param src            The \alib{strings,StringBase,String} to convert into a std::vector<char>.
     * @param regionStart   The start index of the region in this to create the std::vector<char> from.
     * @param regionLength  The maximum length of the region to be copied.
     *                      Defaults to \b MaxLen.
     *
     * @return A \c std::vector<char> that represents the specified sub region of this object.
     **********************************************************************************************/
    inline
    std::vector<char>  ToStdVector( const NString&  src,
                                    integer         regionStart,
                                    integer         regionLength= strings::MaxLen )
    {
        std::vector<char> result;
        ToStdVector( src, result, regionStart, regionLength, lang::CurrentData::Keep );
        return result;
    }

    /** ********************************************************************************************
     * Copies the contents of a given \alib{strings,StringBase,String} into the given
     * object of type \c std::vector<char>.
     *
     * @param src            The \alib{strings,StringBase,String} to convert into a std::vector<char>.
     * @return A \c std::vector<char> that represents this object.
     **********************************************************************************************/
    inline
    std::vector<char>  ToStdVector( const NString& src )
    {
        return ToStdVector( src, 0, src.Length() );
    }


    /** ********************************************************************************************
     * Copies the contents of a given \alib{strings,StringBase,String} into the
     * given object of type \c std::vector<char>.
     *
     * @param src           The \alib{strings,StringBase,String} to convert into a std::vector<char>.
     * @param target       A target std::vector<char> to copy the contents of this %String into.
     * @param targetData   If \c CurrentData::Keep, parameter \p{target} is not cleared before
     *                     the result is written. Defaults to \c CurrentData::Clear.
     * @return The (modified) \p{target} that was provided (returned to allow concatenation of calls).
     **********************************************************************************************/
    inline
    std::vector<char>& ToStdVector( const NString&      src,
                                    std::vector<char>&  target,
                                    lang::CurrentData   targetData = lang::CurrentData::Clear )
    {
        return ToStdVector( src, target, 0, src.Length(), targetData );
    }

    /** ********************************************************************************************
     *  Copies or appends a region of a given \alib{strings,StringBase,String} into
     *  a given object of type \c std::vector<char>.
     *  The region is adjusted to the length of the %String.
     *
     * @param src           The \alib{strings,StringBase,String} to convert into a std::vector<char>.
     * @param target        A result \c std::vector<char> to copy the specified region into.
     * @param regionStart   The start index of the region to be copied.
     * @param regionLength  The maximum length of the region to be copied.
     *                      Defaults to \b MaxLen.
     * @param targetData    If \c CurrentData::Keep, the target \c std::vector<char> is not cleared before
     *                      the result is written. Defaults to \c CurrentData::Clear.
     *
     * @return The (modified) \c std::vector<char>& result that was provided (for concatenation of calls).
     **********************************************************************************************/
    inline
    std::vector<wchar_t>& ToStdVector( const WString&          src,
                                       std::vector<wchar_t>&   target,
                                       integer                 regionStart,
                                       integer                 regionLength = strings::MaxLen,
                                       lang::CurrentData       targetData   = lang::CurrentData::Clear)
    {

        if ( targetData == lang::CurrentData::Clear )
            target.clear();

        // adjust range, if empty return empty vector<char>
        if ( src.AdjustRegion( regionStart, regionLength ) )
            return target;

        // copy our buffer into target
        target.reserve( static_cast<size_t>( regionLength ) );
        const wchar_t* ps= src.Buffer() + regionStart;
        const wchar_t* pe= ps     + regionLength;
        while(ps < pe )
            target.push_back( *ps++ );
        return  target;
    }

    /** ********************************************************************************************
     * Copies a region of a given \alib{strings,StringBase,String} into an
     * object of type \c std::vector<char> which is created.
     * The given region is adjusted to match the length of the %String.
     *
     * @param src            The \alib{strings,StringBase,String} to convert into a std::vector<char>.
     * @param regionStart   The start index of the region in this to create the std::vector<char> from.
     * @param regionLength  The maximum length of the region to be copied.
     *                      Defaults to \b MaxLen.
     *
     * @return A \c std::vector<char> that represents the specified sub region of this object.
     **********************************************************************************************/
    inline
    std::vector<wchar_t>  ToStdVector( const WString&  src,
                                       integer         regionStart,
                                       integer         regionLength= strings::MaxLen )
    {
        std::vector<wchar_t> result;
        ToStdVector( src, result, regionStart, regionLength, lang::CurrentData::Keep );
        return result;
    }

    /** ********************************************************************************************
     * Copies the contents of a given \alib{strings,StringBase,String} into the given
     * object of type \c std::vector<char>.
     *
     * @param src            The \alib{strings,StringBase,String} to convert into a std::vector<char>.
     * @return A \c std::vector<char> that represents this object.
     **********************************************************************************************/
    inline
    std::vector<wchar_t>  ToStdVector( const WString& src )
    {
        return ToStdVector( src, 0, src.Length() );
    }


    /** ********************************************************************************************
     * Copies the contents of a given \alib{strings,StringBase,String} into the
     * given object of type \c std::vector<char>.
     *
     * @param src           The \alib{strings,StringBase,String} to convert into a std::vector<char>.
     * @param target       A target std::vector<char> to copy the contents of this %String into.
     * @param targetData   If \c CurrentData::Keep, parameter \p{target} is not cleared before
     *                     the result is written. Defaults to \c CurrentData::Clear.
     * @return The (modified) \p{target} that was provided (returned to allow concatenation of calls).
     **********************************************************************************************/
    inline
    std::vector<wchar_t>& ToStdVector( const WString&           src,
                                       std::vector<wchar_t>&    target,
                                       lang::CurrentData        targetData = lang::CurrentData::Clear )
    {
        return ToStdVector( src, target, 0, src.Length(), targetData );
    }

// We are faking template specializations of namespace string for doxygen into sub namespace
// thirdparty::std to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
}}
#endif


}}}

#endif // ALIB_MODULE_STRINGS


#endif // HPP_ALIB_COMPATIBILITY_STD_VECTOR
