// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_ENUM_META_DATA)
#   include "alib/resources/enummetadata.hpp"
#endif


#if !defined (HPP_ALIB_RESOURCES_TUPLE_LOADER)
#   include "alib/resources/resourcedtupleloader.hpp"
#endif



namespace aworx { namespace lib { namespace resources { namespace detail {

void   loadStandardEnumTable( int             tupleType,
                              void*           table,
                              Resources*      resources,
                              const NString&  category,
                              const NString&  name          )
{
    using TTupleNamed=     std::tuple< int, String>;
    using TTupleParsable=  std::tuple< int, String, int>;

    if( tupleType == 2 )
        ResourcedTupleLoader::LoadTable( *reinterpret_cast< std::vector<TTupleNamed   >*>(table),
                                         *resources, category, name                                );
    else
        ResourcedTupleLoader::LoadTable( *reinterpret_cast< std::vector<TTupleParsable>*>(table),
                                         *resources, category, name                                );
}


}}}} // namespace [aworx::lib::resources::detail]





