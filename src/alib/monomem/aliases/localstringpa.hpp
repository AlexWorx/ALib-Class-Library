//==================================================================================================
/// \file
/// This header file is part of module \alib_monomem of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_ALIASES_LOCALSTRINGPA
#define HPP_ALIB_MONOMEM_ALIASES_LOCALSTRINGPA
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(MONOMEM)
ALIB_ASSERT_MODULE(STRINGS)

#include "alib/monomem/aliases/astringpa.hpp"
#include "alib/strings/localstring.hpp"
#include "alib/monomem/poolallocator.hpp"

namespace alib
{
    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using LocalStringPA          = strings::TLocalString<character     , TCapacity, PoolAllocator>;

    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using ComplementLocalStringPA= strings::TLocalString<complementChar, TCapacity, PoolAllocator>;

    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using StrangeLocalStringPA   = strings::TLocalString<strangeChar   , TCapacity, PoolAllocator>;

    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using NLocalStringPA         = strings::TLocalString <nchar        , TCapacity, PoolAllocator>;

    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using WLocalStringPA         = strings::TLocalString <wchar        , TCapacity, PoolAllocator>;

    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using XLocalStringPA         = strings::TLocalString <xchar        , TCapacity, PoolAllocator>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,8,PoolAllocator>}.
    using StringPA8  =  LocalStringPA<   8>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,16,PoolAllocator>}.
    using StringPA16 =  LocalStringPA<  16>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,32,PoolAllocator>}.
    using StringPA32 =  LocalStringPA<  32>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,64,PoolAllocator>}.
    using StringPA64 =  LocalStringPA<  64>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,128,PoolAllocator>}.
    using StringPA128=  LocalStringPA< 128>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,256,PoolAllocator>}.
    using StringPA256=  LocalStringPA< 256>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,512,PoolAllocator>}.
    using StringPA512=  LocalStringPA< 512>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,1024,PoolAllocator>}.
    using StringPA1K =  LocalStringPA<1024>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,2048,PoolAllocator>}.
    using StringPA2K =  LocalStringPA<2048>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,4096,PoolAllocator>}.
    using StringPA4K =  LocalStringPA<4096>;
} // namespace [alib]


#endif // HPP_ALIB_MONOMEM_ALIASES_LOCALSTRINGPA

