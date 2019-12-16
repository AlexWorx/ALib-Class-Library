/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALOX_DETAIL_SCOPE
#define HPP_ALOX_DETAIL_SCOPE 1

#if !defined(HPP_ALIB_LOX_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !defined (HPP_ALOX_DETAIL_DOMAIN)
#   include "alib/alox/detail/domain.inl"
#endif

#if !defined(HPP_ALIB_MONOMEM_STRINGTREE)
#   include "alib/monomem/stringtree.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_HASHMAP)
#   include "alib/monomem/hashmap.hpp"
#endif

#if !defined (_GLIBCXX_MAP) && !defined(_MAP_)
#   include <map>
#endif

// forward declarations
class ScopeInfo;
class PrefixLogable;

namespace aworx { namespace lib { namespace lox { namespace detail {

/** ************************************************************************************************
 * Static axiomatic methods on value types stored in class \b ScopeStore.
 * \note This is a pure internal helper class. Documentation may be limited.
 **************************************************************************************************/
template<typename T>
class ScopeStoreType
{
    public:

    /** ********************************************************************************************
     * Static method to retrieve a value representing 'null' for the template type.
     * @return The value representing null.
     **********************************************************************************************/
    ALIB_API
    static T NullValue();

    /** ********************************************************************************************
     * Returns true if the value is representing 'null'.
     * @param value  The object to test for 'null'.
     * @returns \c true if the value is representing 'null', \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    static bool IsNull( T value );

    /** ********************************************************************************************
     * Compares two template values.
     * @param first          First value.
     * @param second         Second value.
     * @returns \c true if objects are equal, \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    static bool AreEqual( T first, T second );

}; // ScopeStoreType


// forwards
template<typename T, bool TStackedThreadValues>
class ScopeStore;


/**
 * A helper class that has two specializations to implement different versions of methods
 * \alib{lox::detail,ScopeStore::Walk} and
 * \alib{lox::detail,ScopeStore::access} for each boolean value of template argument
 * \p{TStackedThreadValues}.<br>
 * The helper is needed to support If C++ 11 and the current Microsoft compiler by \alib.
 * (With C++ 17, the helper would not be needed.)
 * @tparam T                    The stored object type.
 * @tparam TStackedThreadValues If true, values stored for thread scopes will be always replaced
 *                              instead of appended.
 *                              This is for example \c false for <em>Log Data</em> and
 *                              <em>Log Once</em> and \c true for <em>Scope Domains</em>
 *                              and <em>Prefix Logables</em>.
 */
template<typename T, bool TStackedThreadValues>
struct ScopeStoreHelper
{
    /** Implements \alib{lox::detail,ScopeStore::Walk} with two specializations.
     * @param self The \b ScopeStore that invoked us.
     * @return The result as specified in \alib{lox::detail,ScopeStore::Walk}.
     */
    T   doWalk  ( ScopeStore<T, TStackedThreadValues>& self );

    /** Implements \alib{lox::detail,ScopeStore::access} with two specializations.
     * @param self  The \b ScopeStore that invoked us.
     * @param cmd   Parameter \p{cmd} of the original method.
     * @param value Parameter \p{value} of the original method.
     * @return The result as specified in \alib{lox::detail,ScopeStore::access}.
     */
    T   doAccess( ScopeStore<T, TStackedThreadValues>& self, int cmd, T value );
};

#if !defined(ALIB_DOX)
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

/** ************************************************************************************************
 * This class is responsible for scope-related functionality of class Lox.
 * \note This is a pure internal helper class. Documentation may be limited.
 * @tparam T                    The stored object type.
 * @tparam TStackedThreadValues If true, values stored for thread scopes will be always replaced
 *                              instead of appended.
 *                              This is for example \c false for <em>Log Data</em> and
 *                              <em>Log Once</em> and \c true for <em>Scope Domains</em>
 *                              and <em>Prefix Logables</em>.
 **************************************************************************************************/
template<typename T, bool TStackedThreadValues>
class ScopeStore
{
    #if !defined(ALIB_DOX)
        friend struct ScopeStoreHelper<T, TStackedThreadValues>;
    #endif

    public:
        /**
         * Alias name for the string tree template used for storing language-related data.
         * The language store uses a \c StringTree with a monotonic allocator.
         * This does not lead to memory leaks, because during the life-time of a \b %Lox objects
         * are only added, but never deleted. If a value is unset, the node is not deleted but
         * set to a \e nulled value. This makes the language store very memory efficient (and fast).
         */
        using LanguageStoreT= monomem::StringTree<T, StringTreeNamesMonoAlloc<character>>;

        /**
         * The type of object stored for the thread values. This depends on whether multiple
         * (a stack of) values can be stored, which is not true for log data and log once, as
         * those operate with hash maps.
         */
        using ThreadMapValueT= ATMP_IF_T_F(TStackedThreadValues,
                                             std::vector<T ALIB_COMMA StdContMA<T>>,
                                             T );

        /** The value of the global scope. */
        T                                       globalStore;

        /** \b %StringTree to store data for language-related scopes (path,source,method). */
        LanguageStoreT                          languageStore;

#if ALIB_THREADS
        /** Key type for the thread store. */
        using ThreadMapKey   = std::pair<bool,threads::ThreadID>;

        /** Hash functor for <c>std::pair<bool,ThreadID></c>. */
        struct BoolThreadIDHash
        {
            /**
             * Calculates a hash code.
             * @param src The object to hash.
             * @return The hash code.
             */
            std::size_t operator()(const std::pair<bool, threads::ThreadID>& src)              const
            {
                return src.first ? static_cast<std::size_t>( src.second * 282312799l  )
                                 : static_cast<std::size_t>( src.second * 573292817l  ) ^ static_cast<std::size_t>(-1);
            }
        };

        /** The inner/outer thread map of values. The boolean value of the key is \c true for
         *  the inner store and \c false for the outer. */
        HashMap<ThreadMapKey,
                ThreadMapValueT,
                BoolThreadIDHash >              threadStore;
#endif


    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:

        /** ScopeInfo of 'our' lox. */
        ScopeInfo&                              scopeInfo;

        /** Flag used to lazily create the key to language-related scope values. */
        bool                                    lazyLanguageNode;

        /** Indicates if currently a scope walk is active. */
        bool                                    walking;

        /** The actual scope of a walk. */
        Scope                                   actScope;

        /** The actual language related scope's map node. */
        typename LanguageStoreT::NodePtr        actStringTreeNode;

        /** The path level when using access methods. */
        int                                     actPathLevel;

#if ALIB_THREADS
        /** Actual thread ID */
        threads::ThreadID                       actThreadID;
#endif

        /** The 'local object' returned by a walk after Scope::ThreadInner and before Scope::Method. */
        T                                       walkLocalObject;

        /** The next value of a walk during \e Scope::ThreadInner/Outer. */
        int                                     walkNextThreadIdx;

        /** The list of values of \e Scope::ThreadOuter/Inner during a walk. */
        ThreadMapValueT*                        walkThreadValues;

    // #############################################################################################
    // Public interface
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Constructor
         * @param scopeInfo            The ScopeInfo singleton of 'our' Lox.
         * @param monoAllocator       The monotonic allocator used for the
         *                             \alib{monomem,StringTree} needed by member
         *                             #languageStore.
         ******************************************************************************************/
        ALIB_API
        ScopeStore( ScopeInfo&              scopeInfo,
                    monomem::MonoAllocator* monoAllocator          );

        /** ****************************************************************************************
         * Destructor
         ******************************************************************************************/
        ALIB_API
        ~ScopeStore();

        /** ****************************************************************************************
         * Deletes all data stored and resets containers as the monotonic allocator will be reset
         * after an invocation to this method as well.
         *
         * \note
         *   This method is used only with \alib{lox,Lox::Reset}, which is only provided
         *   to reset a lox in the unit-tests.
         ******************************************************************************************/
        ALIB_API
        void Reset();

        /** ****************************************************************************************
         * Initializes access methods #Store, #Get and #Remove and has to be invoked prior to
         * using them
         * @param scope      Scope to use.
         * @param pathLevel  Used only if parameter \p{scope} equals
         *                   \ref aworx::lib::lox::Scope::Path "Scope::Path"
         *                   to reference parent directories. Optional and defaults to \c 0.
         * @param threadID   ID of the associated thread (for thread-related scopes only).
         *                   If \alib{threads::UNDEFINED} is given, the ID provided in
         *                   \p{scopeInfo} is used.
         ******************************************************************************************/
        ALIB_API
        void InitAccess    ( Scope scope, int pathLevel, threads::ThreadID threadID );

        /** ****************************************************************************************
         * Stores a new value.
         * @param value      The value to set.
         * @return Returns the previous value stored.
         ******************************************************************************************/
        T Store    ( T value )
        {
            ALIB_ASSERT( value != ScopeStoreType<T>::NullValue() )
            return access( 0, value );
        }


        /** ****************************************************************************************
         * Removes a value.
         * @param value      The value to remove (must only be given for thread-related \e Scopes).
         * @return Returns the previous value stored.
         ******************************************************************************************/
        T Remove   ( T value )
        {
            return access( 1, value );
        }

        /** ****************************************************************************************
         * Retrieves the value.
         * @return Returns the current value stored.
         ******************************************************************************************/
        T Get      ()
        {
            return access( 2, ScopeStoreType<T>::NullValue() );
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
        void InitWalk( Scope startScope, const T localObject  );

        /** ****************************************************************************************
         * Searches next value in the actual scope. While not found, moves walk state to next outer
         * state and continues there.
         * @return The next object found in the current or any next outer scope.
         ******************************************************************************************/
        T   Walk() { return ScopeStoreHelper<T, TStackedThreadValues>().doWalk(*this); }




    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Retrieves and optionally creates an entry in the map that stores language-related
         * scope information. The result is stored in field #actStringTreeNode.
         * @param create     If \c true, a non-existing entry is created.
         ******************************************************************************************/
        void initNodePtr( bool create );

        /** ****************************************************************************************
         * Receives, inserts or removes a value.
         * @param cmd        0= insert, 1= remove, 2= get.
         * @param value      The new value or the one to be removed.
         * @return Returns the previous value stored.
         ******************************************************************************************/
        T access( int cmd, T value ) { return ScopeStoreHelper<T, TStackedThreadValues>()
                                              .doAccess( *this, cmd, value ); }
}; // ScopeStore


#if !defined(ALIB_DOX)
#if !defined(_MSC_VER)
extern template                                 ScopeStore<NString                  ,true>    ::ScopeStore (ScopeInfo&, monomem::MonoAllocator* );
extern template                                 ScopeStore<NString                  ,true>    ::~ScopeStore();
extern template   void                          ScopeStore<NString                  ,true>    ::Reset      ();
extern template   void                          ScopeStore<NString                  ,true>    ::InitWalk   (Scope,NString);
extern template   NString                 ScopeStoreHelper<NString                  ,true>    ::doWalk     (ScopeStore<NString, true>& );
extern template   void                          ScopeStore<NString                  ,true>    ::InitAccess (Scope,int,threads::ThreadID);
extern template   void                          ScopeStore<NString                  ,true>    ::initNodePtr(bool);
extern template   NString                 ScopeStoreHelper<NString                  ,true>    ::doAccess   (ScopeStore<NString, true>&, int, NString  );

extern template                                 ScopeStore<PrefixLogable*           ,true>    ::ScopeStore (ScopeInfo&, monomem::MonoAllocator*);
extern template                                 ScopeStore<PrefixLogable*           ,true>    ::~ScopeStore();
extern template   void                          ScopeStore<PrefixLogable*           ,true>    ::Reset      ();
extern template   void                          ScopeStore<PrefixLogable*           ,true>    ::InitWalk   (Scope,PrefixLogable*);
extern template   PrefixLogable*          ScopeStoreHelper<PrefixLogable*           ,true>    ::doWalk     (ScopeStore<PrefixLogable*, true>& );
extern template   void                          ScopeStore<PrefixLogable*           ,true>    ::InitAccess (Scope,int,threads::ThreadID);
extern template   void                          ScopeStore<PrefixLogable*           ,true>    ::initNodePtr(bool);
extern template   PrefixLogable*          ScopeStoreHelper<PrefixLogable*           ,true>    ::doAccess   (ScopeStore<PrefixLogable*, true>&, int, PrefixLogable*  );

extern template                                 ScopeStore<std::map<NString, int>*  ,false>   ::ScopeStore (ScopeInfo&, monomem::MonoAllocator*);
extern template                                 ScopeStore<std::map<NString, int>*  ,false>   ::~ScopeStore();
extern template   void                          ScopeStore<std::map<NString, int>*  ,false>   ::Reset      ();
extern template   void                          ScopeStore<std::map<NString, int>*  ,false>   ::InitWalk   (Scope,std::map<NString, int>*);
extern template   std::map<NString, int>* ScopeStoreHelper<std::map<NString, int>*  ,false>   ::doWalk     (ScopeStore<std::map<NString, int>*, false>& );
extern template   void                          ScopeStore<std::map<NString, int>*  ,false>   ::InitAccess (Scope,int,threads::ThreadID);
extern template   void                          ScopeStore<std::map<NString, int>*  ,false>   ::initNodePtr(bool);
extern template   std::map<NString, int>* ScopeStoreHelper<std::map<NString, int>*  ,false>   ::doAccess   (ScopeStore<std::map<NString, int>*, false>&, int, std::map<NString, int>*  );

extern template                                 ScopeStore<std::map<NString, Box>*  ,false>   ::ScopeStore (ScopeInfo&, monomem::MonoAllocator*);
extern template                                 ScopeStore<std::map<NString, Box>*  ,false>   ::~ScopeStore();
extern template   void                          ScopeStore<std::map<NString, Box>*  ,false>   ::Reset      ();
extern template   void                          ScopeStore<std::map<NString, Box>*  ,false>   ::InitWalk   (Scope,std::map<NString, Box>*);
extern template   std::map<NString, Box>* ScopeStoreHelper<std::map<NString, Box>*  ,false>   ::doWalk     (ScopeStore<std::map<NString, Box>*, false>& );
extern template   void                          ScopeStore<std::map<NString, Box>*  ,false>   ::InitAccess (Scope,int,threads::ThreadID);
extern template   void                          ScopeStore<std::map<NString, Box>*  ,false>   ::initNodePtr(bool);
extern template   std::map<NString, Box>* ScopeStoreHelper<std::map<NString, Box>*  ,false>   ::doAccess   (ScopeStore<std::map<NString, Box>*, false>&, int, std::map<NString, Box>*  );
#endif

template<> inline bool                    ScopeStoreType<NString              >   ::AreEqual   ( NString first, NString second )                                   { return first.Equals( second );      }
template<> inline bool                    ScopeStoreType<NString              >   ::IsNull     ( NString value )                                                   { return value.IsNull();              }
template<> inline NString                 ScopeStoreType<NString              >   ::NullValue  ()                                                                  { return NullNString();               }
template<> inline bool                    ScopeStoreType<PrefixLogable*       >   ::AreEqual   ( PrefixLogable* first, PrefixLogable* second )                     { return static_cast<Box*>(first)->Call<FEquals>(*second); }
template<> inline bool                    ScopeStoreType<PrefixLogable*       >   ::IsNull     ( PrefixLogable* value )                                            { return value == nullptr;              }
template<> inline PrefixLogable*          ScopeStoreType<PrefixLogable*       >   ::NullValue  ()                                                                  { return nullptr;                       }
template<> inline bool                    ScopeStoreType<std::map<NString, int>*> ::AreEqual   ( std::map<NString, int>* first, std::map<NString, int>* second )   { return first == second;  }
template<> inline bool                    ScopeStoreType<std::map<NString, int>*> ::IsNull     ( std::map<NString, int>* value )                                   { return value == nullptr; }
template<> inline std::map<NString, int>* ScopeStoreType<std::map<NString, int>*> ::NullValue  ()                                                                  { return nullptr;          }
template<> inline bool                    ScopeStoreType<std::map<NString, Box>*> ::AreEqual   ( std::map<NString, Box>* first, std::map<NString, Box>* second )   { return first == second;  }
template<> inline bool                    ScopeStoreType<std::map<NString, Box>*> ::IsNull     ( std::map<NString, Box>* value )                                   { return value == nullptr; }
template<> inline std::map<NString, Box>* ScopeStoreType<std::map<NString, Box>*> ::NullValue  ()                                                                  { return nullptr;          }

#endif


}}}}// namespace [aworx::lib::lox::detail]



#endif // HPP_ALOX_DETAIL_SCOPE
