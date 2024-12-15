// #################################################################################################
//  alib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/alox/alox.hpp"
#   define HPP_ALIB_LOX_PROPPERINCLUDE
#       include "alib/alox/detail/domain.inl"
#   undef HPP_ALIB_LOX_PROPPERINCLUDE
#   include "alib/alox/aloxcamp.hpp"
#endif // !DOXYGEN


namespace alib {  namespace lox { namespace detail {

// #################################################################################################
// static fields
// #################################################################################################

// #################################################################################################
// Constructor/Destructor
// #################################################################################################

Domain::Domain( MonoAllocator& allocator, PoolAllocator& pool, const NString& name )
: Name          (allocator, name)
, Parent        (nullptr)
, SubDomains    (allocator)
, Data          (allocator)
, PrefixLogables(pool)
{
    Data      .reserve( size_t( 2 ) );

    // The full of the root domain equals the name
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    nchar* fullPath= allocator().AllocArray<nchar>( name.Length() + 1 );
    name.CopyTo( fullPath );
    fullPath[name.Length()]= '/';
    FullPath= NString( fullPath, name.Length() + 1 );
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
}


Domain::Domain( Domain* parent, const NString& name )
: Name( parent->Data.get_allocator().GetAllocator(), name )
, Parent        ( parent )
, SubDomains    ( parent->Data.get_allocator().GetAllocator() )
, Data          ( parent->Data.get_allocator().GetAllocator() )
, PrefixLogables( parent->PrefixLogables.GetAllocator() )
{
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

// #################################################################################################
// Methods
// #################################################################################################
Domain* Domain::Find( NSubstring domainPath, int maxCreate, bool* wasCreated )
{
    // set optional output parameter as default to false
    bool dummy;
    if ( wasCreated == nullptr )
        wasCreated= &dummy;
    *wasCreated= false;

    integer lenBeforeTrim= domainPath.Length();

    // if string is empty (resp. contains only separator characters), return ourselves
    while ( domainPath.ConsumeChar( Domain::Separator() ) )
        ;
    if( domainPath.IsEmpty() )
    {
        return this;
    }

    // Trailing domain separator found: call find on root domain
    Domain* startDomain= this;
    if ( lenBeforeTrim > domainPath.Length() )
    {
        while (startDomain->Parent != nullptr )
            startDomain= startDomain->Parent;
    }

    // call find
    return startDomain->findRecursive( domainPath, maxCreate, wasCreated );
}

Domain* Domain::findRecursive( NSubstring& domainPath, int maxCreate, bool* wasCreated )
{
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
    else
    {
        decltype(SubDomains)::Iterator subDomainIt;
        bool fixedOnce= false;
        for(;;)
        {
            subDomainIt=  SubDomains.begin();
            while ( subDomainIt != SubDomains.end() )
            {
                int comparison= (*subDomainIt).Name.CompareTo<NC, lang::Case::Sensitive>( domainPath );

                if( comparison >= 0 )
                {
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
            if( !fixedOnce )
            {
                fixedOnce= true;

                bool illegalCharacterFound= false;
                for( int i= 0; i< domainPath.Length() ; ++i )
                {
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
                    }
                }

                if ( illegalCharacterFound )
                    continue;
             }

            // create
            if ( maxCreate == 0 )
                return nullptr;

            *wasCreated= true;
            subDomainIt= SubDomains.Emplace( subDomainIt, this,  domainPath );
            --maxCreate;
            if ( maxCreate == 0 )
                return &(*subDomainIt);
            break;
        }
    }

    // recursion?
    return  restOfDomainPath.IsNotEmpty()
            ? subDomain->findRecursive( restOfDomainPath, maxCreate, wasCreated )
            : subDomain;
}

Verbosity Domain::SetVerbosity( int loggerNo, Verbosity verbosity, Priority priority )
{
    LoggerData& ld= Data[size_t(loggerNo)];
    if( priority >= ld.Priority )
    {
        ld.Priority=        priority;
        ld.LoggerVerbosity= verbosity;

        for( Domain& subDomain : SubDomains )
            subDomain.SetVerbosity( loggerNo, verbosity, priority );
    }
    return ld.LoggerVerbosity;
}

void  Domain::addLoggerRecursive( Logger* logger)
{
    Data.emplace_back( LoggerData( logger ) );
    for( Domain& subDomain : SubDomains )
        subDomain.addLoggerRecursive( logger );
}

void  Domain::removeLoggerRecursive( int loggerNo )
{
    Data.erase( Data.begin() + loggerNo );
    for( Domain& subDomain : SubDomains )
        subDomain.removeLoggerRecursive( loggerNo );
}

void Domain::ToString( NAString& tAString )
{
    tAString << FullPath;
    tAString._('[')._( NFormat( CntLogCalls,3 ) )._("] ");

    // get verbosities
    tAString._(" { ");
        for( size_t i= 0; i < Data.size() ; ++i )
        {
            LoggerData& ld= Data[i];
            tAString._(i!=0 ? ", " : "" )
                    ._('(')
                        ._('[')._( NFormat(ld.LogCallsPerDomain, 3) )._( "], " )
                        ._( std::make_pair( ld.LoggerVerbosity, ld.Priority) )
                    ._( ')' );
        }
    tAString._(" }");
}


}}}// namespace [alib::lox::detail]

