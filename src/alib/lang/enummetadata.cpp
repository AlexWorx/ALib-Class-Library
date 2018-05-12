// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if !defined (HPP_ALIB_LANG_RESOURCE_TUPLE_LOADER)
    #include "alib/lang/resourcedtupleloader.hpp"
#endif

namespace aworx { namespace lib { namespace lang {

namespace detail {

void   loadStandardEnumTable( int                          tupleType,
                              void*                        table,
                              aworx::lib::lang::Library&   library,
                              const aworx::String&         name          )
{
    using TTupleNamed=     std::tuple< int, aworx::String>;
    using TTupleParsable=  std::tuple< int, aworx::String, int>;

    if( tupleType == 2 )
        ResourcedTupleLoader::LoadTable( *reinterpret_cast< std::vector<TTupleNamed>*    >(table),
                                         library, name                                            );
    else
        ResourcedTupleLoader::LoadTable( *reinterpret_cast< std::vector<TTupleParsable>* >(table),
                                         library, name                                            );
}

}  // namespace aworx::lib::lang[::detail]


}}}// namespace [aworx::lib::lang]





