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
#include "alib/boxing/boxing.prepro.hpp"

#include <cmath>
#include <functional>
#include <cstring>
#include <typeindex>

//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Boxing;
#else
#if ALIB_MONOMEM && ALIB_CONTAINERS
#   include "ALib.Monomem.H"
#endif
#   include "ALib.Boxing.H"
#if ALIB_MONOMEM && ALIB_CONTAINERS
#   include "ALib.Monomem.H"
#endif
#endif
#   include "ALib.Lang.H"
//========================================== Implementation ========================================
#if ALIB_STRINGS
ALIB_BOXING_VTABLE_DEFINE( std::reference_wrapper<alib::strings::TAString<nchar ALIB_COMMA lang::HeapAllocator>>, vt_alib_wrapped_tanstring )
ALIB_BOXING_VTABLE_DEFINE( std::reference_wrapper<alib::strings::TAString<wchar ALIB_COMMA lang::HeapAllocator>>, vt_alib_wrapped_tawstring )
ALIB_BOXING_VTABLE_DEFINE( std::reference_wrapper<alib::strings::TAString<xchar ALIB_COMMA lang::HeapAllocator>>, vt_alib_wrapped_taxstring )
ALIB_BOXING_VTABLE_DEFINE( alib::strings::util::Token*, vt_alib_strings_token )
#endif

namespace alib {

//==================================================================================================
/// This is the reference documentation of sub-namespace \b boxing of the \aliblink, which
/// holds types of library module \alib_boxing.
///
/// Extensive documentation for this module is provided with
/// \ref alib_mod_boxing "ALib Module Boxing - Programmer's Manual".
//==================================================================================================
namespace boxing {

//##################################################################################################
//##################################################################################################
// class Box
//##################################################################################################
//##################################################################################################

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
            return     IsType< int64_t >()  ?        ( Unbox< int64_t >() )
                    :  IsType< int32_t >()  ? integer( Unbox< int32_t >() )
        #else
            return     IsType< int32_t >()  ?        ( Unbox< int32_t >() )
                    :  IsType< int64_t >()  ? integer( Unbox< int64_t >() )
        #endif
                    :  IsType< int8_t  >()  ? integer( Unbox< int8_t  >() )
                    :  IsType< int16_t >()  ? integer( Unbox< int16_t >() )
                    :                         integer( Unbox< intGap_t>() );
    }


    uinteger Box::UnboxUnsignedIntegral() const
    {
        #if  ALIB_SIZEOF_INTEGER == 8
            return     IsType<uint64_t >() ?          ( Unbox<uint64_t >() )
                    :  IsType<uint32_t >() ?  uinteger( Unbox<uint32_t >() )
        #else
            return     IsType<uint32_t >() ?          ( Unbox<uint32_t >() )
                    :  IsType<uint64_t >() ?  uinteger( Unbox<uint64_t >() )
        #endif
                    :  IsType<uint8_t  >() ?  uinteger( Unbox<uint8_t  >() )
                    :  IsType<uint16_t >() ?  uinteger( Unbox<uint16_t >() )
                    :                         uinteger( Unbox<uintGap_t>() );
    }
#endif

#if ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
bool    Box::IsCharacter()                                                                     const
{
    return    IsType<char    >()
           || IsType<wchar_t >()
           || IsType<char8_t >()
           || IsType<char16_t>()
           || IsType<char32_t>();
}

wchar   Box::UnboxCharacter()                                                                  const
{
    return   IsType<char    >() ? wchar( Unbox<char    >() )
           : IsType<char8_t >() ? wchar( Unbox<char    >() )
           : IsType<wchar_t >() ? wchar( Unbox<wchar_t >() )
           : IsType<char16_t>() ? wchar( Unbox<char16_t>() )
           :                      wchar( Unbox<char32_t>() );
}
#endif

bool    Box::IsFloatingPoint()                                                               const {
    return    IsType<     double>()
        #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
           || IsType<     float >()
        #endif
        #if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
           || IsType<long double>()
        #endif
        ;
}

double  Box::UnboxFloatingPoint()                                                            const {
  #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    if( IsType<     float >() ) return   double( Unbox<float      >() );
  #endif

  #if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
    if( IsType<long double>() ) return   double( Unbox<long double>() );
  #endif

  return Unbox<double>();
}

#if !DOXYGEN
       Box::operator bool()                                   const { return  Call<FIsTrue    >(); }
bool   Box::IsNotNull ()                                      const { return  Call<FIsNotNull >(); }
size_t Box::Hashcode  ()                                      const { return  Call<FHashcode  >(); }
IF_ALIB_MONOMEM(
void   Box::Clone( MonoAllocator& memory) {         Call<FClone     >( memory );  } )
#endif

//##################################################################################################
// Non-anonymous functions
//##################################################################################################
bool FIsNotNull::ConstantTrue( const alib::Box & )                                 { return false; }

}} // namespace [alib::boxing]

using namespace alib::boxing;
bool   Box::operator==(const Box& rhs)                          const { return Call<FEquals>(rhs); }
bool   Box::operator< (const Box& rhs)                          const { return Call<FIsLess>(rhs); }
bool   Box::operator<=(const Box& rhs)  const { return  Call<FIsLess>(rhs) ||  Call<FEquals>(rhs); }
bool   Box::operator> (const Box& rhs)  const { return !Call<FIsLess>(rhs) && !Call<FEquals>(rhs); }
