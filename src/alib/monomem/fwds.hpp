/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_FWDS
#define HPP_ALIB_MONOMEM_FWDS 1

#if !defined (HPP_ALIB_FS_COMMONENUMS_DEFS)
#   include "alib/lib/fs_commonenums/commonenumdefs.hpp"
#endif

ALIB_ASSERT_MODULE(MONOMEM)

#if !defined(ALIB_DEBUG_MONOMEM)
#   define   ALIB_DEBUG_MONOMEM           0
#elif !ALIB_DEBUG && ALIB_DEBUG_MONOMEM
#   undef    ALIB_DEBUG_MONOMEM
#   define   ALIB_DEBUG_MONOMEM           0
#   pragma message "Symbol ALIB_DEBUG_MONOMEM set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#endif


namespace aworx { namespace lib { namespace monomem {

/** ************************************************************************************************
 * This struct contains three tag-types which denote the type of node-recycling that a container
 * instantiation is deemed to perform. One of these inner types is to be set as the value
 * for a corresponding template parameter named \p{TRecycling} found in container types
 * of this module.
 *
 * In the case \b Shared is given, the container type will expose a type definition named
 * \p{TSharedRecycler} which can be used to define the external shared recycler object.
 * This object has then to be passed to the constructor of the container instance.
 *
 * \see
 *   Chapter \ref alib_monomem_intro_recycling of the Programmer's Manual
 *   for this \alibmod.
 **************************************************************************************************/
struct Recycling
{
    /** Denotes a private recycling. */
    struct Private {};

    /** Denotes a shared recycling. */
    struct Shared  {};

    /** Denotes that no recycling should be performed. */
    struct None    {};
}; // struct Recycling

//####################################   Forward declarations   ####################################
                                                 class MonoAllocator;
template<typename T,
         typename TRecycling = Recycling::Private >   class List;

template<typename T,
         typename TStored,
         typename TKey,
         typename TIfMapped,
         typename THash,
         typename TEqual,
         typename TAccess,
         Caching  THashCaching,
         typename TRecycling                 >   class HashTable;


}} // namespace aworx[::lib::monomem]

/// Type alias in namespace #aworx.
using Recycling= lib::monomem::Recycling;

} // namespace [aworx]


#endif // HPP_ALIB_MONOMEM_FWDS


