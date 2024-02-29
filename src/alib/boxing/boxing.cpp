// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_BOXING_BOXING)
#      include "alib/boxing/boxing.hpp"
#   endif

#   if !defined(HPP_ALIB_CHARACTERS_CHARARRAY)
#      include "alib/characters/chararray.hpp"
#   endif

#   if ALIB_MONOMEM && !defined(HPP_ALIB_MONOMEM_MONOALLOCATOR)
#       include "alib/monomem/monoallocator.hpp"
#   endif

#   if ALIB_DEBUG && !defined (HPP_ALIB_BOXING_DBGBOXING)
#      include "alib/boxing/dbgboxing.hpp"
#   endif

#   if ALIB_ENUMS && ALIB_STRINGS
#       if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#           include "alib/enums/serialization.hpp"
#       endif
#       if !defined(HPP_ALIB_STRINGS_UTIL_TOKEN)
#          include "alib/strings/util/token.hpp"
#       endif
#   endif


#   if ALIB_STRINGS
#      if  !defined (HPP_ALIB_STRINGS_ASTRING)
#         include "alib/strings/astring.hpp"
#      endif
#      if !defined(HPP_ALIB_STRINGS_NUMBERFORMAT)
#         include "alib/strings/numberformat.hpp"
#      endif
#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#       include "alib/strings/format.hpp"
#   endif
#      if !defined(HPP_ALIB_STRINGS_UTIL_TOKEN)
#         include "alib/strings/util/token.hpp"
#      endif
#   endif

#   if ALIB_CAMP
#      if !defined(HPP_ALIB_LANG_COMMONENUMS)
#           include "alib/lang/commonenums.hpp"
#      endif
#   endif

#   if !defined (_GLIBCXX_CMATH) && !defined (_CMATH_)
#      include <cmath>
#   endif

#   if !defined(_GLIBCXX_FUNCTIONAL) && !defined(_FUNCTIONAL_)
       #include <functional>
#   endif
#endif // !defined(ALIB_DOX)


#if ALIB_STRINGS
    ALIB_BOXING_VTABLE_DEFINE( std::reference_wrapper<alib::strings::TAString<nchar>>, vt_alib_wrapped_tanstring )
    ALIB_BOXING_VTABLE_DEFINE( std::reference_wrapper<alib::strings::TAString<wchar>>, vt_alib_wrapped_tawstring )
    ALIB_BOXING_VTABLE_DEFINE( std::reference_wrapper<alib::strings::TAString<xchar>>, vt_alib_wrapped_taxstring )
#endif

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
namespace alib {

/** ************************************************************************************************
 * This is the reference documentation of sub-namespace \c boxing of the \aliblink, which
 * holds types of library module \alib_boxing.
 *
 * Extensive documentation for this module is provided with
 * \ref alib_mod_boxing "ALib Module Boxing - Programmer's Manual".
 **************************************************************************************************/
namespace boxing {

// #################################################################################################
// #################################################################################################
// class Box
// #################################################################################################
// #################################################################################################

#if ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    bool    Box::IsSignedIntegral() const
    {
        #if  ALIB_SIZEOF_INTEGER == 8
        return         IsType< int64_t >()
                    || IsType< int32_t >()
        #else
        return         IsType< int32_t >()
                    || IsType< int64_t >()
        #endif
                    || IsType< int8_t  >()
                    || IsType< int16_t >()
                    || IsType< intGap_t>();
    }

    bool    Box::IsUnsignedIntegral() const
    {
        #if  ALIB_SIZEOF_INTEGER == 8
        return         IsType<uint64_t >()
                    || IsType<uint32_t >()
        #else
        return         IsType<uint32_t >()
                    || IsType<uint64_t >()
        #endif
                    || IsType<uint8_t  >()
                    || IsType<uint16_t >()
                    || IsType<uintGap_t>();
    }

    integer Box::UnboxSignedIntegral() const
    {
        #if  ALIB_SIZEOF_INTEGER == 8
            return     IsType< int64_t >()  ?                      ( Unbox< int64_t >() )
                    :  IsType< int32_t >()  ? static_cast< integer>( Unbox< int32_t >() )
        #else
            return     IsType< int32_t >()  ?                      ( Unbox< int32_t >() )
                    :  IsType< int64_t >()  ? static_cast< integer>( Unbox< int64_t >() )
        #endif
                    :  IsType< int8_t  >()  ? static_cast< integer>( Unbox< int8_t  >() )
                    :  IsType< int16_t >()  ? static_cast< integer>( Unbox< int16_t >() )
                    :                         static_cast< integer>( Unbox< intGap_t>() );
    }


    uinteger Box::UnboxUnsignedIntegral() const
    {
        #if  ALIB_SIZEOF_INTEGER == 8
            return     IsType<uint64_t >() ?                       ( Unbox<uint64_t >() )
                    :  IsType<uint32_t >() ?  static_cast<uinteger>( Unbox<uint32_t >() )
        #else
            return     IsType<uint32_t >() ?                       ( Unbox<uint32_t >() )
                    :  IsType<uint64_t >() ?  static_cast<uinteger>( Unbox<uint64_t >() )
        #endif
                    :  IsType<uint8_t  >() ?  static_cast<uinteger>( Unbox<uint8_t  >() )
                    :  IsType<uint16_t >() ?  static_cast<uinteger>( Unbox<uint16_t >() )
                    :                         static_cast<uinteger>( Unbox<uintGap_t>() );
    }
#endif

#if ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
bool    Box::IsCharacter()                                                                     const
{
    return    IsType<char    >()
           || IsType<wchar_t >()
           || IsType<char16_t>()
           || IsType<char32_t>();
}

wchar   Box::UnboxCharacter()                                                                  const
{
    return   IsType<char    >() ? static_cast<wchar>( Unbox<char    >() )
           : IsType<wchar_t >() ? static_cast<wchar>( Unbox<wchar_t >() )
           : IsType<char16_t>() ? static_cast<wchar>( Unbox<char16_t>() )
           :                      static_cast<wchar>( Unbox<char32_t>() );
}
#endif

bool    Box::IsFloatingPoint()                                                                 const
{
    return    IsType<     double>()
        #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
           || IsType<     float >()
        #endif
        #if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
           || IsType<long double>()
        #endif
        ;
}

double  Box::UnboxFloatingPoint()                                                              const
{
  #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    if( IsType<     float >() ) return   static_cast<double>( Unbox<float      >() );
  #endif

  #if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
    if( IsType<long double>() ) return   static_cast<double>( Unbox<long double>() );
  #endif

  return Unbox<double>();
}

       Box::operator bool()              const     { return  Call<FIsTrue    >();          }
bool   Box::IsNotNull ()                 const     { return  Call<FIsNotNull >();          }
size_t Box::Hashcode  ()                 const     { return  Call<FHashcode  >();          }
ALIB_IF_MONOMEM(
void   Box::Clone( monomem::MonoAllocator& memory) {         Call<FClone     >( memory );  } )


}} // namespace [alib::boxing]

using namespace alib::boxing;
bool   Box::operator==(const Box& rhs) const { return  Call<FEquals>( rhs );                            }
bool   Box::operator< (const Box& rhs) const { return  Call<FIsLess>( rhs );                            }
bool   Box::operator<=(const Box& rhs) const { return  Call<FIsLess>( rhs )  ||   Call<FEquals>( rhs ); }
bool   Box::operator> (const Box& rhs) const { return !Call<FIsLess>( rhs )  &&  !Call<FEquals>( rhs ); }
namespace alib {  namespace boxing {


// #################################################################################################
// #################################################################################################
// class Boxes
// #################################################################################################
// #################################################################################################
#if ALIB_MONOMEM
    void Boxes::CloneAll(monomem::MonoAllocator& memory)
    {
        for( auto& box : *this )
            box.Clone( memory );
    }
#endif

#if !defined(ALIB_DOX)

namespace {

// #################################################################################################
// #################################################################################################
// Functions
// #################################################################################################
// #################################################################################################


// #################################################################################################
// FIsNotNull
// #################################################################################################
bool FIsNotNull_Default( const Box& box )
{
    return !(    (box.IsArray()    && box.UnboxLength() == 0               )
              || (box.IsPointer()  && box.Data().Pointers.CVoid == nullptr )
            );
}


// #################################################################################################
// FHashcode
// #################################################################################################
std::size_t FHashcode_Default( const Box& self )
{
    if( self.IsPointer() )
    {
        return   static_cast<size_t>(0xa814e72cUL)
               + static_cast<size_t>( self.TypeID().hash_code() )
               + self.Data().Integrals.UInt * 89047023;
    }

    if( self.IsEnum() )
    {
        return   static_cast<size_t>(0x49a024efUL)
               + static_cast<size_t>( self.TypeID().hash_code() )
               + self.Data().Integrals.UInt * 79204799;
    }

    if( self.IsArray() )
    {
        std::size_t result=    0xa925eb91L
                             + std::size_t( self.ElementTypeID().hash_code() );

        // do different loops depending on array element size
        auto size  = self.ArrayElementSize();
        auto length= self.UnboxLength();
        if( size == 2 || size == 6 )
        {
            if (size == 6)
                length*= 3;

            uint16_t* array= self.Data().Pointer<uint16_t>();
            for( integer i= 0 ; i < length ; ++i )
                result = 67 * result + array[i];

            return static_cast<size_t>( result );
        }

        if( size == 4 )
        {
            uint32_t* array= self.Data().Pointer<uint32_t>();
            for( integer i= 0 ; i < length ; ++i )
                result = 67*result + array[i];

            return static_cast<size_t>( result );
        }

        if( size == 8 )
        {
            uint64_t* array= self.Data().Pointer<uint64_t>();
            for( integer i= 0 ; i < length ; ++i )
                result = std::size_t( 67 * result + array[i] );

            return std::size_t( result );
        }

        // any other size
        {
            length*= size;
            unsigned char* array= self.Data().Pointer<unsigned char>();
            for( int i= 0 ; i < length ; ++i )
                result = 67*result + array[i];

            return static_cast<size_t>( result );
        }
    }

    //--- default (value types) ---
    size_t result=  static_cast<size_t>(0xcf670957UL)
        + static_cast<size_t>( self.TypeID().hash_code() );

    unsigned int usedLen= self.GetPlaceholderUsageLength();

    constexpr uinteger Bit1= static_cast<uinteger>( 1 );

    // smaller than first "word"
    if( usedLen < sizeof( uinteger ) )
        return static_cast<size_t>( (  self.Data().GetUInteger(0)
                                      & ((Bit1 << (usedLen * 8) )- 1)   )   * 32194735 )
                  + result;

    // add first word
    result+= self.Data().GetUInteger(0) * 32194735;

    if( usedLen == sizeof(uinteger) )
        return result;

    // tests if smaller than second "word"
    if( usedLen - sizeof( uinteger ) < sizeof( uinteger ) )
    {
        return static_cast<size_t>( (   self.Data().GetUInteger(1)
                                      & ((Bit1 << ((usedLen - sizeof(uinteger)) * 8) )- 1)   )   * 321947 )
                  + result;
    }

    return  result + self.Data().GetUInteger(1) * 321947;


}

// #################################################################################################
// FEquals
// #################################################################################################
bool FEquals_Default( const Box& self, const Box& rhs )
{
    if( !self.IsSameType( rhs ) )
        return false;

    // array types?
    if( self.IsArray() )
    {
        // different in type, length or nulled?
        if(    self.UnboxLength() != rhs.UnboxLength()
            || (    ( self.Data().Pointer<char>() == nullptr)
                 != ( rhs .Data().Pointer<char>() == nullptr  ) )     )
            return false;

        return    self.Data().Pointer<char>() == nullptr
               || self.UnboxLength()  == 0
               || self.Data().Pointer<char>() == rhs.Data().Pointer<char>()
               || memcmp( self.Data().Pointer<char>(),
                          rhs .Data().Pointer<char>(),
                          static_cast     <size_t>( self.UnboxLength() ) * self.ArrayElementSize()
                         ) == 0;
    }

    // non-array types
    unsigned int usedLen= self.GetPlaceholderUsageLength();

    constexpr uinteger Bit1= static_cast<uinteger>( 1 );
    // tests if smaller than first "word"
    if( usedLen < sizeof( uinteger ) )
    {
        uinteger mask=  (Bit1 << (usedLen * 8) )- 1;
        return      ( self.Data().GetUInteger(0) & mask )
                ==  ( rhs .Data().GetUInteger(0) & mask );
    }

    // test first word
    if( self.Data().GetUInteger(0)  !=  rhs.Data().GetUInteger(0) )
        return false;

    if( usedLen == sizeof( uinteger ) )
        return true;

    // tests if smaller than second "word"
    if( usedLen - sizeof( uinteger ) < sizeof( uinteger ) )
    {
        uinteger mask=  (Bit1 << (usedLen - sizeof(uinteger)) * 8 )- 1;
        return      ( self.Data().GetUInteger(1) & mask )
                ==  ( rhs .Data().GetUInteger(1) & mask );
    }

    return  self.Data().GetUInteger(1)  ==  rhs.Data().GetUInteger(1);
}


bool FEquals_double( const Box& self, const Box& rhsBox )
{
    double lhs= self.UnboxFloatingPoint();
    double rhs;
         if( rhsBox.IsFloatingPoint   () )  rhs=                      rhsBox.UnboxFloatingPoint   ()  ;
    else if( rhsBox.IsSignedIntegral  () )  rhs= static_cast<double>( rhsBox.UnboxSignedIntegral  () );
    else if( rhsBox.IsUnsignedIntegral() )  rhs= static_cast<double>( rhsBox.UnboxUnsignedIntegral() );
    else                                    return false;

    // we can ignore this warning, because we fixed it with the second test
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wfloat-equal"
    #endif

    return      lhs == rhs
            // take rounding errors into account.
            // We use the "float-epsilon" and double it to be even a little weaker!
            ||  std::fabs( lhs - rhs ) <= static_cast<double>( 2.0f * std::numeric_limits<float>::epsilon() );

    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif
}

bool FEquals_integer( const Box& self, const Box& rhsBox )
{
    if( rhsBox.IsFloatingPoint() )
        return  FEquals_double( rhsBox, self );

    integer rhs;
         if( rhsBox.IsSignedIntegral  () )  rhs=                        rhsBox.UnboxSignedIntegral  ()  ;
    else if( rhsBox.IsUnsignedIntegral() )  rhs= static_cast< integer>( rhsBox.UnboxUnsignedIntegral() );
    else                                    return false;

    return self.UnboxSignedIntegral() == rhs;
}

bool FEquals_uinteger( const Box& self, const Box& rhsBox )
{
    if( rhsBox.IsFloatingPoint() )
        return  FEquals_double( rhsBox, self );

    uinteger rhs;
         if( rhsBox.IsSignedIntegral  () )  rhs= static_cast<uinteger>( rhsBox.UnboxSignedIntegral  () );
    else if( rhsBox.IsUnsignedIntegral() )  rhs=                        rhsBox.UnboxUnsignedIntegral()  ;
    else                                    return false;

    return self.UnboxUnsignedIntegral() == rhs;
}

bool FEquals_char( const Box& self, const Box& rhs )
{
    if( !rhs.IsCharacter() )
        return false;
    return  self.UnboxCharacter()  ==  rhs.UnboxCharacter();
}


template<typename TChar>
bool FEquals_TChar_Arr( const Box& lhs, const Box& rhs )
{
    if ( !rhs.IsArrayOf<TChar>() )
        return false;

    const TChar* compBuf= rhs.Data().Pointer<TChar>();
    const TChar*  boxBuf= lhs.Data().Pointer<TChar>();
    if ( (boxBuf == nullptr) != (compBuf == nullptr) )
            return false;

    integer compLen= rhs.UnboxLength();
    integer boxLen=  lhs.UnboxLength();
    if ( boxLen != compLen )
        return false;

    return  0 == characters::CharArray<TChar>::Compare ( boxBuf, compBuf, boxLen );
}

// #################################################################################################
// FIsLess
// #################################################################################################
bool FIsLess_Default( const Box& box, const Box& comp )
{
    return           std::type_index( box.TypeID() ) < std::type_index(comp.TypeID() )
            ||  (    box.TypeID() == comp.TypeID()
                   &&box.Data().GetUInteger(0)   <  comp.Data().GetUInteger(0)                    );

}

#if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
bool FIsLess_integer( const Box& self, const Box& rhs )
{
    auto lhs= self.Data().GetInteger(0);
    if( rhs.IsSameType( self ) )    return                       lhs   <                         rhs.Unbox<integer >   ()  ;
    if( rhs.IsType<uinteger>() )    return                       lhs   <  static_cast<integer> ( rhs.Unbox<uinteger>   () );
    if( rhs.IsFloatingPoint()  )    return static_cast<double> ( lhs ) <                         rhs.UnboxFloatingPoint();

    return std::type_index( self.TypeID() ) < std::type_index( rhs.TypeID() );
}

bool FIsLess_uinteger( const Box& self, const Box& rhs )
{
    auto lhs= self.Data().GetUInteger(0);
    if( rhs.IsSameType( self ) )    return                         lhs   <  rhs.Data().GetUInteger(0)  ;
    if( rhs.IsType<integer> () )    return static_cast<integer>  ( lhs ) <  rhs.Unbox<integer>();
    if( rhs.IsFloatingPoint()  )    return static_cast<double  > ( lhs ) <  rhs.UnboxFloatingPoint();

    return std::type_index( self.TypeID() ) < std::type_index( rhs.TypeID() );
}

#else

bool helperBijectiveLessS( integer selfVal, const Box& selfType, const Box& rhs )
{
    if( rhs.IsSignedIntegral  () )  return selfVal                        <                        rhs.UnboxSignedIntegral  ()  ;
    if( rhs.IsUnsignedIntegral() )  return selfVal                        < static_cast< integer>( rhs.UnboxUnsignedIntegral() );
    if( rhs.IsFloatingPoint   () )  return static_cast<double>( selfVal ) <                        rhs.UnboxFloatingPoint   ()  ;

    // no number?
    return std::type_index( selfType.TypeID() ) < std::type_index( rhs.TypeID() );
}

bool helperBijectiveLessU( uinteger selfVal, const Box& selfType, const Box& rhs )
{
    if( rhs.IsSignedIntegral  () )  return selfVal                        < static_cast<uinteger>( rhs.UnboxSignedIntegral  () );
    if( rhs.IsUnsignedIntegral() )  return selfVal                        <                        rhs.UnboxUnsignedIntegral()  ;
    if( rhs.IsFloatingPoint   () )  return static_cast<double>( selfVal ) <                        rhs.UnboxFloatingPoint   ()  ;

    // no number?
    return std::type_index( selfType.TypeID() ) < std::type_index( rhs.TypeID() );
}

bool FIsLess_int8_t   ( const Box& self, const Box& rhs )  { return helperBijectiveLessS( static_cast< integer>( self.Unbox< int8_t  >() ), self, rhs ); }
bool FIsLess_int16_t  ( const Box& self, const Box& rhs )  { return helperBijectiveLessS( static_cast< integer>( self.Unbox< int16_t >() ), self, rhs ); }
bool FIsLess_int32_t  ( const Box& self, const Box& rhs )  { return helperBijectiveLessS( static_cast< integer>( self.Unbox< int32_t >() ), self, rhs ); }
bool FIsLess_int64_t  ( const Box& self, const Box& rhs )  { return helperBijectiveLessS( static_cast< integer>( self.Unbox< int64_t >() ), self, rhs ); }
bool FIsLess_intGap_t ( const Box& self, const Box& rhs )  { return helperBijectiveLessS( static_cast< integer>( self.Unbox< intGap_t>() ), self, rhs ); }

bool FIsLess_uint8_t  ( const Box& self, const Box& rhs )  { return helperBijectiveLessU( static_cast<uinteger>( self.Unbox<uint8_t  >() ), self, rhs ); }
bool FIsLess_uint16_t ( const Box& self, const Box& rhs )  { return helperBijectiveLessU( static_cast<uinteger>( self.Unbox<uint16_t >() ), self, rhs ); }
bool FIsLess_uint32_t ( const Box& self, const Box& rhs )  { return helperBijectiveLessU( static_cast<uinteger>( self.Unbox<uint32_t >() ), self, rhs ); }
bool FIsLess_uint64_t ( const Box& self, const Box& rhs )  { return helperBijectiveLessU( static_cast<uinteger>( self.Unbox<uint64_t >() ), self, rhs ); }
bool FIsLess_uintGap_t( const Box& self, const Box& rhs )  { return helperBijectiveLessU( static_cast<uinteger>( self.Unbox<uintGap_t>() ), self, rhs ); }

#endif

bool FIsLess_char( const Box& self, const Box& rhs )
{
    if( rhs.IsCharacter   () )
        return  self.UnboxCharacter()  <  rhs.UnboxCharacter   ()  ;

    return std::type_index( self.TypeID() ) < std::type_index( rhs.TypeID() );
}



bool FIsLess_double( const Box& self, const Box& rhs )
{
    double lhs= self.Unbox<double>();
    if( rhs.IsFloatingPoint   () )    return    lhs  <                        rhs.UnboxFloatingPoint   ()  ;
    if( rhs.IsSignedIntegral  () )    return    lhs  <  static_cast<double> ( rhs.UnboxSignedIntegral  () );
    if( rhs.IsUnsignedIntegral() )    return    lhs  <  static_cast<double> ( rhs.UnboxUnsignedIntegral() );

    return std::type_index( self.TypeID() ) < std::type_index( rhs.TypeID() );
}

#if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    bool FIsLess_float( const Box& self, const Box& rhs )
    {
        float lhs= self.Unbox<float>();
        if( rhs.IsType<float     >() )    return    lhs  <                       rhs.Unbox<float >()  ;
        if( rhs.IsType<double    >() )    return    lhs  <  static_cast<float> ( rhs.Unbox<double>() );
        if( rhs.IsSignedIntegral  () )    return    lhs  <  static_cast<float> ( rhs.UnboxSignedIntegral  () );
        if( rhs.IsUnsignedIntegral() )    return    lhs  <  static_cast<float> ( rhs.UnboxUnsignedIntegral() );

        return std::type_index( self.TypeID() ) < std::type_index( rhs.TypeID() );
    }
#endif



// #################################################################################################
// FClone
// #################################################################################################
#if ALIB_MONOMEM

void FClone_Default( Box& self, monomem::MonoAllocator& memory)
{
    if ( !self.IsArray() || self.UnboxLength() == 0)
        return;

    Placeholder& placeHolder= self.Data();
    auto* src= placeHolder.Pointer<char>();
    if( src== nullptr || placeHolder.GetInteger(1) < 0 )
        return;

    size_t alignment= self.ArrayElementSize();
    if( alignment > sizeof(std::ptrdiff_t) )
        alignment=  sizeof(std::ptrdiff_t);


    placeHolder.Pointer( memory.Alloc( self.ArrayElementSize() * placeHolder.GetUInteger(1) ,
                                       alignment ) );
    memcpy( placeHolder.Pointer<char>(), src, self.ArrayElementSize() * placeHolder.GetUInteger(1) );
}

#endif

// #################################################################################################
// FIsTrue
// #################################################################################################
bool FIsTrue_Default( const Box& self )
{
    if( self.IsArray() )
        return  self.UnboxLength() != 0;

    // non-array types
    unsigned int usedLen= self.GetPlaceholderUsageLength();

    constexpr uinteger Bit1= static_cast<uinteger>( 1 );
    // tests if smaller than first "word"
    if( usedLen < sizeof( uinteger ) )
        return   (   self.Data().GetUInteger(0)
                   & ((Bit1 << (usedLen * 8) )- 1)   )  !=  0;

    // test first word
    if( self.Data().GetUInteger(0)  !=  0 )
        return true;

    if( usedLen == sizeof( uinteger ) )
        return false;

    // tests if smaller than second "word"
    if( usedLen - sizeof( uinteger ) < sizeof( uinteger ) )
    {
        return      (   self.Data().GetUInteger(1)
                      & ((Bit1 << (usedLen - sizeof(uinteger)) * 8 )- 1) ) != 0;
    }

    return  self.Data().GetUInteger(1)  !=  0;
}


// #################################################################################################
// ############################       Strings And Boxing       #####################################
// #################################################################################################
#if ALIB_STRINGS

template<typename TChar>
bool FIsLess_TChar_arr( const Box& lhs, const Box& rhs )
{
    if( rhs.IsArrayOf<TChar>() )
        return lhs.Unbox<strings::TString<TChar>>() < rhs.Unbox<strings::TString<TChar>>();

    return std::type_index( lhs.TypeID() ) < std::type_index(rhs.TypeID() );
}


template<typename TChar>
void FAppend_Default( const Box& self, strings::TAString<TChar>& target)
{
    if( self.IsPointer() )
    {
        target << ALIB_REL_DBG(  "PointerType", self.TypeID() )
               << '(' << strings::TNumberFormat<TChar>::Computational.HexLiteralPrefix
               <<  typename strings::TFormat<TChar>::Hex( self.Data().Integrals.UInt )
               << ')';
        return;
    }

    if( self.IsEnum() )
    {
        target << ALIB_REL_DBG(  "EnumType", self.TypeID() )   <<  '(' << self.Data().Integrals.Int <<  ')';
        return;
    }

    if( self.IsArray() )
    {
        target << ALIB_REL_DBG(  "ArrayType", self.ElementTypeID() )   <<  '[' << self.UnboxLength() <<  ']';
        return;
    }

    // value type
    target << ALIB_REL_DBG(  "ValueType", self.TypeID() )   <<  "(Size: " << self.GetPlaceholderUsageLength() <<  " bytes)";
}


template<typename TCharSrc, typename TChar>
void FAppend_TcharArr( const Box& box, strings::TAString<TChar>& target )
{
    target.template Append<false>( box.UnboxArray<TCharSrc>(), box.UnboxLength() );
}

#endif


} // anonymous namespace

#endif // !defined(ALIB_DOX)


// #################################################################################################
// Non-anonymous functions
// #################################################################################################

bool FIsNotNull::ConstantTrue( const alib::Box & )
{
    return false;
}


// static member definition

// #################################################################################################
// Bootstrap()
// #################################################################################################
#if !defined(ALIB_DOX)
namespace{ unsigned int initFlag= 0; }
#endif // !defined(ALIB_DOX)

#if ALIB_DEBUG && !defined(ALIB_DOX)
    // This is used by boxing::Bootstrap to do runtime-check for compatibility of boxing
    // and long double values.
    // It was moved to file boxes.cpp to prevent the compiler to optimize and remove the code.
    extern  long double dbgLongDoubleWriteTestMem[2];
    extern  void dbgLongDoubleTrueLengthSet();
    extern  bool dbgLongDoubleTrueLengthTest();
#endif

void Bootstrap()
{
    ALIB_ASSERT_ERROR( initFlag != 2, "BOXING", "Can't bootstrap after termination" )
    if( initFlag == 0x92A3EF61 )
        return;
    initFlag= 0x92A3EF61;

    //############### Debug-compilation checks ################
    #if ALIB_DEBUG
        // check size of long double
        {
            dbgLongDoubleTrueLengthSet();
            dbgLongDoubleWriteTestMem[0]= 0.0L;
            ALIB_ASSERT_ERROR( dbgLongDoubleTrueLengthTest(), "BOXING",
            "Platform not supported. Template specialization T_SizeInPlaceholder<long double> contains wrong size" )
        }
    #endif

    //#############################     BootstrapRegister Static VTables    #################################
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_voidP    )
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_boxes    )
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_boxarray )

DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_REGISTER_1])
ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_bool )
DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_REGISTER_1])

    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER(  vt_integer    )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_uinteger    )
    #else
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER(    vt_int8_t   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER(   vt_uint8_t   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER(   vt_int16_t   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER(  vt_uint16_t   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER(   vt_int32_t   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER(  vt_uint32_t   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER(  vt_intGap_t   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_uintGap_t   )
      #if ALIB_SIZEOF_INTEGER == 8
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER(   vt_int64_t   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER(  vt_uint64_t   )
      #endif
    #endif // !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS


    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_double )
    #if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_long_double )
    #endif
    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_float  )
    #endif


    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_wchar       )
    #else
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_char        )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_wchar_t     )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_char16_t    )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_char32_t    )
    #endif

DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_REGISTER_2])
ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_arr_char )
DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_REGISTER_2])

    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_arr_wchar_t   )
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_arr_char16_t  )
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_arr_char32_t  )

    // Static VTables for standard types
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_std_type_info )

    // CodeMarker_CommonEnums
    // Static VTables for low-level ALib types
    #if !defined(HPP_ALIB_LANG_COMMONENUMS_DEFS)
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Alignment         )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Bool              )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Caching           )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Case              )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_ContainerOp       )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_CreateDefaults    )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_CreateIfNotExists )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_CurrentData       )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Inclusion         )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Initialization    )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Phase             )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Propagation       )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Reach             )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Responsibility    )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Safeness          )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Side              )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_SortOrder         )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_SourceData        )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Switch            )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Timezone          )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Timing            )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_ValueReference    )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_Whitespaces       )
    #endif

    #if ALIB_STRINGS
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_wrapped_tanstring )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_wrapped_tawstring )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_wrapped_taxstring )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_strings_token )
    #endif

    //########################       Register default implementations        #######################
    BootstrapRegisterDefault<FIsTrue    >( FIsTrue_Default    );
    BootstrapRegisterDefault<FIsNotNull >( FIsNotNull_Default );
    BootstrapRegisterDefault<FIsLess    >( FIsLess_Default    );
    BootstrapRegisterDefault<FHashcode  >( FHashcode_Default  );
    BootstrapRegisterDefault<FEquals    >( FEquals_Default    );
ALIB_IF_MONOMEM(
    BootstrapRegisterDefault<FClone     >( FClone_Default     ); )

    // ################################      IsNotNull    ##########################################
    BootstrapRegister<FIsNotNull, TMappedTo<bool      >>( FIsNotNull::ConstantTrue  );
    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    BootstrapRegister<FIsNotNull, TMappedTo< integer  >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo<uinteger  >>( FIsNotNull::ConstantTrue  );
    #else
    BootstrapRegister<FIsNotNull, TMappedTo< int8_t   >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo<uint8_t   >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo< int16_t  >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo<uint16_t  >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo< int32_t  >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo<uint32_t  >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo< int64_t  >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo<uint64_t  >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo< intGap_t >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo<uintGap_t >>( FIsNotNull::ConstantTrue  );
    #endif

    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    BootstrapRegister<FIsNotNull, TMappedTo<wchar     >>( FIsNotNull::ConstantTrue  );
    #else
    BootstrapRegister<FIsNotNull, TMappedTo<char      >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo<wchar_t   >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo<char16_t  >>( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, TMappedTo<char32_t  >>( FIsNotNull::ConstantTrue  );
    #endif


    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    BootstrapRegister<FIsNotNull, TMappedTo<float     >>( FIsNotNull::ConstantTrue   );
    #endif
    BootstrapRegister<FIsNotNull, TMappedTo<double    >>( FIsNotNull::ConstantTrue   );

    // ################################      Hashcode     ##########################################
    BootstrapRegister<FHashcode, TMappedTo<bool       >>( FHashcode::UsePlaceholderBytes<sizeof(bool     )>  );
    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    BootstrapRegister<FHashcode, TMappedTo< integer   >>( FHashcode::UsePlaceholderBytes<sizeof( integer )>  );
    BootstrapRegister<FHashcode, TMappedTo<uinteger   >>( FHashcode::UsePlaceholderBytes<sizeof(uinteger )>  );
    #else
    BootstrapRegister<FHashcode, TMappedTo< int8_t    >>( FHashcode::UsePlaceholderBytes<sizeof( int8_t  )>  );
    BootstrapRegister<FHashcode, TMappedTo<uint8_t    >>( FHashcode::UsePlaceholderBytes<sizeof(uint8_t  )>  );
    BootstrapRegister<FHashcode, TMappedTo< int16_t   >>( FHashcode::UsePlaceholderBytes<sizeof( int16_t )>  );
    BootstrapRegister<FHashcode, TMappedTo<uint16_t   >>( FHashcode::UsePlaceholderBytes<sizeof(uint16_t )>  );
    BootstrapRegister<FHashcode, TMappedTo< int32_t   >>( FHashcode::UsePlaceholderBytes<sizeof( int32_t )>  );
    BootstrapRegister<FHashcode, TMappedTo<uint32_t   >>( FHashcode::UsePlaceholderBytes<sizeof(uint32_t )>  );
    BootstrapRegister<FHashcode, TMappedTo< int64_t   >>( FHashcode::UsePlaceholderBytes<sizeof( int64_t )>  );
    BootstrapRegister<FHashcode, TMappedTo<uint64_t   >>( FHashcode::UsePlaceholderBytes<sizeof(uint64_t )>  );
    BootstrapRegister<FHashcode, TMappedTo< intGap_t  >>( FHashcode::UsePlaceholderBytes<sizeof( intGap_t)>  );
    BootstrapRegister<FHashcode, TMappedTo<uintGap_t  >>( FHashcode::UsePlaceholderBytes<sizeof(uintGap_t)>  );
    #endif

    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    BootstrapRegister<FHashcode, TMappedTo<wchar      >>( FHashcode::UsePlaceholderBytes<sizeof(wchar    )>  );
    #else
    BootstrapRegister<FHashcode, TMappedTo<char       >>( FHashcode::UsePlaceholderBytes<sizeof(char     )>  );
    BootstrapRegister<FHashcode, TMappedTo<wchar_t    >>( FHashcode::UsePlaceholderBytes<sizeof(wchar_t  )>  );
    BootstrapRegister<FHashcode, TMappedTo<char16_t   >>( FHashcode::UsePlaceholderBytes<sizeof(char16_t )>  );
    BootstrapRegister<FHashcode, TMappedTo<char32_t   >>( FHashcode::UsePlaceholderBytes<sizeof(char32_t )>  );
    #endif


    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    BootstrapRegister<FHashcode, TMappedTo<float      >>( FHashcode::UsePlaceholderBytes<sizeof(float    )>  );
    #endif
    BootstrapRegister<FHashcode, TMappedTo<double     >>( FHashcode::UsePlaceholderBytes<sizeof(double   )>  );
    BootstrapRegister<FHashcode, TMappedTo<long double>>( FHashcode::UsePlaceholderBytes<
                                                        T_SizeInPlaceholder<long double>::value >  );


    // ################################      Equals      ###########################################
    BootstrapRegister<FEquals  , TMappedTo<bool       >>( FEquals::ComparableTypes<bool> );

    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    BootstrapRegister<FEquals  , TMappedTo< integer   >>( FEquals_integer  );
    BootstrapRegister<FEquals  , TMappedTo<uinteger   >>( FEquals_uinteger );
    #else
    BootstrapRegister<FEquals  , TMappedTo< int8_t    >>( FEquals_integer  );
    BootstrapRegister<FEquals  , TMappedTo<uint8_t    >>( FEquals_uinteger );
    BootstrapRegister<FEquals  , TMappedTo< int16_t   >>( FEquals_integer  );
    BootstrapRegister<FEquals  , TMappedTo<uint16_t   >>( FEquals_uinteger );
    BootstrapRegister<FEquals  , TMappedTo< int32_t   >>( FEquals_integer  );
    BootstrapRegister<FEquals  , TMappedTo<uint32_t   >>( FEquals_uinteger );
    BootstrapRegister<FEquals  , TMappedTo< int64_t   >>( FEquals_integer  );
    BootstrapRegister<FEquals  , TMappedTo<uint64_t   >>( FEquals_uinteger );
    BootstrapRegister<FEquals  , TMappedTo< intGap_t  >>( FEquals_integer  );
    BootstrapRegister<FEquals  , TMappedTo<uintGap_t  >>( FEquals_uinteger );
    #endif

    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    BootstrapRegister<FEquals  , TMappedTo<float      >>( FEquals_double   );
    #endif
    BootstrapRegister<FEquals  , TMappedTo<double     >>( FEquals_double   );
    BootstrapRegister<FEquals  , TMappedTo<long double>>( FEquals_double   );


    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    BootstrapRegister<FEquals  , TMappedTo<wchar      >>( FEquals_char );
    #else
    BootstrapRegister<FEquals  , TMappedTo<char       >>( FEquals_char );
    BootstrapRegister<FEquals  , TMappedTo<wchar_t    >>( FEquals_char );
    BootstrapRegister<FEquals  , TMappedTo<char16_t   >>( FEquals_char );
    BootstrapRegister<FEquals  , TMappedTo<char32_t   >>( FEquals_char );
    #endif

    BootstrapRegister<FEquals  , TMappedToArrayOf<nchar>>( FEquals_TChar_Arr<nchar>);
    BootstrapRegister<FEquals  , TMappedToArrayOf<wchar>>( FEquals_TChar_Arr<wchar>);
    BootstrapRegister<FEquals  , TMappedToArrayOf<xchar>>( FEquals_TChar_Arr<xchar>);

    // ################################       IsLess     ###########################################
    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    BootstrapRegister<FIsLess  , TMappedTo< integer   >>( FIsLess_integer );
    BootstrapRegister<FIsLess  , TMappedTo<uinteger   >>( FIsLess_uinteger);
    #else
    BootstrapRegister<FIsLess  , TMappedTo< int8_t    >>( FIsLess_int8_t   );
    BootstrapRegister<FIsLess  , TMappedTo<uint8_t    >>( FIsLess_uint8_t  );
    BootstrapRegister<FIsLess  , TMappedTo< int16_t   >>( FIsLess_int16_t  );
    BootstrapRegister<FIsLess  , TMappedTo<uint16_t   >>( FIsLess_uint16_t );
    BootstrapRegister<FIsLess  , TMappedTo< int32_t   >>( FIsLess_int32_t  );
    BootstrapRegister<FIsLess  , TMappedTo<uint32_t   >>( FIsLess_uint32_t );
    BootstrapRegister<FIsLess  , TMappedTo< int64_t   >>( FIsLess_int64_t  );
    BootstrapRegister<FIsLess  , TMappedTo<uint64_t   >>( FIsLess_uint64_t );
    BootstrapRegister<FIsLess  , TMappedTo< intGap_t  >>( FIsLess_intGap_t );
    BootstrapRegister<FIsLess  , TMappedTo<uintGap_t  >>( FIsLess_uintGap_t);
    #endif

    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    BootstrapRegister<FIsLess  , TMappedTo<float      >>( FIsLess_float   );
    #endif
    BootstrapRegister<FIsLess  , TMappedTo<double     >>( FIsLess_double  );
    BootstrapRegister<FIsLess  , TMappedTo<long double>>( FIsLess_double  );

    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    BootstrapRegister<FIsLess  , TMappedTo<wchar      >>( FIsLess_char );
    #else
    BootstrapRegister<FIsLess  , TMappedTo<char       >>( FIsLess_char );
    BootstrapRegister<FIsLess  , TMappedTo<wchar_t    >>( FIsLess_char );
    BootstrapRegister<FIsLess  , TMappedTo<char16_t   >>( FIsLess_char );
    BootstrapRegister<FIsLess  , TMappedTo<char32_t   >>( FIsLess_char );
    #endif


    // #############################################################################################
    // ##########################       Strings And Boxing       ###################################
    // #############################################################################################
#if ALIB_STRINGS
    // register functions interface implementations
    BootstrapRegister<FIsLess, TMappedToArrayOf<nchar>>( FIsLess_TChar_arr<nchar> );
    BootstrapRegister<FIsLess, TMappedToArrayOf<wchar>>( FIsLess_TChar_arr<wchar> );
    BootstrapRegister<FIsLess, TMappedToArrayOf<xchar>>( FIsLess_TChar_arr<xchar> );

    // register functions of type FAppend
    BootstrapRegisterDefault<FAppend<character     >>( FAppend_Default<character     > );
    BootstrapRegisterDefault<FAppend<complementChar>>( FAppend_Default<complementChar> );
    BootstrapRegisterDefault<FAppend<strangeChar   >>( FAppend_Default<strangeChar   > );

    BootstrapRegister<FAppend<nchar>, TMappedTo<bool     >>(FAppend<nchar>::Appendable<  bool   >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<bool     >>(FAppend<wchar>::Appendable<  bool   >);

    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    BootstrapRegister<FAppend<nchar>, TMappedTo<wchar    >>(FAppend<nchar>::Appendable<  wchar  >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<wchar    >>(FAppend<wchar>::Appendable<  wchar  >);
    #else
    BootstrapRegister<FAppend<nchar>, TMappedTo<nchar    >>(FAppend<nchar>::Appendable<  nchar  >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<nchar    >>(FAppend<wchar>::Appendable<  nchar  >);
    BootstrapRegister<FAppend<nchar>, TMappedTo<wchar    >>(FAppend<nchar>::Appendable<  wchar  >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<wchar    >>(FAppend<wchar>::Appendable<  wchar  >);
    BootstrapRegister<FAppend<nchar>, TMappedTo<xchar    >>(FAppend<nchar>::Appendable<  xchar  >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<xchar    >>(FAppend<wchar>::Appendable<  xchar  >);
    #endif

    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    BootstrapRegister<FAppend<nchar>, TMappedTo<integer  >>(FAppend<nchar>::Appendable<integer  >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<integer  >>(FAppend<wchar>::Appendable<integer  >);
    BootstrapRegister<FAppend<nchar>, TMappedTo<uinteger >>(FAppend<nchar>::Appendable<uinteger >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<uinteger >>(FAppend<wchar>::Appendable<uinteger >);
    #else
    BootstrapRegister<FAppend<nchar>, TMappedTo< int8_t  >>(FAppend<nchar>::Appendable< int8_t  >);
    BootstrapRegister<FAppend<wchar>, TMappedTo< int8_t  >>(FAppend<wchar>::Appendable< int8_t  >);
    BootstrapRegister<FAppend<nchar>, TMappedTo< int16_t >>(FAppend<nchar>::Appendable< int16_t >);
    BootstrapRegister<FAppend<wchar>, TMappedTo< int16_t >>(FAppend<wchar>::Appendable< int16_t >);
    BootstrapRegister<FAppend<nchar>, TMappedTo< int32_t >>(FAppend<nchar>::Appendable< int32_t >);
    BootstrapRegister<FAppend<wchar>, TMappedTo< int32_t >>(FAppend<wchar>::Appendable< int32_t >);
    BootstrapRegister<FAppend<nchar>, TMappedTo< int64_t >>(FAppend<nchar>::Appendable< int64_t >);
    BootstrapRegister<FAppend<wchar>, TMappedTo< int64_t >>(FAppend<wchar>::Appendable< int64_t >);
    BootstrapRegister<FAppend<nchar>, TMappedTo< intGap_t>>(FAppend<nchar>::Appendable< intGap_t>);
    BootstrapRegister<FAppend<wchar>, TMappedTo< intGap_t>>(FAppend<wchar>::Appendable< intGap_t>);
    BootstrapRegister<FAppend<nchar>, TMappedTo<uint8_t  >>(FAppend<nchar>::Appendable<uint8_t  >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<uint8_t  >>(FAppend<wchar>::Appendable<uint8_t  >);
    BootstrapRegister<FAppend<nchar>, TMappedTo<uint16_t >>(FAppend<nchar>::Appendable<uint16_t >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<uint16_t >>(FAppend<wchar>::Appendable<uint16_t >);
    BootstrapRegister<FAppend<nchar>, TMappedTo<uint32_t >>(FAppend<nchar>::Appendable<uint32_t >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<uint32_t >>(FAppend<wchar>::Appendable<uint32_t >);
    BootstrapRegister<FAppend<nchar>, TMappedTo<uint64_t >>(FAppend<nchar>::Appendable<uint64_t >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<uint64_t >>(FAppend<wchar>::Appendable<uint64_t >);
    BootstrapRegister<FAppend<nchar>, TMappedTo<uintGap_t>>(FAppend<nchar>::Appendable<uintGap_t>);
    BootstrapRegister<FAppend<wchar>, TMappedTo<uintGap_t>>(FAppend<wchar>::Appendable<uintGap_t>);
    #endif

    BootstrapRegister<FAppend<nchar>, TMappedTo<double>>(FAppend<nchar>::Appendable<double>);
    BootstrapRegister<FAppend<wchar>, TMappedTo<double>>(FAppend<wchar>::Appendable<double>);
    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    BootstrapRegister<FAppend<nchar>, TMappedTo<float >>(FAppend<nchar>::Appendable<float >);
    BootstrapRegister<FAppend<wchar>, TMappedTo<float >>(FAppend<wchar>::Appendable<float >);
    #endif

    BootstrapRegister<FAppend<nchar>, TMappedToArrayOf<nchar     >>(FAppend_TcharArr<nchar, nchar>);
    BootstrapRegister<FAppend<nchar>, TMappedToArrayOf<wchar     >>(FAppend_TcharArr<wchar, nchar>);
    BootstrapRegister<FAppend<nchar>, TMappedToArrayOf<xchar     >>(FAppend_TcharArr<xchar, nchar>);
    BootstrapRegister<FAppend<wchar>, TMappedToArrayOf<nchar     >>(FAppend_TcharArr<nchar, wchar>);
    BootstrapRegister<FAppend<wchar>, TMappedToArrayOf<wchar     >>(FAppend_TcharArr<wchar, wchar>);
    BootstrapRegister<FAppend<wchar>, TMappedToArrayOf<xchar     >>(FAppend_TcharArr<xchar, wchar>);
    BootstrapRegister<FAppend<xchar>, TMappedToArrayOf<nchar     >>(FAppend_TcharArr<nchar, xchar>);
    BootstrapRegister<FAppend<xchar>, TMappedToArrayOf<wchar     >>(FAppend_TcharArr<wchar, xchar>);
    BootstrapRegister<FAppend<xchar>, TMappedToArrayOf<xchar     >>(FAppend_TcharArr<xchar, xchar>);

    BootstrapRegister<FAppend<nchar>, TMappedTo<std::reference_wrapper<NAString>>>(FAppend<nchar>::WrappedAppendable<NAString>);
    BootstrapRegister<FAppend<nchar>, TMappedTo<std::reference_wrapper<WAString>>>(FAppend<nchar>::WrappedAppendable<WAString>);
    BootstrapRegister<FAppend<nchar>, TMappedTo<std::reference_wrapper<XAString>>>(FAppend<nchar>::WrappedAppendable<XAString>);
    BootstrapRegister<FAppend<wchar>, TMappedTo<std::reference_wrapper<NAString>>>(FAppend<wchar>::WrappedAppendable<NAString>);
    BootstrapRegister<FAppend<wchar>, TMappedTo<std::reference_wrapper<WAString>>>(FAppend<wchar>::WrappedAppendable<WAString>);
    BootstrapRegister<FAppend<wchar>, TMappedTo<std::reference_wrapper<XAString>>>(FAppend<wchar>::WrappedAppendable<XAString>);
    BootstrapRegister<FAppend<xchar>, TMappedTo<std::reference_wrapper<NAString>>>(FAppend<xchar>::WrappedAppendable<NAString>);
    BootstrapRegister<FAppend<xchar>, TMappedTo<std::reference_wrapper<WAString>>>(FAppend<xchar>::WrappedAppendable<WAString>);
    BootstrapRegister<FAppend<xchar>, TMappedTo<std::reference_wrapper<XAString>>>(FAppend<xchar>::WrappedAppendable<XAString>);

    // CodeMarker_CommonEnums
    #if ALIB_CAMP
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Alignment        )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Bool             )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Caching          )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Case             )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::ContainerOp      )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::CreateDefaults   )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::CreateIfNotExists)
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::CurrentData      )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Inclusion        )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Initialization   )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Phase            )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Propagation      )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Reach            )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Recursive        )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Responsibility   )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Safeness         )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Side             )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::SortOrder        )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::SourceData       )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Switch           )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Timezone         )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Timing           )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::ValueReference   )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::Whitespaces      )
    #endif

    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( strings::util::Token* )

    #if ALIB_DEBUG
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( std::type_info* )
    #endif


#endif // ALIB_STRINGS

}


// Box::dbgCheckRegistration
// Note: this method has to go here, to be able to check if boxing was bootstrapped already.
//       If not, no check is performed.
#if ALIB_DEBUG_BOXING

void detail::DbgCheckIsInitialized()
{
    // ERROR: A global or static instance of class Box is created and initialized to a
    //        mapped type  that uses a dynamic vtable. This is forbidden.
    //        See chapter "12.4 Global And Static Box Instances" of the Programmer's Manual
    //        of module ALib Boxing, for more information.
    assert( initFlag == 0x92A3EF61 ); // magic number
}

void detail::DbgCheckRegistration( detail::VTable* vtable, bool increaseUsageCounter )
{
    if( vtable==nullptr)
        return;

    if( increaseUsageCounter )
        ++vtable->DbgCntUsage;

    if( initFlag== 0 || vtable->DbgProduction != detail::VTable::DbgFactoryType::Unregistered )
        return;

    if( !vtable->IsArray() )
    {
        lang::DbgTypeDemangler type( vtable->Type );
        ALIB_ERROR( "BOXING", "Static VTable of mapped type <", type.Get(),
         "> not registered. Use Macro ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER with bootstrapping." )
    }
    else
    {
        lang::DbgTypeDemangler type( vtable->ElementType );
        ALIB_ERROR( "BOXING", "Static VTable of mapped type <", type.Get(),
         "[]> not registered. Use Macro ALIB_BOXING_REGISTER_MAPPED_ARRAY_TYPE with bootstrapping." )
    }
}
#endif

}} // namespace [alib::boxing]
ALIB_WARNINGS_RESTORE
