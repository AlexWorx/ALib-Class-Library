// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGFORMAT_FWDS
#define HPP_ALIB_STRINGFORMAT_FWDS 1

#if !defined(HPP_ALIB_CHARACTERS_CHARACTERS)
#   include "alib/characters/characters.hpp"
#endif

ALIB_ASSERT_MODULE(STRINGFORMAT)
ALIB_ASSERT_FILESET(MODULES)

#if !defined (_GLIBCXX_MEMORY) && !defined(_MEMORY_)
#   include <memory>
#endif

// #################################################################################################
// Forwards
// #################################################################################################
namespace aworx { namespace lib { namespace stringformat {

   class Formatter;
   class FormatterPythonStyle;
   class FormatterJavaStyle;
   class Text;

}} // namespace aworx[::lib::strings]


// #################################################################################################
// Alias types in namespace #aworx.
// #################################################################################################
/// Type alias in namespace #aworx.
using  Formatter        =     aworx::lib::stringformat::Formatter;

/// Type alias in namespace #aworx.
using  FormatterPythonStyle=  aworx::lib::stringformat::FormatterPythonStyle;

/// Type alias in namespace #aworx.
using  FormatterJavaStyle=    aworx::lib::stringformat::FormatterJavaStyle;

/// Type alias in namespace #aworx.
using   Text            =     aworx::lib::stringformat::Text;

/**
 * Shared pointer to instances of \alib{stringformat,Formatter,standard formatters}.
 *
 * \see Namespace function \aworx{GetDefaultFormatter}.
 */
using   SPFormatter  =    std::shared_ptr<lib::stringformat::Formatter>;

} // namespace [aworx]


#endif // HPP_ALIB_STRINGFORMAT_FWDS

