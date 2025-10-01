//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::lang {

/// A type-tag that is a type not equal to any other type.
/// For example, this is used with macro \ref ALIB_HAS_METHOD.
struct UnknownTag {};


ALIB_WARNINGS_IGNORE_DOCS

/// A concept to identify  type <c>std::nullptr_t</c>.
/// @tparam T       The type to be tested for being \c std::nullptr_t.
template <typename T>
concept IsNullptr = std::same_as<T, std::nullptr_t>;

ALIB_WARNINGS_RESTORE

/// Cast function that chooses either <c>static_cast</c> or <c>dynamic_cast</c>, dependent
/// on whether type \p{TTo} is polymorphic or not.
/// @tparam TTo       The type to cast down to.
/// @tparam TFrom     The type to cast from.
/// @param derived    A pointer to the derived type.
/// @return A pointer to the base type.
template <typename TTo, typename TFrom>
TTo* SafeCast(TFrom* derived)
{
    // Ensure TTo is a base of TFrom
    ALIB_STATIC_ASSERT( SafeCast_not_allowed,   std::is_base_of<TTo   ALIB_COMMA TFrom>::value
                                             || std::is_base_of<TFrom ALIB_COMMA TTo  >::value,
        "TFrom and TTo must be related by inheritance.")

    if constexpr (std::is_polymorphic<TTo>::value)   return dynamic_cast<TTo*>(derived);
    else                                             return  static_cast<TTo*>(derived);
}

/// Checks if a given object equals a default-constructed value of the same type.
/// This function is useful with types that are not otherwise testable, for example,
/// type <c>std::thread::id</c>.
/// @param t The instance to test.
/// @return \c true if \p{t} is default-constructed, \c false otherwise.
template<typename T>
requires std::default_initializable<T>
constexpr bool IsNull(const T& t)                                             { return t==T{}; }

/// The negation of #alib::lang::IsNull.
/// @param t The instance to test.
/// @return \c false if \p{t} is default-constructed, \c true otherwise.
template<typename T>
requires std::default_initializable<T>
constexpr bool IsNotNull(const T& t)                                          { return t!=T{}; }

/// Assigns a default-constructed value to the given instance.
/// This function is useful with types that are not otherwise nullable, for example,
/// type <c>std::thread::id</c>.
/// @param t The instance to test.
template<typename T>
requires std::default_initializable<T>
constexpr void SetNull(T& t)                                                         { t= T{}; }


//==================================================================================================
/// This static inline namespace function calls the destructor <b>~T()</b>of given \p{object}.
/// The use of this method is recommended instead of calling the destructor directly,
/// to increase readability of the code.
///
/// \attention
///   Pointers have to be dereferenced when passed as arguments.
///   Otherwise, this method does nothing, as the "destructor of a pointer" is called.<br>
///   Because, for example, container types would pass a pointer here, in case their
///   (custom) stored type is a pointer type, no static assertion can be made here.
///
/// @tparam T        The object type. Deduced by the compiler and not need to be given.
/// @param object    The object to destruct.
//==================================================================================================
template<typename T>
inline
void     Destruct(T& object)                                                        { object.~T(); }

} // namespace alib[::lang]




