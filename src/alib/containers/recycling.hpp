//==================================================================================================
/// \file
/// This header file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_FWDS
#define HPP_ALIB_MONOMEM_FWDS 1
#pragma once
#include "alib/lang/commonenumdefs.hpp"

ALIB_ASSERT_MODULE(CONTAINERS)

namespace alib {  namespace containers {

//==================================================================================================
/// This enumeration contains three elements, which denote the type of node-recycling that a
/// container instantiation is deemed to perform. One of these elements is to be set as the value
/// for a corresponding template parameter named \p{TRecycling} found in container types
/// of this module.
///
/// In the case \b Shared is given, the container type will expose a type definition named
/// \b SharedRecyclerType, which can be used to define the external shared recycler object.
/// This object has then to be passed to the constructor of the container instance.
///
/// \see
///   Chapter \ref alib_contmono_intro_recycling of the Programmer's Manual
///   for this \alibmod.
//==================================================================================================
enum class Recycling
{
    None   ,    ///<< Denotes that no recycling should be performed.
    Private,    ///<< Denotes private recycling. This is usaully the default value.
    Shared ,    ///<< Denotes shared recycling.
};


} // namespace alib[::containers]

/// Type alias in namespace \b alib.
using Recycling= containers::Recycling;

} // namespace [alib]


#endif // HPP_ALIB_MONOMEM_FWDS

