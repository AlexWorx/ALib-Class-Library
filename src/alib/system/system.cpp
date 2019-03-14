// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(HPP_ALIB_SYSTEM_SYSTEM)
#   include "alib/system/system.hpp"
#endif

#if !defined (HPP_ALIB_LIB_ALIBMODULES)
#    include "alib/lib/alibmodules.hpp"
#endif

#if !defined(HPP_ALIB_TIME_TIME)
#   include "alib/time/time.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#    include "alib/resources/resources.hpp"
#endif

#if !defined (HPP_ALIB_RESULTS_RESULTS)
#    include "alib/results/results.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
#    include "alib/strings/substring.hpp"
#endif

using namespace aworx::lib;

ALIB_BOXING_VTABLE_DEFINE( aworx::lib::system::Exceptions   , vt_system_exceptions   )
ALIB_BOXING_VTABLE_DEFINE( aworx::lib::system::SystemErrors , vt_system_systemerrors )

namespace aworx { namespace lib {

system::System SYSTEM;


String   system::System::months[12];
String   system::System::days  [ 7];

namespace system {

System::System()
: Module( ALIB_VERSION, ALIB_REVISION, "ALIB_SYSTEM" )
{
    ALIB_ASSERT_ERROR( this == &SYSTEM,
        "Instances of class System must not be created. Use singleton aworx::lib::SYSTEM" )

    Dependencies.emplace_back( &lib::RESULTS );
}


void System::init( InitLevels level, int, const char**, const wchar_t** )
{
    if( level == InitLevels::PrepareResources )
    {
        ALIB.CheckDistribution();

        ALIB_BOXING_VTABLE_REGISTER( vt_system_exceptions   )
        ALIB_BOXING_VTABLE_REGISTER( vt_system_systemerrors )


        #define EOS ,
        Resources->AddBulk( ResourceCategory,

        // Calendar
          "Months"  , A_CHAR("January,February,March,April,May,June,July,August,September,October,November,December")
        , "Days"    , A_CHAR("Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday,")     EOS

        // Exceptions
        "ExceptionsPrefix" , "system::" ,
        "ExceptionsPostfix", ""         ,
        "Exceptions",      "0,OK"               "NIY,"
                           "1,Error"            "NIY,"
                           "2,FileExists"       "NIY,"
                           "3,InvalidPath"      "NIY"  EOS

        "SpecialFolder",   "0,Root"        ",1,"
                           "1,Current"     ",1,"
                           "3,HomeConfig"  ",5,"
                           "2,Home"        ",1,"
                           "4,Module"      ",1,"
                           "5,Temp"        ",1,"
                           "6,VarTemp"     ",1"        EOS


            // System errors
        #if defined (__GLIBC__) || defined(__APPLE__)
            "SystemErrorsPrefix" , "glibc::"                                 ,
            "OSERR"              , "GNU-C Library call returned {!Q} ({})."  ,
        #elif defined(_WIN32)
            "SystemErrorsPrefix" , "Windows::"                               ,
            "OSERR"              , "Windows system call returned {!Q} ({})." ,
        #endif
        "SystemErrorsPostfix"    , "",
        "SystemErrors",
          "0"    ","    "OK"                ","     ","

#if defined (__GLIBC__) || defined(__APPLE__)
          "1"    ","    "EPERM"             ","  "OSERR"  ","
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
         "43"    ","    "EIDRM"             ","  "OSERR"  ","
#if !defined(__APPLE__)
         "44"    ","    "ECHRNG"            ","  "OSERR"  ","
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
         "59"    ","    "EBFONT"            ","  "OSERR"  ","
#endif
         "60"    ","    "ENOSTR"            ","  "OSERR"  ","
         "61"    ","    "ENODATA"           ","  "OSERR"  ","
         "62"    ","    "ETIME"             ","  "OSERR"  ","
         "63"    ","    "ENOSR"             ","  "OSERR"  ","
#if !defined(__APPLE__)
         "64"    ","    "ENONET"            ","  "OSERR"  ","
         "65"    ","    "ENOPKG"            ","  "OSERR"  ","
#endif
         "66"    ","    "EREMOTE"           ","  "OSERR"  ","
         "67"    ","    "ENOLINK"           ","  "OSERR"  ","
#if !defined(__APPLE__)
         "68"    ","    "EADV"              ","  "OSERR"  ","
         "69"    ","    "ESRMNT"            ","  "OSERR"  ","
         "70"    ","    "ECOMM"             ","  "OSERR"  ","
#endif
         "71"    ","    "EPROTO"            ","  "OSERR"  ","
         "72"    ","    "EMULTIHOP"         ","  "OSERR"  ","
#if !defined(__APPLE__)
         "73"    ","    "EDOTDOT"           ","  "OSERR"  ","
#endif
         "74"    ","    "EBADMSG"           ","  "OSERR"  ","
         "75"    ","    "EOVERFLOW"         ","  "OSERR"  ","
#if !defined(__APPLE__)
         "76"    ","    "ENOTUNIQ"          ","  "OSERR"  ","
         "77"    ","    "EBADFD"            ","  "OSERR"  ","
         "78"    ","    "EREMCHG"           ","  "OSERR"  ","
         "79"    ","    "ELIBACC"           ","  "OSERR"  ","
         "80"    ","    "ELIBBAD"           ","  "OSERR"  ","
         "81"    ","    "ELIBSCN"           ","  "OSERR"  ","
         "82"    ","    "ELIBMAX"           ","  "OSERR"  ","
         "83"    ","    "ELIBEXEC"          ","  "OSERR"  ","
#endif
         "84"    ","    "EILSEQ"            ","  "OSERR"  ","
#if !defined(__APPLE__)
         "85"    ","    "ERESTART"          ","  "OSERR"  ","
         "86"    ","    "ESTRPIPE"          ","  "OSERR"  ","
#endif
         "87"    ","    "EUSERS"            ","  "OSERR"  ","
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
        "116"    ","    "ESTALE"            ","  "OSERR"  ","
#if !defined(__APPLE__)
        "117"    ","    "EUCLEAN"           ","  "OSERR"  ","
        "118"    ","    "ENOTNAM"           ","  "OSERR"  ","
        "119"    ","    "ENAVAIL"           ","  "OSERR"  ","
        "120"    ","    "EISNAM"            ","  "OSERR"  ","
        "121"    ","    "EREMOTEIO"         ","  "OSERR"  ","
#endif
        "122"    ","    "EDQUOT"            ","  "OSERR"  ","
#if !defined(__APPLE__)
        "123"    ","    "ENOMEDIUM"         ","  "OSERR"  ","
        "124"    ","    "EMEDIUMTYPE"       ","  "OSERR"  ","
#endif
        "125"    ","    "ECANCELED"         ","  "OSERR"  ","
#if !defined(__APPLE__)
        "126"    ","    "ENOKEY"            ","  "OSERR"  ","
        "127"    ","    "EKEYEXPIRED"       ","  "OSERR"  ","
        "128"    ","    "EKEYREVOKED"       ","  "OSERR"  ","
        "129"    ","    "EKEYREJECTED"      ","  "OSERR"  ","
#endif
        "130"    ","    "EOWNERDEAD"        ","  "OSERR"  ","
        "131"    ","    "ENOTRECOVERABLE"   ","  "OSERR"
#if !defined(__APPLE__)
                                                          ","
        "132"    ","    "ERFKILL"           ","  "OSERR"  ","
        "133"    ","    "EHWPOISON"         ","  "OSERR"
#endif
                                                           EOS

#elif defined(_WIN32)
          "1"    ","    "EPERM"             ","  "OSERR"  ","
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
        "140"    ","    "EWOULDBLOCK"       ","  "OSERR"  EOS
#endif

        // end of AddBulk()
        nullptr );


        #if ALIB_MODULE_BOXING && ALIB_MODULE_STRINGS

            ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( aworx::lib::system::SystemErrors   )
            ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( aworx::lib::system::Exceptions     )

        #endif

    }

    else if( level == InitLevels::Final )
    {
        Substring parser;
        parser= GetResource( "Months" ); for( int i= 0 ; i < 12 ; ++i ) { months[i]= parser.ConsumeToken(','); }
        parser= GetResource( "Days"   ); for( int i= 0 ; i <  7 ; ++i ) {   days[i]= parser.ConsumeToken(','); }
    }

}

void System::terminationCleanUp()
{
}

}}}// namespace [aworx::lib::system]

