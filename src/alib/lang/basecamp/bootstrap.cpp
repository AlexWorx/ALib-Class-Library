// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/alib.hpp"
#   include "alib/lang/basecamp/basecamp.hpp"
#   include "alib/lang/system/path.hpp"
#   include "alib/lang/format/fmtexceptions.hpp"
#   if ALIB_BOXING
#      include "alib/boxing/boxing.hpp"
#   endif
#   if ALIB_CONFIGURATION
#       include "alib/config/configcamp.hpp"
#       include "alib/config/configuration.hpp"
#       include "alib/config/plugins.hpp"
#   endif
#   include "alib/lang/resources/localresourcepool.hpp"
#   if  ALIB_TIME
#       include "alib/time/time.hpp"
#   endif
#   if ALIB_THREADMODEL
#      include "alib/threadmodel/dedicatedworker.hpp"
#   endif
#   if ALIB_THREADS
#      include "alib/threads/thread.hpp"
#   endif
#   if ALIB_ALOX
#       include "alib/alox/aloxcamp.hpp"
#   endif
#   if ALIB_CLI
#       include "alib/cli/clicamp.hpp"
#   endif
#   if ALIB_EXPRESSIONS
#       include "alib/expressions/expressionscamp.hpp"
#   endif
#   if ALIB_FILES
#       include "alib/files/filescamp.hpp"
#   endif
#   include "alib/lang/basecamp/bootstrap.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#endif // !DOXYGEN

//#include <iostream>

using namespace alib::lang;

namespace alib {

#if !DOXYGEN
List<MonoAllocator, Camp*>   CAMPS(monomem::GLOBAL_ALLOCATOR);
#include "alib/lang/callerinfo_functions.hpp"
#endif

void BootstrapAddDefaultCamps()
{
    // if the global allocator was not initialized from outside, then we have to do it.
    if (!monomem::GLOBAL_ALLOCATOR.IsInitialized())
        new (&monomem::GLOBAL_ALLOCATOR) MonoAllocator(ALIB_DBG("Global",) 128);

    ALIB_ASSERT_ERROR( CAMPS.IsEmpty(), "CAMPS", "List CAMPS already set." )

    IF_ALIB_CAMP(          CAMPS.PushBack( &BASECAMP    );  )
    IF_ALIB_CONFIGURATION( CAMPS.PushBack( &CONFIG      );  )
    IF_ALIB_ALOX(          CAMPS.PushBack( &ALOX        );  )
    IF_ALIB_CLI(           CAMPS.PushBack( &CLI         );  )
    IF_ALIB_EXPRESSIONS(   CAMPS.PushBack( &EXPRESSIONS );  )
    IF_ALIB_FILES(         CAMPS.PushBack( &FILES       );  )
}

void Bootstrap( BootstrapPhases     targetPhase,
                Camp*               targetCamp,
                int alibVersion, int alibRevision, TCompilationFlags compilationFlags )
{
    // verify ALib
    AssertALibVersionAndFlags( alibVersion, alibRevision, compilationFlags );

    #if ALIB_MONOMEM
        // if the global allocator was not initialized from outside, then we have to do it.
        if (!monomem::GLOBAL_ALLOCATOR.IsInitialized())
            new (&monomem::GLOBAL_ALLOCATOR) MonoAllocator(ALIB_DBG("Global",) 128);
    #endif

    // if not customized, create default module list
    if( CAMPS.IsEmpty() )
        BootstrapAddDefaultCamps();

    if( targetCamp == nullptr )
        targetCamp= CAMPS.Back();

    //std::cout << "Camp::Bootstrap called on: '" << this->ResourceCategory << "', target phase: " << int(targetPhase) << std::endl;

    // Initialize non-camp modules once
    if( !NonCampModulesInitialized )
    {
        IF_ALIB_TIME       (time  ::Bootstrap     ( );)
        IF_ALIB_BOXING     (boxing::Bootstrap     ( );)

        // now we add the base-camps' boxing vtables already. This is primarily needed
        // for assertions in debug-compilation ( vt_alib_report_types )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_system_exceptions   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_system_systemerrors )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_system_path         )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_system_fmtexceptions)
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_exception      )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_report_types   )

        IF_ALIB_THREADS    (threads    ::Bootstrap    ( );)
        IF_ALIB_ENUMS      (enums      ::Bootstrap      ( );)
        IF_ALIB_THREADMODEL(threadmodel::Bootstrap      ( );)
        NonCampModulesInitialized= true;
    }

    // find target camp in the list of camps
    auto targetCampIt= CAMPS.rbegin();
    while(targetCampIt != CAMPS.rend() &&
          *targetCampIt != targetCamp )
        ++targetCampIt;

    ALIB_ASSERT_ERROR(targetCampIt != CAMPS.rend(), "CAMPS",
      "Target camp given to function alib::Bootstrap() is not included in list alib::CAMPS.\n"
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
                               monomem::GLOBAL_ALLOCATOR().New<resources::LocalResourcePool>();

                targetCamp->resourcePool= pool;

                // \releasetask{update resource numbers numbers}
                integer                  expectedSize=   97  // ALIB distribution resources
                IF_ALIB_ALOX           (               + 42   )
                IF_ALIB_CONFIGURATION  (               + 9    )
                IF_ALIB_CLI            (               + 17   )
                IF_ALIB_EXPRESSIONS    (               + 256  )
                IF_ALIB_FILES          (               + 43    ) ;

                auto& hashMap= pool->BootstrapGetInternalHashMap();
                hashMap.BaseLoadFactor( 2.0 );
                hashMap.MaxLoadFactor ( 5.0 );
                hashMap.Reserve       ( expectedSize, ValueReference::Absolute );
            }


            // loop in reverse order over modules, start with this module
            auto* actPool  =  targetCamp->resourcePool;
            for(auto campIt=  targetCampIt ; campIt != CAMPS.rend() ; ++campIt )
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
                auto* monoAllocator = monomem::GLOBAL_ALLOCATOR().New<MonoAllocator>(
                                                                   ALIB_DBG("Configuration",) 16u);
IF_ALIB_THREADS(targetCamp->configLock= (*monoAllocator)().New<SharedLock>();)
                targetCamp->config    = (*monoAllocator)().New<Configuration>(*monoAllocator);
                #if ALIB_DEBUG_CRITICAL_SECTIONS
                targetCamp->config->NodeTable().dcs.DCSName= "ALib-Camp-Configuration";
                #endif
            }

            // loop in reverse order over modules, start with this module
            auto* actConfig         = targetCamp->config;
IF_ALIB_THREADS(auto* actConfigLock = targetCamp->configLock; )
            for(auto module=   targetCampIt ; module != CAMPS.rend() ; ++module )
            {
                // if a different resources object is set, then use that one from now on
                if( (*module)->config != nullptr && (*module)->config != actConfig)
                {
                    actConfig    = (*module)->config;
IF_ALIB_THREADS(    actConfigLock= (*module)->configLock;
           ALIB_DBG( (*module)->configLock->Dbg.Name= "CampConfig";)          )
                }
                else
                {
                    (*module)->config    = actConfig;
IF_ALIB_THREADS(    (*module)->configLock= actConfigLock;  )
                }

            } // resources distribution loop
        }
        #endif

        // initialize modules on this phase
        ALIB_DBG( bool foundThisModuleInList = false; )
        for ( auto* camp : CAMPS )
        {
            // bootstrap camp
            if(camp->bootstrapState >= UnderlyingIntegral(actualPhase ) )
                 continue;

            //std::cout << "Camp::Bootstrap  '" << module->ResourceCategory << "', phase: " << int(actualPhase) << std::endl;

            ALIB_ASSERT_ERROR( camp->bootstrapState == phaseIntegral - 1,
              "With this invocation of Bootstrap() a camp skips a bootstrap phase.\n"
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
          "The target camp of function Bootstrap is not included in list alib::CAMPS "
          "or was already bootstrapped for this phase!\n"
          "Resource category of the target camp: ", targetCamp->ResourceCategory )
    }

    // Are all camps finalized?
    if ( targetPhase == BootstrapPhases::Final && targetCamp == CAMPS.Back() )
    {
        #if ALIB_DEBUG_CRITICAL_SECTIONS
        #   if ALIB_MONOMEM
            monomem::GLOBAL_ALLOCATOR.DbgCriticalSectionsPH.Get()->DCSLock= &monomem::GLOBAL_ALLOCATOR_LOCK;
            monomem::GLOBAL_ALLOCATOR_LOCK.Dbg.Name= "GlobalAllocator";
        #   endif
        #   if ALIB_CONFIGURATION
            targetCamp->config->NodeTable().dcs.DCSLock= targetCamp->configLock;
        #   endif
        #endif


    }
}

void Shutdown( ShutdownPhases targetPhase,
               Camp*          targetCamp   )
{
    #if ALIB_DEBUG_CRITICAL_SECTIONS && ALIB_MONOMEM
        monomem::GLOBAL_ALLOCATOR.DbgCriticalSectionsPH.Get()->DCSLock= nullptr;
    #endif


    ALIB_ASSERT_ERROR( CAMPS.IsNotEmpty(), "CAMPS", "Empty camp list on shutdown. Shutdown invoked twice?"  )
    if( targetCamp == nullptr )
        targetCamp= CAMPS.Front();

    //std::cout << "Camp::Shutdown called on'" << targetCamp->ResourceCategory << "', target phase: " << int(targetPhase) << std::endl;

    // find target camp in the list of camps
    auto targetCampIt= CAMPS.begin();
    while(    targetCampIt != CAMPS.end()
          && *targetCampIt != targetCamp )
        ++targetCampIt;
    ALIB_ASSERT_ERROR(targetCampIt != CAMPS.end(), "CAMPS",
      "Target camp given to function alib::Shutdown() is not included in list alib::CAMPS.\n"
      "Resource category of the target camp: ", targetCamp->ResourceCategory )


    ALIB_DBG( bool foundThisModuleInList= false );
    // loop over all (both) termination levels
    for( auto phaseIntegral=  UnderlyingIntegral( ShutdownPhases::Announce )
          ;   phaseIntegral<= UnderlyingIntegral( targetPhase )
          ; ++phaseIntegral                                                   )
    {
        ShutdownPhases actualPhase = ShutdownPhases( phaseIntegral );

        // shutdown in reverse order
        for(auto campIt= CAMPS.rbegin() ; campIt != CAMPS.rend() ; ++campIt )
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
      "The target camp of function Shutdown is not included in list alib::CAMPS "
      "or was already shutdown for this phase!\n"
      "Resource category of the target camp: ", targetCamp->ResourceCategory         )


    #if ALIB_DEBUG_CRITICAL_SECTIONS && ALIB_CONFIGURATION
        // deactivate assertions for non-locked access to configuration
        if( targetPhase == ShutdownPhases::Announce)
            for(auto campIt= CAMPS.rbegin() ; campIt != CAMPS.rend() ; ++campIt )
                if ( ( *campIt )->isConfigOwner )
                    (*campIt)->config->NodeTable().dcs.DCSLock= nullptr;
    #endif

    // delete resources/config
    if( targetPhase == ShutdownPhases::Destruct)
        for(auto campIt= CAMPS.rbegin() ; campIt != CAMPS.rend() ; ++campIt )
        {
            if ( ( *campIt )->isResourceOwner )  lang::Destruct( *(*campIt)->resourcePool );
          #if ALIB_CONFIGURATION
            if ( ( *campIt )->isConfigOwner )
            {
                auto& cfgAllocator= (*campIt)->config->GetAllocator();

                #if ALIB_DEBUG_CRITICAL_SECTIONS
                (*campIt)->config->NodeTable().dcs.DCSLock= nullptr;
                #endif
                lang::Destruct( *(*campIt)->config );
IF_ALIB_THREADS(lang::Destruct( *(*campIt)->configLock ); )
                lang::Destruct( cfgAllocator );
            }
          #endif
            if(( *campIt ) == targetCamp )
                break;
        }

    if(     targetPhase == ShutdownPhases::Destruct
        &&  targetCamp  == CAMPS.Front()              )
    {
        IF_ALIB_THREADS(    threads    ::Shutdown(); )
        IF_ALIB_TIME(       time       ::Shutdown(); )
        IF_ALIB_ENUMS(      enums      ::Shutdown(); )
        IF_ALIB_BOXING(     boxing     ::Shutdown(); )
        IF_ALIB_SINGLETONS( singletons ::Shutdown(); )
        CAMPS.Reset();
    }

}
#include "alib/lang/callerinfo_methods.hpp"
    
} // namespace [alib]
