//==================================================================================================
/// \file
/// This header file is part of module \alib_monomem of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_ALIASES_STRINGVECTOR
#define HPP_ALIB_MONOMEM_ALIASES_STRINGVECTOR
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(MONOMEM)
ALIB_ASSERT_MODULE(STRINGS)

#include "alib/strings/util/stringvector.hpp"
#include "alib/monomem/poolallocator.hpp"

namespace alib
{
    /// Type alias in namespace \b alib.
    using   StringVectorMA        =     strings::util::TStringVector<character, MonoAllocator>;

    /// Type alias in namespace \b alib.
    using  NStringVectorMA        =     strings::util::TStringVector<nchar    , MonoAllocator>;

    /// Type alias in namespace \b alib.
    using  WStringVectorMA        =     strings::util::TStringVector<wchar    , MonoAllocator>;


    /// Type alias in namespace \b alib.
    using   StringVectorPA       =     strings::util::TStringVector<character , PoolAllocator>;

    /// Type alias in namespace \b alib.
    using  NStringVectorPA       =     strings::util::TStringVector<nchar     , PoolAllocator>;

    /// Type alias in namespace \b alib.
    using  WStringVectorPA       =     strings::util::TStringVector<wchar     , PoolAllocator>;

} // namespace [alib]


#endif // HPP_ALIB_MONOMEM_ALIASES_STRINGVECTOR

