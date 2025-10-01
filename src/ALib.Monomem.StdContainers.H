//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_MONOMEM_STDCONTAINERS
#define H_ALIB_MONOMEM_STDCONTAINERS
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#if ALIB_MONOMEM
#include <vector>
#include <list>
#include <deque>
#include "ALib.Monomem.H"

namespace alib
{
    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdVectorMono=     std::vector<T, SCAMono<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdVectorPool=     std::vector<T, SCAPool<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdListMono  =     std::list<T, SCAMono<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdListPool  =     std::list<T, SCAPool<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdDequeMono  =     std::deque<T, SCAMono<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdDequePool  =     std::deque<T, SCAPool<T>>;

} // namespace [alib]

#endif // ALIB_MONOMEM
#endif // H_ALIB_MONOMEM_STDCONTAINERS


