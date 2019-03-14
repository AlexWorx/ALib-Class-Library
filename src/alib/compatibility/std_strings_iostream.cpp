// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/compatibility/std_strings_iostream.hpp"


#if defined(_MSC_VER) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif

#if ALIB_MODULE_STRINGS

namespace aworx {


#if !ALIB_DOCUMENTATION_PARSER


std::ostream& operator<<( std::ostream& stream, const WString& string )
{
    NString1K conv;
    integer   maxConv= 1024 / static_cast<integer>(MB_CUR_MAX);

    integer startIdx= 0;
    while( startIdx < string.Length() )
    {
        integer length= (std::min)( integer(maxConv), string.Length() - startIdx);
        conv.Reset( string.Substring<false>(startIdx, length) );
        stream.write( conv.Buffer(), conv.Length() );
        startIdx+= length;
    };

    return stream;
}

std::wostream& operator<<( std::wostream& stream, const NString& string )
{
    lib::strings::TLocalString<wchar_t, 256> conv;
    conv.DbgDisableBufferReplacementWarning();
    conv << string;
    stream.write( conv.Buffer(), conv.Length() );
    return stream;
}

template<typename TChar>
void lib::strings::T_Append<lib::strings::compatibility::std::TISReadLine<TChar>, TChar>::operator()(
         TAString<TChar>&                                  target,
         const compatibility::std::TISReadLine<TChar>&     reader              )
{
    // we are required to read from to the param object. So we cast to non-const. This is OK, as
    // the const specifier came through TMP.
    compatibility::std::TISReadLine<TChar>& param= const_cast<compatibility::std::TISReadLine<TChar>&>( reader );

    if ( param.TargetData == CurrentData::Clear )
        target.Reset();
    integer origLength= target.Length();

    // read loop
    while( !param.IStream->eof() )
    {
        // calc buffer size (if we hit the overall line width)
        // and check if we reached the limit per line
        integer actReadSize= (std::min)( param.BufferSize,  param.MaxLineWidth - ( target.Length() - origLength) + 1  );
        if ( actReadSize < 2 )
            return;

        target.EnsureRemainingCapacity( actReadSize );

        // read
        int64_t start= target.Length();
        param.IStream->getline( target.VBuffer() + start, actReadSize );
        std::streamsize gCount= param.IStream->gcount();
        std::streamsize count=  static_cast<std::streamsize>( characters::CharArray<TChar>::Length( target.Buffer() + start ) );

        bool lineComplete=  count + 1 == gCount;

        // something read?
        if ( count > 0 )
        {
            // be sure to not have a carriage return at the start
            if( *(target.Buffer() + start) == '\r' )
            {
                target.template Delete<false>( static_cast<integer>(start), 1 );
                count--;
            }

            // be sure to not have a carriage return at the end
            start+= count;
            if( *(target.Buffer() + start -1 ) == '\r' )
                start--;

            target.SetLength( static_cast<integer>(start) );

            // if we are at the end of the file (without delimiter) we stop now
            if ( param.IStream->eof() )
            {
                param.IsEOF= true;
                return;
            }
        }


        // delim read
        if ( lineComplete )
            return;

        // buffer was not big enough
        if ( gCount == actReadSize -1)
        {
            if ( param.IStream->eof() )
                return;


            // otherwise, it should really have been the buffer size, so let's clear the bit
            // and continue with more buffer space
            ALIB_ASSERT( param.IStream->rdstate() == std::iostream::failbit )
            param.IStream->clear();
            continue;
        }

        // the eof just happened now
        if ( param.IStream->eof() )
            break;

        if ( param.IStream->rdstate() == std::iostream::failbit )
        {
            ALIB_ERROR( "Unknown Error Reading File. Maybe method implemented incomplete?" );
            break;
        }

        // anything else to add here? I guess not! But you never know with this strange
        // iostream classes!

        //...

        // This happens if \0 is in the file
        ALIB_ERROR( "Unknown Error Reading File. Probably not a text file." );
        break;
    }

    param.IsEOF= true;
}


// instantiations of T_Append::operator() for char and wchar_t versions of stream reader class
template void lib::strings::T_Append<lib::strings::compatibility::std::TISReadLine<char   >, char   >::operator()( TAString<char   >& target, const compatibility::std::TISReadLine<char   >& reader );
template void lib::strings::T_Append<lib::strings::compatibility::std::TISReadLine<wchar_t>, wchar_t>::operator()( TAString<wchar_t>& target, const compatibility::std::TISReadLine<wchar_t>& reader );

#endif  // ALIB_DOCUMENTATION_PARSER


} // namespace [aworx]

#endif // ALIB_MODULE_STRINGS


