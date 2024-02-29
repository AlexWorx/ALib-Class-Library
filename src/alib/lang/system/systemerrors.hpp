/** ************************************************************************************************
 * \file
 * This header file is part of sub-namespace #  alib::lang::system of module \alib_basecamp of
 * the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CAMP_ERRORS
#define HPP_ALIB_CAMP_ERRORS 1

#if !defined (HPP_ALIB_CAMP_MESSAGE_EXCEPTION)
#   include "alib/lang/message/exception.hpp"
#endif

ALIB_ASSERT_MODULE(CAMP)

#if   defined( __GNUC__ ) || ( defined(_WIN32) && defined(_MSC_VER))
#   if  !defined(_ERRNO_H)
#       include <errno.h>
#   endif
#endif

namespace alib { namespace  lang::system {

/** ************************************************************************************************
 * Denotes result values returned by system functions (glibc, etc).
 **************************************************************************************************/
enum class SystemErrors
{
        UNKNOWN         = -1              , ///< Unknown Error
        OK              =  0              , ///< No Error
        None            =  0              , ///< No Error (alias for OK)

    #if defined (__GLIBC__) || defined(__APPLE__)  || defined(__ANDROID_NDK__)

        eperm           = EPERM           , ///<   1  "Operation not permitted"
        enoent          = ENOENT          , ///<   2  "No such file or directory"
        esrch           = ESRCH           , ///<   3  "No such process"
        eintr           = EINTR           , ///<   4  "Interrupted system call"
        eio             = EIO             , ///<   5  "I/O error"
        enxio           = ENXIO           , ///<   6  "No such device or address"
        e2Big           = E2BIG           , ///<   7  "Argument list too long"
        enoexec         = ENOEXEC         , ///<   8  "Exec format error"
        ebadf           = EBADF           , ///<   9  "Bad file number"
        echild          = ECHILD          , ///<  10  "No child processes"
        eagain          = EAGAIN          , ///<  11  "Try again. (E.g operation would block)"
        enomem          = ENOMEM          , ///<  12  "Out of memory"
        eacces          = EACCES          , ///<  13  "Permission denied"
        efault          = EFAULT          , ///<  14  "Bad address"
        enotblk         = ENOTBLK         , ///<  15  "Block device required"
        ebusy           = EBUSY           , ///<  16  "Device or resource busy"
        eexist          = EEXIST          , ///<  17  "File exists"
        exdev           = EXDEV           , ///<  18  "Cross-device link"
        enodev          = ENODEV          , ///<  19  "No such device"
        enotdir         = ENOTDIR         , ///<  20  "Not a directory"
        eisdir          = EISDIR          , ///<  21  "Is a directory"
        einval          = EINVAL          , ///<  22  "Invalid argument"
        enfile          = ENFILE          , ///<  23  "File table overflow"
        emfile          = EMFILE          , ///<  24  "Too many open files"
        enotty          = ENOTTY          , ///<  25  "Not a typewriter"
        etxtbsy         = ETXTBSY         , ///<  26  "Text file busy"
        efbig           = EFBIG           , ///<  27  "File too large"
        enospc          = ENOSPC          , ///<  28  "No space left on device"
        espipe          = ESPIPE          , ///<  29  "Illegal seek"
        erofs           = EROFS           , ///<  30  "Read-only file system"
        emlink          = EMLINK          , ///<  31  "Too many links"
        epipe           = EPIPE           , ///<  32  "Broken pipe"
        edom            = EDOM            , ///<  33  "Math argument out of domain of func"
        erange          = ERANGE          , ///<  34  "Math result not representable"
        edeadlk         = EDEADLK         , ///<  35  "Resource deadlock would occur"
        enametoolong    = ENAMETOOLONG    , ///<  36  "File name too long"
        enolck          = ENOLCK          , ///<  37  "No record locks available"
        enosys          = ENOSYS          , ///<  38  "Invalid system call number"
        enotempty       = ENOTEMPTY       , ///<  39  "Directory not empty"
        eloop           = ELOOP           , ///<  40  "Too many symbolic links encountered"
        enomsg          = ENOMSG          , ///<  42  "No message of desired type"
        eidrm           = EIDRM           , ///<  43  "Identifier removed"
#if !defined(__APPLE__)
        echrng          = ECHRNG          , ///<  44  "Channel number out of range"
        el2Nsync        = EL2NSYNC        , ///<  45  "Level 2 not synchronized"
        el3Hlt          = EL3HLT          , ///<  46  "Level 3 halted"
        el3Rst          = EL3RST          , ///<  47  "Level 3 reset"
        elnrng          = ELNRNG          , ///<  48  "Link number out of range"
        eunatch         = EUNATCH         , ///<  49  "Protocol driver not attached"

        enocsi          = ENOCSI          , ///<  50  "No CSI structure available"
        el2Hlt          = EL2HLT          , ///<  51  "Level 2 halted"
        ebade           = EBADE           , ///<  52  "Invalid exchange"
        ebadr           = EBADR           , ///<  53  "Invalid request descriptor"
        exfull          = EXFULL          , ///<  54  "Exchange full"
        enoano          = ENOANO          , ///<  55  "No anode"
        ebadrqc         = EBADRQC         , ///<  56  "Invalid request code"
        ebadslt         = EBADSLT         , ///<  57  "Invalid slot"
        ebfont          = EBFONT          , ///<  59  "Bad font file format"
#endif
        enostr          = ENOSTR          , ///<  60  "Device not a stream"
        enodata         = ENODATA         , ///<  61  "No data available"
        etime           = ETIME           , ///<  62  "Timer expired"
        enosr           = ENOSR           , ///<  63  "Out of streams resources"
#if !defined(__APPLE__)
        enonet          = ENONET          , ///<  64  "Machine is not on the network"
        enopkg          = ENOPKG          , ///<  65  "Package not installed"
#endif
        eremote         = EREMOTE         , ///<  66  "Object is remote"
        enolink         = ENOLINK         , ///<  67  "Link has been severed"
#if !defined(__APPLE__)
        eadv            = EADV            , ///<  68  "Advertise error"
        esrmnt          = ESRMNT          , ///<  69  "Srmount error"
        ecomm           = ECOMM           , ///<  70  "Communication error on send"
#endif
        eproto          = EPROTO          , ///<  71  "Protocol error"
        emultihop       = EMULTIHOP       , ///<  72  "Multihop attempted"
#if !defined(__APPLE__)
        edotdot         = EDOTDOT         , ///<  73  "RFS specific error"
#endif
        ebadmsg         = EBADMSG         , ///<  74  "Not a data message"
        eoverflow       = EOVERFLOW       , ///<  75  "Value too large for defined data type"
#if !defined(__APPLE__)
        enotuniq        = ENOTUNIQ        , ///<  76  "Name not unique on network"
        ebadfd          = EBADFD          , ///<  77  "File descriptor in bad state"
        eremchg         = EREMCHG         , ///<  78  "Remote address changed"
        elibacc         = ELIBACC         , ///<  79  "Can not access a needed shared library"
        elibbad         = ELIBBAD         , ///<  80  "Accessing a corrupted shared library"
        elibscn         = ELIBSCN         , ///<  81  ".lib section in a.out corrupted"
        elibmax         = ELIBMAX         , ///<  82  "Attempting to link in too many shared libraries"
        elibexec        = ELIBEXEC        , ///<  83  "Cannot exec a shared library directly"
#endif
        eilseq          = EILSEQ          , ///<  84  "Illegal byte sequence"
#if !defined(__APPLE__)
        erestart        = ERESTART        , ///<  85  "Interrupted system call should be restarted"
        estrpipe        = ESTRPIPE        , ///<  86  "Streams pipe error"
#endif
        eusers          = EUSERS          , ///<  87  "Too many users"
        enotsock        = ENOTSOCK        , ///<  88  "Socket operation on non-socket"
        edestaddrreq    = EDESTADDRREQ    , ///<  89  "Destination address required"
        emsgsize        = EMSGSIZE        , ///<  90  "Message too long"
        eprototype      = EPROTOTYPE      , ///<  91  "Protocol wrong type for socket"
        enoprotoopt     = ENOPROTOOPT     , ///<  92  "Protocol not available"
        eprotonosupport = EPROTONOSUPPORT , ///<  93  "Protocol not supported"
        esocktnosupport = ESOCKTNOSUPPORT , ///<  94  "Socket type not supported"
        eopnotsupp      = EOPNOTSUPP      , ///<  95  "Operation not supported on transport endpoint"
        epfnosupport    = EPFNOSUPPORT    , ///<  96  "Protocol family not supported"
        eafnosupport    = EAFNOSUPPORT    , ///<  97  "Address family not supported by protocol"
        eaddrinuse      = EADDRINUSE      , ///<  98  "Address already in use"
        eaddrnotavail   = EADDRNOTAVAIL   , ///<  99  "Cannot assign requested address"
        enetdown        = ENETDOWN        , ///< 100  "Network is down"
        enetunreach     = ENETUNREACH     , ///< 101  "Network is unreachable"
        enetreset       = ENETRESET       , ///< 102  "Network dropped connection because of reset"
        econnaborted    = ECONNABORTED    , ///< 103  "Software caused connection abort"
        econnreset      = ECONNRESET      , ///< 104  "Connection reset by peer"
        enobufs         = ENOBUFS         , ///< 105  "No buffer space available"
        eisconn         = EISCONN         , ///< 106  "Transport endpoint is already connected"
        enotconn        = ENOTCONN        , ///< 107  "Transport endpoint is not connected"
        eshutdown       = ESHUTDOWN       , ///< 108  "Cannot send after transport endpoint shutdown"
        etoomanyrefs    = ETOOMANYREFS    , ///< 109  "Too many references: cannot splice"
        etimedout       = ETIMEDOUT       , ///< 110  "Connection timed out"
        econnrefused    = ECONNREFUSED    , ///< 111  "Connection refused"
        ehostdown       = EHOSTDOWN       , ///< 112  "Host is down"
        ehostunreach    = EHOSTUNREACH    , ///< 113  "No route to host"
        ealready        = EALREADY        , ///< 114  "Operation already in progress"
        einprogress     = EINPROGRESS     , ///< 115  "Operation now in progress"
        estale          = ESTALE          , ///< 116  "Stale file handle"
#if !defined(__APPLE__)
        euclean         = EUCLEAN         , ///< 117  "Structure needs cleaning"
        enotnam         = ENOTNAM         , ///< 118  "Not a XENIX named type file"
        enavail         = ENAVAIL         , ///< 119  "No XENIX semaphores available"
        eisnam          = EISNAM          , ///< 120  "Is a named type file"
        eremoteio       = EREMOTEIO       , ///< 121  "Remote I/O error"
#endif
        edquot          = EDQUOT          , ///< 122  "Quota exceeded"
#if !defined(__APPLE__)
        enomedium       = ENOMEDIUM       , ///< 123  "No medium found"
        emediumtype     = EMEDIUMTYPE     , ///< 124  "Wrong medium type"
#endif
        ecanceled       = ECANCELED       , ///< 125  "Operation Canceled"
#if !defined(__APPLE__)
        enokey          = ENOKEY          , ///< 126  "Required key not available"
        ekeyexpired     = EKEYEXPIRED     , ///< 127  "Key has expired"
        ekeyrevoked     = EKEYREVOKED     , ///< 128  "Key has been revoked"
        ekeyrejected    = EKEYREJECTED    , ///< 129  "Key was rejected by service"
#endif
        eownerdead      = EOWNERDEAD      , ///< 130  "Owner died"
        enotrecoverable = ENOTRECOVERABLE , ///< 131  "State not recoverable"
#if !defined(__APPLE__)
        erfkill         = ERFKILL         , ///< 132  "Operation not possible due to RF-kill"
        ehwpoison       = EHWPOISON       , ///< 133  "Memory page has hardware error"
#endif

    #elif defined(_WIN32)
        eperm           = EPERM           , ///< 1
        enoent          = ENOENT          , ///< 2
        esrch           = ESRCH           , ///< 3
        eintr           = EINTR           , ///< 4
        eio             = EIO             , ///< 5
        enxio           = ENXIO           , ///< 6
        e2Big           = E2BIG           , ///< 7
        enoexec         = ENOEXEC         , ///< 8
        ebadf           = EBADF           , ///< 9
        echild          = ECHILD          , ///< 10
        eagain          = EAGAIN          , ///< 11
        enomem          = ENOMEM          , ///< 12
        eacces          = EACCES          , ///< 13
        efault          = EFAULT          , ///< 14
        ebusy           = EBUSY           , ///< 16
        eexist          = EEXIST          , ///< 17
        exdev           = EXDEV           , ///< 18
        enodev          = ENODEV          , ///< 19
        enotdir         = ENOTDIR         , ///< 20
        eisdir          = EISDIR          , ///< 21
        enfile          = ENFILE          , ///< 23
        emfile          = EMFILE          , ///< 24
        enotty          = ENOTTY          , ///< 25
        efbig           = EFBIG           , ///< 27
        enospc          = ENOSPC          , ///< 28
        espipe          = ESPIPE          , ///< 29
        erofs           = EROFS           , ///< 30
        emlink          = EMLINK          , ///< 31
        epipe           = EPIPE           , ///< 32
        edom            = EDOM            , ///< 33
        edeadlk         = EDEADLK         , ///< 36
        enametoolong    = ENAMETOOLONG    , ///< 38
        enolck          = ENOLCK          , ///< 39
        enosys          = ENOSYS          , ///< 40
        enotempty       = ENOTEMPTY       , ///< 41
        einval          = EINVAL          , ///< 22
        erange          = ERANGE          , ///< 34
        eilseq          = EILSEQ          , ///< 42
        struncate       = STRUNCATE       , ///< 80
        eaddrinuse      = EADDRINUSE      , ///< 100
        eaddrnotavail   = EADDRNOTAVAIL   , ///< 101
        eafnosupport    = EAFNOSUPPORT    , ///< 102
        ealready        = EALREADY        , ///< 103
        ebadmsg         = EBADMSG         , ///< 104
        ecanceled       = ECANCELED       , ///< 105
        econnaborted    = ECONNABORTED    , ///< 106
        econnrefused    = ECONNREFUSED    , ///< 107
        econnreset      = ECONNRESET      , ///< 108
        edestaddrreq    = EDESTADDRREQ    , ///< 109
        ehostunreach    = EHOSTUNREACH    , ///< 110
        eidrm           = EIDRM           , ///< 111
        einprogress     = EINPROGRESS     , ///< 112
        eisconn         = EISCONN         , ///< 113
        eloop           = ELOOP           , ///< 114
        emsgsize        = EMSGSIZE        , ///< 115
        enetdown        = ENETDOWN        , ///< 116
        enetreset       = ENETRESET       , ///< 117
        enetunreach     = ENETUNREACH     , ///< 118
        enobufs         = ENOBUFS         , ///< 119
        enodata         = ENODATA         , ///< 120
        enolink         = ENOLINK         , ///< 121
        enomsg          = ENOMSG          , ///< 122
        enoprotoopt     = ENOPROTOOPT     , ///< 123
        enosr           = ENOSR           , ///< 124
        enostr          = ENOSTR          , ///< 125
        enotconn        = ENOTCONN        , ///< 126
        enotrecoverable = ENOTRECOVERABLE , ///< 127
        enotsock        = ENOTSOCK        , ///< 128
        enotsup         = ENOTSUP         , ///< 129
        eopnotsupp      = EOPNOTSUPP      , ///< 130

    #if !defined(_WIN32)
        eother          = EOTHER          , ///< 131
    #else
        eother          = 131             , ///< 131
    #endif
        eoverflow       = EOVERFLOW       , ///< 132
        eownerdead      = EOWNERDEAD      , ///< 133
        eproto          = EPROTO          , ///< 134
        eprotonosupport = EPROTONOSUPPORT , ///< 135
        eprototype      = EPROTOTYPE      , ///< 136
        etime           = ETIME           , ///< 137
        etimedout       = ETIMEDOUT       , ///< 138
        etxtbsy         = ETXTBSY         , ///< 139
        ewouldblock     = EWOULDBLOCK     , ///< 140
    #endif
};

/** ************************************************************************************************
 * Denotes exceptions thrown by classes of namespace \ref alib::lang::system.
 **************************************************************************************************/
enum class SystemExceptions
{
    /** Everything is fine. */
    OK,
};

/**
 * Namespace function that creates an according \alib{lang,Exception} to a corresponding
 * system error number.
 *
 * The small challenge here is that arbitrary error numbers (of unknown) type might occur, that
 * do not have a corresponding enum record. \alib{lang::system,SystemErrors::UNKNOWN} is thrown
 * and only the number is displayed in the description text.
 *
 * @param file  File name of the place of exception creation.
 * @param line  Line number of the place of exception creation.
 * @param func  Function/method name of the place of exception creation.
 * @param errNo The system's error number.
 * @return An exception object.
 */
ALIB_API Exception CreateSystemException( const NCString& file, int line, const NCString& func,
                                          int errNo );


} // namespace alib[:: lang::system]

/// Type alias in namespace \b alib.
using     SystemErrors=        lang::system::SystemErrors;

} // namespace [alib]

#endif // HPP_ALIB_CAMP_ERRORS
