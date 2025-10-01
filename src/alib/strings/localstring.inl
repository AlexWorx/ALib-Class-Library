//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace strings {

//==================================================================================================
/// This type specializes class \b AString by adding a character array member of templated size
/// which is used as the "external" string buffer. The single motivation for the use of this
/// class is performance optimization.
///
/// Assembled character strings often have a rather short lifetime and are disposed quickly
/// after their usage. If the maximum length of the assembled string is known, such strings
/// may be created as local values of this type, with that maximum length given as template
/// parameter \p{TCapacity}. With that, the allocation and de-allocation of heap memory
/// (or whatever is used with the provided allocator) for the string is omitted and allocation
/// and destruction of the local string is performed in constant time O(1).
///
/// But even if an instance of this class is allocated (instead of being a local variable on the
/// stack), in common cases, still a performance improvement applies. This is due to the fact that
/// besides the class object itself, no second memory allocation for the string buffer is performed.
///
/// If during the use of an instance of this class the buffer's fixed \p{TCapacity} is exceeded,
/// a new buffer is allocated from the given allocator, just as it is always done with growing
/// strings managed in \b AString objects. This means, the use of this class is safe and no
/// restrictions apply.
/// Of course, if possible, for performance-critical code sections, the predefined size
/// \p{TCapacity} should be chosen large enough to allow the internal buffer to survive
/// the use.<br>
/// With debug-builds of \alib, parent class #AString optionally (and by default)
/// \ref alib_mod_assert "raises a warning" if an external buffer is replaced by a
/// new (heap) allocation. (Note that from an \b %AString perspective, this class's internal
/// field \b buffer is an external character array).
/// With use cases that do not allow foreseeing a maximum buffer size, the warning has to be
/// disabled by invoking \alib{strings::TAString;DbgDisableBufferReplacementWarning}.
///
/// For more information on warnings, see \alib{strings;TAString::SetBuffer;AString::SetBuffer}.
///
/// With the provision of an assignment operator, the explicit restriction of parent class to
/// \ref alib_ns_strings_astring_copymove "not allow assignments" is lifted with this type.
///
/// \note
///   For commonly used sizes and character types, some convenient typedefs exists in namespace
///   alib, which use \alib{lang;HeapAllocator}.
///   These type definitions are:
///   - #String8, #String16, #String32 ... #String1K, #String2K, #String4K,
///   - #NString8, #NString16, #NString32 ... #NString1K, #NString2K, #NString4K, and
///   - #WString8, #WString16, #WString32 ... #WString1K, #WString2K, #WString4K.
///
/// @tparam TChar The \ref alib_characters_chars "character type" of this string.
///   Alias names for specializations of this class are provided in namespace #alib with type
///   definitions \b LocalString<TCapacity>,
///   \b NLocalString<TCapacity>, \b WLocalString<TCapacity>,
///   \b XLocalString<TCapacity>, \b ComplementLocalString<TCapacity> and
///   \b StrangeLocalString<TCapacity>.
/// @tparam TCapacity  The capacity of the string buffer allocated locally "inside" the class.
/// @tparam TAllocator The allocator type to use. Defaults to \alib{lang;HeapAllocator}.
//==================================================================================================
template <typename TChar, integer TCapacity, typename TAllocator= lang::HeapAllocator>
class TLocalString : public TAString<TChar, TAllocator>
{
  // ###############################################################################################
  //  protected fields
  // ###############################################################################################
  protected:
    /// The base AString-type.
    using base=   TAString<TChar, TAllocator>;

    /// The base String-type.
    using sBase=  TString<TChar>;

    /// The internal buffer with size specified by the template parameter \p{TCapacity}.
    /// Passed as an external buffer to parent class \b AString.
    TChar         localBuffer[TCapacity];

  // ###############################################################################################
  //  Constructors/Destructor
  // ###############################################################################################
  public:
    //==============================================================================================
    /// Constructs an empty \b %LocalString. Inherited field \alib{strings::TString;buffer}
    /// will be set as an the external buffer of parent \b AString.
    /// Unlike all other \alib string classes, objects of this type are not \e nulled
    /// after default construction.
    /// @param pAllocator      The allocator to use.
    //==============================================================================================
    constexpr
    TLocalString(TAllocator& pAllocator)
    : base( pAllocator, localBuffer, TCapacity )
    , localBuffer {}
    {}

    //==============================================================================================
    /// Constructs an empty \b %LocalString. Inherited field \alib{strings::TString;buffer}
    /// will be set as an the external buffer of parent \b AString.
    /// Unlike all other \alib string classes, objects of this type are not \e nulled
    /// after default construction.
    //==============================================================================================
    constexpr
    TLocalString()
    : base( localBuffer, TCapacity )
    , localBuffer {}
    {}

    //==============================================================================================
    /// Copy constructor. Copies the string data of parameter \p{copy} to this instance
    /// @param copy The object to copy the contents from.
    //==============================================================================================
    TLocalString(const TLocalString& copy)
    : base( localBuffer, TCapacity )
    {
        ALIB_DBG( base::dbgWarnWhenExternalBufferIsReplaced= copy.dbgWarnWhenExternalBufferIsReplaced; )
        base::Append( copy.Buffer(), copy.Length() );
    }

    //==============================================================================================
    /// Move constructor.
    /// See \ref alib_ns_strings_astring_copymove "Copy/Move Constructor and Assignment"
    /// for details.
    /// @param move The object to move.
    //==============================================================================================
    TLocalString(TLocalString&& move)                                                   noexcept
    : base( localBuffer, TCapacity )
    {
        // given move object has external buffer: we have to copy
        if ( !move.HasInternalBuffer() )
        {
            ALIB_DBG( base::dbgWarnWhenExternalBufferIsReplaced= move.dbgWarnWhenExternalBufferIsReplaced; )
            base::Append( move );
            return;
        }

        // copy values
        sBase::buffer =  move.buffer;
        sBase::length =  move.length;
        base::capacity=  move.capacity;

        // clean moved object (buffer does not need to be nulled as AString destructor
        // checks capacity > 0  )
        move.capacity= 0;

        // in debug mode, more copying and more destructor prevention is needed
#if ALIB_DEBUG
        base::dbgWarnWhenExternalBufferIsReplaced= move.dbgWarnWhenExternalBufferIsReplaced;
#if ALIB_DEBUG_STRINGS
        base::debugLastAllocRequest  = move.debugLastAllocRequest;
        move.buffer                 = nullptr;
        move.length                 = 0;
#endif
#endif
    }

    //==============================================================================================
    /// Copy assign operator.
    /// Copies the contents of the given object \p{copy}.
    ///
    /// @param  copy  The object to copy the contents from.
    /// @return \c *this to allow concatenated calls.
    //==============================================================================================
    TLocalString& operator= (const TLocalString&  copy)
    {
        if ( copy.IsNull())
        {
            base::SetNull();
            return *this;
        }
        base::Reset();
        return static_cast<TLocalString&>( base::template Append<NC>( copy.Buffer(), copy.Length() ) );
    }

    //==============================================================================================
    /// Move assign operator.
    /// Copies the contents of the given object \p{copy}.
    ///
    /// @param  move  The object to move the contents from.
    /// @return \c *this to allow concatenated calls.
    //==============================================================================================
    TLocalString& operator= (TLocalString&&  move)
    {
        if( move.IsNull() )
        {
            ALIB_DBG( base::dbgWarnWhenExternalBufferIsReplaced= move.dbgWarnWhenExternalBufferIsReplaced; )
            base::SetNull();
            return *this;
        }

        // copy if move has its local buffer or this has lost its local buffer
        if ( !move.HasInternalBuffer() || base::HasInternalBuffer() )
        {
            base::Reset( move );
            return *this;
        }

        // copy other buffer from moved object
        sBase::buffer =  move.buffer;
        sBase::length =  move.length;
        base::capacity=  move.capacity;

        // clean moved object (buffer does not need to be nulled)
        move.capacity= 0;

        // in debug mode, more copying and more destructor prevention is needed
#if ALIB_DEBUG
        base::dbgWarnWhenExternalBufferIsReplaced= move.dbgWarnWhenExternalBufferIsReplaced;
#if ALIB_DEBUG_STRINGS
        base::debugLastAllocRequest  = move.debugLastAllocRequest;
        move.buffer                 = nullptr;
        move.length                 = 0;
#endif
#endif
        return *this;
    }

    //==============================================================================================
    /// Constructs this instance and invokes parent's method \b %Append to create a string
    /// representation of the given "appendable" object \p{src}.
    ///
    /// \see
    ///   Manual chapter \ref alib_strings_assembly_ttostring for more information
    ///   about which types are supported and how external, user-defined types can be made
    ///   compatible to this implicit constructor.
    ///
    /// @tparam TAppendable  The type of parameter \p{src} that has a specialization of
    ///                      functor \alib{strings;AppendableTraits}.
    /// @param src  The source to take the buffer and length from of template type T.
    //==============================================================================================
    template <typename TAppendable>
    TLocalString (const  TAppendable& src )
    : base( localBuffer, TCapacity )
    {
        base::Append( src );
    }


    //==============================================================================================
    /// Assign operator.
    /// Invokes inherited method \alib{strings::TAString;Reset(const TAppendable&)}.
    ///
    /// @tparam TAppendable The type of parameter \p{source}.
    /// @param  src         The source of type \p{TAppendable} to append.
    ///
    /// @return  \c *this to allow concatenated calls.
    //==============================================================================================
    template <typename TAppendable>
    TLocalString& operator= (const TAppendable& src )
    {
        base::Reset( src );
        return *this;
    }

}; // class TLocalString

// #################################################################################################
// Specializations of ArrayTraits for this class String
// #################################################################################################
} namespace characters {
#if !DOXYGEN
template<typename TChar, integer TCapacity, typename TAllocator>
struct  ArrayTraits<strings::TLocalString<TChar,TCapacity,TAllocator>, TChar>
{
    using T= strings::TLocalString<TChar,TCapacity,TAllocator>;
    static constexpr Policy       Access                                    = Policy::Implicit;
    static constexpr Policy       Construction                              = Policy::NONE;
    static constexpr const TChar* Buffer(const T& src)                      { return src.Buffer(); }
    static constexpr integer      Length(const T& src)                      { return src.Length(); }
};

template<typename TChar, integer TCapacity, typename TAllocator>
struct  ZTArrayTraits<strings::TLocalString<TChar,TCapacity,TAllocator>, TChar>
{
    using T= strings::TLocalString<TChar,TCapacity,TAllocator>;
    static constexpr Policy       Access                                 = Policy::Implicit;
    static constexpr Policy       Construction                           = Policy::NONE;
    static constexpr const TChar* Buffer(const T& src)                   { return src.Terminate(); }
    static constexpr integer      Length(const T& src)                   { return src.Length(); }
};
#endif // !DOXYGEN


} namespace strings {

#if ALIB_DEBUG
//==================================================================================================
/// This simple specialization of \alib{strings;TLocalString} disables the warning about
/// replacements of the internal buffer in debug-compilations. This may be used in situations,
/// where it is not possible to disable this warning after construction, for example if a local
/// string is <em>emplaced</em> in a container and extensions of it's local capacity are well
/// accepted (for a minority of the emplaced strings).
///
/// In release compilations, this type does not exist, but is replaced by a simple using
/// statement.
///
/// @tparam TChar      The character type.
/// @tparam TCapacity  The capacity of local, embedded string buffer.
/// @tparam TAllocator The allocator to use.
//==================================================================================================
template <typename TChar, integer TCapacity, typename TAllocator>
struct  TLocalStringNoWarning : public TLocalString<TChar,TCapacity,TAllocator>
{
    /// Default constructor.
    constexpr
    TLocalStringNoWarning()
    : TLocalString<TChar,TCapacity>()
    {
        TAString<TChar,lang::HeapAllocator>::dbgWarnWhenExternalBufferIsReplaced= false;
    }

    /// Constructor taking a string to copy.
    /// @param src  The string to copy into this object.
    TLocalStringNoWarning( const String& src )
    : TLocalString<TChar,TCapacity>()
    {
        TAString<TChar,lang::HeapAllocator>::dbgWarnWhenExternalBufferIsReplaced= false;
        TAString<TChar,lang::HeapAllocator>::Append( src.Buffer(), src.Length() );
    }
}; // TLocalStringNoWarning
#else
    template <typename TChar, integer TCapacity, typename TAllocator>
    using   TLocalStringNoWarning = TLocalString<TChar, TCapacity, TAllocator>;
#endif

}

/// Type alias in namespace \b alib.
template<integer TCapacity>
using LocalString       =     strings::TLocalString<character     , TCapacity, lang::HeapAllocator>;

/// Type alias in namespace \b alib.
template<integer TCapacity>
using ComplementLocalString=  strings::TLocalString<complementChar, TCapacity, lang::HeapAllocator>;

/// Type alias in namespace \b alib.
template<integer TCapacity>
using StrangeLocalString=     strings::TLocalString<strangeChar   , TCapacity, lang::HeapAllocator>;

/// Type alias in namespace \b alib.
template<integer TCapacity>
using NLocalString      =     strings::TLocalString <nchar        , TCapacity, lang::HeapAllocator>;

/// Type alias in namespace \b alib.
template<integer TCapacity>
using WLocalString      =     strings::TLocalString <wchar        , TCapacity, lang::HeapAllocator>;

/// Type alias in namespace \b alib.
template<integer TCapacity>
using XLocalString      =     strings::TLocalString <xchar        , TCapacity, lang::HeapAllocator>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<character,8>}.
using String8  =  LocalString<   8>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<character,16>}.
using String16 =  LocalString<  16>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<character,32>}.
using String32 =  LocalString<  32>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<character,64>}.
using String64 =  LocalString<  64>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<character,128>}.
using String128=  LocalString< 128>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<character,256>}.
using String256=  LocalString< 256>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<character,512>}.
using String512=  LocalString< 512>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<character,1024>}.
using String1K =  LocalString<1024>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<character,2048>}.
using String2K =  LocalString<2048>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<character,4096>}.
using String4K =  LocalString<4096>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<character,8192>}.
using String8K =  LocalString<8192>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<nchar,8>}.
using NString8 =  NLocalString<  8>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<nchar,16>}.
using NString16 =  NLocalString<  16>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<nchar,32>}.
using NString32 =  NLocalString<  32>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<nchar,64>}.
using NString64 =  NLocalString<  64>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<nchar,128>}.
using NString128=  NLocalString< 128>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<nchar,256>}.
using NString256=  NLocalString< 256>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<nchar,512>}.
using NString512=  NLocalString< 512>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<nchar,1024>}.
using NString1K =  NLocalString<1024>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<nchar,2048>}.
using NString2K =  NLocalString<2048>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<nchar,8192>}.
using NString4K =  NLocalString<4096>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<nchar,4096>}.
using NString8K =  NLocalString<8192>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<wchar,8>}.
using WString8  =  WLocalString<   8>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<wchar,16>}.
using WString16 =  WLocalString<  16>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<wchar,32>}.
using WString32 =  WLocalString<  32>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<wchar,64>}.
using WString64 =  WLocalString<  64>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<wchar,128>}.
using WString128=  WLocalString< 128>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<wchar,256>}.
using WString256=  WLocalString< 256>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<wchar,512>}.
using WString512=  WLocalString< 512>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<wchar,1024>}.
using WString1K =  WLocalString<1024>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<wchar,2048>}.
using WString2K =  WLocalString<2048>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<wchar,4096>}.
using WString4K =  WLocalString<4096>;

/// Type alias name for \alib{strings;TLocalString;TLocalString<wchar,8192>}.
using WString8K =  WLocalString<8192>;
} // namespace [alib::strings]
