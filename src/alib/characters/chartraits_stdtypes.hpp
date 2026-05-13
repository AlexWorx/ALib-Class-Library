//==================================================================================================
/// \file
/// This header-file is part of module \alib_characters of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib::characters {

//##################################################################################################
//###################################### ArrayTraits<std::xyz> #####################################
//##################################################################################################

#if DOXYGEN
/// This namespace contains sub-namespaces that provide compatibility of 3rd-party types and
/// module \alib_characters_nl.<br>
/// The entities of those namespaces become available with the inclusion of specific headers
/// that import a certain C++20-Module or inject the functionality into a namespace in a
/// traditional fashion, for example, header #"F;ALib.Strings.StdFormatter.H".
namespace compatibility {

/// This namespace documents compatibility features of \alib_characters_nl and the
/// standard C++ class library found in namespace \c std.
namespace std {
#endif

//######################################### std::string_view #######################################
/// Specialization of the type trait #"ArrayTraits" for type
/// <c>std::basic_string_view<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The type may be implicitly created from character array data.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy the concept #"ch IsCharacter".
template<typename TChar>
requires IsCharacter<TChar>
struct     ArrayTraits<std::basic_string_view<TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                               = Policy::Implicit;
    static constexpr Policy              Construction                                         = Policy::Implicit;
    static constexpr const TChar*        Buffer   (std::basic_string_view<TChar> const & src) { return          src.data  ()  ; }
    static constexpr integer             Length   (std::basic_string_view<TChar> const & src) { return integer( src.length() ); }
    constexpr static std::basic_string_view<TChar> Construct(const TChar* array, integer length       ) { return std::basic_string_view<TChar>( array, size_t(length) ); }
  #endif
};

/// Specialization of the type trait #"ArrayTraits" for type
/// <c>std::basic_string_view<char8_t></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The type may be implicitly created from character array data.
template<>
struct     ArrayTraits<std::basic_string_view<char8_t>, nchar>
{
  #if !DOXYGEN
    static constexpr Policy                Access                                               = Policy::Implicit;
    static constexpr Policy                Construction                                         = Policy::Implicit;
    static           const nchar*          Buffer   (std::basic_string_view<char8_t> const & src) { return reinterpret_cast<const nchar*>(src.data  ())  ; }
    static constexpr integer               Length   (std::basic_string_view<char8_t> const & src) { return integer( src.length() ); }
    static std::basic_string_view<char8_t> Construct(const nchar* array, integer length       )
    { return std::basic_string_view<char8_t>( reinterpret_cast<const char8_t*>(array), size_t(length) ); }
  #endif
};

/// Specialization of the type trait #"ZTArrayTraits" for type
/// <c>std::basic_string_view<TChar></c>:
/// - Zero-terminated string data is allowed to be explicitly accessed as usually data represented
///   by type \c std::string_view is not zero-terminated.
/// - The type may be implicitly created from zero-terminated character arrays.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy the concept #"ch IsCharacter".
template<typename TChar>
requires IsCharacter<TChar>
struct   ZTArrayTraits<std::basic_string_view<TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                                = Policy::ExplicitOnly;
    static constexpr Policy              Construction                                          = Policy::Implicit;
    static constexpr const TChar*        Buffer   (std::basic_string_view<TChar> const & src ) { return          src.data  ()  ; }
    static constexpr integer             Length   (std::basic_string_view<TChar> const & src ) { return integer( src.length() ); }
    static constexpr std::basic_string_view<TChar> Construct(const TChar* array, integer length  ) { return std::basic_string_view<TChar>( array, size_t(length) ); }
  #endif
};

/// Specialization of the type trait #"ZTArrayTraits" for type
/// <c>std::basic_string_view<char8_t></c>:
/// - Zero-terminated string data is allowed to be explicitly accessed as usually data represented
///   by type \c std::string_view is not zero-terminated.
/// - The type may be implicitly created from zero-terminated character arrays.
template<>
struct   ZTArrayTraits<std::basic_string_view<char8_t>, nchar>
{
  #if !DOXYGEN
    static constexpr Policy                Access                                                = Policy::ExplicitOnly;
    static constexpr Policy                Construction                                          = Policy::Implicit;
    static           const nchar*          Buffer   (std::basic_string_view<char8_t> const & src ) { return reinterpret_cast<const nchar*>(src.data  ())  ; }
    static constexpr integer               Length   (std::basic_string_view<char8_t> const & src ) { return integer( src.length() ); }
    static std::basic_string_view<char8_t> Construct(const nchar* array, integer length  )
    { return std::basic_string_view<char8_t>( reinterpret_cast<const char8_t*>(array), size_t(length) ); }
  #endif
};

//########################################### std::string ##########################################

/// Specialization of the type trait #"ArrayTraits" for type
/// <c>std::basic_string<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The construction from character arrays is defined to be allowed in explicit fashion only,
///   because \c std::string is a heavy-weight string type that will copy the data to an allocated
///   buffer.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy the concept #"ch IsCharacter".
template<typename TChar>
requires IsCharacter<TChar>
struct     ArrayTraits<std::basic_string<TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy                     Access                                             = Policy::Implicit;
    static constexpr Policy                     Construction                                       = Policy::ExplicitOnly;
    static constexpr const TChar*               Buffer   ( std::basic_string<TChar> const &  src ) { return          src.data  ()  ; }
    static constexpr integer                    Length   ( std::basic_string<TChar> const &  src ) { return integer( src.length() ); }
    static constexpr std::basic_string<TChar>   Construct( const TChar* array, integer length    ) { return std::basic_string<TChar>( array, size_t(length) ); }
  #endif
};

/// Specialization of the type trait #"ArrayTraits" for type
/// <c>std::basic_string<char8_t></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The construction from character arrays is defined to be allowed in explicit fashion only,
///   because \c std::string is a heavy-weight string type that will copy the data to an allocated
///   buffer.
template<>
struct     ArrayTraits<std::basic_string<char8_t>, nchar>
{
  #if !DOXYGEN
    static constexpr Policy             Access                                             = Policy::Implicit;
    static constexpr Policy             Construction                                       = Policy::ExplicitOnly;
    static           const nchar*       Buffer   ( std::basic_string<char8_t> const &  src ) { return reinterpret_cast<const nchar*>(src.data()); }
    static constexpr integer            Length   ( std::basic_string<char8_t> const &  src ) { return integer( src.length() ); }
    static std::basic_string<char8_t>   Construct( const nchar* array, integer length    )
    { return std::basic_string<char8_t>( reinterpret_cast<const char8_t*>(array), size_t(length) ); }
  #endif
};


/// Specialization of the type trait #"ZTArrayTraits" for type
/// <c>std::basic_string<TChar></c>:
/// - Zero-terminated character string data is allowed to be implicitly accessed, because the
///   type's buffer access method \c data() returns zero-terminated strings and is defined \c const.
/// - The type may be created from character array data in an explicit fashion only, because it is a
///   heavy-weight string type that will copy the data to an allocated buffer.
///
/// \note
///   In combination with classes #"^CString" and #"^AString", explicit creation is
///   suppressed using the type trait #"NoAutoCastTraits", because otherwise an ambiguity would
///   occur due to their ability to implicitly cast to <c>const char*</c>, which implicitly
///   constructs \c std::string in turn.
///   This leads to the bad situation that an explicit construction like this:
///
///             std::string stdString( cString );
///
///   uses the implicit cast to <c>const char*</c> and with that constructs the \c std::string.
///   This would be inefficient, as the length of the string has to be determined internally.
///
///   The most efficient way to create a \c std::string object from an object of type #"%CString"
///   or #"%AString" is to use the explicit constructor:
///
///             std::string stdString( String.Buffer(), String.Length() );
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy the concept #"ch IsCharacter".
template<typename TChar>
requires IsCharacter<TChar>
struct     ZTArrayTraits<std::basic_string<TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy                   Access                                             = Policy::Implicit;
    static constexpr Policy                   Construction                                       = Policy::ExplicitOnly;
    static constexpr const TChar*             Buffer   ( std::basic_string<TChar> const &  src ) { return          src.data  ()  ; }
    static constexpr integer                  Length   ( std::basic_string<TChar> const &  src ) { return integer( src.length() ); }
    static constexpr std::basic_string<TChar> Construct( const TChar* array, integer length    ) { return std::basic_string<TChar>( array, size_t(length) ); }
  #endif
};


//######################################## std::vector<char> #######################################
/// Specialization of the type trait #"ArrayTraits" for type <c>std::vector<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The construction from character arrays is defined to be allowed in explicit fashion only,
///   because \c std::vector is a heavy-weight type which will copy the data to an allocated
///   buffer.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy the concept #"ch IsCharacter".
template<typename TChar>
requires IsCharacter<TChar>
struct     ArrayTraits<std::vector<TChar>, TChar>
{
    #if !DOXYGEN
        static constexpr Policy              Access                                    = Policy::Implicit;
        static constexpr Policy              Construction                              = Policy::ExplicitOnly;
    static constexpr const TChar*        Buffer   (std::vector<TChar> const & src) { return          src.data()  ; }
    static constexpr integer             Length   (std::vector<TChar> const & src) { return integer( src.size() ); }
    static std::vector<TChar>            Construct(const TChar* array, integer length ) {
        std::vector<TChar> result;
        result.reserve( size_t(length) );
        const TChar* end= array + length;
        while( array < end )
            result.push_back( *array++ );
        return  result;
    }
    #endif
};

/// Specialization of the type trait #"ZTArrayTraits" for type
/// <c>std::vector<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The construction from zero-terminated character arrays is defined to be allowed in explicit
///   fashion only, because \c std::vector is a heavy-weight type which will copy the data to an
///   allocated buffer.<br>
///   Note that the zero-termination character is not included in the vector when created from
///   a zero-terminated character array. The length of the vector will have the lengh of the
///   source string.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy the concept #"ch IsCharacter".
template<typename TChar>
requires IsCharacter<TChar>
struct ZTArrayTraits<std::vector<TChar>, TChar >
{
  #if !DOXYGEN
    static constexpr Policy              Access                                    = Policy::Implicit;
    static constexpr Policy              Construction                              = Policy::ExplicitOnly;
    static constexpr const TChar*        Buffer   (std::vector<TChar> const & src) { return          src.data()  ; }
    static constexpr integer             Length   (std::vector<TChar> const & src) { return integer( src.size() ); }
    static constexpr std::vector<TChar>  Construct(const TChar* array, integer length ) {
        std::vector<TChar> result;
        result.reserve( size_t(length) );
        const TChar* end= array + length;
        while( array < end )
            result.push_back( *array++ );
        return  result;
    }
  #endif
};

//############################################ std::array ##########################################
/// Specialization of the type trait #"ArrayTraits" for type
/// <c>std::array<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The type may be implicitly created from character array data.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy the concept #"ch IsCharacter".
template<typename TChar, size_t TLength>
requires IsCharacter<TChar>
struct     ArrayTraits<std::array<TChar,TLength>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                            = Policy::Implicit;
    static constexpr Policy              Construction                                      = Policy::NONE;
    static constexpr const TChar*        Buffer   (std::array<TChar, TLength> const & src) { return          src.data  ()  ; }
    static constexpr integer             Length   (std::array<TChar, TLength> const & src) { return integer( src.size() ); }
  #endif
};


/// Specialization of the type trait #"ZTArrayTraits" for type
/// <c>std::array<TChar></c>:
/// - Zero-terminated string data is allowed to be explicitly accessed as usually data represented
///   by type \c std::string_view is not zero-terminated.
/// - The type may be implicitly created from zero-terminated character arrays.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy the concept #"ch IsCharacter".
template<typename TChar, size_t TLength>
requires IsCharacter<TChar>
struct   ZTArrayTraits<std::array<TChar, TLength>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                             = Policy::ExplicitOnly;
    static constexpr Policy              Construction                                       = Policy::NONE;
    static constexpr const TChar*        Buffer   (std::array<TChar, TLength> const & src ) { return          src.data  ()  ; }
    static constexpr integer             Length   (std::array<TChar, TLength> const & src ) { return integer( src.size() ); }
  #endif
};

//############################################ std::span ###########################################
/// Specialization of the type trait #"ArrayTraits" for type
/// <c>std::span<const TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The type may be implicitly created from character array data.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy the concept #"ch IsCharacter".
template<typename TChar>
requires IsCharacter<TChar>
struct     ArrayTraits<std::span<const TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                         = Policy::Implicit;
    static constexpr Policy              Construction                                   = Policy::Implicit;
    static constexpr const TChar*        Buffer   (std::span<const TChar> const & src)  { return          src.data  ()  ; }
    static constexpr integer             Length   (std::span<const TChar> const & src)  { return integer( src.size() ); }
    static std::span<const TChar>        Construct(const TChar* array, integer length ) { return std::span<const TChar>( array, size_t(length) ); }
  #endif
};


/// Specialization of the type trait #"ZTArrayTraits" for type
/// <c>std::span<const TChar></c>:
/// - Zero-terminated string data is allowed to be explicitly accessed as usually data represented
///   by type \c std::span is not zero-terminated.
/// - The type may be implicitly created from zero-terminated character arrays.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy the concept #"ch IsCharacter".
template<typename TChar>
requires IsCharacter<TChar>
struct   ZTArrayTraits<std::span<const TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                         = Policy::ExplicitOnly;
    static constexpr Policy              Construction                                   = Policy::Implicit;
    static constexpr const TChar*        Buffer   (std::span<const TChar> const & src ) { return          src.data  ()  ; }
    static constexpr integer             Length   (std::span<const TChar> const & src ) { return integer( src.size() ); }
    static std::span<const TChar> Construct(const TChar* array, integer length        ) { return std::span<const TChar>( array, size_t(length) ); }
  #endif
};

#if DOXYGEN
}} // namespace alib::characters[::compatibility::std]
#endif

} // namespace [alib::character]
