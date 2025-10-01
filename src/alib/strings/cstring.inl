//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::strings {

template<typename TChar>  class  TCString;

ALIB_WARNINGS_IGNORE_DOCS


ALIB_WARNINGS_RESTORE

//==================================================================================================
/// This class specializes its base class \alib{strings;TString;String} in that respect that
/// the character strings represented are guaranteed to be zero-terminated.<br>
/// Zero-terminated strings are widely used by programming language \b C and are often called
/// "C-strings", what gave the class its name.
///
/// \see
///   For an introduction into the \alib string classes see this module's
///   \ref alib_mod_strings "Programmer's Manual".
///
/// @tparam TChar The \ref alib_characters_chars "character type" of this string.
//==================================================================================================
template<typename TChar>
class TCString : public TString<TChar>
{
  protected:
    /// Shortcut to the base type.
    using base= TString<TChar>;

  public:
    /// Defaulted default constructor. Leaves this instance \b uninitialized and undefined.
    constexpr TCString()                                                                  = default;

    /// Constructor accepting a pointer to a character array and a string length.
    /// \note
    ///   It is a user's responsibility to ensure that the character array provided includes
    ///   a terminating \c '\0' character.<br>
    ///   In debug-compilations a run-time assertion is raised if the provided buffer is not
    ///   zero-terminated.
    /// @param pBuffer         The buffer to use.
    /// @param contentLength   The length of the content in the given buffer.
    constexpr
    explicit
    TCString( const TChar* pBuffer, integer contentLength )
    : base( pBuffer, contentLength )
    {
        #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
        ALIB_ASSERT_ERROR(   base::IsNull()
                          || base::buffer[base::length] == '\0', "STRINGS",
             "Error: Explicit construction of CString with unterminated string."     )
        #endif
    }


    /// Constructor accepting \c nullptr. Constructs a nulled string.
    constexpr           TCString(lang::IsNullptr auto const &) noexcept : base( nullptr, 0 )  {}

    /// Templated \b implicit constructor accepting a \c const reference to an object of a
    /// type that satisfies the concept \alib{characters;IsImplicitZTArraySource}.
    ///
    /// Custom types can be enabled for this constructor by specializing the traits-type
    /// \alib{characters;ZTArrayTraits}, which is used for both;
    /// the implementation of the concept, and
    /// the implementation of this constructor itself.
    ///
    /// @tparam T    The type of the given \p{src}.
    ///              Requires satisfying the concept \alib{characters;IsImplicitZTArraySource}.
    ///              Deduced by the compiler.
    /// @param  src  The source of the string data to copy.
    template <typename T>
    requires alib::characters::IsImplicitZTArraySource<T, TChar>
    constexpr           TCString(const T& src)
    : base( characters::ZTArrayTraits<std::remove_cv_t<T>, TChar>::Buffer(  src ),
            characters::ZTArrayTraits<std::remove_cv_t<T>, TChar>::Length(  src ) )
    {
        #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
          ALIB_ASSERT_ERROR(   base::IsNull()
                            || base::buffer[base::length] == '\0', "STRINGS",
                "Error: Implicit construction of CString with unterminated string."     )
        #endif
    }

    /// Templated \b implicit constructor accepting a \c const pointer to an object of a
    /// type that satisfies the concept \alib{characters;IsImplicitZTArraySource}.
    ///
    /// Custom types can be enabled for this constructor by specializing the traits-type
    /// \alib{characters;ZTArrayTraits}, which is used for both;
    /// the implementation of the concept, and
    /// the implementation of this constructor itself.
    ///
    /// @tparam T    The type of the given \p{src}.
    ///              Requires satisfying the concept \alib{characters;IsImplicitZTArraySource}.
    ///              Deduced by the compiler.
    /// @param  src  A pointer to the source of the string data to copy.
    template <typename T>
    requires alib::characters::IsImplicitZTArraySource<T, TChar>
    constexpr           TCString(const T* src)
    : base( characters::ZTArrayTraits<std::remove_cv_t<T>, TChar>::Buffer(  *src ),
            characters::ZTArrayTraits<std::remove_cv_t<T>, TChar>::Length(  *src ) )
    {
        ALIB_ASSERT_ERROR(   base::IsNull()
                          || base::buffer[base::length] == '\0', "STRINGS",
             "Error: Implicit construction of CString with unterminated string."  )
    }

    /// Templated \b explicit constructor accepting a \c const reference to an object of a
    /// type that satisfies the concept \alib{characters;IsExplicitZTArraySource}.
    ///
    /// Custom types can be enabled for this constructor by specializing the traits-type
    /// \alib{characters;ZTArrayTraits}, which is used for both;
    /// the implementation of the concept, and
    /// the implementation of this constructor itself.
    ///
    /// @tparam T    The type of the given \p{src}.
    ///              Requires satisfying the concept \alib{characters;IsExplicitZTArraySource}.
    ///              Deduced by the compiler.
    /// @param  src  The source of the string data to copy.
    template <typename T>
    requires alib::characters::IsExplicitZTArraySource<T, TChar>
    constexpr explicit  TCString(T& src)
    : base( characters::ZTArrayTraits<std::remove_cv_t<T>,TChar>::Buffer( src ),
            characters::ZTArrayTraits<std::remove_cv_t<T>,TChar>::Length( src ) )
    {
        #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
        ALIB_ASSERT_ERROR(   base::IsNull()
                          || base::buffer[base::length] == '\0', "STRINGS",
             "Error: Explicit construction of CString with unterminated string."     )
        #endif
    }

    /// Templated \b explicit constructor accepting a \c const pointer to an object of a
    /// type that satisfies the concept \alib{characters;IsExplicitZTArraySource}.
    ///
    /// Custom types can be enabled for this constructor by specializing the traits-type
    /// \alib{characters;ZTArrayTraits}, which is used for both;
    /// the implementation of the concept, and
    /// the implementation of this constructor itself.
    ///
    /// @tparam T    The type of the given \p{src}.
    ///              Requires satisfying the concept \alib{characters;IsExplicitZTArraySource}.
    ///              Deduced by the compiler.
    /// @param  src  A pointer to the source of the string data to copy.
    template <typename T>
    requires alib::characters::IsExplicitZTArraySource<T, TChar>
    constexpr explicit  TCString(T* src)
    : base( characters::ZTArrayTraits<std::remove_cv_t<T>,TChar>::Buffer( *src ),
            characters::ZTArrayTraits<std::remove_cv_t<T>,TChar>::Length( *src ) )
    {
        #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
        ALIB_ASSERT_ERROR(   base::IsNull()
                          || base::buffer[base::length] == '\0', "STRINGS",
             "Error: Explicit construction of CString with unterminated string."     )
        #endif
    }

    /// Templated \b explicit constructor accepting a \b mutable reference to an object of a
    /// type that satisfies the concept \alib{characters;IsMutableZTArraySource}.
    ///
    /// Custom types can be enabled for this constructor by specializing the traits-type
    /// \alib{characters;ZTArrayTraits}, which is used for both;
    /// the implementation of the concept, and
    /// the implementation of this constructor itself.
    ///
    /// @tparam T    The type of the given \p{src}.
    ///              Requires satisfying the concept \alib{characters;IsMutableZTArraySource}.
    ///              Deduced by the compiler.
    /// @param  src  The source of the string data to copy.
    template <typename T>
    requires alib::characters::IsMutableZTArraySource<T, TChar>
    constexpr explicit  TCString(T& src)
    : base( characters::ZTArrayTraits<std::remove_cv_t<T>,TChar>::Buffer( const_cast<T&>( src ) ),
            characters::ZTArrayTraits<std::remove_cv_t<T>,TChar>::Length( const_cast<T&>( src ) ) )
    {
        #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
        ALIB_ASSERT_ERROR(   base::IsNull()
                          || base::buffer[base::length] == '\0', "STRINGS",
             "Error: Construction of CString (from mutable object) with unterminated string." )
        #endif
    }

    /// Templated \b explicit constructor accepting a \b mutable pointer to an object of a
    /// type that satisfies the concept \alib{characters;IsMutableZTArraySource}.
    ///
    /// Custom types can be enabled for this constructor by specializing the traits-type
    /// \alib{characters;ZTArrayTraits}, which is used for both;
    /// the implementation of the concept, and
    /// the implementation of this constructor itself.
    ///
    /// @tparam T    The type of the given \p{src}.
    ///              Requires satisfying the concept \alib{characters;IsMutableZTArraySource}.
    ///              Deduced by the compiler.
    /// @param  src  A pointer to the source of the string data to copy.
    template <typename T>
    requires alib::characters::IsMutableZTArraySource<T, TChar>
    constexpr explicit  TCString(T* src)
    : base( characters::ZTArrayTraits<std::remove_cv_t<T>,TChar>::Buffer( *src ),
            characters::ZTArrayTraits<std::remove_cv_t<T>,TChar>::Length( *src ) )
    {
        #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
        ALIB_ASSERT_ERROR(   base::IsNull()
                          || base::buffer[base::length] == '\0', "STRINGS",
             "Error: Construction of CString (from mutable object) with unterminated string." )
        #endif
    }

    /// Constructor which allocates memory including an extra character for zero-termination,
    /// copies the given string's contents and lets this \b %CString represent this new
    /// zero-terminated character array.<br>
    /// Note that it is up to the using code to duly deallocate the memory, because the
    /// destructor of this type does not do so.
    ///
    /// \see
    ///   Methods #Allocate and #Free for information how to use allocated string objects.
    ///
    /// @tparam TAllocator The type of the given \p{allocator}, as prototyped with
    ///                    \alib{lang;Allocator}. Deduced by the compiler.
    /// @param  allocator  The allocator to use.
    /// @param  copy       The string to copy to the new memory allocated.
    template<typename TAllocator>
    requires alib::lang::IsAllocator<TAllocator>
    TCString( TAllocator& allocator, const TString<TChar>& copy )
    {
        if( (base::length= copy.Length()) == 0 )
        {
            base::buffer= copy.Buffer();
            return;
        }

        auto* newBuf=  allocator().template AllocArray<TChar>( copy.Length() + 1);
        copy.CopyTo( newBuf );
        newBuf[base::length]= '\0';
        base::buffer= newBuf;
    }

    // ##############################    casting  back    ######################################
    /// Templated \b implicit <c>cast operator</c> constructing an instance of type \p{T} from
    /// this string instance.
    /// This operator requires the type \p{T} satisfying the concept
    /// \alib{characters;IsImplicitZTArrayCast}.
    ///
    /// Custom types can be enabled for this operator by specializing the traits-type
    /// \alib{characters;ArrayTraits}, which is used for both;
    /// the implementation of the concept, and the implementation of this operator itself.
    ///
    /// @tparam T    The type to implicitly cast this instance to.
    ///              Requires satisfying the concept \alib{characters;IsImplicitZTArrayCast}.
    ///              Deduced by the compiler.
    /// @return A value of type \p{T}.
    template< typename T>
    requires  (     alib::characters::IsImplicitArrayCast<T, TChar>
                && !alib::strings::NoAutoCastTraits< TCString<TChar>,
                                                     characters::Policy::Implicit,
                                                     std::remove_cv_t<T>  >::value )

    constexpr           operator T()                                                           const
    { return characters::ArrayTraits<T,TChar>::Construct( base::buffer, base::length ); }

    /// Templated \b explicit <c>cast operator</c> constructing an instance of type \p{T} from
    /// this string instance.
    /// This operator requires the type \p{T} satisfying the concept
    /// \alib{characters;IsExplicitZTArrayCast}.
    ///
    /// Custom types can be enabled for this operator by specializing the traits-type
    /// \alib{characters;ArrayTraits}, which is used for both;
    /// the implementation of the concept, and
    /// the implementation of this operator itself.
    ///
    /// @tparam T    The type to implicitly cast this instance to.
    ///              Deduced by the compiler.
    /// @return A value of type \p{T}.
    template< typename T>
    requires (     alib::characters::IsExplicitArrayCast<T, TChar>
               && !alib::characters::IsImplicitZTArrayCast<T, TChar>
               && !alib::characters::IsExplicitZTArrayCast<T, TChar>
               && !alib::strings::NoAutoCastTraits< TCString<TChar>,
                                                    characters::Policy::ExplicitOnly,
                                                    std::remove_cv_t<T>       >::value )

    constexpr explicit  operator T()                                                           const
    { return characters::ArrayTraits<T,TChar>::Construct( base::buffer, base::length ); }


    /// Templated \b implicit <c>cast operator</c> constructing an instance of type \p{T} from
    /// this string instance.
    /// This operator requires the type \p{T} satisfying the concept
    /// \alib{characters;IsImplicitZTArrayCast}.
    ///
    /// Custom types can be enabled for this operator by specializing the traits-type
    /// \alib{characters;ZTArrayTraits}, which is used for both;
    /// the implementation of the concept, and
    /// the implementation of this operator itself.
    ///
    /// @tparam T    The type to implicitly cast this instance to.
    ///              Requires satisfying the concept \alib{characters;IsImplicitZTArrayCast}.
    ///              Deduced by the compiler.
    /// @return A value of type \p{T}.
    template< typename T>
    requires (    !alib::characters::IsImplicitArrayCast  <T, TChar>
               &&  alib::characters::IsImplicitZTArrayCast<T, TChar>
               && !alib::strings::NoAutoCastTraits< TCString<TChar>,
                                                    characters::Policy::Implicit,
                                                    std::remove_cv_t<T>       >::value )
                                                    
    constexpr           operator T()                                                           const
    { return characters::ZTArrayTraits<T,TChar>::Construct( base::buffer, base::length ); }


    /// Templated \b explicit <c>cast operator</c> constructing an instance of type \p{T} from
    /// this string instance.
    /// This operator requires the type \p{T} satisfying the concept
    /// \alib{characters;IsExplicitZTArrayCast}.
    ///
    /// Custom types can be enabled for this operator by specializing the traits-type
    /// \alib{characters;ZTArrayTraits}, which is used for both;
    /// the implementation of the concept, and
    /// the implementation of this operator itself.
    ///
    /// @tparam T    The type to explicitly cast this instance to.
    ///              Deduced by the compiler.
    /// @return A value of type \p{T}.
    template< typename T>
    requires (     alib::characters::IsExplicitZTArrayCast<T, TChar>
               && !alib::characters::IsImplicitZTArrayCast<T, TChar>
               && !alib::strings::NoAutoCastTraits< TCString<TChar>,
                                                    characters::Policy::ExplicitOnly,
                                                    std::remove_cv_t<T>       >::value )
    constexpr explicit  operator T()                                                           const
    { return characters::ZTArrayTraits<T,TChar>::Construct( base::buffer, base::length ); }


    //==============================================================================================
    /// Reads a character at a given index.<br> Overrides
    /// \alib{strings;TString::operator[];String::operator[]} to change the debug assertion
    /// to allow inclusion of the termination character.
    ///
    /// \attention
    ///   No parameter check is performed (other than an assertions in debug-compilation of
    ///   \alib). See \alib{strings;TString::operator[];String::operator[]} for details.
    ///
    /// @param   op    The index of the character within this object's buffer.
    /// @returns If the character contained at index \p{op}.
    //==============================================================================================
     TChar    operator[] (integer  op) const
     {
        ALIB_ASSERT_ERROR( op >= 0  && op <= base::length, "STRINGS",
                           "Index out of bounds: 0 <= {} < {}.", op, base::length )
        return base::buffer[op];
     }

    //==============================================================================================
    /// Returns the index of the first character which is included, respectively <em>not</em>
    /// included in a given set of characters.
    ///
    /// This method searches forwards. For backwards search, see
    /// \alib{strings;TString::LastIndexOfAny;String::LastIndexOfAny}.
    ///
    /// \note
    ///   This method overrides method \alib{strings;TString::IndexOfAny;String::IndexOf}.
    ///   This implementation however expects a \b %CString with parameter \p{needles}
    ///   (beside the fact that it has to be invoked on a \b %CString itself).
    ///   If no zero-terminated needle string is available, the parent's original method
    ///   needs to be invoked. This has to be done by explicitly naming
    ///   the parent class in the invocation, like for example in
    ///
    ///          myCString.TString::IndexOfAny<Inclusion::Include>( myString );
    ///
    /// \note
    ///   On most platforms, this zero-terminated version should perform slightly faster than
    ///   the original method in class \b %String.
    ///
    /// @tparam TCheck     Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                    If \c <false\> is added to the method name, no parameter checks are
    ///                    performed and the needles must not be empty.
    /// @tparam TInclusion Denotes whether the search returns the first index that holds a value
    ///                    that is included or that is not excluded in the set of needle
    ///                    characters.
    /// @param needles     Set of characters to be taken into account.
    /// @param startIdx    The index to start the search at. If the given value is less than \c 0,
    ///                    it is set to \c 0. If it exceeds the length of the string, the length of
    ///                    the string is returned.
    ///                    Defaults to \c 0.
    ///
    /// @return The index of the first character found which is included, respectively not
    ///         included, in the given set of characters. If nothing is found, \c -1 is returned.
    //==============================================================================================
    template <lang::Inclusion  TInclusion,
              typename         TCheck= CHK >
    integer  IndexOfAny( const TCString& needles, integer startIdx= 0 )
    const
    {
        if (TCheck::value)
        {
            if ( startIdx < 0                          ) startIdx= 0;
            if ( startIdx >= base::length ) return   -1;
        }
        else
        {
            ALIB_ASSERT_ERROR(    startIdx >= 0
                               && startIdx < base::length
                               && needles.Length() != 0, "STRINGS",
                  "Non checking and illegal parameters: 0 <= {} < {}, needles: {}",
                  startIdx, base::length, needles.Length() )
        }


        if constexpr ( TInclusion == lang::Inclusion::Include )
        {
            integer idx=  characters::IndexOfAnyIncludedZT<TChar>(base::buffer + startIdx, needles);
            return  idx >= 0 ? idx + startIdx : -1;
        }
        else
        {
            const TChar* haystack= base::buffer + startIdx;
            integer idx=  characters::IndexOfAnyExcludedZT<TChar>( haystack, needles );
            return idx < 0 || *( haystack + idx ) == '\0' ? -1 : startIdx + idx;
        }
    }

    //==============================================================================================
    /// Sets this object to a zero-terminated copy of the given string, allocated in
    /// given \p{allocator}.
    ///
    /// \note
    ///   In case given \p{copy} is empty or nulled, no allocation is performed and this string
    ///   is set to empty. Still the pointer to the buffer is copied. Thus, this string
    ///   behaves in respect to method #IsNull the same as given string \p{copy}.
    ///
    /// @tparam TAllocator The type of the given \p{allocator}, as prototyped with
    ///                    \alib{lang;Allocator}. Deduced by the compiler.
    /// @param  allocator  The allocator to use.
    /// @param  copy       The string to copy to the new memory allocated.
    //==============================================================================================
    template<typename TAllocator>
    requires alib::lang::IsAllocator<TAllocator>
    void  Allocate( TAllocator& allocator, const TString<TChar>& copy )
    {
        if( (base::length= copy.Length()) == 0 )
        {
            base::buffer= copy.Buffer();
            return;
        }

        auto* newBuf=  allocator().template AllocArray<TChar>( base::length + 1);
        copy.CopyTo( newBuf );
        newBuf[base::length]= '\0';
        base::buffer= newBuf;
    }

    /// Deallocates this String's memory in \p{allocator} and sets this instance to \e nulled.
    ///
    /// @see Notes in the similar method \alib{strings;TString::Free} of the base class, which
    ///      apply with this override likewise.
    ///
    /// @tparam TAllocator The type of the given \p{allocator}, as prototyped with
    ///                    \alib{lang;Allocator}. Deduced by the compiler.
    /// @param  allocator  The allocator to use.
    template<typename TAllocator>
    requires alib::lang::IsAllocator<TAllocator>
    void  Free( TAllocator& allocator )                                                        const
    {
        if( base::length == 0 || base::buffer == nullptr )
            return;
        allocator().FreeArray( base::buffer, base::length + 1 );
    }
}; // class TCString

// #################################################################################################
// Specializations of ArrayTraits for this class CString
// #################################################################################################
} ALIB_EXPORT namespace alib::characters {
#if !DOXYGEN
template<typename TChar>    struct  ZTArrayTraits<strings::TCString<TChar>, TChar>
{
    using T= strings::TCString<TChar>;
    static constexpr Policy       Access                                      = Policy::Implicit;
    static constexpr Policy       Construction                                = Policy::ExplicitOnly;
    static constexpr const TChar* Buffer(const T& src) { return src.Buffer(); }
    static constexpr integer      Length(const T& src) { return src.Length(); }
    static constexpr T            Construct(const TChar* b, integer l ) { return T(b, l); }
};

template<typename TChar>    struct  ArrayTraits<strings::TCString<TChar>, TChar>
{
    using T= strings::TCString<TChar>;
    static constexpr Policy       Access                                      = Policy::Implicit;
    static constexpr Policy       Construction                                = Policy::ExplicitOnly;
    static constexpr const TChar* Buffer(const T& src) { return src.Buffer(); }
    static constexpr integer      Length(const T& src) { return src.Length(); }

    static constexpr T            Construct(const TChar* b, integer l){ return T(b, l); }
};
#endif // !DOXYGEN

} ALIB_EXPORT namespace alib {

/// Type alias in namespace \b alib.
using  CString          =     strings::TCString   <character>;

/// Type alias in namespace \b alib.
using  ComplementCString=     strings::TCString   <complementChar>;

/// Type alias in namespace \b alib.
using  StrangeCString   =     strings::TCString   <strangeChar>;

/// Type alias in namespace \b alib.
using  NCString         =     strings::TCString   <nchar>;

/// Type alias in namespace \b alib.
using  WCString         =     strings::TCString   <wchar>;

/// Type alias in namespace \b alib.
using  XCString         =     strings::TCString   <xchar>;


} ALIB_EXPORT namespace alib::strings {


//==================================================================================================
/// This template class has three specializations for types \alib{characters;nchar},
/// \alib{characters;wchar}, and \alib{characters;xchar}, which each provide the following
/// static \c constexpr methods:
/// - #EmptyString,
/// - #NewLine, and
/// - #DefaultWhitespaces.
///
/// The class is useful to implement methods that are templated with the character type they use.
///
/// \note
///   In non-templated code (that works with fixed or logical character sizes), it might lead to
///   better readability if the following shortcut/alias functions of namespace #alib are used:
///   - \ref EMPTY_CSTRING, \ref EMPTY_COMPLEMENT_CSTRING, \ref EMPTY_STRANGE_CSTRING,
///     \ref EMPTY_NCSTRING, \ref EMPTY_WCSTRING, \ref EMPTY_XCSTRING,
///   - \ref NEW_LINE, \ref COMPLEMENT_NEW_LINE, \ref STRANGE_NEW_LINE,
///     \ref NNEW_LINE, \ref WNEW_LINE, \ref XNEW_LINE and
///   - \ref DEFAULT_WHITESPACES, \ref COMPLEMENT_DEFAULT_WHITESPACES,
///     \ref STRANGE_DEFAULT_WHITESPACES, \ref NDEFAULT_WHITESPACES, \ref WDEFAULT_WHITESPACES,
///     \ref XDEFAULT_WHITESPACES.
///  <p>
///
/// \note
///   Constants for \e nulled strings are given with
///   \ref NULL_STRING, \ref NULL_COMPLEMENT_STRING, \ref NULL_STRANGE_STRING,
///   \ref NULL_NSTRING, \ref NULL_WSTRING, and \ref NULL_XSTRING. However, those are returning
///   type \b String instead of \b CString. For \e nulled objects of type \b CString use the
///   keyword \c nullptr which performs implicit constexpr creation.
///
/// \see
///   See also manual chapter \ref alib_strings_details_constants.
///
/// @tparam TChar The \ref alib_characters_chars "character type".
//==================================================================================================
template<typename TChar>
struct CStringConstantsTraits
{
    #if DOXYGEN
    /// @return A \e zero-terminated empty string.
    constexpr inline static CString<TChar>     EmptyString();

    /// On Windows OS, the returned string contains characters <c>'\\r'</c> and <c>'\\n'</c>, on
    /// other platforms just character <c>'\\n'</c>.
    /// @return A \e zero-terminated string containing platform-dependent "newline" charcater(s).
    constexpr inline static CString<TChar>     NewLine;

    /// @return A zero-terminated string containing default whitespace characters "space", "newline"
   /// "carriage return" and "tabulator", hence <c>" \n\r\t"</c>.
    constexpr inline static CString<TChar>     DefaultWhitespaces());
    #endif
};


#if !DOXYGEN

template<> struct CStringConstantsTraits<nchar>
{
    constexpr static NCString  EmptyString()        { return         ""        ; }

  #if defined(_WIN32)
    constexpr static NCString  NewLine()            { return         "\r\n"    ; }
  #else
    constexpr static NCString  NewLine()            { return         "\n"      ; }
  #endif

    constexpr static NCString  DefaultWhitespaces() { return         " \n\r\t" ; }
};

template<> struct CStringConstantsTraits<wchar>
{
    constexpr static WCString  EmptyString()        { return A_WCHAR(""       ); }

  #if defined(_WIN32)
    constexpr static WCString  NewLine()            { return A_WCHAR("\r\n"   ); }
  #else
    constexpr static WCString  NewLine()            { return A_WCHAR("\n"     ); }
  #endif

    constexpr static WCString  DefaultWhitespaces() { return A_WCHAR(" \n\r\t"); }
};

template<> struct CStringConstantsTraits<xchar>
{
    constexpr static XCString  EmptyString()        { return A_XCHAR(""       ); }

  #if defined(_WIN32)
    constexpr static XCString  NewLine()            { return A_XCHAR("\r\n"   ); }
  #else
    constexpr static XCString  NewLine()            { return A_XCHAR("\n"     ); }
  #endif

    constexpr static XCString  DefaultWhitespaces() { return A_XCHAR(" \n\r\t"); }
};

#endif  //!DOXYGEN

} ALIB_EXPORT namespace alib {


/// A zero-terminated, empty string.
inline constexpr CString             EMPTY_CSTRING          = strings::CStringConstantsTraits<character>::EmptyString();

/// A zero-terminated, empty string.
inline constexpr ComplementCString   EMPTY_COMPLEMENT_CSTRING= strings::CStringConstantsTraits<complementChar>::EmptyString();

/// A zero-terminated, empty string.
inline constexpr StrangeCString      EMPTY_STRANGE_CSTRING   = strings::CStringConstantsTraits<strangeChar>::EmptyString();

/// A zero-terminated, empty string.
inline constexpr NCString            EMPTY_NCSTRING         = strings::CStringConstantsTraits<nchar    >::EmptyString();

/// A zero-terminated, empty string.
inline constexpr WCString            EMPTY_WCSTRING         = strings::CStringConstantsTraits<wchar    >::EmptyString();

/// A zero-terminated, empty string.
inline constexpr XCString            EMPTY_XCSTRING         = strings::CStringConstantsTraits<xchar    >::EmptyString();



/// A zero-terminated string containing the new-line character sequence.
inline constexpr  CString            NEW_LINE           =  strings::CStringConstantsTraits<character     >::NewLine();

/// A zero-terminated string containing the new-line character sequence.
inline constexpr  ComplementCString  COMPLEMENT_NEW_LINE=  strings::CStringConstantsTraits<complementChar>::NewLine();

/// A zero-terminated string containing the new-line character sequence.
inline constexpr  StrangeCString     STRANGE_NEW_LINE   =  strings::CStringConstantsTraits<strangeChar   >::NewLine();

/// A zero-terminated string containing the new-line character sequence.
inline constexpr NCString            NNEW_LINE          =  strings::CStringConstantsTraits<nchar         >::NewLine();

/// A zero-terminated string containing the new-line character sequence.
inline constexpr WCString            WNEW_LINE          =  strings::CStringConstantsTraits<wchar         >::NewLine();

/// A zero-terminated string containing the new-line character sequence.
inline constexpr XCString            XNEW_LINE          =  strings::CStringConstantsTraits<xchar         >::NewLine();



/// A zero-terminated string of default whitespace characters.
inline constexpr  CString            DEFAULT_WHITESPACES           =  strings::CStringConstantsTraits<character     >::DefaultWhitespaces();

/// A zero-terminated string of default whitespace characters.
inline constexpr  ComplementCString  COMPLEMENT_DEFAULT_WHITESPACES =  strings::CStringConstantsTraits<complementChar>::DefaultWhitespaces();

/// A zero-terminated string of default whitespace characters.
inline constexpr  StrangeCString     STRANGE_DEFAULT_WHITESPACES    =  strings::CStringConstantsTraits<strangeChar   >::DefaultWhitespaces();

/// A zero-terminated string of default whitespace characters.
inline constexpr NCString            NDEFAULT_WHITESPACES          =  strings::CStringConstantsTraits<nchar         >::DefaultWhitespaces();

/// A zero-terminated string of default whitespace characters.
inline constexpr WCString            WDEFAULT_WHITESPACES          =  strings::CStringConstantsTraits<wchar         >::DefaultWhitespaces();

/// A zero-terminated string of default whitespace characters.
inline constexpr XCString            XDEFAULT_WHITESPACES          =  strings::CStringConstantsTraits<xchar         >::DefaultWhitespaces();


} // namespace [alib]


// Note(25/01/17):
//   Clang strangely did not find the following templated operators when they resided in an
//   exported namespace.
//   The workaround was to not export the namespace but export each operator instead.
//   We think this is wrong behavior and not aligned with the language specification.
#if !DOXYGEN
namespace alib::strings {

ALIB_EXPORT
template<typename TChar>
bool operator== (const TCString<TChar>& lhs, const TCString<TChar>& rhs)
{ return  lhs. template Equals   <CHK, lang::Case::Sensitive>(rhs); }

ALIB_EXPORT
template<typename TChar, typename T>
requires (!std::is_same_v<T, TCString<TChar>>)
bool operator== (const TCString<TChar>& lhs, const T&     rhs)
{ return  lhs. template Equals   <CHK, lang::Case::Sensitive>(rhs); }

ALIB_EXPORT
template<typename TChar>
auto operator<=> (const TCString<TChar>& lhs, const TCString<TChar>& rhs)
{ return  lhs. template CompareTo<CHK, lang::Case::Sensitive>(rhs); }

ALIB_EXPORT
template<typename TChar, typename T>
requires (!std::same_as<TCString<TChar>, T>)
auto operator<=> (const TCString<TChar>& lhs, const T&     rhs)
{ return  lhs. template CompareTo<CHK, lang::Case::Sensitive>(rhs); }

} // namespace [alib::strings]
#endif // !DOXYGEN

