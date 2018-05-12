// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION) || defined(HPP_ALIB_LANG_REPORT)
    #error "include alib/alib.hpp instead of this header"
#endif

// then, set include guard
#ifndef HPP_ALIB_LANG_REPORT
#define HPP_ALIB_LANG_REPORT 1

#if !defined(_GLIBCXX_STACK) && !defined(_STACK_)
    #include <stack>
#endif


namespace aworx { namespace lib {

// #################################################################################################
// forward declarations
// #################################################################################################
namespace threads { class ThreadLock; }

namespace lang {

class ReportWriter;

// #################################################################################################
// Redefine ALIB Debug macros
// #################################################################################################
#if ALIB_DEBUG && !defined( DOX_PARSER )

    #undef ALIB_ERROR
    #undef ALIB_WARNING
    #undef ALIB_MESSAGE
    #undef ALIB_ASSERT
    #undef ALIB_ASSERT_ERROR
    #undef ALIB_ASSERT_WARNING

    #define ALIB_ERROR(   ... )              {                aworx::lib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, 0, __VA_ARGS__      );  }
    #define ALIB_WARNING( ... )              {                aworx::lib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, 1, __VA_ARGS__      );  }
    #define ALIB_MESSAGE( ... )              {                aworx::lib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, 2, __VA_ARGS__      );  }

    #define ALIB_ASSERT( cond )              { if (!(cond)) { aworx::lib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, 0, "Internal Error" ); } }
    #define ALIB_ASSERT_ERROR( cond, ... )   { if (!(cond)) { aworx::lib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, 0, __VA_ARGS__      ); } }
    #define ALIB_ASSERT_WARNING( cond, ... ) { if (!(cond)) { aworx::lib::lang::Report::GetDefault().DoReport( ALIB_CALLER_PRUNED, 1, __VA_ARGS__      ); } }

#endif


/** ************************************************************************************************
 * This class provides a simple facility to collect what is called a \e 'report'.
 * Reports are maintenance messages, mostly error and warning messages, but is not aiming to replace
 * any sort of error handling.
 * (In \alib itself, sending a \e 'report' usually precedes raising an error.)
 *
 * While a process can create different objects of this class, usually, the default instance
 * available by inheriting from class
 * \ref aworx::lib::lang::Singleton "Singleton".
 * is sufficient and all \alib internal warnings and errors will be directed to this one.
 * Software built on \alib should do the same with internal warnings and errors. An own
 * instance might be created to collect other types of reports.
 *
 * This class uses a member of type
 * \ref aworx::lib::lang::ReportWriter "ReportWriter" to actually write the reports. By default, an
 * object of type
 * \ref aworx::lib::lang::ReportWriterStdIO "ReportWriterStdIO" is attached.
 *
 * The reporting method,
 * \ref aworx::lib::lang::Report::DoReport "DoReport" will check the flags provided with
 * \ref aworx::lib::lang::Report::PushHaltFlags "PushHaltFlags" for message types \c 0 (errors)
 * and \c 1 (warnings), and may invoke \e assert(). Such assertions are effective
 * only in the debug compilation of the library/executable. Custom \e 'ReportWriters' might
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
 * header <b>"alib/lang/report.hpp"</b>, these macros become redefined to use \alib reports.
 *
 * For convenience, in debug compilations of the library, these macros provide the file name,
 * line number and method name of the invocation source, which can be used by more sophisticated
 * versions of currently attached \ref aworx::lib::lang::ReportWriter "ReportWriter".
 *
 * \note
 *  For debug output statements, it is advised to rather use the
 *  [ALox Logging Library](http://alexworx.github.io/ALox-Logging-Library/index.html) instead
 *  of \alib reports and corresponding macros.
 **************************************************************************************************/
class Report
{
    // #############################################################################################
    // Inner classes
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * A report message.
         ******************************************************************************************/
        class Message : public Boxes
        {
            public:

            /// The file name that reported.
            const NTString&     File;

            /// The line number in the source file that reported.
            int                 Line;

            /// The function/method name that reported.
            const NTString&     Func;

            /**
             * The message type. \c 0 indicates \e 'severe' errors, \c 1 warnings.
             * Others are status messages and may be defined (interpreted) by custom
             * implementations of
             * \ref aworx::lib::lang::ReportWriter "ReportWriter".
             */
            int                 Type;

            /** Constructs a message.
             * @param file     Information about the scope of invocation.
             * @param line     Information about the scope of invocation.
             * @param func     Information about the scope of invocation.
             * @param type     The message type.
             * @param boxables Variadic, templated list of arguments.
             */
            template <typename... TBoxables>
            Message( const NTString& file, int line, const NTString& func,
                     int type, TBoxables&&... boxables )
            : File(file),  Line(line), Func(func), Type(type)
            {
                Add( std::forward<TBoxables>(boxables)... );
            }
        };

    // #############################################################################################
    // protected fields
    // #############################################################################################
    protected:
        /// The default Report used internally by \alib and usually by processes that rely on \alib.
        ALIB_API static Report*         defaultReport;

        /**
         * A stack of writers. The topmost one is the actual.
         * Can be set at runtime using methods #PushWriter and #PopWriter.
         */
        std::stack<ReportWriter*>       writers;


        /** This is a flag that avoids recursion. Recursion might occur when a more sophisticated
         * report writer sends a report (e.g. an ALIB Error or Warning). Recursive calls are
         * rejected without further notice.
         */
        bool                            recursionBlocker                                    = false;

        /// A Lock to protect against multithreaded calls.
        threads::ThreadLock*            lock;

        /**
         * A stack of integers. The topmost value is used to decide, whether program execution is
         * halted on message of type 'error' (type \c 0, bit \c 0) or of type 'warning'
         * (type \c 1, bit \c 1).
         * Can be set at runtime using methods #PushHaltFlags and #PopHaltFlags.
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
         * Deletes static / global resources.
         ******************************************************************************************/
        ALIB_API
        static
        void TerminationCleanUp();

        /** ****************************************************************************************
         * Reports the given message to the current
         * \ref aworx::lib::lang::ReportWriter "ReportWriter" in place. The default \b ReportWriter
         * will print the message on the process console. Furthermore, in debug
         * execution the flags provided with #PushHaltFlags is checked.
         * If this is set in respect to type of message (field
         * \ref aworx::lib::lang::Report::Message::Type "Report::Message::Type"), he program
         * halts or suspends into the debugger (platform and language specific).
         *
         * @param message The message to report.
         ******************************************************************************************/
        ALIB_API
        void     DoReport( const Message& message );

        /** ****************************************************************************************
         * Overloaded method that fetches all arguments needed to construct a
         * \ref aworx::lib::lang::Report::Message "Report::Message" object
         * to pass to #DoReport.
         *
         * @param file    Information about the scope of invocation.
         * @param line    Information about the scope of invocation.
         * @param func    Information about the scope of invocation.
         * @param type    The report type.
         * @param msgs    Variadic list of boxes.
         ******************************************************************************************/
        template <typename... Boxes>
        void DoReport( const NTString& file, int line, const NTString& func,
                       int type,  Boxes&&... msgs )
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
 * Interface that defines a writer for for %ALib \ref aworx::lib::lang::Report "Report".
 * By default, an instance of \ref aworx::lib::lang::ReportWriterStdIO "ReportWriterStdIO"
 * is installed.
 * Applications may implement their own ReportWriter.
 **************************************************************************************************/
class ReportWriter
{
    public:
        /** ****************************************************************************************
         * Virtual destructor
         ******************************************************************************************/
        virtual ~ReportWriter()                    {}

        /** ****************************************************************************************
         * Notify activation/deactivation
         * @param phase     Information if activated or deactivated.
         ******************************************************************************************/
        virtual void NotifyActivation  ( lang::Phase phase )      =0;

        /** ****************************************************************************************
         * Report a message. Pure virtual abstract interface method.
         * @param msg  The message to report.
         ******************************************************************************************/
        virtual void Report  ( const Report::Message& msg )    =0;
};

/** ************************************************************************************************
 * The standard \b %ReportWriter writing the message to \c std::cout and \c std::cerr.
 * The global formatter singleton is used is used to process the objects in the report message.
 * This is by default of type \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}.
 * See method * \alib{strings,Strings::GetDefaultFormatter} for more information.
 **************************************************************************************************/
class ReportWriterStdIO : public ReportWriter, public Singleton<ReportWriterStdIO>
{
    //! @cond NO_DOX
        friend class Singleton<ReportWriterStdIO>;
        friend class Report;
    //! @endcond

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
        virtual void NotifyActivation  ( lang::Phase phase );

        /** ****************************************************************************************
         * Just writes the prefix \"ALib Report (Error):\" (respectively \"ALib Report (Warning):\"
         * and the error message to the cout.
         * On Windows platform, if a debugger is present, the message is also written using
         * <em>OutputDebugStringA</em>.
         *
         * @param msg  The message to report.
         ******************************************************************************************/
        ALIB_API
        virtual void Report  ( const Report::Message& msg );
};


}}}// namespace [aworx::lib::lang]

#endif // HPP_ALIB_LANG_REPORT
