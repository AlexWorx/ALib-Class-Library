//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_PP
#define HPP_ALIB_MONOMEM_PP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

// #################################################################################################
// Symbols introduced by module ALib.Monomem
// #################################################################################################
#if ALIB_MONOMEM

#if !DOXYGEN
#   if ALIB_DEBUG_MEMORY && ALIB_ALOX
#       define DBG_ALIGNMENT_INIT(buffer)     size_t qtyLeftBeforeAlloc= size_t(buffer->end - buffer->act);
#       define DBG_ALIGNMENT_MEASURE(buffer)  if(mem) dbgStats.AlignmentWaste+=                    \
                                                          ( qtyLeftBeforeAlloc                     \
                                                          - size_t(buffer->end - buffer->act)      \
                                                          - lang::DbgAlloc::extSize(size) );
#   else
#       define DBG_ALIGNMENT_INIT(...)
#       define DBG_ALIGNMENT_MEASURE(...)
#   endif
#endif

#if !defined(ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT) && !DOXYGEN
#   define ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT     alignof(uint64_t)
#endif

#endif
#endif // HPP_ALIB_MONOMEM_PP


