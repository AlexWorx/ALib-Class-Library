// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/lang/basecamp/basecamp.hpp"
#   include "alib/lang/system/processinfo.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#   include "alib/strings/numberformat.hpp"
#   include "alib/strings/substring.hpp"
#   include "alib/strings/astring.hpp"
#   include "alib/strings/detail/numberconversion.hpp"
#   include "alib/time/time.hpp"
#   include "alib/lang/resources/resources.hpp"
#   include "alib/lang/system/path.hpp"
#   include "alib/lang/system/calendar.hpp"
#   include "alib/enums/recordbootstrap.hpp"
#   include "alib/enums/serialization.hpp"
#   include "alib/lang/message/report.hpp"
#   include "alib/lang/format/fwds.hpp"
#   include "alib/lang/format/fmtexceptions.hpp"
#   include "alib/lang/format/formatterpythonstyle.hpp"
#   include "alib/lang/format/formatterjavastyle.hpp"
#   include "alib/lang/format/bytesize.hpp"
#   include "alib/lang/format/fmtcallerinfo.hpp"
#   include "alib/lang/system/environment.hpp"
#   if ALIB_THREADS
#       include "alib/threads/thread.hpp"
#   endif
#   if ALIB_THREADMODEL
#       include "alib/threadmodel/dedicatedworker.hpp"
#   endif
#   if ALIB_CONFIGURATION
#       include "alib/config/configcamp.hpp"
#       include "alib/config/configuration.hpp"
#   endif
#   if ALIB_ALOX
#       include "alib/alox/aloxcamp.hpp"
#   endif
#   if ALIB_CLI
#       include "alib/cli/clicamp.hpp"
#   endif
#   if ALIB_EXPRESSIONS
#       include "alib/expressions/expressionscamp.hpp"
#   endif
#   if ALIB_FILES
#       include "alib/files/filescamp.hpp"
#   endif
#   if ALIB_BITBUFFER
#       include "alib/bitbuffer/arraycompressor.hpp"
#   endif
#   include "alib/compatibility/std_strings_iostream.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#endif // !DOXYGEN

using namespace alib;


ALIB_BOXING_VTABLE_DEFINE( alib::lang::system::SystemExceptions , vt_system_exceptions    )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::system::SystemErrors     , vt_system_systemerrors  )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::system::Path*            , vt_system_path          )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::format::FMTExceptions    , vt_system_fmtexceptions )


namespace alib   {  lang::basecamp::BaseCamp BASECAMP;
namespace lang   {
namespace format { NumberFormat* BYTESIZE_NUMBER_FORMAT= nullptr; }

/// This is the reference documentation of sub-namespace \b system of module \alib_basecamp.
namespace system {}

// #################################################################################################
// Replacement method for ALib Essential Reports
// #################################################################################################
#if ALIB_DEBUG
#if !DOXYGEN
namespace {
#endif
ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
/// This method is installed with \alib{lang::DBG_SIMPLE_ALIB_MSG_PLUGIN} in method
/// \alib{lang;Camp::bootstrap}.
///
/// The message strings are simply passed to the default \alib{lang;Report}.
/// This way, the essential assert, error and message macros are using the \alib report system
/// at the moment that the \alib_basecamp_nl is included in the \alibdist.
///
/// @param ci      Information about the scope of invocation.
/// @param type    The type of message. See \alib{lang;Message}.
/// @param topic   The topic of message.
/// @param qtyMsgs The number of messages in \p{msgs}.
/// @param msgs    A list of strings (this is all that the essential versions of \alib reporting
///                macros provide).
void debugReportPlugin(const CallerInfo& ci, int type, const char* topic,
                       int qtyMsgs, const nchar** msgs)
{
    LocalAllocator1K allocator; 
    Message message( ci, allocator, Report::Types(type), topic );
    for (int i= 0; i< qtyMsgs; ++i )
        message.Add( msgs[i] );
    Report::GetDefault().DoReport( message );
}
ALIB_WARNINGS_RESTORE

#if !DOXYGEN
} // anonymous namespace
#endif
#endif // ALIB_DEBUG


/// This is the reference documentation of sub-namespace \b format of module \alib_basecamp.
///
/// Extensive documentation for this namespace is provided with chapter
/// \ref alib_basecamp_format "4. Namespace alib::lang::format" of the Programmer's Manual of that module.
namespace format {}


#if !DOXYGEN
namespace
{
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
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
    ALIB_WARNINGS_RESTORE
} // namespace alib::lang::format[::anonymous]
#endif // DOXYGEN

namespace basecamp {

BaseCamp::BaseCamp()
: Camp( "ALIB" )
, LocaleFound(nullptr)
{
    ALIB_ASSERT_ERROR( this == &BASECAMP, "BASECAMP",
        "Instances of class BaseCamp must not be created. Use singleton alib::BASECAMP" )
}

void BaseCamp::bootstrap( BootstrapPhases phase )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        #if ALIB_DEBUG
            DBG_SIMPLE_ALIB_MSG_PLUGIN= debugReportPlugin;
        #   if ALIB_THREADS
                Formatter::DefaultLock.Dbg.Name= "DefaultFormatter";
        #   endif
        #endif


        #define EOS ,
#if !ALIB_RESOURCES_OMIT_DEFAULTS
        resourcePool->BootstrapBulk( ResourceCategory,


       // CodeMarker_CommonEnums
       "Alignment",         A_CHAR("0"  ","  "Left"               ","  "1" ","
                                   "1"  ","  "Right"              ","  "1" ","
                                   "2"  ","  "Center"             ","  "1"     ),

       "Bool",              A_CHAR("0"  ","  "False"              ","  "1"  ","
                                   "1"  ","  "True"               ","  "1"  ","
                                   "0"  ","  "0"                  ","  "1"  ","
                                   "1"  ","  "1"                  ","  "1"  ","
                                   "0"  ","  "No"                 ","  "1"  ","
                                   "1"  ","  "Yes"                ","  "1"  ","
                                   "0"  ","  "Off"                ","  "2"  ","
                                   "1"  ","  "On"                 ","  "2"  ","
                                   "0"  ","  "-"                  ","  "1"  ","
                                   "1"  ","  "Ok"                 ","  "2"     ),

       "Caching",           A_CHAR("0"  ","  "Disabled"           ","  "1" ","
                                   "1"  ","  "Enabled"            ","  "1" ","
                                   "1"  ","  "Auto"               ","  "1"     ),

       "Case",              A_CHAR("0"  ","  "Sensitive"          ","  "1" ","
                                   "1"  ","  "Ignore"             ","  "1"     ),

       "ContainerOp",       A_CHAR("0"  ","  "Insert"             ","  "1" ","
                                   "1"  ","  "Remove"             ","  "1" ","
                                   "3"  ","  "GetCreate"          ","  "4" ","
                                   "2"  ","  "Get"                ","  "1" ","
                                   "4"  ","  "Create"             ","  "1"     ),

       // CreateDefaults:    using Bool
       // CreateIfNotExists: using Bool

       "CurrentData",       A_CHAR("0"  ","  "Keep"               ","  "1" ","
                                   "1"  ","  "Clear"              ","  "1"     ),

       "Inclusion",         A_CHAR("0"  ","  "Include"            ","  "1" ","
                                   "1"  ","  "Exclude"            ","  "1"     ),

       "Initialization",    A_CHAR("0"  ","  "Suppress"           ","  "1" ","
                                   "1"  ","  "Default"            ","  "1" ","
                                   "2"  ","  "Nulled"             ","  "1" ","
                                   "0"  ","  "None"               ","  "2" ","
                                   "1"  ","  "Initialize"         ","  "1" ","
                                   "2"  ","  "Zero"               ","  "1"     ),

       "Phase",             A_CHAR("0"  ","  "Begin"              ","  "1" ","
                                   "1"  ","  "End"                ","  "1"     ),

       "Propagation",       A_CHAR("0"  ","  "Omit"               ","  "1" ","
                                   "1"  ","  "ToDescendants"      ","  "1"     ),

       "Reach",             A_CHAR("0"  ","  "Global"             ","  "1" ","
                                   "1"  ","  "Local"              ","  "1"     ),

       // Recursive: using Bool

       "Responsibility",    A_CHAR("0"  ","  "KeepWithSender"     ","  "1" ","
                                   "1"  ","  "Transfer"           ","  "1"     ),

       "Safeness",          A_CHAR("0"  ","  "Safe"               ","  "1" ","
                                   "1"  ","  "Unsafe"             ","  "1"     ),

       "Side",              A_CHAR("0"  ","  "Left"               ","  "1" ","
                                   "1"  ","  "Right"              ","  "1"     ),

       "SortOrder",         A_CHAR("0"  ","  "Ascending"          ","  "1" ","
                                   "1"  ","  "Descending"         ","  "1"     ),

       "SourceData",        A_CHAR("0"  ","  "Copy"               ","  "1" ","
                                   "1"  ","  "Move"               ","  "1"     ),

       "Switch",            A_CHAR("0"  ","  "Off"                ","  "2" ","
                                   "1"  ","  "On"                 ","  "2"     ),

       "Timezone",          A_CHAR("0"  ","  "Local"              ","  "1" ","
                                   "1"  ","  "UTC"                ","  "1"     ),

       "Timing",            A_CHAR("0"  ","  "Async"              ","  "1" ","
                                   "1"  ","  "Sync"               ","  "1" ","
                                   "0"  ","  "Asynchronous"       ","  "1" ","
                                   "1"  ","  "Synchronous"        ","  "1" ","
                                   "1"  ","  "Synchronized"       ","  "1"     ),

       "ValueReference",    A_CHAR("0"  ","  "Absolute"           ","  "1" ","
                                   "1"  ","  "Relative"           ","  "1"     ),

       "Whitespaces",       A_CHAR("0"  ","  "Trim"               ","  "1" ","
                                   "1"  ","  "Keep"               ","  "1"     ),


       #if ALIB_THREADS
       "TSts",             A_CHAR( "0"  ","  "Unstarted"          ","  "1" ","
                                   "1"  ","  "Started"            ","  "3" ","
                                   "2"  ","  "Running"            ","  "1" ","
                                   "3"  ","  "Done"               ","  "3" ","
                                   "4"  ","  "Terminated"         ","  "1"      ),

         #if ALIB_THREADMODEL
           "TMDWP",        A_CHAR(    "0"  ","  "Lowest"              ","  "4" ","
                                    "500"  ","  "DeferredDeletion"    ","  "1" ","
                                   "1000"  ","  "Low"                 ","  "1" ","
                                   "2000"  ","  "Standard"            ","  "1" ","
                                   "4000"  ","  "Highest"             ","  "5" ","
                                   "3000"  ","  "High"                ","  "1"     ),
         #endif
       #endif



#if ALIB_CONFIGURATION
       // variables
DOX_MARKER([DOX_CONFIG_DECLARATION6])
"Var0",    A_CHAR("1|ALIB/LOCALE|"             "S"  ),
"Var1",    A_CHAR("2|ALIB/WAIT_FOR_KEY_PRESS|" "B"  ),
"Var2",    A_CHAR("3|ALIB/HAS_CONSOLE_WINDOW|" "B"  ),

"Var_C1",  A_CHAR("Defines the locale of the application. If empty or not set, the system's locale is used.\n"
                 "If set, a UTF8 local should be used, for example 'us_US.UTF-8' or 'de_DE.UTF-8'" ),
"Var_C2",  A_CHAR("If true, the process waits for a key stroke on termination. If empty, under Windows"   "\n"
                 "behavior is detected, under other OSes, defaults to false."                               ),
"Var_C3",  A_CHAR("Boolean value that denotes what its name indicates. If empty, under Windows value is " "\n"
                 "detected, under other OSes, defaults to true."                                            ),
DOX_MARKER([DOX_CONFIG_DECLARATION6])

#endif // ALIB_CONFIGURATION

       // INI-File comment for section ALIB
       "INI_CMT_ALIB", A_CHAR("@>'/// '@HL-General ALib library settings.\n@HL-"),

       // Exit message when waiting for key-press requested with variable WAIT_FOR_KEY_PRESS
       "EXIT_MSG"    , A_CHAR( "ALIB: Press 'Enter' to exit..." ),

    #if ALIB_BITBUFFER && ALIB_ENUMS
       "ACAlgos"     , A_CHAR(  "0"    ",NONE"                 ","   "1"      ","
                               "31"    ",ALL"                  ","   "1"      ","
                                "1"    ",Uncompressed"         ","   "1"      ","
                                "2"    ",MinMax"               ","   "1"      ","
                                "4"    ",Sparse"               ","   "1"      ","
                                "8"    ",VerySparse"           ","   "1"      ","
                               "16"    ",Incremental"          ","   "1"      ","
                               "32"    ",Huffman"              ","   "1"            ),
    #endif

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

        "FMTDT"  ,  A_CHAR( "yyyy-MM-dd HH:mm:ss"),             // default format string for DateTime values used by FFormat_DateTime()
        "FMTCI"  ,  A_CHAR( "[@ sf:sl from ''ya'' by ''ta'']"), // default format string for CallerInfo values used by FFormat_CallerInfo()
        "FMTCINT",  A_CHAR( "<None>"),       // format output if CallerInfo has no thread set
        "FMTCINY",  A_CHAR( "<None>"),       // format output if CallerInfo originates from static method or namespace function
        "FMTCINR",  A_CHAR( "<Null>"),       // format output if CallerInfo originates from static method or namespace function

        // Exceptions
        "E<", A_CHAR("system::"),
        "E",  A_CHAR("0,OK"               ",NON"  ) EOS

        // Enums
        "SystemFolders", A_CHAR("0,Root"        ",1,"
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

#if defined (__GLIBC__) || defined(__APPLE__)   || defined(__ANDROID_NDK__)
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
#endif // #elif defined(_WIN32)

// ----------------------------------- system/message ------------------------------------
    "REPE<" , A_CHAR("lang::") ,
    "REPE"  , A_CHAR("1,ErrorWritingReport,REPED1") ,

    "REPED1", A_CHAR("Error writing ALib report."),

    "ExcFmtExc",       A_CHAR("\nAn exception occurred while formatting another (!) exception:\n" ),
    "RepFmtExc",       A_CHAR("\nAn exception occurred while formatting an ALib report (!):\n" ),

// ----------------------------------- system/format ------------------------------------
    "FE<", A_CHAR("format::"),

    "PHT",         A_CHAR(  "0,NotGiven"          ",1,"
                            "1,String"            ",1,"
                            "2,Character"         ",1,"
                            "3,IntBase10"         ",6,"
                            "4,IntBinary"         ",6,"
                            "5,IntOctal"          ",6,"
                            "6,IntHex"            ",6,"
                            "7,Float"             ",1,"
                            "8,Bool"              ",1,"
                            "9,HashCode"          ",1,"
                           "10,Fill"              ",4" ) ,


// general formatter errors

    "FE0" , A_CHAR( "11,ArgumentIndexIs0"                       ",FMG11"   ),
    "FE1" , A_CHAR( "12,ArgumentIndexOutOfBounds"               ",FMG12"   ),
    "FE2" , A_CHAR( "13,IncompatibleTypeCode"                   ",FMG13"   ),

    // Formatter Python Style
    "FE3" , A_CHAR( "101,MissingClosingBracket"                 ",FPS101"  ),
    //...
    //...
    "FE4" , A_CHAR( "102,MissingPrecisionValuePS"               ",FPS102"  ),
    "FE5" , A_CHAR( "103,DuplicateTypeCode"                     ",FPS103"  ),
    "FE6" , A_CHAR( "104,UnknownTypeCode"                       ",FPS104"  ),
    "FE7" , A_CHAR( "105,ExclamationMarkExpected"               ",FPS105"  ),
    "FE8" , A_CHAR( "106,UnknownConversionPS"                   ",FPS106"  ),
    "FE9" , A_CHAR( "107,PrecisionSpecificationWithInteger"     ",FPS107"  ),
    "FE10", A_CHAR( "108,MissingReplacementStrings"             ",FPS108"  ),

    // Formatter Java Style
    "FE11" , A_CHAR( "201,NegativeValuesInBracketsNotSupported" ",FJS101"  ),
    "FE12" , A_CHAR( "202,MissingPrecisionValueJS"              ",FJS102"  ),
    "FE13" , A_CHAR( "203,HexadecimalFloatFormatNotSupported"   ",FJS103"  ),
    "FE14" , A_CHAR( "204,NoAlternateFormOfConversion"          ",FJS104"  ),
    "FE15" , A_CHAR( "205,NoPrecisionWithConversion"            ",FJS105"  ),
    "FE16" , A_CHAR( "206,UnknownDateTimeConversionSuffix"      ",FJS106"  ),
    "FE17" , A_CHAR( "207,UnknownConversionJS"                  ",FJS107"  ),

    // PropertyFormatter and PropertyFormatters
    "FE18" , A_CHAR( "501,UnknownPropertyInFormatString"        ",PFM501"  ),
    "FE19" , A_CHAR( "502,ErrorInResultingFormatString"         ",PFM502"  ),
    "FE20" , A_CHAR( "510,MissingConfigurationVariable"         ",PFM510"  ),

    // class Paragraphs
    "FE21" , A_CHAR( "601,UnknownMarker"                        ",ST601"   ),
    "FE22" , A_CHAR( "602,EndmarkerWithoutStart"                ",ST602"   ),

    // general formatter errors
    "FMG11"      , A_CHAR( "Argument index 0 not allowed.\n"
                           "In: {!Q}\n    >{!FillC-}^" ),
    "FMG12"      , A_CHAR( "Argument index #{} is out of bounds. {} arguments are available.\n"
                           "In: {!Q}\n    >{!FillC-}^" ),
    "FMG13"      , A_CHAR( "Detected placeholder type does not match given argument.\n"
                           "Placeholder number:    {}\n"
                           "Placeholder type:      {} ({})\n" )
                           "Deduced argument type: {!Q}\n"
     ALIB_REL_DBG( A_CHAR( "{!X}") ,
                   A_CHAR( "Native argument type:   <{}>\n" )
                           "In: {!Q}\n    >{!FillC-}^"  ),

    // Formatter Python Style
    "FPS101"     , A_CHAR( "Closing bracket '}}' of placeholder not found (or syntax error).\n"
                           "In: {!Q}\n    >{!FillC-}^" ),
    //...
    //...
    //...
    "FPS102"     , A_CHAR( "Missing precision value after '.' character."
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),
    "FPS103"     , A_CHAR( "Duplicate type code {!Q'} given (previous was {!Q'})."
     ALIB_REL_DBG( A_CHAR( "{!X}") , A_CHAR( "\nNative argument type: <{}> ") )
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),
    "FPS104"     , A_CHAR( "Unknown type code {!Q'} given."
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  )
     ALIB_REL_DBG( A_CHAR( "{!X}") , A_CHAR( "\nNative argument type: <{}> ")                             ),
    "FPS105"     , A_CHAR( "Expected '!' in continuation of placeholder."
     ALIB_REL_DBG( A_CHAR( "{!X}") , A_CHAR( "\nNative argument type: <{}> ") )
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),
    "FPS106"     , A_CHAR( "Unknown conversion \"!{}\"."
     ALIB_REL_DBG( A_CHAR( "{!X}") , A_CHAR( "\nNative argument type: <{}> ") )
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),
    "FPS107"     , A_CHAR( "Precision not allowed with integer format."
     ALIB_REL_DBG( A_CHAR( "{!X}") , A_CHAR( "\nNative argument type: <{}> ") )
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),
    "FPS108"     , A_CHAR( "Missing pair of replacement strings \"<src><repl>\" after \"!Replace\"."
     ALIB_REL_DBG( A_CHAR( "{!X}") , A_CHAR( "\nNative argument type: <{}> ") )
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),


    // Formatter Java Style
    "FJS101"     , A_CHAR( "Brackets for negative values not implemented/supported."
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),
    "FJS102"     , A_CHAR( "Missing precision value after '.' character."
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),
    "FJS103"     , A_CHAR( "Hexadecimal float format not implemented/supported."
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),
    "FJS104"     , A_CHAR( "Alternate form '#' not supported with conversion {!Q}."
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),
    "FJS105"     , A_CHAR( "Precision specification (\".{}\") is not supported with conversion {!Q}."
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),
    "FJS106"     , A_CHAR( "Unknown date/time conversion suffix {!Q'}."
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),
    "FJS107"     , A_CHAR( "Unknown conversion character {!Q'}."
                           "\nIn: {!Q}\n    >{!FillC-}^"                                                  ),


    // PropertyFormatter and PropertyFormatters
    "PFM501"     , A_CHAR( "Unknown property identifier \"{}{}\" in format string.\n"
                           "Format string: {!Q}.\n"
                           "Allowed identifiers: "                                                               ),
    "PFM502"     , A_CHAR( "Error in resulting format string.\nSource format: {!Q}."                             ),
    "PFM510"     , A_CHAR( "Missing or empty configuration variable {!Q} containing user-defined format string." ),

    // class Paragraphs
    "ST601"     ,  A_CHAR( "Unknown marker at position {}.\n"
                           "  Marked text: {!Q}.\n"
                           "               >{!FillC-}^\n"                                                          ),
    "ST602"     ,  A_CHAR( "Found an end-marker without a start marker at position {}.\n"
                           "  Marked text: {!Q}.\n"
                           "               >{!FillC-}^\n"                                                          ),


    "BS",          A_CHAR(  "0,B"                 ",3,"
                            "1,KiB"               ",3,"
                            "2,MiB"               ",3,"
                            "3,GiB"               ",3,"
                            "4,TiB"               ",3,"
                            "5,PiB"               ",3,"
                            "6,EiB"               ",3,"
                            "7,ZiB"               ",3,"
                            "8,YiB"               ",3,"
                            "9,RiB"               ",3,"
                           "10,QiB"               ",3,"
                           "11,B"                 ",1,"
                           "12,kB"                ",2,"
                           "13,MB"                ",2,"
                           "14,GB"                ",2,"
                           "15,TB"                ",2,"
                           "16,PB"                ",2,"
                           "17,EB"                ",2,"
                           "18,ZB"                ",2,"
                           "19,YB"                ",2,"
                           "20,RB"                ",2,"
                           "21,QB"                ",2"   ) EOS

        // end of BootstrapBulk()
        nullptr );
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS


        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lang_format_bytesize_iec )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lang_format_bytesize_si  )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lang_format_bytesize_units )

        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(  lang::system::SystemErrors    )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(  lang::system::SystemExceptions)
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(  lang::system::Path*           )

        boxing::BootstrapRegister< lang::format::FFormat, boxing::TMappedTo<time::DateTime   >>(system::FFormat_DateTime);
        boxing::BootstrapRegister< lang::format::FFormat, boxing::TMappedTo<lang::CallerInfo*>>(format::FFormat_CallerInfo);
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N( time::DateTime::Duration )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W( time::DateTime::Duration )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X( time::DateTime::Duration )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N( time::Ticks   ::Duration )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W( time::Ticks   ::Duration )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X( time::Ticks   ::Duration )

        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(   format::FormatterStdImpl::PHTypes  )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(   format::FMTExceptions    )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(   format::ByteSizeSI    )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(   format::ByteSizeIEC   )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N( format::ByteSizeUnits )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W( format::ByteSizeUnits )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X( format::ByteSizeUnits )
    #if ALIB_THREADS
        alib::boxing::BootstrapRegister<FAppend<character, alib::HeapAllocator>,
                                        alib::boxing::TMappedTo <alib::threads::Thread*> >(alib::threads::FAppend_thread);
       ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::threads::Thread::State )
    #endif
    #if ALIB_BITBUFFER
       ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::bitbuffer::ac_v1::ArrayCompressor::Algorithm  )
    #endif


        // create formatters. This has to be done very early, so that they can be used during
        // initialization.
        Formatter::Default      .InsertDerived<FormatterPythonStyle>();
        Formatter::Default->Next.InsertDerived<FormatterJavaStyle  >();

        // CodeMarker_CommonEnums
        EnumRecords<alib::lang::Alignment        >::Bootstrap( *this, "Alignment"      );
        EnumRecords<alib::lang::Bool             >::Bootstrap( *this, "Bool"           );
        EnumRecords<alib::lang::Caching          >::Bootstrap( *this, "Caching"        );
DOX_MARKER( [DOX_ENUMS_MAKE_PARSABLE_22] )
        EnumRecords<alib::lang::Case>::Bootstrap( *this, "Case"  );
DOX_MARKER( [DOX_ENUMS_MAKE_PARSABLE_22] )
        EnumRecords<alib::lang::ContainerOp      >::Bootstrap( *this, "ContainerOp"    );
        EnumRecords<alib::lang::CreateDefaults   >::Bootstrap( *this, "Bool"           );
        EnumRecords<alib::lang::CreateIfNotExists>::Bootstrap( *this, "Bool"           );
        EnumRecords<alib::lang::CurrentData      >::Bootstrap( *this, "CurrentData"    );
        EnumRecords<alib::lang::Inclusion        >::Bootstrap( *this, "Inclusion"      );
        EnumRecords<alib::lang::Initialization   >::Bootstrap( *this, "Initialization" );
        EnumRecords<alib::lang::Phase            >::Bootstrap( *this, "Phase"          );
        EnumRecords<alib::lang::Propagation      >::Bootstrap( *this, "Propagation"    );
        EnumRecords<alib::lang::Reach            >::Bootstrap( *this, "Reach"          );
        EnumRecords<alib::lang::Recursive        >::Bootstrap( *this, "Bool"           );
        EnumRecords<alib::lang::Responsibility   >::Bootstrap( *this, "Responsibility" );
        EnumRecords<alib::lang::Safeness         >::Bootstrap( *this, "Safeness"       );
        EnumRecords<alib::lang::Side             >::Bootstrap( *this, "Side"           );
        EnumRecords<alib::lang::SortOrder        >::Bootstrap( *this, "SortOrder"      );
        EnumRecords<alib::lang::SourceData       >::Bootstrap( *this, "SourceData"     );
        EnumRecords<alib::lang::Switch           >::Bootstrap( *this, "Switch"         );
        EnumRecords<alib::lang::Timezone         >::Bootstrap( *this, "Timezone"       );
        EnumRecords<alib::lang::Timing           >::Bootstrap( *this, "Timing"         );
        EnumRecords<alib::lang::ValueReference   >::Bootstrap( *this, "ValueReference" );
        EnumRecords<alib::lang::Whitespaces      >::Bootstrap( *this, "Whitespaces"    );

IF_ALIB_BITBUFFER(  EnumRecords<alib::bitbuffer::ac_v1::ArrayCompressor::Algorithm>::Bootstrap( *this, "ACAlgos"  ); )
IF_ALIB_THREADS(    EnumRecords<alib::threads::Thread::State>                      ::Bootstrap( *this, "TSts"     ); )
IF_ALIB_THREADMODEL(EnumRecords<alib::threadmodel::Priority>                       ::Bootstrap( *this, "TMDWP"    ); )
#if ALIB_CONFIGURATION
DOX_MARKER([DOX_CONFIG_DECLARATION7])
EnumRecords<Variables>::Bootstrap('|');
DOX_MARKER([DOX_CONFIG_DECLARATION7])
#endif

        EnumRecords<system::SystemExceptions          >::Bootstrap();
        EnumRecords<SystemErrors                      >::Bootstrap();
        EnumRecords<SystemFolders                     >::Bootstrap( *this, "SystemFolders" );
        EnumRecords<ReportExceptions                  >::Bootstrap();
        EnumRecords<format::FormatterStdImpl::PHTypes >::Bootstrap();
        EnumRecords<format::FMTExceptions             >::Bootstrap();
        EnumRecords<format::ByteSizeUnits             >::Bootstrap();
    }

    else if( phase == BootstrapPhases::PrepareConfig )
    {

        // preload all variable with declarations (and without placeholders)
        #if ALIB_CONFIGURATION
            config->PreloadVariables<basecamp::Variables>();
        #endif
    }

    else if( phase == BootstrapPhases::Final )
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        Substring parser;
        parser= GetResource( "Months" ); for( int i= 0 ; i < 12 ; ++i ) { months[i]= parser.ConsumeToken(','); }
        parser= GetResource( "Days"   ); for( int i= 0 ; i <  7 ; ++i ) {   days[i]= parser.ConsumeToken(','); }

        parser= GetResource( "DurUnts" );
        for( int i= 0 ; i < Units::SIZE_OF_UNITS ; ++i )
            units[i]= parser.ConsumeToken(',');

        ALIB_ASSERT( units[Units::SIZE_OF_UNITS-1].IsNotEmpty() )
        ALIB_ASSERT( parser.IsEmpty() )
        ALIB_WARNINGS_RESTORE

        format::BYTESIZE_NUMBER_FORMAT= monomem::GLOBAL_ALLOCATOR().New<NumberFormat>(NumberFormat::Global );
        format::BYTESIZE_NUMBER_FORMAT->FractionalPartWidth= 1;

        //############### set locale ###############
        int receivedFrom= 0;
        String256 locale;
        #if ALIB_CONFIGURATION
            Variable varLocale(*config, Variables::LOCALE);
            if (     varLocale.IsDefined()
                 &&  varLocale.GetString().IsNotEmpty()        )
            {
                receivedFrom= 1;
                locale._( varLocale.GetString() );
            }
            else
        #endif

             if ( EnvironmentVariables::Get( A_CHAR("LANG")      ,locale ) )  receivedFrom= 2;
        else if ( EnvironmentVariables::Get( A_CHAR("LANGUAGE")  ,locale ) )  receivedFrom= 3;

        if( receivedFrom > 0 && !locale.Equals<NC,lang::Case::Ignore>( A_CHAR("none") ) )
        {
            ALIB_STRINGS_TO_NARROW( locale, nLocale, 1024)
            if( !setlocale(LC_ALL, nLocale ) )
            {
                NString256 msg( "ALib Error: setlocale(\""); msg << nLocale <<"\") failed. Setting read from ";
                msg << (   receivedFrom == 1 ? "config variable 'ALIB_LOCALE'"   :
                           receivedFrom == 2 ? "environment variable 'LANG'"     :
                           receivedFrom == 3 ? "environment variable 'LANGUAGE'" : "ERROR"  );

                std::cerr.write( msg.Buffer(), msg.Length() );

                std::cerr << ". Trying  'setlocale(LC_ALL, \"\")': ";
                if ( setlocale(LC_ALL, ""  ) )
                    std::cerr << " success.";
                else
                {
                    std::cerr << "failed. Trying  'setlocale(LC_ALL, \"C\")': ";
                    if ( setlocale(LC_ALL, "C" ) )
                        std::cerr << " success.";
                    else
                        std::cerr << std::endl << "     Panic: No standard locale setting was successful!";
                }
                std::cerr << std::endl;
            }
            else
                LocaleFound.Allocate( monomem::GLOBAL_ALLOCATOR, locale);
        }

        // set the system's locale as the default for our static default number format
        #if ALIB_STRINGS
            NumberFormat::Global.SetFromLocale();
            NumberFormat::Global.Flags+= NumberFormatFlags::WriteGroupChars;
        #endif


        // --- determine if we have a console window ---
        // read configuration
        #if ALIB_CONFIGURATION
            Variable varHasConsoleWindow(*config, Variables::HAS_CONSOLE_WINDOW );
            if ( varHasConsoleWindow.IsDefined() )
                HasConsoleWindow=  varHasConsoleWindow;
            else
            {
                #if defined(_WIN32)
                    // determine if we have a console window
                    HasConsoleWindow= GetConsoleWindow() != NULL;
                #else
                    HasConsoleWindow= true;
                #endif
                (void) varHasConsoleWindow.Define();
                varHasConsoleWindow=  HasConsoleWindow;
            }
        #else // -> !ALIB_CONFIGURATION
            #if defined(_WIN32)
                // determine if we have a console window
                HasConsoleWindow= GetConsoleWindow() != NULL;
            #else
                HasConsoleWindow= true;
            #endif
        #endif // ALIB_CONFIGURATION

        // --- determine if we want to wait for a keypress upon termination ---
        #if ALIB_CONFIGURATION
            Variable varWaitForKeyPress(*config, Variables::WAIT_FOR_KEY_PRESS );
            if ( varWaitForKeyPress.IsDefined() )
                WaitForKeyPressOnTermination= varWaitForKeyPress;
            else
            {
                #if defined(_WIN32) && ALIB_DEBUG
                    WaitForKeyPressOnTermination=    HasConsoleWindow
                                                  && IsDebuggerPresent();
                #else
                    WaitForKeyPressOnTermination=  false;
                #endif
                (void) varWaitForKeyPress.Define();
                varWaitForKeyPress= WaitForKeyPressOnTermination;
            }
        #else // -> !ALIB_CONFIGURATION
            #if defined(_WIN32) && ALIB_DEBUG
                WaitForKeyPressOnTermination=    HasConsoleWindow
                                              && IsDebuggerPresent();
            #else
                WaitForKeyPressOnTermination=  false;
            #endif
        #endif

        // --- attach Lock to Default-Formatter ---
DOX_MARKER([DOX_CRITICAL_SECTIONS_ADD_LOCK2])
#if ALIB_DEBUG_CRITICAL_SECTIONS && ALIB_MONOMEM
    Formatter::Default->DCSLock= &Formatter::DefaultLock;
#endif
DOX_MARKER([DOX_CRITICAL_SECTIONS_ADD_LOCK2])

    }
}

void BaseCamp::shutdown( ShutdownPhases phase )
{
    if( phase == ShutdownPhases::Announce )
    {
        #if ALIB_DEBUG_CRITICAL_SECTIONS && ALIB_MONOMEM
            Formatter::Default->DCSLock= nullptr;
        #endif
        return;
    }

    // ShutdownPhases::Destruct:
    if ( BASECAMP.WaitForKeyPressOnTermination )
    {
        #if defined(_WIN32)
            OutputDebugStringA( "\r\nALIB: Waiting for 'Enter' key in the console window.\r\n" );
        #endif

        std::cout << std::endl << GetResource("EXIT_MSG") << std::endl;
        while ( getchar() != '\n' )
            ;
    }

    // ::format
    ALIB_ASSERT_WARNING( Formatter::Default.UseCount() == 1,
                         "ALIB", "DefaultFormatter still shared more than once: ",
                         int(Formatter::Default.UseCount()) )

    Formatter::Default= nullptr;

    // alib/lang/message
    #if ALIB_DEBUG
        DBG_SIMPLE_ALIB_MSG_PLUGIN= nullptr;
    #endif

    if ( Report::defaultReport != nullptr )
        delete Report::defaultReport;

    if( ReportWriterStdIO::singleton )
        delete ReportWriterStdIO::singleton;
}

// #############################################################################################
// Environment definition/detection
// #############################################################################################

//! @cond NO_DOX
   #if defined(__unix__) || defined(__APPLE__)
   namespace {
       String      DebuggerProcessNames= A_CHAR("gdb|debugserver");
   }
   #endif
//! @endcond

   bool BaseCamp::IsDebuggerPresent()
   {
   #if defined(__unix__) || defined(__APPLE__)

       // Unix: search debugger process in parent processes
       const ProcessInfo& pi= ProcessInfo::Current();
       uinteger parentPID= pi.PPID;
       if ( parentPID == 0 )
           return false;

       ProcessInfo actPI( parentPID );
       Tokenizer tknzr( DebuggerProcessNames, '|' );
       while(tknzr.HasNext() )
       {
           Substring& debuggerName= tknzr.Next();
           if (actPI.Name.StartsWith( debuggerName ) )
               return true;
       }
       return false;

   #elif defined(_WIN32)

        return (bool) ( ::IsDebuggerPresent() != 0 ) ;

   #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
        return false;
   #endif
   }


// #################################################################################################
// namespace  lang::system
// #################################################################################################
} namespace system {
Exception CreateSystemException( const CallerInfo& ci, int errNo )
{
    auto* enumRecord= enums::TryRecord( SystemErrors(errNo) );
    if( enumRecord == nullptr )
        return Exception( ci, SystemErrors::UNKNOWN, errNo );

    return Exception( ci, SystemErrors(errNo),  // as exception
                          SystemErrors(errNo),  // boxing the exception's name (!)
                          errNo                    );
}

// #################################################################################################
// CalendarDateTime::Format
// #################################################################################################
AString& CalendarDateTime::Format( Substring format, AString& target, lang::CurrentData targetData ) const
{
    if ( targetData == lang::CurrentData::Clear )
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
                    int pairs= n / 2;
                    target.InsertChars<NC>( '\'', pairs );
                    n-= (pairs * 2);
                }

                // one single quote?
                if ( n == 1 )
                {
                    // search end
                    integer end= format.IndexOf( '\'' );
                    if ( end < 1 )
                    {
                        ALIB_WARNING( "ALIB", "Format Error: Missing single Quote" )
                        target <<     "Format Error: Missing closing single quote character <'>" ;
                        return target;
                    }

                    target._<NC>( format, 0, end );
                    format.ConsumeChars<NC>( end + 1 );
                }

            } break;

            case 's': // second
                target.template _<NC>( alib::Format( Second, n, nf ) );
                break;

            case 'm': //minute
                target._<NC>( alib::Format( Minute, n, nf ) );
                break;

            case 'K': // hour 0..11
                target._<NC>( alib::Format( Hour % 12, n, nf ) );
                target._<NC>( Hour < 12 ? " am" : " pm" );
                break;

            case 'H': // hour 0..23
                target._<NC>( alib::Format( Hour,   n, nf ) );
                break;

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            case 'd': // day
                     if ( n <= 2 )     target._<NC>( alib::Format( Day, n, nf) );
                else if ( n == 3 )     target._<NC>( days[DayOfWeek], 0, 3 );
                else                   target._<NC>( days[DayOfWeek]    );
                break;

            case 'M': // month
                     if ( n <= 2 )     target._<NC>( alib::Format( Month, n, nf ) );
                else if ( n == 3 )     target._<NC>( months[Month-1], 0, 3 );
                else                   target._<NC>( months[Month-1]     );
                break;
            ALIB_WARNINGS_RESTORE

            case 'y': // year
                     if ( n == 1 )     target._<NC>( alib::Format(Year,        1, nf) );
                else if ( n == 2 )     target._<NC>( alib::Format(Year %  100, 2, nf) );
                else                   target._<NC>( alib::Format(Year,        n, nf) );
                break;

            default: // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
        }

    }

    return target;
}



// #################################################################################################
// FFormat_DateTime
// #################################################################################################
#if !DOXYGEN
DOX_MARKER([DOX_BOXING_IFORMAT_DATETIME])
void FFormat_DateTime( const Box& box, const String& formatSpec, NumberFormat&, AString& target )
{
    lang::system::CalendarDateTime tct( box.Unbox<DateTime>() );
    tct.Format( formatSpec.IsNotEmpty() ? formatSpec
                                        : BASECAMP.GetResource("FMTDT"),
                target );
}
DOX_MARKER([DOX_BOXING_IFORMAT_DATETIME])
#endif

}}} // namespace [alib::lang::system]


#if !DOXYGEN
namespace alib::strings {

// #################################################################################################
// T_Append<DateTime::Duration>
// #################################################################################################
using namespace lang;

namespace {

    template<typename TChar, typename TAllocator>
    void appendDateTime(TAString<TChar,TAllocator>& target, const DateTime::Duration pSrc)
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
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
        ALIB_WARNINGS_RESTORE
    }
} //anonymous namespace


template<typename TChar, typename TAllocator>
void T_Append<time::DateTime::Duration,TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target, const time::DateTime::Duration src )
{
    appendDateTime( target, src );
}

template<typename TChar, typename TAllocator>
void T_Append<time::Ticks::Duration,TChar,TAllocator>::operator()( TAString<TChar,TAllocator>& target, const time::Ticks::Duration src)
{
    // simply convert the ticks-duration to a DateTime duration and use its append function
    appendDateTime( target, time::DateTime::Duration::FromNanoseconds( src.InNanoseconds() ));
}

// Instantiations
template void T_Append<time::DateTime::Duration, nchar, lang::HeapAllocator>::operator()( TAString<nchar, lang::HeapAllocator>&, const time::DateTime::Duration   );
template void T_Append<time::DateTime::Duration, wchar, lang::HeapAllocator>::operator()( TAString<wchar, lang::HeapAllocator>&, const time::DateTime::Duration   );
template void T_Append<time::DateTime::Duration, xchar, lang::HeapAllocator>::operator()( TAString<xchar, lang::HeapAllocator>&, const time::DateTime::Duration   );
template void T_Append<time::Ticks::   Duration, nchar, lang::HeapAllocator>::operator()( TAString<nchar, lang::HeapAllocator>&, const time::Ticks   ::Duration   );
template void T_Append<time::Ticks::   Duration, wchar, lang::HeapAllocator>::operator()( TAString<wchar, lang::HeapAllocator>&, const time::Ticks   ::Duration   );
template void T_Append<time::Ticks::   Duration, xchar, lang::HeapAllocator>::operator()( TAString<xchar, lang::HeapAllocator>&, const time::Ticks   ::Duration   );

} // namespace [alib::strings]
#endif  //!DOXYGEN


// #################################################################################################
// format::ByteSize
// #################################################################################################
#if !DOXYGEN
namespace alib::lang::format {
namespace {

double   convertTo(uinteger val, ByteSizeUnits unit)
{
    double v= double(val);
    int loopEnd;
    double divisor;
    if( unit < ByteSizeUnits::IEC_END)
    {
        loopEnd= int(unit);
        divisor= double(1024);
    }
    else
    {
        loopEnd= int(unit)   - ByteSizeUnits::SI;
        divisor= double(1000);
    }

    for (int i = 0; i < loopEnd; ++i)
            v/= divisor;
    return v;
}

ALIB_WARNINGS_IGNORE_NOTHING_RETURNED
std::pair<double,ByteSizeUnits>     getMagnitude( uinteger val, uinteger byteLimit, uinteger factor)
{
    ByteSizeUnits unit= ByteSizeUnits(0);
    if( val < byteLimit )
        return std::make_pair( double(val), ByteSizeUnits(0) );

    while(val != 0)
    {
        ++unit;
        if( val < byteLimit * factor)
            return std::make_pair( double(val) / double(factor), unit );

        val/= factor;
    }
}
ALIB_WARNINGS_RESTORE
} // namespace alib::lang::format[::anonymous]

void FormatByteSize(AString& target, uinteger val, uint16_t magnitudeThreshold,
                    char unitSeparator, ByteSizeUnits unit, NumberFormat& nf)
{
    target.EnsureRemainingCapacity(128);

    auto magnitude= (unit ==  ByteSizeUnits::IEC) ? getMagnitude(val, magnitudeThreshold, 1024ull)
                                                  : getMagnitude(val, magnitudeThreshold, 1000ull);

    if( magnitude.second ==  0 )
        target << Format( val, nullptr );
    else
        target.SetLength( strings::detail::WriteFloat( magnitude.first,
                                                       target.VBuffer(), target.Length(), 0, nf)  );

    if( unitSeparator )
        target << unitSeparator;
    target <<  (magnitude.second + unit);
} // FormatByteSize()

std::pair<double, ByteSizeUnits>     ByteSizeIEC::GetMagnitude()
{
    auto result= getMagnitude( Value, MagnitudeThreshold, 1024 );
    result.second+= ByteSizeUnits::IEC;
    return result;
}

std::pair<double,ByteSizeUnits>     ByteSizeSI::GetMagnitude()
{
    auto result= getMagnitude( Value, MagnitudeThreshold, 1000 );
    result.second+= ByteSizeUnits::SI;
    return result;
}

double   ByteSizeIEC::ConvertTo(ByteSizeUnits unit) { return convertTo(Value, unit); };
double   ByteSizeSI ::ConvertTo(ByteSizeUnits unit) { return convertTo(Value, unit); };


} namespace alib::strings {

void T_Append< lang::format::ByteSizeSI, character, lang::HeapAllocator>::operator()( AString& target, const  lang::format::ByteSizeSI src )
{
     lang::format::FormatByteSize( target, src.Value, src.MagnitudeThreshold, src.UnitSeparator,
                           lang::format::ByteSizeUnits::SI, * lang::format::BYTESIZE_NUMBER_FORMAT          );
}
void T_Append< lang::format::ByteSizeIEC, character, lang::HeapAllocator>::operator()( AString& target, const  lang::format::ByteSizeIEC src )
{
     lang::format::FormatByteSize( target, src.Value, src.MagnitudeThreshold, src.UnitSeparator,
                           lang::format::ByteSizeUnits::IEC, * lang::format::BYTESIZE_NUMBER_FORMAT         );
}

} // namespace [alib::strings]

ALIB_BOXING_VTABLE_DEFINE( alib::lang::format::ByteSizeIEC      , vt_lang_format_bytesize_iec )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::format::ByteSizeSI       , vt_lang_format_bytesize_si  )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::format::ByteSizeUnits    , vt_lang_format_bytesize_units )

#endif  //!DOXYGEN

