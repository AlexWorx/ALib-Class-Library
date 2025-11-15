//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace strings {

//==================================================================================================
/// This is a type-traits functor that allows making custom types "appendable" to objects of
/// type \b AString.
///
/// Specializations of this struct have to implement
/// \alib{strings;AppendableTraits::operator()(TAString<TChar>&,const TAppendable&);operator()},
/// which is invoked by method \alib{strings;TAString::Append;AString::Append}, when an instance
/// of the type in question is passed.
///
/// For user-defined string-types that get adopted to \alib string system using a specialization of
/// struct \alib{characters;ArrayTraits}, no specialization of this functor is needed, because
/// the method \b AString::Append will accept such types likewise.
///
/// \note
///   This struct is called a "functor" because its single member is
///   \alib{strings;AppendableTraits::operator()(TAString<TChar>&,const TAppendable&);operator()}.
///   This is different to other type traits defined by \alib, which demand to implement
///   one or more static methods with specializations. As
///   \alib{strings;AppendableTraits::operator()(TAString<TChar>&,const TAppendable&);operator()}
///   must not be static, the internal call to this operator is done by creating a temporary object
///   of this "functor struct"  and calling the operator on that.<br>
///   The result in respect to the generated code is with both approaches the same. Using a functor
///   here, is just a design decision.
///
/// \see
///   For details consult chapter \ref alib_strings_assembly_ttostring of the Programmer's Manual
///   of module \alib_strings.
///
/// \see
///   The documentation of built-in specializations for \alib types are collected in
///   sub-namespace \ref alib::strings::APPENDABLES.
///
/// # Reference Documentation #
/// @tparam  TAppendable The type that should be made compatible with method
///                      \alib{strings::TAString;Append(const TAppendable&)}.
/// @tparam  TChar       The character type of the target \b AString.
/// @tparam  TAllocator  The allocator type of the target \b AString.
//==================================================================================================
template< typename TAppendable,
          typename TChar,
          typename TAllocator >
struct AppendableTraits
{
    #if DOXYGEN
    //==============================================================================================
    /// This operator is invoked on a temporary object of this type by
    /// \alib{strings;TAString::Append(const TAppendable&)}, when an object of type
    /// \p{TAppendable} is passed.
    ///
    /// Usually, specializations of this operator append a string representation of \p{src} to
    /// \p{target}. Special "appendable types" might modify \p{target} in other, arbitrary ways.
    ///
    /// @param   target The target string.
    /// @param   src    The source object.
    //==============================================================================================
    void operator()( TAString<TChar>& target, const TAppendable& src );

    #endif // doxygen
};

/// This concept tests whether for type \p{T}, a specialization of the type trait
/// \alib{strings;AppendableTraits} exists, which has a valid call-<c>operator()</c> that accepts
/// a target string and a \c const reference to an instance of \p{T}.
template<typename T, typename TChar, typename TAllocator>
concept IsAppendable = requires(TAString<TChar, TAllocator>& target,  const T& value) {
    { AppendableTraits<T, TChar, TAllocator>()(target, value) } -> std::same_as<void>;
};

//==================================================================================================
/// Specializes base class \alib{strings;TString;String} to implement mutable character strings
/// using writable and extendable buffer memory.
///
/// <b>Construction:</b><br>
/// Construction is described in Programmer's Manual, section
/// \ref alib_strings_cc_construction_astring.
///
/// <b>Buffer Management:</b><br>
/// There are two possible types of buffers:
/// - <b>Internal buffers</b><br>
///   This is the standard case and implements a buffer that is allocated and freed using
///   the allocator type-traits given with template parameter \p{TAllocator}.
///   Allocation size eventually grows over time and never shrinks, unless explicitly demanded by
///   the using code. Those buffers are deleted when objects of this type are deleted.
/// - <b>External buffers</b><br>
///   Set with overloaded method #SetBuffer(TChar*, integer, integer, lang::Responsibility).
///   External buffers are not managed by this class. However, if their capacity is exceeded, they
///   will automatically become replaced by an internal buffer.
///   Such a replacement, by default, \ref alib_mod_assert "raises a warning" with debug-builds.
///   In situations that buffer replacement is accepted, warnings can be disabled with the method
///   #DbgDisableBufferReplacementWarning.
///
/// Method #SetBuffer provides a boolean parameter that also allows leting an object of this
/// class take control on a buffer provided from outside. In this case, the buffer is considered an
/// internal buffer, hence allocated using the same allocator traits (and if not heap allocation
/// than also the same allocator instance!), rather than an external one.
///
/// The default constructor creates a \e nulled \b AString which does not dispose of an allocated
/// buffer, yet. Destruction will free the currently allocated buffer - if internal.
///
/// \anchor alib_ns_strings_astring_copymove
/// <b>Copy/Move Constructor and Assignment</b><br>
/// The class provides the minimum equipment to be usable as member type of standard containers like
/// \c std::vector. This includes a copy and move constructor as well as a copy assignment operator.
/// Nevertheless, this class is not guaranteed to perform well when used with container types
/// and such use should be avoided if possible.<br>
/// Outside of containers, the automatic C++ copy and move construction semantics should be avoided.
/// For example, the move constructor, grabs the buffer of a given movable \b %AString, as long
/// as this given object does not use an external buffer. If it does, the contents of the
/// movable object is copied like in the copy constructor.
///
/// Consequently, objects of this class should have a well defined scope and not be copied and moved like
/// the lightweight string-types. A move assignment operator is not given. The rationale for this
/// design decision is that usually, a "more temporary" string would be assigned to a "less temporary"
/// one. In this case, it would not be helpful to replace the already allocated storage of the
/// assignee.
///
/// By the same token, besides the copy assignment, no other assignment operators are given. Instead,
/// method #Reset(const TAppendable&) is to be used to clear the existing buffer and insert new
/// string content. This helps to distinguish \b AString variables from those of the lightweight
/// string-types as shown in the following snippet:
///
///        string1= "Hello";       // Can't be an AString. Rather String, Substring or CString
///        string2.Reset("World"); // Obviously an AString. The given string is copied!
///
/// \anchor alib_ns_strings_astring_write_access
/// <b>Writing directly into the Buffer:</b><br>
/// Parent class \alib{strings;TString;String} holds its protected field
/// \alib{strings;TString::buffer;buffer} in an anonymous C++ union of two pointers,
/// one typed <em>const char*</em> and the other <em>char*</em>.
/// This class exposes the non-constant buffer pointer of that union with method #VBuffer.
/// This allows users of this class to <em>freely</em> operate on the buffer.
/// Of course, it is up to the programmer that the integrity of the instance is kept intact and
/// that also the #Capacity of the buffer must not be exceeded.
/// In the case that the string's length is changed, method #SetLength needs to be
/// used to notify such change with the \b AString object. The latter of course is invokable only
/// on mutable objects, while method #VBuffer is declared \c const.
///
/// In addition to this, a bunch of methods allow the modification of single characters.
/// #operator[] is extended by a non-const version that returns a reference to a character instead
/// of just the character value.
///
/// \anchor alib_ns_strings_astring_appendto
/// <b>Appending Objects to AStrings:</b><br>
/// This class is used to provide a global concept of having a sort of <b>"ToString"</b> method
/// with any C++ class. For this, the class provides method #Append, which uses template
/// meta programming to accept types with a corresponding specialization of functor
/// \alib{strings;AppendableTraits}.
///
/// The concept is described in detail with chapter \ref alib_strings_assembly_ttostring
/// of this module's \ref alib_mod_strings "Programmer's Manual".
///
///
/// @tparam TChar      The character type.
///                    Alias names for specializations of this class using character types
///                    \alib{characters;character}, \alib{characters;nchar}, \alib{characters;wchar},
///                    \alib{characters;xchar}, \alib{characters;complementChar} and
///                    \alib{characters;strangeChar} are provided in namespace #alib with type ,
///                    definitions \alib{AString}, \alib{NAString} \alib{WAString}, \alib{XAString},
///                    \alib{ComplementAString} and \alib{StrangeAString}.
/// @tparam TAllocator The \alib{lang;Allocator;allocator type} to use.
//==================================================================================================
template<typename TChar, typename TAllocator= lang::HeapAllocator>
requires alib::lang::IsAllocator<TAllocator>
class TAString : public TString<TChar>
               , public lang::AllocatorMember<TAllocator>
{
  //################################################################################################
  // Protected fields
  //################################################################################################
  protected:
    /// The base string-type.
    using base=        TString<TChar>;

    /// The type of the base class that stores the allocator.
    using allocBase=   lang::AllocatorMember<TAllocator>;

    /// The current size of the buffer excluding the trailing <c>'\0'</c>.
    /// If no buffer is allocated, this field is \c 0.
    /// If an external Buffer not managed by this class is used, then the size of that buffer is
    /// stored as a negative value. Method #Capacity therefore returns the absolute value
    /// of this field.
    integer                         capacity;

  #if ALIB_DEBUG_STRINGS
  protected:
    /// Used to check if previous grow request was exactly what is now the length.<br>
    /// This field is available only if code selection symbol \ref ALIB_DEBUG_STRINGS
    /// is set.
    integer     debugLastAllocRequest                                                            =0;

  public:
  //################################################################################################
  /// @name Debug Features
  /// @{
  //################################################################################################
    /// Checks this object's state. This method is internally invoked with almost
    /// every other method of this class, but only if the compiler-symbol
    /// \ref ALIB_DEBUG_STRINGS is \c true.<br>
    /// Invocations to this method should be performed using macro
    /// \ref ALIB_STRING_DBG_CHK.<br>
    ///
    /// \see
    ///   For (a little) more information see
    ///   \ref alib_strings_details_debugging of this module's
    ///   \ref alib_mod_strings "Programmer's Manual"
    void        dbgCheck()                                                                    const;
/// @}


  #endif

  protected:
    /// If \c true, a \ref alib_mod_assert "warning is raised" when an external buffer, whose
    /// life-cycle is not controlled by this instance gets replaced.
    /// This field exists only with debug-compilations of this class.
    ///
    /// \see
    ///    See method #DbgDisableBufferReplacementWarning for more information.
    #if ALIB_DEBUG
    bool    dbgWarnWhenExternalBufferIsReplaced                                               =true;
    #endif

  public:
  //################################################################################################
  /// @name Debug Features
  /// @{
  //################################################################################################
    /// Used to disable warnings that are by default raised in debug-compilations of
    /// method #SetBuffer.
    ///
    /// \note
    ///   In release-compilations this inline method is empty and will be optimized out.
    ///
    /// \see See method #SetBuffer for more information.
    void        DbgDisableBufferReplacementWarning()
    { ALIB_DBG( dbgWarnWhenExternalBufferIsReplaced= false; ) }
    /// @}

  protected:
  //################################################################################################
  // Protected Constructors
  //################################################################################################
    /// Constructs an \b %AString with the given external buffer and allocator.
    /// The given buffer's life-cycle is considered to be managed externally.<br>
    /// This constructor is protected and provided for derived classes that dispose of
    /// their own buffer.
    ///
    /// \note
    ///   Protected access was given to this method also to avoid misunderstandings that this
    ///   constructor is not for providing copyable string data. If the functionality of this
    ///   constructor is needed, it can simply be imitated by
    ///   - default construction and
    ///   - immediate invocation of #SetBuffer(TChar*, integer, integer, lang::Responsibility).
    ///
    /// @param pAllocator      The allocator to use.
    /// @param extBuffer       The external buffer to use.
    /// @param extBufferSize   The capacity of the given buffer.
    constexpr
    explicit TAString( TAllocator& pAllocator, TChar* extBuffer, integer extBufferSize )
    : base      ( extBuffer, 0 )
    , allocBase ( pAllocator )
    , capacity  (- (extBufferSize - 1))
    #if ALIB_DEBUG_STRINGS
    ,debugLastAllocRequest(extBufferSize-1)
    #endif
    {}

    /// Constructs an \b %AString with the given external buffer.
    /// The given buffer's life-cycle is considered to be managed externally.<br>
    /// This constructor is protected and provided for derived classes that dispose of
    /// their own buffer.
    ///
    /// \note
    ///   Protected access was given to this method also to avoid misunderstandings that this
    ///   constructor is not for providing copyable string data. If the functionality of this
    ///   constructor is needed, it can simply be imitated by
    ///   - default construction and
    ///   - immediate invocation of #SetBuffer(TChar*, integer, integer, lang::Responsibility).
    ///
    /// @param extBuffer       The external buffer to use.
    /// @param extBufferSize   The capacity of the given buffer.
    constexpr
    explicit TAString( TChar* extBuffer, integer extBufferSize )
    : base( extBuffer, 0 )
    , capacity  (- (extBufferSize - 1))
    #if ALIB_DEBUG_STRINGS
    ,debugLastAllocRequest(extBufferSize-1)
    #endif
    {}

  public:
    /// Exposes the allocator type specified by template parameter \p{TAllocator}.
    using AllocatorType=        TAllocator;

  //################################################################################################
  // Constructors, Destructor and Assignment
  //################################################################################################
    /// Constructs an empty, \e nulled \b %AString (does not allocate a buffer).
    explicit constexpr        TAString()
    : base(nullptr, 0)
    , capacity(0)                                                                                 {}

    /// Constructs an empty, \e nulled \b %AString (does not allocate a buffer).
    /// This constructor is to be used with template parameter \p{TAllocator} denoting an
    /// allocation strategy that includes the need of storing an allocator type in field
    /// #allocator. For the standard, heap-allocated type \alib{AString}, this constructor is not
    /// applicable.
    /// @param pAllocator The allocator to use.
    explicit constexpr
    TAString(TAllocator& pAllocator)
    : base     (nullptr, 0)
    , allocBase(pAllocator)
    , capacity (0)                                                                                {}

    /// Copy constructor that allocates memory and copies the contents of the given object.
    /// @param copy The object to copy.
    explicit
    TAString(const TAString& copy)
    : base(nullptr, 0)
    , allocBase(copy)
    , capacity      (0)
    {
        ALIB_DBG(dbgWarnWhenExternalBufferIsReplaced= copy.dbgWarnWhenExternalBufferIsReplaced;)
        Append(copy);
    }

    /// Move constructor.
    /// See \ref alib_ns_strings_astring_copymove "Copy/Move Constructor and Assignment"
    /// for details.
    /// @param move The object to move.
    TAString(TAString&& move)                                                               noexcept
    : allocBase(move) {
        // given move object has external buffer: we have to copy
        if ( !move.HasInternalBuffer() ) {
            base::buffer= nullptr;
            base::length= 0;
                capacity= 0;
            ALIB_DBG( dbgWarnWhenExternalBufferIsReplaced= move.dbgWarnWhenExternalBufferIsReplaced; )
            Append( move );
            return;
        }

        // copy values
        base::buffer=   move.buffer;
        base::length=   move.length;
            capacity=   move.capacity;

        // clean moved object (buffer does not need to be nulled)
        move.length=
        move.capacity=  0;

        // in debug mode, more copying and more destructor prevention is needed
        #if ALIB_DEBUG
            dbgWarnWhenExternalBufferIsReplaced= move.dbgWarnWhenExternalBufferIsReplaced;
            #if ALIB_DEBUG_STRINGS
            debugLastAllocRequest=               move.debugLastAllocRequest;
            move.buffer=  nullptr;
            #endif
        #endif
    }


    /// Constructs the object and uses #Append to create a string representation of the given
    /// object.
    ///
    /// @tparam TAppendable  The type of parameter \p{source} that has a specialization of
    ///                      functor \alib{strings;AppendableTraits}.
    /// @param  src          The source object to append to this string.
    template <typename TAppendable>
    explicit
    TAString (const  TAppendable& src )
    : base(nullptr, 0)
    , capacity(0)                                                                 { Append( src ); }

    /// Destructs an \b %AString object. An internally allocated buffer will be deleted.
    ~TAString()                                                                           noexcept {
        ALIB_STRING_DBG_CHK(this)
        if ( HasInternalBuffer() )
            allocBase::GetAllocator().free( base::vbuffer
                                                                    #if ALIB_DEBUG_STRINGS
                                                                            - 16
                                                                    #endif
                                           , size_t(capacity + 1) * sizeof(TChar)
                                                                    #if ALIB_DEBUG_STRINGS
                                                                            + 32* sizeof(TChar)
                                                                    #endif
                                           );
    }

  //######################################### casting  back ########################################
    /// Templated \b implicit <c>cast operator</c> constructing an instance of type \p{T} from
    /// this string instance.
    /// This operator requires the type \p{T} satisfying the concept
    /// \alib{characters;IsImplicitZTArrayCast}.
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
    requires  (     alib::characters::IsImplicitArrayCast<T, TChar>
                && !alib::strings::NoAutoCastTraits< TAString<TChar,TAllocator>,
                                                     characters::Policy::Implicit,
                                                     std::remove_cv_t<T> >::value)
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
    requires (     alib::characters::IsExplicitArrayCast  <T, TChar>
               && !alib::characters::IsImplicitZTArrayCast<T, TChar>
               && !alib::strings::NoAutoCastTraits< TAString<TChar,TAllocator>,
                                                    characters::Policy::ExplicitOnly,
                                                    std::remove_cv_t<T> >::value          )
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
    ///              Deduced by the compiler.
    /// @return A value of type \p{T}.
    template< typename T>
    requires (    !alib::characters::IsImplicitArrayCast  <T, TChar>
               &&  alib::characters::IsImplicitZTArrayCast<T, TChar>
               && !alib::strings::NoAutoCastTraits< TAString<TChar,TAllocator>,
                                                    characters::Policy::Implicit,
                                                    std::remove_cv_t<T> >::value          )
    constexpr           operator T()                                                         const {
        Terminate();
        return characters::ZTArrayTraits<T,TChar>::Construct( base::buffer, base::length );
    }


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
    requires (    !alib::characters::IsExplicitArrayCast  <T, TChar>
               &&  alib::characters::IsExplicitZTArrayCast<T, TChar>
               && !alib::strings::NoAutoCastTraits< TAString<TChar,TAllocator>,
                                                    characters::Policy::ExplicitOnly,
                                                    std::remove_cv_t<T> >::value          )
    constexpr explicit  operator T()                                                         const {
        Terminate();
        return characters::ZTArrayTraits<T,TChar>::Construct( base::buffer, base::length );
    }

    /// Copy assign operator.
    /// If the given other \b AString is \e nulled, this object becomes \e nulled. Otherwise,
    /// this string is cleared and the given other string is appended.
    ///
    /// \see
    ///   For details why no other assignment operators exists for this class, note paragraph
    ///   \ref alib_ns_strings_astring_copymove "Copy/Move Constructor and Assignment"
    ///   of this class's reference documentation.
    ///
    /// @param  copy  The object to copy the contents from.
    /// @return \c *this to allow concatenated calls.
    TAString& operator= (const TAString&  copy) {
        if ( copy.IsNull() ) {
            SetNull();
            return *this;
        }

        return Reset().Append( copy.Buffer(), copy.Length() );
    }

  //################################################################################################
  /// @name Memory allocation and buffer access
  //################################################################################################

    /// Resizes the buffer to meet exactly the given size.
    ///
    /// The following rules apply:
    /// - The string represented by this instance is copied to the new buffer.
    ///   If this is larger than the new buffer size, the string is cut at the end to fit.
    /// - If the desired new size is \c 0, then the currently allocated buffer will be disposed
    ///   and the object's state is \e nulled.
    /// - If the current buffer's life-cycle is managed externally (e.g., was set using
    ///   #SetBuffer(TChar*,integer,integer,lang::Responsibility)
    ///   with parameter \p{responsibility} being \c Responsibility::KeepWithSender), this method
    ///   will replace the buffer by a new one, even if the new requested size is the same as
    ///   the external buffer's size. In other words, the only case when this method does not
    ///   replace the current buffer is when the current buffer's life-cycle is (already)
    ///   internally managed and it has the same size than what is requested.
    /// - In this C++ version of \alib, the true allocation size is one character larger than
    ///   what is given with parameter \p{newCapacity}.
    ///   This allows method #Terminate to add a termination character without checking
    ///   (and eventually reallocating and copying) an internally managed buffer.
    ///
    /// Any method of this class that extends the length of the string will directly or
    /// indirectly invoke this method, when the current buffer size is not sufficient.
    /// If a future string length, which is the result of more than one concatenation of data
    /// to an \b %AString is predictable, then it is advisable to reserve the allocated size
    /// before performing the planned concatenations, by invoking this method.
    /// This is to avoid unnecessary allocations and data copy operations.
    ///
    /// If an \ref SetBuffer(TChar*,integer,integer,lang::Responsibility) "external buffer" is set,
    /// in debug-compilations a \ref alib_mod_assert "warning is raised", because usually it
    /// is not wanted that an external buffer becomes replaced during the growth of a string.<br>
    /// Such warnings can be switched off using method #DbgDisableBufferReplacementWarning.
    /// For example, in some situation it might be taken into account that instances of derived
    /// type \alib{strings;TLocalString;LocalString} sometimes may grow more than average and
    /// in such case a heap-allocated buffer replaces a local one. By placing a call
    /// to method #DbgDisableBufferReplacementWarning, the code explicitly hints to that
    /// possibility and is well readable. In release-compilations no warnings are issued
    /// and method \b %DbgDisableBufferReplacementWarning is optimized out.
    ///
    /// @param newCapacity   The new capacity of the internal buffer.
    ALIB_DLL  void    SetBuffer( integer newCapacity );

    /// This methods replaces the current buffer with the one provided.
    ///
    /// The following rules apply:
    /// - If a \c nullptr is provided, the current buffer is released.
    /// - If the provided buffer is not \c nullptr, its size given with parameter
    ///   \p{extBufferSize} has to be at least \c 1 for providing the space for a termination
    ///   character.
    /// - After the operation, #Capacity will report \p{extBufferSize} <c>- 1</c>.
    /// - Optional parameter \p{responsibility} can be used to pass the responsibility for the
    ///   deletion of the buffer to this object.
    /// - The length of the content provided with parameter \p{extLength} must not exceed
    ///   the value of parameter \p{extBufferSize} \c -1.
    /// - In no event any data of an existing buffer is copied into the new one. The rationale
    ///   here is that in most use cases, this is not needed. Should this be desired,
    ///   then the contents has to be copied "manually" before invoking this method.
    ///
    /// The internal buffer allocation is performed with methods \alib{lang;Allocator::allocate},
    /// \alib{lang;Allocator::reallocate} and \alib{lang;Allocator::free}. The latter two are
    /// also used on external buffers that are provided with this method in case parameter
    /// \p{responsibility} is given as \b Responsibility::Transfer, because in this case such
    /// externally created buffer is considered to have been allocated using the same
    /// instance of template type \p{TAllocator}. Consequently, it has to be ensured that the
    /// given piece of memory is "compatible" in this respect.
    ///
    /// @param extBuffer       The external buffer to use.
    /// @param extBufferSize   The size of the given buffer.
    /// @param extLength       The length of any content located in the given buffer that should
    ///                        be used.
    ///                        Has to be smaller or equal to extBufferSize -1 to preserve
    ///                        space for a trailing '\0'.
    /// @param responsibility  If \c Responsibility::Transfer, the given buffer will be deleted
    ///                        by this object when a new buffer is set or it is deleted itself.
    ///                        Defaults to \c Responsibility::KeepWithSender which denotes that
    ///                        the life-cycle of the given external buffer is managed elsewhere.
    ALIB_DLL
    void     SetBuffer( TChar*               extBuffer,
                        integer              extBufferSize,
                        integer              extLength          = 0,
                        lang::Responsibility responsibility     = lang::Responsibility::KeepWithSender );

    /// Ensures that the capacity of the internal buffer meets or exceeds the actual length
    /// plus the given growth value.
    ///
    /// @param spaceNeeded  The desired growth of the length of the string represented by this.
    void     EnsureRemainingCapacity( integer spaceNeeded ) {
        #if ALIB_DEBUG_STRINGS
            ALIB_ASSERT_ERROR( base::length <= debugLastAllocRequest,
                               "STRINGS", "Previous allocation request was too short: {} >= {}",
                               base::length, debugLastAllocRequest )
        #endif

        if ( Capacity() < base::length + spaceNeeded )
            GrowBufferAtLeastBy( spaceNeeded );

        #if ALIB_DEBUG_STRINGS
            debugLastAllocRequest= base::length + spaceNeeded;
        #endif
    }

    /// Increases the allocation size by either 50% of the current capacity or by the value
    /// provided, whichever is higher.
    ///
    /// @param minimumGrowth    The desired minimum growth of length.
    ALIB_DLL
    void     GrowBufferAtLeastBy( integer minimumGrowth );

    /// The size of the internal buffer (this is excluding the trailing \c '\0' character)
    /// which is reserved to terminate the string if needed.
    /// In other words, the internal memory available is the size returned here plus one.
    ///
    /// @return The size of the allocated buffer.
    integer Capacity()                        const { return capacity >= 0 ? capacity : -capacity; }

    /// Returns \c true, if the buffer was allocated by this class itself. If the buffer was
    /// set using #SetBuffer(TChar*,integer,integer,lang::Responsibility) with parameter
    /// \p{responsibility} given as \c Responsibility::KeepWithSender (and not automatically
    /// replaced, yet, because it became too small) then \c false is returned.
    /// \note Derived class
    ///       \alib{strings;TLocalString;LocalString} will report \c false here.
    ///       This sounds wrong on the first sight, as the buffer is allocated by an 'internal'
    ///       member. But from an AString's perspective, class <em>LocalString</em> works on
    ///       an 'external' buffer.
    ///
    /// @return \c true if the buffer is internally allocated and will be deleted with the
    ///         deletion of this object. False otherwise.
    bool HasInternalBuffer()                                         const { return  capacity > 0; }

    /// Invokes \ref SetBuffer "SetBuffer(0)".
    void           SetNull()                                                     { SetBuffer( 0 ); }

    /// Writes a zero-termination character <c>'\0'</c> to the end of the used part of the
    /// internal string buffer and returns the pointer to the start.
    /// In other words, this function returns the represented string as a \e "cstring".
    ///
    /// One implementation detail of this class is that it always ensures that the internal
    /// buffer's capacity is sufficient for a termination character. This way, using this
    /// method will not reallocate the string and the method can be invoked on constant objects.
    ///
    /// The explicit invocation of this method can often be omitted, due to the availability
    /// of the definition of an implicit cast operator to <c>const TChar</c>, which inlines
    /// a call to this method.
    ///
    /// @return The pointer to the zero-terminated character buffer.
    constexpr const TChar* Terminate()                                                       const {
        if ( base::vbuffer )
             base::vbuffer[ base::length ]= '\0';

        return base::buffer;
    }

  //################################################################################################
  /// @name Writable Buffer Access
  //################################################################################################

    /// The internal buffer character array provided as non constant character pointer.
    /// \see Chapter
    /// \ref alib_ns_strings_astring_write_access "Write Access to the Buffer"
    /// of the reference documentation of this class.
    ///
    /// @return The internal buffer array.
    TChar*          VBuffer()                                        const { return base::vbuffer; }

    /// Sets the character at the given index. A range check is performed. If this fails,
    /// nothing is done.
    ///
    /// \note
    ///   The C++ language would allow to declare this method \c const, as it does not
    ///   manipulate the data of the class itself but a character in the buffer pointer.<br>
    ///   In exceptional cases, to manipulate the contents of <em>const %AString</em>, use
    ///   method VBuffer() like in the following sample:
    ///   \snippet "DOX_STRINGS.cpp" DOX_ASTRING_MODIFY_CONST_BUFFER
    ///   <p>
    ///
    /// @tparam TCheck Performs a range check on the given index and a check for illegal setting
    ///                of termination character '\0' anywhere else but at idx==length.
    /// @param idx     The index of the character to write.
    /// @param c       The character to write.
    template<typename TCheck =CHK>
    void        SetCharAt( integer idx, TChar c ) {
        ALIB_ASSERT_ERROR( c != '\0' || idx==base::length,
                           "STRINGS", "Can't write character '\0'" )
        if constexpr ( TCheck::value ) {
            if(    (idx >= 0  && idx <  base::length )
                || ( c =='\0' && idx == base::length ) )
                *(base::vbuffer + idx )= c;
        } else {
            ALIB_ASSERT_ERROR( idx >= 0 && idx < base::length, "STRINGS",
                "Non-checking invocation: ","Index out of range: 0 <= {} < {}", idx, base::length )
            *(base::vbuffer + idx )= c;
    }   }

    /// Provides read/write access to single characters.
    /// Overrides \alib{strings;TString::operator[];String::operator[]} returning a reference to
    /// a \p{TChar} value, which allows changing the character stored.
    ///
    /// \attention
    ///   No parameter check is performed (other than an assertions in debug-compilation of
    ///   \alib). See \alib{strings;TString::operator[];String::operator[]} for details.
    ///
    /// @param idx The index of the character within this object's buffer.
    /// @returns If the character contained (or, if lvalue the one to set).
    TChar&      operator[] (integer  idx) {
        ALIB_ASSERT_ERROR( idx >= 0  && idx < base::length , "STRINGS",
                           "Index out of range: 0 <= {} < {}", idx, base::length )
        return base::vbuffer[idx];
    }

    using base::operator[];

    /// Sets a new length for this string.
    ///
    /// In debug-compilations, given \p{newLength} is checked to be positive and smaller or
    /// equal to the buffer's capacity.
    ///
    /// In the (frequent) situation that the given length is shorter (or equal) to the current
    /// length, for better readability, the use of method #ShortenTo instead of this method is
    /// recommended. Extending the length of the string should be done only in rare cases,
    /// when the string buffer was modified "externally" after retrieving it using
    /// #VBuffer.
    ///
    /// @param newLength  The new length of the \b %AString. Must be between 0 and
    ///                   #Capacity.
    void    SetLength( integer newLength ) {
        ALIB_ASSERT_ERROR( newLength >= 0         ,
                           "STRINGS", "Negative AString length {} requested", newLength )
        ALIB_ASSERT_ERROR( newLength <= Capacity(),
                           "STRINGS", "Requested AString length {} exceeds capacity {}",
                           newLength, Capacity() )
        base::length= newLength;
        ALIB_STRING_DBG_CHK(this)
    }

    /// Searches termination character <c>'\0'</c> and sets the corresponding length of the
    /// string.
    /// In debug-compilations, the detected length is smaller or equal to the buffer's capacity.
    ///
    /// This method may be used in situations, where the strings's buffer is
    /// exposed to other libraries (for example, many operating system calls), which internally
    /// fill a given character buffer, zero-terminate it, but just do not return it's length.
    /// @param offset The offset for searching the termination byte \c '\0'.
    ///               Defaults to \c 0. Using this parameter allows faster detection in the case
    ///               a minimum length is known.
    /// @return Returns the new length of this string for convenience.
    integer    DetectLength(integer offset = 0) {
        base::length= characters::Length( base::Buffer() + offset ) + offset;
        ALIB_ASSERT_ERROR( base::length <= Capacity(),
                           "STRINGS", "Detected AString length {} exceeds capacity {}",
                           base::length, Capacity() )
        ALIB_STRING_DBG_CHK(this)
        return base::length;
    }

    /// Sets the length of the string to a shorter (or equal) value.
    ///
    /// In release-compilations, this method has the same simple inline implementation as
    /// #SetLength, it just sets the internal field \b length to the given value.
    /// The reason for the method's existence is primarily readability of the code: The name
    /// expresses that the given \p{newLength} is shorter than the current length.
    ///
    /// In debug-compilations, an \ref alib_mod_assert "error is raised" if the length provided
    /// is longer than the current length. An equal value is accepted.
    ///
    /// In situations when it is sure that a new length is equal or shorter to the existing
    /// one, the use of this method is recommended over the use of #SetLength.
    /// This is especially true for the frequent use case where a "base string" should be
    /// restored after one or more concatenations had been performed.
    ///
    /// @param newLength  The new length of this \b %AString. Must be between 0 and the current
    ///                   length.
    /// @return    \c *this to allow concatenated calls.
    TAString&    ShortenTo( integer newLength ) {
        ALIB_ASSERT_ERROR( newLength >= 0,
                           "STRINGS", "Negative AString length {} requested", newLength )
        ALIB_ASSERT_ERROR( newLength <= base::length,
                           "STRINGS", "Increase from {} to {} of AString length requested",
                           base::Length(), newLength )
        #if ALIB_DEBUG_STRINGS
        // Avoid analyser warnings with UT when above assertions are switched off.
        if ( newLength >= 0 && newLength < base::length )
        #endif
        base::length= newLength;
        ALIB_STRING_DBG_CHK(this)
        return *this;
    }

    /// Same as #ShortenTo, but accepts a value interpreted as the difference to the current length.
    /// @param charsToRemove  The number of characters to remove.
    /// @return   \c *this to allow concatenated calls.
    TAString&    ShortenBy( integer charsToRemove ) {
        return ShortenTo( base::Length() - charsToRemove );
    }
    

  //################################################################################################
  /// @name Append-Methods
  //################################################################################################

    /// Appends an array of an incompatible character type.
    ///
    /// @tparam TCharSrc    The character type of the given array.
    /// @tparam TCheck      Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                     If \c \<false\>, no nullptr check is done on parameter \p{src}.
    ///                     Also, this object would not lose a \e nulled state when the given
    ///                     cstring portion is empty.
    /// @param  src         A pointer to the start of the array to append.
    /// @param  srcLength   The length of the string.
    ///
    /// @return    \c *this to allow concatenated calls.
    template <typename TCheck= CHK, typename TCharSrc >
    requires (    characters::IsCharacter<TCharSrc>
               && !std::same_as<TCharSrc,TChar>           )
    TAString& Append( const TCharSrc* src, integer srcLength ) {

        if constexpr ( std::same_as<TCheck, alib::CHK> ) {
            if( !src )  return *this;
            if ( srcLength <= 0 ) {
                if ( base::IsNull() )
                    SetBuffer( 15 );
                return *this;
        }   }

        // We have to know what the "real" types are.
        #if !DOXYGEN
        #if ALIB_CHARACTERS_NATIVE_WCHAR
        #   define REAL_WCHAR wchar
        #   define REAL_XCHAR xchar
        #else
        #   define REAL_WCHAR xchar
        #   define REAL_XCHAR wchar
        #endif
        #endif

        // TAString<nchar>
        if constexpr ( std::same_as<TChar, nchar> ) {

            // wchar -> nchar
            if constexpr ( std::same_as<TCharSrc, REAL_WCHAR> ) {
              //---------------------------------- Windows Version ---------------------------------
                #if defined( _WIN32 )

                    // loop until reserved size is big enough
                    for(integer bufSizeFactor= 2; bufSizeFactor < 8; ++bufSizeFactor) {
                        EnsureRemainingCapacity( srcLength * bufSizeFactor );
                        int conversionSize= WideCharToMultiByte( CP_UTF8, 0,
                                                                 src, int( srcLength),
                                                                 base::vbuffer + base::length, int( Capacity() - base::length ),
                                                                 NULL, NULL );
                        if ( conversionSize > 0 ) {
                            base::length+= conversionSize;
                            return *this;
                        }

                        // not enough space?
                        int error= GetLastError();
                        if (error == ERROR_INSUFFICIENT_BUFFER ) {
                            EnsureRemainingCapacity( srcLength );
                            continue;
                        }

                        // quit on other errors
                        ALIB_WARNING( "STRINGS",
                         "AString: Cannot convert wide character string to UTF-8. Error: {} ({})",
                          (   error == ERROR_INVALID_FLAGS          ? "ERROR_INVALID_FLAGS."
                            : error == ERROR_INVALID_PARAMETER      ? "ERROR_INVALID_PARAMETER"
                            : error == ERROR_NO_UNICODE_TRANSLATION ? "ERROR_NO_UNICODE_TRANSLATION"
                                                                    : "<unknown>" ),
                          error  )
                        return *this;
                    }
                    // Error: not enough space
                    ALIB_ERROR(  "STRINGS", "AString: Cannot convert wide character string to "
                                            "UTF-8. Error: ERROR_INSUFFICIENT_BUFFER" )
                    return *this;

              //-------------------------------- __GLIBCXX__ Version -------------------------------
                #elif defined (__GLIBCXX__)  || defined(_LIBCPP_VERSION) || defined(__APPLE__) || defined(__ANDROID_NDK__)

                    integer maxConversionSize= integer(MB_CUR_MAX) * srcLength;

                    mbstate_t ps;
                    EnsureRemainingCapacity( maxConversionSize );
                    memset( &ps, 0, sizeof(mbstate_t) );
                    const REAL_WCHAR* srcp= src;
                    size_t conversionSize= wcsnrtombs( base::vbuffer + base::length, &srcp, size_t(srcLength), size_t(maxConversionSize),  &ps);
                    if ( conversionSize == size_t( -1 ) ) {
                        ALIB_WARNING( "STRINGS", "Cannot convert WCS to MBCS. "
                                                 "Check locale settings (should be UTF-8)" )
                        return *this;
                    }

                    if ( conversionSize < 1 ) {
                        ALIB_ERROR( "STRINGS", "Error converting WCS to MBCS." )
                        return *this;
                    }

                    TString<TChar>::length+= conversionSize;
                    return *this;

                #else
                    #pragma message ("Unknown Platform in file: " __FILE__ )
                    return *this;
                #endif
            } // wchar -> nchar

            // xchar -> nchar
            if constexpr ( std::same_as<TCharSrc, REAL_XCHAR> ) {
                // convert to REAL_WCHAR and invoke REAL_WCHAR version
                TAString<REAL_WCHAR, lang::HeapAllocator>   converter;
                REAL_WCHAR externalBuffer[2048];
                converter.SetBuffer( externalBuffer, 2048 );
                converter.DbgDisableBufferReplacementWarning();
                converter.Append<NC>( src, srcLength );
                Append<NC>( converter.Buffer(), converter.Length() );
            }

        } // TAString<nchar>

        // TAString<wchar_t>
        if constexpr ( std::same_as<TChar, wchar_t> ) {
            // nchar -> wchar_t
            if constexpr ( std::same_as<TCharSrc, nchar> ) {
                EnsureRemainingCapacity( srcLength );
              //---------------------------------- Windows Version ---------------------------------
                #if defined( _WIN32 )
                    if( srcLength == 0)
                        return *this;
                    integer conversionSize= MultiByteToWideChar( CP_UTF8, 9,
                                                                 src, int( srcLength ),
                                                                 base::vbuffer + base::length,
                                                                 int( Capacity() - base::length ) );
                    // check for errors
                    #if ALIB_DEBUG
                        if ( conversionSize == 0 ) {
                            // not enough space?
                            int error= GetLastError();

                            ALIB_WARNING( "STRINGS",
                                "MBCS to WCS conversion failed. Error: {} ({})",
                                (  error == ERROR_INSUFFICIENT_BUFFER      ?  "ERROR_INSUFFICIENT_BUFFER."
                                 : error == ERROR_INVALID_FLAGS            ?  "ERROR_INVALID_FLAGS."
                                 : error == ERROR_INVALID_PARAMETER        ?  "ERROR_INVALID_PARAMETER"
                                 : error == ERROR_NO_UNICODE_TRANSLATION   ?  "ERROR_NO_UNICODE_TRANSLATION"
                                                                           :  "<unknown>" ),
                                error )

                        }

                        ALIB_ASSERT_ERROR( conversionSize <= srcLength, "STRINGS",
                           "MBCS to WCS conversion failed. Requested length={}, conversion length=",
                           srcLength, conversionSize )
                    #endif

                    base::length+= conversionSize;
                    return *this;


              //-------------------------------- __GLIBCXX__ Version -------------------------------
                #elif defined (__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__) || defined(__ANDROID_NDK__)

                    // copy loop
                    while(srcLength > 0 ) {
                        integer actConversionLenght= srcLength;
                        for( int pass= 0 ; pass < 2 ; ++pass ) {

                            mbstate_t    ps;    memset( &ps, 0, sizeof(mbstate_t) );
                            const nchar* srcp= src;
                            size_t       wcWritten= mbsnrtowcs( base::vbuffer + base::length,  &srcp,
                                                                size_t(actConversionLenght),
                                                                size_t(Capacity() - base::length), &ps );

                            // single character failed?
                            if( wcWritten == static_cast<size_t >(-1) ) {
                                // already repeated?
                                // This can't (must not) happen! If it did, release code does infinite loop!
                                ALIB_ASSERT( pass == 0, "STRINGS" )

                                // first character that failed?
                                if( srcp == src ) {
                                    ++src;
                                    --srcLength;
                                    *(base::vbuffer + base::length++)= '?';
                                    break; // break try loop, continue with next character
                                }

                                // retry those characters that succeeded
                                actConversionLenght= srcp - src;
                                continue;
                            }

                            base::length+=    wcWritten;
                            src+=       wcWritten;
                            srcLength-= actConversionLenght;
                            break;
                    }   }
                    return *this;

                #else
                    #pragma message ("Unknown Platform in file: " __FILE__ )
                    return *this;
                #endif

            } // nchar -> wchar_t

            // xchar -> wchar_t
            if constexpr ( std::same_as<TCharSrc, REAL_XCHAR> ) {
              #if ALIB_SIZEOF_WCHAR_T == 4
                EnsureRemainingCapacity( srcLength );

                // convert UTF16 to UTF32
                const char16_t* srcEnd=    src + srcLength;
                while (src < srcEnd) {
                    const char32_t uc = char32_t(*src++);
                    if ((uc - 0xd800) >= 2048)  { // not surrogate
                        base::vbuffer[base::length++] = static_cast<wchar_t>(uc);
                    } else {
                        ALIB_ASSERT_ERROR(    src < srcEnd                        // has one more?
                                           && ((uc    & 0xfffffc00) == 0xd800)    // is low
                                           && ((*src  & 0xfffffc00) == 0xdc00),   // is high
                                           "STRINGS", "Error decoding UTF16" )
                        base::vbuffer[base::length++]=  static_cast<wchar_t>(     (uc << 10)
                                                                       +  ((*src++) - 0x35fdc00 ) );
                }   }

              #else
                // convert UTF32 to UTF16
                EnsureRemainingCapacity( srcLength * 2 );

                const char32_t* srcEnd=    src + srcLength;
                while (src < srcEnd) {
                    uinteger uc= *src++;
                    ALIB_ASSERT_ERROR(       uc <  0xd800
                                        || ( uc >= 0xe000 && uc <= 0x10ffff ),
                                        "STRINGS", "Illegal unicode 32 bit codepoint"       )

                    if( uc < 0x10000 ) {
                          base::vbuffer[base::length++]=  static_cast<wchar_t>( uc );
                    } else {
                        uc-= 0x10000;
                        base::vbuffer[base::length++]= static_cast<wchar_t>(  ( uc >> 10    ) + 0xd800  );
                        base::vbuffer[base::length++]= static_cast<wchar_t>(  ( uc &  0x3ff ) + 0xdc00  );
                }   }

                return *this;
              #endif
            } // nchar -> wchar_t
        } //wchar_t

        // TAString<REAL_XCHAR>
        if constexpr ( std::same_as<TChar, REAL_XCHAR> ) {
            // wchar_t -> REAL_XCHAR
            if constexpr ( std::same_as<TCharSrc, wchar_t> ) {
              #if ALIB_SIZEOF_WCHAR_T == 2
                EnsureRemainingCapacity( srcLength );

                // convert UTF16 to UTF32
                const wchar_t* srcEnd=    src + srcLength;
                while (src < srcEnd) {
                    const char32_t uc = *src++;
                    if ((uc - 0xd800) >= 2048)  { // not surrogate
                        base::vbuffer[base::length++] = static_cast<REAL_XCHAR>(uc);
                    } else {
                        ALIB_ASSERT_ERROR(    src < srcEnd                        // has one more?
                                           && ((uc    & 0xfffffc00) == 0xd800)    // is low
                                           && ((*src  & 0xfffffc00) == 0xdc00),   // is high
                                           "STRINGS", "Error decoding UTF16" )

                        base::vbuffer[base::length++]= static_cast<REAL_XCHAR>(     (uc << 10)
                                                                                 +  ((*src++) - 0x35fdc00 ) );
                }   }

              #else
                // convert UTF32 to UTF16
                EnsureRemainingCapacity( srcLength * 2 ); // can potentially double!

                const wchar_t* srcEnd=    src + srcLength;
                while (src < srcEnd) {
                    uinteger uc= uinteger( *src++ );
                    ALIB_ASSERT_ERROR(       uc <  0xd800
                                        || ( uc >= 0xe000 && uc <= 0x10ffff ),
                                        "STRINGS", "Illegal unicode 32 bit codepoint"   )

                    if( uc < 0x10000 ) {
                          base::vbuffer[base::length++]=  static_cast<REAL_XCHAR>( uc );
                    } else {
                        uc-= 0x10000;
                        base::vbuffer[base::length++]= static_cast<REAL_XCHAR>(  ( uc >> 10    ) + 0xd800  );
                        base::vbuffer[base::length++]= static_cast<REAL_XCHAR>(  ( uc &  0x3ff ) + 0xdc00  );
                }   }
              #endif
            } // wchar_t -> REAL_XCHAR

            // nchar -> REAL_XCHAR
            if constexpr ( std::same_as<TCharSrc, nchar> ) {
                ALIB_STRING_DBG_CHK( this )

                // We are using a WAString to do the job. Not efficient, but for today, this should be all we do!
                TAString<REAL_WCHAR,lang::HeapAllocator>   converter;
                REAL_WCHAR externalBuffer[8192];
                externalBuffer[0]= 0;
                converter.SetBuffer( externalBuffer, 8192 );
                converter.Append<NC>( src, srcLength );
                converter.DbgDisableBufferReplacementWarning();
                return Append<NC>( converter.Buffer(), converter.Length() );
            } // nchar -> REAL_XCHAR


        } //wchar_t

        return *this;
    }


    /// Appends an array of the same character type.
    ///
    /// @tparam TCheck      Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                     If \c \<false\>, no nullptr check is done on parameter \p{src}.
    ///                     Also, this object would not lose a \e nulled state when the given
    ///                     cstring portion is empty.
    /// @param  src         A pointer to the start of the array to append.
    /// @param  srcLength   The length of the string.
    ///
    /// @return    \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TAString&
    Append( const TChar* src, integer srcLength ) {
        ALIB_STRING_DBG_CHK(this)

        if constexpr ( TCheck::value ) {
            if (!src)
                return *this;

            // check empty
            if ( srcLength <= 0 ) {
                // set "un-nulled"
                if ( base::IsNull() )
                    SetBuffer( 15 );

                return *this;
            }
        } else {
            ALIB_STRING_DBG_CHK(this)
            ALIB_ASSERT_ERROR( src || srcLength == 0, "STRINGS",
                               "Nullptr passed with non-checking method version." )
        }

        EnsureRemainingCapacity( srcLength );
        characters::Copy( src, srcLength, base::vbuffer + base::length );
        base::length+= srcLength;

        return *this;
    }

    /// Appends a region of another \alib{strings;TString;String}.
    /// The checking version adjusts the given region to the bounds of the source string.
    ///
    /// \note When using the non-checking version, parameter \p{regionLength} must be set
    ///       explicitly to the correct value (instead of using the default value).
    ///
    /// @tparam TCheck        Chooses checking or non-checking implementation.
    ///                       Defaults to \alib{CHK}.
    /// @param  src           The \b %String to append.
    /// @param  regionStart   The start of the region in \p{src} to append.
    /// @param  regionLength  The maximum length of the region in \p{src} to append.
    ///                       Defaults to \b %MAX_LEN
    ///
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TAString& Append(const TString<TChar>& src, integer regionStart, integer regionLength =MAX_LEN){
        if constexpr ( TCheck::value ) {
            if ( src.IsNull() )
                return *this;
            if ( src.base::AdjustRegion( regionStart, regionLength ) ) {
                // special treatment if currently nothing is allocated and a blank string ("") is added:
                // we allocate, which means, we are not a nulled object anymore!
                // (...also, in this case we check the src parameter)
                if ( base::IsNull() )
                    SetBuffer( 15 );
                return *this;
            }
        } else {
            //---- non-checking version ----
            ALIB_ASSERT_ERROR(    regionStart >= 0 && regionLength >= 0
                               && regionLength !=strings::MAX_LEN
                               && regionStart +  regionLength  <= src.Length(),  "STRINGS",
               "Non-checking invocation: ","Invalid region [{} {}] given. AString length: {}",
               regionStart, regionLength, src.Length()    )
        }

        // both versions
        return Append<NC>( src.Buffer() + regionStart, regionLength );
    }

    /// Appends a string-representation of the given object \p{src} of template type \p{T} to
    /// this \b %AString.
    /// Type \p{T} needs to satisfy the concept \alib{strings;IsAppendable}, which is achieved by
    /// specializing the type trait \p{AppendableTraits} accordingly.
    ///
    /// \see
    ///   See chapter \ref alib_strings_assembly_ttostring of this module's
    ///   \ref alib_mod_strings "Programmer's Manual" for more information.
    ///
    /// @tparam TCheck      Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                     If after the append operation this string is still \e nulled,
    ///                     an initial buffer is allocated and the state of this instance
    ///                     changes to \e empty.
    ///                     If \alib{NC} is given, a \e nulled string may remain \e nulled.
    /// @tparam TAppendable The type of parameter \p{src}.
    /// @param  src         The object to "stringify" and append.
    /// @return \c *this to allow concatenated calls.
    template<typename TCheck= CHK, typename TAppendable>
    requires alib::strings::IsAppendable<TAppendable, TChar,TAllocator>
    TAString& Append(const  TAppendable& src ) {
        AppendableTraits<TAppendable, TChar,TAllocator>()( *this,  src );

        if ( TCheck::value && base::IsNull() )
            SetBuffer( 15 );
        return *this;
    }

    /// Appends string-like types which satisfy either of the concepts
    /// \alib{characters;IsImplicitArraySource} \alib{characters;IsImplicitZTArraySource}.
    ///
    /// @tparam TCheck        Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                       If \alib{NC} is given, a \e nulled string may remain \e nulled,
    ///                       and if the character buffer received from \p{src} is \c nullptr,
    ///                       this method has undefined behavior.
    /// @tparam TStringSource The type of parameter \p{src}.
    /// @param  src    The string-type object to append.
    /// @return \c *this to allow concatenated calls.
    template< typename TCheck= CHK, typename TStringSource>
    requires (     alib::characters::IsImplicitZTArraySource<TStringSource,
                                                          characters::ZTType<TStringSource>>
               ||  alib::characters::IsImplicitArraySource <TStringSource,
                                                          characters::Type<TStringSource>> )
    TAString& Append(const  TStringSource& src ) {
        // implicit
        if constexpr ( characters::IsImplicitArraySource<TStringSource, characters::Type<TStringSource>> ) {
            using TSrc=  characters::Type<TStringSource>;
            using TCA=   characters::ArrayTraits<TStringSource,TSrc>;
            const TSrc* buf=  TCA::Buffer( src );
            return Append<TCheck>( buf, TCA::Length( src ) );
        // explicit
        } else {
            using TSrc= characters::ZTType<TStringSource>;
            using TCA=  characters::ZTArrayTraits<TStringSource,TSrc>;
            const TSrc* buf=     TCA::Buffer( src );
            return Append<TCheck>( buf, TCA::Length( src ) );
    }   }

    /// Appends platform-specific new line character(s) by appending literal string
    /// \alib{NEW_LINE}.
    /// @return \c *this to allow concatenated calls.
    TAString&          NewLine()  { return Append<NC>( CStringConstantsTraits<TChar>::NewLine() ); }


    /// Appends a single character of compatible type.
    /// @tparam TCheck Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                Here, nothing is appended if the given character is \c 0, but still
    ///                this instance will switch to \e empty state if it was \e nulled before.
    ///                If \alib{NC} is given, \p{src} is \b not checked for being \c 0.
    /// @param  src    The character object to append.
    /// @return \c *this to allow concatenated calls.
    template<typename TCheck= CHK>
    TAString&   Append(TChar src ) {
        if constexpr ( TCheck::value ) {
            if ( src == 0 ) {
                if (base::IsNull() )
                    SetBuffer( 15 );
            }
            else {
                EnsureRemainingCapacity( 1 );
                base::vbuffer[ base::length++ ]= src;
            }
            return *this;
        }

        EnsureRemainingCapacity( 1 );
        base::vbuffer[ base::length++ ]= src;
        return *this;
    }

    /// Appends a single character of a different type.
    /// If this string is narrow, the given character is UTF-8 encoded.
    /// @tparam TCheck Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                Here, nothing is appended if the given character is \c 0, but still
    ///                this instance will switch to \e empty state if it was \e nulled before.
    ///                If \alib{NC} is given, \p{src} is \b not checked for being \c 0.
    /// @param  src    The character value to append. Only values of types that satisfy
    ///                the concept \alib{characters;IsCharacter} are accepted.
    /// @return \c *this to allow concatenated calls.
    template< typename TCheck= CHK>
    TAString& Append( characters::IsCharacter auto src ) {
        // check for 0
        if ( TCheck::value && src == 0 ) {
            if (base::IsNull() )
                SetBuffer( 15 );
            return *this;
        }


        // this is an AString<nchar>?
        if constexpr ( std::same_as< TChar, nchar > ) {
            wchar wc=  static_cast<wchar>( src );
            int mbLength;
            #if defined(_WIN32)
                EnsureRemainingCapacity( MB_LEN_MAX * 2);
                mbLength= WideCharToMultiByte( CP_UTF8, 0, &wc, 1,
                                               ((nchar*) base::vbuffer) + base::length,
                                               MB_LEN_MAX * 2, NULL, NULL );
                if ( mbLength <= 0 ) {
                    ALIB_DBG( DWORD error= GetLastError(); )
                    ALIB_WARNING( "STRINGS", "Cannot convert wide character string to UTF-8. Error: {} ({})",
                     (   error == ERROR_INSUFFICIENT_BUFFER    ? "ERROR_INSUFFICIENT_BUFFER"
                      :  error == ERROR_INVALID_FLAGS          ? "ERROR_INVALID_FLAGS."
                      :  error == ERROR_INVALID_PARAMETER      ? "ERROR_INVALID_PARAMETER"
                      :  error == ERROR_NO_UNICODE_TRANSLATION ? "ERROR_NO_UNICODE_TRANSLATION"
                                                               : "<unknown>" ),
                      error )
                }
            #else
                EnsureRemainingCapacity( integer(MB_CUR_MAX) + 1);
                mbLength= wctomb( reinterpret_cast<nchar*>(base::vbuffer)
                                                         + base::length, wc );
            #endif

            if ( mbLength <= 0 ) {
                ALIB_WARNING( "STRINGS", "Cannot convert WC to MBC." )
                return *this;
            }

            base::length+= mbLength;
            return *this;
        }

        // AString<wchar>/<xchar>? (we are just casting)
        EnsureRemainingCapacity( 1 );
        base::vbuffer[ base::length++ ]= static_cast<TChar>( src );
        return *this;
    }

    /// Alias to overloaded methods #Append.<br>
    /// @tparam TCheck      Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                     If \alib{NC} is given, checks are omitted.
    ///                     (The specific behavior depends on the overloaded \b %Append method
    ///                     that is called.)
    /// @tparam TAppendable The type of parameter \p{source}.
    /// @param  src         The source object to append a string representation for.
    ///
    /// @return   \c *this to allow concatenated calls.
    template <typename TCheck= CHK, class TAppendable >
    TAString& _(const  TAppendable& src )                          { return Append<TCheck>( src ); }

    /// Alias method of #Append(const TString<TChar>&, integer, integer).<br>
    /// Provided for compatibility with C# and Java versions of \alib.
    ///
    /// @tparam TCheck        Chooses checking or non-checking implementation.
    ///                       Defaults to \alib{CHK}.
    /// @param  src           The \b %String to append.
    /// @param  regionStart   The start of the region in \p{src} to append.
    /// @param  regionLength  The maximum length of the region in \p{src} to append.
    ///                       Defaults to \b %MAX_LEN
    ///
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TAString& _( const TString<TChar>& src, integer regionStart, integer regionLength =MAX_LEN )
    { return Append( src, regionStart, regionLength ); }


    /// Alias to overloaded methods #Append.<br>
    /// When this operator is used, template parameter \p{TCheck} of the <b>Append</b>-method
    /// called is set to \alib{CHK}.
    ///
    /// @tparam TAppendable  The type of parameter \p{source}.
    /// @param  src   The object of type T to append.
    /// @return \c *this to allow concatenated calls.
    template <typename TAppendable>
    TAString& operator<< (const  TAppendable& src )                     { return Append<CHK>(src); }

    /// Alias to overloaded methods #Append.<br>
    /// When this operator is used, template parameter \p{TCheck} of the <b>Append</b>-method
    /// called is set to \alib{CHK}.
    /// \note
    ///   According to the C++ idiom, this class is considered a _container_ rather than a
    ///   _stream_.
    ///   Therefore, this operator is included for the sake of completeness and compatibiltiy with
    ///   class <c>std::string</c> which (only!) defines this operator.
    ///   However, the differences in operator **associativity** and **precedence rules** in C++
    ///   affect how <c>operator+=</c> behaves in chained expressions.
    ///   Specifically, this operator is left-associative, meaning expressions are evaluated from
    ///   left to right.
    ///   As a result, chaining multiple <c>+=</c> operations requires explicit parentheses to avoid
    ///   compilation errors and ensure the correct evaluation order.
    ///
    /// \note
    ///   For example:
    ///   \code{.cpp}
    ///         myString  += "hello"  += "world";  // Compilation error: parentheses are required
    ///         (myString += "hello") += "world";  // Correct.
    ///         myString  << "hello"  << "world";  // Correct without parentheses
    ///   \endcode
    /// \note
    ///   Because <c>operator<<</c> typically expresses a streaming-like behavior, it avoids such
    ///   ambiguity or precedence issues and is more straightforward for chaining multiple append
    ///   operations. Hence, it is recommended to prefer <c><<</c> over <c>+=</c> when performing
    ///   multiple appends within a single expression.
    ///
    /// @tparam TAppendable  The type of parameter \p{source}.
    /// @param  src   The object of type T to append.
    /// @return \c *this to allow concatenated calls.
    template <typename TAppendable>
    TAString& operator+= (const  TAppendable& src )                     { return Append<CHK>(src); }

  //################################################################################################
  /// @name Insert and Delete
  //################################################################################################

    /// Sets the length of this string to zero. A \e nulled object remains \e nulled.
    /// @return \c *this to allow concatenated calls.
    TAString&    Reset()          { ALIB_STRING_DBG_CHK(this)  base::length=    0;   return *this; }

    /// Sets the length of the string to zero and then invokes one of the overloaded methods
    /// #Append.
    ///
    /// @tparam TCheck      Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                     The value is used with the invocation of method \b Append.
    /// @tparam TAppendable The type of parameter \p{source}.
    /// @param  src         The source of type \p{TAppendable} to append.
    ///
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK, typename TAppendable>
    TAString& Reset(const TAppendable& src ) {
        ALIB_STRING_DBG_CHK(this)
        base::length=    0;
        Append<TCheck>( src );
        return *this;
    }

    /// Sets the length of the string to zero. Same as method #Reset.
    /// Provided for compatibility with C# and Java versions of \alib.
    /// @return \c *this to allow concatenated calls.
    TAString&    _() {
        ALIB_STRING_DBG_CHK(this)
        base::length=    0;
        return *this;
    }

    /// Inserts the given string at given position. If the position is not between 0 and the
    /// length of the target, nothing is inserted.
    ///
    /// \note
    ///   To insert a string with replacing a different one at the same time, use
    ///   \ref ReplaceSubstring(const TString<TChar>&,integer,integer) "ReplaceSubstring(src, pos, regionLength)".
    ///
    /// @tparam TCheck   Chooses checking or non-checking implementation.
    ///                  Defaults to \alib{CHK}.
    /// @param  src      The \b %String to insert.
    /// @param  pos      The position to insert \p{src}.
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TAString&   InsertAt( const TString<TChar>& src, integer pos ) {
        ALIB_STRING_DBG_CHK(this)
        integer srcLength= src.Length();
        if constexpr ( TCheck::value ) {
            if ( srcLength == 0 || pos < 0 || pos > base::length )
                return *this;
        } else {
            ALIB_ASSERT_ERROR( srcLength > 0 && pos >=0 && pos <= base::length, "STRINGS",
            "Non-checking invocation: ","Illegal insertion position 0 <= {} < {}.", pos, base::length )
        }

        EnsureRemainingCapacity( srcLength );

        // move content and copy string new region
        if ( pos != base::length )
            characters::Move( base::vbuffer + pos,
                                                base::length -  pos,
                                                base::vbuffer + pos + srcLength    );
        base::length+= src.CopyTo( base::vbuffer + pos );

        return *this;
    }

    /// Appends the given character \p{c} \p{qty}-times.
    /// The non-checking version does not check parameter \p{qty} to be greater than zero.
    ///
    /// @tparam TCheck Defaults to \alib{CHK}, which chooses the checking version of the method.
    /// @param c     The character to insert \p{qty} times.
    /// @param qty   The quantity of characters to insert.
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TAString&   InsertChars( TChar c, integer qty ) {
        if constexpr ( TCheck::value ) {
            if ( qty <= 0 )
                return *this;
        }
        else
            ALIB_ASSERT_ERROR( qty >= 0, "STRINGS",
              "Non-checking invocation: ", "Negative quantity {} given", qty )

        EnsureRemainingCapacity( qty );
        characters::Fill( base::vbuffer + base::length, qty, c );
        base::length+=  qty;
        return *this;
    }

    /// Inserts the given character \p{c} \p{qty}-times at a given position.
    /// If the given position is out of range, nothing is inserted.
    ///
    /// The non-checking version does not check the position to be in a valid range and
    /// the \p{qty} to be greater than zero.
    ///
    /// @tparam TCheck Defaults to \alib{CHK}, which chooses the checking version of the method.
    /// @param c     The character to insert \p{qty} times.
    /// @param qty   The quantity of characters to insert.
    /// @param pos   The index in this object where \p{c} is inserted \p{qty} times.
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TAString&   InsertChars( TChar c, integer qty, integer pos ) {
        if constexpr ( TCheck::value ) {
            if ( qty <= 0 || pos < 0 ||  pos > base::length )
                return *this;
        } else {
            ALIB_ASSERT_ERROR( qty >= 0, "STRINGS",
                               "Non-checking invocation: ", "Negative quantity {} given", qty )
            ALIB_ASSERT_ERROR( pos >= 0 && pos <= base::length, "STRINGS",
             "Non-checking invocation: ","Illegal position given: 0 <= {} < {}.", pos,base::length )
        }

        EnsureRemainingCapacity( qty );

        // move content ?
        if ( pos != base::length )
            characters::Move( base::vbuffer + pos,
                                                base::length  - pos,
                                                base::vbuffer + pos + qty    );

        // set
        characters::Fill( base::vbuffer + pos, qty, c );
        base::length+=  qty;

        return *this;
    }

    /// Removes a region from the string by moving the remaining string behind the region to
    /// the region start and adjusting the string's length.
    ///
    /// A range check is performed and the region is cut to fit to the string.
    /// The non-checking version (\p{TCheck} = \alib{NC}) omits the check, but still allows
    /// leaving parameter \p{regionLength} as default (respectively allows the sum of parameters
    /// \p{regionStart} and \p{regionLength} to be longer than the length of this \b %AString).
    /// In this case, this string is cut starting from index \p{regionStart}.
    ///
    /// \note See also methods #Reset, #DeleteStart and #DeleteEnd.
    ///
    /// @tparam TCheck       Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                      If \c \<false\> is added to the method name, the start of the region
    ///                      is not adjusted to the string's bounds.
    /// @param regionStart   The start of the region to delete.
    /// @param regionLength  The length of the region to delete. Defaults to \b %MAX_LEN.
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK >
    TAString&    Delete( integer regionStart, integer regionLength =MAX_LEN ) {
        ALIB_STRING_DBG_CHK(this)

        integer regionEnd;

        if constexpr ( TCheck::value ) {
            if ( base::AdjustRegion( regionStart, regionLength ) )
                return *this;

            // delete over the end?
            if ( (regionEnd= regionStart + regionLength) >= base::length ) {
                base::length= regionStart;
                return *this;
            }
        } else {
            ALIB_ASSERT_ERROR(     regionStart  >= 0
                               &&  regionStart  <= base::length   , "STRINGS",
              "Non-checking invocation: ", "Illegal arguments. Region start: 0 <= {} < {}.",
              regionStart, base::length )
            ALIB_ASSERT_ERROR( regionLength >= 0              , "STRINGS",
              "Non-checking invocation: ", "Illegal arguments. Negative region length {} given.",
              regionLength )

            // delete over the end?
            if ( (regionEnd= regionStart + regionLength) >= base::length ) {
                base::length= regionStart;
                return *this;
        }   }

        // both versions
        characters::Move( base::vbuffer + regionEnd,
                                            base::length  - regionEnd + 1,
                                            base::vbuffer + regionStart   );
        base::length-= regionLength;

        return *this;
    }

    /// Deletes the given number of characters from the start of the string by moving the
    /// rest of the string to the start and adjusting the string's length.
    ///
    /// @tparam TCheck       Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                      If \c \<false\> is added to the method name, no parameter checks are
    ///                      performed.
    ///
    /// @param regionLength  The length of the region at the start to delete.
    ///
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK >
    TAString&                 DeleteStart( integer regionLength ) {
        ALIB_STRING_DBG_CHK(this)

        if constexpr ( TCheck::value ) {
            if ( regionLength <= 0 ) {
                ALIB_STRING_DBG_CHK(this)
                return *this;
            }
            if ( regionLength >= base::length )
                return Reset();
        } else {
            ALIB_ASSERT_ERROR(  regionLength >=0 && regionLength <= base::length, "STRINGS",
                "Non-checking invocation: ", "Region length out of range: 0 <= {} < {}.",
                regionLength, base::length )
        }

        characters::Move( base::buffer + regionLength,
                          base::length - regionLength + 1,
                          base::vbuffer                     );
        base::length-= regionLength;
        return *this;
    }

    /// Deletes the given string from the start of this string. If this string does not start
    /// with the given string, nothing is done.
    ///
    /// @param deleteIfMatch  The string to be deleted at the start.
    ///
    /// @return \c *this to allow concatenated calls.
    TAString&                DeleteStart( const TString<TChar>& deleteIfMatch ) {
        ALIB_STRING_DBG_CHK(this)
        if( deleteIfMatch.IsNotEmpty() && base::StartsWith(deleteIfMatch) )
            return DeleteStart(deleteIfMatch.Length());
        return *this;
    }

    /// Reduces the length of the string by the given number of characters.
    ///
    /// @tparam TCheck       Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                      If \c \<false\> is added to the method name, no parameter checks are
    ///                      performed and any given value is just subtracted from the
    ///                      current length.
    /// @param regionLength  The length of the region at the end to delete.
    ///
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK >
    TAString&                DeleteEnd( integer regionLength  ) {
        ALIB_STRING_DBG_CHK(this)

        if constexpr ( TCheck::value ) {
            if ( regionLength > 0 ) {
                if ( regionLength >= base::length )
                    base::length= 0;
                else
                    base::length-= regionLength;
            }
        } else {
            ALIB_ASSERT_ERROR(  regionLength >=0 && regionLength <= base::length,  "STRINGS",
                   "Non-checking invocation: ", "Region length out of range: 0 <= {} < {}.",
                    regionLength, base::length )
            base::length-= regionLength;
        }

        return *this;
    }

    /// Deletes the given string from the end of this string. If this string does not end with
    /// the given string, nothing is done.
    ///
    /// @param deleteIfMatch  The string to be deleted at the end.
    ///
    /// @return \c *this to allow concatenated calls.
    TAString&                DeleteEnd( const TString<TChar>& deleteIfMatch ) {
        ALIB_STRING_DBG_CHK(this)
        if( deleteIfMatch.IsNotEmpty() && base::EndsWith(deleteIfMatch) )
            return DeleteEnd(deleteIfMatch.Length());
        return *this;
    }

    /// All characters defined in given set are removed at the beginning and at the end of this
    /// string.
    ///
    /// \see Method #TrimAt to remove whitespaces at arbitrary places in the string.
    ///
    /// @param trimChars   Pointer to a zero-terminated set of characters to be omitted.
    ///                    Defaults to \ref DEFAULT_WHITESPACES.
    /// @return \c *this to allow concatenated calls.
    ALIB_DLL
    TAString& Trim( const TCString<TChar>& trimChars
                                            = CStringConstantsTraits<TChar>::DefaultWhitespaces() );

    /// All characters defined in given set at, left of and right of the given index
    /// are removed from the string.<br>
    /// The method returns index of the first character of those characters that were behind the
    /// trimmed region. With legal \p{idx} given, this value can only be smaller or equal to
    /// \p{idx}. If \p{idx} is out of bounds, the length of the string is returned.
    ///
    /// @param idx         The index to perform the trim operation at. Has to be between zero
    ///                    and <em>Length() -1</em>.
    /// @param trimChars   Pointer to a zero-terminated set of characters to be omitted.
    ///                    Defaults to \ref DEFAULT_WHITESPACES.
    /// @return The index of the first character of those characters that were behind the
    ///         trimmed region.
    ALIB_DLL
    integer  TrimAt( integer idx, const TCString<TChar>& trimChars
                                            = CStringConstantsTraits<TChar>::DefaultWhitespaces() );

    /// All characters defined in given set are removed at the beginning of this string.
    ///
    /// \see Method #TrimAt to remove whitespaces at arbitrary places in the string.
    ///
    /// @param trimChars   Pointer to a zero-terminated set of characters to be omitted.
    ///                    Defaults to \ref DEFAULT_WHITESPACES.
    /// @return \c *this to allow concatenated calls.
    TAString& TrimStart( const TCString<TChar>& trimChars
                                           = CStringConstantsTraits<TChar>::DefaultWhitespaces() ) {
        if (base::length == 0 || trimChars.IsEmpty() )
            return *this;

        integer idx= base::template IndexOfAny<lang::Inclusion::Exclude, NC>( trimChars );
        if ( idx > 0 )
            Delete<NC>( 0, idx );
        else if ( idx < 0 )
            base::length= 0;

        return *this;
    }

    /// All characters defined in given set are removed at the end of this string.
    ///
    /// \see Method #TrimAt to remove whitespaces at arbitrary places in the string.
    ///
    /// @param trimChars   Pointer to a zero-terminated set of characters to be omitted.
    ///                    Defaults to \ref DEFAULT_WHITESPACES.
    /// @return \c *this to allow concatenated calls.
    TAString& TrimEnd( const TCString<TChar>& trimChars
                                           = CStringConstantsTraits<TChar>::DefaultWhitespaces() ) {
        if( base::length > 0 &&  trimChars.IsNotEmpty() )
            base::length=
                base::template LastIndexOfAny<lang::Inclusion::Exclude, NC>(
                                             trimChars, base::length - 1 ) + 1;
        return *this;
    }

//##################################################################################################
/// @name Replace
//##################################################################################################

    /// Replaces a region in this object by a given string.
    /// The given region is adjusted to this string's bounds.
    ///
    /// The non-checking version does not adjust the region and
    /// \ref alib_mod_assert "raises an ALib error" in debug-compilations if the given region is
    /// out of bounds.
    ///
    /// @tparam TCheck         Chooses checking or non-checking implementation.
    ///                        Defaults to \alib{CHK}.
    /// @param src             The replacement string.
    /// @param regionStart     The start of the region.
    /// @param regionLength    The length of the region.
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TAString&   ReplaceSubstring( const TString<TChar>& src,
                                  integer regionStart, integer regionLength ) {
        ALIB_STRING_DBG_CHK(this)
        if constexpr ( TCheck::value ) {
            base::AdjustRegion( regionStart, regionLength );
        } else {
            ALIB_ASSERT_ERROR( src.IsNotNull(), "STRINGS",
                               "Non-checking invocation: ", "Source string is nulled" )
            #if ALIB_DEBUG
                integer rs=  regionStart;
                integer rl=  regionLength;
                base::AdjustRegion( rs, rl );
                ALIB_ASSERT_ERROR( rs == regionStart && rl == regionLength, "STRINGS",
                    "Non-checking invocation: ", "Invalid region {}/{} given. Adjusted: {}/{}",
                    regionStart, regionLength, rs, rl )

            #endif
        }

        integer lenDiff= src.Length() - regionLength;

        // check buffer size
        if ( lenDiff > 0 )
            EnsureRemainingCapacity( lenDiff );

        // move content
        if ( lenDiff != 0 )
            characters::Move( base::vbuffer +  regionStart + regionLength,
                                                base::length  - (regionStart + regionLength),
                                                base::vbuffer +  regionStart + src.Length()  );

        // copy the source
        src.CopyTo( base::vbuffer + regionStart );
        base::length+= lenDiff;

        return *this;
    }

    /// Replaces a region in the string with the given character.
    /// The given region is adjusted to this string's bounds. If the adjusted region is empty,
    /// nothing is done.
    ///
    /// The non-checking version does not adjust the region.
    ///
    /// @tparam TCheck         Chooses checking or non-checking implementation.
    ///                        Defaults to \alib{CHK}.
    /// @param regionStart     The start of the region
    /// @param regionLength    The length of the region
    /// @param c               The character to set in the region.
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TAString&   ReplaceRegion( TChar c, integer regionStart, integer regionLength ) {
        if constexpr ( TCheck::value ) {
            if ( base::AdjustRegion( regionStart, regionLength ) )
                return *this;
        } else {
            #if ALIB_DEBUG
                integer rs=  regionStart;
                integer rl=  regionLength;
                base::AdjustRegion( rs, rl );
                ALIB_ASSERT_ERROR( rs == regionStart && rl == regionLength, "STRINGS",
                    "Non-checking invocation: ", "Invalid region {}/{} given. Adjusted: {}/{}",
                    regionStart, regionLength, rs, rl )
            #endif
        }

        characters::Fill( base::vbuffer + regionStart, regionLength, c );
        return *this;
    }

    /// Replaces all occurrences of character \p{needle} found at or behind position \p{startIdx}
    /// by character \p{replacement}.
    ///
    /// @param needle           The character to search.
    /// @param replacement      The replacement character.
    /// @param startIdx         The index where the search ends. Optional and defaults to \c 0.
    /// @param endIdx           The index where the search ends. Precisely, the index of the
    ///                         first character that is not replaced.
    ///                         Defaults to \alib{strings;MAX_LEN}.
    ///
    /// @return The number of replacements that where performed.
    ALIB_DLL
    integer SearchAndReplace( TChar          needle,
                              TChar          replacement,
                              integer        startIdx            = 0,
                              integer        endIdx              =strings::MAX_LEN  );

    /// Replaces up to \p{maxReplacements} occurrences of string \p{needle} found
    /// at or behind position \p{startIdx} by string \p{replacement} .
    ///
    /// @param needle           The string to be searched and replaced.
    /// @param replacement      The replacement string.
    /// @param startIdx         The index where the search starts. Optional and defaults 0.
    /// @param maxReplacements  The maximum number of replacements to perform.
    ///                         Optional and defaults to \b %MAX_LEN.
    /// @param sensitivity      Case sensitivity of the comparison.
    ///                         Optional and defaults to Case::Sensitive.
    /// @param endIdx           The index where the search ends. Precisely, the index of the
    ///                         first character that is not tested to be the start of
    ///                         \p{replacement}.
    ///                         Defaults to \alib{strings;MAX_LEN}.
    ///
    /// @return The number of replacements that where performed.
    ALIB_DLL
    integer SearchAndReplace( const TString<TChar>&  needle,
                              const TString<TChar>&  replacement,
                              integer                startIdx        = 0,
                              integer                maxReplacements =strings::MAX_LEN,
                              lang::Case             sensitivity     = lang::Case::Sensitive,
                              integer                endIdx          =strings::MAX_LEN  );

    /// Converts all or a region of characters in the Buffer to upper case.
    ///
    /// @param regionStart     Start of the region to be converted. Defaults to 0
    /// @param regionLength    Length of the region to be converted. Defaults to \b %MAX_LEN.
    ///
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TAString& ToUpper( integer regionStart= 0, integer regionLength =MAX_LEN ) {
        if constexpr ( TCheck::value ) {
            if ( base::AdjustRegion( regionStart, regionLength ) )
                return *this;
        } else {
            #if ALIB_DEBUG
                integer rs=  regionStart;
                integer rl=  regionLength;
                base::AdjustRegion( rs, rl );
                ALIB_ASSERT_ERROR( rs == regionStart && rl == regionLength, "STRINGS",
                    "Non-checking invocation: ", "Invalid region {}/{} given. Adjusted: {}/{}",
                    regionStart, regionLength, rs, rl )
            #endif
        }

        characters::ToUpper( base::vbuffer + regionStart, regionLength );
        return *this;
    }


    /// Converts all or a region of characters in the Buffer to lower case.
    ///
    /// @param regionStart     Start of the region to be converted. Defaults to 0
    /// @param regionLength    Length of the region to be converted. Defaults to \b %MAX_LEN.
    ///
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TAString& ToLower( integer regionStart= 0, integer regionLength =MAX_LEN ) {
        if constexpr ( TCheck::value ) {
            if ( base::AdjustRegion( regionStart, regionLength ) )
                return *this;
        } else {
            #if ALIB_DEBUG
                integer rs=  regionStart;
                integer rl=  regionLength;
                base::AdjustRegion( rs, rl );
                ALIB_ASSERT_ERROR( rs == regionStart && rl == regionLength, "STRINGS",
                    "Non-checking invocation: ", "Invalid region {}/{} given. Adjusted: {}/{}",
                    regionStart, regionLength, rs, rl )
            #endif
        }

        characters::ToLower( base::vbuffer + regionStart, regionLength );
        return *this;
    }

    /// Reverses the order of the characters of this string (or a region hereof).
    ///
    /// @param regionStart     Start of the region to be reversed. Defaults to 0
    /// @param regionLength    Length of the region to be reversed. Defaults to \b %MAX_LEN.
    ///
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TAString& Reverse( integer regionStart= 0, integer regionLength =MAX_LEN ) {
        if constexpr ( TCheck::value ) {
            if ( base::AdjustRegion( regionStart, regionLength ) )
                return *this;
        } else {
            #if ALIB_DEBUG
                integer rs=  regionStart;
                integer rl=  regionLength;
                base::AdjustRegion( rs, rl );
                ALIB_ASSERT_ERROR( rs == regionStart && rl == regionLength, "STRINGS",
                    "Non-checking invocation: ", "Invalid region {}/{} given. Adjusted: {}/{}",
                    regionStart, regionLength, rs, rl )
            #endif
        }

        characters::Reverse( base::vbuffer + regionStart, regionLength );
        return *this;
    }

  public:
    using base::begin;
    using base::end;
    using base::rbegin;
    using base::rend;

    /// A \c std::iterator_traits type, implementing the standard library concept of
    /// \https{RandomAccessIterator,en.cppreference.com/w/cpp/concept/RandomAccessIterator}.
    /// While parent class \b %String provides a constant iterator only, this class exposes
    /// an iterator that allows the modification of the character an iterator references.
    using iterator= typename base::template TRandomAccessIterator<TChar>;

    /// Same as #iterator, but working from the end to the start of the string.
    using reverse_iterator  = std::reverse_iterator<iterator>;

  //################################################################################################
  /// @name std::iterator_traits
  //################################################################################################

    /// Returns an iterator pointing to a constant character at the start of this string.
    /// @return The start of this string.
    iterator         begin()                                  { return iterator(  base::vbuffer ); }

    /// Returns an iterator pointing behind this string.
    /// @return The end of this string.
    iterator         end()                    { return iterator(  base::vbuffer + base::length  ); }

    /// Returns a reverse iterator pointing to a constant character at the end of this string.
    /// @return The last character of this string.
    reverse_iterator rbegin()                                { return reverse_iterator( end()   ); }

    /// Returns a reverse iterator pointing before the start of this string.
    /// @return The character before this string.
    reverse_iterator rend()                                  { return reverse_iterator( begin() ); }

    /// Appends the given character to this \b AString.<br>
    /// Needed, for example, to make this type compatible with <c>std::back_insert_iterator</c>.
    /// @param ch The character to append to the end of this string.
    constexpr void   push_back( TChar ch )                                         { Append( ch ); }

    /// Pops one character from the end of this string.<br>
    /// With debug-compilations, an assertion is raised in case this string is empty.
    /// @return The character that got removed.
    constexpr TChar  pop_back() {
        ALIB_ASSERT_ERROR(  base::length > 0,  "STRINGS", "pop_back called on empty string" )
        return base::vbuffer[ --base::length ];
    }

}; // class TAString

//##################################################################################################
// TAString type aliases
//##################################################################################################
}

DOX_MARKER( [DOX_MONOMEM_ALLOCMEMBER])
/// Type alias in namespace \b alib.
using AString=  strings::TAString<character, lang::HeapAllocator>;
DOX_MARKER( [DOX_MONOMEM_ALLOCMEMBER])

/// Type alias in namespace \b alib.
using  ComplementAString=     strings::TAString   <complementChar  , lang::HeapAllocator>;

/// Type alias in namespace \b alib.
using  StrangeAString   =     strings::TAString   <strangeChar     , lang::HeapAllocator>;

/// Type alias in namespace \b alib.
using  NAString         =     strings::TAString   <nchar          , lang::HeapAllocator>;

/// Type alias in namespace \b alib.
using  WAString         =     strings::TAString   <wchar          , lang::HeapAllocator>;

/// Type alias in namespace \b alib.
using  XAString         =     strings::TAString   <xchar          , lang::HeapAllocator>;

//##################################################################################################
// Specializations of ArrayTraits for this class String
//##################################################################################################
namespace characters {
#if !DOXYGEN
template<typename TChar, typename TAllocator>
struct ArrayTraits<strings::TAString<TChar,TAllocator>, TChar>
{
    using T= strings::TAString<TChar,TAllocator>;
    static constexpr Policy       Access                               = Policy::Implicit;
    static constexpr Policy       Construction                         = Policy::ExplicitOnly;
    static constexpr const TChar* Buffer   (const T& src)                   { return src.Buffer(); }
    static constexpr integer      Length   (const T& src)                   { return src.Length(); }
    static constexpr T            Construct(const TChar* b, integer l)  { return T().Append(b, l); }
};

template<typename TChar, typename TAllocator>
struct ZTArrayTraits<strings::TAString<TChar,TAllocator>, TChar>
{
    using T= strings::TAString<TChar,TAllocator>;
    static constexpr Policy       Access                               = Policy::Implicit;
    static constexpr Policy       Construction                         = Policy::ExplicitOnly;
    static constexpr const TChar* Buffer   (const T& src)                { return src.Terminate(); }
    static constexpr integer      Length   (const T& src)                   { return src.Length(); }
    static constexpr T            Construct(const TChar* b, integer l)  { return T().Append(b, l); }
};
#endif // !DOXYGEN
} namespace strings {


/// This is a simple utility class that can be used in situations where a \alib{strings;TAString;AString}
/// is intermediately extended and later shortened back to its original length.
/// With the use of this class, C++ stack-unwinding is used to ensure that the length is reset
/// in all execution paths including exception handling, etc.
///
/// \see Macro \ref ALIB_STRING_RESETTER which automatically creates a unique identifier for
///      the otherwise unused instances of this type.
///
/// @tparam TChar      The character type of the \b %AString that is to be reset.<br>
/// @tparam TAllocator The allocator type of the \b %AString, as prototyped with
///                    \alib{lang;Allocator}.
template<typename TChar, typename TAllocator>
class TStringLengthResetter
{
  protected:
    TAString<TChar,TAllocator>&  aString;        ///< The \b AString to reset.
    integer                      originalLength; ///< The \b The original length of the string.

  private:
    /// Private new to disallow heap allocation.
    /// @return Never called.
    void* operator new  (size_t);
    /// Private new to disallow heap allocation.
    /// @return Never called.
    void* operator new  (size_t, void*);
    /// Private new to disallow heap allocation.
    /// @return Never called.
    void* operator new[](size_t);
    /// Private new to disallow heap allocation.
    /// @return Never called.
    void* operator new[](size_t, void*);
    /// Private assignment operator.
    void  operator                                                 =(const TStringLengthResetter& );

  public:
    /// Constructor taking the string. Stores the current length in field #originalLength.
    /// @param pAString  The String to take the length of and reset on destruction.
    TStringLengthResetter( TAString<TChar,TAllocator>& pAString )
    : aString(pAString)
    , originalLength(pAString.Length())                                                           {}

    /// Destructor. Restores the string's original length.
    ~TStringLengthResetter()                                  { aString.ShortenTo(originalLength); }

    /// Resets the strings to the original length before destruction of this object.
    /// \note With using macro \ref ALIB_STRING_RESETTER, this method is not invocable, because
    ///       the name of the object is not available in this case. But this method is not
    ///       a true member of the usual use case of this class.
    void ResetNow()                                           { aString.ShortenTo(originalLength); }

    /// Returns the original length.
    /// @return The length of the \b AString when this object was constructed.
    integer OriginalLength()                                              { return originalLength; }
}; // class TStringLengthResetter

}
/// Type alias in namespace \b alib.
using  StringLengthResetter          =     strings::TStringLengthResetter<character     ,lang::HeapAllocator>;

/// Type alias in namespace \b alib.
using  ComplementStringLengthResetter=     strings::TStringLengthResetter<complementChar,lang::HeapAllocator>;

/// Type alias in namespace \b alib.
using  StrangeStringLengthResetter   =     strings::TStringLengthResetter<strangeChar   ,lang::HeapAllocator>;

/// Type alias in namespace \b alib.
using  NStringLengthResetter         =     strings::TStringLengthResetter<nchar         ,lang::HeapAllocator>;

/// Type alias in namespace \b alib.
using  WStringLengthResetter         =     strings::TStringLengthResetter<wchar         ,lang::HeapAllocator>;

/// Type alias in namespace \b alib.
using  XStringLengthResetter         =     strings::TStringLengthResetter<xchar         ,lang::HeapAllocator>;

} // namespace [alib::strings]

// Note(25/01/17):
//   Clang strangely did not find the following templated operators when they resided in an
//   exported namespace.
//   The workaround was to not export the namespace but export each operator instead.
//   We think this is wrong behavior and not aligned with the language specification.
#if !DOXYGEN
namespace alib::strings {

ALIB_EXPORT
template<typename TChar, typename TAllocator>
bool operator== (const TAString<TChar, TAllocator>& lhs, const TAString<TChar,TAllocator>& rhs)
{ return  lhs. template Equals   <CHK, lang::Case::Sensitive>(rhs); }

ALIB_EXPORT
template<typename TChar, typename TAllocator, typename T>
requires (!std::is_same_v<T, TAString<TChar,TAllocator>>)
bool operator== (const TAString<TChar,TAllocator>& lhs, const T&     rhs)
{ return  lhs. template Equals   <CHK, lang::Case::Sensitive>(rhs); }

ALIB_EXPORT
template<typename TChar, typename TAllocator>
auto operator<=> (const TAString<TChar,TAllocator>& lhs, const TAString<TChar,TAllocator>& rhs)
{ return  lhs. template CompareTo<CHK, lang::Case::Sensitive>(rhs); }

ALIB_EXPORT
template<typename TChar, typename TAllocator, typename T>
requires (!std::same_as<TAString<TChar,TAllocator>, T>)
auto operator<=> (const TAString<TChar,TAllocator>& lhs, const T&     rhs)
{ return  lhs. template CompareTo<CHK, lang::Case::Sensitive>(rhs); }

} // namespace [alib::strings]
#endif // !DOXYGEN
