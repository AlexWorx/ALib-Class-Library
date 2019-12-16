// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_SYSTEM_SYSTEM)
#      include "alib/system/system.hpp"
#   endif
#   if !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
#      include "alib/strings/numberformat.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#       include "alib/strings/format.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_SUBSTRING)
#      include "alib/strings/substring.hpp"
#   endif
#   if !defined(HPP_ALIB_TIME_TIME)
#      include "alib/time/time.hpp"
#   endif
#   if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#      include "alib/resources/resources.hpp"
#   endif
#   if !defined (HPP_ALIB_RESULTS_RESULTS)
#      include "alib/results/results.hpp"
#   endif
#   if !defined (HPP_ALIB_SYSTEM_DIRECTORY)
#      include "alib/system/directory.hpp"
#   endif
#   if !defined (HPP_ALIB_SYSTEM_CALENDAR)
#      include "alib/system/calendar.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#      include "alib/enums/recordbootstrap.hpp"
#   endif
#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#if !defined(HPP_ALIB_TEXT_FWDS)
#   include "alib/text/fwds.hpp"
#endif
#endif // !defined(ALIB_DOX)

// For code compatibility with ALox Java/C++
// We have to use underscore as the start of the name and for this have to disable a compiler
// warning. But this is a local code (cpp file) anyhow.
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

    #define _NC _<false>

#if defined(__clang__)
    #pragma clang diagnostic pop
#endif


using namespace aworx::lib;

ALIB_BOXING_VTABLE_DEFINE( aworx::lib::system::Exceptions   , vt_system_exceptions   )
ALIB_BOXING_VTABLE_DEFINE( aworx::lib::system::SystemErrors , vt_system_systemerrors )

namespace aworx { namespace lib {

system::System SYSTEM;

namespace system {




#if !defined(ALIB_DOX)
namespace
{
    String months[12];
    String days[7];

    enum Units
    {
        TS_ZERO,
        DayPlural,
        DaySingular,
        HourPlural,
        HourSingular,
        MinPlural,
        MinSingular,
        SecPlural,
        SecSingular,
        MlSecPlural,
        MlSecSingular,
        McSecPlural,
        McSecSingular,
        NSecPlural,
        NSecSingular,        SIZE_OF_UNITS
    };

    String units[SIZE_OF_UNITS];

}
#endif // defined(ALIB_DOX)

System::System()
: Module( ALIB_VERSION, ALIB_REVISION, "SYS" )
{
    ALIB_ASSERT_ERROR( this == &SYSTEM,
        "Instances of class System must not be created. Use singleton aworx::lib::SYSTEM" )
}


void System::bootstrap( BootstrapPhases phase, int, const char**, const wchar_t** )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        ALIB.CheckDistribution();

        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_system_exceptions   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_system_systemerrors )


        #define EOS ,
#if !ALIB_RESOURCES_OMIT_DEFAULTS
        resourcePool->BootstrapBulk( ResourceCategory,

        // Calendar
          "Months"  , A_CHAR("January,February,March,April,May,June,July,August,September,October,November,December")
        , "Days"    , A_CHAR("Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday,")     EOS

          "DurUnts" , A_CHAR("zero time"   ","     //  0  TS_ZERO
                             " days"       ","     //  1  DayP
                             " day"        ","     //  2  DayS
                             " hours"      ","     //  3  HourP
                             " hour"       ","     //  4  HourS
                             " minutes"    ","     //  5  MinP
                             " minute"     ","     //  6  MinS
                             " seconds"    ","     //  7  SecP
                             " second"     ","     //  8  SecS
                             " ms"         ","     //  9  MlSecP
                             " ms"         ","     // 10  MlSecS
                             " \u00B5s"    ","     // 11  McSecP
                             " \u00B5s"    ","     // 12  McSecS
                             " ns"         ","     // 13  NSecP
                             " ns"            ),   // 14  NSecS

        // Exceptions
        "E<", A_CHAR("system::"),
        "E",  A_CHAR("0,OK"               ",NON"  ) EOS

        "SpecialFolder", A_CHAR("0,Root"        ",1,"
                                "1,Current"     ",1,"
                                "3,HomeConfig"  ",5,"
                                "2,Home"        ",1,"
                                "4,Module"      ",1,"
                                "5,Temp"        ",1,"
                                "6,VarTemp"     ",1"  )      EOS


            // System errors
        #if defined (__GLIBC__) || defined(__APPLE__)
            "SE<"                , A_CHAR("glibc::"                               ),
            "OSERR"              , A_CHAR("GNU-C Library call returned {!Q} ({})."),
            "UknSE"              , A_CHAR("GNU-C Library call returned ({}). (Unknown error number)"),
        #elif defined(_WIN32)
            "SE<"                , A_CHAR("WindowsOS::"                            ),
            "OSERR"              , A_CHAR("Windows system call returned {!Q} ({})."),
            "UknSE"              , A_CHAR("Windows system call returned ({}). (Unknown error number)"),
        #endif
        "SE",

   A_CHAR("-1"   ","    "UNKNOWN"           ","  "UknSE"  "," )
   A_CHAR("0"    ","    "OK"                ","           "," )

#if defined (__GLIBC__) || defined(__APPLE__)
   A_CHAR("1"    ","    "EPERM"             ","  "OSERR"  ","
          "2"    ","    "ENOENT"            ","  "OSERR"  ","
          "3"    ","    "ESRCH"             ","  "OSERR"  ","
          "4"    ","    "EINTR"             ","  "OSERR"  ","
          "5"    ","    "EIO"               ","  "OSERR"  ","
          "6"    ","    "ENXIO"             ","  "OSERR"  ","
          "7"    ","    "E2BIG"             ","  "OSERR"  ","
          "8"    ","    "ENOEXEC"           ","  "OSERR"  ","
          "9"    ","    "EBADF"             ","  "OSERR"  ","
         "10"    ","    "ECHILD"            ","  "OSERR"  ","
         "11"    ","    "EAGAIN"            ","  "OSERR"  ","
         "12"    ","    "ENOMEM"            ","  "OSERR"  ","
         "13"    ","    "EACCES"            ","  "OSERR"  ","
         "14"    ","    "EFAULT"            ","  "OSERR"  ","
         "15"    ","    "ENOTBLK"           ","  "OSERR"  ","
         "16"    ","    "EBUSY"             ","  "OSERR"  ","
         "17"    ","    "EEXIST"            ","  "OSERR"  ","
         "18"    ","    "EXDEV"             ","  "OSERR"  ","
         "19"    ","    "ENODEV"            ","  "OSERR"  ","
         "20"    ","    "ENOTDIR"           ","  "OSERR"  ","
         "21"    ","    "EISDIR"            ","  "OSERR"  ","
         "22"    ","    "EINVAL"            ","  "OSERR"  ","
         "23"    ","    "ENFILE"            ","  "OSERR"  ","
         "24"    ","    "EMFILE"            ","  "OSERR"  ","
         "25"    ","    "ENOTTY"            ","  "OSERR"  ","
         "26"    ","    "ETXTBSY"           ","  "OSERR"  ","
         "27"    ","    "EFBIG"             ","  "OSERR"  ","
         "28"    ","    "ENOSPC"            ","  "OSERR"  ","
         "29"    ","    "ESPIPE"            ","  "OSERR"  ","
         "30"    ","    "EROFS"             ","  "OSERR"  ","
         "31"    ","    "EMLINK"            ","  "OSERR"  ","
         "32"    ","    "EPIPE"             ","  "OSERR"  ","
         "33"    ","    "EDOM"              ","  "OSERR"  ","
         "34"    ","    "ERANGE"            ","  "OSERR"  ","
         "35"    ","    "EDEADLK"           ","  "OSERR"  ","
         "36"    ","    "ENAMETOOLONG"      ","  "OSERR"  ","
         "37"    ","    "ENOLCK"            ","  "OSERR"  ","
         "38"    ","    "ENOSYS"            ","  "OSERR"  ","
         "39"    ","    "ENOTEMPTY"         ","  "OSERR"  ","
         "40"    ","    "ELOOP"             ","  "OSERR"  ","
         "42"    ","    "ENOMSG"            ","  "OSERR"  ","
         "43"    ","    "EIDRM"             ","  "OSERR"  "," )
#if !defined(__APPLE__)
  A_CHAR("44"    ","    "ECHRNG"            ","  "OSERR"  ","
         "45"    ","    "EL2NSYNC"          ","  "OSERR"  ","
         "46"    ","    "EL3HLT"            ","  "OSERR"  ","
         "47"    ","    "EL3RST"            ","  "OSERR"  ","
         "48"    ","    "ELNRNG"            ","  "OSERR"  ","
         "49"    ","    "EUNATCH"           ","  "OSERR"  ","
         "50"    ","    "ENOCSI"            ","  "OSERR"  ","
         "51"    ","    "EL2HLT"            ","  "OSERR"  ","
         "52"    ","    "EBADE"             ","  "OSERR"  ","
         "53"    ","    "EBADR"             ","  "OSERR"  ","
         "54"    ","    "EXFULL"            ","  "OSERR"  ","
         "55"    ","    "ENOANO"            ","  "OSERR"  ","
         "56"    ","    "EBADRQC"           ","  "OSERR"  ","
         "57"    ","    "EBADSLT"           ","  "OSERR"  ","
         "59"    ","    "EBFONT"            ","  "OSERR"  ",")
#endif
  A_CHAR("60"    ","    "ENOSTR"            ","  "OSERR"  ","
         "61"    ","    "ENODATA"           ","  "OSERR"  ","
         "62"    ","    "ETIME"             ","  "OSERR"  ","
         "63"    ","    "ENOSR"             ","  "OSERR"  ",")
#if !defined(__APPLE__)
  A_CHAR("64"    ","    "ENONET"            ","  "OSERR"  ","
         "65"    ","    "ENOPKG"            ","  "OSERR"  ",")
#endif
  A_CHAR("66"    ","    "EREMOTE"           ","  "OSERR"  ","
         "67"    ","    "ENOLINK"           ","  "OSERR"  ",")
#if !defined(__APPLE__)
  A_CHAR("68"    ","    "EADV"              ","  "OSERR"  ","
         "69"    ","    "ESRMNT"            ","  "OSERR"  ","
         "70"    ","    "ECOMM"             ","  "OSERR"  ",")
#endif
  A_CHAR("71"    ","    "EPROTO"            ","  "OSERR"  ","
         "72"    ","    "EMULTIHOP"         ","  "OSERR"  ",")
#if !defined(__APPLE__)
  A_CHAR("73"    ","    "EDOTDOT"           ","  "OSERR"  ",")
#endif
  A_CHAR("74"    ","    "EBADMSG"           ","  "OSERR"  ","
         "75"    ","    "EOVERFLOW"         ","  "OSERR"  ",")
#if !defined(__APPLE__)
  A_CHAR("76"    ","    "ENOTUNIQ"          ","  "OSERR"  ","
         "77"    ","    "EBADFD"            ","  "OSERR"  ","
         "78"    ","    "EREMCHG"           ","  "OSERR"  ","
         "79"    ","    "ELIBACC"           ","  "OSERR"  ","
         "80"    ","    "ELIBBAD"           ","  "OSERR"  ","
         "81"    ","    "ELIBSCN"           ","  "OSERR"  ","
         "82"    ","    "ELIBMAX"           ","  "OSERR"  ","
         "83"    ","    "ELIBEXEC"          ","  "OSERR"  ",")
#endif
  A_CHAR("84"    ","    "EILSEQ"            ","  "OSERR"  ",")
#if !defined(__APPLE__)
  A_CHAR("85"    ","    "ERESTART"          ","  "OSERR"  ","
         "86"    ","    "ESTRPIPE"          ","  "OSERR"  ",")
#endif
  A_CHAR("87"    ","    "EUSERS"            ","  "OSERR"  ","
         "88"    ","    "ENOTSOCK"          ","  "OSERR"  ","
         "89"    ","    "EDESTADDRREQ"      ","  "OSERR"  ","
         "90"    ","    "EMSGSIZE"          ","  "OSERR"  ","
         "91"    ","    "EPROTOTYPE"        ","  "OSERR"  ","
         "92"    ","    "ENOPROTOOPT"       ","  "OSERR"  ","
         "93"    ","    "EPROTONOSUPPORT"   ","  "OSERR"  ","
         "94"    ","    "ESOCKTNOSUPPORT"   ","  "OSERR"  ","
         "95"    ","    "EOPNOTSUPP"        ","  "OSERR"  ","
         "96"    ","    "EPFNOSUPPORT"      ","  "OSERR"  ","
         "97"    ","    "EAFNOSUPPORT"      ","  "OSERR"  ","
         "98"    ","    "EADDRINUSE"        ","  "OSERR"  ","
         "99"    ","    "EADDRNOTAVAIL"     ","  "OSERR"  ","
        "100"    ","    "ENETDOWN"          ","  "OSERR"  ","
        "101"    ","    "ENETUNREACH"       ","  "OSERR"  ","
        "102"    ","    "ENETRESET"         ","  "OSERR"  ","
        "103"    ","    "ECONNABORTED"      ","  "OSERR"  ","
        "104"    ","    "ECONNRESET"        ","  "OSERR"  ","
        "105"    ","    "ENOBUFS"           ","  "OSERR"  ","
        "106"    ","    "EISCONN"           ","  "OSERR"  ","
        "107"    ","    "ENOTCONN"          ","  "OSERR"  ","
        "108"    ","    "ESHUTDOWN"         ","  "OSERR"  ","
        "109"    ","    "ETOOMANYREFS"      ","  "OSERR"  ","
        "110"    ","    "ETIMEDOUT"         ","  "OSERR"  ","
        "111"    ","    "ECONNREFUSED"      ","  "OSERR"  ","
        "112"    ","    "EHOSTDOWN"         ","  "OSERR"  ","
        "113"    ","    "EHOSTUNREACH"      ","  "OSERR"  ","
        "114"    ","    "EALREADY"          ","  "OSERR"  ","
        "115"    ","    "EINPROGRESS"       ","  "OSERR"  ","
        "116"    ","    "ESTALE"            ","  "OSERR"  "," )
#if !defined(__APPLE__)
 A_CHAR("117"    ","    "EUCLEAN"           ","  "OSERR"  ","
        "118"    ","    "ENOTNAM"           ","  "OSERR"  ","
        "119"    ","    "ENAVAIL"           ","  "OSERR"  ","
        "120"    ","    "EISNAM"            ","  "OSERR"  ","
        "121"    ","    "EREMOTEIO"         ","  "OSERR"  "," )
#endif
 A_CHAR("122"    ","    "EDQUOT"            ","  "OSERR"  "," )
#if !defined(__APPLE__)
 A_CHAR("123"    ","    "ENOMEDIUM"         ","  "OSERR"  ","
        "124"    ","    "EMEDIUMTYPE"       ","  "OSERR"  "," )
#endif
 A_CHAR("125"    ","    "ECANCELED"         ","  "OSERR"  "," )
#if !defined(__APPLE__)
 A_CHAR("126"    ","    "ENOKEY"            ","  "OSERR"  ","
        "127"    ","    "EKEYEXPIRED"       ","  "OSERR"  ","
        "128"    ","    "EKEYREVOKED"       ","  "OSERR"  ","
        "129"    ","    "EKEYREJECTED"      ","  "OSERR"  "," )
#endif
 A_CHAR("130"    ","    "EOWNERDEAD"        ","  "OSERR"  ","
        "131"    ","    "ENOTRECOVERABLE"   ","  "OSERR"     )
#if !defined(__APPLE__)
 A_CHAR(                                                  ","
        "132"    ","    "ERFKILL"           ","  "OSERR"  ","
        "133"    ","    "EHWPOISON"         ","  "OSERR"       )
#endif
                                                           EOS

#elif defined(_WIN32)
 A_CHAR(  "1"    ","    "EPERM"             ","  "OSERR"  ","
          "2"    ","    "ENOENT"            ","  "OSERR"  ","
          "3"    ","    "ESRCH"             ","  "OSERR"  ","
          "4"    ","    "EINTR"             ","  "OSERR"  ","
          "5"    ","    "EIO"               ","  "OSERR"  ","
          "6"    ","    "ENXIO"             ","  "OSERR"  ","
          "7"    ","    "E2BIG"             ","  "OSERR"  ","
          "8"    ","    "ENOEXEC"           ","  "OSERR"  ","
          "9"    ","    "EBADF"             ","  "OSERR"  ","
         "10"    ","    "ECHILD"            ","  "OSERR"  ","
         "11"    ","    "EAGAIN"            ","  "OSERR"  ","
         "12"    ","    "ENOMEM"            ","  "OSERR"  ","
         "13"    ","    "EACCES"            ","  "OSERR"  ","
         "14"    ","    "EFAULT"            ","  "OSERR"  ","
         "16"    ","    "EBUSY"             ","  "OSERR"  ","
         "17"    ","    "EEXIST"            ","  "OSERR"  ","
         "18"    ","    "EXDEV"             ","  "OSERR"  ","
         "19"    ","    "ENODEV"            ","  "OSERR"  ","
         "20"    ","    "ENOTDIR"           ","  "OSERR"  ","
         "21"    ","    "EISDIR"            ","  "OSERR"  ","
         "23"    ","    "ENFILE"            ","  "OSERR"  ","
         "24"    ","    "EMFILE"            ","  "OSERR"  ","
         "25"    ","    "ENOTTY"            ","  "OSERR"  ","
         "27"    ","    "EFBIG"             ","  "OSERR"  ","
         "28"    ","    "ENOSPC"            ","  "OSERR"  ","
         "29"    ","    "ESPIPE"            ","  "OSERR"  ","
         "30"    ","    "EROFS"             ","  "OSERR"  ","
         "31"    ","    "EMLINK"            ","  "OSERR"  ","
         "32"    ","    "EPIPE"             ","  "OSERR"  ","
         "33"    ","    "EDOM"              ","  "OSERR"  ","
         "36"    ","    "EDEADLK"           ","  "OSERR"  ","
         "38"    ","    "ENAMETOOLONG"      ","  "OSERR"  ","
         "39"    ","    "ENOLCK"            ","  "OSERR"  ","
         "40"    ","    "ENOSYS"            ","  "OSERR"  ","
         "41"    ","    "ENOTEMPTY"         ","  "OSERR"  ","
         "22"    ","    "EINVAL"            ","  "OSERR"  ","
         "34"    ","    "ERANGE"            ","  "OSERR"  ","
         "42"    ","    "EILSEQ"            ","  "OSERR"  ","
         "80"    ","    "STRUNCATE"         ","  "OSERR"  ","
        "100"    ","    "EADDRINUSE"        ","  "OSERR"  ","
        "101"    ","    "EADDRNOTAVAIL"     ","  "OSERR"  ","
        "102"    ","    "EAFNOSUPPORT"      ","  "OSERR"  ","
        "103"    ","    "EALREADY"          ","  "OSERR"  ","
        "104"    ","    "EBADMSG"           ","  "OSERR"  ","
        "105"    ","    "ECANCELED"         ","  "OSERR"  ","
        "106"    ","    "ECONNABORTED"      ","  "OSERR"  ","
        "107"    ","    "ECONNREFUSED"      ","  "OSERR"  ","
        "108"    ","    "ECONNRESET"        ","  "OSERR"  ","
        "109"    ","    "EDESTADDRREQ"      ","  "OSERR"  ","
        "110"    ","    "EHOSTUNREACH"      ","  "OSERR"  ","
        "111"    ","    "EIDRM"             ","  "OSERR"  ","
        "112"    ","    "EINPROGRESS"       ","  "OSERR"  ","
        "113"    ","    "EISCONN"           ","  "OSERR"  ","
        "114"    ","    "ELOOP"             ","  "OSERR"  ","
        "115"    ","    "EMSGSIZE"          ","  "OSERR"  ","
        "116"    ","    "ENETDOWN"          ","  "OSERR"  ","
        "117"    ","    "ENETRESET"         ","  "OSERR"  ","
        "118"    ","    "ENETUNREACH"       ","  "OSERR"  ","
        "119"    ","    "ENOBUFS"           ","  "OSERR"  ","
        "120"    ","    "ENODATA"           ","  "OSERR"  ","
        "121"    ","    "ENOLINK"           ","  "OSERR"  ","
        "122"    ","    "ENOMSG"            ","  "OSERR"  ","
        "123"    ","    "ENOPROTOOPT"       ","  "OSERR"  ","
        "124"    ","    "ENOSR"             ","  "OSERR"  ","
        "125"    ","    "ENOSTR"            ","  "OSERR"  ","
        "126"    ","    "ENOTCONN"          ","  "OSERR"  ","
        "127"    ","    "ENOTRECOVERABLE"   ","  "OSERR"  ","
        "128"    ","    "ENOTSOCK"          ","  "OSERR"  ","
        "129"    ","    "ENOTSUP"           ","  "OSERR"  ","
        "130"    ","    "EOPNOTSUPP"        ","  "OSERR"  ","
        "131"    ","    "EOTHER"            ","  "OSERR"  ","
        "132"    ","    "EOVERFLOW"         ","  "OSERR"  ","
        "133"    ","    "EOWNERDEAD"        ","  "OSERR"  ","
        "134"    ","    "EPROTO"            ","  "OSERR"  ","
        "135"    ","    "EPROTONOSUPPORT"   ","  "OSERR"  ","
        "136"    ","    "EPROTOTYPE"        ","  "OSERR"  ","
        "137"    ","    "ETIME"             ","  "OSERR"  ","
        "138"    ","    "ETIMEDOUT"         ","  "OSERR"  ","
        "139"    ","    "ETXTBSY"           ","  "OSERR"  ","
        "140"    ","    "EWOULDBLOCK"       ","  "OSERR"      ) EOS


#endif

        // end of BootstrapBulk()
        nullptr );
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS


        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( system::SystemErrors   )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( system::Exceptions     )

        lib::boxing::BootstrapRegister<text::FFormat, lib::boxing::TMappedTo<time::DateTime>>(FFormat_DateTime);
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N( time::DateTime::Duration )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W( time::DateTime::Duration )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X( time::DateTime::Duration )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N( time::Ticks   ::Duration )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W( time::Ticks   ::Duration )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X( time::Ticks   ::Duration )


    }

    else if( phase == BootstrapPhases::PrepareConfig )
    {
        EnumRecords<Exceptions              >::Bootstrap();
        EnumRecords<SystemErrors            >::Bootstrap();
        EnumRecords<Directory::SpecialFolder>::Bootstrap( *this, "SpecialFolder" );

    }
    else if( phase == BootstrapPhases::Final )
    {
        Substring parser;
        parser= GetResource( "Months" ); for( int i= 0 ; i < 12 ; ++i ) { months[i]= parser.ConsumeToken(','); }
        parser= GetResource( "Days"   ); for( int i= 0 ; i <  7 ; ++i ) {   days[i]= parser.ConsumeToken(','); }

        parser= GetResource( "DurUnts" );
        for( int i= 0 ; i < Units::SIZE_OF_UNITS ; ++i )
            units[i]= parser.ConsumeToken(',');

        ALIB_ASSERT( units[Units::SIZE_OF_UNITS-1].IsNotEmpty() )
        ALIB_ASSERT( parser.IsEmpty() )
    }
}

Exception CreateSystemException( const NCString& file, int line, const NCString& func, int errNo )
{
    auto* enumRecord= enums::TryRecord( system::SystemErrors(errNo) );
    if( enumRecord == nullptr )
        return Exception( file, line, func, SystemErrors::UNKNOWN, errNo );

    return Exception( file, line, func, SystemErrors(errNo),  // as exception
                                        SystemErrors(errNo),  // boxing the exception's name (!)
                                        errNo                    );
}


// #################################################################################################
// CalendarDateTime
// #################################################################################################

void CalendarDateTime::Clear()
{
    Year=
    Month=
    Day=
    DayOfWeek=
    Hour=
    Minute=
    Second=
    Millisecond=    0;
}

void CalendarDateTime::Set( const DateTime& timeStamp, Timezone timezone )
{
    Clear();

    #if defined (__GLIBCXX__) || defined(__APPLE__)
        struct tm  tm;
        time_t tt= timeStamp.InEpochSeconds();
        if ( timezone == Timezone::UTC )
        {
            tm.tm_isdst=      0; // daylight saving off
            gmtime_r( &tt, &tm );
        }
        else
        {
            tm.tm_isdst=     -1; // daylight saving auto
            localtime_r( &tt, &tm );
        }

        Year=       tm.tm_year + 1900;
        Day=        tm.tm_mday;
        DayOfWeek=  tm.tm_wday;
        Month=      tm.tm_mon + 1;
        Second=     tm.tm_sec;
        Hour=       tm.tm_hour;
        Minute=     tm.tm_min;

    #elif defined( _WIN32 )
        SYSTEMTIME st= timeStamp.ToSystemTime( timezone );

        Year=       st.wYear;
        Day=        st.wDay;
        DayOfWeek=  st.wDayOfWeek;
        Month=      st.wMonth;
        Hour=       st.wHour;
        Minute=     st.wMinute;
        Second=     st.wSecond;

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
}

DateTime  CalendarDateTime::Get( Timezone timezone )
{
    DateTime result(Initialization::Suppress);

    #if defined (__GLIBCXX__) || defined(__APPLE__)
        struct tm  tm;
        tm.tm_year=       Year - 1900;
        tm.tm_mday=       Day;
        tm.tm_mon=        Month -1;
        tm.tm_hour=       Hour;
        tm.tm_min=        Minute;
        tm.tm_sec=        Second;

        time_t tt;
        if ( timezone == Timezone::UTC )
        {
            tm.tm_isdst=      0; // daylight saving off
            tt= timegm( &tm );
        }
        else
        {
            tm.tm_isdst=     -1; // daylight saving auto
            tt= mktime( &tm );
        }

        result= DateTime::FromEpochSeconds( tt );


    #elif defined( _WIN32 )

        SYSTEMTIME st;
        st.wYear=           (WORD) Year;
        st.wDay=            (WORD) Day;
        st.wDayOfWeek=      (WORD) DayOfWeek;
        st.wMonth=          (WORD) Month;
        st.wHour=           (WORD) Hour;
        st.wMinute=         (WORD) Minute;
        st.wSecond=         (WORD) Second;
        st.wMilliseconds=   0;

        result= DateTime::FromSystemTime( st, timezone );

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif

    return result;
}


AString& CalendarDateTime::Format( Substring format, AString& target, CurrentData targetData )
{
    if ( targetData == CurrentData::Clear )
        target.Reset();

    // this ensures that target is not nulled, as all other appends are NC-versions
    target._("");
    NumberFormat* nf= &NumberFormat::Computational;

    while ( format.IsNotEmpty() )
    {
        // read n equal characters
        int   n=  1;
        character c=  format.ConsumeChar();
        while ( format.ConsumeChar(c) )
            ++n;

        switch (c)
        {
            case '\'': // single quotes
            {
                // one or more pairs of single quotes?
                if ( n > 1 )
                {
                    int pairs= n >> 1;
                    target.InsertChars<false>( '\'', pairs );
                    n-= (pairs << 1);
                }

                // one single quote?
                if ( n == 1 )
                {
                    // search end
                    integer end= format.IndexOf( '\'' );
                    if ( end < 1 )
                    {
                        ALIB_WARNING( "Format Error: Missing single Quote" )
                        target <<     "Format Error: Missing single Quote" ;
                        return target;
                    }

                    target._NC( format, 0, end );
                    format.ConsumeChars<false>( end + 1 );
                }

            } break;

            case 's': // second
                target.template _<false>( aworx::Format( Second, n, nf ) );
                break;

            case 'm': //minute
                target._NC( aworx::Format( Minute, n, nf ) );
                break;

            case 'K': // hour 0..11
                target._NC( aworx::Format( Hour % 12, n, nf ) );
                target._NC( Hour < 12 ? " am" : " pm" );
                break;

            case 'H': // hour 0..23
                target._NC( aworx::Format( Hour,   n, nf ) );
                break;

            case 'd': // day
                     if ( n <= 2 )     target._NC( aworx::Format( Day, n, nf) );
                else if ( n == 3 )     target._NC( days[DayOfWeek], 0, 3 );
                else                   target._NC( days[DayOfWeek]    );
                break;

            case 'M': // month
                     if ( n <= 2 )     target._NC( aworx::Format( Month, n, nf ) );
                else if ( n == 3 )     target._NC( months[Month-1], 0, 3 );
                else                   target._NC( months[Month-1]     );
                break;

            case 'y': // year
                     if ( n == 1 )     target._NC( aworx::Format(Year,        1, nf) );
                else if ( n == 2 )     target._NC( aworx::Format(Year %  100, 2, nf) );
                else                   target._NC( aworx::Format(Year,        n, nf) );
                break;

            default: // otherwise: copy what was in
                target.InsertChars<false>( c, n );
                break;
        }

    }

    return target;
}

// #################################################################################################
// CalendarDuration
// #################################################################################################
void CalendarDuration::Clear()
{
    Days=
    Hours=
    Minutes=
    Seconds=
    Milliseconds=
    Microseconds=
    Nanoseconds=    0;
}
#define  NanosPerDay           INT64_C( 86400000000000 ) ///< Constant denoting number of nanoseconds of a day.
#define  NanosPerHour          INT64_C(  3600000000000 ) ///< Constant denoting number of nanoseconds of an hour.
#define  NanosPerMinute        INT64_C(    60000000000 ) ///< Constant denoting number of nanoseconds of a minute.
#define  NanosPerSecond        INT64_C(     1000000000 ) ///< Constant denoting number of nanoseconds of a second.
#define  NanosPerMillisecond   INT64_C(        1000000 ) ///< Constant denoting number of nanoseconds of a millisecond.
#define  NanosPerMicrosecond   INT64_C(           1000 ) ///< Constant denoting number of nanoseconds of a microsecond.


void CalendarDuration::FromNanoSeconds( int64_t nanos )
{
    Clear();
    decltype(nanos) fract;
    if ( nanos > NanosPerDay )          { Days=         static_cast<int>( fract= nanos / NanosPerDay         );  nanos-= fract * NanosPerDay;         }
    if ( nanos > NanosPerHour )         { Hours=        static_cast<int>( fract= nanos / NanosPerHour        );  nanos-= fract * NanosPerHour;        }
    if ( nanos > NanosPerMinute )       { Minutes=      static_cast<int>( fract= nanos / NanosPerMinute      );  nanos-= fract * NanosPerMinute;      }
    if ( nanos > NanosPerSecond )       { Seconds=      static_cast<int>( fract= nanos / NanosPerSecond      );  nanos-= fract * NanosPerSecond;      }
    if ( nanos > NanosPerMillisecond )  { Milliseconds= static_cast<int>( fract= nanos / NanosPerMillisecond );  nanos-= fract * NanosPerMillisecond; }
    if ( nanos > NanosPerMicrosecond )  { Microseconds= static_cast<int>( fract= nanos / NanosPerMicrosecond );                                       }
}

int64_t     CalendarDuration::ToNanoSeconds()
{
    return      Days          * NanosPerDay
             +  Hours         * NanosPerHour
             +  Minutes       * NanosPerMinute
             +  Seconds       * NanosPerSecond
             +  Milliseconds  * NanosPerMillisecond
             +  Microseconds  * NanosPerMicrosecond
             +  Nanoseconds;
}

// #################################################################################################
// FFormat_DateTime
// #################################################################################################
#if !defined(ALIB_DOX)
DOX_MARKER([DOX_ALIB_BOXING_IFORMAT_DATETIME])
void FFormat_DateTime( const Box& box, const String& formatSpec, AString& target )
{
    system::CalendarDateTime tct( box.Unbox<DateTime>() );
    tct.Format( formatSpec, target );
}
DOX_MARKER([DOX_ALIB_BOXING_IFORMAT_DATETIME])
#endif



} // namespace aworx::lib[::system]

#if !defined(ALIB_DOX)
namespace strings {

// #################################################################################################
// T_Append<DateTime::Duration>
// #################################################################################################
using namespace system;

namespace {



    template<typename TChar>
    void appendDateTime(TAString<TChar>& target, const DateTime::Duration pSrc)
    {
        using Duration= DateTime::Duration;
        Duration src= pSrc;
        auto nanos= src.InNanoseconds();
        if( nanos == 0 )
        {
            target << units[Units::TS_ZERO];
            return;
        }

        if( nanos < 0 )
        {
            target << A_CHAR("- ");
            src= Duration() - src;
        }

        TNumberFormat<TChar> nf( TNumberFormat<TChar>::Global );
        nf.FractionalPartWidth= 2;
        int64_t v= src.InAbsoluteDays();
        if( v >= 10 )
        {
            target << TFormat<TChar>( src.InDays(), &nf ) << units[Units::DayPlural];
            return;
        }

        if( v >  0 )
        {
            target << v << ( v != 1 ?  units[Units::DayPlural]
                                    :  units[Units::DaySingular] );

            Duration cpy= src - ( Duration::FromAbsoluteDays(v) );

            target << ' ' << TFormat<TChar>( cpy.InHours(), &nf ) << units[Units::HourPlural];
            return;
        }

        v= src.InAbsoluteHours();
        if( v >  0 )
        {
            target << v << ( v != 1 ?  units[Units::HourPlural]
                                    :  units[Units::HourSingular]  );

            Duration cpy= src - ( Duration::FromAbsoluteHours(v) );

            auto minutes= cpy.InAbsoluteMinutes();
            target << ' ' << minutes <<  (minutes!= 1 ?  units[Units::MinPlural]
                                                      :  units[Units::MinSingular] );
            return;
        }

        v= src.InAbsoluteMinutes();
        if( v > 0 )
        {
            target << v << ( v != 1 ?  units[Units::MinPlural]
                                    :  units[Units::MinSingular] );

            Duration cpy= src - ( Duration::FromAbsoluteMinutes(v) );

            auto seconds= cpy.InAbsoluteSeconds();
            target << ' ' << seconds <<  (seconds!= 1 ?  units[Units::SecPlural]
                                                      :  units[Units::SecSingular] );
            return;
        }

        v= src.InAbsoluteSeconds();
        if( v > 0 )
        {
            target << TFormat<TChar>( src.InSeconds(), &nf ) << units[Units::SecPlural];
            return;
        }

        nf.DecMinimumFieldWidth= 3;

        auto val= src.InAbsoluteMilliseconds();
        if( val >= 1 )
        {
            target << TFormat<TChar>(val,&nf) << ( val!= 1  ?  units[Units::MlSecPlural]
                                                            :  units[Units::MlSecSingular] );
            return;
        }

        val= src.InAbsoluteMicroseconds();
        if( val >= 1 )
        {
            target << TFormat<TChar>(val,&nf) << ( val!= 1  ?  units[Units::McSecPlural]
                                                            :  units[Units::McSecSingular] );
            return;
        }

        val= src.InNanoseconds();
        target << TFormat<TChar>(val,&nf) << ( val!= 1  ?  units[Units::NSecPlural]
                                                        :  units[Units::NSecSingular] );
        return;
    }
} //anonymous namespace


template<typename TChar>
void T_Append<time::DateTime::Duration,TChar>::operator()( TAString<TChar>& target, const time::DateTime::Duration src )
{
    appendDateTime( target, src );
}

template<typename TChar>
void T_Append<time::Ticks::Duration,TChar>::operator()( TAString<TChar>& target, const time::Ticks::Duration src)
{
    // simply convert the ticks-duration to a DateTime duration and use its append function
    appendDateTime( target, time::DateTime::Duration::FromNanoseconds( src.InNanoseconds() ));
}

// Instantiations
template void T_Append<time::DateTime::Duration, nchar>::operator()( TAString<nchar>&, const time::DateTime::Duration   );
template void T_Append<time::DateTime::Duration, wchar>::operator()( TAString<wchar>&, const time::DateTime::Duration   );
template void T_Append<time::DateTime::Duration, xchar>::operator()( TAString<xchar>&, const time::DateTime::Duration   );
template void T_Append<time::Ticks::   Duration, nchar>::operator()( TAString<nchar>&, const time::Ticks   ::Duration   );
template void T_Append<time::Ticks::   Duration, wchar>::operator()( TAString<wchar>&, const time::Ticks   ::Duration   );
template void T_Append<time::Ticks::   Duration, xchar>::operator()( TAString<xchar>&, const time::Ticks   ::Duration   );

} // namespace aworx::lib[::strings]
#endif  //!defined(ALIB_DOX)


}}// namespace [aworx::lib]

