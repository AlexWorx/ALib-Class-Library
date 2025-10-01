
//==================================================================================================
/// \file
/// This header-file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace containers {
//==================================================================================================
/// # Contents #
/// \ref alib_ns_containers_hashtable_intro                   "1. Introduction"                  <br>
/// \ref alib_ns_containers_hashtable_setandmap               "2. Hash Sets vs. Hash Maps"       <br>
/// \ref alib_ns_containers_hashtable_single_multi            "3. Single And Multiple Entries"   <br>
/// \ref alib_ns_containers_hashtable_rehashing               "4. Re-Hashing"                    <br>
/// \ref alib_ns_containers_hashtable_iterators               "5. Iterators"                     <br>
/// \ref alib_ns_containers_hashtable_hashcodes               "6. Hash Codes"                    <br>
/// &nbsp;&nbsp;\ref alib_ns_containers_hashtable_caching       "6.1 Caching Hash Codes"         <br>
/// &nbsp;&nbsp;\ref alib_ns_containers_hashtable_hashprecalc   "6.2 Hash Codes Pre-calculation" <br>
/// &nbsp;&nbsp;\ref alib_ns_containers_hashtable_hashquality   "6.3 Hash Codes Pre-calculation" <br>
/// \ref alib_ns_containers_hashtable_memory                 "7. Memory Use"                     <br>
/// \ref alib_ns_containers_hashtable_comparison             "8. Comparison To Standard Library Hash-Types" <br>
/// \ref alib_ns_containers_hashtable_referencedoc            "Reference Documentation"                     <br>
///
/// \I{#############################################################################################}
/// \anchor alib_ns_containers_hashtable_intro
/// # 1. Introduction #
/// This class implements a \https{hash table,en.wikipedia.org/wiki/Hash_table} that
/// stores and retrieves objects very efficiently in respect to execution performance.
/// All memory for the hash table and its entries are allocated using a template
/// \alib{lang;Allocator;allocator type}.
///
/// Two type definitions based on this class exist, which change the set of template parameters
/// of this type by providing reasonable replacements. These types are:
/// - \alib{containers;HashMap} and
/// - \alib{containers;HashSet}.
///
/// In many cases, the use of one of these definitions is more convenient than instantiating this
/// type directly. The meaning of these types is discussed in the next section.
///
///  \I{#############################################################################################}
/// \anchor alib_ns_containers_hashtable_setandmap
/// # 2. Hash Sets vs. Hash Maps#
/// A <em>"hash set"</em> is commonly understood as a type that contains custom values of
/// #StoredType, which are also used as the key to finding such stored objects.
/// <em>"Hash maps"</em> are hash tables that store objects of a custom #MappedType which are
/// associated to a value of a key #KeyType. In this "mode" the key is not contained in the custom
/// value.<br>
/// The template parameters and implementation of this class supports both concepts and they are
/// covered by the two provided alternative type definitions mentioned in the previous section.
/// Furthermore, those go along well with what is found in the standard C++ library:
///
/// 1. \alib{containers;HashSet}
///    Removes template parameter \p{TValueDescriptor} (by presetting it to a built-in helper-type)
///    and instead denotes all three types, namely #StoredType, #KeyType and #MappedType to the
///    same new template parameter \p{T}.
///    Functors \p{THash} and \p{TEqual} consequently expect the \b %StoredType.<br>
///
///    This type definition is similar to what types <c>std::unordered_set</c> and
///    <c>std::unordered_multiset</c> of the standard C++ class library provide.
///
/// 2. \alib{containers;HashMap}
///    Removes template parameter \p{TValueDescriptor} (by presetting it to a built-in helper-type)
///    and instead introduces template parameters \p{TKey} and \p{TMapped}.
///    Here, only the <em>key-portion</em> is used for calculating hash values and testing elements
///    for equality. The <em>mapped-portion</em> is the true custom type that is to be stored. <br>
///    Consequently, \b %HashMap defines the #StoredType as <c>std::pair<TKey, TMapped></c>.<br>
///
///    The type definition is similar to what types <c>std::unordered_map</c> and
///    <c>std::unordered_multimap</c> of the standard C++ class library provide.
///
/// To achieve this flexibility, template parameter \p{TValueDescriptor}, which is exposed as
/// #DescriptorType, is constrained to have two methods \b Key() and \b Mapped() to extract
/// a reference to the corresponding portions out of the stored value.
///
/// One of the advantages of this approach is that this type supports a third "mode", besides
/// implementing <em>"hash sets"</em> and <em>"hash maps"</em>: Custom value type #StoredType
/// may have a <em>key-portion</em> embedded, instead of using the complete type as a search key.<br>
/// While there is no specific type definition for this "key-embedded mode" available, the using
/// code needs to create a custom version of template parameter \p{TValueDescriptor}
/// which enables the extraction of the key and mapped portions of the stored type.
/// The following table summarizes this:
///
///   Working Mode              | Type to Use                         |  Value Descriptor Type
///  ---------------------------|-------------------------------------|-------------------------
///  Hash Set                   |\alib{containers;HashSet} (A type definition on \b %HashTable)  | Automaticly chosen as built-in \alib{containers;TIdentDescriptor}
///  Hash Map                   |\alib{containers;HashMap} (A type definition on \b %HashTable)  | Automaticly chosen as built-in \alib{containers;TPairDescriptor}
///  Hash Set with embedded Key |\alib{containers;HashTable} (The original type)                 | A <b>custom type</b> has to be provided
///
/// \note The hash-table implementations of the standard C++ library do not support a similar
///       approach. While with the provision of customized hash- and comparison-functors, of course
///       only a portion of a type stored in a set can be used for hashing and comparing, still the
///       interface functions require to receive a "complete" instance, which then is often
///       "nulled" or "empty" in respect to the fields that are not used for the key.<br>
///       Because of this limitation, \alib introduces template parameter \p{TValueDescriptor} in
///       addition to parameters \p{THash} and \p{TEqual}.
///
///
/// As a sample for the advantage consider method
/// #EmplaceIfNotExistent(const KeyType& key, TArgs&&... args).
/// A corresponding method is found with type <c>std::unordered_map::try_emplace</c>, but not
/// with <c>std::unordered_set</c>. In contrast with this implementation, the method
/// is usable with <em>hash maps</em> as well as with <em>hash sets</em>!<br>
/// The only precondition to the availability of this method, is that the #StoredType has to be
/// constructible from the combined list of given arguments, hence the \p{KeyType} argument along
/// with given arguments of variadic template types \p{TArgs}.
/// The same is true for method #EmplaceOrAssign(const KeyType&, TArgs&&... args)
///
///
/// The set of available interface methods slightly changes with the two operation modes:
/// 1. <b>Hash Set Mode:</b><br>
///    Method overload #EmplaceIfNotExistent(TArgs&&... args) is exclusively available with hash
///    sets.<br>
///
/// 2. <b>Hash Map Mode:</b><br>
///    The following method overloads are exclusively available with hash \b maps:
///    - #InsertOrAssign(const KeyType&, MappedType&&)
///    - #InsertOrAssign(const KeyType&, const MappedType&)
///    - #InsertIfNotExistent(const KeyType&, MappedType&&)
///    - #InsertIfNotExistent(const KeyType&, const MappedType&)
///    <br><br>
///
///    In addition, the following method overloads of inner types are also exclusively available
///    with hash \b maps.
///    - \alib{containers::detail::HashTableBase;TIterator::Mapped}
///    - \alib{containers::detail::HashTableBase;TLocalIterator::Mapped}
///    - \alib{containers::HashTable;ElementHandle::Mapped}
///
/// A hint to restrictions is given with the documentation of each method of concern.
/// Note that methods that expect a single #KeyType are usable with both operation modes, because
/// with hash sets the key-type equals type #StoredType.
///
/// \note
///   Technically, the availability of the methods is selected at compile-time.
///
/// \I{#############################################################################################}
/// \anchor alib_ns_containers_hashtable_single_multi
/// # 3. Single And Multiple Entries #
/// The standard C++ library differentiates between hashing containers that accept only one
/// element with a specific <em>key-portion</em> of the value (see <c>std::unordered_set</c> and
/// <c>std::unordered_map</c>) and those that accept multiple insertions of objects with the
/// same <em>key-portion</em> (see <c>std::unordered_multiset</c> <c>std::unordered_multimap</c>).
///
/// This library does \b not provide separate types and any instantiation of this class allows
/// multiple entries. But this is rather an extension of functionality, than a restriction
/// and does not impose a penalty on performance.
///
/// If unique entries are to be achieved, the user of the type has to make sure for herself that
/// no multiple entries are inserted. This can be guaranteed, with the exclusive use of the
/// following set of (partly overloaded) methods, which prevent the creation of multiple entries:
///
/// - #InsertUnique / #EmplaceUnique
/// - #InsertOrAssign / #EmplaceOrAssign
/// - #InsertIfNotExistent / #EmplaceIfNotExistent
///
/// In contrast to this, methods #Insert and #Emplace (and their overloads) will insert
/// an equal value without giving further notice (for example by providing a special return value
/// that indicates if the inserted key existed before).<br>
/// Method #EraseUnique(const KeyType&) is more efficient than #erase(const KeyType&)
/// and a further advantage is that it asserts (in debug-compilations) that not more than one
/// element is found in the table.
///
/// \note
///   If uniqueness of key values is needed, it might be seen by the reader to be a "disadvantage"
///   that the burden is on the user of the class to guarantee such uniqueness.
///   However, such burden exists with the set-types of the standard library likewise:
///   There, result values have to be evaluated to determine if an insertion was successful or not.
///   The various smaller changes and extensions of the <c>std</c>-types with C++ 14 and 17 reflect
///   the design problems of the approach to provide "unique" and "multi" types.
///
/// \note
///   The approach that \alib takes here has three small benefits:
///   1. A user is free to choose a "mixed mode" by allowing duplicates of certain values (keys)
///      and not allowing duplicates for others.
///      While this can be achieved with <c>std::unordered_multimap/set</c> as well, a performance
///      penalty is given, unless the extended interface of C++17 standard is used with great care.
///   2. The code is better readable, due to explicit naming of the invocations, in contrast
///      to the need to knowing a container's type with each invocation.
///   3. The use of the type and this documentation is less confusing, because only one type exists.
///      In contrast, the two types found in <c>std</c> have equally named methods that act
///      differently and return different result types.
///
/// \I{#############################################################################################}
/// \anchor alib_ns_containers_hashtable_rehashing
/// # 4. Re-Hashing #
/// A check for the need to perform re-hashing is made with every insertion of an element.
/// The policy for re-hashing is described as follows:
/// - With insertions, the new average bucket size is calculated as #Size divided by #BucketCount.
/// - This value is compared with #MaxLoadFactor and if higher the number of buckets is increased.
/// - When increased, the new minimum number of buckets is calculated as #Size divided by
///   #BaseLoadFactor. Starting from this minimum, the effective number of buckets is chosen as a
///   next higher prime number from a static table.
/// - Automatic rehashes may be disabled by setting #MaxLoadFactor to a very high value, e.g.
///   <c>std::numeric_limits<float>::max()</c>
///
/// The number of buckets is never decreased, unless method #Reset is invoked.
///
/// Manual re-hashing is not supported by design. In our view, with monotonic growth (or stability
/// in size) and hence the absence of dynamic increase/decrease scenarios, manual rehashing is not
/// needed. Only the base and maximum load factors are of concern, which both can be
/// specified with \ref HashTable::HashTable "construction" and with methods #MaxLoadFactor
/// respectively #MaxLoadFactor.<br>
/// What can be done, however, is to use the method #MaxLoadFactor to "disable" rehashing
/// temporarily and thus to allow an efficient mass insertion. Nevertheless, if the number of
/// elements to be inserted is known upfront, the use of method #Reserve, respectively
/// #ReserveRecyclables is the preferred approach.
///
///  \I{############################################################################################}
/// \anchor alib_ns_containers_hashtable_iterators
/// # 5. Iterators #
/// ## 5.1 Iterator Types ##
/// There are two types of iterators provided:
/// - \alib{containers::HashTable;Iterator} and its constant sibling
///   \alib{containers::HashTable;ConstIterator}, used to iterate over all elements of the
///   hash table,
/// - \alib{containers::HashTable;LocalIterator} and its constant sibling
///   \alib{containers::HashTable;ConstLocalIterator} used to iterate over the elements
///   of a certain bucket only.
///
/// Both types satisfy the C++ standard library concept
/// \https{ForwardIterator,en.cppreference.com/w/cpp/named_req/ForwardIterator}.
///
/// ## 5.2 Validity Rules ##
/// The following rules define the validity of existing iterator instances on table operations:
/// - <b>Element \b Insertions:</b>
///   - If no rehashing is performed (see previous section) all iterators remain valid.
///   - If rehashing is performed, existing iterators become invalid in respect to allowance
///     of increments and comparison. The elements that existing iterators referred to
///     before the insertion remain valid. Consequently existing iterators may still be used
///     to access the custom element value and modify the mapped portion of it.
/// - <b>Element \b Removals:</b> <br>
///   All existing iterators that referred to elements that have been removed become invalid.
///   All others remain fully intact.<br>
///   The order of the elements that are not erased is preserved, what makes it possible to
///   erase individual elements while iterating through the container.
///
///
///\I{#############################################################################################}
/// \anchor alib_ns_containers_hashtable_hashcodes
/// # 6. Hash Codes #
///
///\I{#############################################################################################}
/// \anchor alib_ns_containers_hashtable_caching
/// ## 6.1 Caching Hash Codes ##
///
/// Template parameter \p{THashCaching} may be used to control if hash codes are cached.
/// Caching the hash codes increases the memory consumption of this class by <c>sizeof(size_t)</c>
/// per inserted element. While this is only a small amount and memory consumption should not
/// be a great concern when monotonic allocation is used, caching the hash codes may impose a
/// reasonable performance impact. This impact depends on the performance of functor \p{THash}
/// working on the <c>key-portion</c> of the inserted value.
///
/// The cached hash code is used when the table is
/// \ref alib_ns_containers_hashtable_rehashing "re-hashed". In addition, the cached
/// hash code is also used with almost all interface methods (insertion, deletion and search
/// operations): If cached, any needed comparison of two elements will first compare the hash
/// codes and only invoke templated functor \p{TEqual} if those match.
///
/// If template parameter \p{THashCaching} evaluates to <b>Caching::Auto</b> then this class
/// defaults to use caching if type #KeyType is not arithmetic
/// (using <c>std::is_arithmetic<TKey></c> for this check).<br>
///
/// The caching option of an instance of this class can be queried with enum #CachedHashCodes.
///
///
///\I{#############################################################################################}
/// \anchor alib_ns_containers_hashtable_hashprecalc
/// ## 6.2 Hash Codes Pre-calculation ##
/// The following overloaded methods accept parameter \p{hashCode} in addition to the parameters
/// accepted by their corresponding base version:
/// - #Find(const KeyType&, size_t)
/// - #Insert(const T&, size_t)
/// - #Insert(T&&, size_t)
/// - #InsertUnique(T&&, size_t)
/// - #InsertOrAssign(const KeyType&, MappedType&& mapped, size_t)
/// - #InsertIfNotExistent(const KeyType&, MappedType&& mapped, size_t)
/// - #InsertIfNotExistent(T&&, size_t )
/// - #Extract(const KeyType&, size_t )
/// - #Erase(const KeyType&, size_t )
/// - #EraseUnique( const KeyType&, size_t )
///
/// The rationale for the provision of these methods is to allow to "pre-calculate" a key's hash
/// code before invoking the method. This is efficient in situations where one or more subsequent
/// operations with the same key are performed. For example:
/// - Insertions of multiple objects with the same key.
/// - Insertions of an object into multiple hash tables.
/// - Situations where the result of a find operation may may lead to further operations with the
///   same object.
///
///\I{#############################################################################################}
/// \anchor alib_ns_containers_hashtable_hashquality
/// ## 6.3 Hash Code Quality ##
/// To have hash tables perform in constant time <em>O(1)</em> in the average case, a well
/// implemented calculation of hash-codes has to be provided for template type \p{TKey} with
/// template functor \p{THash}. This is an obligation of the user of this type.
///
/// This \alibmod_nl supports the compiler-symbol \ref ALIB_DEBUG_CONTAINERS which enables extra
/// features.
/// The entities relevant for this type are:
/// - \alib{containers;DbgGetHashTableDistribution},
/// - \alib{containers;DbgDumpDistribution} and
/// - \alib{containers;DbgDumpHashtable}.
///
/// These methods may be used to assert the quality of custom hash algorithms.
///
///\I{#############################################################################################}
/// \anchor alib_ns_containers_hashtable_memory
/// # 7. Memory Use #
/// With template parameter \p{TRecycling} being either \alib{containers;Recycling;Private}
/// (the default) or \alib{containers;Recycling;Shared}, the internal
/// <em>"node objects"</em> are remembered when deleted, and "recycled" with future insertions.
/// The rationale for this lies in the simple fact that memory cannot be returned to
/// the monotonic allocator. The recycling mechanism is very lightweight and does not cost measurable
/// performance. If it is assured that no deletions are made during the life-cycle of an instance,
/// type \alib{containers;Recycling;None} may be used to eliminate this little overhead
/// of recycling. This is why type-definition \alib{MonoAllocator} is recommended to be used
/// with this container.
///
/// If the table is re-hashed, the former bucket list is likewise recycled and sliced into as many
/// internal node objects as possible. The precondition for this is that the allocator
/// interface method \alib{lang::Allocator;allowsMemSplit} returns \c true. This is true for type
/// \b MonoAllocator.
///
/// With these two mechanisms in place, the use of monotonic allocation with this container
/// is very safe in respect to avoid memory exhaustion.
/// The use of this class, even in scenarios where a lot of (theoretically an endless amount of)
/// erase and insert operations are performed, will not increase memory consumption, as long it is
/// guaranteed that the overall size of the table is limited.<br>
///
/// If a maximum number of possible insertions is known, method #ReserveRecyclables might be used
/// to allocate all needed memory at once. With that, a
/// \alib{containers;MonoAllocator::TakeSnapshot;snapshot} of the allocator can be taken and
/// later used to reset the allocator to the minimum that preserves all memory in the according
/// hash table instance.<br>
/// For advanced usage, it is advisable to fully understand the concept of monotonic allocation
/// implemented with this module \alib_containers.
///
///  \I{############################################################################################}
/// \anchor alib_ns_containers_hashtable_comparison
/// # 8. Comparison To Standard Library Hash-Types #
/// In the previous sections, it was already referred several times to types
/// - <c>std::unordered_map</c>,
/// - <c>std::unordered_multimap,</c>
/// - <c>std::unordered_set</c> and
/// - <c>std::unordered_multiset</c>
///
/// of the C++ standard library. The uses cases and features of these four compared to this type are
/// generally the same and this type can be used to replace the standard tyes without general
/// limitations and vice versa.
///
/// Then with C++17, the standard library was extended by a new set of types, namely
/// - <c>std::pmr::unordered_map</c>,
/// - <c>std::pmr::unordered_multimap,</c>
/// - <c>std::pmr::unordered_set</c> and
/// - <c>std::pmr::unordered_multiset</c>
///
/// which, likewise this type, may use monotonic allocation (for example, in combination with C++17
/// type <c>std::pmr::monotonic_buffer_resource</c>).
/// On the one hand, \alib in the past needed support of monotonic allocation already for
/// C++ 11, and on the other hand, the C++17 library extensions follow slightly different design
/// goals. Details of these were given in the previous section.
///
/// Besides these general differences, the specific similarities and differences can be summarized
/// as follows:
///
/// - This \alib type does not distinguish sets and maps but provides a more flexible approach:
///   Mapped types do not necessarily need to be build using a <c>std::pair</c> of key and value
///   elements.
///   For convenience, when the use of <c>std::pair</c> is suitable, type definition
///   \alib{containers;HashMap} offers exactly this.
/// - This \alib type does not distinguish sets/maps from multi-sets/maps.
///   The rationale for this design decision has been described in section
///   \ref alib_ns_containers_hashtable_single_multi "3. Single And Multiple Entries".
/// - Almost all members of the four standard library types are implemented with this type
///   in a very compatible fashion. The member names were translated from <em>lower_snake_case</em>
///   to <em>UpperCamelCase</em> and then sometimes slightly changed.<br>
///   C++17 extensions of the standard types were reflected.
/// - Method #Find provides additional overloads that accept an externally (pre-) calculated
///   hash code. This allows efficiently using the same key with a series of searches within
///   different hash table instances. Note that C++20 offers a templated version of <c>find</c>,
///   which allows performing the same optimization.
/// - Erase methods that accept a #LocalIterator (bucket iterators) that are not available with
///   the four standard library types, are provided with this type.
/// - There is no <c>operator[]</c> defined. The rationale to omit this - at first sight convenient
///   and intuitive - operator, is that it imposes insert operation even if used in r-value
///   expressions. This is considered an unwanted side effect.
/// - The caching of hash-codes can be controlled with this type, while it is an implementation
///   dependent feature with the standard library.
/// - The automatic increase (re-hashing) of the bucket number can be tweaked with
///   constructor parameter \p{pBaseLoadFactor} (and method #BaseLoadFactor(float)), which is not
///   specified with the standard library types.
/// - Assignment <c>operator=</c> is not available with this implementation.
///   If needed, such has to implemented by a user (full iteration with copying elements from one
///   instance to another).
///
/// @see
///  - Chapter \ref alib_contmono_containers_types of the joint Programmer's Manuals of modules
///    \alib_containers and \alib_monomem.
///  - Chapter \ref alib_threads_intro_assert_entry of the Programmer's Manual of module
///    \alib_threads for information about debugging multithreaded access on instances of this type.
///\I{#############################################################################################}
/// \anchor alib_ns_containers_hashtable_referencedoc
/// # Reference Documentation #
/// @tparam TAllocator       The allocator type to use, as prototyped with \alib{lang;Allocator}.
/// @tparam TValueDescriptor Defines the #StoredType, #KeyType and #MappedType. Furthermore has to
///                          proving methods that to extract key- and mapped values out of the
///                          stored type.<br>
///                          For details on required type definitions and method signatures, see
///                          provided implementations
///                          \alib{containers;TIdentDescriptor} and
///                          \alib{containers;TPairDescriptor} as a sample.<br>
///                          The type is published as
///                          \alib{containers;HashTable::DescriptorType}.
/// @tparam THash            The hash functor applicable to the key-type defined by
///                          \p{TValueDescriptor}.
///                          Defaults to <c>std::hash<typename TValueDescriptor::KeyType></c>
///                          and is published as \alib{containers;HashTable::HashType}.
/// @tparam TEqual           The comparison functor on the key-type defined by \p{TValueDescriptor}.
///                          Defaults to <c>std::equal_to<typename TValueDescriptor::KeyType></c>
///                          and is published as \alib{containers;HashTable::EqualType}.
/// @tparam THashCaching     Determines if hash codes are cached when elements are inserted.
///                          Defaults to <b>Caching::Auto</b>, which enables caching if
///                          <c>std::is_arithmetic<TKey>::value</c> evaluates to \c false.
/// @tparam TRecycling       Denotes the type of recycling that is to be performed. Possible values
///                          are
///                          \alib{containers;Recycling;Private} (the default),
///                          \alib{containers;Recycling;Shared} or
///                          \alib{containers;Recycling;None}.
//==================================================================================================
template< typename      TAllocator,
          typename      TValueDescriptor,
          typename      THash        = std::hash    <typename TValueDescriptor::KeyType>,
          typename      TEqual       = std::equal_to<typename TValueDescriptor::KeyType>,
          lang::Caching THashCaching = lang::Caching::Auto,
          Recycling     TRecycling   = Recycling::Private >
class HashTable : protected detail::HashTableBase<TAllocator,TValueDescriptor,THash,TEqual,THashCaching,TRecycling>
{
    public:
    #if !DOXYGEN
        #if ALIB_DEBUG_CRITICAL_SECTIONS
            mutable lang::DbgCriticalSections   dcs;
            #define  DCS       ALIB_DCS_WITH(dcs)
            #define  DCSSHRD   ALIB_DCS_SHARED_WITH(dcs)
        #else
            #define  DCS
            #define  DCSSHRD
        #endif
    #endif

  protected:
    // protected shortcuts to parent and its types we need
    #if !DOXYGEN
        using base      = detail::HashTableBase<TAllocator,TValueDescriptor,THash,TEqual,THashCaching,TRecycling>;
        using Element   = typename base::Element;
        using Node      = typename base::Node;
    #endif

    /// The recycler type.
    using recyclerType=  typename base::recyclerType;


  public:

    //##############################################################################################
    // Types and Constants
    //##############################################################################################
    /// Type definition publishing template parameter  \p{TAllocator}.
    using AllocatorType = TAllocator;

    /// Type definition publishing template parameter \p{TValueDescriptor}.
    using DescriptorType= TValueDescriptor;

    /// Type definition publishing the stored type of this container as defined with template
    /// parameter \p{TValueDescriptor}.
    using StoredType    = typename TValueDescriptor::StoredType;

    /// Type definition publishing the key type of this container as defined with template
    /// parameter \p{TValueDescriptor}.
    using KeyType       = typename TValueDescriptor::KeyType;

    /// Type definition publishing the map type of this container as defined with template
    /// parameter \p{TValueDescriptor}.
    using MappedType    = typename TValueDescriptor::MappedType;

    /// Type definition publishing template parameter \p{THash}.
    using HashType      = THash;

    /// Type definition publishing template parameter \p{TEqual}.
    using EqualType     = TEqual;

    /// Determines whether hash codes are stored with the elements.
    /// It is done in case the given template parameter \p{THashCashing} equals
    /// \alib{lang;Caching;Caching::Enabled} or if it equals \alib{lang;Caching;Caching::Auto}
    /// and the #KeyType type is an arithmetic type.
    /// @return \c true if hash codes are stored for reuse, \c false if not.
    static constexpr bool IsCachingHashes()                      { return base::IsCachingHashes(); }

    /// @return The enum element value of template parameter \p{TRecycling}.
    static constexpr Recycling RecyclingTag()                                 { return TRecycling; }

    /// This type definition may be used to define an externally managed shared recycler,
    /// which can be passed to the alternative constructor of this class when template
    /// parameter \p{TRecycling} equals \alib{containers;Recycling;Shared}.
    /// \see
    ///   Chapter \ref alib_contmono_containers_recycling_shared of the Programmer's Manual
    ///   for this \alibmod.
    using SharedRecyclerType=  typename base::SharedRecyclerType;


    /// Determines whether the used recycler type is in fact recycling elements.
    /// @return \c false if template parameter \p{TRecycling} equals
    ///         \alib{containers;Recycling;None}, \c true otherwise.
    static constexpr bool IsRecycling()                    { return recyclerType::IsRecycling(); }

    /// Denotes whether hash codes are cached or not.
    static constexpr bool CachedHashCodes = base::Element::CachedHashCodes;

    /// The mutable iterator exposed by this container.
    using Iterator              = typename base::template TIterator       <      StoredType>;

    /// The constant iterator exposed by this container.
    using ConstIterator         = typename base::template TIterator       <const StoredType>;

    /// The mutable iterator for a single bucket exposed by this container.
    using LocalIterator         = typename base::template TLocalIterator  <      StoredType>;

    /// The constant iterator for a single bucket exposed by this container.
    using ConstLocalIterator    = typename base::template TLocalIterator  <const StoredType>;

    /// A value of this type is returned with methods #Extract, which allows removing
    /// an element from the hashtable, without deleting its allocated storage and destructing its
    /// custom value.
    ///
    /// This handle allows writing access to the value of an extracted element.
    /// In combination with methods #Insert(ElementHandle&) and #InsertIfNotExistent(ElementHandle&),
    /// this supports changing parts of the element value, including the <em>key-portion</em> with
    /// proper re-insertion.
    ///
    /// Objects of this type cannot be copied, but just moved.
    class ElementHandle
    {
        #if !DOXYGEN
            friend class HashTable;
        #endif

        private:
            HashTable*  table;     ///< The table we belong to.
            Element*    element;   ///< The extracted element.

            /// Constructor setting fields #table and #element.
            /// @param pTable    The table we belong to.
            /// @param pElement  The extracted element.
            ElementHandle( HashTable*  pTable, Element* pElement )
            : table  ( pTable   )
            , element( pElement )
            {}

        public:
            /// Move constructor setting the moved object to emtpy.
            /// @param other The handle to move.
            ElementHandle( ElementHandle&& other )
            : table  ( other.table     )
            , element( other.element )
            {
                other.element= nullptr;
            }

            /// Default constructor creating and empty handle.
            ElementHandle()
            : element( nullptr )
            {}

            /// Deleted copy constructor.
            ElementHandle( ElementHandle& other )                        = delete;

            /// Deleted copy assignment operator.
            ElementHandle& operator=( const ElementHandle&  other )      = delete;

            /// Move assignment. Disposes any current content, and moves \p{other} into this.
            /// @param other The handle to move into this object.
            /// @return A reference to <c>this</c>.
            ElementHandle& operator=(       ElementHandle&& other )
            {
                if( element != nullptr )
                    table->recyclerType::Recycle(element);
                table  = other.table;
                element= other.element;
                other.element= nullptr;

                return *this;
            }

            /// Destructor. If this handle is not empty, the allocated storage of the
            /// represented element is added to the list of recyclable objects.
            ~ElementHandle()
            {
                if( element != nullptr )
                    table->recyclerType::Recycle(element);
            }

            /// Determines if this is a "valid" handle.
            /// @return Returns \c true if this objects represents a valid element, \c false
            ///        otherwise.
            bool        IsEmpty()       const   { return element == nullptr;    }

            /// Returns a mutable reference to this element's data.
            /// Must not be invoked on empty instances.
            /// @return Returns a mutable reference to value of the represented element.
            StoredType& Value  ()       const   { return element->value;    }

            /// Returns a mutable reference to the <em>key-portion</em> of this element's data.
            /// Must not be invoked on empty instances.
            /// @return Returns a mutable reference to the <em>key-portion</em> of the represented
            ///         element.
            KeyType&    Key    ()       const   { return TValueDescriptor().Key( element->value ); }

            /// Returns a mutable reference to the <em>mapped-portion</em> of this element's data.
            /// Must not be invoked on empty instances.
            /// @return Returns a mutable reference to the mapped object.
            MappedType& Mapped ()       const   { return TValueDescriptor().Mapped( element->value );   }

    }; // class ElementHandle

    //##############################################################################################
    // Construction/Destruction And Allocator Access
    //##############################################################################################

    //==============================================================================================
    /// Constructor.
    /// \note
    ///   This constructor is not available if the template argument \p{TRecycling} equals
    ///   \alib{containers;Recycling;Shared}.
    ///
    /// @tparam TRequires        Defaulted template parameter. Must not be specified.
    /// @param  pAllocator       The allocator to use.
    /// @param  pBaseLoadFactor  The base load factor. Defaults to <c>1.0</c>.
    /// @param  pMaxLoadFactor   The maximum load factor. Defaults to <c>2.0</c>.
    //==============================================================================================
    template<bool TRequires= TRecycling!= Recycling::Shared>
    requires TRequires
    explicit
    HashTable( AllocatorType&   pAllocator,
               float            pBaseLoadFactor = 1.0,
               float            pMaxLoadFactor  = 2.0  )
    : base( pAllocator, pBaseLoadFactor, pMaxLoadFactor  )
    #if ALIB_DEBUG_CRITICAL_SECTIONS
    , dcs("HashTable")
    #endif
    {}

    //==============================================================================================
    /// Constructor.
    /// \note
    ///   This constructor is not available if the template argument \p{TRecycling} equals
    ///   \alib{containers;Recycling;Shared} and the
    ///   if template argument \p{TAllocator} does not equal \alib{lang;HeapAllocator}
    ///
    ///
    /// @tparam TRequires        Defaulted template parameter. Must not be specified.
    /// @param  pBaseLoadFactor  The base load factor. Defaults to <c>1.0</c>.
    /// @param  pMaxLoadFactor   The maximum load factor. Defaults to <c>2.0</c>.
    //==============================================================================================
    template<bool TRequires= TRecycling!= Recycling::Shared>
    requires TRequires
    explicit
    HashTable( float            pBaseLoadFactor = 1.0,
               float            pMaxLoadFactor  = 2.0  )
    : base( pBaseLoadFactor, pMaxLoadFactor  )
    #if ALIB_DEBUG_CRITICAL_SECTIONS
    , dcs("HashTable")
    #endif
    {}


    //==============================================================================================
    /// Constructor taking a shared recycler.
    /// @param pAllocator       The allocator to use.
    /// @param pSharedRecycler  The shared recycler.
    /// @param pBaseLoadFactor  The base load factor. See method #BaseLoadFactor for details.
    ///                         Defaults to <c>1.0</c>.
    /// @param pMaxLoadFactor   The maximum load factor. See method #MaxLoadFactor for details.
    ///                         Defaults to <c>2.0</c>.
    /// @tparam TSharedRecycler  Used to select this constructor. Deduced by the compiler.
    //==============================================================================================
    template<typename TSharedRecycler= SharedRecyclerType>
    requires (!std::same_as<TSharedRecycler, void>)
    HashTable( AllocatorType&    pAllocator,
               TSharedRecycler&  pSharedRecycler,
               float             pBaseLoadFactor = 1.0,
               float             pMaxLoadFactor  = 2.0  )
    : base( pAllocator, pSharedRecycler, pBaseLoadFactor, pMaxLoadFactor  )
    #if ALIB_DEBUG_CRITICAL_SECTIONS
    , dcs("HashTable")
    #endif
    {}

    //==============================================================================================
    /// Constructor taking a shared recycler.
    /// @param pSharedRecycler  The shared recycler.
    /// @param pBaseLoadFactor  The base load factor. See method #BaseLoadFactor for details.
    ///                         Defaults to <c>1.0</c>.
    /// @param pMaxLoadFactor   The maximum load factor. See method #MaxLoadFactor for details.
    ///                         Defaults to <c>2.0</c>.
    /// @tparam TSharedRecycler  Used to select this constructor. Deduced by the compiler.
    //==============================================================================================
    template<typename TSharedRecycler= SharedRecyclerType>
    requires (!std::same_as<TSharedRecycler, void>)
    HashTable( TSharedRecycler&  pSharedRecycler,
               float             pBaseLoadFactor = 1.0,
               float             pMaxLoadFactor  = 2.0  )
    : base( pSharedRecycler, pBaseLoadFactor, pMaxLoadFactor  )
    #if ALIB_DEBUG_CRITICAL_SECTIONS
    , dcs("HashTable")
    #endif
    {}

    //==============================================================================================
    /// Returns the allocator of this object. Usually the allocator might be used to perform
    /// allocations in respect to data found in objects stored in this object.
    /// However, such allowance is dependent on the use case and not part of this class's
    /// contract.
    ///
    /// @return The allocator that was provided in the constructor.
    //==============================================================================================
    AllocatorType&  GetAllocator()                                                          noexcept
    { return base::base::GetAllocator();  }


    //##############################################################################################
    /// @name Size and Capacity
    //##############################################################################################

    //==============================================================================================
    /// Destructs and removes all elements from this hash table. The allocated space
    /// of the elements will be preserved and "recycled" with future insertions.
    //==============================================================================================
    void            Clear()                                 { DCS base::clear(); }

    //==============================================================================================
    /// Same as clear, but does not recycle internal nodes. Furthermore, all recyclables
    /// are deleted. The latter is done only if template parameter \p{TRecycling} is not
    /// \alib{containers;Recycling;Shared}. In this case, the elements are still recycled.
    ///
    /// This method is useful with monotonic allocators, that can be reset as well, after
    /// this instance is reset.
    /// But, because the life-cycle of the monotonic allocator(s) used for insertions is not
    /// under control of this object, it is the obligation of the caller to ensure that the
    /// monotonic allocator is kept in sync with this object.
    /// The following recipe shows a valid use:
    /// - Construct a \b HashTable passing a \b MonoAllocator.
    /// - Create a snapshot of the \b MonoAllocator.
    /// - Use the \b HashTable.
    /// - Reset the \b HashTable and right afterwards the \b MonoAllocator to the snapeshot taken.
    //==============================================================================================
    void            Reset()
    {
        recyclerType oldRecycler(*this);
        auto   baseLoadFactor = base::baseLoadFactor;
        auto   maxLoadFactor  = base::maxLoadFactor ;
        auto&  allocator      = base::base::GetAllocator();
        lang::Destruct(*this);
        if constexpr ( std::same_as< typename base::recyclerType,
                                     detail::RecyclerShared<TAllocator, Element>> )
            new (this) HashTable( oldRecycler, baseLoadFactor, maxLoadFactor );
        else
            new (this) HashTable( allocator, baseLoadFactor, maxLoadFactor );
    }

    /// Returns the number of stored elements. Note that this method runs in constant time, as
    /// the number of elements is kept counted during operation.
    /// @return The number of elements stored in the hash table.
    integer         Size()                                                            const noexcept
    { return base::size; }

    //==============================================================================================
    /// Invokes #Size and compares result with \c 0.
    /// @return \c true if this list is empty, \c false otherwise.
    //==============================================================================================
    bool            IsEmpty()                                                         const noexcept
    { return base::size == 0; }

    //==============================================================================================
    /// Reserves space for at least the given number of elements.
    /// This might re-hash this table.
    /// \see Method #ReserveRecyclables.
    /// @param qty       The expected number or increase of elements to be stored in the hash table.
    /// @param reference Denotes whether \p{expected} is meant as an absolute size or an increase.
    //==============================================================================================
    void            Reserve( integer qty, lang::ValueReference reference )
    {DCS
        float expectedSize= float(  qty +  (reference == lang::ValueReference::Relative ? Size()
                                                                                        : 0     ) );
        return base::rehash( uinteger(std::ceil(  expectedSize / base::baseLoadFactor)) );
    }

    //==============================================================================================
    /// Same as #Reserve but in addition also already allocates the required space for the number
    /// of additional elements expected.
    ///
    /// @see Chapter \ref alib_contmono_containers_recycling_reserving of the Programmer's
    ///      Manual.
    ///
    /// @param qty       The expected resulting number (or increase) of elements to be stored in
    ///                  this container.
    /// @param reference Denotes whether \p{qty} is meant as an absolute size or an
    ///                  increase.
    //==============================================================================================
    void            ReserveRecyclables( integer qty, lang::ValueReference reference )
    {
        Reserve( qty, reference );
        {DCS
            auto requiredRecyclables= ( qty - (reference == lang::ValueReference::Absolute ? Size()
                                                                                           : 0  ) )
                                            - base::recyclerType::Count();
            if( requiredRecyclables > 0 )
                recyclerType::Reserve( requiredRecyclables );
        }
    }

    //==============================================================================================
    /// Counts the number of currently allocated but unused (not contained) element nodes
    /// that will be recycled with upcoming insertions.
    ///
    /// \note
    ///   This method is provided for completeness and unit-testing. It should not be of
    ///   relevance for common usage.<br>
    ///   Furthermore, this method is not available (aka does not compile) with instantiations
    ///   that specify template parameter \p{TRecycling} as \alib{containers;Recycling;None}.
    ///
    /// @return The number of removed and not yet recycled elements.
    //==============================================================================================
    integer         RecyclablesCount()                                                const noexcept
    {DCSSHRD   return base::recyclerType::Count(); }

    //##############################################################################################
    /// @name Hash Policy
    //##############################################################################################

    //==============================================================================================
    /// Sets a new value for the "base load factor" used with this container.
    /// The base load factor determines the minimum number of buckets
    /// when re-hashing is performed.
    ///
    /// The formula to determine the minimum number of buckets is #Size divided by this factor.
    /// A static table of prime numbers is searched for the next higher number and this value
    /// is then used to determine the number of buckets.
    ///
    /// The default value for this factor is defined as <c>1.0</c> by the default value
    /// of parameter \p{pBaseLoadFactor} of the constructor.
    ///
    /// \note
    ///   Invoking this method never triggers rehashing.
    ///
    /// \see
    ///   Overloaded method #BaseLoadFactor() and this class's documentation section
    ///   \ref alib_ns_containers_hashtable_rehashing "4. Re-Hashing".
    ///
    /// @param newBaseLoadFactor The new base load factor to use when a rehash is performed.
    //==============================================================================================
    void            BaseLoadFactor( float newBaseLoadFactor )                               noexcept
    { base::baseLoadFactor= newBaseLoadFactor; }

    //==============================================================================================
    /// Returns the actual base load factor.
    ///
    /// \see
    ///   Overloaded method #BaseLoadFactor(float) and this class's documentation section
    ///   \ref alib_ns_containers_hashtable_rehashing "4. Re-Hashing".
    ///
    /// @return The current value of the base load factor.
    //==============================================================================================
    float           BaseLoadFactor()                                                  const noexcept
    { return base::baseLoadFactor; }


    //==============================================================================================
    /// Sets a new value for the "maximum load factor" which is the average number of elements
    /// per bucket.
    ///
    /// The default value for this factor is defined as <c>2.0</c> by the default value
    /// of parameter \p{pMaxLoadFactor} of the constructor.
    ///
    /// \note
    ///   Invoking this method triggers rehashing, in case the hash table is not empty and
    ///   the new maximum load factor is below the current load factor of this container, which
    ///   equals #Size divided by #BucketCount.<br>
    ///   This method may be used to temporarily disable re-hashing by providing
    ///   <c>std::numeric_limits<float>::max()</c>.
    ///
    /// \see
    ///   Overloaded method #MaxLoadFactor() and this class's documentation section
    ///   \ref alib_ns_containers_hashtable_rehashing "4. Re-Hashing".
    ///
    /// @param newMaxLoadFactor The maximum load factor used to determine the need of re-hashing.
    //==============================================================================================
    void            MaxLoadFactor( float newMaxLoadFactor )                                 noexcept
    { base::setMaxLoadFactor( newMaxLoadFactor ); }

    //==============================================================================================
    /// Returns the actual maximum load factor.
    ///
    /// \see
    ///   Overloaded method #MaxLoadFactor(float) and this class's documentation section
    ///   \ref alib_ns_containers_hashtable_rehashing "4. Re-Hashing".
    ///
    /// @return The current value of the maximum load factor.
    //==============================================================================================
    float          MaxLoadFactor()                                                    const noexcept
    { return base::maxLoadFactor; }

    //##############################################################################################
    /// @name Bucket Interface
    //##############################################################################################

    //==============================================================================================
    /// Returns the number of "buckets" that this hash table currently uses.
    ///
    /// @return The size of the array of hash buckets.
    //==============================================================================================
    uinteger        BucketCount()                                                     const noexcept
    { return base::bucketCount; }

    //==============================================================================================
    /// Returns the number of entries stored in the bucket with the given number.
    ///
    /// @param bucketNumber  The number of the bucket to receive the size for.
    /// @return The number of entries in the specified bucket.
    //==============================================================================================
    uinteger        BucketSize( uinteger bucketNumber )                               const noexcept
    {DCSSHRD
        ALIB_ASSERT_ERROR(  bucketNumber <  base::bucketCount, "MONOMEM/HASHTABLE",
                    "Bucket number out of range. {}>={}", bucketNumber, base::bucketCount )
        return uinteger(base::buckets[bucketNumber].count());
    }

    //==============================================================================================
    /// Returns the number of the bucket corresponding to \p{key}.
    ///
    /// @param key  The key to evaluate the bucket number for.
    /// @return The bucket number that \p{key} is assigned to.
    //==============================================================================================
    uinteger        BucketNumber( const KeyType& key )                                const noexcept
    { return THash{}(key) % base::bucketCount; }

    //##############################################################################################
    /// @name Element Insertion
    //##############################################################################################
    //==============================================================================================
    /// See #Insert(StoredType&&) which is invoked with a copy of \p{value}.
    ///
    /// @param  value   A value to copy and insert.
    /// @return An iterator referring to the element added.
    //==============================================================================================
    Iterator        Insert( const StoredType& value)
    { return Insert( value, THash{}( TValueDescriptor().Key( reinterpret_cast<StoredType&>(value)) ) ); }

    //==============================================================================================
    /// Overloaded version of method \alib{containers::HashTable;Insert(const StoredType&)} which
    /// accepts the \p{hashCode} of the given \p{value} as a second parameter.
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// @param  value    A value to copy and insert.
    /// @param  hashCode Pre-calculated hash code of \p{value}.
    /// @return An iterator referring to the element added.
    //==============================================================================================
    Iterator        Insert( const StoredType& value, size_t hashCode )
    { return Insert(value, hashCode ); }

    //==============================================================================================
    /// Moves the given value into this table.<br>
    /// Existing iterators remain valid.
    ///
    /// \note
    ///   The use of this method may insert elements sharing the same key as already existing
    ///   elements.
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    ///
    /// @param  value   A rvalue reference of contained type \p{StoredType} to insert.
    /// @return An iterator referring to the element added.
    //==============================================================================================
    Iterator        Insert( StoredType&& value )
    {
        auto   hashCode = THash{}( TValueDescriptor().Key( reinterpret_cast<StoredType&>(value)) );
        return Insert( std::move(value), hashCode );
    }

    //==============================================================================================
    /// Overloaded version of method \alib{containers::HashTable;Insert(StoredType&&)} which
    /// accepts the \p{hashCode} of the given \p{value} as a second parameter.
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// @param  value    An rvalue reference of contained type \p{StoredType} to insert.
    /// @param  hashCode Pre-calculated hash code of \p{value}.
    /// @return An iterator referring to the element added.
    //==============================================================================================
    Iterator        Insert( StoredType&& value, size_t hashCode )
    {DCS
        // Recycle node or create a new one
        Element* element= base::allocElement(hashCode);

        // placement-new
        new ( &element->value )   StoredType ( std::move(value) );

        // insert to hash table
        base::increaseSize( 1 );
        auto bucketIdx=  base::insertInBucket( element, hashCode );
        return Iterator( this, bucketIdx, element);
    }

    //==============================================================================================
    /// Inserts the element contained in the given \alib{containers::HashTable;ElementHandle}
    /// into the hash table.
    ///
    /// \note
    ///   The use of this method may insert elements sharing the same key as already existing
    ///   elements.
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    ///
    /// <p>
    ///   Objects of type \b ElementHandle objects may be received using overloaded methods
    ///   #Extract. The combination of \b %Extract and this method (as well as method
    ///   #InsertIfNotExistent(ElementHandle&) are the only way to change the <em>key-portion</em> of an
    ///   element without element destruction/re-construction.
    ///
    /// @param  handle   A reference to a handle to an element,  previously received with #Extract.
    /// @return On success, returns an iterator that refers to the inserted element.
    ///         On failure (if parameter \p{handle} was empty), the returned iterator equals #end.
    //==============================================================================================
    Iterator        Insert( ElementHandle& handle )
    {DCS
        if( handle.IsEmpty() )
            return end();

        base::increaseSize( 1 );
        Element* element= handle.element;
        auto hashCode= THash{}( TValueDescriptor().Key( element->value ));
        element->fixHashCode( hashCode );
        auto bucketIdx=  base::insertInBucket(  element, hashCode );
        handle.element= nullptr;
        return Iterator( this, bucketIdx, element );
    }

    //==============================================================================================
    /// See #InsertUnique(StoredType&&) which is invoked with a copy of \p{value}.
    ///
    /// @param  value   An element to insert whose <em>key-portion</em> has to be different to
    ///                 all currently contained elements.
    /// @return An iterator referring to the element added.
    //==============================================================================================
    Iterator        InsertUnique( const StoredType& value )
    { return InsertUnique(std::move(StoredType( value )) ); }

    //==============================================================================================
    /// Overloaded version of method
    /// \alib{containers::HashTable;InsertUnique(const StoredType&)} which accepts the
    /// \p{hashCode} of the given \p{key} as a second parameter.
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// @param  value    An element to insert whose <em>key-portion</em> has to be different to
    ///                  all currently contained elements.
    /// @param  hashCode Pre-calculated hash code of \p{value}.
    /// @return An iterator referring to the element added.
    //==============================================================================================
    Iterator        InsertUnique( const StoredType& value, size_t hashCode )
    { return InsertUnique( StoredType( value ), hashCode ); }

    //==============================================================================================
    /// Moves the given value into this table without checking to place it in the right
    /// position in respect to existing elements with the same <em>key-portion</em>.
    ///
    /// \attention
    ///   This method must only be used if the caller guarantees that no other element is
    ///   currently stored in this container having an equal <em>key-portion</em>.
    ///   In such situations, this method is very efficient.<br>
    ///   If one exists already, this hash table is not considered in a consistent state
    ///   after the operation. I.e., method #EqualRange will discontinue to function properly.
    ///
    /// \attention
    ///   In debug-compilations, an \alib assertion is raised, if an equal element exists.
    ///   For this reason, performance differences to method #Insert will be seen only with
    ///   release-compilations.
    ///
    /// \attention
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    ///
    ///
    /// @param  value   An element to insert whose <em>key-portion</em> has to be different to
    ///                 all currently contained elements.
    /// @return An iterator referring to the element added.
    //==============================================================================================
    Iterator        InsertUnique(StoredType&& value)
    {
        auto   hashCode = THash{}( TValueDescriptor().Key( reinterpret_cast<StoredType&>(value)) );
        return InsertUnique(std::move(value), hashCode );
    }

    //==============================================================================================
    /// Overloaded version of method \alib{containers::HashTable;InsertUnique(StoredType&&)}
    /// which accepts the \p{hashCode} of the given \p{key} as a second parameter.
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// @param  value    An element to insert whose <em>key-portion</em> has to be different to
    ///                  all currently contained elements.
    /// @param  hashCode Pre-calculated hash code of \p{value}.
    /// @return An iterator referring to the element added.
    //==============================================================================================
    Iterator        InsertUnique( StoredType&& value, size_t hashCode )
    {DCS
        Element* element  = base::allocElement( hashCode );

        base::increaseSize( 1 );
        auto bucketIdx= hashCode % base::bucketCount;
        base::buckets[bucketIdx].pushFront( element );

        new ( &element->value )   StoredType( std::move(value) );

        #if ALIB_DEBUG
            //Check that this was the first of
            auto it= ConstLocalIterator( bucketIdx, base::buckets[bucketIdx].first() ); // cbegin(bucketIdx);
            ALIB_ASSERT( it.element == element, "MONOMEM/HASHTABLE" ) // has to be the first inserted
            while( ++it != cend(bucketIdx) )
            {
                ALIB_ASSERT_ERROR( !base::areEqual(element, it.element ), "MONOMEM/HASHTABLE",
                     "InsertUnique used while element with same key-portion existed!" )
            }
        #endif

        return Iterator( this, bucketIdx, element);
    }


    //==============================================================================================
    /// See #InsertOrAssign(const KeyType&, MappedType&&) which is invoked with a copy of \p{mapped}.
    ///
    /// \par Availability
    ///   This method is only available with
    ///   \ref alib_ns_containers_hashtable_setandmap "hash map mode".
    ///
    /// @tparam  TRequires Used to disable this method for instantiations of this
    ///                    template type with <em>hash set mode</em>.<br>
    ///                    Defaulted and must not be specified with invocations.
    /// @param   key       The key to use for search and insertion.
    /// @param   mapped    The mapped value to copy and then insert or assign.
    /// @return A pair containing an iterator referencing the element added.
    ///         The bool component is \c true if the insertion took place and \c false if the
    ///         assignment took place.
    //==============================================================================================
    template<typename TRequires= MappedType>
    requires(!std::same_as<TRequires, StoredType>)
    std::pair<Iterator, bool> InsertOrAssign( const KeyType& key, const MappedType& mapped)
    { return InsertOrAssign( key, MappedType(mapped) );  }

    //==============================================================================================
    /// Replaces an existing, respectively inserts a new element into this hash table.
    ///
    /// \note
    ///   This method allows preventing the insertion of double entries.
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    ///
    /// \par Availability
    ///   This method is only available with
    ///   \ref alib_ns_containers_hashtable_setandmap "hash map mode".
    ///
    /// @tparam TRequires Used to disable this method for instantiations of this
    ///                   template type with <em>hash set mode</em>.<br>
    ///                   Defaulted and must not be specified with invocations.
    /// @param  key       The key to use for search and insertion.
    /// @param  mapped    The mapped value to insert or assign.
    /// @return A pair containing an iterator referring to the element added.
    ///         The bool component is \c true if the insertion took place and \c false if the
    ///         assignment took place.
    //==============================================================================================
    template<typename TRequires= MappedType>
    requires(!std::same_as<TRequires, StoredType>)
    std::pair<Iterator, bool>  InsertOrAssign( const KeyType& key, MappedType&& mapped )
    { return InsertOrAssign( key, std::move(mapped), THash{}(key) ); }

    //==============================================================================================
    /// Overloaded version of method
    /// \alib{containers::HashTable;InsertOrAssign(const KeyType&; MappedType&&)} which
    /// accepts the \p{hashCode} of the given \p{key} as a third  parameter.
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// \par Availability
    ///   This method is only available with
    ///   \ref alib_ns_containers_hashtable_setandmap "hash map mode".
    ///
    /// @tparam TRequires Used to disable this method for instantiations of this
    ///                   template type with <em>hash set mode</em>.<br>
    ///                    Defaulted and must not be specified with invocations.
    /// @param  key       The key to use for search and insertion.
    /// @param  mapped    The mapped value to insert or assign.
    /// @param  hashCode  Pre-calculated hash code of \p{key}.
    /// @return A pair containing an iterator referring to the element added.
    ///         The bool component is \c true if the insertion took place and \c false if the
    ///         assignment took place.
    //==============================================================================================
    template<typename TRequires= MappedType>
    requires(!std::same_as<TRequires, StoredType>)
    std::pair<Iterator, bool> InsertOrAssign( const KeyType& key, MappedType&& mapped, size_t hashCode )
    {DCS
        std::pair<Iterator, bool> result= base::insertOrGet( key, hashCode );

        // if an existing element was found, we have to destruct the mapped value
        if( result.second == false )
            lang::Destruct( TValueDescriptor().Mapped( result.first.element->value ) );

        // if otherwise a new element was inserted, we have to copy the key
        else
            new (&TValueDescriptor().Key( result.first.element->value ))  KeyType( key );

        // placement-new for the mapped object
        new ( &TValueDescriptor().Mapped( result.first.element->value ) )   MappedType( std::move( mapped) );

        return result;
    }

    //==============================================================================================
    /// See #InsertIfNotExistent(const KeyType&, MappedType&&) which is invoked with a copy of
    /// \p{mapped}.
    ///
    /// \par Availability
    ///   This method is only available with
    ///   \ref alib_ns_containers_hashtable_setandmap "hash map mode".
    ///
    /// @tparam TRequires Used to disable this method for instantiations of this
    ///                   template type with <em>hash set mode</em>.<br>
    ///                   Defaulted and must not be specified with invocations.
    /// @param  key       The key to use for search and insertion.
    /// @param  mapped    The mapped object to copy and insert if \p{key} is not existing.
    /// @return A pair containing an iterator referencing either the element found or the new
    ///         element added. The bool component is \c true if the insertion took place and \c false
    ///         if nothing was changed.
    //==============================================================================================
    template<typename TRequires= MappedType>
    requires(!std::same_as<TRequires, StoredType>)
    std::pair<Iterator, bool> InsertIfNotExistent( const KeyType& key, const MappedType& mapped)
    { return InsertIfNotExistent( key, MappedType(mapped) );  }

    //==============================================================================================
    /// Overloaded version of method
    /// \alib{containers::HashTable;InsertIfNotExistent(const KeyType&,MappedType&&)} which
    /// accepts the \p{hashCode} of the given \p{key} as a third parameter.
    ///
    /// \par Availability
    ///   This method is only available with
    ///   \ref alib_ns_containers_hashtable_setandmap "hash map mode".
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// @tparam TRequires Used to disable this method for instantiations of this
    ///                   template type with <em>hash set mode</em>.<br>
    ///                   Defaulted and must not be specified with invocations.
    /// @param   key      The key to use for search and insertion.
    /// @param   hashCode Pre-calculated hash code of \p{key}.
    /// @param   mapped   The mapped value to insert if \p{key} is not existing.
    /// @return A pair containing an iterator referencing either the element found or the new
    ///         element added. The bool component is \c true if the insertion took place and \c false
    ///         if nothing was changed.
    //==============================================================================================
    template<typename TRequires= MappedType>
    requires(!std::same_as<TRequires, StoredType>)
    std::pair<Iterator, bool> InsertIfNotExistent( const KeyType& key, MappedType&& mapped, size_t hashCode)
    {DCS
        // search element
        std::pair<Iterator, bool> result= base::insertIfNotExists( key, hashCode );

        // existed? Do nothing
        if( result.second == false )
            return result;

        // placement-new for the key (using copy constructor)
        new (&TValueDescriptor().Key( result.first.element->value ))   KeyType( key );

        // placement-new for the mapped (using move constructor)
        new ( &TValueDescriptor().Mapped( result.first.element->value ) )   MappedType( std::move( mapped) );

        return result;
    }

    //==============================================================================================
    /// Inserts a new mapped object only if no other object is contained that is associated
    /// already with the same key as given \p{key}.
    ///
    /// \note
    ///   This method allows preventing the insertion of double entries.
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    ///
    /// \par Availability
    ///   This method is only available with
    ///   \ref alib_ns_containers_hashtable_setandmap "hash map mode".
    ///
    /// @tparam TRequires Used to disable this method for instantiations of this
    ///                   template type with <em>hash set mode</em>.<br>
    ///                   Defaulted and must not be specified with invocations.
    /// @param   key      The key to use for search and insertion.
    /// @param   mapped   The mapped value to insert if \p{key} is not existing.
    /// @return A pair containing an iterator referencing either the element found or the new
    ///         element added. The bool component is \c true if the insertion took place and \c false
    ///         if nothing was changed.
    //==============================================================================================
    template<typename TRequires= MappedType>
    requires(!std::same_as<TRequires, StoredType>)
    std::pair<Iterator, bool> InsertIfNotExistent( const KeyType& key, MappedType&& mapped)
    { return InsertIfNotExistent( key, std::move(mapped), THash{}(key) ); }

    //==============================================================================================
    /// See #InsertIfNotExistent(StoredType&&) which is invoked with a copy of \p{value}.
    ///
    /// @param  value    The value to copy and insert.
    /// @return A pair containing an iterator referencing either the element found or the new
    ///         element added. The bool component is \c true if the insertion took place and \c false
    ///         if nothing was changed.
    //==============================================================================================
    std::pair<Iterator, bool>    InsertIfNotExistent( const StoredType& value )
    { return InsertIfNotExistent( StoredType(value) ); }

    //==============================================================================================
    /// Inserts a new mapped object only if no other object is contained that is associated
    /// already with the same key as given \p{key}.
    ///
    /// \note
    ///   This method allows preventing the insertion of double entries.
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    ///
    /// @param  value   A rvalue reference of a \p{StoredType} to insert.
    /// @return A pair containing an iterator referencing either the element found or the new
    ///         element added.
    ///         The bool component is \c true if the insertion took place and \c false if nothing
    ///         was changed.
    //==============================================================================================
    std::pair<Iterator, bool>    InsertIfNotExistent( StoredType&& value )
    {
        auto hashCode=  THash{}( TValueDescriptor().Key(value) );
        return InsertIfNotExistent(std::move(value), hashCode);
    }

    //==============================================================================================
    /// Overloaded version of method
    /// \alib{containers::HashTable;InsertIfNotExistent(StoredType&&)} which accepts the
    /// \p{hashCode} of the given \p{key} as a second parameter.
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// @param  value    A rvalue reference of a \p{StoredType} to insert.
    /// @param  hashCode Pre-calculated hash code of \p{value}.
    /// @return A pair containing an iterator referencing either the element found or the new
    ///         element added.
    ///         The bool component is \c true if the insertion took place and \c false if nothing
    ///         was changed.
    //==============================================================================================
    std::pair<Iterator, bool>    InsertIfNotExistent( StoredType&& value, size_t hashCode )
    {DCS
        // search element
        std::pair<Iterator, bool> result= base::insertIfNotExists( TValueDescriptor().Key(value), hashCode );

        // existed? Do nothing
        if( result.second == false )
            return result;

        // placement-new for the value(using move constructor)
        new ( &result.first.element->value )   StoredType( std::move(value) );

        return result;
    }

    //==============================================================================================
    /// Inserts the element contained in the given \alib{containers::HashTable;ElementHandle} into
    /// this table, if no equal element exists. In the unsuccessful case, the given
    /// \b %ElementHandle remains set and can be reused.<br>
    ///
    /// Existing iterators remain valid.
    ///
    /// \note
    ///   This method allows preventing the insertion of double entries.
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    ///
    /// <p>
    /// \see
    ///   Objects of type \b ElementHandle objects may be received using overloaded methods
    ///   #Extract. The combination of \b %Extract and this method (as well as method
    ///   #Insert(ElementHandle&) are the only way to change the <em>key-portion</em> of an element
    ///   without element destruction/re-construction.
    ///
    /// @param  handle   A reference to a handle to an element,  previously received with #Extract.
    /// @return If an empty handle is given, #end is returned.
    ///         Otherwise, if no equal element existed, an iterator that refers to the inserted
    ///         element is returned and the given \p{handle} is emptied.<br>
    ///         If an equal element existed, the returned iterator refers to the existing element
    ///         and the \p{handle} remains set (not empty).
    //==============================================================================================
    Iterator    InsertIfNotExistent( ElementHandle& handle )
    {DCS
        if( handle.IsEmpty() )
            return Iterator( this, base::bucketCount, nullptr ); //end();

        Element* element  = handle.element;
        auto  hashCode = THash{}( TValueDescriptor().Key( handle.element->value ) );
        auto  bucketIdx= hashCode % base::bucketCount;

        Element* existing= base::findElement( hashCode, TValueDescriptor().Key( element->value ), hashCode );
        if ( existing != nullptr )
            return Iterator( this, bucketIdx, existing );

        handle.element= nullptr;
        element->fixHashCode( hashCode ); // the key might have been changed outside

        bucketIdx= base::increaseSize( 1, hashCode );
        base::buckets[bucketIdx].pushFront( element );
        return Iterator( this, bucketIdx, element);
    }

    //==============================================================================================
    /// Constructs a new element within this container.
    ///
    /// \note
    ///   The use of this method may insert elements sharing the same key as already existing
    ///   elements.
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    ///
    /// @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
    /// @param  args   Variadic parameters to be forwarded to the constructor of the inserted
    ///                instance of type #StoredType.
    /// @return An iterator referring to the element added.
    //==============================================================================================
    template<typename... TArgs>
    Iterator        Emplace( TArgs&&... args )
    {DCS
        // Recycle node or create a new one
        Element* element= base::allocElement( 0 );

        // placement-new
        new ( &element->value )  StoredType( std::forward<TArgs>(args)... );

        // fix the hash code which was not available at allocation, yet.
        auto hashCode= THash{}( TValueDescriptor().Key( element->value ));
        element->fixHashCode( hashCode );

        // insert to hash table
        base::increaseSize( 1 );
        auto bucketIdx=  base::insertInBucket( element, hashCode );
        return Iterator( this, bucketIdx, element);
    }

    //==============================================================================================
    /// Constructs a value within this container without checking to place it in the right
    /// position in respect to existing elements with the same <em>key-portion</em>.
    ///
    /// \attention
    ///   This method must only be used if the caller guarantees that no other element is
    ///   currently stored in this container having an equal <em>key-portion</em>.
    ///   In such situations, this method is very efficient.<br>
    ///   If one exists already, this hash table is not considered in a consistent state
    ///   after the operation. I.e., method #EqualRange will discontinue to function properly.
    ///
    /// \attention
    ///   In debug-compilations, an \alib assertion is raised, if an equal element exists.
    ///   For this reason, performance differences to method #Insert will be seen only with
    ///   release-compilations.
    ///
    /// \attention
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    ///
    /// @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
    /// @param  args   Variadic parameters to be forwarded to the constructor of the
    ///                element to insert whose <em>key-portion</em> has to be different to
    ///                all currently contained elements.
    /// @return An iterator referring to the element added.
    //==============================================================================================
    template<typename... TArgs>
    Iterator        EmplaceUnique( TArgs&&... args )
    {DCS
        // Recycle node or create a new one
        Element* element= base::allocElement(0);

        // placement-new
        new ( &element->value )  StoredType( std::forward<TArgs>(args)... );

        // replace hash code (which is available only now)
        auto hashCode= THash{}( TValueDescriptor().Key( element->value ));
        element->fixHashCode( hashCode );


        // insert to hash table
        auto bucketIdx= base::increaseSize( 1, hashCode );
        base::buckets[bucketIdx].pushFront( element );
        auto result= Iterator( this, bucketIdx, element);

        #if ALIB_DEBUG
            // Check that this was the first of
            auto it=  ConstLocalIterator( result.bucketIdx, base::buckets[result.bucketIdx].first() ); // cbegin();
            ALIB_ASSERT( it.element == result.element, "MONOMEM/HASHTABLE" ) // has to be the first inserted
            while( ++it != ConstLocalIterator( result.bucketIdx, nullptr ) ) //cend(result.bucketIdx)
               ALIB_ASSERT_ERROR( !base::areEqual(result.element, it.element ), "MONOMEM/HASHTABLE",
                  "EmplaceUnique used while element with same key-portion existed!" )
        #endif

        return result;
    }

#if DOXYGEN
    //==============================================================================================
    /// Replaces an existing, respectively constructs a new element within this container.
    ///
    /// \note
    ///   This method allows preventing the insertion of double entries.
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    ///
    /// \par Availability
    ///   This method is available if this templated type is instantiated with
    ///   \ref alib_ns_containers_hashtable_setandmap "hash map mode"
    ///   or for hash sets that only define a subset of \p{StoredType} as a key type \p{TKey} and
    ///   whose stored type \p{StoredType} is constructible if given a key value and the variadic
    ///   template arguments.
    ///
    ///
    /// @tparam TRequires Used to disable this method where not available.<br>
    ///                   Defaulted and must not be specified with invocations.
    /// @tparam TArgs     Types of variadic parameters given with parameter \p{args}.
    /// @param  key       The key to use for search and insertion.
    /// @param  args      Variadic parameters to be forwarded to the constructor of the mapped object
    ///                   of type <c>\p{MappedType}</c>.
    /// @return A pair containing an iterator referring to the element added.
    ///         The bool component is \c true if the insertion took place and \c false if the
    ///         assignment took place.
    //==============================================================================================
    template<typename TRequires= MappedType, typename... TArgs>
    requires(!std::same_as<TRequires, StoredType>)
    std::pair<Iterator, bool> EmplaceOrAssign( const KeyType& key, TArgs&&... args);
#else
    template<typename TRequires= MappedType, typename... TArgs>
    requires(!std::same_as<TRequires, StoredType>)
    std::pair<Iterator, bool> EmplaceOrAssign( const KeyType& key, TArgs&&... args)
    {DCS
        // insert to hash table
        std::pair<Iterator, bool> result= base::insertOrGet( key, THash{}(key) );

        // if an existing element was found, we have to destruct the currently mapped object
        if( result.second == false )
            lang::Destruct( TValueDescriptor().Mapped( result.first.element->value )  );

        // if otherwise an insertion took place, we have to copy the key
        else
            new (&TValueDescriptor().Key( result.first.element->value ))  KeyType( key );

        // placement-new for the mapped object
        new ( &TValueDescriptor().Mapped( result.first.element->value ))  MappedType( std::forward<TArgs>( args)... );

        return result;
    }

    // set mode
    template<typename TRequires= MappedType, typename... TArgs>
    requires(    std::same_as<TRequires, StoredType>
              && std::is_constructible< StoredType,
                                        const KeyType&,
                                        TArgs&&...     >::value )
    std::pair<Iterator, bool> EmplaceOrAssign( const KeyType& key, TArgs&&... args)
    {DCS
        // insert to hash table
        std::pair<Iterator, bool> result= base::insertOrGet( key, THash{}(key) );

        // if an existing element was found, we have to destruct the whole object
        if( result.second == false )
            lang::Destruct( result.first.element->value );

        // placement-new for the whole object
        new (&result.first.element->value) StoredType( key, std::forward<TArgs>( args)...  );

        return result;
    }
#endif

    //==============================================================================================
    /// Inserts a new element only if no other element is contained equals to the one
    /// that is constructed by \p{args}.
    ///
    /// \note
    ///   This method allows preventing the insertion of double entries.
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    /// <p>
    /// \note
    ///   For comparison, a local object of type #StoredType is constructed. In case an equal
    ///   object exists, it is destructed. Otherwise it is moved into this container.
    ///
    /// \par Availability
    ///   This method is available only if this templated type is instantiated with
    ///   \ref alib_ns_containers_hashtable_setandmap "hash set mode". For <em>hash map mode</em>,
    ///   use overloaded version #EmplaceIfNotExistent(const KeyType& key, TArgs&&... args).
    /// \par
    ///   Furthermore it is available only if custom type \p{StoredType} has a move constructor.
    ///
    /// \attention
    ///   If a move constructor exists, but is not duly defined, the method produces undefined
    ///   behavior.<br>
    ///   An alternative version that does not require a move constructor is found with
    ///   #EmplaceIfNotExistent(const KeyType& key, TArgs&&... args). This can be used for hash sets
    ///   that define a subset of \p{StoredType} as a key type \p{TKey} and whose stored type
    ///   is constructible from a constant <em>key-portion</em> and the variadic template arguments.
    ///
    /// @tparam TRequires Used to disable this method where not available.
    /// @tparam TArgs     Types of variadic parameters given with parameter \p{args}.<br>
    ///                   Defaulted and must not be specified with invocations.
    /// @param  args      Variadic parameters to be forwarded to the constructor of \p{StoredType}.
    /// @return A pair containing an iterator referencing either the element found or the new
    ///         element added.
    ///         The bool component is \c true if the insertion took place and \c false nothing
    ///         was changed.
    //==============================================================================================
    template<typename TRequires= MappedType, typename... TArgs>
    requires (     std::same_as<TRequires, StoredType>
                && std::is_move_constructible<StoredType>::value )
    std::pair<Iterator, bool> EmplaceIfNotExistent( TArgs&&... args)
    {DCS
        StoredType value( std::forward<TArgs>( args)... );

        // search element
        std::pair<Iterator, bool> result= base::insertIfNotExists( TValueDescriptor().Key(value),
                                                                   THash{}(TValueDescriptor().Key(value)) );

        // existed? Do nothing
        if( result.second == false )
            return result;

        // placement-new moving the locally created object
        new ( &result.first.element->value )   StoredType( std::move(value) );

        return result;
    }

#if DOXYGEN
    //==============================================================================================
    /// Inserts a new mapped object only if no other object is contained that is associated
    /// already with a key that equals the given \p{key}.
    ///
    /// \note
    ///   This method allows preventing the insertion of double entries.
    ///   For more information, see
    ///   \ref alib_ns_containers_hashtable_single_multi "Single And Multiple Entries"
    ///   of the documentation of this class.
    ///
    /// \par Availability
    ///   This method is available if this templated type is instantiated with
    ///   \ref alib_ns_containers_hashtable_setandmap "hash map mode"
    ///   or for hash sets that only define a subset of \p{StoredType} as a key type \p{TKey} and
    ///   whose stored type is constructible if given a key value and the variadic template
    ///   arguments.
    ///
    /// @tparam TArgs     Types of variadic parameters given with parameter \p{args}.
    /// @param  key       The key to use for search and insertion.
    /// @param  args      Variadic parameters to be forwarded to the constructor of the mapped object
    ///                   of type <c>\p{MappedType}</c>.
    /// @return A pair containing an iterator referencing either the element found or the new
    ///         element added.
    ///         The bool component is \c true if the insertion took place and \c false nothing
    ///         was changed.
    //==============================================================================================
    template<typename... TArgs>
    std::pair<Iterator, bool> EmplaceIfNotExistent( const KeyType& key, TArgs&&... args);
#else
    template<typename... TArgs>
    requires( !std::is_constructible< StoredType, const KeyType&, TArgs&&...     >::value )
    std::pair<Iterator, bool> EmplaceIfNotExistent( const KeyType& key, TArgs&&... args)
    {DCS
        // search element
        std::pair<Iterator, bool> result= base::insertIfNotExists( key, THash{}(key) );

        // existed? Do nothing
        if( result.second == false )
            return result;

        // placement-new for the key (using copy constructor)
        new (&TValueDescriptor().Key(result.first.element->value))     KeyType( key );

        // placement-new for the mapped object (using custom constructor)
        new (&TValueDescriptor().Mapped(result.first.element->value)) MappedType( std::forward<TArgs>( args)... );

        return result;
    }

    template<typename... TArgs>
    requires(std::is_constructible< StoredType, const KeyType&, TArgs&&...>::value )
    std::pair<Iterator, bool> EmplaceIfNotExistent( const KeyType& key, TArgs&&... args)
    {DCS
        // search element
        std::pair<Iterator, bool> result= base::insertIfNotExists( key, THash{}(key) );

        // existed? Do nothing
        if( result.second == false )
            return result;

        // placement-new for the element passing key and args together
        new (&result.first.element->value) StoredType( key, std::forward<TArgs>( args)...  );

        return result;
    }
#endif

    //##############################################################################################
    /// @name Element Search
    //##############################################################################################
    //==============================================================================================
    /// Returns an iterator pointing to the first element of equal key value.
    ///
    /// \note
    ///   The iterator returned may be incremented. It is ensured that further elements contained
    ///   in this hash table with the same key are consecutively following this first element
    ///   returned. However, the iterator does \b not end with the last element of that key.
    ///
    /// \see
    ///    Method #EqualRange, which also returns an iterator pointing behind the last element
    ///    with that key.
    ///
    ///
    /// @param  key   The key to search for.
    /// @return An iterator pointing to the first element found with equal <em>key-portion</em>,
    ///         respectively, one being equal to #end, if no element was found with \p{key}.
    //==============================================================================================
    Iterator        Find( const KeyType& key )
    {DCSSHRD
        auto     hashCode = THash{}(key);
        auto     bucketIdx= hashCode % base::bucketCount;
        Element* elem     = base::findElement( bucketIdx, key, hashCode );
        return Iterator( this, elem == nullptr ? base::bucketCount : bucketIdx, elem );
    }

    //==============================================================================================
    /// Searches an element.
    /// @param  key   The key to search for.
    /// @return An iterator pointing to the first element found with equal <em>key-portion</em>,
    ///         respectively, one being equal to #end, if no element was found with \p{key}.
    //==============================================================================================
    ConstIterator   Find( const KeyType& key )                                                 const
    {DCSSHRD
        auto     hashCode = THash{}(key);
        auto     bucketIdx= hashCode % base::bucketCount;
        Element* elem     = base::findElement( bucketIdx, key, hashCode );
        return ConstIterator( this, elem == nullptr ? base::bucketCount : bucketIdx, elem );
    }

    //==============================================================================================
    /// Overloaded version of method \alib{containers::HashTable;Find(const KeyType&)} which
    /// accepts the \p{hashCode} of the given \p{key} as a second parameter.
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// @param  key      The key to search for.
    /// @param  hashCode Pre-calculated hash code of \p{key}.
    /// @return An iterator pointing to the first element found with equal <em>key-portion</em>,
    ///         respectively, one being equal to #end, if no element was found with \p{key}.
    //==============================================================================================
    Iterator        Find( const KeyType& key, size_t hashCode )
    {DCSSHRD
        auto     bucketIdx= hashCode % base::bucketCount;
        Element* elem     = base::findElement( bucketIdx, key, hashCode );
        return Iterator( this, elem == nullptr ? base::bucketCount : bucketIdx, elem );
    }

    //==============================================================================================
    /// Overloaded version of method \alib{containers::HashTable;Find(const KeyType&)const} which
    /// accepts the \p{hashCode} of the given \p{key} as a second parameter.
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// @param  key      The key to search for.
    /// @param  hashCode Pre-calculated hash code of \p{key}.
    /// @return An iterator pointing to the first element found with equal <em>key-portion</em>,
    ///         respectively, one being equal to #end, if no element was found with \p{key}.
    //==============================================================================================
    ConstIterator   Find( const KeyType& key, size_t hashCode )                                const
    {DCSSHRD
        auto     bucketIdx= hashCode % base::bucketCount;
        Element* elem     = base::findElement( bucketIdx, key, hashCode );
        return ConstIterator( this, elem == nullptr ? base::bucketCount : bucketIdx, elem );
    }

    //==============================================================================================
    /// Tests if an element with given \p{key} is stored in this container.
    /// @param  key   The key to search for.
    /// @return \c true if this hash table contains at least one element with given
    ///         <em>key-portion</em> \p{key}, \c false otherwise.
    //==============================================================================================
    bool            Contains( const KeyType& key )                                             const
    {DCSSHRD
        auto hashCode= THash{}(key);
        return  base::findElement(hashCode % base::bucketCount, key, hashCode )
                != nullptr;
    }

    //==============================================================================================
    /// Searches a key and returns a pair of iterators. The first is pointing to the first
    /// element of the range, the second is pointing to the first element past the range.
    ///
    /// If both iterators are equal, the range is empty (both iterators will be equal to #end).
    ///
    /// @param  key   The key to search for.
    /// @return A pair of iterators defining the range of elements with key \p{key}.
    //==============================================================================================
    std::pair<Iterator,Iterator>                EqualRange(const KeyType& key )
    {DCSSHRD return base::findRange( key ); }

    //==============================================================================================
    /// Searches a key and returns a pair of iterators. The first is pointing to the first
    /// element of the range, the second is pointing to the first element past the range.
    ///
    /// If both iterators are equal, the range is empty (both iterators will be equal to #end).
    ///
    /// @param  key   The key to search for.
    /// @return A pair of iterators defining the range of elements with key \p{key}.
    //==============================================================================================
    std::pair<ConstIterator,ConstIterator>      EqualRange(const KeyType& key )                const
    {DCSSHRD return base::findRange( key ); }

    //##############################################################################################
    /// @name Element Removal
    //##############################################################################################
    //==============================================================================================
    /// Extracts the first element found with the given key from the hash table and returns a
    /// handle to it.<br>
    /// Extracting an element invalidates only the iterators to the extracted element and preserves
    /// the relative order of the elements that are not extracted.
    ///
    /// Extracting and re-inserting nodes is the only way to change a key of an element without
    /// performing reallocation and or destruction/construction.
    ///
    /// @param  key   The key to search a first element for.
    /// @return A handle to an element that allows changes of the formerly stored value.
    ///         Changes may include the <em>key-portion</em> of the data stored.
    ///         Handles may be passed to one of the overloaded insert methods.
    ///         If no element was found, the returned handle is empty.
    //==============================================================================================
    ElementHandle      Extract(const KeyType& key )
    { return Extract( key, THash{}(key) ); }

    //==============================================================================================
    /// Overloaded version of method \alib{containers::HashTable;Extract(const KeyType&)} which
    /// accepts the \p{hashCode} of the given \p{key} as a second parameter.
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// @param  key      The key to search a first element for.
    /// @param  hashCode Pre-calculated hash code of \p{key}.
    /// @return A handle to an element that allows changes of the formerly stored value.
    ///         Changes may include the <em>key-portion</em> of the data stored.
    ///         Handles may be passed to one of the overloaded insert methods.
    ///         If no element was found, the returned handle is empty.
    //==============================================================================================
    ElementHandle      Extract(const KeyType& key, size_t hashCode )
    {DCS
        Node* previous= base::findElementBefore( hashCode % base::bucketCount, hashCode, key );
        if( previous == nullptr )
            return ElementHandle(this, nullptr);

        Element* element= previous->next();
        previous->removeNext();
        --base::size;
        return ElementHandle( this, element );
    }

    //==============================================================================================
    /// Extracts the first element found with the given key from the hash table and returns a
    /// handle to it.<br>
    /// If the iterator was not valid (i.e., #end), the method has undefined behavior.
    /// With debug-builds an \alib assertion is raised.
    ///
    /// Extracting a element invalidates only the iterators to the extracted element, and preserves
    /// the relative order of the elements that are not extracted.
    ///
    /// Extracting and re-inserting nodes is the only way to change a key of an element without
    /// performing reallocation and or destruction/construction.
    ///
    /// @param  pos   The position of the element to extract.
    /// @return A handle to an element that allows changes of the formerly stored value.
    ///         Changes may include the <em>key-portion</em> of the data stored.
    ///         Handles may be passed to one of the overloaded insert methods.
    ///         If no element was found, the returned handle is empty.
    //==============================================================================================
    ElementHandle      Extract( ConstIterator pos )
    {DCS
        ALIB_ASSERT_ERROR(    pos.element        != nullptr
                           && pos.table          != nullptr   ,
                           "MONOMEM/HASHTABLE", "Illegal iterator." )

        Node*    previous= base::buckets[pos.bucketIdx].findLastBefore( pos.element );
        ALIB_ASSERT_ERROR( previous != nullptr, "MONOMEM/HASHTABLE",
                                                "Illegal iterator: Element not found." )

        previous->removeNext();
        --base::size;
        return ElementHandle( this, pos.element );
    }

    //==============================================================================================
    /// Erases all elements stored with the given key.
    ///
    /// @param  key   The key to search elements for deletion.
    /// @return The number of elements removed.
    //==============================================================================================
    integer             erase(const KeyType& key )
    { return Erase( key, THash{}(key) ); }

    //==============================================================================================
    /// Overloaded version of method \alib{containers::HashTable;erase(const KeyType&)}
    /// which accepts the \p{hashCode} of the given \p{key} as a second parameter.
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// @param  key      The key to search elements for deletion.
    /// @param  hashCode Pre-calculated hash code of \p{key}.
    /// @return The number of elements removed.
    //==============================================================================================
    integer             Erase(const KeyType& key, size_t hashCode )
    {DCS
        // search start
        Node* beforeFirst= base::findElementBefore( hashCode % base::bucketCount, hashCode, key );
        if( beforeFirst == nullptr )
            return 0;

        // search end
        Element* end= beforeFirst->next();
        while( end && base::areEqual( end, key, hashCode ) )
            end= end->next();

        // erase and add to recycler
        auto result= base::recyclerType::RecycleList(beforeFirst->next(), end);
        beforeFirst->next( end );

        base::size-= result.second;
        return result.second;
    }

    //==============================================================================================
    /// Erases the unique element with the given key.
    ///
    /// \note
    ///   This method is slightly more efficient than method #erase(const KeyType&), as it will
    ///   not search for a next element with an equal key.<br>
    ///   In debug-compilations, the method asserts that no second element with the same \p{key}
    ///   is available.<br>
    ///   If this table is supposed to
    ///   \ref alib_ns_containers_hashtable_single_multi "store only unique elements", the
    ///   use of this method is therefore recommended, as an assertions hints to an erroneous use
    ///   of the insertion methods.
    ///
    /// @param  key   The key to search elements for deletion.
    /// @return \c true if an element was found and removed, \c false otherwise.
    //==============================================================================================
    bool             EraseUnique( const KeyType& key )
    { return EraseUnique( key, THash{}(key) ); }


    //==============================================================================================
    /// Overloaded version of method \alib{containers::HashTable;EraseUnique(const KeyType&)} which
    /// accepts the \p{hashCode} of the given \p{key} as a second parameter.
    ///
    /// @see Use cases of this method are discussed in reference documentation section
    ///      \ref alib_ns_containers_hashtable_hashprecalc "6.2 Hash Code Pre-calculation".
    ///
    /// @param  key      The key to search elements for deletion.
    /// @param  hashCode Pre-calculated hash code of \p{key}.
    /// @return \c true if an element was found and removed, \c false otherwise.
    //==============================================================================================
    bool             EraseUnique( const KeyType& key, size_t hashCode )
    {DCS
        Node* before= base::findElementBefore( hashCode % base::bucketCount, hashCode, key );
        if( before == nullptr )
            return false;

        ALIB_ASSERT_ERROR(    before->next()->next() == nullptr
                           || !base::areEqual( before->next()->next(), key, hashCode ),
                         "MONOMEM/HASHTABLE", "More than one element found matching the given key" )

        Element* elem= before->removeNext();
        base::recyclerType::Recycle(elem);

        --base::size;
        return true;
    }


    //==============================================================================================
    /// Removes an element specified by an iterator.<br>
    /// If the iterator was not valid (i.e #end), the method has undefined behavior.
    /// With debug-builds an \alib assertion is raised.
    ///
    /// The order of the elements that are not erased is preserved, what makes it possible to
    /// erase individual elements while iterating through the container.
    ///
    /// @param  pos The iterator to the element to remove.
    /// @return An iterator following the removed element.
    //==============================================================================================
    Iterator            erase( ConstIterator pos )
    {DCS
        ALIB_ASSERT_ERROR(    pos.element        != nullptr
                           && pos.table          != nullptr ,
                           "MONOMEM/HASHTABLE", "Illegal iterator." )

        Iterator result( this, pos.bucketIdx, pos.element );
        ++result;

        // search pointer to element before pos
        Node* previous= base::buckets[pos.bucketIdx].findLastBefore( pos.element );
        ALIB_ASSERT_ERROR( previous != nullptr, "MONOMEM/HASHTABLE",
                           "Illegal iterator: Element not found." )


        Element* toDelete= previous->removeNext();
        base::recyclerType::Recycle(toDelete);

        --base::size;
        return result;
    }

ALIB_WARNINGS_IGNORE_NOTHING_RETURNED
    //==============================================================================================
    /// Removes all elements from the given position \p{start} to the element
    /// before given position \p{end}.
    ///
    /// The order of the elements that are not erased is preserved, what makes it possible to
    /// erase individual elements while iterating through the container.
    ///
    /// @param  start The iterator to the element to remove.
    /// @param  end   The first element not to remove.
    /// @return An iterator following the last removed element.
    //==============================================================================================
    Iterator            erase( ConstIterator start, ConstIterator end )
    {DCS
        ALIB_ASSERT_ERROR(    start.element        != nullptr
                           && start.table          != nullptr ,
                           "MONOMEM/HASHTABLE", "Illegal iterator." )

        ALIB_ASSERT_ERROR( start.table == end.table, "MONOMEM/HASHTABLE",
                           "Iterators are referring to different hash tables." )

        if( start.element == end.element )
            return Iterator(this, start.bucketIdx, start.element );

        // loop over all buckets in question
        for( uinteger bucketIdx= start.bucketIdx; bucketIdx <= end.bucketIdx; ++bucketIdx )
        {
            // end of buckets? Return iterator that marks hashtable end
            if( bucketIdx == base::bucketCount )
                return HashTable::end();

            // find the previous pointer to the start node:
            Node* previous;
            if( bucketIdx == start.bucketIdx ) // With the first bucket in the loop, this has to be searched...
            {
                // search pointer to element before start
                previous= base::buckets[start.bucketIdx].findLastBefore( start.element );
                ALIB_ASSERT_ERROR( previous != nullptr, "MONOMEM/HASHTABLE",
                                                        "Illegal iterator: Element not found." )
            }
            else                              // ...afterwards, its of course just the bucket that points to it
            {
                if( base::buckets[bucketIdx].isEmpty() )
                    continue;
                previous= &base::buckets[bucketIdx];
            }

            // destruct either to end of list or to end-iterator element
            if ( bucketIdx < end.bucketIdx )
            {
                base::size-= previous->count();
                base::recyclerType::RecycleList( previous->next() );
                previous->next( nullptr );
            }
            else
            {
                auto pair= base::recyclerType::RecycleList(previous->next(), end.element );
                previous->next(  end.element );
                base::size-= pair.second;

                return Iterator( this, bucketIdx, end.element );
            }
        }
    }
ALIB_WARNINGS_RESTORE


    //==============================================================================================
    /// Removes an element specified by a bucket iterator.
    /// Bucket iterators are receivable using overloaded methods #begin(uinteger) and
    /// \alib{containers::HashTable;cbegin(uinteger)const;cbegin(uinteger)}.
    ///
    /// The order of the elements that are not erased is preserved, what makes it possible to
    /// erase individual elements while iterating through the container.
    ///
    /// @param  pos The iterator to the element to remove.
    /// @return An iterator following the removed element.
    //==============================================================================================
    LocalIterator       erase( ConstLocalIterator pos )
    {DCS
        ALIB_ASSERT_ERROR( pos.element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )

        LocalIterator result( pos.bucketIdx, pos.element->next() );

        Element* element= pos.element;
        base::buckets[pos.bucketIdx].findAndRemove( element );
        base::recyclerType::Recycle( element);
        --base::size;

        return result;
    }

    //==============================================================================================
    /// Removes all element from the given bucket iterator position \p{start} to the element
    /// before given position \p{end}.
    ///
    /// The order of the elements that are not erased is preserved, what makes it possible to
    /// erase individual elements while iterating through the container.
    ///
    /// @param  start The bucket iterator to the element to remove.
    /// @param  end   The bucket iterator to the first element not to remove.
    /// @return An iterator following the last removed element.
    //==============================================================================================
    LocalIterator       erase( ConstLocalIterator start, ConstLocalIterator end )
    {DCS
        ALIB_ASSERT_ERROR( start.element != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )

        Node* previous= base::buckets[start.bucketIdx].findLastBefore( start.element );
        ALIB_ASSERT_ERROR( previous != nullptr, "MONOMEM/HASHTABLE", "Illegal iterator." )
        if( start.element == end.element )
            return LocalIterator( start.bucketIdx, start.element );

        previous->next( end.element );
        auto pair= base::recyclerType::RecycleList( start.element, end.element );

        base::size-= pair.second;

        return LocalIterator( start.bucketIdx, end.element );
    }

    //##############################################################################################
    /// @name std::iterator_traits Interface
    //##############################################################################################

    /// Returns an iterator referring to a mutable element at the start of this table.
    /// @return The first of element in this container.
    Iterator            begin()         { return Iterator     ( this, 0 );     }

    /// Returns an iterator referring to a mutable, non-existing element.
    /// @return The end of the list of elements in this container.
    Iterator            end()           {DCSSHRD
                                          return Iterator    ( this, base::bucketCount, nullptr ); }

    /// Returns an iterator referring to a constant element at the start of this container.
    /// @return The first of element in this container.
    ConstIterator       begin()   const { return ConstIterator( this, 0 ); }

    /// Returns an iterator referring to a constant, non-existing element.
    /// @return The end of the list of elements in this container.
    ConstIterator       end()     const {DCSSHRD
                                          return ConstIterator( this, base::bucketCount, nullptr); }

    /// Returns an iterator referring to a constant element at the start of this container.
    /// @return The first of element in this container.
    ConstIterator       cbegin()  const { return ConstIterator( this, 0 ); }

    /// Returns an iterator referring to a constant, non-existing element.
    /// @return The end of the list of elements in this container.
    ConstIterator       cend()    const {DCSSHRD
                                          return ConstIterator( this, base::bucketCount, nullptr); }

    /// Returns an iterator referring to a mutable element at the start of bucket of index
    /// \p{bucketNumber}.
    /// @param bucketNumber The bucket to iterate on.
    /// @return The first element in bucket \p{bucketNumber}.
    LocalIterator       begin( uinteger bucketNumber )
    {DCSSHRD
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount, "MONOMEM/HASHTABLE",
                           "Bucket number out of range: {}>={}.", bucketNumber, base::bucketCount )
        return LocalIterator( bucketNumber, base::buckets[bucketNumber].first() );
    }

    /// Returns an iterator referring to a constant, non-existing element in bucket of index
    /// \p{bucketNumber}.
    /// @param bucketNumber The bucket to iterate on.
    /// @return The end of the list of elements in bucket \p{bucketNumber}.
    LocalIterator       end( uinteger bucketNumber )
    {DCSSHRD
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount, "MONOMEM/HASHTABLE",
                           "Bucket number out of range: {}>={}.", bucketNumber, base::bucketCount )
        return LocalIterator( bucketNumber, nullptr );
    }

    /// Returns an iterator referring to a mutable element at the start of bucket of index
    /// \p{bucketNumber}.
    /// @param bucketNumber The bucket to iterate on.
    /// @return The first element in bucket \p{bucketNumber}.
    ConstLocalIterator  begin( uinteger bucketNumber )                                         const
    {DCSSHRD
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount, "MONOMEM/HASHTABLE",
                           "Bucket number out of range: {}>={}.", bucketNumber, base::bucketCount )
        return ConstLocalIterator( bucketNumber, base::buckets[bucketNumber].first() );
    }

    /// Returns an iterator referring to a constant, non-existing element in bucket of index
    /// \p{bucketNumber}.
    /// @param bucketNumber The bucket to iterate on.
    /// @return The end of the list of elements in bucket \p{bucketNumber}.
    ConstLocalIterator  end( uinteger bucketNumber )                                           const
    {DCSSHRD
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount, "MONOMEM/HASHTABLE",
                           "Bucket number out of range: {}>={}.", bucketNumber, base::bucketCount )
        return ConstLocalIterator( bucketNumber, nullptr );
    }

    /// Returns an iterator referring to a mutable element at the start of bucket of index
    /// \p{bucketNumber}.
    /// @param bucketNumber The bucket to iterate on.
    /// @return The first element in bucket \p{bucketNumber}.
    ConstLocalIterator  cbegin( uinteger bucketNumber )                                        const
    {DCSSHRD
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount, "MONOMEM/HASHTABLE",
                           "Bucket number out of range: {}>={}.", bucketNumber, base::bucketCount )
        return ConstLocalIterator( bucketNumber, base::buckets[bucketNumber].first() );
    }

    /// Returns an iterator referring to a constant, non-existing element in bucket of index
    /// \p{bucketNumber}.
    /// @param bucketNumber The bucket to iterate on.
    /// @return The end of the list of elements in bucket \p{bucketNumber}.
    ConstLocalIterator  cend( uinteger bucketNumber )                                          const
    {DCSSHRD
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount, "MONOMEM/HASHTABLE",
                           "Bucket number out of range: {}>={}.", bucketNumber, base::bucketCount )
        return ConstLocalIterator( bucketNumber, nullptr );
    }
};  // class Hashtable


// #################################################################################################
// #################################################################################################
// Debug Functions
// #################################################################################################
// #################################################################################################
#if ALIB_DEBUG_CONTAINERS
#include "ALib.Lang.CIFunctions.H"
/// Generates statistics on the given hash table. The meanings of the returned tuple are:
/// 0. The expected average size of a bucket (simply table size divided by number of buckets).
/// 1. The <em>standard deviation</em> of the buckets. The lower this value, the better
///    is the hash algorithm used. A value of <c>1.0</c> denotes the <em>gaussian distribution</em>
///    which indicates perfect randomness. However, this value is unlikely (impossible) to be
///    achieved.
/// 2. The minimum number of elements found in a bucket.
/// 3. The maximum number of elements found in a bucket.
///
/// \par Availability
///   Available only if the compiler-symbol \ref ALIB_DEBUG_CONTAINERS is set.
///
/// \see
///   Sibling namespace functions \alib{containers;DbgDumpDistribution} and
///   \alib{containers;DbgDumpHashtable} provided for debugging and optimization.
///
/// @tparam THashtable A specification  of templated type \alib{containers;HashTable}.
///                    Deduced by the compiler by given parameter \p{hashtable}.
/// @param  hashtable  The hashtable to investigate on.
/// @return The tuple as described above.
template<typename THashtable>
inline
std::tuple<double,double,integer,integer>
DbgGetHashTableDistribution(const THashtable& hashtable )
{
    auto     qtyBuckets      = hashtable.BucketCount();
    double   averageExpected = double(hashtable.Size()) / double(qtyBuckets);
    uinteger minimum         = std::numeric_limits<uinteger>::max();
    uinteger maximum         = std::numeric_limits<uinteger>::min();
    double   diffs           = 0.0;
    integer  sumCheck        = 0;
    for( uinteger i= 0 ; i < qtyBuckets ; ++i )
    {
        auto bucketSize= hashtable.BucketSize( i );
        sumCheck+= bucketSize;
        if( minimum > bucketSize ) minimum= bucketSize;
        if( maximum < bucketSize ) maximum= bucketSize;

        double diff= averageExpected - double(bucketSize);
        diffs+=  diff > 0 ?  diff : - diff;
    }

    ALIB_ASSERT_ERROR( sumCheck == hashtable.Size(), "MONOMEM/HASHTABLE",
      "Error: Hashtable::Size() and sum of bucket sizes differ: {}!={}", hashtable.Size(),sumCheck )
    double deviation= diffs / double(qtyBuckets);

    return std::make_tuple( averageExpected, deviation, minimum, maximum );
}


#include "ALib.Lang.CIMethods.H"
#endif //ALIB_DEBUG_CONTAINERS

//==================================================================================================
//=====================================     HashSet    =============================================
//==================================================================================================

/// This type definition is a shortcut to \alib{containers;HashTable}, usable if the full
/// portion of the data stored in the container is used for the comparison of values.
///
/// \note
///   As with this definition template type \p{TKey} equals stored type \p{StoredType}, methods of
///   target type \alib{containers;HashTable} that accept an object of template type
///   \b TKey expect an object of \p{StoredType} when this type is used.<br>
///   In case this is not wanted (or possible), and only the true key-portion should be expected
///   by interface functions such as \alib{containers;HashTable::Find}, the original
///   type has to be used. Here, typically template parameter \p{TValueDescriptor} can be
///   set to \alib{containers;TSubsetKeyDescriptor}.
///
/// \see
///   For a detailed description of this type, see original type
///   \alib{containers;HashTable}, as well as alternative type definition
///   \alib{containers;HashMap}.
///
/// @tparam TAllocator   The allocator type to use, as prototyped with \alib{lang;Allocator}.
/// @tparam T            The element type stored with this container.
///                      This type is published as \alib{containers;HashTable::StoredType}
///                      and type definition \alib{containers;HashTable::KeyType} becomes
///                      a synonym.
/// @tparam THash        The hash functor applicable to \p{T}.<br>
///                      Defaults to <c>std::hash<T></c> and is published as
///                      \alib{containers;HashTable::HashType}.
/// @tparam TEqual       The comparison functor on \p{T}.<br>
///                      Defaults to <c>std::equal_to<T></c> and is published as
///                      \alib{containers;HashTable::EqualType}.
/// @tparam THashCaching Determines if hash codes are cached when elements are inserted.<br>
///                      Defaults to <b>Caching::Auto</b>, which enables caching if
///                      <c>std::is_arithmetic<StoredType>::value</c> evaluates to \c false.
/// @tparam TRecycling   Denotes the type of recycling that is to be performed. Possible values are
///                      \alib{containers;Recycling;None},
///                      \alib{containers;Recycling;Private} (the default), or
///                      \alib{containers;Recycling;Shared}.
template< typename      TAllocator,
          typename      T,
          typename      THash        = std::hash    <T>,
          typename      TEqual       = std::equal_to<T>,
          lang::Caching THashCaching = lang::Caching::Auto,
          Recycling     TRecycling   = Recycling::Private >
using HashSet= HashTable< TAllocator,
                          TIdentDescriptor<T>,
                          THash, TEqual,
                          THashCaching,
                          TRecycling                  >;

//==================================================================================================
//===================================     HashMap     ==============================================
//==================================================================================================

/// This type definition is a shortcut to \alib{containers;HashTable}, usable if data
/// stored in the container does not include a key-portion, and thus the key to the data
/// is to be separately defined.<br>
/// To achieve this, this type definition aggregates types \p{TKey} and \p{TMapped} into a
/// <c>std::pair<TKey,TMapped></c>. This is done using special value descriptor type
/// \alib{containers;TPairDescriptor}.
///
/// \see
///   For a detailed description of this type, see original type
///   \alib{containers;HashTable}, as well as alternative type definition
///   \alib{containers;HashSet}.
///
/// @tparam TAllocator   The allocator type to use, as prototyped with \alib{lang;Allocator}.
/// @tparam TKey         The type of the <em>key-portion</em> of the inserted data.<br>
///                      This type is published as \alib{containers;HashTable::KeyType}.
/// @tparam TMapped      The type of the <em>mapped-portion</em> of the inserted data.<br>
///                      This type is published as \alib{containers;HashTable::MappedType}.
/// @tparam THash        The hash functor applicable to \p{TKey}.<br>
///                      Defaults to <c>std::hash<TKey></c> and is published as
///                      \alib{containers;HashTable::HashType}.
/// @tparam TEqual       The comparison functor on \p{TKey}.<br>
///                      Defaults to <c>std::equal_to<TKey></c> and is published as
///                      \alib{containers;HashTable::EqualType}.
/// @tparam THashCaching Determines if hash codes are cached when elements are inserted.<br>
///                      Defaults to <b>Caching::Auto</b>, which enables caching if
///                      <c>std::is_arithmetic<TKey>::value</c> evaluates to \c false.
/// @tparam TRecycling   Denotes the type of recycling that is to be performed. Possible values are
///                      \alib{containers;Recycling;None},
///                      \alib{containers;Recycling;Private} (the default), or
///                      \alib{containers;Recycling;Shared}.
template< typename      TAllocator,
          typename      TKey,
          typename      TMapped,
          typename      THash        = std::hash    <TKey>,
          typename      TEqual       = std::equal_to<TKey>,
          lang::Caching THashCaching = lang::Caching::Auto,
          Recycling     TRecycling   = Recycling::Private   >
using HashMap= HashTable<TAllocator,
                         TPairDescriptor<TKey, TMapped>,
                         THash,TEqual,
                         THashCaching,
                         TRecycling                                           >;

} // namespace alib[::containers]

/// Type alias in namespace \b alib. See type definition \ref alib::containers::HashSet.
template< typename      TAllocator,
          typename      TValueDescriptor,
          typename      THash        = std::hash    <typename TValueDescriptor::KeyType>,
          typename      TEqual       = std::equal_to<typename TValueDescriptor::KeyType>,
          lang::Caching THashCaching = lang::Caching::Auto,
          Recycling     TRecycling   = Recycling::Private    >
using HashTable= containers::HashTable<TAllocator,TValueDescriptor,THash,TEqual,THashCaching,TRecycling >;

/// Type alias in namespace \b alib. See type definition \ref alib::containers::HashSet.
template< typename      TAllocator,
          typename      T,
          typename      THash        = std::hash    <T>,
          typename      TEqual       = std::equal_to<T>,
          lang::Caching THashCaching = lang::Caching::Auto,
          Recycling     TRecycling   = Recycling::Private >
using HashSet= containers::HashSet<TAllocator,T,THash,TEqual,THashCaching,TRecycling >;

/// Type alias in namespace \b alib.
template< typename      TAllocator,
          typename      TKey,
          typename      TMapped,
          typename      THash        = std::hash    <TKey>,
          typename      TEqual       = std::equal_to<TKey>,
          lang::Caching THashCaching = lang::Caching::Auto,
          Recycling     TRecycling   = Recycling::Private  >
using HashMap= containers::HashMap<TAllocator,TKey,TMapped,THash,TEqual,THashCaching,TRecycling >;

} // namespace [alib]

#if ALIB_DEBUG_CRITICAL_SECTIONS
#   undef  DCS
#   undef  DCSSHRD
#endif


