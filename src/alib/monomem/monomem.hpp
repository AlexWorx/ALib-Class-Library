/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_MONOMEM
#define HPP_ALIB_MONOMEM_MONOMEM 1

#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
#endif

ALIB_ASSERT_MODULE(MONOMEM)


#if !defined(HPP_ALIB_FS_COMMONENUMS_DEFS)
#   include "alib/lib/fs_commonenums/commonenumdefs.hpp"
#endif


#if !defined(ALIB_DEBUG_MONOMEM)
#   define   ALIB_DEBUG_MONOMEM           0
#elif !ALIB_DEBUG && ALIB_DEBUG_MONOMEM
#   undef    ALIB_DEBUG_MONOMEM
#   define   ALIB_DEBUG_MONOMEM           0
#   pragma message "Symbol ALIB_DEBUG_MONOMEM set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#endif

#if !defined(ALIB_DOX)
#   if ALIB_DEBUG_MONOMEM
#      if !ALIB_ALOX
#          error "Module ALox needs to be selected with compiler symbol ALIB_DEBUG_MONOMEM set."
#      endif
#      include "alib/alox.hpp"

       namespace aworx { namespace lib { namespace monomem {
          ALIB_API bool dbgLogLock(bool lock);
       }}}
#      define MONOMEM_PATHDOMAIN          { if( dbgLogLock(true) ) {Log_SetDomain( "ALIB/MONOMEM", Scope::Path ) } dbgLogLock(false); }
#      define MONOMEM_FILE_DOMAIN(domain) { if( dbgLogLock(true) ) {Log_SetDomain( domain, Scope::Filename )     } dbgLogLock(false); }
#      define MONOMEM_ERROR(...)          { if( dbgLogLock(true) ) {Log_Error(   __VA_ARGS__ )                   } dbgLogLock(false); }
#      define MONOMEM_WARNING(...)        { if( dbgLogLock(true) ) {Log_Warning( __VA_ARGS__ )                   } dbgLogLock(false); }
#      define MONOMEM_INFO(...)           { if( dbgLogLock(true) ) {Log_Info(    __VA_ARGS__ )                   } dbgLogLock(false); }
#      define MONOMEM_VERBOSE(...)        { if( dbgLogLock(true) ) {Log_Verbose( __VA_ARGS__ )                   } dbgLogLock(false); }
#   else
#      define MONOMEM_PATHDOMAIN
#      define MONOMEM_FILE_DOMAIN(domain)
#      define MONOMEM_ERROR(...)
#      define MONOMEM_WARNING(...)
#      define MONOMEM_INFO(...)
#      define MONOMEM_VERBOSE(...)
#   endif
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


}}} // namespace [aworx::lib::monomem]

#endif // HPP_ALIB_MONOMEM_MONOMEM
