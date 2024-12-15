// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/compatibility/std_strings_iostream.hpp"
#endif // !DOXYGEN

#include <algorithm>

#if ALIB_STRINGS


#if !DOXYGEN

namespace alib::strings::compatibility::std {

namespace {

int adjustNewLines(const NString& src, NAString& dest ) {
    constexpr bool  isRN= NEW_LINE.Length() ==  2;
    int             result= 0;
    integer         idx= 0;
    const NString*   search= &src;
    while ((idx=search->IndexOf('\n', idx)) != -1 ) {
        if constexpr (isRN ) {
            if (idx == 0 || search->CharAt(idx - 1) != '\r') {
                if (++result == 1) {
                    dest.Reset(search->Substring(0, idx))._('\r')._<NC>(search->Substring(idx));
                    search= &dest;
                }
                else
                    dest.InsertAt("\r", idx);
            }
            ++idx;
        }
        else { // not windows
            if (idx > 0 && search->CharAt(idx - 1) == '\r' ) {
                if (++result == 1) {
                    dest.Reset(search->Substring(0, idx-1))._(search->Substring(idx));
                    search= &dest;
                }
                else
                    dest.Delete(idx-1, 1);
            }
            else
                ++idx;
        }
    }
    return result;
}

int adjustNewLines(const WString& src, NAString& dest ) {
    dest.Reset(src);
    constexpr bool  isRN= NEW_LINE.Length() ==  2;
    int             result= 0;
    integer         idx= 0;
    while ((idx=dest.IndexOf('\n', idx)) != -1 ) {
        if constexpr (isRN)
            if (idx == 0 || dest.CharAt(idx - 1) != '\r')
                dest.InsertAt("\r", idx);
            
        if constexpr (!isRN)
            if (!isRN && idx > 0 && dest.CharAt(idx - 1) == '\r' )
                dest.Delete(idx-1, 1);

        ++idx;
    }
    return result;
}
} // anonymous namespace

integer         StringWriter::WriteAndGetWideLength( const NString& src )
{
    if( ostream == nullptr )
    {
        ALIB_WARNING( "STRINGS", "StringWriter::WriteAndGetWideLength: No output stream")
        return 0;
    }

    if (adjustNewLines(src, converter) > 0)
        ostream->write( converter.Buffer(), converter.Length() );
    else
        ostream->write( src.Buffer(), src.Length() );
    return src.WStringLength();
}

integer         StringWriter::WriteAndGetWideLength( const WString& src )
{
    if( ostream == nullptr )
    {
        ALIB_WARNING( "STRINGS", "StringWriter::WriteAndGetWideLength: No output stream")
        return 0;
    }

    adjustNewLines( src, converter );
    ostream->write( converter.Buffer(), converter.Length() );
    return src.Length();
}

void            StringWriter::Write( const NString& src )
{
    if( ostream == nullptr )
    {
        ALIB_WARNING( "STRINGS", "StringWriter::WriteAndGetWideLength: No output stream")
        return;
    }
    if (adjustNewLines(src, converter) > 0)
        ostream->write( converter.Buffer(), converter.Length() );
    else
        ostream->write( src.Buffer(), src.Length() );
}

void            StringWriter::WriteChars( const nchar fillChar, integer count )
{
    if( ostream == nullptr ) {
        ALIB_WARNING( "STRINGS", "StringWriter::WriteAndGetWideLength: No output stream")
        return;
    }

    characters::AlignedCharArray<nchar, 8*sizeof(void*)> alc( fillChar );
    while (count >= alc.Length() ) {
        ostream->write(alc.Buffer(), alc.Length());
        count-= alc.Length();
    }

    if (count > 0)
        ostream->write(alc.Buffer(), count);
}

void            StringWriter::Write( const WString& src )
{
    if( ostream == nullptr )
    {
        ALIB_WARNING( "STRINGS", "StringWriter::WriteAndGetWideLength: No output stream")
        return;
    }
    adjustNewLines( src, converter );
    ostream->write( converter.Buffer(), converter.Length() );
}
} // namespace [alib::strings::compatibility::std]

std::ostream& operator<<( std::ostream& stream, const alib::WString& string )
{
    alib::NString1K conv;
    alib::integer   maxConv= 1024 / static_cast<alib::integer>(MB_CUR_MAX);

    alib::integer startIdx= 0;
    while( startIdx < string.Length() )
    {
        alib::integer length= (std::min)( alib::integer(maxConv), string.Length() - startIdx);
        conv.Reset( string.Substring<alib::NC>(startIdx, length) );
        stream.write( conv.Buffer(), conv.Length() );
        startIdx+= length;
    }

    return stream;
}

std::wostream& operator<<( std::wostream& stream, const alib::NString& string )
{
    alib::strings::TLocalString<wchar_t, 256> conv;
    conv.DbgDisableBufferReplacementWarning();
    conv << string;
    stream.write( conv.Buffer(), conv.Length() );
    return stream;
}

template<typename TChar>
void alib::strings::T_Append<alib::strings::compatibility::std::TISReadLine<TChar>, TChar, alib::lang::HeapAllocator>::operator()(
         TAString<TChar, alib::lang::HeapAllocator>&                 target,
         const compatibility::std::TISReadLine<TChar>&     reader              )
{
    // we are required to read from to the param object. So we cast to non-const. This is OK, as
    // the const specifier came through TMP.
    compatibility::std::TISReadLine<TChar>& param= const_cast<compatibility::std::TISReadLine<TChar>&>( reader );

    if ( param.TargetData == lang::CurrentData::Clear )
        target.Reset();
    alib::integer origLength= target.Length();

    // read loop
    while( !param.IStream->eof() )
    {
        // calc buffer size (if we hit the overall line width)
        // and check if we reached the limit per line
        alib::integer actReadSize= (std::min)( param.BufferSize,  param.MaxLineWidth - ( target.Length() - origLength) + 1  );
        if ( actReadSize < 2 )
            return;

        target.EnsureRemainingCapacity( actReadSize );

        // read
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        int64_t start= target.Length();
        param.IStream->getline( target.VBuffer() + start, actReadSize );
        std::streamsize gCount= param.IStream->gcount();
        std::streamsize count=  static_cast<std::streamsize>( characters::Length( target.Buffer() + start ) );

        bool lineComplete=  count + 1 == gCount;

        // something read?
        if ( count > 0 )
        {
            // be sure to not have a carriage return at the start
            if( *(target.Buffer() + start) == '\r' )
            {
                target.template Delete<NC>( static_cast<alib::integer>(start), 1 );
                --count;
            }

            // be sure to not have a carriage return at the end
            start+= count;
            if( *(target.Buffer() + start -1 ) == '\r' )
                --start;

            target.SetLength( static_cast<alib::integer>(start) );

            // if we are at the end of the file (without delimiter) we stop now
            if ( param.IStream->eof() )
            {
                param.IsEOF= true;
                return;
            }
        }
        ALIB_WARNINGS_RESTORE


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
            ALIB_ERROR( "STRINGS", "Unknown Error Reading File. Maybe method implemented incomplete?" )
            break;
        }

        // anything else to add here? I guess not! But you never know with this strange
        // iostream classes!

        //...

        // This happens if \0 is in the file
        ALIB_ERROR( "STRINGS", "Unknown Error Reading File. Probably not a text file." )
        break;
    }

    param.IsEOF= true;
}


// instantiations of T_Append::operator() for char and wchar_t versions of stream reader class
template void alib::strings::T_Append<alib::strings::compatibility::std::TISReadLine<char   >, char   , alib::lang::HeapAllocator>::operator()( TAString<char   , alib::lang::HeapAllocator>& target, const compatibility::std::TISReadLine<char   >& reader );
template void alib::strings::T_Append<alib::strings::compatibility::std::TISReadLine<wchar_t>, wchar_t, alib::lang::HeapAllocator>::operator()( TAString<wchar_t, alib::lang::HeapAllocator>& target, const compatibility::std::TISReadLine<wchar_t>& reader );

#endif  // DOXYGEN

#endif // ALIB_STRINGS

