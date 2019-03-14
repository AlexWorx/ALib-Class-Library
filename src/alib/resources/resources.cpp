// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/resources/resources.hpp"

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#    include "alib/strings/localstring.hpp"
#endif

#if ALIB_MODULE_CONFIGURATION
#   if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
#       include "alib/config/configuration.hpp"
#   endif
#elif ALIB_MODULE_THREADS
#   if !defined (HPP_ALIB_THREADS_THREADLOCK)
#       include "alib/threads/threadlock.hpp"
#   endif
#endif


#if !defined (_GLIBCXX_CSTDARG) && !defined (_CSTDARG_)
#   include <cstdarg>
#endif


namespace aworx { namespace lib {

/**
 * This is the reference documentation of sub-namespace \b resources of the \aliblink which
 * holds types of library module \alibmod_resources.
 *
 * Extensive documentation for this module is provided with
 * \ref alib_mod_resources "ALib Module Resources - Programmer's Manual".
 */
namespace resources {

// Constructor/destructor. Needs to stay in cpp file as class Configuration is not know in header
Resources::Resources()
:  cache  (1024)
#if ALIB_MODULE_CONFIGURATION
,  Config ( new config::Configuration(false)  )
#endif
{
    #if ALIB_MODULE_THREADS
        lock= new threads::ThreadLock;
    #endif
}

Resources::~Resources()
{
    #if ALIB_MODULE_THREADS
        delete lock;
    #endif
    #if ALIB_MODULE_CONFIGURATION
        delete Config;
        if( variable != nullptr )
            delete variable;
    #endif
}


void Resources::Add( const NString& category, const NString& name, const String& data
          ALIB_DBG( ,bool dbgAssert )                                               )
{
    ALIB_LOCK_WITH(*lock)

    // find / create category
    auto catIt= defaults.find( category );
    if( catIt == defaults.end() )
        catIt= defaults.emplace( category, UnorderedStringMap<String,nchar>() ).first;

    // store in category as name
    ALIB_ASSERT_ERROR( !dbgAssert || catIt->second.find( name ) == catIt->second.end(),
                       "Doubly defined resource \"", NString64(name    ),
                       "\" in category: "          , NString64(category)                   )

    catIt->second[name]= data;
}


void Resources::AddBulk( const nchar* category, ... )
{
    ALIB_LOCK_WITH(*lock)
    // find / create category
    NString categoryS= category;
    auto catIt= defaults.find( categoryS );
    if( catIt == defaults.end() )
        catIt= defaults.emplace( categoryS, UnorderedStringMap<String,nchar>() ).first;

    va_list args;
    va_start(args, category);
    for(;;)
    {
        NString name= va_arg( args, const nchar* );
        if( name.IsNull() )
            break;

        String val = va_arg( args, const character* );
        catIt->second.emplace( name, val );
    }
    va_end(args);
}

void Resources::AddCopy( const NString& category, const NString& name, const String& data )
{
    ALIB_LOCK_WITH(*lock)

    // find / create category
    auto catIt= defaults.find( category );
    if( catIt == defaults.end() )
        catIt= defaults.emplace( cache.Clone(category), UnorderedStringMap<String,nchar>() ).first;

    auto nameIt=   catIt->second.find( name );
    if ( nameIt == catIt->second.end() )
        nameIt= catIt->second.emplace( cache.Clone(name), nullptr ).first;
    nameIt->second=  cache.Clone( data );
}


String Resources::Get( const NString& category, const NString& name   ALIB_DBG(, bool dbgAssert ) )
{
    ALIB_LOCK_WITH(*lock)

    // first, try load from cached data
    auto catIt= cachedVariables.find( category );
    if( catIt != cachedVariables.end() )
    {
        auto nameIt= catIt->second.find( name );
        if( nameIt != catIt->second.end() )
            return nameIt->second;
    }


    #if ALIB_MODULE_CONFIGURATION
    if( Config->HasPlugins() )
    {
        if( variable == nullptr )
            variable= new Variable();

        // load variable
        ALIB_STRINGS_FROM_NARROW( category, categoryCv, 64 )
        ALIB_STRINGS_FROM_NARROW( name    , nameCv    , 64 )
        if( Config->Load( variable->Declare( categoryCv, nameCv ) ) != Priorities::NONE )
        {
            // success: store in cachedData map
            catIt= cachedVariables.find( category );
            if( catIt == cachedVariables.end() )
                catIt= cachedVariables.emplace( cache.Clone(category), UnorderedStringMap<String,nchar>() ).first;

            catIt->second[cache.Clone(name)]= cache.Clone( variable->GetString() );
        }
    }
    #endif

    // load static data
    catIt= defaults.find( category );
    if( catIt == defaults.end() )
    {
        ALIB_ASSERT_ERROR( !dbgAssert, "Unknown resource category: ", NString64(category) )
        return NullString();
    }
    auto nameIt= catIt->second.find( name );
    if( nameIt == catIt->second.end() )
    {
        ALIB_ASSERT_ERROR( !dbgAssert, "Unknown resource \"", NString64(name    ),
                                       "\" in category: "   , NString64(category) )
        return NullString();
    }

    return nameIt->second;
}


#if ALIB_MODULE_CONFIGURATION

int Resources::FetchDefaults( config::ConfigurationPlugin& dest )
{
    ALIB_LOCK_WITH(*this->lock)

    if( variable == nullptr )
        variable= new Variable();

    int cntCopied= 0;
    for( auto outer= defaults.begin()  ; outer != defaults.end() ; outer++ )
        for( auto inner= outer->second.begin() ; inner != outer->second.end(); inner++ )
        {
            ALIB_STRINGS_FROM_NARROW( outer->first, category, 64 )
            ALIB_STRINGS_FROM_NARROW( inner->first, name    , 64 )
            if( !dest.Load( variable->Declare( category, name ), true ) )
            {
                variable->ClearValues().Add( inner->second );
                dest.Store( *variable );
                cntCopied++;
            }
        }
    return  cntCopied;
}

#endif

}}} // namespace [aworx::lib::resources]

