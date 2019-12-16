// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_FS_MODULES_MODULE)
#      include "alib/lib/fs_modules/module.hpp"
#   endif
#   if !defined(HPP_ALIB_FS_MODULES_DISTRIBUTION)
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif

#   if ALIB_BOXING && !defined (HPP_ALIB_BOXING_BOXING)
#      include "alib/boxing/boxing.hpp"
#   endif

#   if ALIB_CONFIGURATION
#      if !defined (HPP_ALIB_CONFIG_CONFI)
#          include "alib/config/config.hpp"
#      endif
#      if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
#          include "alib/config/configuration.hpp"
#      endif
#   endif

#   if !defined (HPP_ALIB_ENUMS_ITERATABLE)
#      include "alib/enums/iteratable.hpp"
#   endif

#   if !defined (HPP_ALIB_RESOURCES_LocalResourcePool)
#      include "alib/resources/localresourcepool.hpp"
#   endif

#   if  ALIB_TIME
#      if !defined (HPP_ALIB_TIME_TIME)
#         include "alib/time/time.hpp"
#      endif
#   endif

#   if ALIB_THREADS && !defined (HPP_ALIB_THREADS_THREAD)
#      include "alib/threads/thread.hpp"
#   endif


#   if !defined (_GLIBCXX_IOSTREAM) && !defined (_IOSTREAM_ )
#      include <iostream>
#   endif
#   if !defined (_GLIBCXX_IOMANIP) && !defined (_IOMANIP_ )
#      include <iomanip>
#   endif
#endif // !defined(ALIB_DOX)



namespace aworx { namespace lib {

// static fields
bool                Module::microModulesInitialized     = false;

bool Module::Bootstrap( BootstrapPhases targetPhase, int argc, const char** argvN, const wchar_t** argvW )
{
    // if not customized, create default module list
    if( ALIB.Modules.IsEmpty() )
        ALIB.BootstrapFillDefaultModuleList();


    // Initialize "micro" modules once
    if( !Module::microModulesInitialized )
    {
        ALIB_IF_THREADS( threads::Bootstrap(); )
        ALIB_IF_BOXING(   boxing::Bootstrap(); )
        ALIB_IF_TIME(       time::Bootstrap(); )
        Module::microModulesInitialized= true;
    }

    // args given but not first phase?
    ALIB_ASSERT_WARNING( argc == 0 || bootstrapState == 0,
                         "Command line args ignored. "
                         "Accepted only with initialization level 'PrepareResources'."  )

    if( bootstrapState >= UnderlyingIntegral( targetPhase )  )
    {
        ALIB_WARNING( "Given initialization level already performed on module.  "
                       "Module Name (resource category): ", ResourceCategory  )
        return false;
    }

    // find this module in the list of modules
    auto thisModuleIt= ALIB.Modules.rbegin();
    while(  thisModuleIt != ALIB.Modules.rend() &&
           *thisModuleIt != this )
        ++thisModuleIt;

    ALIB_ASSERT_ERROR( thisModuleIt != ALIB.Modules.rend(),
      "Trying to bootstrap a module that is not included in list ALibDistribution::Modules.")


    // loop over all phases
    for ( int phaseIntegral= bootstrapState + 1 ;
              phaseIntegral <= UnderlyingIntegral( targetPhase ) ;
            ++phaseIntegral )
    {
        BootstrapPhases actualPhase = BootstrapPhases( phaseIntegral );

        // phase 1: distribute a resource pool
        if ( actualPhase == BootstrapPhases::PrepareResources  )
        {
            // create a resource pool?
            if ( resourcePool == nullptr )
            {
                isResourceOwner = true;
                resources::LocalResourcePool* pool= lib::monomem::GlobalAllocator.Emplace<resources::LocalResourcePool>();
                resourcePool= pool;

                integer                  expectedSize= 35; // ALIB distribution resources
                ALIB_IF_ALOX           ( expectedSize+=  43 ; )
                ALIB_IF_CONFIGURATION  ( expectedSize+=  9  ; )
                ALIB_IF_CLI            ( expectedSize+=  17 ; )
                ALIB_IF_EXPRESSIONS    ( expectedSize+=  135; )
                ALIB_IF_RESULTS        ( expectedSize+=  5  ; )
                ALIB_IF_TEXT   ( expectedSize+=  48 ; )
                ALIB_IF_SYSTEM         ( expectedSize+=  9  ; )
                auto& hashMap= pool->BootstrapGetInternalHashMap();
                hashMap.BaseLoadFactor( 2.0 );
                hashMap.MaxLoadFactor ( 5.0 );
                hashMap.Reserve       ( expectedSize );
            }


            // loop in reverse order over modules, start with this module
            auto* actPool     = resourcePool;
            for( auto moduleIt=   thisModuleIt ; moduleIt != ALIB.Modules.rend() ; ++moduleIt )
            {
                // if a different resources object is set, then use that one from now on
                if( (*moduleIt)->resourcePool != nullptr && (*moduleIt)->resourcePool != actPool)
                {
                    actPool= (*moduleIt)->resourcePool;
                    continue;
                }

                (*moduleIt)->resourcePool= actPool;

            } // resources distribution loop
        }

        // phase 2: create and distribute a configuration
        #if !ALIB_CONFIGURATION
            (void) argc; (void) argvN; (void) argvW;
        #else
        else if ( actualPhase == BootstrapPhases::PrepareConfig)
        {
            // create a configuration?
            if ( config == nullptr )
            {
                isConfigOwner = true;
                auto & alloc = lib::monomem::AcquireGlobalAllocator( ALIB_CALLER_PRUNED );

                config = alloc.Emplace<Configuration>( CreateDefaults::No );

                config->InsertPlugin(
                    alloc.Emplace<InMemoryPlugin>( CONFIG.GetResource( "CfgPlgDef" ) ),
                    Priorities::DefaultValues, Responsibility::KeepWithSender );

                config->InsertPlugin( alloc.Emplace<lib::config::Environment>(),
                                      Priorities::Environment, Responsibility::KeepWithSender );

                config->InsertPlugin( alloc.Emplace<lib::config::CLIArgs>(),
                                      Priorities::CLI, Responsibility::KeepWithSender );

                config->InsertPlugin(
                    alloc.Emplace<InMemoryPlugin>( CONFIG.GetResource( "CfgPlgPro" ) ),
                    Priorities::ProtectedValues, Responsibility::KeepWithSender );

                lib::monomem::ReleaseGlobalAllocator();

                if ( argc > 0 )
                {
                    if ( argvN != nullptr )
                        config->SetCommandLineArgs( argc, argvN );
                    else
                        config->SetCommandLineArgs( argc, argvW );
                }
            }

            // loop in reverse order over modules, start with this module
            auto* actConfig = config;
            for( auto module=   thisModuleIt ;  module != ALIB.Modules.rend() ; ++module )
            {
                // if a different resources object is set, then use that one from now on
                if( (*module)->config != nullptr && (*module)->config != actConfig)
                {
                    actConfig= (*module)->config;
                    continue;
                }

                (*module)->config= actConfig;

            } // resources distribution loop
        }
        #endif

        // initialize modules on this phase
        ALIB_DBG( bool foundThisModuleInList = false; )
        for ( auto* module : ALIB.Modules )
        {
            if( module->bootstrapState >= UnderlyingIntegral( actualPhase ) )
                 continue;


            // propagate resource pool and config, if sub-module does not provide an own instance
            if ( actualPhase == BootstrapPhases::PrepareResources &&
                 module->resourcePool == nullptr )
            {
                module->resourcePool = resourcePool;
            }
                #if ALIB_CONFIGURATION
            else if ( actualPhase == BootstrapPhases::PrepareConfig &&
                      module->config == nullptr )
            {
                module->config = config;
            }
            #endif

            // bootstrap module
            module->bootstrap( actualPhase, argc, argvN, argvW );

            module->bootstrapState= UnderlyingIntegral( actualPhase );

            // stop if this is us
            if ( module == this )
            {
                ALIB_DBG( foundThisModuleInList = true );
                break;
            }
        }
        ALIB_ASSERT_ERROR( foundThisModuleInList,
                           "The module that method Bootstrap was invoked on is not includeed in "
                           "list Module::BootstrapModules." )
    }

    return true;
}

void Module::Shutdown( ShutdownPhases targetPhase )
{

    // check and set ourselves initialized
    if( bootstrapState <= - UnderlyingIntegral( targetPhase ) )
    {
       ALIB_WARNING( "Termination level already performed" )
       return;
    }


    // loop over all (both) termination levels
    for( auto phaseIntegral=  UnderlyingIntegral( ShutdownPhases::Announce )
          ;   phaseIntegral<= UnderlyingIntegral( targetPhase )
          ; ++phaseIntegral                                                   )
    {

        if( bootstrapState <= - phaseIntegral )
            continue;

        ShutdownPhases actualPhase = ShutdownPhases( phaseIntegral );

        // shutdown in reverse order
        bool foundThisModule= false;
        for( auto module=   ALIB.Modules.rbegin()
              ;   module != ALIB.Modules.rend()
              ; ++module                                        )
        {
            // not us (yet)
            if ( !foundThisModule )
            {
                if ( *module != this )
                    continue;
                foundThisModule = true;
            }

            ALIB_ASSERT_ERROR(
                ( *module )->bootstrapState == UnderlyingIntegral( BootstrapPhases::Final )
                || ( *module )->bootstrapState < 0,
                "Trying to terminate a not (fully) initialized module. "
                "Module Name (resource category): ", ResourceCategory )

            // shutdown module
            if ( ( *module )->bootstrapState > -UnderlyingIntegral( actualPhase ) )
            {
                ( *module )->shutdown( actualPhase );
                ( *module )->bootstrapState = -UnderlyingIntegral( actualPhase );
            }
            else
                break; // if already done on this phase, modules following are likewise done

        } // module loop

        if( actualPhase == ShutdownPhases::Destruct )
        {
            #if ALIB_CONFIGURATION
            if ( isResourceOwner ) monomem::Destruct( resourcePool );
            if ( isConfigOwner )
            {
                auto *im = config->GetPluginTypeSafe<InMemoryPlugin>( Priorities::DefaultValues );
                if ( im )
                {
                    config->RemovePlugin( Priorities::DefaultValues );
                    monomem::Destruct( im );
                }


                auto *e = config->GetPluginTypeSafe<lib::config::Environment>(
                    Priorities::Environment );
                if ( e )
                {
                    config->RemovePlugin( Priorities::Environment );
                    monomem::Destruct( e );
                }


                auto *cli = config->GetPluginTypeSafe<lib::config::CLIArgs>( Priorities::CLI );
                if ( cli )
                {
                    config->RemovePlugin( Priorities::CLI );
                    monomem::Destruct( cli );
                }


                im = config->GetPluginTypeSafe<InMemoryPlugin>( Priorities::ProtectedValues );
                if ( im )
                {
                    config->RemovePlugin( Priorities::ProtectedValues );
                    monomem::Destruct( im );
                }

                monomem::Destruct( config );
            }
            #endif

            ALIB_IF_THREADS(       threads::Shutdown(); )
            ALIB_IF_TIME(             time::Shutdown(); )
            ALIB_IF_SINGLETONS( singletons::Shutdown(); )
        }
    }
}




bool Module::VerifyCompilationFlags( uint64_t flags )
{
    // verify the flags
    if ( flags == CompilationFlags )
        return true;

    // dump out the flags
    std::cout << std::left << std::setw(30) <<  "Symbol" << '|' << std::setw(5) << " Lib" <<'|' << " Comp. Unit"  << std::endl;

    int cnt= 0;
    for( auto& p : CompilationFlagMeanings )
    {
        ++cnt;
        std::cout << std::setw(30) <<  p.first << '|' << std::setw(5) << (CompilationFlags & p.second  ? " On" : " Off")
                                               << "|" << std::setw(5) << (flags            & p.second  ? " On" : " Off")
                  << std::endl;
    }

    return false;
}




}} // namespace [aworx::lib]
