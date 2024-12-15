//==================================================================================================
/// \file
/// This header file is part of module \alib_basecamp of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_CAMP_MESSAGE_REPORT
#define HPP_ALIB_CAMP_MESSAGE_REPORT 1
#pragma once
#include "alib/lang/message/message.hpp"
#include "alib/lang/basecamp/basecamp.hpp"
#include <stack>

#include "alib/monomem/localallocator.hpp"

#if ALIB_THREADS
#   include "alib/threads/recursivelock.hpp"
#endif

namespace alib {

// forward declarations
IF_ALIB_THREADS( namespace threads { class RecursiveLock; } )

namespace lang {                  class ReportWriter;

//==================================================================================================
/// Exception codes of class \alib{lang;Report}.
//==================================================================================================
enum class ReportExceptions
{
    /// Error when writing a report. This typically indicates an erroneous format string in an
    /// \ref ALIB_ASSERT_ERROR or related macro.
    ErrorWritingReport= 1,
};


//==================================================================================================
/// This class provides a simple facility to collect what is called a \e 'report'.
/// Reports are maintenance messages, mostly error and warning messages, but is not aiming to replace
/// any sort of error handling.
/// (In \alib itself, sending a \e 'report' usually precedes raising an error.)
///
/// While a process can create different objects of this class, usually, the default instance
/// available by inheriting from class \alib{singletons;Singleton}.
/// is sufficient and all \alib internal warnings and errors will be directed to this one.
/// Software built on \alib should do the same with internal warnings and errors. An own
/// instance might be created to collect other types of reports.
///
/// This class uses a member of type  * \alib{lang;ReportWriter} to actually write the reports.
/// By default, an object of type \alib{lang;ReportWriterStdIO} is attached.
///
/// The reporting method, \alib{lang::Report;DoReport} will check the flags provided with
/// \alib{lang::Report;PushHaltFlags} for message types \c 0 (errors) and \c 1 (warnings), and may
/// invoke \e assert(). Such assertions are effective only in the debug-builds of the
/// library/executable. Custom \e 'ReportWriters' might take action (e.g., for security reasons)
/// and e.g., terminate the application also in release compilations.
///
/// To simplify things, a set of macros is defined which are pruned in release
/// versions of the compilation unit. These are:
///
/// - #ALIB_MESSAGE
/// - #ALIB_ERROR
/// - #ALIB_WARNING
/// - #ALIB_ASSERT
/// - #ALIB_ASSERT_ERROR
/// - #ALIB_ASSERT_WARNING
///
/// In fact, these macros exist in  \alibheader{alib.hpp} already, but with the inclusion of
/// header \alibheader{lang/message/report.hpp}, these macros become redefined to use
/// this class.
///
/// For convenience, with debug-builds of the library, these macros provide the file name,
/// line number and method name of the invocation source, which can be used by more sophisticated
/// versions of currently attached \alib{lang;ReportWriter}.
///
/// \note
///  For debug output statements, it is advised to rather use the module \alib_alox
///  instead of \alib reports and corresponding macros.<br>
///  In addition, \alox itself replaces the standard report writer given with this class
///  by an instance of type \alib{lox;ALoxReportWriter}, which uses the \alox standard debug logger.
//==================================================================================================
class Report
{
    #if !DOXYGEN
        friend class basecamp::BaseCamp;
    #endif

    public:
       /// Types of reports
        enum class Types
        {
            Error,   ///< An assertion.
            Warning, ///< A warning.
            Message, ///< A report message.
        };

    // #############################################################################################
    // protected fields
    // #############################################################################################
    protected:
        /// The default Report used internally by \alib and usually by processes that rely on \alib.
        ALIB_API static Report*         defaultReport;

        /// A stack of writers. The topmost one is the actual.
        /// Can be set at run-time using methods #PushWriter and #PopWriter.
        std::stack<ReportWriter*>       writers;


        /// This is a flag that avoids recursion. Recursion might occur when a more sophisticated
        /// report writer sends a report (e.g., an ALIB Error or Warning). Recursive calls are
        /// rejected without further notice.
        bool                            recursionBlocker                                    = false;

        #if ALIB_THREADS
        /// A Lock to protect against multithreaded calls.
        threads::RecursiveLock             lock;
        #endif

        /// A stack of integers. The topmost value is used to decide, whether program execution is
        /// halted on message of type 'error' (type \c 0, bit \c 0) or of type 'warning'
        /// (type \c 1, bit \c 1).
        /// Can be set at run-time using methods #PushHaltFlags and #PopHaltFlags.
        std::stack<int>                 haltAfterReport;

    // #############################################################################################
    // constructor/destructor
    // #############################################################################################
    public:
        /// Constructor.
        ALIB_API     Report();

        /// Destructor.
        ALIB_API    ~Report();

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        //==========================================================================================
        /// Receives the default report object used by \alib and processes that rely on \alib.
        /// @returns The default \b Report.
        //==========================================================================================
        static
        Report&  GetDefault()
        {
            if ( defaultReport == nullptr )
                defaultReport= new Report();
            return *defaultReport;
        }

        //==========================================================================================
        /// Reports the given message to the current \alib{lang;ReportWriter} in place.
        /// The default \b ReportWriter will print the message on the process console.
        /// Furthermore, in debug execution the flags provided with #PushHaltFlags is checked.
        /// If this is set in respect to type of message (field \alib{lang;Message::Type}),
        /// the program halts or suspends into the debugger (platform and language specific).
        ///
        /// @param message The message to report.
        //==========================================================================================
        ALIB_API
        void     DoReport( Message& message );

        //==========================================================================================
        /// Overloaded method that fetches all arguments needed to construct a
        /// \alib{lang;Message} object to pass to #DoReport.
        ///
        /// @param ci      Information about the scope of invocation.
        /// @param type    The report type.
        /// @param msgs    Variadic list of boxes.
        //==========================================================================================
        template <typename... Boxes>
        void DoReport( const CallerInfo& ci,
                       const Enum& type,  Boxes&&... msgs )
        {
            LocalAllocator1K allocator;
            Message message( ci, allocator, type, std::forward<Boxes>(msgs)...);
            DoReport( message );
        }

        //==========================================================================================
        /// Writes new values to the internal flags that decide if calls to #DoReport with
        /// report type \e '0' (errors), respectively report type '>0' (warnings) cause
        /// to halt program execution by calling <em>assert(false)</em>.
        /// The previous values can be restored using #PopHaltFlags.
        /// @param haltOnErrors      Specifies if halting on errors is wanted.
        /// @param haltOnWarnings    Specifies if halting on warnings is wanted.
        //==========================================================================================
        ALIB_API
        void     PushHaltFlags( bool haltOnErrors, bool haltOnWarnings );

        //==========================================================================================
        /// Restores the previous values after an invocation to #PushHaltFlags.
        //==========================================================================================
        ALIB_API
        void     PopHaltFlags();

        //==========================================================================================
        /// Sets a new writer. The actual writer is implemented as a stack. It is important to
        /// keep the right order when pushing and popping writers, as there lifetime is externally
        /// managed. (In standard use-cases, only one, app-specific writer should be pushed anyhow).
        /// To give a little assurance, method #PopWriter takes the same parameter as this method
        /// does, to verify if if the one to be removed is really the topmost.
        /// @param newWriter   The writer to use.
        //==========================================================================================
        ALIB_API
        void     PushWriter( ReportWriter* newWriter );

        //==========================================================================================
        /// Retrieves the actual report writer.
        ///
        /// \note This method should not be used to retrieve the writer and use it. It should be used
        ///       only to test the installation.
        /// @return The actual report writer in place.
        //==========================================================================================
        ALIB_API
        ReportWriter* PeekWriter();

        //==========================================================================================
        /// Restores the previous writer after setting a new one using #PushWriter.
        /// It is important to keep the right order when pushing and popping writers, as there
        /// lifetime is externally managed.
        /// (In standard use-cases, only one, app-specific writer should be pushed anyhow).
        /// To give a little assurance, this method #PopWriter takes the same parameter as
        /// #PushWriter does, to verify if the one to be removed is really the topmost.
        ///
        /// @param checkWriter  The previously pushed writer (for checking of call order).
        //==========================================================================================
        ALIB_API
        void     PopWriter( ReportWriter* checkWriter );
};// class Report


//==================================================================================================
/// Interface that defines a writer for for %ALib \ref alib::lang::Report "Report".
/// By default, an instance of \ref alib::lang::ReportWriterStdIO "ReportWriterStdIO"
/// is installed.
/// Applications may implement their own ReportWriter.
//==================================================================================================
class ReportWriter
{
    public:
        //==========================================================================================
        /// Virtual destructor
        //==========================================================================================
        virtual ~ReportWriter        () {}

        //==========================================================================================
        /// Notify activation/deactivation
        /// @param phase     Information if activated or deactivated.
        //==========================================================================================
        virtual void NotifyActivation( lang::Phase phase )                                       =0;

        //==========================================================================================
        /// Report a message. Pure virtual abstract interface method.
        /// @param msg  The message to report.
        //==========================================================================================
        virtual void Report          ( Message& msg )                                            =0;
};

//==================================================================================================
/// The standard \b %ReportWriter writing the message to \c std::cout and \c std::cerr.
/// The global formatter singleton is used is used to process the objects in the report message.
/// This is by default of type \alib{lang::format;FormatterPythonStyle;FormatterPythonStyle}.
/// See static field * \alib{lang::format;Formatter::Default} for more information.
///
/// ## Friends ##
/// class \alib{singletons;Singleton;Singleton<ReportWriterStdIO>}
/// class \alib{lang;Report}
//==================================================================================================
class ReportWriterStdIO : public ReportWriter, public Singleton<ReportWriterStdIO>
{
    #if !DOXYGEN
        friend class singletons::Singleton<ReportWriterStdIO>;
        friend class Report;
        friend class basecamp::BaseCamp;
    #endif

    private:
        /// Private constructor, while parent class \b %Singleton is friend. Therefore, only one
        /// instance may exist.
        ReportWriterStdIO()                                                               = default;

        /// Private destructor.
        virtual   ~ReportWriterStdIO()                                          override  = default;

    public:
        //==========================================================================================
        /// Notify activation/deactivation
        //==========================================================================================
        virtual void NotifyActivation  ( lang::Phase  )                                  override {}

        //==========================================================================================
        /// Writes the prefix \"ALib Report (Error):\" (respectively \"ALib Report (Warning):\"
        /// and the error message to the cout.
        /// On Windows platform, if a debugger is present, the message is also written using
        /// <em>OutputDebugStringA</em>.
        ///
        /// If the first member of \p msg is of type string and it's contents follow the
        /// rules given with (higher level) module \alib in respect to
        /// \alib{lox;Lox::EntryDetectDomain;valid domain path detection}
        /// then this string is followed by a colon and a space (<c>": "</c>) to separate this
        /// "topic" from the rest of the message.
        ///
        /// @param msg  The message to report.
        //==========================================================================================
        ALIB_API
        virtual void Report  ( Message& msg )                                        override;
};

}} // namespace [alib::lang]

ALIB_ASSERT_GLOBAL_NAMESPACE

ALIB_BOXING_VTABLE_DECLARE( alib::lang::Report::Types, vt_alib_report_types )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::ReportExceptions, alib::lang::ERException  )
ALIB_RESOURCED_IN_MODULE( alib::lang::ReportExceptions, alib::BASECAMP, "REPE" )

// #################################################################################################
// Redefine ALIB Debug macros
// #################################################################################################
#if ALIB_DEBUG && !DOXYGEN

    #undef ALIB_ERROR
    #undef ALIB_WARNING
    #undef ALIB_MESSAGE
    #undef ALIB_ASSERT
    #undef ALIB_ASSERT_ERROR
    #undef ALIB_ASSERT_WARNING
    #undef ALIB_ASSERT_MESSAGE

    #define ALIB_ERROR(   ... )              {                alib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, alib::lang::Report::Types::Error  , __VA_ARGS__       );  }
    #define ALIB_WARNING( ... )              {                alib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, alib::lang::Report::Types::Warning, __VA_ARGS__       );  }
    #define ALIB_MESSAGE( ... )              {                alib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, alib::lang::Report::Types::Message, __VA_ARGS__       );  }

    #define ALIB_ASSERT( cond )              { if (!(cond)) { alib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, alib::lang::Report::Types::Error  , "Internal Error." ); } }
    #define ALIB_ASSERT_ERROR( cond, ... )   { if (!(cond)) { alib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, alib::lang::Report::Types::Error  , __VA_ARGS__       ); } }
    #define ALIB_ASSERT_WARNING( cond, ... ) { if (!(cond)) { alib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, alib::lang::Report::Types::Warning, __VA_ARGS__       ); } }
    #define ALIB_ASSERT_MESSAGE( cond, ... ) { if (!(cond)) { alib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, alib::lang::Report::Types::Message, __VA_ARGS__       ); } }

#endif


#endif // HPP_ALIB_CAMP_MESSAGE_REPORT

