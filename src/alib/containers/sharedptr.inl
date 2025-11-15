//==================================================================================================
/// \file
/// This header-file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace containers {

//==================================================================================================
/// This templated class lifts restrictions imposed by (otherwise slightly more efficient)
/// class \alib{containers;SharedVal} and is an almost full-featured alternative for
/// C++ standard library type
/// \https{std::shared_ptr,en.cppreference.com/w/cpp/memory/shared_ptr}.
///
/// What is missing compared to <c>std::shared_ptr</c>:
/// - Pointers to externally allocated objects cannot be assigned.
///   The shared objects are always created with construction of this class or with method
///   #InsertDerived .
/// - Special method #InsertDerived is to be used, in case types derived from \p{T} are to be
///   placed inside.
/// - Overall, the interface is more explicit and seems less convenient.
/// - This implementation misses the coexistence of sibling type <c>std::weak_ptr</c> and
///   corresponding functionality.
/// - This implementation misses an equivalent to method <c>owner_before</c> and corresponding
///   comparison operators.
/// - This implementation misses dedicated array support (at least as of today).
///
/// Advantages are:
/// - The type has a footprint of only <c>sizeof(void*)</c>, where the standard's type has
///   a size of two pointers.
/// - The type performs only one allocation.
///   (Common implementations of the standard's type perform two allocations.)
/// - The type supports storing references to \alib {lang;Allocator;allocators} which are
///   used for allocation and freeing of memory. (In this case, of course, the footprint
///   increases to two times the <c>sizeof(void*)</c>.)
///
/// \see
///   - Sibling class \alib{containers;SharedVal}, with is a restricted but slightly more efficient
///     version of this class.
///   - Class \alib{monomem;TSharedMonoVal} of module \alib_monomem, which incorporates
///     an own embedded instance of class \alib{monomem;TMonoAllocator}.
///     This allocator can be used for further monotonic allocations by the contained type or
///     other code entities that receive the shared pointer.
/// @tparam T           The custom type that is shared with this pointer.
/// @tparam TAllocator  The allocator that is used to allocate an instance of \p{T} together
///                     with a reference counter and optionally a reference to such allocator
///                     if passed with construction.
//==================================================================================================
template<typename T, typename TAllocator= HeapAllocator>
class SharedPtr
{
  protected:
    /// The combined struct of members which are from the \p{TAllocator} passed in the constructor.
    /// Note, that in the case that \alib{lang;HeapAllocator} is used parent class
    /// \b AllocatorMember will be empty.
    template<typename U>
    struct FieldMembers : lang::AllocatorMember<TAllocator>
    {
        /// The duly cast pointer to the custom type behind us.
        T*                          custom;

        /// The size of the allocated pair of these fields and the custom type.
        size_t                      allocSize;

        /// The reference counter used to implement the <c>std::shared_ptr</c> behavior.
        std::atomic<unsigned>       refCount;

        /// The instance, either derived or T.
        U                           u;
        
        /// Constructor.
        /// @param  pAllocator The allocator, stored in parent \b AllocatorMember.
        /// @param  pCustom    The pointer to #custom, rightfully cast.
        /// @param  pAllocSize The size of the allocated pair, containing this and the
        ///                    potentially derived object \p{T}.
        FieldMembers( TAllocator& pAllocator, T* pCustom, size_t pAllocSize)
        : lang::AllocatorMember<TAllocator>( pAllocator )
        , custom   (pCustom)
        , allocSize(pAllocSize)
        , refCount (1)                                                                            {}

        /// Alternative constructor missing the allocator instance.
        /// @param  pAllocSize  The size of the allocated pair, containing this and the
        ///                     potentially derived object \p{T}.
        /// @param  pCustom     The pointer to #custom, rightfully cast.
        FieldMembers(T* pCustom, size_t pAllocSize)
        : custom   (pCustom)
        , allocSize(pAllocSize)
        , refCount (1)                                                                            {}
    }; // struct FieldMembers;

    /// The allocated stored data. Note that \p{T} can be a derived type.
    FieldMembers<void*>*     members;

    /// Internal shortcut to receive the custom member.
    /// @return The pointer to the contained type, or \c nullptr in case this is empty.
    T* getP()       const noexcept { return members ? reinterpret_cast<T*>(&members->u) : nullptr; }

    /// Internal shortcut to receive a reference to the custom member.
    /// Asserts in debug-compilations.
    /// @return A reference the contained type.
    T& getR()                                                                       const noexcept {
        ALIB_ASSERT_ERROR( members, "CONTAINERS", "Accessing nulled SharedVal." )
        return reinterpret_cast<T&>(members->u);
    }


  public:
    /// Exposes the allocator as given with template parameter \p{TAllocator}.
    using AllocatorType= TAllocator;

    /// Exposes the stored type specified with template parameter \p{T}.
    using StoredType   = T;

    /// Default Constructor. Leaves this object \e nulled.
    SharedPtr()                                                       noexcept : members(nullptr) {}

    /// Constructs an empty instance from \c std::nullptr.
    /// This constructor is necessary to allow assignment of \c std::nullptr to values of this type,
    /// which clears the automatic pointer.
    SharedPtr(std::nullptr_t)                                         noexcept : members(nullptr) {}

    /// Copy Constructor. Increases the reference counter of the shared pointer (in case given
    /// \p{other} is not nulled).
    /// @param other The object to copy.
    SharedPtr(const SharedPtr& other)                                                       noexcept
    : members(other.members)                                  { if(members) ++(members->refCount); }


    /// Move Constructor. Does not increase the reference counter, instead nulls the \p{other}.
    /// @param other The object to copy.
    SharedPtr(SharedPtr&& other)                                                            noexcept
    : members(other.members)                                             { other.members= nullptr; }

    /// Copy Assignment Operator. Cares for self-assignment and assignment of a shared pointer with
    /// the same content.
    /// Otherwise, the reference counter of the current object is decreased, disposed if
    /// necessary, and then the object in \p{other} is copied to this object.
    /// @param other The object to copy into this one.
    /// @return A reference to \c this.
    SharedPtr& operator=(const SharedPtr& other)                                          noexcept {
        // handle self assignment and assignment with same contents
        if (this == &other || this->members == other.members)
            return *this;

        // decrement the old reference count and delete the old data if needed
        if (members && members->refCount.fetch_sub(1) == 1) {
            lang::Destruct(getR());
            members->GetAllocator().free( members, members->allocSize );
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
    SharedPtr& operator=(SharedPtr&& other)                                               noexcept {
        // handle self assignment
        if (this == &other)
            return *this;

        // decrement the old reference count and delete the old data if needed
        if (members && members != other.members && members->refCount.fetch_sub(1) == 1) {
            lang::Destruct(getR());
            members->GetAllocator().free( members, members->allocSize );
        }

        // move members
        members= other.members;
        other.members= nullptr;
        return *this;
    }

    /// Constructor taking an allocator along with the construction parameters for the instance
    /// of \p{T}. The allocator is used allocate the needed memory (one allocation) and the
    /// reference to it is internally stored, to be able to free the memory later.
    ///
    /// \note
    ///   This constructor is accepted by the compiler only if template type \p{TAllocator}
    ///   is \b not default-constructible.
    /// @tparam TArgs      The argument types used for constructing \p{T}.
    /// @tparam TRequires  Defaulted template parameter. Must not be specified.
    /// @param  allocator  The allocator used to allocate and free needed storage.
    /// @param  args       The arguments for constructing \p{T}.
    template<  typename... TArgs, typename TRequires= TAllocator>
    requires ( !std::default_initializable<TRequires> )
    SharedPtr( TAllocator& allocator, TArgs&&... args ) {
        auto* mem= allocator().template Alloc<FieldMembers<T>>();
        members   = reinterpret_cast<FieldMembers<void*>*>(mem);
        new (members)  FieldMembers<void*>(allocator, &mem->u, sizeof(FieldMembers<T>));
        new (&mem->u) T(std::forward<TArgs>(args)...);
    }

    /// Constructor missing the allocator instance.
    /// To be used only with allocators that are default-constructible
    /// (like \alib{lang;HeapAllocator} is).
    ///
    /// \note
    ///   This constructor is accepted by the compiler only if
    ///   - \p{TAllocator} is default-constructible, and
    ///   - the variadic argument list is not empty (here default construction is chosen), and
    ///   - the variadic arguments would not match to the copy or move constructor, and
    ///   - the variadic arguments are constructing type \p{T}.
    ///
    /// @tparam TRequires  Defaulted template parameter. Must not be specified.
    /// @tparam TArgs      The argument types used for constructing \p{T}.
    /// @param  args       The arguments for constructing \p{T}.
    template <typename... TArgs, typename TRequires= TAllocator>
    requires(    std::is_default_constructible_v<TRequires>
              && !std::is_same_v<std::decay_t<TArgs>... , SharedPtr>
              && (sizeof...(TArgs) > 0)                           )
    SharedPtr(TArgs&&... args) {
        auto* mem= TAllocator()().template Alloc<FieldMembers<T>>();
        members   = reinterpret_cast<FieldMembers<void*>*>(mem);
        new (members)  FieldMembers<void*>(&mem->u, sizeof(FieldMembers<T>));
        new (&mem->u) T(std::forward<TArgs>(args)...);
    }

    /// Disposes any currently held data (in case this was the last user, the current object
    /// is deleted and memory freed) and places a new instance of (potentially) derived
    /// type \p{TDerived} in this object.
    ///
    /// This overload of the method is accepted by the compiler only if type \p{TAllocator} is
    /// default-constructible.
    ///
    /// @tparam TRequires  Defaulted template parameter. Must not be specified.
    /// @tparam TArgs      The argument types used for constructing \p{T}.
    /// @param  args       The arguments for constructing \p{T}.
    template <typename TDerived, typename TRequires= TAllocator, typename... TArgs>
    requires std::is_default_constructible_v<TRequires>
    void InsertDerived(TArgs&&... args) {
        // delete any existing
        lang::Destruct(*this);

        auto* mem= TAllocator()().template Alloc<FieldMembers<TDerived>>();
        members   = reinterpret_cast<FieldMembers<void*>*>(mem);
        new (members)  FieldMembers<void*>( lang::SafeCast<T>(&mem->u),
                                            sizeof(FieldMembers<TDerived>)        );
        new (&mem->u) TDerived(std::forward<TArgs>(args)...);
    }

    /// Disposes any currently held data (in case this was the last user, the current object
    /// is deleted and memory freed) and places a new instance of (potentially) derived
    /// type \p{TDerived} in this object.
    ///
    /// This overload of the method is accepted by the compiler only if type \p{TAllocator} is
    /// not default-constructible.
    ///
    /// @tparam TRequires  Defaulted template parameter. Must not be specified.
    /// @tparam TArgs      The argument types used for constructing \p{T}.
    /// @param  allocator  The allocator to use.
    /// @param  args       The arguments for constructing \p{T}.
    template <typename TDerived, typename TRequires= TAllocator, typename... TArgs>
    requires( !std::is_default_constructible_v<TRequires> )
    void InsertDerived(TAllocator& allocator, TArgs&&... args) {
        // delete any existing
        lang::Destruct(*this);
        new (this) SharedPtr(); // note: without this, some compilers complain about
                                //       uninitialized member 'members'!

        auto* mem= allocator().template Alloc<FieldMembers<TDerived>>();
        members   = reinterpret_cast<FieldMembers<void*>*>(mem);
        new (members)  FieldMembers<void*>( allocator,
                                            lang::SafeCast<T>(&mem->u),
                                            sizeof(FieldMembers<TDerived>)        );
        new (&mem->u) TDerived(std::forward<TArgs>(args)...);
    }

    /// Destructor. If this is the last copy, the destructor of \p{T} is invoked and the
    /// memory is freed to \p{TAllocator}.
    ~SharedPtr() {
        if (members && members->refCount.fetch_sub(1) == 1) {
            lang::Destruct(getR());
            members->GetAllocator().free( members, members->allocSize );
    }   }

    /// @return The size of the memory that is allocated for the \p{T} as well as for
    ///         the reference counter and the allocator member.
    ///         (To whom it may concern.)
    template<typename TStored= T>
    static constexpr size_t SizeOfAllocation()    noexcept { return sizeof(FieldMembers<TStored>); }

    /// @return The allocator given with construction that will be used to free the memory
    ///         that had been allocated, at the moment the use counter becomes \c 0.
    AllocatorType&    GetAllocator()                                                         const {
        ALIB_ASSERT_ERROR( members, "CONTAINERS", "Accessing nulled SharedVal." )
        return members->GetAllocator();
    }

    /// @return The allocator interface of the allocator received with construction.
    lang::AllocatorInterface<TAllocator> AI()            const noexcept { return GetAllocator()(); }

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
    void              SetNulled()                       { lang::Destruct(*this); members= nullptr; }

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

    /// @return \c true if this instance is not \e nulled, \c false otherwise.
    operator          bool()                           const noexcept { return members != nullptr; }

    /// Returns a non-constant pointer to the stored object of type \p{T}.
    /// @return A pointer to \p{T}.
    T*                Get()                                                 const { return getP(); }

    /// Overloaded operator to access members of custom type \p{T}
    /// @return A pointer to \p{T}.
    T*                operator->()                                          const { return getP(); }

    /// Overloaded operator to access members of custom type \p{T}
    /// @return A pointer to \p{T}.
    T&                operator*()                                           const { return getR(); }

}; // class SharedPtr

} // namespace alib[::containers]

/// Type alias in namespace \b alib.
template<typename T, typename TAllocator= lang::HeapAllocator>
using     SharedPtr =   containers::SharedPtr<T, TAllocator>;

} // namespace [alib]
