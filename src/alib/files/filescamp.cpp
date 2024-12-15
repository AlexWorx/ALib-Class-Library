// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/lang/resources/resources.hpp"
#   include "alib/files/ftree.hpp"
#   include "alib/files/filescamp.hpp"
#   include "alib/enums/serialization.hpp"
#   include "alib/enums/recordbootstrap.hpp"
#   include "alib/lang/format/fwds.hpp"
#endif // !DOXYGEN

ALIB_BOXING_VTABLE_DEFINE( alib::files::File               , vt_files_cursor    )


namespace  alib {

files::FilesCamp FILES;

namespace files {

FilesCamp::FilesCamp()
: Camp( "FILES" )
{
    ALIB_ASSERT_ERROR( this == &FILES, "FILES",
        "Instances of class FILES must not be created. Use singleton alib::FILES" )
}


void FilesCamp::bootstrap( BootstrapPhases phase )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_cursor       )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::files::File )
        alib::boxing::BootstrapRegister<alib::lang::format::FFormat,
                                        alib::boxing::TMappedTo<files::File> >( FFormat_File );
        #define EOS ,
#if !ALIB_RESOURCES_OMIT_DEFAULTS
        resourcePool->BootstrapBulk( ResourceCategory,

        "FT",            A_CHAR("0,Directory"          ",1,"
                                "1,SymbolicLinkToDir"  ",15,"
                                "3,SymbolicLinkToFile" ",15,"
                                "2,Regular"            ",1,"
                                "4,Block"              ",1,"
                                "5,Character"          ",1,"
                                "6,Fifo"               ",1,"
                                "7,Socket"             ",2,"
                                "8,UNKNOWN_OR_ERROR"   ",1"   )  EOS

        "FT1",           A_CHAR("0,d"                   ",1,"
                                "1,L"                   ",1,"
                                "2,-"                   ",1,"
                                "3,l"                   ",1,"
                                "4,b"                   ",1,"
                                "5,c"                   ",1,"
                                "6,p"                   ",1,"
                                "7,s"                   ",1"  )  EOS

        "FT2",           A_CHAR("0,dr"                 ",1,"
                                "1,ld"                 ",3,"
                                "2,rf"                 ",1,"
                                "3,lf"                 ",3,"
                                "4,bl"                 ",1,"
                                "5,ch"                 ",1,"
                                "6,ff"                 ",1,"
                                "7,so"                 ",2"  )  EOS

        "FT3",           A_CHAR("0,dir"                 ",1,"
                                "1,sld"                 ",3,"
                                "2,reg"                 ",1,"
                                "3,slf"                 ",3,"
                                "4,blk"                 ",1,"
                                "5,chr"                 ",1,"
                                "6,ffo"                 ",1,"
                                "7,sck"                 ",2"  )  EOS


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
                               "16,UNKNOWN_ERROR"       ",5"  )  EOS

        "FQ3",           A_CHAR("0,NON"                 ",3,"
                                "1,STA"                 ",3,"
                                "2,RES"                 ",3,"
                                "3,MDR"                 ",3,"
                                "4,NFO"                 ",3,"
                                "5,NCF"                 ",3,"
                                "6,NAF"                 ",3,"
                                "7,REC"                 ",3,"
                                "8,NAC"                 ",3,"
                                "9,NSL"                 ",3,"
                               "10,NAT"                 ",3,"
                               "11,NAD"                 ",3,"
                               "12,BRL"                 ",3,"
                               "13,CIL"                 ",3,"
                               "14,DUP"                 ",3,"
                               "15,NEX"                 ",3,"
                               "16,UKE"                 ",3"  )  EOS
                               
        // Identifier/Function names
        "TFP"    , A_CHAR("File Permission"  )    EOS
        "TID"    , A_CHAR("File Usr/Grp ID"  )    EOS
        "TTY"    , A_CHAR("File Type"        )    EOS


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
       "CPF22"   , A_CHAR( "Date"               " I 4"     ),
       "CPF23"   , A_CHAR( "MDate"              " I 1 1"   ),
       "CPF24"   , A_CHAR( "BDate"              " I 1 1"   ),
       "CPF25"   , A_CHAR( "CDate"              " I 1 1"   ),
       "CPF26"   , A_CHAR( "ADate"              " I 1 1"   ),
       "CPF27"   , A_CHAR( "PermissionS"        " I 4 0"   ),
       "CPF28"   , A_CHAR( "KiloBytes"          " I 1 1"   ),
       "CPF29"   , A_CHAR( "MegaBytes"          " I 1 1"   ),
       "CPF30"   , A_CHAR( "GigaBytes"          " I 1 1"   ),
       "CPF31"   , A_CHAR( "TeraBytes"          " I 1 1"   ),
       "CPF32"   , A_CHAR( "PetaBytes"          " I 1 1"   ),
       "CPF33"   , A_CHAR( "ExaBytes"           " I 1 1"   ),
       "CPF34"   , A_CHAR( "Owner"              " I 5"     ),
       "CPF35"   , A_CHAR( "Group"              " I 5"     ),
       "CPF36"   , A_CHAR( "UserID"             " I 1 1 1" ),
       "CPF37"   , A_CHAR( "GroupID"            " I 1 1 1" ),
       "CPF38"   , A_CHAR( "Path"               " I 4" ),

       // default format for method File::Format when used with Formatter and no placeholder
       // string was given.
       "FFMT"    , A_CHAR( "ta h on gn s dm nal" ),

        // end of BootstrapBulk()
        nullptr );
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS


        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::Types             )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::TypeNames1Letter  )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::TypeNames2Letters )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::TypeNames3Letters )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::Qualities )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FInfo::Qualities3Letters )

        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_perms  )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_type   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_type1  )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_type2  )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_type3  )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_qual   )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_files_qual3  )
                                                                             

        // parse enum records
        EnumRecords<files::FInfo::Types            >::Bootstrap( *this, "FT"  );
        EnumRecords<files::FInfo::TypeNames1Letter >::Bootstrap( *this, "FT1" );
        EnumRecords<files::FInfo::TypeNames2Letters>::Bootstrap( *this, "FT2" );
        EnumRecords<files::FInfo::TypeNames3Letters>::Bootstrap( *this, "FT3" );
        EnumRecords<files::FInfo::Qualities        >::Bootstrap( *this, "FQ"  );
        EnumRecords<files::FInfo::Qualities3Letters>::Bootstrap( *this, "FQ3" );
    }

    else if( phase == BootstrapPhases::PrepareConfig )
    {}

    else if( phase == BootstrapPhases::Final )
    {}
}


}} // namespace [alib::files]
