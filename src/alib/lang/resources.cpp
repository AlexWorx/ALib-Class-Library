// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_LANG_RESOURCES)
    #include "alib/lang/resources.hpp"
#endif

#if ALIB_MODULE_CONFIGURATION
#   if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
#       include "alib/config/configuration.hpp"
#   endif
#elif ALIB_MODULE_CORE
#   if !defined (HPP_ALIB_THREADS_THREADLOCK)
#       include "alib/threads/threadlock.hpp"
#   endif
#endif


#if !defined (_GLIBCXX_CSTDARG) && !defined (_CSTDARG_)
#   include <cstdarg>
#endif


#if !(ALIB_MODULE_CONFIGURATION || ALIB_MODULE_CORE)
#   define ALIB_LOCK_WITH(pruned)
#endif


namespace aworx { namespace lib { namespace lang {



// Constructor/destructor. Needs to stay in cpp file as class Configuration is not know in header
Resources::Resources()
:  cache  (1024)
#if ALIB_MODULE_CONFIGURATION
,  Config ( new config::Configuration(false)  )
#endif
{
    #if ALIB_MODULE_CONFIGURATION || ALIB_MODULE_CORE
        lock= new ThreadLock;
    #endif
}

Resources::~Resources()
{
    #if ALIB_MODULE_CONFIGURATION || ALIB_MODULE_CORE
        delete lock;
    #endif
    #if ALIB_MODULE_CONFIGURATION
        delete Config;
        if( variable != nullptr )
            delete variable;
    #endif
}


void Resources::Add( const String& category, const String& name, const String& data
          ALIB_DBG( ,bool dbgAssert )                                               )
{
    ALIB_LOCK_WITH(*lock)

    // find / create category
    auto catIt= defaults.find( category );
    if( catIt == defaults.end() )
        catIt= defaults.emplace( category, UnorderedStringMap<String>() ).first;

    // store in category as name
    ALIB_ASSERT_ERROR( !dbgAssert || catIt->second.find( name ) == catIt->second.end(),
                       ASTR("Doubly defined resource \""), String64(name    ).ToCString(),
                       ASTR("\" in category: ")          , String64(category).ToCString() );

    catIt->second[name]= data;
}


void Resources::AddBulk( const character* category, ... )
{
    ALIB_LOCK_WITH(*lock)
    // find / create category
    auto catIt= defaults.find( category );
    if( catIt == defaults.end() )
        catIt= defaults.emplace( category, UnorderedStringMap<String>() ).first;

    va_list args;
    va_start(args, category);
    for(;;)
    {
        String name= va_arg( args, const character* );
        if( name.IsNull() )
            break;

        String val = va_arg( args, const character* );
        catIt->second.emplace( name, val );
    }
    va_end(args);
}

void Resources::AddCopy( const String& category, const String& name, const String& data )
{
    ALIB_LOCK_WITH(*lock)

    // find / create category
    auto catIt= defaults.find( category );
    if( catIt == defaults.end() )
        catIt= defaults.emplace( cache.AllocAndCopy(category), UnorderedStringMap<String>() ).first;

    auto nameIt=   catIt->second.find( name );
    if ( nameIt == catIt->second.end() )
        nameIt= catIt->second.emplace( cache.AllocAndCopy(name), nullptr ).first;
    nameIt->second=  cache.AllocAndCopy( data );
}


String Resources::Get( const String& category, const String& name   ALIB_DBG(, bool dbgAssert ) )
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
        if( Config->Load( variable->Declare( category, name ) ) != Priorities::NONE )
        {
            // success: store in cachedData map
            catIt= cachedVariables.find( category );
            if( catIt == cachedVariables.end() )
                catIt= cachedVariables.emplace( cache.AllocAndCopy(category), UnorderedStringMap<String>() ).first;

            catIt->second[cache.AllocAndCopy(name)]= cache.AllocAndCopy( *variable->GetString() );
        }
    }
    #endif

    // load static data
    catIt= defaults.find( category );
    if( catIt == defaults.end() )
    {
        ALIB_ASSERT_ERROR( !dbgAssert, ASTR("Unknown resource category: "), String64(category).ToCString() );
        return NullString;
    }
    auto nameIt= catIt->second.find( name );
    if( nameIt == catIt->second.end() )
    {
        ALIB_ASSERT_ERROR( !dbgAssert, ASTR("Unknown resource \""), String64(name    ).ToCString(),
                                       ASTR("\" in category: "   ), String64(category).ToCString() );
        return NullString;
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
            if( !dest.Load( variable->Declare( outer->first, inner->first), true ) )
            {
                variable->ClearValues().Add( inner->second );
                dest.Store( *variable );
                cntCopied++;
            }

    return  cntCopied;
}

#endif



}}}// namespace [aworx::lib::lang]

