//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace lox {
//==================================================================================================
/// This is the C++ namespace for the implementation of <em>logger classes</em> that are provided
/// by default with <b>%ALox Logging Library</b>.
///
/// Developed by A-Worx GmbH and published under Boost Software License.
//==================================================================================================
namespace loggers {

//==================================================================================================
/// A logger that logs all messages to the standard output <em>std::cout</em>.
/// The name of the \e Logger defaults to "CONSOLE".
///
/// \alox text logger escape sequences (see class \ref alib::lox::ESC "ESC")
/// are removed and ignored.
//==================================================================================================
class ConsoleLogger : public textlogger::PlainTextLogger
{
  protected:
    /// Allocator used for the \alib{strings::compatibility::std;OStreamWriter}.
    MonoAllocator       ma;

    /// The \b OStreamWriter encapsulated in a placeholder. The instance is constructed
    /// and destructed with #notifyPlainTextLogOp. With that, synchronized output is
    /// guaranteed.
    lang::Placeholder<OStreamWriter<nchar, MonoAllocator, true>>  writer;

  public:
    /// Creates a ConsoleLogger.
    /// @param name (Optional) The name of the \e Logger, defaults to "CONSOLE".
    explicit            ConsoleLogger( const NString& name= nullptr )
    : PlainTextLogger( name, "CONSOLE" )
    , ma(ALIB_DBG("ConsoleLogger",) 8)                                                            {}

    /// Destructs a ConsoleLogger
    virtual            ~ConsoleLogger()                                                  override {}

  //################################################################################################
  // Abstract method implementations
  //################################################################################################
  protected:
    /// Starts/ends log line. Appends a new-line character sequence to previously logged lines.
    ///
    /// @param phase  Indicates the beginning or end of a log line.
    /// @return Always returns true.
    ALIB_DLL
    virtual bool        notifyPlainTextLogOp( lang::Phase phase )                          override;

    /// Writes the given region of the given string to the console.
    ///
    /// @param buffer   The string to write a portion of.
    /// @param start    The start of the portion in \p{buffer} to write out.
    /// @param length   The length of the portion in \p{buffer} to write out.
    /// @return The number of characters written, -1 on error.
    ALIB_DLL
    virtual integer     logPlainTextPart( const     String& buffer,
                                          integer   start,           integer length )      override;


    /// Empty implementation, not needed for this class
    virtual void        notifyMultiLineOp( lang::Phase )                                 override {}

}; // class ConsoleLogger


}} // namespace alib[::lox::loggers]

/// Type alias in namespace \b alib.
using     ConsoleLogger=           lox::loggers::ConsoleLogger;

}  // namespace [alib]
