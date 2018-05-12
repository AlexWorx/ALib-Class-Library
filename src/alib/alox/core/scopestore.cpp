// #################################################################################################
//  aworx::lib::lox::core - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"
#include "alib/alox.hpp"

#if !defined(_GLIBCXX_IOSTREAM) && !defined(_IOSTREAM_)
    #include <iostream>
#endif

#if !defined (_GLIBCXX_CSTRING) && !defined(_CSTRING_)
    #include <cstring>
#endif

using namespace aworx;

namespace aworx { namespace lib { namespace lox { namespace core {

// #################################################################################################
// template instantiations
// #################################################################################################

//! @cond NO_DOX

// AString*
template                                ScopeStore<NAString*>                   ::ScopeStore (ScopeInfo&, bool);
template                                ScopeStore<NAString*>                   ::~ScopeStore();
template   void                         ScopeStore<NAString*>                   ::Clear      ();
template   void                         ScopeStore<NAString*>                   ::InitWalk   (Scope,NAString*);
template   NAString*                    ScopeStore<NAString*>                   ::Walk       ();
template   void                         ScopeStore<NAString*>                   ::InitAccess (Scope,int,int);
template   void                         ScopeStore<NAString*>                   ::initCursor (bool);
template   NAString*                    ScopeStore<NAString*>                   ::access     (int,NAString*);

// Logable*
template                                ScopeStore<Box*>                        ::ScopeStore (ScopeInfo&, bool);
template                                ScopeStore<Box*>                        ::~ScopeStore();
template   void                         ScopeStore<Box*>                        ::Clear      ();
template   void                         ScopeStore<Box*>                        ::InitWalk   (Scope,Box*);
template   Box*                         ScopeStore<Box*>                        ::Walk       ();
template   void                         ScopeStore<Box*>                        ::InitAccess (Scope,int,int);
template   void                         ScopeStore<Box*>                        ::initCursor (bool);
template   Box*                         ScopeStore<Box*>                        ::access     (int,Box*);

// std::map<AString, int>*
template                                ScopeStore<std::map<AString, int>*>     ::ScopeStore (ScopeInfo&, bool);
template                                ScopeStore<std::map<AString, int>*>     ::~ScopeStore();
template   void                         ScopeStore<std::map<AString, int>*>     ::Clear      ();
template   void                         ScopeStore<std::map<AString, int>*>     ::InitWalk   (Scope,std::map<AString, int>*);
template   std::map<AString, int>*      ScopeStore<std::map<AString, int>*>     ::Walk       ();
template   void                         ScopeStore<std::map<AString, int>*>     ::InitAccess (Scope,int,int);
template   void                         ScopeStore<std::map<AString, int>*>     ::initCursor (bool);
template   std::map<AString, int>*      ScopeStore<std::map<AString, int>*>     ::access     (int,std::map<AString, int>*);

// std::map<AString, Box>*
template                                ScopeStore<std::map<AString, Box>*>     ::ScopeStore (ScopeInfo&, bool);
template                                ScopeStore<std::map<AString, Box>*>     ::~ScopeStore();
template   void                         ScopeStore<std::map<AString, Box>*>     ::Clear      ();
template   void                         ScopeStore<std::map<AString, Box>*>     ::InitWalk   (Scope,std::map<AString, Box>*);
template   std::map<AString, Box>*      ScopeStore<std::map<AString, Box>*>     ::Walk       ();
template   void                         ScopeStore<std::map<AString, Box>*>     ::InitAccess (Scope,int,int);
template   void                         ScopeStore<std::map<AString, Box>*>     ::initCursor (bool);
template   std::map<AString, Box>*      ScopeStore<std::map<AString, Box>*>     ::access     (int,std::map<AString, Box>*);

#define CMD_INSERT 0
#define CMD_REMOVE 1
#define CMD_GET    2


// #################################################################################################
// Constructor/Destructor
// #################################################################################################

template<typename StoreT>
ScopeStore<StoreT>::ScopeStore(ScopeInfo& pScopeInfo,  bool pCfgSingleThreadValue )
  : globalStore ( ScopeStoreType<StoreT>::NullValue() )
  , languageStore( nullptr )
  , scopeInfo   ( pScopeInfo   )
  , cfgSingleThreadValue( pCfgSingleThreadValue )
{
    languageStore= new LanguageStore();
}

template<typename StoreT>
ScopeStore<StoreT>::~ScopeStore()
{
    delete languageStore;
}


// #################################################################################################
// Methods
// #################################################################################################

template<typename StoreT>
void ScopeStore<StoreT>::Clear()
{
    globalStore= ScopeStoreType<StoreT>::NullValue();
    threadInnerStore.clear();
    languageStore->Clear();
    threadOuterStore.clear();
}


template<typename StoreT>
void ScopeStore<StoreT>::InitWalk( Scope startScope, StoreT localObject  )
{
    actScope=            startScope;
    walkLocalObject=     localObject;
    actPathLevel=        0;
    walkNextThreadIdx=   -2;
    lazyLanguageNode=    true;
    walking=             true;
}

template<typename StoreT>
StoreT ScopeStore<StoreT>::Walk()
{
    while ( walking )  switch( actScope )
    {
        case Scope::ThreadInner:
        {
            // initialize
            if ( walkNextThreadIdx == -2 )
            {
                walkNextThreadIdx= -1;
                if ( threadInnerStore.size() != 0 )
                {
                    auto it= threadInnerStore.find( scopeInfo.GetThreadID() );
                    if ( it != threadInnerStore.end() )
                    {
                        walkThreadValues=    &it->second;
                        walkNextThreadIdx=   static_cast<int>( walkThreadValues->size() );
                    }
                }
            }

            // return next inner thread object (traversalNextScope keeps being ThreadInner)
            if ( walkNextThreadIdx > 0 )
            {
                walkNextThreadIdx--;
                return (*walkThreadValues)[static_cast<size_t>(walkNextThreadIdx)];
            }

            // next scope is Method
            actScope= Scope::Method;

            // if we have a valid 'local object' return this first
            if ( walkLocalObject != ScopeStoreType<StoreT>::NullValue() )
                return walkLocalObject;
        }
        break;

        case Scope::Method:
        case Scope::Filename:
        case Scope::Path:
        {
            if( lazyLanguageNode )
                initCursor( false );

            while( actStringTreeNode.IsValid() )
            {
                StoreT actValue= *actStringTreeNode;
                actStringTreeNode.MoveToParentUnchecked();
                if( actValue != ScopeStoreType<StoreT>::NullValue() )
                    return actValue;
            }

            actScope=  Scope::ThreadOuter;
            walkNextThreadIdx= -2;
        }
        break;

        case Scope::ThreadOuter:
        {
            // initialize
            if ( walkNextThreadIdx == -2 )
            {
                if ( threadOuterStore.size() != 0 )
                {
                    auto it= threadOuterStore.find( scopeInfo.GetThreadID() );
                    if ( it != threadOuterStore.end() )
                    {
                        walkThreadValues=    &it->second;
                        walkNextThreadIdx=   static_cast<int>( walkThreadValues->size() );
                    }
                }
            }

            // return next outer thread object (walkNext keeps being THREAD_OUTER)
            if ( walkNextThreadIdx > 0 )
            {
                walkNextThreadIdx--;
                return (*walkThreadValues)[static_cast<size_t>(walkNextThreadIdx)];
            }

            // next scope is Global
            actScope= Scope::Global;
        }
        break;

        case Scope::Global:
        {
            walking=  false;
            return globalStore;
        }
        break;
    }

    return ScopeStoreType<StoreT>::NullValue();
}

// #################################################################################################
// internals
// #################################################################################################
template<typename StoreT>
void ScopeStore<StoreT>::initCursor( bool create )
{
    lazyLanguageNode=   false;
    actStringTreeNode=  languageStore->Root();

    // path key for the StringTree
    String512 path;
    scopeInfo.GetTrimmedPath( path );
    #if defined( _WIN32 )
        path.SearchAndReplace( '\\', '/' );
    #endif

    // read-only mode
    if( !create )
    {
        // in non-creation mode, it is always scope Method
        ALIB_ASSERT( actScope == Scope::Method )

        // in read only mode, we can leave as soon as a portion was not read
        auto remainingPath= actStringTreeNode.MoveToExistingPart( path );
        if ( remainingPath.IsNotEmpty() )
            return;

        // filename: append '#' to distinguish from directories
        if ( !actStringTreeNode.MoveToChild( path._()._<false>( scopeInfo.GetFileNameWithoutExtension() )._( '#' ) ) )
            return;

        // filename: prepend '#' to distinguish from filenames
        actStringTreeNode.MoveToChild( path._()._<false>( '#' )._<false>( scopeInfo.GetMethod() ) );

        return;
    }

    // create mode:
    actStringTreeNode.MoveToAndCreateNonExistingPart( path );
    if ( actScope == Scope::Path )
    {
        // subtract folders at the back
        int pathLevel= actPathLevel;
        while ( --pathLevel >= 0 && !actStringTreeNode.IsRoot() )
            actStringTreeNode.MoveToParent();
        return;
    }

    // filename: append '#' to distinguish from directories
    path._()._( scopeInfo.GetFileNameWithoutExtension() )._( '#' );

    // filename: prepend '#' to distinguish from filenames
    if ( actScope == Scope::Method )
        path._( "/#" )._( scopeInfo.GetMethod() );

    actStringTreeNode.MoveToAndCreateNonExistingPart( path );
}

template<typename StoreT>
void ScopeStore<StoreT>::InitAccess( Scope scope, int pathLevel, int threadID )
{
    actScope=           scope;
    actPathLevel=       pathLevel;
    actThreadID=        threadID;
    lazyLanguageNode=   true;
}


template<typename StoreT>
StoreT ScopeStore<StoreT>::access(  int cmd, StoreT value  )
{
    StoreT oldValue= ScopeStoreType<StoreT>::NullValue();

    // --------- global scope ---------
    if( actScope == Scope::Global )
    {
        oldValue= globalStore;
        if ( cmd == CMD_INSERT )
            globalStore= value;
        else if ( cmd == CMD_REMOVE )
            globalStore= ScopeStoreType<StoreT>::NullValue();

        return oldValue;
    }

    // --------- thread-related scopes ---------
    if(     actScope == Scope::ThreadOuter
        ||  actScope == Scope::ThreadInner    )
    {
        // choose outer/inner store
        std::map<int, std::vector<StoreT>>*  threadStore=
            actScope == Scope::ThreadInner  ? &threadInnerStore
                                            : &threadOuterStore;

        // check if empty (to avoid key creation/thread detection )
        if ( cmd != CMD_INSERT && threadStore->size() == 0 )
            return oldValue;

        // thread given?
        if ( actThreadID == lib::threads::NullThreadId )
            actThreadID= scopeInfo.GetThreadID();

        // find (create) the vector of values
        std::vector<StoreT>* values;
        {
            auto it= threadStore->find( actThreadID );
            if ( it != threadStore->end() )
                values= &it->second;
            else
                values= &threadStore->insert( std::make_pair( actThreadID, std::vector<StoreT>() ) ).first->second;
        }

        // 'get'
        if ( cmd == CMD_GET )
            return ( values->size() > 0) ? (*values)[ values->size() -1 ] : ScopeStoreType<StoreT>::NullValue();

        // insert is simple, we do not even return an 'oldValue'
        if ( cmd == CMD_INSERT )
        {
            if ( cfgSingleThreadValue )
            {
                if ( values->size() > 0)
                {
                    oldValue= *values->begin();
                    if ( value != ScopeStoreType<StoreT>::NullValue())
                        *values->begin()= value;
                    else
                        values->clear();
                }
                else // value is never null here
                    values->emplace_back( value );
            }
            else
                // if multiple values are allowed we do not return an 'oldValue'
                values->emplace_back( value );

            return oldValue;
        }

        // remove has two options: the last or, if given, a specific one
        if ( cmd == CMD_REMOVE  && values->size() > 0)
        {
            // remove the last
            if ( value == ScopeStoreType<StoreT>::NullValue() )
            {
                oldValue= values->back();
                values->pop_back();
            }

            // remove a specific one.
            else
                for ( auto rem= values->begin() ; rem != values->end(); rem++ )
                {
                    //We return the found value, if found, otherwise we don't do anything
                    if ( ScopeStoreType<StoreT>::AreEqual( (*rem), value ) )
                    {
                        oldValue= *rem;
                        values->erase( rem );
                        break;
                    }
                }
        }

        return oldValue;
    }

    // --------- language-related scopes ---------
    {
        if ( cmd == CMD_INSERT && value == ScopeStoreType<StoreT>::NullValue() )
            cmd= CMD_REMOVE;

        if (    lazyLanguageNode
            ||  ( actStringTreeNode.IsInvalid() && cmd == CMD_INSERT ) )
            initCursor( true ); // always create

        if ( actStringTreeNode.IsValid() )
        {
            oldValue= *actStringTreeNode;

            if ( cmd == CMD_INSERT )
                *actStringTreeNode= value;

            else if ( cmd == CMD_REMOVE )
            {
                *actStringTreeNode= ScopeStoreType<StoreT>::NullValue();
            }
        }

        return oldValue;
    }
}

//! @endcond

}}}}// namespace [aworx::lib::lox::core]
