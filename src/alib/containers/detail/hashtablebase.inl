//==================================================================================================
/// \file
/// This header-file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
// Not exported
#if   ALIB_SIZEOF_INTEGER == 4
    static constexpr int PRIME_TABLE_SIZE = 26;
#elif ALIB_SIZEOF_INTEGER == 8
    /// The size of the static table of prime numbers. Depends on the platform.
    static constexpr int PRIME_TABLE_SIZE = 58;
#else
    #error "Unknown size of integer"
#endif

// forward declaration of HashTable
ALIB_EXPORT namespace alib::containers {
    template< typename      TAllocator,
              typename      TValueDescriptor,
              typename      THash,
              typename      TEqual,
              lang::Caching THashCaching,
              Recycling     TRecycling       >
    class HashTable;
}

namespace alib::containers::detail {

/// Table of prime numbers. The effective bucket size is chosen to be the first value found in
/// this table that is equal or higher than the requested size.
ALIB_DLL    extern const uinteger PRIME_NUMBERS[PRIME_TABLE_SIZE];

/// A dummy bucket used for nulled hash tables to avoid otherwise necessary checks.
ALIB_DLL    extern void*          DUMMY_BUCKET;

/// HashTable element type if hash codes are cached.
/// @tparam TStored The custom data stored.
template<typename TStored>
struct HTElementCached   : public lang::SidiNodeBase<HTElementCached<TStored>>
{
    /// Denotes that hash codes are cached.
    static constexpr bool   CachedHashCodes = 1;

    /// The custom data stored in nodes of this table.
    TStored                 value;

    size_t                  hashCode; ///< The cached hash code.

    /// Stores the given hash code when an element is recycled or extracted and changed.
    /// @param pHashCode The new hash code to set for this (recycled) element.
    void fixHashCode( size_t pHashCode )         { *const_cast<size_t*>(&hashCode)= pHashCode; }

    /// Returns the cached hash code.
    /// @return The hash code of this element.
    size_t getCached()                                                      { return hashCode; }

};

/// HashTable element type if hash codes are not cached.
/// @tparam TStored The custom data stored.
template<typename TStored>
struct HTElementUncached  : public lang::SidiNodeBase<HTElementUncached<TStored>>
{
    /// Denotes that hash codes are not cached.
    static constexpr bool   CachedHashCodes = 0;

    /// The custom data stored in nodes of this table.
    TStored                 value;

    /// Does nothing, parameter ignored.
    /// @param pHashCode Ignored
    void   fixHashCode( size_t pHashCode )                                 { (void) pHashCode; }

    /// Never called.
    /// @return Undefined.
    size_t getCached()                                                            {  return 0; }
};

/// Node types used with \alib{containers;HashTable}.
/// @tparam TValueDescriptor The descriptor of contained types provided with a template parameter
///                          of the \b HashTable.
/// @tparam THashCaching     A template parameter of the \b HashTable that determines whether
///                          hash values are cached or not.
template<typename TValueDescriptor, lang::Caching THashCaching>
struct HTElementSelector
{
    /// @return \c true, if the selected #Type caches hash values, otherwise \c false.
    static constexpr bool IsCachingHashes()
    {
        return          THashCaching == lang::Caching::Enabled
                || (    THashCaching == lang::Caching::Auto
                     && !std::is_arithmetic<typename TValueDescriptor::KeyType>::value );
    }

    /// The type stored in a hash table's bucket list.
    using Type= std::conditional_t< IsCachingHashes(),
                                    HTElementCached  <typename TValueDescriptor::StoredType>,
                                    HTElementUncached<typename TValueDescriptor::StoredType>  >;
};

//==================================================================================================
/// Base struct of \alib{containers;HashTable} providing internals.
/// \note
///   The separation of the internals of class \b HashTable to this type in namespace \b detail
///   has no benefit on compilation speed or other positive "technical" effect, nor is it a matter
///   of software design.<br>
///   A user of derived class \alib{containers;HashTable} finds all interface methods and types
///   in one place, which is not cluttered by the documentation of the internals found here.
///   Otherwise, the separation is exclusively supporting source code organization.
///
/// @see For a description of the template parameters and a general introduction to
///      this module's hash table implementation, see the
///      \ref alib_ns_containers_hashtable_referencedoc "reference documentation"
///      of class \b HashTable.
//==================================================================================================
template< typename      TAllocator,
          typename      TValueDescriptor,
          typename      THash,
          typename      TEqual,
          lang::Caching THashCaching,
          Recycling     TRecycling      >
struct HashTableBase
: RecyclingSelector<TRecycling>:: template Type< TAllocator,
                                             typename HTElementSelector<TValueDescriptor, THashCaching>::Type >
{
    /// Our base type.
    using base= typename RecyclingSelector<TRecycling>:: template Type< TAllocator,
                                    typename HTElementSelector<TValueDescriptor, THashCaching>::Type >;

    /// Type definition publishing template parameter \p{T}.
    using T        = typename TValueDescriptor::StoredType;

    /// Type definition publishing template parameter \p{TKey}.
    using TKey     = typename TValueDescriptor::KeyType;

    /// Type definition publishing template parameter \p{TKey}.
    using TMapped  = typename TValueDescriptor::MappedType;


    /// Determines whether hash codes are stored with the elements.
    /// It is done in case the given template parameter \p{THashCashing} equals
    /// \alib{lang;Caching;Caching::Enabled} or if it equals \alib{lang;Caching;Caching::Auto}
    /// and the key type is an arithmetic type.
    /// @return \c true if hash codes are stored for reuse, \c false if not.
    static constexpr bool IsCachingHashes()
    { return HTElementSelector<TValueDescriptor, THashCaching>::IsHashCaching(); }

    /// The type stored in the bucket of class \b HashTable.
    using Element= typename HTElementSelector<TValueDescriptor, THashCaching>::Type;


  // ###############################################################################################
  // ### Types and Constants
  // ###############################################################################################

    /// Type of a single linked node list.
    using recyclerType=  typename RecyclingSelector<TRecycling>:: template Type< TAllocator,
                                             typename HTElementSelector<TValueDescriptor, THashCaching>::Type >;

    /// This type definition may be used to define an externally managed shared recycler,
    /// which can be passed to the alternative constructor of this class when template
    /// parameter \p{TRecycling} equals \alib{containers;Recycling;Shared}.
    /// \see
    ///   Chapter \ref alib_contmono_containers_recycling_shared of the Programmer's Manual
    ///   for this \alibmod.
    using SharedRecyclerType=  typename detail::RecyclingSelector<TRecycling>
                    :: template HookType<TAllocator,
                                         typename HTElementSelector<TValueDescriptor, THashCaching>::Type>;


    /// Type of a single linked node list.
    using  FwdList= lang::SidiListHook<Element>;

    /// Type of a node in the \b List.
    using  Node=    lang::SidiNodeBase<Element>;


  // ###############################################################################################
  // ### internal helpers
  // ###############################################################################################

    /// Either returns the cached hash code or calculates it.
    /// @param elem The element to receive the hashcode for.
    /// @return The hash code of the given element.
    static size_t    getHashCode(Element* elem)
    {
        if constexpr ( Element::CachedHashCodes )
            return elem->getCached();
        else
            return THash{}( TValueDescriptor().Key( elem->value ) );
    }

    /// Returns either a recycled or newly allocated element.
    /// @param hashCode The hashCode of the new element. Used only in cached case.
    /// @return A pointer to the element created or recycled.
    Element* allocElement( const size_t hashCode )
    {
        Element* elem= recyclerType::Get();
        elem->fixHashCode( hashCode );
        return elem;
    }

  // ###############################################################################################
  // std::iterator_traits types.
  // ###############################################################################################

    //==============================================================================================
    /// Templated implementation of \c std::iterator_traits.
    /// Will be exposed by derived class's definitions \alib{containers::HashTable;Iterator} and
    /// \alib{containers::HashTable;ConstIterator}.
    ///
    /// As the name of the class indicates, this iterator satisfies the C++ standard library concept
    /// \https{ForwardIterator,en.cppreference.com/w/cpp/named_req/ForwardIterator}.
    ///
    /// @tparam TConstOrMutable A constant or mutable version of the parent's template type
    ///                         \p{TMapped}.
    //==============================================================================================
    template<typename TConstOrMutable>
    class TIterator
    {
        #if !DOXYGEN
            friend struct HashTableBase;
            friend class  containers::HashTable<TAllocator, TValueDescriptor,THash,TEqual,THashCaching,TRecycling>;
        #endif

        /// Const or mutable version of HashTableBase.
        using THashtable = std::conditional_t< !std::is_const_v< TConstOrMutable>,
                                                                 HashTableBase,
                                                                 const HashTableBase >;
        using iterator_category = std::forward_iterator_tag;  ///< Implementation of <c>std::iterator_traits</c>.
        using value_type        = TMapped                  ;  ///< Implementation of <c>std::iterator_traits</c>.
        using difference_type   = integer                  ;  ///< Implementation of <c>std::iterator_traits</c>.
        using pointer           = TConstOrMutable*         ;  ///< Implementation of <c>std::iterator_traits</c>.
        using reference         = TConstOrMutable&         ;  ///< Implementation of <c>std::iterator_traits</c>.

        protected:
            /// The pointer to the hash table.
            THashtable*  table;

            /// The actual bucket index.
            uinteger     bucketIdx;

            /// The pointer to the actual element.
            Element*     element;


            /// Internal constructor. Searches the first element, starting with given bucket
            /// number.
            /// @param pTable       Pointer to the hash table.
            /// @param pBbucketIx   The bucket index.
            TIterator( THashtable* pTable, uinteger pBbucketIx )
            : table(pTable)
            {
                while( pBbucketIx < pTable->bucketCount )
                {
                    if( !pTable->buckets[pBbucketIx].isEmpty() )
                    {
                        bucketIdx= pBbucketIx;
                        element  = pTable->buckets[pBbucketIx].first();
                        return;
                    }
                    ++pBbucketIx;
                }
                bucketIdx= pBbucketIx;
                element  = nullptr;
           }

            /// Internal constructor creating a specific iterator.
            /// @param pTable       Pointer to the hash table.
            /// @param pBbucketIx   The bucket index.
            /// @param pElement     Pointer to the current element.
            TIterator( THashtable* pTable, uinteger pBbucketIx, Element* pElement)
                : table    ( pTable     )
                , bucketIdx( pBbucketIx )
                , element  ( pElement   )
            {}

            /// Moves an iterator with a nulled element pointer to the next element.
            void    repair()
            {
                while( ++bucketIdx < table->bucketCount )
                    if( !table->buckets[bucketIdx].isEmpty() )
                    {
                        element= table->buckets[bucketIdx].first();
                        return;
                    }
            }


        public:
            /// Default constructor. Keeps everything uninitialized.
            TIterator()                                                                   = default;

            /// Copy constructor (default).
            /// @param other The iterator to assign from.
            TIterator( const TIterator& other)                                            = default;


            /// Copy assignment (default).
            /// @param other The iterator to assign from.
            /// @return A reference to this object.
            TIterator& operator=( const TIterator& other )                                = default;


            /// Copy constructor accepting a mutable iterator.
            /// Available only for the constant version of this iterator.
            /// @tparam TMutable The type of this constructor's argument.
            /// @param mutableIt Mutable iterator to copy from.
            template<typename TMutable>
            requires std::same_as<TMutable, TIterator<T>>
            TIterator( const TMutable& mutableIt )
            : table    ( mutableIt.table     )
            , bucketIdx( mutableIt.bucketIdx )
            , element  ( mutableIt.element   )                                                    {}

            //####################   To satisfy concept of  InputIterator   ####################

            /// Prefix increment operator.
            /// @return A reference to this object.
            TIterator& operator++()
            {
                if(element->hasNext())
                {
                    element= element->next();
                    return *this;
                }

                while( ++bucketIdx < table->bucketCount )
                {
                    if( !table->buckets[bucketIdx].isEmpty() )
                    {
                        element= table->buckets[bucketIdx].first();
                        return *this;
                    }
                }

                element= nullptr;
                return *this;
            }

            /// Postfix increment operator.
           /// @return An iterator value that is not increased, yet.
            TIterator operator++( int)
            {
                auto result= TIterator( *this);
                ++(*this);
                return result;
            }

            /// Comparison operator.
            /// @param other  The iterator to compare ourselves to.
            /// @return \c true if this and the given iterator are pointing to the same element,
            ///         \c false otherwise.
            bool operator==( TIterator other)                                                  const
            {
                return element == other.element;
            }

            /// Comparison operator.
            /// @param other  The iterator to compare ourselves to.
            /// @return \c true if this and given iterator are not equal, \c false otherwise.
            bool operator!=( TIterator other)                                                  const
            {
                return !(*this == other);
            }


            // ############   access to templated members   ############

            /// Retrieves the stored object that this iterator references.
            /// @return A reference to the stored object.
            TConstOrMutable& operator*()                                                       const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
                return element->value;
            }

            /// Retrieves a pointer to the stored object that this iterator references.
            /// @return A pointer to the stored object.
            TConstOrMutable* operator->()                                                      const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
                return &element->value;
            }

            /// Retrieves the stored object that this iterator references.
            /// @return A reference to the stored object.
            TConstOrMutable& Value()                                                           const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
                return element->value;
            }

            /// Retrieves the key-portion of the stored object that this iterator references.
            /// @return A reference to the key-portion of the stored object.
            const TKey& Key()                                                                  const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
                return TValueDescriptor().Key( element->value );
            }

            /// Retrieves the mapped-portion of the stored object that this iterator references.
            /// This method is an alias to <c>operator*</c>
            /// @return A reference to the mapped-portion of the stored object.
            TMapped&    Mapped()                                                               const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
                return TValueDescriptor().Mapped( element->value );
            }

    }; // class TIterator


    //==============================================================================================
    /// Templated implementation of \c std::iterator_traits.
    /// Will be exposed by derived class's definitions
    /// \alib{containers::HashTable;LocalIterator} and
    /// \alib{containers::HashTable;ConstLocalIterator}.
    ///
    /// As the name of the class indicates, this iterator satisfies the C++ standard library concept
    /// \https{ForwardIterator,en.cppreference.com/w/cpp/named_req/ForwardIterator}.
    ///
    /// @tparam TConstOrMutable A constant or mutable version of template parameter \p{T}.
    //==============================================================================================
    template<typename TConstOrMutable>
    class TLocalIterator
    {
        #if !DOXYGEN
            friend struct HashTableBase;
            friend class  containers::HashTable<TAllocator,TValueDescriptor,THash,TEqual,THashCaching,TRecycling>;
        #endif

        using iterator_category = std::forward_iterator_tag;  ///< Implementation of <c>std::iterator_traits</c>.
        using value_type        = TConstOrMutable          ;  ///< Implementation of <c>std::iterator_traits</c>.
        using difference_type   = integer                  ;  ///< Implementation of <c>std::iterator_traits</c>.
        using pointer           = TConstOrMutable*         ;  ///< Implementation of <c>std::iterator_traits</c>.
        using reference         = TConstOrMutable&         ;  ///< Implementation of <c>std::iterator_traits</c>.

        protected:
            /// The pointer to the actual element.
            Element*    element;

            /// The index of the bucket that this iterator works on.
            uinteger    bucketIdx;

        public:
            /// Default constructor.
            TLocalIterator()
            {}

            /// Copy constructor (default).
            /// @param other The iterator to assign from.
            TLocalIterator( const TLocalIterator& other )                             = default;

            /// Copy constructor accepting a mutable iterator.
            /// Available only for the constant version of this iterator.
            /// @tparam TMutable The type of this constructor's argument.
            /// @param mutableIt Mutable iterator to copy from.
            template<typename TMutable>
            requires std::same_as<TMutable, TLocalIterator<T>>
            TLocalIterator( const TMutable& mutableIt )
            : element  ( mutableIt.element   )
            , bucketIdx( mutableIt.bucketIdx )                                                    {}

            /// Constructor.
            /// @param pBucketIdx   Index of the bucket this iterator works on.
            /// @param pElement     Pointer to current element.
            explicit TLocalIterator( uinteger pBucketIdx, Element* pElement )
                : element  (pElement  )
                , bucketIdx(pBucketIdx)
            {}

            /// Copy assignment (default).
            /// @param other The iterator to assign from.
            /// @return A reference to this object.
            TLocalIterator& operator=( const TLocalIterator& other)                   = default;

            // ###################   To satisfy concept of  InputIterator   ####################

            /// Prefix increment operator.
            /// @return A reference to this object.
            TLocalIterator& operator++()
            {
                ALIB_ASSERT_ERROR( element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
                element= element->next();
                return *this;
            }

            /// Postfix increment operator.
           /// @return An iterator value that is not increased, yet.
            TLocalIterator operator++( int)
            {
                auto result= TLocalIterator( *this);
                ++(*this);
                return result;
            }

            /// Comparison operator.
            /// @param other  The iterator to compare ourselves to.
            /// @return \c true if this and the given iterator are pointing to the same element,
            ///         \c false otherwise.
            bool operator==( TLocalIterator other)                                             const
            {
                return element == other.element
                       && bucketIdx == other.bucketIdx;
            }

            /// Comparison operator.
            /// @param other  The iterator to compare ourselves to.
            /// @return \c true if this and given iterator are not equal, \c false otherwise.
            bool operator!=( TLocalIterator other)                                             const
            {
                return !(*this == other);
            }

            // ############   access to templated members   ############

            /// Retrieves the stored object that this iterator references.
            /// @return A reference to the stored object.
            TConstOrMutable&    operator*()                                                    const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
                return element->value;
            }

            /// Retrieves a pointer to the stored object that this iterator references.
            /// @return A pointer to the stored object.
            TConstOrMutable*    operator->()                                                   const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
                return &element->value;
            }

            /// Retrieves the stored object that this iterator references.
            /// @return A reference to the stored object.
            TConstOrMutable&        Value()                                                    const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
                return element->value;
            }

            /// Retrieves the key-portion of the stored object that this iterator references.
            /// @return A reference to the key-portion of the stored object.
            const TKey& Key()                                                                  const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
                return TValueDescriptor().Key( element->value );
            }

            /// Retrieves the stored object that this iterator references.<br>
            /// This method is an alias to <c>operator*</c>
            /// @return A reference to the mapped-portion of the stored object.
            TMapped& Mapped()                                                                  const
            {
                ALIB_ASSERT_ERROR( element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
                return TValueDescriptor().Mapped( element->value );
            }
    }; // class TLocalIterator


  // ###############################################################################################
  // Fields
  // ###############################################################################################
    /// The number of buckets managed by this tree.
    uinteger            bucketCount;

    /// The list of buckets.
    FwdList*            buckets;

    /// The load factor that is set when the table is rehashed automatically.
    float               baseLoadFactor;

    /// The maximum quotient of #size and #bucketCount that triggers a rehash.
    float               maxLoadFactor;

    /// The number of elements stored.
    integer             size;

    /// Calculated once with rehash. Product of #maxLoadFactor and #bucketCount.
    integer             sizeLimitToRehash;


  // ###############################################################################################
  // ### mini helpers
  // ###############################################################################################

    /// Compares two elements. If cached mode, the hash code is compared before the keys.
    /// @param lhs          The first node to compare.
    /// @param rhs          The second node to compare.
    /// @return The result of the comparison.
    bool        areEqual( Element* lhs, Element* rhs )                                 const
    {
        return       ( !Element::CachedHashCodes || ( getHashCode(lhs) == getHashCode(rhs) ) )
                 &&  TEqual{}( TValueDescriptor().Key( lhs->value ),
                               TValueDescriptor().Key( rhs->value ) );
    }

    /// Compares a key and a node. If cached mode, the hash codes are compared before the
    /// keys.
    /// @param elem        The element to compare.
    /// @param key         The key to compare.
    /// @param keyHashCode The hash code of \p{key}.
    /// @return The result of the comparison.
    bool        areEqual( Element* elem,  const TKey& key, size_t keyHashCode )                const
    {
        return      ( !Element::CachedHashCodes || ( keyHashCode == getHashCode(elem) ) )
                &&  TEqual{}( TValueDescriptor().Key( elem->value ), key  );
    }

    /// Searches the first element equal to \p{key} in bucket \p{bucketIdx}.
    ///
    /// @param bucketIdx    The bucket number to search in (hash code divided by #bucketCount).
    /// @param key          The <em>key-portion</em> of the element to search.
    /// @param keyHashCode The hash code of \p{key}.
    /// @return A pointer to the element searched, respectively \c nullptr if not found.
    Element*    findElement( uinteger bucketIdx,  const TKey& key,  size_t keyHashCode )       const
    {
        Node* result= buckets[bucketIdx].first();
        while( result)
        {
            if( areEqual( static_cast<Element*>(result), key, keyHashCode  ) )
                return static_cast<Element*>(result);

            result= result->next();
        }
        return nullptr;
    }

    /// Searches the predecessor of the first element equal to \p{key} in bucket \p{bucketIdx}.
    ///
    /// @param bucketIdx    The bucket number to search in (hash code divided by #bucketCount).
    /// @param key          The <em>key-portion</em> of the element to search.
    /// @param keyHashCode The hash code of \p{key}.
    /// @return A pointer to the element before the searched one, respectively \c nullptr if not
    ///         found.
    Node*    findElementBefore( uinteger bucketIdx, size_t keyHashCode, const TKey& key )      const
    {
        Node* result= &buckets[bucketIdx];

        while( result->hasNext() && !areEqual( result->next(), key, keyHashCode  ) )
            result= result->next();

        return result->hasNext() ? result : nullptr;
    }

    /// Inserts the given element into the bucket.
    /// If an element of the same key exists, then the element is put in front of that element.
    /// Otherwise it is added to the start of the bucket.
    /// @param element   The element to insert to its bucket.
    /// @param hashCode  The hash code of parameter \p{element}.
    /// @return The bucket number that the element was inserted to.
    uinteger     insertInBucket( Element* element, const size_t hashCode )
    {
        auto bucketIdx= hashCode % bucketCount;
        Node* previous= findElementBefore( bucketIdx, hashCode, TValueDescriptor().Key( element->value ) );
        if( previous == nullptr )
            previous= &buckets[bucketIdx];

        previous->addBehind( element );
        return bucketIdx;
    }

    /// Increases field #size and checks for a rehash.
    ///
    /// @param increase  The amount to increase.
    /// @param hashCode  A hashCode that caller might want to have converted into a new
    ///                  actual bucket index.
    /// @return The bucket index of \p{hashCode}.
    size_t      increaseSize( integer increase, const size_t hashCode= 0 )
    {
        size+= increase;
        if( size >=sizeLimitToRehash  )
            rehash( (std::max)( uinteger( float(size) / baseLoadFactor ), bucketCount + 1 ) );

        return hashCode % bucketCount;
    }

    //==============================================================================================
    /// Constructor.
    /// @param pAllocator       The allocator to use.
    /// @param pBaseLoadFactor  The base load factor.
    /// @param pMaxLoadFactor   The maximum load factor.
    //==============================================================================================
    HashTableBase( TAllocator&       pAllocator,
                   float             pBaseLoadFactor,
                   float             pMaxLoadFactor   )
    : recyclerType  ( pAllocator      )
    , baseLoadFactor( pBaseLoadFactor )
    , maxLoadFactor ( pMaxLoadFactor  )
    , size          ( 0 )
    {
        buckets          = reinterpret_cast<FwdList*>(&detail::DUMMY_BUCKET);
        bucketCount      = 1;
        size             = 0;
        sizeLimitToRehash= 0;
    }

    //==============================================================================================
    /// Constructor using \alib{lang;HeapAllocator}.
    /// @param pBaseLoadFactor  The base load factor.
    /// @param pMaxLoadFactor   The maximum load factor.
    //==============================================================================================
    HashTableBase( float             pBaseLoadFactor,
                   float             pMaxLoadFactor   )
    : baseLoadFactor( pBaseLoadFactor )
    , maxLoadFactor ( pMaxLoadFactor  )
    , size          ( 0 )
    {
        buckets          = reinterpret_cast<FwdList*>(&detail::DUMMY_BUCKET);
        bucketCount      = 1;
        size             = 0;
        sizeLimitToRehash= 0;
    }

    //==============================================================================================
    /// Constructor taking a shared recycler.
    /// @param pSharedRecycler  The shared recycler hook.
    /// @param pBaseLoadFactor  The base load factor.
    /// @param pMaxLoadFactor   The maximum load factor.
    /// @tparam TSharedRecycler  Used to select this constructor. Deduced by the compiler.
    //==============================================================================================
    template<typename TSharedRecycler= SharedRecyclerType>
    requires (!std::same_as<TSharedRecycler , void>)
    HashTableBase( TSharedRecycler&     pSharedRecycler,
                   float                pBaseLoadFactor = 1.0,
                   float                pMaxLoadFactor  = 2.0  )
    : recyclerType  ( pSharedRecycler )
    , baseLoadFactor( pBaseLoadFactor )
    , maxLoadFactor ( pMaxLoadFactor  )
    , size          ( 0 )
    {
        buckets          = reinterpret_cast<FwdList*>(&detail::DUMMY_BUCKET);
        bucketCount      = 1;
        size             = 0;
        sizeLimitToRehash= 0;
    }

    //==============================================================================================
    /// Destructor. Destructs all elements in this object.
    /// Deletes recyclables, buckets and bucket array.
    //==============================================================================================
    ~HashTableBase()
    {
        if ( buckets == reinterpret_cast<FwdList*>( &detail::DUMMY_BUCKET ) )
            return;

        // destruct entry data and delete entry objects
        for( uinteger bucketIdx= 0 ; bucketIdx < bucketCount ; ++bucketIdx )
        {
            Element* first= buckets[bucketIdx].first();
            if ( first != nullptr )
                recyclerType::DisposeList(first);
        }

        // free bucket array
        recyclerType::template DisposeChunk<FwdList>(buckets, bucketCount );
    }

  // ###############################################################################################
  // ### method implementations
  // ###############################################################################################

    //==============================================================================================
    /// Destructs and removes all entries from this hash table.
    //==============================================================================================
    void            clear()
    {
        if( size == 0 )
            return;

        // destruct and recycle entries
        for( uinteger bucketIdx= 0 ; bucketIdx < bucketCount ; ++bucketIdx )
            if ( buckets[bucketIdx].first() )
            {
                recyclerType::RecycleList( buckets[bucketIdx].first() );
                buckets[bucketIdx].reset();
            }

        size= 0;
    }


    //==============================================================================================
    /// Changes the maximum load factor value and invokes #rehash providing the actual bucket
    /// count as the minimum bucket count that is to be chosen.
    /// @param pMaxLoadFactor The maximum load factor used to determine the need of re-hashing.
    //==============================================================================================
    void            setMaxLoadFactor( float pMaxLoadFactor )
    {
        maxLoadFactor= pMaxLoadFactor;
        if( bucketCount > 1 )
            rehash( bucketCount );
    }

    //==============================================================================================
    /// Changes the number of buckets to be at least the higher value of
    /// a) the given \p{newMinBucketCount}, and<br>
    /// b) the quotient of the current size and the maximum load factor.
    ///
    /// The result of the above, is increased to the next higher prime number.
    /// Rehash is only performed if bucket size increases. It never is decreased.
    ///
    /// @param newMinBucketCount The minimum new bucket count requested.
    //==============================================================================================
    void            rehash( uinteger newMinBucketCount )
    {
        // smaller than before?
        if( newMinBucketCount <= bucketCount )
            return;

        const auto oldBucketCount= bucketCount;

        // adjust requested bucket count to the maximum load factor
        newMinBucketCount= (std::max)( newMinBucketCount, uinteger(float(size) / maxLoadFactor) );

        // adjust requested bucket count to next higher prime value
        {
            int idx= 0;
            while( detail::PRIME_NUMBERS[idx] < newMinBucketCount )
                ++idx;
            bucketCount= detail::PRIME_NUMBERS[idx];
        }

        ALIB_ASSERT_ERROR( bucketCount > oldBucketCount, "MONOMEM/HASHTABLE",
                "Internal error: Rehashing to equal or smaller bucket count." )

        // store new rehash trigger
        sizeLimitToRehash= integer( float(bucketCount) * maxLoadFactor );

        // collect elements
        FwdList elements;
        for( uinteger bucketIdx= 0 ; bucketIdx < oldBucketCount ; ++bucketIdx )
        {
            Element* first= buckets[bucketIdx].first();
            if( first != nullptr )
                elements.pushFront( first, buckets[bucketIdx].findLast() );
        }


        // create a new data array
        FwdList*    oldData  = buckets;

        buckets= base::AI().template NewArray<FwdList>(bucketCount);

        // re-insert objects
        Element* actual= elements.first();
        while( actual != nullptr )
        {
            Element* next= actual->next();
            insertInBucket( actual, getHashCode(actual) );
            actual= next;
        }

        // recycle old array and data (make future nodes elements out of it)
        // But this must only be done with MonoAllocator and if ALIB_DEBUG_MEMORY is not set.
        // (This is ensured by 'TAllocator::allowsMemSplit()')
        if ( oldData != reinterpret_cast<FwdList*>( &detail::DUMMY_BUCKET ) )
            recyclerType::template RecycleChunk<FwdList>( oldData, oldBucketCount );
    }

    //==============================================================================================
    /// Searches the first and last element stored according to given \p{key}.
    /// Returns a pare of iterators that define a range containing all elements with key
    /// \p{key} of the container.<br>
    /// Parameter \p{resultStart} is set to the first element of the matching range and
    /// parameter \p{resultEnd} is set to point past the last element of the range.
    ///
    /// If no element with key \p{key} is found, both iterators are set to \b end.
    ///
    /// @param  key  The <em>key-portion</em> of the elements to find.
    /// @return A pair of iterators defining the range of elements with key \p{key}.
    //==============================================================================================
    std::pair<TIterator<T>,TIterator<T>>    findRange( const TKey& key )
    {
        const auto hashCode= THash{}(key);
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

    //==============================================================================================
    /// Searches (a first) element with the given key.
    /// If not found, an invalid iterator to the bucket is returned with a nulled element pointer.
    /// Before this counter #size is increased and, if a load limit is reached, a re-hash is
    /// performed.
    ///
    /// If otherwise an element was found, its valid iterator is returned.
    ///
    /// Note: Used by \alib{containers;HashMap::EmplaceIfNotExistent}.
    ///
    /// @param key       The key to the (first) element to find.
    /// @param hashCode  The hash code of parameter \p{key}.
    /// @return A pair of an iterator referring to the found or inserted element and a boolean
    ///         that is \c true if the insertion took place and \c false nothing was changed.
    //==============================================================================================
    std::pair<TIterator<T>, bool> insertIfNotExists( const TKey& key, size_t hashCode )
    {
        auto     bucketIdx= hashCode % bucketCount;
        Element* element  = findElement( bucketIdx, key, hashCode );
        if (element != nullptr )
            return std::make_pair(TIterator<T>( this, bucketIdx, element ), false);

        bucketIdx= increaseSize( 1, hashCode );

        Element* newElement= allocElement( hashCode );
        buckets[bucketIdx].pushFront( newElement );
        return std::make_pair(TIterator<T>( this, bucketIdx, newElement ) , true);
    }

    //==============================================================================================
    /// Inserts the topmost recyclable element if no element with the same key-portion of its value
    /// exists.
    ///
    /// @param key       Pointer to the key to search elements for deletion.
    /// @param hashCode  The hash code of parameter \p{key}.
    /// @return A pair containing an iterator referring to the element added.
    ///         The bool component is \c true if the insertion took place and \c false if a new
    ///         element was created.
    //==============================================================================================
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

} // namespace [alib::containers::detail]
