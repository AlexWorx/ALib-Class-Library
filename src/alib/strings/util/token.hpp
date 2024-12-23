//==================================================================================================
/// \file
/// This header file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_STRINGS_UTIL_TOKEN
#define HPP_ALIB_STRINGS_UTIL_TOKEN 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
#include "alib/strings/substring.hpp"
#include "alib/strings/localstring.hpp"
#if ALIB_CAMP
#   include "alib/lang/basecamp/camp.hpp"
#endif
#if ALIB_BOXING
#   include "alib/boxing/boxing.hpp"
#endif
#if ALIB_ENUMS
#   include "alib/enums/bitwise.hpp"
#endif


namespace alib {  namespace strings::util  {


//==================================================================================================
/// Tokens in the context of \alib_strings_nl, are human-readable "words" or "symbols" that
/// represent a certain value or entity of a software. Tokens may be used with configuration files,
/// mathematical or general expressions, programming languages, communication protocols and so forth.
///
/// This struct contains attributes to describe a token, a method to parse the attributes from a
/// (resource) string and finally method #Match that matches a given string against the token
/// definition.
///
/// ## %Token Format: ##
/// With the construction, respectively the \ref Define "definition" of a token, special formats are
/// detected. These formats are:
/// - <em>"snake_case"</em><br>
/// - <em>"kebab-case"</em><br>
/// - <em>"CamelCase"</em><br>
///
/// \note
///   Information about such case formats is given in this
///   \https{Wikipedia article,en.wikipedia.org/wiki/Letter_case#Special_case_styles}.
///
/// \note
///   If the name indicates a mix of \e snake_case, \e kebab-case or \e CamelCase formats
///   (e.g., \e "System_Propery-ValueTable"), then snake_case supersedes both others and kebab-case
///   supersedes CamelCase.
///
/// The format detection is only performed when more than one minimum length is given. In this case,
/// the number of "segments" (e.g., "camel humps") has to match the number of length values.
///
///
/// ## Character Case Sensitivity: ##
/// Independent of the token format (normal or snake_case, kebab-case, CamelCase), character case
/// sensitivity can be chosen. With \e CamelCase and case-sensitive parsing, the first character of
/// the first hump may be defined lower or upper case (called "lowerCamelCase" vs. "UpperCamelCase").
///
/// If none of the special formats is detected, the tokens can optionally be abbreviated by just
/// providing a minimum amount of starting characters as specified by the then single entry
/// in #minLengths.
/// Otherwise, each segment of the token (e.g., "camel hump") can (again optionally) be shortened
/// on its own.
/// As an example, if for token <c>"SystemProperty"</c> the minimum lengths given are
/// \c 3 and \c 4, the minimum abbreviation is <c>"SysProp"</c>, while <c>"SystProper"</c> also
/// matches.<br>
///
///
/// ## Limitation To Seven Segments: ##
/// This class supports minimum length definitions for up to \c 7 "camel humps", respectively
/// segments. Should a name contain even more segments, those cannot be abbreviated.
/// Providing more than \c 7 values for minimum segment lengths with the definition string results
/// in a definition error (see below).
///
///
/// ## Special Treatment For CamelCase: ##
/// ### Omitable Last Camel Hump: ####
/// The minimum length values provided must be greater than \c 0, with one exception:
/// With \e CamelCase format and case-insensitive definition, the last "camel hump" may have a
/// minimum length of \c 0 and hence may be omitted when matched.
/// If so, the "normalized" version of the token, which can be received by
/// \alib{strings;T_Append;appending} an instance to an \alib{strings;TAString;AString}, will have
/// the last letter of the defined name converted to lower case.<br>
/// The rationale for this specific approach is to support the English plural case. This can be best
/// explained in a sample. If a token was defined using definition string:
///
///      "MilliSecondS Ignore 1 1 0"
///
/// then all of the following words match:
///
///      milliseconds
///      MilliSecs
///      millis
///      MSec
///      MSecs
///      MSs
///      ms
///
/// In the case that the rightfully (normalized) spelled token name is to be written, then with
/// the last character converted to lower case, the token becomes
///
///      MilliSeconds
///
/// This is performed with methods #GetExportName (which is also used by the specialization of
/// functor \alib{strings;T_Append} for this type.
/// Hence, when appending a \b Token to an \b AString, if omitable, the last character
/// of the token name is converted to lower case.
///
/// If the above is not suitable, or for any other reasons a different "normalized" name is wanted
/// when writing the token, then method #Define offers a next mechanism to explicitly define
/// any custom string to be written.
///
/// ### Rollback: ####
/// \e CamelCase supports a simple "rollback" mechanism, which is needed for example for token
///
///      "SystemTemperature Ignore 1 1 0"
///
/// and given match argument
///
///      system
///
/// All six characters are matching the first hump, but then there are not characters left to
/// match the start of the second hump \c "Temperature". In this case, a loop of retries is
/// performed by rolling back characters from the back of the hump (\c 'm') and ending with the
/// first optional character of that hump (\c 'y'). The loop will be broken when
/// character \c 't' is found.
///
/// However: This is not continued in the case that the term that was rolled back does not match,
/// yet. This means, that certain (very unlikely!) tokens, with nested repeating character sequences
/// in camel humps, cannot be abbreviated to certain (unlikely wanted) lengths.
///
/// ## Handling Definition Errors: ###
///
/// The definition strings passed to method #Define are considered static (resourced) data.
/// In other words, this definition data should be compile-time defined and not be customizable
/// by end-users, but only by experts.
/// Therefore, only in debug-compilations of the library, a due testing of correctness of the
/// definitions is available.
///
/// The source code of static utility method #LoadResourcedTokens demonstrates how error
/// codes defined with enumeration #DbgDefinitionError can be handled in debug-compilations
/// by raising debug-assertions.
//==================================================================================================
class Token
{
    public:
        /// Format types detected with #detectFormat.
        enum class Formats : int8_t
        {
            Normal        = 0,  ///< Normal, optionally abbreviated words.
            SnakeCase     = 2,  ///< snake_case using underscores.
            KebabCase     = 4,  ///< kebab-case using hyphens.
            CamelCase     = 8,  ///< UpperCamelCase or lowerCamelCase.
        };

#if ALIB_DEBUG
        /// Error codes which which are written in field #format in the case that method #Define
        /// suffers a parsing error.<br>
        /// This enum, as well as the error detection is only available in debug-compilations
        /// of the library.
        enum class DbgDefinitionError : int8_t
        {
            OK                              =   0, ///< All is fine.
            EmptyName                       = - 1, ///< No token name found.
            ErrorReadingSensitivity         = - 2, ///< Sensitivity value not found.
            ErrorReadingMinLengths          = - 3, ///< Error parsing the list of minimum lengths.
            TooManyMinLengthsGiven          = - 4, ///< A maximum of \c 7 minimum length values was exceeded.
            InconsistentMinLengths          = - 5, ///< The number of given minimum length values is greater than \c 1
                                                   ///< but does not match the number of segments in the identifier.
            NoCaseSchemeFound               = - 6, ///< More than one minimum length value was given but no
                                                   ///< segmentation scheme could be detected.
            MinLenExceedsSegmentLength      = - 7, ///< A minimum length is specified to be higher than the token
                                                   ///< name, respectively the according segment name.
            DefinitionStringNotConsumed     = - 8, ///< The definition string was not completely consumed.
            ZeroMinLengthAndNotLastCamelHump= - 9, ///< A minimum length of \c 0 was specified for a segment that is not
                                                   ///< a last camel case hump.
        };
#endif
    protected:

        /// The tokens' definition string part.
        String      definitionName;

        /// The tokens' optional explicit export name.
        String      exportName                  = NULL_STRING;


        /// Defines the "case type" as well as the letter case sensitivity of this token.
        Formats     format;

        /// The minimum abbreviation length per segment. If only one is given (second is \c -1), then
        /// field #Format indicates normal tokens.
        /// Otherwise, the token is either snake_case, kebab-case or CamelCase.
        int8_t      minLengths[7]               = {0,0,0,0,0,0,0};


        /// Letter case sensitivity. This is combined with the format bits.
        static constexpr Formats   ignoreCase    = Formats(1);


    // #############################################################################################
    //  Constructors
    // #############################################################################################
    public:
        /// Parameterless constructor. Creates an "undefined" token.
        Token()
        : format(Formats( ALIB_REL_DBG( Formats::Normal, DbgDefinitionError::EmptyName ) ))       {}

        //==========================================================================================
        /// Constructor used with function names that do not contain snake_case, kebab-case or
        /// CamelCase name scheme.
        /// \note Of course, the name may follow such scheme. With this constructor, it just will not
        ///       be detected.
        /// @param name           The function name.
        /// @param sensitivity    The letter case sensitivity of reading the function name.
        /// @param minLength      The minimum starting portion of the function name to read..
        /// @param exportName     An optional export name. If \b not given, the \p{name} is
        ///                       used with method #GetExportName.
        //==========================================================================================
        ALIB_API
        Token(const String& name, lang::Case sensitivity, int8_t minLength,
              const String& exportName= NULL_STRING );


        //==========================================================================================
        /// Constructor with at least two minimum length values, used to define tokens that follow
        /// snake_case, kebab-case or CamelCase naming schemes.
        ///
        /// @param name           The function name.
        /// @param sensitivity    The letter case sensitivity of reading the function name.
        /// @param minLength1     The minimum starting portion of the first segment to read.
        /// @param minLength2     The minimum starting portion of the second segment to read.
        /// @param minLength3     The minimum starting portion of the third segment to read.
        ///                       Defaults to \c 1.
        /// @param minLength4     The minimum starting portion of the fourth segment to read.
        ///                       Defaults to \c 1.
        /// @param minLength5     The minimum starting portion of the fifth segment to read.
        ///                       Defaults to \c 1.
        /// @param minLength6     The minimum starting portion of the sixth segment to read.
        ///                       Defaults to \c 1.
        /// @param minLength7     The minimum starting portion of the seventh segment to read.
        ///                       Defaults to \c 1.
        //==========================================================================================
        ALIB_API
        Token( const String& name, lang::Case sensitivity, int8_t minLength1, int8_t minLength2,
               int8_t minLength3= -1, int8_t minLength4= -1, int8_t minLength5= -1,
               int8_t minLength6= -1, int8_t minLength7= -1                                 );

#if ALIB_ENUMS
        //==========================================================================================
        /// Constructor using a (usually resourced) string to read the definitions.
        /// Invokes #Define.
        ///
        /// @param definitionSrc  The input string.
        /// @param separator      Separation character used to parse the input.
        ///                       Defaults to <c>';'</c>.
        ///
        /// \par Module Dependencies
        ///   This method is only available if module \alib_enums is included in the \alibdist.
        //==========================================================================================
        Token( const String& definitionSrc, character separator = ';'  )
        { Define( definitionSrc, separator ); }
#endif

    // #############################################################################################
    //  Interface
    // #############################################################################################
    public:
        #if ALIB_DEBUG
            //======================================================================================
            /// Tests if this token was well defined.
            ///
            /// \note
            ///   This method is only available in debug-compilations.
            ///   Definition strings are considered static data (preferably resourced).
            ///   Therefore, in debug-compilations, this method should be invoked and with that,
            ///   the consistency of the resources be tested. In the case of failure, a debug
            ///   assertion should be raised.
            ///
            /// @return \alib{strings::util::Token;DbgDefinitionError::OK}, if this token is well
            ///         defined, a different error code otherwise.
            //======================================================================================
            DbgDefinitionError DbgGetError()
            {
                return  int(format) >= 0 ?  DbgDefinitionError::OK
                                         :  DbgDefinitionError(format);
            }
        #endif

        //==========================================================================================
        /// Returns the definition name used for parsing the token.
        ///
        /// \note
        ///   To receive the "normalized" name of this token, method #GetExportName can be used, or
        ///   a token can simply be \ref alib_strings_assembly_ttostring "appended" to an
        ///   instance of type \alib{strings;TAString;AString}.
        ///
        /// @return This token's #definitionName.
        //==========================================================================================
        const String&   GetDefinitionName()                                                    const
        {
            ALIB_ASSERT_ERROR( int8_t(format) >= 0, "STRINGS/TOK"
                               "Error in token definition. Use DbgGetError in debug-compilations!" )
            return definitionName;
        }

        //==========================================================================================
        /// If field #exportName is not \e nulled (hence explicitly given with resourced definition
        /// string or with a constructor), this is appended.
        ///
        /// Otherwise appends the result of \alib{strings::util;Token::GetDefinitionName} to
        /// the \p{target}. If the token is defined \e CamelCase and the minimum length of the last
        /// segment is defined \c 0, then the last character written is converted to lower case.
        ///
        /// As a result, in most cases it is \b not necessary to provide a specific #exportName
        /// with the definition. Instead, this method should provide a reasonable output.
        ///
        /// \see Documentation section <b>Omitable Last Camel Hump</b> of this classes'
        ///      \alib{strings::util;Token;documentation}, for more information about why the
        ///      character conversion to lower case might be performed.
        ///
        /// @param target The \b AString that method \b Append was invoked on.
        //==========================================================================================
        ALIB_API
        void            GetExportName(AString& target)                                        const;

        //==========================================================================================
        /// Returns the format of this token.
        ///
        /// \note Same as methods #Sensitivity and #GetMinLength, this method is usually not
        ///       of interest to standard API usage.
        ///       These three informational methods are rather provided to support the unit tests.
        /// @return This token's format,  used with method #Match.
        //==========================================================================================
        Formats         GetFormat()                                                            const
        {
            ALIB_ASSERT_ERROR( int8_t(format) >= 0, "STRINGS/TOK",
              "Error ", NString64(int8_t(format)), " in definition of token \"",
              NString128(definitionName), "\". Use DbgGetError in debug-compilations!"  )
            return Formats( int8_t(format) & ~int8_t(ignoreCase) );
        }

        //==========================================================================================
        /// Returns the letter case sensitivity of this token.
        ///
        /// \note Same as methods #GetFormat and #GetMinLength, this method is usually not
        ///       of interest to standard API usage.
        ///       These three informational methods are rather provided to support the unit tests.
        /// @return The letter case sensitivity used with method #Match.
        //==========================================================================================
        lang::Case            Sensitivity()                                                    const
        {             return (int(format) & 1 ) == 1 ? lang::Case::Ignore : lang::Case::Sensitive; }

        //==========================================================================================
        /// Returns the minimum length to be read. In case that this token is not of
        /// snake_case, kebab-case or CamelCase naming scheme, only \c 0 is allowed for parameter
        /// \p{idx} and this defines the minimal abbreviation length. If one of the naming schemes
        /// applies, parameter \p{idx} may be as high as the number of segments found in the
        /// name (and a maximum of \c 6, as this class supports only up to seven segments).
        ///
        /// The first index that exceeds the number of segments, will return \c -1 for the length.
        /// If even higher index values are requested, then the returned value is undefined.
        ///
        /// @param idx The index of the minimum length to receive.
        ///
        /// \note Same as methods #GetFormat and #Sensitivity, this method is usually not
        ///       of interest to standard API usage.
        ///       These three informational methods are rather provided to support the unit tests.
        ///
        /// @return The minimum length of segment number \p{idx}.
        //==========================================================================================
        int8_t          GetMinLength( int idx )                                                const
        {
            ALIB_ASSERT_ERROR( idx >= 0 && idx <= 6 , "STRINGS/TOK",
                "Index ", NString64() << idx, " out of range."  )

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            return (idx >= 0 && idx <= 6) ? minLengths[idx] : -1;
            ALIB_WARNINGS_RESTORE
        }

    #if ALIB_ENUMS
        //==========================================================================================
        /// Defines or redefines this token by parsing the attributes from the given substring.
        /// This method is usually invoked by code that loads tokens and other data from
        /// \alib{lang::resources;ResourcePool;resources} of \alib {lang;Camp} objects.
        ///
        /// The expected format is defined as a list of the following values, separated by
        /// the character given with parameter \p{separator}:
        /// - The #definitionName of the token. Even if letter case is ignored, this should contain
        ///   the name in "normalized" format, as it may be used with #GetExportName, if no specific
        ///   name to export is given.
        /// - Letter case sensitivity. This can be "Sensitive" or "Ignore"
        ///   (respectively what is defined with resourced
        ///   \ref alib_enums_records "ALib Enum Records" of type \alib{lang::Case}),
        ///   can be abbreviated to just one character (i.e., <c>'s'</c> and
        ///   <c>'i'</c>) and itself is not parsed taking letter case into account.
        /// - Optionally the standard export string used with method #GetExportName and when
        ///   appended to an \b AString. Output names defined with this function must not start
        ///   with a digit, because a digit in this position of \p{definition}, indicates that
        ///   no export name is given.
        /// - The list of minimum length for each segment of the name. The number of values have
        ///   to match the number of segments. A value of \c 0 specifies that no abbreviation
        ///   must be done and therefore is the same as specifying the exact length of the segment.
        ///
        /// \note The given \p{definition} string has to survive the use of the token, which
        ///       is naturally true if the string resides in resources. (String contents is not
        ///       copied. Instead, this class later refers to substrings of given \p{definition}.)
        ///
        /// @param definition  The input string.
        /// @param separator   Separation character used to parse the input.
        ///                    Defaults to <c>';'</c>.
        ///
        /// \par Module Dependencies
        ///   This method is only available if module \alib_enums is included in the \alibdist.
        //==========================================================================================
        ALIB_API
        void            Define( const String& definition, character separator = ';' );
    #endif
        //==========================================================================================
        /// Matches a given string with this token. See this class's description for details.
        ///
        /// @param needle      The potentially abbreviated input string to match.
        /// @return \c true if \p{needle} matches this token, \c false otherwise.
        //==========================================================================================
        ALIB_API
        bool Match( const String& needle );

    #if ALIB_ENUMS && ALIB_CAMP
      #if DOXYGEN
        //==========================================================================================
        /// Static utility function that defines a table of token objects from external resourced
        /// strings.
        ///
        /// It is possible to provide the table lines in two ways:
        /// - In one resource string: In this case, parameter \p{outerDelim} has to specify
        ///   the delimiter that separates the records.
        /// - In an array of resource strings: If the resource string as given is not defined, this
        ///   method appends an integral index starting with \c 0 to the resource name, parses
        ///   a single record and increments the index.
        ///   Parsing ends when a resource with a next higher index is not found.
        ///
        /// The second option is recommended for larger token sets. While the separation causes
        /// some overhead in a resource backend, the external (!) management (translation,
        /// manipulation, etc.) is most probably simplified with this approach.
        ///
        /// \note
        ///   The length of the given table has to fit to the number of entries found in
        ///   the resource pool. To ensure this, with debug-builds, parameter \p{dbgSizeVerifier}
        ///   has to be provided (preferably by using macro \ref ALIB_DBG "ALIB_DBG(, N)").
        ///
        /// @param resourcePool     The resource pool to load the resource from.
        /// @param resourceCategory The resource category.
        /// @param resourceName     The resource name.
        /// @param target           The table to fill.
        /// @param dbgSizeVerifier  This parameter has to be specified only in debug-builds and
        ///                         provides the expected size of the resourced table.
        ///                         To be surrounded by macro #ALIB_DBG (not to be given in
        ///                         release-builds.)
        /// @param outerSeparator   The character that separates the entries.
        ///                         Defaults to <c>','</c>.
        /// @param innerSeparator   The character that separates the values of an entry.
        ///                         Defaults to <c>' '</c> (space).
        ///
        /// \par Module Dependencies
        ///   This method is only available if module \alib_enums as well as module \alib_basecamp
        ///   is included in the \alibdist.
        //==========================================================================================
        ALIB_API static
        void LoadResourcedTokens( lang::resources::ResourcePool&    resourcePool,
                                  const NString&                    resourceCategory,
                                  const NString&                    resourceName,
                                  strings::util::Token*             target,
                                  int                               dbgSizeVerifier,
                                  character                         outerSeparator = ',',
                                  character                         innerSeparator = ' '     );
      #else
        ALIB_API static
        void LoadResourcedTokens( lang::resources::ResourcePool&    resourcePool,
                                  const NString&                    resourceCategory,
                                  const NString&                    resourceName,
                                  strings::util::Token*             target,
                      ALIB_DBG(   int                               dbgSizeVerifier, )
                                  character                         outerSeparator = ',' ,
                                  character                         innerSeparator = ' '     );
      #endif
    #endif

    #if ALIB_CAMP
     #if DOXYGEN
        //==========================================================================================
        /// Shortcut to #LoadResourcedTokens that accepts a module and uses its resource pool
        /// and resource category.
        ///
        /// @param module          The \alibcamp to load the resource from.
        /// @param resourceName    The resource name.
        /// @param target          The table to fill.
        /// @param dbgSizeVerifier This parameter has to be specified only in debug comilations and
        ///                        provides the expected size of the resourced table.
        ///                        To be surrounded by macro #ALIB_DBG (not to be given in
        ///                        release-builds.)
        /// @param outerSeparator  The character that separates the entries.
        ///                        Defaults to <c>','</c>.
        /// @param innerSeparator  The character that separates the values of an entry.
        ///                        Defaults to <c>' '</c> (space).
        ///
        /// \par Module Dependencies
        ///   This method is only available if module \alib_basecamp is included in the \alibdist.
        //==========================================================================================
        static inline
        void LoadResourcedTokens( lang::Camp&           module,
                                  const NString&        resourceName,
                                  strings::util::Token* target,
                                  int                   dbgSizeVerifier,
                                  character             outerSeparator = ',',
                                  character             innerSeparator = ' '     );
      #else
        static
        void LoadResourcedTokens( lang::Camp&           module,
                                  const NString&        resourceName,
                                  strings::util::Token* target,
                      ALIB_DBG(   int                   dbgSizeVerifier, )
                                  character             outerSeparator = ',',
                                  character             innerSeparator = ' '     )
        {
            LoadResourcedTokens( module.GetResourcePool(), module.ResourceCategory, resourceName,
                                 target, ALIB_DBG(dbgSizeVerifier,) outerSeparator, innerSeparator );
        }
      #endif

    #endif


    protected:
        //==========================================================================================
        /// Detects snake_case, kebab-case or CamelCase.
        //==========================================================================================
        ALIB_API
        void    detectFormat();

}; // struct Token

} // namespace alib[::strings::util]

/// Type alias in namespace \b alib.
using     Token=     strings::util::Token;

} // namespace [alib]

#if ALIB_BOXING
    ALIB_BOXING_VTABLE_DECLARE( alib::strings::util::Token*, vt_alib_strings_token )
#endif

namespace alib {  namespace strings {
#if DOXYGEN
namespace APPENDABLES {
#endif
    //==============================================================================================
    /// Specialization of functor \alib{strings;T_Append} for type \alib{strings::util;Token}.
    //==============================================================================================
    template<typename TAllocator> struct T_Append<strings::util::Token, alib::character,TAllocator>
    {
        /// Appends the result of \alib{strings::util;Token::GetExportName} to the \p{target}.
        /// @param target The \b AString that method \b Append was invoked on.
        /// @param src    The \b Token to append.
        inline void operator()( strings::TAString<character,TAllocator>& target, const strings::util::Token& src )
        { src.GetExportName(target); }
    };
#if DOXYGEN
}   // namespace alib::strings[::APPENDABLES]
#endif
}} // namespace [alib::strings]

IF_ALIB_ENUMS( ALIB_ENUMS_MAKE_BITWISE( alib::strings::util::Token::Formats ) )

#endif // HPP_ALIB_STRINGS_UTIL_TOKEN

