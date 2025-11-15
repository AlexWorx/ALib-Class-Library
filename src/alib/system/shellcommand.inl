//==================================================================================================
/// \file
/// This header-file is part of module \alib_system of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace  system {

//==================================================================================================
/// \b %TShellCommand provides a lightweight interface for executing external shell commands and
/// capturing their output. It reads from the command’s standard output stream and accumulates
/// the text in a buffer.
/// The captured output is split into individual lines and stored in the class’s underlying
/// \alib{strings;util::TStringVector;StringVector}.
///
/// \par Usage options:
///   - Pure static usage: Call the static #Run method with a target buffer and an optional vector
///     for collecting lines. The new output is appended to the provided buffer; if a vector is
///     supplied, the newly captured portion is tokenized into lines and added to it.
///   - Instance-based usage: Create an instance and call its non-static #Run method.
///     By default, this clears the instance’s buffer and line vector before executing.
///     To preserve the current content and append new output, pass
///     \alib{lang;CurrentData;CurrentData::Keep}.
///
/// Method #Run can be sequentially invoked multiple times to aggregate the output of several shell
/// commands. With the static #Run variant, the same buffer and vector is to be passed to continue
/// appending.
/// With the non-static #Run, parameter \p{keepCurrent} controls whether existing content is
/// retained or not.
///
/// @see This is a very simple wrapper type. For example, no blocking or timely shell commands
///      can be handled. While simple, relient commands can be invoked with this class, more
///      complicated cases should be handled with alternatives, as:
///      - \https{boost.process,www.boost.org/doc/libs/latest/libs/process/doc/html/index.html}
///      - \https{POCO Process,docs.pocoproject.org/current/Poco.Process.html}, or
///      - \https{cpp-subprocess,github.com/arun11299/cpp-subprocess}.
//==================================================================================================
template<typename TAllocator= lang::HeapAllocator>
class TShellCommand : public strings::util::TStringVector<nchar, TAllocator>
{
  public:
    /// The allocator type that \p{TAllocator} specifies.
    using AllocatorType = TAllocator;

    /// The base type of this class.
    using StringVector  = strings::util::TStringVector<nchar, TAllocator>;

    /// The input buffer, collecting the output of the invoked shell command(s).
    strings::TAString<nchar, AllocatorType>  ReadBuffer;

    /// Default constructor. Usable with type \alib{lang;HeapAllocator}.
    TShellCommand() :  StringVector{}                                                             {}

    /// Constructor taking an allocator.
    /// @param ma The allocator to use.
    TShellCommand(AllocatorType& ma)
    :  StringVector{ma}
    ,  ReadBuffer{ma}                                                                             {}


    /// Executes the given command line by invoking the static variant of this method
    /// passing member #ReadBuffer and the inherited string vector (<c>*this</c>).
    /// @param  cmd      The command to execute.
    /// @param  keepData Denotes whether any prior results are kept or not.
    /// @return The exit code of the command. If -1 errno is set.
    int Run( const NCString& cmd, lang::CurrentData keepData= lang::CurrentData::Clear ) {
        if ( keepData == lang::CurrentData::Clear ) {
            ReadBuffer.Reset();
            this->clear();
        }
        return Run( cmd, ReadBuffer, this );
    }
    /// Executes the given command line.<br>
    /// The given \p{readBuffer} and vector \p{lines} are \b not reset. Instead the command result
    /// is appended to both. If this is not wanted, methods \b Reset and \c clear have to be
    /// invoked prior to calling this method.
    /// @param  cmd         The command to execute.
    /// @param  readBuffer  A string buffer to receive the command's output.
    /// @param  lines       An optional pointer to a vector of strings, which receives the lines
    ///                     of the output text.
    /// @return The exit code of the command. If -1 errno is set.
    static int Run( const NCString&                          cmd,
                    strings::TAString<nchar, AllocatorType>& readBuffer,
                    StringVector*                            lines= nullptr ) {

        constexpr int initialBufferSize = 4096;
        constexpr int readSize          = 1024 - 1;

        integer origBufferLen= readBuffer.Length();

      #if !defined(_WIN32) // true posix
        FILE* pipe= popen(cmd, "r");
      #elif defined (_WIN32)
        FILE* pipe= _popen(cmd, "r");
      #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
      #endif
        if (!pipe)
            return -1;

        readBuffer.EnsureRemainingCapacity(initialBufferSize);
        for (;;) {
            readBuffer.EnsureRemainingCapacity(readSize);
            if (!fgets(readBuffer.VBuffer()+readBuffer.Length(), readSize, pipe))
                break;
            integer len= readBuffer.Length();
            len= readBuffer.DetectLength(len);
            readBuffer.TrimEnd("\r\n"); // remove also '\r' in case different platform
            if ( readBuffer.Length() < len )
                readBuffer.Append(NNEW_LINE);
        }

      #if !defined(_WIN32) // true posix
        int resultCode = pclose(pipe);
        if (resultCode != -1) {
            // 0..255
            if (WIFEXITED(resultCode))          resultCode= WEXITSTATUS(resultCode);

            // Conventional shell-style encoding: 128 + signal number
            else if (WIFSIGNALED(resultCode))   resultCode= 128 + WTERMSIG(resultCode);
        }
      #elif defined (_WIN32) // win posix
        int resultCode = _pclose(pipe);
      #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
      #endif

        // prevent tokenizer to add an empty line. If after TimEnd() below, the string is emtpy,
        // one empty line will be added.
        if ( readBuffer.IsEmpty() )
            return resultCode;

        // trim whitespace/newlines
        readBuffer.TrimEnd();

        // Split to lines
        if (lines) {
            TokenizerN tknzr(NString(readBuffer.Buffer() + origBufferLen,
                                     readBuffer.Length() - origBufferLen )  , '\n');
            while (tknzr.HasNext())
                lines->Add(tknzr.Next(lang::Whitespaces::Keep).TrimEnd());
        }
        return resultCode;
    }
}; // class ShellCommand

#if !DOXYGEN
extern template ALIB_DLL int TShellCommand<lang::HeapAllocator>::Run(
                                           const NCString&,
                                           strings::TAString<nchar>&,
                                           strings::util::TStringVector<nchar, HeapAllocator>*);

#if ALIB_MONOMEM
extern template ALIB_DLL int TShellCommand<alib::MonoAllocator>::Run(
                                           const NCString&,
                                           strings::TAString<nchar, MonoAllocator>&,
                                           strings::util::TStringVector<nchar, MonoAllocator>*);
#endif
#endif // !DOXYGEN

} // namespace [ system]

/// Type alias in namespace \b alib.
using     ShellCommand  = system::TShellCommand<lang::HeapAllocator>;

#if ALIB_MONOMEM
/// Type alias in namespace \b alib.
using     ShellCommandMA= system::TShellCommand<MonoAllocator>;
#endif

} // namespace [alib]
