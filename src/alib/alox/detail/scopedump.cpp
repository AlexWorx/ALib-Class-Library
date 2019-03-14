// #################################################################################################
//  aworx::lib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALOX_CORE_SCOPEDUMP)
    #define HPP_ALIB_LOX_PROPPERINCLUDE
    #   include "alib/alox/detail/scopedump.inl"
    #undef HPP_ALIB_LOX_PROPPERINCLUDE
#endif

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


namespace aworx { namespace lib { namespace lox { namespace detail {

//! @cond NO_DOX

// #################################################################################################
// template instantiations
// #################################################################################################
template   int ScopeDump::writeStore   ( ScopeStore<NAString*                >* store, int indentSpaces );
template   int ScopeDump::writeStore   ( ScopeStore<Box*                     >* store, int indentSpaces );
template   int ScopeDump::writeStoreMap( ScopeStore<std::map<NAString, int>* >* store );
template   int ScopeDump::writeStoreMap( ScopeStore<std::map<NAString, Box>* >* store );

// #################################################################################################
// local helper functions (non members)
// #################################################################################################
namespace {

template<typename T> void write( const T&  val, NAString& target )
{
    target._(val);
}

template<typename T> void write(       T*  val, NAString& target )
{
    // prefix logable?
    if( std::is_same<T, Box*>::value )
    {
        String256 buffer;
        buffer << '"';
        integer actLen= buffer.Length();
        buffer._( *val );
        ESC::ReplaceToReadable( buffer, actLen );
        buffer << Format::Escape( Switch::On, actLen );
        buffer << '"';
        target << buffer;
    }
    else
        target._(*val);
}
} // anonymous namespace

// #################################################################################################
// protected methods
// #################################################################################################
NAString& ScopeDump::storeKeyToScope( String key )
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
        targetBuffer.ReplaceSubstring<false>( " @", targetStart + fileNameEnd +1, 2 ); // characters: "/#"
        targetBuffer._NC( "()" );
    }

    if ( fileNameEnd >= 0 )
        targetBuffer.ReplaceSubstring<false>(".*", targetStart + fileNameEnd,  1);
    else
        targetBuffer._('/');

    targetBuffer._(']');

    return targetBuffer;
}

NAString& ScopeDump::storeThreadToScope( ThreadID threadID )
{
    auto it= threadDict.find( threadID );
    if ( it != threadDict.end() )
        return targetBuffer._("[Thread=\"")._( it->second )._("\"]");

    return targetBuffer._("[ThreadID=")._( threadID )._(']');
}

template<typename T>
integer ScopeDump::writeStoreMapHelper( std::map<NAString, T>& map, const NString& prefix )
{
    for ( auto& it : map )
    {
        targetBuffer._NC( prefix );

        String64 keyString;

        if ( it.first.Equals( noKey ) )
            keyString._NC( "<global>" );
        else
            keyString._NC( '"' )._( it.first )._( '"' );
        if ( maximumKeyLength < keyString.Length() + 1 )
            maximumKeyLength= keyString.Length() + 1;

        targetBuffer._NC(NFormat::Field(keyString, maximumKeyLength, Alignment::Left))._NC( '=' );


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
        targetBuffer._NC( "  Scope::Global:" ).NewLine();
        maximumKeyLength= writeStoreMapHelper( *store->globalStore, "    " );
    }

    for ( auto& thread : store->threadOuterStore )
    {
        if ( thread.second.size() == 0 )
            continue;
        ALIB_ASSERT( thread.second.size() == 1 );
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._NC("  Scope::ThreadOuter ");  storeThreadToScope( thread.first )._( ':' ).NewLine();
        cnt+= static_cast<int>( thread.second[0]->size() );
        maximumKeyLength= writeStoreMapHelper( *thread.second[0], "    " );
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
        targetBuffer._NC( "  " );
        storeKeyToScope( walker.GetPath(keyStr) ).NewLine();
        maximumKeyLength= writeStoreMapHelper( **walker, "    " );
    }

    for ( auto& thread : store->threadInnerStore )
    {
        if ( thread.second.size() == 0 )
            continue;
        ALIB_ASSERT( thread.second.size() == 1 );
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._NC("  Scope::ThreadInner ");  storeThreadToScope( thread.first )._( ':' ).NewLine();
        cnt+= static_cast<int>( thread.second[0]->size() );
        maximumKeyLength= writeStoreMapHelper( *thread.second[0], "    " );
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
        targetBuffer._NC(NFormat::Tab( 25, -1 ) )._NC( "Scope::Global " ).NewLine();
    }

    for ( auto& thread : store->threadOuterStore )
        for ( auto& it : thread.second )
        {
            cnt++;
            targetBuffer.InsertChars( ' ', indentSpaces );
            write(it, targetBuffer);
            targetBuffer._NC( NFormat::Tab( 25, -1 ) )
                  ._NC( "Scope::ThreadOuter " );
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
            targetBuffer._NC(NFormat::Tab( 25, -1 ) );
            storeKeyToScope( walker.GetPath(keyStr) ).NewLine();
        }
    }

    for ( auto& thread : store->threadInnerStore )
        for ( auto& it : thread.second )
        {
            cnt++;
            targetBuffer.InsertChars( ' ', indentSpaces );
            write(it, targetBuffer);
            targetBuffer._NC( NFormat::Tab( 25, -1 ) )
                  ._NC( "Scope::ThreadInner " );
            storeThreadToScope( thread.first ).NewLine();
        }
    return cnt;
}

//! @endcond
}}}}// namespace [aworx::lib::lox::detail]
