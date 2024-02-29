/** ************************************************************************************************
 * \file
 * This header file is part of sub-namespace #alib::lang::format of module \alib_basecamp of
 * the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_FORMAT_FWDS
#define HPP_ALIB_LANG_FORMAT_FWDS 1

#if !defined (HPP_ALIB_CAMP_MESSAGE_EXCEPTION)
    #include "alib/lang/message/exception.hpp"
#endif

ALIB_ASSERT_MODULE(CAMP)

namespace alib { namespace lang { namespace format {

    /** ************************************************************************************************
     * This interface class exposes interface method \c Invoke which writes the content of the
     * box to the given \b %AString object in accordance with \c formatSpec.
     *
     * \see
     *   For more information about this class see chapter
     *   \ref alib_basecamp_format_custom_types "4.3. Formatting Custom Types" of the
     *   \ref alib_basecamp_format "Programmer's Manual" of module \alib_basecamp_nl.
     **************************************************************************************************/
    struct FFormat
    {
        /**
         * Signature of the invokable function.<br>
         * Implementations write the content of \p{box} to the given \b %AString object \p{target} in
         * accordance to the type-specific format specification \p{formatSpec}.
         *
         * @param self       The box that the function was invoked on.
         * @param formatSpec The specification of the format (type specific). If empty, a default
         *                   specification string might have to be chosen.
         * @param nf         A copy of the number format of the formatter (allowed to be modified).
         * @param target     The AString object receiving the formatted string.
         */
        using Signature = void (*) ( const Box& self, const String& formatSpec, NumberFormat& nf,
                                     AString& target );
    };



    // #################################################################################################
    // Forwards
    // #################################################################################################

    class Formatter;
    class FormatterPythonStyle;
    class FormatterJavaStyle;
    class Paragraphs;

} // namespace alib::lang[::format]

namespace basecamp { class BaseCamp; }

} // namespace alib[:lang]


// #################################################################################################
// Alias types in namespace #alib.
// #################################################################################################
/// Type alias in namespace \b alib.
using  Formatter           =   lang::format::Formatter;

/// Type alias in namespace \b alib.
using  FormatterPythonStyle=   lang::format::FormatterPythonStyle;

/// Type alias in namespace \b alib.
using  FormatterJavaStyle  =   lang::format::FormatterJavaStyle;

/// Type alias in namespace \b alib.
using  Paragraphs          =   lang::format::Paragraphs;

/**
 * Shared pointer to instances of \alib{lang::format,Formatter,standard formatters}.
 *
 * \see Method \alib{lang::format,Formatter::GetDefault}.
 */
using   SPFormatter        =  std::shared_ptr< lang::format::Formatter>;

} // namespace [alib]


#endif // HPP_ALIB_LANG_FORMAT_FWDS
