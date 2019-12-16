// #################################################################################################
//  aworx::lib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_ALOX)
#      include "alib/alox/alox.hpp"
#   endif

#   define HPP_ALIB_LOX_PROPPERINCLUDE
#      if !defined (HPP_ALOX_DETAIL_SCOPE)
#          include "alib/alox/detail/scopestore.inl"
#      endif
#      if !defined (HPP_ALOX_DETAIL_SCOPEINFO)
#          include "alib/alox/detail/scopeinfo.inl"
#      endif
#      include "alib/alox/detail/scopedump.inl"
#   undef HPP_ALIB_LOX_PROPPERINCLUDE

#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#       include "alib/strings/format.hpp"
#   endif
#endif // !defined(ALIB_DOX)


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
template   int ScopeDump::writeStore   ( ScopeStore<NString                , true >* store, int indentSpaces );
template   int ScopeDump::writeStore   ( ScopeStore<PrefixLogable*         , true >* store, int indentSpaces );
template   int ScopeDump::writeStoreMap( ScopeStore<std::map<NString, int>*, false>* store );
template   int ScopeDump::writeStoreMap( ScopeStore<std::map<NString, Box>*, false>* store );

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
        target._(*static_cast<Box*>(val));
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

#if ALIB_THREADS
NAString& ScopeDump::storeThreadToScope( threads::ThreadID threadID )
{
    auto it= threadDict.Find( threadID );
    if ( it != threadDict.end() )
        return targetBuffer._("[Thread=\"")._( it->second )._("\"]");

    return targetBuffer._("[ThreadID=")._( threadID )._(']');
}
#endif

template<typename T>
integer ScopeDump::writeStoreMapHelper( std::map<NString, T>& map, const NString& prefix )
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
int ScopeDump::writeStoreMap( ScopeStore<T, false>* store )
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

#if ALIB_THREADS
    for ( auto thread= store->threadStore.begin() ; thread != store->threadStore.end() ; ++thread )
    {
        if ( thread->first.first== false )
            continue;
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._NC("  Scope::ThreadOuter ");  storeThreadToScope( thread->first.second )._( ':' ).NewLine();
        cnt+= static_cast<int>( thread->second->size() );
        maximumKeyLength= writeStoreMapHelper( *thread->second, "    " );
    }
#endif


    String512 keyStr;
    typename ScopeStore<T, false>::LanguageStoreT::RecursiveIterator iterator;
    iterator.SetSorting( Switch::On );
    iterator.SetPathGeneration( Switch::On );
    for( iterator.Initialize( store->languageStore) ; iterator.IsValid() ; iterator.Next() )
    {
        if( iterator.Node().Value() == ScopeStoreType<T>::NullValue() )
            continue;
        cnt+= static_cast<int>( iterator.Node().Value()->size() );
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._NC( "  " );
        storeKeyToScope( iterator.FullPath( keyStr) ).NewLine();
        maximumKeyLength= writeStoreMapHelper( *iterator.Node().Value(), "    " );
    }

#if ALIB_THREADS
    for ( auto thread= store->threadStore.begin() ; thread != store->threadStore.end() ; ++thread )
    {
        if ( thread->first.first == true )
            continue;
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._NC("  Scope::ThreadInner ");  storeThreadToScope( thread->first.second )._( ':' ).NewLine();
        cnt+= static_cast<int>( thread->second->size() );
        maximumKeyLength= writeStoreMapHelper( *thread->second, "    " );
    }
#endif
    return cnt;
}

template<typename T>
int ScopeDump::writeStore( ScopeStore<T, true>* store, int indentSpaces )
{
    int cnt= 0;

    // global store
    if ( !ScopeStoreType<T>::IsNull(store->globalStore) )
    {
        ++cnt;
        targetBuffer.InsertChars( ' ', indentSpaces );
        write( store->globalStore, targetBuffer );
        targetBuffer._NC(NFormat::Tab( 25, -1 ) )._NC( "Scope::Global " ).NewLine();
    }

    // outer thread store
#if ALIB_THREADS
    for ( auto thread= store->threadStore.begin() ; thread != store->threadStore.end() ; ++thread )
        if( thread->first.first == false )
            for ( auto& it : thread->second )
            {
                ++cnt;
                targetBuffer.InsertChars( ' ', indentSpaces );
                write(it, targetBuffer);
                targetBuffer._NC( NFormat::Tab( 25, -1 ) )
                      ._NC( "Scope::ThreadOuter " );
                storeThreadToScope( thread->first.second ).NewLine();
            }
#endif

    // language store
    {
        String512 keyStr;
        typename ScopeStore<T, true>::LanguageStoreT::RecursiveIterator iterator;
        iterator.SetSorting( Switch::On );
        iterator.SetPathGeneration( Switch::On );
        for( iterator.Initialize( store->languageStore ); iterator.IsValid() ; iterator.Next() )
        {
            if( iterator.Node().Value() == ScopeStoreType<T>::NullValue() )
                continue;
            ++cnt;
            targetBuffer.InsertChars( ' ', indentSpaces );
            write( iterator.Node().Value(), targetBuffer );
            targetBuffer._NC(NFormat::Tab( 25, -1 ) );
            storeKeyToScope( iterator.FullPath( keyStr) ).NewLine();
        }
    }

    // inner thread store
#if ALIB_THREADS
    for ( auto thread= store->threadStore.begin() ; thread != store->threadStore.end() ; ++thread )
        if( thread->first.first == true )
            for ( auto& it : thread->second )
            {
                ++cnt;
                targetBuffer.InsertChars( ' ', indentSpaces );
                write(it, targetBuffer);
                targetBuffer._NC( NFormat::Tab( 25, -1 ) )
                      ._NC( "Scope::ThreadInner " );
                storeThreadToScope( thread->first.second ).NewLine();
            }
#endif
    return cnt;
}

//! @endcond
}}}}// namespace [aworx::lib::lox::detail]
