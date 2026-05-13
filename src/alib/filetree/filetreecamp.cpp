ALIB_BOXING_VTABLE_DEFINE( alib::filetree::FTFile               , vt_files_cursor    )


namespace  alib {

filetree::FilesCamp FILETREE;

namespace filetree {

#if !DOXYGEN
namespace {
void bulkloadResources(ResourcePool& resourcePool, const NCString& resourceCategory) {
    resourcePool.BootstrapBulk( resourceCategory,

    // ALIB-RESOURCE-COMPILER-REPLACEMENT-START
    "FT"   , A_CHAR("0,Directory,1,1,SymbolicLinkToDir,15,3,SymbolicLinkToFile,15,2,Regular,1,4,Block,1,5,Character,1,6,Fifo,1,7,Socket,2,8,UNKNOWN_OR_ERROR,1"),
    "FT1"  , A_CHAR("0,d,1,1,L,1,2,-,1,3,l,1,4,b,1,5,c,1,6,p,1,7,s,1"),
    "FT2"  , A_CHAR("0,dr,1,1,ld,3,2,rf,1,3,lf,3,4,bl,1,5,ch,1,6,ff,1,7,so,2"),
    "FT3"  , A_CHAR("0,dir,1,1,sld,3,2,reg,1,3,slf,3,4,blk,1,5,chr,1,6,ffo,1,7,sck,2"),
    "FQ"   , A_CHAR("0,NONE,3,1,STATS,1,2,RESOLVED,3,3,MAX_DEPTH_REACHED,1,15,NOT_EXISTENT,5,4,NOT_FOLLOWED,5,5,NOT_CROSSING_FS,5,6,NO_AFS,5,10,NO_ACCESS_SL_TARGET,13,9,NO_ACCESS_SL,11,11,NO_ACCESS_DIR,11,8,NO_ACCESS,5,7,RECURSIVE,1,12,BROKEN_LINK,1,13,CIRCULAR_LINK,1,14,DUPLICATE,1,16,UNKNOWN_ERROR,5"),
    "FQ3"  , A_CHAR("0,NON,3,1,STA,3,2,RES,3,3,MDR,3,4,NFO,3,5,NCF,3,6,NAF,3,7,REC,3,8,NAC,3,9,NSL,3,10,NAT,3,11,NAD,3,12,BRL,3,13,CIL,3,14,DUP,3,15,NEX,3,16,UKE,3"),
    "FFMT" , A_CHAR("ta h on gn s dm nal"),
    "TFP"  , A_CHAR("File Permission"),
    "TID"  , A_CHAR("File Usr/Grp ID"),
    "TTY"  , A_CHAR("File Type"),
    "CPF0" , A_CHAR("OwnerRead I 1 2"),
    "CPF1" , A_CHAR("OwnerWrite I 1 1"),
    "CPF2" , A_CHAR("OwnerExecute I 1 1"),
    "CPF3" , A_CHAR("GroupRead I 1 1"),
    "CPF4" , A_CHAR("GroupWrite I 1 1"),
    "CPF5" , A_CHAR("GroupExecute I 1 1"),
    "CPF6" , A_CHAR("OthersRead I 2 1"),
    "CPF7" , A_CHAR("OthersWrite I 2 1"),
    "CPF8" , A_CHAR("OthersExecute I 2 1"),
    "CPF9" , A_CHAR("Directory I 3"),
    "CPF10", A_CHAR("SymbolicLinkDir I 1 1 1"),
    "CPF11", A_CHAR("Regular I 3"),
    "CPF12", A_CHAR("SymbolicLink I 1 1"),
    "CPF13", A_CHAR("Block I 5"),
    "CPF14", A_CHAR("Character I 9"),
    "CPF15", A_CHAR("Fifo I 4"),
    "CPF16", A_CHAR("Socket I 6"),
    "CPF17", A_CHAR("Name I 4"),
    "CPF18", A_CHAR("Type I 2"),
    "CPF19", A_CHAR("IsDirectory I 2 3"),
    "CPF20", A_CHAR("IsSymbolicLink I 2 1 1"),
    "CPF21", A_CHAR("Size I 4"),
    "CPF22", A_CHAR("Date I 4"),
    "CPF23", A_CHAR("MDate I 1 1"),
    "CPF24", A_CHAR("BDate I 1 1"),
    "CPF25", A_CHAR("CDate I 1 1"),
    "CPF26", A_CHAR("ADate I 1 1"),
    "CPF27", A_CHAR("PermissionS I 4 0"),
    "CPF28", A_CHAR("KiloBytes I 1 1"),
    "CPF29", A_CHAR("MegaBytes I 1 1"),
    "CPF30", A_CHAR("GigaBytes I 1 1"),
    "CPF31", A_CHAR("TeraBytes I 1 1"),
    "CPF32", A_CHAR("PetaBytes I 1 1"),
    "CPF33", A_CHAR("ExaBytes I 1 1"),
    "CPF34", A_CHAR("Owner I 5"),
    "CPF35", A_CHAR("Group I 5"),
    "CPF36", A_CHAR("UserID I 1 1 1"),
    "CPF37", A_CHAR("GroupID I 1 1 1"),
    "CPF38", A_CHAR("Path I 4"),
    // ALIB-RESOURCE-COMPILER-REPLACEMENT-END

    // end of BootstrapBulk()
    nullptr );
}
} // anonymous namespace
#endif // !DOXYGEN



FilesCamp::FilesCamp()
: Camp( "FILETREE" ) {
    #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
      ALIB_ASSERT_ERROR( this == &FILETREE, "FILETREE",
          "Instances of class FILETREE must not be created. Use singleton alib::FILETREE" )
    #endif
}


void FilesCamp::Bootstrap() {
    if( GetBootstrapState() == BootstrapPhases::PrepareResources ) {
        #define EOS ,
#if !ALIB_CAMP_OMIT_DEFAULT_RESOURCES
        // now, we just add the resources that parent App did not define for us.
        camp::DevtimeResourceCompiler rc;
        if(!rc.Do( "filetreecamp.alibrc", __FILE__, *this, ResourceCategory, true,
                                          __FILE__ )
                               //       nullptr )
            )
            bulkloadResources(GetResourcePool(), ResourceCategory);
#endif // !ALIB_CAMP_OMIT_DEFAULT_RESOURCES

        // parse enum records
        enumrecords::bootstrap::Bootstrap<system::FileStatus::Types             >( *this, "FT"  );
        enumrecords::bootstrap::Bootstrap<system::FileStatus::TypeNames1Letter  >( *this, "FT1" );
        enumrecords::bootstrap::Bootstrap<system::FileStatus::TypeNames2Letters >( *this, "FT2" );
        enumrecords::bootstrap::Bootstrap<system::FileStatus::TypeNames3Letters >( *this, "FT3" );
        enumrecords::bootstrap::Bootstrap<system::FileStatus::ScanStates        >( *this, "FQ"  );
        enumrecords::bootstrap::Bootstrap<system::FileStatus::ScanStates3Letters>( *this, "FQ3" );
    }

    else if( GetBootstrapState() == BootstrapPhases::PrepareConfig )    {}
    else if( GetBootstrapState() == BootstrapPhases::Final )            {}
}


}} // namespace [alib::filetree]
