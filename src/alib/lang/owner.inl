//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace lang {

/// Ensures that an object of template type \p{TOwnable} is acquired and properly released when
/// unwinding the stack.
/// This class is meant to be allocated only on the stack and not on the heap.
/// Therefore, the new operators as well as copy and move constructors and assignment operators
/// are declared private.
///
/// With debug-builds, the constructor expects caller source information parameters
/// of type \b CallerInfo.
/// Use macro \ref ALIB_CALLER_PRUNED to prune those with release-builds.
///
/// \see
///   - Several sibling types exist.
///     They are described and listed in chapter \ref alib_threads_locks_auto_owner of the
///     Programmer's Manual of module \alib_threads.
///   - Besides generic preprocessor macro \ref ALIB_OWN, several application-specific alias macros
///     exist.
///     For example, macros
///     - \ref ALIB_LOCK,
///     - \ref ALIB_LOCK_WITH,
///     - \ref ALIB_DCS, or
///     - \ref ALIB_DCS_WITH
///
///     provide a convenient and "speaking" way to use this class.
///   - Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.
///
/// @tparam TOwnable The type to own.
///                  Requirements are to have methods \b %Acquire and \b %Release available.
template <typename TOwnable>
class Owner
{
    ALIB_STACK_ALLOCATED_TYPE(Owner)

  protected:
    TOwnable&         owned;  ///< The resource to acquire and release.
    #if ALIB_DEBUG
        CallerInfo    dbgCI;  ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release, dbgCI))
        void callRelease() { owned.Release(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release,      ))
        void callRelease() { owned.Release(); }
    #endif
  public:

    /// Constructor. Invokes Acquire() on the owner.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    #if ALIB_DEBUG
        Owner( TOwnable& ownable, const CallerInfo& ci)
        : owned(ownable)
        , dbgCI(ci)                                                       { ownable.Acquire( ci ); }
    #else
        Owner( TOwnable& ownable ) : owned(ownable)                           { ownable.Acquire(); }
    #endif


    /// Destructor. Releases the owner by invoking Release().
    ~Owner()                                                                      { callRelease(); }
}; //class Owner

/// Similar to class \alib{lang;Owner}, but calls method \b TryAcquire instead of \b Acquire.
/// The result is retrievable with method #IsOwning().
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of  module
///      \alib_threads
/// @tparam TOwnable The type to own.
///                  Requirements are to have methods \b %TryAcquire and \b %Release available.
template <typename TOwnable>
class OwnerTry
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerTry)

  protected:
    TOwnable&       owned;     ///< The resource to acquire and release.
    bool            isOwning;  ///< The result of the call to \b TryAcquire.
    #if ALIB_DEBUG
        CallerInfo  dbgCI;     ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release, dbgCI))
        void callRelease() { owned.Release(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release,      ))
        void callRelease() { owned.Release(); }

        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive, dbgCI))
        void callRelease() { owned.ReleaseRecursive(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive,      ))
        void callRelease() { owned.ReleaseRecursive(); }

    #endif
  public:

    /// Constructor. Invokes TryAcquire() on member #owned.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    #if ALIB_DEBUG
        OwnerTry( TOwnable& ownable, const CallerInfo& ci)
        : owned(ownable)
        , dbgCI(ci)                                            { isOwning= ownable.TryAcquire(ci); }
    #else
        OwnerTry( TOwnable& ownable )
        : owned(ownable)                                         { isOwning= ownable.TryAcquire(); }
    #endif


    /// Destructor.  Invokes Release() on member #owned.
    ~OwnerTry()                                                     { if (isOwning) callRelease(); }

    /// @return \c true if the try to acquire the #owned with construction of this type was
    ///            successful, \c false otherwise.
    bool        IsOwning()                                       const noexcept { return isOwning; }
}; //class OwnerTry

/// Similar to class \alib{lang;Owner}, but calls method \b TryAcquireTimed instead of \b Acquire.
/// The result is retrievable with method #IsOwning().
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of  module
///      \alib_threads
/// @tparam TOwnable The type to own.
///                  Requirements are to have methods \b %TryAcquireTimed and \b %Release available.
template <typename TOwnable>
class OwnerTimed
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerTimed)

  protected:
    TOwnable&       owned;      ///< The resource to acquire and release.
    bool            isOwning;   ///< The result of the call to \b TryAcquire.
    #if ALIB_DEBUG
        CallerInfo  dbgCI;      ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release, dbgCI))
        void callRelease() { owned.Release(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release,      ))
        void callRelease() { owned.Release(); }

        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive, dbgCI))
        void callRelease() { owned.ReleaseRecursive(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive,      ))
        void callRelease() { owned.ReleaseRecursive(); }

    #endif
  public:

    /// Constructor. Invokes TryAcquire() on member #owned.
    /// @tparam TTimeValue Type of time parameter accepted with construction and passed to
    ///                    method <b>TOwnable::TryAcquireTimed</b>.<br>
    ///                    Usually this is type \alib{time;Ticks} or
    ///                    \alib{time;TimePointBase::Duration;Ticks::Duration}.
    /// @param ownable  The ownable to acquire.
    /// @param time     The duration to wait for, or point in time to wait until.
    /// @param ci       Caller information.
    #if ALIB_DEBUG
        template<typename TTimeValue>
        OwnerTimed( TOwnable& ownable, const TTimeValue& time, const CallerInfo& ci)
        : owned(ownable)
        , dbgCI(ci)                                 { isOwning= ownable.TryAcquireTimed( time,ci); }
    #else
        template<typename TTimeValue>
        OwnerTimed( TOwnable& ownable, const TTimeValue& time )
        : owned(ownable)                              { isOwning= ownable.TryAcquireTimed( time ); }
    #endif


    /// Destructor.  Invokes Release() on member #owned.
    ~OwnerTimed()                                                   { if (isOwning) callRelease(); }

    /// @return \c true if the try to acquire the #owned with construction of this type was
    ///            successful, \c false otherwise.
    bool        IsOwning()                                       const noexcept { return isOwning; }
}; //class OwnerTimed


/// Similar to class \alib{lang;Owner}, but calls methods \b AcquireRecursive and
/// \b ReleaseRecursive.
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of  module
///      \alib_threads
/// @tparam TOwnable The type to own.
///                  Requirements are to have methods \b %AcquireRecursive and \b %ReleaseRecursive
///                  available.
template <typename TOwnable>
class OwnerRecursive
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerRecursive)

  protected:
    TOwnable&       owned;    ///< The resource to acquire and release.
    #if ALIB_DEBUG
        CallerInfo  dbgCI;    ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive,dbgCI))
        void callRelease() { owned.ReleaseRecursive(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive, ))
        void callRelease() { owned.ReleaseRecursive(); }
    #endif
  public:

    /// Constructor. Invokes AcquireRecursive() on member #owned.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    #if ALIB_DEBUG
        OwnerRecursive( TOwnable& ownable, const CallerInfo& ci)
        : owned(ownable)
        , dbgCI(ci)                                                { ownable.AcquireRecursive(ci); }
    #else
        OwnerRecursive( TOwnable& ownable ) : owned(ownable)         { ownable.AcquireRecursive(); }
    #endif


    /// Destructor.  Invokes ReleaseRecursive() on member #owned.
     ~OwnerRecursive()                                                            { callRelease(); }
}; //class OwnerRecursive

/// Similar to class \alib{lang;Owner}, but calls methods \b AcquireShared and
/// \b ReleaseShared.
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of  module
///      \alib_threads
/// @tparam TOwnable The type to own.
///                  Requirements are to have methods \b %AcquireShared and \b %ReleaseShared
///                  available.
template <typename TOwnable>
class OwnerShared
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerShared)

  protected:
    TOwnable&         owned;    ///< The resource to acquire and release.
    #if ALIB_DEBUG
        CallerInfo    dbgCI;    ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared, dbgCI))
        void callRelease() const { owned.ReleaseShared(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared,     ))
        void callRelease() const { owned.ReleaseShared(); }
    #endif
  public:

    /// Constructor. Invokes AcquireShared() on member #owned.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    #if ALIB_DEBUG
        OwnerShared( TOwnable& ownable, const CallerInfo& ci)
        : owned(ownable)
        , dbgCI(ci)                                                   { ownable.AcquireShared(ci); }
    #else
        OwnerShared( TOwnable& ownable ) : owned(ownable)               { ownable.AcquireShared(); }
    #endif


    /// Destructor.  Invokes ReleaseShared() on member #owned.
    ~OwnerShared()                                                                { callRelease(); }
}; //class OwnerShared

/// Similar to class \alib{lang;Owner}, but calls method \b TryAcquireShared instead of \b Acquire
/// and \b ReleaseShared instead of \b Release.
/// The result is retrievable with method #IsOwning().
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of  module
///      \alib_threads
/// @tparam TOwnable The type to own.
///                  Requirements are to have methods \b %TryAcquireShared and \b %ReleaseShared
///                  available.
template <typename TOwnable>
class OwnerTryShared
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerTryShared)

  protected:
    TOwnable&       owned;      ///< The resource to acquire and release.
    bool            isOwning;   ///< The result of the call to \b TryAcquire.
    #if ALIB_DEBUG
        CallerInfo  dbgCI;      ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared, dbgCI))
        void callRelease() { owned.ReleaseShared(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared,      ))
        void callRelease() { owned.ReleaseShared(); }
    #endif
  public:

    /// Constructor. Invokes TryAcquire() on member #owned.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    #if ALIB_DEBUG
        OwnerTryShared( TOwnable& ownable, const CallerInfo& ci)
        : owned(ownable)
        , dbgCI(ci)                                      { isOwning= ownable.TryAcquireShared(ci); }
    #else
        OwnerTryShared( TOwnable& ownable )
        : owned(ownable)                                   { isOwning= ownable.TryAcquireShared(); }
    #endif


    /// Destructor.  Invokes Release() on member #owned.
    ~OwnerTryShared()                                               { if (isOwning) callRelease(); }

    /// @return \c true if the try to acquire the #owned with construction of this type was
    ///            successful, \c false otherwise.
    bool        IsOwning()                                       const noexcept { return isOwning; }
}; //class OwnerTryShared


/// Similar to class \alib{lang;Owner}, but calls method \b TryAcquireSharedTimed instead
/// of \b Acquire and \b ReleaseShared instead of \b Shared.
/// The result is retrievable with method #IsOwning().
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of  module
///      \alib_threads
/// @tparam TOwnable The type to own.
///                  Requirements are to have methods \b %TryAcquireSharedTimed and
///                  \b %ReleaseShared available.
template <typename TOwnable>
class OwnerSharedTimed
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerSharedTimed)

  protected:
    TOwnable&       owned;      ///< The resource to acquire and release.
    bool            isOwning;   ///< The result of the call to \b TryAcquire.
    #if ALIB_DEBUG
        CallerInfo  dbgCI;      ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared, dbgCI))
        void callRelease() { owned.ReleaseShared(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared,      ))
        void callRelease() { owned.ReleaseShared(); }
    #endif
  public:

    /// Constructor. Invokes TryAcquire() on member #owned.
    /// @tparam TTimeValue Type of time parameter accepted with construction and passed to
    ///                    method <b>TOwnable::TryAcquireTimed</b>.<br>
    ///                    Usually this is type \alib{time;Ticks} or
    ///                    \alib{time;TimePointBase::Duration;Ticks::Duration}.
    /// @param  time       The duration to wait for, or point in time to wait until.
    /// @param  ownable    The ownable to acquire.
    /// @param ci          Caller information.
    #if ALIB_DEBUG
        template<typename TTimeValue>
        OwnerSharedTimed( TOwnable& ownable, const TTimeValue& time, const CallerInfo& ci)
        : owned(ownable)
        , dbgCI(ci)                          { isOwning= ownable.TryAcquireSharedTimed( time, ci); }
    #else
        template<typename TTimeValue>
        OwnerSharedTimed( TOwnable& ownable, const TTimeValue& time )
        : owned(ownable)                        { isOwning= ownable.TryAcquireSharedTimed( time ); }
    #endif


    /// Destructor.  Invokes Release() on member #owned.
    ~OwnerSharedTimed()                                             { if (isOwning) callRelease(); }

    /// @return \c true if the try to acquire the #owned with construction of this type was
    ///            successful, \c false otherwise.
    bool        IsOwning()                                       const noexcept { return isOwning; }
}; //class OwnerSharedTimed

} // namespace alib[::lang]

} // namespace [alib]



