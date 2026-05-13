//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef HPP_ALIB_RESOURCES_PP
#define HPP_ALIB_RESOURCES_PP
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

//##################################################################################################
// Macro  introduced by module ALib.Resources
//##################################################################################################
#if !ALIB_RESOURCES
#   define   ALIB_DEBUG_RESOURCES           0
#elif !defined(ALIB_DEBUG_RESOURCES)
#   define   ALIB_DEBUG_RESOURCES           0
#elif !ALIB_DEBUG && ALIB_DEBUG_RESOURCES
#   undef    ALIB_DEBUG_RESOURCES
#   define   ALIB_DEBUG_RESOURCES           0
#   pragma message "Symbol ALIB_DEBUG_RESOURCES set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#endif

#if ALIB_RESOURCES
#define  ALIB_RESOURCED( T, ResPool, ResCategory, ResName )                                        \
namespace alib::resources {                                                                        \
template<> struct ResourcedTraits<T> : std::true_type                                              \
{                                                                                                  \
    static            ResourcePool* Pool()        { return  ResPool;     }                         \
    static constexpr  NString       Category()    { return  ResCategory; }                         \
    static constexpr  NString       Name()        { return  ResName;     }                         \
};}

#endif
#endif // HPP_ALIB_RESOURCES_PP
