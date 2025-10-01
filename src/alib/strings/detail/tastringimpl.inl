// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#if !defined(ALIB_STRINGS_TASTRING_INSTANTIATION)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !DOXYGEN
#endif // !DOXYGEN



namespace alib {  namespace strings {

/// \attention
///   This is a non-existing namespace! It is exclusively defined for the
///   \https{documentation parser,www.doxygen.nl}.
///
/// In this <b>"documentation namespace"</b>, you will find specializations of functor
/// \alib{strings,AppendableTraits} which in reality are implemented in parent
/// namespace \ref alib::strings (as required by C++ language syntax).<br>
/// The rationale for tricking the documentation to this pseude-namespace, is to twofold:
/// On the one hand to keep namespace \b %alib::strings clean and on the other to have
/// an overview of all specializations in one place.
namespace APPENDABLES {}


// #################################################################################################
// AString::_dbgCheck()
// #################################################################################################
//! @cond NO_DOX
#if ALIB_DEBUG_STRINGS

template<typename TChar, typename TAllocator>
requires alib::lang::IsAllocator<TAllocator>
void TAString<TChar,TAllocator>::dbgCheck() const
{
    base::dbgCheck();

    integer cap= Capacity();

    ALIB_ASSERT_ERROR( debugLastAllocRequest == 0
                       ||  base::length <= debugLastAllocRequest, "STRINGS",
       "Error: Previous allocation request was too short: {} < {} ",
       debugLastAllocRequest, base::length )

    ALIB_ASSERT_ERROR( base::length <= cap, "STRINGS",
                       "Error: Length greater than allocation size: {} > {}",
                       base::length, cap )

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
}

#endif
//! @endcond


// ####################################################################################################
// Allocation
// ####################################################################################################
template<typename TChar, typename TAllocator>
requires alib::lang::IsAllocator<TAllocator>
void TAString<TChar, TAllocator>::GrowBufferAtLeastBy( integer minimumGrowth )
{
    integer actCapacity= Capacity();

    ALIB_ASSERT_WARNING (  base::length + minimumGrowth > actCapacity, "STRINGS",
      "Unnecessary invocation of Grow(): {} <= {}", base::length + minimumGrowth, actCapacity )

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
requires alib::lang::IsAllocator<TAllocator>
void TAString<TChar, TAllocator>::SetBuffer( integer newCapacity )
{
    #if ALIB_DEBUG_STRINGS
       ALIB_STRING_DBG_CHK(this)
        if(capacity > 0)
            allocBase::GetAllocator().dbgCheckMemory( base::vbuffer - 16 ,
                                                      sizeof(TChar) * (size_t(capacity + 1) + 32));
    #endif


    ALIB_ASSERT( newCapacity >= 0, "STRINGS" )

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
    }

    #if ALIB_DEBUG
        if( dbgWarnWhenExternalBufferIsReplaced && capacity < 0)
            ALIB_WARNING( "STRINGS",
              "Replacing an external buffer by an internally managed one. This may not be wanted: ",
              *this  )
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
        // add 16 characters of padding at start/end
        allocSize+= 32 * sizeof(TChar);
        TChar* newBuffer= static_cast<TChar*>( allocBase::GetAllocator().allocate( allocSize, alignof(TChar)) ) + 16;
        newCapacity=   integer(allocSize / sizeof(TChar)) - 32 - 1;

        // write '\2' to start, '\3' to end ('\0'= termination byte, '\1'= untermination byte )
        characters::Fill( newBuffer                    - 16, 16 , TChar('\2') );
        characters::Fill( newBuffer + newCapacity + 1      , 16 , TChar('\3') );
        allocBase::GetAllocator().dbgAcknowledgeIncreasedAllocSize( newBuffer - 16, allocSize );
    #endif

    // if we had a buffer before
    if ( capacity != 0 )
    {
        // copy data and delete old buffer
        characters::Copy( base::buffer, (std::min)( base::length + 1, newCapacity + 1), newBuffer );

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
    }
    else
    {
        ALIB_ASSERT( base::length == 0, "STRINGS")
    }

    // set new Buffer and adjust length
    base::buffer=     newBuffer;
    capacity= newCapacity;
    if ( base::length > capacity )
        base::length= capacity;
}


template<typename TChar, typename TAllocator>
requires alib::lang::IsAllocator<TAllocator>
void TAString<TChar, TAllocator>::SetBuffer( TChar* extBuffer, integer extBufferSize, integer extLength,
                                             lang::Responsibility responsibility  )
{
    ALIB_ASSERT_ERROR(    !(extBufferSize == 0 && extBuffer != nullptr)
                       && !(extBufferSize != 0 && extBuffer == nullptr) , "STRINGS",
    "AString::SetBuffer(): Given buffer is nullptr while given alloc size is not 0 (or vice versa)")

    // delete any existing
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
        ALIB_ERROR( "STRINGS", "ext length {} >= ext allocation size {}", extLength, extBufferSize )
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
requires alib::lang::IsAllocator<TAllocator>
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
requires alib::lang::IsAllocator<TAllocator>
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
requires alib::lang::IsAllocator<TAllocator>
integer TAString<TChar, TAllocator>::SearchAndReplace(  TChar      needle,
                                                        TChar      replacement,
                                                        integer    startIdx,
                                                        integer    endIdx        )
{
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
}

template<typename TChar, typename TAllocator>
requires alib::lang::IsAllocator<TAllocator>
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
    }

    // that's it
    return cntReplacements;
}

// #################################################################################################
// AppendableTraits<Integrals>
// #################################################################################################

template<typename TChar, typename TAllocator>
void AppendableTraits<int64_t,TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                     int64_t value )
{
    target.EnsureRemainingCapacity(28);//   20 digits, grouping symbol, sign and what have you
    integer length=      target.Length();
    length=  detail::WriteDecSigned ( value, target.VBuffer(), length, 0, TNumberFormat<TChar>::Computational );
    target.SetLength( length );
}

template<typename TChar, typename TAllocator>
void AppendableTraits<uint64_t,TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                      uint64_t value )
{
    target.EnsureRemainingCapacity(28);//   20 digits, grouping symbol, sign and what have you
    integer length=      target.Length();
    length=  detail::WriteDecUnsigned ( value, target.VBuffer(), length, 0, TNumberFormat<TChar>::Computational );
    target.SetLength( length );
}


template<typename TChar, typename TAllocator>
void AppendableTraits<double,TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                    double value )
{
    target.EnsureRemainingCapacity(48);  // float: 2x15 + '.' + ',' + sign + fear
    integer length=      target.Length();
    length=  detail::WriteFloat( value, target.VBuffer(), length, 0, TNumberFormat<TChar>::Computational );
    target.SetLength( length );
}

// #################################################################################################
// AppendableTraits<Format>
// #################################################################################################

// #################################################################################################
// TTab()
// #################################################################################################
template<typename TChar, typename TAllocator>
void AppendableTraits<TTab<TChar>, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                           const TTab<TChar>& tab)
{
    integer reference= tab.reference;
    if (reference < 0 )
    {
        // search backwards
        reference= target.template LastIndexOfAny<lang::Inclusion::Include>( CStringConstantsTraits<TChar>::NewLine(),
                                                                             target.Length() -1 );
        if ( reference < 0 )
            reference= 0;
        else
        {
            // if new line has more than one character (windows) we have to now search the first
            // character that is not in newline
            reference= target.template IndexOfAny<lang::Inclusion::Exclude, NC>( CStringConstantsTraits<TChar>::NewLine(), reference );
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
// TField()
// #################################################################################################
#if !ALIB_BOXING
template<typename TChar, typename TAllocator>
void AppendableTraits< TField<TChar>, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                     const TField<TChar>& field)
{
    TString<TChar> theContent= field.theContent;

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
#endif

// #################################################################################################
// TEscape()
// #################################################################################################
template<typename TChar, typename TAllocator>
void AppendableTraits<TEscape<TChar>, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>&    target,
                                                                     const TEscape<TChar>& escape)
{
    if( target.AdjustRegion( const_cast<TEscape<TChar>&>(escape).startIdx,
                             const_cast<TEscape<TChar>&>(escape).length   ) )
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
// TDec Integers
// #################################################################################################
template<typename TChar, typename TAllocator>
void AppendableTraits<TDec<TChar>, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                             const TDec<TChar>& fmt )
{
    const TNumberFormat<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &TNumberFormat<TChar>::Computational;

    target.EnsureRemainingCapacity( fmt.valueType== 3 ?  48  //float: 2x15 + '.' + ',' + sign + fear
                                                      :  28  //int:   20 digits, grouping symbol, sign and what have you
                                  );

    integer length=      target.Length();

    length=
    fmt.valueType == 1 ?  detail::WriteDecSigned  (          fmt.v.value  , target.VBuffer(), length, fmt.width   , *nf  ) :
    fmt.valueType == 2 ?  detail::WriteDecUnsigned( uint64_t(fmt.v.value) , target.VBuffer(), length, fmt.width   , *nf  ) :
                          detail::WriteFloat      (          fmt.v.fpValue, target.VBuffer(), length, fmt.width   , *nf  );

    target.SetLength( length );
}



template<typename TChar, typename TAllocator>
void AppendableTraits<TBin<TChar>, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                  const  TBin<TChar>& fmt )
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
void AppendableTraits<THex<TChar>, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                  const THex<TChar>& fmt )
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
void AppendableTraits<TOct<TChar>, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                  const TOct<TChar>& fmt )
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
void AppendableTraits<TFill<TChar>, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>&  target,
                                                                   const TFill<TChar>& fmt )
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
void AppendableTraits<std::type_info, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                     const std::type_info& type )
{
    lang::DbgTypeDemangler dmg(type);
    NString     typeName(dmg.Get());
    NString2K   result;

    integer     nameStart= 0;
    bool        startedWithDoubleColon= false;
    for (integer i = 0; i < typeName.Length(); ++i)
    {
        // MSVC adds struct/class/union
        if(typeName.Substring(i, 7).Equals("struct ")) i+= 7;
        if(typeName.Substring(i, 6).Equals("class " )) i+= 6;
        if(typeName.Substring(i, 6).Equals("union " )) i+= 6;

        char c = typeName.CharAt<NC>(i);
        if (c==':')
        {
            ALIB_ASSERT(typeName.CharAt<NC>(i+1) == ':', "STRINGS")
            nameStart= i+2;
            ++i;
            continue;
        }
        if (!(isalnum(c) || c=='_') || i == typeName.Length() - 1) // (c=='<' || c=='>' || c==',' || c=='(')
        {
            if (startedWithDoubleColon)
                result << "::";
            result << typeName.Substring(nameStart, i-nameStart+1);
            nameStart= i+1;
            startedWithDoubleColon= typeName.CharAt(nameStart) == ':';

            // remove C++20 Module name
            if (c == '@')
            {
                result.DeleteEnd<NC>(1);
                while ( ++i < typeName.Length()) {
                    c=  typeName.CharAt<NC>(i);
                    if (!(isalnum(c) || c == '_' || c == '.'))
                    {
                        nameStart= i;
                        --i;
                        break;
                    }
                }
            }
        }
    }

    // some ABIs add a disambiguation-space, others don't (for example some MacOS compiler)
    // Thus we remove the space to have a unique behavior (and have the unit tests succeed).
    result.SearchAndReplace("> >", ">>");
    target << result;
}
#endif

#if ALIB_EXT_LIB_THREADS_AVAILABLE
template<typename TChar, typename TAllocator>
void AppendableTraits<std::thread::id, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                      const std::thread::id& threadID )
{
    #if ALIB_EXT_LIB_THREADS_AVAILABLE
        size_t nativeIDWidth;
        uint64_t nativeID;
             if constexpr ( sizeof(std::thread::id) == sizeof(uint16_t) )  { nativeID= *reinterpret_cast<const uint16_t*>(&threadID); nativeIDWidth= 4; }
        else if constexpr ( sizeof(std::thread::id) == sizeof(uint32_t) )  { nativeID= *reinterpret_cast<const uint32_t*>(&threadID); nativeIDWidth= 8; }
        else                                                               { nativeID= *reinterpret_cast<const uint64_t*>(&threadID); nativeIDWidth=16; }
        if (lang::IsNotNull(threadID) )
        #if !ALIB_SINGLE_THREADED
        {
            integer length= target.Length();
            Thread* thread= Thread::Get(threadID);
            target << thread->GetName()
                   << '(' << thread->GetID()
                   << ",0x" << THex<TChar>(nativeID, int(nativeIDWidth)) <<')';
            target.InsertChars( ' ', 30 + length - target.Length() );
        }
        #else
            target << "TID=0x" << THex<TChar>(nativeID, int(nativeIDWidth) );
        #endif
        else
            target << "<NULL>";

    #endif
}
#endif

template<typename TChar, typename TAllocator>
void AppendableTraits<lang::CallerInfo, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                       const lang::CallerInfo& ci )
{
    NString2K nbuf;
    nbuf << "[@ ";
        if (ci.File)    nbuf << ci.File << ':' << ci.Line;
        else            nbuf << "<NULL>";
    #if ALIB_DEBUG
    nbuf  << " from '";
    if (ci.TypeInfo)   nbuf << *ci.TypeInfo << "::";
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


template<typename TChar, typename TAllocator>
void AppendableTraits<time::DateTime::Duration, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                               time::DateTime::Duration    pSrc )
{
    using Duration= DateTime::Duration;
    Duration src= pSrc;
    auto nanos= src.InNanoseconds();
    if( nanos == 0 )
    {
        target << DT_UNITS[size_t(DayTimeUnits::TS_ZERO)];
        return;
    }

    if( nanos < 0 )
    {
        target << A_CHAR("- ");
        src= Duration() - src;
    }

    TNumberFormat<TChar> nf( TNumberFormat<TChar>::Global );
    nf.FractionalPartWidth= 2;
    int64_t v= src.InAbsoluteDays();
    if( v >= 10 )
    {
        target << TDec<TChar>( src.InDays(), &nf ) << DT_UNITS[size_t(DayTimeUnits::DayPlural)];
        return;
    }

    if( v >  0 )
    {
        target << v << ( v != 1 ?  DT_UNITS[size_t(DayTimeUnits::DayPlural)]
                                :  DT_UNITS[size_t(DayTimeUnits::DaySingular)] );

        Duration cpy= src - ( Duration::FromAbsoluteDays(v) );

        target << ' ' << TDec<TChar>( cpy.InHours(), &nf ) << DT_UNITS[size_t(DayTimeUnits::HourPlural)];
        return;
    }

    v= src.InAbsoluteHours();
    if( v >  0 )
    {
        target << v << ( v != 1 ?  DT_UNITS[size_t(DayTimeUnits::HourPlural)]
                                :  DT_UNITS[size_t(DayTimeUnits::HourSingular)]  );

        Duration cpy= src - ( Duration::FromAbsoluteHours(v) );

        auto minutes= cpy.InAbsoluteMinutes();
        target << ' ' << minutes <<  (minutes!= 1 ?  DT_UNITS[size_t(DayTimeUnits::MinPlural)]
                                                  :  DT_UNITS[size_t(DayTimeUnits::MinSingular)] );
        return;
    }

    v= src.InAbsoluteMinutes();
    if( v > 0 )
    {
        target << v << ( v != 1 ?  DT_UNITS[size_t(DayTimeUnits::MinPlural)]
                                :  DT_UNITS[size_t(DayTimeUnits::MinSingular)] );

        Duration cpy= src - ( Duration::FromAbsoluteMinutes(v) );

        auto seconds= cpy.InAbsoluteSeconds();
        target << ' ' << seconds <<  (seconds!= 1 ?  DT_UNITS[size_t(DayTimeUnits::SecPlural)]
                                                  :  DT_UNITS[size_t(DayTimeUnits::SecSingular)] );
        return;
    }

    v= src.InAbsoluteSeconds();
    if( v > 0 )
    {
        target << TDec<TChar>( src.InSeconds(), &nf ) << DT_UNITS[size_t(DayTimeUnits::SecPlural)];
        return;
    }

    nf.DecMinimumFieldWidth= 3;

    auto val= src.InAbsoluteMilliseconds();
    if( val >= 1 )
    {
        target << TDec<TChar>(val,&nf) << ( val!= 1  ?  DT_UNITS[size_t(DayTimeUnits::MlSecPlural)]
                                                        :  DT_UNITS[size_t(DayTimeUnits::MlSecSingular)] );
        return;
    }

    val= src.InAbsoluteMicroseconds();
    if( val >= 1 )
    {
        target << TDec<TChar>(val,&nf) << ( val!= 1  ?  DT_UNITS[size_t(DayTimeUnits::McSecPlural)]
                                                        :  DT_UNITS[size_t(DayTimeUnits::McSecSingular)] );
        return;
    }

    val= src.InNanoseconds();
    target << TDec<TChar>(val,&nf) << ( val!= 1  ?  DT_UNITS[size_t(DayTimeUnits::NSecPlural)]
                                                    :  DT_UNITS[size_t(DayTimeUnits::NSecSingular)] );
    return;
}

template<typename TChar, typename TAllocator>
void AppendableTraits<time::Ticks::Duration, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                            time::Ticks::Duration       src )
{
    // simply convert the ticks-duration to a DateTime duration and use its append function
    AppendableTraits<time::DateTime::Duration, TChar,TAllocator>()(
        target, time::DateTime::Duration::FromNanoseconds( src.InNanoseconds() ));
}


}} // namespace [alib::strings]

