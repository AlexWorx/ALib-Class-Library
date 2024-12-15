//==================================================================================================
/// \file
/// This header file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LOX_DETAIL_SCOPE
#define HPP_ALIB_LOX_DETAIL_SCOPE 1
#pragma once
#if !defined(HPP_ALIB_LOX_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#include "alib/alox/detail/domain.inl"
#include "alib/monomem/poolallocator.hpp"
#include "alib/containers/stringtree.hpp"
#include "alib/containers/hashtable.hpp"

namespace alib {  namespace lox { namespace detail {

// forward declarations
class ScopeInfo;
class PrefixLogable;


/// Shortcut to the ScopeStore's hashmap.
template <typename T>   using SSMap = HashMap< PoolAllocator           ,
                                               NString                 ,// key
                                               T                       ,// map
                                               std::hash    <NString>  ,
                                               std::equal_to<NString>  ,
                                               lang::Caching::Enabled  ,
                                               Recycling::None           >;


// forwards
template<typename T, bool TStackedThreadValues>
class ScopeStore;

/// A helper-class that has two specializations to implement different versions of methods
/// \alib{lox::detail;ScopeStore::Walk} and
/// \alib{lox::detail;ScopeStore::access} for each boolean value of template argument
/// \p{TStackedThreadValues}.<br>
///
/// @tparam T                    The stored object type.
/// @tparam TStackedThreadValues If true, values stored for thread scopes will be always replaced
///                              instead of appended.
///                              This is for example \c false for <em>Log Data</em> and
///                              <em>Log Once</em> and \c true for <em>Scope Domains</em>
///                              and <em>Prefix Logables</em>.
template<typename T, bool TStackedThreadValues>
struct ScopeStoreHelper
{
    /// Implements \alib{lox::detail;ScopeStore::Walk} with two specializations.
    /// @param self The \b ScopeStore that invoked us.
    /// @return The result as specified in \alib{lox::detail;ScopeStore::Walk}.
    T   doWalk  ( ScopeStore<T, TStackedThreadValues>& self );

    /// Implements \alib{lox::detail;ScopeStore::access} with two specializations.
    /// @param self  The \b ScopeStore that invoked us.
    /// @param cmd   Parameter \p{cmd} of the original method.
    /// @param value Parameter \p{value} of the original method.
    /// @return The result as specified in \alib{lox::detail;ScopeStore::access}.
    T   doAccess( ScopeStore<T, TStackedThreadValues>& self, int cmd, T value );
};

#if !DOXYGEN
// specializations for true/false of TStackedThreadValues
template<typename T                           > struct ScopeStoreHelper<T, false>
{
    T doWalk  ( ScopeStore<T, false>& self );
    T doAccess( ScopeStore<T, false>& self, int cmd, T value );
};

template<typename T                           > struct ScopeStoreHelper<T, true>
{
    T doWalk  ( ScopeStore<T, true>& self );
    T doAccess( ScopeStore<T, true>& self, int cmd, T value );
};
#endif

//==================================================================================================
/// This class is responsible for scope-related functionality of class Lox.
/// \note This is a pure internal helper-class. Documentation may be limited.
/// @tparam T                    The stored object type.
/// @tparam TStackedThreadValues If true, values stored for thread scopes will be always replaced
///                              instead of appended.
///                              This is for example \c false for <em>Log Data</em> and
///                              <em>Log Once</em> and \c true for <em>Scope Domains</em>
///                              and <em>Prefix Logables</em>.
//==================================================================================================
template<typename T, bool TStackedThreadValues>
class ScopeStore
{
    #if !DOXYGEN
        friend struct ScopeStoreHelper<T, TStackedThreadValues>;
    #endif

    public:
        /// Alias name for the string tree template used for storing language-related data.
        /// The language store uses a \c StringTree with a monotonic allocator.
        /// This does not lead to memory leaks, because during the life-time of a \b %Lox objects
        /// are only added, but never deleted. If a value is unset, the node is not deleted but
        /// set to a \e nulled value. This makes the language store very memory efficient (and fast).
        using TLanguageStore= containers::StringTree< MonoAllocator,
                                                      T,
                                                      StringTreeNamesAlloc<character> >;

        /// The type of object stored for the thread values. This depends on whether multiple
        /// (a stack of) values can be stored, which is not true for log data and log once, as
        /// those operate with hash maps.
        using TThreadMapValue= ATMP_IF_T_F(TStackedThreadValues, StdVectorMono<T>, T );

        /// The value of the global scope.
        T                                       globalStore;

        /// \b %StringTree to store data for language-related scopes (path,source,method).
        TLanguageStore                          languageStore;

#if ALIB_THREADS
        /// Key type for the thread store.
        using ThreadMapKey   = std::pair<bool,threads::ThreadID>;

        /// Hash functor for <c>std::pair<bool,ThreadID></c>.
        struct BoolThreadIDHash
        {
            /// Calculates a hash code.
            /// @param src The object to hash.
            /// @return The hash code.
            std::size_t operator()(const std::pair<bool, threads::ThreadID>& src)              const
            {
                return src.first ? std::size_t( src.second * 282312799l  )
                                 : std::size_t( src.second * 573292817l  ) ^ std::size_t(-1);
            }
        };

        /// The inner/outer thread map of values. The boolean value of the key is \c true for
        /// the inner store and \c false for the outer.
        HashMap<MonoAllocator,
                ThreadMapKey, TThreadMapValue,
                BoolThreadIDHash              > threadStore;
#endif


    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:

        /// ScopeInfo of 'our' lox.
        ScopeInfo&                              scopeInfo;

        /// Flag used to lazily create the key to language-related scope values.
        bool                                    lazyLanguageNode;

        /// Indicates if currently a scope walk is active.
        bool                                    walking;

        /// The actual scope of a walk.
        Scope                                   actScope;

        /// The actual language related scope's map node.
        typename TLanguageStore::Cursor         actStringTreeNode;

        /// The path level when using access methods.
        int                                     actPathLevel;

#if ALIB_THREADS
        /// Actual thread ID
        threads::ThreadID                       actThreadID;
#endif

        /// The 'local object' returned by a walk after Scope::ThreadInner and before Scope::Method.
        T                                       walkLocalObject;

        /// The next value of a walk during \e Scope::ThreadInner/Outer.
        int                                     walkNextThreadIdx;

        /// The list of values of \e Scope::ThreadOuter/Inner during a walk.
        TThreadMapValue*                        walkThreadValues;

    // #############################################################################################
    // Public interface
    // #############################################################################################
    public:

        //==========================================================================================
        /// Constructor
        /// @param scopeInfo      The ScopeInfo singleton of 'our' Lox.
        /// @param monoAllocator  The monotonic allocator used for the
        ///                        \alib{containers;StringTree} needed by member #languageStore.
        //==========================================================================================
        ALIB_API
        ScopeStore( ScopeInfo& scopeInfo, MonoAllocator& monoAllocator  );

        //==========================================================================================
        /// Destructor
        //==========================================================================================
        ALIB_API
        ~ScopeStore();

        //==========================================================================================
        /// Initializes access methods #Store, #Get and #Remove and has to be invoked before
        /// using them
        /// @param scope      Scope to use.
        /// @param pathLevel  Used only if parameter \p{scope} equals \alib{lox;Scope;Scope::Path}
        ///                   to reference parent directories. Optional and defaults to \c 0.
        /// @param threadID   ID of the associated thread (for thread-related scopes only).
        ///                   If \alib{threads::UNDEFINED} is given, the ID provided in
        ///                   \p{scopeInfo} is used.
        //==========================================================================================
        ALIB_API
        void InitAccess    ( Scope scope, int pathLevel, threads::ThreadID threadID );

        //==========================================================================================
        /// Stores a new value.
        /// @param value      The value to set.
        /// @return Returns the previous value stored.
        //==========================================================================================
        T Store    ( T value )
        {
            ALIB_ASSERT( value != nullptr )
            return access( 0, value );
        }

        //==========================================================================================
        /// Removes a value.
        /// @param value      The value to remove (must only be given for thread-related \e Scopes).
        /// @return Returns the previous value stored.
        //==========================================================================================
        T Remove   ( T value )                                        { return access( 1, value ); }

        //==========================================================================================
        /// Retrieves the value.
        /// @return Returns the current value stored.
        //==========================================================================================
        T Get      ()
        {
            return access( 2, nullptr );
        }

        //==========================================================================================
        /// Initializes a scope 'walk' by storing the given scope information and
        /// setting fields of our walk 'state-machine' to proper start values.
        ///
        /// @param startScope       The \e Scope to start searching for.
        /// @param localObject      The 'local object' returned by a walk after Scope::ThreadInner
        ///                         and before Scope::Method
        //==========================================================================================
        ALIB_API
        void InitWalk( Scope startScope, const T localObject  );

        //==========================================================================================
        /// Searches next value in the actual scope. While not found, moves walk state to next outer
        /// state and continues there.
        /// @return The next object found in the current or any next outer scope.
        //==========================================================================================
        T   Walk() { return ScopeStoreHelper<T, TStackedThreadValues>().doWalk(*this); }




    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:
        //==========================================================================================
        /// Retrieves and optionally creates an entry in the map that stores language-related
        /// scope information. The result is stored in field #actStringTreeNode.
        /// @param create     If \c true, a non-existing entry is created.
        //==========================================================================================
        void initCursor( bool create );

        //==========================================================================================
        /// Receives, inserts or removes a value.
        /// @param cmd        0= insert, 1= remove, 2= get.
        /// @param value      The new value or the one to be removed.
        /// @return Returns the previous value stored.
        //==========================================================================================
        T access( int cmd, T value ) { return ScopeStoreHelper<T, TStackedThreadValues>()
                                              .doAccess( *this, cmd, value ); }
}; // ScopeStore


#if !DOXYGEN

extern template struct ScopeStoreHelper<NString                , true>;
extern template class  ScopeStore      <NString                , true>;

extern template struct ScopeStoreHelper<PrefixLogable*         , true>;
extern template class  ScopeStore      <PrefixLogable*         , true>;


extern template struct ScopeStoreHelper<SSMap<int>*, false>;
extern template class  ScopeStore      <SSMap<int>*, false>;

extern template struct ScopeStoreHelper<SSMap<Box>*, false>;
extern template class  ScopeStore      <SSMap<Box>*, false>;

#endif


}}} // namespace [alib::lox::detail]



#endif // HPP_ALIB_LOX_DETAIL_SCOPE

