//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace boxing  {

#if !DOXYGEN

#include "ALib.Lang.CIFunctions.H"

template<typename TComparable>
requires (!std::is_pointer_v<TComparable> )
bool  FEquals::ComparableTypes( const Box& self, const Box& rhsBox )
{
    if( !rhsBox.IsType<TComparable >() )
        return false;

    return self.Unbox<TComparable>() == rhsBox.Unbox<TComparable>();
}

template<typename TComparableP>
requires (std::is_pointer_v<TComparableP>)
bool FEquals::ComparableTypes( const Box& self, const Box& rhsBox )
{
    if( !rhsBox.IsType<TComparableP>() )
        return false;

    auto lhs= self  .Unbox<TComparableP>();
    auto rhs= rhsBox.Unbox<TComparableP>();
    if( lhs == nullptr )
        return rhs == nullptr;
    if( rhs == nullptr )
        return false;

    return *lhs == *rhs;
}


template<typename TComparable>
requires (!std::is_pointer_v<TComparable>)
bool FIsLess::ComparableTypes( const Box& self, const Box& rhsBox )
{
    if( !rhsBox.IsType<TComparable >() )
        return std::type_index( self.TypeID() ) < std::type_index( rhsBox.TypeID() );

    return self.Unbox<TComparable>() < rhsBox.Unbox<TComparable>();
}

template<typename TComparableP>
requires (std::is_pointer_v<TComparableP>)
bool FIsLess::ComparableTypes( const Box& self, const Box& rhsBox )
{
    if( !rhsBox.IsType<TComparableP>() )
        return false;

    auto lhs= self  .Unbox<TComparableP>();
    auto rhs= rhsBox.Unbox<TComparableP>();
    if( lhs == nullptr )
        return rhs != nullptr;
    if( rhs == nullptr )
        return false;

    return *lhs < *rhs;
}

template<size_t N>
size_t FHashcode::UsePlaceholderBytes( const Box& self )
{
    ALIB_WARNINGS_IGNORE_INTEGRAL_CONSTANT_OVERFLOW
        ALIB_ASSERT_ERROR(N == self.GetPlaceholderUsageLength(), "BOXING",
                          "Hash function registered with type of wrong usage length")

        size_t result=  std::size_t(0x52a6937UL) - (N * 0x387e)
                      + std::size_t( self.TypeID().hash_code() );

        constexpr uinteger Bit1= uinteger( 1 );

        // smaller than first "word"
        if constexpr( N < sizeof( uinteger ) )
            return size_t( (  self.Data().Integrals.UArray[0]
                                          & ((Bit1 << (N * 8) )- 1)   )   * 92334534 )
                   + result;

        // add first word
        result+= self.Data().Integrals.UArray[0] * 52424735;

        if constexpr ( N == sizeof(uinteger) )
            return result;

        // tests if smaller than second "word"
        else if constexpr ( N - sizeof( uinteger ) < sizeof( uinteger ) )
        {
            return size_t( (   self.Data().Integrals.UArray[1]
                                          & ((Bit1 << ((N - sizeof(uinteger)) * 8) )- 1)   )   * 892112 )
                   + result;
        }
        else
            return  result + self.Data().Integrals.UArray[1] * 485923;
    ALIB_WARNINGS_RESTORE
}

#if ALIB_STRINGS
template<typename TChar, typename TAllocator>
template<typename TAppendable>
requires( !std::is_pointer_v<TAppendable> )
void FAppend<TChar,TAllocator>::Appendable( const Box& self,
                                            strings::TAString<TChar, TAllocator>& target ) {
    target.template Append<NC>( self.Unbox<TAppendable>() );
}

template<typename TChar, typename TAllocator>
template<typename TAppendable>
requires( std::is_pointer_v<TAppendable> )
void FAppend<TChar,TAllocator>::Appendable( const Box& self,
                                            strings::TAString<TChar, TAllocator>& target ) {
    auto pointer= self.Unbox<TAppendable>();
    if( pointer )
        target.template Append<NC>( *pointer );
    else
    {
        #if ALIB_DEBUG
            target << self.TypeID() <<"(nullptr)";
        #endif
        target << "(nullptr)";
    }
}

// Definition of FAppend::WrappedAppendable()
template<typename TChar, typename TAllocator>
template<typename TAppendable>
void FAppend<TChar,TAllocator>::WrappedAppendable( const Box& self,
                                                   strings::TAString<TChar, TAllocator>& target) {
    target.template _<NC>( self.Unbox<std::reference_wrapper<TAppendable>>().get() );
}

#if !ALIB_SINGLE_THREADED
inline void FAppend_thread( const boxing::Box& self,
                            strings::TAString<character,lang::HeapAllocator>& target ) {
    const Thread* src= self.Unbox<Thread*>();
    if ( src )  target << *src;
    else        target << "nullptr";
}
#endif // !ALIB_SINGLE_THREADED
#endif // ALIB_STRINGS


}} // namespace [alib::boxing]

#include "ALib.Lang.CIMethods.H"

#endif // !DOXYGEN
