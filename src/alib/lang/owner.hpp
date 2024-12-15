//==================================================================================================
/// \file
/// This header file is part of the \aliblong. It does not belong to an \alibmod and is
/// included in any \alibdist.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LANG_OWNER
#define HPP_ALIB_LANG_OWNER 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
#include "alib/lang/tmp.hpp"
#include <cstddef>

namespace alib { namespace lang {

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
        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, Release, dbgCI))=0>
        void callRelease() { owned.Release(dbgCI); }
        #endif

        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, Release,      ))=0>
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
        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, Release, dbgCI))=0>
        void callRelease() { owned.Release(dbgCI); }
        #endif

        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, Release,      ))=0>
        void callRelease() { owned.Release(); }

        #if  ALIB_DEBUG
        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseRecursive, dbgCI))=0>
        void callRelease() { owned.ReleaseRecursive(dbgCI); }
        #endif

        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseRecursive,      ))=0>
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
        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, Release, dbgCI))=0>
        void callRelease() { owned.Release(dbgCI); }
        #endif

        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, Release,      ))=0>
        void callRelease() { owned.Release(); }

        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseRecursive, dbgCI))=0>
        void callRelease() { owned.ReleaseRecursive(dbgCI); }
        #endif

        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseRecursive,      ))=0>
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
        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseRecursive,dbgCI))=0>
        void callRelease() { owned.ReleaseRecursive(dbgCI); }
        #endif

        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseRecursive, ))=0>
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
        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseShared, dbgCI))=0>
        void callRelease() const { owned.ReleaseShared(dbgCI); }
        #endif

        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseShared,     ))=0>
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
        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseShared, dbgCI))=0>
        void callRelease() { owned.ReleaseShared(dbgCI); }
        #endif

        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseShared,      ))=0>
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
        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseShared, dbgCI))=0>
        void callRelease() { owned.ReleaseShared(dbgCI); }
        #endif

        template<typename TEnableIf= TOwnable,
        ATMP_T_IF(int,  ATMP_HAS_METHOD(TEnableIf, ReleaseShared,      ))=0>
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


// #################################################################################################
// Macros
// #################################################################################################
#define   ALIB_OWN(          ownable) alib::lang::Owner         <decltype(ownable)> ALIB_IDENTIFIER(owner) (ownable ALIB_COMMA_CALLER_PRUNED);
#define   ALIB_OWN_RECURSIVE(ownable) alib::lang::OwnerRecursive<decltype(ownable)> ALIB_IDENTIFIER(owner) (ownable ALIB_COMMA_CALLER_PRUNED);
#define   ALIB_OWN_SHARED(   ownable) alib::lang::OwnerShared   <decltype(ownable)> ALIB_IDENTIFIER(owner) (ownable ALIB_COMMA_CALLER_PRUNED);

// Thread-related aliases (placed here to achieve code agnostic from availability of module Threads)
#if ALIB_THREADS
#   define        ALIB_LOCK                              ALIB_OWN(*this)
#   define        ALIB_LOCK_RECURSIVE                    ALIB_OWN_RECURSIVE(*this)
#   define        ALIB_LOCK_SHARED                       ALIB_OWN_SHARED(*this)
#   define        ALIB_LOCK_WITH(lock)                   ALIB_OWN(lock)
#   define        ALIB_LOCK_RECURSIVE_WITH(lock)         ALIB_OWN_RECURSIVE(lock)
#   define        ALIB_LOCK_SHARED_WITH(lock)            ALIB_OWN_SHARED(lock)
#else
#   if ALIB_DEBUG
#       define   ALIB_LOCK                               { alib::DbgAssertSingleThreaded(); }
#       define   ALIB_LOCK_RECURSIVE                     { alib::DbgAssertSingleThreaded(); }
#       define   ALIB_LOCK_SHARED                        { alib::DbgAssertSingleThreaded(); }
#       define   ALIB_LOCK_WITH(VOID)                    { alib::DbgAssertSingleThreaded(); }
#       define   ALIB_LOCK_RECURSIVE_WITH(VOID)          { alib::DbgAssertSingleThreaded(); }
#       define   ALIB_LOCK_SHARED_WITH(VOID)             { alib::DbgAssertSingleThreaded(); }
#   else
#       define   ALIB_LOCK                               {}
#       define   ALIB_LOCK_RECURSIVE                     {}
#       define   ALIB_LOCK_SHARED                        {}
#       define   ALIB_LOCK_WITH(VOID)                    {}
#       define   ALIB_LOCK_RECURSIVE_WITH(VOID)          {}
#       define   ALIB_LOCK_SHARED_WITH(VOID)             {}
#   endif
#endif // !ALIB_THREADS


// Recursive invocation detection
#if ALIB_DEBUG
#define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL                                     \
bool   dbgRecursionDetectionFlag   = false;

#define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS                                                 \
struct RecursionDetection                                                                          \
{                                                                                                  \
    bool& TestMember;                                                                              \
    RecursionDetection( bool& testMember ) : TestMember(testMember) {}                             \
                                                                                                   \
    void  Acquire( const lang::CallerInfo& ci )                                                    \
    {                                                                                              \
       ALIB_ASSERT_ERROR(TestMember==false,"FSOWNER","Forbidden recursive use of method ", ci.Func)\
       TestMember= true;                                                                           \
    }                                                                                              \
    void  Release()    { TestMember= false;    }                                                   \
};                                                                                                 \
RecursionDetection dbgRecursionDetection( dbgRecursionDetectionFlag );                             \
ALIB_OWN(dbgRecursionDetection);
#else
#   define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL
#   define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS
#endif

#endif // HPP_ALIB_LANG_OWNER

