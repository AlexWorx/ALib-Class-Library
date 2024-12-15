//==================================================================================================
/// \file
/// This header file is part of the \aliblong. It does not belong to an \alibmod and is
/// included in any \alibdist.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
///
/// \note
///   To reduce their complexity, this header is not shown in inclusion graphs of this documentation.
//==================================================================================================
#ifndef HPP_ALIB_LANG_TMP
#define HPP_ALIB_LANG_TMP 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif

// is/eq/isof
#define ATMP_IS_CONST( T       )   std::is_const   <T>::value
#define ATMP_IS_ENUM( T       )    std::is_enum    <T>::value
#define ATMP_IS_PTR( T        )    std::is_pointer <T>::value
#define ATMP_IS_ARR( T        )    std::is_array   <T>::value
#define ATMP_IS_INT( T        )    std::is_integral<T>::value
#define ATMP_IS_UINT( T       )   (std::is_integral<T>::value && std::is_unsigned<T>::value)
#define ATMP_IS_SINT( T       )   (std::is_integral<T>::value && std::is_signed  <T>::value)
#define ATMP_EQ(     T, TEqual)    std::is_same    <T, TEqual>::value
#define ATMP_ISOF(   T, TBase)     std::is_base_of <TBase, T >::value

// remove
#define ATMP_RC(     T        )    typename std::remove_const    <T>::type
#define ATMP_RR(     T        )    typename std::remove_reference<T>::type
#define ATMP_RP(     T        )    typename std::remove_pointer  <T>::type
#define ATMP_RE(     T        )    typename std::remove_extent   <T>::type
#define ATMP_RCV(    T        )    typename std::remove_cv       <T>::type
#define ATMP_RCVR(   T        )    typename std::remove_cv       <                                 \
                                   typename std::remove_reference<T>::type>::type
#define ATMP_RCVP(   T        )    typename std::remove_cv       <                                 \
                                   typename std::remove_pointer  <                                 \
                                   typename std::remove_reference<T>::type>::type>::type
#define ATMP_RECVP(   T       )    typename std::remove_extent   <                                 \
                                   typename std::remove_cv       <                                 \
                                   typename std::remove_pointer  <                                 \
                                   typename std::remove_reference<T>::type>::type>::type>::type

// enable_if / conditional
#define ATMP_VOID_IF( Cond       )    typename std::enable_if<Cond,void >::type
#define ATMP_BOOL_IF( Cond       )    typename std::enable_if<Cond,bool >::type
#define ATMP_T_IF( T, Cond       )    typename std::enable_if<Cond,T    >::type
#define ATMP_IF_T_F(  Cond, T, F )    typename std::conditional<Cond,T,F>::type
#define ATMP_HAS_METHOD(T,Method,...) !ATMP_EQ(lang::TMPUnknownType, decltype(std::declval<T>(). Method( __VA_ARGS__ )))
#define ATMP_RESULT_TYPE(T,Method,...) decltype(std::declval<T>(). Method( __VA_ARGS__ ))


// method selection
#define ATMP_IF( Cond )            typename = typename std::enable_if<Cond,void >::type

#define ATMP_SELECT_IF_1TP( TParam,                           ... )                                \
  template <TParam,                    typename std::enable_if<__VA_ARGS__  ,int>::type = 0 >      \
  ALIB_FORCE_INLINE                                                                                \

#define ATMP_SELECT_IF_2TP( TParam1, TParam2,                   ... )                              \
  template <TParam1, TParam2,          typename std::enable_if<__VA_ARGS__  ,int>::type = 0 >      \
  ALIB_FORCE_INLINE                                                                                \

#define ATMP_SELECT_IF_3TP( TParam1, TParam2, TParam3,          ... )                              \
  template <TParam1, TParam2, TParam3, typename std::enable_if<__VA_ARGS__  ,int>::type = 0 >      \
  ALIB_FORCE_INLINE                                                                                \

#define ATMP_SELECT_IF_4TP( TParam1, TParam2, TParam3, TParam4, ... )                              \
  template <TParam1, TParam2,                                                                      \
            TParam3, TParam4,          typename std::enable_if<__VA_ARGS__  ,int>::type = 0 >      \
  ALIB_FORCE_INLINE                                                                                \


// method selection using return type.
#define ATMP_RETURN_IF_1TP( TReturn, TParam,           ... )                                       \
  template <TParam>                                                                                \
  ALIB_FORCE_INLINE                                                                                \
  ATMP_T_IF( TReturn, (__VA_ARGS__))                                                               \

#define ATMP_RETURN_IF_2TP( TReturn, TParam1, TParam2, ... )                                       \
  template <TParam1, TParam2>                                                                      \
  ALIB_FORCE_INLINE                                                                                \
  ATMP_T_IF( TReturn, (__VA_ARGS__))                                                               \


// common type tags

namespace alib::lang {

/// Template meta programming struct that is a type not equal to any other type.
/// For example, used with \ref ATMP_HAS_METHOD.
struct TMPUnknownType {};

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


} // namespace [alib::lang]

#endif // HPP_ALIB_LANG_TMP

