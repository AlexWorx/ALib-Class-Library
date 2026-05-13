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
    for ( auto threadIt= store->threadStore.begin() ; threadIt != store->threadStore.end() ; ++threadIt ) {
        if ( threadIt->first.first== false )
            continue;
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._<NC>("  Scope::ThreadOuter ");  storeThreadToScope( threadIt->first.second )._( ':' ).NewLine();
        cnt+= int( threadIt->second->Size() );
        maximumKeyLength= writeStoreMapHelper( *threadIt->second, "    " );
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
    for ( auto threadIt= store->threadStore.begin() ; threadIt != store->threadStore.end() ; ++threadIt ) {
        if ( threadIt->first.first == true )
            continue;
        if( firstEntry ) firstEntry= false; else   targetBuffer.NewLine();
        targetBuffer._<NC>("  Scope::ThreadInner ");  storeThreadToScope( threadIt->first.second )._( ':' ).NewLine();
        cnt+= int( threadIt->second->Size() );
        maximumKeyLength= writeStoreMapHelper( *threadIt->second, "    " );
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
    for ( auto threadIt= store->threadStore.begin() ; threadIt != store->threadStore.end() ; ++threadIt )
        if( threadIt->first.first == false )
            for ( auto& it : threadIt->second ) {
                ++cnt;
                targetBuffer.InsertChars( ' ', indentSpaces );
                write(it, targetBuffer);
                targetBuffer._<NC>( NTab( 25, -1 ) )
                      ._<NC>( "Scope::ThreadOuter " );
                storeThreadToScope( threadIt->first.second ).NewLine();
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
    for ( auto threadIt= store->threadStore.begin() ; threadIt != store->threadStore.end() ; ++threadIt )
        if( threadIt->first.first == true )
            for ( auto& it : threadIt->second ) {
                ++cnt;
                targetBuffer.InsertChars( ' ', indentSpaces );
                write(it, targetBuffer);
                targetBuffer._<NC>( NTab( 25, -1 ) )
                      ._<NC>( "Scope::ThreadInner " );
                storeThreadToScope( threadIt->first.second ).NewLine();
            }
#endif
    return cnt;
}

//! @endcond
}}} // namespace [alib::lox::detail]
