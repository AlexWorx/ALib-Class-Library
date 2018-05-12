// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if ALIB_MODULE_CONFIGURATION
    #if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
        #include "alib/config/configuration.hpp"
    #endif
#endif

#if !defined (HPP_ALIB_LANG_RESOURCES)
    #include "alib/lang/resources.hpp"
#endif

#if !defined (HPP_ALIB_LANG_ENUM_ITERATORS)
    #include "alib/lang/enumiterators.hpp"
#endif


#if !defined (_GLIBCXX_IOSTREAM) && !defined (_IOSTREAM_ )
    #include <iostream>
#endif
#if !defined (_GLIBCXX_IOMANIP) && !defined (_IOMANIP_ )
    #include <iomanip>
#endif



ALIB_LANG_ENUM_IS_BITWISE( aworx::lib::lang::Library::Phases )
ALIB_LANG_ENUM_IS_ITERATABLE_BEGIN_END( aworx::lib::lang::Library::Phases,
                                        aworx::lib::lang::Library::Phases::resourceset  ,
                                        aworx::lib::lang::Library::Phases::END_OF_ITERATION )



namespace aworx { namespace lib {  namespace lang {


bool Library::Init( InitLevels level )
{
    Phases phases= Phases( EnumValue( level ) );
    if ( EnumContains( initState, phases) )
        return false;

    return doInit( phases, 0, nullptr, nullptr );
}

bool Library::Init( int argc, char    **argv, InitLevels level )
{
    Phases phases= Phases( EnumValue( level ) );
    if ( EnumContains( initState, phases ) )
    {
        ALIB_WARNING( ASTR("Command line args ignored, library is alredy initialized") );
        return false;
    }
    ALIB_ASSERT_WARNING( EnumContains(phases, Phases::resourceset),
                         ASTR("Command line args ignored, wrong initialization level")  );
    return doInit( phases, argc, argv, nullptr );
}

bool Library::Init( int argc, wchar_t **argv, InitLevels level )
{
    Phases phases= Phases( EnumValue( level ) );
    if ( EnumContains( initState, phases ) )
    {
        ALIB_WARNING( ASTR("Command line args ignored, library is alredy initialized") );
        return false;
    }

    ALIB_ASSERT_WARNING( EnumContains(phases, Phases::resourceset),
                         ASTR("Command line args ignored, wrong initialization level")  );
    return doInit( phases, argc, nullptr, argv );
}

bool Library::doInit( Phases       phases,
                      int          argc,
                      char**       argv,
                      wchar_t**    wargv   )
{
    if ( EnumContains( initState, phases ) )
        return false;

    // loop over all phases
    for ( auto phase : EnumIterator<Phases>() )
    {
        // phase not requested or already initialized on that phase?
        if(   !EnumContains( phases   , phase )
            || EnumContains( initState, phase ))
            continue;

        // create a resources object
        if( phase == Phases::resourceset        && Res == nullptr)
        {
            //ALIB_ASSERT_ERROR( EnumContains( phases, Phases::resourceset),
            //                   "No resources set while initializing library without Phases::resourceset." )
            isResourceAndConfigOwner|= 1;
            Res= new class Resources();
        }

        // initialize libs recursive.
        doInitRecursion( phase );

        // At the end of phase resourceload, we create our configuration object. This will
        // only be propagated in the next phase. This is needed to allow plugins to be attached
        // by the caller.
        #if ALIB_MODULE_CONFIGURATION
        if( phase == Phases::resourceload && Config == nullptr)
        {
            isResourceAndConfigOwner|= 2;
            Config= new Configuration();
            if( argc != 0 )
            {
                if( argv != nullptr )
                    Config->SetCommandLineArgs( argc, argv );
                else
                    Config->SetCommandLineArgs( argc, wargv );
            }
        }
        #else
            (void) argc;
            (void) argv;
            (void) wargv;
        #endif

    }

    return true;
}

void Library::doInitRecursion( Phases phase )
{
     ALIB_ASSERT_ERROR( !EnumContains( initState, Phases::TERMINATED),
                        ASTR("Trying to initialize a terminated library")    );

    // check and set ourselves initialized, to prevent visits in circular dependencies.
     if( EnumContains( initState, phase) )
         return;
    initState+= phase;

    // loop over all dependent libraries
    for( auto* lib : dependencies )
    {
        // already initialized?
        if( EnumContains( lib->initState, phase) )
            continue;

        // propagate resources and config, if sub-library does not provide on own instance
             if( phase == Phases::resourceset    )
        {
            ALIB_ASSERT_ERROR( lib->Res == nullptr,
                               ASTR("Lib not initialized on phase 'static', but resource object set") )
            lib->Res= Res;
        }
        else if( phase == Phases::final )
        {
            #if ALIB_MODULE_CONFIGURATION
                ALIB_ASSERT_ERROR( lib->Config == nullptr,
                                   ASTR("Lib not initialized on phase 'variables', but config object set") )
                lib->Config   = Config;
            #endif
        }

        // recursively initialize parents
        lib->doInitRecursion( phase );
    }

    // now initialize ourselves
    init(phase);
}

void Library::TerminationCleanUp()
{
     ALIB_ASSERT_ERROR( initState == Phases::ALL || initState == Phases::TERMINATED ,
                        ASTR("Trying to terminate a not (fully) initialized library")  );

    // check and set ourselves initialized, to prevent visits in circular dependencies.
     if( EnumContains( initState, Phases::TERMINATED) )
         return;
    initState= Phases::TERMINATED;

    // loop over all dependent libraries
    for( auto* lib : dependencies )
    {
        // already initialized?
        if( EnumContains( lib->initState, Phases::TERMINATED) )
            continue;

        // recursively terminate parents
        lib->TerminationCleanUp();
    }

    // now terminate ourselves
    terminationCleanUp();

    #if ALIB_MODULE_CONFIGURATION
        if( isResourceAndConfigOwner & 1 )   delete Res;
        if( isResourceAndConfigOwner & 2 )   delete Config;
    #endif
}


bool Library::VerifyCompilationFlags( uint64_t flags )
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




}}} // namespace [aworx::lib::lang]
