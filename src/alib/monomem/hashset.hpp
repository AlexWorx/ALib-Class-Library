/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_HASHSET
#define HPP_ALIB_MONOMEM_HASHSET 1

#if !defined(HPP_ALIB_MONOMEM_HASHTABLE)
#   include "alib/monomem/hashtable.hpp"
#endif


namespace alib {  namespace monomem { namespace detail {

/**
 * Helper struct used for implementing \alib{monomem,HashSet}.
 * This type is used as template parameter \p{TAccess} of \alib{monomem,HashTable}.
 *
 * @tparam TStored    The object type stored in the set, which is the same as the key type.
 */
template<typename TStored>
struct HashSetAccess
{
    /** Returns the given \p{src} as is.
     *  @param src The value of the element to hash.
     *  @return The key-portion of the stored value.          */
    TStored&       Key(TStored& src)                                                           const
    {
        return src;
    }

    /** This will never be called.   */
    void           Mapped(TStored&)                                                            const
    {}
};

} // namespace alib::monomem[::detail]

/** ************************************************************************************************
 * This type definition is a shortcut to \alib{monomem,HashTable}, usable if the full portion of
 * the data stored in the container is used for the comparison of values.
 *
 * \note
 *   As with this definition template type \p{TKey} equals inserted type \p{T}, methods of
 *   target type \alib{monomem,HashTable} that accept an object of template type \b TKey
 *   expect an object of \p{T} when this type is used.
 *
 * \see
 *   For a detailed description of this type, see original type \alib{monomem,HashTable}.<br>
 *   Another type definition based on \b %HashTable is provided with \alib{monomem,HashMap}.
 *
 * @tparam T            The element type stored with this container.
 *                      This type is published as \alib{monomem,HashTable::ValueType} and type
 *                      definition \alib{monomem,HashTable::KeyType} becomes a synonym.
 * @tparam THash        The hash functor applicable to \p{TKey}.<br>
 *                      Defaults to <c>std::hash<TKey></c> and is published as
 *                      \alib{monomem,HashTable::HashType}.
 * @tparam TEqual       The comparison functor on \p{TKey}.<br>
 *                      Defaults to <c>std::equal_to<TKey></c> and is published as
 *                      \alib{monomem,HashTable::EqualType}.
 * @tparam THashCaching Determines if hash codes are cached when elements are inserted.<br>
 *                      Defaults to <b>Caching::Auto</b>, which enables caching if
 *                      <c>std::is_arithmetic<T>::value</c> evaluates to \c false.
 * @tparam TRecycling   Denotes the type of recycling that is to be performed. Possible values are
 *                      \alib{monomem,Recycling::Private} (the default),
 *                      \alib{monomem,Recycling::Shared} or \alib{monomem,Recycling::None}.
 **************************************************************************************************/
template< typename      T,
          typename      THash        = std::hash    <T>,
          typename      TEqual       = std::equal_to<T>,
          lang::Caching THashCaching = lang::Caching::Auto,
          typename TRecycling   = Recycling::Private >
using HashSet= monomem::HashTable< T, T,
                                       T, void,
                                       THash,
                                       TEqual,
                                       monomem::detail::HashSetAccess<T>,
                                       THashCaching,
                                       TRecycling                                      >;



} // namespace alib[::monomem]

/// Type alias in namespace \b alib. See type definition \ref alib::monomem::HashSet.
template< typename      T,
          typename      THash        = std::hash    <T>,
          typename      TEqual       = std::equal_to<T>,
          lang::Caching THashCaching = lang::Caching::Auto,
          typename TRecycling        = monomem::Recycling::Private >
using HashSet= monomem::HashSet<T, THash, TEqual, THashCaching, TRecycling >;


} // namespace [alib]

#endif // HPP_ALIB_MONOMEM_HASHSET
