// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_TEXT_FORMATTER_PYTHONSTYLE)
#   include "alib/text/formatterpythonstyle.hpp"
#endif

#if !defined (_GLIBCXX_CMATH) && !defined (_CMATH_)
#   include <cmath>
#endif
#endif // !defined(ALIB_DOX)


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

using namespace aworx::lib::strings;

namespace aworx { namespace lib { namespace text {

SPFormatter     Formatter::defaultFormatter;

#if !defined(ALIB_DOX)
Boxes& Formatter::Acquire( ALIB_DBG( const NCString& file, int line, const NCString& func ) )
{
    // call parent's implementation
    #if ALIB_THREADS
        ALIB_REL_DBG(    ThreadLock::Acquire();
                       , ThreadLock::Acquire(file,line,func);    )

    #else
        ++cntAcquirements;
    #   if ALIB_DEBUG
            (void) file;
            (void) line;
            (void) func;
            DbgCheckSingleThreaded();
    #   endif
    #endif

    // if this is the first (recursive) lock, we reset
    if( cntAcquirements == 1 )
        reset();

    if( Next )
        Next->Acquire( ALIB_DBG( file, line, func ) );

    boxes.clear();
    return boxes;
}
#endif //!defined(ALIB_DOX)



void Formatter::Release()
{
    #if ALIB_THREADS
        ThreadLock::Release();
    #else
        --cntAcquirements;
    #endif

    if( Next )
        Next->Release();
}


Formatter& Formatter::formatLoop( AString& target, const Boxes&  args )
{
    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS

    // initialize formatters
    Formatter* formatter= this;
    do
        formatter->initializeFormat();
    while( (formatter= formatter->Next.get()) != nullptr );

    // loop over boxes
    integer argIdx= 0;
    while ( argIdx < args.Size() - 1 )
    {
        String formatString;
        const Box& actual= args[static_cast<size_t>(argIdx++)];
        if( actual.IsType<void>() )
            continue;

        // Either this is a string, or we convert the box to a string. This fetches anything
        // that is string like, including string types that are encapsulated in BoxedAs, etc.
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
               && (formatter= formatter->Next.get()) != nullptr   )
        {}

        // no formatter reacted?
        if( qtyConsume == 0 )
            // we just append the string...
            target.template _<false>( formatString );
        else
            // ...otherwise arguments were consumed
            argIdx+= static_cast<size_t>( qtyConsume );
    }

    // the last argument was not consumed?
    if ( argIdx==args.Size() - 1 && !args.back().IsType<void>() )
        target.template _<false>( args.back() );
    return *this;
}



void Formatter::CloneSettings( Formatter& reference )
{
    DefaultNumberFormat    .Set( &reference.DefaultNumberFormat     );
    AlternativeNumberFormat.Set( &reference.AlternativeNumberFormat );

    // if both sub-formatters are of std type, we do the same for them.
    if( Next &&  reference.Next )
        Next->CloneSettings( *reference.Next );
}


}}} // namespace [aworx::lib::text]
