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
#include "alib/resources/resources.prepro.hpp"
#include <cstdarg>
#if ALIB_DEBUG_RESOURCES
#   include <vector>
#   include <algorithm>
#endif
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Resources;
    import   ALib.Lang;
#  if ALIB_STRINGS
    import   ALib.Strings;
#  endif
#if ALIB_DEBUG_RESOURCES
#   include "ALib.Strings.StdIOStream.H"
#endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#if ALIB_DEBUG_RESOURCES
#   include "ALib.Strings.StdIOStream.H"
#endif
#   include "ALib.Resources.H"
#endif
// ======================================   Implementation   =======================================
namespace alib {

/// This is the reference documentation module \alib_resources.<br>
/// Extensive documentation for this namespace is provided with the
/// \ref alib_mod_resources "Programmer's Manual" of that module.
namespace resources {

#if ALIB_DEBUG_RESOURCES
std::ostream* LocalResourcePool::DbgResourceLoadObserver= nullptr;
#endif

bool LocalResourcePool::BootstrapAddOrReplace( const NString& category,
                                               const NString& name,
                                               const  String& resource        )
{

#if !ALIB_DEBUG_RESOURCES
    auto it= data.InsertOrAssign( detail::Key {category, name},   resource      );
#else
    auto it= data.InsertOrAssign( detail::Key {category, name}, { resource, 0 } );
    if( DbgResourceLoadObserver )
    {
        (*DbgResourceLoadObserver) << (it.second ? "Adding Resource: " : "Replacing Resource: " )
                                   << category
                                   << "/"              << name << "=" << resource << std::endl;
    }
#endif

    return !it.second;
}

void LocalResourcePool::BootstrapBulk( const nchar* category, ... )
{
    // find / create category
    detail::Key key {category, nullptr};

    va_list args;
    va_start(args, category);
    for(;;)
    {
        key.Name= NString( va_arg( args, const nchar* ) );
        if( key.Name.IsNull() )
            break;

        String val = va_arg( args, const character* );
#if ALIB_DEBUG_RESOURCES
        if( DbgResourceLoadObserver )
            (*DbgResourceLoadObserver) << "Bulk Resource: "  << category
                                       << "/"           << key.Name << "=" << val << std::endl;
#endif

ALIB_DBG( auto result=)
#if !ALIB_DEBUG_RESOURCES
        data.EmplaceOrAssign( key, val );
#else
        data.EmplaceOrAssign( key, std::make_pair(val,0) );
#endif

ALIB_ASSERT_WARNING( result.second, "RESOURCES",
                     "Replacing resource with BootstrapBulk: {} / {} = val",
                     category, key.Name, val )
        // \checkpromise: when typed ALib assertions and warnings are available, then
        // raise a warning if the result of above EmplaceOrAssign is an assign, aka the
        // bulk data existed already.
    }
    va_end(args);
    }


const String& LocalResourcePool::Get( const NString& category, const NString& name   ALIB_DBG(, bool dbgAssert ) )
{
    // search main map
    auto dataIt=  data.Find( detail::Key { category, name } );
    if( dataIt != data.end() )
    {
#if !ALIB_DEBUG_RESOURCES
        return dataIt.Mapped();
#else
               dataIt.Mapped().second++;
        return dataIt.Mapped().first;
#endif
    }
    ALIB_ASSERT_ERROR( !dbgAssert, "RESOURCES",
       "Unknown resource! Category: \"{}\", Name: \"{}\".", category, name )
    return NULL_STRING;

}


#if ALIB_DEBUG_RESOURCES

std::vector<std::tuple<NString, NString, String, integer>>
ResourcePool::DbgGetList()
{
    ALIB_WARNING( "STRINGS",
             "ResourcePool::DbgGetList was not overridden by the ResourcePool type set. "
             "Note that type built-in ALib type LocalResourcePool does provide an implementation." )

    return std::vector<std::tuple<NString, NString, String, integer>>();
}

std::vector<std::pair<NString, integer>>
ResourcePool::DbgGetCategories()
{
    ALIB_WARNING( "STRINGS",
             "ResourcePool::DbgGetCategories was not overridden by the ResourcePool type set. "
             "Note that type built-in ALib type LocalResourcePool does provide an implementation." )

    return std::vector<std::pair<NString, integer>>();
}

std::vector<std::tuple<NString, NString, String, integer>>
LocalResourcePool::DbgGetList()
{
    std::vector<std::tuple<NString, NString, String, integer>> result;

    result.reserve( size_t( data.Size() ) );
    for( auto& it : data )
    {
        result.emplace_back(
                             it.first.Category,
                             it.first.Name,
                             it.second.first,
                             it.second.second   );
    }

    std::sort( result.begin(), result.end(),
               [] (const std::tuple<NString, NString, String, integer>& a,
                   const std::tuple<NString, NString, String, integer>& b )
               {

                    auto comp= std::get<0>(a).template CompareTo<CHK, lang::Case::Ignore>( std::get<0>(b) );
                    if( comp != 0 )
                        return comp < 0;

                    return     std::get<1>(a).template CompareTo<CHK, lang::Case::Ignore>( std::get<1>(b) ) < 0;
               }
    );
    return result;
}

std::vector<std::pair<NString, integer>>
LocalResourcePool::DbgGetCategories()
{
    std::vector<std::pair<NString, integer>> result;

    auto list= DbgGetList();
    NString lastCat= nullptr;
    for( auto& entry : list )
    {
        if( !lastCat.Equals( std::get<0>(entry)  ) )
        {
            lastCat=  std::get<0>(entry);
            result.push_back( { std::get<0>(entry), 0 } );
        }
        ++result.back().second;
    }

    return result;
}

#endif // ALIB_DEBUG_RESOURCES

}} // namespace [alib::resources]

