// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_CONFIG_CONFIG)
#      include "alib/config/config.hpp"
#   endif
#   if !defined (HPP_ALIB_LANG_BASECAMP)
#      include "alib/lang/basecamp/basecamp.hpp"
#   endif
#   if !defined(HPP_ALIB_CAMP_ERRORS)
#      include "alib/lang/system/systemerrors.hpp"
#   endif
#   if !defined (HPP_ALIB_LANG_RESOURCES_RESOURCES)
#      include "alib/lang/resources/resources.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#      include "alib/enums/recordbootstrap.hpp"
#   endif
#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#endif // !defined(ALIB_DOX)



ALIB_BOXING_VTABLE_DEFINE( alib::config::Priorities, vt_config_priorities )
ALIB_BOXING_VTABLE_DEFINE( alib::config::Exceptions, vt_config_exceptions )

using namespace alib;
using namespace alib::config;

namespace alib {

config::Config CONFIG;

namespace config  {

Config::Config()
: Camp( "CFG" )
{
    ALIB_ASSERT_ERROR( this == &CONFIG, "CONFIG",
        "Instances of class Config must not be created. Use singleton  alib::CONFIG" )
}


void Config::bootstrap( BootstrapPhases phase )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_config_priorities )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_config_exceptions )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( config::Exceptions     )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::config::Priorities )


#if !ALIB_RESOURCES_OMIT_DEFAULTS
        resourcePool->BootstrapBulk( ResourceCategory,

            "E<", A_CHAR("config::"),
            "E" , A_CHAR("0"  ","    "OK"                ","  ""     ","
                         "1"  ","    "ErrorOpeningFile"  ","  "EOF"  ","
                         "2"  ","    "ErrorWritingFile"  ","  "EWF"       ),

             "EOF", A_CHAR("Can't open {}file {!Q'} for reading.") ,
             "EWF", A_CHAR("Can't open {}file {!Q'} for writing.") ,


         "Priorities",
           A_CHAR(     "0"   ","   "NONE"              ",1,"
                     "500"   ","   "AutoDetected"      ",1,"
                   "10000"   ","   "DefaultValues"     ",1,"
                   "20000"   ","   "Standard"          ",1,"
                   "30000"   ","   "Environment"       ",1,"
                   "40000"   ","   "CLI"               ",1,"
                     "max"   ","   "ProtectedValues"   ",1"      ),


            // names of the configuration plug-ins added to configuration in default construction.
            "CfgPlgDef"  , A_CHAR("Internal Default Variables") ,
            "CfgPlgPro"  , A_CHAR("Protected Variables")        ,
            "CfgPlgEnv"  , A_CHAR("Environment Variables")      ,
            "CfgPlgCLI"  , A_CHAR("Command Line Arguments")     ,

        // end of BootstrapBulk()
        nullptr );
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS


        // load plugin slots resources already now, so that plug-ins may be added to resource
        EnumRecords<Priorities>::Bootstrap( *this, "Priorities" );
    }

    else if( phase == BootstrapPhases::PrepareConfig )
    {
        EnumRecords<Exceptions>::Bootstrap();
    }


}



// #################################################################################################
// Implementation of parsing methods of built-in record types.
// #################################################################################################
void ERPriorities::Parse()
{
    enums::EnumRecordParser::Get( ERSerializable::EnumElementName  );
    enums::EnumRecordParser::Get( Priority                  , true );
}
}} // namespace [alib::config]
