using namespace alib::expressions;
using namespace alib::system;

namespace alib::filetree {

#if !DOXYGEN

//==================================================================================================
//=== Anonymous expression functions and constant objects
//==================================================================================================
namespace
{
    #define ES      ExpressionScope
    #define FS      dynamic_cast<FileExpressions::FexScope&>(scope)
    #define AI      ArgIterator
    #define NODE    FS.Node
    #define VAL     FS.Node
    #define INTARG0 args->Unbox<integer>()
Box getType    (ES& scope, AI     , AI)                                      { return VAL->Type(); }
Box isDirectory(ES& scope, AI     , AI)                               { return VAL->IsDirectory(); }
Box isSymLink  (ES& scope, AI     , AI)                            { return VAL->IsSymbolicLink(); }
Box getSize    (ES& scope, AI     , AI)                             { return integer(VAL->Size()); }
Box getTime    (ES& scope, AI     , AI)                                     { return VAL->MDate(); }
Box getBTime   (ES& scope, AI     , AI)                                     { return VAL->BDate(); }
Box getCTime   (ES& scope, AI     , AI)                                     { return VAL->CDate(); }
Box getATime   (ES& scope, AI     , AI)                                     { return VAL->ADate(); }
Box getPerms   (ES& scope, AI     , AI)                                     { return VAL->Perms(); }
Box getOwner   (ES& scope, AI     , AI)                                     { return VAL->Owner(); }
Box getGroup   (ES& scope, AI     , AI)                                     { return VAL->Group(); }
#if  ALIB_SYSTEM_FILE_STATUS_IMPL == ALIB_SYSTEM_FILE_POSIX_STATUS
Box userID     (ES&      , AI     , AI)              { return FTValue::TOwnerAndGroupID(getuid()); }
Box groupID    (ES&      , AI     , AI)              { return FTValue::TOwnerAndGroupID(getgid()); }
#else
    Box userID     (ES&      , AI     , AI) { return FTValue::UnknownID; }
    Box groupID    (ES&      , AI     , AI) { return FTValue::UnknownID; }
#endif
Box kiloBytes  (ES&      , AI args, AI)                          { return INTARG0 * integer(1024); }
Box megaBytes  (ES&      , AI args, AI)          { return INTARG0 * integer(1024) * integer(1024); }
Box gigaBytes  (ES&      , AI args, AI) { return INTARG0 * integer(1024) * integer(1024) * integer(1024); }
Box teraBytes  (ES&      , AI args, AI) { return INTARG0 * integer(1024) * integer(1024) * integer(1024) * integer(1024); }
Box petaBytes  (ES&      , AI args, AI) { return INTARG0 * integer(1024) * integer(1024) * integer(1024) * integer(1024) * integer(1024); }
Box exaBytes   (ES&      , AI args, AI) { return INTARG0 * integer(1024) * integer(1024) * integer(1024) * integer(1024) * integer(1024) * integer(1024); }


#if ALIB_PATH_CHARACTERS_WIDE == ALIB_CHARACTERS_WIDE
Box getName    (ES& scope, AI     , AI)                                      { return NODE.Name(); }
Box getPath    (ES& scope, AI     , AI)                                    { return FS.ParentPath; }
#else
    // allocate converted name in the scope
    Box getName    (ES& scope, AI     , AI) { return String(scope.Allocator, String256(NODE.Name()  )); }
    Box getPath    (ES& scope, AI     , AI) { return String(scope.Allocator, String256(FS.ParentPath)); }
#endif

    #undef ES
    #undef FS
    #undef AI
    #undef NODE
    #undef VAL
    #undef INTARG0

    extern Box constOwnRead          ; Box constOwnRead ;
    extern Box constOwnWrite         ; Box constOwnWrite;
    extern Box constOwnExec          ; Box constOwnExec ;
    extern Box constGrpRead          ; Box constGrpRead ;
    extern Box constGrpWrite         ; Box constGrpWrite;
    extern Box constGrpExec          ; Box constGrpExec ;
    extern Box constOthRead          ; Box constOthRead ;
    extern Box constOthWrite         ; Box constOthWrite;
    extern Box constOthExec          ; Box constOthExec ;

    extern Box constTDirectory       ; Box constTDirectory      ;
    extern Box constTSymbolicLinkDir ; Box constTSymbolicLinkDir;
    extern Box constTRegular         ; Box constTRegular        ;
    extern Box constTSymbolicLink    ; Box constTSymbolicLink   ;
    extern Box constTBlock           ; Box constTBlock          ;
    extern Box constTCharacter       ; Box constTCharacter      ;
    extern Box constTFifo            ; Box constTFifo           ;
    extern Box constTSocket          ; Box constTSocket         ;

    extern Box TypeUsrGrpID          ; Box TypeUsrGrpID         ;

} // anonymous namespace
#endif //!DOXYGEN

  //================================================================================================
  //=== FileExpressions::Plugin
  //================================================================================================
FileExpressions::Plugin::Plugin( Compiler& pCompiler )
: Calculus( "Files Plug-in", pCompiler, expressions::CompilePriorities::Custom ) {
    // Initialize constant static boxes. This must not be done in the C++ bootstrap code.
    constOwnRead         = FileStatus::Permissions::OWNER_READ  ;
    constOwnWrite        = FileStatus::Permissions::OWNER_WRITE ;
    constOwnExec         = FileStatus::Permissions::OWNER_EXEC  ;
    constGrpRead         = FileStatus::Permissions::GROUP_READ  ;
    constGrpWrite        = FileStatus::Permissions::GROUP_WRITE ;
    constGrpExec         = FileStatus::Permissions::GROUP_EXEC  ;
    constOthRead         = FileStatus::Permissions::OTHERS_READ ;
    constOthWrite        = FileStatus::Permissions::OTHERS_WRITE;
    constOthExec         = FileStatus::Permissions::OTHERS_EXEC ;

    constTDirectory      = FileStatus::Types::DIRECTORY         ;
    constTSymbolicLinkDir= FileStatus::Types::SYMBOLIC_LINK_DIR ;
    constTRegular        = FileStatus::Types::REGULAR           ;
    constTSymbolicLink   = FileStatus::Types::SYMBOLIC_LINK     ;
    constTBlock          = FileStatus::Types::BLOCK             ;
    constTCharacter      = FileStatus::Types::CHARACTER         ;
    constTFifo           = FileStatus::Types::FIFO              ;
    constTSocket         = FileStatus::Types::SOCKET            ;

    TypeUsrGrpID         = FTValue::UnknownID;

#if ALIB_CHARACTERS_WIDE
    NString256 converter;
    converter.Reset( FILETREE.GetResource("TFP") ); pCompiler.AddType(constOwnRead   , NString(pCompiler.GetAllocator(), converter) );
    converter.Reset( FILETREE.GetResource("TID") ); pCompiler.AddType(TypeUsrGrpID   , NString(pCompiler.GetAllocator(), converter) );
    converter.Reset( FILETREE.GetResource("TTY") ); pCompiler.AddType(constTDirectory, NString(pCompiler.GetAllocator(), converter) );
#else
    pCompiler.AddType(constOwnRead   , FILETREE.GetResource("TFP"));
    pCompiler.AddType(TypeUsrGrpID   , FILETREE.GetResource("TID"));
    pCompiler.AddType(constTDirectory, FILETREE.GetResource("TTY"));
#endif

    // load identifier/function names from resources
    constexpr int tableSize= 39;
    Token functionNames[tableSize];
    strings::util::LoadResourcedTokens( FILETREE, "CPF", functionNames  ALIB_DBG(,tableSize)  );

    Token* descriptor= functionNames;

    // Constant identifiers
    ConstantIdentifiers=
    {
        { *descriptor++, constOwnRead   },
        { *descriptor++, constOwnWrite  },
        { *descriptor++, constOwnExec   },
        { *descriptor++, constGrpRead   },
        { *descriptor++, constGrpWrite  },
        { *descriptor++, constGrpExec   },
        { *descriptor++, constOthRead   },
        { *descriptor++, constOthWrite  },
        { *descriptor++, constOthExec   },

        { *descriptor++, constTDirectory       },
        { *descriptor++, constTSymbolicLinkDir },
        { *descriptor++, constTRegular         },
        { *descriptor++, constTSymbolicLink    },
        { *descriptor++, constTBlock           },
        { *descriptor++, constTCharacter       },
        { *descriptor++, constTFifo            },
        { *descriptor++, constTSocket          },
    };


    Functions=
    {
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getName    ), &Types::String , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getType    ), &constTDirectory, ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(isDirectory), &Types::Boolean , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(isSymLink  ), &Types::Boolean , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getSize    ), &Types::Integer , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getTime    ), &Types::DateTime, ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getTime    ), &Types::DateTime, ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getBTime   ), &Types::DateTime, ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getCTime   ), &Types::DateTime, ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getATime   ), &Types::DateTime, ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getPerms   ), &constOwnRead   , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(kiloBytes  ), &Types::Integer , CTI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(megaBytes  ), &Types::Integer , CTI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(gigaBytes  ), &Types::Integer , CTI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(teraBytes  ), &Types::Integer , CTI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(petaBytes  ), &Types::Integer , CTI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(exaBytes   ), &Types::Integer , CTI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getOwner   ), &TypeUsrGrpID   , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getGroup   ), &TypeUsrGrpID   , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(userID     ), &TypeUsrGrpID   , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(groupID    ), &TypeUsrGrpID   , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getPath    ), &Types::String , ETI },

    };

    AutoCasts=
    {
        { constOwnRead   , nullptr, nullptr, CALCULUS_DEFAULT_AUTOCAST   , nullptr      , nullptr },
        { constTDirectory, nullptr, nullptr, CALCULUS_DEFAULT_AUTOCAST   , nullptr      , nullptr },
        { TypeUsrGrpID   , nullptr, nullptr, CALCULUS_DEFAULT_AUTOCAST   , nullptr      , nullptr },
    };
    
} // FileExpressions::Plugin constructor

//==================================================================================================
//=== FileExpressions
//==================================================================================================
FileExpressions::FileExpressions()
: plugin( compiler )                 { compiler.SetupDefaults(); compiler.InsertPlugin( &plugin ); }

SPFileFilter  FileExpressions::CreateFilter( const String& expressionString )
{ return SPFileFilter(new FileExpressions::Filter( *this, expressionString )); }

//==================================================================================================
//=== FileExpressions::Filter
//==================================================================================================
FileExpressions::Filter::Filter( FileExpressions& pFex, const String& expressionString )
: fex  ( pFex )
, scope( pFex.compiler.CfgFormatter ) {
    expression= fex.compiler.Compile( expressionString );
    if( !expression->ResultType().IsType<bool>() )
        throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
}

} // namespace [alib::filetree]
