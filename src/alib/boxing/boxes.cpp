// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/boxing/boxing.hpp"
#   if ALIB_MONOMEM
#       include "alib/monomem/monoallocator.hpp"
#   endif
#endif // !DOXYGEN

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE

namespace alib {  namespace boxing {

#if ALIB_DEBUG && !DOXYGEN
    // This is used by boxing::Bootstrap to do runtime-check for compatibility of boxing
    // and long double values.
    // It was put here to prevent the compiler to optimize and remove the code.
    extern  long double dbgLongDoubleWriteTestMem[2];
            long double dbgLongDoubleWriteTestMem[2];
    extern  void dbgLongDoubleTrueLengthSet();
            void dbgLongDoubleTrueLengthSet()
    {
        memset( dbgLongDoubleWriteTestMem, 0x3E, 2 * ALIB_SIZEOF_LONGDOUBLE_REPORTED);
    }
    extern  bool dbgLongDoubleTrueLengthTest();
            bool dbgLongDoubleTrueLengthTest()
    {
        const char* mem= reinterpret_cast<const char*>( dbgLongDoubleWriteTestMem );
        return    mem[ALIB_SIZEOF_LONGDOUBLE_WRITTEN - 1] != 0x3E
               && mem[ALIB_SIZEOF_LONGDOUBLE_WRITTEN    ] == 0x3E;
    }
#endif


#if !DOXYGEN

namespace {
integer flattenCount(const Box* boxArray, integer length)
{
    integer ctdFlattened= 0;
    for( integer i= 0; i < length ; ++i )
    {
        const Box& box= boxArray[i];

        if( box.IsType<boxing::TBoxes<lang::HeapAllocator>*>() )
        {
            const auto* boxes= box.Unbox<boxing::TBoxes<lang::HeapAllocator>*>();
            ctdFlattened+= flattenCount( boxes->data(), static_cast<integer>(boxes->size()) );
            continue;
        }
    #if ALIB_MONOMEM
        if( box.IsType<boxing::TBoxes<MonoAllocator>*>() )
        {
            const auto* boxes= box.Unbox<boxing::TBoxes<MonoAllocator>*>();
            ctdFlattened+= flattenCount( boxes->data(), static_cast<integer>(boxes->size()) );
            continue;
        }
        if( box.IsType<boxing::TBoxes<PoolAllocator>*>() )
        {
            const auto* boxes= box.Unbox<boxing::TBoxes<PoolAllocator>*>();
            ctdFlattened+= flattenCount( boxes->data(), static_cast<integer>(boxes->size()) );
            continue;
        }
    #endif

        if( box.IsArrayOf<Box>() )
        {
            ctdFlattened+= flattenCount( box.UnboxArray<Box>(), box.UnboxLength() );
            continue;
        }

        ++ctdFlattened;
    }

    return ctdFlattened;
}

template<typename TAllocator>
void flattenInsert(typename TBoxes<TAllocator>::iterator& it, const Box* boxArray, integer length)
{
    for( integer i= 0; i < length ; ++i )
    {
        const Box& box= boxArray[i];

        if( box.IsType<boxing::TBoxes<lang::HeapAllocator>*>() )
        {
            const auto* boxes= box.Unbox<boxing::TBoxes<lang::HeapAllocator>*>();
            flattenInsert<TAllocator>( it, boxes->data(), static_cast<integer>(boxes->size()) );
            continue;
        }
    #if ALIB_MONOMEM
        if( box.IsType<boxing::TBoxes<MonoAllocator>*>() )
        {
            const auto* boxes= box.Unbox<boxing::TBoxes<MonoAllocator>*>();
            flattenInsert<TAllocator>( it, boxes->data(), static_cast<integer>(boxes->size()) );
            continue;
        }
        if( box.IsType<boxing::TBoxes<PoolAllocator>*>() )
        {
            const auto* boxes= box.Unbox<boxing::TBoxes<PoolAllocator>*>();
            flattenInsert<TAllocator>( it, boxes->data(), static_cast<integer>(boxes->size()) );
            continue;
        }
    #endif

        if( box.IsArrayOf<Box>() )
        {
            flattenInsert<TAllocator>( it, box.UnboxArray<Box>(), box.UnboxLength() );
            continue;
        }

        new( &*it ) Box( box );
        ++it;
    }
}

} // anonymous namespace


template<typename TAllocator>
void  TBoxes<TAllocator>::AddArray( const Box* boxArray, integer length )
{
    // 1. Count the number of boxes if "recursively flattened"
    integer ctdFlattened= flattenCount( boxArray, length );

    // 2. create space in vector
    auto it= vectorBase::insert(vectorBase::end(), size_t(ctdFlattened), Box() );

    // 3. insert recursively all boxes found (flatten)
    flattenInsert<TAllocator>( it, boxArray, length);

    ALIB_ASSERT( it == vectorBase::end() )
}

template ALIB_API  void TBoxes<   lang::HeapAllocator>::AddArray( const Box* boxArray, integer length );
#if ALIB_MONOMEM
template ALIB_API  void TBoxes<MonoAllocator>::AddArray( const Box* boxArray, integer length );
#endif


#endif //  #if DOXYGEN

}} // namespace [alib::boxing]
ALIB_WARNINGS_RESTORE
