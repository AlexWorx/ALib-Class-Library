
//##################################################################################################
// TField()
//##################################################################################################
namespace alib::strings {

template<typename TChar, typename TAllocator>
void AppendableTraits< TField<TChar>, TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target,
                                                                     const TField<TChar>& field) {
    TString<TChar> theContent;

    // buffer used for conversion (if none string)
    TLocalString<TChar, 256> noneStringArgBuf;
    noneStringArgBuf.DbgDisableBufferReplacementWarning();

    // string-type box given?
    if( !field.theContent.template IsType<void>() && field.theContent.template IsType<TString<TChar>>() )
        theContent= field.theContent.template Unbox<TString<TChar>>();
    else {
        // write box into local buffer
        noneStringArgBuf << field.theContent;
        theContent= noneStringArgBuf;
    }

    integer padSize=  field.fieldWidth
                    - theContent.WStringLength();

    // check pad field.width
    if (padSize <= 0 || field.alignment == lang::Alignment::Left ) {
        target.template _          <NC>( theContent );
        if (padSize > 0 )       target.template InsertChars<NC>( field.padChar, padSize );
        return;
    }

    // align Right
    if ( field.alignment == lang::Alignment::Right ) {
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

template void AppendableTraits<TField <nchar>, nchar, lang::HeapAllocator>::operator()( TAString<nchar, lang::HeapAllocator>&, const TField <nchar>& );
template void AppendableTraits<TField <wchar>, wchar, lang::HeapAllocator>::operator()( TAString<wchar, lang::HeapAllocator>&, const TField <wchar>& );
}
