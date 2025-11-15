//==================================================================================================
/// \file
/// This header-file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace containers {

/// This type implements a container used for caching objects. When its #Size reaches its #Capacity,
/// one of the least recently used (LRU) objects is replaced with the next insertion of a non-cached
/// object.
///
/// This implementation uses an array of lists. The lists are forward lists of cached elements
/// sorted by their insertion time, with the latest insertion in the front.
/// The list (within the array of lists) used to store and later search an object, is chosen by
/// the hash value of its key, modulo the number of lists in place.
/// Both, the size of the lists as well the number of lists (the size of the array) can be chosen
/// on construction or with method #Reserve.
/// The memory needed for all entries (and the array of pointers to the start of the list)
/// is allocated once, when the capacity is set. Generic allocation options (heap-, monotonic-
/// and pool allocation) are given with template parameter \p{TAllocator}.<br>
/// Together with the custom data, the data's key's hashcode is stored with each element.
/// This can reduce search time dramatically in some cases, because the equal functor is invoked
/// only if two objects have the same hashcode.
///
/// This design has the following consequences:
/// - The maximum amount of objects stored, equals the product of the size of the hash-array
///   and the maximum size of the list.
/// - The maximum number of comparisons performed before a cache miss is detected, equals
///   the maximum size of the lists.
/// - The \b last recently used object is always found with only one comparison. The probability
///   that other previously used objects (the second last, third last, etc) are likewise found
///   with only one comparison, rises with the size of the array of lists.
/// - This container does not necessarily free the least recently used object with the insertion
///   of an unknown one. Instead, the least recently used object of those cached objects whose hash
///   function leads to the same list index is freed.
///
/// As a sample, two reasonable use cases taken from \alib should be quickly discussed:
/// 1. Class \alib{files;OwnerAndGroupResolver} of camp \alib_files:
///    - The type translates owner and group IDs of files to corresponding owner and group names.
///    - For this, the OS has to be called.
///    - If, for example, a directory listing similar to bash command <c>ls -la</c> is to be printed,
///      the entries in a directory usually share the same owner with very few exceptions.
///      This is true for home directories inside <c>/home</c> as well as for system directories
///      like <c>/usr/bin</c>.
///    - A perfect exception is directory <c>/home</c> itself. Here, every subfolder belongs to a
///      different owner.
///
///    This sample shows that already a very small cache capacity can dramatically decrease the
///    number of calls to the OS. On the other hand, in the case of the exceptional folders, still
///    the number of searches performed before a cache-miss is detected is limited to the length
///    of the lists and not to the total capacity.
///    The default capacity of the \b %LRUCacheTable chosen by class \b OwnerAndGroupResolver is
///    <c>6 x 6</c> and can be changed with
///    \alib{files::OwnerAndGroupResolver;SetOwnerCacheCapacity} and
///    \alib{files::OwnerAndGroupResolver;SetGroupCacheCapacity}.
/// 2. Camp \alib_alox:
///    - With each log operation the placement of the call in a sourcefile (\alib{lox;Scope})
///      is evaluated to resolve the resulting
///      \"\ref alib_mod_alox_tak_terminology_domains "Log Domain"\".
///    - For this, the source file name given by the C++ preprocessor, is to be parsed.
///    - \alox uses an \b %LRUCacheTable to avoid parsing repetitive scopes.
///    - It is very much depending on the use case of logging, how often repetitive scopes occur.
///      For example, if verbose logging is active, series of log statements comming from the same
///      code entity should be expected. If logging is limited and furthermore logging is made
///      in parallel by multiple threads, then the scopes should be rather mixed, but still
///      resulting from a limited number of files during a certain period of execution time.
///
///    The default capacity of the \b %LRUCacheTable chosen by \alib is likewise <c>4 x 6</c>
///    and can be changed with \alib{lox;Lox::SetFileNameCacheCapacity}.
///
/// Choosing the right values for the two dimensions of the capacity, depends on different
/// factors of the specific use case, and in most cases cannot be chosen to be perfect in
/// all situations. Among the things to consider are:
/// - The costs of creating a new object.
/// - The cost of comparing two keys.
/// - The statistical distribution of requests on (later) objects. In cases of even
///   distribution, the use of this type can become counter-productive!
/// - Potential memory constraints of the target platform, which may limit the capacity to set.
///
/// The stored type is not restricted in respect to complexity or construction, destruction, copy
/// and move semantics, etc. This is reached by deferring construction to
/// the caller of method #Try, who, in case of a cache miss, is obligated to duly use a
/// C++ placement-new to construct an entry.
///
/// This class provides a forward-iterator type that fetches all currently cached elements.
/// While this in most cases is not needed, the central interface method #Try also returns an
/// iterator. While this iterator is always valid, a second returned value indicates whether also
/// the element the iterator points to is valid. This distinction is necessary because this
/// container only reserves memory for the cached element, but it does not construct one.
/// To support elegant construction of elements, the iterator class provides method
/// \alib{containers::LRUCacheTable::TForwardIterator;Construct}.
/// Furthermore, methods
/// \alib{containers::LRUCacheTable::TForwardIterator;Key} and
/// \alib{containers::LRUCacheTable::TForwardIterator;Mapped} allow access to the stored types.
///
/// @see
/// - Type definitions \alib{containers;LRUCacheSet} and
///   \alib{containers;LRUCacheMap} based on this type.
/// - The approach to distinguish between "sets", "maps" and "sets with embedded keys" are
///   the same with class \b %HashTable. Therefore, the
///   \ref alib_ns_containers_hashtable_setandmap "documentation of class HashTable", which introduces
///   the same template parameter \p{TValueDescriptor} and also provides similar type definitions
///   \b %HashSet and \b %HashMap, is very helpful to understanding the concept of this type.
///
///
/// @tparam TAllocator       The \alib{lang;Allocator;allocator type} to use.
/// @tparam TValueDescriptor Defines the #StoredType, #KeyType, and #MappedType. Furthermore has to
///                          provide methods that extract the key and the mapped value from the
///                          stored type.<br>
///                          For details on required type definitions and method signatures, see
///                          provided implementations
///                          \alib{containers;TIdentDescriptor} and
///                          \alib{containers;TPairDescriptor} as a sample.<br>
///                          The type is published as #DescriptorType.
/// @tparam THash            The hash functor applicable to the key-type defined by
///                          \p{TValueDescriptor}.<br>
///                          Defaults to <c>std::hash<typename TValueDescriptor::KeyType></c>
///                          and is published as #HashType.
/// @tparam TEqual           The comparison functor on the key-type defined by
///                          \p{TValueDescriptor}.<br>
///                          Defaults to <c>std::equal_to<typename TValueDescriptor::KeyType></c>
///                          and is published as #EqualType.
template< typename TAllocator,
          typename TValueDescriptor,
          typename THash          = std::hash    <typename TValueDescriptor::KeyType>,
          typename TEqual         = std::equal_to<typename TValueDescriptor::KeyType>  >
class LRUCacheTable  : public lang::AllocatorMember<TAllocator>
#if ALIB_DEBUG_CRITICAL_SECTIONS
                     , public lang::DbgCriticalSections
#endif
{
  protected:
    /// The type of the base class that stores the allocator.
    using allocBase    =   lang::AllocatorMember<TAllocator>;

  public:
    /// Type definition publishing template parameter  \p{TAllocator}.
    using AllocatorType     = TAllocator;

    /// Type definition publishing template parameter \p{TValueDescriptor}.
    using DescriptorType    = TValueDescriptor;

    /// Type definition publishing the stored type of this container as defined with template
    /// parameter \p{TValueDescriptor}.
    using StoredType        = typename TValueDescriptor::StoredType;

    /// Type definition publishing the key type of this container as defined with template
    /// parameter \p{TValueDescriptor}.
    using KeyType           = typename TValueDescriptor::KeyType;

    /// Type definition publishing the map type of this container as defined with template
    /// parameter \p{TValueDescriptor}.
    using MappedType        = typename TValueDescriptor::MappedType;

    /// Type definition publishing template parameter \p{THash}.
    using HashType          = THash;

    /// Type definition publishing template parameter \p{TEqual}.
    using EqualType         = TEqual;

  protected:
    /// The node type of the cache lists.
    struct Entry : public lang::SidiNodeBase<Entry>
    {
        size_t      hashCode; ///< This entries hash code (calculated once on insertion)
        StoredType  data;     ///< The data cached.
    };

    /// Shortcut to a forward list
    using ForwardList   = lang::SidiListHook<Entry>;

    /// Pointer to reserved memory for elements. Size is #capacityLists x #capacityEntries.
    Entry*          elementPool;

    /// The next element to use with a cache-miss on a list that is not of full length, yet.
    Entry*          nextPoolElement;

    /// Array of size #capacityLists that holds the list..
    ForwardList*    lists;

    /// The number of LRU-lists.
    integer         capacityLists;

    /// The number of entries collected in each LRU-list.
    integer         capacityEntries;

  //================================================================================================
  //=== Iterator implementation
  //================================================================================================

    /// Templated implementation of \c std::iterator_traits.
    /// Will be exposed by outer class's definitions
    /// \alib{containers::LRUCacheTable;Iterator} and
    /// \alib{containers::LRUCacheTable;ConstIterator}.
    ///
    /// As the name of the class indicates, this iterator satisfies the C++ standard library concept
    /// \https{ForwardIterator,en.cppreference.com/w/cpp/named_req/ForwardIterator}.
    ///
    /// @tparam TConstOrMutable A constant or mutable version of #StoredType.
    template<typename TConstOrMutable>
    class TForwardIterator
    {
        #if !DOXYGEN
            friend class  LRUCacheTable;
        #endif

        using iterator_category = std::forward_iterator_tag;  ///< Implementation of <c>std::iterator_traits</c>.
        using value_type        = TConstOrMutable          ;  ///< Implementation of <c>std::iterator_traits</c>.
        using difference_type   = integer                  ;  ///< Implementation of <c>std::iterator_traits</c>.
        using pointer           = TConstOrMutable*         ;  ///< Implementation of <c>std::iterator_traits</c>.
        using reference         = TConstOrMutable&         ;  ///< Implementation of <c>std::iterator_traits</c>.

      protected:
        /// The pointer to the actual cache entry.
        Entry*            entry;

        /// The pointer to the actual cache entry.
        LRUCacheTable*    table;

        /// The actual list.
        integer           listIdx;

      public:
        /// Default constructor.
        TForwardIterator()                                                                        {}

        /// Copy constructor (default).
        /// @param other The iterator to assign from.
        TForwardIterator( const TForwardIterator& other )                                  =default;

        /// Copy constructor accepting a mutable iterator.
        /// Available only for the constant version of this iterator.
        /// @tparam TMutable The type of this constructor's argument.
        /// @param mutableIt Mutable iterator to copy from.
        template<typename TMutable>
        requires std::same_as<TMutable, TForwardIterator<StoredType>>
        TForwardIterator( const TMutable& mutableIt )
        : entry  ( mutableIt.entry   )                                                            {}

        /// Constructor that explicitly sets a valid iterator.
        /// @param pEntry    Pointer to a valid element.
        /// @param pTable    The cache table we belong to.
        /// @param pListIdx  The index of the list that \p{pEntry} belongs to.
        explicit
        TForwardIterator( Entry* pEntry, LRUCacheTable* pTable, integer pListIdx )
        : entry  (pEntry)
        , table  (pTable)
        , listIdx(pListIdx)                                                                       {}


        /// Constructor.
        /// @param pTable     The cache table we belong to.
        /// @param pListIdx   The index of the list to start with.
        explicit
        TForwardIterator( LRUCacheTable* pTable, integer pListIdx )
        : table  (pTable) {
            while( pListIdx < pTable->capacityLists ) {
                if( !pTable->lists[pListIdx].isEmpty() ) {
                    listIdx= pListIdx;
                    entry  = pTable->lists[pListIdx].first();
                    return;
                }
                ++pListIdx;
            }
            listIdx= pListIdx;
            entry  = nullptr;
        }

        /// Copy assignment (default).
        /// @param other The iterator to assign from.
        /// @return A reference to this object.
        TForwardIterator& operator=( const TForwardIterator& other)                        =default;

          //########################## To satisfy concept of  InputIterator ########################

        /// Prefix increment operator.
        /// @return A reference to this object.
        TForwardIterator& operator++() {
            ALIB_ASSERT_ERROR( entry != nullptr, "MONOMEM/LRUCACHE", "Illegal iterator." )
            entry= entry->next();
            while( entry == nullptr && ++listIdx < table->capacityLists )
                entry= table->lists[listIdx].first();

            return *this;
        }

        /// Postfix increment operator.
        /// @return An iterator value that is not increased, yet.
        TForwardIterator operator++( int) {
            auto result= TForwardIterator( *this);
            ++(*this);
            return result;
        }

        /// Comparison operator.
        /// @param other  The iterator to compare ourselves to.
        /// @return \c true if this and the given iterator are pointing to the same entry,
        ///         \c false otherwise.
        bool operator==( TForwardIterator other)              const { return entry == other.entry; }

        /// Comparison operator.
        /// @param other  The iterator to compare ourselves to.
        /// @return \c true if this and given iterator are not equal, \c false otherwise.
        bool operator!=( TForwardIterator other)                 const { return !(*this == other); }

          //############################## access to templated members #############################

        /// Retrieves the stored object that this iterator references.
        /// @return A reference to the stored object.
        TConstOrMutable&    operator*()                                                      const {
            ALIB_ASSERT_ERROR( entry != nullptr, "MONOMEM/LRUCACHE", "Illegal iterator." )
            return entry->data;
        }

        /// Retrieves a pointer to the stored object that this iterator references.
        /// @return A pointer to the stored object.
        TConstOrMutable*    operator->()                                                     const {
            ALIB_ASSERT_ERROR( entry != nullptr, "MONOMEM/LRUCACHE", "Illegal iterator." )
            return &entry->data;
        }

        /// Helper method that performs a placement-new on the data this iterator refers to.
        /// This method can be used if method #Try indicates a cache miss.
        ///@tparam TArgs  Types of variadic parameters given with parameter \p{args}.
        ///@param  args   Variadic parameters to be forwarded to the constructor of the inserted
        ///               instance of type #StoredType.
        ///@return A reference to the just constructed object.
        template<typename... TArgs>
        TConstOrMutable&        Construct( TArgs&&... args )                                 const {
            ALIB_ASSERT_ERROR( entry != nullptr, "MONOMEM/LRUCACHE", "Illegal iterator." )
            return *new( &entry->data ) TConstOrMutable(std::forward<TArgs>(args)...);
        }

        /// Retrieves the stored object that this iterator references.
        /// @return A reference to the stored object.
        TConstOrMutable&    Value()                                                          const {
            ALIB_ASSERT_ERROR( entry != nullptr, "MONOMEM/LRUCACHE", "Illegal iterator." )
            return entry->data;
        }

        /// Retrieves the key-portion of the stored object that this iterator references.
        /// @return A reference to the key-portion of the stored object.
        const KeyType&      Key()                                                            const {
            ALIB_ASSERT_ERROR( entry != nullptr, "MONOMEM/LRUCACHE", "Illegal iterator." )
            return TValueDescriptor().Key(entry->data);
        }

        /// Retrieves the stored object that this iterator references.<br>
        /// This method is an alias to <c>operator*</c>
        /// @return A reference to the mapped-portion of the stored object.
        MappedType&         Mapped()                                                         const {
            ALIB_ASSERT_ERROR( entry != nullptr, "MONOMEM/LRUCACHE", "Illegal iterator." )
            return TValueDescriptor().Mapped(entry->data);
        }
    }; // class TForwardIterator

  public:
    /// The mutable iterator over the cache entries.
    using Iterator         = TForwardIterator  <      StoredType>;

    /// The constant  iterator over the cache entries.
    using ConstIterator    = TForwardIterator  <const StoredType>;

    /// Returns an iterator referring to a mutable entry at the start of this cache.
    /// @return The first of entry in this container.
    Iterator      begin()                                       { return Iterator     ( this, 0 ); }

    /// Returns an iterator referring to a mutable, non-existing entry.
    /// @return The end of the list of entries in this container.
    Iterator      end()     { return Iterator     ( this, (std::numeric_limits<integer>::max)() ); }

    /// Returns an iterator referring to a constant entry at the start of this container.
    /// @return The first of entry in this container.
    ConstIterator begin()                                 const { return ConstIterator( this, 0 ); }

    /// Returns an iterator referring to a constant, non-existing entry.
    /// @return The end of the list of entries in this container.
    ConstIterator end() const { return ConstIterator(this, (std::numeric_limits<integer>::max)()); }

    /// Returns an iterator referring to a constant entry at the start of this container.
    /// @return The first of entry in this container.
    ConstIterator cbegin()                                const { return ConstIterator( this, 0 ); }

    /// Returns an iterator referring to a constant, non-existing entry.
    /// @return The end of the list of entries in this container.
    ConstIterator cend() const { return ConstIterator(this,(std::numeric_limits<integer>::max)()); }


    /// Constructor taking an allocator as well as the sizes forming the capacity of the cache.
    /// If one of the size parameters is \c 0, no pre-allocation is deferred and #Reserve has
    /// to be invoked before using this type.
    /// @param pAllocator The allocator type to use.
    /// @param tableSize  The number of LRU-lists.
    /// @param listSize   The (maximum) size of each list.
    LRUCacheTable(TAllocator& pAllocator, integer tableSize, integer listSize )
     : allocBase       (pAllocator)
    #if ALIB_DEBUG_CRITICAL_SECTIONS
     ,lang::DbgCriticalSections("LRUCacheTable")
    #endif
     , elementPool     (nullptr)
     , nextPoolElement (nullptr)
     , capacityLists   (0)
     , capacityEntries (0)                                       { Reserve( tableSize, listSize ); }

    /// Constructor omitting the allocator, usable only with heap allocation
    /// If one of the size parameters is \c 0, no pre-allocation is deferred and #Reserve has
    /// to be invoked before using this type.
    /// @param tableSize  The number of LRU-lists.
    /// @param listSize   The (maximum) size of each list.
    LRUCacheTable(integer tableSize, integer listSize )
     :
    #if ALIB_DEBUG_CRITICAL_SECTIONS
      lang::DbgCriticalSections("LRUCacheTable"),
    #endif
       elementPool     (nullptr)
     , nextPoolElement (nullptr)
     , capacityLists   (0)
     , capacityEntries (0)                                       { Reserve( tableSize, listSize ); }

    /// Destructor. Calls the destructor of each cached value.
    ~LRUCacheTable() {
        if( Capacity() == 0 )
            return;
        Clear();
        allocBase::AI().template FreeArray<Entry      >(elementPool, Capacity()    );
        allocBase::AI().template FreeArray<ForwardList>(lists      , capacityLists );
    }

    /// Returns the number of lists used for the cache.
    /// (Given with construction or a later invocation of method #Reserve.)
    /// @return This caches' size.
    integer  CapacityLists()                                         const { return capacityLists; }

    /// Returns the maximum number of entries held in each list.
    /// (Given with construction or a later invocation of method #Reserve.)
    /// @return This caches' size.
    integer  CapacityEntries()                                     const { return capacityEntries; }

    /// Returns the product of #CapacityLists and #CapacityEntries.
    /// (Both given with construction or a later invocation of method #Reserve.)
    /// @return This caches' size.
    integer  Capacity()                            const { return capacityEntries * capacityLists; }

    /// Counts the number of stored elements (operates in O(N)).
    /// @return The number of elements stored in the cache.
    integer  Size()                                                  const noexcept {ALIB_DCS_SHARED
        integer result= 0;
        for (int i = 0; i < capacityLists; ++i)
            result+= lists[i].count();
        return result;
    }

    /// Changes the size of this cache.
    /// @param newQtyLists          The number of LRU-lists to use.
    /// @param newQtyEntriesPerList The maximum length of each LRU-list list.
    void        Reserve( integer newQtyLists, integer newQtyEntriesPerList ) {
        if(    CapacityLists()   == newQtyLists
            && CapacityEntries() == newQtyEntriesPerList )
            return;

        Clear();
        ALIB_DCS
        auto newCapacity= newQtyLists * newQtyEntriesPerList;
        if( Capacity() != newCapacity) {
            if( Capacity() != 0 )
                allocBase::AI(). template FreeArray<Entry>(elementPool, Capacity());

            elementPool = newCapacity ? allocBase::AI(). template AllocArray<Entry>(newCapacity)
                                      : nullptr;
        }

        if( capacityLists != newQtyLists) {
            if( capacityLists )
                allocBase::AI(). template FreeArray<ForwardList>(lists, capacityLists );
            lists= newQtyLists ? allocBase::AI(). template AllocArray<ForwardList>(newQtyLists)
                               : nullptr;
            
            for( integer i = 0; i < newQtyLists; ++i )
                lists[i].reset();
        }

        capacityLists=      newQtyLists;
        capacityEntries=    newQtyEntriesPerList;
        nextPoolElement=    elementPool;
    }

    /// Clears this cache.
    void        Clear()                                                                    {ALIB_DCS
        for (integer i = 0; i < capacityLists; ++i) {
            auto* elem= lists[i].first();
            while(elem) {
                lang::Destruct(elem->data);
                elem= elem->next();
            }
            lists[i].reset();
        }
        nextPoolElement= elementPool;
    }

    /// Retrieves a value through this cache. The following cases can occur:
    /// 1. No element with matching \p{key} is found in the cache while not all elements
    ///    of #elementPool are used, yet. In this case the next entry is taken from the pool, and
    ///    the entry is added to the front of the list.
    /// 2. No element is found while all elements of the #elementPool have been inserted into the
    ///    list already. In this case the last entry of the list is removed, the destructor is
    ///    called on its contents, and the entry is moved to the front of the list.
    /// 3. The element with matching \p{key} is found. In this case, the entry is moved to the start
    ///    of the list (if not already there).
    ///
    /// In cases 1. and 2., this method returns \c false in the first element of the result pair,
    /// which tells the caller that the value found in the second element has to be constructed.
    /// This can be done using convenient method
    /// \alib{containers::LRUCacheTable::TForwardIterator;Construct;Iterator::Construct}.
    /// Note that besides operator*
    /// \alib{containers::LRUCacheTable::TForwardIterator;operator*}, the iterator type
    /// offers methods
    /// \alib{containers::LRUCacheTable::TForwardIterator;Key} and
    /// \alib{containers::LRUCacheTable::TForwardIterator;Mapped} to directly access
    /// corresponding portions of the stored value.
    ///
    /// @param key  The key value to search for.
    /// @return A pair of a boolean and an iterator. If the boolean value is \c true, a cached
    ///         entry was found and can be used. If it is \c false, the iterator is valid but
    ///         its data is not! In this case, the caller has to use a placement-new on the
    ///         address of the data receivable with the iterator. This might best be done by
    ///         calling \ref Iterator::Construct.
    [[nodiscard]]
    std::pair<bool, Iterator>  Try(const KeyType& key)                                     {ALIB_DCS
        ALIB_ASSERT_ERROR(Capacity() > 0, "MONOMEM","Capacity of LRUCacheTable equals 0 (not set).")

        // start the search below with the first element
        const size_t  keyHash = THash{}(key);
        const integer listIdx = integer(keyHash % size_t(capacityLists));
        auto&         list    = lists[listIdx];
        Entry*        prev2   = nullptr;
        Entry*        prev1   = reinterpret_cast<Entry*>(&list);
        Entry*        actual  = list.first();
        integer       cnt     = 0;

        
        while( actual != nullptr ) {
            if (    actual->hashCode == keyHash
                 && TEqual{}(TValueDescriptor().Key(actual->data), key)  )
            {
                // Move the accessed item to the front
                if (prev1 != nullptr) {
                    prev1->next( actual->next() );
                    actual->next( list.next() );
                    list.next( actual );
                }
                return std::make_pair(true, Iterator(actual, this, listIdx ));
            }
            prev2=  prev1;
            prev1=  actual;
            actual= actual->next();
            ++cnt;
        }

        // Value not found

        // cache is full? -> take the last entry, change it and push it to the front.
        if( cnt == capacityEntries ) {
            prev2->next( nullptr );
            prev1->data.~StoredType();
            prev1->hashCode= keyHash;
            prev1->next( list.next() );
            list.next( prev1 );
            return std::make_pair(false, Iterator(list.first(), this, listIdx ));
        }

        // cache not full yet -> Create new entry
        Entry* newEntry= nextPoolElement++;
        newEntry->hashCode= keyHash;
        newEntry->next(list.next());
        list.next(newEntry);
        return std::make_pair(false, Iterator(list.first(), this, listIdx ));
    }

}; // struct LRUCacheTable



/// This type definition is a shortcut to \alib{containers;LRUCacheTable}, usable if data
/// stored in the container does not include a key-portion, and thus the key to the data
/// retrievable from the cache has to be separately defined.<br>
/// To achieve this, this type definition aggregates types \p{TKey} and \p{TMapped} into a
/// <c>std::pair<TKey,TMapped></c>. This is done using special value descriptor type
/// \alib{containers;TPairDescriptor}.
///
/// \see
///   For a detailed description of this type, see original type
///   \alib{containers;LRUCacheTable}, as well as alternative type definition
///   \alib{containers;LRUCacheSet}.
///
/// @tparam TAllocator      The \alib{lang;Allocator;allocator type} to use.
/// @tparam TKey            The type of the <em>key-portion</em> of the inserted data.<br>
///                         This type is published as \alib{containers;LRUCacheTable::KeyType}.
/// @tparam TMapped         The type of the <em>mapped-portion</em> of the inserted data.<br>
///                         This type is published as
///                         \alib{containers;LRUCacheTable::MappedType}.
/// @tparam THash           The hash functor applicable to \p{TKey}.<br>
///                         Defaults to <c>std::hash<TKey></c> and is published as
///                         \alib{containers;LRUCacheTable::HashType}.
/// @tparam TEqual          The comparison functor on \p{TKey}.<br>
///                         Defaults to <c>std::equal_to<TKey></c> and is published as
///                         \alib{containers;LRUCacheTable::EqualType}.
template< typename TAllocator,
          typename TKey, typename TMapped,
          typename THash          = std::hash    <TKey>,
          typename TEqual         = std::equal_to<TKey>   >
using LRUCacheMap    = LRUCacheTable< TAllocator,
                                      containers::TPairDescriptor<TKey, TMapped>,
                                      THash, TEqual >;

/// This type definition is a shortcut to \alib{containers;LRUCacheTable}, usable if the full
/// portion of the data stored in the container is used for the comparison of values.
///
/// \note
///   As with this definition template type \p{TKey} equals stored type \p{T}, methods of
///   target type \alib{containers;LRUCacheTable} that accept an object of template type
///   \b TKey expect an object of \p{T} when this type type defintion is used.<br>
///   In case this is not wanted (or possible), and only the true key-portion should be expected
///   by interface functions such as \alib{containers;LRUCacheTable::Try}, the underlying
///   original type \alib{containers;LRUCacheTable}) has to be used.
///
/// \see
///   For a detailed description of this type, see original type
///   \alib{containers;LRUCacheTable}, as well as alternative type definition
///   \alib{containers;LRUCacheMap}.
///
/// @tparam TAllocator      The \alib{lang;Allocator;allocator type} to use.
/// @tparam T               The element type stored with this container.
///                         This type is published as
///                         \alib{containers;LRUCacheTable::StoredType} and type definition
///                         \alib{containers;LRUCacheTable::KeyType} becomes a synonym.
/// @tparam THash           The hash functor applicable to \p{T}. If \c void is given, no hashing
///                         is performed.<br>
///                         Defaults to <c>std::hash<T></c> and is published as
///                         \alib{containers;LRUCacheTable::HashType}.
/// @tparam TEqual          The comparison functor on \p{TKey}.<br>
///                         Defaults to <c>std::equal_to<TKey></c> and is published as
///                         \alib{containers;LRUCacheTable::EqualType}.
template< typename TAllocator,
          typename T,
          typename THash          = std::hash    <T>,
          typename TEqual         = std::equal_to<T>  >
using LRUCacheSet    = LRUCacheTable< TAllocator,
                                      TIdentDescriptor<T>,
                                      THash, TEqual        >;

} // namespace alib[::containers]

/// Type alias in namespace \b alib.
template< typename TAllocator,
          typename TValueDescriptor,
          typename THash          = std::hash    <typename TValueDescriptor::KeyType>,
          typename TEqual         = std::equal_to<typename TValueDescriptor::KeyType> >
using LRUCacheTable  = containers::LRUCacheTable< TAllocator, TValueDescriptor, THash, TEqual>;


/// Type alias in namespace \b alib.
template< typename TAllocator,
          typename TKey, typename TMapped,
          typename THash          = std::hash    <TKey>,
          typename TEqual         = std::equal_to<TKey>  >
using LRUCacheMap    = containers::LRUCacheTable< TAllocator,
                                                           containers::TPairDescriptor<TKey, TMapped> >;

/// Type alias in namespace \b alib.
template< typename TAllocator,
          typename T,
          typename THash          = std::hash    <T>,
          typename TEqual         = std::equal_to<T>  >
using LRUCacheSet    = containers::LRUCacheTable< TAllocator,
                                                           containers::TIdentDescriptor<T>,
                                                           THash, TEqual >;
} // namespace [alib]
