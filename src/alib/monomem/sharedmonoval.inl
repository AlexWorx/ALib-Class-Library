//==================================================================================================
/// \file
/// This header-file is part of module \alib_monomem of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace monomem {

//==================================================================================================
/// This templated class is a utility type for \alib{monomem;TMonoAllocator} and supports the
/// following mechanics:
/// - It creates a first \alib{monomem;detail::Buffer;buffer} usable by a \b MonoAllocator.
/// - A custom type defined by template parameter \p{T} is placed at the beginning of
///   that buffer.
/// - Along with the custom type, the \b MonoAllocator that receives this first buffer is likewise
///   placed in the buffer.
/// - Finally, an atomic usage counter is placed as a third member inside that buffer.
/// - Only one pointer into this first buffer is stored with this type.
/// - This type overloads #operator->() and #operator*() to access the members of the custom type.
/// - The concept of an "automatic pointer", similar to <c>std::shared_ptr</c> is
///   implemented with this type.
/// - The default-constructor of this type just sets the internal pointer to \c nullptr.
///
/// All of the above results in the following:
/// - The size of an instance of this class is equal to the size of a single C++ pointer.
///   The only member is a pointer to an object of internal type
///   \alib{monomem::TSharedMonoVal;FieldMembers}.
/// - A single dynamic memory allocation is performed to create an instance of the class, which
///   holds all data and allows further monotonic allocations.
/// - Values of this type are \e nulled, when default constructed.
/// - Values of this type can be shared (copied as value), which increases the use counter.
/// - Values of this type can be moved, which keeps the use counter as is.
/// - Values of this type can be emptied by assigning \c nullptr or invoking #SetNulled.
/// - At the moment that the last copy gets out of scope, is deleted or \e nulled,
///   the contained object is destructed, and all monotonic memory is freed.
/// - The allocator can be received with #GetAllocator and used by the contained type.
///   A derived type may volunteer to publish the allocator as well to be used by any
///   entity that gets access to a copy of the automatic pointer.
/// - Member access is performed with the overloaded <c>operator->()</c>.
///   Alternatively (and sometimes needed) other provided operators and methods like #Get may
///   be used.
///
/// In contrast to the container types \alib{containers;SharedVal} and \alib{containers;SharedPtr},
/// this type is <b>most commonly used as a base class</b> of types that should to be fully
/// self-contained (like the types listed below).
///
/// \see
///   - A step by step sample that can be used to bootstrap custom usages is presented in chapter
///     \ref alib_contmono_smv of the Programmer's Manual of this \alibmod.
///   - The following types found across \alib may be reviewed for understanding this type's usage
///     and its potential use cases:
///     - Class \alib{variables;TSharedConfiguration},
///     - Class \alib{files;TSharedFTree}, and
///     - Class \alib{exceptions;Exception}.
///   - Chapter \ref alib_contmono_smv_locking of the Programmer's Manual of module
///     \alib_monomem_nl.
///
/// @tparam T           The custom type that is embedded along with the \b MonoAllocator.
/// @tparam TAllocator  The \ref alib_contmono_chaining "chained allocator" that is used by the
///                     monotonous allocator that this type creates inside its first buffer.<br>
///                     Usually, type \alib{lang;HeapAllocator} is to be given here.
/// @tparam TLock       The type of \ref alib_threads_locks "ALib lock" to embed besides \p{T}.
///                     With the provision of <c>void</c>, the integration of a lock is
///                     suppressed.<br>
///                     See chapter \ref alib_contmono_smv_locking of the Programmer's Manual of
///                     module \alib_monomem_nl for further details.
//==================================================================================================
template<typename T, typename TAllocator, typename TLock>
class TSharedMonoVal
{
  protected:
    /// Same as \alib{monomem::TSharedMonoVal;FieldMembersWithLock} but missing the lock.
    /// This type is chosen if template parameter \p{TLock} equals <c>void</c>.<br>
    /// The internals of this type are not further documented.
    struct FieldMembersNoLock
    {
        #if !DOXYGEN
        lang::Placeholder<T>        custom;
        TMonoAllocator<TAllocator>  allocator;
        std::atomic<unsigned>       refCount;

        FieldMembersNoLock( TAllocator&     pAllocator,
                            detail::Buffer* firstBuffer,
                            size_t          initialBufferSize,
                            unsigned        bufferGrowthInPercent )
        : allocator( pAllocator, firstBuffer, initialBufferSize, bufferGrowthInPercent )
        , refCount(1)                                                                             {}

        template<typename TRequires= TAllocator>
        requires std::default_initializable<TRequires>
        FieldMembersNoLock( detail::Buffer* firstBuffer,
                            size_t          initialBufferSize,
                            unsigned        bufferGrowthInPercent )
        : allocator(ALIB_DBG("ShardMonoVal",) firstBuffer, initialBufferSize, bufferGrowthInPercent)
        , refCount(1)                                                                             {}
        #endif
    }; // struct FieldMembersNoLock;

    /// The combined struct of members that are allocated in the first buffer of the
    /// monotonic allocator.
    struct FieldMembersWithLock
    {
        /// The space for the custom member. The instance will be constructed using placement-new.
        lang::Placeholder<T>        custom;

        /// The allocator that this class is contained in.
        TMonoAllocator<TAllocator>  allocator;

        /// The reference counter used to implement the <c>std::shared_ptr</c> behavior.
        std::atomic<unsigned>       refCount;

        /// The embedded lock.
        TLock                       lock;

        /// Constructor.
        /// Creates the allocator and initializes #refCount. The #custom contents will be
        /// constructed by an obligatory, separated call to #ConstructT.
        /// @param pAllocator            The chained allocator of the monotonic allocator.
        /// @param firstBuffer           The first argument to field #allocator.
        /// @param initialBufferSize     The second argument to field #allocator.
        /// @param bufferGrowthInPercent Optional growth factor in percent, applied to the buffer size
        ///                              with each next buffer allocation.
        ///                              Values provided should be greater than 100.<p>
        FieldMembersWithLock( TAllocator&     pAllocator,
                              detail::Buffer* firstBuffer,
                              size_t          initialBufferSize,
                              unsigned        bufferGrowthInPercent )
        : allocator( pAllocator, firstBuffer, initialBufferSize, bufferGrowthInPercent )
        , refCount(1)                                                                             {}

        /// Alternative constructor missing the allocator instance.
        /// This is used only with allocators that are default-constructible
        /// (like \alib{lang;HeapAllocator} is).
        /// @tparam TRequires            Defaulted template parameter. Must not be specified.
        /// @param firstBuffer           The first argument to field #allocator.
        /// @param initialBufferSize     The second argument to field #allocator.
        /// @param bufferGrowthInPercent Optional growth factor in percent, applied to the buffer
        ///                              size with each next buffer allocation.
        ///                              Values provided should be greater than 100.<p>
        template< typename TRequires= TAllocator>
        requires std::default_initializable<TRequires>
        FieldMembersWithLock( detail::Buffer* firstBuffer,
                              size_t          initialBufferSize,
                              unsigned        bufferGrowthInPercent )
        : allocator( ALIB_DBG("ShardMonoVal",)
                     firstBuffer, initialBufferSize, bufferGrowthInPercent )
        , refCount(1)                                                                             {}
    }; // struct FieldMembersWithLock;

    #if !DOXYGEN
        #if ALIB_DEBUG && !DOXYGEN
    void dbgassert()         const { ALIB_ASSERT_ERROR(members,"MONOMEM", "Empty shared instance") }
        #else
            void dbgassert() const {}
        #endif
    #endif

    /// The type of field members to include.
    /// Dependent on the template parameter \p{TLock}, either
    /// \alib{monomem::TSharedMonoVal;FieldMembersNoLock} or
    /// \alib{monomem::TSharedMonoVal;FieldMembersWithLock} is chosen.
    using FieldMembers= std::conditional_t<std::same_as<void, TLock>, FieldMembersNoLock, FieldMembersWithLock>;

    /// The object that is placed in the allocator's first buffer.
    /// Contains \p{T}, the allocator itself, and a reference counter.
    FieldMembers*     members;

  //================================================================================================
  //===  The self-contained implementation of this type
  //================================================================================================
  public:
    /// Exposes the monotonic allocator used. Equals to <c>TMonoAllocator<TAllocator></c>.
    using AllocatorType = TMonoAllocator<TAllocator>;

    /// Exposes the stored type specified with template parameter \p{T}.
    using StoredType    = T;

    /// Exposes the lock type specified with template parameter \p{TLock}.
    using LockType      = TLock;

    /// Constructs an initial buffer of given size and creates the mono allocator within.
    ///
    /// \attention
    ///   The instance of custom type \p{T} will \b not be constructed with
    ///   any constructor of this type.
    ///   Instead, an explicit call to #ConstructT has to be made after construction!
    ///
    /// @param allocator             The (chained) allocator used to create the initial buffer
    ///                              and passed to the monotonous allocator that is created.
    /// @param initialBufferSizeInKB The initial size of memory buffers used with the monotonic
    ///                              allocator in kB (1024 bytes)
    /// @param bufferGrowthInPercent Optional growth factor in percent, applied to the buffer size
    ///                              with each next buffer allocation.
    ///                              Should be set to \c 200, to double the size with each
    ///                              allocation.
    TSharedMonoVal( TAllocator& allocator,
                   size_t initialBufferSizeInKB, unsigned bufferGrowthInPercent ) {
        auto size= initialBufferSizeInKB * 1024;
        void* mem= allocator.allocate( size, alignof(detail::Buffer) );
        auto* buffer= new (mem) detail::Buffer( size );

        members=  reinterpret_cast<FieldMembers*>( buffer->allocate( sizeof( FieldMembers),
                                                                     alignof(FieldMembers) ) );
        ALIB_ASSERT_ERROR(members, "MONOMEM",
          "Initial buffer size {} to small to hold shared value of size {}.",
          size, sizeof( FieldMembers) )
        new (members) FieldMembers( allocator, buffer, size, bufferGrowthInPercent );
    }

    /// Constructor missing the allocator instance.
    /// To be used only with allocators that are default-constructible
    /// (like \alib{lang;HeapAllocator} is).
    ///
    /// \attention
    ///   The instance of custom type \p{T} will \b not be constructed with
    ///   any constructor of this type.
    ///   Instead, an explicit call to #ConstructT has to be made after construction!
    ///
    /// @param initialBufferSizeInKB The initial size of memory buffers used with the monotonic
    ///                              allocator in kB (1024 bytes)
    /// @param bufferGrowthInPercent Optional growth factor in percent, applied to the buffer size
    ///                              with each next buffer allocation.
    ///                              Should be set to \c 200, to double the size with each
    ///                              allocation.
    TSharedMonoVal( size_t initialBufferSizeInKB, unsigned bufferGrowthInPercent ) {
        auto size= initialBufferSizeInKB * 1024;
        void* mem= TAllocator().allocate( size, alignof(detail::Buffer) );
        auto* buffer= new (mem) detail::Buffer( size );
        members=  reinterpret_cast<FieldMembers*>( buffer->allocate( sizeof (FieldMembers),
                                                                     alignof(FieldMembers) ) );
        ALIB_ASSERT_ERROR(members, "MONOMEM",
          "Initial buffer size {} to small to hold shared value of size {}.",
          size, sizeof( FieldMembers) )
        new (members) FieldMembers( buffer, size, bufferGrowthInPercent );
    }

    /// Destructor. If this is the last copy, the destructors of \p{T} and of the
    /// \b MonoAllocator are invoked.
    ~TSharedMonoVal() {
        ALIB_STATIC_ASSERT( Locks_not_supported_when_module_ALibThreads_is_exlcuded,
                            !ALIB_SINGLE_THREADED || std::same_as<void ALIB_COMMA TLock>,
        "Template parameter TLock of class TSharedMonoVal must be <void> if module ALib Threads is "
        "not included in the ALib Build."  )

        if (members && members->refCount.fetch_sub(1) == 1) {
            members->custom.Destruct();          // Destruct the contained object first,
            lang::Destruct( members->allocator); // then the allocator.
    }   }

    /// Resets the monotonic allocator that this object is contained in to the snapshot created
    /// right after construction.
    /// The allocated memory buffers will remain allocated and reused. Before resetting,
    /// the destructor of the custom object \p{T} is invoked, and after
    /// the reset, in-place construction is performed.
    ///
    /// All shared instances remain valid (while, of course, their content is likewise reset).
    ///
    /// It is up to the implementation of the derived class if this method should be exposed
    /// or not. It is also up to the implementation of the derived class if the internal allocator
    /// should be exposed for 3rd-party usage or not.
    ///
    /// @tparam  TArgs    The argument types used for re-constructing \p{T}.
    /// @param   args     The arguments to re-construct the instance of \p{T}.
    template<typename... TArgs>
    void    Reset( TArgs&&... args ) {
        // Destruct custom object first
        members->custom.Destruct();

        // reset allocator to behind members
        members->allocator.Reset( sizeof(FieldMembers), alignof(FieldMembers) );

        // construct custom object again
        new (&members->custom) T( std::forward<TArgs>(args)... );
    }

    /// Constructs the custom members.
    /// This method <b>has to be called</b> right after this instance was created with a
    /// non-nulled state.<br>
    /// Usually, this is done in the constructor of a dedicated derived type.
    ///
    /// \note
    ///   The construction of the contained type is intentionally deferred to the (therefore
    ///   mandatory) call of this method.
    ///   The reason is that this way, the monotonic allocator is accessible with the method #
    ///   GetAllocator() and valid and thus can be used here.
    /// @tparam TArgs  The argument types used for constructing \p{T}.
    /// @param  args   The arguments to construct the instance of \p{T}.
    template<typename... TArgs>
    void ConstructT( TArgs&&... args ) { members->custom.Construct(std::forward<TArgs>(args)... ); }

    /// @return The size of the memory that is allocated for the \p{T} as well as for
    ///         the reference counter and the allocator member.
    ///         (To whom it may concern.)
    static constexpr size_t SizeOfAllocation()                      { return sizeof(FieldMembers); }

    /// @return The monotonic allocator that this object has created and embedded itself in.
    ///         It may be used for custom allocations and especially may be passed to the
    ///         constructor of \p{T} with the method #ConstructT for further use.
    AllocatorType&  GetAllocator()              noexcept { dbgassert(); return members->allocator; }

    /// Returns a non-constant reference to the stored object of type \p{T}.<br>
    /// This can be used as an alias to <c>(**this)</c>.
    /// @return A reference to \p{T}.
    T&              Self()                        noexcept { dbgassert(); return *members->custom; }

    /// Returns a constant reference to the stored object of type \p{T}.<br>
    /// This can be used as an alias to <c>(**this)</c>.
    /// @return A constant reference to \p{T}.
    const T&        Self()                  const noexcept { dbgassert(); return *members->custom; }

    /// Overloaded operator to access members of custom type \p{T}
    /// @return A pointer to \p{T}.
    T*              operator->()             noexcept { dbgassert(); return members->custom.Get(); }

    /// Overloaded operator to access members of custom type \p{T}
    /// @return A constant pointer to \p{T}.
    const T*        operator->()       const noexcept { dbgassert(); return members->custom.Get(); }

    /// Overloaded operator to access members of the custom type \p{T}
    /// @return A pointer to \p{T}.
    T&              operator*()                   noexcept { dbgassert(); return *members->custom; }

    /// Overloaded operator to access members of the custom type \p{T}
    /// @return A constant pointer to \p{T}.
    const T&        operator*()             const noexcept { dbgassert(); return *members->custom; }


  //================================================================================================
  //===  The automatic pointer implementation of this type
  //================================================================================================
    /// Default Constructor. Leaves this object \e nulled.
    TSharedMonoVal()                                               noexcept : members(nullptr)    {}

    /// Constructs an empty instance from \c std::nullptr.
    /// This constructor is necessary to allow assignment of \c std::nullptr to values of this type,
    /// which clears the automatic pointer.
    /// \note As the common way to use this class is to derive an own type, this own type should
    ///       have this same constructor. Only then, the assignment of \c std::nullptr is possible.
    TSharedMonoVal(std::nullptr_t)                                 noexcept : members(nullptr)    {}

    /// Copy Constructor. Increases the reference counter of the shared pointer (in case given
    /// \p{other} is not nulled).
    /// @param other The object to copy.
    TSharedMonoVal(const TSharedMonoVal& other)                                             noexcept
    : members(other.members)                                  { if(members) ++(members->refCount); }


    /// Move Constructor. Does not increase the reference counter, instead nulls the \p{other}.
    /// @param other The object to copy.
    TSharedMonoVal(TSharedMonoVal&& other)                                                  noexcept
    : members(other.members)                                             { other.members= nullptr; }

    /// Copy Assignment Operator. Cares for self-assignment and assignment of a shared pointer with
    /// the same content.
    /// Otherwise, the reference counter of the current object is decreased, disposed if
    /// necessary, and then the object in \p{other} is copied to this object.
    /// @param other The object to copy into this one.
    /// @return A reference to \c this.
    TSharedMonoVal& operator=(const TSharedMonoVal& other)                                noexcept {
        // handle self assignment and assignment with same contents
        if (this == &other || this->members == other.members)
            return *this;

        // decrement the old reference count and delete the old data if needed
        if (members && members->refCount.fetch_sub(1) == 1) {
            members->custom.Destruct();          // Destruct the contained object first,
            lang::Destruct( members->allocator); // then the allocator.
        }

        // copy the new data
        if((members= other.members) != nullptr)
            ++(members->refCount);

        return *this;
    }

    /// Move Assignment Operator. Cares for self-assignment.
    /// Otherwise, the object in \p{other} is copied to this.
    /// @param other The object to move into this one.
    /// @return A reference to \c this.
    TSharedMonoVal& operator=(TSharedMonoVal&& other)                                     noexcept {
        // handle self assignment
        if (this == &other)
            return *this;

        // decrement the old reference count and delete the old data if needed
        if (members && members != other.members && members->refCount.fetch_sub(1) == 1) {
            members->custom.Destruct();          // Destruct the contained object first,
            lang::Destruct( members->allocator); // then the allocator.
        }

        // move members
        members= other.members;
        other.members= nullptr;
        return *this;
    }

    /// Returns the number of shared usages.
    /// In a multithreaded environment, the value returned is approximate.
    /// @return \c The number of shared usages.
    ///            If this instance was default-constructed, moved, method #SetNulled was called,
    ///            or \c nullptr was assigned, then \c 0 is returned.
    unsigned          UseCount()                                                      const noexcept
    { return members != nullptr ? members->refCount.load() : 0; }

    /// Returns \c true if the #UseCount is \c 1.
    /// @return \c true if this instance is set but not shared.
    bool              Unique()   const noexcept { return members && members->refCount.load() == 1; }

    /// Sets this object to \e nulled state, as if default constructed or \c nullptr was assigned.
    /// If no shared copy exists, all data is destructed and memory is freed.<br>
    /// As an alternative to this method, \c nullptr can be assigned.
    void              SetNulled()              noexcept { lang::Destruct(*this); members= nullptr; }

    /// Returns \c true if this is an empty instance.
    /// @return \c true if #UseCount is \c 0, \c false otherwise.
    bool              IsNulled()                       const noexcept { return members == nullptr; }

    /// Assignment of <c>nullptr</c>. Same as #SetNulled.
    void              operator=(std::nullptr_t)         { lang::Destruct(*this); members= nullptr; }

    /// Comparison with <c>nullptr</c>.
    /// @return \c true if #UseCount is greater than \c 0.
    bool              operator==(std::nullptr_t)       const noexcept { return members == nullptr; }

    /// Comparison with <c>nullptr</c>.
    /// @return \c false if #UseCount is greater than \c 0.
    bool              operator!=(std::nullptr_t)       const noexcept { return members != nullptr; }

    /// Returns a non-constant pointer to the stored object of type \p{T}.
    /// @return A pointer to \p{T}.
    T*       Get()                           noexcept { dbgassert(); return members->custom.Get(); }

    /// Returns a constant reference to the stored object of type \p{T}.
    /// @return A constant reference to \p{T}.
    const T* Get()                     const noexcept { dbgassert(); return members->custom.Get(); }

    /// @return \c true if this instance is not \e nulled, \c false otherwise.
    operator bool()                                    const noexcept { return members != nullptr; }


  //================================================================================================
  //===  The threads::XYZLock implementation of this type
  //================================================================================================
    #if DOXYGEN
    /// Returns the embedded \p{TLock}.
    /// This method is available only if the template parameter \p{TLock} is not equal to
    /// <c>void</c>.
    /// @return A reference to the embedded lock.
    TLock&      GetLock()                                                            const noexcept;

    /// Calls this \b Acquire on the embedded instance specified with template parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param ci  Caller information. Available only with debug-builds.
    inline void Acquire( const CallerInfo& ci )                                      const noexcept;

    /// Calls this \b Acquire on the embedded instance specified with template parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param ci  Caller information. Available only with debug-builds.
    /// @return    \c true if the lock was not acquired by a different thread and thus, this call
    ///            was successful. \c false otherwise.
        [[nodiscard]]
    bool        TryAcquire( const CallerInfo& ci )                                   const noexcept;

    /// Calls this \b Acquire on the embedded instance specified with template parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param ci  Caller information. Available only with debug-builds.
    void        Release(const CallerInfo& ci)                                        const noexcept;

    /// Calls this \b Acquire on the embedded instance specified with template parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param ci  Caller information. Available only with debug-builds.
    void        AcquireRecursive( const CallerInfo& ci )                             const noexcept;

    /// Calls this \b Acquire on the embedded instance specified with template parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param ci  Caller information. Available only with debug-builds.
    void        ReleaseRecursive(const CallerInfo& ci)                               const noexcept;

    /// Calls this \b Acquire on the embedded instance specified with template parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param ci  Caller information. Available only with debug-builds.
    void        AcquireShared( const CallerInfo& ci )                                const noexcept;

    /// Calls this \b Acquire on the embedded instance specified with template parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param ci  Caller information. Available only with debug-builds.
    /// @return \c true if the lock was not acquired by a different thread and thus, this call
    ///         was successful. \c false otherwise.
    [[nodiscard]]
    bool        TryAcquireShared( const CallerInfo& ci )                             const noexcept;

    /// Calls this \b Acquire on the embedded instance specified with template parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param ci  Caller information. Available only with debug-builds.
    void        ReleaseShared(const CallerInfo& ci)                                  const noexcept;

    /// Calls this \b TryAcquireTimed on the embedded instance specified with template parameter
    /// \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param waitDuration The point in time, when this method stops waiting.
    /// @param ci           Caller information. Available only with debug-builds.
    /// @return \c true if the lock was not acquired by a different thread and thus, this call
    ///         was successful. \c false otherwise.
    [[nodiscard]]
    bool        TryAcquireTimed( const Ticks::Duration& waitDuration, const CallerInfo& ci )
                                                                                     const noexcept;

    /// Calls this \b TryAcquireTimed on the embedded instance specified with template parameter
    /// \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param waitDuration The point in time, when this method stops waiting.
    /// @param ci           Caller information. Available only with debug-builds.
    /// @return \c true if the lock was not acquired by a different thread and thus, this call
    ///         was successful. \c false otherwise.
    [[nodiscard]]
    bool        TryAcquireTimed( const Ticks::Duration::TDuration& waitDuration,
                                 const CallerInfo& ci )                              const noexcept;

    /// Calls this \b TryAcquireTimed on the embedded instance specified with template parameter
    /// \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param pointInTime  The point in time, when this method stops waiting.
    /// @param ci           Caller information. Available only with debug-builds.
    /// @return \c true if the lock was not acquired by a different thread and thus, this call
    ///         was successful. \c false otherwise.
    bool        TryAcquireTimed( const Ticks& pointInTime, const CallerInfo& ci )    const noexcept;

    /// Calls this \b TryAcquireTimed on the embedded instance specified with template parameter
    /// \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param pointInTime  The point in time, when this method stops waiting.
    /// @param ci           Caller information. Available only with debug-builds.
    /// @return \c true if the lock was not acquired by a different thread and thus, this call
    ///         was successful. \c false otherwise.
    [[nodiscard]]
    bool        TryAcquireTimed( const Ticks::TTimePoint& pointInTime,
                                 const CallerInfo& ci )                              const noexcept;

    /// Calls this \b TryAcquireSharedTimed on the embedded instance specified with template
    /// parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param waitDuration The point in time, when this method stops waiting.
    /// @param ci           Caller information. Available only with debug-builds.
    /// @return    \c true if the lock was not acquired by a different thread and thus, this call
    ///            was successful. \c false otherwise.
    [[nodiscard]]
    bool        TryAcquireSharedTimed( const Ticks::Duration& waitDuration,
                                       const CallerInfo& ci )                        const noexcept;

    /// Calls this \b TryAcquireSharedTimed on the embedded instance specified with template
    /// parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param waitDuration The point in time, when this method stops waiting.
    /// @param ci           Caller information. Available only with debug-builds.
    /// @return \c true if the lock was not acquired by a different thread and thus, this call
    ///         was successful. \c false otherwise.
    [[nodiscard]]
    bool        TryAcquireSharedTimed( const Ticks::Duration::TDuration& waitDuration,
                                       const CallerInfo& ci )                        const noexcept;

    /// Calls this \b TryAcquireSharedTimed on the embedded instance specified with template
    /// parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param pointInTime  The point in time, when this method stops waiting.
    /// @param ci           Caller information. Available only with debug-builds.
    /// @return \c true if the lock was not acquired by a different thread and thus, this call
    ///         was successful. \c false otherwise.
    [[nodiscard]]
    bool        TryAcquireSharedTimed( const Ticks& pointInTime,
                                       const CallerInfo& ci )                        const noexcept;

    /// Calls this \b TryAcquireSharedTimed on the embedded instance specified with template
    /// parameter \p{TLock}.
    /// This method participates in the overload resolution only if \p{TLock} provides an
    /// equivalent method.
    /// @param pointInTime  The point in time, when this method stops waiting.
    /// @param ci           Caller information. Available only with debug-builds.
    /// @return \c true if the lock was not acquired by a different thread and thus, this call
    ///         was successful. \c false otherwise.
    [[nodiscard]]
    bool        TryAcquireSharedTimed( const Ticks::TTimePoint& pointInTime,
                                       const CallerInfo& ci )                        const noexcept;


    #elif !ALIB_SINGLE_THREADED
        #define ATPASS ALIB_STATIC_ASSERT(ForbiddenTemplateParameterGiven, std::same_as<TRq ALIB_COMMA TLock>, "Template parameter of this method is deduced by the compiler and must not be given!");
        template<typename TRq= TLock>  requires (!std::same_as<TRq, void> )
    TRq& GetLock()                  const noexcept { ATPASS dbgassert(); return members->lock; }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, Acquire,ALIB_DBG(CallerInfo())) )
    void Acquire(ALIB_DBG(const CallerInfo& ci)) const noexcept { ATPASS dbgassert(); members->lock.Acquire (ALIB_DBG(ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, TryAcquire,ALIB_DBG(CallerInfo())) )
    bool TryAcquire(ALIB_DBG(const CallerInfo& ci)) const noexcept { ATPASS dbgassert(); return members->lock.TryAcquire (ALIB_DBG(ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, Release,ALIB_DBG(CallerInfo())) )
    void Release(ALIB_DBG(const CallerInfo& ci)) const noexcept { ATPASS dbgassert(); members->lock.Release (ALIB_DBG(ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, AcquireRecursive,ALIB_DBG(CallerInfo())) )
    void AcquireRecursive(ALIB_DBG(const CallerInfo& ci)) const noexcept { ATPASS dbgassert(); members->lock.AcquireRecursive (ALIB_DBG(ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, ReleaseRecursive,ALIB_DBG(CallerInfo())) )
    void ReleaseRecursive(ALIB_DBG(const CallerInfo& ci)) const noexcept { ATPASS dbgassert(); members->lock.ReleaseRecursive (ALIB_DBG(ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, AcquireShared,ALIB_DBG(CallerInfo())) )
    void AcquireShared(ALIB_DBG(const CallerInfo& ci)) const noexcept { ATPASS dbgassert(); members->lock.AcquireShared (ALIB_DBG(ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, TryAcquireShared,ALIB_DBG(CallerInfo())) )
    bool TryAcquireShared(ALIB_DBG(const CallerInfo& ci)) const noexcept { ATPASS dbgassert(); return members->lock.TryAcquireShared (ALIB_DBG(ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, ReleaseShared,ALIB_DBG(CallerInfo())) )
    void ReleaseShared(ALIB_DBG(const CallerInfo& ci)) const noexcept { ATPASS dbgassert(); members->lock.ReleaseShared (ALIB_DBG(ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, TryAcquireTimed, Ticks::Duration() ALIB_DBG(,CallerInfo())) )
    bool TryAcquireTimed( const Ticks::Duration& t ALIB_DBG(,const CallerInfo& ci) ) const noexcept { ATPASS dbgassert(); return members->lock.TryAcquireTimed( t ALIB_DBG(,ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, TryAcquireTimed, Ticks::Duration().Export() ALIB_DBG(,CallerInfo())) )
    bool TryAcquireTimed( const Ticks::Duration::TDuration t ALIB_DBG(,const CallerInfo& ci) ) const noexcept { ATPASS dbgassert(); return members->lock.TryAcquireTimed( t ALIB_DBG(,ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, TryAcquireTimed, Ticks() ALIB_DBG(,CallerInfo())) )
    bool TryAcquireTimed( const Ticks& t ALIB_DBG(,const CallerInfo& ci) ) const noexcept { ATPASS dbgassert(); return members->lock.TryAcquireTimed( t ALIB_DBG(,ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, TryAcquireTimed, Ticks().Export() ALIB_DBG(,CallerInfo())) )
    bool TryAcquireTimed( const Ticks::TTimePoint& t ALIB_DBG(,const CallerInfo& ci) ) const noexcept { ATPASS dbgassert(); return members->lock.TryAcquireTimed( t ALIB_DBG(,ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, TryAcquireSharedTimed, Ticks::Duration() ALIB_DBG(,CallerInfo())) )
    bool TryAcquireSharedTimed( const Ticks::Duration& t ALIB_DBG(,const CallerInfo& ci) ) const noexcept { ATPASS dbgassert(); return members->lock.TryAcquireSharedTimed( t ALIB_DBG(,ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, TryAcquireSharedTimed, Ticks::Duration().Export() ALIB_DBG(,CallerInfo())) )
    bool TryAcquireSharedTimed( const Ticks::Duration::TDuration t ALIB_DBG(,const CallerInfo& ci) ) const noexcept { ATPASS dbgassert(); return members->lock.TryAcquireSharedTimed( t ALIB_DBG(,ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, TryAcquireSharedTimed, Ticks() ALIB_DBG(,CallerInfo())) )
    bool TryAcquireSharedTimed( const Ticks& t ALIB_DBG(,const CallerInfo& ci) ) const noexcept { ATPASS dbgassert(); return members->lock.TryAcquireSharedTimed( t ALIB_DBG(,ci)); }

        template<typename TRq= TLock>  requires ( ALIB_HAS_METHOD(TRq, TryAcquireSharedTimed, Ticks().Export() ALIB_DBG(,CallerInfo())) )
    bool TryAcquireSharedTimed( const Ticks::TTimePoint& t ALIB_DBG(,const CallerInfo& ci) ) const noexcept { ATPASS dbgassert(); return members->lock.TryAcquireSharedTimed( t ALIB_DBG(,ci)); }

        #undef ATPASS
    #endif
};

} // namespace alib[::monomem]


DOX_MARKER([DOX_MANUAL_ALIASES_SHAREDMONOVAL])
/// Type alias in namespace \b alib.
template<typename T, typename TAllocator= lang::HeapAllocator, typename TLock= void>
using TSharedMonoVal =  monomem::TSharedMonoVal<T, TAllocator, TLock>;
DOX_MARKER([DOX_MANUAL_ALIASES_SHAREDMONOVAL])

} // namespace [alib]
