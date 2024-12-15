// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#if !DOXYGEN
#   include "alib/lang/format/formatterpythonstyle.hpp"
#   include <cmath>
#endif // !DOXYGEN


// For code compatibility with ALox Java/C++
// We have to use underscore as the start of the name and for this have to disable a compiler
// warning. But this is a local code (cpp file) anyhow.
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

using namespace alib::strings;

namespace alib::lang::format {

SPFormatter                 Formatter::Default;
#if ALIB_THREADS
    threads::RecursiveLock  Formatter::DefaultLock;
#endif



#if !DOXYGEN

template<>
Formatter& Formatter::formatLoop( AString& target, const BoxesMA&  args )
{ALIB_DCS
    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS

    // initialize formatters
    Formatter* formatter= this;
    do
        formatter->initializeFormat();
    while( (formatter= formatter->Next.Get()) != nullptr );

    // loop over boxes
    integer argIdx= 0;
    while ( argIdx < args.Size() - 1 )
    {
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
        while(    ( qtyConsume= formatter->format( target, formatString, args, static_cast<int>(argIdx ) ) )
                  == 0
               && (formatter= formatter->Next.Get()) != nullptr   )
        {}

        // no formatter reacted?
        if( qtyConsume == 0 )
            // we just append the string...
            target.template _<NC>( formatString );
        else
            // ...otherwise arguments were consumed
            argIdx+= size_t( qtyConsume );
    }

    // the last argument was not consumed?
    if ( argIdx==args.Size() - 1 && !args.back().template IsType<void>() )
        target.template _<NC>( args.back() );
    return *this;
}
#endif // !DOXYGEN



void Formatter::CloneSettings( Formatter& reference )
{
    DefaultNumberFormat    .Set( &reference.DefaultNumberFormat     );
    AlternativeNumberFormat.Set( &reference.AlternativeNumberFormat );

    // if both sub-formatters are of std type, we do the same for them.
    if( Next &&  reference.Next )
        Next->CloneSettings( *reference.Next );
}

#if !DOXYGEN
template<>
Formatter& Formatter::formatLoop( AString& target, const boxing::TBoxes<lang::HeapAllocator>&  args )
{
    boxes.clear();
    boxes.Add( args );
    formatLoop( target, boxes );
    return *this;
}

template<>
Formatter& Formatter::formatLoop( AString& target, const boxing::TBoxes<PoolAllocator>&  args )
{
    boxes.clear();
    boxes.Add( args );
    formatLoop( target, boxes );
    return *this;
}
#endif // !DOXYGEN

} // namespace [alib::lang::format]


