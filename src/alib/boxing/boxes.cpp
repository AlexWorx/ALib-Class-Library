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
#include <cstring>

//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Boxing;
    import   ALib.Lang;
#if ALIB_MONOMEM
#   include "ALib.Monomem.H"
#endif
#else
#   include "ALib.Lang.H"
#if ALIB_MONOMEM
#   include "ALib.Monomem.H"
#endif
#   include "ALib.Boxing.H"
#endif
//========================================== Implementation ========================================

//##################################################################################################
// static assertions for the platform
//##################################################################################################
static_assert(         sizeof(alib::integer )       ==        sizeof(alib::uinteger ),          "Error in ALib type definitions" );
static_assert(         sizeof(alib::integer )       ==         sizeof(std::size_t    ),         "Error in ALib type definitions" );
static_assert(std::is_signed< alib::integer>::value == std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed< alib::integer>::value != std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<alib::uinteger>::value == std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<alib::uinteger>::value != std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );



namespace alib {  namespace boxing {

#if ALIB_DEBUG && !DOXYGEN
namespace detail {
    // This is used by boxing::Bootstrap to do runtime-check for compatibility of boxing
    // and long double values.
    // It was put here to prevent the compiler to optimize and remove the code.
    extern  long double LONGDOUBLE_WRITE_TEST_MEM[2];
extern  void dbgLongDoubleTrueLengthSet();
extern  bool dbgLongDoubleTrueLengthTest();
}
#endif

#if !DOXYGEN

namespace {
integer flattenCount(const Box* boxArray, integer length) {
    integer ctdFlattened= 0;
    for( integer i= 0; i < length ; ++i ) {
        const Box& box= boxArray[i];

        if( box.IsType<boxing::TBoxes<lang::HeapAllocator>*>() ) {
            const auto* boxes= box.Unbox<boxing::TBoxes<lang::HeapAllocator>*>();
            ctdFlattened+= flattenCount( boxes->data(), integer(boxes->size()) );
            continue;
        }
    #if ALIB_MONOMEM
        if( box.IsType<boxing::TBoxes<MonoAllocator>*>() ) {
            const auto* boxes= box.Unbox<boxing::TBoxes<MonoAllocator>*>();
            ctdFlattened+= flattenCount( boxes->data(), integer(boxes->size()) );
            continue;
        }
        if( box.IsType<boxing::TBoxes<PoolAllocator>*>() ) {
            const auto* boxes= box.Unbox<boxing::TBoxes<PoolAllocator>*>();
            ctdFlattened+= flattenCount( boxes->data(), integer(boxes->size()) );
            continue;
        }
    #endif

        if( box.IsArrayOf<Box>() ) {
            ctdFlattened+= flattenCount( box.UnboxArray<Box>(), box.UnboxLength() );
            continue;
        }

        ++ctdFlattened;
    }

    return ctdFlattened;
}

template<typename TAllocator>
void flattenInsert(typename TBoxes<TAllocator>::iterator& it, const Box* boxArray, integer length) {
    for( integer i= 0; i < length ; ++i ) {
        const Box& box= boxArray[i];

        if( box.IsType<boxing::TBoxes<lang::HeapAllocator>*>() ) {
            const auto* boxes= box.Unbox<boxing::TBoxes<lang::HeapAllocator>*>();
            flattenInsert<TAllocator>( it, boxes->data(), integer(boxes->size()) );
            continue;
        }
    #if ALIB_MONOMEM
        if( box.IsType<boxing::TBoxes<MonoAllocator>*>() ) {
            const auto* boxes= box.Unbox<boxing::TBoxes<MonoAllocator>*>();
            flattenInsert<TAllocator>( it, boxes->data(), integer(boxes->size()) );
            continue;
        }
        if( box.IsType<boxing::TBoxes<PoolAllocator>*>() ) {
            const auto* boxes= box.Unbox<boxing::TBoxes<PoolAllocator>*>();
            flattenInsert<TAllocator>( it, boxes->data(), integer(boxes->size()) );
            continue;
        }
    #endif

        if( box.IsArrayOf<Box>() ) {
            flattenInsert<TAllocator>( it, box.UnboxArray<Box>(), box.UnboxLength() );
            continue;
        }

        new( &*it ) Box( box );
        ++it;
}   }

} // anonymous namespace


template<typename TAllocator>
void  TBoxes<TAllocator>::AddArray( const Box* boxArray, integer length ) {
    // 1. Count the number of boxes if "recursively flattened"
    integer ctdFlattened= flattenCount( boxArray, length );

    // 2. create space in vector
    auto it= vectorBase::insert(vectorBase::end(), size_t(ctdFlattened), Box() );

    // 3. insert recursively all boxes found (flatten)
    flattenInsert<TAllocator>( it, boxArray, length);

    ALIB_ASSERT( it == vectorBase::end(), "BOXING" )
}

template ALIB_DLL  void TBoxes<   lang::HeapAllocator>::AddArray( const Box* boxArray, integer length );
#if ALIB_MONOMEM
template ALIB_DLL  void TBoxes<MonoAllocator>::AddArray( const Box* boxArray, integer length );
#endif


#endif //  #if DOXYGEN

}} // namespace [alib::boxing]
