//==================================================================================================
/// \file
/// This header-file is part of module \alib_characters of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::characters {

/// Enumeration of possible values for fields \b %Access and \b %Construction of traits-types
/// \alib{characters;ArrayTraits} and \alib{characters;ZTArrayTraits}.
/// The elements of this enumeration indicate if and how the data of a "char-array-like type"
/// (which may be non-zero- or zero-terminated) may be accessed and, reversely, how such types
/// may be constructed from character arrays.
///
/// This is best explained when looking at class \alib{strings;TString} of higher-level
/// module \alib_strings.
/// That classes' constructors use this policy to decide if an instance it may be constructed
/// implicitly or explicitly from a third-party type.
/// In the other direction, the type provides cast methods, which let instances of \b TString
/// implicitly or explicitly convert into third-party types.
enum class Policy
{
    /// Data may not be received, respectively the custom type may not be constructed from
    /// a (character-array-) type.
    /// This value usually indicates that a custom type does not represent a character array at all.
    /// Hence, this is the default value exposed by the non-specialized version of type-traits
    /// structs \alib{characters;ArrayTraits} and \alib{characters;ZTArrayTraits}.
    NONE,

    /// Allows implicit (and explicit) access of the character array data, respectively allows
    /// implicit and explicit construction of string-like custom types, from mutable or constant
    /// objects.
    Implicit,

    /// Allows explicit access of the character array data respectively allows
    /// explicit construction of string-like custom types, from mutable or constant objects.
    ExplicitOnly,

    /// Allows explicit access of the character array data from mutable objects only.<br>
    /// With field \b %Construction of traits-types \alib{characters;ArrayTraits} and
    /// \alib{characters;ZTArrayTraits}, this enumeration element is not used.
    MutableOnly,
};

//==================================================================================================
/// This type provides type-traits for character arrays. Specializations of this struct for
/// a custom type \p{TStringSource}, expose information about that type representing a character
/// array and how the array data may be accessed.<br>
/// In addition, information about how the type may be constructed from character array data
/// may be provided with specializations of this type.
///
/// \see
///   For detailed information, see chapter \ref alib_characters_arrays "4. Character Arrays" of
///   the Programmer's Manual of module \alib_characters.
///
/// @tparam TStringSource The type for which specializations of this struct provide array type-traits.
/// @tparam TChar       The character type of the character array that specializations provide
///                     type-traits for.
//==================================================================================================
template<typename TStringSource, typename TChar>
struct ArrayTraits
{
    /// Provides information about how the character array data of instances of type
    /// \p{TStringSource} may be accessed
    static constexpr Policy    Access                                                = Policy::NONE;

    /// Provides information about if and how instances of type \p{TStringSource} may be created
    /// from character array data.
    static constexpr Policy    Construction                                          = Policy::NONE;

  #if DOXYGEN
    /// In specialized versions, this method has to be provided in case that field #Access is
    /// not equal to \alib{characters;Policy::NONE}.
    /// In addition to this static method, sibling method #Length has to be provided.
    ///
    /// For example, in a specialization for standard C++ class \c std::string, this method returns
    /// the result of method <c>std::string::data()</c>.
    ///
    /// Note that in the case of access type \alib{characters;Policy::MutableOnly}, the
    /// signature of this method needs to be slightly different with the specialization: argument
    /// \p{src} in this case must be of type <c>TStringSource&</c>, hence must not be \c const.
    ///
    /// @param  src The source object of external type \p{TStringSource}.
    /// @returns Specializations have to return a pointer to the start of the character array
    ///          represented by the given object \p{src} of type \p{TStringSource}.
    static
    const TChar* Buffer( const TStringSource& src );

    /// In specialized versions, this method has to be provided in case that field #Access is
    /// not equal to \alib{characters;Policy::NONE}.
    /// In addition to this static method, sibling method #Buffer has to be provoided.
    ///
    /// For example, in a specialization for standard C++ class \c std::string, this method returns
    /// the result of method <c>std::string::size()</c>.
    ///
    /// Note, that in the case of access type \alib{characters;Policy::MutableOnly}, the
    /// signature of this method needs to be slightly different with the specialization: argument
    /// \p{src} in this case must be of type <c>TStringSource&</c>, hence must not be \c const.
    ///
    /// @param  src The source object of external type \p{TStringSource}.
    /// @returns Specializations have to return the length of the character array
    ///          represented by the given object \p{src} of type \p{TStringSource}.
    static
    integer Length( const TStringSource& src );

    /// In specialized versions, this method has to be provided in case that field #Construction is
    /// not equal to \alib{characters;Policy::NONE}.
    ///
    /// If so, this method needs to construct and return an instance of type \p{TStringSource}, created
    /// from the character array specified by arguments \p{array} and \p{length}
    ///
    /// @param  array  The external array to be used to create the return value.
    /// @param  length The length of the external array.
    /// @returns A new instance (value) of type \p{TStringSource}.
    static
    TStringSource  Construct(  const TChar* array, integer length );
  #endif
};

//==================================================================================================
/// This type trait is in all aspects equivalent to \alib{characters;ArrayTraits}, only that
/// this struct provides traits on zero-terminated character arrays, while \b %ArrayTraits is
/// about non-zero-terminated arrays.
///
/// Please, therefore consult the documentation of type \alib{characters;ArrayTraits}.
///
/// \see
///   For detailed information, see chapter \ref alib_characters_arrays "4. Character Arrays" of
///   the Programmer's Manual of module \alib_characters.
///
/// @tparam TStringSource The type for which specializations of this struct provide array type-traits.
/// @tparam TChar       The character type of the character array that specializations provide
///                     type-traits for.
//==================================================================================================
template<typename TStringSource, typename TChar>
struct ZTArrayTraits
{
    /// Provides information about how the zero-terminated character arrays of instances of
    /// type \p{TStringSource} may be accessed.
    static constexpr Policy    Access                                              = Policy::NONE;

    /// Provides information about if and how instances of type \p{TStringSource} may be created
    /// from zero-terminated character arrays.
    static constexpr Policy    Construction                                        = Policy::NONE;

  #if DOXYGEN
    /// Same as corresponding method \alib{characters::ArrayTraits;Buffer} of sibling struct
    /// \b ArrayTraits.
    ///
    /// @param   src The source object of external type \p{TStringSource}.
    /// @returns Specializations have to return a pointer to the start of the zero-terminated
    ///          character array represented by the given object \p{src} of type \p{TStringSource}.
    static
    const TChar*    Buffer( const TStringSource& src );

    /// Same as corresponding method \alib{characters::ArrayTraits;Length} of sibling struct
    /// \b ArrayTraits.
    ///
    /// @param   src The source object of external type \p{TStringSource}.
    /// @returns Specializations have to return the length of the character array
    ///          represented by the given object \p{src} of type \p{TStringSource}.
    static
    integer         Length( const TStringSource& src );

    /// Same as corresponding method \alib{characters::ArrayTraits;Construct} of sibling struct
    /// \b ArrayTraits.
    ///
    /// @param  array  The external zero-terminated array to be used to create the return value.
    /// @param  length The length of the external array.
    /// @returns A new instance (value) of type \p{TStringSource}.
    static
    TStringSource   Construct(  const TChar* array, integer length );
  #endif
}; // struct ZTArrayTraits


ALIB_WARNINGS_IGNORE_DOCS
//==================================================================================================
/// Concept which is satisfied for given template type \p{T} in case a specialization of
/// the type trait \alib{characters;ArrayTraits;ArrayTraits<T, TChar>} exists.
///
/// \see
///  - For details see chapter \ref alib_characters_arrays_traits_tool_arraytype of
///    the Programmer's Manual of module \alib_characters_nl.
///  - Type alias {characters;CharType}.
///  - A sibling concept exists with concept \alib{characters;IsZTArray}.
///
/// @tparam T       The custom type to test for being or incorporating a character array.
/// @tparam TChar   The character type of the array to test for.
//==================================================================================================
template<typename T, typename TChar>
concept IsArray=  ArrayTraits<T,TChar>::Access != Policy::NONE;

//==================================================================================================
/// Determines the character type of the array that the given template type \p{T} represents.
/// In case no a specialization of the type trait
/// \alib{characters;ArrayTraits;ArrayTraits<T, TChar>} exists, this evaluates to \c void.
/// Otherwise, this alias evaluates to one of \alib{characters;nchar}, \alib{characters;wchar}
/// or \alib{characters;xchar}.
///
/// \see
///  - For details see chapter \ref alib_characters_arrays_traits_tool_arraytype of
///    the Programmer's Manual of module \alib_characters_nl.
///  - Concept \alib{characters;IsArray}.
///  - A sibling alias exists with \alib{characters;ZTType}.
///
/// @tparam T    The custom type to get the character type for.
//==================================================================================================
template<typename T>
using  Type= std::conditional_t<ArrayTraits<T,nchar>::Access != Policy::NONE, nchar,
             std::conditional_t<ArrayTraits<T,wchar>::Access != Policy::NONE, wchar,
             std::conditional_t<ArrayTraits<T,xchar>::Access != Policy::NONE, xchar,
                                                                              void   > > >;

//==================================================================================================
/// Concept which is satisfied for given template type \p{T} in case a specialization of
/// the type trait \alib{characters;ZTArrayTraits;ZTArrayTraits<T, TChar>} exists.
///
/// \see
///  - For details see chapter \ref alib_characters_arrays_traits_tool_arraytype of
///    the Programmer's Manual of module \alib_characters_nl.
///  - Type alias \alib{characters;ZTType}.
///  - A sibling concept exists with concept \alib{characters;IsZTArray}.
///
/// @tparam T       The custom type to test for being or incorporating a zero-terminated character
///                 array.
/// @tparam TChar   The character type of the array to test for.
//==================================================================================================
template<typename T, typename TChar>
concept IsZTArray=  ZTArrayTraits<T,TChar>::Access != Policy::NONE;

/// Determines the character type of the zero-terminated array that the given template type
/// \p{T} represents.
/// In case no a specialization of the type trait
/// \alib{characters;ArrayTraits;ZTArrayTraits<T, TChar>} exists, this evaluates to \c void.
/// Otherwise, this alias evaluates to one of \alib{characters;nchar}, \alib{characters;wchar}
/// or \alib{characters;xchar}.
///
/// \see
///  - For details see chapter \ref alib_characters_arrays_traits_tool_arraytype of
///    the Programmer's Manual of module \alib_characters_nl.
///  - Concept \alib{characters;IsArray}.
///  - A sibling alias exists with \alib{characters;ZTType}.
///
/// @tparam T    The custom type to get the character type for.
template<typename T>
using  ZTType= std::conditional_t<ZTArrayTraits<T,nchar>::Access != Policy::NONE, nchar,
               std::conditional_t<ZTArrayTraits<T,wchar>::Access != Policy::NONE, wchar,
               std::conditional_t<ZTArrayTraits<T,xchar>::Access != Policy::NONE, xchar,
                                                                                  void  > > >;

/// A concept to identify types which can be used to \b implicitly construct string types.
/// @see Traits class \alib{characters;ArrayTraits} which may be specialized for custom types to
///      satisfy this concept - with the aim to make type \p{T} compatible (exchangeable) with
///      string types.
/// @tparam T       The type to be tested (with \c const and \c volatile removed).
/// @tparam TChar   The character type of the string type.
template <typename T, typename TChar>
concept IsImplicitArraySource =     ArrayTraits<std::remove_cv_t<T>, TChar>::Access
                                  == Policy::Implicit;

/// A concept to identify types which can be used to \b explicitly construct string types.
/// @see Traits class \alib{characters;ArrayTraits} which may be specialized for custom types to
///      satisfy this concept - with the aim to make type \p{T} compatible (exchangeable) with
///      string types.
/// @tparam T       The type to be tested (with \c const and \c volatile removed).
/// @tparam TChar   The character type of the string type
///                 to be constructed.
template <typename T, typename TChar>
concept IsExplicitArraySource =     ArrayTraits<std::remove_cv_t<T>, TChar>::Access
                                  == Policy::ExplicitOnly;

/// A concept to identify <b>non-const</b> types which can be used to \b explicitly construct
/// string types.
/// Used for explicit string construction, when the access-traits need to
/// perform changes on the source object, which imply that those are mutable.
/// @see Traits class \alib{characters;ArrayTraits} which may be specialized for custom types to
///      satisfy this concept - with the aim to make type \p{T} compatible (exchangeable) with
///      string types.
/// @tparam T       The type to be tested (with \c const and \c volatile removed).
/// @tparam TChar   The character type of the string type
///                 to be constructed.
template <typename T, typename TChar>
concept IsMutableArraySource =         ArrayTraits<std::remove_cv_t<T>, TChar>::Access
                                                == Policy::MutableOnly
                                 && !std::is_const_v<T>;

/// A concept to identify types that string types can be \b implicitly cast to.
/// In other words, types which are constructible from string types.
/// @see Traits class \alib{characters;ArrayTraits} which may be specialized for custom types to
///      satisfy this concept - with the aim to make type \p{T} compatible (exchangeable) with
///      string types.
/// @tparam T       The type to be tested (with \c const and \c volatile removed).
/// @tparam TChar   The character type of the string type that should be cast to \p{T}.
template <typename T, typename TChar>
concept IsImplicitArrayCast=    ArrayTraits<std::remove_cv_t<T>,TChar>::Construction
                             == Policy::Implicit;

/// A concept to identify types that string types can be \b implicitly cast to.
/// In other words, types which are constructible from string types.
/// @see Traits class \alib{characters;ArrayTraits} which may be specialized for custom types to
///      satisfy this concept - with the aim to make type \p{T} compatible (exchangeable) with
///      string types.
/// @tparam T       The type to be tested (with \c const and \c volatile removed).
/// @tparam TChar   The character type of the string type that should be cast to \p{T}.
template <typename T, typename TChar>
concept IsExplicitArrayCast=     ArrayTraits<std::remove_cv_t<T>,TChar>::Construction
                              == Policy::ExplicitOnly;

/// A concept to identify types which can be used to \b implicitly construct zero-terminated
/// strings.
/// @see Traits class \alib{characters;ZTArrayTraits} which may be specialized for custom types to
///      satisfy this concept - with the aim to make type \p{T} compatible (exchangeable) with
///      zero-terminated string types.
/// @tparam T       The type to be tested (with \c const and \c volatile removed).
/// @tparam TChar   The character type of the string type.
template <typename T, typename TChar>
concept IsImplicitZTArraySource=     ZTArrayTraits<std::remove_cv_t<T>, TChar>::Access
                                  == Policy::Implicit;

/// A concept to identify types which can be used to \b explicitly construct zero-terminated
/// strings.
/// @see Traits class \alib{characters;ZTArrayTraits} which may be specialized for custom types to
///      satisfy this concept - with the aim to make type \p{T} compatible (exchangeable) with
///      zero-terminated string types.
/// @tparam T       The type to be tested (with \c const and \c volatile removed).
/// @tparam TChar   The character type of the string type.
template <typename T, typename TChar>
concept IsExplicitZTArraySource=     ZTArrayTraits<std::remove_cv_t<T>, TChar>::Access
                                  == Policy::ExplicitOnly;

/// A concept to identify <b>non-const</b> types which can be used to \b explicitly construct
/// \alib \alib{strings;TString;strings}.
/// Used for explicit string construction, when the access-traits need to
/// perform changes on the source object, which imply that those are mutable.
/// @see Traits class \alib{characters;ZTArrayTraits} which may be specialized for custom types to
///      satisfy this concept - with the aim to make type \p{T} compatible (exchangeable) with
///      zero-terminated string types.
/// @tparam T       The type to be tested (with \c const and \c volatile removed).
/// @tparam TChar   The character type of the string type to be constructed.
template <typename T, typename TChar>
concept IsMutableZTArraySource=        ZTArrayTraits<std::remove_cv_t<T>, TChar>::Access
                                    == Policy::MutableOnly
                                && !std::is_const_v<T>;


/// Same as \alib{characters;IsImplicitZTArrayCast}, but uses type-traits class
/// \alib{characters;ZTArrayTraits} instead of  \alib{characters;ArrayTraits}.
/// @tparam T       The type to be tested (with \c const and \c volatile removed).
/// @tparam TChar   The character type of the zero-terminated string type that should be cast
///                 to \p{T}.
template <typename T, typename TChar>
concept IsImplicitZTArrayCast=     ZTArrayTraits<std::remove_cv_t<T>,TChar>::Construction
                                == Policy::Implicit;

/// Same as \alib{characters;IsExplicitZTArrayCast}, but uses type-traits class
/// \alib{characters;ZTArrayTraits} instead of \alib{characters;ArrayTraits}.
/// @tparam T       The type to be tested.
/// @tparam TChar   The character type of the zero-terminated string type that should be cast
///                 to \p{T}.
template <typename T, typename TChar>
concept IsExplicitZTArrayCast=     ZTArrayTraits<std::remove_cv_t<T>,TChar>::Construction
                                == Policy::ExplicitOnly;


ALIB_WARNINGS_RESTORE // ignore docs due to clang not knowing concepts


//##################################################################################################
// Specializations of ArrayTraits and ZTArrayTraits for const and non-const character pointer types
//##################################################################################################

#if !DOXYGEN
//--------------------------------------- Fixed length arrays --------------------------------------
template<size_t TCapacity, typename TChar>
struct ArrayTraits<TChar[TCapacity], TChar>
{
    static constexpr Policy         Access                                       = Policy::Implicit;
    static constexpr const TChar*   Buffer( TChar const (&src) [TCapacity] )         { return src; }
    static constexpr integer        Length( TChar const (&   ) [TCapacity] ){ return TCapacity -1; }
};

template<size_t TCapacity, typename TChar>
struct ZTArrayTraits<TChar[TCapacity], TChar>
{
    static constexpr Policy         Access                                       = Policy::Implicit;
    static constexpr const TChar*   Buffer( TChar const (&src) [TCapacity] )         { return src; }
    static constexpr integer        Length( TChar const (&   ) [TCapacity] ){ return TCapacity -1; }
};

// C++20 type 'char8_t'
template<size_t TCapacity>
struct ArrayTraits<char8_t[TCapacity], nchar>
{
    static constexpr Policy         Access                                       = Policy::Implicit;
    static constexpr const nchar*   Buffer( char8_t const (&src) [TCapacity] ) { return reinterpret_cast<const nchar*>(src); }
    static constexpr integer        Length( char8_t const (&   ) [TCapacity] ) { return TCapacity -1; }
};

template<size_t TCapacity>
struct ZTArrayTraits<char8_t[TCapacity], nchar>
{
    static constexpr Policy         Access                                       = Policy::Implicit;
    static constexpr const nchar*   Buffer( char8_t const (&src) [TCapacity] ) { return reinterpret_cast<const nchar*>(src); }
    static constexpr integer        Length( char8_t const (&   ) [TCapacity] ) { return TCapacity -1; }
};


//----------------------------------- constant character pointers ----------------------------------
template<typename TChar> struct ArrayTraits<const TChar*, TChar>
{
    static constexpr Policy         Access                                  = Policy::Implicit;
    static constexpr Policy         Construction                            = Policy::ExplicitOnly;
    static constexpr const TChar*   Buffer(const TChar* const & src       ) { return src; }
    static constexpr integer        Length(const TChar* const & src       ) { return src ? integer( std::char_traits<TChar>::length(src) ) : 0; }
    static constexpr const TChar*   Construct(const TChar* array, integer ) { return array; }
};

template<typename TChar> struct ZTArrayTraits<const TChar*, TChar>
{
    static constexpr Policy         Access                                  = Policy::Implicit;
    static constexpr Policy         Construction                            = Policy::Implicit;
    static constexpr const TChar*   Buffer(const TChar* const & src       ) { return src; }
    static constexpr integer        Length(const TChar* const & src       ) { return src ? integer( std::char_traits<TChar>::length(src) ) : 0; }
    static constexpr const TChar*   Construct(const TChar* array, integer ) { return array; }
};

// C++20 type 'char8_t'
template<> struct ArrayTraits<const char8_t*, nchar>
{
    static constexpr Policy         Access                                  = Policy::Implicit;
    static constexpr Policy         Construction                            = Policy::ExplicitOnly;
    static           const nchar*   Buffer(const char8_t* const & src     ) { return reinterpret_cast<const nchar*>(src); }
    static constexpr integer        Length(const char8_t* const & src     ) { return src ? integer( std::char_traits<char8_t>::length(src) ) : 0; }
    static           const char8_t* Construct(const nchar* array, integer ) { return reinterpret_cast<const char8_t*>(array); }
};

template<> struct ZTArrayTraits<const char8_t*, nchar>
{
    static constexpr Policy         Access                                  = Policy::Implicit;
    static constexpr Policy         Construction                            = Policy::Implicit;
    static           const nchar*   Buffer(const char8_t* const & src     ) { return reinterpret_cast<const nchar*>(src); }
    static constexpr integer        Length(const char8_t* const & src     ) { return src ? integer( std::char_traits<char8_t>::length(src) ) : 0; }
    static           const char8_t* Construct(const nchar* array, integer ) { return reinterpret_cast<const char8_t*>(array); }
};

//------------------------------------ mutable character pointers ----------------------------------
template<typename TChar> struct ArrayTraits<TChar*, TChar>
{
    static constexpr Policy         Access                                  = Policy::ExplicitOnly;
    static constexpr Policy         Construction                            = Policy::ExplicitOnly;
    static constexpr const TChar*   Buffer(      TChar* const & src       ) { return src; }
    static constexpr integer        Length(      TChar* const & src       ) { return src ? integer( std::char_traits<TChar>::length(src) ) : 0; }
    static constexpr       TChar*   Construct(const TChar* array, integer ) { return const_cast<TChar*>( array ); }
};

template<typename TChar> struct ZTArrayTraits<TChar*, TChar>
{
    static constexpr Policy         Access                                  = Policy::ExplicitOnly;
    static constexpr Policy         Construction                            = Policy::ExplicitOnly;
    static constexpr const TChar*   Buffer(     TChar* const & src        ) { return src; }
    static constexpr integer        Length(     TChar* const & src        ) { return src ? integer( std::char_traits<TChar>::length(src) ) : 0; }
    static constexpr       TChar*   Construct(const TChar* array, integer ) { return const_cast<TChar*>( array ); }
};

// C++20 type 'char8_t'
template<> struct ArrayTraits<char8_t*, nchar>
{
    static constexpr Policy         Access                                  = Policy::ExplicitOnly;
    static constexpr Policy         Construction                            = Policy::ExplicitOnly;
    static           const nchar*   Buffer(      char8_t* const & src     ) { return reinterpret_cast<nchar*>(src);  }
    static constexpr integer        Length(      char8_t* const & src     ) { return src ? integer( std::char_traits<char8_t>::length(src) ) : 0; }
    static           char8_t*       Construct(const nchar* array, integer ) { return const_cast<char8_t*>( reinterpret_cast<const char8_t*>(array) ); }
};

template<> struct ZTArrayTraits<char8_t*, nchar>
{
    static constexpr Policy         Access                                  = Policy::ExplicitOnly;
    static constexpr Policy         Construction                            = Policy::ExplicitOnly;
    static           const nchar*   Buffer(   char8_t* const & src        ) { return reinterpret_cast<nchar*>(src);  }
    static constexpr integer        Length(   char8_t* const & src        ) { return src ? integer( std::char_traits<char8_t>::length(src) ) : 0; }
    static           char8_t*       Construct(const nchar* array, integer ) { return const_cast<char8_t*>( reinterpret_cast<const char8_t*>(array) ); }
};

#endif

//##################################################################################################
// Tools
//##################################################################################################

//==================================================================================================
/// This templated alias type-definition provides the "complement" to the given character type.
/// By that, it is defined to <c>wchar</c> if type <c>nchar</c> is given and vice versa.
///
/// Note that for type \alib{characters;xchar}, no complement is defined.
/// For this type (and all other types), this alias evaluates to <c>void</c>.
/// @tparam TChar The character type to get the complement for.
//==================================================================================================
template<typename TChar>
using ComplementType= std::conditional_t<std::same_as<TChar, nchar>, wchar,
                      std::conditional_t<std::same_as<TChar, wchar>, nchar,   void  > >;


//==================================================================================================
/// This templated type alias can be used to determine the corresponding \alib character type
/// from the size of the type. Typically (on common 64-bit platforms), this alias evaluates to:
/// \alib{characters;nchar} if \p{TSizeOf} is given as \c 0. Values \c 2 and \c 4 evaluate to
/// \alib{characters;wchar} or \alib{characters;xchar} depending on the compiler and platform.
///
/// As an example, the \https{QT Class Library,www.qt.io} uses a 2-byte character width,
/// independent of compiler and platform. Therefore, to convert a \b QT character value to
/// an \alib character value, the destination type is:
///
///          using qtChar= alib::characters::TypeBySize<2>;
///
/// @tparam TSizeOf The size of the required character type in bytes.
//==================================================================================================
template<size_t TSizeOf>
using TypeBySize = std::conditional_t<TSizeOf == sizeof(nchar), nchar,
                   std::conditional_t<TSizeOf == sizeof(wchar), wchar,
                   std::conditional_t<TSizeOf == sizeof(xchar), xchar, void   > > >;


ALIB_WARNINGS_IGNORE_DOCS

//==================================================================================================
/// A concept to identify \alib character types  \alib{characters;nchar}, \alib{characters;wchar}
/// and \alib{characters;xchar}. For these three types the concept is satisfied.
///
/// @tparam T The type to test for being an \alib character type.
//==================================================================================================
template<typename T>
concept IsCharacter=    std::is_same_v<T,nchar>
                     || std::is_same_v<T,wchar>
                     || std::is_same_v<T,xchar>;

//==================================================================================================
/// Returns the length of C++ arrays like <c>std::extent</c>, but subtracts \c 1 if the
/// array's element-type satisfies concept \alib{characters;IsCharacter}.
/// (Thus, this "removes" the zero-termination character.)
/// @tparam T The type to get the length for. Deduced by the compiler.
/// @return In case of character-arrays, the string length. For other arrays the true length
///         and \c 0 for non-array types.
//==================================================================================================
template<typename T>
constexpr integer  ArrayLength() {
    if constexpr (!std::is_array_v<T>)
        return 0;

    using TChar= std::remove_cv_t<std::remove_pointer_t<std::decay_t<T>>>;
    if constexpr( IsCharacter<TChar> )
        return std::extent<T>::value - 1;
    return std::extent<T>::value;
}

ALIB_WARNINGS_RESTORE

} // namespace [alib::character]
