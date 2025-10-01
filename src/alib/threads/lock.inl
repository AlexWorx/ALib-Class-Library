//==================================================================================================
/// \file
/// This header-file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if !ALIB_SINGLE_THREADED
ALIB_EXPORT namespace alib {  namespace threads {

// =================================================================================================
/// This class is a simple wrapper around C++ standard library type \c std::mutex.
/// Thus, it is used to implement <em>mutual exclusive access</em> to resources by protecting
/// critical code sections from being executed in parallel in concurrent threads.
///
/// When a pair of #Acquire and #Release invocations is performed within the same code block, then
/// it is recommended to use a stack instantiation of class \alib{lang;Owner} to acquire and release
/// objects of this class. Such a use is highly simplified with macros \ref ALIB_LOCK and
/// \ref ALIB_LOCK_WITH.
///
/// This class does not allow nested calls to the method #Acquire - method #Release has to be
/// invoked (from within the same thread that acquired this mutex), before any other thread can
/// again gain access. Nested acquisitions constitute undefined behavior.
///
/// \par Debug-Features
/// Public member #Dbg is available with debug-compilations. It offers the following features:
/// - An assertion is raised when nested use is performed.
/// - The object stores the actual owning thread and the source code position of the last
///   acquirement.
/// - Releasing non-acquired instances, as well as destructing acquired one, raise an assertion.
/// - Field \alib{threads::DbgLockAsserter;WaitTimeLimit} enables the raise of \alib warnings in
///   case a certain wait time is exceeded.
///   Note that instead of wrapping \c std::mutex, with debug-compilations class
///   \c std::timed_mutex is wrapped.
///
/// \par Availability
/// This type is not available if the compiler-symbol \ref ALIB_SINGLE_THREADED is set.
///
/// @see
///  - Chapter \ref alib_threads_locks of the Programmer's Manual of the module \alib_threads_nl.
///  - Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.
///  - For this class, as well as for all other five lock types of this module, a
///    \ref alibtools_debug_helpers_gdb "pretty printer" for the GNU debugger is provided.
// =================================================================================================
class Lock
#if ALIB_DEBUG_CRITICAL_SECTIONS
: public lang::DbgCriticalSections::AssociatedLock
#endif
{
  protected:
  #if !ALIB_DEBUG && !DOXYGEN
    std::mutex              mutex; // the only member in release compilations

  #else
    #if DOXYGEN
        /// The internal object to lock on.
        /// \note With debug-compilations, this is of type <c>std::timed_mutex</c>.
        std::mutex          mutex;
    #else
        std::timed_mutex    mutex;
    #endif

  public:
    /// The debug tool instance.
    DbgLockAsserter         Dbg;
  #endif

  public:

    #if ALIB_DEBUG_CRITICAL_SECTIONS
        /// Destructor. With debug-compilations, asserts that this lock is not acquired.
        ~Lock() override
        { Dbg.AssertNotOwned( ALIB_CALLER, ALIB_CALLER, "Destructing acquired lock" ); }

        /// @return \c true if the lock is acquired (in non-shared mode), \c false otherwise.
        ALIB_DLL virtual bool DCSIsAcquired()                                        const override;

        /// @return \c true if the lock is shared-acquired (by at least any thread).
        ///            Otherwise, returns \c false.
        ALIB_DLL virtual bool DCSIsSharedAcquired()                                  const override;

    #elif ALIB_DEBUG
        ~Lock()
        { Dbg.AssertNotOwned( ALIB_CALLER, ALIB_CALLER, "Destructing acquired lock" ); }
    #endif

  #if ALIB_DEBUG || DOXYGEN
    /// Acquires this lock.
    /// In the case that this object is already owned by another thread, the invoking thread is
    /// suspended until ownership can be gained.
    /// Multiple (nested) calls to this method are not supported and lead to undefined behavior.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
     ALIB_DLL
     void  Acquire( ALIB_DBG_TAKE_CI );

    /// Tries to acquire this lock.
    /// Multiple (nested) successful calls to this method or method #Acquire are not supported and
    /// lead to undefined behavior.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    ///
    /// @return \c true if the lock was not acquired by a different thread and thus, this
    ///            call was successful. \c false otherwise.
    ALIB_DLL
    [[nodiscard]]
    bool TryAcquire( ALIB_DBG_TAKE_CI );

    /// Releases ownership of this object.
    /// If this method is invoked on an object that is not acquired, in debug-compilations an
    /// assertion is raised.
    /// In release compilations, this leads to undefined behavior.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    ALIB_DLL
    void Release(ALIB_DBG_TAKE_CI);
  #else
                   void  Acquire()                                                 { mutex.lock(); }
    [[nodiscard]]  bool  TryAcquire()                                   { return mutex.try_lock(); }
                   void  Release()                                               { mutex.unlock(); }
  #endif
};

extern ALIB_DLL Lock STD_IOSTREAMS_LOCK;

} // namespace alib[threads]

/// Type alias in namespace \b alib.
using     Lock= threads::Lock;

} // namespace [alib]
#endif // !ALIB_SINGLE_THREADED

