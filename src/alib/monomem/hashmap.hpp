/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_HASHMAP
#define HPP_ALIB_MONOMEM_HASHMAP 1

#if !defined(HPP_ALIB_MONOMEM_HASHTABLE)
#   include "alib/monomem/hashtable.hpp"
#endif

#if ALIB_STRINGS && !defined (HPP_ALIB_COMPATIBILITY_STD_STRINGS_FUNCTIONAL)
    #include "alib/compatibility/std_strings_functional.hpp"
#endif

namespace alib {  namespace monomem {

namespace detail {
/**
 * Helper struct used for implementing \alib{monomem,HashMap}.
 * This type is used as template parameter \p{TAccess} of \alib{monomem,HashTable}.
 * @tparam TKey    The key type.
 * @tparam TMapped The type of the mapped objects.
 */
template<typename TKey, typename TMapped>
struct HashMapAccess
{
    /**
     * Returns the first element of a <c>std::pair</c>.
     * @param src The value of the element to hash.
     * @return The key-portion of the stored value.
     */
    TKey&       Key(std::pair<TKey, TMapped>& src)                                             const
    {
        return src.first;
    }

    /**
     * Returns the second element of a <c>std::pair</c>.
     * @param src The value of the element to hash.
     * @return The mapped-portion of the stored value.
     */
    TMapped&    Mapped(std::pair<TKey, TMapped>& src)                                          const
    {
        return src.second;
    }
};

} // namespace alib::monomem[::detail]

/** ************************************************************************************************
 * This type definition is a shortcut to \alib{monomem,HashTable}, usable if the data stored
 * in the container consists of two parts, a <em>key</em> and a <em>mapped</em> part, where only
 * the key-part is to be used for the comparison of values.
 *
 * \see
 *   For a detailed description of this type, see original type \alib{monomem,HashTable}.<br>
 *   Another type definition based on \b %HashTable is provided with \alib{monomem,HashSet}.
 *
 * @tparam TKey         The type of the <em>key-portion</em> of the inserted data.<br>
 *                      This type is published as \alib{monomem,HashTable::KeyType}.
 * @tparam TMapped      The type of the <em>mapped-portion</em> of the inserted data.<br>
 *                      This type is published as \alib{monomem,HashTable::MappedType}.
 * @tparam THash        The hash functor applicable to \p{TKey}.<br>
 *                      Defaults to <c>std::hash<TKey></c> and is published as
 *                      \alib{monomem,HashTable::HashType}.
 * @tparam TEqual       The comparison functor on \p{TKey}.<br>
 *                      Defaults to <c>std::equal_to<TKey></c> and is published as
 *                      \alib{monomem,HashTable::EqualType}.
 * @tparam THashCaching Determines if hash codes are cached when elements are inserted.<br>
 *                      Defaults to <b>Caching::Auto</b>, which enables caching if
 *                      <c>std::is_arithmetic<TKey>::value</c> evaluates to \c false.
 * @tparam TRecycling   Denotes the type of recycling that is to be performed. Possible values are
 *                      \alib{monomem,Recycling::Private} (the default),
 *                      \alib{monomem,Recycling::Shared} or \alib{monomem,Recycling::None}.
 **************************************************************************************************/
template< typename      TKey,
          typename      TMapped,
          typename      THash        = std::hash    <TKey>,
          typename      TEqual       = std::equal_to<TKey>,
          lang::Caching THashCaching = lang::Caching::Auto,
          typename TRecycling   = Recycling::Private   >
using HashMap= monomem::HashTable<std::pair<const TKey, TMapped>,
                                       std::pair<      TKey, TMapped>,
                                       TKey, TMapped,
                                       THash,
                                       TEqual,
                                       monomem::detail::HashMapAccess<TKey, TMapped>,
                                       THashCaching,
                                       TRecycling                                           >;

}// namespace alib[::monomem]

/// Type alias in namespace \b alib. See type definition \ref alib::monomem::HashMap.
template< typename      TKey,
          typename      TMapped,
          typename      THash        = std::hash    <TKey>,
          typename      TEqual       = std::equal_to<TKey>,
          lang::Caching THashCaching = lang::Caching::Auto,
          typename TRecycling        = monomem::Recycling::Private >
using HashMap= monomem::HashMap<TKey, TMapped,THash,TEqual, THashCaching, TRecycling >;

} // namespace [alib]

#endif // HPP_ALIB_MONOMEM_HASHMAP
