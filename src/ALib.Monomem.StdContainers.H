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
    using  StdVectorMA=     std::vector<T, StdMA<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdVectorPA=     std::vector<T, StdPA<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdListMA  =     std::list  <T, StdMA<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdListPA  =     std::list  <T, StdPA<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdDequeMA  =    std::deque <T, StdMA<T>>;

    /// Type alias in namespace \b alib.
    template<typename T>
    using  StdDequePA  =    std::deque <T, StdPA<T>>;

} // namespace [alib]

#endif // ALIB_MONOMEM
#endif // H_ALIB_MONOMEM_STDCONTAINERS
