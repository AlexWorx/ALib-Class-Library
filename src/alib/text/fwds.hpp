/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_text of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_TEXT_FWDS
#define HPP_ALIB_TEXT_FWDS 1

#if !defined (HPP_ALIB_RESULTS_EXCEPTION)
    #include "alib/results/exception.hpp"
#endif

ALIB_ASSERT_MODULE(TEXT)
ALIB_ASSERT_FILESET(MODULES)

namespace aworx { namespace lib { namespace text {



/** ************************************************************************************************
 * This interface class exposes interface method \c Invoke which writes the content of the
 * box to the given \b %AString object in accordance with \c formatSpec.
 *
 * \see
 *   For more information about this class see chapter
 *   \ref alib_text_custom_types "3. Formatting Custom Types" of the
 *   \ref alib_mod_text "Programmer's Manual" of module \alib_text_nl.
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
     * @param target     The AString object receiving the formatted string.
     */
    using Signature = void (*) ( const Box& self, const String& formatSpec, AString& target );
};



// #################################################################################################
// Forwards
// #################################################################################################

   class Text;
   class Formatter;
   class FormatterPythonStyle;
   class FormatterJavaStyle;
   class Paragraphs;

}} // namespace aworx[::lib::text]


// #################################################################################################
// Alias types in namespace #aworx.
// #################################################################################################
/// Type alias in namespace #aworx.
using  Formatter           =  lib::text::Formatter;

/// Type alias in namespace #aworx.
using  FormatterPythonStyle=  lib::text::FormatterPythonStyle;

/// Type alias in namespace #aworx.
using  FormatterJavaStyle  =  lib::text::FormatterJavaStyle;

/// Type alias in namespace #aworx.
using  Paragraphs          =  lib::text::Paragraphs;

/**
 * Shared pointer to instances of \alib{text,Formatter,standard formatters}.
 *
 * \see Method \alib{text,Formatter::GetDefault}.
 */
using   SPFormatter        =  std::shared_ptr<lib::text::Formatter>;

} // namespace [aworx]


#endif // HPP_ALIB_TEXT_FWDS

