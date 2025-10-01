//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace strings::util {


/// This is a virtual base type defining the interface for derived struct
/// \alib{strings::util;StringEscaperStandard} and possible custom derivates.<br>
/// However, this class is not abstract but instead implements the interface by just copying
/// the strings to the given target buffers. The rationale for this is that it can be used
/// in situations, where no externalization and import of externalized strings are needed.
/// In other words: In situations where a function expects a \b StringEscaper where none is necessary
/// a local instance of this type can be created and just passed.
///
/// @see
///   Any further explanation is found with the documentation of derived type
///   \alib{strings::util;StringEscaperStandard}.
struct StringEscaper
{
    ///  Virtual destructor.
    virtual ~StringEscaper()                                                                      {}

    //==============================================================================================
    /// Just copies the given \p{src} string to \p{dest}.
    ///
    /// @param  src        The source string.
    /// @param  dest       The destination string.
    /// @param  delimiters Ignored
    /// @return \p{dest} to allow concatenated operations.
    //==============================================================================================
    virtual inline
    AString&    Escape( const String& src, AString& dest, const String& delimiters )           const
    { (void) delimiters; return dest << src; }

    //==============================================================================================
    /// Just copies the given \p{src} string to \p{dest}.
    ///
    /// @param  src      The source string.
    /// @param  dest     The destination string.
    /// @return \p{dest} to allow concatenated operations.
    //==============================================================================================
    virtual inline
    AString&    Unescape( const String& src, AString& dest )                                   const
    { return dest << src; }

#if ALIB_MONOMEM
    //==============================================================================================
    /// Simply tokenizes \p{src} and feeds the tokens into \p{result}.
    ///
    /// \par Availability
    ///   This method is defined only if module \alib_monomem is included in the \alibbuild.
    /// @param  result     The destination list of strings.
    /// @param  src        The source string.
    /// @param  delimiters A set of characters accepted as delimiters.
    /// @return The number of tokens found.
    //==============================================================================================
    ALIB_DLL virtual
    int    EscapeTokens( StringVectorMA& result, const String& src, const String& delimiters) const;

    //==============================================================================================
    /// Simply tokenizes \p{src} and feeds the tokens into \p{result}.
    /// (In fact, #EscapeTokens is called.)
    ///
    /// \par Availability
    ///   This method is defined only if module \alib_monomem is included in the \alibbuild.
    /// @param  result     The destination list of strings.
    /// @param  src        The source string.
    /// @param  delimiters A set of characters accepted as delimiters.
    /// @return The number of tokens found.
    //==============================================================================================
    virtual inline
    int    UnescapeTokens(StringVectorMA& result, const String& src, const String& delimiters) const
    { return EscapeTokens( result, src, delimiters); }
                                                       
#endif // #if ALIB_MONOMEM

};  // StringEscaper

//==================================================================================================
/// This struct implements the interface given with \alib{strings::util;StringEscaper}.
/// Its purpose is to convert string data to external string representations and vice versa.
/// Such conversion is needed when C++ strings contain non-readable characters like
/// 'new line', 'carriage return' or 'tabulator' and such strings should be stored in ASCII or
/// unicode files. The common way of conversion is to add "escape sequences", for example <c>"\n"</c>
/// for the 'new line' character.
///
/// Furthermore if a C++ string starts or ends with spaces, the string has to be quoted and in this
/// case occurences of the quote character <c>(\")</c> inside the string have to be escaped.
/// Quotation is also needed, if a series of strings, separated by possible delimiter characters
/// needs to be escaped, and a string contains one of the delimiters itself. Likewise, when such
/// string is imported back to C++ representation, delimiters must only be recognized when outside
/// of quoted string tokens.
///
/// With this approach, this implementation should be compatible with INI-files, JSon files
/// and such. If a different approach is needed, the virtual functions may be overridden by
/// custom descendents.
///
/// ---------------------------
/// This default implementation proceeds as follows:
/// - "Externalizing" a value:
///   - Value is surrounded by quotes if it starts or ends with spaces or if it includes one of
///     the delimiter tokens.
///   - A few characters are escaped using \c '\\'. Those are
///     \c \\n, \c \\r, \c \\t , \c \\a, \c \\b, \c \\v, \c \\f, \c \\e and also
///     the double quotation marks \c \\" and the backslash itself (\c \\\\).
///
/// - "Internalizing" a value:
///   - If (non-escaped) quote \c " characters are found, those are removed and whitespaces
///     within such quotes are kept.
///   - Escaped characters are converted to their original value.
///
/// - Loading variables from external strings:
///   - If the provided variable has a valid delimiter, this character is used to tokenize
///     the external string.
///   - Values are trimmed, unless quoted. Quoted characters themselves are removed.
///   - Delimiters found within a pair of quotes are ignored.
///   - Each value found is internalized separately.
//==================================================================================================
struct StringEscaperStandard   : public StringEscaper
{
    ///  Virtual destructor.
    virtual ~StringEscaperStandard()                                                    override  {}

    //==============================================================================================
    /// Converts the given \p{src} string to an external representation.
    /// The escape symbol is backslash \c '\\' and the following characters are escaped:
    /// \c \\n, \c \\r, \c \\t , \c \\a, \c \\b, \c \\v, \c \\f, \c \\e. Furthermore
    /// the double quotation marks \c \\" and the backslash itself (\c \\\\).<br>
    ///
    /// Besides that, the value is surrounded by double quotes \c " if it starts or ends with spaces
    /// or if it includes one of the delimiter characters. The rationale of the latter is understood
    /// when it comes to method #UnescapeTokens: Here, delimiters are ignored if they reside in a
    /// quoted string. If neither trailing nor leading spaces nor a delimiter is found, the string
    /// is \b not quoted. In situations, a quoted string is always needed, the caller has to test
    /// if a string was quoted and add the quote after the invocation of this method.
    ///
    /// @param  src        The source string to convert to external representation.
    /// @param  dest       The destination string buffer.
    /// @param  delimiters If one of these characters is found in the string, the value is quoted
    /// @return \p{dest} to allow concatenated operations.
    //==============================================================================================
    ALIB_DLL virtual
    AString&    Escape( const String& src, AString& dest, const String& delimiters ) const override;

    //==============================================================================================
    /// Trims \p{src}, removes surrounding quotes and, un-escapes characters as defined with
    /// method #Escape.
    ///
    /// @param  src      The source string.
    /// @param  dest     The destination string.
    /// @return \p{dest} to allow concatenated operations.
    //==============================================================================================
    ALIB_DLL virtual
    AString&    Unescape( const String& src, AString& dest )                         const override;

#if ALIB_MONOMEM
    //==============================================================================================
    /// Parses a list of tokens separated by the given \p{delimiter} and calls method
    /// #Escape for each of them. The results are copied to string list \p{result}.
    ///
    /// \par Availability
    ///   This method is defined only if module \alib_monomem is included in the \alibbuild.
    ///
    /// @param  result     The destination list of strings.
    /// @param  src        The source string.
    /// @param  delimiters A set of characters defining the delimiters.
    /// @return The number of tokens found.
    //==============================================================================================
    ALIB_DLL virtual
    int  EscapeTokens( StringVectorMA& result, const String& src, const String& delimiters ) const override;

    //==============================================================================================
    /// Parses a list of tokens separated by the given \p{delimiter} and calls method
    /// #Unescape for each of them. The results are copied to string list \p{result}.
    /// Delimiters found within quoted strings are rightfully ignored.
    ///
    /// \par Availability
    ///   This method is defined only if module \alib_monomem is included in the \alibbuild.
    /// @param  result     The destination list of strings.
    /// @param  src        The source string.
    /// @param  delimiters A set of characters accepted as delimiters.
    /// @return The number of tokens found.
    //==============================================================================================
    ALIB_DLL virtual
    int  UnescapeTokens(StringVectorMA& result,
                        const String&   src   , const String& delimiters)            const override;
#endif //#if ALIB_MONOMEM

};  // struct StringEscaperStandard


} // namespace alib::[strings::util]

/// Type alias in namespace \b alib.
using     StringEscaper=                strings::util::StringEscaper;

/// Type alias in namespace \b alib.
using     StringEscaperStandard=        strings::util::StringEscaperStandard;


} // namespace [alib]


