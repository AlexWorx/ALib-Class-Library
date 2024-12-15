//==================================================================================================
/// \file
/// This header file is part of module \alib_monomem of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_ALIASES_LOCALSTRINGMA
#define HPP_ALIB_MONOMEM_ALIASES_LOCALSTRINGMA
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(MONOMEM)
ALIB_ASSERT_MODULE(STRINGS)

#include "alib/monomem/aliases/astringma.hpp"
#include "alib/strings/localstring.hpp"
#include "alib/monomem/monoallocator.hpp"

namespace alib
{
    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using LocalStringMA          = strings::TLocalString<character     , TCapacity, MonoAllocator>;

    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using ComplementLocalStringMA= strings::TLocalString<complementChar, TCapacity, MonoAllocator>;

    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using StrangeLocalStringMA   = strings::TLocalString<strangeChar   , TCapacity, MonoAllocator>;

    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using NLocalStringMA         = strings::TLocalString <nchar        , TCapacity, MonoAllocator>;

    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using WLocalStringMA         = strings::TLocalString <wchar        , TCapacity, MonoAllocator>;

    /// Type alias in namespace \b alib.
    template<integer TCapacity>
    using XLocalStringMA         = strings::TLocalString <xchar        , TCapacity, MonoAllocator>;


    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,8,MonoAllocator>}.
    using StringMA8  =  LocalStringMA<   8>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,16,MonoAllocator>}.
    using StringMA16 =  LocalStringMA<  16>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,32,MonoAllocator>}.
    using StringMA32 =  LocalStringMA<  32>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,64,MonoAllocator>}.
    using StringMA64 =  LocalStringMA<  64>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,128,MonoAllocator>}.
    using StringMA128=  LocalStringMA< 128>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,256,MonoAllocator>}.
    using StringMA256=  LocalStringMA< 256>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,512,MonoAllocator>}.
    using StringMA512=  LocalStringMA< 512>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,1024,MonoAllocator>}.
    using StringMA1K =  LocalStringMA<1024>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,2048,MonoAllocator>}.
    using StringMA2K =  LocalStringMA<2048>;

    /// Type alias name for \alib{strings;TLocalString;TLocalString<character,4096,MonoAllocator>}.
    using StringMA4K =  LocalStringMA<4096>;

} // namespace [alib]


#endif // HPP_ALIB_MONOMEM_ALIASES_LOCALSTRINGMA

