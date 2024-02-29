// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_LANG_RESOURCES_CONFIG_RESOURCE_POOL)
#      include "alib/lang/resources/configresourcepool.hpp"
#   endif

#   if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#      include "alib/strings/localstring.hpp"
#   endif
#endif // !defined(ALIB_DOX)

#if !defined (_GLIBCXX_CSTDARG) && !defined (_CSTDARG_)
#   include <cstdarg>
#endif

namespace alib::lang::resources {

ConfigResourcePool::ConfigResourcePool()
: allocator( 8 * 1024   )
, cache    ( &allocator )
, Config   ( lang::CreateDefaults::No )
{
    Config.InsertPlugin( allocator.Emplace<InMemoryPlugin>(A_CHAR( "DefaultResources" ), &allocator ),
                         Priorities::DefaultValues,
                         lang::Responsibility::KeepWithSender);
}

ConfigResourcePool::~ConfigResourcePool()
{
    monomem::Destruct( Config.GetPluginTypeSafe<InMemoryPlugin>(Priorities::DefaultValues) );
}



bool ConfigResourcePool::BootstrapAddOrReplace( const NString& category,
                                                const NString& name,
                                                const  String& resource     )
{
    variable.Declare( category, name );
    Config.Load( variable );
    bool existed=  variable.Priority() != Priorities::NONE;

    variable.Declare( category, name );
    variable.Add( resource );
    Config.Store( variable );

    return existed;
}


void ConfigResourcePool::BootstrapBulk( const nchar* category, ... )
{
    va_list args;
    va_start(args, category);
    for(;;)
    {
        NString name=  va_arg( args, const nchar* );
        if( name.IsNull() )
            break;

        variable.Declare( category, name );
        variable.Add( String(va_arg( args, const character* )) );
        Config.Store( variable );

        // \checkpromise: when typed ALib assertions and warnings are available, then
        // raise a warning if the result of above EmplaceOrAssign is an assign, aka the
        // bulk data existed already.
    }
    va_end(args);
}


const String& ConfigResourcePool::Get( const NString& category, const NString& name   ALIB_DBG(, bool dbgAssert ) )
{
    ALIB_LOCK

    // pre-calculate hashCode
    const detail::Key  key       { category, name };
    auto hashCode= detail::Key::Hash()( key );

    // search replacement map
    auto replacementsIt=  cache.Find( key, hashCode );
    if( replacementsIt != cache.end() )
#if !ALIB_DEBUG_RESOURCES
        return replacementsIt.Mapped();
#else
        return replacementsIt.Mapped().first;
#endif

    // load variable
    if( Config.Load( variable.Declare( category, name ) ) != Priorities::NONE )
    {
#if !ALIB_DEBUG_RESOURCES
        return cache.InsertUnique( {key,   variable.GetString()     }, hashCode  )
                    .Mapped();
#else
        return cache.InsertUnique( {key, { variable.GetString(), 0} }, hashCode  )
                    .Mapped().first;
#endif
    }

    ALIB_ASSERT_ERROR( !dbgAssert, "RESOURCES", "Unknown resource! Category: ", NString64(category),
                                                                    ", Name: ", NString64(name)    )
    return NULL_STRING;
}

} // namespace [alib::lang::resources]
