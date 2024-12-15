//==================================================================================================
/// \file
/// This header file is part of module \alib_monomem of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_ALIASES_STDVECTOR
#define HPP_ALIB_MONOMEM_ALIASES_STDVECTOR
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(MONOMEM)
#include <vector>
#include "alib/monomem/poolallocator.hpp"
namespace alib
{
    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdVectorMono  =     std::vector<T, SCAMono<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdVectorPool  =     std::vector<T, SCAPool<T>>;

} // namespace [alib]


#endif // HPP_ALIB_MONOMEM_ALIASES_STDVECTOR

