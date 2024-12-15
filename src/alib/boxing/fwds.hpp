//==================================================================================================
/// \file
/// This header file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_BOXING_FWDS
#define HPP_ALIB_BOXING_FWDS 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(BOXING)
#include "alib/lang/allocation.hpp"

namespace alib {

#if ALIB_MONOMEM

#if !defined(ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT)
#   define ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT     alignof(uint64_t)
#endif

namespace monomem {
    template<typename TAllocator>                       class TMonoAllocator;
    template<typename TAllocator, size_t TAlignment>    class TPoolAllocator;
}

    using  MonoAllocator   = monomem::TMonoAllocator<lang::HeapAllocator>;
    using  PoolAllocator   = monomem::TPoolAllocator<MonoAllocator         , ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT>;
    using  PoolAllocatorHA = monomem::TPoolAllocator<lang::HeapAllocator   , ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT>;
#endif
namespace boxing {

namespace detail {  struct  VTable;  }
class  Box;

template<typename TAllocator>
class TBoxes;

struct Enum;

struct FHashcode;
struct FClone;
struct FIsNotNull;
struct FIsEmpty;
struct FEquals;
struct FIsLess;
struct FIsTrue;
IF_ALIB_STRINGS(
template<typename TChar, typename TAllocator>
struct FAppend;             )


} // namespace alib[::boxing]


/// Type alias in namespace \b alib.
using     Box           =   boxing::Box;

/// Type alias in namespace \b alib.
using     BoxesHA= boxing::TBoxes<lang::HeapAllocator>;

#if ALIB_MONOMEM
/// Type alias in namespace \b alib.
using     BoxesMA= boxing::TBoxes<MonoAllocator>;

/// Type alias in namespace \b alib.
using     BoxesPA= boxing::TBoxes<PoolAllocator>;
#endif

/// Type alias in namespace \b alib.
using     Enum          =   boxing::Enum;

/// Type alias in namespace \b alib.
using     FHashcode     =   boxing::FHashcode;

/// Type alias in namespace \b alib.
using     FClone        =   boxing::FClone;

/// Type alias in namespace \b alib.
using     FEquals       =   boxing::FEquals;

/// Type alias in namespace \b alib.
using     FIsNotNull    =   boxing::FIsNotNull;

/// Type alias in namespace \b alib.
using     FIsLess       =   boxing::FIsLess;

/// Type alias in namespace \b alib.
using     FIsTrue       =   boxing::FIsTrue;

#if ALIB_STRINGS
/// Type alias in namespace \b alib.
template<typename TChar, typename TAllocator>
using     FAppend       =   boxing::FAppend<TChar, TAllocator>;
#endif
} // namespace [alib]

#endif // HPP_ALIB_BOXING_FWDS

