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
#include "alib/variables/variables.prepro.hpp"
#include <cstdarg>

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Variables.ResourcePool;
    import   ALib.Strings.StdIOStream;
    import   ALib.EnumRecords;
#  if ALIB_EXCEPTIONS
    import   ALib.Exceptions;
#  endif
    import   ALib.System;
#  if ALIB_VARIABLES
    import   ALib.Variables;
#  endif
#else
#   include "ALib.Strings.StdIOStream.H"
#   include "ALib.Exceptions.H"
#   include "ALib.System.H"
#   include "ALib.Variables.H"
#   include "ALib.Variables.ResourcePool.H"
#endif
// ======================================   Implementation   =======================================
#if ALIB_RESOURCES

namespace alib::variables {

bool ConfigResourcePool::BootstrapAddOrReplace( const NString& category,
                                                const NString& name,
                                                const  String& resource     )
{
    Variable var( Self(), String(String256(category) << '/' <<  name), A_CHAR("S") );
    bool existed=  var.IsDefined();
    if(var.Define())
        var.GetString().Reset(resource);
    return existed;
}


void ConfigResourcePool::BootstrapBulk( const nchar* category, ... )
{
    va_list args;
    va_start(args, category);
    auto cursor= Self().Root();
#if ALIB_CHARACTERS_WIDE
    String256 wCategory(category);
    ALIB_ASSERT_RESULT_EQUALS(cursor.GoToCreateChildIfNotExistent( wCategory ), true)
#else
    ALIB_ASSERT_RESULT_EQUALS(cursor.GoToCreateChildIfNotExistent( category  ), true)
#endif
    for(;;)
    {
        NString name=  va_arg( args, const nchar* );

        if( name.IsNull() )
            break;

        #if ALIB_CHARACTERS_WIDE
        String256 wName(name);
          auto child= cursor.CreatePathIfNotExistent(wName).first;
        #else
          auto child= cursor.CreatePathIfNotExistent(name).first;
        #endif
        if( !child.IsValid() )
        {
            ALIB_WARNING("RESOURCES", "ConfigResourcePool: Resource value existed already. Name=",
                                      name)
            continue;
        }

        Variable var(child, A_CHAR("S") );
        (void) var.Define(Priority::DefaultValues);
        var.GetString().Reset( String(va_arg( args, const character* )) );

        // \checkpromise: when typed ALib assertions and warnings are available, then
        // raise a warning if the result of above EmplaceOrAssign is an assign, aka the
        // bulk data existed already.
    }
    va_end(args);
}


const String& ConfigResourcePool::Get( const NString& category, const NString& name   ALIB_DBG(, bool dbgAssert ) )
{

    String256 path(category); path << '/' << name;
    Variable var(Self());
    if( var.Try(path) )
        return var.GetString();

    ALIB_ASSERT_ERROR( !dbgAssert, "RESOURCES",
                                   "Unknown resource! Category: {}, Name: ", category, name )
    return NULL_STRING;
}

} // namespace [alib::variables]
#endif // ALIB_RESOURCES
