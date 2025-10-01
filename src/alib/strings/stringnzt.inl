//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace strings {

//==================================================================================================
/// This is rather a helper-class than a genuine string class. While derived from class \b String,
/// no extension of its interface is offered. Instead, the templated constructor is limited to
/// accepting only string-types that are not zero-terminated.
///
/// The class is designed to be used to create API interfaces (i.e., overloaded methods) that accept
/// zero-terminated strings of type \alib{strings;TCString;CString} independently of
/// non-zero-terminated strings: If two overloaded methods accepted type \c String and \c CString
/// alternatively, compilation would be ambiguous because the \b String version would accept
/// zero-terminated strings as well. Therefore, overloaded methods rather must accept this class
/// and \b CString.
///
/// \see
///   Details and a quick sample are given in chapter
///   \ref alib_strings_nzt "4. Non-Zero-Terminated String Detection" of the
///   \ref alib_mod_strings "Programmer's Manual" of module \alib_strings_nl.
///
/// @tparam TChar The \ref alib_characters_chars "character type" of this string.
///               Alias names of this class using different character types are provided
///               with in namespace #alib with type definitions
///               \alib{StringNZT},
///               \alib{NStringNZT},
///               \alib{WStringNZT}, and
///               \alib{XStringNZT}.
//==================================================================================================
template<typename TChar>
class TStringNZT  : public TString<TChar>
{
  public:
    /// Constructor accepting a pointer to a character array and a string length.
    ///
    /// @param pBuffer   Pointer to the start of the character string to represent.
    /// @param pLength   The length of the character string to represent.
    constexpr
    TStringNZT( const TChar* pBuffer, integer pLength )
    : TString<TChar>( pBuffer, pLength)                                                           {}


    /// Templated \b implicit constructor accepting a \c const reference to an object of a
    /// type that satisfies the concept \alib{characters;IsImplicitArraySource} while it does
    /// \b not satisfy the concept \alib{characters;IsImplicitZTArraySource}.<br>
    /// In other words, this implicit constructor accepts only non-zero-terminated character
    /// arrays.
    ///
    /// @tparam T    The type of the given \p{src}.
    /// @param  src  The source of the string data to copy.
    template <typename T>
    requires    (       characters::IsImplicitArraySource <T, TChar>
                  &&   !characters::IsImplicitZTArraySource<T, TChar> )
    constexpr           TStringNZT(const T& src)
    : TString<TChar>( characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Buffer( src ),
                      characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Length( src ) )         {}

    /// Templated \b implicit constructor accepting a \c const pointer to an object of a
    /// type that satisfies the concept \alib{characters;IsImplicitArraySource} while it does
    /// \b not satisfy the concept \alib{characters;IsImplicitZTArraySource}.<br>
    /// In other words, this implicit constructor accepts only non-zero-terminated character
    /// arrays.
    ///
    /// @tparam T    The type of the given \p{src}.
    /// @param  src  The source of the string data to copy.
    template <typename T>
    requires    (       characters::IsImplicitArraySource <T, TChar>
                  &&   !characters::IsImplicitZTArraySource<T, TChar> )
    constexpr           TStringNZT(const T* src)
    : TString<TChar>( characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Buffer( *src ),
                      characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Length( *src ) )        {}


    /// Templated \b implicit constructor accepting a \c const reference to an object of a
    /// type that satisfies the concept \alib{characters;IsExplicitArraySource}.
    ///
    /// @tparam T    The type of the given \p{src}.
    /// @param  src  The source of the string data to copy.
    template <typename T>
    requires characters::IsExplicitArraySource <T, TChar>
    constexpr explicit  TStringNZT(const T& src)
    : TString<TChar>( characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Buffer( src ),
                      characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Length( src ) )         {}

    /// Templated \b implicit constructor accepting a \c const pointer to an object of a
    /// type that satisfies the concept \alib{characters;IsExplicitArraySource}.
    ///
    /// @tparam T    The type of the given \p{src}.
    /// @param  src  The source of the string data to copy.
    template <typename T>
    requires characters::IsExplicitArraySource <T, TChar>
    constexpr explicit  TStringNZT(const T* src)
    : TString<TChar>( characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Buffer( *src ),
                      characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Length( *src ) )        {}

    /// Templated \b implicit constructor accepting a mutable reference to an object of a
    /// type that satisfies the concept \alib{characters;IsMutableArraySource}.
    ///
    /// @tparam T    The type of the given \p{src}.
    /// @param  src  The source of the string data to copy.
    template<typename T>
    requires characters::IsMutableArraySource<T, TChar>
    constexpr explicit  TStringNZT(      T& src)
    : TString<TChar>( characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Buffer( src ),
                      characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Length( src ) )         {}

    /// Templated \b implicit constructor accepting a mutable pointer to an object of a
    /// type that satisfies the concept \alib{characters;IsMutableArraySource}.
    ///
    /// @tparam T    The type of the given \p{src}.
    /// @param  src  The source of the string data to copy.
    template<typename T>
    requires characters::IsMutableArraySource<T, TChar>
    constexpr explicit  TStringNZT(      T* src)
    : TString<TChar>( characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Buffer( *src ),
                      characters::ArrayTraits<std::remove_cv_t<T>,TChar>::Length( *src ) )        {}

}; // class TStringNZT

// #################################################################################################
// Specializations of ArrayTraits for class TStringNZT
// #################################################################################################
} namespace characters {
#if !DOXYGEN
template<typename TChar>
struct  ArrayTraits<strings::TStringNZT<TChar>, TChar>
{
    static constexpr Policy       Access                                      = Policy::Implicit;
    static constexpr Policy       Construction                                = Policy::NONE;
    static constexpr const TChar* Buffer(const strings::TString<TChar>& src)  {return src.Buffer();}
    static constexpr integer      Length(const strings::TString<TChar>& src)  {return src.Length();}
};
#endif // !DOXYGEN
}

/// Type alias in namespace \b alib.
using  StringNZT        =     strings::TStringNZT<character>;

/// Type alias in namespace \b alib.
using  ComplementStringNZT=   strings::TStringNZT<complementChar>;

/// Type alias in namespace \b alib.
using  StrangeStringNZT =     strings::TStringNZT<strangeChar>;

/// Type alias in namespace \b alib.
using  NStringNZT       =     strings::TStringNZT<nchar>;

/// Type alias in namespace \b alib.
using  WStringNZT       =     strings::TStringNZT<wchar>;

/// Type alias in namespace \b alib.
using  XStringNZT       =     strings::TStringNZT<xchar>;

} // namespace [alib::strings]


