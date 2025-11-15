//==================================================================================================
/// \file
/// This header-file is part of the module \alib_resources of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if !DOXYGEN
#include "ALib.Lang.CIFunctions.H"

ALIB_EXPORT namespace alib::strings {

template<typename TEnum, typename TChar, typename TAllocator>
requires (  enumrecords::IsSerializable<TEnum> && !enumops::IsBitwise<TEnum> )
struct  AppendableTraits<TEnum, TChar, TAllocator>
{
    void operator()( TAString<TChar,TAllocator>& target, TEnum element ) {
        ALIB_ASSERT_ERROR( EnumRecords<TEnum>().begin() != EnumRecords<TEnum>().end(), "ENUMS",
                          "No Enum Records for type <{}> found.", &typeid(TEnum) )

// CHANGE 1 (compared to original implementation, without module Resources in the ALib Build)
        target << ResourcedType<TEnum>::TypeNamePrefix();
        auto* record= enumrecords::TryRecord( element );
        if( record != nullptr )
            target << record->EnumElementName;
        else
            target << UnderlyingIntegral( element );
// CHANGE 1 (compared to original implementation, without module Resources in the ALib Build)
        target << ResourcedType<TEnum>::TypeNamePostfix();
    }
};


template<typename TBitwiseEnum, typename TChar, typename TAllocator>
requires (     alib::enumrecords::IsSerializable<TBitwiseEnum>
            && alib::enumops::IsBitwise     <TBitwiseEnum>  )
struct  AppendableTraits<TBitwiseEnum, TChar,TAllocator> {

    void operator()( TAString<TChar,TAllocator>& target, TBitwiseEnum elements ) {
        ALIB_ASSERT_ERROR( EnumRecords<TBitwiseEnum>().begin() != EnumRecords<TBitwiseEnum>().end(),
            "ENUMS", "No Enum Records for type <{}> found.", &typeid(TBitwiseEnum) )

// CHANGE 1 (compared to original implementation, without module Resources in the ALib Build)
        target << ResourcedType<TBitwiseEnum>::TypeNamePrefix();

        // check what has been covered and omit double entries
        TBitwiseEnum covered= TBitwiseEnum(0);

        // loop over entry 2 to end, check bit
        integer len= target.Length();

        for( auto recordIt=   EnumRecords<TBitwiseEnum>().begin() ;
                  recordIt != EnumRecords<TBitwiseEnum>().end()   ; ++recordIt  )
        {
            // no bits are set and this entry does not contain bits, then stop here
            if( recordIt.Integral() == 0 ) {
                if( elements == TBitwiseEnum(0) ) {
                    target << recordIt->EnumElementName;
// CHANGE 2 (compared to original implementation, without module Resources in the ALib Build)
                    target << ResourcedType<TBitwiseEnum>::TypeNamePostfix();
                    return;
            }   }
            else if(     HasBits( elements, recordIt.Enum() )
                     && !HasBits( covered , recordIt.Enum() )  )
            {
                covered|= recordIt.Enum();
                target << recordIt->EnumElementName << ',';
        }   }
        len= target.Length() - len;

        // remove the last comma
        if( len != 0 )
            target.DeleteEnd( 1 );

        ALIB_ASSERT_ERROR( covered == elements, "ENUMS",
           "Not all bits have been covered while writing bitset '{}' of enumeration type <{}>. "
           "Remaining bits are '{}'.", NString(NString128(NBin( elements ))),
                &typeid(TBitwiseEnum), NString(NString128(NBin( covered & elements ))) )

// CHANGE 3 (compared to original implementation, without module Resources in the ALib Build)
        target << ResourcedType<TBitwiseEnum>::TypeNamePostfix();
    }
};

} // namespace [alib::strings]
#include "ALib.Lang.CIMethods.H"

#endif // !DOXYGEN
