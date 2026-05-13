//==================================================================================================
/// \file
/// This header-file is part of module \alib_monomem of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================

//==================================================================================================
//==== AString versions using Mono- and PoolAllocator
//==================================================================================================
#if !DOXYGEN

#define ALIB_STRINGS_TASTRING_INSTANTIATION

// AStringMA
#define ASTR_TALLOC_SPEC    MonoAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_W
#include "alib/strings/detail/tastring_ca_decl.hpp.inl"

#define ASTR_TALLOC_SPEC    MonoAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_N
#include "alib/strings/detail/tastring_ca_decl.hpp.inl"

// AStringMA
#define ASTR_TALLOC_SPEC    PoolAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_W
#include "alib/strings/detail/tastring_ca_decl.hpp.inl"

#define ASTR_TALLOC_SPEC    PoolAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_N
#include "alib/strings/detail/tastring_ca_decl.hpp.inl"

#endif

ALIB_EXPORT namespace alib {

/// Type alias in namespace #"%alib".
using  AStringMA          =     strings::TAString<character     , MonoAllocator >;

/// Type alias in namespace #"%alib".
using  NAStringMA         =     strings::TAString<nchar         , MonoAllocator >;

/// Type alias in namespace #"%alib".
using  WAStringMA         =     strings::TAString<wchar         , MonoAllocator >;

/// Type alias in namespace #"%alib".
using  AStringPA          =     strings::TAString<character     , PoolAllocator >;

/// Type alias in namespace #"%alib".
using  NAStringPA         =     strings::TAString<nchar         , PoolAllocator >;

/// Type alias in namespace #"%alib".
using  WAStringPA         =     strings::TAString<wchar         , PoolAllocator >;

/// Type alias in namespace #"%alib".
using  StringPALengthResetter        =     strings::TStringLengthResetter<character     , PoolAllocator>;

/// Type alias in namespace #"%alib".
using  NStringPALengthResetter       =     strings::TStringLengthResetter<nchar         , PoolAllocator>;

/// Type alias in namespace #"%alib".
using  WStringPALengthResetter       =     strings::TStringLengthResetter<wchar         , PoolAllocator>;

/// Type alias in namespace #"%alib".
using  StringMALengthResetter        =     strings::TStringLengthResetter<character     , MonoAllocator>;

/// Type alias in namespace #"%alib".
using  NStringMALengthResetter       =     strings::TStringLengthResetter<nchar         , MonoAllocator>;

/// Type alias in namespace #"%alib".
using  WStringMALengthResetter       =     strings::TStringLengthResetter<wchar         , MonoAllocator>;

/// Type alias in namespace #"%alib".
template<integer TCapacity>
using LocalStringMA          = strings::TLocalString<character     , TCapacity, MonoAllocator>;

/// Type alias in namespace #"%alib".
template<integer TCapacity>
using NLocalStringMA         = strings::TLocalString <nchar        , TCapacity, MonoAllocator>;

/// Type alias in namespace #"%alib".
template<integer TCapacity>
using WLocalStringMA         = strings::TLocalString <wchar        , TCapacity, MonoAllocator>;

/// Type alias name for #"TLocalString;TLocalString<character,8,MonoAllocator>".
using StringMA8  =  LocalStringMA<   8>;

/// Type alias name for #"TLocalString;TLocalString<character,16,MonoAllocator>".
using StringMA16 =  LocalStringMA<  16>;

/// Type alias name for #"TLocalString;TLocalString<character,32,MonoAllocator>".
using StringMA32 =  LocalStringMA<  32>;

/// Type alias name for #"TLocalString;TLocalString<character,64,MonoAllocator>".
using StringMA64 =  LocalStringMA<  64>;

/// Type alias name for #"TLocalString;TLocalString<character,128,MonoAllocator>".
using StringMA128=  LocalStringMA< 128>;

/// Type alias name for #"TLocalString;TLocalString<character,256,MonoAllocator>".
using StringMA256=  LocalStringMA< 256>;

/// Type alias name for #"TLocalString;TLocalString<character,512,MonoAllocator>".
using StringMA512=  LocalStringMA< 512>;

/// Type alias name for #"TLocalString;TLocalString<character,1024,MonoAllocator>".
using StringMA1K =  LocalStringMA<1024>;

/// Type alias name for #"TLocalString;TLocalString<character,2048,MonoAllocator>".
using StringMA2K =  LocalStringMA<2048>;

/// Type alias name for #"TLocalString;TLocalString<character,4096,MonoAllocator>".
using StringMA4K =  LocalStringMA<4096>;

/// Type alias in namespace #"%alib".
template<integer TCapacity>
using LocalStringPA          = strings::TLocalString<character     , TCapacity, PoolAllocator>;

/// Type alias in namespace #"%alib".
template<integer TCapacity>
using NLocalStringPA         = strings::TLocalString <nchar        , TCapacity, PoolAllocator>;

/// Type alias in namespace #"%alib".
template<integer TCapacity>
using WLocalStringPA         = strings::TLocalString <wchar        , TCapacity, PoolAllocator>;

/// Type alias name for #"TLocalString;TLocalString<character,8,PoolAllocator>".
using StringPA8  =  LocalStringPA<   8>;

/// Type alias name for #"TLocalString;TLocalString<character,16,PoolAllocator>".
using StringPA16 =  LocalStringPA<  16>;

/// Type alias name for #"TLocalString;TLocalString<character,32,PoolAllocator>".
using StringPA32 =  LocalStringPA<  32>;

/// Type alias name for #"TLocalString;TLocalString<character,64,PoolAllocator>".
using StringPA64 =  LocalStringPA<  64>;

/// Type alias name for #"TLocalString;TLocalString<character,128,PoolAllocator>".
using StringPA128=  LocalStringPA< 128>;

/// Type alias name for #"TLocalString;TLocalString<character,256,PoolAllocator>".
using StringPA256=  LocalStringPA< 256>;

/// Type alias name for #"TLocalString;TLocalString<character,512,PoolAllocator>".
using StringPA512=  LocalStringPA< 512>;

/// Type alias name for #"TLocalString;TLocalString<character,1024,PoolAllocator>".
using StringPA1K =  LocalStringPA<1024>;

/// Type alias name for #"TLocalString;TLocalString<character,2048,PoolAllocator>".
using StringPA2K =  LocalStringPA<2048>;

/// Type alias name for #"TLocalString;TLocalString<character,4096,PoolAllocator>".
using StringPA4K =  LocalStringPA<4096>;

} // namespace [alib]

//==================================================================================================
//==== Debug-features in namespace alib::monomem
//==================================================================================================
#if ALIB_DEBUG
ALIB_EXPORT namespace alib::monomem {
//==================================================================================================
/// Provides allocation statistics for manual performance optimization.
///
/// \par Availability
///   This method is included only in debug-compilations and when module \alib_strings is
///   included in the \alibbuild.<br>
///   In case the configuration macro #"ALIB_DEBUG_MEMORY" is set, additional information
///   retrieved with #"TMonoAllocator::DbgGetStatistics;*" is included in the
///   generated string.
///
/// @param ma  The allocator to get formatted statistics for.
/// @return Some textual information on the allocation statistics.
//==================================================================================================
template<typename TAllocator>
NAString    DbgDumpStatistics(monomem::TMonoAllocator<TAllocator>& ma);

//################################ Template instantiation declaration ##############################
#if !DOXYGEN
    extern template ALIB_DLL NAString monomem::DbgDumpStatistics(TMonoAllocator<lang::HeapAllocator> &ma);
#endif

} // namespace [alib::monomem]
#endif // ALIB_DEBUG
