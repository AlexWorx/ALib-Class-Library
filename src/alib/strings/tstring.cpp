// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
    #include "alib/strings/substring.hpp"
#endif


#if !defined (_GLIBCXX_CSTRING)  && !defined(_CSTRING_)
    #include <cstring>
#endif
#if !defined (_STRING_H)         && !defined(_INC_STRING)
    #include <string.h>
#endif
#if !defined (_GLIBCXX_CSTDLIB)  && !defined(_CSTDLIB_)
    #include <cstdlib>
#endif
#if !defined (_GLIBCXX_IOSTREAM ) && !defined(_IOSTREAM_)
    #include <iostream>
#endif


namespace aworx { namespace lib { namespace strings {


// *************************************************************************************************
// TString::_dbgCheck()
// *************************************************************************************************
//! @cond NO_DOX
#if ALIB_DEBUG_STRINGS

#if !ALIB_DEBUG
    #pragma message "Compiler symbol ALIB_DEBUG_STRINGS_ON set, while ALIB_DEBUG is off. Is this really wanted?"
#endif

template<typename TChar>
void TStringBase<TChar>::_dbgCheck() const
{
    StringBase<TChar>::_dbgCheck();

    ALIB_ASSERT_ERROR(  StringBase<TChar>::buffer == nullptr
                        ||  debugIsTerminated <= 0
                        ||  StringBase<TChar>::buffer[StringBase<TChar>::length] == '\0'
                        ,ASTR("Terminated but terminator char '\\0' not present")         );
}

template   void TStringBase<nchar>::_dbgCheck() const;
template   void TStringBase<wchar>::_dbgCheck() const;

#endif
//! @endcond


}}}// namespace [aworx::lib::strings]
