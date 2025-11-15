//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace lang {

/// Provides debugging utility methods used by allocator types \alib{lang;HeapAllocator},
/// \alib{monomem;TMonoAllocator}, and \alib{monomem;TPoolAllocator}.
/// Each method is empty and thus optimized out in case the compiler-symbol
/// \ref ALIB_DEBUG_ALLOCATIONS is not set.
/// @see Chapter \ref alib_contmono_further_debug of the Programmer's Manual.
struct DbgAlloc
{
    /// Adds needed space for \b 4 magic bytes plus the <c>size_t</c> value that is stored.
    /// @tparam TIntegral The type of given \p{size}.
    ///                   (Deduced by the compiler, templated for convenience.)
    /// @param  size      The given size or the extended size.
    /// @return The truly allocated size.
    template<typename TIntegral>
    static constexpr size_t extSize (TIntegral size) {
        #if !ALIB_DEBUG_ALLOCATIONS
            return size_t(size);
        #else
            return size_t(size) + 4 + sizeof(size_t);
        #endif
    }

    /// Writes magic bytes and size-information behind the given memory.
    /// @tparam T     The type of the given pointer \p{mem}.
    ///               (Deduced by the compiler, templated for convenience.)
    /// @param  mem   The pointer to the allocated object.
    /// @param  size  The originally requested size.
    /// @param  magic The magic byte to write after the element.
    template<typename T>
    static constexpr void annotate(T* mem, size_t size, unsigned char magic) {
        #if ALIB_DEBUG_ALLOCATIONS
                auto* cMem= reinterpret_cast<unsigned char*>(mem);
                size_t idx= size;
                for( ; idx< size + 4; ++idx)
                    cMem[idx] = magic;
                for( size_t i= 0; i< sizeof(size_t); ++i) {
                    cMem[idx++]= static_cast<unsigned char>( size & 0xFF );
                    size>>=8;
                }
        #else
            (void) mem; (void) size; (void) magic;
        #endif
    }

    /// The given character \p{c} is written into the allocated memory (including its extended
    /// size).
    /// @tparam T     The type of the given pointer \p{mem}. (Deduced by the compiler.)
    /// @param  mem   The pointer to the allocated object.
    /// @param  size  The allocated size from the user's perspective (not the real size)
    /// @param  magic The magic byte to overwrite the freed memory with.
    template<typename T>
    static constexpr void clearMem(T* mem, size_t size, unsigned char magic) {
        #if ALIB_DEBUG_ALLOCATIONS
            memset( reinterpret_cast<char*>(mem), magic, extSize(size) );
        #else
            (void) mem; (void) size; (void) magic;
        #endif
    }

    #include "ALib.Lang.CIFunctions.H"
    /// \ref alib_mod_assert "Raises an ALib error" if the piece of allocated memory is
    /// corrupted or its allocation size is not rightfully given by the using code.
    ///
    /// @tparam TSize    The type of parameter \p{size}. (Deduced by the compiler.)
    /// @param  mem      The address of the allocated object.
    /// @param  size     The requested allocation size of the object.
    /// @param  magic    The value of the magic bytes that had been written with #annotate.
    /// @param  name     The type of the allocator.
    ///                  In case of type \b MonoAllocator, the debug-name of the instance.
    template<typename TSize>
    static
    void checkMem( void* mem, const TSize size, unsigned char magic, const char* name ) {
        #if ALIB_DEBUG_ALLOCATIONS
            auto* cMem= reinterpret_cast<unsigned char*>(mem);
            size_t idx= size;
            while ( idx< size + 4)
                if( cMem[idx++] != magic )
                    ALIB_ERROR( "MONOMEM", "Corrupt memory with allocator ", name )

            size_t storedSize= 0;
            idx+= sizeof(size_t);
            do
            {
                storedSize<<= 8;
                storedSize|=  cMem[--idx];
            }
            while( idx > size + 4);

            if( storedSize != size )
                ALIB_ERROR( "MONOMEM", "Given size does not match the allocated size "
                                       "(or corrupt memory). Allocator: ", name )
        #else
            (void) mem; (void) size; (void) magic; (void) name;
        #endif
    }
    #include "ALib.Lang.CIMethods.H"

}; // struct DbgAlloc

#if DOXYGEN
//==================================================================================================
/// This is class does not exist but is just exposed in the documentation of \alib.
/// Its purpose is to precisely define what an \alib "Allocator" needs to offer as an interface
/// to be usable with \alib template types that expect an allocator.
///
/// Implementations of this interface exist with \alib.:
/// - \alib{lang;HeapAllocator},
/// - \alib{monomem;TMonoAllocator}, its derivate
/// - \alib{monomem;TLocalAllocator}, and
/// - \alib{monomem;TPoolAllocator}.
///
/// The first is just using \c std::malloc and \c std::free, hence it does just the same
/// allocation that C++ keywords <c>new</c> and <c>delete</c> do. The others are introduced and
/// discussed by module \alib_monomem.
///
/// As an exception to the \ref alib_manual_appendix_naming "naming conventions used by ALib",
/// the members of this type start with a non-capital letter, although they are declared \c public.
/// This was done to emphasize that an indirect use of this type through type
/// \alib{lang;AllocatorInterface} is available with #operator()(), and such usage is preferred.
///
/// Along with this pure documentation class, a C++20 concept exists with
/// \alib{lang;IsAllocator}, which duly checks if  all entities defined with this type are present
/// in a custom allocator implementation.
/// Whenever an allocator is expected as a template type, this concept is required to be satisfied.
//==================================================================================================
struct Allocator
{
    /// The type of the allocator that this allocator uses underneath for its allocations.
    /// In case of \alib{lang;HeapAllocator}, this type is defined as <c>void</c>.<br>
    /// Allocators provided by \alib allow to access the instance with publicly inherited methods
    /// \alib{lang::AllocatorMember;GetAllocator} and \alib{lang::AllocatorMember;AI}.
    using ChainedAllocator =    TAllocator;

    /// The minimum alignment an allocator supports, respectively uses even if lower values
    /// are requested.
    /// @see Chapter \ref alib_contmono_further_alignment of the Programmer's Manual of module
    ///      \alib_monomem.
    static constexpr size_t     MIN_ALIGNMENT;

    /// The maximum alignment an allocator supports.
    /// @see Chapter \ref alib_contmono_further_alignment of the Programmer's Manual of module
    ///      \alib_monomem.
    static constexpr size_t     MAX_ALIGNMENT;

    /// Allocate memory using the allocator.<br>
    /// Note, that parameter \p{size} is provided as a reference. If an allocator implementation
    /// allocates bigger pieces than requested (which is true for
    /// \alib{monomem;TPoolAllocator;PoolAllocator}), then this parameter is modified to this higher
    /// value.
    /// @param[in,out] size The size of memory the block to allocate in bytes. This is an input and
    ///                     output parameter. Might be increased to the truly allocated size.
    ///                     In case it was increased, the new value does not need to be passed
    ///                     back to #reallocate or #free. Allocators are agnostic which of the
    ///                     sizes are passed back, the requested, or the true size of the block.
    /// @param  alignment   The (minimum) alignment of the memory block to allocate in bytes.
    /// @return Pointer to the allocated memory.
    void* allocate( size_t& size, size_t alignment );

    /// Shrinks or grows a piece of memory. If a new allocation was performed the existing
    /// data is copied.
    /// Note, that parameter \p{newSize} is provided as a reference. If an allocator implementation
    /// allocates bigger pieces than requested (which is true for
    /// \alib{monomem;TPoolAllocator;PoolAllocator}), then this parameter is modified to this higher
    /// value.
    /// @param  mem            The memory to reallocate.
    /// @param  oldSize        The current size of \p{mem}.
    /// @param[in,out] newSize The new size of memory requested to allocate in bytes. This is an
    ///                        input and output parameter. It might be increased to the truly
    ///                        allocated size.
    /// @param  alignment      The (minimum) alignment of the memory block to allocate in bytes.
    /// @return Pointer to the re-allocated memory.
    void* reallocate( void* mem, size_t oldSize, size_t& newSize, size_t alignment );

    /// Frees memory that was previously allocated with the same allocator.
    ///
    /// @param mem   The memory to dispose.
    /// @param size  The size of the given \p{mem}.
    void    free(void* mem, size_t size);

    /// Returns a temporary object (which is usually optimized out together with a call to this
    /// operator) providing high-level convenience methods for allocation.
    /// @see Class \alib{lang::AllocatorInterface}
    /// @return A temporary high-level interface into the allocator.
    AllocatorInterface<Allocator>   operator()();

    /// This is a constexpr static method that determines if a type allows splitting memory
    /// and later passing the pieces back to free.<br>
    /// Of the allocators exposed by module \alib_monomem, only \alib{monomem;TMonoAllocator}
    /// returns \c true. This is because this allocator has an empty \b free method and thus
    /// is agnostic about the memory freed.
    /// If the compiler-symbol \ref ALIB_DEBUG_ALLOCATIONS is set, then also this allocator returns
    /// \c false.
    ///
    /// @return \c true if this allocator is agnostic against freeing of objects which have not
    ///         been allocated as given.
    static constexpr bool allowsMemSplit()                                                 noexcept;

    /// Allocator name. This is a pure debug-field that helps to identify leaks and other issues
    /// with allocators. The field is public an may be changed as wanted by a user of the library.
    ///
    /// Class \alib{monomem;TMonoAllocator;MonoAllocator} requires a name as the first parameter
    /// with every overloaded constructor. Consequently, this argument has to be passed enclosed
    /// in macro \ref ALIB_DBG (with a trailing comma) to have it pruned with release-builds.
    ///
    /// Class \alib{monomem;TMonoAllocator;PoolAllocator} grabs this name from its chained
    /// allocator, which most often is an already named \b MonoAllocator. Thus, no constructor
    /// argument is available. Its name may be changed after construction as appropriate.
    const char*             DbgName;

    /// This method has to be called to set the correct object size in the case that:
    /// a) An allocation method returned a higher allocation size (as of the current \alib release,
    ///    this only happens with the use of class \alib{monomem;TPoolAllocator;PoolAllocator}, and
    /// b) this higher size is not ignored by the using code and,
    /// c) this higher size is reported to one of the <em>free</em>-methods on object destruction.
    ///
    /// Within \alib a sample of such a type is class \alib{strings;TAString;AString}: this type
    /// requests a string buffer as needed but acknowledges if the returned buffer is lager.
    /// With destruction, this higher capacity is passed when the buffer is deleted.
    /// Thus, this type invokes this method after performing allocations.
    ///
    /// Note that this method is empty and optimized out, when the compiler-symbol
    /// \ref ALIB_DEBUG_ALLOCATIONS is not set. If set, an \alib_assertion is raised if either the
    /// magic bytes around the memory are not found, or the given \p{allocSize} does not
    ///
    /// @see Chapter \ref alib_contmono_further_debug of the Programmer's Manual.
    ///
    /// @tparam TSize   The type of parameter \p{allocSize}. (Deduced by the compiler.)
    /// @param mem        The address of the allocated object.
    /// @param allocSize  The true allocation size returned by the method #allocate .
    template<typename TSize>
    void dbgAcknowledgeIncreasedAllocSize( void* mem, TSize allocSize )                       const;

    /// If the compiler-symbol \ref ALIB_DEBUG_ALLOCATIONS is not set, this method is empty and will
    /// be optimized out. Otherwise, this will raise an \alib_assertion if the piece of allocated
    /// memory is corrupted or its allocation size is not rightfully given by the using code.
    /// @see Chapter \ref alib_contmono_further_debug of the Programmer's Manual.
    ///
    /// @tparam TSize   The type of parameter \p{size}. (Deduced by the compiler.)
    /// @param mem   The address of the allocated object.
    /// @param size  The requested allocation size of the object.
    template<typename TSize>
    void dbgCheckMemory( void* mem, TSize size )                                              const;

}; // struct Allocator
#endif // Doxygen

//==================================================================================================
/// This type offers high-level convenience methods for \alib allocators.<br>
/// An \alib allocator (a type that offers an interface as specified with prototype
/// \alib{lang;Allocator}) has to provide only three <b>low-level (!)</b> allocation methods.
/// Those are:
/// - \alib{lang::Allocator;allocate(size_t& size, size_t alignment)},
/// - \alib{lang::Allocator;reallocate(void* mem, size_t oldSize, size_t& newSize, size_t alignment)},
/// - \alib{lang::Allocator;free(void* disposable, size_t size)}, and
///
/// Besides these, the C++ function call operator \alib{lang::Allocator;operator()()} has to be
/// implemented to return a temporary instance of this type.
/// Note that the compiler will optimize the function call and the temporary instance out.
/// With this setup, each allocator offers the <b>high-level</b> allocation methods provided
/// with this class.
///
/// The naming scheme of the convenience methods found with this type, is along the standards:
/// - Methods containing the words <c>"Alloc"</c> or <c>"Free"</c> allocate, respectively free
///   memory without construction, respectively destruction of type instances.
/// - Methods containing the word <c>"New"</c> and <c>"Delete"</c> allocate, respectively free
///   memory and in addition invoke constructors, respectively destructors of type instances.
///
/// In contrast to the low-level interface methods \alib{lang;Allocator::allocate} and
/// \alib{lang;Allocator::reallocate}, information about a piece of memory returned that is larger
/// than requested is not retrievable with the methods provided here. In case this can be of
/// value, the low-level interface of an allocator has to be used.
///
/// The following quick sample illustrates the use of this class in combination with a
/// \alib{monomem;TMonoAllocator}:
///
/// \snippet "ut_monomem.cpp"     DOX_MONOMEM_ALLOCATOR_INTERFACE_TYPE
/// \snippet "ut_monomem.cpp"     DOX_MONOMEM_ALLOCATOR_INTERFACE
///
/// \note
///    In this sample, the destructor is of class \c MyType is emtpy, and furthermore method
///    \b free of class \b MonoAllocator is empty. Thus, the whole call to #Delete will be optimized
///    out. With other allocators this is different. It is recommended to still include
///    such calls to avoid memory leaks at the moment either the allocator is changed, an allocated
///    type gets equipped with a destructor, etc.
///
///@see
///   - The \ref alib_mods_contmono "Programmer's Manual" of module \alib_monomem_nl.
///   - Allocator types \alib{monomem;TMonoAllocator} and \alib{monomem;TPoolAllocator}
///     found in module \alib_monomem_nl.
///   - Class \alib{lang;HeapAllocator}, which is useful for templated types which are agnostic
///     to underlying allocation strategies.
///   - To allocate and create a copy of a character string, method \alib{strings;TString::Allocate}
///     or constructor \alib{strings;TString;TString(TAllocator& allocator, const TString<TChar>&)}
///     may be used. Freeing is then to be performed by using \alib{strings;TString::Free}.
//==================================================================================================
template<typename TAllocator>
struct AllocatorInterface
{
    /// The allocator type to use.
    TAllocator& allocator;

    /// Constructor. Usually construction is performed by the function operator of the
    /// an \b %Allocator, which then passes itself as parameter \p{pAllocator}.
    /// @param pAllocator The allocator.
    AllocatorInterface(TAllocator& pAllocator) : allocator(pAllocator)                            {}

    /// Allocates memory of the requested \p{size} and \p{alignment}.
    /// @tparam TSize       The type of parameter \p{size}. Deduced by the compiler.
    ///                     (Must be integral.)
    /// @tparam TAlignment  The type of parameter \p{alignment}. Deduced by the compiler.
    ///                     (Must be integral.)
    /// @param size         The allocation size requested.
    /// @param alignment    The required alignment.
    /// @return A pointer to the allocated memory.
    template<typename TSize, typename TAlignment>
    void*           Alloc( TSize size, TAlignment alignment)
    {
        size_t s= size_t(size);
        return allocator.allocate( s, size_t(alignment) );
    }

    /// Allocates memory of <c>sizeof(T)</c> and <c>alignof(T)</c>.
    /// @tparam T The type of object to allocate memory for.
    /// @return A pointer to the allocated memory.
    template<typename T>
    T*  Alloc()
    {
        auto size= sizeof(T);
        return reinterpret_cast<T*>( allocator.allocate(size, alignof(T) ) );
    }

    /// Allocates aligned memory for an array of objects of type \p{T} of size \p{length}
    /// leaving the memory uninitialized.
    /// @tparam T       The array element type.
    /// @tparam TLength The type of parameter \p{length}. Has to be integral.
    ///                 Deduced by the compiler.
    /// @param  length  The capacity of the requested array.
    /// @return A pointer to the first element of the allocated array.
    template<typename T, typename TLength>
    T*      AllocArray( TLength length )
    {
        auto size= sizeof(T[1]) * size_t(length);
        return reinterpret_cast<T*>( allocator.allocate( size, alignof(T[]) ) );
    }

    /// Allocates memory of size- and alignment-suitable for type \p{T} and performs a
    /// C++ "placement-new", passing the given variadic arguments to the type's constructor.
    /// @tparam T     Type of the object to allocate and construct.
    /// @tparam TArgs Types of variadic parameters given with parameter \p{args}.
    /// @param  args  Variadic parameters to be forwarded to the constructor of type \p{T}.
    /// @return A pointer to the initialized array.
    template<typename T, typename... TArgs>
    T*              New( TArgs&& ... args )
    { return new (Alloc<T>())   T( std::forward<TArgs>( args )... ); }

    /// Allocates aligned memory for an array of objects of type \p{T} of size \p{length}.
    /// All array elements are initialized using a "placement-new", passing the given \p{args} to
    /// each element's constructor.
    /// @tparam T      Element type of the array to allocate and construct.
    /// @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
    /// @tparam TSize  The type of parameter \p{length}. Deduced by the compiler.
    ///                (Must be integral.)
    /// @param  length The capacity of the requested array.
    /// @param  args   Variadic parameters to be forwarded to the constructor of each array element
    ///                of type \p{T}.
    /// @return A pointer to the first element of the allocated array.
    template<typename T,typename TSize, typename... TArgs>
    T*          NewArray(TSize length, TArgs&& ... args ) {
        T* mem= AllocArray<T, TSize>( length );
        for( TSize i= 0 ; i < length ; ++i )
            new (mem + i) T(std::forward<TArgs>( args )...);
        return mem;
    }

    /// Calls the destructor of the given \p{object} of type \p{T} and then frees the memory
    /// with the associated allocator.
    /// \attention
    ///   In the context of single inheritance, this convenience function supports deleting pointers
    ///   to objects of base types, also when they were previously allocated using a derived type,
    ///   provided that virtual destructors are in place. Virtual destructors ensure that the
    ///   appropriate destructor is called, preventing resource leaks.
    ///
    /// \attention
    ///   However, in cases of multiple inheritance, passing pointers to base types for deletion
    ///   might lead to undefined behavior. The underlying challenge here is that safely converting
    ///   a base type pointer back to the originally allocated derived type pointer (as required
    ///   for correct memory deallocation) is not feasible with standard C++ mechanisms.
    ///   While the C++ `delete` operator is equipped to handle such scenarios - thanks to internal
    ///   compiler implementations that track the original pointer - standard C++ does not expose
    ///   a portable way to replicate this functionality.
    ///   Consequently, objects utilizing multiple inheritance have to be deleted using the same
    ///   derived type pointer that was used for allocation.
    ///
    /// @tparam T        The type that parameter \p{object} points to.
    /// @param  object   The object to delete.
    template<typename T>
    inline void     Delete(T* object)     { object->~T(); allocator.free(object, sizeof(*object)); }

    /// Destructs all array elements and frees the array's memory.
    /// @tparam T         The pointer type of parameter \p{array}.
    /// @tparam TIntegral Integral type of parameter \p{size}. Deduced by the compiler.
    /// @param  array     Pointer to the first array element.
    /// @param  length    The size of the given \p{mem}.
    template<typename T, typename TIntegral>
    inline
    void DeleteArray(T* array, TIntegral length) {
        for( TIntegral i= 0 ; i < length ; ++i )
            (array + i)->~T();
        allocator.free(array, sizeof(T[1]) * size_t(length) );
    }


    /// Frees memory of size <c>sizeof(T)</c> pointed to by \p{mem}.
    /// This method does \b not call a destructor. To dispose custom types with due destruction,
    /// use method #Delete<T>(T*).
    ///
    /// @tparam T         The pointer type of the given \p{mem} to free.
    /// @param mem        The memory to free.
    template<typename T>
    void Free(T* mem)
    { allocator.free(static_cast<void*>(const_cast<std::remove_const_t<T>*>(mem)),  sizeof(T) ); }

    /// Frees memory of given \p{size} pointed to by \p{mem}.
    /// This method does \b not call a destructor. To dispose custom types with due destruction,
    /// use method #Delete<T>(T*).
    ///
    /// @tparam T         The pointer type of the given \p{mem} to free.
    /// @tparam TIntegral The type that parameter \p{size} is provided with.
    ///                   Deduced by the compiler. (Has to be integral.)
    /// @param mem        The memory to free.
    /// @param size       The size of the given \p{mem}.
    template<typename T, typename TIntegral>
    void Free(T* mem, TIntegral size)
    { allocator.free(static_cast<void*>(const_cast<std::remove_const_t<T>*>(mem)), size_t(size) ); }

    /// Frees memory of an array of objects.
    /// This method does \b not call a destructor. To also destruct the elements of an array,
    /// use method #DeleteArray<T>(T*, TIntegral).
    /// @tparam T         The pointer type of parameter \p{array}.
    /// @tparam TIntegral Integral type of parameter \p{size}. Deduced by the compiler.
    /// @param  array     Pointer to the first array element.
    /// @param  length    The size of the given \p{mem}.
    template<typename T, typename TIntegral>
    inline
    void FreeArray(T* array, TIntegral length)
    {
        allocator.free( static_cast<void*>(const_cast<std::remove_const_t<T>*>(array)),
                        sizeof(T[1]) * size_t(length)                                     );
    }
}; // struct AllocatorInterface

ALIB_WARNINGS_IGNORE_DOCS
/// A concept to identify types that satisfy the interface documented with prototype class
/// \alib{lang;Allocator}.<br>
/// In fact, all methods which are defined in the non-existing class \b Allocator (which is
/// purely living in the documentation to define what an allocator needs to provide) are
/// tested with this concept.
/// This way, its use verifies likewise the due implementation of (custom) allocators.
/// @tparam T       The type to be tested.
template <typename T>
concept IsAllocator=
    requires(T obj, size_t& size, size_t alignment, size_t oldSize, void* mem) {
    typename T::ChainedAllocator;
    { T::MIN_ALIGNMENT                                } -> std::convertible_to<size_t>;
    { T::MAX_ALIGNMENT                                } -> std::convertible_to<size_t>;
    { obj.allocate  (size, alignment)                 } -> std::same_as<void*>;
    { obj.reallocate( mem, oldSize, size, alignment ) } -> std::same_as<void*>;
    { obj.free      (mem, size)                       } -> std::same_as<void>;
    { obj.operator()()                                } -> std::same_as<AllocatorInterface<T>>;
    { obj.allowsMemSplit()                            } -> std::convertible_to<bool>;

    #if ALIB_DEBUG
    { obj.DbgName                                     } -> std::convertible_to<const char*>;
    { obj.dbgAcknowledgeIncreasedAllocSize(mem, size) } -> std::same_as<void>;
    { obj.dbgCheckMemory(mem, size)                   } -> std::same_as<void>;
    #endif
};
ALIB_WARNINGS_RESTORE


//==================================================================================================
/// Implementation of (prototype-) class \alib{lang;Allocator} which performs standard
/// heap-allocation by using <c>std::malloc</c>, <c>std::realloc</c>, and <c>std::free</c>
///
/// This class may be passed as the template parameter \p{TAllocator} with \alib- or custom types
/// that expect an allocator.
/// For example, the type definition \ref alib::AString uses this type and thus implements a common
/// heap-allocated string buffer.
///
/// In contrast to other allocator types, this type is stateless and default-constructible, and
/// temporary instances can be used and disposed of right away with no effort.
/// Therefore, all types across \alib that are templated over an allocator type, provide two
/// versions of each constructor, one that expects an allocator and one that does not expect an
/// external instance. If this type \b %HeapAllocator is used, the constructors that do not
/// expect an allocator become effective.
///
/// @see
/// - Struct \alib{lang;AllocatorMember}, which should be used as a base class to inherit from
///   if an arbitrary allocator is to be stored. A specialization for this \b %HeapAllocator
///   is given, which does not contain a member and hence avoids increasing the footprint
///   of derived types.
/// - Types \alib{monomem;TMonoAllocator}, \alib{monomem;TLocalAllocator}, and
///   \alib{monomem;TPoolAllocator} introduced with module \alib_monomem.
//==================================================================================================
struct HeapAllocator
{
    /// A magic byte, used with the compiler-symbol \ref ALIB_DEBUG_ALLOCATIONS to mark
    /// memory and detect out-of-bounds writes.
    /// @see Method \alib{lang;Allocator::dbgCheckMemory}.
    static constexpr unsigned char      MAGIC= 0xA1;

    /// A magic byte written when memory is freed.
    /// @see Field #CLEAR.
    static constexpr unsigned char      CLEAR= 0xF1;

    /// Evaluates to <c>alignof(std::max_align_t)</c>.
    /// @see Field \alib{lang;Allocator::MIN_ALIGNMENT}.
    static constexpr size_t             MIN_ALIGNMENT                   = alignof(std::max_align_t);

    /// Evaluates to <c>alignof(std::max_align_t)</c>.
    /// @see Field \alib{lang;Allocator::MAX_ALIGNMENT}.
    static constexpr size_t             MAX_ALIGNMENT                   = alignof(std::max_align_t);

    /// The type of the allocator that this allocator uses underneath.
    /// In this case, no chaining is available, hence this evaluates to <c>void</c>.
    using ChainedAllocator = void;

    /// Allocate memory using <c>std::malloc</c>.
    /// @param size      The size of memory the block to allocate in bytes. With this allocator
    ///                  this is not an input/output parameter.
    /// @param alignment This type ignores the alignment parameter because <c>std::malloc</c>
    ///                  always uses <c>alignof(std::max_align_t)</c>.
    /// @return Pointer to the allocated memory block.
    inline void*  allocate( size_t size, size_t alignment )                                  const {
        ALIB_ASSERT_ERROR( alignment <= alignof(std::max_align_t), "ALIB",
          "The HeapAllocator is not designed to provide alignments greater "
          "than alignof(std::max_align_t): {} > {}.  ", alignment, alignof(std::max_align_t) )
        (void)  alignment;
        void* mem= std::malloc(lang::DbgAlloc::extSize(size));
        DbgAlloc::annotate(mem, size, MAGIC);
        return mem;
    }

    /// Shrinks or grows a piece of memory.
    /// If a new allocation was performed, the existing data is copied.
    /// Note that unlike specified by the prototype, parameter \p{newSize} is not provided as a
    /// reference. This allocator will not allocate bigger objects.
    /// @param  mem      The memory to reallocate.
    /// @param  oldSize  The current size of \p{mem}.
    /// @param  newSize  The now required size of \p{mem} in bytes.
    ///                  With this allocator this is not an input/output parameter.
    /// @return Pointer to the re-allocated memory.
    inline void* reallocate(void* mem, size_t oldSize, size_t newSize, size_t  ) {
        DbgAlloc::checkMem( mem, oldSize, MAGIC, "HeapAllocator" );
        void* newMem= std::realloc(mem, lang::DbgAlloc::extSize(newSize));
        DbgAlloc::annotate(newMem, newSize, MAGIC);
        return newMem;
    }

    /// Frees memory by invoking <c>std::free</c>.
    /// @param mem   The memory to dispose.
    /// @param size  The size of the given \p{mem}.
    inline void free(void* mem, size_t size)                                                 const {
        DbgAlloc::checkMem( mem, size, MAGIC, "HeapAllocator" );
        DbgAlloc::clearMem(mem, size, CLEAR);
        std::free(mem);
    }

    /// This is an empty implementation of the prototyped method.
    /// It is empty because this allocator never returns a higher allocation size than requested.
    /// \alib{lang;Allocator::dbgAcknowledgeIncreasedAllocSize}.
    /// @tparam TSize    The type of parameter \p{allocSize}. (Deduced by the compiler.)
    template<typename TSize>
    void dbgAcknowledgeIncreasedAllocSize( void*, TSize  )                                  const {}

    /// Returns a temporary object (which is usually optimized out together with a call to this
    /// operator) providing high-level convenience methods for allocation.
    /// @see Class \alib{lang::AllocatorInterface}
    /// @return A temporary high-level interface into the allocator.
    AllocatorInterface<HeapAllocator> operator()()                                    const noexcept
    { return AllocatorInterface<HeapAllocator>(*const_cast<HeapAllocator*>(this)); }

    /// This is a constexpr static method that determines if a type allows splitting memory
    /// and later passing the pieces back to free.<br>
    /// @return \c false.
    static constexpr bool allowsMemSplit()                                         { return false; }

    #if ALIB_DEBUG
    /// With this allocator, the debug-name is \c constexpr "HeapAllocator".
    static constexpr const char*    DbgName                                       = "HeapAllocator";
    #endif

    /// If the compiler-symbol \ref ALIB_DEBUG_ALLOCATIONS is not set, this method is empty and will
    /// be optimized out. Otherwise, this will raise an \alib_assertion if the piece of allocated
    /// memory is corrupted or its allocation size is not rightfully given by the using code.
    /// @see Chapter \ref alib_contmono_further_debug of the Programmer's Manual.
    ///
    /// @tparam TSize   The type of parameter \p{size}. (Deduced by the compiler.)
    /// @param mem   The address of the allocated object.
    /// @param size  The requested allocation size of the object.
    template<typename TSize>
    void dbgCheckMemory( void* mem, TSize size )
    { DbgAlloc::checkMem( mem, size, MAGIC, "HeapAllocator" ); }

};  // struct HeapAllocator


//==================================================================================================
/// This templated class is used to inherit an allocation member. The rationale for choosing
/// inheritance instead of just having types defining a member for an allocator, lies in the
/// C++ standard's concept of \https{Empty Base Optimization,en.cppreference.com/w/cpp/language/ebo}.
///
/// The class has a specialization with \ref lang::AllocatorMember<HeapAllocator>, which does
/// not define a member and thus does not increase the footprint of a type that inherits it.
/// With the standard implementation, a reference to the allocator specified with template
/// parameter \p{TAllocator} is defined.
///
/// Consequently, this type is used to implement templated types that are allocation agnostic.
/// Across \alib, a template parameter specifying an allocator is always named \p{TAllocator}.
/// Types with this template parameter, in parallel inherit this type.
/// A prominent example is the string buffer type of module \alib_strings, which usually is known
/// through it's alias name \ref alib::AString, which performs classic heap allocation.
/// Its alias type definition, is given as:
///   \snippet "strings/detail/tastring.inl"     DOX_MONOMEM_ALLOCMEMBER
/// Obviously the original type \b TString has two template parameters, one specifying the
/// character type, the other specifying the allocator. Now looking at the
/// \alib{strings;TAString;reference documentation of type TAString}, it can be seen that this type
/// \b AllocatorMember is a protected base class and template parameter \p{TAllocator} is forwarded
/// to this base.
///
/// \note As explained, this type is useful if allocation agnostic types are needed. This is true
///       for types in this library. However, in many end-user cases, the allocation strategy will
///       be fixed to a certain allocator, and therefore in this case, neither a templated approach,
///       nor the helper-struct is needed.
///
/// @tparam TAllocator The allocator type of the inherited member, as prototyped with
///                    \alib{lang;Allocator}.
//==================================================================================================
template<typename TAllocator>
struct AllocatorMember
{
  protected:
    TAllocator*   allocator; ///< The allocator stored.

  public:
    /// Exposes the allocator type.
    using AllocatorType = TAllocator;

    /// Default constructor. Only applicable if the allocator type is default-constructible,
    /// i.e., \alib{lang;HeapAllocator}.
    /// @tparam TIf Defaulted, must not be given.
    template<typename TIf= TAllocator>
    requires std::is_default_constructible_v<TIf>
    AllocatorMember()                                                                             {}

    /// Constructor.
    /// @param pAllocator A reference to the allocator to store in member #allocator.
    AllocatorMember( TAllocator& pAllocator )                                               noexcept
    : allocator(&pAllocator)                                                                      {}

    /// Returns the reference to the allocator.
    /// @return The allocator given with construction.
    TAllocator&         GetAllocator()                         const noexcept { return *allocator; }

    /// This is a convenience operator that returns the \alib{lang;AllocatorInterface} for the
    /// stored allocator (by calling \alib{lang;Allocator::operator()()}).
    /// @return The allocator interface of the stored allocator.
    AllocatorInterface<TAllocator> AI()                    const noexcept { return (*allocator)(); }
};

/// Specialization of this helper-struct for type \alib{lang;HeapAllocator}. Does not define a
/// member and thus does not increase the footprint of a type that inherits this specialization.
template<>
struct AllocatorMember<HeapAllocator>
{
    /// This is an empty instance of the empty heap allocation type..
    /// It is used to receive a non-const reference with method #GetAllocator.
    static constexpr HeapAllocator HEAP_ALLOCATOR_INSTANCE {};

    /// Default Constructor.
    AllocatorMember()                                                                      =default;

    /// Constructor.
    /// @param heapAllocator This parameter is ignored, as always #HEAP_ALLOCATOR_INSTANCE is used.
    AllocatorMember(const HeapAllocator& heapAllocator)           noexcept { (void) heapAllocator; }

    /// Static implementation of interface function. (Optimized out.)
    /// @return A reference to the 'fake' object defined with# HEAP_ALLOCATOR_INSTANCE.
    HeapAllocator& GetAllocator()                                                     const noexcept
    { return const_cast<HeapAllocator&>(HEAP_ALLOCATOR_INSTANCE); }

    /// This is a convenience operator which returns the \alib{lang;AllocatorInterface} for the
    /// stored allocator (by calling \alib{lang;Allocator::operator()()}).
    /// @return The allocator interface of the stored allocator.
    AllocatorInterface<HeapAllocator> AI()      const noexcept { return HEAP_ALLOCATOR_INSTANCE(); }
};


//==================================================================================================
// C++ standard allocators based on ALib allocators
//==================================================================================================

//==================================================================================================
/// This struct is an implementation of C++ standard library type <c>std::allocator</c>.
/// It is to be used with container types provided by the C++ standard library in combination
/// with \alib{lang;Allocator;ALib Allocators}.
///
/// With the inclusion of module \alib_monomem in the \alibbuild, this allocator is suitable to use
/// cases of \ref alib_contmono_intro_strictweak "strict or weak monotonic allocation".
///
/// ### Shortcuts for Specific ALib Allocators ###
/// The following alias type definitions exist for this type, each addressing a specific allocator:
/// - \alib{StdMA}, and
/// - \alib{StdPA}.
///
/// Along with this, shortcuts to the container types are likewise provided with:
/// - \alib{StdVectorMA}, \alib{StdVectorPA},
/// - \alib{StdListMA}, \alib{StdListPA}, and
/// - \alib{StdDequeMA}, \alib{StdDequePA}.
///
/// \anchor alib_ns_monomem_stdma_reset
/// ### Resetting A Container ###
/// While the dedicated container types provided with module \alib_containers offer a method
/// named \b Reset (see, for example, \alib{containers;HashTable::Reset}), the C++ standard
/// containers do not. This is a challenge because their internal memory will be invalid
/// with a reset. For example, if allocator type \alib{MonoAllocator} was used with class
/// <c>std::vector</c>, there is no interface method that makes the vector "forget" its internal
/// data array. Its method <c>shrink_to_fit</c> by its specification is not forced to shrink
/// anything or even dispose the data if the size was \c 0 when called.
/// This is implementation-dependent.
///
/// The way out is as simple as radical: The container is just to be destructed and reconstructed
/// "in place". This can be done using a <em>C++ placement-new</em>. The following
/// code snippet demonstrates this:
/// \snippet "ut_stdcontainers.cpp"     DOX_MONOMEM_STDMA_PLACEMENT_NEW
///
/// \see
///   - An alternative version that \ref alib_contmono_intro_recycling "recycles nodes" (and also
///     bucket arrays in case of <c>std::unordered_set</c> and <c>std::unordered_map</c>), is
///     provided with sibling type \alib{lang;StdRecyclingAllocator}.
///   - Type definitions \alib{StdMA} and \alib{StdPA} given with module \alib_monomem.
///   - Further shortcuts for standard container types, given with \alib{StdVectorMA},
///     \alib{StdVectorPA}, \alib{StdListMA}, \alib{StdListPA}, \alib{StdDequeMA}, and
///     \alib{StdDequePA}.
///
/// @tparam T          The type of objects to be allocated.
/// @tparam TAllocator The allocator type, as prototyped with \alib{lang;Allocator}.
//==================================================================================================
template<typename T, typename TAllocator>
struct StdAllocator :  AllocatorMember<TAllocator>
{
    using value_type                            = T              ; ///< Type definition as required by C++ library standards.
    using size_type                             = std::size_t    ; ///< Type definition as required by C++ library standards.
    using difference_type                       = std::ptrdiff_t ; ///< Type definition as required by C++ library standards.
    using propagate_on_container_move_assignment= std::false_type; ///< Type definition as required by C++ library standards.

    /// Exposes template parameter \p{TAllocator}.
    using AllocatorType  = TAllocator;

    /// The type of the base class that stores the allocator.
    using allocBase      =   AllocatorMember<TAllocator>;

    /// Parameterless constructor used with heap allocation.
    constexpr StdAllocator()                                                      noexcept =default;

    /// Constructor for the initial allocator instantiation.
    /// @param pAllocator   The allocator.
    constexpr
    StdAllocator( TAllocator& pAllocator ) noexcept
    : allocBase(pAllocator)                                                                       {}

    /// Defaulted copy constructor
    constexpr StdAllocator( const StdAllocator&  )                                noexcept =default;

    /// Defaulted move constructor
    constexpr StdAllocator(       StdAllocator&& )                                noexcept =default;

    /// Copy constructor using an instance of a different template type.
    /// @tparam TSibling  The originating allocator's type (<b>StdAllocator<X></b>).
    /// @param  origin    The originating allocator of type \p{TSibling} .
    template<typename TSibling>
    requires alib::lang::IsAllocator<typename TSibling::allocator>
    constexpr
    StdAllocator(const TSibling& origin) noexcept
    : allocBase(origin)                                                                           {}

    /// Copy constructor using an instance of a different template type.
    /// @tparam TSibling  The originating allocator's type (<b>StdAllocator<X></b>).
    /// @param  origin    The originating allocator of type \p{TSibling} .
    template<typename TSibling>
    StdAllocator( const StdAllocator<TSibling,TAllocator>& origin)
    : allocBase(origin)                                                                           {}

    /// Parameterless destructor used with heap allocation.
    constexpr ~StdAllocator()                                                     noexcept =default;

  //################################################################################################
  // ### Allocate/de-allocate
  //################################################################################################
    /// Passes the allocation request to the \b AllocatorMember.
    ///
    /// @param n  The number of requested objects to allocate storage for.
    /// @return Pointer to the start of the array of \p{n} objects of type \p{T}.
    [[nodiscard]] constexpr
    T*      allocate( size_t n )             { return allocBase::AI().template AllocArray<T>( n ); }

    #if defined(__cpp_lib_allocate_at_least) || DOXYGEN
        /// The C++23 extension of std::allocator finally supports \alib mechanics of using
        /// oversized allocation objects.
        /// @param n  The number of requested objects to allocate storage for.
        /// @return The Pointer to the start of the allocated objects of type \p{T} as well as
        ///         the number of objects allocated, which will be equal or higher than \p{n}.
        [[nodiscard]] constexpr
        std::allocation_result<T*, std::size_t>
        allocate_at_least( std::size_t n ) {
            std::allocation_result<T*, std::size_t> result;
            result.count= n * sizeof(T);
            result.ptr= static_cast<T*>( allocBase::GetAllocator().allocate(result.count, alignof(T)));
            result.count/= sizeof(T);
            return result;
        }

    #endif

    /// Frees the given array of objects.
    /// @param p Pointer to the previously allocated memory.
    /// @param n The number of objects allocated.
    constexpr
    void deallocate( T* p, std::size_t n )                    { allocBase::AI().FreeArray( p, n ); }

}; // struct StdAllocator


/// Comparison operator for type \b StdAllocator<T, TAllocator>.
/// @param  lhs  The left-hand side allocator.
/// @param  rhs  The right-hand side allocator.
/// @tparam TLhs The allocation type of the left-hand side allocator.
/// @tparam TRhs The allocation type of the right-hand side allocator.
/// @return \c true if both allocators use the same allocator internally, \c false otherwise.
template< typename TLhs, typename TRhs, typename TAllocator >
constexpr bool operator==( const StdAllocator<TLhs, TAllocator>& lhs,
                           const StdAllocator<TRhs, TAllocator>& rhs )                    noexcept {
    return &lhs.GetAllocator() == &rhs.GetAllocator();
}

//==================================================================================================
/// This structs' name stands for "run-time type recycling allocator".
/// The struct encapsulates an allocator and recycles (caches) objects whose size and alignment are
/// only determined and detected at run-time.
/// This type should \e only be used in combination with struct
/// \alib{lang;StdRecyclingAllocator} or in similar (unlikely) situations when the type of
/// recyclable objects is unknown at compile time.
/// \note The reason why the size of node types of containers of the C++ standard library is known
///       only at run-time, is that the implementation of the containers is not standardized.
///       Hence, in fact the node size is of course known at compile time, it is just not accessible
///       in a platform-independent fashion.
///
/// This type is provided by \alib to support recycling of monotonically allocated memory with
/// container types provided by the C++ standard library. With those, the internal node types are
/// unspecified.
/// A typical implementation and use of the containers will always allocate the same object size.
/// This is detected with the first allocation, and from this point in time, future deallocations
/// will recycle pieces of memory of exactly this type.
///
/// If type \alib{lang::HeapAllocator} is given with the template parameter \p{TAllocator}, this
/// recycler uses <c>std::malloc</c> and <c>std::free</c> for memory allocation and deallocation.
/// While such an operation mode does not imply all performance benefits of monotonic allocation
/// scenarios, still, the recycling of node objects may avoid many malloc/free operations and
/// therefore reduce memory fragmentation significantly.<br>
/// Method #RecycleChunk will not create recyclable objects in that mode but will duly
/// use <c>std::free</c> to directly free a chunk of non-object size.
///
/// @tparam TAllocator The allocator type, as prototyped with \alib{lang;Allocator}.
//==================================================================================================
template<typename TAllocator>
struct RTTRAllocator : AllocatorMember<TAllocator>
{
    /// The type of the base class that stores the allocator.
    using allocBase=   AllocatorMember<TAllocator>;

    /// The node type of the internal node type used for stacking recyclables. Besides inheriting
    /// the single-list pointer, this type is empty.
    struct Node : SidiNodeBase<Node>
    {};

    /// List of destructed objects available for recycling.
    SidiListHook<Node>          stack;

    /// The object size of recyclables. Will be detected with the first invocation of #Get.
    size_t                      detectedObjectSize                                               =0;

    /// The required object alignment. Will be detected with the first invocation of #Get.
    size_t                      detectedObjectAlignment                                          =0;

    #if ALIB_DEBUG
    /// The detected object's run-time type information struct.<br>
    /// Available only in debug-builds.
    const std::type_info*   dbgDetectedObjectTypeInfo                                      =nullptr;

    /// Flag on raising an \alib_warning. Defaults to \c true and set to \c false when a
    /// corresponding warning was given.<br>
    /// Available only in debug-builds.
    bool                    dbgWarnDifferentObjectTypeAlloc                                   =true;

    /// Flag on raising an \alib_warning. Defaults to \c true and set to \c false when a
    /// corresponding warning was given.<br>
    /// Available only in debug-builds.
    bool                    dbgWarnDifferentObjectTypeDealloc                                 =true;

    /// Flag on raising an \alib_warning. Defaults to \c true and set to \c false when a
    /// corresponding warning was given.<br>
    /// Available only in debug-builds.
    bool                    dbgWarnDeallocationPriorToAllocation                              =true;

    /// Flag on raising an \alib_warning. Defaults to \c true and set to \c false when a
    /// corresponding warning was given.<br>
    /// Available only in debug-builds.
    bool                    dbgWarnRecycleChunkPriorToAllocation                              =true;
    #endif

    /// Constructor taking the monotonic allocator.
    ///
    /// If \c nullptr is given, this recycler uses <c>std::malloc</c> and <c>std::free</c> for memory
    /// allocation and deallocation.
    ///
    /// @param pAllocator The monotonic allocator. If \c nullptr is given, still recycling of node
    ///                   objects is performed.
    RTTRAllocator( TAllocator& pAllocator )                 noexcept : allocBase(pAllocator)      {}

  //################################################################################################
  // ### Allocation
  //################################################################################################
#if DOXYGEN
    //==============================================================================================
    /// Allocates or recycles memory for the dedicated object type. With the first invocation
    /// of this method, this type is determined object type.
    /// In debug-builds, this method will raise an \alib_warning in case a different
    /// object type is requested.
    ///
    /// @param size         The requested size.
    /// @param alignment    The requested alignment
    /// @param dbgTypeInfo  The type information of the object to allocate.
    ///                     Available only in debug-builds.
    /// @return The requested memory.
    //==============================================================================================
    inline
    void* Get( size_t size, size_t alignment, const type_info& dbgTypeInfo);
#else
    void* Get( size_t size, size_t alignment ALIB_DBG(, const std::type_info& dbgTypeInfo) ) {
        ALIB_DBG( (void) dbgTypeInfo; )

        // detect object size
        if(detectedObjectSize == 0) {
            detectedObjectSize               = size;
            detectedObjectAlignment          = alignment;
  ALIB_DBG( dbgDetectedObjectTypeInfo        = &dbgTypeInfo;   )

            ALIB_MESSAGE( "RTTRA", "Object type detected     : ", &dbgTypeInfo )
            ALIB_ASSERT_ERROR( alignment >= alignof(Node), "RTTRA",
                "Struct RTTRAllocator cannot be used to recycle types with an alignment "
                "smaller than {}. Requested: {}", alignof(Node), alignment )
        }

        if( (size == detectedObjectSize) && (detectedObjectAlignment== alignment) ) {
            if( !stack.isEmpty() ) {
                #if ALIB_DEBUG_MEMORY
                    ALIB_MESSAGE( "RTTRA", "Recycling object.    Type: ", &dbgTypeInfo )
                #endif
                return reinterpret_cast<char*>( stack.popFront() );
        }   }
        #if ALIB_DEBUG
        else {
            if(dbgWarnDifferentObjectTypeAlloc) {
                ALIB_MESSAGE( "RTTRA",
                "A different object was requested for allocation!\n"
                "  Previous type : <{}>\n"
                "  Requested type: <{}>\n"
                "Note: This allocator may not be efficient when used.\n"
                "      If this is a use case using a 'std' library container, this message indicates\n"
                "      that a RTTRAllocator was shared between different container instantiations.\n"
                "      If this is not the case, than an 'unusual' implementation of such C++ library may\n"
                "      prevent this concept from working. See ALib manual for further information.",
                dbgDetectedObjectTypeInfo, &dbgTypeInfo )


                dbgWarnDifferentObjectTypeAlloc= false;
            }
            ALIB_MESSAGE( "RTTRA", "Allocating a different object type \"{}\"\n"
                                   "  Note: This object cannot be recycled.", &dbgTypeInfo )
            return allocBase::AI().Alloc( size, alignment );
        }
        #endif

        #if ALIB_DEBUG_MEMORY
            ALIB_MESSAGE( "RTTRA", "Allocating object.   Type: \"{}\"", &dbgTypeInfo )
        #endif

        return allocBase::AI().Alloc( size, alignment );
    }
#endif // DOXYGEN

#if DOXYGEN
    //==============================================================================================
    /// Allocates memory for a type different to the dedicated, detected object type.
    /// @param size         The requested size.
    /// @param alignment    The requested alignment
    /// @param dbgTypeInfo  The type information of the object to allocate.
    ///                     Available only in debug-builds.
    /// @return The requested memory.
    //==============================================================================================
    inline
    void* AllocUnrelated( size_t size, size_t alignment, const type_info& dbgTypeInfo );
#else
    void* AllocUnrelated(size_t size,size_t alignment ALIB_DBG(,const std::type_info& dbgTypeInfo)){
        ALIB_DBG( (void) dbgTypeInfo; )
        #if ALIB_DEBUG_MEMORY
            ALIB_MESSAGE( "RTTRA", "Allocating other.    Type: <{}>", &dbgTypeInfo )
        #endif
        return allocBase::AI().Alloc( size, alignment );
    }
#endif // DOXYGEN


  //################################################################################################
  // ### Deallocation
  //################################################################################################
#if DOXYGEN
    //==============================================================================================
    /// Deallocates memory for the dedicated, detected object type.
    ///
    /// In debug-builds, this method will raise an \alib_warning in case a different
    /// object type is deallocated as had been detected.
    /// Furthermore, a \ref alib_mod_assert "warning is raised" in case no previous call to #Get
    /// has been performed.
    ///
    /// @param mem          The object to deallocate.
    /// @param size         The size of the object to deallocate.
    /// @param alignment    The alignment of the object to deallocate.
    /// @param dbgTypeInfo  The type information of the object to de-allocate.
    ///                     Available only in debug-builds.
    //==============================================================================================
    inline
    void Recycle( void* mem, size_t size, size_t alignment, const type_info& dbgTypeInfo );
#else
    void Recycle( void* mem, size_t size, size_t alignment
                  ALIB_DBG(, const std::type_info& dbgTypeInfo) ) {
        if(    size      == detectedObjectSize
            && alignment == detectedObjectAlignment )
        {
            stack.pushFront( reinterpret_cast<Node*>( mem ) );
            #if ALIB_DEBUG_MEMORY
                ALIB_MESSAGE( "RTTRA", "Stacking object.     Type: ", &dbgTypeInfo )
            #endif
        } else {
            allocBase::GetAllocator().free(mem, size);
            #if ALIB_DEBUG
                if( detectedObjectSize == 0 ) {
                    if( dbgWarnDeallocationPriorToAllocation) {
                        ALIB_WARNING( "RTTRA",
                        "Deallocation before a first object allocation needed to detect recyclable type!\n"
                        "  De-allocated object type: <{}>\n"
                        "Note: This allocator may not be efficient when used.\n"
                        "      If this is a use case using a 'std' library container, this message indicates\n"
                        "      an 'unusual' implementation of such C++ standard library."
                        , &dbgTypeInfo )
                    }
                    dbgWarnDeallocationPriorToAllocation= false;
                } else {
                    if( dbgWarnDifferentObjectTypeDealloc ) {
                        ALIB_WARNING( "RTTRA",
                        "A different object for was requested for de-allocoation!\n"
                        "  Previous type : <{}>\n"
                        "  Requested type: <{}>\n"
                        "Note: This allocator may not be efficient when used.\n"
                        "      If this is a use case using a 'std' library container, this message indicates\n"
                        "      that a RTTRAllocator was shared between different container instantiations.\n"
                        "      If this is not the case, than an 'unusual' implementation of such C++ library may\n"
                        "      prevent this concept from working. See ALib manual for further information"
                        , dbgDetectedObjectTypeInfo, &dbgTypeInfo )
                        dbgWarnDifferentObjectTypeDealloc= false;
                }   }
            #endif
    }   }
#endif


#if DOXYGEN
    //==============================================================================================
    /// Deallocates memory for a type different to the dedicated, detected object type.
    ///
    /// In debug-builds, this method will raise an \alib_warning no previous call to #Get
    /// has been performed.<br>
    /// Furthermore, a \ref alib_mod_assert "warning is raised" in case that the provided memory
    /// chunk is too small to be sliced into at least one recyclable object.
    ///
    /// @param mem          The object to deallocate.
    /// @param size         The size of the object to deallocate.
    /// @param dbgTypeInfo  The type information of the object to de-allocate.
    ///                     Available only in debug-builds.
    //==============================================================================================
    inline
    void RecycleChunk( void* mem, size_t size, const type_info& dbgTypeInfo );
#else
    void RecycleChunk( void* memUnaligned, size_t size
                       ALIB_DBG(, const std::type_info& dbgTypeInfo) ) {
        // This whole exercise must only be done with MonoAllocator and if ALIB_DEBUG_ALLOCATIONS is
        // not set. (This is ensured by 'TAllocator::allowsMemSplit()')
        if( !TAllocator::allowsMemSplit() ) {
            allocBase::GetAllocator().free(memUnaligned, size);
            return;
        }

        // if object size not detected, yet, we cannot create recyclables.
        ALIB_DBG( size_t origSize= size; )
        if( detectedObjectSize == 0) {
            #if ALIB_DEBUG
            if( dbgWarnRecycleChunkPriorToAllocation ) {
                ALIB_WARNING( "RTTRA",
                "Deallocation before a first object allocation needed to detect recyclable type!\n"
                "  De-allocated object type: <{}>.\n"
                "Note: If this recycler is used with a 'std' library container, this either\n"
                "      indicates an 'unusual' implementation of such C++ standard library,\n"
                "      or a manual shrink of the capacity without any prior object insertion.\n",
                &dbgTypeInfo )
                dbgWarnRecycleChunkPriorToAllocation = false ;
            }
            #endif

            return;
        }

        // align beginning of buffer
        char* mem = reinterpret_cast<char*>( (size_t(memUnaligned) + detectedObjectAlignment - 1) & ~(detectedObjectAlignment -1) );
        size-= size_t(mem - reinterpret_cast<char*>(memUnaligned));

        // create recyclables
        ALIB_DBG( size_t cntStackedObjects= 0; )
        while(size > detectedObjectSize) {
            stack.pushFront( reinterpret_cast<Node*>( mem ) );
            mem  =  reinterpret_cast<char*>(mem) + detectedObjectSize;
            size -= detectedObjectSize;
            ALIB_DBG( ++cntStackedObjects; )
        }

        #if ALIB_DEBUG
            if( cntStackedObjects > 0 ) {
                ALIB_WARNING( "RTTRA",
                "De-allocated chunk's size is smaller than detected object size.\n"
                "  Deallocated object: Type: <{}>\n"
                "                      Size: {} bytes\n"
                "  Detected object:    Type: <{}>\n"
                "                      Size: {} bytes, alignment: {}\n"
                "Note: If this recycler is used with a <std::unordered_map> or <std::unordered_set>,\n"
                "      this message may be eliminated by reserving a reasonable initial bucket size.",
                &dbgTypeInfo, origSize, dbgDetectedObjectTypeInfo,
                detectedObjectSize, detectedObjectAlignment )
            }

        #endif
        #if ALIB_DEBUG_MEMORY
            ALIB_MESSAGE( "RTTRA",
               "Stacking {} objects from de-allocated memory of size {} (lost {} bytes).\n"
               "Deallocated type: {}", cntStackedObjects, origSize,
               origSize - cntStackedObjects * detectedObjectSize, &dbgTypeInfo )
        #endif
    }
#endif

}; // RTTRAllocator

//==================================================================================================
/// Implementation of <c>std::allocator</c> to be used with container types provided by the
/// C++ standard library.
///
/// As the C+++ library's specification does not include details of the container class's
/// implementation, this allocator uses type \alib{lang;RTTRAllocator} which detects
/// node types automatically.
///
/// \attention
///   It cannot be guaranteed that a certain, uncommon implementation of the C++ library
///   allocates memory in a way that this allocator truly recycles objects, and - even if the
///   authors of \alib are optimistic hat this type works with any implementation - it is
///   recommended to use the alternative container types found in this \alibmod_nl.
///
/// \see
///   For background information about monotonic memory allocation, recycling, and the issues
///   of C++ standard container types, see the \ref alib_mods_contmono "Programmer's Manual" of
///   \alibmods_nl \b Containers and \b Monomem.
///
/// @tparam T          The type of objects to be allocated.
/// @tparam TAllocator The allocator type, as prototyped with \alib{lang;Allocator}.
//==================================================================================================
template<typename T, typename TAllocator>
struct StdRecyclingAllocator
{
  //################################################################################################
  // ### Type definitions
  //################################################################################################
    using size_type      =  size_t        ; ///< Type definition as required by C++ library standards.
    using difference_type=  ptrdiff_t     ; ///< Type definition as required by C++ library standards.
    using value_type     =  T             ; ///< Type definition as required by C++ library standards.
    using is_always_equal= std::false_type; ///< Type definition as required by C++ library standards.
    using reference      =  T&            ; ///< Type definition as required by C++ library standards.
    using const_reference=  const T&      ; ///< Type definition as required by C++ library standards.
    using AllocatorType  =  TAllocator    ; ///< Exposes template parameter \p{TAllocator}.

  //################################################################################################
  // ### Fields
  //################################################################################################
    /// The only member of this allocator type used to perform all duties.
    RTTRAllocator<TAllocator>& recycler;

  //################################################################################################
  // ### Construction/Destruction
  //################################################################################################
    /// Defaulted copy constructor
    constexpr StdRecyclingAllocator( const StdRecyclingAllocator& )               noexcept =default;

    /// Defaulted move constructor
    constexpr StdRecyclingAllocator(       StdRecyclingAllocator&& )              noexcept =default;

    /// Copy constructor using an instance of a different template type.
    /// @tparam TSibling            The originating allocator's type (<b>StdRecyclingAllocator<X></b>).
    /// @param  StdRecyclingAllocator  The originating allocator of type \p{TSibling} .
    template<typename TSibling>
    StdRecyclingAllocator( TSibling& StdRecyclingAllocator)
    : recycler(StdRecyclingAllocator.recycler)                                                    {}

    /// Constructor for the initial allocator instantiation.
    /// @param  pRecycler The recycler to for allocations and deallocations.
    constexpr StdRecyclingAllocator( RTTRAllocator<TAllocator>& pRecycler)
    : recycler(pRecycler)                                                                         {}

  //################################################################################################
  // ### Comparison
  //################################################################################################
    /// Comparison operator.
    /// @tparam U    The allocation type of the other allocator.
    /// @param  rhs  The right-hand side allocator.
    /// @return \c true if this and \p{rhs} use the same #recycler, \c false otherwise.
    template< typename U >
    bool operator==( const StdRecyclingAllocator<U,TAllocator>& rhs )                 const noexcept
    { return recycler == rhs.recycler; }

    /// Comparison operator.
    /// @tparam U    The allocation type of the other allocator.
    /// @param  rhs  The right-hand side allocator.
    /// @return \c false if this and \p{rhs} use the same #recycler, \c true otherwise.
    template< typename U >
    bool operator!=( const StdRecyclingAllocator<U,TAllocator>& rhs )                 const noexcept
    { return recycler != rhs.recycler; }

  //################################################################################################
  // ### Allocate/de-allocate
  //################################################################################################
    /// Implementation of <c>std::allocator</c> interface.
    /// Dependent on whether a single or multiple object is requested, this method invokes
    /// either \alib{lang;RTTRAllocator::Get} or \alib{lang;RTTRAllocator::AllocUnrelated} on the
    /// field #recycler.
    ///
    /// @param n  The number of requested objects to allocate storage for.
    /// @return Pointer to the start of the array of \p{n} objects of type \p{T}.
    [[nodiscard]]
    T*      allocate( size_t n ) {
        if( n == 1 )
            return reinterpret_cast<T*>( recycler.Get( sizeof(T),
                                                       alignof(T)
                                            ALIB_DBG(, typeid(T))   )   );

        return reinterpret_cast<T*>(     recycler.AllocUnrelated( sizeof(T) * n,
                                         alignof(T)
                              ALIB_DBG(, typeid(T) )                                  ) );
    }

    /// Implementation of <c>std::allocator</c> interface.
    /// Dependent on whether a single or multiple objects are de-allocated (parameter \p{n}), this
    /// method invokes either \alib{lang;RTTRAllocator::Recycle} or
    /// \alib{lang;RTTRAllocator::RecycleChunk} on field #recycler.
    ///
    /// @param p  Pointer to the object to deallocate.
    /// @param n  The number of objects to de-allocate.
    void deallocate( T* p, std::size_t n ) {
        // deallocate node
        if( n == 1 ) recycler.Recycle(       p, sizeof(T)    , alignof(T)  ALIB_DBG(, typeid(T) ) );
        else         recycler.RecycleChunk ( p, sizeof(T) * n              ALIB_DBG(, typeid(T) ) );
    }
}; // struct StdRecyclingAllocator

} // namespace alib[::lang]

/// Type alias in namespace \b alib.
using     HeapAllocator =   lang::HeapAllocator;

} // namespace [alib]
