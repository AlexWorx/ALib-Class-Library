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
#   if ALIB_MONOMEM && !defined(HPP_ALIB_MONOMEM_MONOALLOCATOR)
#       include "alib/monomem/monoallocator.hpp"
#   endif
#endif // !defined(ALIB_DOX)

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE

namespace alib {  namespace boxing {

#if ALIB_DEBUG && !defined(ALIB_DOX)
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


#if ALIB_MONOMEM
ALIB_API
[[nodiscard]]
char*  detail::monoAlloc( monomem::MonoAllocator& allocator, size_t size, size_t alignment)
{
    return allocator.Alloc( size, alignment );
}
#endif

#if !defined(ALIB_DOX)

namespace {
integer flattenCount(const Box* boxArray, integer length)
{
    integer qtyFlattened= 0;
    for( integer i= 0; i < length ; ++i )
    {
        const Box& box= boxArray[i];

        if( box.IsType<Boxes*>() )
        {
            const Boxes* boxes= box.Unbox<Boxes*>();
            qtyFlattened+= flattenCount( boxes->data(), static_cast<integer>(boxes->size()) );
            continue;
        }

        if( box.IsArrayOf<Box>() )
        {
            qtyFlattened+= flattenCount( box.UnboxArray<Box>(), box.UnboxLength() );
            continue;
        }

        ++qtyFlattened;
    }

    return qtyFlattened;
}

void flattenInsert(Boxes::iterator& it, const Box* boxArray, integer length)
{
    for( integer i= 0; i < length ; ++i )
    {
        const Box& box= boxArray[i];

        if( box.IsType<Boxes*>() )
        {
            const Boxes* boxes      = box.Unbox<Boxes*>();
            flattenInsert( it, boxes->data(), static_cast<integer>(boxes->size()) );
            continue;
        }

        if( box.IsArrayOf<Box>() )
        {
            flattenInsert( it, box.UnboxArray<Box>(), box.UnboxLength() );
            continue;
        }

        new( &*it ) Box( box );
        ++it;
    }
}

} // anonymous namespace



void  Boxes::AddArray( const Box* boxArray, integer length )
{
    // 1. Count the number of boxes if "recursively flattened"
    // (We copy the anonymous method flattenCount() from above, to avoid JSR, which is unlikely
    // anyhow)
    integer qtyFlattened= 0;
    for( integer i= 0; i < length ; ++i )
    {
        const Box& box= boxArray[i];

        if( box.IsType<Boxes*>() )
        {
            const Boxes* boxes= box.Unbox<Boxes*>();
            qtyFlattened+= flattenCount( boxes->data(), static_cast<integer>(boxes->size()) );
            continue;
        }

        if( box.IsArrayOf<Box>() )
        {
            qtyFlattened+= flattenCount( box.UnboxArray<Box>(), box.UnboxLength() );
            continue;
        }

        ++qtyFlattened;
    }

    // 2. create space in vector
    auto it= insert(end(), static_cast<size_t>(qtyFlattened), Box() );

    // 3. insert recursively all boxes found (flatten)
    // (We copy the anonymous method flattenInsert() from above, to avoid JSR, which is unlikely
    // anyhow)
    for( integer i= 0; i < length ; ++i )
    {
        const Box& box= boxArray[i];

        if( box.IsType<Boxes*>() )
        {
            const Boxes* boxes      = box.Unbox<Boxes*>();
            flattenInsert( it, boxes->data(), static_cast<integer>(boxes->size()) );
            continue;
        }

        if( box.IsArrayOf<Box>() )
        {
            flattenInsert( it, box.UnboxArray<Box>(), box.UnboxLength() );
            continue;
        }

        new( &*it ) Box( box );
        ++it;
    }

    ALIB_ASSERT( it == end() )
}


#endif //  #if defined(ALIB_DOX)

}} // namespace [alib::boxing]
ALIB_WARNINGS_RESTORE
