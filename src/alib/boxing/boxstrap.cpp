// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include <cmath>
#include <functional>
#include <cstring>
#include <typeindex>
#include <span>

#   include "ALib.Lang.H"
#   include "ALib.Characters.Functions.H"
#   include "ALib.Time.H"
#   include "ALib.Threads.H"
#   include "ALib.Monomem.H"
#   include "ALib.Strings.H"
#   include "ALib.Strings.Token.H"
#   include "ALib.Singletons.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.ThreadModel.H"
#   include "ALib.Exceptions.H"
#   include "ALib.System.H"
#   include "ALib.Format.H"
#   include "ALib.Format.StdImplementation.H"
#   include "ALib.Format.FormatterPythonStyle.H"
#   include "ALib.Format.FormatterJavaStyle.H"
#   include "ALib.Variables.H"
#   include "ALib.Variables.IniFile.H"
#   include "ALib.BitBuffer.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.Bootstrap.H"
#   include "ALib.CLI.H"
#   include "ALib.Expressions.H"
#   include "ALib.ALox.Impl.H"
#   include "ALib.Files.H"

#if ALIB_DEBUG && !DOXYGEN
namespace alib::boxing::debug {
    // This is used by boxing::Bootstrap to do a runtime-check for compatibility of boxing
    // and long double values.
    // It was put here to prevent the compiler to optimize and remove the code.
    extern  long double LONGDOUBLE_WRITE_TEST_MEM[2];
    extern  void LongDoubleTrueLengthSet();
    extern  bool LongDoubleTrueLengthTest();
}
#endif

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Boxing;
#endif
// ======================================   Implementation   =======================================

// #################################################################################################
// Anonymous methods needed for Bootstrap()
// #################################################################################################
#if !DOXYGEN

namespace alib::boxing {
namespace {

bool FIsNotNull_Default( const Box& box )
{
    return !(    (box.IsArray()    && box.UnboxLength() == 0               )
              || (box.IsPointer()  && box.Data().PointerPair.P1 == nullptr )
            );
}

std::size_t FHashcode_Default( const Box& self )
{
    if( self.IsPointer() )
    {
        return   size_t(0xa814e72cUL)
               + size_t( self.TypeID().hash_code() )
               + self.Data().Integrals.UArray[0] * 89047023;
    }

    if( self.IsEnum() )
    {
        return   size_t(0x49a024efUL)
               + size_t( self.TypeID().hash_code() )
               + self.Data().Integrals.UArray[0] * 79204799;
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

            std::span<uint16_t> array= {self.Data().GetPointer<uint16_t>(), size_t(length) };
            for( uint16_t v : array )
                result = 67 * result + v;

            return size_t( result );
        }

        if( size == 4 )
        {
            std::span<uint32_t> array= {self.Data().GetPointer<uint32_t>(), size_t(length) };
            for( uint32_t v : array )
                result = 67*result + v;

            return size_t( result );
        }

        if( size == 8 )
        {
            std::span<uint64_t> array= {self.Data().GetPointer<uint64_t>(), size_t(length) };
            for( uint64_t v : array )
                result = std::size_t( 67 * result + v );

            return std::size_t( result );
        }

        // any other size
        {
            length*= size;
            std::span<unsigned char> array= {self.Data().GetPointer<unsigned char>(), size_t(length) };
            for( unsigned char v : array )
                result = 67 * result + v;

            return size_t( result );
        }
    }

    //--- default (value types) ---
    size_t result=  size_t(0xcf670957UL)
        + size_t( self.TypeID().hash_code() );

    unsigned int usedLen= self.GetPlaceholderUsageLength();

    constexpr uinteger Bit1= 1;

    // smaller than first "word"
    if( usedLen < sizeof( uinteger ) )
        return size_t( (  self.Data().Integrals.UArray[0]
                                      & ((Bit1 << (usedLen * 8) )- 1)   )   * 32194735 )
                  + result;

    // add first word
    result+= self.Data().Integrals.UArray[0] * 32194735;

    if( usedLen == sizeof(uinteger) )
        return result;

    // tests if smaller than second "word"
    if( usedLen - sizeof( uinteger ) < sizeof( uinteger ) )
    {
        return size_t( (   self.Data().Integrals.UArray[1]
                                      & ((Bit1 << ((usedLen - sizeof(uinteger)) * 8) )- 1)   )   * 321947 )
                  + result;
    }

    return  result + self.Data().Integrals.UArray[1] * 321947;


}

bool FEquals_Default( const Box& self, const Box& rhs )
{
    if( !self.IsSameType( rhs ) )
        return false;

    // array types?
    if( self.IsArray() )
    {
        // different in type, length or nulled?
        if(    self.UnboxLength() != rhs.UnboxLength()
            || (    ( self.Data().GetPointer<char>() == nullptr)
                 != ( rhs .Data().GetPointer<char>() == nullptr  ) )     )
            return false;

        return    self.Data().GetPointer<char>() == nullptr
               || self.UnboxLength()  == 0
               || self.Data().GetPointer<char>() == rhs.Data().GetPointer<char>()
               || memcmp( self.Data().GetPointer<char>(),
                          rhs .Data().GetPointer<char>(),
                          static_cast     <size_t>( self.UnboxLength() ) * self.ArrayElementSize()
                         ) == 0;
    }

    // non-array types
    unsigned int usedLen= self.GetPlaceholderUsageLength();

    constexpr uinteger Bit1= uinteger( 1 );
    // tests if smaller than first "word"
    if( usedLen < sizeof( uinteger ) )
    {
        uinteger mask=  (Bit1 << (usedLen * 8) )- 1;
        return      ( self.Data().Integrals.UArray[0] & mask )
                ==  ( rhs .Data().Integrals.UArray[0] & mask );
    }

    // test first word
    if( self.Data().Integrals.UArray[0]  !=  rhs.Data().Integrals.UArray[0] )
        return false;

    if( usedLen == sizeof( uinteger ) )
        return true;

    // tests if smaller than second "word"
    if( usedLen - sizeof( uinteger ) < sizeof( uinteger ) )
    {
        uinteger mask=  (Bit1 << (usedLen - sizeof(uinteger)) * 8 )- 1;
        return      ( self.Data().Integrals.UArray[1] & mask )
                ==  ( rhs .Data().Integrals.UArray[1] & mask );
    }

    return  self.Data().Integrals.UArray[1]  ==  rhs.Data().Integrals.UArray[1];
}


bool FEquals_double( const Box& self, const Box& rhsBox )
{
    double lhs= self.UnboxFloatingPoint();
    double rhs;
         if( rhsBox.IsFloatingPoint   () )  rhs=         rhsBox.UnboxFloatingPoint   ()  ;
    else if( rhsBox.IsSignedIntegral  () )  rhs= double( rhsBox.UnboxSignedIntegral  () );
    else if( rhsBox.IsUnsignedIntegral() )  rhs= double( rhsBox.UnboxUnsignedIntegral() );
    else                                    return false;

    // we can ignore this warning, because we fixed it with the second test
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wfloat-equal"
    #endif

    return      lhs == rhs
            // take rounding errors into account.
            // We use the "float-epsilon" and double it to be even a little weaker!
            ||  std::fabs( lhs - rhs ) <= double( 2.0f * std::numeric_limits<float>::epsilon() );

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
         if( rhsBox.IsSignedIntegral  () )  rhs= uinteger( rhsBox.UnboxSignedIntegral  () );
    else if( rhsBox.IsUnsignedIntegral() )  rhs=           rhsBox.UnboxUnsignedIntegral()  ;
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

    const TChar* compBuf= rhs.Data().GetPointer<TChar>();
    const TChar*  boxBuf= lhs.Data().GetPointer<TChar>();
    if ( (boxBuf == nullptr) != (compBuf == nullptr) )
            return false;

    integer compLen= rhs.UnboxLength();
    integer boxLen=  lhs.UnboxLength();
    if ( boxLen != compLen )
        return false;

    return  0 == characters::Compare <TChar>( boxBuf, compBuf, boxLen );
}

bool FIsLess_Default( const Box& box, const Box& comp )
{
    return           std::type_index( box.TypeID() ) < std::type_index(comp.TypeID() )
            ||  (    box.TypeID() == comp.TypeID()
                   &&box.Data().Integrals.UArray[0]   <  comp.Data().Integrals.UArray[0]                    );

}

#if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
bool FIsLess_integer( const Box& self, const Box& rhs )
{
    auto lhs= self.Data().Integrals.Array[0];
    if( rhs.IsSameType( self ) )    return         lhs   <          rhs.Unbox<integer >   ();
    if( rhs.IsType<uinteger>() )    return         lhs   <  integer(rhs.Unbox<uinteger>   ());
    if( rhs.IsFloatingPoint()  )    return double( lhs ) <          rhs.UnboxFloatingPoint();

    return std::type_index( self.TypeID() ) < std::type_index( rhs.TypeID() );
}

bool FIsLess_uinteger( const Box& self, const Box& rhs )
{
    auto lhs= self.Data().Integrals.UArray[0];
    if( rhs.IsSameType( self ) )    return          lhs   <  rhs.Data().Integrals.UArray[0]  ;
    if( rhs.IsType<integer> () )    return integer( lhs ) <  rhs.Unbox<integer>();
    if( rhs.IsFloatingPoint()  )    return double ( lhs ) <  rhs.UnboxFloatingPoint();

    return std::type_index( self.TypeID() ) < std::type_index( rhs.TypeID() );
}

#else

bool helperBijectiveLessS( integer selfVal, const Box& selfType, const Box& rhs )
{
    if( rhs.IsSignedIntegral  () )  return selfVal           <         rhs.UnboxSignedIntegral  () ;
    if( rhs.IsUnsignedIntegral() )  return selfVal           < integer(rhs.UnboxUnsignedIntegral());
    if( rhs.IsFloatingPoint   () )  return double( selfVal ) <         rhs.UnboxFloatingPoint   () ;

    // no number?
    return std::type_index( selfType.TypeID() ) < std::type_index( rhs.TypeID() );
}

bool helperBijectiveLessU( uinteger selfVal, const Box& selfType, const Box& rhs )
{
    if( rhs.IsSignedIntegral  () ) return selfVal         < uinteger(rhs.UnboxSignedIntegral  ());
    if( rhs.IsUnsignedIntegral() ) return selfVal         <          rhs.UnboxUnsignedIntegral() ;
    if( rhs.IsFloatingPoint   () ) return double(selfVal) <          rhs.UnboxFloatingPoint   () ;

    // no number?
    return std::type_index( selfType.TypeID() ) < std::type_index( rhs.TypeID() );
}

bool FIsLess_int8_t   ( const Box& self, const Box& rhs )  { return helperBijectiveLessS(  integer( self.Unbox< int8_t  >() ), self, rhs ); }
bool FIsLess_int16_t  ( const Box& self, const Box& rhs )  { return helperBijectiveLessS(  integer( self.Unbox< int16_t >() ), self, rhs ); }
bool FIsLess_int32_t  ( const Box& self, const Box& rhs )  { return helperBijectiveLessS(  integer( self.Unbox< int32_t >() ), self, rhs ); }
bool FIsLess_int64_t  ( const Box& self, const Box& rhs )  { return helperBijectiveLessS(  integer( self.Unbox< int64_t >() ), self, rhs ); }
bool FIsLess_intGap_t ( const Box& self, const Box& rhs )  { return helperBijectiveLessS(  integer( self.Unbox< intGap_t>() ), self, rhs ); }

bool FIsLess_uint8_t  ( const Box& self, const Box& rhs )  { return helperBijectiveLessU( uinteger( self.Unbox<uint8_t  >() ), self, rhs ); }
bool FIsLess_uint16_t ( const Box& self, const Box& rhs )  { return helperBijectiveLessU( uinteger( self.Unbox<uint16_t >() ), self, rhs ); }
bool FIsLess_uint32_t ( const Box& self, const Box& rhs )  { return helperBijectiveLessU( uinteger( self.Unbox<uint32_t >() ), self, rhs ); }
bool FIsLess_uint64_t ( const Box& self, const Box& rhs )  { return helperBijectiveLessU( uinteger( self.Unbox<uint64_t >() ), self, rhs ); }
bool FIsLess_uintGap_t( const Box& self, const Box& rhs )  { return helperBijectiveLessU( uinteger( self.Unbox<uintGap_t>() ), self, rhs ); }

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
    if( rhs.IsFloatingPoint   () )    return    lhs  <         rhs.UnboxFloatingPoint   ()  ;
    if( rhs.IsSignedIntegral  () )    return    lhs  < double( rhs.UnboxSignedIntegral  () );
    if( rhs.IsUnsignedIntegral() )    return    lhs  < double( rhs.UnboxUnsignedIntegral() );

    return std::type_index( self.TypeID() ) < std::type_index( rhs.TypeID() );
}

#if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    bool FIsLess_float( const Box& self, const Box& rhs )
    {
        float lhs= self.Unbox<float>();
        if( rhs.IsType<float     >() )    return    lhs  <         rhs.Unbox<float >()  ;
        if( rhs.IsType<double    >() )    return    lhs  <  float( rhs.Unbox<double>() );
        if( rhs.IsSignedIntegral  () )    return    lhs  <  float( rhs.UnboxSignedIntegral  () );
        if( rhs.IsUnsignedIntegral() )    return    lhs  <  float( rhs.UnboxUnsignedIntegral() );

        return std::type_index( self.TypeID() ) < std::type_index( rhs.TypeID() );
    }
#endif



#if ALIB_MONOMEM

void FClone_Default( Box& self, MonoAllocator& memory)
{
    if ( !self.IsArray() || self.UnboxLength() == 0)
        return;

    Placeholder& placeHolder= self.Data();
    auto* src= placeHolder.GetPointer<char>();
    if( src== nullptr || placeHolder.Integrals.Array[1] < 0 )
        return;

    size_t alignment= self.ArrayElementSize();
    if( alignment > sizeof(std::ptrdiff_t) )
        alignment=  sizeof(std::ptrdiff_t);


    placeHolder.SetPointer( memory().Alloc( self.ArrayElementSize() * placeHolder.Integrals.UArray[1],
                                            alignment ) );
    memcpy( placeHolder.GetPointer<char>(), src, self.ArrayElementSize() * placeHolder.Integrals.UArray[1] );
}
#endif

// #################################################################################################
bool FIsTrue_Default( const Box& self )
{
    if( self.IsArray() )
        return  self.UnboxLength() != 0;

    // non-array types
    unsigned int usedLen= self.GetPlaceholderUsageLength();

    constexpr uinteger Bit1= uinteger( 1 );
    // tests if smaller than first "word"
    if( usedLen < sizeof( uinteger ) )
        return   (   self.Data().Integrals.UArray[0]
                   & ((Bit1 << (usedLen * 8) )- 1)   )  !=  0;

    // test first word
    if( self.Data().Integrals.UArray[0]  !=  0 )
        return true;

    if( usedLen == sizeof( uinteger ) )
        return false;

    // tests if smaller than second "word"
    if( usedLen - sizeof( uinteger ) < sizeof( uinteger ) )
    {
        return      (   self.Data().Integrals.UArray[1]
                      & ((Bit1 << (usedLen - sizeof(uinteger)) * 8 )- 1) ) != 0;
    }

    return  self.Data().Integrals.UArray[1]  !=  0;
}


#if ALIB_STRINGS
template<typename TChar>
bool FIsLess_TChar_arr( const Box& lhs, const Box& rhs )
{
    if( rhs.IsArrayOf<TChar>() )
        return lhs.Unbox<strings::TString<TChar>>() < rhs.Unbox<strings::TString<TChar>>();

    return std::type_index( lhs.TypeID() ) < std::type_index(rhs.TypeID() );
}


template<typename TChar, typename TAllocator>
void FAppend_Default( const Box& self, strings::TAString<TChar,TAllocator>& target)
{
    if( self.IsPointer() )
    {
        target << ALIB_REL_DBG(  "PointerType", self.TypeID() )
               << '(' << strings::TNumberFormat<TChar>::Computational.HexLiteralPrefix
               <<  strings::THex<TChar>( self.Data().Integrals.UArray[0] )
               << ')';
        return;
    }

    if( self.IsEnum() )
    {
        target << ALIB_REL_DBG(  "EnumType", self.TypeID() )   <<  '(' << self.Data().Integrals.Array[0] <<  ')';
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


template<typename TCharSrc, typename TChar, typename TAllocator>
void FAppend_TcharArr( const Box& box, strings::TAString<TChar,TAllocator>& target )
{
    target.template Append<NC>( box.UnboxArray<TCharSrc>(), box.UnboxLength() );
}

#endif

} // anonymous namespace
} // namespace [alib::boxing]
#endif //!DOXYGEN

// #################################################################################################
// Bootstrap()
// #################################################################################################
#   include "ALib.Lang.CIFunctions.H"

namespace alib::boxing{
#if ALIB_DEBUG && !DOXYGEN
namespace{ unsigned int initFlag= 0; }
#endif // !DOXYGEN

void shutdown() {
    ALIB_ASSERT_ERROR( initFlag == 0x92A3EF61, "BOXING", "Not initialized when calling shutdown." )
    ALIB_DBG(initFlag= 1);
    detail::FunctionTable::Shutdown();
    detail::FunctionTable::Shutdown();
}

void bootstrap()
{
    ALIB_ASSERT_ERROR( initFlag == 0, "BOXING", "This method must not be invoked twice." )
    ALIB_DBG(initFlag= 0x92A3EF61);

    //############### Debug-compilation checks ################
    #if ALIB_DEBUG
        // check size of long double
        {
            debug::LongDoubleTrueLengthSet();
            debug::LONGDOUBLE_WRITE_TEST_MEM[0]= 0.0L;
            ALIB_ASSERT_ERROR( debug::LongDoubleTrueLengthTest(), "BOXING",
            "Platform not supported. SizeTraits<long double> contains wrong size" )
        }
    #endif

    //#############################     BootstrapRegister Static VTables    #################################
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_voidP    )
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_boxes    )
#if ALIB_MONOMEM
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_boxesma  )
#endif
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_boxarray )

DOX_MARKER([DOX_BOXING_OPTIMIZE_REGISTER_1])
ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_bool )
DOX_MARKER([DOX_BOXING_OPTIMIZE_REGISTER_1])

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
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_char8_t     )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_char16_t    )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_char32_t    )
    #endif

DOX_MARKER([DOX_BOXING_OPTIMIZE_REGISTER_2])
ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_arr_char )
DOX_MARKER([DOX_BOXING_OPTIMIZE_REGISTER_2])

    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_arr_wchar_t   )
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_arr_char8_t   )
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_arr_char16_t  )
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_arr_char32_t  )

    // Static VTables for standard types
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_std_type_info )

    // CodeMarker_CommonEnums
    // Static VTables for low-level ALib types
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

    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lang_callerinfo        )
    #if !ALIB_SINGLE_THREADED
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_threads_tp           )
    #endif
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_time_ticks             )
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_time_ticks_duration    )
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_time_datetime          )
    ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_time_datetime_duration )


    #if ALIB_STRINGS
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_wrapped_tanstring )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_wrapped_tawstring )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_wrapped_taxstring )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_strings_token )
    #endif

    #if ALIB_SYSTEM
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_system_systemerrors )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_system_path         )
    #endif
    #if ALIB_EXCEPTIONS
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_alib_exception      )
    #endif
    #if ALIB_VARIABLES
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_config_priorities   )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_config_exceptions   )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_config_variable       )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_config_constcursor  )
    #endif
    #if ALIB_FORMAT
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_system_fmtexceptions)
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lang_format_bytesize_iec )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lang_format_bytesize_si  )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lang_format_bytesize_units )
    #endif

    #if ALIB_CLI
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_cli_exceptions )
    #endif

    #if ALIB_ALOX
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_verbosity       )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_scope           )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_logger          )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_pair_verby_prio )
    #endif

    #if ALIB_EXPRESSIONS
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_expressions_exceptions )
    #endif

    #if ALIB_FILES
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_cursor       )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_perms  )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_type   )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_type1  )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_type2  )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_type3  )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_qual   )
      ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_qual3  )
    #endif

    //########################       Register default implementations        #######################
    BootstrapRegisterDefault<FIsTrue    >( FIsTrue_Default    );
    BootstrapRegisterDefault<FIsNotNull >( FIsNotNull_Default );
    BootstrapRegisterDefault<FIsLess    >( FIsLess_Default    );
    BootstrapRegisterDefault<FHashcode  >( FHashcode_Default  );
    BootstrapRegisterDefault<FEquals    >( FEquals_Default    );
IF_ALIB_MONOMEM(
    BootstrapRegisterDefault<FClone     >( FClone_Default     ); )

    // ################################      IsNotNull    ##########################################
    BootstrapRegister<FIsNotNull, bool      >( FIsNotNull::ConstantTrue  );
    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    BootstrapRegister<FIsNotNull,  integer  >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, uinteger  >( FIsNotNull::ConstantTrue  );
    #else
    BootstrapRegister<FIsNotNull,  int8_t   >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, uint8_t   >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull,  int16_t  >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, uint16_t  >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull,  int32_t  >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, uint32_t  >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull,  int64_t  >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, uint64_t  >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull,  intGap_t >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, uintGap_t >( FIsNotNull::ConstantTrue  );
    #endif

    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    BootstrapRegister<FIsNotNull, wchar     >( FIsNotNull::ConstantTrue  );
    #else
    BootstrapRegister<FIsNotNull, char      >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, wchar_t   >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, char8_t   >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, char16_t  >( FIsNotNull::ConstantTrue  );
    BootstrapRegister<FIsNotNull, char32_t  >( FIsNotNull::ConstantTrue  );
    #endif


    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    BootstrapRegister<FIsNotNull, float     >( FIsNotNull::ConstantTrue   );
    #endif
    BootstrapRegister<FIsNotNull, double    >( FIsNotNull::ConstantTrue   );

    // ################################      Hashcode     ##########################################
    BootstrapRegister<FHashcode, bool       >( FHashcode::UsePlaceholderBytes<sizeof(bool     )>  );
    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    BootstrapRegister<FHashcode,  integer   >( FHashcode::UsePlaceholderBytes<sizeof( integer )>  );
    BootstrapRegister<FHashcode, uinteger   >( FHashcode::UsePlaceholderBytes<sizeof(uinteger )>  );
    #else
    BootstrapRegister<FHashcode,  int8_t    >( FHashcode::UsePlaceholderBytes<sizeof( int8_t  )>  );
    BootstrapRegister<FHashcode, uint8_t    >( FHashcode::UsePlaceholderBytes<sizeof(uint8_t  )>  );
    BootstrapRegister<FHashcode,  int16_t   >( FHashcode::UsePlaceholderBytes<sizeof( int16_t )>  );
    BootstrapRegister<FHashcode, uint16_t   >( FHashcode::UsePlaceholderBytes<sizeof(uint16_t )>  );
    BootstrapRegister<FHashcode,  int32_t   >( FHashcode::UsePlaceholderBytes<sizeof( int32_t )>  );
    BootstrapRegister<FHashcode, uint32_t   >( FHashcode::UsePlaceholderBytes<sizeof(uint32_t )>  );
    BootstrapRegister<FHashcode,  int64_t   >( FHashcode::UsePlaceholderBytes<sizeof( int64_t )>  );
    BootstrapRegister<FHashcode, uint64_t   >( FHashcode::UsePlaceholderBytes<sizeof(uint64_t )>  );
    BootstrapRegister<FHashcode,  intGap_t  >( FHashcode::UsePlaceholderBytes<sizeof( intGap_t)>  );
    BootstrapRegister<FHashcode, uintGap_t  >( FHashcode::UsePlaceholderBytes<sizeof(uintGap_t)>  );
    #endif

    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    BootstrapRegister<FHashcode, wchar      >( FHashcode::UsePlaceholderBytes<sizeof(wchar    )>  );
    #else
    BootstrapRegister<FHashcode, char       >( FHashcode::UsePlaceholderBytes<sizeof(char     )>  );
    BootstrapRegister<FHashcode, wchar_t    >( FHashcode::UsePlaceholderBytes<sizeof(wchar_t  )>  );
    BootstrapRegister<FHashcode, char8_t    >( FHashcode::UsePlaceholderBytes<sizeof(char16_t )>  );
    BootstrapRegister<FHashcode, char16_t   >( FHashcode::UsePlaceholderBytes<sizeof(char16_t )>  );
    BootstrapRegister<FHashcode, char32_t   >( FHashcode::UsePlaceholderBytes<sizeof(char32_t )>  );
    #endif


    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    BootstrapRegister<FHashcode, float      >( FHashcode::UsePlaceholderBytes<sizeof(float    )>  );
    #endif
    BootstrapRegister<FHashcode, double     >( FHashcode::UsePlaceholderBytes<sizeof(double   )>  );
    BootstrapRegister<FHashcode, long double>( FHashcode::UsePlaceholderBytes<SizeTraits<long double>>  );


    // ################################      Equals      ###########################################
    BootstrapRegister<FEquals  , bool       >( FEquals::ComparableTypes<bool> );

    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    BootstrapRegister<FEquals  ,  integer   >( FEquals_integer  );
    BootstrapRegister<FEquals  , uinteger   >( FEquals_uinteger );
    #else
    BootstrapRegister<FEquals  ,  int8_t    >( FEquals_integer  );
    BootstrapRegister<FEquals  , uint8_t    >( FEquals_uinteger );
    BootstrapRegister<FEquals  ,  int16_t   >( FEquals_integer  );
    BootstrapRegister<FEquals  , uint16_t   >( FEquals_uinteger );
    BootstrapRegister<FEquals  ,  int32_t   >( FEquals_integer  );
    BootstrapRegister<FEquals  , uint32_t   >( FEquals_uinteger );
    BootstrapRegister<FEquals  ,  int64_t   >( FEquals_integer  );
    BootstrapRegister<FEquals  , uint64_t   >( FEquals_uinteger );
    BootstrapRegister<FEquals  ,  intGap_t  >( FEquals_integer  );
    BootstrapRegister<FEquals  , uintGap_t  >( FEquals_uinteger );
    #endif

    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    BootstrapRegister<FEquals  , float      >( FEquals_double   );
    #endif
    BootstrapRegister<FEquals  , double     >( FEquals_double   );
    BootstrapRegister<FEquals  , long double>( FEquals_double   );


    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    BootstrapRegister<FEquals  , wchar      >( FEquals_char );
    #else
    BootstrapRegister<FEquals  , char       >( FEquals_char );
    BootstrapRegister<FEquals  , wchar_t    >( FEquals_char );
    BootstrapRegister<FEquals  , char8_t    >( FEquals_char );
    BootstrapRegister<FEquals  , char16_t   >( FEquals_char );
    BootstrapRegister<FEquals  , char32_t   >( FEquals_char );
    #endif

    BootstrapRegister<FEquals  , nchar,true >( FEquals_TChar_Arr<nchar>);
    BootstrapRegister<FEquals  , wchar,true >( FEquals_TChar_Arr<wchar>);
    BootstrapRegister<FEquals  , xchar,true >( FEquals_TChar_Arr<xchar>);

    // ################################       IsLess     ###########################################
    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    BootstrapRegister<FIsLess  ,  integer   >( FIsLess_integer );
    BootstrapRegister<FIsLess  , uinteger   >( FIsLess_uinteger);
    #else
    BootstrapRegister<FIsLess  ,  int8_t    >( FIsLess_int8_t   );
    BootstrapRegister<FIsLess  , uint8_t    >( FIsLess_uint8_t  );
    BootstrapRegister<FIsLess  ,  int16_t   >( FIsLess_int16_t  );
    BootstrapRegister<FIsLess  , uint16_t   >( FIsLess_uint16_t );
    BootstrapRegister<FIsLess  ,  int32_t   >( FIsLess_int32_t  );
    BootstrapRegister<FIsLess  , uint32_t   >( FIsLess_uint32_t );
    BootstrapRegister<FIsLess  ,  int64_t   >( FIsLess_int64_t  );
    BootstrapRegister<FIsLess  , uint64_t   >( FIsLess_uint64_t );
    BootstrapRegister<FIsLess  ,  intGap_t  >( FIsLess_intGap_t );
    BootstrapRegister<FIsLess  , uintGap_t  >( FIsLess_uintGap_t);
    #endif

    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    BootstrapRegister<FIsLess  , float      >( FIsLess_float   );
    #endif
    BootstrapRegister<FIsLess  , double     >( FIsLess_double  );
    BootstrapRegister<FIsLess  , long double>( FIsLess_double  );

    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    BootstrapRegister<FIsLess  , wchar      >( FIsLess_char );
    #else
    BootstrapRegister<FIsLess  , char       >( FIsLess_char );
    BootstrapRegister<FIsLess  , wchar_t    >( FIsLess_char );
    BootstrapRegister<FIsLess  , char8_t    >( FIsLess_char );
    BootstrapRegister<FIsLess  , char16_t   >( FIsLess_char );
    BootstrapRegister<FIsLess  , char32_t   >( FIsLess_char );
    #endif


    // #############################################################################################
    // ##########################       Strings And Boxing       ###################################
    // #############################################################################################
#if ALIB_STRINGS
    // register functions interface implementations
    BootstrapRegister<FIsLess, nchar,true >( FIsLess_TChar_arr<nchar> );
    BootstrapRegister<FIsLess, wchar,true >( FIsLess_TChar_arr<wchar> );
    BootstrapRegister<FIsLess, xchar,true >( FIsLess_TChar_arr<xchar> );

    // register functions of type FAppend
    BootstrapRegisterDefault<FAppend<character     , lang::HeapAllocator>>( FAppend_Default<character     , lang::HeapAllocator> );
    BootstrapRegisterDefault<FAppend<complementChar, lang::HeapAllocator>>( FAppend_Default<complementChar, lang::HeapAllocator> );
    BootstrapRegisterDefault<FAppend<strangeChar   , lang::HeapAllocator>>( FAppend_Default<strangeChar   , lang::HeapAllocator> );

    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, bool     >(FAppend<nchar, lang::HeapAllocator>::Appendable<  bool   >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, bool     >(FAppend<wchar, lang::HeapAllocator>::Appendable<  bool   >);

    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, wchar    >(FAppend<nchar, lang::HeapAllocator>::Appendable<  wchar  >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, wchar    >(FAppend<wchar, lang::HeapAllocator>::Appendable<  wchar  >);
    #else
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, nchar    >(FAppend<nchar, lang::HeapAllocator>::Appendable<  nchar  >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, nchar    >(FAppend<wchar, lang::HeapAllocator>::Appendable<  nchar  >);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, wchar    >(FAppend<nchar, lang::HeapAllocator>::Appendable<  wchar  >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, wchar    >(FAppend<wchar, lang::HeapAllocator>::Appendable<  wchar  >);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, xchar    >(FAppend<nchar, lang::HeapAllocator>::Appendable<  xchar  >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, xchar    >(FAppend<wchar, lang::HeapAllocator>::Appendable<  xchar  >);
    #endif

    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, integer  >(FAppend<nchar, lang::HeapAllocator>::Appendable<integer  >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, integer  >(FAppend<wchar, lang::HeapAllocator>::Appendable<integer  >);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, uinteger >(FAppend<nchar, lang::HeapAllocator>::Appendable<uinteger >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, uinteger >(FAppend<wchar, lang::HeapAllocator>::Appendable<uinteger >);
    #else
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>,  int8_t  >(FAppend<nchar, lang::HeapAllocator>::Appendable< int8_t  >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>,  int8_t  >(FAppend<wchar, lang::HeapAllocator>::Appendable< int8_t  >);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>,  int16_t >(FAppend<nchar, lang::HeapAllocator>::Appendable< int16_t >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>,  int16_t >(FAppend<wchar, lang::HeapAllocator>::Appendable< int16_t >);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>,  int32_t >(FAppend<nchar, lang::HeapAllocator>::Appendable< int32_t >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>,  int32_t >(FAppend<wchar, lang::HeapAllocator>::Appendable< int32_t >);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>,  int64_t >(FAppend<nchar, lang::HeapAllocator>::Appendable< int64_t >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>,  int64_t >(FAppend<wchar, lang::HeapAllocator>::Appendable< int64_t >);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>,  intGap_t>(FAppend<nchar, lang::HeapAllocator>::Appendable< intGap_t>);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>,  intGap_t>(FAppend<wchar, lang::HeapAllocator>::Appendable< intGap_t>);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, uint8_t  >(FAppend<nchar, lang::HeapAllocator>::Appendable<uint8_t  >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, uint8_t  >(FAppend<wchar, lang::HeapAllocator>::Appendable<uint8_t  >);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, uint16_t >(FAppend<nchar, lang::HeapAllocator>::Appendable<uint16_t >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, uint16_t >(FAppend<wchar, lang::HeapAllocator>::Appendable<uint16_t >);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, uint32_t >(FAppend<nchar, lang::HeapAllocator>::Appendable<uint32_t >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, uint32_t >(FAppend<wchar, lang::HeapAllocator>::Appendable<uint32_t >);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, uint64_t >(FAppend<nchar, lang::HeapAllocator>::Appendable<uint64_t >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, uint64_t >(FAppend<wchar, lang::HeapAllocator>::Appendable<uint64_t >);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, uintGap_t>(FAppend<nchar, lang::HeapAllocator>::Appendable<uintGap_t>);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, uintGap_t>(FAppend<wchar, lang::HeapAllocator>::Appendable<uintGap_t>);
    #endif

    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, double>(FAppend<nchar, lang::HeapAllocator>::Appendable<double>);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, double>(FAppend<wchar, lang::HeapAllocator>::Appendable<double>);
    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, float >(FAppend<nchar, lang::HeapAllocator>::Appendable<float >);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, float >(FAppend<wchar, lang::HeapAllocator>::Appendable<float >);
    #endif
    if constexpr (sizeof(long double) <= sizeof(Placeholder) )
    {
        BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, long double>(FAppend<nchar, lang::HeapAllocator>::Appendable<long double>);
        BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, long double>(FAppend<wchar, lang::HeapAllocator>::Appendable<long double>);
    }
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, nchar     ,true>(FAppend_TcharArr<nchar, nchar, lang::HeapAllocator>);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, wchar     ,true>(FAppend_TcharArr<wchar, nchar, lang::HeapAllocator>);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, xchar     ,true>(FAppend_TcharArr<xchar, nchar, lang::HeapAllocator>);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, nchar     ,true>(FAppend_TcharArr<nchar, wchar, lang::HeapAllocator>);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, wchar     ,true>(FAppend_TcharArr<wchar, wchar, lang::HeapAllocator>);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, xchar     ,true>(FAppend_TcharArr<xchar, wchar, lang::HeapAllocator>);
    BootstrapRegister<FAppend<xchar, lang::HeapAllocator>, nchar     ,true>(FAppend_TcharArr<nchar, xchar, lang::HeapAllocator>);
    BootstrapRegister<FAppend<xchar, lang::HeapAllocator>, wchar     ,true>(FAppend_TcharArr<wchar, xchar, lang::HeapAllocator>);
    BootstrapRegister<FAppend<xchar, lang::HeapAllocator>, xchar     ,true>(FAppend_TcharArr<xchar, xchar, lang::HeapAllocator>);

    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, std::reference_wrapper<NAString>>(FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<NAString>);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, std::reference_wrapper<WAString>>(FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<WAString>);
    BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, std::reference_wrapper<XAString>>(FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<XAString>);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, std::reference_wrapper<NAString>>(FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<NAString>);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, std::reference_wrapper<WAString>>(FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<WAString>);
    BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, std::reference_wrapper<XAString>>(FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<XAString>);
    BootstrapRegister<FAppend<xchar, lang::HeapAllocator>, std::reference_wrapper<NAString>>(FAppend<xchar, lang::HeapAllocator>::WrappedAppendable<NAString>);
    BootstrapRegister<FAppend<xchar, lang::HeapAllocator>, std::reference_wrapper<WAString>>(FAppend<xchar, lang::HeapAllocator>::WrappedAppendable<WAString>);
    BootstrapRegister<FAppend<xchar, lang::HeapAllocator>, std::reference_wrapper<XAString>>(FAppend<xchar, lang::HeapAllocator>::WrappedAppendable<XAString>);

    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N( time::DateTime::Duration )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W( time::DateTime::Duration )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X( time::DateTime::Duration )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N( time::Ticks   ::Duration )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W( time::Ticks   ::Duration )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X( time::Ticks   ::Duration )

    // CodeMarker_CommonEnums
    #if ALIB_ENUMRECORDS
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

    #if ALIB_VARIABLES
    #   if ALIB_CAMP
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( variables::Exceptions   )
    #   endif
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::variables::Priority )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::variables::Variable )
    #endif

    #if ALIB_DEBUG
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( std::type_info* )
      #if ALIB_EXT_LIB_THREADS_AVAILABLE
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( std::thread::id )
      #endif
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( lang::CallerInfo* )
    #endif

    #if ALIB_FORMAT
    boxing::BootstrapRegister< format::FFormat, time::DateTime   >(format::FFormat_DateTime);
    boxing::BootstrapRegister< format::FFormat, lang::CallerInfo*>(format::FFormat_CallerInfo);
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(   format::FormatterStdImpl::PHTypes  )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(   format::FMTExceptions    )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(   format::ByteSizeSI    )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(   format::ByteSizeIEC   )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N( format::ByteSizeUnits )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W( format::ByteSizeUnits )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X( format::ByteSizeUnits )
    #endif

    #if ALIB_SYSTEM && ALIB_EXCEPTIONS
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(  system::SystemErrors    )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(  system::Path*           )
    #endif

    #if ALIB_ALOX
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(alib::lox::Verbosity)
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(alib::lox::Scope)
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(alib::lox::detail::Logger*)
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(alib::Pair<Verbosity ALIB_COMMA Priority>)
    #endif

    #if ALIB_CLI
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( cli::Exceptions )
    #endif
    #if ALIB_EXPRESSIONS
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::expressions::Exceptions )
    #endif

    #if ALIB_FILES
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::files::File )
    alib::boxing::BootstrapRegister<alib::format::FFormat, files::File >( files::FFormat_File );
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::Types             )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::TypeNames1Letter  )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::TypeNames2Letters )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::TypeNames3Letters )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::Qualities )
    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::Qualities3Letters )
    #endif


    #if !ALIB_SINGLE_THREADED
      alib::boxing::BootstrapRegister<FAppend<character, alib::HeapAllocator>,
                                      alib::threads::Thread* >(alib::boxing::FAppend_thread);
      #if ALIB_ENUMRECORDS
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::threads::Thread::State )
      #endif
    #endif
    #if ALIB_BITBUFFER
      ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::bitbuffer::ac_v1::ArrayCompressor::Algorithm  )
    #endif

#endif // ALIB_STRINGS
}

// Box::dbgCheckRegistration
// Note: this method has to go here, to be able to check if boxing was bootstrapped already.
//       If not, no check is performed.
#if ALIB_DEBUG_BOXING

void debug::DbgCheckIsInitialized()
{
    // ERROR: A global or static instance of class Box is created and initialized to a
    //        mapped type that uses a dynamic vtable. This is forbidden.
    //        See chapter "12.4 Global And Static Box Instances" of the Programmer's Manual
    //        of module ALib Boxing, for more information.
    assert( initFlag == 0x92A3EF61 ); // magic number
}

void debug::DbgCheckRegistration( detail::VTable* vtable, bool increaseUsageCounter )
{
    if( vtable==nullptr)
        return;

    if( increaseUsageCounter )
        ++vtable->DbgCntUsage;

    if( initFlag== 0 || vtable->DbgProduction != detail::VTable::DbgFactoryType::Unregistered )
        return;

    if( !vtable->IsArray() )
        ALIB_ERROR( "BOXING", "Static VTable of mapped type <{}> not registered.\n"
            "Use Macro ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER with bootstrapping.",
            &vtable->Type )
    else
        ALIB_ERROR( "BOXING", "Static VTable of mapped type <{}[]> not registered.\n"
            "Use Macro ALIB_BOXING_REGISTER_MAPPED_ARRAY_TYPE with bootstrapping.",
            &vtable->ElementType)
}
#endif


} // namespace [alib::boxing]

