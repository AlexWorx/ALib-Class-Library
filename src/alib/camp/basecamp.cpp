// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include <iostream>
#include "alib/strings/strings.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/resources/resources.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
#include "ALib.Compatibility.StdStrings.H"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Camp.Base;
    import        ALib.Lang;
    import        ALib.Time;
    import        ALib.EnumOps;
#   if !ALIB_SINGLE_THREADED
       import     ALib.Threads;
#   endif
    import        ALib.Monomem;
    import        ALib.Strings.Tokenizer;
    import        ALib.Strings;
    import        ALib.Strings.StdIOStream;
    import        ALib.Strings.Calendar;
    import        ALib.Strings.Tokenizer;
    import        ALib.Boxing;
    import        ALib.EnumRecords.Bootstrap;
    import        ALib.Exceptions;
    import        ALib.System;
    import        ALib.Variables;
    import        ALib.Variables.IniFile;
    #if ALIB_THREADMODEL
        import    ALib.ThreadModel;
    #endif
    #if ALIB_BITBUFFER
        import    ALib.BitBuffer;
    #endif
    import        ALib.Format;
    import        ALib.Format.StdImplementation;
    import        ALib.Format.FormatterPythonStyle;
    import        ALib.Format.FormatterJavaStyle;
    import        ALib.Camp;
#else
#   include "ALib.Lang.H"
#   include "ALib.Time.H"
#   if !ALIB_SINGLE_THREADED
#      include   "ALib.Threads.H"
#   endif
#   include "ALib.Monomem.H"
#   include "ALib.Strings.H"
#   include "ALib.Strings.StdIOStream.H"
#   include "ALib.Strings.Calendar.H"
#   include "ALib.Strings.Tokenizer.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Exceptions.H"
#   include "ALib.System.H"
#   include "ALib.Variables.IniFile.H"
#   include "ALib.ThreadModel.H"
#   include "ALib.BitBuffer.H"
#   include "ALib.Format.H"
#   include "ALib.Format.StdImplementation.H"
#   include "ALib.Format.FormatterPythonStyle.H"
#   include "ALib.Format.FormatterJavaStyle.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#endif
// ======================================   Implementation   =======================================
namespace enbs= alib::enumrecords::bootstrap;
namespace alib { camp::Basecamp BASECAMP; }

using namespace alib::strings;

namespace alib::camp {

Basecamp::Basecamp()
: Camp( "ALIB" )
, LocaleFound(nullptr)
{
    #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
        ALIB_ASSERT_ERROR( this == &BASECAMP, "BASECAMP",
            "Instances of class Basecamp must not be created. Use singleton alib::BASECAMP" )
    #endif
}

void Basecamp::Bootstrap()
{
    if( GetBootstrapState() == BootstrapPhases::PrepareResources )
    {
//! @cond NO_DOX
        #define EOS ,
//! @endcond
#if !ALIB_CAMP_OMIT_DEFAULT_RESOURCES
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


       #if !ALIB_SINGLE_THREADED
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



       //------------ module variables ------------
       "VARE<", A_CHAR("variables::"),
       "VARE" , A_CHAR("0"  ","    "OK"                ","  ""     ","
                       "1"  ","    "ErrorOpeningFile"  ","  "VARERF"  ","
                       "2"  ","    "ErrorWritingFile"  ","  "VAREWF"       ),

       "VARERF", A_CHAR("Can't open {}file {!Q'} for reading.") ,
       "VAREWF", A_CHAR("Can't open {}file {!Q'} for writing.") ,


       "VarPrio",     A_CHAR(     "0"   ","   "NONE"              ",1,"
                               "1000"   ","   "AutoDetected"      ",1,"
                               "2000"   ","   "DefaultValues"     ",1,"
                               "4000"   ","   "Standard"          ",1,"
                               "6000"   ","   "ConfigFile"        ",2,"
                               "8000"   ","   "Environment"       ",1,"
                              "10000"   ","   "SessionFile"       ",8,"
                              "12000"   ","   "CLI"               ",2,"
                              "14000"   ","   "Session"           ",2,"
                                "max"   ","   "Protected"         ",1"      ),

       // names of the configuration plug-ins added to configuration in default construction.
       "CFGPlgEnv"  , A_CHAR("Environment Variables")  ,
       "CFGPlgCLI"  , A_CHAR("Command Line Arguments") ,

       "CFGIniWB"      , A_CHAR("writeback")              ,

       // boolean tokens
       "CFGBTF", A_CHAR("False I 1," )  A_CHAR( "True I 1," )
                 A_CHAR(    "0 I 1," )  A_CHAR(    "1 I 1," )
                 A_CHAR(   "No I 1," )  A_CHAR(  "Yes I 1," )
                 A_CHAR(  "Off I 3," )  A_CHAR(   "On I 2," )
                 A_CHAR(  "--- I 1," )  A_CHAR(   "OK I 2"  ),


      // variables
DOX_MARKER([DOX_VARIABLES_DECLARATION6])
"Var0",    A_CHAR("1|ALIB/LOCALE|"             "S"  ),
"Var1",    A_CHAR("2|ALIB/WAIT_FOR_KEY_PRESS|" "B"  ),
"Var2",    A_CHAR("3|ALIB/HAS_CONSOLE_WINDOW|" "B"  ),

"Var_C1",  A_CHAR("Defines the locale of the application. If empty or not set, the system's locale is used.\n"
                 "If set, a UTF8 local should be used, for example 'us_US.UTF-8' or 'de_DE.UTF-8'" ),
"Var_C2",  A_CHAR("If true, the process waits for a key stroke on termination. If empty, under Windows"   "\n"
                 "behavior is detected, under other OSes, defaults to false."                               ),
"Var_C3",  A_CHAR("Boolean value that denotes what its name indicates. If empty, under Windows value is " "\n"
                 "detected, under other OSes, defaults to true."                                            ),
DOX_MARKER([DOX_VARIABLES_DECLARATION6])


       // INI-File comment for section ALIB
       "INI_CMT_ALIB", A_CHAR("@>'/// '@HL-General ALib library settings.\n@HL-"),

       // Exit message when waiting for key-press requested with variable WAIT_FOR_KEY_PRESS
       "EXIT_MSG"    , A_CHAR( "ALIB: Press 'Enter' to exit..." ),

    #if ALIB_BITBUFFER && ALIB_ENUMRECORDS
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

        // Enums
        "SF", A_CHAR("0,Root"        ",1,"
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

// ----------------------------------- module exceptions ------------------------------------
    "ExcFmtExc",       A_CHAR("\nAn exception occurred while formatting another (!) exception:\n" ),

// ----------------------------------- module format ------------------------------------
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


    "FE<", A_CHAR("format::"),
// general formatter errors

    "FE0" , A_CHAR( "11,ArgumentIndexIs0"                       ",FMG11"   ),
    "FE1" , A_CHAR( "12,ArgumentIndexOutOfBounds"               ",FMG12"   ),
    "FE2" , A_CHAR( "13,IncompatibleTypeCode"                   ",FMG13"   ),

    // Formatter Python Style
    "FE3" , A_CHAR( "101,MissingClosingBracket"                 ",FPS101"  ),
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
#endif // !ALIB_CAMP_OMIT_DEFAULT_RESOURCES

        // CodeMarker_CommonEnums
        enbs::Bootstrap<alib::lang::Alignment        >( *this, "Alignment"      );
        enbs::Bootstrap<alib::lang::Bool             >( *this, "Bool"           );
        enbs::Bootstrap<alib::lang::Caching          >( *this, "Caching"        );
DOX_MARKER( [DOX_ENUMS_MAKE_PARSABLE_22] )
        enbs::Bootstrap<alib::lang::Case>( *this, "Case"  );
DOX_MARKER( [DOX_ENUMS_MAKE_PARSABLE_22] )
        enbs::Bootstrap<alib::lang::ContainerOp      >( *this, "ContainerOp"    );
        enbs::Bootstrap<alib::lang::CreateDefaults   >( *this, "Bool"           );
        enbs::Bootstrap<alib::lang::CreateIfNotExists>( *this, "Bool"           );
        enbs::Bootstrap<alib::lang::CurrentData      >( *this, "CurrentData"    );
        enbs::Bootstrap<alib::lang::Inclusion        >( *this, "Inclusion"      );
        enbs::Bootstrap<alib::lang::Initialization   >( *this, "Initialization" );
        enbs::Bootstrap<alib::lang::Phase            >( *this, "Phase"          );
        enbs::Bootstrap<alib::lang::Propagation      >( *this, "Propagation"    );
        enbs::Bootstrap<alib::lang::Reach            >( *this, "Reach"          );
        enbs::Bootstrap<alib::lang::Recursive        >( *this, "Bool"           );
        enbs::Bootstrap<alib::lang::Responsibility   >( *this, "Responsibility" );
        enbs::Bootstrap<alib::lang::Safeness         >( *this, "Safeness"       );
        enbs::Bootstrap<alib::lang::Side             >( *this, "Side"           );
        enbs::Bootstrap<alib::lang::SortOrder        >( *this, "SortOrder"      );
        enbs::Bootstrap<alib::lang::SourceData       >( *this, "SourceData"     );
        enbs::Bootstrap<alib::lang::Switch           >( *this, "Switch"         );
        enbs::Bootstrap<alib::lang::Timezone         >( *this, "Timezone"       );
        enbs::Bootstrap<alib::lang::Timing           >( *this, "Timing"         );
        enbs::Bootstrap<alib::lang::ValueReference   >( *this, "ValueReference" );
        enbs::Bootstrap<alib::lang::Whitespaces      >( *this, "Whitespaces"    );

IF_ALIB_BITBUFFER(  enbs::Bootstrap<alib::bitbuffer::ac_v1::ArrayCompressor::Algorithm>( *this, "ACAlgos"  ); )
IF_ALIB_THREADS(    enbs::Bootstrap<alib::threads::Thread::State>( *this, "TSts"     ); )
IF_ALIB_THREADMODEL(enbs::Bootstrap<alib::threadmodel::Priority> ( *this, "TMDWP"    ); )


        enumrecords::bootstrap::Bootstrap<variables::Priority  >();
        enumrecords::bootstrap::Bootstrap<variables::Exceptions>();


DOX_MARKER([DOX_VARIABLES_DECLARATION7])
enbs::Bootstrap<camp::Variables>('|');
DOX_MARKER([DOX_VARIABLES_DECLARATION7])

        enbs::Bootstrap<SystemErrors                      >();
        enbs::Bootstrap<SystemFolders                     >( *this, "SF"  );
        enbs::Bootstrap<format::FormatterStdImpl::PHTypes >( *this, "PHT" );
        enbs::Bootstrap<format::ByteSizeUnits             >();
        enbs::Bootstrap<alib::format::FMTExceptions       >();
    }

    else if( GetBootstrapState() == BootstrapPhases::PrepareConfig )
    {

        // preload all variable with declarations (and without placeholders)
        config->PreloadVariables<camp::Variables>();
    }

    else if( GetBootstrapState() == BootstrapPhases::Final )
    {
        Substring parser;
        parser= GetResource( "Months" ); for( int i= 0 ; i < 12 ; ++i ) { CalendarDateTime::MONTH_NAMES[i]= parser.ConsumeToken(','); }
        parser= GetResource( "Days"   ); for( int i= 0 ; i <  7 ; ++i ) { CalendarDateTime::  DAY_NAMES[i]= parser.ConsumeToken(','); }

        parser= GetResource( "DurUnts" );
        for( int i= 0 ; i < int(DayTimeUnits::SIZE_OF_UNITS) ; ++i )
            DT_UNITS[i]= parser.ConsumeToken(',');

        ALIB_ASSERT( DT_UNITS[size_t(int(DayTimeUnits::SIZE_OF_UNITS)-1)].IsNotEmpty(), "BASECAMP" )
        ALIB_ASSERT( parser.IsEmpty(), "BASECAMP" )

        format::BYTESIZE_NUMBER_FORMAT= monomem::GLOBAL_ALLOCATOR().New<NumberFormat>(NumberFormat::Global );
        format::BYTESIZE_NUMBER_FORMAT->FractionalPartWidth= 1;

        //############### set locale ###############
        int receivedFrom= 0;
        String256 locale;

        Variable varLocale(*config, Variables::LOCALE);
        if (     varLocale.IsDefined()
             &&  varLocale.GetString().IsNotEmpty()        )
        {
            receivedFrom= 1;
            locale._( varLocale.GetString() );
        }
        else if ( EnvironmentVariables::Get( A_CHAR("LANG")      ,locale ) )  receivedFrom= 2;
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

        // --- determine if we want to wait for a keypress upon termination ---
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
    }
}

void Basecamp::Shutdown( ShutdownPhases phase )
{
    if( phase == ShutdownPhases::Announce )
        return;

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

    // alib/lang/message
    #if ALIB_DEBUG
        assert::PLUGIN= nullptr;
    #endif
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

   bool Basecamp::IsDebuggerPresent()
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


} // namespace [alib::camp]




