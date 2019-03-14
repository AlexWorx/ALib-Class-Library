// #################################################################################################
//  aworx::lib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_ALOX)
#   include "alib/alox/alox.hpp"
#endif

using namespace aworx;
using namespace aworx::lib::lox::detail;

// #################################################################################################
// static fields
// #################################################################################################

// #################################################################################################
// Constructor/Destructor
// #################################################################################################

Domain::Domain( Domain* parent, const NString& name )
: SubDomains()
, Data()
{
    Name.DbgDisableBufferReplacementWarning();

    // store parameters
    this->Parent=   parent;
    Name=           name;

    SubDomains.reserve(3);
    Data      .reserve( parent == nullptr ? static_cast<size_t>( 2 ) :  parent->Data.size() );

    // if we have a parent, we inherit all logger's verbosities
    if( parent != nullptr )
    {
        Data= parent->Data;
    }

    // assemble the full path once
    const Domain* dom= this;
    do
    {
        if ( dom != this || dom->Parent == nullptr )
            FullPath.InsertAt( "/"      , 0 );
        FullPath.InsertAt( dom->Name, 0 );
        dom= dom->Parent;
    }
    while( dom != nullptr );
}

Domain::~Domain()
{
    for ( Domain* sub : SubDomains )
        delete sub;
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

    ALIB_ASSERT_ERROR( endSubName != 0, "Internal Error" )

    // find end of actual domain name and save rest
    NSubstring restOfDomainPath;
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
        std::vector<Domain*>::iterator subDomainIt;
        bool fixedOnce= false;
        for(;;)
        {
            subDomainIt=  SubDomains.begin();
            while ( subDomainIt != SubDomains.end() )
            {
                int comparison= (*subDomainIt)->Name.CompareTo<false, Case::Sensitive>( domainPath );

                if( comparison >= 0 )
                {
                    if ( comparison == 0 )
                        subDomain=    *subDomainIt;
                    break;
                }
                subDomainIt++;
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
            subDomainIt= SubDomains.insert( subDomainIt, subDomain= new Domain( this,  domainPath) );
            maxCreate--;
            if ( maxCreate == 0 )
                return *subDomainIt;
            break;
        }
    }

    // recursion?
    return  restOfDomainPath.IsNotEmpty()
            ? subDomain->findRecursive( restOfDomainPath, maxCreate, wasCreated )
            : subDomain;
}

void Domain::ToString( NAString& tAString )
{
    tAString << FullPath;
    tAString._('[')._( NFormat( CntLogCalls,3 ) )._("] ");

    // get verbosities
    tAString._(" { ");
        for( size_t i= 0; i < Data.size() ; i++ )
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




