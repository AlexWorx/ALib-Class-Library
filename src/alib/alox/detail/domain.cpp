// #################################################################################################
//  aworx::lib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_ALOX)
#      include "alib/alox/alox.hpp"
#   endif

#   define HPP_ALIB_LOX_PROPPERINCLUDE
#      if !defined (HPP_ALOX_DETAIL_DOMAIN)
#          include "alib/alox/detail/domain.inl"
#      endif
#   undef HPP_ALIB_LOX_PROPPERINCLUDE

#   if !defined (HPP_ALIB_ALOXMODULE)
#      include "alib/alox/aloxmodule.hpp"
#   endif

#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#       include "alib/strings/format.hpp"
#   endif
#endif // !defined(ALIB_DOX)


namespace aworx { namespace lib { namespace lox { namespace detail {

// #################################################################################################
// static fields
// #################################################################################################

// #################################################################################################
// Constructor/Destructor
// #################################################################################################

Domain::Domain( MonoAllocator* allocator, const NString& name )
: SubDomains    ( allocator )
, Data          (*allocator )
, PrefixLogables( allocator )
{
    // store parameters
    Parent=   nullptr;
    Name  =   allocator->EmplaceString( name );

    Data      .reserve( static_cast<size_t>( 2 ) );

    // The full of the root domain equals the name
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    nchar* fullPath= allocator->AllocArray<nchar>( name.Length() + 1 );
    name.CopyTo( fullPath );
    fullPath[name.Length()]= '/';
    FullPath= NString( fullPath, name.Length() + 1 );
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
}


Domain::Domain( Domain* parent, const NString& name )
: SubDomains    ( &parent->Data.get_allocator().allocator )
, Data          (  parent->Data.get_allocator().allocator )
, PrefixLogables( &parent->Data.get_allocator().allocator )
{
    // store parameters
    Parent=   parent;
    Name=     Data.get_allocator().allocator.EmplaceString( name );

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
    FullPath= Data.get_allocator().allocator.EmplaceString( fullPath );
}

Domain::~Domain()
{
    for ( auto& it : PrefixLogables )
        delete  it.first;
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

    ALIB_ASSERT_ERROR( endSubName != 0, "ALOX","Internal Error" )

    // find end of actual domain name and save rest
    NSubstring restOfDomainPath= nullptr;
    if ( endSubName > 0 )
        domainPath.Split<false>( endSubName, restOfDomainPath, 1 );

    // search sub-domain
    Domain* subDomain= nullptr;

    // "."
    if( domainPath.Equals( "." ) )
        subDomain= this;

    // ".."
    else if( domainPath.Equals( ".." ) )
        subDomain= Parent != nullptr ? Parent : this;

    // search in sub-domain
    else
    {
        List<Domain>::Iterator subDomainIt;
        bool fixedOnce= false;
        for(;;)
        {
            subDomainIt=  SubDomains.begin();
            while ( subDomainIt != SubDomains.end() )
            {
                int comparison= (*subDomainIt).Name.CompareTo<false, Case::Sensitive>( domainPath );

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

Verbosity Domain::SetVerbosity( int loggerNo, Verbosity verbosity, Priorities priority )
{
    LoggerData& ld= Data[static_cast<size_t>(loggerNo)];
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


}}}}// namespace [aworx::lib::lox::detail]


