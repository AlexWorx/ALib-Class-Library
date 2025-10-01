//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_BOOTSTRAP_PP
#define HPP_ALIB_BOOTSTRAP_PP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

// #################################################################################################
// Symbols introduced by module ALib.Bootstrap
// #################################################################################################

// Compilation flags
// Create a bitset from the ALib compilation features.
// For this, we ensure that the individual headers tested are included
#if !DOXYGEN
#   define    ALIB_COMPILATION_FLAGS                       \
 +  (1<<0)  *  ALIB_ALOX                                   \
 +  (1<<1)  *  ALIB_BITBUFFER                              \
 +  (1<<2)  *  ALIB_BOXING                                 \
 +  (1<<3)  *  ALIB_CAMP                                   \
 +  (1<<4)  *  ALIB_CONTAINERS                             \
 +  (1<<5)  *  ALIB_CLI                                    \
 +  (1<<6)  *  ALIB_ENUMRECORDS                            \
 +  (1<<7)  *  ALIB_EXCEPTIONS                             \
 ,                                                         \
 +  (1<<0)  *  ALIB_EXPRESSIONS                            \
 +  (1<<1)  *  ALIB_FILES                                  \
 +  (1<<2)  *  ALIB_FORMAT                                 \
 +  (1<<3)  *  ALIB_MONOMEM                                \
 +  (1<<4)  *  ALIB_RESOURCES                              \
 +  (1<<5)  *  ALIB_SINGLETONS                             \
 +  (1<<6)  *  ALIB_STRINGS                                \
 +  (1<<7)  *  ALIB_SYSTEM                                 \
,                                                          \
 +  (1<<0)  *  ALIB_THREADMODEL                            \
 +  (1<<1)  *  ALIB_VARIABLES                              \
 +  (1<<2)  *  ALIB_DEBUG                                  \
 +  (1<<3)  *  ALIB_DEBUG_ARRAY_COMPRESSION                \
 +  (1<<4)  *  ALIB_DEBUG_ALLOCATIONS                      \
 +  (1<<5)  *  ALIB_DEBUG_BOXING                           \
 +  (1<<6)  *  ALIB_DEBUG_CONTAINERS                       \
 +  (1<<7)  *  ALIB_DEBUG_CRITICAL_SECTIONS                \
 ,                                                          \
 +  (1<<1)  *  ALIB_DEBUG_MEMORY                           \
 +  (1<<2)  *  ALIB_DEBUG_STRINGS                          \
 +  (1<<3)  *  ALIB_DEBUG_RESOURCES                        \
 +  (1<<4)  *  ALIB_C20_MODULES                            \
 +  (1<<2)  *  ALIB_SINGLE_THREADED                        \
 +  (1<<4)  *  ALIB_FEAT_SINGLETON_MAPPED                  \
 +  (1<<5)  *  ALIB_CHARACTERS_WIDE                        \
 +  (1<<6)  *  (ALIB_CHARACTERS_SIZEOF_WCHAR == 4 ? 1 : 0) \
 +  (1<<7)  *  ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS        \
,                                                          \
 +  (1<<0)  *  ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS       \
 +  (1<<1)  *  ALIB_FEAT_BOXING_BIJECTIVE_FLOATS           \
 +  (1<<2)  *  ALIB_FEAT_BOOST_REGEX                       \
 +  (1<<3)  *  ALOX_DBG_LOG                                \
 +  (1<<4)  *  ALOX_DBG_LOG_CI                             \
 +  (1<<5)  *  ALOX_REL_LOG                                \
 +  (1<<6)  *  ALOX_REL_LOG_CI
#endif //DOXYGEN

#endif // HPP_ALIB_BOOTSTRAP_PP

