//==================================================================================================
/// \file
/// This header file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_DETAIL_RECYCLER
#define HPP_ALIB_MONOMEM_DETAIL_RECYCLER 1
#pragma once
#include "alib/lang/tmp.hpp"
ALIB_ASSERT_MODULE(CONTAINERS)
#include "alib/lang/sidilist.hpp"
#include "alib/lang/allocation.hpp"
#include "alib/containers/recycling.hpp"
#if ALIB_DEBUG
#   include "alib/lang/dbgtypedemangler.hpp"
#   if ALIB_STRINGS
#      include "alib/strings/localstring.hpp"
#   endif
#endif

#include "alib/lang/callerinfo_functions.hpp"

namespace alib::containers { namespace detail {

/// Stores an element for recycling.
/// @param recyclables  The recycling stack.
/// @param elem         The element to recycle.
template<typename TNode>
void    recycleImpl( lang::SidiListHook<TNode>& recyclables, TNode* elem )
{
    lang::Destruct(*elem);
    recyclables.pushFront( elem );
}


/// Stores a list of elements for recycling (from the \p{begin} element to the last one).
/// @param recyclables  The recycling stack.
/// @param begin        The first element of the list store, must not be \c nullptr.
template<typename TNode>
void    recycleListImpl( lang::SidiListHook<TNode>& recyclables, TNode* begin )
{
    TNode* actual  = begin;
    for (;;)
    {
        TNode* next= actual->next();
        lang::Destruct(*actual);
        if (next == nullptr)
            break;
        actual= next;
    }

    recyclables.pushFront( begin, actual );
}

/// Stores a list of elements for recycling. Both given nodes have to exist.
/// @param recyclables  The recycling stack.
/// @param begin        The first element of the list store.
/// @param end          The first element not to be recycled.
/// @return A pair of values. The first is a pointer to the last node recycled, and the second
///         provides the number of nodes recycled.
template<typename TNode>
std::pair<TNode*,integer>  recycleListImpl( lang::SidiListHook<TNode>& recyclables,
                                           TNode* begin, TNode* end )
{
    std::pair<TNode*,integer> result;
    result.second= 1;
    result.first = begin;
    for (;;)
    {
        TNode* next= result.first->next();
        lang::Destruct(*result.first);
        if (next == end)
            break;
        ++result.second;
        result.first= next;
    }

    recyclables.pushFront( begin, result.first );
    return result;
}

/// Reserves the given \p{qty} of recyclables. If the allocator used returns \c true with
/// method \alib{lang::Allocator;allowsMemSplit} (like like \b MonoAllocator does), then one
/// chunk is allocated and cut into pieces.<br>
/// Otherwise, common allocations are performed
/// @param allocator    The allocator to use.
/// @param recyclables  The recycling stack.
/// @param qty          The quantity of elements to reserve.
template<typename TAllocator, typename TNode>
void    reserveImpl( TAllocator& allocator, lang::SidiListHook<TNode>& recyclables, integer qty )
{
    if (qty <= 0)
        return;

    if constexpr ( TAllocator::allowsMemSplit() )
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        TNode* newElements= allocator().template AllocArray<TNode>(qty);
        for( auto i= qty - 2; i >= 0   ; --i )
            newElements[i].next( &newElements[i + 1] );

        recyclables.pushFront( &newElements[0], &newElements[qty-1] );
    }
    else
    {
        TNode* start= allocator().template Alloc<TNode>();
        TNode* end = start;
        for( auto i= 1; i < qty   ; ++i )
        {
            end->next( allocator().template Alloc<TNode>() );
            end= end->next();
        }
        recyclables.pushFront( start, end );
    }
    ALIB_WARNINGS_RESTORE
}

/// Frees an element (no recycling).
/// @param allocator  The allocator to use.
/// @param elem         The element to recycle.
template<typename TAllocator, typename TNode>
void    disposeImpl( TAllocator& allocator, TNode* elem )
{
    lang::Destruct(*elem);
    allocator().Free(elem);
}


/// Deletes the given list.
/// This shared recycler does not free the node, but recycles them.
/// @param allocator  The allocator to use.
/// @param begin      The first element of the list to free.
template<typename TAllocator, typename TNode>
void disposeListImpl( TAllocator& allocator, TNode* begin )
{
    TNode* actual  = begin;
    for (;;)
    {
        TNode* next= actual->next();
        lang::Destruct(*actual);
        allocator().Free(actual);
        if (next == nullptr)
            return;
        actual= next;
    }
}

/// Deletes a list of elements for recycling. Both given nodes have to exist.
/// @param allocator  The allocator to use.
/// @param begin      The first element of the list to free.
/// @param end        The first element not to be freed.
/// @return A pair of values. The first is a pointer to the last node deleted, and the second
///         provides the number of nodes deleted.
template<typename TAllocator, typename TNode>
std::pair<TNode*,integer>    disposeListImpl( TAllocator& allocator,  TNode* begin, TNode* end )
{
    std::pair<TNode*,integer> result;
    result.second= 1;
    result.first= begin;
    for (;;)
    {
        TNode* next= result.first->next();
        lang::Destruct(*result.first);
        allocator().Free(result.first);
        if (next == end)
            return result;
        ++result.second;
        result.first= next;
    }
}

/// Recycles a chunk of memory that is \e not of the node type.
/// Such recycling is useful for example, in combination with hash tables, which usually
/// allocate a "bucket array" that grows over time. In this case, the former smaller bucket
/// array may be passed to this method to recycle it as future node elements.
/// @tparam TChunk      The type of the external chunk.
/// @param  recyclables The recycling stack.
/// @param  chunk       The recyclable chunk's address.
/// @param  count       The number of objects. (For non-array types, \c 1 is to be given.)
template<typename TNode, typename TChunk>
void    recycleChunkImpl( lang::SidiListHook<TNode>& recyclables, TChunk* chunk, size_t count )
{
    void*   mem = chunk;
    size_t  size= sizeof(TChunk[1]) * count;

    // align beginning of buffer (if necessary)
    if constexpr( alignof(TNode) > alignof(TChunk[1])  )
    {
        mem =  reinterpret_cast<void*>((size_t(chunk) + alignof(TNode) - 1) & ~(alignof(TNode) -1));
        size-= size_t(reinterpret_cast<char*>(mem) - reinterpret_cast<char*>(chunk));
    }

    // create recyclables
    ALIB_DBG( size_t cntRecycledObjects= 0; )
    while(size > sizeof(TNode))
    {
        recyclables.pushFront( reinterpret_cast<TNode*>( mem ) );
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        mem  =  reinterpret_cast<char*>(mem) + sizeof(TNode);
        size -= sizeof (TNode);
        ALIB_DBG( ++cntRecycledObjects; )
        ALIB_WARNINGS_RESTORE
    }

    #if ALIB_DEBUG
        if( cntRecycledObjects <= 0 )
        {
            #if ALIB_STRINGS
                NString2K typeChunk; lang::DbgTypeDemangler( typeid(TChunk) ).GetShort(typeChunk);
                NString2K typeNode ; lang::DbgTypeDemangler( typeid(TNode)  ).GetShort(typeNode );
                ALIB_WARNING( "MONOMEM/RECYCLER", NString4K() <<
                  "De-allocated chunk size is smaller than node size.\n"
                  "  Chunk object:       Type: <" << typeChunk << ">\n"
                  "    Size, Count, Alignment: "  <<  sizeof(TChunk[1]) << " * " << count << " = "
                                                  << (sizeof(TChunk[1]) * count) << " bytes, alignment: "
                                                  << alignof(TChunk[1]) << "\n"
                  "           Recyclable Type: <" << typeNode << ">\n"
                  "           Size, Alignment: "  << sizeof(TNode) << " bytes, alignment: " << alignof(TNode) << "\n"
                  "Note: If this recycler is used with a <containers::HashTable>, this message may be eliminated\n"
                  "      by reserving a reasonable initial bucket size."  )
            #else
                ALIB_WARNING( cntRecycledObjects > 0, "MONOMEM/RECYCLER",
                  "De-allocated chunk size is smaller than node size." )
            #endif
        }
    #endif
}

#include "alib/lang/callerinfo_methods.hpp"

/// Implements internal recycling interface. Used by container types of module \alib_containers_nl that
/// use private recycling (when \b their template parameter \p{TRecycling} evaluates to
/// \alib{containers;Recycling;Private}).
///
/// @tparam TAllocator The allocator to store and use.
/// @tparam TNode      The type to recycle. Has to be derived of \alib{lang::SidiNodeBase}.
template<typename TAllocator, typename TNode>
struct RecyclerPrivate  : lang::AllocatorMember<TAllocator>
{
  protected:
        /// The base type.
        using base= lang::AllocatorMember<TAllocator>;

        /// The list of recyclables.
        lang::SidiListHook<TNode>  recyclables;

  public:
    /// Parameterless constructor. Used with type \b HeapAllocator.
    RecyclerPrivate()                                                            noexcept = default;

    /// Constructor taking an allocator.
    /// @param pAllocator The allocator to use.
    explicit
    RecyclerPrivate( TAllocator& pAllocator )                                               noexcept
    : base( pAllocator )                                                                          {}

    /// Copy constructor. Does not copy the recycle node, but clears this one.
    /// @param copy The object to copy.
    RecyclerPrivate( const RecyclerPrivate& copy )                                          noexcept
    : base( copy )                                                                                {}

    /// Move constructor. Grabs the recyclables from the moved one and sets moved to nulled.
    /// @param move The private recycler to move.
    RecyclerPrivate( RecyclerPrivate&& move )                                               noexcept
    : base( move )
    , recyclables(move)
    {
        move.recyclables.reset();
    }

    /// Destructor. Frees all recyclables with the allocator.
    ~RecyclerPrivate()                                                                      noexcept
    {
        TNode* actual  = recyclables.first();
        while (actual)
        {
            TNode* next= actual->next();
            base::AI().Free(actual);
            actual= next;
        }
    }

    /// Resets this recycler. Frees all recyclables with the allocator.
    void Reset()                                                                            noexcept
    {
        if(recyclables.first())
        {
            disposeListImpl(base::GetAllocator(), recyclables.first());
            recyclables.reset();
        }
    }

    /// @return Returns the allocator received with construction.
    TAllocator&  GetAllocator()                      const noexcept { return base::GetAllocator(); }

    /// @return The allocator interface of the allocator received with construction.
    lang::AllocatorInterface<TAllocator> AI()      const noexcept { return base::GetAllocator()(); }

    /// @return Returns \c true to indicate that this is not a non-recycling version.
    static constexpr bool IsRecycling()                                    noexcept { return true; }

    ///  Counts the number of recyclables.<br>
    ///  Attention: This method runs in linear time.
    ///  @return The number of available elements.
    [[nodiscard]]
    integer       Count()                             const noexcept { return recyclables.count(); }


    /// Reserves the given number of recyclables.
    /// @param qty  The number of new recyclables to allocate.
    void Reserve( integer qty )   noexcept { reserveImpl( base::GetAllocator(), recyclables, qty); }

    /// Returns a recycled object or allocates a new one.
    /// @return A recycled or allocated element.
    [[nodiscard]]
    TNode*        Get()
    { return  !recyclables.isEmpty() ? recyclables.popFront()
                                     : base::AI().template Alloc<TNode>(); }

    /// Stores an element for recycling.
    /// @param elem The element to store.
    void Recycle( TNode* elem )                       noexcept { recycleImpl( recyclables, elem ); }

    /// Stores a list of elements for recycling (from the \p{begin} element to the last one).
    /// @param begin The first element of the list store, must not be \c nullptr.
    void RecycleList( TNode* begin )             noexcept { recycleListImpl( recyclables, begin ); }

    /// Stores a list of elements for recycling. Both given nodes have to exist.
    /// @param begin The first element of the list to recycle.
    /// @param end   The first element not to be recycled.
    /// @return A pair of values. The first is a pointer to the last node recycled, and the second
    ///         provides the number of nodes recycled.
    std::pair<TNode*, integer> RecycleList(TNode* begin, TNode* end)                        noexcept
    { return recycleListImpl( recyclables, begin, end ); }

    /// Deletes the given list.
    /// This shared recycler does not free the node, but recycles them.
    /// @param begin The first element of the list to free.
    void DisposeList( TNode* begin )    noexcept { disposeListImpl( base::GetAllocator(), begin ); }

    /// Deletes a list of elements for recycling. Both given nodes have to exist.
    /// @param begin The first element of the list to free.
    /// @param end   The first element not to be freed.
    void DisposeList( TNode* begin, TNode* end )                                            noexcept
    { disposeListImpl( base::GetAllocator(), begin, end ); }


    /// Converts a given chunk of memory, previously allocated with the same allocator that
    /// this type uses, into recyclable node objects.<br>
    /// This is only done if \alib{lang;Allocator::allowsMemSplit} returns \c true.
    /// @tparam TChunk The type of array that is to be cut into node elements.
    /// @param chunk   The chunk array to convert into nodes
    /// @param length  The length of the chunk array (pass \c 1 if the chunk is not an array type).
    template<typename TChunk>
    void RecycleChunk(TChunk* chunk, size_t length )                                        noexcept
    {
        if constexpr ( TAllocator::allowsMemSplit() )
            recycleChunkImpl( recyclables, chunk, length );
        else
            #if !defined(__MINGW32__)
                base::AI().FreeArray(chunk, length );
            #else
                base::AI().template FreeArray(chunk, length );
            #endif
    }

    /// Frees the given memory. Note that the shared recycler calls #RecycleChunk with this method.
    /// @tparam TChunk The type of array that is to be freed.
    /// @param chunk   The chunk array to convert into nodes
    /// @param length  The length of the chunk array (pass \c 1 if the chunk is not an array type).
    template<typename TChunk>
    void DisposeChunk(TChunk* chunk, size_t length )                                        noexcept
    {
#if !defined(__MINGW32__)
        base::AI().FreeArray(chunk, length );
#else
        base::AI().template FreeArray(chunk, length );
#endif
    }
};

/// This is the type that is exposed to users of the containers, in case shared recycling is
/// chosen, for example with \alib{containers;List::SharedRecyclerType} or
/// \alib{containers;HashTable::SharedRecyclerType}.<br>
/// A user has to pass an instance of this type to the constructor of the container.
/// Constructors that accept this type do not accept an allocator instance, as this is embedded
/// already here.
/// @tparam TAllocator The allocator to store and use.
/// @tparam TNode      The node type to recycle.
template<typename TAllocator, typename TNode>
class SharedRecycler  : public    lang::AllocatorMember<TAllocator>
                      , protected lang::SidiListHook<TNode>
{
  protected:
    template<typename TAllocator1, typename TNode1>
    friend class RecyclerShared;

    /// Shortcut to a base class.
    using allocBase=   lang::AllocatorMember<TAllocator>;

    /// Shortcut to a base class.
    using hookBase=    lang::SidiListHook<TNode>;
		

  public:
    /// Constructor taking no allocator, used with \b %HeapAllocator.
    SharedRecycler ()                                                                       noexcept
    : lang::AllocatorMember<TAllocator>()                                                         {}

    /// Constructor taking an allocator.
    /// @param pAllocator The allocator to use.
    SharedRecycler ( TAllocator& pAllocator )                                               noexcept
    : allocBase( pAllocator )                                                                     {}

    /// Destructor. Deletes all recyclables with the allocator.
    ~SharedRecycler()                                                          noexcept { Reset(); }

    /// Reset. Deletes all recyclables with the allocator.
    void Reset()                                                                            noexcept
    {
        TNode* actual  = this->first();
        while (actual)
        {
            TNode* next= actual->next();
            allocBase::AI().Free(actual);
            actual= next;
        }
        this->reset();
    }

    /// Counts the number of recyclables.
    /// @return The number of recycled container elements available.
    integer Count()                   const noexcept { return this->count();  }

    /// Reserves space for at least the given number of recyclables.
    /// @param qty       The expected number or increase of elements to be stored in the containers
    ///                  that share this recycler.
    /// @param reference Denotes whether \p{expected} is meant as an absolute size or an increase .
    void Reserve( integer qty, lang::ValueReference reference )
    {
        auto requiredRecyclables= ( qty - (reference == lang::ValueReference::Absolute ? Count()
                                                                                       : 0     ) );

        if( requiredRecyclables > 0 )
            reserveImpl( allocBase::GetAllocator(), *this, requiredRecyclables );
    }
};

/// Implements internal recycling interface. Used by container types of module \alib_containers_nl that
/// use shared recycling (when \b their template parameter \p{TRecycling} evaluates to
///  \alib{containers;Recycling;Shared}).
///
/// @tparam TAllocator The allocator to store and use.
/// @tparam TNode      The type to recycle. Has to be derived of \alib{lang::SidiNodeBase}.
template<typename TAllocator, typename TNode>
class RecyclerShared
{
  protected:
        /// The reference to the list of recyclables.
        SharedRecycler<TAllocator,TNode>&  sr;

  public:

    /// Constructor taking an allocator and the hook.
    /// @param hook       The external, shared recycling list.
    RecyclerShared ( SharedRecycler<TAllocator,TNode>& hook )                               noexcept
    : sr(hook)                                                                                    {}


    /// Copy constructor. Copies the reference to the shared recycler.
    /// @param copy The private recycler to copy.
    RecyclerShared( const RecyclerShared& copy )                                 noexcept = default;

    /// Move constructor. Just copies the reference but leaves original intact. (We don't care)
    /// @param move The private recycler to move.
    RecyclerShared( RecyclerShared&& move )                                                 noexcept
    : sr(move.sr)                                                                                 {}

    /// Destructor.
    ~RecyclerShared()                                                                   noexcept  {}

    /// Does nothing. Shared recyclers can't be reset.
    void Reset()                                                                        noexcept  {}

    /// @return Returns the allocator received with construction.
    TAllocator&  GetAllocator()                         const noexcept { return sr.GetAllocator(); }

    /// @return The allocator interface of the allocator received with construction.
    lang::AllocatorInterface<TAllocator> AI()         const noexcept { return sr.GetAllocator()(); }


    /// @return Returns \c true to indicate that this is not a non-recycling version.
    static constexpr bool IsRecycling()                                    noexcept { return true; }

    ///  Counts the number of recyclables.<br>
    ///  Attention: This method runs in linear time.
    ///  @return The number of available elements.
    [[nodiscard]]
    integer       Count()                                      const noexcept { return sr.count(); }

    /// Reserves the given number of recyclables.
    /// @param qty  The number of new recyclables to allocate.
    void Reserve( integer qty )                        { reserveImpl( sr.GetAllocator(), sr, qty); }

    /// Returns a recycled object or allocates a new one.
    /// @return A recycled or allocated element.
    [[nodiscard]]
    TNode*        Get()
    { return  !sr.isEmpty() ? sr.popFront()
                            : sr.AI().template Alloc<TNode>(); }

    /// Stores an element for recycling.
    /// @param elem The element to store.
    void Recycle( TNode* elem )                                noexcept { recycleImpl( sr, elem ); }

    /// Stores a list of elements for recycling (from the \p{begin} element to the last one).
    /// @param begin The first element of the list store, must not be \c nullptr.
    void RecycleList( TNode* begin )                      noexcept { recycleListImpl( sr, begin ); }

    /// Stores a list of elements for recycling. Both given nodes have to exist.
    /// @param begin The first element of the list to recycle.
    /// @param end   The first element not to be recycled.
    /// @return A pair of values. The first is a pointer to the last node recycled, and the second
    ///         provides the number of nodes recycled.
    std::pair<TNode*, integer>  RecycleList(TNode* begin, TNode* end)                       noexcept
    { return recycleListImpl( sr, begin, end ); }

    /// Does not free, but recycles instead.
    /// @param begin The first element of the list to free.
    void DisposeList( TNode* begin )                      noexcept { recycleListImpl( sr, begin ); }

    /// Does not free, but recycles instead.
    /// @param begin The first element of the list to free.
    /// @param end   The first element not to be freed.
    void DisposeList( TNode* begin, TNode* end )     noexcept { recycleListImpl( sr, begin, end ); }

    /// Converts a given chunk of memory, previously allocated with the same allocator that
    /// this type uses, into recyclable node objects.<br>
    /// This is only done if \alib{lang;Allocator::allowsMemSplit} returns \c true.
    /// @tparam TChunk The type of array that is to be cut into node elements.
    /// @param chunk   The chunk array to convert into nodes
    /// @param length  The length of the chunk array (pass \c 1 if the chunk is not an array type).
    template<typename TChunk>
    void RecycleChunk(TChunk* chunk, size_t length )                                        noexcept
    {
        if constexpr ( TAllocator::allowsMemSplit() )
            recycleChunkImpl( sr, chunk, length );
        else
            sr.AI().FreeArray(chunk, length );
    }

    /// This shared recycler calls #RecycleChunk with this method.
    /// @tparam TChunk The type of array that is to be freed.
    /// @param chunk   The chunk array to convert into nodes
    /// @param length  The length of the chunk array (pass \c 1 if the chunk is not an array type).
    template<typename TChunk>
    void DisposeChunk(TChunk* chunk, size_t length )     noexcept { RecycleChunk( chunk, length ); }
};


/// Implements internal recycling interface. Used by container types of module \alib_containers_nl
/// that omit recycling (when \b their template parameter \p{TRecycling} evaluates to
/// \alib{containers;Recycling;None}).
///
/// @tparam TAllocator The allocator to store and use.
/// @tparam TNode      The type to recycle. Has to be derived of \alib{lang::SidiNodeBase}.
template<typename TAllocator, typename TNode>
struct RecyclerVoid  : lang::AllocatorMember<TAllocator>
{
  protected:
    /// The base type.
    using base= lang::AllocatorMember<TAllocator>;

  public:
    /// Parameterless constructor. Used with type \b HeapAllocator.
    RecyclerVoid()                                                                        = default;

    /// Defaulted default constructor.
    /// @param pAllocator The allocator to use.
    RecyclerVoid( TAllocator& pAllocator )                                                  noexcept
    : base( pAllocator )                                                                          {}

    /// Does nothing. Shared recyclers can't be reset.
    void Reset()                                                                         noexcept {}

    /// @return Returns \c false to indicate that this is a non-recycling version.
    static constexpr bool   IsRecycling()                                 noexcept { return false; }

    ///  This recycler just returns \c 0.
    ///  Attention: With other implementations, this method may run in linear time.
    ///  @return \c 0, which in this case is always the number of available elements.
    [[nodiscard]]
    constexpr integer       Count()                                    const noexcept  { return 0; }

    /// Does nothing. In debug-compilations a warning is raised.
    void    Reserve( integer )                                                              noexcept
    { ALIB_WARNING("MONOMEM","Requested reservation of recyclables with non-recycling container.") }

    /// Allocates a new one.
    /// @return An allocated element.
    [[nodiscard]]
    TNode*  Get()                         { return base::AI().template Alloc<TNode>(); }

    /// Frees an element.
    /// @param elem The element to store.
    void    Recycle( TNode* elem )           noexcept { disposeImpl( base::GetAllocator(), elem ); }

    /// Frees the list.
    /// @param begin The first element of the list store, must not be \c nullptr.
    void    RecycleList( TNode* begin ) noexcept { disposeListImpl( base::GetAllocator(), begin ); }

    /// Frees the list (as this is a non-recycling recycler).
    /// @param begin The first element of the list recycled.
    /// @param end   The first element not to be recycled.
    /// @return A pair of values. The first is a pointer to the last node deleted, and the second
    ///         provides the number of nodes deleted.
    std::pair<TNode*, integer>      RecycleList( TNode* begin, TNode* end )                 noexcept
    { return disposeListImpl( base::GetAllocator(), begin, end ); }


    /// Deletes a list of elements (from the \p{begin} element to the last one).
    /// @param begin The first element of the list recycle, must not be \c nullptr.
    void    DisposeList( TNode* begin ) noexcept { disposeListImpl( base::GetAllocator(), begin ); }

    /// Deletes a list of elements for recycling. Both given nodes have to exist.
    /// @param begin The first element of the list to free.
    /// @param end   The first element not to be freed.
    /// @return A pair of values. The first is a pointer to the last node deleted, and the second
    ///         provides the number of nodes deleted.
    std::pair<TNode*, integer>      DisposeList( TNode* begin, TNode* end )                 noexcept
    { return disposeListImpl( base::GetAllocator(), begin, end ); }

    /// With other recyclers, this method
    /// converts a given chunk of memory, previously allocated with the same allocator that
    /// this type uses, into recyclable node objects.<br>
    /// This recycler just frees the object.
    /// @tparam TChunk The type of array that is to be cut into node elements.
    /// @param chunk  The chunk array to convert into nodes
    /// @param length The length of the chunk array (pass \c 1 if the chunk is not an array type).
    template<typename TChunk>
    constexpr void          RecycleChunk( TChunk* chunk, size_t length )                    noexcept
    {
#if !defined(__MINGW32__)
        base::AI().FreeArray(chunk, length );
#else
        base::AI().template FreeArray(chunk, length );
#endif
    }

    /// Frees the given memory. Note that the shared recycler calls #RecycleChunk with this method.
    /// @tparam TChunk The type of array that is to be freed.
    /// @param chunk   The chunk array to convert into nodes
    /// @param length  The length of the chunk array (pass \c 1 if the chunk is not an array type).
    template<typename TChunk>
    void DisposeChunk(TChunk* chunk, size_t length )                                        noexcept
    {
#if !defined(__MINGW32__)
        base::AI().FreeArray(chunk, length );
#else
        base::AI().template FreeArray(chunk, length );
#endif
    }

}; // struct RecyclerVoid

/// Helper-struct that chooses the right recycler type depending on template parameter
/// \p{TRecycling}. Specializations exists for each element in enumeration
/// \alib{containers;Recycling}.
/// @tparam TRecycling The sort of recycling, as given with template parameter of container types
///                    with the same name.
template<Recycling TRecycling>
struct RecyclingSelector
{
    #if DOXYGEN
    /// The type chosen.
    using Type= void;

    /// The hook type. Only used with shared recycling, otherwise <c>void</c>.
    using HookType= void;
    #endif
};
#if !DOXYGEN
template<>
struct RecyclingSelector<Recycling::Private>
{
    template<typename TAllocator, typename TNode>    using Type= RecyclerPrivate<TAllocator, TNode>;
    template<typename TAllocator, typename TNode>    using HookType= void;
};

template<> struct RecyclingSelector<Recycling::Shared>
{
    template<typename TAllocator, typename TNode>   using Type    = RecyclerShared<TAllocator, TNode>;
    template<typename TAllocator, typename TNode>   using HookType= SharedRecycler<TAllocator, TNode>;
};

template<> struct RecyclingSelector<Recycling::None>
{
    template<typename TAllocator, typename TNode>  using Type= RecyclerVoid<TAllocator, TNode>;
    template<typename TAllocator, typename TNode>  using HookType= void;
};
#endif


}} // namespace [alib::containers]

#endif // HPP_ALIB_MONOMEM_DETAIL_RECYCLER

