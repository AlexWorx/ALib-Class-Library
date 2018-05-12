// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_STRINGS_FORMAT_SIMPLETEXT)
    #error "Header already included"
#endif


// then, set include guard
#ifndef HPP_ALIB_STRINGS_FORMAT_SIMPLETEXT
//! @cond NO_DOX
#define HPP_ALIB_STRINGS_FORMAT_SIMPLETEXT 1
//! @endcond

// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALIB_STRINGS_FORMATTER_PYTHONSTYLE)
    #include "alib/strings/format/formatterpythonstyle.hpp"
#endif

#if !defined (_GLIBCXX_STACK) && !defined(_STACK_)
    #include <stack>
#endif

namespace aworx { namespace lib {

    namespace lang { class Exception; }

namespace strings { namespace format  {

/** ************************************************************************************************
 * The class provides static method #FormatParagraph to format a "paragraph"
 * that starts at a certain index of an  \b %AString buffer and reaches to its end.
 *
 * When an instance of the class is created, the (public) members of the class provide the text
 * buffer and the parameters which are needed for the static method. With methods
 * #Add, member #Formatter is used to append the given parameters and then format the
 * new paragraph. This way, a longer text might be built by repetitive calls.
 *
 * @tparam TChar    The character type. Implementations for \c char and \c wchar_t are provided
 *                  with type definitions \ref aworx::NSimpleText and \ref aworx::WSimpleText.
 **************************************************************************************************/
template<typename TChar>
class SimpleTextBase
{
    // #############################################################################################
    // Fields
    // #############################################################################################
    public:
        /// The text buffer.
        AStringBase<TChar>              Text;

        /** The formatter to use.
         *  In the constructor, this shared pointer is initialized with
         *  - \alib{strings,Strings::GetDefaultFormatter} in the case that this object's template
         *    character type equals \ref aworx::character and
         *  - A shared pointer to a newly created formatter object of corresponding character type.
         */
        std::shared_ptr<lib::strings::format::FormatterBase<TChar>>
                                        Formatter;

        /// Used as parameter \p{lineWidth} of static method invocations.
        integer                         LineWidth                                               = 0;

        /**
         * Used as parameter \p{justifyChar} of static method invocations.
         * Usually set to <c>' '</c> to enable paragraph width justification.<br>
         * Defaults to <c>'\\0'</c> which disables it.
         */
        TChar                           JustifyChar                                          = '\0';

        /**
         * Used to detect special commands given with format strings. \see Method #AddMarked
         * for more information.
         *
         * Defaults to <c>'@'</c>.
         */
        TChar                           MarkerChar                                            = '@';

        /// The bullet used with uneven bullet depths.
        std::vector<TChar>              MarkerBullets              = {'*', '-', '*', '-', '*', '-'};


#if defined(DOX_PARSER)
        /**
         * Used as parameter \p{indent} of static method invocations.<br>
         * The indent string of the first line.
         *
         * This field can either be manipulated by direct access or preferably with
         * overloaded methods#PushIndent and #PopIndent.
         *
         * \note For technical reasons the template parameters of this member's type are missing
         *       in this documentation.
         *       In the source code, the type iss <b>%PreallocatedStringBase<TChar,64></b>.
         */
        PreallocatedStringBase IndentFirstLine;

        /**
         * Used as parameter \p{indent} of static method invocations.<br>
         * The indent string of text lines, excluding the first line.
         *
         * This field can either be manipulated by direct access or preferably with
         * overloaded methods#PushIndent and #PopIndent.
         *
         * \note For technical reasons the template parameters of this member's type are missing
         *       in this documentation.
         *       In the source code, the type iss <b>%PreallocatedStringBase<TChar,64></b>.
         */
        PreallocatedStringBase IndentOtherLines;
#else
        // As of 2017/4: When this goes to doxygen type PreallocatedStringBase<TChar,64> appears
        // in the type list, e.g. in the inheritance graph of class StringBase.
        PreallocatedStringBase<TChar,64> IndentFirstLine;
        PreallocatedStringBase<TChar,64> IndentOtherLines;
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
        /// Internally reused list of boxes.
        Boxes                           boxes;

        /// Buffer for processing marked text.
        AStringBase<TChar>              markedBuffer;

        /// Buffer for processing marked text.
        size_t                          markerBulletLevel                                        =0;

    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    public:

        ALIB_API                 SimpleTextBase();

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
         * @param justifyChar       If this is unequal to <c>'\0'</c> it  denotes the fill
         *                          character used to justify the paragraph. If <c>'\0'</c>,
         * @param[out] maxLineWidth Provides the maximum width of all text lines written.
         *                          paragraph is not justified.
         * @param indentFirstLine   The indent string of the first line. Defaults to \c nullptr.
         * @param indentOtherLines  The indent string of subsequent lines. Defaults to \c nullptr.
         *******************************************************************************************/
        ALIB_API
        static
        void    FormatParagraph( AStringBase<TChar>& text,
                                 integer startIdx, integer lineWidth, TChar justifyChar,
                                 integer& maxLineWidth,
                                 const StringBase<TChar>& indentFirstLine = nullptr,
                                 const StringBase<TChar>& indentOtherLines= nullptr  );

        /** ****************************************************************************************
         * Appends the given objects \p{args} to the internal buffer #Text with the help of member
         * #Formatter. Then, static method #FormatParagraph is invoked, providing our public
         * members as parameters. Finally, a newline sequence is added to #Text, but only if the
         * buffer is not empty and if it does not already end with a newline sequence.
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
        SimpleTextBase&  Add( const BoxedObjects&... args )
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
         * from resources) with multiple paragraphs to be formatted by embedded escape sequences
         * to include indents and nested bullet schemes.
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
         * @throws aworx::lib::strings::format::Exceptions
         *   - \alib{strings::format::Exceptions,UnknownMarker}
         *   - \alib{strings::format::Exceptions,EndmarkerWithoutStart}
         *
         * @param args       The list of arguments to add.
         ******************************************************************************************/
        ALIB_API
        void    AddMarked( Boxes&  args );

        /** ****************************************************************************************
         * Variadic template argument version of #AddMarked.
         *
         * @throws aworx::lib::strings::format::Exceptions
         *   - \alib{strings::format::Exceptions,UnknownMarker}
         *   - \alib{strings::format::Exceptions,EndmarkerWithoutStart}
         *
         * @param args   The variadic list of arguments to add.
         * @return A reference to ourselves to allow concatenated calls.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        SimpleTextBase&  AddMarked( const BoxedObjects&... args )
        {
            boxes.clear();
            boxes.Add( args... );
            AddMarked( boxes );
            return *this;
        }

        /** ****************************************************************************************
         * Removes the last new line character at the end of #Text.
         *
         * @return A reference to the text object.
         ******************************************************************************************/
        AString&        RemoveLastNewLine()
        {
            if( Text.EndsWith( NewLine ) )
                Text.template DeleteEnd<false>( NewLine.Length() );
            return Text;
        }

        /** ****************************************************************************************
         * Clears field #Text.
         *
         * @return A reference to ourselves to allow concatenated calls.
         ******************************************************************************************/
        ALIB_API
        SimpleTextBase&     Clear();


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
        SimpleTextBase&     PushIndent( uinteger qty, TChar fillChar = ' ' );

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
        SimpleTextBase&     PushIndent( const StringBase<TChar>& indentFirstLine,
                                        const StringBase<TChar>& indentOtherLines = nullptr );

        /** ****************************************************************************************
         * Removes the most recently added indent.
         * @return A reference to ourselves to allow concatenated calls.
         ******************************************************************************************/
        ALIB_API
        SimpleTextBase&     PopIndent();


        /** ****************************************************************************************
         * Appends the details of the given \b %Exception object to this text.
         *
         * @param exception  The exception to log.
         * @return A reference to ourselves to allow concatenated calls.
         ******************************************************************************************/
        ALIB_API
        SimpleTextBase&    AddException( aworx::lib::lang::Exception&  exception );

};

//! @cond NO_DOX

       template<>                                  SimpleTextBase<char   >::SimpleTextBase ();
extern template ALIB_API SimpleTextBase<char   >&  SimpleTextBase<char   >::PushIndent     (const NString&,const NString&);
extern template ALIB_API SimpleTextBase<char   >&  SimpleTextBase<char   >::PushIndent     (uinteger qty, char   );
extern template ALIB_API SimpleTextBase<char   >&  SimpleTextBase<char   >::PopIndent      ();
extern template ALIB_API SimpleTextBase<char   >&  SimpleTextBase<char   >::Clear          ();
extern template ALIB_API void                      SimpleTextBase<char   >::Add            (Boxes&);
extern template ALIB_API void                      SimpleTextBase<char   >::AddMarked      (Boxes&);
extern template ALIB_API void                      SimpleTextBase<char   >::FormatParagraph(NAString&,integer,integer,char   ,integer&,const NString&,const NString&);
extern template ALIB_API SimpleTextBase<char   >&  SimpleTextBase<char   >::AddException   (Exception&);

       template<>                                  SimpleTextBase<wchar_t>::SimpleTextBase ();
extern template ALIB_API SimpleTextBase<wchar_t>&  SimpleTextBase<wchar_t>::PushIndent     (const WString&,const WString&);
extern template ALIB_API SimpleTextBase<wchar_t>&  SimpleTextBase<wchar_t>::PushIndent     (uinteger qty, wchar_t);
extern template ALIB_API SimpleTextBase<wchar_t>&  SimpleTextBase<wchar_t>::PopIndent      ();
extern template ALIB_API SimpleTextBase<wchar_t>&  SimpleTextBase<wchar_t>::Clear          ();
extern template ALIB_API void                      SimpleTextBase<wchar_t>::Add            (Boxes&);
extern template ALIB_API void                      SimpleTextBase<wchar_t>::AddMarked      (Boxes&);
extern template ALIB_API void                      SimpleTextBase<wchar_t>::FormatParagraph(WAString&,integer,integer,wchar_t,integer&,const WString&,const WString&);
extern template ALIB_API SimpleTextBase<wchar_t>&  SimpleTextBase<wchar_t>::AddException   (Exception&);
//! @endcond

}}} // namespace aworx[::lib::strings::util]



}  // namespace aworx

#endif // HPP_ALIB_STRINGS_FORMAT_SIMPLETEXT
