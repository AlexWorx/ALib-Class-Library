//==================================================================================================
/// \file
/// This header file is part of module \alib_singletons of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_SINGLETONS_SINGLETON
#define HPP_ALIB_SINGLETONS_SINGLETON 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
#if ALIB_STRINGS
#   include "alib/strings/fwds.hpp"
#endif

namespace alib {  namespace singletons {

// #################################################################################################
// Unordered map utility for storing type_info objects
// #################################################################################################

//! @cond NO_DOX
#if ALIB_FEAT_SINGLETON_MAPPED
    extern ALIB_API void* getSingleton   ( const std::type_info& type );
    extern ALIB_API void  storeSingleton ( const std::type_info& type, void* theSingleton );
    extern ALIB_API void  removeSingleton( const std::type_info& type );
#   if ALIB_THREADS
    extern ALIB_API void  unlock();
#   else
    inline  void  unlock() {};
#   endif
#endif
//! @endcond

//==================================================================================================
/// This class implements the "singleton pattern" for C++ using a common templated approach.
/// In case of Windows OS and DLL usage, the class overcomes the problem of having
/// a global data segment per DLL in addition to the one associated with the process that is using
/// the DLL.
///
/// All details about implementation and usage of this class is provided in the module's
/// \ref alib_mod_singletons "Programmer's Manual".
///
/// @tparam TDerivedClass Template parameter that denotes the name of the class that implements
///                       the singleton.
//==================================================================================================
template <typename TDerivedClass>
class Singleton
{
    protected:
        /// A pointer to the one and only singleton.
        static TDerivedClass*  singleton;

    public:
        /// Creates (if not done, yet) and returns the singleton of type \p{TDerivedClass}.
        /// @return The singleton instance.
        static TDerivedClass&    GetSingleton()
        {
            #if !ALIB_FEAT_SINGLETON_MAPPED

                if( singleton == nullptr )
                    singleton= new TDerivedClass();

                return *singleton;

            #else
                // already created and known
                if( singleton != nullptr )
                    return *singleton;


                // try loading from static map
                void* storedSingleton= getSingleton( typeid(TDerivedClass) );
                if( storedSingleton != nullptr ) {
                    singleton= dynamic_cast<TDerivedClass*>(
                                        reinterpret_cast<Singleton<TDerivedClass>*>(storedSingleton) );
                    unlock();
                    return *singleton;
               }

                // create and store in map
                auto* firstInstance= new TDerivedClass();
                storeSingleton( typeid(TDerivedClass), dynamic_cast<Singleton<TDerivedClass>*>( firstInstance ) );

                // In debug mode, do not set this singleton right away. This "simulates"
                // a windows DLL/Exec scope change
                #if ALIB_DEBUG
                    return *firstInstance;
                #else
                    return *(singleton= firstInstance);
                #endif
            #endif
        }

        /// Virtual destructor.
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

//==================================================================================================
/// Deletes the singletons.
/// Upon exit of the process, programmers might want to explicitly free the hash table to avoid
/// the detection of memory leaks by metrics tools like \http{Valgrind,valgrind.org/}.
/// (Otherwise this can be omitted, as the memory is cleaned by the OS probably much faster when a
/// process exits).
///
/// The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib, hence the (overloaded)
/// functions \ref alib::Shutdown will call this function.
///
/// \note This method is not thread-safe and hence must be called only on termination of the process
///       when all threads which are using singletons are terminated.
//==================================================================================================
ALIB_API void  Shutdown();

} // namespace alib[::singletons]

/// Type alias in namespace \b alib.
template<typename T>
using Singleton=    singletons::Singleton<T>;

} // namespace alib



#endif // HPP_ALIB_SINGLETONS_SINGLETON

