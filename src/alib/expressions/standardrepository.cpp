//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/expressions/expressions.prepro.hpp"

//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Expressions;
#else
#   include "ALib.Expressions.H"
#endif
//========================================== Implementation ========================================
namespace alib {  namespace expressions {

int StandardRepository::StoreLoadedExpressions( Compiler*  compiler, Priority slot ) {
    int count= 0;
    for( auto& entry : VariablesLoaded )
        if( std::get<0>( entry ) == slot ) {
            ++count;
            Variable var(*config, std::get<1>( entry ), A_CHAR("S") );
            var= compiler->GetNamed( std::get<2>( entry ))->GetNormalizedString();
        }

    return count;
}

bool StandardRepository::Get( const String& identifier, AString& target ) {
    if( config != nullptr ) {
        if( ConfigPaths.empty() )
            ConfigPaths.emplace_back( "" );

        // search in given default categories first.
        Variable var(*config);
        for( auto& path : ConfigPaths )
        {ALIB_LOCK_WITH(config)
            String256 name(path); name << '/' <<  identifier;
            if( var.Try(name, A_CHAR("S") ) && var.IsDefined() ) {
                target << var.GetString();
                VariablesLoaded.emplace_back( var.GetPriority(), name, identifier );
                return true;
    }   }   }

    if( resources != nullptr ) {
        auto& result= resources->Get( resourceCategory, identifier ALIB_DBG(, false) );
        if( result.IsNotNull() ) {
            target << result;
            return true;
    }   }

    // failed
    return false;

}

}} // namespace [alib::expressions]
