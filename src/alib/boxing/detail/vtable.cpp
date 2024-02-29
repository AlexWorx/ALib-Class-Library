// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#ifndef HPP_ALIB_BOXING_BOXING
#   include "alib/boxing/boxing.hpp"
#endif

#if !defined(HPP_ALIB_COMPATIBILITY_STD_TYPEINFO)
#   include "alib/compatibility/std_typeinfo.hpp"
#endif

#if ALIB_DEBUG
#   if !defined (HPP_ALIB_BOXING_DBGBOXING)
#      include "alib/boxing/dbgboxing.hpp"
#   endif
#endif

#endif // !defined(ALIB_DOX)

#if ALIB_MONOMEM
#   if !defined(HPP_ALIB_MONOMEM_HASHMAP)
#      include "alib/monomem/hashmap.hpp"
#   endif
#   if !defined(HPP_ALIB_MONOMEM_HASHSET)
#      include "alib/monomem/hashset.hpp"
#   endif
#else
#   if !defined(_GLIBCXX_UNORDERED_MAP) && !defined(_UNORDERED_MAP_)
#      include <unordered_map>
#   endif
#   if !defined(_GLIBCXX_UNORDERED_SET) && !defined(_UNORDERED_SET_)
#      include <unordered_set>
#   endif
#endif



namespace alib {  namespace boxing { namespace detail {

#if !defined(ALIB_DOX)

#if ALIB_MONOMEM
    extern HashSet           <TypeFunctors::Key,                  TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownCustomFunctions;
    extern HashMap           <TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTables;
    extern HashMap           <TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTablesArray;
#else
    extern std::unordered_set<TypeFunctors::Key,                  TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownCustomFunctions;
    extern std::unordered_map<TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTables;
    extern std::unordered_map<TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTablesArray;
#endif

extern ALIB_API void DbgLockMaps( bool doLock );


// #################################################################################################
// Custom function hash map implementation
// #################################################################################################
namespace
{
    struct CustomFunctionKey
    {
        const FunctionTable*    Parent;
        const std::type_info&   Type;

        CustomFunctionKey( const FunctionTable* parent, const std::type_info& type )
        : Parent(parent)
        , Type  (type )
        {}
    };

    struct CustomFunctionMapped
    {
         void*                   Implementation;
ALIB_DBG(uinteger               DbgCntInvocations; )

         CustomFunctionMapped( void* implementation )
         : Implementation    (implementation)
ALIB_DBG(,DbgCntInvocations  (0             )  )
         {}
    };


    struct CustomFunctionHash
    {
        std::size_t operator()(const CustomFunctionKey& key)             const
        {
            size_t result=    reinterpret_cast<size_t>(key.Parent)
                            ^ key.Type.hash_code();
            result^= (result << 21 );
            result^= (result >> 11);
            return result;
        }

    };

    struct CustomFunctionEqualTo
    {
        bool operator()(const CustomFunctionKey& lhs, const CustomFunctionKey& rhs) const
        {
            return     lhs.Parent == rhs.Parent
                    && lhs.Type   == rhs.Type;
        }
    };

    #if ALIB_MONOMEM
        HashMap           < CustomFunctionKey, CustomFunctionMapped,
                            CustomFunctionHash,
                            CustomFunctionEqualTo >                  customFunctionMap( &monomem::GlobalAllocator );
    #else
        std::unordered_map< CustomFunctionKey, CustomFunctionMapped,
                            CustomFunctionHash,
                            CustomFunctionEqualTo >                  customFunctionMap;
    #endif

}// anonymous namespace

#endif

// #################################################################################################
// struct Functions
// #################################################################################################
FunctionTable DEFAULT_FUNCTIONS;



void* FunctionTable::getCustom( const std::type_info& rtti ALIB_DBG(, bool isInvocation) )     const
{
#if ALIB_MONOMEM
    auto it= customFunctionMap.Find( CustomFunctionKey(this, rtti) );
#else
    auto it= customFunctionMap.find( CustomFunctionKey(this, rtti) );
#endif
    if ( it != customFunctionMap.end() )
    {
        ALIB_DBG(   if( isInvocation )
                        ++it->second.DbgCntInvocations;      )
        return it->second.Implementation;
    }
    return nullptr;
}

void  FunctionTable::setCustom( const std::type_info& rtti, void* impl )
{
    #if ALIB_DEBUG_BOXING
        detail::DbgLockMaps(true);
            #if ALIB_MONOMEM
                DbgKnownCustomFunctions.InsertIfNotExistent( &rtti );
            #else
                DbgKnownCustomFunctions.emplace( &rtti );
            #endif
        detail::DbgLockMaps(false);
    #endif

    // search existing (replace)
    #if ALIB_MONOMEM
        customFunctionMap.InsertOrAssign( CustomFunctionKey(this, rtti), CustomFunctionMapped(impl) );
    #else
        if( customFunctionMap.size() == 0 )
            customFunctionMap.reserve( 50 );
        customFunctionMap.insert_or_assign( CustomFunctionKey(this, rtti), CustomFunctionMapped(impl) );
    #endif

}

} // namespace alib::boxing[::detail]

using namespace detail;

// #################################################################################################
// DBGBoxing Function Lists Implementation
// (located here due to anonymous function table)
// #################################################################################################
#if ALIB_DEBUG_BOXING
std::vector<detail::VTable*>  DbgBoxing::GetKnownVTables()
{
    DbgLockMaps(true);

        std::vector<detail::VTable*> result;
    #if ALIB_MONOMEM
        result.reserve( static_cast<size_t>(
                          DbgKnownVTables     .Size()
                        + DbgKnownVTablesArray.Size()  ) );
    #else
        result.reserve(   DbgKnownVTables     .size()
                        + DbgKnownVTablesArray.size()  );
    #endif

        for( int type= 0 ; type < 2 ; ++type )
        {
            auto& map= type == 0 ? DbgKnownVTables
                                 : DbgKnownVTablesArray;
            for( auto it= map.begin() ; it!= map.end() ; ++it )
                result.emplace_back( it->second );
        }

    DbgLockMaps(false);
    return result;
}

std::vector<std::pair<const std::type_info*,uinteger>>  DbgBoxing::GetKnownFunctionTypes()
{
    std::vector<std::pair<const std::type_info*,uinteger>> result;
    result.emplace_back( &typeid( FHashcode         ), detail::DEFAULT_FUNCTIONS.fHashcode  ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFHashcode  : (std::numeric_limits<uinteger>::max)() );
ALIB_IF_MONOMEM(
    result.emplace_back( &typeid( FClone            ), detail::DEFAULT_FUNCTIONS.fClone     ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFClone     : (std::numeric_limits<uinteger>::max)() ); )
    result.emplace_back( &typeid( FIsNotNull        ), detail::DEFAULT_FUNCTIONS.fIsNotNull ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFIsNotNull : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FEquals           ), detail::DEFAULT_FUNCTIONS.fEquals    ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFEquals    : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FIsLess           ), detail::DEFAULT_FUNCTIONS.fIsLess    ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFIsLess    : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FIsTrue           ), detail::DEFAULT_FUNCTIONS.fIsTrue    ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFIsTrue    : (std::numeric_limits<uinteger>::max)() );
ALIB_IF_STRINGS(
    result.emplace_back( &typeid( FAppend<character>), detail::DEFAULT_FUNCTIONS.fAppend    ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFAppend    : (std::numeric_limits<uinteger>::max)() ); )

    detail::DbgLockMaps(true);
    {
        for (auto* typeIt : detail::DbgKnownCustomFunctions )
        {
            // search corresponding default implementation.
            auto usage= (std::numeric_limits<uinteger>::max)();

        #if ALIB_MONOMEM
            auto implIt= customFunctionMap.Find( CustomFunctionKey(&detail::DEFAULT_FUNCTIONS, *typeIt) );
        #else
            auto implIt= customFunctionMap.find( CustomFunctionKey(&detail::DEFAULT_FUNCTIONS, *typeIt) );
        #endif
            if( implIt != customFunctionMap.end() )
                usage= implIt->second.DbgCntInvocations;

            result.emplace_back( typeIt, usage );
        }
    }
    detail::DbgLockMaps(false);

    return result;
}

void DbgBoxing::getFunctionTypes( const detail::FunctionTable&                             functionTable,
                                  std::vector<std::pair<const std::type_info*,uinteger>>&  output  )
{
    output.clear();
    if(functionTable.fHashcode ) output.emplace_back( &typeid( FHashcode         ), functionTable.DbgCntInvocationsFHashcode  );
  ALIB_IF_MONOMEM(
    if(functionTable.fClone    ) output.emplace_back( &typeid( FClone            ), functionTable.DbgCntInvocationsFClone     ); )
    if(functionTable.fIsNotNull) output.emplace_back( &typeid( FIsNotNull        ), functionTable.DbgCntInvocationsFIsNotNull );
    if(functionTable.fEquals   ) output.emplace_back( &typeid( FEquals           ), functionTable.DbgCntInvocationsFEquals    );
    if(functionTable.fIsLess   ) output.emplace_back( &typeid( FIsLess           ), functionTable.DbgCntInvocationsFIsLess    );
    if(functionTable.fIsTrue   ) output.emplace_back( &typeid( FIsTrue           ), functionTable.DbgCntInvocationsFIsTrue    );
  ALIB_IF_STRINGS(
    if(functionTable.fAppend   ) output.emplace_back( &typeid( FAppend<character>), functionTable.DbgCntInvocationsFAppend    ); )

    // add custom function types
    {
        for( auto funcIt= customFunctionMap.begin() ; funcIt != customFunctionMap.end() ; ++funcIt )
            if( funcIt->first.Parent == &functionTable )
                output.emplace_back( &funcIt->first.Type , funcIt->second.DbgCntInvocations );
    }
}


#if ALIB_MONOMEM && ALIB_DEBUG_MONOMEM
void DbgBoxing::DumpCustomFunctionHashMapMetrics( AString& target, bool detailedBucketList )
{
    target << monomem::DbgDumpDistribution( customFunctionMap, detailedBucketList );
}
#endif


#endif // ALIB_DEBUG_BOXING


}} // namespace [alib::boxing]



// #################################################################################################
// Static VTables for fundamentals
// #################################################################################################

ALIB_BOXING_VTABLE_DEFINE(           void* , vt_voidP    )
ALIB_BOXING_VTABLE_DEFINE(           Boxes*, vt_boxes    )
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( Box   , vt_boxarray )

DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_DEFINE_1])
ALIB_BOXING_VTABLE_DEFINE( bool,  vt_bool )
DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_DEFINE_1])


#if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS

    ALIB_BOXING_VTABLE_DEFINE(   integer  ,  vt_integer )
    ALIB_BOXING_VTABLE_DEFINE(   uinteger ,  vt_uinteger)

#else
    ALIB_BOXING_VTABLE_DEFINE(     int8_t ,    vt_int8_t)
    ALIB_BOXING_VTABLE_DEFINE(    uint8_t ,   vt_uint8_t)
    ALIB_BOXING_VTABLE_DEFINE(    int16_t ,   vt_int16_t)
    ALIB_BOXING_VTABLE_DEFINE(   uint16_t ,  vt_uint16_t)
    ALIB_BOXING_VTABLE_DEFINE(    int32_t ,  vt_int32_t)
    ALIB_BOXING_VTABLE_DEFINE(   uint32_t , vt_uint32_t)
    ALIB_BOXING_VTABLE_DEFINE(   intGap_t ,  vt_intGap_t)
    ALIB_BOXING_VTABLE_DEFINE(  uintGap_t , vt_uintGap_t)

  #if ALIB_SIZEOF_INTEGER == 8
    ALIB_BOXING_VTABLE_DEFINE(    int64_t ,  vt_int64_t)
    ALIB_BOXING_VTABLE_DEFINE(   uint64_t , vt_uint64_t)
  #endif

#endif

    ALIB_BOXING_VTABLE_DEFINE(      double, vt_double )
#if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
    ALIB_BOXING_VTABLE_DEFINE( long double, vt_long_double )
#endif
#if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    ALIB_BOXING_VTABLE_DEFINE(      float , vt_float  )
#endif

#if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
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

// #################################################################################################
// Static VTables for standard types
// #################################################################################################
ALIB_BOXING_VTABLE_DEFINE( std::type_info*              , vt_std_type_info          )

// #################################################################################################
// Static VTables for low-level ALib types
// #################################################################################################
// CodeMarker_CommonEnums
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Alignment        , vt_alib_Alignment         )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Bool             , vt_alib_Bool              )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Caching          , vt_alib_Caching           )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Case             , vt_alib_Case              )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::ContainerOp      , vt_alib_ContainerOp       )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::CreateDefaults   , vt_alib_CreateDefaults    )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::CreateIfNotExists, vt_alib_CreateIfNotExists )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::CurrentData      , vt_alib_CurrentData       )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Inclusion        , vt_alib_Inclusion         )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Initialization   , vt_alib_Initialization    )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Phase            , vt_alib_Phase             )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Propagation      , vt_alib_Propagation       )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Reach            , vt_alib_Reach             )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Reach            , vt_alib_Recursive         )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Responsibility   , vt_alib_Responsibility    )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Safeness         , vt_alib_Safeness          )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Side             , vt_alib_Side              )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::SortOrder        , vt_alib_SortOrder         )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::SourceData       , vt_alib_SourceData        )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Switch           , vt_alib_Switch            )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Timezone         , vt_alib_Timezone          )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Timing           , vt_alib_Timing            )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::ValueReference   , vt_alib_ValueReference    )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Whitespaces      , vt_alib_Whitespaces       )
