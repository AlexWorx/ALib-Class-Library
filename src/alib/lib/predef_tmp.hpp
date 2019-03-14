// #################################################################################################
//  ALib C++ Library
//
//  General ALib preprocessor macros
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_LIB_PREDEF_TMP
#define HPP_ALIB_LIB_PREDEF_TMP 1


#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_FILESET(PREDEF_TMP)

#if !defined(_GLIBCXX_TYPE_TRAITS) || !defined(_TYPE_TRAITS_)
#   include <type_traits>
#endif


// is/eq/isof
#define ATMP_IS_ENUM( T       )    std::is_enum   <T        >::value
#define ATMP_IS_PTR( T        )    std::is_pointer<T        >::value
#define ATMP_IS_ARR( T        )    std::is_array  <T        >::value
#define ATMP_EQ(     T, TEqual)    std::is_same   <T, TEqual>::value
#define ATMP_ISOF(   T, TBase)     std::is_base_of<TBase, T >::value

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
#define ATMP_VOID_IF( Cond       ) typename std::enable_if<Cond  >::type
#define ATMP_T_IF( T, Cond       ) typename std::enable_if<Cond,T>::type
#define ATMP_IF_T_F(  Cond, T, F ) typename std::conditional<Cond,T,F>::type

// method selection
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



#endif // HPP_ALIB_LIB_PREDEF_TMP
