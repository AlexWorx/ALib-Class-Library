// #################################################################################################
//  alib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/alox/alox.hpp"
#   include "alib/lang/message/report.hpp"
#   define HPP_ALIB_LOX_PROPPERINCLUDE
#       include "alib/alox/detail/scopestore.inl"
#       include "alib/alox/detail/scopeinfo.inl"
#   undef HPP_ALIB_LOX_PROPPERINCLUDE
#endif // !DOXYGEN

using namespace alib;

namespace alib {  namespace lox { namespace detail {

#define    CMD_INSERT 0
#define    CMD_REMOVE 1
#if ALIB_THREADS
#   define CMD_GET    2
#endif


// #################################################################################################
// Constructor/Destructor
// #################################################################################################

template<typename T, bool TStackedThreadValues>
ScopeStore<T, TStackedThreadValues>::ScopeStore( ScopeInfo&      pScopeInfo,
                                                 MonoAllocator&  monoAllocator  )
  : globalStore  ( nullptr   )
  , languageStore( monoAllocator , '/'              )
  IF_ALIB_THREADS(, threadStore  ( monoAllocator )  )
  , scopeInfo    ( pScopeInfo                       )
{
    #if ALIB_DEBUG_CRITICAL_SECTIONS
        languageStore.DbgSetDCSName("ScopeStore");
    #endif
    languageStore.ConstructRootValue(nullptr);
}

template<typename T, bool TStackedThreadValues>
ScopeStore<T, TStackedThreadValues>::~ScopeStore()
{
    languageStore.DestructRootValue();
}


// #################################################################################################
// Methods
// #################################################################################################
template<typename T, bool TStackedThreadValues>
void ScopeStore<T,TStackedThreadValues>::InitWalk( Scope startScope, T localObject  )
{
    actScope=            startScope;
    walkLocalObject=     localObject;
    actPathLevel=        0;
    walkNextThreadIdx=   -2;
    lazyLanguageNode=    true;
    walking=             true;
}

#if !DOXYGEN

template<typename T>
T ScopeStoreHelper<T, false>:: doWalk( ScopeStore<T, false>& self )
{
    while ( self.walking )  switch( self.actScope )
    {
        case Scope::ThreadInner:
        {
            // initialize
            if ( self.walkNextThreadIdx == -2 )
            {
                self.walkNextThreadIdx= -1;
                #if ALIB_THREADS
                    if ( self.threadStore.Size() != 0 )
                    {
                        auto it= self.threadStore.Find( typename ScopeStore<T, true>::ThreadMapKey(true, self.scopeInfo.GetThreadID()) );
                        if ( it != self.threadStore.end() )
                        {
                            self.walkThreadValues=    &it->second;
                            self.walkNextThreadIdx=   1;
                        }
                    }
                #endif
            }

            // return next inner thread object (traversalNextScope keeps being ThreadInner)
            if ( self.walkNextThreadIdx > 0 )
            {
                --self.walkNextThreadIdx;
                return (*self.walkThreadValues);
            }

            // next scope is Method
            self.actScope= Scope::Method;

            // if we have a valid 'local object' return this first
            if ( self.walkLocalObject != nullptr )
                return self.walkLocalObject;
        }
        break;

        case Scope::Method:
        case Scope::Filename:
        case Scope::Path:
        {
            if( self.lazyLanguageNode )
                self.initCursor( false );

            while( self.actStringTreeNode.IsValid() )
            {
                T actValue= *self.actStringTreeNode;
                self.actStringTreeNode.GoToParent();
                if( actValue != nullptr )
                    return actValue;
            }

            self.actScope=  Scope::ThreadOuter;
            self.walkNextThreadIdx= -2;
        }
        break;

        case Scope::ThreadOuter:
        {
            // initialize
            if ( self.walkNextThreadIdx == -2 )
            {
                #if ALIB_THREADS
                    if ( self.threadStore.Size() != 0 )
                    {
                        auto it= self.threadStore.Find( typename ScopeStore<T, true>::ThreadMapKey(false, self.scopeInfo.GetThreadID()) );
                        if ( it != self.threadStore.end() )
                        {
                            self.walkThreadValues=    &it->second;
                            self.walkNextThreadIdx=   1;

                        }
                    }
                #endif
            }

            // return next outer thread object (walkNext keeps being THREAD_OUTER)
            if ( self.walkNextThreadIdx > 0 )
            {
                --self.walkNextThreadIdx;
                return (*self.walkThreadValues);
            }

            // next scope is Global
            self.actScope= Scope::Global;
        }
        break;

        case Scope::Global:
        {
            self.walking=  false;
            return self.globalStore;
        }
        break;

        default: ALIB_ERROR("Illegal switch state.") break;
    }

    return nullptr;
}

template<typename T>
T ScopeStoreHelper<T, false>::doAccess( ScopeStore<T, false>& self, int cmd, T value )
{
    T oldValue= nullptr;

    // --------- global scope ---------
    if( self.actScope == Scope::Global )
    {
        oldValue= self.globalStore;
        if ( cmd == CMD_INSERT )
            self.globalStore= value;
        else if ( cmd == CMD_REMOVE )
            self.globalStore= nullptr;

        return oldValue;
    }

#if ALIB_THREADS
    // --------- thread-related scopes ---------
    if(     self.actScope == Scope::ThreadOuter
        ||  self.actScope == Scope::ThreadInner    )
    {
        // choose outer/inner store
        bool isInner= self.actScope == Scope::ThreadInner;

        // check if empty (to avoid key creation/thread detection )
        if ( cmd != CMD_INSERT && self.threadStore.Size() == 0 )
            return oldValue;

        // thread given?
        if ( self.actThreadID == threads::UNDEFINED )
             self.actThreadID=   self.scopeInfo.GetThreadID();

        // --- implementation for non-stacked values (values stored in a ma) ---
        ALIB_ASSERT( cmd != CMD_REMOVE ) // no remove implemented (needed)

        // 'get'
        auto key = typename ScopeStore<T, true>::ThreadMapKey(isInner, self.actThreadID);
        auto hash=  typename decltype(self.threadStore)::HashType ()( key );
        if ( cmd == CMD_GET )
        {
            auto it= self.threadStore.Find( key, hash );
            if ( it != self.threadStore.end() )
                return it->second;

            return oldValue;
        }

        // insert is simple, we do not even return an 'oldValue'
        ALIB_ASSERT( cmd == CMD_INSERT )
        self.threadStore.InsertUnique( std::make_pair( key, value), hash );

        return oldValue;

    }
#endif

    // --------- language-related scopes ---------
    {
        if ( cmd == CMD_INSERT && value == nullptr )
            cmd= CMD_REMOVE;

        if (    self.lazyLanguageNode
            ||  ( self.actStringTreeNode.IsInvalid() && cmd == CMD_INSERT ) )
            self.initCursor( true ); // always create

        oldValue= *self.actStringTreeNode;
             if ( cmd == CMD_INSERT )  *self.actStringTreeNode= value;
        else if ( cmd == CMD_REMOVE )  *self.actStringTreeNode= nullptr;

        return oldValue;
    }
}

template<typename T> T ScopeStoreHelper<T, true>::doWalk( ScopeStore<T, true>& self )
{
    while ( self.walking )  switch( self.actScope )
    {
        case Scope::ThreadInner:
        {
            // initialize
            if ( self.walkNextThreadIdx == -2 )
            {
                self.walkNextThreadIdx= -1;
                #if ALIB_THREADS
                    if ( self.threadStore.Size() != 0 )
                    {
                        auto it= self.threadStore.Find( typename ScopeStore<T, true>::ThreadMapKey(true, self.scopeInfo.GetThreadID()) );
                        if ( it != self.threadStore.end() )
                        {
                            self.walkThreadValues=    &it.Mapped();
                            self.walkNextThreadIdx=   static_cast<int>( self.walkThreadValues->size() );
                        }
                    }
                #endif
            }

            // return next inner thread object (traversalNextScope keeps being ThreadInner)
            if ( self.walkNextThreadIdx > 0 )
            {
                --self.walkNextThreadIdx;
                return (*self.walkThreadValues)[size_t(self.walkNextThreadIdx)];
            }

            // next scope is Method
            self.actScope= Scope::Method;

            // if we have a valid 'local object' return this first
            if ( self.walkLocalObject != nullptr )
                return self.walkLocalObject;
        }
        break;

        case Scope::Method:
        case Scope::Filename:
        case Scope::Path:
        {
            if( self.lazyLanguageNode )
                self.initCursor( false );

            while( self.actStringTreeNode.IsValid() )
            {
                T actValue= *self.actStringTreeNode;
                self.actStringTreeNode.GoToParent();
                if( actValue != nullptr )
                    return actValue;
            }

            self.actScope=  Scope::ThreadOuter;
            self.walkNextThreadIdx= -2;
        }
        break;

        case Scope::ThreadOuter:
        {
            // initialize
            if ( self.walkNextThreadIdx == -2 )
            {
                #if ALIB_THREADS
                    if ( self.threadStore.Size() != 0 )
                    {
                        auto it= self.threadStore.Find( typename ScopeStore<T, true>::ThreadMapKey(false, self.scopeInfo.GetThreadID()) );
                        if ( it != self.threadStore.end() )
                        {
                            self.walkThreadValues=    &it.Mapped();
                            self.walkNextThreadIdx=   static_cast<int>( self.walkThreadValues->size() );
                        }
                    }
                #endif
            }

            // return next outer thread object (walkNext keeps being THREAD_OUTER)
            if ( self.walkNextThreadIdx > 0 )
            {
                --self.walkNextThreadIdx;
                return (*self.walkThreadValues)[size_t(self.walkNextThreadIdx)];
            }

            // next scope is Global
            self.actScope= Scope::Global;
        }
        break;

        case Scope::Global:
        {
            self.walking=  false;
            return self.globalStore;
        }
        break;

        default: ALIB_ERROR("Illegal switch state.") break;
    }

    return nullptr;
}


template<typename T> T ScopeStoreHelper<T, true>::doAccess( ScopeStore<T, true>& self, int cmd, T value )
{
    T oldValue= nullptr;

    // --------- global scope ---------
    if( self.actScope == Scope::Global )
    {
        oldValue= self.globalStore;
             if ( cmd == CMD_INSERT )    self.globalStore= value;
        else if ( cmd == CMD_REMOVE )    self.globalStore= nullptr;

        return oldValue;
    }

#if ALIB_THREADS
    // --------- thread-related scopes ---------
    if(     self.actScope == Scope::ThreadOuter
        ||  self.actScope == Scope::ThreadInner    )
    {
        // choose outer/inner store
        bool isInner= self.actScope == Scope::ThreadInner;

        // check if empty (to avoid key creation/thread detection )
        if ( cmd != CMD_INSERT && self.threadStore.Size() == 0 )
            return oldValue;

        // thread given?
        if ( self.actThreadID == threads::UNDEFINED )
             self.actThreadID=   self.scopeInfo.GetThreadID();

        // --- implementation for stacked values ---
        // find (create) the vector of values
        StdVectorMono<T>* values;
        {
            values= &self.threadStore.EmplaceIfNotExistent(
                        typename ScopeStore<T, true>::ThreadMapKey(isInner, self.actThreadID),
                              self.threadStore.GetAllocator()  ).first.Mapped();
        }

        // 'get'
        if ( cmd == CMD_GET )
            return ( values->size() > 0) ? (*values)[ values->size() -1 ] : nullptr;

        // insert is simple, we do not even return an 'oldValue'
        if ( cmd == CMD_INSERT )
        {
            values->emplace_back( value );
            return oldValue; // if multiple values are allowed we do not return an 'oldValue'
        }

        // remove has two options: the last or, if given, a specific one
        if ( cmd == CMD_REMOVE  && values->size() > 0)
        {
            // remove the last
            if ( value == nullptr )
            {
                oldValue= values->back();
                values->pop_back();
            }

            // remove a specific one.
            else
                for ( auto rem= values->begin() ; rem != values->end(); ++rem )
                    if ( (*rem) == value )
                    {
                        // If found, we return the value, otherwise we don't do anything
                        oldValue= *rem;
                        values->erase( rem );
                        break;
                    }
        }

        return oldValue;
    }
#endif

    // --------- language-related scopes ---------
    {
        if ( cmd == CMD_INSERT && value == nullptr )
            cmd= CMD_REMOVE;

        if (    self.lazyLanguageNode
            ||  ( self.actStringTreeNode.IsInvalid() && cmd == CMD_INSERT ) )
            self.initCursor( true ); // always create

        oldValue= *self.actStringTreeNode;
             if ( cmd == CMD_INSERT )   *self.actStringTreeNode= value;
        else if ( cmd == CMD_REMOVE )   *self.actStringTreeNode= nullptr;

        return oldValue;
    }
}

#endif

// #################################################################################################
// internals
// #################################################################################################
template<typename T, bool TStackedThreadValues>
void ScopeStore<T,TStackedThreadValues>::initCursor( bool create )
{
    lazyLanguageNode=   false;
    actStringTreeNode=  languageStore.Root();

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
        auto remainingPath= actStringTreeNode.GoTo( path );
        if ( remainingPath.IsNotEmpty() )
            return;

        // filename: append '#' to distinguish from directories
        if ( !actStringTreeNode.GoToChild( path.Reset<NC>( scopeInfo.GetFileNameWithoutExtension() )._( '#' ) ) )
            return;

        // method: prepend '#' to distinguish from filenames
        actStringTreeNode.GoToChild( path.Reset<NC>( '#' )._<NC>( scopeInfo.GetMethod() ) );

        return;
    }

    // create mode:
    actStringTreeNode.GoToCreatedPathIfNotExistent( path );
    if ( actScope == Scope::Path )
    {
        // subtract folders at the back
        int pathLevel= actPathLevel;
        while ( --pathLevel >= 0 && !actStringTreeNode.IsRoot() )
            actStringTreeNode.GoToParent();
        return;
    }

    // filename: append '#' to distinguish from directories
    path.Reset( scopeInfo.GetFileNameWithoutExtension() )._( '#' );

    // method: prepend '#' to distinguish from filenames
    if ( actScope == Scope::Method )
        path._( "/#" )._( scopeInfo.GetMethod() );

    actStringTreeNode.GoToCreatedPathIfNotExistent( path );
}

template<typename T, bool TStackedThreadValues>
void ScopeStore<T,TStackedThreadValues>::InitAccess( Scope scope, int pathLevel, threads::ThreadID threadID )
{
    actScope=           scope;
    actPathLevel=       pathLevel;
#if ALIB_THREADS
    actThreadID=        threadID;
#else
    (void) threadID;
#endif

    lazyLanguageNode=   true;
}


// #################################################################################################
// template instantiations
// #################################################################################################

//! @cond NO_DOX

template struct ScopeStoreHelper<NString                , true>;
template class  ScopeStore      <NString                , true>;

template struct ScopeStoreHelper<PrefixLogable*         , true>;
template class  ScopeStore      <PrefixLogable*         , true>;

template struct ScopeStoreHelper<SSMap<int>*, false>;
template class  ScopeStore      <SSMap<int>*, false>;

template struct ScopeStoreHelper<SSMap<Box>*, false>;
template class  ScopeStore      <SSMap<Box>*, false>;

//! @endcond

}}} // namespace [alib::lox::detail]

