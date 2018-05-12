// #################################################################################################
//  aworx::lib::lox::core - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"
#include "scopedump.hpp"

// For code compatibility with ALox Java/C++
// We have to use underscore as the start of the name and for this have to disable a compiler
// warning. But this is a local code (cpp file) anyhow.
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

    #define _NC _<false>

#if defined(__clang__)
    #pragma clang diagnostic pop
#endif


using namespace aworx;


namespace aworx { namespace lib { namespace lox { namespace core {

//! @cond NO_DOX

// #################################################################################################
// template instantiations
// #################################################################################################
template   int ScopeDump::writeStore   ( ScopeStore<NAString*               >* store, int indentSpaces );
template   int ScopeDump::writeStore   ( ScopeStore<Box*                    >* store, int indentSpaces );
template   int ScopeDump::writeStoreMap( ScopeStore<std::map<AString, int>* >* store );
template   int ScopeDump::writeStoreMap( ScopeStore<std::map<AString, Box>* >* store );

// #################################################################################################
// local helper functions (non members)
// #################################################################################################
template<typename T> void write( T  val, AString& target )
{
    // prefix logable?
    if( std::is_same<T, Box*>::value )
    {
        target << '"';
        integer actLen= target.Length();
        target._( val );
        ESC::ReplaceToReadable( target, actLen );
        target << Format::Escape( Switch::On, actLen );
        target << '"';
    }
    else
        target._(val);
}

// #################################################################################################
// protected methods
// #################################################################################################
AString& ScopeDump::storeKeyToScope( String key )
{
    integer fileNameEnd= key.IndexOf('#');
    integer methodEnd=   fileNameEnd >= 0 ? key.IndexOf('#', fileNameEnd + 1)  : -1;

    targetBuffer._NC("Scope::");
         if ( methodEnd   >= 0 )  targetBuffer._NC( "Method      [" );
    else if ( fileNameEnd >= 0 )  targetBuffer._NC( "FileName    [" );
    else                          targetBuffer._NC( "Path        [" );

    integer targetStart= targetBuffer.Length();
    targetBuffer._NC( key );

    if ( methodEnd >= 0 )
    {
        targetBuffer.ReplaceSubstring( ASTR(" @"), targetStart + fileNameEnd +1, 2 ); // characters: "/#"
        targetBuffer._NC( ASTR("()") );
    }

    if ( fileNameEnd >= 0 )
        targetBuffer.ReplaceSubstring(ASTR(".*"), targetStart + fileNameEnd,  1);
    else
        targetBuffer._('/');

    targetBuffer._(']');

    return targetBuffer;
}

AString& ScopeDump::storeThreadToScope( int threadID )
{
    auto it= threadDict.find( threadID );
    if ( it != threadDict.end() )
        return targetBuffer._(ASTR("[Thread=\""))._( it->second )._(ASTR("\"]"));

    return targetBuffer._(ASTR("[ThreadID="))._( threadID )._(']');
}

template<typename T>
integer ScopeDump::writeStoreMapHelper( std::map<AString, T>& map, const String& prefix )
{
    for ( auto& it : map )
    {
        targetBuffer._NC( prefix );

        String64 keyString;

        if ( it.first.Equals( noKey ) )
            keyString._NC( ASTR("<global>") );
        else
            keyString._NC( '"' )._( it.first )._( '"' );
        if ( maximumKeyLength < keyString.Length() + 1 )
            maximumKeyLength= keyString.Length() + 1;

        targetBuffer._NC(Format::Field(keyString, maximumKeyLength, Alignment::Left))._NC( '=' );


        write( it.second, targetBuffer);
        targetBuffer.NewLine();
    }
    return maximumKeyLength;
}

// #################################################################################################
// Interface
// #################################################################################################
template<typename T>
int ScopeDump::writeStoreMap( ScopeStore<T>* store )
{
    int cnt= 0;
    bool firstEntry= true;
    if ( store->globalStore && store->globalStore->size() > 0)
    {
        cnt+=  static_cast<int>( store->globalStore->size() );
        firstEntry= false;
        targetBuffer._NC( ASTR("  Scope::Global:") ).NewLine();
        maximumKeyLength= writeStoreMapHelper( *store->globalStore, ASTR("    ") );
    }

    for ( auto& thread : store->threadOuterStore )
    {
        if ( thread.second.size() == 0 )
            continue;
        ALIB_ASSERT( thread.second.size() == 1 );
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._NC(ASTR("  Scope::ThreadOuter "));  storeThreadToScope( thread.first )._( ':' ).NewLine();
        cnt+= static_cast<int>( thread.second[0]->size() );
        maximumKeyLength= writeStoreMapHelper( *thread.second[0], ASTR("    ") );
    }

    String512 keyStr;
    typename ScopeStore<T>::LanguageStore::Walker walker( store->languageStore );
    walker.PathGeneration( Switch::On );
    walker.SetRecursionDepth(-1);
    for( walker.SetStart( store->languageStore) ;walker.IsValid() ; walker.Next() )
    {
        if( *walker == ScopeStoreType<T>::NullValue() )
            continue;
        cnt+= static_cast<int>( (*walker)->size() );
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._NC( ASTR("  ") );
        storeKeyToScope( walker.GetPath(keyStr) ).NewLine();
        maximumKeyLength= writeStoreMapHelper( **walker, ASTR("    ") );
    }

    for ( auto& thread : store->threadInnerStore )
    {
        if ( thread.second.size() == 0 )
            continue;
        ALIB_ASSERT( thread.second.size() == 1 );
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._NC(ASTR("  Scope::ThreadInner "));  storeThreadToScope( thread.first )._( ':' ).NewLine();
        cnt+= static_cast<int>( thread.second[0]->size() );
        maximumKeyLength= writeStoreMapHelper( *thread.second[0], ASTR("    ") );
    }
    return cnt;
}

template<typename T>
int ScopeDump::writeStore( ScopeStore<T>* store, int indentSpaces )
{
    int cnt= 0;
    if ( store->globalStore )
    {
        cnt++;
        targetBuffer.InsertChars( ' ', indentSpaces );
        write( store->globalStore, targetBuffer );
        targetBuffer._NC(Format::Tab( 25, -1 ) )._NC( ASTR("Scope::Global ") ).NewLine();
    }

    for ( auto& thread : store->threadOuterStore )
        for ( auto& it : thread.second )
        {
            cnt++;
            targetBuffer.InsertChars( ' ', indentSpaces );
            write(it, targetBuffer);
            targetBuffer._NC( Format::Tab( 25, -1 ) )
                  ._NC( ASTR("Scope::ThreadOuter ") );
            storeThreadToScope( thread.first ).NewLine();
        }

    {
        String512 keyStr;
        typename ScopeStore<T>::LanguageStore::Walker walker( store->languageStore );
        walker.PathGeneration( Switch::On );
        walker.SetRecursionDepth(-1);
        for( walker.SetStart( store->languageStore) ;walker.IsValid() ; walker.Next() )
        {
            if( *walker == ScopeStoreType<T>::NullValue() )
                continue;
            cnt++;
            targetBuffer.InsertChars( ' ', indentSpaces );
            write( *walker, targetBuffer );
            targetBuffer._NC(Format::Tab( 25, -1 ) );
            storeKeyToScope( walker.GetPath(keyStr) ).NewLine();
        }
    }

    for ( auto& thread : store->threadInnerStore )
        for ( auto& it : thread.second )
        {
            cnt++;
            targetBuffer.InsertChars( ' ', indentSpaces );
            write(it, targetBuffer);
            targetBuffer._NC( Format::Tab( 25, -1 ) )
                  ._NC( ASTR("Scope::ThreadInner ") );
            storeThreadToScope( thread.first ).NewLine();
        }
    return cnt;
}

//! @endcond
}}}}// namespace [aworx::lib::lox::core]
