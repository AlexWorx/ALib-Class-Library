/** ************************************************************************************************
 * \file
 * This header file is part of sub-namespace #alib::lang::format of module \alib_basecamp of
 * the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_FORMAT_PARAGRAPHS
#define HPP_ALIB_LANG_FORMAT_PARAGRAPHS 1

#if !defined (HPP_ALIB_LANG_FORMAT_FORMATTER_PYTHONSTYLE)
    #include "alib/lang/format/formatterpythonstyle.hpp"
#endif
#if !defined (_GLIBCXX_STACK) && !defined(_STACK_)
    #include <stack>
#endif

namespace alib {
    namespace results { class Exception; }

namespace  lang::format {
    /** ************************************************************************************************
     * This class is used to format textual output, like console output.
     *
     * One central \b static method is #Format, which  format a "paragraph" that starts at a
     * certain index of the managed \b %AString buffer and reaches to its end.
     *
     * When an instance of the class is created, the members of the class provide the text
     * buffer as well as the other parameters which are needed for the static method. With methods
     * #Add, member #Formatter is used to append the given parameters and then the new paragraph is
     * formatted using static method #Format.<br>
     * This way, a longer text might be built by repetitive calls.
     **************************************************************************************************/
    class Paragraphs
    {
        // #############################################################################################
        // Fields
        // #############################################################################################
        protected:
        /** Internal buffer, used for field #Paragraphs, if no external string object was given. */
        AString                 text;

        public:
        /** A reference to the text buffer. Either refers to otherwise protected field #text or to
         * what was given in alternative construction. */
        AString&                Buffer;

        /**
         * The formatter to use.
         * In the constructor, this shared pointer is initialized with the
         * - \alib{lang::format,Formatter::GetDefault,ALib Default Formatter},
         *   in the case that this object's template character type equals \ref alib::character, or
         * - A shared pointer to a newly created formatter object of corresponding character type
         *   otherwise.
         */
        SPFormatter             Formatter;

        /** Used as parameter \p{lineWidth} of static method invocations. */
        integer                 LineWidth                                                       = 0;

        /**
         * Used as parameter \p{justifyChar} of static method invocations.
         * Usually set to <c>' '</c> to enable paragraph width justification.<br>
         * Defaults to <c>'\\0'</c> which disables it.
         */
        character               JustifyChar                                                  = '\0';

        /**
         * Used to detect special commands given with format strings.
         * \see Method #AddMarked for more information.
         *
         * Defaults to <c>'@'</c>.
         */
        character               MarkerChar                                                    = '@';

        /** The bullet used with increasing bullet depths. */
        std::vector<character>  MarkerBullets                      = {'*', '-', '*', '-', '*', '-'};


#if defined(ALIB_DOX)
        /**
         * Used as parameter \p{indent} of static method invocations.<br>
         * The indent string of the first line.
         *
         * This field can either be manipulated by direct access or preferably with
         * overloaded methods#PushIndent and #PopIndent.
         *
         * \note For technical reasons the template parameters of this member's type are missing
         *       in this documentation.
         *       In the source code, the type iss <b>%TLocalString<TChar,64></b>.
         */
        TLocalString          IndentFirstLine;

        /**
         * Used as parameter \p{indent} of static method invocations.<br>
         * The indent string of text lines, excluding the first line.
         *
         * This field can either be manipulated by direct access or preferably with
         * overloaded methods#PushIndent and #PopIndent.
         *
         * \note For technical reasons the template parameters of this member's type are missing
         *       in this documentation.
         *       In the source code, the type iss <b>%TLocalString<TChar,64></b>.
         */
        TLocalString          IndentOtherLines;
#else
        // As of 2017/4: When this goes to doxygen type TLocalString<TChar,64> appears
        // in the type list, e.g. in the inheritance graph of class TString.
        String64  IndentFirstLine;
        String64  IndentOtherLines;
#endif

        /**
         * The stack of indent sub-string sizes in string #IndentFirstLine.
         * Used with #PushIndent and #PopIndent.
         */
        std::stack<integer>             IndentSizesFirstLine;

        /**
         * The stack of indent sub-string sizes in string #IndentOtherLines.
         * Used with #PushIndent and #PopIndent.
         */
        std::stack<integer>             IndentSizesOtherLines;

        /**
         * This field is increased whenever a line of text added is longer than its current
         * value.
         * Might be used by to detect the maximum line width when field #LineWidth is set to \c 0
         * and hence no auto wrap is done.
         */
        integer                         DetectedMaxLineWidth                                     =0;


        protected:
        /** Internally reused list of boxes. */
        Boxes                           boxes;

        /** Buffer for processing marked text. */
        AString     markedBuffer;

        /** Buffer for processing marked text. */
        size_t                          markerBulletLevel                                        =0;

        // #############################################################################################
        // Constructor/Destructor
        // #############################################################################################
        public:
        /**
         * Parameterless constructor. Internal buffer #text will be used with reference #Buffer.
         */
        ALIB_API                 Paragraphs();

        /**
         * Alternative constructor that accepts an external buffer to use.
         * @param externalBuffer The external buffer to use and fill.
         */
        ALIB_API                 Paragraphs( AString& externalBuffer );

        // #############################################################################################
        // Interface
        // #############################################################################################
        public:
        /** ****************************************************************************************
         * Formats one or more paragraphs (separated by \b NewLine symbols) with three optional
         * features:
         *
         * - Wrapping of lines longer than lineWidth (word wrap)
         * - Justify the text, which here means "full justification", i.e. format the text to have
         *   lines of the exact same width.
         * - Adding an indentation to each line with an optionally different indentation for
         *   the first line after a \b NewLine symbol and subsequent ones.
         *
         * The paragraph starts at \p{startIdx} and all of the rest of the string is treated
         * as one paragraph. New-line character sequences found within the paragraph are considered
         * manual line ends. Hence, no block formatting for lines ending with a new line character
         * is performed.
         *
         * The method is static and hence can be used with arbitrary buffers.
         * Non-static methods #Add invoke this methods after adding the given content
         * to the internal buffer. Furthermore, convenience functions and corresponding member
         * variables, simplify the use of this method, when indirectly used through an instance of
         * the class.
         *
         * @param text              The text containing the paragraph to format.
         * @param startIdx          The start of the paragraph.
         * @param lineWidth         The width of the line. If \c 0 or negative, no line wrap is
         *                          performed.
         * @param justifyChar       If this is unequal to <c>'\0'</c> it denotes the fill
         *                          character used to justify the paragraph.
         *                          Defaults to <c>'\0'</c>, which disables justification.
         * @param[out] maxLineWidth Provides the maximum width of all text lines written.
         * @param indentFirstLine   The indent string of the first line. Defaults to \c nullptr.
         * @param indentOtherLines  The indent string of subsequent lines. Defaults to \c nullptr.
         *******************************************************************************************/
        ALIB_API
        static
        void    Format( AString&      text,
                        integer       startIdx,
                        integer       lineWidth,
                        character     justifyChar,
                        integer&      maxLineWidth,
                        const String& indentFirstLine = nullptr,
                        const String& indentOtherLines= nullptr  );

        /** ****************************************************************************************
         * Appends the given objects \p{args} to the internal #Buffer with the help of member
         * #Formatter. Then, static method #Format is invoked, providing our public
         * members as parameters. Finally, a newline sequence is added to #Buffer, but only if the
         * buffer is not empty and if it does not already end with a newline sequence.
         *
         * @throws <b>alib::lang::format::FMTExceptions</b><br>
         *         Rethrows exceptions from the formatter caused by errors in provided \p{args}.
         *
         * @param args   The list of arguments to add.
         *******************************************************************************************/
        ALIB_API
        void    Add( Boxes&  args );

        /** ****************************************************************************************
         * Variadic template argument version of #Add.
         *
         * @param args   The variadic list of arguments to add.
         * @return A reference to ourselves to allow concatenated calls.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        Paragraphs&  Add( const BoxedObjects&... args )
        {
            boxes.clear();
            boxes.Add( args... );
            Add( boxes );
            return *this;
        }

        /** ****************************************************************************************
         * This method implements a pre-processing of the text prior to #Add paragraphs found in
         * the text.
         *
         * The pre-processing is quite simple. Its purpose is to allow longer strings (e.g. loaded
         * from a resource pool) with multiple paragraphs to be formatted by embedded escape
         * sequences to include indents and nested bullet schemes.
         *
         * The escape sequences begin with the character stored in field #MarkerChar, which defaults
         * to <c>'@'</c>. The following table documents the sequences:
         *
         * Sequence | Description
         * ---------|------------------------------------------------------------------------
         * \@@   | Inserts the marker character itself.
         * \@>>  | Indent text by two spaces
         * \@<<  | Un-indent text by two spaces
         * \@*>  | Increases bullet level.
         * \@<*  | Decreases bullet level
         * \@P   | Inserts a new line (like '\n') but without ending the current and starting a new bullet point.
         * \@HLc | Inserts a horizontal line of width #LineWidth using \p{c} as fill character.
         *
         * The nested bullet point characters are received from vector #MarkerBullets.
         *
         * @throws <b>alib::lang::format::FMTExceptions</b>
         *   - \alib{lang::format::FMTExceptions,UnknownMarker}
         *   - \alib{lang::format::FMTExceptions,EndmarkerWithoutStart}
         *   - Rethrows formatter exceptions occurring due to errors in provided \p{args}.
         *
         * @param args       The list of arguments to add.
         ******************************************************************************************/
        ALIB_API
        void            AddMarked( Boxes&  args );

        /** ****************************************************************************************
         * Variadic template argument version of #AddMarked.
         *
         * @throws <b>alib::lang::format::FMTExceptions</b>
         *   - \alib{lang::format::FMTExceptions,UnknownMarker}
         *   - \alib{lang::format::FMTExceptions,EndmarkerWithoutStart}
         *
         * @param args   The variadic list of arguments to add.
         * @return A reference to ourselves to allow concatenated calls.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        Paragraphs&     AddMarked( const BoxedObjects&... args )
        {
            boxes.clear();
            boxes.Add( args... );
            AddMarked( boxes );
            return *this;
        }

        /** ****************************************************************************************
         * Removes the last new line character at the end of the #Buffer.
         *
         * @return A reference to the text object.
         ******************************************************************************************/
        AString&        RemoveLastNewLine()
        {
            if( Buffer.EndsWith( NewLine() ) )
                Buffer.template DeleteEnd<false>( NewLine().Length() );
            return Buffer;
        }

        /** ****************************************************************************************
         * Clears field #Buffer.
         *
         * @return A reference to ourselves to allow concatenated calls.
         ******************************************************************************************/
        ALIB_API
        Paragraphs&     Clear();


        /** ****************************************************************************************
         * Add a given number of characters (default is spaces) to the indentation strings
         * #IndentFirstLine and #IndentOtherLines.
         *
         * Use #PopIndent to remove the indent.
         *
         * @param qty       The quantity of characters to add or remove
         * @param fillChar  The character (used only if \p{qty} is positive).
         * @return A reference to ourselves to allow concatenated calls.
         ******************************************************************************************/
        ALIB_API
        Paragraphs&     PushIndent( uinteger qty, character fillChar = ' ' );

        /** ****************************************************************************************
         * Add the given strings to members #IndentFirstLine and #IndentOtherLines.
         *
         * @param indentFirstLine   The string to add to the current indentation stored in
         *                          #IndentFirstLine.
         * @param indentOtherLines  The string to add to the current indentation stored in
         *                          #IndentOtherLines.<br>
         *                          Defaults to \b NullString, which sets it to the same value as
         *                          \p{indentFirstLine}.
         * @return A reference to ourselves to allow concatenated calls.
         ******************************************************************************************/
        ALIB_API
        Paragraphs&     PushIndent( const String& indentFirstLine,
                               const String& indentOtherLines=nullptr);

        /** ****************************************************************************************
         * Removes the most recently added indent.
         * @return A reference to ourselves to allow concatenated calls.
         ******************************************************************************************/
        ALIB_API
        Paragraphs&     PopIndent();
    };

}} // namespace [alib::lang::format]

#endif // HPP_ALIB_LANG_FORMAT_PARAGRAPHS
