// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_LANG_RESOURCES_RESOURCES)
#      include "alib/lang/resources/resources.hpp"
#   endif

#   if !defined (HPP_ALIB_FILES_FINFO)
#      include "alib/files/finfo.hpp"
#   endif
#   if !defined (HPP_ALIB_FILES_CAMP)
#      include "alib/files/filescamp.hpp"
#   endif
#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#      include "alib/enums/recordbootstrap.hpp"
#   endif
#endif // !defined(ALIB_DOX)

namespace  alib {

files::Files FILES;

namespace files {

Files::Files()
: Camp( "FILES" )
{
    ALIB_ASSERT_ERROR( this == &FILES, "FILES",
        "Instances of class FILES must not be created. Use singleton alib::FILES" )
}


void Files::bootstrap( BootstrapPhases phase )
{
    if( phase == BootstrapPhases::PrepareResources )
    {

        #define EOS ,
#if !ALIB_RESOURCES_OMIT_DEFAULTS
        resourcePool->BootstrapBulk( ResourceCategory,

        "FT",            A_CHAR("0,DIRECTORY"          ",1,"
                                "1,SYMBOLIC_LINK_DIR" ",14,"
                                "3,SYMBOLIC_LINK"      ",2,"
                                "2,REGULAR"            ",1,"
                                "4,BLOCK"              ",1,"
                                "5,CHARACTER"          ",1,"
                                "6,FIFO"               ",1,"
                                "7,SOCKET"             ",2,"
                                "8,UNKNOWN_OR_ERROR"   ",1")     EOS

        "FQ",            A_CHAR("0,NONE"                ",3,"
                                "1,STATS"               ",1,"
                                "2,RESOLVED"            ",3,"
                                "3,MAX_DEPTH_REACHED"   ",1,"
                               "15,NOT_EXISTENT"        ",5,"
                                "4,NOT_FOLLOWED"        ",5,"
                                "5,NOT_CROSSING_FS"     ",5,"
                                "6,NO_AFS"              ",5,"
                               "10,NO_ACCESS_SL_TARGET" ",13,"
                                "9,NO_ACCESS_SL"        ",11,"
                               "11,NO_ACCESS_DIR"       ",11,"
                                "8,NO_ACCESS"           ",5,"
                                "7,RECURSIVE"           ",1,"
                               "12,BROKEN_LINK"         ",1,"
                               "13,CIRCULAR_LINK"       ",1,"
                               "14,DUPLICATE"           ",1,"
                               "16,UNKNOWN_ERROR"       ",5"    ) EOS

        // Identifier/Function names
        "TFP"    , "File Permission"      EOS
        "TID"    , "File Usr/Grp ID"      EOS
        "TTY"    , "File Type"            EOS


        "CPF0"   , A_CHAR( "OwnerRead"          " I 1 2"   ),
        "CPF1"   , A_CHAR( "OwnerWrite"         " I 1 1"   ),
        "CPF2"   , A_CHAR( "OwnerExecute"       " I 1 1"   ),
        "CPF3"   , A_CHAR( "GroupRead"          " I 1 1"   ),
        "CPF4"   , A_CHAR( "GroupWrite"         " I 1 1"   ),
        "CPF5"   , A_CHAR( "GroupExecute"       " I 1 1"   ),
        "CPF6"   , A_CHAR( "OthersRead"         " I 2 1"   ),
        "CPF7"   , A_CHAR( "OthersWrite"        " I 2 1"   ),
        "CPF8"   , A_CHAR( "OthersExecute"      " I 2 1"   ),
        "CPF9"   , A_CHAR( "Directory"          " I 3"     ),
       "CPF10"   , A_CHAR( "SymbolicLinkDir"    " I 1 1 1" ),
       "CPF11"   , A_CHAR( "Regular"            " I 3"     ),
       "CPF12"   , A_CHAR( "SymbolicLink"       " I 1 1"   ),
       "CPF13"   , A_CHAR( "Block"              " I 5"     ),
       "CPF14"   , A_CHAR( "Character"          " I 9"     ),
       "CPF15"   , A_CHAR( "Fifo"               " I 4"     ),
       "CPF16"   , A_CHAR( "Socket"             " I 6"     ),
       "CPF17"   , A_CHAR( "Name"               " I 4"     ),
       "CPF18"   , A_CHAR( "Type"               " I 2"     ),
       "CPF19"   , A_CHAR( "IsDirectory"        " I 2 3"   ),
       "CPF20"   , A_CHAR( "IsSymbolicLink"     " I 2 1 1" ),
       "CPF21"   , A_CHAR( "Size"               " I 4"     ),
       "CPF22"   , A_CHAR( "Time"               " I 4"     ),
       "CPF23"   , A_CHAR( "MTime"              " I 1 1"   ),
       "CPF24"   , A_CHAR( "CTime"              " I 1 1"   ),
       "CPF25"   , A_CHAR( "ATime"              " I 1 1"   ),
       "CPF26"   , A_CHAR( "PermissionS"        " I 4 0"     ),
       "CPF27"   , A_CHAR( "KiloBytes"          " I 1 1"   ),
       "CPF28"   , A_CHAR( "MegaBytes"          " I 1 1"   ),
       "CPF29"   , A_CHAR( "GigaBytes"          " I 1 1"   ),
       "CPF30"   , A_CHAR( "TeraBytes"          " I 1 1"   ),
       "CPF31"   , A_CHAR( "PetaBytes"          " I 1 1"   ),
       "CPF32"   , A_CHAR( "ExaBytes"           " I 1 1"   ),
       "CPF33"   , A_CHAR( "Owner"              " I 5"     ),
       "CPF34"   , A_CHAR( "Group"              " I 5"     ),
       "CPF35"   , A_CHAR( "UserID"             " I 1 1 1" ),
       "CPF36"   , A_CHAR( "GroupID"            " I 1 1 1" ),
       "CPF37"   , A_CHAR( "Path"               " I 4" ),



























        // end of BootstrapBulk()
        nullptr );
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS


        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::Types    )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::Qualities)
    }

    else if( phase == BootstrapPhases::PrepareConfig )
    {
        EnumRecords<files::FInfo::Types      >::Bootstrap( *this, "FT" );
        EnumRecords<files::FInfo::Qualities  >::Bootstrap( *this, "FQ" );
    }

//    else if( phase == BootstrapPhases::Final )
//    {
//    }
}


}} // namespace [alib::files]
