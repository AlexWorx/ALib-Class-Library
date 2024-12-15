//==================================================================================================
/// \file
/// This header file is part of the \aliblong. It does not belong to an \alibmod and is
/// included in any \alibdist.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LANG_ALLOCATION
#define HPP_ALIB_LANG_ALLOCATION 1
#pragma once
#include "alib/lang/tmp.hpp"
#include "alib/lang/integers.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <utility>

namespace alib { namespace lang {


/// Provides debugging utility methods used by allocator types \alib{lang;HeapAllocator},
/// \alib{monomem;TMonoAllocator}, and \alib{monomem;TPoolAllocator}.
/// Each method is empty and thus optimized out in case compiler symbol
/// \ref ALIB_DEBUG_ALLOCATIONS is not set.
/// @see Chapter \ref alib_contmono_further_debug of the Programmer's Manual.
struct DbgAlloc
{
    /// Adds needed space for \b 4 magic bytes plus the <c>size_t</c> value that is stored.
    /// @tparam TIntegral The type of given \p{size}.
    ///                   (Deduced by the compiler, templated for convenience.)
    /// @param  size      The given size or the extended size.
    /// @return The truly allocated size.
    template<class TIntegral>
    static constexpr size_t extSize (TIntegral size)
    {
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
    template<class T>
    static constexpr void annotate(T* mem, size_t size, unsigned char magic)
    {
        #if ALIB_DEBUG_ALLOCATIONS
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                auto* cMem= reinterpret_cast<unsigned char*>(mem);
                size_t idx= size;
                for( ; idx< size + 4; ++idx)
                    cMem[idx] = magic;
                for( size_t i= 0; i< sizeof(size_t); ++i)
                {
                    cMem[idx++]= static_cast<unsigned char>( size & 0xFF );
                    size>>=8;
                }
            ALIB_WARNINGS_RESTORE
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
    template<class T>
    static constexpr void clearMem(T* mem, size_t size, unsigned char magic)
    {
        #if ALIB_DEBUG_ALLOCATIONS
            memset( reinterpret_cast<char*>(mem), magic, extSize(size) );
        #else
            (void) mem; (void) size; (void) magic;
        #endif
    }

    #include "alib/lang/callerinfo_functions.hpp"
    /// Raises an \alib assertion if the piece of allocated memory is corrupted or its allocation
    /// size is not rightfully given by the using code.
    ///
    /// @tparam TSize    The type of parameter \p{size}. (Deduced by the compiler.)
    /// @param  mem      The address of the allocated object.
    /// @param  size     The requested allocation size of the object.
    /// @param  magic    The value of the magic bytes that had been written with #annotate.
    /// @param  name     The type of the allocator.
    ///                  In case of type \b MonoAllocator, the debug-name of the instance.
    template<typename TSize>
    static
    void checkMem( void* mem, const TSize size, unsigned char magic, const char* name )
    {
        #if ALIB_DEBUG_ALLOCATIONS
          ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
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
            ALIB_WARNINGS_RESTORE
        #else
            (void) mem; (void) size; (void) magic; (void) name;
        #endif
    }
    #include "alib/lang/callerinfo_methods.hpp"

}; // struct DbgAlloc

#if DOXYGEN
// =================================================================================================
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
// =================================================================================================
struct Allocator
{
    /// The type of the allocator that this allocator uses underneath for its allocations.
    /// In case of \alib{lang;HeapAllocator}, this type is defined as <c>void</c>.<br>
    /// Allocators provided by \alib allow to access the instance with publicly inherited methods
    /// \alib{lang::AllocatorMember;GetAllocator} and \alib{lang::AllocatorMember;AI}.
    using ChainedAllocator = TAllocator;

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
    /// If compiler symbol \ref ALIB_DEBUG_ALLOCATIONS is set, then also this allocator returns
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
    /// Note that this method is empty and optimized out, when compiler symbol
    /// \ref ALIB_DEBUG_ALLOCATIONS is not set. If set, an \alib assertion is raised if either the
    /// magic bytes around the memory are not found, or the given \p{allocSize} does not
    ///
    /// @see Chapter \ref alib_contmono_further_debug of the Programmer's Manual.
    ///
    /// @tparam TSize   The type of parameter \p{allocSize}. (Deduced by the compiler.)
    /// @param mem        The address of the allocated object.
    /// @param allocSize  The true allocation size returned by the method #allocate .
    template<typename TSize>
    void dbgAcknowledgeIncreasedAllocSize( void* mem, TSize allocSize )                       const;

    /// If the compiler symbol \ref ALIB_DEBUG_ALLOCATIONS is not set, this method is empty and will
    /// be optimized out. Otherwise, this will raise an \alib assertion if the piece of allocated
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


// =================================================================================================
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
///  \snippet "ut_monomem.cpp"     DOX_MONOMEM_ALLOCATOR_INTERFACE_TYPE
///  \snippet "ut_monomem.cpp"     DOX_MONOMEM_ALLOCATOR_INTERFACE
///
///  \note
///    In this sample, the destructor is of class \c MyType is emtpy, and furthermore method
///    \b free of class \b MonoAllocator is empty. Thus, the whole call to #Delete will be optimized
///    out. With other allocators this is different. It is recommended to still include
///    such calls to avoid memory leaks at the moment either the allocator is changed, an allocated
///    type gets equipped with a destructor, etc.
///
/// @see
///   - The \ref alib_mods_contmono "Programmer's Manual" of module \alib_monomem_nl.
///   - Allocator types \alib{monomem;TMonoAllocator} and \alib{monomem;TPoolAllocator}
///     found in module \alib_monomem_nl.
///   - Class \alib{lang;HeapAllocator}, which is useful for templated types which are agnostic
///     to underlying allocation strategies.
///   - To allocate and create a copy of a character string, method \alib{strings;TString::Allocate}
///     or constructor \alib{strings;TString;TString(TAllocator& allocator, const TString<TChar>&)}
///     may be used. Freeing is then to be performed by using \alib{strings;TString::Free}.
// =================================================================================================
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
    ALIB_FORCE_INLINE
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
    T*          NewArray(TSize length, TArgs&& ... args )
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        T* mem= AllocArray<T, TSize>( length );
        for( TSize i= 0 ; i < length ; ++i )
            new (mem + i) T(std::forward<TArgs>( args )...);
        ALIB_WARNINGS_RESTORE
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
    inline void     Delete(T* object)
    {
        object->~T();
        allocator.free(object, sizeof(*object));
    }

    /// Destructs all array elements and frees the array's memory.
    /// @tparam T         The pointer type of parameter \p{array}.
    /// @tparam TIntegral Integral type of parameter \p{size}. Deduced by the compiler.
    /// @param  array     Pointer to the first array element.
    /// @param  length    The size of the given \p{mem}.
    template<typename T, typename TIntegral>
    inline
    void DeleteArray(T* array, TIntegral length)
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        for( TIntegral i= 0 ; i < length ; ++i )
            (array + i)->~T();
        allocator.free(array, sizeof(T[1]) * size_t(length) );
        ALIB_WARNINGS_RESTORE
    }


    /// Frees memory of size <c>sizeof(T)</c> pointed to by \p{mem}.
    /// This method does \b not call a destructor. To dispose custom types with due destruction,
    /// use method #Delete<T>(T*).
    ///
    /// @tparam T         The pointer type of the given \p{mem} to free.
    /// @param mem        The memory to free.
    template<typename T>
    inline
    void Free(T* mem)
    {
        allocator.free(static_cast<void*>(const_cast<ATMP_RC(T)*>(mem)),  sizeof(T) );
    }

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
    inline
    void Free(T* mem, TIntegral size)
    {
        allocator.free(static_cast<void*>(const_cast<ATMP_RC(T)*>(mem)), size_t(size) );
    }

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
        allocator.free( static_cast<void*>(const_cast<ATMP_RC(T)*>(array)),
                        sizeof(T[1]) * size_t(length)                          );
    }

}; // struct AllocatorInterface

// =================================================================================================
/// Implementation of (prototype-) class \alib{lang;Allocator} which performs standard heap
/// allocation by using <c>std::malloc</c>, <c>std::realloc</c>, and <c>std::free</c>
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
/// - Types \alib{monomem;TMonoAllocator},  \alib{monomem;TLocalAllocator}, and
///   \alib{monomem;TPoolAllocator} introduced with module \alib_monomem.
// =================================================================================================
struct HeapAllocator
{
    /// A magic byte, used with compiler-symbol \ref ALIB_DEBUG_ALLOCATIONS to mark
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
    /// @param alignment This type ignores the alignment parameter, because <c>std::malloc</c>
    ///                  always uses <c>alignof(std::max_align_t)</c>.
    /// @return Pointer to the allocated memory block.
    inline void*  allocate( size_t size, size_t alignment )                                    const
    {
        ALIB_ASSERT_ERROR( alignment <= alignof(std::max_align_t), "ALIB",
          "The HeapAllocator is not designed to provide alignments greater "
          "than alignof(std::max_align_t).  ")
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
    inline void* reallocate(void* mem, size_t oldSize, size_t newSize, size_t  )
    {
        DbgAlloc::checkMem( mem, oldSize, MAGIC, "HeapAllocator" );
        void* newMem= std::realloc(mem, lang::DbgAlloc::extSize(newSize));
        DbgAlloc::annotate(newMem, newSize, MAGIC);
        return newMem;
    }

    /// Frees memory by invoking <c>std::free</c>.
    /// @param mem   The memory to dispose.
    /// @param size  The size of the given \p{mem}.
    inline void free(void* mem, size_t size)                                                   const
    {
        DbgAlloc::checkMem( mem, size, MAGIC, "HeapAllocator" );
        DbgAlloc::clearMem(mem, size, CLEAR);
        std::free(mem);
    }

    /// This is an empty implementation of the prototyped method.
    /// It is empty because this allocator never returns a higher allocation size than requested.
    /// \alib{lang;Allocator::dbgAcknowledgeIncreasedAllocSize}.
    /// @tparam TSize    The type of parameter \p{allocSize}. (Deduced by the compiler.)
    template<typename TSize>
    void dbgAcknowledgeIncreasedAllocSize( void*, TSize  )                                 const  {}

    /// Returns a temporary object (which is usually optimized out together with a call to this
    /// operator) providing high-level convenience methods for allocation.
    /// @see Class \alib{lang::AllocatorInterface}
    /// @return A temporary high-level interface into the allocator.
    AllocatorInterface<HeapAllocator> operator()()                                   const  noexcept
    { return AllocatorInterface<HeapAllocator>(*const_cast<HeapAllocator*>(this)); }

    /// This is a constexpr static method that determines if a type allows splitting memory
    /// and later passing the pieces back to free.<br>
    /// @return \c false.
    static constexpr bool allowsMemSplit()                                         { return false; }

    #if ALIB_DEBUG
    /// With this allocator, the debug-name is \c constexpr "HeapAllocator".
    static constexpr const char*    DbgName                                       = "HeapAllocator";
    #endif

    /// If the compiler symbol \ref ALIB_DEBUG_ALLOCATIONS is not set, this method is empty and will
    /// be optimized out. Otherwise, this will raise an \alib assertion if the piece of allocated
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
///   \snippet "alib/strings/fwds.hpp"     DOX_MONOMEM_ALLOCMEMBER
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
    TAllocator&   allocator; ///<  A reference to the allocator.

  public:
    /// Exposes the allocator type.
    using AllocatorType = TAllocator;

    /// Deleted default constructor. (The allocator has to be given with construction)
    AllocatorMember()                                                                      = delete;

    /// Constructor.
    /// @param pAllocator A reference to the allocator to store in member #allocator.
    AllocatorMember( TAllocator& pAllocator )                                               noexcept
    : allocator(pAllocator)                                                                       {}

    /// Returns the reference to the allocator.
    /// @return The allocator given with construction.
    TAllocator&         GetAllocator()                          const noexcept { return allocator; }

    /// This is a convenience operator that returns the \alib{lang;AllocatorInterface} for the
    /// stored allocator (by calling \alib{lang;Allocator::operator()()}).
    /// @return The allocator interface of the stored allocator.
    AllocatorInterface<TAllocator> AI()                       const noexcept { return allocator(); }
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
    AllocatorMember()                                                                     = default;

    /// Constructor.
    /// @param heapAllocator This parameter is ignored, as always #HEAP_ALLOCATOR_INSTANCE is used.
    AllocatorMember(const HeapAllocator& heapAllocator)          noexcept  { (void) heapAllocator; }

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
/// This static inline namespace function calls the destructor <b>~T()</b>of given \p{object}.
/// The use of this method is recommended instead of calling the destructor directly,
/// to increase readability of the code.
///
/// \attention
///   Pointers have to be dereferenced when passed as arguments.
///   Otherwise, this method does nothing, as the "destructor of a pointer" is called.<br>
///   Because, for example, container types would pass a pointer here, in case their
///   (custom) stored type is a pointer type, no static assertion can be made here.
///
/// @tparam T        The object type. Deduced by the compiler and not need to be given.
/// @param object    The object to destruct.
//==================================================================================================
template<typename T>
static ALIB_FORCE_INLINE
void     Destruct(T& object)                                                        { object.~T(); }

} // namespace alib[::lang]

/// Type alias in namespace \b alib.
using     HeapAllocator =   lang::HeapAllocator;

} // namespace [alib]

#endif // HPP_ALIB_LANG_ALLOCATION

