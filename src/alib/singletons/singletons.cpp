// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/singletons/singletons.prepro.hpp"
#if !ALIB_MONOMEM || !ALIB_CONTAINERS
#    include <unordered_map>
#endif
#if !ALIB_MONOMEM && !ALIB_SINGLE_THREADED
#    include <mutex>
#endif
#include <cstring>
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Singletons;
    import   ALib.Lang;
#  if ALIB_MONOMEM
    import   ALib.Monomem;
#  endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Monomem.H"
#   include "ALib.Singletons.H"
#endif
// ======================================   Implementation   =======================================
namespace alib {
/// This is the namespace of \alibmod <b>"Singletons"</b>. Please refer to the
/// \ref alib_mod_singletons "Programmer's Manual Of ALib Singletons" for information about
/// using this (single :-) \b %Singleton class in this tiny namespace.
namespace singletons {

#if ALIB_FEAT_SINGLETON_MAPPED && !DOXYGEN
namespace { bool inShutdown= false; }
#endif

#if ALIB_FEAT_SINGLETON_MAPPED && !DOXYGEN
namespace hidden {
          #   if ALIB_MONOMEM && ALIB_CONTAINERS
                  HashMap< MonoAllocator,
                           lang::TypeFunctors::Key, void*,
                           lang::TypeFunctors::Hash,
                           lang::TypeFunctors::EqualTo,
                           lang::Caching::Auto,
                           Recycling::None             > singletonMap(monomem::GLOBAL_ALLOCATOR);
#   else
        std::unordered_map<lang::TypeFunctors::Key, void*,
                           lang::TypeFunctors::Hash,
                           lang::TypeFunctors::EqualTo > singletonMap;
#   endif
#   if !ALIB_MONOMEM && !ALIB_SINGLE_THREADED
        std::mutex singletonMapMutex;
#   endif

#   include "ALib.Lang.CIFunctions.H"
void  storeSingleton( const std::type_info& type, void* theSingleton )
{
    #if ALIB_MONOMEM && ALIB_CONTAINERS
       if( singletonMap.Size() == 0)
       {
           singletonMap.MaxLoadFactor( 10 );
           singletonMap.Reserve( 23, lang::ValueReference::Absolute );
       }

       singletonMap.EmplaceUnique( &type, theSingleton );
    #else
       if( singletonMap.size() == 0)
       {
           singletonMap.max_load_factor( 10 );
           singletonMap.reserve( 23 );
       }

       singletonMap.emplace(&type, theSingleton);
    #endif
}

void  removeSingleton( const std::type_info& type )
{
    if(!inShutdown)
    {
        #if ALIB_MONOMEM && ALIB_CONTAINERS
            ALIB_ASSERT_RESULT_EQUALS( singletonMap.erase(&type), 1)
        #else
            ALIB_ASSERT_RESULT_EQUALS( singletonMap.erase(&type), 1)
       #endif
    }
}

void* getSingleton  ( const std::type_info& type )
{
    #if ALIB_MONOMEM && ALIB_CONTAINERS
        auto entry= singletonMap.Find( &type );
        if ( entry != singletonMap.end() )
        {
            void* result= entry->second;
            return result;
        }
        return nullptr;
    #else
        auto entry= singletonMap.find( &type );
        if ( entry != singletonMap.end() )
        {
            void* result= entry->second;
            return result;
        }
        return nullptr;
    #endif
}

} // namespace alib::singletons[::hidden]

#endif  //ALIB_FEAT_SINGLETON_MAPPED

void shutdown()
{
    #if ALIB_FEAT_SINGLETON_MAPPED
        inShutdown= true;
        for( auto& mapPair : hidden::singletonMap)
        {
            Singleton<void*>* theSingleton;
            memcpy( &theSingleton, &mapPair.second, sizeof(void*) );
            delete theSingleton;
        }
        #if ALIB_MONOMEM && ALIB_CONTAINERS
            new (&hidden::singletonMap) HashMap< MonoAllocator,
                                         lang::TypeFunctors::Key, void*,
                                         lang::TypeFunctors::Hash,
                                         lang::TypeFunctors::EqualTo,
                                         lang::Caching::Auto,
                                         Recycling::None             >(monomem::GLOBAL_ALLOCATOR);
        #endif
    #endif
}

}} // namespace [alib::singletons]

#if ALIB_DEBUG && ALIB_FEAT_SINGLETON_MAPPED
namespace alib::singletons {
#   include "ALib.Lang.CIFunctions.H"


    #if ALIB_MONOMEM && ALIB_CONTAINERS
        HashMap           <MonoAllocator,
                           lang::TypeFunctors::Key, void*,
                           lang::TypeFunctors::Hash,
                           lang::TypeFunctors::EqualTo,
                           lang::Caching::Auto,
                           Recycling::None              >& DbgGetSingletons() { return hidden::singletonMap; }
    #else
        std::unordered_map<lang::TypeFunctors::Key, void*,
                           lang::TypeFunctors::Hash,
                           lang::TypeFunctors::EqualTo  >& DbgGetSingletons() { return hidden::singletonMap; }
    #endif


    #if ALIB_STRINGS
    int DbgGetSingletons( NAString& target )
    {
        auto& types= DbgGetSingletons();
        for( auto& it : types )
            target << lang::DbgTypeDemangler(*it.first).Get()
                   <<  " = 0x" << NHex(reinterpret_cast<uint64_t>(it.second) )
                   << NNEW_LINE;

      #if ALIB_MONOMEM && ALIB_CONTAINERS
        return int( types.Size() );
      #else
        return int( types.size() );
      #endif
    }
    #endif // ALIB_SINGLETONS


#   include "ALib.Lang.CIMethods.H"

} // namespace [alib::singletons]
#endif // ALIB_DEBUG && ALIB_FEAT_SINGLETON_MAPPED

