//==================================================================================================
/// \file
/// This header file is part of module \alib_monomem of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_ALIASES_STDDEQUE
#define HPP_ALIB_MONOMEM_ALIASES_STDDEQUE
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(MONOMEM)
#include "alib/monomem/poolallocator.hpp"
#include <deque>
namespace alib
{
    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdDequeMono  =     std::deque<T, SCAMono<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdDequePool  =     std::deque<T, SCAPool<T>>;

} // namespace [alib]


#endif // HPP_ALIB_MONOMEM_ALIASES_STDDEQUE

