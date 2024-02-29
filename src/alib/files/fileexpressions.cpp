// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(HPP_ALIB_FILES_FILEEXPRESSIONS)
#   include "alib/files/fileexpressions.hpp"
#endif

#include "alib/lang/basecamp/camp_inlines.hpp"


#if   (     defined(__GLIBCXX__)       \
         || defined(__APPLE__)         \
         || defined(__ANDROID_NDK__) )        && !defined(ALIB_TESTSTDFS)
#   include <unistd.h>
#endif

using namespace alib::expressions;

/////////////////////////////////////  Expressions ///////////////////////////////////////////////////
namespace alib::files {

#if !defined(ALIB_DOX)

//==================================================================================================
//=== Anonymous expression functions and constant objects
//==================================================================================================
namespace
{
    #define ES      ExpressionScope
    #define FS      dynamic_cast<FileExpressions::Scope&>(scope)
    #define AI      ArgIterator
    #define NODE    FS.Node
    #define VAL     FS.Node.Value()
    #define INTARG0 args->Unbox<integer>()
    Box getName    (ES& scope, AI     , AI) { return MAString(scope.Allocator, NODE.Name(), 0); }
    Box getType    (ES& scope, AI     , AI) { return VAL.Type();              }
    Box isDirectory(ES& scope, AI     , AI) { return VAL.IsDirectory();       }
    Box isSymLink  (ES& scope, AI     , AI) { return VAL.IsSymbolicLink();    }
    Box getSize    (ES& scope, AI     , AI) { return integer(VAL.Size());     }
    Box getTime    (ES& scope, AI     , AI) { return VAL.MTime();             }
    Box getCTime   (ES& scope, AI     , AI) { return VAL.CTime();             }
    Box getATime   (ES& scope, AI     , AI) { return VAL.ATime();             }
    Box getPerms   (ES& scope, AI     , AI) { return VAL.Perms();             }
    Box getOwner   (ES& scope, AI     , AI) { return VAL.Owner();             }
    Box getGroup   (ES& scope, AI     , AI) { return VAL.Group();             }
#if   (    (defined(__GLIBCXX__) && !defined(__MINGW32__))  \
         || defined(__APPLE__)                              \
         || defined(__ANDROID_NDK__) )        && !defined(ALIB_TESTSTDFS)
    Box userID     (ES&      , AI     , AI) { return FInfo::TOwnerAndGroupID(getuid());             }
    Box groupID    (ES&      , AI     , AI) { return FInfo::TOwnerAndGroupID(getgid());             }
#else
    Box userID     (ES&      , AI     , AI) { return FInfo::UnknownID; }
    Box groupID    (ES&      , AI     , AI) { return FInfo::UnknownID; }
#endif
    Box kiloBytes  (ES&      , AI args, AI) { return INTARG0 * integer(1024); }
    Box megaBytes  (ES&      , AI args, AI) { return INTARG0 * integer(1024) * integer(1024); }
    Box gigaBytes  (ES&      , AI args, AI) { return INTARG0 * integer(1024) * integer(1024) * integer(1024); }
    Box teraBytes  (ES&      , AI args, AI) { return INTARG0 * integer(1024) * integer(1024) * integer(1024) * integer(1024); }
    Box petaBytes  (ES&      , AI args, AI) { return INTARG0 * integer(1024) * integer(1024) * integer(1024) * integer(1024) * integer(1024); }
    Box exaBytes   (ES&      , AI args, AI) { return INTARG0 * integer(1024) * integer(1024) * integer(1024) * integer(1024) * integer(1024) * integer(1024); }
    Box getPath    (ES& scope, AI     , AI) { return MAString(scope.Allocator, FS.ParentPath, 0); }
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

    NString32 TypeNameConverterTFP;
    NString32 TypeNameConverterTID;
    NString32 TypeNameConverterTTY;
} // anonymous namespace
#endif //!defined(ALIB_DOX)

//==================================================================================================
//=== FileExpressions::Plugin
//==================================================================================================
FileExpressions::Plugin::Plugin( Compiler& pCompiler )
: Calculus( "Files Plug-in", pCompiler )
{
    // Initialize constant static boxes. This must not be done in the C++ bootstrap code.
    constOwnRead         = FInfo::Permissions::OWNER_READ  ;
    constOwnWrite        = FInfo::Permissions::OWNER_WRITE ;
    constOwnExec         = FInfo::Permissions::OWNER_EXEC  ;
    constGrpRead         = FInfo::Permissions::GROUP_READ  ;
    constGrpWrite        = FInfo::Permissions::GROUP_WRITE ;
    constGrpExec         = FInfo::Permissions::GROUP_EXEC  ;
    constOthRead         = FInfo::Permissions::OTHERS_READ ;
    constOthWrite        = FInfo::Permissions::OTHERS_WRITE;
    constOthExec         = FInfo::Permissions::OTHERS_EXEC ;

    constTDirectory      = FInfo::Types::DIRECTORY         ;
    constTSymbolicLinkDir= FInfo::Types::SYMBOLIC_LINK_DIR ;
    constTRegular        = FInfo::Types::REGULAR           ;
    constTSymbolicLink   = FInfo::Types::SYMBOLIC_LINK     ;
    constTBlock          = FInfo::Types::BLOCK             ;
    constTCharacter      = FInfo::Types::CHARACTER         ;
    constTFifo           = FInfo::Types::FIFO              ;
    constTSocket         = FInfo::Types::SOCKET            ;



    TypeUsrGrpID= FInfo::UnknownID;

    // load identifier/function names from resources
    constexpr int tableSize= 38;
    Token functionNames[tableSize];
    Token::LoadResourcedTokens( FILES, "CPF", functionNames  ALIB_DBG(,tableSize)  );

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    Token* descriptor= functionNames;

#if defined(_WIN32)
    TypeNameConverterTFP= FILES.GetResource("TFP"); pCompiler.AddType(constOwnRead   , TypeNameConverterTFP);
    TypeNameConverterTID= FILES.GetResource("TID"); pCompiler.AddType(TypeUsrGrpID   , TypeNameConverterTID);
    TypeNameConverterTTY= FILES.GetResource("TTY"); pCompiler.AddType(constTDirectory, TypeNameConverterTTY);
#else
    pCompiler.AddType(constOwnRead, FILES.GetResource("TFP"));
    pCompiler.AddType(TypeUsrGrpID, FILES.GetResource("TID"));
    pCompiler.AddType(constTDirectory, FILES.GetResource("TTY"));
#endif
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
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getName    ), &Types::String  , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getType    ), &constTDirectory, ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(isDirectory), &Types::Boolean , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(isSymLink  ), &Types::Boolean , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getSize    ), &Types::Integer , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getTime    ), &Types::DateTime, ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getTime    ), &Types::DateTime, ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getCTime   ), &Types::DateTime, ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getATime   ), &Types::DateTime, ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getPerms   ), &constOwnRead   , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(kiloBytes  ), &Types::Integer , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(megaBytes  ), &Types::Integer , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(gigaBytes  ), &Types::Integer , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(teraBytes  ), &Types::Integer , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(petaBytes  ), &Types::Integer , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(exaBytes   ), &Types::Integer , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getOwner   ), &TypeUsrGrpID   , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getGroup   ), &TypeUsrGrpID   , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(userID     ), &TypeUsrGrpID   , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(groupID    ), &TypeUsrGrpID   , ETI },
      { *descriptor++, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getPath    ), &Types::String  , ETI },

    };

    AutoCasts=
    {
        { constOwnRead   , nullptr, nullptr, CALCULUS_DEFAULT_AUTOCAST, nullptr, nullptr },
        { constTDirectory, nullptr, nullptr, CALCULUS_DEFAULT_AUTOCAST, nullptr, nullptr },
        { TypeUsrGrpID   , nullptr, nullptr, CALCULUS_DEFAULT_AUTOCAST, nullptr, nullptr },
    };
} // FileExpressions::Plugin constructor

//==================================================================================================
//=== FileExpressions
//==================================================================================================
FileExpressions::FileExpressions()
: compiler()
, plugin( compiler )
{
    compiler.SetupDefaults();
    compiler.InsertPlugin( &plugin, expressions::CompilePriorities::Custom );
}

SPFileFilter  FileExpressions::CreateFilter( const String& expressionString )
{
    return SPFileFilter(new FileExpressions::Filter( *this, expressionString ));
}

//==================================================================================================
//=== FileExpressions::Filter
//==================================================================================================
FileExpressions::Filter::Filter( FileExpressions& pFex, const String& expressionString )
: fex  ( pFex )
, scope( pFex.compiler.CfgFormatter )
{
    expression= fex.compiler.Compile( expressionString );
    if( !expression->ResultType().IsType<bool>() )
        throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
}

} // namespace [alib::files]
