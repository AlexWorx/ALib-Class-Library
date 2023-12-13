/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_boxing of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_BOXING_FUNCTIONDEFS
#define HPP_ALIB_BOXING_FUNCTIONDEFS 1

#if !defined(HPP_ALIB_BOXING_BOXING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

namespace aworx { namespace lib { namespace boxing  {

#if !defined(ALIB_DOX)

    template<typename TComparable>
ATMP_T_IF(bool, !ATMP_IS_PTR(TComparable) )  FEquals::ComparableTypes( const Box& self, const Box& rhsBox )
{
    if( !rhsBox.IsType<TComparable >() )
        return false;

    return self.Unbox<TComparable>() == rhsBox.Unbox<TComparable>();
}

template<typename TComparableP>
ATMP_T_IF(bool, ATMP_IS_PTR(TComparableP) )  FEquals::ComparableTypes( const Box& self, const Box& rhsBox )
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
ATMP_T_IF(bool, !ATMP_IS_PTR(TComparable) )  FIsLess::ComparableTypes( const Box& self, const Box& rhsBox )
{
    if( !rhsBox.IsType<TComparable >() )
        return std::type_index( self.TypeID() ) < std::type_index( rhsBox.TypeID() );

    return self.Unbox<TComparable>() < rhsBox.Unbox<TComparable>();
}

template<typename TComparableP>
ATMP_T_IF(bool, ATMP_IS_PTR(TComparableP) )  FIsLess::ComparableTypes( const Box& self, const Box& rhsBox )
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

#endif // !defined(ALIB_DOX)

template<size_t N>
size_t FHashcode::UsePlaceholderBytes( const Box& self )
{
    ALIB_WARNINGS_IGNORE_IF_CONSTEXPR
    ALIB_WARNINGS_IGNORE_INTEGRAL_CONSTANT_OVERFLOW

    ALIB_ASSERT_ERROR(N == self.GetPlaceholderUsageLength(), "BOXING",
                      "Hash function registered with type of wrong usage length")

    size_t result=  static_cast<std::size_t>(0x52a6937UL) - (N * 0x387e)
                  + static_cast<std::size_t>( self.TypeID().hash_code() );

    constexpr uinteger Bit1= static_cast<uinteger>( 1 );

    // smaller than first "word"
    if ALIB_CONSTEXPR17( N < sizeof( uinteger ) )
        return static_cast<size_t>( (  self.Data().GetUInteger(0)
                                      & ((Bit1 << (N * 8) )- 1)   )   * 92334534 )
               + result;

    // add first word
    result+= self.Data().GetUInteger(0) * 52424735;

    if ALIB_CONSTEXPR17 ( N == sizeof(uinteger) )
        return result;

    // tests if smaller than second "word"
    else if ALIB_CONSTEXPR17 ( N - sizeof( uinteger ) < sizeof( uinteger ) )
    {
        return static_cast<size_t>( (   self.Data().GetUInteger(1)
                                      & ((Bit1 << ((N - sizeof(uinteger)) * 8) )- 1)   )   * 892112 )
               + result;
    }
    else
        return  result + self.Data().GetUInteger(1) * 485923;
    ALIB_WARNINGS_RESTORE
    ALIB_WARNINGS_RESTORE
}

#if ALIB_STRINGS && !defined(ALIB_DOX)
template<typename TChar>
template<typename TAppendable>
ATMP_VOID_IF( !ATMP_IS_PTR(TAppendable) )
FAppend<TChar>::Appendable( const Box& self, strings::TAString<TChar>& target )
{
    target.template Append<false>( self.Unbox<TAppendable>() );
}

template<typename TChar>
template<typename TAppendable>
ATMP_VOID_IF(  ATMP_IS_PTR(TAppendable) )
FAppend<TChar>::Appendable( const Box& self, strings::TAString<TChar>& target )
{
    auto pointer= self.Unbox<TAppendable>();
    if( pointer )
        target.template Append<false>( *pointer );
    else
    {
        #if ALIB_DEBUG
            target << self.TypeID() <<"(nullptr)";
        #endif
        target << "(nullptr)";
    }

}

template<typename TChar>
template<typename TAppendable>
void FAppend<TChar>::WrappedAppendable( const Box& self, strings::TAString<TChar>& target)
{
    target.template _<false>( self.Unbox<std::reference_wrapper<TAppendable>>().get() );
}
#endif // defined(ALIB_DOX)
}}} // namespace [aworx::lib::boxing]

#endif // HPP_ALIB_BOXING_FUNCTIONDEFS

