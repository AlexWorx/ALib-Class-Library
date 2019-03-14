// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_LIB_MODULE)
#   include "alib/lib/module.hpp"
#endif


#if ALIB_MODULE_CONFIGURATION
    #if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #   include "alib/config/configuration.hpp"
    #endif
#endif

#if !defined (HPP_ALIB_ENUMS_ENUM_ITERATORS)
#   include "alib/enums/enumiterators.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#    include "alib/resources/resources.hpp"
#endif

#if  ALIB_MODULE_TIME
#   if !defined (HPP_ALIB_TIME_TIME)
#      include "alib/time/time.hpp"
#   endif
#endif

#if ALIB_MODULE_THREADS && !defined (HPP_ALIB_THREADS_THREAD)
#    include "alib/threads/thread.hpp"
#endif



#if !defined (_GLIBCXX_IOSTREAM) && !defined (_IOSTREAM_ )
#   include <iostream>
#endif
#if !defined (_GLIBCXX_IOMANIP) && !defined (_IOMANIP_ )
#   include <iomanip>
#endif



namespace aworx { namespace lib {

bool Module::microModulesInitialized= false;

bool Module::Init( InitLevels level, int argc, const char** argvN, const wchar_t** argvW )
{
    // Initialize "micro" modules once
    if( !Module::microModulesInitialized )
    {
        #if ALIB_MODULE_THREADS
            threads::Init();
        #endif

        #if ALIB_MODULE_BOXING
            boxing::Init();
        #endif

        #if ALIB_MODULE_TIME
            time::Init();
        #endif

        Module::microModulesInitialized= true;
    }



    // convert external level to internal phases
    int maxPhase= EnumValue( level );

    // args given but not first phase?
    ALIB_ASSERT_WARNING( argc == 0 || initState == 0,
                         "Command line args ignored. "
                         "Accepted only with initialization level 'PrepareResources'."  )

    if( initState >= maxPhase  )
        return false;

    // loop over all phases
    for ( int actualPhase= initState + 1 ; actualPhase <= maxPhase ; actualPhase++ )
    {
        InitLevels actualLevel= InitLevels(actualPhase);

        // create a resources object
        if( actualLevel == InitLevels::PrepareResources && Resources == nullptr    )
        {
            isResourceAndConfigOwner|= 1;
            Resources= new class resources::Resources();
        }

        // initialize libs recursive.
        doInitRecursion( actualLevel, argc, argvN, argvW );

        // At the end of phase PrepareConfig, we create our configuration object. This will
        // only be propagated in the next phase. This is needed to allow plugins to be attached
        // by the caller.
        #if ALIB_MODULE_CONFIGURATION
        if( actualLevel == InitLevels::PrepareConfig    && Config == nullptr )
        {
            isResourceAndConfigOwner|= 2;
            Config= new Configuration();
            if( argc > 0 )
            {
                if( argvN != nullptr )
                    Config->SetCommandLineArgs( argc, argvN );
                else
                    Config->SetCommandLineArgs( argc, argvW );
            }
        }
        #else
            (void) argc;
            (void) argvN;
            (void) argvW;
        #endif

    }


    return true;
}

void Module::doInitRecursion( InitLevels level, int argc, const char** argv, const wchar_t** wargv )
{
     ALIB_ASSERT_ERROR( initState >= 0, "Trying to initialize a terminated module"    )

    // convert external level to internal phases
    int phase= EnumValue( level );

    // check and set ourselves initialized
    // (don't remove, even with check below, needed to prevent visits in circular dependencies )
    if( phase <= initState )
         return;
    initState= phase;

    // loop over all dependent libraries
    for( auto* dependentModule : Dependencies )
    {
        // already initialized?
        if( phase < dependentModule->initState )
            continue;

        // propagate resources and config, if sub-module does not provide on own instance
             if( level == InitLevels::PrepareResources && dependentModule->initState == 0 )
        {
            ALIB_ASSERT_ERROR( dependentModule->Resources == nullptr,
                "Lib not initialized on phase 'PrepareResources', but resource object already set" )
            dependentModule->Resources= Resources;
        }
        else if( level == InitLevels::Final && dependentModule->initState == 2 )
        {
            #if ALIB_MODULE_CONFIGURATION
                ALIB_ASSERT_ERROR( dependentModule->Config == nullptr,
                    "Lib not initialized on phase 'final', but config object set" )
                dependentModule->Config   = Config;
            #endif
        }

        // recursively initialize dependent module
        dependentModule->doInitRecursion( level, argc, argv, wargv );
    }

    // now initialize ourselves
    init(level, argc, argv, wargv);
}

void Module::TerminationCleanUp()
{
     ALIB_ASSERT_ERROR( initState == 3 || initState == -1,
                        "Trying to terminate a not (fully) initialized module"  )

    // check and set ourselves initialized, to prevent visits in circular dependencies.
    if( initState < 0 )
         return;
    initState= -1;

    // now terminate ourselves
    terminationCleanUp();

    // loop over all dependent libraries
    for( auto it= Dependencies.rbegin(); it != Dependencies.rend(); ++it )
    {
        // already terminated?
        if( (*it)->initState < 0 )
            continue;

        // recursively terminate
        (*it)->terminationCleanUp();
    }

    #if ALIB_MODULE_CONFIGURATION
        if( isResourceAndConfigOwner & 1 )   delete Resources;
        if( isResourceAndConfigOwner & 2 )   delete Config;
    #endif

    #if ALIB_MODULE_THREADS
        threads::TerminationCleanUp();
    #endif

    #if ALIB_MODULE_TIME
        time::TerminationCleanUp();
    #endif

    // remove singletons
    #if ALIB_MODULE_SINGLETONS
        singletons::DeleteSingletons();
    #endif
}


void Module::AddResource( const NString& name, const String& data )
{
    Resources->Add( ResourceCategory, name, data   ALIB_DBG( , true ) );
}

void   Module::ReplaceResource( const NString& name, const String& data
                                ALIB_DBG(,const  String& expectedResource)  )
{
    #if ALIB_DEBUG
    if( expectedResource.IsNotNull() &&  !expectedResource.Equals( GetResource(name) ) )
    {
        // we do not have reports here yet, must use simple debug message interface.
        nchar  bufStack[1024];
        nchar* buf= bufStack;
        characters::CharArray<nchar>::Copy( ResourceCategory.Buffer(), ResourceCategory.Length(), buf );
        integer idx=  ResourceCategory.Length();
        buf[idx++]= '/';
        integer len= (std::min)( static_cast<integer>(1023 - idx), name.Length() );
        characters::CharArray<nchar>::Copy(  name.Buffer(), len, buf + idx);
        idx+= len;
        buf[idx]= '\0';
        ALIB_ERROR( "Assertion failure: Expected original value not found when replacing resource \"",
                    bufStack, "\"." )
    }
    #endif

    Resources->Add( ResourceCategory, name, data  ALIB_DBG(, false)  );
}

String Module::TryResource( const NString& name )
{
    return Resources->Get( ResourceCategory, name  ALIB_DBG(, false)  );
}

String Module::GetResource( const NString& name )
{
    return Resources->Get( ResourceCategory, name   ALIB_DBG(, true)  );
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
        cnt++;
        std::cout << std::setw(30) <<  p.first << '|' << std::setw(5) << (CompilationFlags & p.second  ? " On" : " Off")
                                               << "|" << std::setw(5) << (flags            & p.second  ? " On" : " Off")
                  << std::endl;
    }

    return false;
}

}} // namespace [aworx::lib]
