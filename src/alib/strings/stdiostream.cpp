//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/strings/strings.prepro.hpp"
#include <algorithm>
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Strings.StdIOStream;
    import   ALib.Strings;
#else
#   include "ALib.Strings.H"
#   include "ALib.Strings.StdIOStream.H"
#endif
//========================================== Implementation ========================================

#if ALIB_STRINGS

#   include "ALib.Characters.Functions.H"

#if !DOXYGEN


//##################################### std::ostream& operator<< ###################################
std::ostream& operator<<( std::ostream& stream, const alib::WString& string ) {
    alib::NString4K conv;
    alib::integer   maxConv= 4 * 1024 / static_cast<alib::integer>(MB_CUR_MAX);

    alib::integer startIdx= 0;
    while( startIdx < string.Length() ) {
        alib::integer length= (std::min)( alib::integer(maxConv), string.Length() - startIdx);
        conv.Reset( string.Substring<alib::NC>(startIdx, length) );
        stream.write( conv.Buffer(), conv.Length() );
        startIdx+= length;
    }

    return stream;
}

std::wostream& operator<<( std::wostream& stream, const alib::NString& string ) {
    alib::strings::TLocalString<wchar_t, 4 * 1024> conv;
    conv.DbgDisableBufferReplacementWarning();
    conv << string;
    stream.write( conv.Buffer(), conv.Length() );
    return stream;
}

//########################################### TIStreamLine #########################################
template<typename TChar>
void alib::strings::AppendableTraits<alib::strings::compatibility::std::TIStreamLine<TChar>,
                                     TChar,
                                     alib::lang::HeapAllocator>::operator()(
         TAString<TChar, HeapAllocator>&                 target,
         const compatibility::std::TIStreamLine<TChar>&   constIsLine     ) {
    // we are required to read from to the param object. So we cast to non-const.
    // This is OK, as the const specifier came through template programming.
    compatibility::std::TIStreamLine<TChar>& isLine=
                               const_cast<compatibility::std::TIStreamLine<TChar>&>( constIsLine );

    if ( isLine.TargetData == lang::CurrentData::Clear )
        target.Reset();
    alib::integer origLength= target.Length();

    // read loop
    while( !isLine.IStream->eof() ) {
        // calc buffer size (if we hit the overall line width)
        // and check if we reached the limit per line
        integer actReadSize= (std::min)( isLine.BufferSize,
                                         isLine.MaxLineWidth - (target.Length() - origLength) + 1 );
        if ( actReadSize < 2 )
            return;

        target.EnsureRemainingCapacity( actReadSize );

        // read
        integer start= target.Length();
        isLine.IStream->getline( target.VBuffer() + start, actReadSize );
        std::streamsize gCount= isLine.IStream->gcount();
        std::streamsize count=  static_cast<std::streamsize>( characters::Length( target.Buffer() + start ) );

        bool lineComplete=  count + 1 == gCount;

        // something read?
        if ( count > 0 ) {
            // be sure to not have a carriage return at the start
            if( *(target.Buffer() + start) == '\r' ) {
                target.template Delete<NC>( static_cast<alib::integer>(start), 1 );
                --count;
            }

            // be sure to not have a carriage return at the end
            start+= count;
            if( *(target.Buffer() + start -1 ) == '\r' )
                --start;

            target.SetLength( static_cast<alib::integer>(start) );

            // if we are at the end of the file (without delimiter) we stop now
            if ( isLine.IStream->eof() ) {
                isLine.IsEOF= true;
                return;
        }   }

        // delim read
        if ( lineComplete )
            return;

        // buffer was not big enough
        if ( gCount == actReadSize -1) {
            if ( isLine.IStream->eof() )
                return;

            // otherwise, it should really have been the buffer size, so let's clear the bit
            // and continue with more buffer space
            ALIB_ASSERT( isLine.IStream->rdstate() == std::iostream::failbit, "STRINGS" )
            isLine.IStream->clear();
            continue;
        }

        // the eof just happened now
        if ( isLine.IStream->eof() )
            break;

        if ( isLine.IStream->rdstate() == std::iostream::failbit ) {
            ALIB_ERROR("STRINGS","Unknown Error Reading File. Maybe method implemented incomplete?")
            break;
        }

        // anything else to add here? I guess not! But you never know with this strange
        // iostream classes!

        //...

        // This happens if \0 is in the file
        ALIB_ERROR( "STRINGS", "Unknown Error Reading File. Probably not a text file." )
        break;
    }

    isLine.IsEOF= true;
}

// instantiations of AppendableTraits::operator() for char and wchar_t versions of TIStreamLine
template void alib::strings::AppendableTraits<alib::strings::compatibility::std::TIStreamLine<char   >, char   , alib::lang::HeapAllocator>::operator()( TAString<char   , alib::lang::HeapAllocator>& target, const compatibility::std::TIStreamLine<char   >& reader );
template void alib::strings::AppendableTraits<alib::strings::compatibility::std::TIStreamLine<wchar_t>, wchar_t, alib::lang::HeapAllocator>::operator()( TAString<wchar_t, alib::lang::HeapAllocator>& target, const compatibility::std::TIStreamLine<wchar_t>& reader );

#endif  // DOXYGEN

#endif // ALIB_STRINGS
