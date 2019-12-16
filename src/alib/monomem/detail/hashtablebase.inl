/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_DETAIL_HASHTABLEBASE
#define HPP_ALIB_MONOMEM_DETAIL_HASHTABLEBASE 1

#if !defined(HPP_ALIB_MONOMEM_HASHTABLE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif
#if !defined (HPP_ALIB_MONOMEM_MONOALLOCATOR)
#   include "alib/monomem/monoallocator.hpp"
#endif

#if !defined(HPP_ALIB_FS_LISTS_FORWARDLIST)
#   include "alib/lib/fs_lists/forwardlist.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_DETAIL_RECYCLER)
#   include "alib/monomem/detail/recycler.hpp"
#endif

#if !defined(HPP_ALIB_TMP) && !defined(ALIB_DOX)
#   include "alib/lib/tmp.hpp"
#endif

#if ALIB_STRINGS && ALIB_ENUMS
#   if !defined(HPP_ALIB_FS_COMMONENUMS)
#      include "alib/lib/fs_commonenums/commonenums.hpp"
#   endif
#else
#   if !defined(HPP_ALIB_FS_COMMONENUMS_DEFS)
#      include "alib/lib/fs_commonenums/commonenumdefs.hpp"
#   endif
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif

namespace aworx { namespace lib { namespace monomem {

/**
 * Details of namespace #aworx::lib::monomem.
 */
namespace detail {


#if   ALIB_SIZEOF_INTEGER == 4
    static constexpr int primeTableSize = 29;
#elif ALIB_SIZEOF_INTEGER == 8
    /** The size of the static table of prime numbers. Depends on the platform. */
    static constexpr int primeTableSize = 61;
#else
    #error "Unknown size of integer"
#endif

/** Table of prime numbers. The effective bucket size is chosen to be the first value found in
 *  this table that is equal or higher than the requested size.  */
ALIB_API    extern const uinteger primeNumbers[primeTableSize];

/** A dummy bucket used for nulled hash tables to avoid otherwise necessary checks.  */
ALIB_API    extern void*    dummyBucket;

// #################################################################################################
// ### Struct HashTableElement
// #################################################################################################

/** Type used by \alib{monomem::detail,HashTableBase::Element} if hash codes are cached.  */
template<typename T, typename TStored>
struct HashTableElementCached   : public ForwardNode<HashTableElementCached<T,TStored>>
{
    /** Deleted default constructor. (Needed to avoid warning with msc). */
    ~HashTableElementCached()                                                              = delete;

    /** TMP constant that denotes that hash codes are cached. */
    static constexpr bool   CachedHashCodes = 1;

    /** The custom data stored in nodes of this table. */
    union
    {
        TStored     value;          ///< The value as seen internally.
        T           valueExternal;  ///< The value as seen externally.
    };

    size_t                  hashCode; ///< The cached hash code.

    /** Stores the given hash code when an element is recycled or extracted and changed.
     *  @param pHashCode The new hash code to set for this (recycled) element.          */
    void fixHashCode( size_t pHashCode )
    {
        *const_cast<size_t*>(&hashCode)= pHashCode;
    }

    /** Returns the cached hash code.
     * @return The hash code of this element.    */
    size_t getCached()
    {
        return hashCode;
    }

    /** Invokes the destructor of templated custom member \p{TStored}. */
    void      destruct()
    {
        value.~TStored();
    }

};

/** Type used by \alib{monomem::detail,HashTableBase::Element} if hash codes are \b not cached.  */
template<typename T, typename TStored>
struct HashTableElementUncached  : public ForwardNode<HashTableElementUncached<T,TStored>>
{
    /** Deleted default constructor. (Needed to avoid warning with msc). */
    ~HashTableElementUncached()                                                            = delete;

    /** TMP constant that denotes that hash codes are not cached. */
    static constexpr bool   CachedHashCodes = 0;

    /** The custom data stored in nodes of this table. */
    union
    {
        T           valueExternal;  ///< The value as seen externally.
        TStored     value;          ///< The value as seen internally.
    };


    /** Does nothing, parameter ignored.
     *  @param pHashCode Ignored             */
    void   fixHashCode( size_t pHashCode )   { (void) pHashCode; }

    /** Never called.
     *  @return Undefined. */
    size_t getCached()                       {  return 0; }

    /** Invokes the destructor of templated custom member \p{TStored}. */
    void   destruct()
    {
        value.~TStored();
    }

};

/**
 * Helper struct used to determine the node type of \alib{monomem,detail::HashTableBase}.
 *
 * Type definition #type equals \alib{monomem,detail::HashTableElementUncached} in case
 * \p{THashCashing} equals \alib{Caching::Enabled} or \alib{Caching::Auto} and \p{TKey} is
 * an arithmetic type.<br>
 * Otherwise, \alib{monomem,detail::HashTableElementCached} is chosen.
 */
template<typename T, typename TStored,typename TKey,Caching THashCaching>
struct HashTableElementType
{
    /** The element type that the hash table uses.
     *  Results from values given for \p{THashCaching} and \p{TKey}.              */
    using type=    ATMP_IF_T_F(         THashCaching == Caching::Enabled
                                || (    THashCaching == Caching::Auto
                                     && !std::is_arithmetic<TKey>::value ),
                               detail::HashTableElementCached  <T ALIB_COMMA TStored>,
                               detail::HashTableElementUncached<T ALIB_COMMA TStored>    );
};


/**
 * Partially specialized helper struct used to determine the recycler type for struct
 * \alib{monomem,detail::HashTableBase}.
 */
template<typename T,typename TStored,typename TKey,Caching THashCaching, typename TRecycling>
struct HashTableRecycler
{
};

#if !defined(ALIB_DOX)
template<typename T,typename TStored,typename TKey,Caching THashCaching>
struct HashTableRecycler<T,TStored,TKey,THashCaching,Recycling::Private>
{ using type= detail::RecyclerPrivate<typename HashTableElementType<T,TStored,TKey,THashCaching>::type >; };

template<typename T,typename TStored,typename TKey,Caching THashCaching>
struct HashTableRecycler<T,TStored,TKey,THashCaching,Recycling::Shared >
{ using type= detail::RecyclerShared <typename HashTableElementType<T,TStored,TKey,THashCaching>::type >; };

template<typename T,typename TStored,typename TKey,Caching THashCaching>
struct HashTableRecycler<T,TStored,TKey,THashCaching,Recycling::None   >
{ using type= detail::RecyclerVoid   <typename HashTableElementType<T,TStored,TKey,THashCaching>::type >; };
#endif


/** ************************************************************************************************
 * Base struct of \alib{monomem,HashTable} providing internals.
 * \note
 *   The separation of the internals of class \b HashTable to this type in namespace \b detail
 *   has no benefit on compilation speed or other positive "technical" effect, nor is it a matter
 *   of software design.<br>
 *   A user of derived class \alib{monomem,HashTable} finds all interface methods and types
 *   in one place, which is not cluttered by the documentation of the internals found here.
 *   Otherwise, the separation is exclusively supporting source code organization.
 *
 *
 * @tparam T            See
 *                      \ref alib_namespace_monomem_hashtable_referencedoc "reference documentation"
 *                      of class \b HashTable.
 * @tparam TStored      See
 *                      \ref alib_namespace_monomem_hashtable_referencedoc "reference documentation"
 *                      of class \b HashTable.
 * @tparam TKey         See
 *                      \ref alib_namespace_monomem_hashtable_referencedoc "reference documentation"
 *                      of class \b HashTable.
 * @tparam TIfMapped    See
 *                      \ref alib_namespace_monomem_hashtable_referencedoc "reference documentation"
 *                      of class \b HashTable.
 * @tparam THash        See
 *                      \ref alib_namespace_monomem_hashtable_referencedoc "reference documentation"
 *                      of class \b HashTable.
 * @tparam TEqual       See
 *                      \ref alib_namespace_monomem_hashtable_referencedoc "reference documentation"
 *                      of class \b HashTable.
 * @tparam TAccess      See
 *                      \ref alib_namespace_monomem_hashtable_referencedoc "reference documentation"
 *                      of class \b HashTable.
 * @tparam THashCaching See
 *                      \ref alib_namespace_monomem_hashtable_referencedoc "reference documentation"
 *                      of class \b HashTable.
 * @tparam TRecycling   See
 *                      \ref alib_namespace_monomem_hashtable_referencedoc "reference documentation"
 *                      of class \b HashTable.
 **************************************************************************************************/
template< typename T,
          typename TStored,
          typename TKey,
          typename TIfMapped,
          typename THash,
          typename TEqual,
          typename TAccess,
          Caching  THashCaching,
          typename TRecycling      >
struct HashTableBase : public HashTableRecycler<T,TStored,TKey,THashCaching,TRecycling>::type
{
// #################################################################################################
// ### Types and Constants
// #################################################################################################
    /**
     * This is an empty struct used as type #TMapped within this class, respectively type
     * \alib{monomem,HashTable::MappedType}, in the case that template parameter \p{TIfMapped}
     * is given as <c>void</c>.
     */
    struct NO_MAPPING
    {};

    /** Type of the recycler that this class is derived from.     */
    using TRecycler= typename HashTableRecycler<T,TStored,TKey,THashCaching,TRecycling>::type;

    /** Equals template parameter \p{TIfMapped} in case this is not <c>void</c>, and helper
     *  struct \alib{monomem::detail::HashTableBase,NO_MAPPING} if it is.    */
    using TMapped= ATMP_IF_T_F( ATMP_EQ(TIfMapped, void), NO_MAPPING, TIfMapped );

    /** The type stored in bucket #List.  */
    using  Element= typename HashTableElementType<T,TStored,TKey,THashCaching>::type;

    /** Type of a single linked node list.  */
    using  List=    ForwardList<Element>;

    /** Type of a node in the \b List.  */
    using  Node=    ForwardNode<Element>;


// #################################################################################################
// ### internal helpers
// #################################################################################################

    /** Return the <em>key-portion</em> of a given element.
     *  @param element The element to receive the value from.
     *  @return    The key value of the element.                 */
    static TKey&    keyPortion   ( Element*    element )
    {
        return TAccess().Key( element->value );
    }

    /** Return the <em>mapped-portion</em> of a given element.
     *  @param element The element to receive the value from.
     *  @return    The key value of the element.                 */
    static TMapped& mappedPortion( Element*    element )
    {
        return TAccess().Mapped( element->value );
    }

    /** Either returns the cached hash code or calculates it.
     *  @param elem The element to receive the hashcode for.
     *  @return The hash code of the given element.              */
    static size_t    getHashCode(Element* elem)
    {
        if ALIB_CONSTEXPR_IF ( Element::CachedHashCodes )
            return elem->getCached();
        else
            return THash() ( keyPortion( elem ) );
    }

    /** Returns either a recycled or newly allocated element.
     *  @param hashCode The hashCode of the new element. Used only in cached case.
     *  @return A pointer to the element created or recycled.    */
    Element* allocElement( const size_t hashCode )
    {
        Element* elem= TRecycler::get();
        if( elem == nullptr )
            elem=    allocator->Alloc<Element>();

        elem->fixHashCode( hashCode );
        return elem;
    }

// #################################################################################################
// std::iterator types.
// #################################################################################################

    /** ********************************************************************************************
     * Templated implementation of \c std::iterator.
     * Will be exposed by derived class's definitions \alib{monomem::HashTable,Iterator} and
     * \alib{monomem::HashTable,ConstIterator}.
     *
     * As the name of the class indicates, this iterator satisfies the C++ standard library concept
     * \https{ForwardIterator,en.cppreference.com/w/cpp/named_req/ForwardIterator}.
     *
     * @tparam TConstOrMutable A constant or mutable version of the parent's template type
     *                         \p{TMapped}.
     **********************************************************************************************/
    template<typename TConstOrMutable>
    class TIterator: public std::iterator< std::forward_iterator_tag,  // iterator_category
                                           TMapped,                    // value_type
                                           integer,                    // distance type
                                           TConstOrMutable*,           // pointer
                                           TConstOrMutable&          > // reference
    {
        #if !defined(ALIB_DOX)
            friend struct HashTableBase;
            friend class  HashTable<T,TStored,TKey,TIfMapped,THash,TEqual,TAccess,THashCaching,TRecycling>;
        #endif

        /** Const or mutable version of HashTableBase. */
        using THashtable = ATMP_IF_T_F( !ATMP_IS_CONST(TConstOrMutable),       HashTableBase,
                                                                         const HashTableBase  );

        protected:
            /** The pointer to the hash table. */
            THashtable*  table;

            /** The actual bucket index. */
            uinteger     bucketIdx;

            /** The pointer to the actual element. */
            Element*     element;


            /** Internal constructor. Searches the first element, starting with given bucket
             *  number.
             *  @param pTable       Pointer to the hash table.
             *  @param pBbucketIx   The bucket index.
             */
            TIterator( THashtable* pTable, uinteger pBbucketIx )
            : table(pTable)
            {
                while( pBbucketIx < pTable->bucketCount )
                {
                    if( pTable->buckets[pBbucketIx].isNotEmpty() )
                    {
                        bucketIdx= pBbucketIx;
                        element  = pTable->buckets[pBbucketIx].start();
                        return;
                    }
                    ++pBbucketIx;
                }
                bucketIdx= pBbucketIx;
                element  = nullptr;
           }

            /** Internal constructor creating a specific iterator.
             *  @param pTable       Pointer to the hash table.
             *  @param pBbucketIx   The bucket index.
             *  @param pElement     Pointer to the current element.
             */
            TIterator( THashtable* pTable, uinteger pBbucketIx, Element* pElement)
                : table    ( pTable     )
                , bucketIdx( pBbucketIx )
                , element  ( pElement   )
            {}

            /** Moves an iterator with a nulled element pointer to the next element.    */
            void    repair()
            {
                ++bucketIdx;
                while( bucketIdx < table->bucketCount )
                {
                    if( table->buckets[bucketIdx].isNotEmpty() )
                    {
                        element= table->buckets[bucketIdx].start();
                        return;
                    }
                    ++bucketIdx;
                }
            }


        public:
            /** Default constructor. Keeps everything uninitialized. */
            TIterator()                                                                   = default;

            /** Copy constructor (default).
             * @param other The iterator to assign from. */
            TIterator( const TIterator& other)                                            = default;


            /** Copy assignment (default).
             * @param other The iterator to assign from.
             * @return A reference to this object.               */
            TIterator& operator=( const TIterator& other )                                = default;


            #if defined(ALIB_DOX)
                /** Copy constructor accepting a mutable iterator.
                 *  Available only for the constant version of this iterator.
                 *  @tparam TMutable The type of this constructor's argument.
                 *  @param mutableIt Mutable iterator to copy from.             */
                ATMP_SELECT_IF_1TP( typename TMutable, ATMP_EQ( TMutable, TIterator<T> ) )
                TIterator( const TMutable& mutableIt );
            #else
                ATMP_SELECT_IF_1TP( typename TMutable, ATMP_EQ( TMutable, TIterator<T> ) )
                TIterator( const TMutable& mutableIt )
                : table    ( mutableIt.table     )
                , bucketIdx( mutableIt.bucketIdx )
                , element  ( mutableIt.element   )
                {}
            #endif

            //####################   To satisfy concept of  InputIterator   ####################

            /** Prefix increment operator.
             *  @return A reference to this object. */
            TIterator& operator++()
            {
                if(element->hasNext())
                {
                    Node::moveForward(element);
                    return *this;
                }

                while( ++bucketIdx < table->bucketCount )
                {
                    if( table->buckets[bucketIdx].isNotEmpty() )
                    {
                        element= table->buckets[bucketIdx].start();
                        return *this;
                    }
                }

                element= nullptr;
                return *this;
            }

            /** Postfix increment operator.
            *  @return An iterator value that is not increased, yet. */
            TIterator operator++( int)
            {
                auto result= TIterator( *this);
                ++(*this);
                return result;
            }

            /** Comparison operator.
             *  @param other  The iterator to compare ourselves to.
             *  @return \c true if this and the given iterator are pointing to the same element,
             *          \c false otherwise. */
            bool operator==( TIterator other)                                                  const
            {
                return element == other.element;
            }

            /** Comparison operator.
             *  @param other  The iterator to compare ourselves to.
             *  @return \c true if this and given iterator are not equal, \c false otherwise. */
            bool operator!=( TIterator other)                                                  const
            {
                return !(*this == other);
            }


            // ############   access to templated members   ############

            /** Retrieves the stored object that this iterator references.
             * @return A reference to the stored object.                    */
            TConstOrMutable& operator*()                                                       const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "Illegal iterator." )
                return element->valueExternal;
            }

            /** Retrieves a pointer to the stored object that this iterator references.
             * @return A pointer to the stored object.                                  */
            TConstOrMutable* operator->()                                                      const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "Illegal iterator." )
                return &element->valueExternal;
            }

            /** Retrieves the stored object that this iterator references.
             * @return A reference to the stored object.                    */
            TConstOrMutable& Value()                                                           const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "Illegal iterator." )
                return element->valueExternal;
            }

            /** Retrieves the key-portion of the stored object that this iterator references.
             *  @return A reference to the key-portion of the stored object. */
            const TKey& Key()                                                                  const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "Illegal iterator." )
                return keyPortion( element );
            }

            /** Retrieves the mapped-portion of the stored object that this iterator references.
             *  This method is an alias to <c>operator*</c>
             *
             * ## Availability ##
             * This method is only available with
             * \ref alib_namespace_monomem_hashtable_setandmap "hash map mode".
             *
             * @return A reference to the mapped-portion of the stored object. */
            template<typename TEnableIf= TMapped>
            ATMP_T_IF(TMapped&, !ATMP_EQ( TEnableIf, void ))
            Mapped()                                                                           const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "Illegal iterator." )
                return mappedPortion( element );
            }

    }; // class TIterator


    /** ********************************************************************************************
     * Templated implementation of \c std::iterator.
     * Will be exposed by derived class's definitions
     * \alib{monomem::HashTable,LocalIterator} and \alib{monomem::HashTable,ConstLocalIterator}.
     *
     * As the name of the class indicates, this iterator satisfies the C++ standard library concept
     * \https{ForwardIterator,en.cppreference.com/w/cpp/named_req/ForwardIterator}.
     *
     * @tparam TConstOrMutable A constant or mutable version of
     *                         \alib{monomem::detail,HashTableBase::TMapped}.
     **********************************************************************************************/
    template<typename TConstOrMutable>
    class TLocalIterator
        :  public std::iterator< std::forward_iterator_tag,  // iterator_category
                                 TConstOrMutable,            // value_type
                                 integer,                    // distance type
                                 TConstOrMutable*,           // pointer
                                 TConstOrMutable&          > // reference

    {
        #if !defined(ALIB_DOX)
            friend struct HashTableBase;
            friend class  HashTable<T,TStored,TKey,TIfMapped,THash,TEqual,TAccess,THashCaching,TRecycling>;
        #endif

        protected:
            /** The pointer to the actual element. */
            Element*    element;

            /** The index of the bucket that this iterator works on. */
            uinteger    bucketIdx;

        public:
            /** Default constructor. */
            TLocalIterator()
            {}

            /** Copy constructor (default).
             * @param other The iterator to assign from. */
            TLocalIterator( const TLocalIterator& other )                             = default;

            #if defined(ALIB_DOX)
                /** Copy constructor accepting a mutable iterator.
                 *  Available only for the constant version of this iterator.
                 *  @tparam TMutable The type of this constructor's argument.
                 *  @param mutableIt Mutable iterator to copy from.             */
                ATMP_SELECT_IF_1TP( typename TMutable, ATMP_EQ( TMutable, TLocalIterator<T> ) )
                TLocalIterator( const TMutable& mutableIt );
            #else
                ATMP_SELECT_IF_1TP( typename TMutable, ATMP_EQ( TMutable, TLocalIterator<T> ) )
                TLocalIterator( const TMutable& mutableIt )
                : element  ( mutableIt.element   )
                , bucketIdx( mutableIt.bucketIdx )
                {}
            #endif

            /** Constructor.
             *  @param pBucketIdx   Index of the bucket this iterator works on.
             *  @param pElement     Pointer to current element.
             */
            explicit TLocalIterator( uinteger pBucketIdx, Element* pElement )
                : element  (pElement  )
                , bucketIdx(pBucketIdx)
            {}

            /** Copy assignment (default).
             * @param other The iterator to assign from.
             * @return A reference to this object.               */
            TLocalIterator& operator=( const TLocalIterator& other)                   = default;

            // ###################   To satisfy concept of  InputIterator   ####################

            /** Prefix increment operator.
             *  @return A reference to this object. */
            TLocalIterator& operator++()
            {
                ALIB_ASSERT_ERROR( element != nullptr, "Illegal iterator." )
                Node::moveForward(element);
                return *this;
            }

            /** Postfix increment operator.
            *  @return An iterator value that is not increased, yet. */
            TLocalIterator operator++( int)
            {
                auto result= TLocalIterator( *this);
                ++(*this);
                return result;
            }

            /** Comparison operator.
             *  @param other  The iterator to compare ourselves to.
             *  @return \c true if this and the given iterator are pointing to the same element,
             *          \c false otherwise. */
            bool operator==( TLocalIterator other)                                             const
            {
                return element == other.element
                       && bucketIdx == other.bucketIdx;
            }

            /** Comparison operator.
             *  @param other  The iterator to compare ourselves to.
             *  @return \c true if this and given iterator are not equal, \c false otherwise. */
            bool operator!=( TLocalIterator other)                                             const
            {
                return !(*this == other);
            }

            // ############   access to templated members   ############

            /** Retrieves the stored object that this iterator references.
             * @return A reference to the stored object.                    */
            TConstOrMutable&    operator*()                                                    const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "Illegal iterator." )
                return element->valueExternal;
            }

            /** Retrieves a pointer to the stored object that this iterator references.
             * @return A pointer to the stored object.                                  */
            TConstOrMutable*    operator->()                                                   const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "Illegal iterator." )
                return &element->valueExternal;
            }

            /** Retrieves the stored object that this iterator references.
             * @return A reference to the stored object.                    */
            TConstOrMutable&        Value()                                                    const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "Illegal iterator." )
                return element->valueExternal;
            }

            /** Retrieves the key-portion of the stored object that this iterator references.
             *  @return A reference to the key-portion of the stored object. */
            const TKey& Key()                                                                  const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "Illegal iterator." )
                return keyPortion( element );
            }

            /** Retrieves the stored object that this iterator references.<br>
             *  This method is an alias to <c>operator*</c>
             *
             * ## Availability ##
             * This method is only available with
             * \ref alib_namespace_monomem_hashtable_setandmap "hash map mode".
             *
             * @return A reference to the mapped-portion of the stored object. */
            template<typename TEnableIf= TMapped>
            ATMP_T_IF(TMapped&, !ATMP_EQ( TEnableIf, void ))
            Mapped()                                                                           const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "Illegal iterator." )
                return mappedPortion( element );
            }
    }; // class TLocalIterator


// #################################################################################################
// Fields
// #################################################################################################
    /** The allocator assigned.  */
    MonoAllocator*     allocator;

    /** The number of buckets managed by this tree.  */
    uinteger            bucketCount;

    /** The list of buckets.  */
    List*               buckets;

    /** The load factor that is set when the table is rehashed automatically.  */
    float               baseLoadFactor;

    /** The maximum quotient of #size and #bucketCount that triggers a rehash.  */
    float               maxLoadFactor;

    /** The number of elements stored.  */
    integer             size;

    /** Calculated once with rehash. Product of #maxLoadFactor and #bucketCount.  */
    integer             sizeLimitToRehash;


    /** ********************************************************************************************
     * Constructor.
     * @param pAllocator       The monotonic allocator to use.
     * @param pBaseLoadFactor  The base load factor.
     * @param pMaxLoadFactor   The maximum load factor.
     **********************************************************************************************/
    HashTableBase( MonoAllocator*   pAllocator,
                   float            pBaseLoadFactor,
                   float            pMaxLoadFactor   )
    : allocator     ( pAllocator      )
    , baseLoadFactor( pBaseLoadFactor )
    , maxLoadFactor ( pMaxLoadFactor  )
    , size          ( 0 )
    {
        buckets          = reinterpret_cast<List*>(&detail::dummyBucket);
        bucketCount      = 1;
        size             = 0;
        sizeLimitToRehash= 0;
    }

    /** ********************************************************************************************
     * Constructor taking a shared recycler.
     * @param pAllocator       The monotonic allocator to use.
     * @param pRecycler        The shared recycler.
     * @param pBaseLoadFactor  The base load factor.
     * @param pMaxLoadFactor   The maximum load factor.
     **********************************************************************************************/
    HashTableBase( MonoAllocator*                pAllocator,
                   ForwardList<Element>& pRecycler,
                   float                         pBaseLoadFactor = 1.0,
                   float                         pMaxLoadFactor  = 2.0  )
    : TRecycler     ( pRecycler       )
    , allocator     ( pAllocator      )
    , baseLoadFactor( pBaseLoadFactor )
    , maxLoadFactor ( pMaxLoadFactor  )
    , size          ( 0 )
    {
        buckets          = reinterpret_cast<List*>(&detail::dummyBucket);
        bucketCount      = 1;
        size             = 0;
        sizeLimitToRehash= 0;
    }



// #################################################################################################
// ### mini helpers
// #################################################################################################

    /**
     * Compares two elements. If cached mode, the hash code is compared prior to the keys.
     * @param lhs          The first node to compare.
     * @param rhs          The second node to compare.
     * @return The result of the comparison.
     */
    bool        areEqual( Element* lhs, Element* rhs )                                         const
    {
        return       ( !Element::CachedHashCodes || ( getHashCode(lhs) == getHashCode(rhs) ) )
                 &&  TEqual()( keyPortion( lhs ), keyPortion( rhs ) );
    }

    /**
     * Compares a key and a node. If cached mode, the hash codes are compared prior to the
     * keys.
     * @param elem        The element to compare.
     * @param key         The key to compare.
     * @param keyHashCode The hash code of \p{key}.
     * @return The result of the comparison.
     */
    bool        areEqual( Element* elem,  const TKey& key, size_t keyHashCode )               const
    {
        return      ( !Element::CachedHashCodes || ( keyHashCode == getHashCode(elem) ) )
                &&  TEqual() ( keyPortion( elem ), key  );
    }

    /**
     * Searches the first element equal to \p{key} in bucket \p{bucketIdx}.
     *
     * @param bucketIdx    The bucket number to search in (hash code divided by #bucketCount).
     * @param key          The <em>key-portion</em> of the element to search.
     * @param keyHashCode The hash code of \p{key}.
     * @return A pointer to the element searched, respectively \c nullptr if not found.
     */
    Element*    findElement( uinteger bucketIdx,  const TKey& key,  size_t keyHashCode )      const
    {
        for( auto& it : buckets[bucketIdx] )
            if( areEqual( &it, key, keyHashCode ) )
                return &it;
        return nullptr;
    }

    /**
     * Searches the predecessor of the first element equal to \p{key} in bucket \p{bucketIdx}.
     *
     * @param bucketIdx    The bucket number to search in (hash code divided by #bucketCount).
     * @param key          The <em>key-portion</em> of the element to search.
     * @param keyHashCode The hash code of \p{key}.
     * @return A pointer to the element before the searched one, respectively \c nullptr if not
     *         found.
     */
    Node*    findElementBefore( uinteger bucketIdx, size_t keyHashCode, const TKey& key )      const
    {
        Node* result= buckets[bucketIdx].castToNode();
        while( result->hasNext() && !areEqual( result->next(), key, keyHashCode  ) )
            Node::moveForward(result);

        return result->hasNext() ? result : nullptr;
    }

    /**
     * Inserts the given element into the bucket.
     * If an element of the same key exists, then the element is put in front of that element.
     * Otherwise it is added to the start of the bucket.
     * @param element   The element to insert to its bucket.
     * @param hashCode  The hash code of parameter \p{element}.
     * @return The bucket number that the element was inserted to.
     */
    uinteger     insertInBucket( Element* element, const size_t hashCode )
    {
        auto bucketIdx= hashCode % bucketCount;
        Node* previous= findElementBefore( bucketIdx, hashCode, keyPortion(element) );
        if( previous == nullptr )
            previous= buckets[bucketIdx].castToNode();

        previous->addBehind( element );
        return bucketIdx;
    }

    /**
     * Increases field #size and checks for a rehash.
     *
     * @param increase  The amount to increase.
     * @param hashCode  A hashCode that caller might want to have converted into a new
     *                  actual bucket index.
     * @return The bucket index of \p{hashCode}.
     */
    size_t      increaseSize( integer increase, const size_t hashCode= 0 )
    {
        size+= increase;
        if( size >=sizeLimitToRehash  )
            rehash( (std::max)( static_cast<uinteger>( size / baseLoadFactor ), bucketCount + 1 ) );

        return hashCode % bucketCount;
    }

// #################################################################################################
// ### method  implementations
// #################################################################################################

    /** ********************************************************************************************
     * Destructs and removes all entries from this hash table.
     **********************************************************************************************/
    void            clear()
    {
        if( size == 0 )
            return;

        // destruct and recycle entries
        for( uinteger bucketIdx= 0 ; bucketIdx < bucketCount ; ++bucketIdx )
        {
            Element* first= buckets[bucketIdx].start();
            if( first != nullptr )
            {
                first->destruct();
                Element* last = first;
                while( last->hasNext() )
                {
                    Node::moveForward(last);
                    last->destruct();
                }
                TRecycler::recycle( first, last );
                buckets[bucketIdx].reset();
            }
        }

        size= 0;
    }


    /** ********************************************************************************************
     * Resets this object. Invokes #clear to destruct all keys and values and disposes all
     * allocated internal management data.
     **********************************************************************************************/
    void            reset()
    {
        clear();
        buckets          = reinterpret_cast<List*>(&detail::dummyBucket);
        bucketCount      = 1;
        size             = 0;
        sizeLimitToRehash= 0;
        TRecycler::disposeRecyclablesIfPrivate();
    }

    /** ********************************************************************************************
     * Changes the maximum load factor value and invokes #rehash providing the actual bucket
     * count as the minimum bucket count that is to be chosen.
     * @param pMaxLoadFactor The maximum load factor used to determine the need of re-hashing.
     **********************************************************************************************/
    void            setMaxLoadFactor( float pMaxLoadFactor )
    {
        maxLoadFactor= pMaxLoadFactor;
        if( bucketCount > 1 )
            rehash( bucketCount );
    }

    /** ********************************************************************************************
     * Changes the number of buckets to a least \p{newMinBucketCount}.
     * If the current size of the container divided by #maxLoadFactor leads to a higher number,
     * then this is chosen.
     * Finally, a next higher prime value is chosen (from static table #primeNumbers).
     *
     * @param newMinBucketCount The minimum new bucket count requested.
     **********************************************************************************************/
    void            rehash( uinteger newMinBucketCount )
    {
        auto oldBucketCount= bucketCount;

        // adjust requested bucket count to the maximum load factor
        newMinBucketCount= (std::max)( newMinBucketCount,
                                       static_cast<uinteger>( size / maxLoadFactor ) );

        // adjust requested bucket count to next higher prime value
        {
            int idx= 0;
            while( detail::primeNumbers[idx] < newMinBucketCount )
                ++idx;
            bucketCount= detail::primeNumbers[idx];
        }

        // same as before?
        if( bucketCount == oldBucketCount )
            return;

        sizeLimitToRehash= static_cast<integer>( bucketCount * maxLoadFactor );


        // collect elements
        List elements;
        for( uinteger bucketIdx= 0 ; bucketIdx < oldBucketCount ; ++bucketIdx )
        {
            Element* first= buckets[bucketIdx].start();
            if( first != nullptr )
                elements.pushFront( first, buckets[bucketIdx].findLast() );
        }


        // create a new data array
        List*    oldData  = buckets;

        buckets= allocator->EmplaceArray<List>( bucketCount );

        // re-insert objects
        Element* actual= elements.start();
        while( actual != nullptr )
        {
            Element* next= actual->next();
            insertInBucket( actual, getHashCode(actual) );
            actual= next;
        }

        // recycle old data (make future nodes elements out of it)
        if( oldData != reinterpret_cast<List*>( &detail::dummyBucket ) )
        {
            TRecycler::template recycleChunk<List>( oldData, oldBucketCount );
        }
    }

    /** ********************************************************************************************
     * Searches the first and last element stored according to given \p{key}.
     * Returns a pare of iterators that define a range containing all elements with key
     * \p{key} of the container.<br>
     * Parameter \p{resultStart} is set to the first element of the matching range and
     * parameter \p{resultEnd} is set to point past the last element of the range.
     *
     * If no element with key \p{key} is found, both iterators are set to \b end.
     *
     * @param  key  The <em>key-portion</em> of the elements to find.
     * @return A pair of iterators defining the range of elements with key \p{key}.
     **********************************************************************************************/
    std::pair<TIterator<T>,TIterator<T>>    findRange( const TKey& key )
    {
        const auto hashCode= THash()(key);
        auto bucketIdx= hashCode % bucketCount;
        Element* element= findElement( bucketIdx, key, hashCode );
        if( element == nullptr )
            return  std::make_pair( TIterator<T>( this, bucketCount, nullptr ),
                                    TIterator<T>( this, bucketCount, nullptr )   );

        auto result= std::make_pair( TIterator<T>( this, bucketIdx, element ),
                                     TIterator<T>( this, bucketIdx, element ) );
        for(;;)
        {
            ++result.second;
            if(     result.second.element == nullptr
                || !areEqual( result.second.element, key, hashCode ) )
                return result;
        }

    }

    /** ********************************************************************************************
     * Searches (a first) element with the given key.
     * If not found, an invalid iterator to the bucket is returned with a nulled element pointer.
     * Prior to this counter #size is increased and, if a load limit is reached, a re-hash is
     * performed.
     *
     * If otherwise an element was found, its valid iterator is returned.
     *
     * Note: Used by \alib{monomem,HashMap::EmplaceIfNotExistent}.
     *
     * @param key       The key to the (first) element to find.
     * @param hashCode  The hash code of parameter \p{key}.
     * @return A pair of an iterator referring to the found or inserted element and a boolean
     *         that is \c true if the insertion took place and \c false nothing was changed.
     **********************************************************************************************/
    std::pair<TIterator<T>, bool> insertIfNotExists( const TKey& key, size_t hashCode )
    {
        auto bucketIdx= hashCode % bucketCount;
        Element* element= findElement( bucketIdx, key, hashCode );
        if (element != nullptr )
            return std::make_pair(TIterator<T>( this, bucketIdx, element ), false);

        bucketIdx= increaseSize( 1, hashCode );

        Element* newElement= allocElement( hashCode );
        buckets[bucketIdx].pushFront( newElement );
        return std::make_pair(TIterator<T>( this, bucketIdx, newElement ) , true);
    }

    /** ********************************************************************************************
     * Inserts the topmost recyclable element if no element with the same key-portion of its value
     * exists.
     *
     * @param key       Pointer to the key to search elements for deletion.
     * @param hashCode  The hash code of parameter \p{key}.
     * @return A pair containing an iterator referring to the element added.
     *         The bool component is \c true if the insertion took place and \c false if a new
     *         element was created.
     **********************************************************************************************/
    std::pair<TIterator<T>, bool> insertOrGet( const TKey& key, size_t hashCode )
    {
        // find (first) element with same key in bucket
        auto  bucketIdx= hashCode % bucketCount;
        auto* elem= findElement( bucketIdx, key, hashCode );
        if( elem != nullptr )
             return std::make_pair( TIterator<T>( this, bucketIdx, elem ), false);

        // create new element
        bucketIdx= increaseSize( 1, hashCode );
        Element* newElem= allocElement( hashCode );
        buckets[bucketIdx].pushFront( newElem );
        return std::make_pair(TIterator<T>( this, bucketIdx, newElem ), true);
    }

}; // HashTableBase

}}}} // namespace [aworx::lib::monomem::detail]


#endif // HPP_ALIB_MONOMEM_DETAIL_HASHTABLEBASE
