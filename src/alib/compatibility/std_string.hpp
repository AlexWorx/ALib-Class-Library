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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_COMPATIBILITY_STD_STRING)
    #error "Header already included"
#endif

#ifndef HPP_ALIB_COMPATIBILITY_STD_STRING
//! @cond NO_DOX
#define HPP_ALIB_COMPATIBILITY_STD_STRING 1
//! @endcond

#if !defined (_GLIBCXX_STRING) && !defined(_STRING_)
#   include <string>
#endif
#if ALIB_CPP17 && !defined (_GLIBCXX_STRING_VIEW) && !defined(_STRING_VIEW_)
#   include <string_view>
#endif

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
    // #########################   String construction from std::string   ##########################

    /// Specialization of struct \alib{strings,T_String} for type \c std::string.
    template<>  struct   T_String<std::string,char> : public std::true_type
    {
        /** ****************************************************************************************
         * Returns the  result of \c std::string::data.
         *
         * @param  src    The source string.
         * @return The buffer to the contents of \p{src}.
         ******************************************************************************************/
        static inline
        const char* Buffer( const std::string& src )
        {
            return  &src[0]; // avoid zero termination possibly performed by data()
        }

        /** ****************************************************************************************
         * Returns the  result of \c std::string::length.
         *
         * @param  src    The source string.
         * @return The length of \p{src}.
         ******************************************************************************************/
        static inline
        integer Length( const std::string& src )
        {
            return static_cast<integer>( src.length() );
        }
    };

    /// Specialization of struct \alib{strings,T_String} for type \c std::wstring.
    template<>  struct   T_String<std::wstring,wchar_t> : public std::true_type
    {
        /** ****************************************************************************************
         * Returns the  result of \c std::wstring::data.
         *
         * @param  src    The source string.
         * @return The buffer to the contents of \p{src}.
         ******************************************************************************************/
        static inline
        const wchar_t* Buffer( const std::wstring& src )
        {
            return  &src[0]; // avoid zero termination possibly performed by data()
        }

        /** ****************************************************************************************
         * Returns the  result of \c std::wstring::length.
         *
         * @param  src    The source string.
         * @return The length of \p{src}.
         ******************************************************************************************/
        static inline
        integer Length( const std::wstring& src )
        {
            return static_cast<integer>( src.length() );
        }
    };

    // ######################   String construction from std::string_view   ########################
#if ALIB_CPP17
    /// Specialization of struct \alib{strings,T_String} for type \c std::string_view.
    template<>  struct   T_String<std::string_view,char> : public std::true_type
    {
        /** ****************************************************************************************
         * Returns the  result of \c std::string_view::data.
         *
         * @param  src    The source string.
         * @return The buffer to the contents of \p{src}.
         ******************************************************************************************/
        static inline
        const char* Buffer( const std::string_view& src )
        {
            return  src.data();
        }

        /** ****************************************************************************************
         * Returns the  result of \c std::string_view::length.
         *
         * @param  src    The source string_view.
         * @return The length of \p{src}.
         ******************************************************************************************/
        static inline
        integer Length( const std::string_view& src )
        {
            return static_cast<integer>( src.length() );
        }
    };

    /// Specialization of struct \alib{strings,T_String} for type \c std::wstring_view.
    template<>  struct   T_String<std::wstring_view,wchar_t> : public std::true_type
    {
        /** ****************************************************************************************
         * Returns the  result of \c std::string_view::data.
         *
         * @param  src    The source string.
         * @return The buffer to the contents of \p{src}.
         ******************************************************************************************/
        static inline
        const wchar_t* Buffer( const std::wstring_view& src )
        {
            return  src.data();
        }

        /** ****************************************************************************************
         * Returns the  result of \c std::string_view::length.
         *
         * @param  src    The source string_view.
         * @return The length of \p{src}.
         ******************************************************************************************/
        static inline
        integer Length( const std::wstring_view& src )
        {
            return static_cast<integer>( src.length() );
        }
    };
#endif

    // ############################ ApplyTo for std::basic_string  #################################

    /// Specialization of template struct \ref T_Apply for type \c std::string.
    template<typename TChar>   struct T_Apply<std::string ,TChar> : public std::true_type
    {

        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const std::string& src )
        {
            // this <true> is needed for the MS compile. Don't know why, keep it in!
            target.template Append<true>( &src[0],  static_cast<integer>( src.length()) );
            return static_cast<integer>( src.length() );
        }
    };


    /// Specialization of template struct \ref T_Apply for type \c std::wstring.
    template<typename TChar>   struct T_Apply<std::wstring ,TChar> : public std::true_type
    {

        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const std::wstring& src )
        {
            // this <true> is needed for the MS compile. Don't know why, keep it in!
            target.template Append<true>( &src[0],  static_cast<integer>( src.length()) );
            return static_cast<integer>( src.length() );
        }
    };

#if ALIB_SIZEOF_WCHAR_T == 4
    /// Specialization of template struct \ref T_Apply for type \c std::u16string.
    template<typename TChar>   struct T_Apply<std::u16string ,TChar> : public std::true_type
    {
        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const std::u16string& src )
        {
            target.Append( &src[0],  static_cast<integer>( src.length()) );
            return static_cast<integer>( src.length() );
        }
    };
#else
    /// Specialization of template struct \ref T_Apply for type \c std::u32string.
    template<typename TChar>   struct T_Apply<std::u32string ,TChar> : public std::true_type
    {
        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const std::u32string& src )
        {
            target.Append( &src[0],  static_cast<integer>( src.length()) );
            return static_cast<integer>( src.length() );
        }
    };
#endif

    // ######################### ApplyTo for std::basic_string_view  ###############################
#if ALIB_CPP17
    /// Specialization of template struct \ref T_Apply for type \c std::string_view.
    template<typename TChar>   struct T_Apply<std::string_view ,TChar> : public std::true_type
    {

        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const std::string_view& src )
        {
            // this <true> is needed for the MS compile. Don't know why, keep it in!
            target.template Append<true>( src.data(),  static_cast<integer>( src.length()) );
            return static_cast<integer>( src.length() );
        }
    };


    /// Specialization of template struct \ref T_Apply for type \c std::wstring_view.
    template<typename TChar>   struct T_Apply<std::wstring_view ,TChar> : public std::true_type
    {

        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const std::wstring_view& src )
        {
            // this <true> is needed for the MS compile. Don't know why, keep it in!
            target.template Append<true>( src.data(),  static_cast<integer>( src.length()) );
            return static_cast<integer>( src.length() );
        }
    };

#if ALIB_SIZEOF_WCHAR_T == 4
    /// Specialization of template struct \ref T_Apply for type \c std::u16string_view.
    template<typename TChar>   struct T_Apply<std::u16string_view ,TChar> : public std::true_type
    {
        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const std::u16string_view& src )
        {
            target.Append( src.data(),  static_cast<integer>( src.length()) );
            return static_cast<integer>( src.length() );
        }
    };
#else
    /// Specialization of template struct \ref T_Apply for type \c std::u32string_view.
    template<typename TChar>   struct T_Apply<std::u32string_view ,TChar> : public std::true_type
    {
        /** ****************************************************************************************
         * Appends contents of \p{src} to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const std::u32string_view& src )
        {
            target.Append( src.data(),  static_cast<integer>( src.length()) );
            return static_cast<integer>( src.length() );
        }
    };
#endif
#endif


    // #############################  ToStdString (from std::string)  ##############################

    /** ********************************************************************************************
     * Copies or appends a region of a given \ref aworx::NString into an object of type
     * \c std::string.
     * The given region is adjusted to match the length of the %String.
     *
     * @param src           The \alib{strings,StringBase,String} to convert into a std::string.
     * @param target        A result std::string to copy the specified region into.
     * @param regionStart   The start index of the region to be copied.
     * @param regionLength  The maximum length of the region to be copied.
     *                      Defaults to \b MaxLen.
     * @param targetData    If \c CurrentData::Keep, the target \c std::string is not cleared before
     *                      the result is written. Defaults to \c CurrentData::Clear.
     *
     * @return The (modified) std::string& result that was provided (for concatenation of calls).
     **********************************************************************************************/
    inline
    std::string& ToStdString( const NString&     src,
                              std::string&       target,
                              integer            regionStart,
                              integer            regionLength    = strings::MaxLen,
                              lang::CurrentData  targetData      = lang::CurrentData::Clear )
    {

        if ( targetData == lang::CurrentData::Clear )
            target.clear();

        // adjust range, if empty return empty string
        if ( src.AdjustRegion( regionStart, regionLength ) )
            return target;

        // copy our buffer into target
        target.append( src.Buffer() + static_cast<size_t>(regionStart), static_cast<size_t>(regionLength ) );
        return  target;
    }

    /** ********************************************************************************************
     * Copies or appends a region of a given \ref aworx::NString into an object of type
     * \c std::string, which is created.
     * The given region is adjusted to match the length of the %String.
     *
     * @param src           The \alib{strings,StringBase,String} to convert into a std::string.
     * @param regionStart   The start index of the region in this to create the std::string from.
     * @param regionLength  The maximum length of the region to be copied.
     *                      Defaults to \b MaxLen.
     *
     * @return A \c std::string that represents the specified sub region of this object.
     **********************************************************************************************/
    inline
    std::string  ToStdString( const NString&    src,
                              integer           regionStart,
                              integer           regionLength= strings::MaxLen )
    {
        std::string result;
        ToStdString( src, result, regionStart, regionLength, lang::CurrentData::Keep );
        return result;
    }

    /** ********************************************************************************************
     * Copies or appends a region of a given \ref aworx::NString into an object of type
     * \c std::string.
     *
     * @param src The \alib{strings,StringBase,String} to convert into a std::string.
     * @return A \c std::string that represents this object.
     **********************************************************************************************/
    inline
    std::string  ToStdString( const NString& src )
    {
        return ToStdString( src, 0, src.Length() );
    }


    /** ********************************************************************************************
     * Copies or appends a region of a given \ref aworx::NString into an object of type
     * \c std::string.
     *
     * @param as           The \alib{strings,StringBase,String} to convert into a std::string.
     * @param target       A target std::string to copy the contents of this %String into.
     * @param targetData   If \c CurrentData::Keep, parameter \p{target} is not cleared before
     *                     the result is written. Defaults to \c CurrentData::Clear.
     * @return The (modified) \p{target} that was provided (returned to allow concatenation of calls).
     **********************************************************************************************/
    inline
    std::string& ToStdString( const NString&    as,
                              std::string&      target,
                              lang::CurrentData targetData = lang::CurrentData::Clear )
    {
        return ToStdString( as, target, 0, as.Length(), targetData );
    }

    /** ********************************************************************************************
     * Copies or appends a region of a given \ref aworx::WString into an object of type
     * \c std::wstring.
     * The given region is adjusted to match the length of the %String.
     *
     * @param src           The \alib{strings,StringBase,String} to convert into a std::string.
     * @param target        A result std::string to copy the specified region into.
     * @param regionStart   The start index of the region to be copied.
     * @param regionLength  The maximum length of the region to be copied.
     *                      Defaults to \b MaxLen.
     * @param targetData    If \c CurrentData::Keep, the target \c std::string is not cleared before
     *                      the result is written. Defaults to \c CurrentData::Clear.
     *
     * @return The (modified) std::string& result that was provided (for concatenation of calls).
     **********************************************************************************************/
    inline
    std::wstring& ToStdString( const WString&     src,
                               std::wstring&      target,
                               integer            regionStart,
                               integer            regionLength    = strings::MaxLen,
                               lang::CurrentData  targetData      = lang::CurrentData::Clear )
    {

        if ( targetData == lang::CurrentData::Clear )
            target.clear();

        // adjust range, if empty return empty string
        if ( src.AdjustRegion( regionStart, regionLength ) )
            return target;

        // copy our buffer into target
        target.append( src.Buffer() + static_cast<size_t>(regionStart), static_cast<size_t>(regionLength ) );
        return  target;
    }

    /** ********************************************************************************************
     * Copies or appends a region of a given \ref aworx::WString into an object of type
     * \c std::wstring, which is created.
     * The given region is adjusted to match the length of the %String.
     *
     * @param src           The \alib{strings,StringBase,String} to convert into a std::string.
     * @param regionStart   The start index of the region in this to create the std::string from.
     * @param regionLength  The maximum length of the region to be copied.
     *                      Defaults to \b MaxLen.
     *
     * @return A \c std::string that represents the specified sub region of this object.
     **********************************************************************************************/
    inline
    std::wstring ToStdString( const WString& src,
                              integer        regionStart,
                              integer        regionLength= strings::MaxLen )
    {
        std::wstring result;
        ToStdString( src, result, regionStart, regionLength, lang::CurrentData::Keep );
        return result;
    }

    /** ********************************************************************************************
     * Copies or appends a region of a given \ref aworx::WString into an object of type
     * \c std::wstring.
     *
     * @param src The \alib{strings,StringBase,String} to convert into a std::string.
     * @return A \c std::string that represents this object.
     **********************************************************************************************/
    inline
    std::wstring ToStdString( const WString& src )
    {
        return ToStdString( src, 0, src.Length() );
    }


    /** ********************************************************************************************
     * Copies or appends a region of a given \ref aworx::WString into an object of type
     * \c std::wstring.
     *
     * @param as           The \alib{strings,StringBase,String} to convert into a std::string.
     * @param target       A target std::string to copy the contents of this %String into.
     * @param targetData   If \c CurrentData::Keep, parameter \p{target} is not cleared before
     *                     the result is written. Defaults to \c CurrentData::Clear.
     * @return The (modified) \p{target} that was provided (returned to allow concatenation of calls).
     **********************************************************************************************/
    inline
    std::wstring& ToStdString( const WString&     as,
                               std::wstring&      target,
                               lang::CurrentData  targetData = lang::CurrentData::Clear )
    {
        return ToStdString( as, target, 0, as.Length(), targetData );
    }


// We are faking template specializations of namespace string for doxygen into sub namespace
// thirdparty::std to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
}}
#endif


}}}

#endif // ALIB_MODULE_STRINGS

// #################################################################################################
// %ALib %Boxing / std::string compatibility support
// #################################################################################################

#if ALIB_MODULE_BOXING

#if !ALIB_FEAT_BOXING_FTYPES
    static_assert(0, "This implementation of boxing std::string may be used only if default "
                     "boxing of fundamental types is activated."
#endif

namespace aworx { namespace lib { namespace boxing
{

#if !defined(DOX_PARSER)
    ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING

// We are faking all template specializations of namespace boxing for doxygen into namespace
// boxing::thirdparty to keep the documentation of namespace string clean!
#else
    /**
     * \attention
     *   This is a non-existing namespace! It is exclusively defined for the
     *   [documentation parser](http://www.stack.nl/~dimitri/doxygen).
     *
     * \attention
     *   In this <b>"documentation namespace"</b>, you will find sub namespaces
     *   which in turn are purely created for documentation.
     */
    namespace thirdparty {
    /**
     * Implements <b>%ALib %Boxing</b> for string types of the standard library.
     * On boxing, the string types are mapped to boxing arrays of the corresponding character types.
     *
     * \note The boxing definitions described here are available through header file <br>
     *       <b>"alib/compatiblity/std_string.hpp"</b>.
     *
     *  The following specializations are made:
     *  - Type \c std::string is boxed as \c char[]
     *  - Type \c std::wstring is boxed as \c wchar_[]
     *  - If \c wchar_t is 2 bytes wide, then \c std::u32string is boxed as \ref aworx::strangeChar[]
     *  - If \c wchar_t is 4 bytes wide, then \c std::u16string is boxed as \ref aworx::strangeChar[]
     *
     *  The same mapping applies for pointer types of named std types.
     *
     *  If languages standard C++ 17 is in place, also the corresponding "view" classes
     *  (e.g. \c std::string_view) are boxed likewise.
     *
     *  All types are unboxable als values.
     *
     * \note
     *   Namespace function  #InitStdString is to be invoked on bootstrap to complete the support
     *   of <c>std::string</c>-type boxing.
     *
     * \attention
     *   This is a non-existing namespace, exclusively defined for the
     *   [documentation parser](http://www.stack.nl/~dimitri/doxygen).
     *   This documentation is moved here to keep the original namespace documentation clean.
     */
    namespace std {
#endif


#if !defined(DOX_PARSER)

    // ###################################  Boxing std::string  ####################################
//! [DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_1]
ALIB_BOXING_SPECIALIZE_CB_CUB( std::string, char, true, true );
//! [DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_1]

//! [DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_2]
inline   void  T_Boxing<std::string>::Boxing( Box& box,  const std::string&  value )
{
    box.data.Value=  reinterpret_cast<boxvalue>( &value[0]  );
    box.data.Length= static_cast     <integer>( value.length() );
}
//! [DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_2]


//! [DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_3]
inline  std::string    T_Boxing<std::string>::Unboxing( const Box& box )
{
    return  std::string( (const char*) box.data.Value,
                          box.data.Length >= 0 ? static_cast<size_t>(box.data.Length)
                                               : strlen( (const char*) box.data.Value )
                       );
}
//! [DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_3]

    // ##########################  Boxing wstring/u16string/u32string  ####################################
    ALIB_BOXING_SPECIALIZE_CB_CUB( std::wstring,    wchar_t, true, true );
    ALIB_BOXING_SPECIALIZE_CB_CUB( std::u16string, char16_t, true, true );
    ALIB_BOXING_SPECIALIZE_CB_CUB( std::u32string, char32_t, true, true );

    inline   void  T_Boxing<std::wstring>::Boxing( Box& box,  const std::wstring&  value )
    {
        box.data.Value=    (boxvalue) &value[0];
        box.data.Length=   static_cast<integer>(value.length());
    }

    inline   void  T_Boxing<std::u16string>::Boxing( Box& box,  const std::u16string&  value )
    {
        box.data.Value=    (boxvalue) &value[0];
        box.data.Length=   static_cast<integer>(value.length());
    }

    inline   void  T_Boxing<std::u32string>::Boxing( Box& box,  const std::u32string&  value )
    {
        box.data.Value=    (boxvalue) &value[0];
        box.data.Length=   static_cast<integer>(value.length());
    }

    inline  std::wstring    T_Boxing<std::wstring>::Unboxing( const Box& box )
    {
        return  std::wstring( (const wchar_t*) box.data.Value,
                              box.data.Length >= 0 ? static_cast<size_t>(box.data.Length)
                                                   : wcslen( (const wchar_t*) box.data.Value )
                            );
    }

    #if ALIB_MODULE_STRINGS
        #if ALIB_SIZEOF_WCHAR_T == 4
            inline  std::u16string    T_Boxing<std::u16string>::Unboxing( const Box& box )
            {
                return  std::u16string( (const char16_t*) box.data.Value, static_cast<size_t>(
                                        box.data.Length >= 0 ? box.data.Length
                                                             : strings::CString<strangeChar>::Length( (const strangeChar*) box.data.Value ) )
                                      );

            }

            inline  std::u32string    T_Boxing<std::u32string>::Unboxing( const Box& box )
            {
                return  std::u32string( (const char32_t*) box.data.Value, static_cast<size_t>(
                                        box.data.Length >= 0 ? box.data.Length
                                                             : strings::CString<wchar>::Length( (const wchar*) box.data.Value ) )
                                      );
            }
        #else
            inline  std::u32string    T_Boxing<std::u32string>::Unboxing( const Box& box )
            {
                return  std::u32string( (const char32_t*) box.data.Value, static_cast<size_t>(
                                        box.data.Length >= 0 ? box.data.Length
                                                             : strings::CString<strangeChar>::Length( (const strangeChar*) box.data.Value ) )
                                      );

            }

            inline  std::u16string    T_Boxing<std::u16string>::Unboxing( const Box& box )
            {
                return  std::u16string( (const char16_t*) box.data.Value, static_cast<size_t>(
                                        box.data.Length >= 0 ? box.data.Length
                                                             : strings::CString<wchar>::Length( (const wchar*) box.data.Value ) )
                                      );
            }
        #endif
    #endif


    // #################################  Boxing std::string_view  #################################
#if ALIB_CPP17
    ALIB_BOXING_SPECIALIZE_CB_CUB( std::string_view,        char, true, true );
    ALIB_BOXING_SPECIALIZE_CB_CUB( std::wstring_view,    wchar_t, true, true );
    ALIB_BOXING_SPECIALIZE_CB_CUB( std::u16string_view, char16_t, true, true );
    ALIB_BOXING_SPECIALIZE_CB_CUB( std::u32string_view, char32_t, true, true );
    inline   void  T_Boxing<std::string_view>::Boxing( Box& box,  const std::string_view&  value )
    {
        box.data.Value=  reinterpret_cast<boxvalue>( value.data()  );
        box.data.Length= static_cast     <integer>( value.length() );
    }

    inline   void  T_Boxing<std::wstring_view>::Boxing( Box& box,  const std::wstring_view&  value )
    {
        box.data.Value=    (boxvalue) value.data();
        box.data.Length=   static_cast<integer>(value.length());
    }

    inline   void  T_Boxing<std::u16string_view>::Boxing( Box& box,  const std::u16string_view&  value )
    {
        box.data.Value=    (boxvalue) value.data();
        box.data.Length=   static_cast<integer>(value.length());
    }

    inline   void  T_Boxing<std::u32string_view>::Boxing( Box& box,  const std::u32string_view&  value )
    {
        box.data.Value=    (boxvalue) value.data();
        box.data.Length=   static_cast<integer>(value.length());
    }

    inline  std::string_view    T_Boxing<std::string_view>::Unboxing( const Box& box )
    {
        return  std::string_view( (const char*) box.data.Value,
                                  box.data.Length >= 0 ? static_cast<size_t>(box.data.Length)
                                                       : strlen( (const char*) box.data.Value )
                               );
    }

    inline  std::wstring_view   T_Boxing<std::wstring_view>::Unboxing( const Box& box )
    {
        return  std::wstring_view( (const wchar_t*) box.data.Value,
                                   box.data.Length >= 0 ? static_cast<size_t>(box.data.Length)
                                                        : wcslen( (const wchar_t*) box.data.Value )
                                 );
    }

    #if ALIB_SIZEOF_WCHAR_T == 4
        inline  std::u16string_view    T_Boxing<std::u16string_view>::Unboxing( const Box& box )
        {
            return  std::u16string_view( (const char16_t*) box.data.Value, static_cast<size_t>(
                                          box.data.Length >= 0 ? box.data.Length
                                                               : strings::CString<strangeChar>::Length( (const strangeChar*) box.data.Value ) )
                                        );

        }

        inline  std::u32string_view    T_Boxing<std::u32string_view>::Unboxing( const Box& box )
        {
            return  std::u32string_view( (const char32_t*) box.data.Value, static_cast<size_t>(
                                         box.data.Length >= 0 ? box.data.Length
                                                              : strings::CString<wchar>::Length( (const wchar*) box.data.Value ) )
                                       );
        }
    #else
        inline  std::u32string_view    T_Boxing<std::u32string_view>::Unboxing( const Box& box )
        {
            return  std::u32string_view( (const char32_t*) box.data.Value, static_cast<size_t>(
                                         box.data.Length >= 0 ? box.data.Length
                                                              : strings::CString<strangeChar>::Length( (const strangeChar*) box.data.Value ) )
                                       );

        }

        inline  std::u16string_view    T_Boxing<std::u16string_view>::Unboxing( const Box& box )
        {
            return  std::u16string_view( (const char16_t*) box.data.Value, static_cast<size_t>(
                                         box.data.Length >= 0 ? box.data.Length
                                                              : strings::CString<wchar>::Length( (const wchar*) box.data.Value ) )
                                       );
        }
    #endif

#endif   //#if ALIB_CPP17
#endif  // #if !defined(DOX_PARSER)


void InitStdString();
/**
 * Initializes <b>%ALib %Boxing</b> system in respect to boxing <c>std::string</c>-types.
 *
 * This method is \b not automatically invoked with
 * \ref aworx::lib::ALib::Init "ALib::Init", because the support for boxing
 * <c>std::string</c>-types is optional and due to the inclusion of header
 * <b>"alib/compatibility/std_string.hpp"</b>.
 *
 * In general boxing of <c>std::string</c>-types works well without the one-time invocation of
 * this function at the bootstrap of a process. What this method currently adds is interface
 * \ref aworx::lib::strings::boxing::IApply "IApply" for boxed type
 * \ref aworx::lib::boxing::BoxedAs "BoxedAs<std::string>" and similar std string types. This is only
 * needed in some particular use cases.
 */
inline void InitStdString()
{
    #if ALIB_MODULE_STRINGS
//! [DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_4]
aworx::lib::boxing::DefineInterface<BoxedAs<std::string   >  , false , strings::boxing::IApply_BoxedAs_TApplicable<nchar,std::string   >>();
aworx::lib::boxing::DefineInterface<BoxedAs<std::string   >  , false , strings::boxing::IApply_BoxedAs_TApplicable<wchar,std::string   >>();
//! [DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_4]
        aworx::lib::boxing::DefineInterface<BoxedAs<std::wstring  >  , false , strings::boxing::IApply_BoxedAs_TApplicable<nchar,std::wstring  >>();
        aworx::lib::boxing::DefineInterface<BoxedAs<std::wstring  >  , false , strings::boxing::IApply_BoxedAs_TApplicable<wchar,std::wstring  >>();
        #if ALIB_SIZEOF_WCHAR_T == 4
        aworx::lib::boxing::DefineInterface<BoxedAs<std::u16string>  , false , strings::boxing::IApply_BoxedAs_TApplicable<nchar,std::u16string>>();
        aworx::lib::boxing::DefineInterface<BoxedAs<std::u16string>  , false , strings::boxing::IApply_BoxedAs_TApplicable<wchar,std::u16string>>();
        #else
        aworx::lib::boxing::DefineInterface<BoxedAs<std::u32string>  , false , strings::boxing::IApply_BoxedAs_TApplicable<nchar,std::u32string>>();
        aworx::lib::boxing::DefineInterface<BoxedAs<std::u32string>  , false , strings::boxing::IApply_BoxedAs_TApplicable<wchar,std::u32string>>();
        #endif
    #endif
}


// We are faking all template specializations of namespace boxing for doxygen into namespace
// boxing::thirdparty to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
}}
#endif

}}} // namespace [aworx::lib::boxing]

#if !defined(DOX_PARSER)
    ALIB_WARNINGS_RESTORE
#endif

#endif // ALIB_MODULE_BOXING

#endif // HPP_ALIB_COMPATIBILITY_STD_STRING
