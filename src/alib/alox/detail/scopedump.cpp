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
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.Boxing.H"
#   include "ALib.Containers.StringTreeIterator.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Variables.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"
#endif
//========================================== Implementation ========================================
namespace alib {  namespace lox { namespace detail {

//! @cond NO_DOX

//##################################################################################################
// template instantiations
//##################################################################################################
template   int ScopeDump::writeStore   ( ScopeStore<NString                              , true >* store, int indentSpaces );
template   int ScopeDump::writeStore   ( ScopeStore<PrefixLogable*                       , true >* store, int indentSpaces );
template   int ScopeDump::writeStoreMap( ScopeStore<SSMap<int>*, false>* store );
template   int ScopeDump::writeStoreMap( ScopeStore<SSMap<Box>*, false>* store );

//##################################################################################################
// local helper functions (non members)
//##################################################################################################
namespace {

template<typename T> void write( const T&  val, NAString& target )                { target._(val); }

template<typename T> void write(       T*  val, NAString& target ) {
    // prefix logable?
    if( std::is_same<T, Box*>::value ) {
        String256 buffer;
        buffer << '"';
        integer actLen= buffer.Length();
        buffer._( *val );
        ESC::ReplaceToReadable( buffer, actLen );
        buffer << Escape( lang::Switch::On, actLen );
        buffer << '"';
        target << buffer;
    }
    else
        target._(*static_cast<Box*>(val));
}
} // anonymous namespace

//##################################################################################################
// protected methods
//##################################################################################################
NAString& ScopeDump::storeKeyToScope( String key ) {
    integer fileNameEnd= key.IndexOf('#');
    integer methodEnd=   fileNameEnd >= 0 ? key.IndexOf('#', fileNameEnd + 1)  : -1;

    targetBuffer._<NC>("Scope::");
         if ( methodEnd   >= 0 )  targetBuffer._<NC>( "Method      [" );
    else if ( fileNameEnd >= 0 )  targetBuffer._<NC>( "FileName    [" );
    else                          targetBuffer._<NC>( "Path        [" );

    integer targetStart= targetBuffer.Length();
    targetBuffer._<NC>( key );

    if ( methodEnd >= 0 ) {
        targetBuffer.ReplaceSubstring<NC>( " @", targetStart + fileNameEnd +1, 2 ); // characters: "/#"
        targetBuffer._<NC>( "()" );
    }

    if ( fileNameEnd >= 0 )
        targetBuffer.ReplaceSubstring<NC>(".*", targetStart + fileNameEnd,  1);
    else
        targetBuffer._('/');

    targetBuffer._(']');

    return targetBuffer;
}

#if !ALIB_SINGLE_THREADED
NAString& ScopeDump::storeThreadToScope( threads::ThreadID threadID ) {
    auto it= threadDict.Find( threadID );
    if ( it != threadDict.end() )
        return targetBuffer._("[Thread=\"")._( it->second )._("\"]");

    return targetBuffer._("[ThreadID=")._( threadID )._(']');
}
#endif

template<typename T>
integer ScopeDump::writeStoreMapHelper(SSMap<T>& map, const NString& prefix ) {
    for ( auto& it : map ) {
        targetBuffer._<NC>( prefix );

        String64 keyString;

        if ( it.first.template Equals<NC>( noKey ) )
            keyString._<NC>( "<global>" );
        else
            keyString._<NC>( '"' )._( it.first )._( '"' );
        if ( maximumKeyLength < keyString.Length() + 1 )
            maximumKeyLength= keyString.Length() + 1;

        targetBuffer._<NC>(NField(keyString, maximumKeyLength, lang::Alignment::Left))._<NC>( '=' );


        write( it.second, targetBuffer);
        targetBuffer.NewLine();
    }
    return maximumKeyLength;
}

//##################################################################################################
// Interface
//##################################################################################################
template<typename T>
int ScopeDump::writeStoreMap( ScopeStore<T, false>* store ) {
    int cnt= 0;
    bool firstEntry= true;
    if ( store->globalStore && store->globalStore->Size() > 0) {
        cnt+=  int( store->globalStore->Size() );
        firstEntry= false;
        targetBuffer._<NC>( "  Scope::Global:" ).NewLine();
        maximumKeyLength= writeStoreMapHelper( *store->globalStore, "    " );
    }

#if !ALIB_SINGLE_THREADED
    for ( auto thread= store->threadStore.begin() ; thread != store->threadStore.end() ; ++thread ) {
        if ( thread->first.first== false )
            continue;
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._<NC>("  Scope::ThreadOuter ");  storeThreadToScope( thread->first.second )._( ':' ).NewLine();
        cnt+= int( thread->second->Size() );
        maximumKeyLength= writeStoreMapHelper( *thread->second, "    " );
    }
#endif


    StringTreeIterator<typename ScopeStore<T, false>::TLanguageStore> iterator;
    typename decltype(iterator)::NameSorter sorter;
    iterator.SetSorting(&sorter);
    iterator.SetPathGeneration( lang::Switch::On );
    for( iterator.Initialize( store->languageStore.Root(), lang::Inclusion::Exclude)
         ; iterator.IsValid()
         ; iterator.Next() )
    {
        if( *iterator.Node() == nullptr )
            continue;
        cnt+= int( (*iterator.Node())->Size() );
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._<NC>( "  " );
        storeKeyToScope( iterator.Path() ).NewLine();
        maximumKeyLength= writeStoreMapHelper( **iterator.Node(), "    " );
    }

#if !ALIB_SINGLE_THREADED
    for ( auto thread= store->threadStore.begin() ; thread != store->threadStore.end() ; ++thread ) {
        if ( thread->first.first == true )
            continue;
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._<NC>("  Scope::ThreadInner ");  storeThreadToScope( thread->first.second )._( ':' ).NewLine();
        cnt+= int( thread->second->Size() );
        maximumKeyLength= writeStoreMapHelper( *thread->second, "    " );
    }
#endif
    return cnt;
}

template<typename T>
int ScopeDump::writeStore( ScopeStore<T, true>* store, int indentSpaces ) {
    int cnt= 0;

    // global store
    if ( store->globalStore != nullptr ) {
        ++cnt;
        targetBuffer.InsertChars( ' ', indentSpaces );
        write( store->globalStore, targetBuffer );
        targetBuffer._<NC>(NTab( 25, -1 ) )._<NC>( "Scope::Global " ).NewLine();
    }

    // outer thread store
#if !ALIB_SINGLE_THREADED
    for ( auto thread= store->threadStore.begin() ; thread != store->threadStore.end() ; ++thread )
        if( thread->first.first == false )
            for ( auto& it : thread->second ) {
                ++cnt;
                targetBuffer.InsertChars( ' ', indentSpaces );
                write(it, targetBuffer);
                targetBuffer._<NC>( NTab( 25, -1 ) )
                      ._<NC>( "Scope::ThreadOuter " );
                storeThreadToScope( thread->first.second ).NewLine();
            }
#endif

    // language store
    {
        StringTreeIterator<typename ScopeStore<T, true>::TLanguageStore> iterator;
        typename decltype(iterator)::NameSorter sorter;
        iterator.SetSorting(&sorter);
        iterator.SetPathGeneration( lang::Switch::On );
        for(  iterator.Initialize( store->languageStore.Root(), lang::Inclusion::Exclude )
            ; iterator.IsValid()
            ; iterator.Next() )
        {
            if( *iterator.Node() == nullptr )
                continue;
            ++cnt;
            targetBuffer.InsertChars( ' ', indentSpaces );
            write( *iterator.Node(), targetBuffer );
            targetBuffer._<NC>(NTab( 25, -1 ) );
            storeKeyToScope( iterator.Path() ).NewLine();
    }   }

    // inner thread store
#if !ALIB_SINGLE_THREADED
    for ( auto thread= store->threadStore.begin() ; thread != store->threadStore.end() ; ++thread )
        if( thread->first.first == true )
            for ( auto& it : thread->second ) {
                ++cnt;
                targetBuffer.InsertChars( ' ', indentSpaces );
                write(it, targetBuffer);
                targetBuffer._<NC>( NTab( 25, -1 ) )
                      ._<NC>( "Scope::ThreadInner " );
                storeThreadToScope( thread->first.second ).NewLine();
            }
#endif
    return cnt;
}

//! @endcond
}}} // namespace [alib::lox::detail]
