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
#include "alib/singletons/singletons.prepro.hpp"
#include "alib/monomem/monomem.prepro.hpp"
#include "alib/strings/strings.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/containers/containers.prepro.hpp"
#include "alib/resources/resources.prepro.hpp"
#include "alib/bitbuffer/bitbuffer.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"
#include "alib/bootstrap/bootstrap.prepro.hpp"
#if ALIB_DEBUG && !DOXYGEN
#   if defined(__GNUC__) || defined(__clang__)
#      include <cxxabi.h>
#      include <cassert>
#   endif
#   include <cstdlib>
#   if defined(_WIN32)
#      include <cstring>
#   endif
#endif

#if !DOXYGEN
#   include "alib/platformincludes.hpp"
#   if (ALIB_DEBUG)
#       include <cassert>
#       include <cstring>
#       if defined( _WIN32 ) && !defined(_STRING_)
#          include <string>
#       endif
#   endif

#endif // !DOXYGEN

#include <iostream>
#include <iomanip>

#if (ALIB_SINGLE_THREADED && ALIB_EXT_LIB_THREADS_AVAILABLE)
#  include <thread>
#endif

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
      module ALib.Bootstrap;
      import   ALib.Lang;
      import   ALib.Time;
      import   ALib.Threads;
#   if ALIB_MONOMEM
      import   ALib.Monomem;
#   endif
#   if ALIB_STRINGS
      import   ALib.Strings;
#   endif
#   if ALIB_SINGLETONS
      import   ALib.Singletons;
#   endif
#   if ALIB_BOXING
      import   ALib.Boxing;
#   endif
#   if ALIB_ENUMRECORDS
      import   ALib.EnumRecords.Bootstrap;
#   endif
#   if ALIB_THREADMODEL
      import   ALib.ThreadModel;
#   endif
#   if ALIB_EXCEPTIONS
      import   ALib.Exceptions;
#   endif
#   if ALIB_SYSTEM
      import   ALib.System;
#   endif
#   if ALIB_FORMAT
      import   ALib.Format;
      import   ALib.Format.StdImplementation;
      import   ALib.Format.FormatterPythonStyle;
      import   ALib.Format.FormatterJavaStyle;
#   endif
#   if ALIB_CAMP
      import   ALib.Camp;
      import   ALib.Camp.Base;
      import   ALib.Resources;
      import   ALib.Variables;
#   endif
#   if ALIB_BITBUFFER
      import   ALib.BitBuffer;
#   endif
#   if ALIB_ALOX
      import ALib.ALox;
      import ALib.ALox.Impl;
#   endif
#   if ALIB_EXPRESSIONS
      import ALib.Expressions;
#   endif
#   if ALIB_CLI
      import ALib.CLI;
#   endif
#   if ALIB_FILES
      import ALib.Files;
#   endif

#else
#   include "ALib.Lang.H"
#   include "ALib.Time.H"
#   include "ALib.Threads.H"
#   include "ALib.Monomem.H"
#   include "ALib.Strings.H"
#   include "ALib.Singletons.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.ThreadModel.H"
#   include "ALib.Exceptions.H"
#   include "ALib.System.H"
#   include "ALib.Format.H"
#   include "ALib.Format.StdImplementation.H"
#   include "ALib.Format.FormatterPythonStyle.H"
#   include "ALib.Format.FormatterJavaStyle.H"
#   include "ALib.Variables.H"
#   include "ALib.BitBuffer.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.Bootstrap.H"
#   include "ALib.CLI.H"
#   include "ALib.Expressions.H"
#   include "ALib.ALox.Impl.H"
#   include "ALib.Files.H"
#endif

// ======================================   Implementation   =======================================
#   include "ALib.Lang.CIFunctions.H"
namespace alib {

//==================================================================================================
//=== Version and compilation flag verification
//==================================================================================================

int                 VERSION              = ALIB_VERSION;
unsigned char       REVISION             = ALIB_REVISION;
TCompilationFlags   COMPILATION_FLAGS    = {ALIB_COMPILATION_FLAGS};

CompilationFlagMeaningsEntry COMPILATION_FLAG_MEANINGS[40]=  {
    {  0, "ALOX"                             }, // modules
    {  1, "BITBUFFER"                        },
    {  2, "BOXING"                           },
    {  3, "CAMP"                             },
    {  4, "CONTAINERS"                       },
    {  5, "CLI"                              },
    {  6, "ENUMRECORDS"                      },
    {  7, "EXCEPTIONS"                       },

    {  8, "EXPRESSIONS"                      },
    {  9, "FILES"                            },
    { 10, "FORMAT"                           },
    { 11, "MONOMEM"                          },
    { 12, "RESOURCES"                        },
    { 13, "SINGLETONS"                       },
    { 14, "STRINGS"                          },
    { 15, "SYSTEM"                           },

    { 16, "THREADMODEL"                      },
    { 17, "VARIABLES"                        },
    { 18, "DEBUG"                            }, // debug features
    { 19, "ALIB_DEBUG_ARRAY_COMPRESSION"     },
    { 20, "ALIB_DEBUG_ALLOCATIONS"           },
    { 21, "ALIB_DEBUG_BOXING"                },
    { 22, "ALIB_DEBUG_CONTAINERS"            },
    { 23, "ALIB_DEBUG_CRITICAL_SECTIONS"     },

    { 24, "ALIB_DEBUG_MEMORY"                },
    { 25, "ALIB_DEBUG_STRINGS"               },
    { 26, "ALIB_DEBUG_RESOURCES"             },
    { 27, "ALIB_C20_MODULES"                 }, // other features
    { 28, "ALIB_SINGLE_THREADED"             },
    { 29, "FEAT_SINGLETON_MAPPED"            },
    { 30, "CHARACTERS_WIDE"                  },
    { 31, "CHARACTERS_WCHAR_IS_4"            },

    { 32, "FEAT_BOXING_BIJECTIVE_INTEGRALS"  },
    { 33, "FEAT_BOXING_BIJECTIVE_CHARACTERS" },
    { 34, "FEAT_BOXING_BIJECTIVE_FLOATS"     },
    { 35, "ALIB_FEAT_BOOST_REGEX"            },
    { 36, "DBG_LOG"                          },
    { 37, "DBG_LOG_CI"                       },
    { 38, "REL_LOG"                          },
    { 39, "REL_LOG_CI"                       },
};


void AssertALibVersionAndFlags( int alibVersion, int alibRevision, TCompilationFlags compilationFlags )
{
    // verify requested ALib version is met
    if (VERSION != alibVersion || REVISION != alibRevision )
    {
        std::cerr << "!!! Error in ALox library compilation: linked against wrong version of ALib" << std::endl;
        std::cerr << "!!! ALib library version:   " <<     VERSION   << "R" << int(REVISION) << std::endl;
        std::cerr << "!!! ALib requested version: " << alibVersion   << "R" << alibRevision << std::endl;
        std::cerr << "!!! Exiting with exit(-1)" << std::endl;
        exit(1);
    }

    // verify the flags
    if (    compilationFlags.bits[0] == COMPILATION_FLAGS.bits[0]
         && compilationFlags.bits[1] == COMPILATION_FLAGS.bits[1]
         && compilationFlags.bits[2] == COMPILATION_FLAGS.bits[2]
         && compilationFlags.bits[3] == COMPILATION_FLAGS.bits[3] )
        return;

    std::cerr << "!!! Error in ALib library compilation: linked library of ALib has "
                 "different compiler-symbols set than the using executable (or library)." << std::endl;

    // dump out the flags
    std::cerr << std::left << std::setw(35) <<  "Symbol" << '|' << std::setw(5) << " Lib" <<'|' << " Comp. Unit"  << std::endl;
    std::cerr<< std::setw(62) <<  std::setfill('-') << ' ' << std::endl <<  std::setfill(' ');

    for( auto& p : COMPILATION_FLAG_MEANINGS )
    {
        bool libFlag= (COMPILATION_FLAGS.bits[p.Flag/8] & (1 << (p.Flag % 8))) != 0;
        bool reqFlag= (compilationFlags.bits[p.Flag/8] & (1 << (p.Flag % 8))) != 0;

        std::cerr << std::setw(35) <<  p.Name << '|' << std::setw(5) << (libFlag  ? " On" : " Off")
                                              << "|" << std::setw(5) << (reqFlag  ? " On" : " Off")
                  << ( libFlag != reqFlag ? " <-- Mismatch! " : "")
                  << std::endl;
    }

    // we just quit!
    std::cerr << "!!! Exiting with exit(-1)" << std::endl;
    exit(1);
}

bool                NonCampModulesInitialized     = false;


//==================================================================================================
//================================     Non-Camp Version      =======================================
//==================================================================================================
#if !ALIB_CAMP
void     Bootstrap( int alibVersion, int alibRevision, TCompilationFlags compilationFlags )
{
    // verify ALib
    AssertALibVersionAndFlags( alibVersion, alibRevision, compilationFlags );

    #if ALIB_MONOMEM
        // if the global allocator was not initialized from outside, then we have to do it.
        if (!monomem::GLOBAL_ALLOCATOR.IsInitialized())
            new (&monomem::GLOBAL_ALLOCATOR) MonoAllocator(ALIB_DBG("Global",) 128);
    #endif

    if( !NonCampModulesInitialized ) {
        IF_ALIB_BOXING ( boxing ::bootstrap(); )
        IF_ALIB_THREADS( threads::bootstrap(); )

        #if ALIB_FORMAT
            Formatter::Default      .InsertDerived<FormatterPythonStyle>();
            Formatter::Default->Next.InsertDerived<FormatterJavaStyle  >();
        #   if !ALIB_SINGLE_THREADED
                Formatter::DefaultLock.Dbg.Name= "DefaultFormatter";
        #       if ALIB_DEBUG_CRITICAL_SECTIONS && ALIB_MONOMEM
                    Formatter::Default->DCSLock= &Formatter::DefaultLock;
                    Formatter::DefaultLock.Dbg.Name= "DefaultFormatter";
        #       endif
        #   endif
        #endif

        //------------------------------------------------------------------------------------------
        //--- Non-resourced versions of enum records
        //------------------------------------------------------------------------------------------
#if ALIB_ENUMRECORDS

DOX_MARKER([DOX_ENUMS_MULTIPLE_RECORDS])
enumrecords::bootstrap::Bootstrap<lang::Bool>(
{
    { lang::Bool::True , A_CHAR("False"), 1 },
    { lang::Bool::False, A_CHAR("True" ), 1 },
    { lang::Bool::True , A_CHAR("0"    ), 1 },
    { lang::Bool::False, A_CHAR("1"    ), 1 },
    { lang::Bool::True , A_CHAR("No"   ), 1 },
    { lang::Bool::False, A_CHAR("Yes"  ), 1 },
    { lang::Bool::True , A_CHAR("Off"  ), 2 },
    { lang::Bool::False, A_CHAR("On"   ), 2 },
    { lang::Bool::True , A_CHAR("-"    ), 1 },
    { lang::Bool::False, A_CHAR("Ok"   ), 2 }
} );
DOX_MARKER([DOX_ENUMS_MULTIPLE_RECORDS])

enumrecords::bootstrap::Bootstrap<lang::Case>(
{
    { lang::Case::Sensitive, A_CHAR("Sensitive"), 1 },
    { lang::Case::Ignore   , A_CHAR("Ignore"   ), 1 },
} );

DOX_MARKER([DOX_ENUMS_MULTIPLE_RECORDS_2])
enumrecords::bootstrap::Bootstrap<lang::ContainerOp>(
{
    { lang::ContainerOp::Insert   , A_CHAR("Insert"   ), 1 }, // integral 0
    { lang::ContainerOp::Remove   , A_CHAR("Remove"   ), 1 }, // integral 1
    { lang::ContainerOp::GetCreate, A_CHAR("GetCreate"), 4 }, // integral 3 <-- Switched order
    { lang::ContainerOp::Get      , A_CHAR("Get"      ), 1 }, // integral 2 <--
    { lang::ContainerOp::Create   , A_CHAR("Create"   ), 1 }, // integral 4
} );
DOX_MARKER([DOX_ENUMS_MULTIPLE_RECORDS_2])

enumrecords::bootstrap::Bootstrap<lang::Switch>(
{
    { lang::Switch::Off                    , A_CHAR("Off")           , 2 },
    { lang::Switch::On                     , A_CHAR("On" )           , 2 },
} );

enumrecords::bootstrap::Bootstrap<lang::Alignment>(
{
    { lang::Alignment::Left                , A_CHAR("Left"  )        , 1 },
    { lang::Alignment::Right               , A_CHAR("Right" )        , 1 },
    { lang::Alignment::Center              , A_CHAR("Center")        , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::SortOrder>(
{
    { lang::SortOrder::Ascending           , A_CHAR("Ascending" )    , 1 },
    { lang::SortOrder::Descending          , A_CHAR("Descending")    , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::Inclusion>(
{
    { lang::Inclusion::Include             , A_CHAR("Include")       , 1 },
    { lang::Inclusion::Exclude             , A_CHAR("Exclude")       , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::Reach>(
{
    { lang::Reach::Global                  , A_CHAR("Global")        , 1 },
    { lang::Reach::Local                   , A_CHAR("Local" )        , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::CurrentData>(
{
    { lang::CurrentData::Keep              , A_CHAR("Keep" )         , 1 },
    { lang::CurrentData::Clear             , A_CHAR("Clear")         , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::SourceData>(
{
    { lang::SourceData::Copy               , A_CHAR("Copy")          , 1 },
    { lang::SourceData::Move               , A_CHAR("Move")          , 1 },
} );


enumrecords::bootstrap::Bootstrap<lang::Safeness>(
{
    { lang::Safeness::Safe                 , A_CHAR("Safe"  )        , 1 },
    { lang::Safeness::Unsafe               , A_CHAR("Unsafe")        , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::Responsibility>(
{
    { lang::Responsibility::KeepWithSender , A_CHAR("KeepWithSender"), 1 },
    { lang::Responsibility::Transfer       , A_CHAR("Transfer"      ), 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::Side>(
{
    { lang::Side::Left                     , A_CHAR("Left" )         , 1 },
    { lang::Side::Right                    , A_CHAR("Right")         , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::Timezone>(
{
    { lang::Timezone::Local                , A_CHAR("v"  )           , 1 },
    { lang::Timezone::UTC                  , A_CHAR("UTC")           , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::Whitespaces>(
{
    { lang::Whitespaces::Trim              , A_CHAR("Trim")          , 1 },
    { lang::Whitespaces::Keep              , A_CHAR("Keep")          , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::Propagation>(
{
    { lang::Propagation::Omit              , A_CHAR("Omit"         ) , 1 },
    { lang::Propagation::ToDescendants     , A_CHAR("ToDescendants") , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::Phase>(
{
    { lang::Phase::Begin                   , A_CHAR("Begin")         , 1 },
    { lang::Phase::End                     , A_CHAR("End"  )         , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::Initialization>(
{
    { lang::Initialization::Suppress         , A_CHAR("Suppress"  )    , 1 },
    { lang::Initialization::Default          , A_CHAR("Default"   )    , 1 },
    { lang::Initialization::Nulled           , A_CHAR("Nulled"    )    , 1 },
    { lang::Initialization::Suppress         , A_CHAR("None"      )    , 2 },
    { lang::Initialization::Default          , A_CHAR("Initialize")    , 1 },
    { lang::Initialization::Nulled           , A_CHAR("Zero"      )    , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::Timing>(
{
    { lang::Timing::Async                  , A_CHAR("Async"  )       , 1 },
    { lang::Timing::Sync                   , A_CHAR("Sync"   )       , 1 },
    { lang::Timing::Async                  , A_CHAR("Asynchronous")  , 1 },
    { lang::Timing::Sync                   , A_CHAR("Synchronous" )  , 1 },
    { lang::Timing::Sync                   , A_CHAR("Synchronized")  , 1 },
} );

enumrecords::bootstrap::Bootstrap<lang::Caching>(
{
    { lang::Caching::Disabled              , A_CHAR("Disabled")      , 1 },
    { lang::Caching::Enabled               , A_CHAR("Enabled" )      , 1 },
    { lang::Caching::Auto                  , A_CHAR("Auto"    )      , 1 },
} );

// Threads
#if !ALIB_SINGLE_THREADED
enumrecords::bootstrap::Bootstrap<alib::threads::Thread::State>(
{
    { alib::threads::Thread::State::Unstarted , A_CHAR("Unstarted" )  },
    { alib::threads::Thread::State::Started   , A_CHAR("Started"   )  },
    { alib::threads::Thread::State::Running   , A_CHAR("Running"   )  },
    { alib::threads::Thread::State::Done      , A_CHAR("Done"      )  },
    { alib::threads::Thread::State::Terminated, A_CHAR("Terminated")  },
} );

#endif

#if ALIB_SYSTEM
enumrecords::bootstrap::Bootstrap<system::SystemFolders>({
    { system::SystemFolders::Root          , A_CHAR("Root"      )      , 1 },
    { system::SystemFolders::Current       , A_CHAR("Current"   )      , 1 },
    { system::SystemFolders::HomeConfig    , A_CHAR("HomeConfig")      , 5 },
    { system::SystemFolders::Home          , A_CHAR("Home"      )      , 1 },
    { system::SystemFolders::Module        , A_CHAR("Module"    )      , 1 },
    { system::SystemFolders::Temp          , A_CHAR("Temp"      )      , 1 },
    { system::SystemFolders::VarTemp       , A_CHAR("VarTemp"   )      , 1 },
} );
#endif

#if ALIB_FORMAT
enumrecords::bootstrap::Bootstrap<format::FormatterStdImpl::PHTypes>({
    { format::FormatterStdImpl::PHTypes::NotGiven   , A_CHAR("NotGiven"  )      , 1 },
    { format::FormatterStdImpl::PHTypes::String     , A_CHAR("String"    )      , 1 },
    { format::FormatterStdImpl::PHTypes::Character  , A_CHAR("Character" )      , 1 },
    { format::FormatterStdImpl::PHTypes::IntBase10  , A_CHAR("IntBase10" )      , 6 },
    { format::FormatterStdImpl::PHTypes::IntBinary  , A_CHAR("IntBinary" )      , 6 },
    { format::FormatterStdImpl::PHTypes::IntOctal   , A_CHAR("IntOctal"  )      , 6 },
    { format::FormatterStdImpl::PHTypes::IntHex     , A_CHAR("IntHex"    )      , 6 },
    { format::FormatterStdImpl::PHTypes::Float      , A_CHAR("Float"     )      , 1 },
    { format::FormatterStdImpl::PHTypes::Bool       , A_CHAR("Bool"      )      , 1 },
    { format::FormatterStdImpl::PHTypes::HashCode   , A_CHAR("HashCode"  )      , 1 },
    { format::FormatterStdImpl::PHTypes::Fill       , A_CHAR("Fill"      )      , 4 },
} );

enumrecords::bootstrap::Bootstrap<format::ByteSizeUnits>({
    { format::ByteSizeUnits::B     , A_CHAR("B"   )      , 3 },
    { format::ByteSizeUnits::KiB   , A_CHAR("KiB" )      , 3 },
    { format::ByteSizeUnits::MiB   , A_CHAR("MiB" )      , 3 },
    { format::ByteSizeUnits::GiB   , A_CHAR("GiB" )      , 3 },
    { format::ByteSizeUnits::TiB   , A_CHAR("TiB" )      , 3 },
    { format::ByteSizeUnits::PiB   , A_CHAR("PiB" )      , 3 },
    { format::ByteSizeUnits::EiB   , A_CHAR("EiB" )      , 3 },
    { format::ByteSizeUnits::ZiB   , A_CHAR("ZiB" )      , 3 },
    { format::ByteSizeUnits::YiB   , A_CHAR("YiB" )      , 3 },
    { format::ByteSizeUnits::RiB   , A_CHAR("RiB" )      , 3 },
    { format::ByteSizeUnits::QiB   , A_CHAR("QiB" )      , 3 },
    { format::ByteSizeUnits::B     , A_CHAR("B"   )      , 1 },
    { format::ByteSizeUnits::kB    , A_CHAR("kB"  )      , 2 },
    { format::ByteSizeUnits::MB    , A_CHAR("MB"  )      , 2 },
    { format::ByteSizeUnits::GB    , A_CHAR("GB"  )      , 2 },
    { format::ByteSizeUnits::TB    , A_CHAR("TB"  )      , 2 },
    { format::ByteSizeUnits::PB    , A_CHAR("PB"  )      , 2 },
    { format::ByteSizeUnits::EB    , A_CHAR("EB"  )      , 2 },
    { format::ByteSizeUnits::ZB    , A_CHAR("ZB"  )      , 2 },
    { format::ByteSizeUnits::YB    , A_CHAR("YB"  )      , 2 },
    { format::ByteSizeUnits::RB    , A_CHAR("RB"  )      , 2 },
    { format::ByteSizeUnits::QB    , A_CHAR("QB"  )      , 2 },
} );
        
enumrecords::bootstrap::Bootstrap<format::FMTExceptions>(
{
    A_CHAR(
    // general formatter errors
          "11,ArgumentIndexIs0,"                       "Argument index 0 not allowed.\n"
                                                       "In: {!Q}\n    >{!FillC-}^"
      "," "12,ArgumentIndexOutOfBounds,"               "Argument index #{} is out of bounds. {} arguments are available.\n"
                                                                "In: {!Q}\n    >{!FillC-}^"
      "," "13,IncompatibleTypeCode,"                   "Detected placeholder type does not match given argument.\n"
                                                       "Placeholder number:    {}\n"
                                                       "Placeholder type:      {} ({})"
                                                                "Placeholder type:      {} ({})\n"
                                                                "Deduced argument type: {!Q}\n"
                                         ALIB_REL_DBG( "{!X}" ,  "\nNative argument type: <{}> ")
                                                                "In: {!Q}\n    >{!FillC-}^"

      "," "101,MissingClosingBracket,"                 "Closing bracket '}}' of placeholder not found (or syntax error).\n"
                                                       "In: {!Q}\n    >{!FillC-}^"
      "," "102,MissingPrecisionValuePS,"               "Missing precision value after '.' character."
                                                     "\nIn: {!Q}\n    >{!FillC-}^"
      "," "103,DuplicateTypeCode,"                     "Duplicate type code {!Q'} given (previous was {!Q'})."
                                         ALIB_REL_DBG( "{!X}" ,  "\nNative argument type: <{}> ")
                                                     "\nIn: {!Q}\n    >{!FillC-}^"
      "," "104,UnknownTypeCode,"                       "Unknown type code {!Q'} given."
                                                     "\nIn: {!Q}\n    >{!FillC-}^"
                                         ALIB_REL_DBG( "{!X}" ,"\nNative argument type: <{}>")
      "," "105,ExclamationMarkExpected,"               "Expected '!' in continuation of placeholder."
                                         ALIB_REL_DBG( "{!X}" ,  "\nNative argument type: <{}>")
                                                    "\nIn: {!Q}\n    >{!FillC-}^"
      "," "106,UnknownConversionPS,"                   "Unknown conversion \"!{}\"."
                                         ALIB_REL_DBG( "{!X}" ,  "\nNative argument type: <{}>")
                                                    "\nIn: {!Q}\n    >{!FillC-}^"
      "," "107,PrecisionSpecificationWithInteger,"     "Precision not allowed with integer format."
                                         ALIB_REL_DBG( "{!X}" ,  "\nNative argument type: <{}>")
                                                    "\nIn: {!Q}\n    >{!FillC-}^"
      "," "108,MissingReplacementStrings,"             "Missing pair of replacement strings \"<src><repl>\" after \"!Replace\"."
                                         ALIB_REL_DBG( "{!X}" ,  "\nNative argument type: <{}>")
                                                    "\nIn: {!Q}\n    >{!FillC-}^"

      "," "201,NegativeValuesInBracketsNotSupported,"  "Brackets for negative values not implemented/supported."
                                                       "\nIn: {!Q}\n    >{!FillC-}^"
      "," "202,MissingPrecisionValueJS,"               "Missing precision value after '.' character."
                                                       "\nIn: {!Q}\n    >{!FillC-}^"
      "," "203,HexadecimalFloatFormatNotSupported,"    "Hexadecimal float format not implemented/supported."
                                                       "\nIn: {!Q}\n    >{!FillC-}^"
      "," "204,NoAlternateFormOfConversion,"           "Alternate form '#' not supported with conversion {!Q}."
                                                       "\nIn: {!Q}\n    >{!FillC-}^"
      "," "205,NoPrecisionWithConversion,"             "Precision specification (\".{}\") is not supported with conversion {!Q}."
                                                       "\nIn: {!Q}\n    >{!FillC-}^"
      "," "206,UnknownDateTimeConversionSuffix,"       "Unknown date/time conversion suffix {!Q'}."
                                                       "\nIn: {!Q}\n    >{!FillC-}^"
      "," "207,UnknownConversionJS,"                   "Unknown conversion character {!Q'}."
                                                       "\nIn: {!Q}\n    >{!FillC-}^"

      "," "501,UnknownPropertyInFormatString,"         "Unknown property identifier \"{}{}\" in format string.\n"
                                                       "Format string: {!Q}.\n"
                                                       "Allowed identifiers:"
      "," "502,ErrorInResultingFormatString,"          "Error in resulting format string.\nSource format: {!Q}."
      "," "510,MissingConfigurationVariable,"          "Missing or empty configuration variable {!Q} containing user-defined format string."

      "," "601,UnknownMarker,"                         "Unknown marker at position {}.\n"
                                                       "  Marked text: {!Q}.\n"
                                                       "               >{!FillC-}^"
      "," "602,EndmarkerWithoutStart,"                 "Found an end-marker without a start marker at position {}.\n"
                                                       "  Marked text: {!Q}.\n"
                                                       "               >{!FillC-}^"
     )
});
#endif

#if ALIB_VARIABLES
enumrecords::bootstrap::Bootstrap<variables::Priority>({
    { variables::Priority::NONE          , A_CHAR("NONE"         ) , 1 },
    { variables::Priority::AutoDetected  , A_CHAR("AutoDetected" ) , 1 },
    { variables::Priority::DefaultValues , A_CHAR("DefaultValues") , 1 },
    { variables::Priority::Standard      , A_CHAR("Standard"     ) , 1 },
    { variables::Priority::ConfigFile    , A_CHAR("ConfigFile"   ) , 2 },
    { variables::Priority::Environment   , A_CHAR("Environment"  ) , 1 },
    { variables::Priority::SessionFile   , A_CHAR("SessionFile"  ) , 8 },
    { variables::Priority::CLI           , A_CHAR("CLI"          ) , 2 },
    { variables::Priority::Session       , A_CHAR("Session"      ) , 2 },
    { variables::Priority::Protected     , A_CHAR("Protected"    ) , 1 },
});
#endif

#if ALIB_BITBUFFER
enumrecords::bootstrap::Bootstrap<bitbuffer::ac_v1::ArrayCompressor::Algorithm>({
    { bitbuffer::ac_v1::ArrayCompressor::Algorithm::NONE        , A_CHAR("NONE"        ) , 1 },
    { bitbuffer::ac_v1::ArrayCompressor::Algorithm::ALL         , A_CHAR("ALL"         ) , 1 },
    { bitbuffer::ac_v1::ArrayCompressor::Algorithm::Uncompressed, A_CHAR("Uncompressed") , 1 },
    { bitbuffer::ac_v1::ArrayCompressor::Algorithm::MinMax      , A_CHAR("MinMax"      ) , 1 },
    { bitbuffer::ac_v1::ArrayCompressor::Algorithm::Sparse      , A_CHAR("Sparse"      ) , 1 },
    { bitbuffer::ac_v1::ArrayCompressor::Algorithm::VerySparse  , A_CHAR("VerySparse"  ) , 1 },
    { bitbuffer::ac_v1::ArrayCompressor::Algorithm::Incremental , A_CHAR("Incremental" ) , 1 },
    { bitbuffer::ac_v1::ArrayCompressor::Algorithm::Huffman     , A_CHAR("Huffman"     ) , 1 },
});
#endif

#if ALIB_THREADMODEL
enumrecords::bootstrap::Bootstrap<threadmodel::Priority>(
{
    { threadmodel::Priority::Lowest          , A_CHAR("Lowest"             ), 4 },
    { threadmodel::Priority::DeferredDeletion, A_CHAR("DeferredDeletion"   ), 1 },
    { threadmodel::Priority::Low             , A_CHAR("Low"                ), 1 },
    { threadmodel::Priority::Standard        , A_CHAR("Standard"           ), 1 },
    { threadmodel::Priority::Highest         , A_CHAR("Highest"            ), 5 },
    { threadmodel::Priority::High            , A_CHAR("High"               ), 1 },
} );
#endif


//------------------------------------------------------------------------------------------
//--- END: Non-resourced versions of enum records
//------------------------------------------------------------------------------------------
#endif // ALIB_ENUMRECORDS

        NonCampModulesInitialized= true;
    }

DOX_MARKER([DOX_CRITICAL_SECTIONS_ADD_LOCK])
#if ALIB_DEBUG_CRITICAL_SECTIONS && ALIB_MONOMEM
    monomem::GLOBAL_ALLOCATOR.DbgCriticalSectionsPH.Get()->DCSLock=
                    &monomem::GLOBAL_ALLOCATOR_LOCK;

    monomem::GLOBAL_ALLOCATOR_LOCK.Dbg.Name= "GlobalAllocator";
#endif
DOX_MARKER([DOX_CRITICAL_SECTIONS_ADD_LOCK])
}


void Shutdown()
{
    #if ALIB_DEBUG_CRITICAL_SECTIONS && ALIB_MONOMEM
        monomem::GLOBAL_ALLOCATOR.DbgCriticalSectionsPH.Get()->DCSLock= nullptr;
        #if ALIB_FORMAT
            Formatter::Default->DCSLock= nullptr;
        #endif
    #endif

    IF_ALIB_THREADS    ( alib::threads            ::shutdown(); )
    IF_ALIB_ENUMRECORDS( alib::enumrecords::detail::shutdown(); )
    IF_ALIB_BOXING     ( alib::boxing             ::shutdown(); )
    IF_ALIB_SINGLETONS ( alib::singletons         ::shutdown(); )

    #if ALIB_FORMAT
        ALIB_ASSERT_WARNING( Formatter::Default.UseCount() == 1, "ALIB",
            "DefaultFormatter still shared more than once: ", Formatter::Default.UseCount() )
        Formatter::Default= nullptr;
    #endif

    NonCampModulesInitialized= false;
}

#else
//==================================================================================================
//==================================     Camp Version      =========================================
//==================================================================================================

#if !DOXYGEN
List<MonoAllocator, camp::Camp*>   CAMPS(monomem::GLOBAL_ALLOCATOR);
#   include "ALib.Lang.CIFunctions.H"
#endif

void BootstrapAddDefaultCamps()
{
    // if the global allocator was not initialized from outside, then we have to do it.
    if (!monomem::GLOBAL_ALLOCATOR.IsInitialized())
        new (&monomem::GLOBAL_ALLOCATOR) MonoAllocator(ALIB_DBG("Global",) 128);

    ALIB_ASSERT_ERROR( CAMPS.empty(), "CAMPS", "List CAMPS already set." )

                           CAMPS.push_back( &BASECAMP    );
    IF_ALIB_ALOX(          CAMPS.push_back( &ALOX        );  )
    IF_ALIB_CLI(           CAMPS.push_back( &CLI         );  )
    IF_ALIB_EXPRESSIONS(   CAMPS.push_back( &EXPRESSIONS );  )
    IF_ALIB_FILES(         CAMPS.push_back( &FILES       );  )
}

void Bootstrap( BootstrapPhases     targetPhase,
                camp::Camp*         targetCamp,
                int alibVersion, int alibRevision, TCompilationFlags compilationFlags )
{
    // verify ALib
    AssertALibVersionAndFlags( alibVersion, alibRevision, compilationFlags );

    // if not customized, create default module list
    if( CAMPS.empty() )
        BootstrapAddDefaultCamps();

    if( targetCamp == nullptr )
        targetCamp= CAMPS.back();

    //std::cout << "Camp::Bootstrap called on: '" << this->ResourceCategory << "', target phase: " << int(targetPhase) << std::endl;

    // Initialize non-camp modules once
    if( !NonCampModulesInitialized )
    {
        IF_ALIB_BOXING ( boxing             ::bootstrap(); )
        IF_ALIB_THREADS( threads            ::bootstrap(); )

        #if ALIB_FORMAT
                         Formatter::Default      .InsertDerived<FormatterPythonStyle>();
                         Formatter::Default->Next.InsertDerived<FormatterJavaStyle  >();
DOX_MARKER([DOX_CRITICAL_SECTIONS_ADD_LOCK2])
#if ALIB_DEBUG_CRITICAL_SECTIONS && ALIB_MONOMEM
    Formatter::Default->DCSLock= &Formatter::DefaultLock;
#endif
DOX_MARKER([DOX_CRITICAL_SECTIONS_ADD_LOCK2])
        #endif
        NonCampModulesInitialized= true;
    }

    // find target camp in the list of camps
    auto targetCampIt= CAMPS.rbegin();
    while(targetCampIt != CAMPS.rend() &&
          *targetCampIt != targetCamp )
        ++targetCampIt;

    ALIB_ASSERT_ERROR(targetCampIt != CAMPS.rend(), "CAMPS",
      "Target camp given to function alib::Bootstrap() is not included in list alib::CAMPS.\n"
      "Resource category of the target camp: ", targetCamp->ResourceCategory )


    // loop over all phases
    for ( int phaseIntegral= int(targetCamp->GetBootstrapState()) + 1 ;
              phaseIntegral <= int( targetPhase ) ;
            ++phaseIntegral )
    {
        BootstrapPhases actualPhase = BootstrapPhases( phaseIntegral );

        // phase 1: distribute a resource pool
        if ( actualPhase == BootstrapPhases::PrepareResources  )
        {
            bool skipOne= false;
            
            // create a resource pool?
            if ( targetCamp->GetResourcePool() == nullptr )
            {
                camp::Camp::SPResourcePool spPool;
                spPool.InsertDerived<resources::LocalResourcePool>(monomem::GLOBAL_ALLOCATOR);
                resources::LocalResourcePool* lPool= dynamic_cast<resources::LocalResourcePool*>(spPool.Get());

                // \releasetask{Update resource numbers numbers. Create resource reference dox for that}
                integer                  expectedSize=   102     // [ALib]
                IF_ALIB_ALOX(                          + 48   )
                IF_ALIB_CLI            (               + 17   )
                IF_ALIB_EXPRESSIONS    (               + 256  )
                IF_ALIB_FILES          (               + 49    ) ;

                auto& hashMap= lPool->BootstrapGetInternalHashMap();
                hashMap.BaseLoadFactor( 2.0 );
                hashMap.MaxLoadFactor ( 5.0 );
                hashMap.Reserve       ( expectedSize, lang::ValueReference::Absolute );

                targetCamp->BootstrapSetResourcePool( spPool );
                skipOne= true;
            }

            // loop in reverse order over modules, start with this module
            auto actPool  =  targetCamp->GetResourcePool();
            for(auto campIt=  targetCampIt ; campIt != CAMPS.rend() ; ++campIt ) {
                if ( skipOne ) { skipOne= false; continue; }

                // if a different resources object is set, then use that one from now on
                if(    (*campIt)->GetResourcePool() != nullptr
                    && (*campIt)->GetResourcePool() != actPool  )
                {
                    actPool= (*campIt)->GetResourcePool();
                    continue;
                }

                (*campIt)->BootstrapSetResourcePool( actPool );

            } // resources distribution loop
        }

        // phase 2: create and distribute a configuration
        else if ( actualPhase == BootstrapPhases::PrepareConfig)
        {
            // create a configuration?
            bool skipOne= false;
            if ( targetCamp->GetConfig() == nullptr )
            {
                targetCamp->BootstrapSetConfig(SharedConfiguration(16u));
                #if ALIB_DEBUG_CRITICAL_SECTIONS
                targetCamp->GetConfig()->NodeTable().dcs.DCSName= "Camp.Config-bootstrap-created";
                #endif
                #if !ALIB_SINGLE_THREADED && ALIB_DEBUG
                targetCamp->GetConfig().GetLock().Dbg.Name= "Camp.Config.Lock-bootstrap-created";
                #endif
                skipOne= true;
            }

            // loop in reverse order over modules, start with this module
            SharedConfiguration* actConfig= &targetCamp->GetConfig();
            for(auto module=   targetCampIt ; module != CAMPS.rend() ; ++module )
            {
                if ( skipOne ) { skipOne= false; continue; }

                // if a different config object is set, then use that one from now on
                if( (*module)->GetConfig() != nullptr && (*module)->GetConfig() != *actConfig)
                    actConfig    = &(*module)->GetConfig();
                else
                    (*module)->BootstrapSetConfig( *actConfig );

            } // resources distribution loop
        }

        // initialize modules on this phase
        ALIB_DBG( bool foundThisModuleInList = false; )
        for ( auto* camp : CAMPS )
        {
            // bootstrap camp
            if(int(camp->GetBootstrapState()) >= int(actualPhase ) )
                 continue;

            //std::cout << "Camp::Bootstrap  '" << module->ResourceCategory << "', phase: " << int(actualPhase) << std::endl;

            ALIB_ASSERT_ERROR( int(camp->GetBootstrapState()) == phaseIntegral - 1,
              "With this invocation of Bootstrap() a camp skips a bootstrap phase.\n"
              "Resource category of the target camp: ", camp->ResourceCategory         )
            camp->BootstrapSetPhase(actualPhase);
            camp->Bootstrap();

            // stop if this is us
            if (camp == targetCamp )
            {
                ALIB_DBG( foundThisModuleInList = true );
                break;
            }
        }
        ALIB_ASSERT_ERROR( foundThisModuleInList, "CAMPS",
          "The target camp of function Bootstrap is not included in list alib::CAMPS "
          "or was already bootstrapped for this phase!\n"
          "Resource category of the target camp: ", targetCamp->ResourceCategory )
    }

    // Are all camps finalized?
    if ( targetPhase == BootstrapPhases::Final && targetCamp == CAMPS.back() )
    {
        #if ALIB_DEBUG_CRITICAL_SECTIONS &&  ALIB_MONOMEM
            monomem::GLOBAL_ALLOCATOR.DbgCriticalSectionsPH.Get()->DCSLock= &monomem::GLOBAL_ALLOCATOR_LOCK;
            monomem::GLOBAL_ALLOCATOR_LOCK.Dbg.Name= "GlobalAllocator";
        #endif
    }
}

void Shutdown( ShutdownPhases targetPhase, camp::Camp*    targetCamp )
{
    #if ALIB_DEBUG_CRITICAL_SECTIONS && ALIB_MONOMEM
        monomem::GLOBAL_ALLOCATOR.DbgCriticalSectionsPH.Get()->DCSLock= nullptr;
        Formatter::Default->DCSLock= nullptr;
    #endif

    ALIB_ASSERT_ERROR( CAMPS.IsNotEmpty(), "CAMPS",
                      "Empty camp list on shutdown. Shutdown invoked twice?"  )
    if( targetCamp == nullptr )
        targetCamp= CAMPS.front();

    //std::cout << "Camp::Shutdown called on'" << targetCamp->ResourceCategory << "', target phase: " << int(targetPhase) << std::endl;

    // find target camp in the list of camps
    auto targetCampIt= CAMPS.begin();
    while(    targetCampIt != CAMPS.end()
          && *targetCampIt != targetCamp )
        ++targetCampIt;
    ALIB_ASSERT_ERROR(targetCampIt != CAMPS.end(), "CAMPS",
      "Target camp given to function alib::Shutdown() is not included in list alib::CAMPS.\n"
      "Resource category of the target camp: ", targetCamp->ResourceCategory )


    ALIB_DBG( bool foundThisModuleInList= false );
    // loop over all (both) termination levels
    for( int  phaseIntegral=  int( ShutdownPhases::Announce )
          ;   phaseIntegral<= int( targetPhase )
          ; ++phaseIntegral                                                   )
    {
        ShutdownPhases actualPhase = ShutdownPhases( phaseIntegral );

        // shutdown in reverse order
        for(auto campIt= CAMPS.rbegin() ; campIt != CAMPS.rend() ; ++campIt )
        {
            ALIB_ASSERT_ERROR(    int(( *campIt )->GetBootstrapState()) < 0
                               || int(( *campIt )->GetBootstrapState()) == int(BootstrapPhases::Final),
                "CAMPS", "Trying to terminate a not (fully) initialized module. "
                         "Module Name (resource category): ", targetCamp->ResourceCategory )

            // shutdown module
            if (int(( *campIt )->GetBootstrapState()) > -int(actualPhase) )
            {
                //std::cout << "Camp::Shutdown '" << (*campIt)->ResourceCategory << "', phase: " << int(actualPhase) << std::endl;

                ALIB_ASSERT_ERROR(    ( int(( *campIt )->GetBootstrapState()) == 3  &&  phaseIntegral == 1 )
                                   || ( int(( *campIt )->GetBootstrapState()) == -1 &&  phaseIntegral == 2 ),
                  "CAMPS", "With this invocation of Bootstrap(), a camp skips a bootstrap phase \n"
                           "Resource category of the target camp: ", ( *campIt )->ResourceCategory )


                ( *campIt )->Shutdown(actualPhase );
                ( *campIt )->BootstrapSetPhase(BootstrapPhases(-int(actualPhase )) );

                ALIB_DBG( if(( *campIt ) == targetCamp )
                             foundThisModuleInList= true;        )
             }

            if(( *campIt ) == targetCamp )
                break;
        }
    }
    ALIB_ASSERT_ERROR( foundThisModuleInList, "CAMPS",
      "The target camp of function Shutdown is not included in list alib::CAMPS "
      "or was already shutdown for this phase!\n"
      "Resource category of the target camp: ", targetCamp->ResourceCategory         )


    #if ALIB_DEBUG_CRITICAL_SECTIONS
        // deactivate assertions for non-locked access to configuration
        if( targetPhase == ShutdownPhases::Announce)
            for(auto campIt= CAMPS.rbegin() ; campIt != CAMPS.rend() ; ++campIt )
               (*campIt)->GetConfig()->NodeTable().dcs.DCSLock= nullptr;
    #endif

    // delete resources/config
    if( targetPhase == ShutdownPhases::Destruct)
        for(auto campIt= CAMPS.rbegin() ; campIt != CAMPS.rend() ; ++campIt )
        {
            ( *campIt )->BootstrapSetResourcePool(nullptr);
            ( *campIt )->BootstrapSetConfig(nullptr);

            if(( *campIt ) == targetCamp )
                break;
        }

    if(     targetPhase == ShutdownPhases::Destruct
        &&  targetCamp  == CAMPS.front()              )
    {
        IF_ALIB_THREADS(    threads            ::shutdown(); )
        IF_ALIB_ENUMRECORDS(enumrecords::detail::shutdown(); )
        IF_ALIB_BOXING(     boxing             ::shutdown(); )
        IF_ALIB_SINGLETONS( singletons         ::shutdown(); )

        ALIB_ASSERT_WARNING( Formatter::Default.UseCount() == 1, "ALIB",
            "DefaultFormatter still shared more than once: ", Formatter::Default.UseCount() )
        Formatter::Default= nullptr;

        CAMPS.Reset();
        NonCampModulesInitialized= false;
    }
}

#endif // ALIB_CAMP
} // namespace [alib]



#   include "ALib.Lang.CIMethods.H"





