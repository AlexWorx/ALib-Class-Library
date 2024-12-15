//==================================================================================================
/// \file
/// This header file is part of module \alib_monomem of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_ALIASES_STDLIST
#define HPP_ALIB_MONOMEM_ALIASES_STDLIST
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(MONOMEM)
#include "alib/monomem/poolallocator.hpp"
#include <list>
namespace alib
{
    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdListMono  =     std::list<T, SCAMono<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdListPool  =     std::list<T, SCAPool<T>>;

} // namespace [alib]


#endif // HPP_ALIB_MONOMEM_ALIASES_STDLIST

