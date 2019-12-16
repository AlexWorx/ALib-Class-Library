/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_HASHTABLE
#define HPP_ALIB_MONOMEM_HASHTABLE 1

#if !defined(HPP_ALIB_MONOMEM_DETAIL_HASHTABLEBASE)
#   include "alib/monomem/detail/hashtablebase.inl"
#endif

#if !defined (_GLIBCXX_NUMERIC_LIMITS) && !defined(_LIMITS_)
#   include <limits>
#endif
#if !defined (_GLIBCXX_CMATH) && !defined (_CMATH_)
#   include <cmath>
#endif

namespace aworx { namespace lib { namespace monomem {

/** ************************************************************************************************
 * # Contents #
 * \ref alib_namespace_monomem_hashtable_intro                   "1. Introduction"                 <br>
 * \ref alib_namespace_monomem_hashtable_setandmap               "2. Hash Set vs. Hash Map Mode"   <br>
 * \ref alib_namespace_monomem_hashtable_single_multi            "3. Single And Multiple Entries"  <br>
 * \ref alib_namespace_monomem_hashtable_rehashing               "4. Re-Hashing"                   <br>
 * \ref alib_namespace_monomem_hashtable_iterators               "5. Iterators"                    <br>
 * \ref alib_namespace_monomem_hashtable_hashcodes               "6. Hash Codes"                   <br>
 * &nbsp;&nbsp;\ref alib_namespace_monomem_hashtable_caching       "6.1 Caching Hash Codes"        <br>
 * &nbsp;&nbsp;\ref alib_namespace_monomem_hashtable_hashprecalc   "6.2 Hash Codes Precalculation" <br>
 * &nbsp;&nbsp;\ref alib_namespace_monomem_hashtable_hashquality   "6.3 Hash Codes Precalculation" <br>
 * \ref alib_namespace_monomem_hashtable_monomem                 "7. Memory Use"                   <br>
 * \ref alib_namespace_monomem_hashtable_comparision             "8. Comparison To Standard Library Hash-Types" <br>
 * \ref alib_namespace_monomem_hashtable_referencedoc            "Reference Documentation"                      <br>
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_intro
 * # 1. Introduction #
 * This class implements a \https{hash table,en.wikipedia.org/wiki/Hash_table} that
 * stores and retrieves objects very efficiently in respect to execution performance.
 * All memory for the hash table and its entries are allocated using a
 * \alib{monomem,MonoAllocator}, which has to be provided with construction of an instance.
 *
 * Two type definitions based on this class exists, which reduce the set of template parameters
 * of this type by providing reasonable replacements. These types are:
 * - \alib{monomem,HashSet} and
 * - \alib{monomem,HashMap}.
 *
 * In most cases, one of the two definitions are to be used instead of instantiating this type
 * directly. The meaning of these types are discussed in the next section.
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_setandmap
 * # 2. Hash Set vs. Hash Map Mode Of Operation#
 * Besides storing values of custom type \p{T}, the template parameters and implementation of this
 * class support the concept of "hash maps" which are hash tables that store objects of
 * #MappedType which are associated to a value of #KeyType.
 *
 * With that, two type definitions mentioned in the previous section go along well with what
 * is found in the standard C++ library:
 *
 * 1. \alib{monomem,HashSet}
 *    Configures this type to not having a #MappedType (the type definition then equals an internal
 *    helper struct and is not usable). The full portion of stored type \p{T} is used for
 *    calculating hash values and testing elements for equality.
 *    <p>
 *    The type definition is similar to what types <c>std::unordered_set</c> and
 *    <c>std::unordered_multiset</c> of the standard C++ class library provide.
 *
 * 2. \alib{monomem,HashMap}
 *    Configures the type to split the value into a <em>key-</em> and <em>value-portion</em>.
 *    Only the <em>key-portion</em> is used for calculating hash values and testing elements for
 *    equality. For the value type \p{T}, this definition uses type
 *    <c>std::pair<const TKey, TMapped></c>.
 *    <p>
 *    The type definition is similar to what types <c>std::unordered_map</c> and
 *    <c>std::unordered_multimap</c> of the standard C++ class library provide.
 *
 * If instead of one of these two type definitions, this type \b HashTable is "directly" used, the
 * terms "set" and "map" become become a slightly different meaning. Therefore this documentation
 * defines "modes of operation" with this type:
 *
 * 1. <b>"hash set mode"</b><br>
 *    Is active if no dedicated mapped type is available. Still the value type \p{T} may consist
 *    of "more" than type \p{TKey}, i.e. the \p{TKey} is embedded in \p{T}, but the rest of
 *    \p{T} is not aggregated into a dedicated \p{TMapped} value.
 *
 *
 * 2. <b>"hash map mode"</b><br>
 *    Is active if otherwise a dedicated \p{TMapped} type is given and type \p{T} can be constructed
 *    by constructing the <em>key-portion</em> and the <em>mapped-portion</em> separately.
 *
 * The advantage of this approach is obvious: This types supports custom value types \p{T}
 * that have a <em>key-portion</em> embedded, as often found in custom types.
 * Remember, this case leads to <b>"hash set mode"</b> although it also has some notion that
 * usual "hash maps" have.
 *
 * As a sample for the advantage consider method
 * #EmplaceIfNotExistent(const KeyType& key, TArgs&&... args).
 * A corresponding method is found with type <c>std::unordered_map::try_emplace</c>, but not
 * with <c>std::unordered_set</c>. In contrast with this implementation, the method
 * is usable with <em>hash map mode</em> as well as with <em>hash set mode</em>!<br>
 * The only precondition to the availability of this method in <em>hash set mode</em>, is that
 * value type \p{T} has to be constructible from the combined list of given arguments, hence
 * the \p{KeyType} argument along with given arguments of variadic template types \p{TArgs}.
 * The same is true for method #EmplaceOrAssign(const KeyType&, TArgs&&... args)
 *
 *
 * The set of available interface methods slightly changes with the two operation modes:
 * 1. <b>Hash set mode:</b><br>
 *    Method overload #EmplaceIfNotExistent(TArgs&&... args) is exclusively available with hash
 *    sets.<br>
 *
 * 2. <b>Hash map mode:</b><br>
 *    The following method overloads are exclusively available with hash \b maps:
 *    - #InsertOrAssign(const KeyType&, MappedType&&)
 *    - #InsertOrAssign(const KeyType&, const MappedType&)
 *    - #InsertIfNotExistent(const KeyType&, MappedType&&)
 *    - #InsertIfNotExistent(const KeyType&, const MappedType&)
 *    <br><br>
 *
 *    In addition, the following method overloads of inner types are also exclusively available
 *    with hash \b maps.
 *    - \alib{monomem::HashTable,TIterator::Mapped}
 *    - \alib{monomem::HashTable,TLocalIterator::Mapped}
 *    - \alib{monomem::HashTable,ElementHandle::Mapped}
 *
 * A hint to restrictions is given with the documentation of each method of concern.
 * Note that methods that expect a single #KeyType are usable with both operation modes, as with
 * hash sets the key-type equals type #ValueType.
 *
 * \note
 *   Technically, the availability of the methods is selected at compile-time (using
 *   <c>std::enable_if</c>).
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_single_multi
 * # 3. Single And Multiple Entries #
 * The standard C++ library differentiates between hashing containers that accept only one
 * element with a specific <em>key-portion</em> of the value (see <c>std::unordered_set</c> and
 * <c>std::unordered_map</c>) and those that accept multiple insertions of objects with the
 * same <em>key-portion</em> (see <c>std::unordered_multiset</c> <c>std::unordered_multimap</c>).
 *
 * This library does \b not provide separate types and any instantiation of this class allows
 * multiple entries. But this is rather an extension of functionality, than a restriction
 * and does not impose a penalty on performance.
 *
 * If unique entries are to be achieved, the user of the type has to make sure for herself that
 * no multiple entries are inserted. This can be guaranteed, with the exclusive use of the
 * following set of (partly overloaded) methods, which prevent the creation of multiple entries:
 *
 * - #InsertUnique / #EmplaceUnique
 * - #InsertOrAssign / #EmplaceOrAssign
 * - #InsertIfNotExistent / #EmplaceIfNotExistent
 *
 * In contrast to this, methods #Insert and #Emplace (and their overloads) will insert
 * an equal value without giving further notice (for example by providing a special return value
 * that indicates if the inserted key existed before).<br>
 * Method #EraseUnique(const KeyType&) is more efficient than #EraseUnique(const KeyType&)
 * and a further advantage is that it asserts (in debug-compilations) that not more than one
 * element is found in the table.
 *
 * \note
 *   If uniqueness of key values is needed, it might be seen by the reader to be a "disadvantage"
 *   that the burden is on the user of the class to guarantee such uniqueness.
 *   However, such burden exists with the set-types of the standard library likewise:
 *   There, result values have to be evaluated to determine if an insertion was successful or not.
 *   The various smaller changes and extensions of the <c>std</c>-types with C++ 14 and 17 reflect
 *   the design problems of the approach to provide "unique" and "multi" types.
 *
 * \note
 *   The approach that \alib takes here has three small benefits:
 *   1. A user is free to choose a "mixed mode" by allowing duplicates of certain values (keys)
 *      and not allowing duplicates for others.
 *      While this can be achieved with <c>std::unordered_multimap/set</c> as well, a performance
 *      penalty is given, unless the extended interface of C++ 17 standard is used with great care.
 *   2. The code is better readable, due to explicit naming of the invocations, in contrast
 *      to the need to knowing a container's type with each invocation.
 *   3. The use of the type and this documentation is less confusing, because only one type exists.
 *      In contrast, the two types found in <c>std</c> have equally named methods that act
 *      differently and return different result types.
 *
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_rehashing
 * # 4. Re-Hashing #
 * A check for the need to perform re-hashing is made with every insertion of an element.
 * The policy for re-hashing is described as follows:
 * - With insertions, the new average bucket size is calculated as #Size divided by #BucketCount.
 * - This value is compared with #MaxLoadFactor and if higher the number of buckets is increased.
 * - When increased, the new minimum number of buckets is calculated as #Size divided by
 *   #BaseLoadFactor. Starting from this minimum, the effective number of buckets is chosen as a
 *   next higher prime number from a static table.
 * - A rehash may be triggered "manually" with method #Rehash.
 * - Automatic rehashes may be disabled by setting #MaxLoadFactor to a very high value, e.g.
 *   <c>std::numeric_limits<float>::max()</c>
 *
 * The number of buckets is never decreased, unless method #Rehash is invoked providing a
 * reasonably lower value for parameter \p{newMinBucketCount} than #BucketCount reports.
 * If this was done without adjusting #MaxLoadFactor, chances are high that a next insertion
 * triggers an increase of the buckets and rehashes again.
 *
 * In common scenarios, manual re-hashes are not advised. To a user of the class, usually only
 * values #BaseLoadFactor and #MaxLoadFactor are of concern, which both can be specified once
 * with \ref HashTable::HashTable "construction".
 *
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_iterators
 * # 5. Iterators #
 * ## 5.1 Iterator Types ##
 * There are two types of iterators provided:
 * - \alib{monomem::HashTable,Iterator} and its constant sibling
 *   \alib{monomem::HashTable,LocalIterator}, used to iterate over all elements of the hash table,
 * - \alib{monomem::HashTable,LocalIterator} and its constant sibling
 *   \alib{monomem::HashTable,ConstLocalIterator} used to iterate over the elements of a certain
 *   bucket only.
 *
 * Both types satisfy the C++ standard library concept
 * \https{ForwardIterator,en.cppreference.com/w/cpp/named_req/ForwardIterator}.
 *
 * ## 5.2 Validity Rules ##
 * The following rules define the validity of existing iterator instances on table operations:
 * - <b>Element \b Insertions:</b>
 *   - If no rehashing is performed (see previous section) all iterators remain valid.
 *   - If rehashing is performed, existing iterators become invalid in respect to allowance
 *     of increments and comparison. The elements that existing iterators referred to
 *     prior to the insertion remain valid. Consequently existing iterators may still be used
 *     to access the custom element value and modify the mapped portion of it.
 * - <b>Element \b Removals:</b> <br>
 *   All existing iterators that referred to elements that have been removed become invalid.
 *   All others remain fully intact.<br>
 *   The order of the elements that are not erased is preserved, what makes it possible to
 *   erase individual elements while iterating through the container.
 *
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_hashcodes
 * # 6. Hash Codes #
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_caching
 * ## 6.1 Caching Hash Codes ##
 *
 * Template parameter \p{THashCaching} may be used to control if hash codes are cached.
 * Caching the hash codes increases the memory consumption of this class by <c>sizeof(size_t)</c>
 * per inserted element. While this is only a small amount and memory consumption should not
 * be a great concern when monotonic allocation is used, caching the hash codes may impose a reasonable
 * performance impact. This impact depends on the performance of functor \p{THash} working on the
 * <c>key-portion</c> of the inserted value.
 *
 * The cached hash code is used when the table is
 * \ref alib_namespace_monomem_hashtable_rehashing "re-hashed". In addition, the cached
 * hash code is also used with almost all interface methods (insertion, deletion and search
 * operations): If cached, any needed comparison of two elements will first compare the hash
 * codes and only invoke templated functor \p{TEqual} if those match.
 *
 * If template parameter \p{THashCaching} evaluates to <b>Caching::Auto</b> then this class
 * defaults to use caching if type #KeyType is not arithmetic
 * (using <c>std::is_arithmetic<TKey></c> for this check).<br>
 *
 * The caching option of an instance of this class can be queried with enum #CachedHashCodes.
 *
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_hashprecalc
 * ## 6.2 Hash Codes Precalculation ##
 * The following overloaded methods accept parameter \p{hashCode} in addition to the parameters
 * accepted by their corresponding base version:
 * - #Find(const KeyType&, size_t)
 * - #Insert(const T&, size_t)
 * - #Insert(T&&, size_t)
 * - #InsertUnique(T&&, size_t)
 * - #InsertOrAssign(const KeyType&, MappedType&& mapped, size_t)
 * - #InsertIfNotExistent(const KeyType&, MappedType&& mapped, size_t)
 * - #InsertIfNotExistent(T&&, size_t )
 * - #Extract(const KeyType&, size_t )
 * - #Erase(const KeyType&, size_t )
 * - #EraseUnique( const KeyType&, size_t )
 *
 * The rational for the provision of these methods is to allow to "pre-calculate" a key's hash
 * code prior to invoking the method. This is efficient in situations where one or more subsequent
 * operations with the same key are performed. For example:
 * - Insertions of multiple objects with the same key.
 * - Insertions of an object into multiple hash tables.
 * - Situations where the result of a find operation may may lead to further operations with the
 *   same object.
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_hashquality
 * ## 6.3 Hash Code Quality ##
 * To have hash tables perform in constant time <em>O(1)</em> in the average case, a well
 * implemented calculation of hash-codes has to be provided for template type \p{TKey} with
 * template functor \p{THash}. This is an obligation of the user of this type.
 *
 * This \alibmod_nl supports compiler symbol \ref ALIB_DEBUG_MONOMEM which enables extra features.
 * The entities relevant for this type are:
 * - \alib{monomem,DbgGetHashTableDistribution},
 * - \alib{monomem,DbgDumpDistribution} and
 * - \alib{monomem,DbgDumpHashtable}.
 *
 * These methods may be used to assert the quality of custom hash algorithms.
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_monomem
 * # 7. Memory Use #
 * With template parameter \p{TRecycling} being either \alib{monomem,Recycling::Private}
 * (the default) or \alib{monomem,Recycling::Shared} the internal <em>"node objects"</em> are
 * remembered when elements are erased, and "recycled" with future insertions.
 * The rational for this lies in the simple fact that memory can not be returned to the monotonic
 * allocator. The recycling mechanism is very lightweight and does not cost measurable performance.
 * If it is assured that no deletions are made during the life-cycle of an instance, type
 * \alib{monomem,Recycling::None} may be used to eliminate this little overhead of recycling.
 *
 * If the table is re-hashed, the former bucket list is likewise recycled and sliced into as many
 * internal node objects as possible.
 *
 * With these two mechanisms in place, the use of monotonic allocation with this container
 * is very safe in respect to avoid memory exhaustion.
 * The use of this class, even in scenarios where a lot of (theoretically an endless amount of)
 * erase and insert operations are performed, will not increase memory consumption, as long it is
 * guaranteed that the overall size of the table is limited.<br>
 *
 * If a maximum number of possible insertions is known, method #ReserveRecyclables might be used
 * to allocate all needed memory at once. With that, a
 * \alib{monomem,MonoAllocator::TakeSnapshot,snapshot} of the allocator can be taken and
 * later used to reset the allocator to the minimum that preserves all memory in the according
 * hash table instance.<br>
 * For such use advanced use, it is advisable to fully understand the concept of monotonic
 * allocation implemented with this module \alib_monomem.
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_comparision
 * # 8. Comparison To Standard Library Hash-Types #
 * In the previous sections, it was already referred several times to types
 * - <c>std::unordered_map</c>,
 * - <c>std::unordered_multimap,</c>
 * - <c>std::unordered_set</c> and
 * - <c>std::unordered_multiset</c>
 *
 * of the C++ standard library. The uses cases and features of these four compared to this type are
 * generally the same and this type can be used to replace the standard tyes without general
 * limitations and vice versa.
 *
 * Then with C++ 17, the standard library was extended by a new set of types, namely
 * - <c>std::pmr::unordered_map</c>,
 * - <c>std::pmr::unordered_multimap,</c>
 * - <c>std::pmr::unordered_set</c> and
 * - <c>std::pmr::unordered_multiset</c>
 *
 * which likewise this type, may use monotonic allocation (for example in combination with C++ 17 type
 * <c>std::pmr::monotonic_buffer_resource</c>).
 * While on the one hand \alib needed support of monotonic allocation already for C++ 11,
 * on the other hand the C++ 17 library extensions follow slightly different design goal.
 * Details of this are given in the previous section.
 *
 * Beside these general differences, the specific similarities and differences can be summarized as
 * follows:
 *
 * - This \alib type does not distinguish sets and maps but provides a more flexible approach:
 *   Mapped types do not necessarily need to be build using a <c>std::pair</c> of key and value
 *   elements.
 *   For convenience, when the use of <c>std::pair</c> is suitable, type definition
 *   \alib{monomem,HashMap} offers exactly this.
 * - This \alib type does not distinguish sets/maps from multi-sets/maps.
 *   The rational for this design decision has been described in section
 *   \ref alib_namespace_monomem_hashtable_single_multi "3. Single And Multiple Entries".
 * - Almost all members of the four standard library types are implemented with this type
 *   in a very compatible fashion. The member names were translated from <em>lower_snake_case</em>
 *   to <em>UpperCamelCase</em> and then sometimes slightly changed.<br>
 *   C++ 17 extensions of the standard types were reflected.
 * - Method #Find provides additional overloads that accept an externally (pre-) calculated
 *   hash code. This allows to efficiently use the same key with a series of searches within
 *   different hash table instances. Note that C++ 20 offers a templated version of <c>find</c>,
 *   which allows to perform the same optimization.
 * - Erase methods that accept a #LocalIterator (bucket iterators) that are not available with
 *   the four standard library types, are provided with this type.
 * - There is no <c>operator[]</c> defined. The rational to omit this - at first sight convenient
 *   and intuitive - operator, is that it imposes insert operation even if used in r-value
 *   expressions. This is considered an unwanted side effect.
 * - The caching of hash-codes can be controlled with this type, while it is an implementation
 *   dependent feature with the standard library.
 * - The automatic increase (re-hashing) of the bucket number can be tweaked with
 *   constructor parameter \p{pBaseLoadFactor} (and method #BaseLoadFactor(float)), which is not
 *   specified with the standard library types.
 * - Assignment <c>operator=</c> is not available with this implementation. If needed such has
 *   to implemented by a user (full iteration with copying elements from one instance to another).
 *
 *
 * \~Comment #################################################################################### \~
 * \anchor alib_namespace_monomem_hashtable_referencedoc
 * # Reference Documentation #
 *
 * @tparam T            The values that this hash table stores. In case of
 *                      \ref alib_namespace_monomem_hashtable_setandmap "hash map mode",
 *                      this type needs to contain both, the <em>key-portion</em> and the
 *                      <em>mapped-portion</em> of the data.
 * @tparam TStored      Internal storage version of \p{T}. This is used with
 *                      \ref alib_namespace_monomem_hashtable_setandmap "hash map mode"
 *                      to provide a data type which is storage-compatible to \p{T} but with a
 *                      constant <em>key-portion</em>.
 *                      The internally stored (mutable) data is reinterpreted (!) as \p{T} in this
 *                      case. Hence, a reinterpret cast from this type to \p{T} has to be "legal".
 *                      With \ref alib_namespace_monomem_hashtable_setandmap "hash set mode", this
 *                      type should be the same as \p{T}.
 * @tparam TKey         The key type. In case of
 *                      \ref alib_namespace_monomem_hashtable_setandmap "hash set mode", the type
 *                      given has to be the same as \p{T}.
 *                      This type is published as \alib{monomem,HashTable::KeyType}.
 * @tparam TIfMapped    The type of the mapped portion of the data.
 *                      This type is published as \alib{monomem,HashTable::MappedType}.
 *                      In case of \ref alib_namespace_monomem_hashtable_setandmap "hash set mode",
 *                      type <c>void</c> has to be given and the resulting type
 *                      \b HashTable::MappedType is "undefined" (not usable).
 * @tparam THash        With \ref alib_namespace_monomem_hashtable_setandmap "hash map mode",
 *                      this type needs to be a hash functor on type \p{TKey}, which in
 *                      \ref alib_namespace_monomem_hashtable_setandmap "hash set mode", equals
 *                      \p{T}.<br>
 *                      This type is published as \alib{monomem,HashTable::HashType}.
 * @tparam TEqual       With \ref alib_namespace_monomem_hashtable_setandmap "hash map mode", this
 *                      type needs to be a comparison functor on \p{TKey}, which in
 *                      \ref alib_namespace_monomem_hashtable_setandmap "hash set mode", equals
 *                      \p{T}.<br>
 *                      The type is published as \alib{monomem,HashTable::EqualType}.
 * @tparam TAccess      A functor-like struct used to access the <em>key-</em> and
 *                      <em>mapped-portion</em> of type \p{T}.<br>
 *                      Needs to provide static method, \b Key with
 *                      \ref alib_namespace_monomem_hashtable_setandmap "hash set mode" and
 *                      with <em>hash map mode</em>, in addition method \b Mapped() has to be
 *                      defined.<br>
 *                      For details on required method signatures see provided implementations
 *                      \alib{monomem,detail::HashSetAccess} and
 *                      \alib{monomem,detail::HashMapAccess} as a sample.
 * @tparam THashCaching Determines if hash codes are cached when elements are inserted.
 *                      Defaults to <b>Caching::Auto</b>, which enables caching if
 *                      <c>std::is_arithmetic<TKey>::value</c> evaluates to \c false.
 * @tparam TRecycling   Denotes the type of recycling that is to be performed. Possible values are
 *                      \alib{monomem,Recycling::Private} (the default),
 *                      \alib{monomem,Recycling::Shared} or \alib{monomem,Recycling::None}.
 **************************************************************************************************/
template<typename T,
         typename TStored,
         typename TKey,
         typename TIfMapped,
         typename THash,
         typename TEqual,
         typename TAccess,
         Caching  THashCaching = Caching::Auto,
         typename TRecycling   = Recycling::Private >
class HashTable : protected detail::HashTableBase<T,TStored,TKey,TIfMapped,THash,TEqual,TAccess,THashCaching,TRecycling>
{
    // protected shortcuts to parent and its types we need
    #if !defined(ALIB_DOX)
        protected:
        using base   = detail::HashTableBase<T,TStored,TKey,TIfMapped,THash,TEqual,TAccess,THashCaching,TRecycling>;
        using Element= typename base::Element;
        using Node   = typename base::Node;
    #endif

    public:

    /** ############################################################################################
     * @name Types and Constants
     ##@{ ########################################################################################*/
    /** Type definition publishing template parameter \p{T}. */
    using ValueType   = T;

    /** Type definition publishing template parameter \p{TKey}. */
    using KeyType     = TKey;

    /** Type definition publishing template parameter \p{TIfMapped} in the case <c>void</c>
     *  is not given. If \p{TIfMapped} is void, this type definition is not usable. */
    using MappedType  = typename base::TMapped;

    /** Type definition publishing template parameter \p{THash}. */
    using HashType    = THash;

    /** Type definition publishing template parameter \p{TEqual}. */
    using EqualType   = TEqual;

    /** Type definition publishing template parameter \p{TAccess}. */
    using AccessType  = TAccess;

    /** This type definition may be used to define an externally managed shared recycler,
     *  which can be passed to the alterative constructor of this class when template
     *  parameter \p{TRecycling} equals \alib{monomem,Recycling::Shared}.
     *  \see
     *    Chapter \ref alib_monomem_intro_recycling of the Programmer's Manual
     *    for this \alibmod.                                                          */
    using TSharedRecycler=  ForwardList<Element>;

    /** TMP constant that denotes whether hash codes are cached or not. */
    static constexpr bool CachedHashCodes = base::Element::CachedHashCodes;

    /** The mutable iterator exposed by this container. */
    using Iterator              = typename base::template TIterator       <      T>;

    /** The constant iterator exposed by this container. */
    using ConstIterator         = typename base::template TIterator       <const T>;

    /** The mutable iterator for a single bucket exposed by this container. */
    using LocalIterator         = typename base::template TLocalIterator  <      T>;

    /** The constant iterator for a single bucket exposed by this container. */
    using ConstLocalIterator    = typename base::template TLocalIterator  <const T>;


    /**
     * A value of this type is returned with methods #Extract, which allows to remove
     * an element from the hashtable, without deleting its allocated storage and destructing its
     * custom value.
     *
     * This handle allows write access to the value of an extracted element.
     * In combination with methods #Insert(ElementHandle&) and #InsertIfNotExistent(ElementHandle&),
     * this allows to change parts of the element value, including the <em>key-portion</em> with propper
     * re-insertion.
     *
     * Objects of this type can not be copied, but just moved.
     */
    class ElementHandle
    {
        #if !defined(ALIB_DOX)
            friend class HashTable;
        #endif

        private:
            HashTable*  table;     ///< The table we belong to.
            Element*        element;   ///< The extracted element.

            /**
             * Constructor setting fields #table and #element.
             * @param pTable    The table we belong to.
             * @param pElement  The extracted element.
             */
            ElementHandle( HashTable*  pTable, Element* pElement )
            : table  ( pTable   )
            , element( pElement )
            {}

        public:
            /**
             * Move constructor setting the moved object to emtpy.
             * @param other The handle to move.
             */
            ElementHandle( ElementHandle&& other )
            : table  ( other.table     )
            , element( other.element )
            {
                other.element= nullptr;
            }

            /** Default constructor creating and empty handle. */
            ElementHandle()
            : element( nullptr )
            {}

            /** Deleted copy constructor. */
            ElementHandle( ElementHandle& other )                        = delete;

            /** Deleted copy assignment operator. */
            ElementHandle& operator=( const ElementHandle&  other )      = delete;

            /**
             * Move assignment. Disposes any current content, and moves \p{other} into this.
             * @param other The handle to move into this object.
             * @return A reference to <c>this</c>.
             */
            ElementHandle& operator=(       ElementHandle&& other )
            {
                if( element != nullptr )
                {
                    element->destruct();
                    table->recycle(element);
                }
                table  = other.table;
                element= other.element;
                other.element= nullptr;

                return *this;
            }

            /**
             * Destructor. If this handle is not empty, the allocated storage of the
             * represented element is added to the list of recyclable objects.
             */
            ~ElementHandle()
            {
                if( element != nullptr )
                {
                    element->destruct();
                    table->recycle(element);
                }
            }

            /** Determines if this is a "valid" handle.
             * @return Returns \c true if this objects represents a valid element, \c false
             *         otherwise.                                                           */
            bool        IsEmpty()       const       { return element == nullptr;    }

            /** Returns a mutable reference to this element's data.
             *  Must not be invoked on empty instances.
             *  @return Returns a mutable reference to value of the represented element.    */
            T&          Value  ()       const       { return element->value;    }

            /** Returns a mutable reference to the <em>key-portion</em> of this element's data.
             *  Must not be invoked on empty instances.
             *  @return Returns a mutable reference to the <em>key-portion</em> of the represented
             *          element.                                                            */
            KeyType&    Key    ()       const       { return base::keyPortion( element );    }

            /** Returns a mutable reference to the <em>mapped-portion</em> of this element's data.
             *  Must not be invoked on empty instances.
             *
             *  ## Availability ##
             *  This method is only available with
             *  \ref alib_namespace_monomem_hashtable_setandmap "hash map mode".
             *
             *  @tparam  TEnableIf Used to select this method if applicable. Do not specify.
             *  @return Returns a mutable reference to the mapped object.                   */
            template<typename TEnableIf= TIfMapped>
            ATMP_T_IF(MappedType&, !ATMP_EQ( TEnableIf, void ))
            Mapped ()       const       { return base::mappedPortion( element );   }

    }; // class ElementHandle

    /** ############################################################################################
     * @name Construction/Destruction And Allocator Access
     ##@{ ########################################################################################*/

    #if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Constructor.
     * \note
     *   This cnstructor is not available if template argument \p{TRecycling} equals
     *   type \alib{monomem,Recycling::Shared}.
     *
     * @param pAllocator       The monotonic allocator to use.
     * @param pBaseLoadFactor  The base load factor. Defaults to <c>1.0</c>.
     * @param pMaxLoadFactor   The maximum load factor. Defaults to <c>2.0</c>.
     **********************************************************************************************/
    explicit inline
    HashTable( MonoAllocator*   pAllocator,
               float            pBaseLoadFactor = 1.0,
               float            pMaxLoadFactor  = 2.0  );
    #else
    template<typename TEnableIf= TRecycling,
        ATMP_T_IF(int, !ATMP_EQ(TEnableIf, Recycling::Shared)) = 0 >
    explicit
    HashTable( MonoAllocator*   pAllocator,
               float            pBaseLoadFactor = 1.0,
               float            pMaxLoadFactor  = 2.0  )
    : base( pAllocator, pBaseLoadFactor, pMaxLoadFactor  )
    {}
    #endif

    /** ********************************************************************************************
     * Constructor taking a shared recycler.
     * @param pAllocator       The monotonic allocator to use.
     * @param pRecycler        The shared recycler.
     * @param pBaseLoadFactor  The base load factor. See method #BaseLoadFactor for details.
     *                         Defaults to <c>1.0</c>.
     * @param pMaxLoadFactor   The maximum load factor. See method #MaxLoadFactor for details.
     *                         Defaults to <c>2.0</c>.
     **********************************************************************************************/
    explicit
    HashTable( MonoAllocator*   pAllocator,
               TSharedRecycler& pRecycler,
               float            pBaseLoadFactor = 1.0,
               float            pMaxLoadFactor  = 2.0  )

    : base( pAllocator, pRecycler, pBaseLoadFactor, pMaxLoadFactor  )
    {}



    /** ********************************************************************************************
     * Destructor. Destructs all elements in this object.
     **********************************************************************************************/
    ~HashTable()
    {
        if ALIB_CONSTEXPR_IF( !std::is_trivially_destructible<TStored>::value )
            Clear();
    }

    /** ********************************************************************************************
     * In some situations, the allocator object to use might not be available with construction
     * of an instance. This method may be used to attach an external allocator at a later
     * point in time - but prior to this instance's first use.
     *
     * @param pAllocator The allocator that was omitted in the constructor.
     **********************************************************************************************/
    void            SetAllocatorPostConstruction( MonoAllocator* pAllocator )
    {
        ALIB_ASSERT_ERROR( base::allocator == nullptr,
                           "Allocator already set.")
        base::allocator= pAllocator;
    }

    /** ********************************************************************************************
     * Returns the allocator of this object. Usually the allocator might be used to perform
     * allocations in respect to data found in objects stored in this object.
     * However, such allowance is dependent on the use case and not part of this class's
     * contract.
     *
     * @return The allocator that was provided in the constructor.
     **********************************************************************************************/
    MonoAllocator*  GetAllocator()                                                             const
    {
        return base::allocator;
    }


    /** ############################################################################################
     * @name Size and Capacity
     ##@{ ########################################################################################*/

    /** ********************************************************************************************
     * Destructs and removes all elements from this hash table. The allocated space
     * of the elements will be preserved and "recycled" with future insertions.
     **********************************************************************************************/
    void            Clear()
    {
        base::clear();
    }

    /** ********************************************************************************************
     * Resets this container. Invokes #Clear to destruct all keys and values and in case of
     * \alib{monomem,Recycling::Private,private recycling} disposes all previously allocated
     * recyclable instances of the internal element type.
     *
     * This method has to be called prior to resetting the associated monotonic allocator
     * given in the constructor.
     **********************************************************************************************/
    void            Reset()
    {
        base::reset();
    }

    /** ********************************************************************************************
     * Returns the number of stored elements. Note that this method runs in constant time, as
     * the number of elements is kept counted during operation.
     * @return The number of elements stored in the hash table.
     **********************************************************************************************/
    integer         Size()                                                                     const
    {
        return base::size;
    }

    /** ********************************************************************************************
     * Invokes #Size and compares result with \c 0.
     * @return \c true if this list is empty, \c false otherwise.
     **********************************************************************************************/
    bool            IsEmpty()                                                                  const
    {
        return base::size == 0;
    }

    /** ********************************************************************************************
     * Reserves space for at least the given number of elements.
     * This might re-hash this table.
     * @param expectedSize The expected number of elements to be stored in the hash table.
     **********************************************************************************************/
    void            Reserve( integer expectedSize )
    {
        return base::rehash( static_cast<uinteger>(std::ceil(expectedSize/base::baseLoadFactor)) );
    }

    /** ********************************************************************************************
     * Same as #Reserve but in addition also already allocates the required space for the number
     * of additional elements expected.
     *
     * This method may be helpful if the definite number of stored elements that is never
     * exceeded is known. In this case, a \alib{monomem,MonoAllocator::TakeSnapshot,snapshot}
     * of the monotonic allocator could be taken after the invocation of this method and
     * then used to reset the monotonic allocator with preserving the memory used by this container.
     *
     * \note
     *   This method is not available (aka does not compile) with instantiations
     *   that specify template parameter \p{TRecycling} as \alib{monomem,Recycling::None}.
     *
     * @param expectedSize The expected number of elements to be stored in the hash table.
     **********************************************************************************************/
    void            ReserveRecyclables( integer expectedSize )
    {
        Reserve( expectedSize );

        auto requiredRecyclables= ( expectedSize - Size()) - RecyclablesCount();
        if( requiredRecyclables > 0 )
        {
            Element* newElements= base::allocator->template AllocArray<Element>(requiredRecyclables);
            for( auto i= requiredRecyclables - 2; i >= 0   ; --i )
                newElements[i].makePointTo( &newElements[i + 1] );

            base::recycle( &newElements[0], &newElements[requiredRecyclables - 1] );
        }
    }

    /** ********************************************************************************************
     * Counts the number of currently allocated but unused (not contained) element nodes
     * that will be recycled with upcoming insertions.
     *
     * \note
     *   This method is provided for completeness and unit-testing. It should not be of
     *   relevance for common usage.<br>
     *   Furthermore, this method is not available (aka does not compile) with instantiations
     *   that specify template parameter \p{TRecycling} as \alib{monomem,Recycling::None}.
     *
     * @return The number of removed and not yet recycled elements.
     **********************************************************************************************/
    integer         RecyclablesCount()                                                         const
    {
        return base::TRecycler::count();
    }

    /** ############################################################################################
     * @name Hash Policy
     ##@{ ########################################################################################*/

    /** ********************************************************************************************
     * Sets a new value for the "base load factor" used with this container.
     * The base load factor determines the minimum number of buckets
     * when re-hashing is performed, either automatically or by invoking method #Rehash
     * with a value of \c 0 for parameter \p{newMinBucketCount}.
     *
     * The formula to determine the minimum number of buckets is #Size divided by this factor.
     * A static table of prime numbers is searched for the next higher number and this value
     * is then used to determine the number of buckets.
     *
     * The default value for this factor is defined as <c>1.0</c> by the default value
     * of parameter \p{pBaseLoadFactor} of the constructor.
     *
     * \note
     *   Invoking this method never triggers rehashing.
     *
     * \see
     *   Overloaded method #BaseLoadFactor() and this class's documentation section
     *   \ref alib_namespace_monomem_hashtable_rehashing "4. Re-Hashing".
     *
     * @param newBaseLoadFactor The new base load factor to use when a rehash is performed.
     **********************************************************************************************/
    void            BaseLoadFactor( float newBaseLoadFactor )
    {
        base::baseLoadFactor= newBaseLoadFactor;
    }

    /** ********************************************************************************************
     * Returns the actual base load factor.
     *
     * \see
     *   Overloaded method #BaseLoadFactor(float) and this class's documentation section
     *   \ref alib_namespace_monomem_hashtable_rehashing "4. Re-Hashing".
     *
     * @return The current value of the base load factor.
     **********************************************************************************************/
    float           BaseLoadFactor()                                                       const
    {
        return base::baseLoadFactor;
    }


    /** ********************************************************************************************
     * Sets a new value for the "maximum load factor" which is the average number of elements
     * per bucket.
     *
     * The default value for this factor is defined as <c>2.0</c> by the default value
     * of parameter \p{pMaxLoadFactor} of the constructor.
     *
     * \note
     *   Invoking this method triggers rehashing, in case the hash table is not empty and
     *   the new maximum load factor is below the current load factor of this container, which
     *   equals #Size divided by #BucketCount.<br>
     *   This method may be used to temporarily disable re-hashing by providing
     *   <c>std::numeric_limits<float>::max()</c>.
     *
     * \see
     *   Overloaded method #MaxLoadFactor() and this class's documentation section
     *   \ref alib_namespace_monomem_hashtable_rehashing "4. Re-Hashing".
     *
     * @param newMaxLoadFactor The maximum load factor used to determine the need of re-hashing.
     **********************************************************************************************/
    void            MaxLoadFactor( float newMaxLoadFactor )
    {
        base::setMaxLoadFactor( newMaxLoadFactor );
    }

    /** ********************************************************************************************
     * Returns the actual maximum load factor.
     *
     * \see
     *   Overloaded method #MaxLoadFactor(float) and this class's documentation section
     *   \ref alib_namespace_monomem_hashtable_rehashing "4. Re-Hashing".
     *
     * @return The current value of the maximum load factor.
     **********************************************************************************************/
    float          MaxLoadFactor()                                                         const
    {
        return base::maxLoadFactor;
    }

    /** ********************************************************************************************
     * Potentially changes the number of buckets and rehashes all elements of this hash table.
     * If parameter \p{newMinBucketCount} equals \c 0 (the default), then the new size is
     * calculated as #Size divided by #BaseLoadFactor.
     *
     * The final bucket count is chosen as the next higher value found in a predefined table of
     * prime numbers. This is made to achieve a better distribution of the entries.
     *
     * @param newMinBucketCount The minimum new bucket count.<br>
     *                          Defaults to \c 0, which chooses a size according to
     *                          #BaseLoadFactor.
     **********************************************************************************************/
    void            Rehash( uinteger newMinBucketCount= 0 )
    {
        if( newMinBucketCount > 0 )
            base::rehash( newMinBucketCount );
        else
            base::rehash( static_cast<uinteger>(std::ceil( base::size/base::baseLoadFactor )) );

    }

    /** ############################################################################################
     * @name Bucket Interface
     ##@{ ########################################################################################*/

    /** ********************************************************************************************
     * Returns the number of "buckets" that this hash table currently uses.
     *
     * @return The size of the array of hash buckets.
     **********************************************************************************************/
    uinteger        BucketCount()                                                              const
    {
        return base::bucketCount;
    }

    /** ********************************************************************************************
     * Returns the number of entries stored in the bucket with the given number.
     *
     * @param bucketNumber  The number of the bucket to receive the size for.
     * @return The number of entries in the specified bucket.
     **********************************************************************************************/
    uinteger        BucketSize( uinteger bucketNumber )                                        const
    {
        ALIB_ASSERT_ERROR(  bucketNumber <  base::bucketCount, "Bucket number out of range." )
        return static_cast<uinteger>(base::buckets[bucketNumber].count());
    }

    /** ********************************************************************************************
     * Returns the number of the bucket corresponding to \p{key}.
     *
     * @param key  The key to evaluate the bucket number for.
     * @return The bucket number that \p{key} is assigned to.
     **********************************************************************************************/
    uinteger        BucketNumber( const KeyType& key )                                         const
    {
        return THash()(key) % base::bucketCount;
    }

    /** ############################################################################################
     * @name Element Insertion
     ##@{ ########################################################################################*/
    /** ********************************************************************************************
     * See #Insert(T&&) which is invoked with a copy of \p{value}.
     *
     * @param  value   A value to copy and insert.
     * @return An iterator referring to the element added.
     **********************************************************************************************/
    Iterator        Insert( const T& value)
    {
        return Insert( T( value ), THash()( TAccess().Key( reinterpret_cast<TStored&>(value))  ) );
    }

    /** ********************************************************************************************
     * Overloaded version of method \alib{monomem::HashTable,Insert(const T&)} which
     * accepts the \p{hashCode} of the given \p{value} as a second parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     * @param  value    A value to copy and insert.
     * @param  hashCode Pre-calculated hash code of \p{value}.
     * @return An iterator referring to the element added.
     **********************************************************************************************/
    Iterator        Insert( const T& value, size_t hashCode )
    {
        return Insert( T( value ), hashCode );
    }

    /** ********************************************************************************************
     * Moves the given value into this table.<br>
     * Existing iterators remain valid.
     *
     * \note
     *   The use of this method may insert elements sharing the same key as already existing
     *   elements.
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     *
     * @param  value   An rvalue reference of contained type \p{T} to insert.
     * @return An iterator referring to the element added.
     **********************************************************************************************/
    Iterator        Insert( T&& value )
    {
        auto   hashCode = THash()( TAccess().Key( reinterpret_cast<TStored&>(value)) );
        return Insert( std::move(value), hashCode );
    }

    /** ********************************************************************************************
     * Overloaded version of method \alib{monomem::HashTable,Insert(T&&)} which
     * accepts the \p{hashCode} of the given \p{value} as a second parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     *
     * @param  value    An rvalue reference of contained type \p{T} to insert.
     * @param  hashCode Pre-calculated hash code of \p{value}.
     * @return An iterator referring to the element added.
     **********************************************************************************************/
    Iterator        Insert( T&& value, size_t hashCode )
    {
        // Recycle node or create a new one
        Element* element= base::allocElement(hashCode);

        // placement new
        new ( &element->value )   T ( std::move(value) );

        // insert to hash table
        base::increaseSize( 1 );
        auto bucketIdx=  base::insertInBucket( element, hashCode );
        return Iterator( this, bucketIdx, element);
    }

    /** ********************************************************************************************
     * Inserts the element contained in the given \alib{monomem::HashTable,ElementHandle}
     * into the hash table.
     *
     * \note
     *   The use of this method may insert elements sharing the same key as already existing
     *   elements.
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     *
     * <p>
     *   Objects of type \b ElementHandle objects may be received using overloaded methods
     *   #Extract. The combination of \b %Extract and this method (as well as method
     *   #InsertIfNotExistent(ElementHandle&) are the only way to change the <em>key-portion</em> of an
     *   element without element destruction/re-construction.
     *
     * @param  handle   A reference to a handle to an element,  previously received with #Extract.
     * @return On success, returns an iterator that refers to the inserted element.
     *         On failure (if parameter \p{handle} was empty), the returned iterator equals #end.
     **********************************************************************************************/
    Iterator        Insert( ElementHandle& handle )
    {
        if( handle.IsEmpty() )
            return end();

        base::increaseSize( 1 );
        Element* element= handle.element;
        auto hashCode= THash()( base::keyPortion( element ));
        element->fixHashCode( hashCode );
        auto bucketIdx=  base::insertInBucket(  element, hashCode );
        handle.element= nullptr;
        return Iterator( this, bucketIdx, element );
    }

    /** ********************************************************************************************
     * See #InsertUnique(T&&) which is invoked with a copy of \p{value}.
     *
     * @param  value   An element to insert whose <em>key-portion</em> has to be different to
     *                 all currently contained elements.
     * @return An iterator referring to the element added.
     **********************************************************************************************/
    Iterator        InsertUnique( const T& value )
    {
        return InsertUnique( T( value ) );
    }

    /** ********************************************************************************************
     * Overloaded version of method \alib{monomem::HashTable,InsertUnique(const T&)} which
     * accepts the \p{hashCode} of the given \p{key} as a second parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     * @param  value    An element to insert whose <em>key-portion</em> has to be different to
     *                  all currently contained elements.
     * @param  hashCode Pre-calculated hash code of \p{value}.
     * @return An iterator referring to the element added.
     **********************************************************************************************/
    Iterator        InsertUnique( const T& value, size_t hashCode )
    {
        return InsertUnique( T( value ), hashCode );
    }

    /** ********************************************************************************************
     * Moves the given value into this table without checking to place it in the right
     * position in respect to existing elements with the same <em>key-portion</em>.
     *
     * \attention
     *   This method must only be used if the caller guarantees that no other element is
     *   currently stored in this container having an equal <em>key-portion</em>.
     *   In such situations, this method is very efficient.<br>
     *   If one exists already, this hash table is not considered in a consistent state
     *   after the operation. I.e., method #EqualRange will discontinue to function properly.
     *
     * \attention
     *   In debug-compilations, an \alib assertion is raised, if an equal element exists.
     *   For this reason, performance differences to method #Insert will be seen only with
     *   release-compilations.
     *
     * \attention
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     *
     *
     * @param  value   An element to insert whose <em>key-portion</em> has to be different to
     *                 all currently contained elements.
     * @return An iterator referring to the element added.
     **********************************************************************************************/
    Iterator        InsertUnique(T&& value)
    {
        auto   hashCode = THash()( TAccess().Key( reinterpret_cast<TStored&>(value)) );
        return InsertUnique(std::move(value), hashCode );
    }

    /** ********************************************************************************************
     * Overloaded version of method \alib{monomem::HashTable,InsertUnique(T&&)} which
     * accepts the \p{hashCode} of the given \p{key} as a second parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     * @param  value    An element to insert whose <em>key-portion</em> has to be different to
     *                  all currently contained elements.
     * @param  hashCode Pre-calculated hash code of \p{value}.
     * @return An iterator referring to the element added.
     **********************************************************************************************/
    Iterator        InsertUnique( T&& value, size_t hashCode )
    {
        Element* element  = base::allocElement( hashCode );

        base::increaseSize( 1 );
        auto    bucketIdx= hashCode % base::bucketCount;
        base::buckets[bucketIdx].pushFront( element );

        new ( &element->value )   T ( std::move(value) );

        #if ALIB_DEBUG
            //Check that this was the first of
            auto it= cbegin(bucketIdx);
            ALIB_ASSERT( it.element == element ) // has to be the first inserted
            while( ++it != cend(bucketIdx) )
            {
                ALIB_ASSERT_ERROR( !base::areEqual(element, it.element ),
                                   "InsertUnique used while element with same key-portion "
                                   "existed!" )
            }
        #endif

        return Iterator( this, bucketIdx, element);
    }


    /** ********************************************************************************************
     * See #InsertOrAssign(const KeyType&, MappedType&&) which is invoked with a copy of \p{mapped}.
     *
     * ## Availability ##
     * This method is only available with
     * \ref alib_namespace_monomem_hashtable_setandmap "hash map mode".
     *
     * @tparam  TEnableIf Used to disable this method for instantiations of this
     *                    template type with <em>hash set mode</em>.
     * @param   key       The key to use for search and insertion.
     * @param   mapped    The mapped value to copy and then insert or assign.
     * @return A pair containing an iterator referencing the element added.
     *         The bool component is \c true if the insertion took place and \c false if the
     *         assignment took place.
     **********************************************************************************************/
    template<typename TEnableIf= TIfMapped>
    ATMP_T_IF(std::pair<Iterator ALIB_COMMA bool>, !ATMP_EQ( TEnableIf, void ))
    InsertOrAssign( const KeyType& key, const MappedType& mapped)
    {
        return InsertOrAssign( key, MappedType(mapped) );
    }

    /** ********************************************************************************************
     * Replaces an existing, respectively inserts a new element into this hash table.
     *
     * \note
     *   This method allows to prevent the insertion of double entries.
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     *
     * ## Availability ##
     * This method is only available with
     * \ref alib_namespace_monomem_hashtable_setandmap "hash map mode".
     *
     * @tparam TEnableIf Used to disable this method for instantiations of this
     *                   template type with <em>hash set mode</em>.
     * @param  key       The key to use for search and insertion.
     * @param  mapped    The mapped value to insert or assign.
     * @return A pair containing an iterator referring to the element added.
     *         The bool component is \c true if the insertion took place and \c false if the
     *         assignment took place.
     **********************************************************************************************/
    template<typename TEnableIf= TIfMapped>
    ATMP_T_IF(std::pair<Iterator ALIB_COMMA bool>, !ATMP_EQ( TEnableIf, void ))
    InsertOrAssign( const KeyType& key, MappedType&& mapped )
    {
        return InsertOrAssign( key, std::move(mapped), THash()(key) );
    }

    /** ********************************************************************************************
     * Overloaded version of method
     * \alib{monomem::HashTable,InsertOrAssign(const KeyType&, MappedType&&)} which
     * accepts the \p{hashCode} of the given \p{key} as a third  parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     * ## Availability ##
     * This method is only available with
     * \ref alib_namespace_monomem_hashtable_setandmap "hash map mode".
     *
     * @tparam TEnableIf Used to disable this method for instantiations of this
     *                   template type with <em>hash set mode</em>.
     * @param  key       The key to use for search and insertion.
     * @param  mapped    The mapped value to insert or assign.
     * @param  hashCode  Pre-calculated hash code of \p{key}.
     * @return A pair containing an iterator referring to the element added.
     *         The bool component is \c true if the insertion took place and \c false if the
     *         assignment took place.
     **********************************************************************************************/
    template<typename TEnableIf= TIfMapped>
    ATMP_T_IF(std::pair<Iterator ALIB_COMMA bool>, !ATMP_EQ( TEnableIf, void ))
    InsertOrAssign( const KeyType& key, MappedType&& mapped, size_t hashCode )
    {
        std::pair<Iterator, bool> result= base::insertOrGet( key, hashCode );

        // if an existing element was found, we have to destruct the mapped value
        if( result.second == false )
            monomem::Destruct( &base::mappedPortion( result.first.element ) );

        // if otherwise a new element was inserted, we have to copy the key
        else
            new (&base::keyPortion( result.first.element ))  KeyType( key );

        // placement new for the mapped object
        new ( &base::mappedPortion( result.first.element ) )   MappedType( std::move( mapped) );

        return result;
    }

    /** ********************************************************************************************
     * See #InsertIfNotExistent(const KeyType&, MappedType&&) which is invoked with a copy of
     * \p{mapped}.
     *
     * ## Availability ##
     * This method is only available with
     * \ref alib_namespace_monomem_hashtable_setandmap "hash map mode".
     *
     * @tparam TEnableIf Used to disable this method for instantiations of this
     *                   template type with <em>hash set mode</em>.
     * @param  key       The key to use for search and insertion.
     * @param  mapped    The mapped object to copy and insert if \p{key} is not existing.
     * @return A pair containing an iterator referencing either the element found or the new
     *         element added. The bool component is \c true if the insertion took place and \c false
     *         if nothing was changed.
     **********************************************************************************************/
    template<typename TEnableIf= TIfMapped>
    ATMP_T_IF(std::pair<Iterator ALIB_COMMA bool>, !ATMP_EQ( TEnableIf, void ))
    InsertIfNotExistent( const KeyType& key, const MappedType& mapped)
    {
        return InsertIfNotExistent( key, MappedType(mapped) );
    }

    /** ********************************************************************************************
     * Overloaded version of method
     * \alib{monomem::HashTable,InsertIfNotExistent(const KeyType&\,MappedType&&)} which
     * accepts the \p{hashCode} of the given \p{key} as a third parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     * @tparam TEnableIf Used to disable this method for instantiations of this
     *                   template type with <em>hash set mode</em>.
     * @param   key      The key to use for search and insertion.
     * @param   hashCode Pre-calculated hash code of \p{key}.
     * @param   mapped   The mapped value to insert if \p{key} is not existing.
     * @return A pair containing an iterator referencing either the element found or the new
     *         element added. The bool component is \c true if the insertion took place and \c false
     *         if nothing was changed.
     **********************************************************************************************/
    template<typename TEnableIf= TIfMapped>
    ATMP_T_IF(std::pair<Iterator ALIB_COMMA bool>, !ATMP_EQ( TEnableIf, void ))
    InsertIfNotExistent( const KeyType& key, MappedType&& mapped, size_t hashCode)
    {
        // search element
        std::pair<Iterator, bool> result= base::insertIfNotExists( key, hashCode );

        // existed? Do nothing
        if( result.second == false )
            return result;

        // placement new for the key (using copy constructor)
        new (&base::keyPortion( result.first.element ))   KeyType( key );

        // placement new for the mapped (using move constructor)
        new ( &base::mappedPortion( result.first.element ) )   MappedType( std::move( mapped) );

        return result;
    }

    /** ********************************************************************************************
     * Inserts a new mapped object only if no other object is contained that is associated
     * already with the same key as given \p{key}.
     *
     * \note
     *   This method allows to prevent the insertion of double entries.
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     *
     * ## Availability ##
     * This method is only available with
     * \ref alib_namespace_monomem_hashtable_setandmap "hash map mode".
     *
     * @tparam TEnableIf Used to disable this method for instantiations of this
     *                   template type with <em>hash set mode</em>.
     * @param   key      The key to use for search and insertion.
     * @param   mapped   The mapped value to insert if \p{key} is not existing.
     * @return A pair containing an iterator referencing either the element found or the new
     *         element added. The bool component is \c true if the insertion took place and \c false
     *         if nothing was changed.
     **********************************************************************************************/
    template<typename TEnableIf= TIfMapped>
    ATMP_T_IF(std::pair<Iterator ALIB_COMMA bool>, !ATMP_EQ( TEnableIf, void ))
    InsertIfNotExistent( const KeyType& key, MappedType&& mapped)
    {
        return InsertIfNotExistent( key, std::move(mapped), THash()(key) );
    }

    /** ********************************************************************************************
     * See #InsertIfNotExistent(T&&) which is invoked with a copy of \p{value}.
     *
     * @param  value    The value to copy and insert.
     * @return A pair containing an iterator referencing either the element found or the new
     *         element added. The bool component is \c true if the insertion took place and \c false
     *         if nothing was changed.
     **********************************************************************************************/
    std::pair<Iterator, bool>    InsertIfNotExistent( const T& value )
    {
        return InsertIfNotExistent( T(value) );
    }

    /** ********************************************************************************************
     * Inserts a new mapped object only if no other object is contained that is associated
     * already with the same key as given \p{key}.
     *
     * \note
     *   This method allows to prevent the insertion of double entries.
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     *
     * @param  value   A rvalue reference of a \p{T} to insert.
     * @return A pair containing an iterator referencing either the element found or the new
     *         element added.
     *         The bool component is \c true if the insertion took place and \c false if nothing
     *         was changed.
     **********************************************************************************************/
    std::pair<Iterator, bool>    InsertIfNotExistent( T&& value )
    {
        auto hashCode=  THash()( TAccess().Key(value) );
        return InsertIfNotExistent(std::move(value), hashCode);
    }

    /** ********************************************************************************************
     * Overloaded version of method \alib{monomem::HashTable,InsertIfNotExistent(T&&)} which
     * accepts the \p{hashCode} of the given \p{key} as a second parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     * @param  value    A rvalue reference of a \p{T} to insert.
     * @param  hashCode Pre-calculated hash code of \p{value}.
     * @return A pair containing an iterator referencing either the element found or the new
     *         element added.
     *         The bool component is \c true if the insertion took place and \c false if nothing
     *         was changed.
     **********************************************************************************************/
    std::pair<Iterator, bool>    InsertIfNotExistent( T&& value, size_t hashCode )
    {
        // search element
        std::pair<Iterator, bool> result= base::insertIfNotExists( TAccess().Key(value), hashCode );

        // existed? Do nothing
        if( result.second == false )
            return result;

        // placement new for the value(using move constructor)
        new ( &result.first.element->value )   T( std::move(value) );

        return result;
    }

    /** ********************************************************************************************
     * Inserts the element contained in the given \alib{monomem::HashTable,ElementHandle} into
     * this table, if no equal element exists. In the unsuccessful case, the given
     * \b %ElementHandle remains set and can be reused.<br>
     *
     * Existing iterators remain valid.
     *
     * \note
     *   This method allows to prevent the insertion of double entries.
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     *
     * <p>
     * \see
     *   Objects of type \b ElementHandle objects may be received using overloaded methods
     *   #Extract. The combination of \b %Extract and this method (as well as method
     *   #Insert(ElementHandle&) are the only way to change the <em>key-portion</em> of an element
     *   without element destruction/re-construction.
     *
     * @param  handle   A reference to a handle to an element,  previously received with #Extract.
     * @return If an empty handle is given, #end is returned.
     *         Otherwise, if no equal element existed, an iterator that refers to the inserted
     *         element is returned and the given \p{handle} is emptied.<br>
     *         If an equal element existed, the returned iterator refers to the existing element
     *         and the \p{handle} remains set (not empty).
     **********************************************************************************************/
    Iterator    InsertIfNotExistent( ElementHandle& handle )
    {
        if( handle.IsEmpty() )
            return end();

        Element* element  = handle.element;
        auto  hashCode = THash()( base::keyPortion( handle.element ) );
        auto  bucketIdx= hashCode % base::bucketCount;

        Element* existing= base::findElement( hashCode, base::keyPortion( element ), hashCode );
        if ( existing != nullptr )
            return Iterator( this, bucketIdx, existing );

        handle.element= nullptr;
        element->fixHashCode( hashCode ); // the key might have been changed outside

        bucketIdx= base::increaseSize( 1, hashCode );
        base::buckets[bucketIdx].pushFront( element );
        return Iterator( this, bucketIdx, element);
    }

    /** ********************************************************************************************
     * Constructs a new element within this container.
     *
     * \note
     *   The use of this method may insert elements sharing the same key as already existing
     *   elements.
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     *
     * @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
     * @param  args   Variadic parameters to be forwarded to constructor of the inserted
     *                element's custom data of #ValueType.
     * @return An iterator referring to the element added.
     **********************************************************************************************/
    template<typename... TArgs>
    Iterator        Emplace( TArgs&&... args )
    {
        // Recycle node or create a new one
        Element* element= base::allocElement( 0 );

        // placement new
        new ( &element->value )  TStored( std::forward<TArgs>(args)... );

        // fix the hash code which was not available at allocation, yet.
        auto hashCode= THash()( base::keyPortion( element ));
        element->fixHashCode( hashCode );

        // insert to hash table
        base::increaseSize( 1 );
        auto bucketIdx=  base::insertInBucket( element, hashCode );
        return Iterator( this, bucketIdx, element);
    }

    /** ********************************************************************************************
     * Constructs a value within this container without checking to place it in the right
     * position in respect to existing elements with the same <em>key-portion</em>.
     *
     * \attention
     *   This method must only be used if the caller guarantees that no other element is
     *   currently stored in this container having an equal <em>key-portion</em>.
     *   In such situations, this method is very efficient.<br>
     *   If one exists already, this hash table is not considered in a consistent state
     *   after the operation. I.e., method #EqualRange will discontinue to function properly.
     *
     * \attention
     *   In debug-compilations, an \alib assertion is raised, if an equal element exists.
     *   For this reason, performance differences to method #Insert will be seen only with
     *   release-compilations.
     *
     * \attention
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     *
     * @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
     * @param  args   Variadic parameters to be forwarded to constructor of the
     *                element to insert whose <em>key-portion</em> has to be different to
     *                all currently contained elements.
     * @return An iterator referring to the element added.
     **********************************************************************************************/
    template<typename... TArgs>
    Iterator        EmplaceUnique( TArgs&&... args )
    {
        // Recycle node or create a new one
        Element* element= base::allocElement(0);

        // placement new
        new ( &element->value )  TStored( std::forward<TArgs>(args)... );

        // replace hash code (which is available only now)
        auto hashCode= THash()( base::keyPortion( element ));
        element->fixHashCode( hashCode );


        // insert to hash table
        auto bucketIdx= base::increaseSize( 1, hashCode );
        base::buckets[bucketIdx].pushFront( element );
        auto result= Iterator( this, bucketIdx, element);

        #if ALIB_DEBUG
            //Check that this was the first of
            auto it= cbegin(result.bucketIdx);
            ALIB_ASSERT( it.element == result.element ) // has to be the first inserted
            while( ++it != cend(result.bucketIdx) )
            {
                ALIB_ASSERT_ERROR( !base::areEqual(result.element, it.element ),
                                   "EmplaceUnique used while element with same key-portion "
                                   "existed!" )
            }
        #endif

        return result;
    }

#if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Replaces an existing, respectively constructs a new element within this container.
     *
     * \note
     *   This method allows to prevent the insertion of double entries.
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     *
     * ## Availability ###
     * This method is available if this templated type is instantiated with
     * \ref alib_namespace_monomem_hashtable_setandmap "hash map mode"
     * or for hash sets that only define a subset of \p{T} as a key type \p{TKey} and whose
     * value type \p{T} is constructible if given a key value and the variadic template arguments.
     *
     *
     * @tparam TEnableIf Used to disable this method where not available.
     * @tparam TArgs     Types of variadic parameters given with parameter \p{args}.
     * @param  key       The key to use for search and insertion.
     * @param  args      Variadic parameters to be forwarded to constructor of the mapped object
     *                   of type <c>\p{MappedType}</c>.
     * @return A pair containing an iterator referring to the element added.
     *         The bool component is \c true if the insertion took place and \c false if the
     *         assignment took place.
     **********************************************************************************************/
    template<typename TEnableIf= TIfMapped, typename... TArgs>
    inline
    EmplaceOrAssign( const KeyType& key, TArgs&&... args);
#else
    template<typename TEnableIf_MapMode= TIfMapped, typename... TArgs>
    ATMP_T_IF(std::pair<Iterator ALIB_COMMA bool>,   !ATMP_EQ(TEnableIf_MapMode, void )  )
    EmplaceOrAssign( const KeyType& key, TArgs&&... args)
    {
        // insert to hash table
        std::pair<Iterator, bool> result= base::insertOrGet( key, THash()(key) );

        // if an existing element was found, we have to destruct the currently mapped object
        if( result.second == false )
            monomem::Destruct( &base::mappedPortion( result.first.element )  );

        // if otherwise an insertion took place, we have to copy the key
        else
            new (&base::keyPortion( result.first.element ))  KeyType( key );

        // placement new for the mapped object
        new ( &base::mappedPortion( result.first.element ))  MappedType( std::forward<TArgs>( args)... );

        return result;
    }

    template<typename TEnableIf_SetMode= TIfMapped, typename... TArgs>
    ATMP_T_IF(std::pair<Iterator ALIB_COMMA bool>,    ATMP_EQ(TEnableIf_SetMode, void )
                                                   && std::is_constructible< TStored        ALIB_COMMA
                                                                             const KeyType& ALIB_COMMA
                                                                             TArgs&&...     >::value )
    EmplaceOrAssign( const KeyType& key, TArgs&&... args)
    {
        // insert to hash table
        std::pair<Iterator, bool> result= base::insertOrGet( key );

        // if an existing element was found, we have to destruct the whole object
        if( result.second == false )
            monomem::Destruct( &result.first.element->value );

        // placement new for the whole object
        new (&result.first.element->value) T( key, std::forward<TArgs>( args)...  );

        return result;
    }
#endif

#if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Inserts a new element only if no other element is contained equals to the one
     * that is constructed by \p{args}.
     *
     * \note
     *   This method allows to prevent the insertion of double entries.
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     * <p>
     * \note
     *   For comparison, a local object of type \p{T} is constructed. In case an equal
     *   object exists, it is destructed. Otherwise it is moved into this container.
     *
     * ## Availability ###
     * This method is available only if this templated type is instantiated with
     * \ref alib_namespace_monomem_hashtable_setandmap "hash set mode". For <em>hash map mode</em>,
     * use overloaded version #EmplaceIfNotExistent(const KeyType& key, TArgs&&... args).
     *
     * Furthermore is available only if custom type \p{T} has a move constructor.
     *
     * \attention
     *   If a move constructor exists, but is not duly defined, the method produces undefined
     *   behavior.<br>
     *   An alternative version that does not require a move constructor is found with
     *   #EmplaceIfNotExistent(const KeyType& key, TArgs&&... args). This can be used for hash sets
     *   that define a subset of \p{T} as a key type \p{TKey} and whose value type \p{T}
     *   is constructible from a constant <em>key-portion</em> and the variadic template arguments.
     *
     * @tparam TEnableIf Used to disable this method where not available.
     * @tparam TArgs     Types of variadic parameters given with parameter \p{args}.
     * @param  args      Variadic parameters to be forwarded to constructor of \p{T}.
     * @return A pair containing an iterator referencing either the element found or the new
     *         element added.
     *         The bool component is \c true if the insertion took place and \c false nothing
     *         was changed.
     **********************************************************************************************/
    template<typename TEnableIf= TIfMapped, typename... TArgs>
    inline
    EmplaceIfNotExistent( TArgs&&... args);
#else
    template<typename TEnableIf= TIfMapped, typename... TArgs>
    ATMP_T_IF(std::pair<Iterator ALIB_COMMA bool>,     ATMP_EQ( TEnableIf, void )
                                                    && std::is_move_constructible<T>::value )
    EmplaceIfNotExistent( TArgs&&... args)
    {
        T value( std::forward<TArgs>( args)... );

        // search element
        std::pair<Iterator, bool> result= base::insertIfNotExists( TAccess().Key(value),
                                                                   THash()(TAccess().Key(value)) );

        // existed? Do nothing
        if( result.second == false )
            return result;

        // placement new moving the locally created object
        new ( &result.first.element->value )   T( std::move(value) );

        return result;
    }
#endif

#if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Inserts a new mapped object only if no other object is contained that is associated
     * already with a key that equals the given \p{key}.
     *
     * \note
     *   This method allows to prevent the insertion of double entries.
     *   For more information, see
     *   \ref alib_namespace_monomem_hashtable_single_multi "Single And Multiple Entries"
     *   of the documentation of this class.
     *
     * ## Availability ###
     * This method is available if this templated type is instantiated with
     * \ref alib_namespace_monomem_hashtable_setandmap "hash map mode"
     * or for hash sets that only define a subset of \p{T} as a key type \p{TKey} and whose
     * value type \p{T} is constructible if given a key value and the variadic template arguments.
     *
     * @tparam TEnableIf Used to disable this method where not available.
     * @tparam TArgs     Types of variadic parameters given with parameter \p{args}.
     * @param  key       The key to use for search and insertion.
     * @param  args      Variadic parameters to be forwarded to constructor of the mapped object
     *                   of type <c>\p{MappedType}</c>.
     * @return A pair containing an iterator referencing either the element found or the new
     *         element added.
     *         The bool component is \c true if the insertion took place and \c false nothing
     *         was changed.
     **********************************************************************************************/
    template<typename TEnableIf, typename... TArgs>
    inline
    EmplaceIfNotExistent(const KeyType& key, TArgs&&... args);
#else

    template<typename TEnableIf= TIfMapped, typename... TArgs>
    ATMP_T_IF(std::pair<Iterator ALIB_COMMA bool>, !std::is_constructible< TStored        ALIB_COMMA
                                                                           const KeyType& ALIB_COMMA
                                                                           TArgs&&...     >::value )
    EmplaceIfNotExistent( const KeyType& key, TArgs&&... args)
    {
        // search element
        std::pair<Iterator, bool> result= base::insertIfNotExists( key, THash()(key) );

        // existed? Do nothing
        if( result.second == false )
            return result;

        // placement new for the key (using copy constructor)
        new (&base::keyPortion   (result.first.element))     KeyType( key );

        // placement new for the mapped object (using custom constructor)
        new (&base::mappedPortion(result.first.element)) MappedType( std::forward<TArgs>( args)... );

        return result;
    }

    template<typename TEnableIf= TIfMapped, typename... TArgs>
    ATMP_T_IF(std::pair<Iterator ALIB_COMMA bool>, std::is_constructible< TStored         ALIB_COMMA
                                                                          const KeyType&  ALIB_COMMA
                                                                          TArgs&&...      >::value )
    EmplaceIfNotExistent( const KeyType& key, TArgs&&... args)
    {
        // search element
        std::pair<Iterator, bool> result= base::insertIfNotExists( key, THash()(key) );

        // existed? Do nothing
        if( result.second == false )
            return result;

        // placement new for the element passing key and args together
        new (&result.first.element->value) T( key, std::forward<TArgs>( args)...  );

        return result;
    }
#endif

    /** ############################################################################################
     * @name Element Search
     ##@{ ########################################################################################*/
    /** ********************************************************************************************
     * Returns an iterator pointing to the first element of equal key value.
     *
     * \note
     *   The iterator returned may be incremented. It is assured that further elements contained
     *   in this hash table with the same key are consecutively following this first element
     *   returned. However, the iterator does \b not end with the last element of that key.
     *
     * \see
     *    Method #EqualRange, which also returns an iterator pointing behind the last element
     *    with that key.
     *
     *
     * @param  key   The key to search for.
     * @return An iterator pointing to the first element found with equal <em>key-portion</em>,
     *         respectively one being equal to #end, if no element was found with \p{key}.
     **********************************************************************************************/
    Iterator        Find( const KeyType& key )
    {
        auto     hashCode = THash()(key);
        auto     bucketIdx= hashCode % base::bucketCount;
        Element* elem     = base::findElement( bucketIdx, key, hashCode );
        return Iterator( this, elem == nullptr ? base::bucketCount : bucketIdx, elem );
    }

    /** ********************************************************************************************
     * Searches an element.
     * @param  key   The key to search for.
     * @return An iterator pointing to the first element found with equal <em>key-portion</em>,
     *         respectively one being equal to #end, if no element was found with \p{key}.
     **********************************************************************************************/
    ConstIterator   Find( const KeyType& key )                                                 const
    {
        auto     hashCode = THash()(key);
        auto     bucketIdx= hashCode % base::bucketCount;
        Element* elem     = base::findElement( bucketIdx, key, hashCode );
        return ConstIterator( this, elem == nullptr ? base::bucketCount : bucketIdx, elem );
    }

    /** ********************************************************************************************
     * Overloaded version of method \alib{monomem::HashTable,Find(const KeyType&)} which
     * accepts the \p{hashCode} of the given \p{key} as a second parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     * @param  key      The key to search for.
     * @param  hashCode Pre-calculated hash code of \p{key}.
     * @return An iterator pointing to the first element found with equal <em>key-portion</em>,
     *         respectively one being equal to #end, if no element was found with \p{key}.
     **********************************************************************************************/
    Iterator        Find( const KeyType& key, size_t hashCode )
    {
        auto     bucketIdx= hashCode % base::bucketCount;
        Element* elem     = base::findElement( bucketIdx, key, hashCode );
        return Iterator( this, elem == nullptr ? base::bucketCount : bucketIdx, elem );
    }

    /** ********************************************************************************************
     * Overloaded version of method \alib{monomem::HashTable,Find(const KeyType&)const} which
     * accepts the \p{hashCode} of the given \p{key} as a second parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     * @param  key      The key to search for.
     * @param  hashCode Pre-calculated hash code of \p{key}.
     * @return An iterator pointing to the first element found with equal <em>key-portion</em>,
     *         respectively one being equal to #end, if no element was found with \p{key}.
     **********************************************************************************************/
    ConstIterator   Find( const KeyType& key, size_t hashCode )                                const
    {
        auto     bucketIdx= hashCode % base::bucketCount;
        Element* elem     = base::findElement( bucketIdx, key, hashCode );
        return ConstIterator( this, elem == nullptr ? base::bucketCount : bucketIdx, elem );
    }

    /** ********************************************************************************************
     * Tests if an element with given \p{key} is stored in this container.
     * @param  key   The key to search for.
     * @return \c true if this hash table contains at least one element with given
     *         <em>key-portion</em> \p{key}, \c false otherwise.
     **********************************************************************************************/
    bool            Contains( const KeyType& key )                                             const
    {
        auto hashCode= THash()(key);
        return  base::findElement(hashCode % base::bucketCount, key, hashCode )
                != nullptr;
    }

    /** ********************************************************************************************
     * Searches a key and returns a pair of iterators. The first is pointing to the first
     * element of the range, the second is pointing to the first element past the range.
     *
     * If both iterators are equal, the range is empty (both iterators will be equal to #end).
     *
     * @param  key   The key to search for.
     * @return A pair of iterators defining the range of elements with key \p{key}.
     **********************************************************************************************/
    std::pair<Iterator,Iterator>                EqualRange(const KeyType& key )
    {
        return base::findRange( key );
    }

    /** ********************************************************************************************
     * Searches a key and returns a pair of iterators. The first is pointing to the first
     * element of the range, the second is pointing to the first element past the range.
     *
     * If both iterators are equal, the range is empty (both iterators will be equal to #end).
     *
     * @param  key   The key to search for.
     * @return A pair of iterators defining the range of elements with key \p{key}.
     **********************************************************************************************/
    std::pair<ConstIterator,ConstIterator>      EqualRange(const KeyType& key )                const
    {
        return base::findRange( key );
    }

    /** ############################################################################################
     * @name Element Removal
     ##@{ ########################################################################################*/
    /** ********************************************************************************************
     * Extracts the first element found with the given key from the hash table and returns a
     * handle to it.<br>
     * Extracting a element invalidates only the iterators to the extracted element, and preserves
     * the relative order of the elements that are not extracted.
     *
     * Extracting and re-inserting nodes is the only way to change a key of an element without
     * performing reallocation and or destruction/construction.
     *
     * @param  key   The key to search a first element for.
     * @return A handle to an element that allows changes of the formerly stored value.
     *         Changes may include the <em>key-portion</em> of the data stored.
     *         Handles may be passed to one of the overloaded insert methods.
     *         If no element was found, the returned handle is empty.
     **********************************************************************************************/
    ElementHandle      Extract(const KeyType& key )
    {
        return Extract( key, THash()(key) );
    }

    /** ********************************************************************************************
     * Overloaded version of method \alib{monomem::HashTable,Extract(const KeyType&)} which
     * accepts the \p{hashCode} of the given \p{key} as a second parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     * @param  key      The key to search a first element for.
     * @param  hashCode Pre-calculated hash code of \p{key}.
     * @return A handle to an element that allows changes of the formerly stored value.
     *         Changes may include the <em>key-portion</em> of the data stored.
     *         Handles may be passed to one of the overloaded insert methods.
     *         If no element was found, the returned handle is empty.
     **********************************************************************************************/
    ElementHandle      Extract(const KeyType& key, size_t hashCode )
    {
        Node* previous= base::findElementBefore( hashCode % base::bucketCount, hashCode, key );
        if( previous == nullptr )
            return ElementHandle(this, nullptr);

        Element* element= previous->next();
        previous->removeNext();
        --base::size;
        return ElementHandle( this, element );
    }

    /** ********************************************************************************************
     * Extracts the first element found with the given key from the hash table and returns a
     * handle to it.<br>
     * If the iterator was not valid (i.e. #end), the method has undefined behavior.
     * With debug builds an \alib assertion is raised.
     *
     * Extracting a element invalidates only the iterators to the extracted element, and preserves
     * the relative order of the elements that are not extracted.
     *
     * Extracting and re-inserting nodes is the only way to change a key of an element without
     * performing reallocation and or destruction/construction.
     *
     * @param  pos   The position of the element to extract.
     * @return A handle to an element that allows changes of the formerly stored value.
     *         Changes may include the <em>key-portion</em> of the data stored.
     *         Handles may be passed to one of the overloaded insert methods.
     *         If no element was found, the returned handle is empty.
     **********************************************************************************************/
    ElementHandle      Extract( ConstIterator pos )
    {
        ALIB_ASSERT_ERROR(    pos.element        != nullptr
                           && pos.table          != nullptr   , "Illegal iterator." )

        Node*    previous= base::buckets[pos.bucketIdx].findLastBefore( pos.element );
        ALIB_ASSERT_ERROR( previous != nullptr,  "Illegal iterator: Element not found." )

        previous->removeNext();
        --base::size;
        return ElementHandle( this, pos.element );
    }

    /** ********************************************************************************************
     * Erases all elements stored with the given key.
     *
     * @param  key   The key to search elements for deletion.
     * @return The number of elements removed.
     **********************************************************************************************/
    integer             Erase(const KeyType& key )
    {
        return Erase( key, THash()(key) );
    }

    /** ********************************************************************************************
     * Overloaded version of method \alib{monomem::HashTable,Erase(const KeyType&)} which
     * accepts the \p{hashCode} of the given \p{key} as a second parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     *
     * @param  key   The key to search elements for deletion.
     * @param  hashCode Pre-calculated hash code of \p{key}.
     * @return The number of elements removed.
     **********************************************************************************************/
    integer             Erase(const KeyType& key, size_t hashCode )
    {
        // search start
        Node* beforeFirst= base::findElementBefore( hashCode % base::bucketCount, hashCode, key );
        if( beforeFirst == nullptr )
            return 0;

        // search end
        integer count= 0;
        Element* last= static_cast<Element*>(beforeFirst); // OK, because moved in the next line
        do
        {
            Node::moveForward(last);
            last->destruct();
            ++count;
        }
        while(    last->hasNext()
               && base::areEqual( last->next(), key, hashCode ) );

        // erase
        Element* first= beforeFirst->removeRangeBehind( last );
        base::recycle( first, last );
        base::size-= count;
        return count;
    }

    /** ********************************************************************************************
     * Erases the unique element with the given key.
     *
     * \note
     *   This method is slightly more efficient than method #Erase(const KeyType&), as it will
     *   not search for a next element with an equal key.<br>
     *   In debug-compilations, the method asserts that no second element with the same \p{key}
     *   is available.<br>
     *   If this table is supposed to
     *   \ref alib_namespace_monomem_hashtable_single_multi "store only unique elements", the
     *   use of this method is therefore recommended, as an assertions hints to an erorrneous use
     *   of the insertion methods.
     *
     * @param  key   The key to search elements for deletion.
     * @return \c true if an element was found and removed, \c false otherwise.
     **********************************************************************************************/
    bool             EraseUnique( const KeyType& key )
    {
        return EraseUnique( key, THash()(key) );
    }


    /** ********************************************************************************************
     * Overloaded version of method \alib{monomem::HashTable,EraseUnique(const KeyType&)} which
     * accepts the \p{hashCode} of the given \p{key} as a second parameter.
     *
     * See \ref alib_namespace_monomem_hashtable_hashprecalc for use cases of this method.
     *
     * @param  key      The key to search elements for deletion.
     * @param  hashCode Pre-calculated hash code of \p{key}.
     * @return \c true if an element was found and removed, \c false otherwise.
     **********************************************************************************************/
    bool             EraseUnique( const KeyType& key, size_t hashCode )
    {
        Node* before= base::findElementBefore( hashCode % base::bucketCount, hashCode, key );
        if( before == nullptr )
            return false;

        ALIB_ASSERT_ERROR( before->next()->next() == nullptr
                           || !base::areEqual( before->next()->next(), key, hashCode ),
                           "More than one element found matching the given key"          )

        Element* elem= before->removeNext();
        elem->destruct();
        base::recycle( elem );
        --base::size;
        return true;
    }


    /** ********************************************************************************************
     * Removes an element specified by an iterator.<br>
     * If the iterator was not valid (i.e #end), the method has undefined behavior.
     * With debug builds an \alib assertion is raised.
     *
     * The order of the elements that are not erased is preserved, what makes it possible to
     * erase individual elements while iterating through the container.
     *
     * @param  pos The iterator to the element to remove.
     * @return An iterator following the removed element.
     **********************************************************************************************/
    Iterator            Erase( ConstIterator pos )
    {
        ALIB_ASSERT_ERROR(    pos.element        != nullptr
                           && pos.table          != nullptr   , "Illegal iterator." )

        Iterator result( this, pos.bucketIdx, pos.element );
        ++result;

        // search pointer to element before pos
        Node* previous= base::buckets[pos.bucketIdx].findLastBefore( pos.element );
        ALIB_ASSERT_ERROR( previous != nullptr,  "Illegal iterator: Element not found." )


        Element* toDelete= previous->removeNext();
        toDelete->destruct();
        base::recycle( toDelete );
        --base::size;
        return result;
    }

ALIB_WARNINGS_IGNORE_NOTHING_RETURNED
    /** ********************************************************************************************
     * Removes all element from the given position \p{start} to the element
     * before given position \p{end}.
     *
     * The order of the elements that are not erased is preserved, what makes it possible to
     * erase individual elements while iterating through the container.
     *
     * @param  start The iterator to the element to remove.
     * @param  end   the first element not to remove.
     * @return An iterator following the removed element.
     **********************************************************************************************/
    Iterator            Erase( ConstIterator start, ConstIterator end )
    {
        ALIB_ASSERT_ERROR(    start.element        != nullptr
                           && start.table          != nullptr   , "Illegal iterator." )

        ALIB_ASSERT_ERROR( start.table == end.table,
                           "Iterators are referring to different hash tables." )

        if( start.element == end.element )
            return Iterator( this, start.bucketIdx, start.element);

        // loop over all buckets in question
        for( auto bucketIdx= start.bucketIdx; bucketIdx <= end.bucketIdx; ++bucketIdx )
        {
            if( bucketIdx == base::bucketCount )
                return HashTable::end();

            Node* previous;
            if( bucketIdx == start.bucketIdx ) // first bucket?
            {
                // search pointer to element before start
                previous= base::buckets[start.bucketIdx].findLastBefore( start.element );
                ALIB_ASSERT_ERROR( previous != nullptr,  "Illegal iterator: Element not found." )
            }
            else
            {
                if( base::buckets[bucketIdx].isEmpty() )
                    continue;
                previous= base::buckets[bucketIdx].castToNode();
            }
            // previous next not null here

            bool isLastBucketOfRange= bucketIdx == end.bucketIdx;

            // destruct either to end of list or to end-iterator element
            integer  count= 0;
            Element* first= previous->next();    first->destruct();
            Element* endE = isLastBucketOfRange ? end.element : nullptr;
            Element* last = static_cast<Element*>(previous); // allowed, because next line moves
            do
            {
                Node::moveForward(last);
                last->destruct();
                ++count;
            }
            while( last->next() != endE );

            Element* next = last->next();
            previous->removeRangeBehind( last );
            base::recycle( first, last );
            base::size-= count;

            if( isLastBucketOfRange )
            {
                Iterator result= Iterator( this, bucketIdx, next );
                if( result.element == nullptr )
                    result.repair();

                return result;
            }
        }
    }
ALIB_WARNINGS_RESTORE


    /** ********************************************************************************************
     * Removes an element specified by a bucket iterator.
     * Bucket iterators are receivable using overloaded methods #begin(uinteger) and
     * \alib{monomem::HashTable,cbegin(uinteger)const,cbegin(uinteger)}.
     *
     * The order of the elements that are not erased is preserved, what makes it possible to
     * erase individual elements while iterating through the container.
     *
     * @param  pos The iterator to the element to remove.
     * @return An iterator following the removed element.
     **********************************************************************************************/
    LocalIterator       Erase( ConstLocalIterator pos )
    {
        ALIB_ASSERT_ERROR( pos.element != nullptr, "Illegal iterator." )

        LocalIterator result( pos.bucketIdx, pos.element->next() );

        Element* element= pos.element;
        base::buckets[pos.bucketIdx].findAndRemove( element );
        element->destruct();
        base::recycle( element);
        --base::size;

        return result;
    }

    /** ********************************************************************************************
     * Removes all element from the given bucket iterator position \p{start} to the element
     * before given position \p{end}.
     *
     * The order of the elements that are not erased is preserved, what makes it possible to
     * erase individual elements while iterating through the container.
     *
     * @param  start The bucket iterator to the element to remove.
     * @param  end   The bucket iterator to the first element not to remove.
     * @return An iterator following the removed element.
     **********************************************************************************************/
    LocalIterator       Erase( ConstLocalIterator start, ConstLocalIterator end )
    {
        ALIB_ASSERT_ERROR( start.element != nullptr, "Illegal iterator." )

        Node* previous= base::buckets[start.bucketIdx].findLastBefore( start.element);
        ALIB_ASSERT_ERROR( previous != nullptr, "Illegal iterator." )
        if( start.element == end.element )
            return LocalIterator( start.bucketIdx, start.element );

        Element* first= previous->next();
        Element* last = static_cast<Element*>( previous ); // allowed, as we move in next line
        integer  count= 0;
        do
        {
            Node::moveForward(last);
            last->destruct();
            ++count;
        }
        while( last->next() != end.element );

        Element* next = last->next();

        previous->removeRangeBehind( last );
        base::recycle( first, last );

        base::size-= count;

        return LocalIterator( start.bucketIdx, next );
    }

    /** ############################################################################################
     * @name std::iterator Interface
     ##@{ ########################################################################################*/

    /** Returns an iterator referring to a mutable element at the start of this table.
     *  @return The first of element in this container.                           */
    Iterator            begin()         { return Iterator     ( this, 0 );     }

    /** Returns an iterator referring to a mutable, non-existing element.
     *  @return The end of the list of elements in this container.                 */
    Iterator            end()           { return Iterator    ( this, base::bucketCount, nullptr ); }

    /** Returns an iterator referring to a constant element at the start of this container.
     *  @return The first of element in this container.                           */
    ConstIterator       begin()   const { return ConstIterator( this, 0 ); }

    /** Returns an iterator referring to a constant, non-existing element.
     *  @return The end of the list of elements in this container.                 */
    ConstIterator       end()     const { return ConstIterator( this, base::bucketCount, nullptr); }

    /** Returns an iterator referring to a constant element at the start of this container.
     *  @return The first of element in this container.                           */
    ConstIterator       cbegin()  const { return ConstIterator( this, 0 ); }

    /** Returns an iterator referring to a constant, non-existing element.
     *  @return The end of the list of elements in this container.                 */
    ConstIterator       cend()    const { return ConstIterator( this, base::bucketCount, nullptr); }



    /** Returns an iterator referring to a mutable element at the start of bucket of index
     *  \p{bucketNumber}.
     *  @param bucketNumber The bucket to iterate on.
     *  @return The first element in bucket \p{bucketNumber}.              */
    LocalIterator       begin( uinteger bucketNumber )
    {
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount,  "Bucket number out of range." )
        return LocalIterator( bucketNumber, base::buckets[bucketNumber].start() );
    }

    /** Returns an iterator referring to a constant, non-existing element in bucket of index
     *  \p{bucketNumber}.
     *  @param bucketNumber The bucket to iterate on.
     *  @return The end of the list of elements in bucket \p{bucketNumber}.  */
    LocalIterator       end( uinteger bucketNumber )
    {
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount,  "Bucket number out of range." )
        return LocalIterator( bucketNumber, nullptr );
    }

    /** Returns an iterator referring to a mutable element at the start of bucket of index
     *  \p{bucketNumber}.
     *  @param bucketNumber The bucket to iterate on.
     *  @return The first element in bucket \p{bucketNumber}.              */
    ConstLocalIterator  begin( uinteger bucketNumber )                                         const
    {
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount,  "Bucket number out of range." )
        return ConstLocalIterator( bucketNumber, base::buckets[bucketNumber].start() );
    }

    /** Returns an iterator referring to a constant, non-existing element in bucket of index
     *  \p{bucketNumber}.
     *  @param bucketNumber The bucket to iterate on.
     *  @return The end of the list of elements in bucket \p{bucketNumber}.  */
    ConstLocalIterator  end( uinteger bucketNumber )                                           const
    {
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount,  "Bucket number out of range." )
        return ConstLocalIterator( bucketNumber, nullptr );
    }

    /** Returns an iterator referring to a mutable element at the start of bucket of index
     *  \p{bucketNumber}.
     *  @param bucketNumber The bucket to iterate on.
     *  @return The first element in bucket \p{bucketNumber}.              */
    ConstLocalIterator  cbegin( uinteger bucketNumber )                                        const
    {
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount,  "Bucket number out of range." )
        return ConstLocalIterator( bucketNumber, base::buckets[bucketNumber].start() );
    }

    /** Returns an iterator referring to a constant, non-existing element in bucket of index
     *  \p{bucketNumber}.
     *  @param bucketNumber The bucket to iterate on.
     *  @return The end of the list of elements in bucket \p{bucketNumber}.  */
    ConstLocalIterator  cend( uinteger bucketNumber )                                          const
    {
        ALIB_ASSERT_ERROR( bucketNumber < base::bucketCount,  "Bucket number out of range." )
        return ConstLocalIterator( bucketNumber, nullptr );
    }

};


}}}  // namespace [aworx::lib::monomem]


// #################################################################################################
// #################################################################################################
// Debug Methods
// #################################################################################################
// #################################################################################################

#if ALIB_DEBUG_MONOMEM

#if ALIB_RESULTS
#   if !defined(HPP_ALIB_RESULTS_REPORT)
#      include "alib/results/report.hpp"
#   endif
#endif

#if ALIB_TEXT
#   if !defined(HPP_ALIB_TEXT_FORMATTER)
#      include "alib/text/formatter.hpp"
#   endif
#endif


namespace aworx { namespace lib { namespace monomem {

/**
 * Generates statistics on the given hash table. The meanings of the returned tuple are:
 * 0. The expected average size of a bucket (simply table size divided by number of buckets).
 * 1. The <em>standard deviation</em> of the buckets. The lower this value, the better
 *    is the hash algorithm used. A value of <c>1.0</c> denotes the <em>gaussian distribution</em>
 *    which indicates perfect randomness. However, this value is unlikely (impossible) to be
 *    achieved.
 * 2. The minimum number of elements found in a bucket.
 * 3. The maximum number of elements found in a bucket.
 *
 * ### Availability ###
 * Available only if compiler symbol \ref ALIB_DEBUG_MONOMEM is set.
 *
 * \see
 *   Sibling namespace functions \alib{monomem,DbgDumpDistribution} and
 *   \alib{monomem,DbgDumpHashtable} provided for debugging and optimization.
 *
 * @tparam THashtable A specification  of templated type \alib{monomem,HashTable}.
 *                    Deduced by the compiler by given parameter \p{hashtable}.
 * @param  hashtable  The hashtable to investigate on.
 * @return The tuple as described above.
 */
template<typename THashtable>
inline
std::tuple<double,double,integer,integer>
DbgGetHashTableDistribution(const THashtable& hashtable )
{
    auto     qtyBuckets      = hashtable.BucketCount();
    double   averageExpected =   static_cast<double>( hashtable.Size() )
                              / static_cast<double>( qtyBuckets       ) ;
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

        double diff= averageExpected - bucketSize;
        diffs+=  diff > 0 ?  diff : - diff;
    }

    #if ALIB_RESULTS
        ALIB_ASSERT_ERROR( sumCheck == hashtable.Size(),
                           "Error: Hashtable::Size() and sum of bucket sizes differ: {} != {}",
                           hashtable.Size(), sumCheck )
    #else
        ALIB_ASSERT_ERROR( sumCheck == hashtable.Size(),
                           "Error: Hashtable::Size() and sum of bucket sizes differ" )
    #endif
    double deviation= diffs/qtyBuckets;

    return std::make_tuple( averageExpected, deviation, minimum, maximum );
}

#if ALIB_TEXT
/**
 * Invokes method \alib{monomem,DbgGetHashTableDistribution} and creates human readable output, ready to be
 * logged or written to the console.
 *
 * ### Availability ###
 * Available only if compiler symbol \ref ALIB_DEBUG_MONOMEM is set and module \alib_text
 * is included in the alibdist.
 *
 * \see
 *   Sibling namespace functions \alib{monomem,DbgGetHashTableDistribution} and
 *   \alib{monomem,DbgDumpHashtable} provided for debugging and optimization.
 *
 * @tparam THashtable          A specification  of templated type \alib{monomem,HashTable}.
 *                             Deduced by the compiler by given parameter \p{hashtable}.
 * @param  hashtable           The hashtable to investigate on.
 * @param  detailedBucketList  If \c true is given, for each bucket a line with its size value and
 *                             a "size bar" is written.
 *
 * @return A string containing human readable information about the distribution of elements
 *         in the hashtable.
 */
template<typename THashtable>
inline
AString DbgDumpDistribution(const THashtable& hashtable, bool detailedBucketList )
{
    auto values = DbgGetHashTableDistribution( hashtable );
    AString result;
    double  loadFactor= std::get<0>( values );
    double  deviation = std::get<1>( values );
    integer minSize   = std::get<2>( values );
    integer maxSize   = std::get<3>( values );
    auto formatter = aworx::Formatter::AcquireDefault(ALIB_CALLER_PRUNED);
        // statistics
        formatter->Format( result, "Size:        {}\\n"
                                   "#Buckets:    {}\\n"
                                   "Load Factor: {:.02}  (Base: {:.01}  Max: {:.01}) \\n"
                                   "Deviation:   {:.02} (~{:%.1})\\n"
                                   "Minimum:     {}\\n"
                                   "Maximum:     {}\\n",
                           hashtable.Size(),
                           hashtable.BucketCount(),
                           loadFactor, hashtable.BaseLoadFactor(), hashtable.MaxLoadFactor(),
                           deviation , ( hashtable.Size() != 0
                                                      ? deviation / loadFactor
                                                      : 0.0                                          ),
                           minSize, maxSize );


        // bucket filling statistics
        integer* bucketFills= new integer[static_cast<size_t>(maxSize + 1)];
        for( integer i= 0; i < maxSize ; ++i)
            bucketFills[i]= 0;

        for( uinteger i= 0; i < hashtable.BucketCount() ; ++i)
            ++bucketFills[hashtable.BucketSize(i)];

        formatter->Format( result, "Bucket Fills:  Size    #Buckets\n" );
        formatter->Format( result, "              -----------------\n" );
        for( integer i= 0; i < maxSize ; ++i)
            formatter->Format( result, "               {}      {}\\n", i, bucketFills[i] );
        delete[] bucketFills;


        // detailed bucked list
        if( detailedBucketList )
        {
            formatter->Format(result, "\nDetailed Bucket List:\n");
            auto    qtyBuckets      = hashtable.BucketCount();
            for( uinteger i= 0 ; i < qtyBuckets ; ++i )
            {
                auto bucketSize= hashtable.BucketSize( i );
                formatter->Format(result, "{:3} ({:2}): {!FillCX}\\n", i, bucketSize,
                                  static_cast<int>(bucketSize) );
            }

        }
    formatter->Release();
    return result;
}

/**
 * Dumps all values of the hash table sorted by buckets.
 * Besides other scenarios of usage, this method allows to investigate into how the keys of
 * the table are distributed in the buckets, and thus learn something about the hash algorithm used.
 *
 * Prior to invoking this method, specializations of \alib{strings,T_Append} have to be made
 * and furthermore, boxed values of the type have to be <em>"made appendable"</em> to
 * instances of type \b %AString. The latter is rather simple, if done using
 * \ref ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE "this macro" during bootstrap.
 *
 * \note
 *   If the prerequisites for using this method seem to be too complicated and not worth the
 *   effort for a "quick debug session", it is recommended to just copy the source code of this
 *   inline function and adopt the \alib{text,Formatter::Format} statement to
 *   suit a specific type stored in \p{hashtable}.
 *
 * ### Availability ###
 * Available only if compiler symbol \ref ALIB_DEBUG_MONOMEM is set and module \alib_text
 * is included in the alibdist.
 *
 * \see
 *   Sibling namespace functions \alib{monomem,DbgGetHashTableDistribution} and
 *   \alib{monomem,DbgDumpDistribution} provided for debugging and optimization.
 *
 * @tparam THashtable          A specification  of templated type \alib{monomem,HashTable}.
 *                             Deduced by the compiler by given parameter \p{hashtable}.
 * @param  hashtable           The hashtable to investigate on.
 * @return A string containing a dump of the given hash table's contents.
 */
template<typename THashtable>
inline
AString DbgDumpHashtable(const THashtable& hashtable )
{
    AString result;
    auto formatter= aworx::Formatter::AcquireDefault(ALIB_CALLER_PRUNED);
        formatter->Format(result, "\nHashtable dump:\n");
        auto    qtyBuckets      = hashtable.BucketCount();
        for( uinteger i= 0 ; i < qtyBuckets ; ++i )
        {
            auto bucketSize= hashtable.BucketSize( i );
            formatter->Format(result, "{:3} ({:2}): ", i, bucketSize );

            int entryNo= 0;
            for( auto  bucketIt=   hashtable.begin(i)
                    ;  bucketIt != hashtable.end  (i)
                    ;++bucketIt )
            {
               if( entryNo!=0)
                   result << "          ";
               formatter->Format(result, "{}: {}\\n", entryNo, *bucketIt );
               ++entryNo;
            }
            if( bucketSize == 0)
                result << "---" << NewLine();
        }

    formatter->Release();
    return result;
}

#endif  // ALIB_TEXT

}}}  // namespace [aworx::lib::monomem]

#endif  // ALIB_DEBUG_MONOMEM


#endif // HPP_ALIB_MONOMEM_HASHTABLE
