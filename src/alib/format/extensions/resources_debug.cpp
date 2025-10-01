// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/containers/containers.prepro.hpp"
#include "alib/resources/resources.prepro.hpp"
#if ALIB_DEBUG_RESOURCES && ALIB_FORMAT
#   include <vector>
#   include <algorithm>
#endif
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Format;
    import   ALib.Strings.Tokenizer;
#   if ALIB_DEBUG_CONTAINERS
      import ALib.Containers.HashTable;
#   endif
#   if ALIB_DEBUG_RESOURCES && ALIB_FORMAT
      import ALib.Resources;
      import ALib.Format;
#   endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Time.H"
#   include "ALib.Strings.Tokenizer.H"
#   if ALIB_DEBUG_CONTAINERS
#     include "ALib.Containers.HashTable.H"
#   endif
#   if ALIB_DEBUG_RESOURCES && ALIB_FORMAT
#       include "ALib.Resources.H"
#       include "ALib.Format.H"
#   endif
#endif

#if ALIB_DEBUG_RESOURCES && ALIB_FORMAT
// ======================================   Implementation   =======================================
#   include "ALib.Lang.CIFunctions.H"
namespace alib::resources {

#   include "ALib.Lang.CIFunctions.H"
AString DbgDump( std::vector<std::tuple<NString, NString, String, integer>>& list,
                 const NString& catFilter, const String& format  )
{
    AString result;
    NString actCategory( nullptr );
    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    auto& formatter= Formatter::Default;
    for( auto& it : list )
    {
        if( catFilter.IsNotEmpty() )
        {
            TokenizerN cats( catFilter, ',');
            bool found= false;
            while( cats.HasNext() )
               found|= cats.Next().Trim().Equals<CHK, lang::Case::Ignore>(std::get<0>(it) );
            if( !found )
                continue;
        }

        if( actCategory != std::get<0>(it) )
        {
            actCategory=   std::get<0>(it);
            result << NEW_LINE
                   << '[' << actCategory << ']' << NEW_LINE;
        }

        formatter->Format( result, format, std::get<0>(it), std::get<1>(it),
                                           std::get<2>(it), std::get<3>(it)     );
    }

    return result;
}
#   include "ALib.Lang.CIMethods.H"
} // namespace [alib::resources]
#endif // ALIB_DEBUG_RESOURCES && ALIB_FORMAT

