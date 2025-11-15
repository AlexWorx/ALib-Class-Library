//==================================================================================================
/// \file
/// This header-file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace containers {

//==================================================================================================
/// This templated class is an alternative for C++ standard library type
/// \https{std::shared_ptr,en.cppreference.com/w/cpp/memory/shared_ptr}
/// with important restrictions:
/// - Instead of managing a pointer, a \b value is managed.
/// - It is not possible to store derived types within this type, which is a common use case with
///   <c>std::shared_ptr</c>, especially in consideration with dynamic (virtual) types.<br>
///   This implies that no abstract types can be stored.
/// - This implementation misses the coexistence of sibling type <c>std::weak_ptr</c> and
///   corresponding functionality.
/// - This implementation misses an equivalent to method <c>owner_before</c> and corresponding
///   comparison operators.
/// - This implementation misses dedicated array support (at least as of today).
///
/// The advantages are:
/// - The type has a footprint of only <c>sizeof(void*)</c>, where the standard's type has
///   a size of two pointers.
/// - An internal second pointer-dereferencing is avoided when accessing the shared value.
/// - The type performs only one allocation.
///   (Common implementations of the standard's type perform two allocations.)
/// - The type supports storing references to \alib {lang;Allocator;allocators} which are
///   used for allocation and freeing of memory.
///   Allocators can be of "heavy" weight and are never copied by value.
///
/// Note that despite being named <b>Shared<em><u>Val</u></em></b>, which is in contrast to
/// sibling type <b>Shared<em><u>Ptr</u></em></b>, the type still behaves like a pointer.
/// It can be \e nulled, value <c>nullptr</c> can be assigned, and member access is performed
/// with the <c>operator-></c>. And of course the object is destructed, and the memory is freed in
/// case the last copy of an instance is nulled or gets out of scope. A different naming
/// proposal could have been <b>Shared<em><u>Static</u></em>Ptr</b> to indicate that no
/// dynamic conversions and abstract types are applicable.
///
/// \see
///   - Class \alib{containers;SharedPtr} which allows storing derived, dynamic types.
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
class SharedVal
{
  protected:
    /// Fields if \p{TAllocator} is default-constructible (e.g., \alib{lang;HeapAllocator}).
    struct FieldMembersNoTA
    {
        /// The instance of the custom type.
        T                           custom;

        /// The reference counter used to implement the <c>std::shared_ptr</c> behavior.
        std::atomic<unsigned>       refCount;

        /// Alternative constructor missing the allocator instance.
        /// This is used only with allocators that are default-constructible
        /// (like \alib{lang;HeapAllocator} is).
        /// @tparam TArgs      The argument types used for constructing \p{T}.
        /// @tparam TRequires  Defaulted template parameter. Must not be specified.
        /// @param  args       The arguments for constructing \p{T}.
        template<typename... TArgs, typename TRequires= TAllocator>
        requires std::default_initializable<TRequires>
        FieldMembersNoTA( TArgs&&... args )
        : custom(std::forward<TArgs>(args)...)
        , refCount(1)                                                                             {}

        ///@return A heap allocator value.
        static constexpr TAllocator GetAllocator()        noexcept { return lang::HeapAllocator(); }
    }; // struct FieldMembers;

    /// Fields if \p{TAllocator} is not default-constructible (not \alib{lang;HeapAllocator}).
    struct FieldMembersWithAllocator
    {
        /// The instance of the custom type.
        T                           custom;

        /// The reference counter used to implement the <c>std::shared_ptr</c> behavior.
        std::atomic<unsigned>       refCount;

        /// The allocator used.
        TAllocator&                 allocator;

        /// Constructor.
        /// @param  pAllocator The allocator, stored in parent \b AllocatorMember.
        /// @tparam TArgs      The argument types used for constructing \p{T}.
        /// @param  args       The arguments for constructing \p{T}.
        template<typename... TArgs>
        FieldMembersWithAllocator( TAllocator& pAllocator, TArgs&&... args )
        : custom(std::forward<TArgs>(args)...)
        , refCount(1)
        , allocator(pAllocator)                                                                   {}

        ///@return The stored allocator.
        TAllocator& GetAllocator()                                       const { return allocator; }

    }; // struct FieldMembers;

    /// The type of the stored data. Note that we cannot use helper \alib{lang;AllocatorMember},
    /// because type T could be derived from the same base class and this would break
    /// EBO rules, that force the compiler to give two distinguishable members, inherited or
    /// not, a different address.
    using FieldMembers = std::conditional_t< std::is_default_constructible<TAllocator>::value,
                                             FieldMembersNoTA,
                                             FieldMembersWithAllocator>;
    
    /// The stored data.
    FieldMembers*           members;

    /// Internal shortcut to receive the custom member.
    /// @return The pointer to the contained type, or \c nullptr in case this is empty.
    T* getP()                                                                         const noexcept
    { return members ? &members->custom : nullptr; }

    /// Internal shortcut to receive a reference to the custom member.
    /// Asserts in debug-compilations.
    /// @return A reference the contained type.
    T& getR()                                                                         const noexcept
    {
        ALIB_ASSERT_ERROR( members, "CONTAINERS", "Accessing nulled SharedVal." )
        return members->custom;
    }
  public:
    /// Exposes the allocator as given with template parameter \p{TAllocator}.
    using AllocatorType= TAllocator;

    /// Exposes the stored type specified with template parameter \p{T}.
    using StoredType   = T;

    /// Default Constructor. Leaves this object \e nulled.
    SharedVal()                                                    noexcept : members(nullptr) {}

    /// Constructs an empty instance from \c std::nullptr.
    /// This constructor is necessary to allow assignment of \c std::nullptr to values of this type,
    /// which clears the automatic pointer.
    SharedVal(std::nullptr_t)                                      noexcept : members(nullptr) {}

    /// Copy Constructor. Increases the reference counter of the shared pointer (in case given
    /// \p{other} is not nulled).
    /// @param other The object to copy.
    SharedVal(const SharedVal& other)                                                       noexcept
    : members(other.members)                                  { if(members) ++(members->refCount); }


    /// Move Constructor. Does not increase the reference counter, instead nulls the \p{other}.
    /// @param other The object to copy.
    SharedVal(SharedVal&& other)                                                            noexcept
    : members(other.members)                                             { other.members= nullptr; }

    /// Copy Assignment Operator. Cares for self-assignment and assignment of a shared pointer with
    /// the same content.
    /// Otherwise, the reference counter of the current object is decreased, disposed if
    /// necessary, and then the object in \p{other} is copied to this object.
    /// @param other The object to copy into this one.
    /// @return A reference to \c this.
    SharedVal& operator=(const SharedVal& other)                                            noexcept
    {
        // handle self assignment and assignment with same contents
        if (this == &other || this->members == other.members)
            return *this;

        // decrement the old reference count and delete the old data if needed
        if (members && members->refCount.fetch_sub(1) == 1)
            members->GetAllocator()().Delete(members);

        // copy the new data
        if((members= other.members) != nullptr)
            ++(members->refCount);

        return *this;
    }

    /// Move Assignment Operator. Cares for self-assignment.
    /// Otherwise, the object in \p{other} is copied to this.
    /// @param other The object to move into this one.
    /// @return A reference to \c this.
    SharedVal& operator=(SharedVal&& other)                                                 noexcept
    {
        // handle self assignment
        if (this == &other)
            return *this;

        // decrement the old reference count and delete the old data if needed
        if (members && members != other.members && members->refCount.fetch_sub(1) == 1)
            members->GetAllocator()().Delete(members);

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
    requires (!std::default_initializable<TRequires>)
    SharedVal( TAllocator& allocator, TArgs&&... args )
    : members( allocator().template New<FieldMembers>( allocator,
                                                       std::forward<TArgs>(args)...) )        {}

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
    /// \note
    ///    If this was not done, this constructor was chosen for copy constructors of
    ///    this type. In this case, the compilation of this constructor would fail when
    ///    initializing field member.
    ///
    /// @tparam TArgs      The argument types used for constructing \p{T}.
    /// @tparam TRequires  Defaulted template parameter. Must not be specified.
    /// @param  args       The arguments for constructing \p{T}.
    template <typename... TArgs, typename TRequires= TAllocator>
    requires(      std::default_initializable<TRequires>
              &&  (sizeof...(TArgs) > 0)
              &&  (     sizeof...(TArgs) != 1
                    || !std::same_as<std::decay_t<std::tuple_element_t<0, std::tuple<TArgs...>>>,
                                     SharedVal> )
              &&  std::is_constructible_v<T, TArgs...>   )
    SharedVal(TArgs&&... args)
    : members( TAllocator()().template New<FieldMembers>(std::forward<TArgs>(args)...) )      {}

    /// Destructor. If this is the last copy, the destructor of \p{T} is invoked and the
    /// memory is freed to \p{TAllocator}.
    ~SharedVal()
    {
        if (members && members->refCount.fetch_sub(1) == 1)
            members->GetAllocator()().Delete(members);
    }

    /// @return The size of the memory that is allocated for the \p{T} as well as for
    ///         the reference counter and the allocator member.
    ///         (To whom it may concern.)
    static constexpr size_t SizeOfAllocation()                      { return sizeof(FieldMembers); }

    /// @return The allocator given with construction that will be used to free the memory
    ///         that had been allocated, at the moment the use counter becomes \c 0.
    AllocatorType&    GetAllocator()                                                           const
    {
        ALIB_ASSERT_ERROR( members, "CONTAINERS", "Accessing nulled SharedVal." )
        return members->GetAllocator();
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
    bool              Unique()                                                        const noexcept
    { return members && members->refCount.load() == 1; }
    
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
    /// @return \c true if #UseCount is greater than \c 0, \c false otherwise.
    bool              operator==(std::nullptr_t)       const noexcept { return members == nullptr; }

    /// Comparison with <c>nullptr</c>.
    /// @return \c false if #UseCount is greater than \c 0, \c false otherwise.
    bool              operator!=(std::nullptr_t)       const noexcept { return members != nullptr; }

    /// @return \c true if this instance is not \e nulled, \c false otherwise.
    operator bool()                                    const noexcept { return members != nullptr; }

    /// Returns a non-constant pointer to the stored object of type \p{T}.
    /// @return A pointer to \p{T}.
    T*                Get()                                                 const { return getP(); }

    /// Overloaded operator to access members of custom type \p{T}
    /// @return A pointer to \p{T}.
    T*                operator->()                                          const { return getP(); }

    /// Overloaded operator to access members of custom type \p{T}
    /// @return A pointer to \p{T}.
    T&                operator*()                                           const { return getR(); }
}; // class SharedVal

} // namespace alib[::containers]

/// Type alias in namespace \b alib.
template<typename T, typename TAllocator= lang::HeapAllocator>
using     SharedVal =   containers::SharedVal<T, TAllocator>;

} // namespace [alib]
