/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_results of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_RESULTS_REPORT
#define HPP_ALIB_RESULTS_REPORT 1

#if !defined (HPP_ALIB_RESULTS_MESSAGE)
#   include "alib/results/message.hpp"
#endif

#if !defined(_GLIBCXX_STACK) && !defined(_STACK_)
    #include <stack>
#endif


namespace aworx { namespace lib {

// forward declarations
ALIB_IF_THREADS( namespace threads { class ThreadLock; } )

namespace results {                  class ReportWriter;



/** ************************************************************************************************
 * This class provides a simple facility to collect what is called a \e 'report'.
 * Reports are maintenance messages, mostly error and warning messages, but is not aiming to replace
 * any sort of error handling.
 * (In \alib itself, sending a \e 'report' usually precedes raising an error.)
 *
 * While a process can create different objects of this class, usually, the default instance
 * available by inheriting from class \alib{singletons,Singleton}.
 * is sufficient and all \alib internal warnings and errors will be directed to this one.
 * Software built on \alib should do the same with internal warnings and errors. An own
 * instance might be created to collect other types of reports.
 *
 * This class uses a member of type
 * \alib{results,ReportWriter} to actually write the reports. By default, an
 * object of type
 * \ref aworx::lib::results::ReportWriterStdIO "ReportWriterStdIO" is attached.
 *
 * The reporting method,
 * \ref aworx::lib::results::Report::DoReport "DoReport" will check the flags provided with
 * \ref aworx::lib::results::Report::PushHaltFlags "PushHaltFlags" for message types \c 0 (errors)
 * and \c 1 (warnings), and may invoke \e assert(). Such assertions are effective
 * only in the debug builds of the library/executable. Custom \e 'ReportWriters' might
 * take action (e.g. for security reasons) and e.g. terminate the application also in
 * release compilations.
 *
 * To simplify things, a set of macros is defined which are pruned in release
 * versions of the compilation unit. These are:
 *
 * - #ALIB_MESSAGE
 * - #ALIB_ERROR
 * - #ALIB_WARNING
 * - #ALIB_ASSERT
 * - #ALIB_ASSERT_ERROR
 * - #ALIB_ASSERT_WARNING
 *
 * In fact, these macros exist in <b>%ALib Modules</b> as well, but with the inclusion of
 * header \alibheader{results/report.hpp}, these macros become redefined to use
 * this class.
 *
 * For convenience, with debug builds of the library, these macros provide the file name,
 * line number and method name of the invocation source, which can be used by more sophisticated
 * versions of currently attached \alib{results,ReportWriter}.
 *
 * \note
 *  For debug output statements, it is advised to rather use the
 *  \https{ALox Logging Library,alexworx.github.io/ALox-Logging-Library/index.html}
 *  instead of \alib reports and corresponding macros.
 **************************************************************************************************/
class Report
{
    #if !defined(ALIB_DOX)
        friend class Results;
    #endif

    public:
       /**
        * Types of reports
        */
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
        /** The default Report used internally by \alib and usually by processes that rely on \alib. */
        ALIB_API static Report*         defaultReport;

        /**
         * A stack of writers. The topmost one is the actual.
         * Can be set at run-time using methods #PushWriter and #PopWriter.
         */
        std::stack<ReportWriter*>       writers;


        /** This is a flag that avoids recursion. Recursion might occur when a more sophisticated
         * report writer sends a report (e.g. an ALIB Error or Warning). Recursive calls are
         * rejected without further notice.
         */
        bool                            recursionBlocker                                    = false;

        #if ALIB_THREADS
        /** A Lock to protect against multithreaded calls. */
        threads::ThreadLock             lock;
        #endif

        /**
         * A stack of integers. The topmost value is used to decide, whether program execution is
         * halted on message of type 'error' (type \c 0, bit \c 0) or of type 'warning'
         * (type \c 1, bit \c 1).
         * Can be set at run-time using methods #PushHaltFlags and #PopHaltFlags.
         */
        std::stack<int>                 haltAfterReport;

    // #############################################################################################
    // constructor/destructor
    // #############################################################################################
    public:
        ALIB_API     Report();
        ALIB_API    ~Report();

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Receives the default report object used by \alib and processes that rely on \alib.
         * @returns The default \b Report.
         ******************************************************************************************/
        static
        Report&  GetDefault()
        {
            if ( defaultReport == nullptr )
                defaultReport= new Report();
            return *defaultReport;
        }

        /** ****************************************************************************************
         * Reports the given message to the current
         * \alib{results,ReportWriter} in place. The default \b ReportWriter
         * will print the message on the process console. Furthermore, in debug
         * execution the flags provided with #PushHaltFlags is checked.
         * If this is set in respect to type of message (field \alib{results,Message::Type}),
         * the program halts or suspends into the debugger (platform and language specific).
         *
         * @param message The message to report.
         ******************************************************************************************/
        ALIB_API
        void     DoReport( const Message& message );

        /** ****************************************************************************************
         * Overloaded method that fetches all arguments needed to construct a
         * \alib{results,Message} object to pass to #DoReport.
         *
         * @param file    Information about the scope of invocation.
         * @param line    Information about the scope of invocation.
         * @param func    Information about the scope of invocation.
         * @param type    The report type.
         * @param msgs    Variadic list of boxes.
         ******************************************************************************************/
        template <typename... Boxes>
        void DoReport( const NCString& file, int line, const NCString& func,
                       const Enum& type,  Boxes&&... msgs )
        {
            Message message( file, line, func, type, std::forward<Boxes>(msgs)...);
            DoReport( message );
        }

        /** ****************************************************************************************
         * Writes new values to the internal flags that decide if calls to #DoReport with
         * report type \e '0' (errors), respectively report type '>0' (warnings) cause
         * to halt program execution by calling <em>assert(false)</em>.
         * The previous values can be restored using #PopHaltFlags.
         * @param haltOnErrors      Specifies if halting on errors is wanted.
         * @param haltOnWarnings    Specifies if halting on warnings is wanted.
         ******************************************************************************************/
        ALIB_API
        void     PushHaltFlags( bool haltOnErrors, bool haltOnWarnings );

        /** ****************************************************************************************
         * Restores the previous values after an invocation to #PushHaltFlags.
         ******************************************************************************************/
        ALIB_API
        void     PopHaltFlags();

        /** ****************************************************************************************
         * Sets a new writer. The actual writer is implemented as a stack. It is important to
         * keep the right order when pushing and popping writers, as there lifetime is externally
         * managed. (In standard use-cases, only one, app-specific writer should be pushed anyhow).
         * To give a little assurance, method #PopWriter takes the same parameter as this method
         * does, to verify if if the one to be removed is really the topmost.
         * @param newWriter   The writer to use.
         ******************************************************************************************/
        ALIB_API
        void     PushWriter( ReportWriter* newWriter );

        /** ****************************************************************************************
         * Retrieves the actual report writer.
         *
         * \note This method should not be used to retrieve the writer and use it. It should be used
         *       only to test the installation.
         * @return The actual report writer in place.
         ******************************************************************************************/
        ALIB_API
        ReportWriter* PeekWriter();

        /** ****************************************************************************************
         * Restores the previous writer after setting a new one using #PushWriter.
         * It is important to keep the right order when pushing and popping writers, as there
         * lifetime is externally managed.
         * (In standard use-cases, only one, app-specific writer should be pushed anyhow).
         * To give a little assurance, this method #PopWriter takes the same parameter as
         * #PushWriter does, to verify if the one to be removed is really the topmost.
         *
         * @param checkWriter  The previously pushed writer (for checking of call order).
         ******************************************************************************************/
        ALIB_API
        void     PopWriter( ReportWriter* checkWriter );
};// class Report


/** ************************************************************************************************
 * Interface that defines a writer for for %ALib \ref aworx::lib::results::Report "Report".
 * By default, an instance of \ref aworx::lib::results::ReportWriterStdIO "ReportWriterStdIO"
 * is installed.
 * Applications may implement their own ReportWriter.
 **************************************************************************************************/
class ReportWriter
{
    public:
        /** ****************************************************************************************
         * Virtual destructor
         ******************************************************************************************/
        virtual ~ReportWriter        () {}

        /** ****************************************************************************************
         * Notify activation/deactivation
         * @param phase     Information if activated or deactivated.
         ******************************************************************************************/
        virtual void NotifyActivation( Phase phase )                                             =0;

        /** ****************************************************************************************
         * Report a message. Pure virtual abstract interface method.
         * @param msg  The message to report.
         ******************************************************************************************/
        virtual void Report          ( const Message& msg )                                      =0;
};

/** ************************************************************************************************
 * The standard \b %ReportWriter writing the message to \c std::cout and \c std::cerr.
 * The global formatter singleton is used is used to process the objects in the report message.
 * This is by default of type \alib{text,FormatterPythonStyle,FormatterPythonStyle}.
 * See method * \alib{text,Formatter::GetDefault} for more information.
 *
 * ## Friends ##
 * class \alib{singletons,Singleton,Singleton<ReportWriterStdIO>}
 * class \alib{results,Report}
 **************************************************************************************************/
class ReportWriterStdIO : public ReportWriter, public Singleton<ReportWriterStdIO>
{
    #if !defined(ALIB_DOX)
        friend class singletons::Singleton<ReportWriterStdIO>;
        friend class Report;
        friend class Results;
    #endif

    /**
     * Private constructor, while parent class \b %Singleton is friend. Therefore, only one
     * instance may exist.
     */
    private:
        ALIB_API            ReportWriterStdIO() {}
        ALIB_API virtual   ~ReportWriterStdIO() {}

    public:
        /** ****************************************************************************************
         * Notify activation/deactivation
         * @param phase     Information if activated or deactivated.
         ******************************************************************************************/
        ALIB_API
        virtual void NotifyActivation  ( Phase phase );

        /** ****************************************************************************************
         * Just writes the prefix \"ALib Report (Error):\" (respectively \"ALib Report (Warning):\"
         * and the error message to the cout.
         * On Windows platform, if a debugger is present, the message is also written using
         * <em>OutputDebugStringA</em>.
         *
         * @param msg  The message to report.
         ******************************************************************************************/
        ALIB_API
        virtual void Report  ( const Message& msg );
};


}}} // namespace [aworx::lib::results]

// #################################################################################################
// Redefine ALIB Debug macros
// #################################################################################################
#if ALIB_DEBUG && !defined(ALIB_DOX)

    #undef ALIB_ERROR
    #undef ALIB_WARNING
    #undef ALIB_MESSAGE
    #undef ALIB_ASSERT
    #undef ALIB_ASSERT_ERROR
    #undef ALIB_ASSERT_WARNING
    #undef ALIB_ASSERT_MESSAGE

    #define ALIB_ERROR(   ... )              {                aworx::lib::results::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, aworx::lib::results::Report::Types::Error  , __VA_ARGS__      );  }
    #define ALIB_WARNING( ... )              {                aworx::lib::results::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, aworx::lib::results::Report::Types::Warning, __VA_ARGS__      );  }
    #define ALIB_MESSAGE( ... )              {                aworx::lib::results::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, aworx::lib::results::Report::Types::Message, __VA_ARGS__      );  }

    #define ALIB_ASSERT( cond )              { if (!(cond)) { aworx::lib::results::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, aworx::lib::results::Report::Types::Error  , "Internal Error" ); } }
    #define ALIB_ASSERT_ERROR( cond, ... )   { if (!(cond)) { aworx::lib::results::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, aworx::lib::results::Report::Types::Error  , __VA_ARGS__      ); } }
    #define ALIB_ASSERT_WARNING( cond, ... ) { if (!(cond)) { aworx::lib::results::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, aworx::lib::results::Report::Types::Warning, __VA_ARGS__      ); } }
    #define ALIB_ASSERT_MESSAGE( cond, ... ) { if (!(cond)) { aworx::lib::results::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, aworx::lib::results::Report::Types::Message, __VA_ARGS__      ); } }

#endif


#endif // HPP_ALIB_RESULTS_REPORT
