// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB)
#      include "alib/alib.hpp"
#   endif

#   if !defined (HPP_ALIB_LANG_BASECAMP)
#      include "alib/lang/basecamp/basecamp.hpp"
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

#   if !defined (HPP_ALIB_LANG_RESOURCES_LOCALRESOURCEPOOL)
#      include "alib/lang/resources/localresourcepool.hpp"
#   endif

#   if  ALIB_TIME
#      if !defined (HPP_ALIB_TIME_TIME)
#         include "alib/time/time.hpp"
#      endif
#   endif

#   if ALIB_THREADS && !defined (HPP_ALIB_THREADS_THREAD)
#      include "alib/threads/thread.hpp"
#   endif


#   if ALIB_ALOX
#      if !defined (HPP_ALIB_ALOXMODULE)
#         include "alib/alox/aloxmodule.hpp"
#      endif
#   endif
#   if ALIB_CLI
#      if !defined (HPP_ALIB_CLI_CLI)
#         include "alib/cli/cli.hpp"
#      endif
#   endif
#   if ALIB_EXPRESSIONS
#      if !defined (HPP_ALIB_EXPRESSIONS_EXPRESSIONS)
#         include "alib/expressions/expressions.hpp"
#      endif
#   endif
#   if ALIB_FILES
#      if !defined (HPP_ALIB_FILES_CAMP)
#         include "alib/files/filescamp.hpp"
#      endif
#   endif
#   if !defined (HPP_ALIB_LANG_CAMP_BOOTSTRAP)
#      include "alib/lang/basecamp/bootstrap.hpp"
#   endif

#   if !defined (HPP_ALIB_LANG_CAMP_INLINES)
#      include "alib/lang/basecamp/camp_inlines.hpp"
#   endif
#endif // !defined(ALIB_DOX)

//#include <iostream>

using namespace alib::lang;

namespace alib {

List<lang::Camp*>   Camps(&monomem::GlobalAllocator);

void BootstrapAddDefaultCamps()
{
    ALIB_ASSERT_ERROR( Camps.IsEmpty(), "CAMPS", "List Camps already set." )

    ALIB_IF_CAMP(          Camps.PushBack( &BASECAMP    );  )
    ALIB_IF_CONFIGURATION( Camps.PushBack( &CONFIG      );  )
    ALIB_IF_ALOX(          Camps.PushBack( &ALOX        );  )
    ALIB_IF_CLI(           Camps.PushBack( &CLI         );  )
    ALIB_IF_EXPRESSIONS(   Camps.PushBack( &EXPRESSIONS );  )
    ALIB_IF_FILES(         Camps.PushBack( &FILES       );  )
}

void Bootstrap( BootstrapPhases     targetPhase,
                Camp*               targetCamp,
                int alibVersion, int alibRevision, TCompilationFlags compilationFlags )
{
    // verify ALib
    AssertALibVersionAndFlags( alibVersion, alibRevision, compilationFlags );

    // if not customized, create default module list
    if( Camps.IsEmpty() )
        BootstrapAddDefaultCamps();

    if( targetCamp == nullptr )
        targetCamp= Camps.Back();

    //std::cout << "Camp::Bootstrap called on: '" << this->ResourceCategory << "', target phase: " << int(targetPhase) << std::endl;

    // Initialize non-camp modules once
    if( !NonCampModulesInitialized )
    {
        ALIB_IF_TIME(       time::Bootstrap(); )
        ALIB_IF_BOXING(   boxing::Bootstrap(); )
        ALIB_IF_THREADS( threads::Bootstrap(); )
        ALIB_IF_ENUMS(     enums::Bootstrap(); )
        NonCampModulesInitialized= true;
    }

    // find target camp in the list of camps
    auto targetCampIt= Camps.rbegin();
    while(targetCampIt != Camps.rend() &&
          *targetCampIt != targetCamp )
        ++targetCampIt;

    ALIB_ASSERT_ERROR(targetCampIt != Camps.rend(), "CAMPS",
      "Target camp given to function alib::Bootstrap() is not included in list alib::Camps.\n"
      "Resource category of the target camp: ", targetCamp->ResourceCategory )


    // loop over all phases
    for ( int phaseIntegral= targetCamp->bootstrapState + 1 ;
              phaseIntegral <= UnderlyingIntegral( targetPhase ) ;
            ++phaseIntegral )
    {
        BootstrapPhases actualPhase = BootstrapPhases( phaseIntegral );

        // phase 1: distribute a resource pool
        if ( actualPhase == BootstrapPhases::PrepareResources  )
        {
            // create a resource pool?
            if ( targetCamp->resourcePool == nullptr )
            {
                targetCamp->isResourceOwner = true;
                resources::LocalResourcePool* pool=
                      monomem::GlobalAllocator.Emplace<resources::LocalResourcePool>();
                targetCamp->resourcePool= pool;

                // \releasetask{update resource numbers numbers}
                integer                  expectedSize=   97  // ALIB distribution resources
                ALIB_IF_ALOX           (               + 42   )
                ALIB_IF_CONFIGURATION  (               + 9    )
                ALIB_IF_CLI            (               + 17   )
                ALIB_IF_EXPRESSIONS    (               + 256  )
                ALIB_IF_FILES          (               + 43    ) ;

                auto& hashMap= pool->BootstrapGetInternalHashMap();
                hashMap.BaseLoadFactor( 2.0 );
                hashMap.MaxLoadFactor ( 5.0 );
                hashMap.Reserve       ( expectedSize, ValueReference::Absolute );
            }


            // loop in reverse order over modules, start with this module
            auto* actPool     = targetCamp->resourcePool;
            for(auto campIt=   targetCampIt ; campIt != Camps.rend() ; ++campIt )
            {

                // if a different resources object is set, then use that one from now on
                if((*campIt)->resourcePool != nullptr && (*campIt)->resourcePool != actPool)
                {
                    actPool= (*campIt)->resourcePool;
                    continue;
                }

                (*campIt)->resourcePool= actPool;

            } // resources distribution loop
        }

        // phase 2: create and distribute a configuration
        #if ALIB_CONFIGURATION
        else if ( actualPhase == BootstrapPhases::PrepareConfig)
        {
            // create a configuration?
            if ( targetCamp->config == nullptr )
            {
                targetCamp->isConfigOwner= true;
                auto & alloc = monomem::AcquireGlobalAllocator( ALIB_CALLER_PRUNED );

                targetCamp->config = alloc.Emplace<Configuration>( CreateDefaults::No );

                targetCamp->config->InsertPlugin( alloc.Emplace<InMemoryPlugin>( CONFIG.GetResource( "CfgPlgDef" ) ),
                                                  Priorities::DefaultValues,    Responsibility::KeepWithSender );

                targetCamp->config->InsertPlugin( alloc.Emplace<config::Environment>(),
                                                  Priorities::Environment,      Responsibility::KeepWithSender );

                targetCamp->config->InsertPlugin( alloc.Emplace<config::CLIArgs>(),
                                                  Priorities::CLI,              Responsibility::KeepWithSender );

                targetCamp->config->InsertPlugin( alloc.Emplace<InMemoryPlugin>( CONFIG.GetResource( "CfgPlgPro" ) ),
                                                  Priorities::ProtectedValues,  Responsibility::KeepWithSender );

                monomem::ReleaseGlobalAllocator();

                if ( ArgC > 0 )
                {
                    if ( ArgVN != nullptr )
                        targetCamp->config->SetCommandLineArgs( ArgC, ArgVN );
                    else
                        targetCamp->config->SetCommandLineArgs( ArgC, ArgVW );
                }
            }

            // loop in reverse order over modules, start with this module
            auto* actConfig = targetCamp->config;
            for(auto module=   targetCampIt ; module != Camps.rend() ; ++module )
            {
                // if a different resources object is set, then use that one from now on
                if( (*module)->config != nullptr && (*module)->config != actConfig)
                    actConfig= (*module)->config;
                else
                    (*module)->config= actConfig;

            } // resources distribution loop
        }
        #endif

        // initialize modules on this phase
        ALIB_DBG( bool foundThisModuleInList = false; )
        for ( auto* camp : Camps )
        {
            if(camp->bootstrapState >= UnderlyingIntegral(actualPhase ) )
                 continue;

            //std::cout << "Camp::Bootstrap  '" << module->ResourceCategory << "', phase: " << int(actualPhase) << std::endl;

            // propagate resource pool and config, if sub-module does not provide an own instance
            if ( actualPhase == BootstrapPhases::PrepareResources &&
                 camp->resourcePool == nullptr )
            {
                camp->resourcePool = targetCamp->resourcePool;
            }
            #if ALIB_CONFIGURATION
            else if ( actualPhase == BootstrapPhases::PrepareConfig &&
                      camp->config == nullptr )
            {
                camp->config = targetCamp->config;
            }
            #endif

            // bootstrap module
            ALIB_ASSERT_ERROR( camp->bootstrapState == phaseIntegral - 1,
              "With this invocation of Bootstrap() a camp skips a bootstrap phase \n"
              "Resource category of the target camp: ", camp->ResourceCategory         )
            camp->bootstrap(actualPhase );

            camp->bootstrapState= UnderlyingIntegral(actualPhase );

            // stop if this is us
            if (camp == targetCamp )
            {
                ALIB_DBG( foundThisModuleInList = true );
                break;
            }
        }
        ALIB_ASSERT_ERROR( foundThisModuleInList, "CAMPS",
          "The target camp of function Bootstrap is not included in list alib::Camps "
          "or was already bootstrapped for this phase!\n"
          "Resource category of the target camp: ", targetCamp->ResourceCategory )

    }
}

void Shutdown( ShutdownPhases targetPhase,
               Camp*                targetCamp   )
{
    if( targetCamp == nullptr )
        targetCamp= Camps.Front();

    //std::cout << "Camp::Shutdown called on'" << targetCamp->ResourceCategory << "', target phase: " << int(targetPhase) << std::endl;

    // find target camp in the list of camps
    auto targetCampIt= Camps.begin();
    while(    targetCampIt != Camps.end()
          && *targetCampIt != targetCamp )
        ++targetCampIt;
    ALIB_ASSERT_ERROR(targetCampIt != Camps.end(), "CAMPS",
      "Target camp given to function alib::Shutdown() is not included in list alib::Camps.\n"
      "Resource category of the target camp: ", targetCamp->ResourceCategory )


    ALIB_DBG( bool foundThisModuleInList= false );
    // loop over all (both) termination levels
    for( auto phaseIntegral=  UnderlyingIntegral( ShutdownPhases::Announce )
          ;   phaseIntegral<= UnderlyingIntegral( targetPhase )
          ; ++phaseIntegral                                                   )
    {
        ShutdownPhases actualPhase = ShutdownPhases( phaseIntegral );

        // shutdown in reverse order
        for(auto campIt= Camps.rbegin() ; campIt != Camps.rend() ; ++campIt )
        {
            ALIB_ASSERT_ERROR(    ( *campIt )->bootstrapState < 0
                               || ( *campIt )->bootstrapState == UnderlyingIntegral(BootstrapPhases::Final ),
                "CAMPS", "Trying to terminate a not (fully) initialized module. "
                "Module Name (resource category): ", targetCamp->ResourceCategory )

            // shutdown module
            if (( *campIt )->bootstrapState > -UnderlyingIntegral(actualPhase) )
            {
                //std::cout << "Camp::Shutdown '" << (*campIt)->ResourceCategory << "', phase: " << int(actualPhase) << std::endl;

                ALIB_ASSERT_ERROR(    ( ( *campIt )->bootstrapState == 3  &&  phaseIntegral == 1 )
                                   || ( ( *campIt )->bootstrapState == -1 &&  phaseIntegral == 2 ),
                  "CAMPS", "With this invocation of Bootstrap(), a camp skips a bootstrap phase \n"
                  "Resource category of the target camp: ", ( *campIt )->ResourceCategory         )


                ( *campIt )->shutdown(actualPhase );
                ( *campIt )->bootstrapState = -UnderlyingIntegral(actualPhase );

                ALIB_DBG( if(( *campIt ) == targetCamp )
                             foundThisModuleInList= true;        )
             }            

            if(( *campIt ) == targetCamp )
                break;
        }
    }
    ALIB_ASSERT_ERROR( foundThisModuleInList, "CAMPS",
      "The target camp of function Shutdown is not included in list alib::Camps "
      "or was already shutdown for this phase!\n"
      "Resource category of the target camp: ", targetCamp->ResourceCategory         )


    // delete resources/config
    if( targetPhase == ShutdownPhases::Destruct)
    for(auto campIt= Camps.rbegin() ; campIt != Camps.rend() ; ++campIt )
    {
        if ( ( *campIt )->isResourceOwner )  monomem::Destruct(( *campIt )->resourcePool );
        #if ALIB_CONFIGURATION
            if ( ( *campIt )->isConfigOwner )
            {
                Configuration* config= (*campIt)->config;
                auto *im = config->GetPluginTypeSafe<InMemoryPlugin>( Priorities::DefaultValues );
                if ( im ) { config->RemovePlugin( Priorities::DefaultValues );
                            monomem::Destruct( im );                                     }

                auto *e = config->GetPluginTypeSafe<config::Environment>( Priorities::Environment );
                if ( e )  { config->RemovePlugin( Priorities::Environment );
                            monomem::Destruct( e );                                      }

                auto *cli = config->GetPluginTypeSafe<config::CLIArgs>( Priorities::CLI );
                if ( cli ){ config->RemovePlugin( Priorities::CLI );
                            monomem::Destruct( cli );                                    }

                im = config->GetPluginTypeSafe<InMemoryPlugin>( Priorities::ProtectedValues );
                if ( im ) { config->RemovePlugin( Priorities::ProtectedValues );
                            monomem::Destruct( im );                                    }

              monomem::Destruct( config );
            }
        #endif

        if(( *campIt ) == targetCamp )
            break;
    }

    if( targetPhase == ShutdownPhases::Destruct )
    {
        ALIB_IF_THREADS(       threads::Shutdown(); )
        ALIB_IF_TIME(             time::Shutdown(); )
        ALIB_IF_SINGLETONS( singletons::Shutdown(); )
    }
}

} // namespace [alib]
