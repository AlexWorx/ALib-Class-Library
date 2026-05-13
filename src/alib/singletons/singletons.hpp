//==================================================================================================
/// \file
/// This header-file is part of module \alib_singletons of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================

#if ALIB_FEAT_SINGLETON_MAPPED && !DOXYGEN
namespace alib::singletons::hidden {
#   if ALIB_MONOMEM && ALIB_CONTAINERS
        extern HashMap<MonoAllocator, lang::TypeFunctors::Key, void *,
                                      lang::TypeFunctors::Hash,
                                      lang::TypeFunctors::EqualTo,
                                      lang::Caching::Auto,
                                      Recycling::None                      >  singletonMap;
#   else
        extern std::unordered_map<    lang::TypeFunctors::Key, void *,
                                      lang::TypeFunctors::Hash,
                                      lang::TypeFunctors::EqualTo          >  singletonMap;
#   endif
#   if !ALIB_MONOMEM && !ALIB_SINGLE_THREADED
        extern std::mutex singletonMapMutex; // if monomem is included, we use lock of global allocator
#   endif
    extern ALIB_DLL void *getSingleton(const std::type_info &type);
    extern ALIB_DLL void storeSingleton(const std::type_info &type, void *theSingleton);
    extern ALIB_DLL void removeSingleton(const std::type_info &type);
}
#endif

ALIB_EXPORT namespace alib::singletons {
//==================================================================================================
/// This class implements the "singleton pattern" for C++ using a common templated approach.
/// In case of Windows OS and DLL usage, the class overcomes the problem of having
/// a global data segment per DLL in addition to the one associated with the process that is using
/// the DLL.
///
/// All details about implementation and usage of this class is provided in the module's
/// #"alib_mod_singletons;Programmer's Manual".
///
/// @tparam TDerivedClass Template parameter that denotes the name of the class that implements
///                       the singleton.
//==================================================================================================
template<typename TDerivedClass>
class Singleton {
  public:
    #include "ALib.Lang.CIFunctions.H"
    /// Creates (if not done, yet) and returns the singleton of type \p{TDerivedClass}.
    /// @return The singleton instance.
    static TDerivedClass &GetSingleton() {
            #if !ALIB_FEAT_SINGLETON_MAPPED
                // local static atomic instance of the singleton
                static TDerivedClass singleton;
                return singleton;
            #else
                #if ALIB_SINGLE_THREADED
                    using TSingletonStorage = TDerivedClass *;
                #else
                    using TSingletonStorage = std::atomic<TDerivedClass *>;
                #endif

        // local static atomic pointer to the singleton (initialized once)
        static TSingletonStorage singleton = nullptr;

        // already created and known?
        TDerivedClass *result = singleton;
        if (result != nullptr)
            return *result;

        // create it
        {
            // for debug purposes, mapped singletons may be used with single-threaded
            // applications. Still we do not lock then.
                    #if !ALIB_SINGLE_THREADED
                    #   if ALIB_MONOMEM
                            ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)
                    #   else
                            std::lock_guard<std::mutex> guard(hidden::singletonMapMutex);
                    #   endif
                    #endif

            // try loading from static map
            void *storedSingleton = hidden::getSingleton(typeid(TDerivedClass));
            if (storedSingleton != nullptr) {
                singleton = dynamic_cast<TDerivedClass *>(
                    static_cast<Singleton<TDerivedClass> *>(storedSingleton));
                return *singleton;
            }

            // create and store in map
            auto* firstInstance = new TDerivedClass();
            hidden::storeSingleton(typeid(TDerivedClass),
                                   dynamic_cast<Singleton<TDerivedClass> *>(firstInstance));

            // In debug mode, do not set this singleton right away. This "simulates"
            // a windows DLL/Exec scope change
            #if ALIB_DEBUG
            return *firstInstance;
            #else
            return *(singleton = firstInstance);
            #endif
        }
            #endif
    }

        #include "ALib.Lang.CIMethods.H"

    /// Virtual destructor.
    virtual ~Singleton() {
        #if ALIB_FEAT_SINGLETON_MAPPED
            #if !ALIB_SINGLE_THREADED
            #   if ALIB_MONOMEM
                ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)
            #   else
                std::lock_guard<std::mutex> guard(hidden::singletonMapMutex);
            #   endif
            #endif
            hidden::removeSingleton(typeid(TDerivedClass));
        #endif
    }
}; // class Singleton

//==================================================================================================
/// Deletes the singletons.
/// Upon exit of the process, programmers might want to explicitly free the hash table to avoid
/// the detection of memory leaks by metrics tools like \http{Valgrind,valgrind.org/}.
/// (Otherwise this can be omitted, as the memory is cleaned by the OS probably much faster when a
/// process exits).
///
/// The #"alib_mod_bs;standard bootstrap" code of \alib, hence the (overloaded)
/// functions #"alib::Shutdown;2" will call this function.
///
/// \note This method is not thread-safe and hence must be called only on termination of the process
///       when all threads that are using singletons are terminated.
//==================================================================================================
ALIB_DLL void shutdown();
} // namespace [alib::singletons]

ALIB_EXPORT namespace alib {
/// Type alias in namespace #"%alib".
template<typename T>
using Singleton = singletons::Singleton<T>;
} // namespace [alib]

//==================================================================================================
//====== Debug functions
//==================================================================================================
#if ALIB_FEAT_SINGLETON_MAPPED  && ALIB_DEBUG
ALIB_EXPORT namespace alib::singletons {


#if ALIB_MONOMEM && ALIB_CONTAINERS
/// This debug helper function returns a type map with void pointers to all singletons.
///
/// The function may be used to investigate which singleton objects are created within a
/// process and the point in (run-) time of creation.
///
/// \note
///   This method is available only with debug-builds of \alib and if
///   code selection macro #"ALIB_FEAT_SINGLETON_MAPPED" evaluates to true.<br>
///
/// \note
///   If the \alibbuild includes \alib_strings then a simple dumping method is available with
///   #"DbgGetSingletons(NAString&)".
///
/// \attention
///   If modules \alib_monomem and \alib_containers are not included in the \alibbuild, then the
///   result type of this method changes to <c>std::unordered_map<TypeKey, void*>&</c>.
///
/// @return A map of singleton types with void pointers to the singletons.
//==================================================================================================
ALIB_DLL  HashMap<MonoAllocator,
                  lang::TypeFunctors::Key, void *,
                  lang::TypeFunctors::Hash,
                  lang::TypeFunctors::EqualTo,
                  lang::Caching::Auto,
                  Recycling::None> &DbgGetSingletons();
#else
ALIB_DLL  std::unordered_map<lang::TypeFunctors::Key, void *,
                             lang::TypeFunctors::Hash,
                             lang::TypeFunctors::EqualTo> &DbgGetSingletons();
#endif

#if ALIB_STRINGS
/// This debug function writes all type names and addresses of each currently defined
/// instance of class
/// #"Singleton"
/// into the given AString.<br>
///
/// \note
///   This method is available only
///   - with #"ALIB_DEBUG;debug-builds" of the software,
///   - if module \alib_strings is included in the \alibbuild, and
///   - if code selection macro #"ALIB_FEAT_SINGLETON_MAPPED" is \c true.
///
/// \note
///   In case module <b>ALib Strings</b> is not available, then the alternative method
///  #"DbgGetSingletons()" can be used. This returns a list of
///  \c std::type_info structs together with (void-) pointers to the singletons.
///
/// @param target The target string to write the list of singletons to.
///
/// @return The number of singletons written.
    ALIB_DLL  int DbgGetSingletons(NAString&  target);
#endif

} // namespace [alib::singletons]
#endif
