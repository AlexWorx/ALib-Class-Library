//==================================================================================================
/// This header file is part of sub-namespace #alib::lang::resources of module \alib_basecamp of
/// the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LANG_RESOURCES_DETAIL_RESOURCEMAP
#define HPP_ALIB_LANG_RESOURCES_DETAIL_RESOURCEMAP 1
#pragma once
#include "alib/monomem/monoallocator.hpp"
#include "alib/containers/hashtable.hpp"

namespace alib::lang::resources {

/// Internal details of namespace #alib::lang::resources.
namespace detail {

/// Key type for hashing resource values.
struct Key
{
    NString    Category;    ///< The resource category.
    NString    Name;        ///< The resource name.

    /// Hash functor for objects of type \b Key.
    struct Hash
    {
        /// Calculates a hash code.
        /// @param key The object to hash.
        /// @return The hash code.
        std::size_t operator()(const Key& key)                                                 const
        {
            return   key.Name    .Hashcode()
                   ^ key.Category.Hashcode();
        }
    };

    /// Equality functor for objects of type \b Key.
    struct EqualTo
    {
        /// Compares two objects of type \b OperatorKey.
        /// @param lhs The left-hand side object.
        /// @param rhs The left-hand side object.
        /// @return The result of the comparison.
        bool        operator()(const Key& lhs, const Key& rhs )                                const
        {
            return     lhs.Name    .Equals<NC>( rhs.Name    )
                    && lhs.Category.Equals<NC>( rhs.Category);
        }
    };
};

/// Hash map type used to store static resources. Key and value strings reference static
/// string buffers.
using StaticResourceMap  = HashMap<MonoAllocator,
                                   Key,
                        #if !ALIB_DEBUG_RESOURCES
                                   String,
                        #else
                          std::pair<String,    integer>,
                        #endif
                                   Key::Hash,
                                   Key::EqualTo,
                                   lang::Caching::Enabled,
                                   Recycling::None    >;

}} // namespace [alib::lang::resources::detail]



#endif // HPP_ALIB_LANG_RESOURCES_DETAIL_RESOURCEMAP

