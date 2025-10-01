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
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "alib/files/files.prepro.hpp"

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Files;
    import   ALib.Lang;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.Format;
#  if ALIB_EXPRESSIONS
    import   ALib.Expressions;
#  endif
    import   ALib.Camp;
#else
#   include "ALib.Lang.H"
#   include "ALib.Characters.Functions.H"
#   include "ALib.Strings.H"
#   include "ALib.Boxing.H"
#   include "ALib.Format.H"
#   include "ALib.Expressions.H"
#   include "ALib.Camp.H"
#   include "ALib.Files.H"
#endif
// ======================================   Implementation   =======================================
ALIB_BOXING_VTABLE_DEFINE( alib::files::File               , vt_files_cursor    )


namespace  alib {

files::FilesCamp FILES;

namespace files {

FilesCamp::FilesCamp()
: Camp( "FILES" )
{
    #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
      ALIB_ASSERT_ERROR( this == &FILES, "FILES",
          "Instances of class FILES must not be created. Use singleton alib::FILES" )
    #endif
}


void FilesCamp::Bootstrap()
{
    if( GetBootstrapState() == BootstrapPhases::PrepareResources )
    {
        #define EOS ,
#if !ALIB_CAMP_OMIT_DEFAULT_RESOURCES
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
#endif // !ALIB_CAMP_OMIT_DEFAULT_RESOURCES

        // parse enum records
        enumrecords::bootstrap::Bootstrap<files::FInfo::Types            >( *this, "FT"  );
        enumrecords::bootstrap::Bootstrap<files::FInfo::TypeNames1Letter >( *this, "FT1" );
        enumrecords::bootstrap::Bootstrap<files::FInfo::TypeNames2Letters>( *this, "FT2" );
        enumrecords::bootstrap::Bootstrap<files::FInfo::TypeNames3Letters>( *this, "FT3" );
        enumrecords::bootstrap::Bootstrap<files::FInfo::Qualities        >( *this, "FQ"  );
        enumrecords::bootstrap::Bootstrap<files::FInfo::Qualities3Letters>( *this, "FQ3" );
    }

    else if( GetBootstrapState() == BootstrapPhases::PrepareConfig )    {}
    else if( GetBootstrapState() == BootstrapPhases::Final )            {}
}


}} // namespace [alib::files]
