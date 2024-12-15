// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/singletons/singleton.hpp"
#   include "alib/compatibility/std_typeinfo.hpp"
#   if ALIB_FEAT_SINGLETON_MAPPED && ALIB_DEBUG
#          include "alib/singletons/dbgsingletons.hpp"
#      if ALIB_STRINGS
#         include "alib/strings/astring.hpp"
#      endif
#   endif
#   if ALIB_STRINGS
#       include "alib/strings/astring.hpp"
#   endif
#   if ALIB_MONOMEM && ALIB_CONTAINERS
#       include "alib/monomem/globalallocator.hpp"
#       include "alib/containers/hashtable.hpp"
#   else
#       include <unordered_map>
#       include <mutex>
#       include <cstring>
#   endif
#endif // !DOXYGEN

namespace alib {

#if ALIB_FEAT_SINGLETON_MAPPED && !DOXYGEN
namespace { bool inShutdown= false; }
#endif
/// This is the namespace of \alibmod <b>"Singletons"</b>. Please refer to the
/// \ref alib_mod_singletons "Programmer's Manual Of ALib Singletons" for information about
/// using this (single :-) \b %Singleton class in this tiny namespace.
namespace singletons {
#if ALIB_FEAT_SINGLETON_MAPPED && !DOXYGEN

#if ALIB_MONOMEM && ALIB_CONTAINERS
extern
ALIB_API HashMap< MonoAllocator,
                  TypeFunctors::Key, void*,
                  TypeFunctors::Hash,
                  TypeFunctors::EqualTo,
                  lang::Caching::Auto,
                  Recycling::None            >  singletonMap;
ALIB_API HashMap< MonoAllocator,
                  TypeFunctors::Key, void*,
                  TypeFunctors::Hash,
                  TypeFunctors::EqualTo,
                  lang::Caching::Auto,
                  Recycling::None            >  singletonMap(monomem::GLOBAL_ALLOCATOR);
#else
extern ALIB_API std::unordered_map<TypeFunctors::Key, void*,
                                   TypeFunctors::Hash,
                                   TypeFunctors::EqualTo    >  singletonMap;
ALIB_API std::unordered_map<TypeFunctors::Key, void*,
                            TypeFunctors::Hash,
                            TypeFunctors::EqualTo    >  singletonMap;

IF_ALIB_THREADS(
extern std::recursive_mutex     singletonLock;
       std::recursive_mutex     singletonLock;   )
#endif

#include "alib/lang/callerinfo_functions.hpp"
void  storeSingleton( const std::type_info& type, void* theSingleton )
       {
    #if ALIB_MONOMEM && ALIB_CONTAINERS
           if( singletonMap.Size() == 0)
           {
               singletonMap.MaxLoadFactor( 10 );
               singletonMap.Reserve( 23, lang::ValueReference::Absolute );
           }

           singletonMap.EmplaceUnique( &type, theSingleton );

           // we unlock now as we were locked in getSingleton
           IF_ALIB_THREADS( monomem::GLOBAL_ALLOCATOR_LOCK.ReleaseRecursive(ALIB_CALLER_PRUNED) );
    #else
           if( singletonMap.size() == 0)
           {
               singletonMap.max_load_factor( 10 );
               singletonMap.reserve( 23 );
           }

           singletonMap.emplace(&type, theSingleton);

           // we unlock now as we were locked in getSingleton
           IF_ALIB_THREADS( singletonLock.unlock(); )
       #endif
   }

void  removeSingleton( const std::type_info& type )
       {
           if( !inShutdown)
           {
        #if ALIB_MONOMEM && ALIB_CONTAINERS
               ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)
               ALIB_ASSERT_RESULT_EQUALS( singletonMap.Erase(&type), 1)
           #else
               IF_ALIB_THREADS( singletonLock.lock();  )
               ALIB_ASSERT_RESULT_EQUALS( singletonMap.erase(&type), 1)
               IF_ALIB_THREADS( singletonLock.unlock(); )
           #endif
       }
       }

#if !DOXYGEN && ALIB_FEAT_SINGLETON_MAPPED && ALIB_THREADS
void  unlock() {
monomem::GLOBAL_ALLOCATOR_LOCK.ReleaseRecursive(ALIB_CALLER_PRUNED);
}
#endif

void* getSingleton  ( const std::type_info& type )
{
    #if ALIB_MONOMEM && ALIB_CONTAINERS
        IF_ALIB_THREADS( monomem::GLOBAL_ALLOCATOR_LOCK.AcquireRecursive(ALIB_CALLER_PRUNED) );
        auto entry= singletonMap.Find( &type );
        if ( entry != singletonMap.end() )
        {
            void* result= entry->second;
            return result;
        }

        // Attn: we do not unlock when we have not found the singleton!
        return nullptr;
    #else
        IF_ALIB_THREADS( singletonLock.lock(); )
        auto entry= singletonMap.find( &type );
        if ( entry != singletonMap.end() )
        {
            void* result= entry->second;
            IF_ALIB_THREADS( singletonLock.unlock(); )
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
        #if ALIB_MONOMEM && ALIB_CONTAINERS
            new (&singletonMap) HashMap< MonoAllocator,
                                         TypeFunctors::Key, void*,
                                         TypeFunctors::Hash,
                                         TypeFunctors::EqualTo,
                                         lang::Caching::Auto,
                                         Recycling::None            >(monomem::GLOBAL_ALLOCATOR);
        #endif
    #endif
}



#if ALIB_DEBUG && ALIB_FEAT_SINGLETON_MAPPED
    #if ALIB_MONOMEM && ALIB_CONTAINERS
        HashMap           <MonoAllocator,
                           TypeFunctors::Key, void*,
                           TypeFunctors::Hash,
                           TypeFunctors::EqualTo,
                           lang::Caching::Auto,
                           Recycling::None          >&   DbgGetSingletons() { return singletonMap; }
    #else
        std::unordered_map<TypeFunctors::Key, void*,
                           TypeFunctors::Hash,
                           TypeFunctors::EqualTo    >&   DbgGetSingletons() { return singletonMap; }
    #endif

    #if ALIB_STRINGS && ALIB_DEBUG
        int DbgGetSingletons( NAString& target )
        {
            auto& types= DbgGetSingletons();
            for( auto& it : types )
                target << lang::DbgTypeDemangler(*it.first).Get()
                       <<  " = 0x" << NFormat::Hex(reinterpret_cast<uint64_t>(it.second) )
                       << NNEW_LINE;

    #if ALIB_MONOMEM && ALIB_CONTAINERS
            return static_cast<int>( types.Size() );
    #else
            return static_cast<int>( types.size() );
    #endif
        }
    #endif
#endif

#include "alib/lang/callerinfo_methods.hpp"

}} // namespace [alib::singletons]

