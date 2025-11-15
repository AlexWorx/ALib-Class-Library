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
#if ALIB_MONOMEM && ALIB_CONTAINERS
#include "alib/boxing/boxing.prepro.hpp"
#endif
#if ALIB_DEBUG && (!ALIB_MONOMEM || !ALIB_CONTAINERS )
#   include <unordered_map>
#   include <unordered_set>
#endif
#if ALIB_DEBUG_BOXING
#   include <vector>
#endif

//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Boxing;
#  if ALIB_DEBUG && ALIB_MONOMEM && ALIB_CONTAINERS
    import   ALib.Monomem;
    import   ALib.Containers.HashTable;
#  endif

#else
#   include "ALib.Boxing.H"
#   if ALIB_DEBUG && ALIB_MONOMEM && ALIB_CONTAINERS
#      include "ALib.Containers.HashTable.H"
#      include "ALib.Monomem.H"
#   endif
#endif
//========================================== Implementation ========================================
namespace alib {  namespace boxing { namespace detail {

//##################################################################################################
// Custom function hash map implementation
//##################################################################################################
#if !DOXYGEN
#if ALIB_MONOMEM && ALIB_CONTAINERS
HashMap           < MonoAllocator,
                    CustomFunctionKey, CustomFunctionMapped,
                    CustomFunctionHash,
                    CustomFunctionEqualTo >     customFunctionMap(monomem::GLOBAL_ALLOCATOR);
#else
std::unordered_map< CustomFunctionKey, CustomFunctionMapped,
                    CustomFunctionHash,
                    CustomFunctionEqualTo >     customFunctionMap;
#endif

#if ALIB_DEBUG
} namespace debug {
#if ALIB_MONOMEM && ALIB_CONTAINERS
    HashSet< MonoAllocator, lang::TypeFunctors::Key                 , lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownCustomFunctions(monomem::GLOBAL_ALLOCATOR);
    HashMap< MonoAllocator, lang::TypeFunctors::Key, detail::VTable*, lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownVTables(monomem::GLOBAL_ALLOCATOR);
    HashMap< MonoAllocator, lang::TypeFunctors::Key, detail::VTable*, lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownVTablesArray(monomem::GLOBAL_ALLOCATOR);
#else
    std::unordered_set<     lang::TypeFunctors::Key                 , lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownCustomFunctions;
    std::unordered_map<     lang::TypeFunctors::Key, detail::VTable*, lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownVTables;
    std::unordered_map<     lang::TypeFunctors::Key, detail::VTable*, lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownVTablesArray;
#endif

IF_ALIB_THREADS( namespace { RecursiveLock dbgLock; } )
void DbgLockMaps( bool doLock ) {
    #if ALIB_SINGLE_THREADED
        (void) doLock;
        assert::SingleThreaded();
    #else
        #   include "ALib.Lang.CIFunctions.H"
        ALIB_DBG(dbgLock.Dbg.Name= "DbgBoxing";)
        if( doLock )
            dbgLock.AcquireRecursive( ALIB_CALLER_PRUNED );
        else
            dbgLock.ReleaseRecursive( ALIB_CALLER_PRUNED );
        #   include "ALib.Lang.CIMethods.H"
    #endif
}
} namespace detail {
#endif // ALIB_DEBUG

#endif // !DOXYGEN

//##################################################################################################
// struct Functions
//##################################################################################################
FunctionTable DEFAULT_FUNCTIONS;

#if (ALIB_MONOMEM && ALIB_CONTAINERS && ALIB_DEBUG)
void FunctionTable::Shutdown() {
    #if ALIB_MONOMEM && ALIB_DEBUG_BOXING
        debug::DbgKnownCustomFunctions.Reset();
        debug::DbgKnownVTables        .Reset();
        debug::DbgKnownVTablesArray   .Reset();
    #endif
    customFunctionMap          .Reset();
}
#endif


void* FunctionTable::getCustom( const std::type_info& rtti ALIB_DBG(, bool isInvocation) )   const {
#if ALIB_MONOMEM && ALIB_CONTAINERS
    auto it= customFunctionMap.Find( CustomFunctionKey(this, rtti) );
#else
    auto it= customFunctionMap.find( CustomFunctionKey(this, rtti) );
#endif
    if ( it != customFunctionMap.end() ) {
        ALIB_DBG(   if( isInvocation )
                        ++it->second.DbgCntInvocations;      )
        return it->second.Implementation;
    }
    return nullptr;
}

void  FunctionTable::setCustom( const std::type_info& rtti, void* impl ) {
    #if ALIB_DEBUG_BOXING
        debug::DbgLockMaps(true);
            #if ALIB_MONOMEM
                debug::DbgKnownCustomFunctions.InsertIfNotExistent( &rtti );
            #else
                debug::DbgKnownCustomFunctions.emplace( &rtti );
            #endif
        debug::DbgLockMaps(false);
    #endif

    // search existing (replace)
    #if ALIB_MONOMEM && ALIB_CONTAINERS
        customFunctionMap.InsertOrAssign( CustomFunctionKey(this, rtti), CustomFunctionMapped(impl) );
    #else
        if( customFunctionMap.size() == 0 )
            customFunctionMap.reserve( 50 );
        customFunctionMap.insert_or_assign( CustomFunctionKey(this, rtti), CustomFunctionMapped(impl) );
    #endif

}

} // namespace alib::boxing[::detail]

using namespace detail;

//##################################################################################################
// Debug Function Lists Implementation
// (located here due to anonymous function table)
//##################################################################################################
#if ALIB_DEBUG_BOXING
std::vector<detail::VTable*>  debug::GetKnownVTables()
{
    DbgLockMaps(true);

        std::vector<detail::VTable*> result;
    #if ALIB_MONOMEM
        result.reserve( size_t(
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

std::vector<std::pair<const std::type_info*,uinteger>>  debug::GetKnownFunctionTypes()
{
    std::vector<std::pair<const std::type_info*,uinteger>> result;
    result.emplace_back( &typeid( FHashcode                    ), detail::DEFAULT_FUNCTIONS.fHashcode  ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFHashcode  : (std::numeric_limits<uinteger>::max)() );
IF_ALIB_MONOMEM(
    result.emplace_back( &typeid( FClone                       ), detail::DEFAULT_FUNCTIONS.fClone     ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFClone     : (std::numeric_limits<uinteger>::max)() ); )
    result.emplace_back( &typeid( FIsNotNull                   ), detail::DEFAULT_FUNCTIONS.fIsNotNull ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFIsNotNull : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FEquals                      ), detail::DEFAULT_FUNCTIONS.fEquals    ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFEquals    : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FIsLess                      ), detail::DEFAULT_FUNCTIONS.fIsLess    ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFIsLess    : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FIsTrue                      ), detail::DEFAULT_FUNCTIONS.fIsTrue    ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFIsTrue    : (std::numeric_limits<uinteger>::max)() );

    debug::DbgLockMaps(true);
    {
        for (auto* typeIt : debug::DbgKnownCustomFunctions )
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
    debug::DbgLockMaps(false);

    return result;
}

void debug::getFunctionTypes( const detail::FunctionTable&                             functionTable,
                                  std::vector<std::pair<const std::type_info*,uinteger>>&  output  )
{
    output.clear();
    if(functionTable.fHashcode ) output.emplace_back( &typeid( FHashcode                    ), functionTable.DbgCntInvocationsFHashcode  );
  IF_ALIB_MONOMEM(
    if(functionTable.fClone    ) output.emplace_back( &typeid( FClone                       ), functionTable.DbgCntInvocationsFClone     ); )
    if(functionTable.fIsNotNull) output.emplace_back( &typeid( FIsNotNull                   ), functionTable.DbgCntInvocationsFIsNotNull );
    if(functionTable.fEquals   ) output.emplace_back( &typeid( FEquals                      ), functionTable.DbgCntInvocationsFEquals    );
    if(functionTable.fIsLess   ) output.emplace_back( &typeid( FIsLess                      ), functionTable.DbgCntInvocationsFIsLess    );
    if(functionTable.fIsTrue   ) output.emplace_back( &typeid( FIsTrue                      ), functionTable.DbgCntInvocationsFIsTrue    );

    // add custom function types
    {
        for( auto funcIt= customFunctionMap.begin() ; funcIt != customFunctionMap.end() ; ++funcIt )
            if( funcIt->first.Parent == &functionTable )
                output.emplace_back( &funcIt->first.Type , funcIt->second.DbgCntInvocations );
    }
}

#endif // ALIB_DEBUG_BOXING

}} // namespace [alib::boxing]
