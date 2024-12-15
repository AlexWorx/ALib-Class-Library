// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#if !defined(HPP_ALIB_STRINGS_TASTRING_INLINING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !DOXYGEN
#   include "alib/strings/localstring.hpp"
#   include "alib/strings/cstring.hpp"
#   include "alib/strings/format.inl"
#   include "alib/strings/detail/numberconversion.hpp"
#   include "alib/lang/dbgtypedemangler.hpp"
#   if ALIB_THREADS
#       include "alib/threads/thread.hpp"
#   elif ALIB_EXT_LIB_THREADS_AVAILABLE
#       include <thread>
#   endif
#endif // !DOXYGEN

#include <algorithm>


namespace alib {  namespace strings {

/// \attention
///   This is a non-existing namespace! It is exclusively defined for the
///   \https{documentation parser,www.doxygen.nl}.
///
/// In this <b>"documentation namespace"</b>, you will find specializations of functor
/// \alib{strings,T_Append} which in reality are implemented in parent
/// namespace \ref alib::strings (as required by C++ language syntax).<br>
/// The rationale for tricking the documentation to this pseude-namespace, is to twofold:
/// On the one hand to keep namespace \b %alib::strings clean and on the other to have
/// an overview of all specializations in one place.
 namespace APPENDABLES {}


// ####################################################################################################
// AString::_dbgCheck()
// ####################################################################################################
//! @cond NO_DOX
#if ALIB_DEBUG_STRINGS

template<typename TChar, typename TAllocator>
void TAString<TChar,TAllocator>::dbgCheck() const
{
    base::dbgCheck();

    integer cap= Capacity();

    ALIB_ASSERT_ERROR( debugLastAllocRequest == 0
                       ||  base::length <= debugLastAllocRequest, "STRINGS",
                       "Error: Previous allocation request was too short"         )

    ALIB_ASSERT_ERROR( base::length <= cap, "STRINGS",
                       "Error: Length greater than allocation size"               )

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    if( base::buffer && HasInternalBuffer() )
    {
        for (integer i= -16 ; i < 0 ; ++i)
            if ( base::buffer[i] != 2 )
            {
                ALIB_ERROR( "STRINGS", "Magic byte not found at start of buffer." )
                break;
            }
        for (integer i= 1 ; i <= 16 ; ++i)
            if ( base::buffer[ cap + i] != 3 )
            {
                ALIB_ERROR( "STRINGS", "Magic byte not found at end of buffer." )
                break;
            }
    }
    ALIB_WARNINGS_RESTORE
}

#endif
//! @endcond


// ####################################################################################################
// Allocation
// ####################################################################################################
template<typename TChar, typename TAllocator>
void TAString<TChar, TAllocator>::GrowBufferAtLeastBy( integer minimumGrowth )
{
    integer actCapacity= Capacity();

    ALIB_ASSERT_WARNING (  base::length + minimumGrowth > actCapacity,
                           "STRINGS", "Unnecessary invocation of Grow()" )

    // first allocation? Go with given growth as size
    if (actCapacity == 0 )
    {
        SetBuffer( minimumGrowth > 15 ? minimumGrowth : 15 );
        #if ALIB_DEBUG_STRINGS
        debugLastAllocRequest= minimumGrowth;
        #endif

        return;
    }

    // calc new size: in general grow by 50%
    integer newCapacity= actCapacity + (actCapacity / 2);
    if ( newCapacity < base::length + minimumGrowth )
        newCapacity+= minimumGrowth;

    if ( newCapacity < 15 )
        newCapacity= 15;

    SetBuffer( newCapacity );
    #if ALIB_DEBUG_STRINGS
    debugLastAllocRequest= actCapacity + minimumGrowth;
    #endif
}

template<typename TChar, typename TAllocator>
void TAString<TChar, TAllocator>::SetBuffer( integer newCapacity )
{
    #if ALIB_DEBUG_STRINGS
       ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
       ALIB_STRING_DBG_CHK(this)
        if(capacity > 0)
            allocBase::GetAllocator().dbgCheckMemory( base::vbuffer - 16 ,
                                                      sizeof(TChar) * (size_t(capacity + 1) + 32));
       ALIB_WARNINGS_RESTORE
    #endif


    ALIB_ASSERT( newCapacity >= 0 )

    // do nothing if life-cycle is managed by us and same size,
    if ( capacity >= 0 && capacity == newCapacity )
        return;

    #if ALIB_DEBUG_STRINGS
        debugLastAllocRequest= newCapacity;
    #endif

    // set uninitialized (and return)
    if ( newCapacity == 0 )
    {
        ALIB_ASSERT_WARNING( !dbgWarnWhenExternalBufferIsReplaced || capacity >= 0, "STRINGS",
            "AString::SetBuffer(): removing an external buffer (setting string nulled). "
            "This may not be wanted."  )

        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        if ( capacity > 0 )
            allocBase::GetAllocator().free( base::vbuffer
                                                                    #if ALIB_DEBUG_STRINGS
                                                                            - 16
                                                                    #endif
                                           , size_t(capacity + 1) * sizeof(TChar)
                                                                    #if ALIB_DEBUG_STRINGS
                                                                            + 32 * sizeof(TChar)
                                                                    #endif
                                                                                               );

        capacity=
        base::length=     0;
        base::buffer=     nullptr;
        return;
        ALIB_WARNINGS_RESTORE
    }

    #if ALIB_DEBUG
        if( dbgWarnWhenExternalBufferIsReplaced && capacity < 0)
            ALIB_WARNING( "STRINGS",
            "AString::SetBuffer(): replacing an external buffer by an internally managed one. "
            "This may not be wanted: ", reinterpret_cast<const char*>(Terminate())   )
    #endif

    // extend or shrink an existing buffer (and return)
    if( capacity > 0 )
    {
        size_t allocSize= size_t(newCapacity  + 1) * sizeof(TChar);
        #if !ALIB_DEBUG_STRINGS
            base::buffer= static_cast<TChar*>(
                  allocBase::GetAllocator().reallocate( base::vbuffer,
                                                        size_t(capacity + 1) * sizeof(TChar),
                                                        allocSize,
                                                        alignof(TChar)                        )   );
            newCapacity=   integer(allocSize / sizeof(TChar)) - 1;
            allocBase::GetAllocator().dbgAcknowledgeIncreasedAllocSize( base::vbuffer, allocSize );
        #else
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            // add 16 characters of padding at start/end
            allocSize+= 32 * sizeof(TChar);
            base::buffer= static_cast<TChar*>(
                allocBase::GetAllocator().reallocate( base::vbuffer - 16,
                                                      size_t(capacity + 1 + 32) * sizeof(TChar),
                                                      allocSize,
                                                      alignof(TChar)                       ) ) + 16;
            newCapacity=   integer(allocSize / sizeof(TChar)) - 32 -1;

            // write '\3' to end ('\0'= termination byte, '\1'= untermination byte )
            characters::Fill( base::vbuffer + newCapacity + 1 ,  16, TChar('\3') );
            allocBase::GetAllocator().dbgAcknowledgeIncreasedAllocSize( base::vbuffer - 16, allocSize );
            ALIB_WARNINGS_RESTORE
        #endif

        capacity=   newCapacity;
        if ( base::length > capacity )
            base::length= capacity;

        return;
    }

    // create new Buffer
    size_t allocSize= size_t(newCapacity +1) * sizeof(TChar);
    #if !ALIB_DEBUG_STRINGS
        TChar* newBuffer= static_cast<TChar*>( allocBase::GetAllocator().allocate( allocSize, alignof(TChar))  );
        newCapacity=   integer(allocSize / sizeof(TChar)) - 1;
        allocBase::GetAllocator().dbgAcknowledgeIncreasedAllocSize( newBuffer, allocSize );
    #else
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        // add 16 characters of padding at start/end
        allocSize+= 32 * sizeof(TChar);
        TChar* newBuffer= static_cast<TChar*>( allocBase::GetAllocator().allocate( allocSize, alignof(TChar)) ) + 16;
        newCapacity=   integer(allocSize / sizeof(TChar)) - 32 - 1;

        // write '\2' to start, '\3' to end ('\0'= termination byte, '\1'= untermination byte )
        characters::Fill( newBuffer                    - 16, 16 , TChar('\2') );
        characters::Fill( newBuffer + newCapacity + 1      , 16 , TChar('\3') );
        allocBase::GetAllocator().dbgAcknowledgeIncreasedAllocSize( newBuffer - 16, allocSize );
        ALIB_WARNINGS_RESTORE
    #endif

    // if we had a buffer before
    if ( capacity != 0 )
    {
        // copy data and delete old buffer
        characters::Copy( base::buffer, (std::min)( base::length + 1, newCapacity + 1),
                                            newBuffer );
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        if ( capacity > 0 )
            allocBase::GetAllocator().free( base::vbuffer
                                                            #if ALIB_DEBUG_STRINGS
                                                                    - 16
                                                            #endif
                                            , size_t(capacity + 1) * sizeof(TChar)
                                                            #if ALIB_DEBUG_STRINGS
                                                                    + 32* sizeof(TChar)
                                                            #endif
                                                                    );
        ALIB_WARNINGS_RESTORE
    }
    else
    {
        ALIB_ASSERT( base::length == 0 )
    }

    // set new Buffer and adjust length
    base::buffer=     newBuffer;
    capacity= newCapacity;
    if ( base::length > capacity )
        base::length= capacity;
}


template<typename TChar, typename TAllocator>
void TAString<TChar, TAllocator>::SetBuffer( TChar* extBuffer, integer extBufferSize, integer extLength,
                                             lang::Responsibility responsibility  )
{
    ALIB_ASSERT_ERROR(    !(extBufferSize == 0 && extBuffer != nullptr)
                       && !(extBufferSize != 0 && extBuffer == nullptr) , "STRINGS",
        "AString::SetBuffer(): Given buffer is nullptr while given alloc size is not 0 (or vice versa)" )

    // delete any existing
    if ( capacity > 0 )
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        allocBase::GetAllocator().free( base::vbuffer
                                                            #if ALIB_DEBUG_STRINGS
                                                                    - 16
                                                            #endif
                                        , size_t(capacity + 1) * sizeof(TChar)
                                                            #if ALIB_DEBUG_STRINGS
                                                                    + 32* sizeof(TChar)
                                                            #endif
                                                                                    );
        ALIB_WARNINGS_RESTORE


    // too small? treat as if a nullptr was given.
    if ( extBufferSize < 1 )
    {
        ALIB_ERROR( "STRINGS", "allocation size < 1" )
        extBuffer= nullptr;
    }

    // null buffer?
    if ( (base::buffer= extBuffer) == nullptr )
    {
        #if ALIB_DEBUG_STRINGS
            debugLastAllocRequest=
        #endif
        capacity=
        base::length=       0;
        return;
    }


    if ( extLength >= extBufferSize  )
    {
        ALIB_ERROR( "STRINGS", "ext length >= ext allocation size" )
        extLength= extBufferSize -1;
    }


    // save given buffer
    --extBufferSize;     // we count one less
    capacity=   responsibility==lang::Responsibility::Transfer ?  extBufferSize
                                                               : -extBufferSize;
    #if ALIB_DEBUG_STRINGS
        debugLastAllocRequest= extBufferSize;
    #endif
    base::length=     extLength;
}



// #############################################################################################
// Trim
// #############################################################################################

template<typename TChar, typename TAllocator>
integer TAString<TChar, TAllocator>::TrimAt( integer idx, const TCString<TChar>& trimChars )
{
    if ( idx < 0 )
         return 0;
    if ( idx >= base::length )
         return base::length;

    integer regionStart=  base::template       LastIndexOfAny<lang::Inclusion::Exclude, NC>( trimChars, idx ) + 1;
    if (regionStart < 0 )
        regionStart= 0;

    integer regionEnd=    TCString<TChar>(this).template IndexOfAny    <lang::Inclusion::Exclude, NC>( trimChars, idx );
    if (regionEnd < 0 )
        regionEnd= base::length;

    integer regionLength= regionEnd - regionStart;
    if ( regionLength > 0 )
        Delete<NC>( regionStart, regionLength );

    return regionStart;
}

template<typename TChar, typename TAllocator>
TAString<TChar, TAllocator>& TAString<TChar, TAllocator>::Trim( const TCString<TChar>& trimChars )
{
    // check
    if (base::length == 0 || trimChars.IsEmpty() )
        return *this;

    // trim end
    integer idx= base::template LastIndexOfAny<lang::Inclusion::Exclude, NC>( trimChars, base::length - 1 ) + 1;
    if ( (base::length= idx) > 0 )
    {
        // trim front
        idx= TCString<TChar>(this).template IndexOfAny<lang::Inclusion::Exclude, NC>( trimChars );
        if ( idx > 0 )
            Delete<NC>( 0, idx );
    }

    return *this;
}


// #################################################################################################
//  Replace()
// #################################################################################################
template<typename TChar, typename TAllocator>
integer TAString<TChar, TAllocator>::SearchAndReplace(  TChar      needle,
                                                        TChar      replacement,
                                                        integer    startIdx,
                                                        integer    endIdx        )
{
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    ALIB_STRING_DBG_CHK(this)
         if ( startIdx < 0  )                       startIdx= 0;
    else if ( startIdx >= base::length )  return 0;
         if ( endIdx   >  base::length  ) endIdx= base::length;
         if ( startIdx >= endIdx )                  return 0;

    // replacement loop
    TCString<TChar> thisAsCString= this;
    integer cntReplacements=    0;
    do
    {
        startIdx= thisAsCString.template IndexOfOrLength<NC>( needle, startIdx );
        if ( startIdx == base::length  )
            break;
        base::vbuffer[ startIdx ]= replacement;
        ++cntReplacements;
    }
    while(  ++startIdx < endIdx) ;
    return cntReplacements;
    ALIB_WARNINGS_RESTORE
}


template<typename TChar, typename TAllocator>
integer TAString<TChar, TAllocator>::SearchAndReplace( const TString<TChar>&  needle,
                                                       const TString<TChar>&  replacement,
                                                       integer                startIdx,
                                                       integer                maxReplacements,
                                                       lang::Case             sensitivity,
                                                       integer                endIdx        )
{
    ALIB_STRING_DBG_CHK(this)

    // check null arguments
    if ( needle.IsEmpty() )        return 0;
    endIdx= (std::min) (endIdx, base::length - needle.Length() + 1 );
    if ( startIdx >= endIdx )      return 0;

    integer nLen=    needle.Length();
    integer rLen=    replacement.Length();
    integer lenDiff= rLen - nLen;

    // replacement loop
    integer cntReplacements=    0;
    while ( cntReplacements < maxReplacements && startIdx < endIdx)
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        // search  next occurrence
        integer    idx= sensitivity == lang::Case::Sensitive
                        ? TString<TChar>(*this).template IndexOf<NC, lang::Case::Sensitive>( needle, startIdx, endIdx )
                        : TString<TChar>(*this).template IndexOf<NC, lang::Case::Ignore   >( needle, startIdx, endIdx );
        if ( idx < 0 )
            break;

        // copy rest up or down
        if ( lenDiff != 0 )
        {
            if ( lenDiff > 0 )
                EnsureRemainingCapacity( lenDiff );
            characters::Move( base::vbuffer + idx + nLen,
                                                base::length  - idx - nLen,
                                                base::vbuffer + idx + nLen + lenDiff );
            base::length+= lenDiff;
            endIdx+= lenDiff;
        }

        // fill replacement in
        if( rLen > 0 )
            characters::Copy( replacement.Buffer(), rLen, base::vbuffer + idx );

        // set start index to first character behind current replacement
        startIdx= idx + rLen;

        // next
        ++cntReplacements;
        ALIB_WARNINGS_RESTORE
    }

    // that's it
    return cntReplacements;
}

// #################################################################################################
// T_Append<Integrals>
// #################################################################################################

template<typename TChar, typename TAllocator>
void T_Append<int64_t,TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                     int64_t value )
{
    target.EnsureRemainingCapacity(28);//   20 digits, grouping symbol, sign and what have you
    integer length=      target.Length();
    length=  detail::WriteDecSigned ( value, target.VBuffer(), length, 0, TNumberFormat<TChar>::Computational );
    target.SetLength( length );
}

template<typename TChar, typename TAllocator>
void T_Append<uint64_t,TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                      uint64_t value )
{
    target.EnsureRemainingCapacity(28);//   20 digits, grouping symbol, sign and what have you
    integer length=      target.Length();
    length=  detail::WriteDecUnsigned ( value, target.VBuffer(), length, 0, TNumberFormat<TChar>::Computational );
    target.SetLength( length );
}


template<typename TChar, typename TAllocator>
void T_Append<double,TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                    double value )
{
    target.EnsureRemainingCapacity(48);  // float: 2x15 + '.' + ',' + sign + fear
    integer length=      target.Length();
    length=  detail::WriteFloat( value, target.VBuffer(), length, 0, TNumberFormat<TChar>::Computational );
    target.SetLength( length );
}

// #################################################################################################
// T_Append<Format>
// #################################################################################################

// #################################################################################################
// TFormat::Tab()
// #################################################################################################
template<typename TChar, typename TAllocator>
void T_Append<typename TFormat<TChar>::Tab, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                           const typename TFormat<TChar>::Tab& tab)
{
    integer reference= tab.reference;
    if (reference < 0 )
    {
        // search backwards
        reference= target.template LastIndexOfAny<lang::Inclusion::Include>( TT_CStringConstants<TChar>::NewLine(),
                                                                             target.Length() -1 );
        if ( reference < 0 )
            reference= 0;
        else
        {
            // if new line has more than one character (windows) we have to now search the first
            // character that is not in newline
            reference= target.template IndexOfAny<lang::Inclusion::Exclude, NC>( TT_CStringConstants<TChar>::NewLine(), reference );
            if (reference < 0 )
                reference= target.Length();

        }
    }
    integer length=   target.Length();
    integer qtyChars= tab.minPad > 0 ? tab.minPad : 0;

    if ( tab.tabSize > 1 )
        qtyChars+= (tab.tabSize - ( (length + qtyChars - reference) % tab.tabSize ) ) % tab.tabSize;

    if ( qtyChars > 0 )
        target.template InsertChars<NC>( tab.tabChar, qtyChars );
}


// #################################################################################################
// TFormat::Field()
// #################################################################################################
template<typename TChar, typename TAllocator>
void T_Append<typename TFormat<TChar>::Field, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                             const typename TFormat<TChar>::Field& field)
{

#if ALIB_BOXING
    TString<TChar> theContent;

    // buffer used for conversion (if none string)
    TLocalString<TChar, 256> noneStringArgBuf;
    noneStringArgBuf.DbgDisableBufferReplacementWarning();

    // string-type box given?
    if( !field.theContent.template IsType<void>() && field.theContent.template IsType<TString<TChar>>() )
        theContent= field.theContent.template Unbox<TString<TChar>>();
    else
    {
        // write box into local buffer
        noneStringArgBuf << field.theContent;
        theContent= noneStringArgBuf;
    }

#else
    TString<TChar> theContent= field.theContent;
#endif

    integer padSize=  field.fieldWidth
                    - theContent.WStringLength();

    // check pad field.width
    if (padSize <= 0 || field.alignment == lang::Alignment::Left )
    {
                                target.template _          <NC>( theContent );
        if (padSize > 0 )       target.template InsertChars<NC>( field.padChar, padSize );
        return;
    }

    // align Right
    if ( field.alignment == lang::Alignment::Right )
    {
        if( padSize > 0 )
            target.template InsertChars<NC>( field.padChar, padSize );
        target.template Append<NC>( theContent );
        return;
    }

    // align Center
    integer leftPadding= padSize / 2;
    if( leftPadding > 0 )
        target.template InsertChars<NC> ( field.padChar, leftPadding  );
    target.template Append<NC> ( theContent );
    if( padSize > leftPadding ) target.template InsertChars<NC> ( field.padChar, padSize - leftPadding );
}

// #################################################################################################
// TFormat::Escape()
// #################################################################################################
template<typename TChar, typename TAllocator>
void T_Append<typename TFormat<TChar>::Escape, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                              const typename TFormat<TChar>::Escape& escape)
{
    if( target.AdjustRegion( const_cast<typename TFormat<TChar>::Escape&>(escape).startIdx,
                             const_cast<typename TFormat<TChar>::Escape&>(escape).length   ) )
        return;

    integer regionEnd= escape.startIdx + escape.length;

    //
    // To escape sequences
    //
    if (escape.pSwitch == lang::Switch::On)
    {
        for( integer idx= escape.startIdx; idx < regionEnd ; ++idx )
        {
            TChar c= target.CharAt(idx);

            TChar resultChar= '\0';
            switch(c)
            {
                case '\\' : resultChar= '\\'; break;
                case '\r' : resultChar= 'r' ; break;
                case '\n' : resultChar= 'n' ; break;
                case '\t' : resultChar= 't' ; break;
                case '\a' : resultChar= 'a' ; break;
                case '\b' : resultChar= 'b' ; break;
                case '\v' : resultChar= 'v' ; break;
                case '\f' : resultChar= 'f' ; break;
             // case '\e' : resultChar= 'e' ; break; Not C++ standard
                case '"'  : resultChar= '"' ; break;

                default   :                   break;
            }

            if( resultChar != '\0')
            {
                target.template InsertChars<NC>('\\', 1, idx);
                target[++idx]= resultChar;
                ++regionEnd;
            }
        }
    }

    //
    // Un-escape escape sequences
    //
    else
    {
        --regionEnd; // we can go 1 over it!
        for( integer idx= escape.startIdx; idx < regionEnd ; ++idx )
        {
            TChar c= target.CharAt(idx);
            if( c != '\\' )
                continue;

            c= target.CharAt(idx + 1);

            TChar resultChar= '\0';
            switch(c)
            {
                case '\\' : resultChar= '\\'; break;
                case 'r'  : resultChar= '\r' ; break;
                case 'n'  : resultChar= '\n' ; break;
                case 't'  : resultChar= '\t' ; break;
                case 'a'  : resultChar= '\a' ; break;
                case 'b'  : resultChar= '\b' ; break;
                case 'v'  : resultChar= '\v' ; break;
                case 'f'  : resultChar= '\f' ; break;
             // case 'e'  : resultChar= '\e' ; break; Not C++ standard
                case '"'  : resultChar= '"' ; break;

                default   :                   break;
            }

            if( resultChar != '\0')
            {
                target.Delete( idx, 1);
                target[idx]= resultChar;
                --regionEnd;
            }
        }
    }
}

// #################################################################################################
// TFormat Integers
// #################################################################################################
template<typename TChar, typename TAllocator>
void T_Append<TFormat<TChar>, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                             const TFormat<TChar>& fmt )
{
    const TNumberFormat<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &TNumberFormat<TChar>::Computational;

    target.EnsureRemainingCapacity( fmt.valueType== 3 ?  48  //float: 2x15 + '.' + ',' + sign + fear
                                                      :  28  //int:   20 digits, grouping symbol, sign and what have you
                                  );

    integer length=      target.Length();

    length=
    fmt.valueType == 1 ?  detail::WriteDecSigned  (                       fmt.v.value  , target.VBuffer(), length, fmt.width   , *nf  ) :
    fmt.valueType == 2 ?  detail::WriteDecUnsigned( static_cast<uint64_t>(fmt.v.value) , target.VBuffer(), length, fmt.width   , *nf  ) :
                          detail::WriteFloat      (                       fmt.v.fpValue, target.VBuffer(), length, fmt.width   , *nf  );

    target.SetLength( length );
}



template<typename TChar, typename TAllocator>
void T_Append<typename TFormat<TChar>::Bin, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                           const typename TFormat<TChar>::Bin& fmt )
{
    TNumberFormat<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &TNumberFormat<TChar>::Computational;

    target.EnsureRemainingCapacity( 80 );

    integer length=      target.Length();

    length= detail::WriteBin( fmt.theValue, target.VBuffer(), length,  fmt.theWidth, *nf );

    target.SetLength( length );
}

template<typename TChar, typename TAllocator>
void T_Append<typename TFormat<TChar>::Hex, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                           const typename TFormat<TChar>::Hex& fmt )
{
    TNumberFormat<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &TNumberFormat<TChar>::Computational;

    target.EnsureRemainingCapacity( 25 );

    integer length=      target.Length();

    length= detail::WriteHex( fmt.theValue, target.VBuffer(), length,  fmt.theWidth, *nf );

    target.SetLength( length );
}

template<typename TChar, typename TAllocator>
void T_Append<typename TFormat<TChar>::Oct, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                           const typename TFormat<TChar>::Oct& fmt )
{
    TNumberFormat<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &TNumberFormat<TChar>::Computational;

    target.EnsureRemainingCapacity( 30 );

    integer length=      target.Length();

    length= detail::WriteOct( fmt.theValue, target.VBuffer(), length,  fmt.theWidth, *nf );

    target.SetLength( length );
}

template<typename TChar, typename TAllocator>
void T_Append<typename TFormat<TChar>::Fill, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                   const typename TFormat<TChar>::Fill& fmt )
{
    if (fmt.count <= 0)
        return;
    target.EnsureRemainingCapacity( fmt.count );
    characters::Fill( target.VBuffer(), fmt.count, fmt.fillChar );
    target.SetLength( target.Length() + fmt.count );
}

// #################################################################################################
// ALIB_DEBUG: std::type_info, lang::CallerInfo
// #################################################################################################
#if ALIB_DEBUG
template<typename TChar, typename TAllocator>
void T_Append<std::type_info, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                             const std::type_info& type )
{
    NString2K buf;
    lang::DbgTypeDemangler( type ).GetShort(buf);
    target << buf;
}
#endif

template<typename TChar, typename TAllocator>
void T_Append<std::thread::id, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                              const std::thread::id& threadID )
{
    #if ALIB_EXT_LIB_THREADS_AVAILABLE
        size_t nativeIDWidth;
        uint64_t nativeID;
             if constexpr ( sizeof(std::thread::id) == sizeof(uint16_t) )  { nativeID= *reinterpret_cast<const uint16_t*>(&threadID); nativeIDWidth= 4; }
        else if constexpr ( sizeof(std::thread::id) == sizeof(uint32_t) )  { nativeID= *reinterpret_cast<const uint32_t*>(&threadID); nativeIDWidth= 8; }
        else                                                               { nativeID= *reinterpret_cast<const uint64_t*>(&threadID); nativeIDWidth=16; }
        if (lang::IsNotNull(threadID) )
        #if ALIB_THREADS
        {
            integer length= target.Length();
            Thread* thread= Thread::Get(threadID);
            target << thread->GetName()
                   << '(' << thread->GetID()
                   << ",0x" << typename TFormat<TChar>::Hex(nativeID, int(nativeIDWidth)) <<')';
            target.InsertChars( ' ', 30 + length - target.Length() );
        }
        #else
            target << "TID=0x" << typename TFormat<TChar>::Hex(nativeID, int(nativeIDWidth) );
        #endif
        else
            target << "<NULL>";

    #endif
}

template<typename TChar, typename TAllocator>
void T_Append<lang::CallerInfo, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                               const lang::CallerInfo& ci )
{
    NString2K nbuf;
    nbuf << "[@ ";
        if (ci.File)    nbuf << ci.File << ':' << ci.Line;
        else            nbuf << "<NULL>";
    #if ALIB_DEBUG
    nbuf  << " from '";
    if (ci.TypeInfo)   lang::DbgTypeDemangler(*ci.TypeInfo).GetShort(nbuf) << "::";
    if(ci.Func) nbuf << ci.Func << "()";
    else        nbuf << "<NULL>";
    nbuf << '\'';
    #endif
    #if ALIB_EXT_LIB_THREADS_AVAILABLE
       nbuf << " by '"<< ci.ThreadID << '\'';
    #endif
    nbuf << ']';

    target << nbuf;
}

}} // namespace [alib::strings]

