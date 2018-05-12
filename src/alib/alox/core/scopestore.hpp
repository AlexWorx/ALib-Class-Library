// #################################################################################################
//  aworx::lib::lox::core - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include ALox main header first...
#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

// then, set include guard
#ifndef HPP_ALOX_CORE_SCOPE
#define HPP_ALOX_CORE_SCOPE 1


// #################################################################################################
// includes
// #################################################################################################

#include "alib/util/stringtree.hpp"

namespace aworx { namespace lib { namespace lox { namespace core {

/** ************************************************************************************************
 * Static axiomatic methods on value types stored in class \b ScopeStore.
 * \note This is a pure internal helper class. Documentation may be limited.
 **************************************************************************************************/
template<typename StoreT>
class ScopeStoreType
{
    public:

    /** ********************************************************************************************
     * Static method to retrieve a value representing 'null' for the template type.
     * @return The value representing null.
     **********************************************************************************************/
    ALIB_API
    static StoreT NullValue();

    /** ********************************************************************************************
     * Returns true if the value is representing 'null'.
     * @param value  The object to test for 'null'.
     * @returns \c true if the value is representing 'null', \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    static bool IsNull( StoreT value );

    /** ********************************************************************************************
     * Compares two template values.
     * @param first          First value.
     * @param second         Second value.
     * @returns \c true if objects are equal, \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    static bool AreEqual( StoreT first, StoreT second );

}; // ScopeStoreType

/** ************************************************************************************************
 * This class is responsible for scope-related functionality of class Lox.
 * \note This is a pure internal helper class. Documentation may be limited.
 **************************************************************************************************/
template<typename StoreT>
class ScopeStore
{
    // #############################################################################################
    // Public fields
    // These fields are public, but publicly accessed only for State and Reset()
    // (Bauhaus code style, omitting getters. For library users this is still invisible, as
    // the stores are protected in class Lox)
    // #############################################################################################
    public:
        /** Alias name for the string tree template used for storing language-related data.
         *  The language store uses a \c StringTree with a block allocator.
         *  This does not lead to memory leaks, because during the life-time of a \b %Lox objects
         *  are only added, but never deleted. If a value is unset, the node is not deleted but
         *  set to a nulled value. This makes the language store very memory efficient (and fast).
         */
        using LanguageStore= lib::util::StringTree<StoreT, lib::util::StringTreeBlockAllocator<StoreT,512>>;


        /// The value of the global scope
        StoreT                                  globalStore;

        /// \b %StringTree to store data for language-related scopes (path,source,method)
        LanguageStore*                          languageStore;

        /// A list of domain paths of \e Scope::ThreadOuter
        std::map<int, std::vector<StoreT>>      threadOuterStore;

        /// A list of domain paths of \e Scope::ThreadInner
        std::map<int, std::vector<StoreT>>      threadInnerStore;


    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:

        /// ScopeInfo of 'our' lox
        ScopeInfo&                              scopeInfo;

        /** If true, values stored for thread scopes will be always replaced instead of appended.
         *  This is for example \c true for <em>Log Data</em> and \c false for
         *  <em>Scope Domains</em>                                                                */
        bool                                    cfgSingleThreadValue;

        /// Flag used to lazily create the key to language-related scope values
        bool                                    lazyLanguageNode;

        /// Indicates if currently a scope walk is active
        bool                                    walking;

        /// The actual scope of a walk
        Scope                                   actScope;

        /// The actual language related scopes' map node
        typename LanguageStore::Cursor          actStringTreeNode;

        /// The path level when using access methods
        int                                     actPathLevel;

        /// Actual thread ID
        int                                     actThreadID;

        /// The 'local object' returned by a walk after Scope::ThreadInner and before Scope::Method
        StoreT                                  walkLocalObject;

        /// The next value of a walk during \e Scope::ThreadInner/Outer
        int                                     walkNextThreadIdx;

        /// The list of values of \e Scope::ThreadOuter/Inner during a walk
        std::vector<StoreT>*                    walkThreadValues;

    // #############################################################################################
    // Public interface
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Constructor
         * @param scopeInfo  The ScopeInfo singleton of 'our' Lox.
         * @param cfgSingleThreadValue Determines if for thread-related scopes, a list of values is
         *                             stored, or just one.
         ******************************************************************************************/
        ALIB_API
        ScopeStore( ScopeInfo& scopeInfo, bool cfgSingleThreadValue );

        /** ****************************************************************************************
         * Destructor
         ******************************************************************************************/
        ALIB_API
        ~ScopeStore();

        /** ****************************************************************************************
         * Clears all data stored, resets all values.
         ******************************************************************************************/
        ALIB_API
        void Clear();

        /** ****************************************************************************************
         * Initializes access methods #Store, #Get and #Remove and has to be invoked prior to
         * using them
         * @param scope      Scope to use.
         * @param pathLevel  Used only if parameter \p{scope} equals
         *                   \ref aworx::lib::lox::Scope::Path "Scope::Path"
         *                   to reference parent directories. Optional and defaults to \c 0.
         * @param threadID   ID of the associated thread (for thread-related scopes only).
         *                   If Thread::NullThreadId is given, the ID from \p{scopeInfo} is used.
         ******************************************************************************************/
        ALIB_API
        void InitAccess    ( Scope scope, int pathLevel, int threadID );

        /** ****************************************************************************************
         * Stores a new value for the given scope.
         * @param value      The value to set.
         * @return Returns the previous value stored.
         ******************************************************************************************/
        StoreT Store    ( StoreT value )
        {
            ALIB_ASSERT( value != ScopeStoreType<StoreT>::NullValue() );
            return access( 0, value );
        }


        /** ****************************************************************************************
         * Removes a value for the given scope.
         * @param value      The value to remove (must only be given for thread-related \e Scopes).
         * @return Returns the previous value stored.
         ******************************************************************************************/
        StoreT Remove   ( StoreT value )
        {
            return access( 1, value );
        }

        /** ****************************************************************************************
         * Retrieves the value for the given scope.
         * If Thread::NullThreadId is given, the ID from \p{scopeInfo} is used.
         * @return Returns the current value stored.
         ******************************************************************************************/
        StoreT Get      ()
        {
            return access( 2, ScopeStoreType<StoreT>::NullValue() );
        }

        /** ****************************************************************************************
         * Initializes a scope 'walk' by storing the given scope information and
         * setting fields of our walk 'state-machine' to proper start values.
         *
         * @param startScope       The \e Scope to start searching for.
         * @param localObject      The 'local object' returned by a walk after Scope::ThreadInner
         *                         and before Scope::Method
         ******************************************************************************************/
        ALIB_API
        void InitWalk( Scope startScope, const StoreT localObject  );

        /** ****************************************************************************************
         * Searches value in the actual scope. While not found, moves walk state to next outer
         * and continues there.
         * @return The next object found in the current or any next outer scope.
         ******************************************************************************************/
        ALIB_API
        StoreT Walk();

    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:

        /** ****************************************************************************************
         * Retrieves and optionally creates an entry in the map that stores language-related
         * scope information. The result is stored in field #actStringTreeNode.
         * @param create     If \c true, a non-existing entry is created.
         ******************************************************************************************/
        void initCursor( bool create );

        /** ****************************************************************************************
         * Receives, inserts or removes a value.
         * @param cmd        0= insert, 1= remove, 2= get.
         * @param value      The new value or the one to be removed.
         * @return Returns the previous value stored.
         ******************************************************************************************/
        StoreT access( int cmd, StoreT value );

}; // ScopeStore


//! @cond NO_DOX
// needed to suppress pedantic warnings with clang
#if !defined(_MSC_VER)
extern template                                ScopeStore<NAString*>                   ::ScopeStore      (ScopeInfo&, bool);
extern template                                ScopeStore<NAString*>                   ::~ScopeStore     ();
extern template   void                         ScopeStore<NAString*>                   ::Clear           ();
extern template   void                         ScopeStore<NAString*>                   ::InitWalk        (Scope,NAString*);
extern template   NAString*                    ScopeStore<NAString*>                   ::Walk            ();
extern template   void                         ScopeStore<NAString*>                   ::InitAccess      (Scope,int,int);
extern template   void                         ScopeStore<NAString*>                   ::initCursor  (bool);
extern template   NAString*                    ScopeStore<NAString*>                   ::access          (int,NAString*);
extern template                                ScopeStore<Box*>                        ::ScopeStore      (ScopeInfo&, bool);
extern template                                ScopeStore<Box*>                        ::~ScopeStore     ();
extern template   void                         ScopeStore<Box*>                        ::Clear           ();
extern template   void                         ScopeStore<Box*>                        ::InitWalk        (Scope,Box*);
extern template   Box*                         ScopeStore<Box*>                        ::Walk            ();
extern template   void                         ScopeStore<Box*>                        ::InitAccess      (Scope,int,int);
extern template   void                         ScopeStore<Box*>                        ::initCursor  (bool);
extern template   Box*                         ScopeStore<Box*>                        ::access          (int,Box*);
extern template                                ScopeStore<std::map<AString, int>*>     ::ScopeStore      (ScopeInfo&, bool);
extern template                                ScopeStore<std::map<AString, int>*>     ::~ScopeStore     ();
extern template   void                         ScopeStore<std::map<AString, int>*>     ::Clear           ();
extern template   void                         ScopeStore<std::map<AString, int>*>     ::InitWalk        (Scope,std::map<AString, int>*);
extern template   std::map<AString, int>*      ScopeStore<std::map<AString, int>*>     ::Walk            ();
extern template   void                         ScopeStore<std::map<AString, int>*>     ::InitAccess      (Scope,int,int);
extern template   void                         ScopeStore<std::map<AString, int>*>     ::initCursor  (bool);
extern template   std::map<AString, int>*      ScopeStore<std::map<AString, int>*>     ::access          (int,std::map<AString, int>*);
extern template                                ScopeStore<std::map<AString, Box>*>::ScopeStore      (ScopeInfo&, bool);
extern template                                ScopeStore<std::map<AString, Box>*>::~ScopeStore     ();
extern template   void                         ScopeStore<std::map<AString, Box>*>::Clear           ();
extern template   void                         ScopeStore<std::map<AString, Box>*>::InitWalk        (Scope,std::map<AString, Box>*);
extern template   std::map<AString, Box>* ScopeStore<std::map<AString, Box>*>::Walk            ();
extern template   void                         ScopeStore<std::map<AString, Box>*>::InitAccess      (Scope,int,int);
extern template   void                         ScopeStore<std::map<AString, Box>*>::initCursor  (bool);
extern template   std::map<AString, Box>* ScopeStore<std::map<AString, Box>*>::access          (int,std::map<AString, Box>*);
#endif

template<> inline bool                         ScopeStoreType<NAString*>                   ::AreEqual    ( NAString* first, NAString* second )                                         { return first->Equals( second );   }
template<> inline bool                         ScopeStoreType<NAString*>                   ::IsNull      ( NAString* value )                                                          { return value == nullptr;          }
template<> inline NAString*                    ScopeStoreType<NAString*>                   ::NullValue   ()                                                                          { return nullptr;                   }
template<> inline bool                         ScopeStoreType<Box*>                        ::AreEqual    ( Box* first, Box* second )                                                 { return first->Invoke<IEquals, bool>(second);    }
template<> inline bool                         ScopeStoreType<Box*>                        ::IsNull      ( Box* value )                                                              { return value == nullptr;          }
template<> inline Box*                         ScopeStoreType<Box*>                        ::NullValue   ()                                                                          { return nullptr;                   }
template<> inline bool                         ScopeStoreType<std::map<AString, int>*>     ::AreEqual    ( std::map<AString, int>* first, std::map<AString, int>* second )           { return first == second;  }
template<> inline bool                         ScopeStoreType<std::map<AString, int>*>     ::IsNull      ( std::map<AString, int>* value )                                           { return value == nullptr; }
template<> inline std::map<AString, int>*      ScopeStoreType<std::map<AString, int>*>     ::NullValue   ()                                                                          { return nullptr;          }
template<> inline bool                         ScopeStoreType<std::map<AString, Box>*>::AreEqual    ( std::map<AString, Box>* first, std::map<AString, Box>* second ) { return first == second;  }
template<> inline bool                         ScopeStoreType<std::map<AString, Box>*>::IsNull      ( std::map<AString, Box>* value )                                      { return value == nullptr; }
template<> inline std::map<AString, Box>* ScopeStoreType<std::map<AString, Box>*>::NullValue   ()                                                                          { return nullptr;          }
//! @endcond


}}}}// namespace [aworx::lib::lox::core]



#endif // HPP_ALOX_CORE_SCOPE
