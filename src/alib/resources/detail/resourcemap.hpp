/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_resources of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_RESOURCES_DETAIL_RESOURCEMAP
#define HPP_ALIB_RESOURCES_DETAIL_RESOURCEMAP 1

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif
#if !defined (HPP_ALIB_MONOMEM_HASHMAP)
#   include "alib/monomem/hashmap.hpp"
#endif

namespace aworx  { namespace lib { namespace resources {

/**
 * Internal details of namespace #aworx::lib::config.
 */
namespace detail {

/** Key type for hashing resource values. */
struct Key
{
    NString    Category;    ///< The resource category.
    NString    Name;        ///< The resource name.

    /** Hash functor for objects of type \b Key. */
    struct Hash
    {
        /**
         * Calculates a hash code.
         * @param key The object to hash.
         * @return The hash code.
         */
        std::size_t operator()(const Key& key)                                                 const
        {
            return   key.Name    .Hashcode()
                   ^ key.Category.Hashcode();
        }
    };

    /** Equality functor for objects of type \b Key. */
    struct EqualTo
    {
        /** Compares two objects of type \b OperatorKey.
         *  @param lhs The left-hand side object.
         *  @param rhs The left-hand side object.
         *  @return The result of the comparison.                      */
        bool        operator()(const Key& lhs, const Key& rhs )                                const
        {
            return     lhs.Name    .Equals( rhs.Name    )
                    && lhs.Category.Equals( rhs.Category);
        }
    };
};

/** Hash map type used to store static resources. Key and value strings reference static
 *  string buffers.                                                                       */
using StaticResourceMap  = HashMap<Key,
                        #if !ALIB_DEBUG_RESOURCES
                                   String,
                        #else
                          std::pair<String,    integer>,
                        #endif
                                   Key::Hash,
                                   Key::EqualTo,
                                   Caching::Enabled,
                                   Recycling::None    >;

/** Hash map type used to store cached/persistent resources. The value strings is a
 *  local string of length \c 32. The key strings have to probably made persistent as well, but
 *  this is an obligation of the user of this type.                                             */
using CachingResourceMap = HashMap<Key,
                        #if !ALIB_DEBUG_RESOURCES
                                   strings::TLocalStringNoWarning<character, 32>,
                        #else
                          std::pair<strings::TLocalStringNoWarning<character, 32>,    integer>,
                        #endif
                                   Key::Hash,
                                   Key::EqualTo,
                                   Caching::Enabled,
                                   Recycling::None    >;


}}}} // namespace [aworx::lib::resources::detail]



#endif // HPP_ALIB_RESOURCES_DETAIL_RESOURCEMAP
