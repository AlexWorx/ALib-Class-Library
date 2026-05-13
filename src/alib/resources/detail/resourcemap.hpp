//==================================================================================================
/// This header-file is part of module \alib_resources of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
namespace alib::resources {

/// Internal details of namespace #"alib::resources;2".
namespace detail {

/// Key type for hashing resource values.
struct Key {
    NString    Category;    ///< The resource category.
    NString    Name;        ///< The resource name.

    /// Hash functor for objects of type #"%resources::detail::Key".
    struct Hash {
        /// Calculates a hash code.
        /// @param key The object to hash.
        /// @return The hash code.
        std::size_t operator()(const Key& key)                                               const {
            return   key.Name    .Hashcode()
                   ^ key.Category.Hashcode();
        }
    };

    /// Equality functor for objects of type #"%resources::detail::Key".
    struct EqualTo {
        /// Compares two objects of type #"%OperatorKey".
        /// @param lhs The left-hand side object.
        /// @param rhs The left-hand side object.
        /// @return The result of the comparison.
        bool        operator()(const Key& lhs, const Key& rhs )                              const {
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

}} // namespace [alib::resources::detail]
