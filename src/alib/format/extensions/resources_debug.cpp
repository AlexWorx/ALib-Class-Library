#if ALIB_DEBUG_RESOURCES && ALIB_FORMAT

#   include "ALib.Lang.CIFunctions.H"
namespace alib::resources {

#   include "ALib.Lang.CIFunctions.H"
AString DbgDump( std::vector<std::tuple<NString, NString, String, integer>>& list,
                 const NString& catFilter, const String& format  ) {
    AString result;
    NString actCategory( nullptr );
    ALIB_LOCK_RECURSIVE_WITH(Formatter::DEFAULT_LOCK)
    auto& formatter= Formatter::DEFAULT;
    for( auto& it : list ) {
        if( catFilter.IsNotEmpty() ) {
            TokenizerN cats( catFilter, ',');
            bool found= false;
            while( cats.HasNext() )
               found|= cats.Next().Trim().Equals<CHK, lang::Case::Ignore>(std::get<0>(it) );
            if( !found )
                continue;
        }

        if( actCategory != std::get<0>(it) ) {
            actCategory=   std::get<0>(it);
            result << NEW_LINE
                   << '[' << actCategory << ']' << NEW_LINE;
        }

        formatter->Format( result, format, std::get<0>(it), std::get<1>(it),
                                           std::get<2>(it), std::get<3>(it)     );
    }

    return result;
}
} // namespace [alib::resources]
#   include "ALib.Lang.CIMethods.H"
#endif // ALIB_DEBUG_RESOURCES
