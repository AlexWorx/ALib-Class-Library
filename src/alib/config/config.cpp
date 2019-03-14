// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_CONFIG_CONFIG)
#   include "alib/config/config.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCE_TUPLE_LOADER)
#   include "alib/resources/resourcedtupleloader.hpp"
#endif

#if !defined (HPP_ALIB_LIB_ALIBMODULES)
#    include "alib/lib/alibmodules.hpp"
#endif

#if !defined (HPP_ALIB_SYSTEM_SYSTEM)
#    include "alib/system/system.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#    include "alib/resources/resources.hpp"
#endif



//! @cond NO_DOX
// we do not use macro ALIB_ENUM_SPECIFICATION_IMPL( aworx::lib::config::VariableDecl )
// because we need to provide a different delimiter
void aworx::lib::resources::T_EnumMetaDataSpecification<aworx::lib::config::VariableDecl>
                          ::LoadTable(void* t, Resources& r, const NString& c, const NString& n )
{
    ResourcedTupleLoader::LoadTable( *reinterpret_cast< std::vector<TTuple>*>(t), r, c, n, '|' );
}
//! @endcond

ALIB_BOXING_VTABLE_DEFINE( aworx::lib::config::Priorities, vt_config_priorities )
ALIB_BOXING_VTABLE_DEFINE( aworx::lib::config::Exceptions, vt_config_exceptions )

using namespace aworx;
using namespace aworx::lib::config;

namespace aworx { namespace lib {

Config CONFIG;

namespace config  {

// anonymous method used to load meta data of enum Priorities
//! @cond NO_DOX
namespace {  void loadPriorityEnum()
{
    auto& prioritiesEnum= EnumMetaData<Priorities>::GetSingleton();
    prioritiesEnum.Table.clear();
    prioritiesEnum.Map.clear();

    resources::ResourcedTupleLoader::LoadTable( prioritiesEnum.Table,
                                                *CONFIG.Resources,
                                                 CONFIG.ResourceCategory,
                                                "Priorities" );
    prioritiesEnum.CheckMap(true);
}} // anonymous namespace
//! @endcond


Config::Config()
: Module( ALIB_VERSION, ALIB_REVISION, "ALIB_CONFIG" )
{
    ALIB_ASSERT_ERROR( this == &CONFIG,
        "Instances of class Config must not be created. Use singleton aworx::lib::CONFIG" )

    Dependencies.emplace_back( &lib::SYSTEM );
}


void Config::init( InitLevels level, int, const char**, const wchar_t** )
{
    if( level == InitLevels::PrepareResources )
    {
        ALIB.CheckDistribution();

        ALIB_BOXING_VTABLE_REGISTER( vt_config_priorities )
        ALIB_BOXING_VTABLE_REGISTER( vt_config_exceptions )
        ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( config::Exceptions     )
        ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( aworx::lib::config::Priorities )


        Resources->AddBulk( ResourceCategory,

        "ExceptionsPrefix" , A_CHAR("config::"),
        "ExceptionsPostfix", A_CHAR(""        ),
        "Exceptions"       , A_CHAR("0"  ","    "OK"                ","  ""     ","
                                        "1"  ","    "ErrorOpeningFile"  ","  "EOF"  ","
                                        "2"  ","    "ErrorWritingFile"  ","  "EWF"       ),

         "EOF", A_CHAR("Can't open {}file '{}' for reading.") ,
         "EWF", A_CHAR("Can't open {}file '{}' for writing.") ,


     "Priorities",
     A_CHAR("0"     ","   "NONE"                   ",1"      ","
          "500"     ","   "AutoDetected"           ",1"      ","
          "10000"   ","   "DefaultValues"          ",1"      ","
          "20000"   ","   "Standard"               ",1"      ","
          "30000"   ","   "Environment"            ",1"      ","
          "40000"   ","   "CLI"                    ",1"      ","
          "-1"      ","   "ProtectedValues"        ",1"           ),


        // names of the configuration plug-ins added to configuration in default construction.
        "CfgPlgDef"  , A_CHAR("Internal Default Variables") ,
        "CfgPlgPro"  , A_CHAR("Protected Variables")        ,
        "CfgPlgEnv"  , A_CHAR("Environment Variables")      ,
        "CfgPlgCLI"  , A_CHAR("Command Line Arguments")     ,

        // end of AddBulk()
        nullptr );


        // load plugin slots resources already now, so that plug-ins may be added to resource
        // object. It will be reloaded in next phase. Hence, a very special treatment here.
        loadPriorityEnum();
    }

    else if( level == InitLevels::PrepareConfig )
    {
        // reload resources of enumeration "Priority", in case it was changed after resource phase
        loadPriorityEnum();
    }


}

void Config::terminationCleanUp()
{
}

}}} // namespace [aworx::lib::config]




