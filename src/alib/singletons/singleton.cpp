// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_SINGLETONS_SINGLETON)
#      include "alib/singletons/singleton.hpp"
#   endif

#   if !defined(HPP_ALIB_COMPATIBILITY_STD_TYPEINFO)
#      include "alib/compatibility/std_typeinfo.hpp"
#   endif

#   if ALIB_FEAT_SINGLETON_MAPPED && ALIB_DEBUG
#      if !defined(HPP_ALIB_SINGLETONS_DBGSINGLETONS)
#          include "alib/singletons/dbgsingletons.hpp"
#      endif
#      if ALIB_STRINGS && !defined (HPP_ALIB_STRINGS_FORMAT)
#         include "alib/strings/format.hpp"
#      endif
#   endif

#   if ALIB_STRINGS
#      if  !defined (HPP_ALIB_STRINGS_ASTRING)
#         include "alib/strings/astring.hpp"
#      endif
#   endif

#   if ALIB_MONOMEM
#      if !defined(HPP_ALIB_MONOMEM_HASHMAP)
#         include "alib/monomem/hashmap.hpp"
#      endif
#      if !defined(HPP_ALIB_MONOMEM_HASHSET)
#         include "alib/monomem/hashset.hpp"
#      endif
#   else
#      if !defined(_GLIBCXX_UNORDERED_MAP) && !defined(_UNORDERED_MAP_)
#         include <unordered_map>
#      endif
#      if !defined (_GLIBCXX_MUTEX) && !defined(_MUTEX_)
#         include <mutex>
#      endif
#      if !defined (_GLIBCXX_CSTRING) && !defined(_CSTRING_)
#         include <cstring>
#      endif
#   endif

#endif // !defined(ALIB_DOX)

namespace alib {

#if ALIB_FEAT_SINGLETON_MAPPED && !defined(ALIB_DOX)
namespace { bool inShutdown= false; }
#endif
/**
 * This is the namespace of \alibmod <b>"Singletons"</b>. Please refer to the
 * \ref alib_mod_singletons "Programmer's Manual Of ALib Singletons" for information about
 * using this (single :-) \b %Singleton class in this tiny namespace.
 */
namespace singletons {


#if ALIB_FEAT_SINGLETON_MAPPED && !defined(ALIB_DOX)

#if ALIB_MONOMEM
    extern ALIB_API HashMap           <TypeFunctors::Key, void*,
                                       TypeFunctors::Hash,
                                       TypeFunctors::EqualTo    >  singletonMap;
           ALIB_API HashMap           <TypeFunctors::Key, void*,
                                       TypeFunctors::Hash,
                                       TypeFunctors::EqualTo    >  singletonMap(&monomem::GlobalAllocator);
#else
    extern ALIB_API std::unordered_map<TypeFunctors::Key, void*,
                                       TypeFunctors::Hash,
                                       TypeFunctors::EqualTo    >  singletonMap;
           ALIB_API std::unordered_map<TypeFunctors::Key, void*,
                                       TypeFunctors::Hash,
                                       TypeFunctors::EqualTo    >  singletonMap;

    ALIB_IF_THREADS(
    extern std::recursive_mutex     singletonLock;
           std::recursive_mutex     singletonLock;   )
#endif

void  storeSingleton( const std::type_info& type, void* theSingleton )
{
    #if ALIB_MONOMEM
        if( singletonMap.Size() == 0)
        {
            singletonMap.MaxLoadFactor( 10 );
            singletonMap.Reserve( 23, lang::ValueReference::Absolute );
        }

        singletonMap.EmplaceUnique( &type, theSingleton );

        // we unlock now as we were locked in getSingleton
        monomem::ReleaseGlobalAllocator();
    #else
        if( singletonMap.size() == 0)
        {
            singletonMap.max_load_factor( 10 );
            singletonMap.reserve( 23 );
        }

        singletonMap.emplace(&type, theSingleton);

        // we unlock now as we were locked in getSingleton
        ALIB_IF_THREADS( singletonLock.unlock(); )
    #endif
}

void  removeSingleton( const std::type_info& type )
{
    if( !inShutdown)
    {
        #if ALIB_MONOMEM
            monomem::AcquireGlobalAllocator(ALIB_CALLER_PRUNED);
            ALIB_ASSERT_RESULT_EQUALS( singletonMap.Erase(&type), 1)
            monomem::ReleaseGlobalAllocator();
        #else
            ALIB_IF_THREADS( singletonLock.lock();  )
            ALIB_ASSERT_RESULT_EQUALS( singletonMap.erase(&type), 1)
            ALIB_IF_THREADS( singletonLock.unlock(); )
        #endif
    }
}

void* getSingleton  ( const std::type_info& type )
{
    #if ALIB_MONOMEM
        monomem::AcquireGlobalAllocator( ALIB_CALLER_PRUNED );
        auto entry= singletonMap.Find( &type );
        if ( entry != singletonMap.end() )
        {
            void* result= entry->second;
            monomem::ReleaseGlobalAllocator();
            return result;
        }

        // Attn: we do not unlock when we have not found the singleton!
        return nullptr;
    #else
        ALIB_IF_THREADS( singletonLock.lock(); )
        auto entry= singletonMap.find( &type );
        if ( entry != singletonMap.end() )
        {
            void* result= entry->second;
            ALIB_IF_THREADS( singletonLock.unlock(); )
            return result;
        }

        // Attn: we do not unlock when we have not found the singleton!
        return nullptr;
    #endif
}


#endif  //ALIB_FEAT_SINGLETON_MAPPED

void Shutdown()
{
    #if ALIB_FEAT_SINGLETON_MAPPED
        inShutdown= true;
        for( auto mapPair : singletonMap)
        {
            Singleton<void*>* theSingleton;
            memcpy( &theSingleton, &mapPair.second, sizeof(void*) );
            delete theSingleton;
        }
    #endif
}



#if ALIB_DEBUG && ALIB_FEAT_SINGLETON_MAPPED
    #if ALIB_MONOMEM
        HashMap           <TypeFunctors::Key, void*,
                           TypeFunctors::Hash,
                           TypeFunctors::EqualTo    >&   DbgGetSingletons()  { return singletonMap; }
    #else
        std::unordered_map<TypeFunctors::Key, void*,
                           TypeFunctors::Hash,
                           TypeFunctors::EqualTo    >&   DbgGetSingletons()  { return singletonMap; }
    #endif

    #if ALIB_STRINGS && ALIB_DEBUG
        int DbgGetSingletons( NAString& target )
        {
            auto& types= DbgGetSingletons();
            for( auto& it : types )
                target << lang::DbgTypeDemangler(*it.first).Get()
                       <<  " = 0x" << NFormat::Hex(reinterpret_cast<uint64_t>(it.second) )
                       << NNewLine();

    #if ALIB_MONOMEM
            return static_cast<int>( types.Size() );
    #else
            return static_cast<int>( types.size() );
    #endif
        }
    #endif
#endif

}} // namespace [alib::singletons]
