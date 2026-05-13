ALIB_BOXING_VTABLE_DEFINE( alib::format::FMTExceptions    , vt_system_fmtexceptions )

using namespace alib::strings;

/// This is the reference documentation module \alib_format.<br>
/// Extensive documentation for this namespace is provided with the
/// #"alib_mod_format;Programmer's Manual" of that module.
namespace alib::format {

SPFormatter                 Formatter::DEFAULT;
#if !ALIB_SINGLE_THREADED
    threads::RecursiveLock  Formatter::DEFAULT_LOCK;
#endif



#if !DOXYGEN

template<>
Formatter& Formatter::formatLoop( AString& target, const BoxesMA&  args )                  {ALIB_DCS
    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS

    // initialize formatters
    Formatter* formatter= this;
    do
        formatter->initializeFormat();
    while( (formatter= formatter->Next.Get()) != nullptr );

    // loop over boxes
    integer argIdx= 0;
    while ( argIdx < args.Size() - 1 ) {
        String formatString;
        const Box& actual= args[size_t(argIdx++)];
        if( actual.IsType<void>() )
            continue;

        // Either this is a string, or we convert the box to a string. This fetches anything
        // that is string like, including string-types that are encapsulated in BoxedAs, etc.
        if( actual.IsArrayOf<character>() )
            formatString= actual.Unbox<String>();
        else
            formatString= formatStringBuffer.Reset( actual );

        if( formatString.IsEmpty() )
            continue;

        // try each formatter unless one succeeds
        integer  qtyConsume= 0;
        formatter= this;
        while(    ( qtyConsume= formatter->format( target, formatString, args, int(argIdx ) ) )
                  == 0
               && (formatter= formatter->Next.Get()) != nullptr   )
        {}

        // no formatter reacted?
        if( qtyConsume == 0 )
            // we append the unescaped string...
            Formatter::writeStringPortion(target, formatString);
        else
            // ...otherwise arguments were consumed
            argIdx+= size_t( qtyConsume );
    }

    // the last argument was not consumed?
    if ( argIdx==args.Size() - 1 && !args.back().template IsType<void>() ){
        String256 lastArg; lastArg.DbgDisableBufferReplacementWarning();
        lastArg._(args.back());
        Formatter::writeStringPortion(target, lastArg);
    }
    return *this;
}
#endif // !DOXYGEN



void Formatter::CloneSettings( Formatter& reference ) {
    DefaultNumberFormat    .Set( &reference.DefaultNumberFormat     );
    AlternativeNumberFormat.Set( &reference.AlternativeNumberFormat );

    // if both sub-formatters are of std type, we do the same for them.
    if( Next &&  reference.Next )
        Next->CloneSettings( *reference.Next );
}

#if !DOXYGEN
template<>
Formatter& Formatter::formatLoop(AString& target, const boxing::TBoxes<lang::HeapAllocator>& args) {
    boxes.clear();
    boxes.Add( args );
    formatLoop( target, boxes );
    return *this;
}

template<>
Formatter& Formatter::formatLoop(AString& target, const boxing::TBoxes<PoolAllocator>&       args) {
    boxes.clear();
    boxes.Add( args );
    formatLoop( target, boxes );
    return *this;
}
#endif // !DOXYGEN

void    Formatter::writeStringPortion( AString& target, const String& escaped ) {
    integer length= escaped.Length();
    if( length == 0)
        return;

    target.EnsureRemainingCapacity( length );
    auto* src = escaped.Buffer();
    auto* dest= target.VBuffer() + target.Length();

    character c1;
    character c2= *src;
    while( length > 1 ) {
        c1= c2;
        c2= *++src;
        if( c1 == '\\' ) {  switch(c2) {
                case  'r': c1= '\r' ; break;
                case  'n': c1= '\n' ; break;
                case  't': c1= '\t' ; break;
                case  'a': c1= '\a' ; break;
                case  'b': c1= '\b' ; break;
                case  'v': c1= '\v' ; break;
                case  'f': c1= '\f' ; break;
                case  '"': c1= '"'  ; break;
                case '\\': c1= '\\' ; break;
                default:   c1= '?'  ; break;
            }
            c2= *++src; --length;
        }
        *dest++= c1; --length;
    }

    // copy the last character and adjust target string length:
    // Note: length usually is 1. Only if last character is an escape sequence, it is 0.
    if( length == 1)
        *dest= *src;

    target.SetLength( dest - target.VBuffer() + length);
}

} // namespace [alib::format]
