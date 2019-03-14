// #################################################################################################
//  ALib C++ Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_SINGLETONS_SINGLETON
#define HPP_ALIB_SINGLETONS_SINGLETON 1

#if  !defined(HPP_ALIB_SINGLETONS_PREDEF)
#   include "alib/singletons/singletons_predef.hpp"
#endif

#if ALIB_DEBUG && !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif


#if ALIB_MODULE_STRINGS && !defined(HPP_ALIB_STRINGS_FWDS)
#   include "alib/strings/fwds.hpp"
#endif

#if ALIB_FEAT_SINGLETON_MAPPED && ALIB_DEBUG  && !defined(HPP_ALIB_LIB_TYPEMAP)
#   include "alib/lib/typemap.hpp"
#endif

namespace aworx { namespace lib { namespace singletons {

// #################################################################################################
// Unordered map utility for storing type_info objects
// #################################################################################################

//! @cond NO_DOX
#if ALIB_FEAT_SINGLETON_MAPPED
extern ALIB_API bool  getSingleton   ( const std::type_info& type, void* theSingleton );
extern ALIB_API void  storeSingleton ( const std::type_info& type, void* theSingleton );
extern ALIB_API void  removeSingleton( const std::type_info& type );
#endif
//! @endcond

/** ************************************************************************************************
 * This class implements the "singleton pattern" for C++ using a common templated approach.
 * In case of Windows OS and DLL usage, the class overcomes the problem of having
 * a global data segment per DLL in addition to the one associated with the process that is using
 * the DLL.
 *
 * All details about implementation and usage of this class is provided in the module's
 * \ref alib_mod_singletons "Programmer's Manual".
 *
 * @tparam TDerivedClass Template parameter that denotes the name of the class that implements
 *                       the singleton.
 **************************************************************************************************/
template <typename TDerivedClass>
class Singleton
{
    protected:
        /** A pointer to the one and only singleton. */
        static TDerivedClass*  singleton;

    public:
        /**
         * Creates (if not done, yet) and returns the singleton of type \p{TDerivedClass}.
         * @return The singleton instance.
         */
        inline static TDerivedClass&    GetSingleton()
        {
            if( !singleton )
            {
                #if ALIB_FEAT_SINGLETON_MAPPED
                    Singleton<TDerivedClass>* castedAsSingleton;
                    if( !getSingleton(typeid(TDerivedClass), &castedAsSingleton ) )
                    {
                        singleton= new TDerivedClass();
                        castedAsSingleton= dynamic_cast<Singleton<TDerivedClass>*>( singleton );

                        storeSingleton( typeid(TDerivedClass), castedAsSingleton );
                    }
                    else
                    {
                        singleton= dynamic_cast<TDerivedClass*>( castedAsSingleton );
                    }
                #else
                    singleton= new TDerivedClass();
                #endif
            }
            return *singleton;
        }

        /** Virtual destructor. */
        virtual  ~Singleton()
        {
            #if ALIB_FEAT_SINGLETON_MAPPED
                removeSingleton( typeid(TDerivedClass) );
            #endif
        }

};// class Singleton

// The static singleton instance initialization
template <typename TDerivedClass>
TDerivedClass* Singleton<TDerivedClass>::singleton= nullptr;


/** ************************************************************************************************
 * Deletes the singletons.
 * Upon exit of the process, programmers might want to explicitly free the hash table to avoid
 * the detection of memory leaks by metrics tools like [Valgrind](http://valgrind.org/).
 * (Otherwise this can be omitted, as the memory is cleaned by the OS probably much faster when a
 * process exits).
 *
 * When using singletons with the full implementation of \alib (in contrast in using just the
 * module <b>%ALib %Singleton</b>), then method \aworx{lib,Module::TerminationCleanUp} invokes this
 * method already.
 *
 * \note This method is not thread-safe and hence must be called only on termination of the process
 *       when all threads which are using singletons are terminated.
 **************************************************************************************************/
ALIB_API void  DeleteSingletons();

#if ALIB_FEAT_SINGLETON_MAPPED &&  ALIB_DEBUG

    /** ********************************************************************************************
     * This debug helper function returns a type map with void pointers to all singletons.
     *
     * The function may be used to investiage which singleton objects are created within a
     * process and the point in (run-) time of creation.
     *
     * \note
     *   This method is available only in debug compilations of %ALib and if
     *   code selection symbol \ref ALIB_FEAT_SINGLETON_MAPPED evaluates to true.<br>
     *
     * \note
     *   If the \alibdist includes \alibmod_strings then a simple dumping method is available with
     *   \alib{singletons,DbgGetSingletons(NAString&)}.
     *
     * @return The map of singleton types with void pointers to the singletons.
     **********************************************************************************************/
    ALIB_API  TypeMap<void*>    DbgGetSingletons();

    #if ALIB_MODULE_STRINGS
         /** ***************************************************************************************
         * This debug function writes all type names and addresses of each currently defined
         * instance of class
         * \alib{singletons,Singleton}
         * into the given AString.<br>
         *
         * \note
         *   This method is available only
         *   - with \ref ALIB_DEBUG "debug compilations" of the software,
         *   - if module \ref aworx::lib::strings "ALib Strings" is included in the \alibdist, and
         *   - if code selection symbol \ref ALIB_FEAT_SINGLETON_MAPPED is \c true.
         *
         * \note
         *   In case module <b>%ALib Strings</b> is not available, then alternative method
          *  \alib{singletons,DbgGetSingletons} can be used. This returns a list of
          *  \c std::type_info structs together with (void-) pointers to the singletons.
         *
         * @param target The target string to write the list of singletons to.
         *
         * @return The number of singletons written.
         ******************************************************************************************/
        ALIB_API  int               DbgGetSingletons( NAString& target );
    #endif // ALIB_MODULE_STRINGS

#endif // ALIB_FEAT_SINGLETON_MAPPED && ALIB_DEBUG


}}; // namespace aworx[::lib::singletons]

/// Type alias in namespace #aworx.
template<typename T>
using Singleton=    aworx::lib::singletons::Singleton<T>;

} // namespace aworx



#endif // HPP_ALIB_SINGLETONS_SINGLETON
