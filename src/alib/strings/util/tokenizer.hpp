// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include guard
#ifndef HPP_ALIB_STRINGS_UTIL_TOKENIZER
#define HPP_ALIB_STRINGS_UTIL_TOKENIZER 1

// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
    #include "alib/strings/substring.hpp"
#endif

namespace aworx { namespace lib { namespace strings {

/**
 * This sub-namespace  provides some utility classes which are related
 * to string classes found in namespace \ref aworx::lib::strings.
 */
namespace util  {


/** ************************************************************************************************
 * This class operates on strings which contains data separated by a delimiter character.
 * It identifies the substrings between the delimiters as \e tokens of type
 * \alib{strings,SubstringBase,Substring}. After an instance of this class is constructed,
 * three methods are available:
 * - #HasNext: Indicates if there are further tokens available.
 * - #Next: Sets field #Actual (which is of type Substring) to reference the next token and
 *   returns it.<br>
 *   With each call to %Next, a different delimiter can be provided, which then serves as the
 *   delimiter for this and subsequent tokens.<br>
 *   The returned token by default will be trimmed according to the current trimable characters.
 * - #Rest:
 *   Like #Next, however returns the complete remaining region without searching for
 *   further delimiters (and tokens).<br>
 *   After this method was invoked, #HasNext() will return \c false.
 *
 * After a token was retrieved, it might be modified using the interface of class
 * \alib{strings,SubstringBase,Substring} as the tokenizer does not rely on the bounds of
 * the current token when receiving the next. Furthermore, even field #Rest is allowed
 * to be changed using the interface of \b %Substring if it seems appropriate. The effect is the
 * same as if method #Set was invoked to apply a different source string.
 *
 * Objects of this class can be reused by freshly initializing them using method #Set.
 *
 * <b>Sample code</b>:<br>
 * The following code sample shows how to tokenize a string:
 *
 * \snippet "DOX_ALIB_TOKENIZER.cpp"     DOX_ALIB_TOKENIZER
 *
 * The output will be:
 *
 * \snippet "DOX_ALIB_TOKENIZER.txt"     OUTPUT
 *
 **************************************************************************************************/
class Tokenizer
{
    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**
         *  A \alib{strings,SubstringBase,Substring} that represents the part of
         *  the underlying data that has not been tokenized, yet.
         *  It is allowed to manipulate this public field, which has a similar effect as
         *  using method #Set.<br>
         */
        Substring       Rest;

        /**
         *  The actual token, which is returned with every invocation of #Next() or #Rest().
         *  It is allowed to manipulate this field any time.<br>
         */
        Substring       Actual;

        /**
         * The white spaces characters used to trim the tokens.
         * Defaults to  \ref aworx::DefaultWhitespaces
         */
        String16        Whitespaces                                            = DefaultWhitespaces;


    // #############################################################################################
    // Internal fields
    // #############################################################################################
    protected:
        /// The most recently set delimiter used by default for the next token extraction.
        character           delim;

        /// If \c true, empty tokens are omitted.
        bool            skipEmpty;


    // #############################################################################################
    //  Constructors/Destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructs an empty tokenizer. To initialize, method #Set needs to be invoked.
         ******************************************************************************************/
        Tokenizer()
        {}

        /** ****************************************************************************************
         * Constructs a tokenizer to work on a given string.
         *
         * @param  src             The string to be tokenized.
         * @param  delimiter       The delimiter that separates the tokens. Can be changed with
         *                         every next token.
         * @param  skipEmptyTokens If \c true, empty tokens are omitted.
         *                         Optional and defaults to \c false.
         ******************************************************************************************/
        Tokenizer( const String& src, character delimiter, bool skipEmptyTokens= false )
        : Rest(src)
        , delim(delimiter)
        , skipEmpty(skipEmptyTokens)
        {}

    // #############################################################################################
    //  Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Resets a tokenizer to work on a given string.
         *
         * @param  src             The string to be tokenized
         * @param  delimiter       The delimiter that separates the tokens. Can be changed with
         *                         every next token.
         * @param  skipEmptyTokens If \c true, empty tokens are omitted.
         *                         Optional and defaults to \c false.
         ******************************************************************************************/
        void Set( const String& src, character delimiter, bool skipEmptyTokens= false )
        {
            Actual= nullptr;
            Rest= src;
            this->delim=            delimiter;
            this->skipEmpty=  skipEmptyTokens;
        }

        /** ****************************************************************************************
         * Returns the next token, which is afterwards also available through field #Actual.
         * If no further token  was available, the returned
         * \alib{strings,SubstringBase,Substring} will be \e nulled.
         * (see \alib{strings,StringBase::IsNull,String::IsNull}).
         * To prevent this, the availability of a next token should be
         * checked using method #HasNext().
         *
         * For clarification, see the explanation and sample code in this classes documentation.
         *
         *  @param trimming  Determines if the token is trimmed in respect to the white space
         *                   characters defined in field #Whitespaces.
         *                   Defaults to \b Whitespaces.Trim.
         *  @param newDelim  The delimiter separates the tokens. Defaults to 0, which keeps the
         *                   current delimiter intact.
         *                   A new delimiter can be provided for every next token.
         * @return \c true if a next token was available, \c false if not.
         ******************************************************************************************/
        ALIB_API
        Substring&  Next( lang::Whitespaces trimming= lang::Whitespaces::Trim, character newDelim= '\0' );

        /** ****************************************************************************************
         * Returns the currently remaining string (without searching for further delimiter
         * characters).
         * After this call #HasNext will return \c false and #Next will return a \e nulled Substring.
         *  @param trimming  Determines if the token is trimmed in respect to the white space
         *                   characters defined in field #Whitespaces.
         *                   Defaults to \b Whitespaces.Trim.
         * @return The rest of the original source string, which was not returned by #Next(), yet.
         ******************************************************************************************/
        Substring&  GetRest( lang::Whitespaces trimming= lang::Whitespaces::Trim )
        {
            // set start, end and end of tokenizer
            Actual=  Rest;
            Rest  =  nullptr;
            if ( trimming == lang::Whitespaces::Trim )
                Actual.Trim( Whitespaces );
            return Actual;
        }

        /** ****************************************************************************************
         * If this returns \c true, a call to #Next will be successful and will return a
         * Substring which is not \e nulled.
         * @return \c true if a next token is available.
         ******************************************************************************************/
        bool        HasNext()       { return Rest.IsNotNull() && ( !skipEmpty || Rest.IsNotEmpty() ); }

}; // class Tokenizer

}}} // namespace aworx[::lib::strings::util]

/// Type alias in namespace #aworx.
using     Tokenizer=     aworx::lib::strings::util::Tokenizer;

}  // namespace aworx

#endif // HPP_ALIB_STRINGS_UTIL_TOKENIZER
