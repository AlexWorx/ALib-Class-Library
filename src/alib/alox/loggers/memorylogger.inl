//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace lox { namespace loggers {

//==================================================================================================
/// A logger that logs all messages to an in-memory buffer of type AString. The name of the \e Logger
/// defaults to "MEMORY".
//==================================================================================================
class MemoryLogger : public alib::lox::textlogger::PlainTextLogger
{
  //################################################################################################
  // public fields
  //################################################################################################
  public:
    /// The logging Buffer. This can be accessed publicly and hence used quite freely.
    /// Especially, the whole log can easily be cleared using
    /// \alib{strings;TAString::Reset;AString::Reset}.
    /// In multithreaded environments, \c Lox interface's mutex should be acquired
    /// before accessing this buffer. The initial size of the buffer is 8kb.
    AString                     MemoryLog;

    /// If this field is set to \c true (which is the default), the effective length of the
    /// messages when converted to wide character strings are taken into account.
    ///
    /// Switching this off increases the overall logging performance (especially when logging
    /// into memory) significantly.
    bool                        UseWStringLengthForTabAdjustments                             =true;

  //################################################################################################
  // Constructor/destructor
  //################################################################################################
  public:
    /// Creates a MemoryLogger with the given name.
    /// @param name              (Optional) The name of the \e Logger. Defaults to "MEMORY".
    /// @param pruneESCSequences (Optional) Sets the member \ref PruneESCSequences.
    ///                          Defaults to \c true.
    /// @param useWStringLengthForTabAdjustments (Optional) Sets the member
    ///                          \ref UseWStringLengthForTabAdjustments.
    ///                          Defaults to \c true.
    explicit        MemoryLogger( const NString& name                   = nullptr,
                                  bool pruneESCSequences                = true,
                                  bool useWStringLengthForTabAdjustments= true )
    : PlainTextLogger( name, "MEMORY" ) {
        MemoryLog.SetBuffer( 8092 );
        PruneESCSequences                = pruneESCSequences;
        UseWStringLengthForTabAdjustments= useWStringLengthForTabAdjustments;
    }


    /// Destructs a MemoryLogger
    virtual        ~MemoryLogger()                                                       override {}

  //################################################################################################
  // Abstract method implementations
  //################################################################################################
  protected:
    /// Start a new log line. Appends a new-line character sequence to previously logged lines.
    ///
    /// @param phase  Indicates the beginning or end of a log line.
    /// @return Always returns true.
    virtual bool        notifyPlainTextLogOp(lang::Phase phase)                           override {
        // append new line if buffer has already lines stored
        if ( phase == lang::Phase::Begin && MemoryLog.IsNotEmpty() )
            MemoryLog.NewLine();
        return true;
    }

    /// Write the given region of the given AString to the destination buffer.
    ///
    /// @param buffer   The string to write a portion of.
    /// @param start    The start of the portion in \p{buffer} to write out.
    /// @param length   The length of the portion in \p{buffer} to write out.
    /// @return The number of characters written, -1 on error.
    virtual integer     logPlainTextPart( const String& buffer,
                                          integer       start,    integer length )        override {
        MemoryLog._<NC>( buffer, start, length );
        return UseWStringLengthForTabAdjustments
            ? buffer.Substring<NC>( start, length ).WStringLength()
            : length;
    }

    /// Empty implementation, not needed for this class
    virtual    void     notifyMultiLineOp( lang::Phase )                                 override {}

}; // class MemoryLogger

}} // namespace alib[::lox::loggers]

/// Type alias in namespace \b alib.
using     MemoryLogger=           lox::loggers::MemoryLogger;

}  // namespace [alib]
