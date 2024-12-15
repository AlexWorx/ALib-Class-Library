// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/config/configcamp.hpp"
#   include "alib/config/configuration.hpp"
#   include "alib/lang/basecamp/basecamp.hpp"
#   include "alib/lang/system/systemerrors.hpp"
#   include "alib/lang/resources/resources.hpp"
#   include "alib/enums/recordbootstrap.hpp"
#   include "alib/enums/serialization.hpp"
#endif // !DOXYGEN



ALIB_BOXING_VTABLE_DEFINE( alib::config::Priority                  , vt_config_priorities  )
ALIB_BOXING_VTABLE_DEFINE( alib::config::Exceptions                , vt_config_exceptions  )
ALIB_BOXING_VTABLE_DEFINE( alib::config::Variable                  , vt_config_variable    )
ALIB_BOXING_VTABLE_DEFINE( alib::config::Configuration::ConstCursor, vt_config_constcursor )

using namespace alib;
using namespace alib::config;

namespace alib {

config::ConfigCamp CONFIG;

namespace config  {

ConfigCamp::ConfigCamp()
: Camp( "CFG" )
{
    ALIB_ASSERT_ERROR( this == &CONFIG, "CONFIG",
        "Instances of class Config must not be created. Use singleton  alib::CONFIG" )
}


void ConfigCamp::bootstrap( BootstrapPhases phase )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_config_priorities   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_config_exceptions   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_config_variable       )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_config_constcursor  )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( config::Exceptions     )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::config::Priority )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::config::Variable )


#if !ALIB_RESOURCES_OMIT_DEFAULTS
        resourcePool->BootstrapBulk( ResourceCategory,
            "E<", A_CHAR("config::"),
            "E" , A_CHAR("0"  ","    "OK"                ","  ""     ","
                         "1"  ","    "ErrorOpeningFile"  ","  "ERF"  ","
                         "2"  ","    "ErrorWritingFile"  ","  "EWF"       ),

             "ERF", A_CHAR("Can't open {}file {!Q'} for reading.") ,
             "EWF", A_CHAR("Can't open {}file {!Q'} for writing.") ,


         "Priority",
           A_CHAR(     "0"   ","   "NONE"              ",1,"
                    "1000"   ","   "AutoDetected"      ",1,"
                    "2000"   ","   "DefaultValues"     ",1,"
                    "4000"   ","   "Standard"          ",1,"
                    "6000"   ","   "ConfigFile"        ",2,"
                    "8000"   ","   "Environment"       ",1,"
                   "10000"   ","   "SessionFile"       ",8,"
                   "12000"   ","   "CLI"               ",2,"
                   "14000"   ","   "Session"           ",2,"
                     "max"   ","   "Protected"         ",1"      ),


            // names of the configuration plug-ins added to configuration in default construction.
            "CfgPlgEnv"  , A_CHAR("Environment Variables")  ,
            "CfgPlgCLI"  , A_CHAR("Command Line Arguments") ,

            "INIWB"      , A_CHAR("writeback")              ,

            // boolean tokens
            "BTF", A_CHAR("False I 1," )  A_CHAR( "True I 1," )
                   A_CHAR(    "0 I 1," )  A_CHAR(    "1 I 1," )
                   A_CHAR(   "No I 1," )  A_CHAR(  "Yes I 1," )
                   A_CHAR(  "Off I 3," )  A_CHAR(   "On I 2," )
                   A_CHAR(  "--- I 1," )  A_CHAR(   "OK I 2"  ),

        // end of BootstrapBulk()
        nullptr );
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS


        // parse enum records
        EnumRecords<Priority>::Bootstrap( *this, "Priority" );
        EnumRecords<Exceptions>::Bootstrap();
    }

    else if( phase == BootstrapPhases::PrepareConfig )
    {}

    else if( phase == BootstrapPhases::Final )
    {}
}



// #################################################################################################
// Implementation of parsing methods of built-in record types.
// #################################################################################################
void ERPriority::Parse()
{
    enums::EnumRecordParser::Get( ERSerializable::EnumElementName  );
    enums::EnumRecordParser::Get( Priority                  , true );
}
}} // namespace [alib::config]
