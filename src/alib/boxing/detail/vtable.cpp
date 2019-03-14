// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#ifndef HPP_ALIB_BOXING_BOXING
#   include "alib/boxing/boxing.hpp"
#endif


#if ALIB_MODULE_STRINGS  && !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif

#if ALIB_MODULE_THREADS && !defined(HPP_ALIB_THREADS_THREADLOCK)
#   include "alib/threads/threadlock.hpp"
#endif


namespace aworx { namespace lib { namespace boxing { namespace detail {

// #################################################################################################
// DbgLockMaps
// #################################################################################################
#if ALIB_DEBUG && !ALIB_DOCUMENTATION_PARSER

    #if ALIB_MODULE_THREADS
        namespace { ThreadLock dbgLock; }
    #endif

    void DbgLockMaps( bool doLock )
    {
        #if !ALIB_MODULE_THREADS
        (void) doLock;
        #else
            if( doLock )
                dbgLock.Acquire( ALIB_CALLER_PRUNED );
            else
                dbgLock.Release();
        #endif
    }

#endif

// #################################################################################################
// struct Functions
// #################################################################################################
FunctionTable DEFAULT_FUNCTIONS;

#if ALIB_DEBUG
    TypeSet   DBG_KNOWN_FUNCTIONS;
#endif



void* FunctionTable::getCustom( const std::type_info& rtti ALIB_DBG(, bool isInvocation) )     const
{
    FunctionTable::Entry* entry= Custom;
    while( entry )
    {
        if( entry->Type == rtti )
        {
ALIB_DBG(   if( isInvocation )
                entry->DbgCntInvocations++;      )
            return entry->Implementation;
        }
        entry= entry->Next;
    }

    return nullptr;
}

void  FunctionTable::setCustom( const std::type_info& rtti, void* impl )
{
    #if ALIB_DEBUG
        detail::DbgLockMaps(true);
            DBG_KNOWN_FUNCTIONS.emplace( rtti );
        detail::DbgLockMaps(false);
    #endif

    // search existing (replace)
    FunctionTable::Entry* entry= Custom;
    while( entry )
    {
        if( entry->Type == rtti )
        {
            entry->Implementation= impl;
            return;
        }
        entry= entry->Next;
    }

    // add to front.
    Custom= new FunctionTable::Entry( rtti, impl, Custom );
}

// #################################################################################################
// struct VTable
// #################################################################################################
#if ALIB_DEBUG
TypeMap<detail::VTable*>    DBG_KNOWN_VTABLES[2];


void  DbgRegisterVTable( detail::VTable* vtable, detail::VTable::DbgFactoryType productionType )
{
    vtable->DbgProduction= productionType;
    DbgLockMaps(true);
        if( !vtable->IsArray() )
        {
            if ( DBG_KNOWN_VTABLES[0].find( vtable->Type ) != DBG_KNOWN_VTABLES[0].end() )
            {
                ALIB_ERROR( "Double instantiation of VTable of Type: \"",
                            DbgTypeDemangler( vtable->Type ).Get(), "\"" )
                DbgLockMaps(false);
                return;
            }

            DBG_KNOWN_VTABLES[0].insert(std::make_pair(std::reference_wrapper<const std::type_info>( vtable->Type ),
                                                     vtable ) );
        }
        else
        {
            if ( DBG_KNOWN_VTABLES[1].find( vtable->ElementType ) != DBG_KNOWN_VTABLES[1].end() )
            {
                ALIB_ERROR( "Double instantiation of VTable of Type: \"",
                            DbgTypeDemangler( vtable->ElementType ).Get(), "[]\"" )
                DbgLockMaps(false);
                return;
            }

            DBG_KNOWN_VTABLES[1].insert(std::make_pair(std::reference_wrapper<const std::type_info>( vtable->ElementType ),
                                                     vtable ) );
        }
    DbgLockMaps(false);
}

#endif


}}}} // namespace [aworx::lib::boxing::detail]



// #################################################################################################
// Static VTables for fundamentals
// #################################################################################################

ALIB_BOXING_VTABLE_DEFINE(           void* , vt_voidP    )
ALIB_BOXING_VTABLE_DEFINE(           Boxes*, vt_boxes    )
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( Box   , vt_boxarray )

DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_DEFINE_1])
ALIB_BOXING_VTABLE_DEFINE( bool,  vt_bool )
DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_DEFINE_1])


#if ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS

    ALIB_BOXING_VTABLE_DEFINE(   integer  ,  vt_integer )
    ALIB_BOXING_VTABLE_DEFINE(   uinteger ,  vt_uinteger)

#else
    ALIB_BOXING_VTABLE_DEFINE(     int8_t ,    vt_int8_t)
    ALIB_BOXING_VTABLE_DEFINE(    uint8_t ,   vt_uint8_t)
    ALIB_BOXING_VTABLE_DEFINE(    int16_t ,   vt_int16_t)
    ALIB_BOXING_VTABLE_DEFINE(   uint16_t ,  vt_uint16_t)
    ALIB_BOXING_VTABLE_DEFINE(   intGap_t ,  vt_intGap_t)
    ALIB_BOXING_VTABLE_DEFINE(  uintGap_t , vt_uintGap_t)

#if ALIB_SIZEOF_INTEGER == 8
    ALIB_BOXING_VTABLE_DEFINE(    int32_t )
    ALIB_BOXING_VTABLE_DEFINE(   uint32_t )
#endif

#endif

    ALIB_BOXING_VTABLE_DEFINE(      double, vt_double )
  #if ALIB_SIZEOF_LONGDOUBLE <= 2 * ALIB_SIZEOF_INTEGER
    ALIB_BOXING_VTABLE_DEFINE( long double, vt_long_double )
  #endif
#if !ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS
    ALIB_BOXING_VTABLE_DEFINE(      float , vt_float  )
#endif

#if ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS
    ALIB_BOXING_VTABLE_DEFINE( wchar    , vt_wchar )
#else
    ALIB_BOXING_VTABLE_DEFINE( char     , vt_char     )
    ALIB_BOXING_VTABLE_DEFINE( wchar_t  , vt_wchar_t  )
    ALIB_BOXING_VTABLE_DEFINE( char16_t , vt_char16_t )
    ALIB_BOXING_VTABLE_DEFINE( char32_t , vt_char32_t )
#endif

DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_DEFINE_2])
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( char, vt_arr_char )
DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_DEFINE_2])
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( wchar_t  , vt_arr_wchar_t )
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( char16_t , vt_arr_char16_t)
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( char32_t , vt_arr_char32_t)
