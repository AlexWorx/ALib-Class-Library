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
#include "alib/alox/alox.prepro.hpp"

//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.ALox.Impl;
    import   ALib.Lang;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Variables;
    import   ALib.Camp;
    import   ALib.Camp.Base;
#else
#   include "ALib.ALox.H"
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Variables.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.ALox.Impl.H"
#endif
//========================================== Implementation ========================================
namespace alib {  namespace lox { namespace detail {

//##################################################################################################
// static fields
//##################################################################################################

//##################################################################################################
// Constructor/Destructor
//##################################################################################################

Domain::Domain( MonoAllocator& allocator, PoolAllocator& pool, const NString& name )
: Name          (allocator, name)
, Parent        (nullptr)
, SubDomains    (allocator)
, Data          (allocator)
, PrefixLogables(pool) {
    Data      .reserve( size_t( 2 ) );

    // The full of the root domain equals the name
    nchar* fullPath= allocator().AllocArray<nchar>( name.Length() + 1 );
    name.CopyTo( fullPath );
    fullPath[name.Length()]= '/';
    FullPath= NString( fullPath, name.Length() + 1 );
}


Domain::Domain( Domain* parent, const NString& name )
: Name( parent->Data.get_allocator().GetAllocator(), name )
, Parent        ( parent )
, SubDomains    ( parent->Data.get_allocator().GetAllocator() )
, Data          ( parent->Data.get_allocator().GetAllocator() )
, PrefixLogables( parent->PrefixLogables.GetAllocator() ) {
    // if we have a parent, we inherit all logger's verbosities
    if( parent != nullptr )
        Data= parent->Data;

    // assemble the full path once
    const Domain* dom= this;
    NString1K fullPath;
    do
    {
        if ( dom != this || dom->Parent == nullptr )
            fullPath.InsertAt( "/"      , 0 );
        fullPath.InsertAt( dom->Name, 0 );
        dom= dom->Parent;
    }
    while( dom != nullptr );
    FullPath.Allocate( Data.get_allocator().GetAllocator(), fullPath );
}

//##################################################################################################
// Methods
//##################################################################################################
Domain* Domain::Find( NSubstring domainPath, int maxCreate, bool* wasCreated ) {
    // set optional output parameter as default to false
    bool dummy;
    if ( wasCreated == nullptr )
        wasCreated= &dummy;
    *wasCreated= false;

    integer lenBeforeTrim= domainPath.Length();

    // if string is empty (resp. contains only separator characters), return ourselves
    while ( domainPath.ConsumeChar( Domain::Separator() ) )
        ;
    if( domainPath.IsEmpty() ) {
        return this;
    }

    // Trailing domain separator found: call find on root domain
    Domain* startDomain= this;
    if ( lenBeforeTrim > domainPath.Length() ) {
        while (startDomain->Parent != nullptr )
            startDomain= startDomain->Parent;
    }

    // call find
    return startDomain->findRecursive( domainPath, maxCreate, wasCreated );
}

Domain* Domain::findRecursive( NSubstring& domainPath, int maxCreate, bool* wasCreated ) {
    //--- get act sub-name and rest of path
    domainPath.ConsumeChar( Domain::Separator() );
    integer endSubName= domainPath.IndexOf( Domain::Separator() );

    ALIB_ASSERT_ERROR( endSubName != 0, "ALOX", "Internal error. This must never happen." )

    // find end of actual domain name and save rest
    NSubstring restOfDomainPath= nullptr;
    if ( endSubName > 0 )
        domainPath.Split<NC>( endSubName, restOfDomainPath, 1 );

    // search subdomain
    Domain* subDomain= nullptr;

    // "."
    if( domainPath.Equals<NC>( "." ) )
        subDomain= this;

    // ".."
    else if( domainPath.Equals<NC>( ".." ) )
        subDomain= Parent != nullptr ? Parent : this;

    // search in subdomain
    else {
        decltype(SubDomains)::iterator subDomainIt;
        bool fixedOnce= false;
        for(;;) {
            subDomainIt=  SubDomains.begin();
            while ( subDomainIt != SubDomains.end() ) {
                int comparison= (*subDomainIt).Name.CompareTo<NC, lang::Case::Sensitive>( domainPath );

                if( comparison >= 0 ) {
                    if ( comparison == 0 )
                        subDomain=    &(*subDomainIt);
                    break;
                }
                ++subDomainIt;
            }

            // domain found?
            if ( subDomain != nullptr )
                break;

            // try and fix name
            if( !fixedOnce ) {
                fixedOnce= true;

                bool illegalCharacterFound= false;
                for( int i= 0; i< domainPath.Length() ; ++i ) {
                    nchar c= domainPath[i];
                    if (!(    isdigit( c )
                           || ( c >= 'A' && c <= 'Z' )
                           || c == '-'
                           || c == '_'
                    ))
                    {
                        illegalCharacterFound= true;
                        // oh dear: modifying const buffer...but this is definitely from an AString!
                        *const_cast<nchar*>( domainPath.Buffer() + i)= '#';
                }   }

                if ( illegalCharacterFound )
                    continue;
             }

            // create
            if ( maxCreate == 0 )
                return nullptr;

            *wasCreated= true;
            subDomainIt= SubDomains.emplace( subDomainIt, this,  domainPath );
            --maxCreate;
            if ( maxCreate == 0 )
                return &(*subDomainIt);
            break;
    }   }

    // recursion?
    return  restOfDomainPath.IsNotEmpty()
            ? subDomain->findRecursive( restOfDomainPath, maxCreate, wasCreated )
            : subDomain;
}

Verbosity Domain::SetVerbosity( int loggerNo, Verbosity verbosity, Priority priority ) {
    LoggerData& ld= Data[size_t(loggerNo)];
    if( priority >= ld.Priority ) {
        ld.Priority=        priority;
        ld.LoggerVerbosity= verbosity;

        for( Domain& subDomain : SubDomains )
            subDomain.SetVerbosity( loggerNo, verbosity, priority );
    }
    return ld.LoggerVerbosity;
}

void  Domain::addLoggerRecursive( Logger* logger) {
    Data.emplace_back( LoggerData( logger ) );
    for( Domain& subDomain : SubDomains )
        subDomain.addLoggerRecursive( logger );
}

void  Domain::removeLoggerRecursive( int loggerNo ) {
    Data.erase( Data.begin() + loggerNo );
    for( Domain& subDomain : SubDomains )
        subDomain.removeLoggerRecursive( loggerNo );
}

void Domain::ToString( NAString& tAString ) {
    tAString << FullPath;
    tAString._('[')._( NDec( CntLogCalls,3 ) )._("] ");

    // get verbosities
    tAString._(" { ");
        for( size_t i= 0; i < Data.size() ; ++i ) {
            LoggerData& ld= Data[i];
            tAString._(i!=0 ? ", " : "" )
                    ._('(')
                        ._('[')._( NDec(ld.LogCallsPerDomain, 3) )._( "], " )
                        ._( boxing::MakePair( ld.LoggerVerbosity, ld.Priority) )
                    ._( ')' );
        }
    tAString._(" }");
}


}}}// namespace [alib::lox::detail]
