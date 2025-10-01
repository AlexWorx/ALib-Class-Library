//==================================================================================================
/// \file
/// This header-file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::containers {

/// Implements template type \p{TValueDescriptor}, which is, for example, offered by types
/// \alib{containers;HashTable} and \alib{containers;LRUCacheTable}.
///
/// Specifically, this implementation is used when the custom type to be stored with a container
/// should be associated to a \p{TKkey}-type, which is not included in the stored type itself.
/// Consequently, to associate custom type \p{TMapped} with the key, the #StoredType
/// results in <c>std::pair<TKey,TMapped</c>.
///
/// Type definitions \alib{containers;HashMap} and \alib{containers;LRUCacheMap}
/// establish a shortcut to their corresponding base type, which incorporates this helper.
/// @tparam TKey    The key type.
/// @tparam TMapped The type of the mapped objects.
template<typename TKey, typename TMapped>
struct TPairDescriptor
{
    /// The type stored in the container.<br>
    /// Container types typically publish an alias to this type.
    /// For example, see \alib{containers::LRUCacheTable;StoredType}, or
    /// see \alib{containers::HashTable;StoredType}.
    using StoredType    = std::pair<TKey, TMapped>;

    /// The key type. (Exposes template parameter \p{TKey}).<br>
    /// Container types typically publish an alias to this type.
    /// For example, see \alib{containers::LRUCacheTable;KeyType}, or
    /// see \alib{containers::HashTable;KeyType}.
    using KeyType       = TKey;

    /// The type associated to a key. (Exposes template parameter \p{TMapped}).
    /// The type of the mapped portion of the data.<br>
    /// Container types typically publish an alias to this type.
    /// For example, see \alib{containers::LRUCacheTable;MappedType}, or
    /// see \alib{containers::HashTable;MappedType}.
    using MappedType    = TMapped;

    /// Returns the first element of the given <c>std::pair</c>.
    /// @param src The value to extract from.
    /// @return The key-portion of the given value.
    TKey&       Key(std::pair<TKey, TMapped>& src)                      const { return src.first;  }

    /// Returns the second element of the given <c>std::pair</c>.
    /// @param src The value to extract from.
    /// @return The mapped-portion of the given value.
    TMapped&    Mapped(std::pair<TKey, TMapped>& src)                   const { return src.second; }
};

/// Implements template type \p{TValueDescriptor}, which is, for example, offered by types
/// \alib{containers;HashTable} and \alib{containers;LRUCacheTable}.
///
/// Specifically, this implementation is used with "sets", hence cases where the full portion
/// of a type that is to be stored in a container should serve as the key to itself.
///
/// Type definitions \alib{containers;HashSet} and \alib{containers;LRUCacheSet}
/// establish a shortcut to their corresponding base type, which incorporates this helper.
///
/// @tparam T   The type stored in the container, serving likewise as the key-type.
template<typename T>
struct TIdentDescriptor
{
    /// The type stored in the container.<br>
    /// Container types typically publish an alias to this type.
    /// For example, see \alib{containers::LRUCacheTable;StoredType}, or
    /// see \alib{containers::HashTable;StoredType}.
    using StoredType    = T;

    /// Exposes template parameter \p{T} and thus equals #StoredType and
    /// #MappedType.<br>
    /// Container types typically publish an alias to this type.
    /// For example, see \alib{containers::LRUCacheTable;KeyType}, or
    /// see \alib{containers::HashTable;KeyType}.
    using KeyType       = T;

    /// Exposes template parameter \p{T} and thus equals #StoredType and
    /// #KeyType.<br>
    /// Container types typically publish an alias to this type.
    /// For example, see \alib{containers::LRUCacheTable;MappedType}, or
    /// see \alib{containers::HashTable;MappedType}.
    using MappedType    = T;

    /// Returns the given \p{src} as is.
    /// @param src The value to extract from.
    /// @return The given reference.
    KeyType&        Key   (T& src)                                             const { return src; }

    /// Returns the given \p{src} as is.
    /// @param src The value to extract from.
    /// @return The given reference.
    MappedType&     Mapped(T& src)                                             const { return src; }
};


/// Implements template type \p{TValueDescriptor}, which is, for example, offered by types
/// \alib{containers;HashTable} and \alib{containers;LRUCacheTable}.
///
/// Specifically, this implementation causes a container neither to add a specific key-type to
/// every stored custom object nor to expect instances of a custom object as the key-type itself.
/// Instead, only a subset of the custom type is used as the key-type.
/// @tparam T    The type stored in the container.
/// @tparam TKey The key type which is to be extracted out of \p{T} by method #Key.
template<typename T, typename TKey>
struct TSubsetKeyDescriptor
{
    /// Exposes template parameter \p{T}.<br>
    /// Container types typically publish an alias to this type.
    /// For example, see \alib{containers::LRUCacheTable;StoredType}, or
    /// see \alib{containers::HashTable;StoredType}.
    using StoredType    = T;

    /// Exposes template parameter \p{TKey}.<br>
    /// Container types typically publish an alias to this type.
    /// For example, see \alib{containers::LRUCacheTable;KeyType}, or
    /// see \alib{containers::HashTable;KeyType}.
    using KeyType       = TKey;

    /// Exposes template parameter \p{T} and thus equals #StoredType.<br>
    /// Container types typically publish an alias to this type.
    /// For example, see \alib{containers::LRUCacheTable;MappedType}, or
    /// see \alib{containers::HashTable;MappedType}.
    using MappedType    = T;

    /// A custom implementation has to return the key-portion of given \p{src}.
    /// \attention This method is just declared. A type-specific implementation has to be
    ///            provided by the using code!
    /// @param src The stored value to extract the key from.
    /// @return The given reference.
    KeyType&        Key   (T& src);

    /// Returns the given \p{src} as is.
    /// @param src The stored value to extract the mapped portion from.
    /// @return The given reference.
    MappedType&     Mapped(T& src)                                             const { return src; }
};

} // namespace [alib::containers]

