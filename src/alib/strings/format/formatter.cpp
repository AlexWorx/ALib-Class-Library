// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER_PYTHONSTYLE)
    #include "alib/strings/format/formatterpythonstyle.hpp"
#endif


#if !defined (_GLIBCXX_CMATH) && !defined (_CMATH_)
    #include <cmath>
#endif


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


namespace aworx { namespace lib { namespace strings { namespace format {

//! @cond NO_DOX

ALIB_REL_DBG(
  template<typename TChar> void FormatterBase<TChar>::Acquire() ,
  template<typename TChar> void FormatterBase<TChar>::Acquire( const NTString& file, int line, const NTString& func )
)
{
    // call parent's implementation
    #if ALIB_MODULE_CORE || ALIB_MODULE_CONFIGURATION
        ALIB_REL_DBG(    ThreadLock::Acquire();
                       , ThreadLock::Acquire(file,line,func);    )
    #elif ALIB_DEBUG
        (void) file;
        (void) line;
        (void) func;
    #endif

    // if this is the first (recursive) lock, we reset
    if( cntAcquirements == 1 )
        reset();
}

//! @endcond

template<typename TChar>
void FormatterBase<TChar>::formatLoop( AStringBase<TChar>& target, const Boxes&  args )
{
    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS

    // initialize formatters
    FormatterBase<TChar>* formatter= this;
    do
        formatter->initializeFormat();
    while( (formatter= formatter->Next) != nullptr );

    // loop over boxes
    size_t argIdx= 0;
    while ( argIdx < args.size() - 1 )
    {
        StringBase<TChar> formatString;
        const Box& actual= args[argIdx++];

        // any sort of string?
             if(   actual.IsArrayOf<character     >() )   formatString= actual.Unbox<StringBase<TChar>>();
        else if(   actual.IsArrayOf<complementChar>()
                || actual.IsArrayOf<strangeChar   >() )   formatString= mbsFormatString.Clear() << actual;

        //
        if( formatString.IsNotNull() )
        {
            // try each formatter unless one succeeds
            int  qtyConsume= 0;
            formatter= this;
            while(    ( qtyConsume= formatter->format( target, formatString, args, static_cast<int>( argIdx ) ) ) == 0
                   &&  (formatter= formatter->Next) != nullptr )
            {}

            // no formatter reacted?
            if( qtyConsume == 0 )
                // we just append the string...
                target.template _<false>( formatString );
            else
                // ...otherwise arguments were consumed
                argIdx+= static_cast<size_t>( qtyConsume );
        }

        // not a string: use ToString method, which might have been overwritten by the user.
        // (if not, a default implementation exists, writing the type name and the pointer address)
        else if( !actual.IsNull() )
            actual.Invoke<aworx::IApply<TChar>>( target );
    }

    // the last argument was not consumed?
    if ( argIdx==args.size() - 1 && !args.back().IsNull() )
        target.template _<false>( args.back() );
}


template<typename TChar>
void FormatterBase<TChar>::CloneSettings( FormatterBase<TChar>& reference )
{
    DefaultNumberFormat    .Set( &reference.DefaultNumberFormat     );
    AlternativeNumberFormat.Set( &reference.AlternativeNumberFormat );

    // if both sub-formatters are of std type, we do the same for them.
    if( Next &&  reference.Next )
        Next->CloneSettings( *reference.Next );
}

//! @cond NO_DOX

template void FormatterBase<char   >::Acquire( ALIB_DBG(const NTString&, int, const NTString&));
template void FormatterBase<char   >::CloneSettings(FormatterBase<char>&);
template void FormatterBase<char   >::initializeFormat();
template void FormatterBase<char   >::reset();
template void FormatterBase<char   >::formatLoop(NAString&,const Boxes&);

template void FormatterBase<wchar_t>::Acquire( ALIB_DBG(const NTString&, int, const NTString&));
template void FormatterBase<wchar_t>::CloneSettings(FormatterBase<wchar_t>&);
template void FormatterBase<wchar_t>::initializeFormat();
template void FormatterBase<wchar_t>::reset();
template void FormatterBase<wchar_t>::formatLoop(WAString&,const Boxes&);

//! @endcond

}}}} // namespace [aworx::lib::strings::format]
