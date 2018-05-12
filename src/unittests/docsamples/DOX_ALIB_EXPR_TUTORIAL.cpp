// #################################################################################################
//  aworx - Unit Tests
//  Documentation sample for ALib Expressions: Calculator
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"
#include "../aworx_unittests.hpp"

#define TESTCLASSNAME       CPP_ALib_Dox_Expr_Tutorial


// Preparations to fake std::cout, main(), etc.
#include <sstream>
#include <iostream>
#include <iomanip>
#include <experimental/filesystem>


namespace std { namespace {   basic_stringstream<char> testOutputStreamN;    } }
#define cout testOutputStreamN


// #################################################################################################
// #### Tutorial code
// #################################################################################################
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
    #pragma clang diagnostic ignored "-Wunused-variable"
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wunused-parameter"
    #pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#elif defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable:4100 ) // unreferenced formal parameter
#endif

#include "alib/expressions/compiler.hpp"
#include "alib/expressions/scope.hpp"
#include "alib/expressions/compilerplugin.hpp"
#include "alib/expressions/plugins/strings.hpp"
#include "alib/expressions/plugins/calculus.hpp"
#include "alib/expressions/plugins/scopestring.hpp"
#include "alib/expressions/detail/program.hpp"
#include "alib/expressions/detail/virtualmachine.hpp"
#include "alib/compatibility/std_string.hpp"
#include "alib/strings/format/simpletext.hpp"
#include <sys/stat.h>

using namespace std;
using namespace aworx;
using namespace aworx::lib;
using namespace aworx::lib::expressions;

namespace fs = experimental::filesystem;


// #################################################################################################
// #### Chapter "Operators" Sample code
// #################################################################################################
namespace{
//! [DOX_ALIB_EXPR_OPS_Plugin]
struct FormatOperator : CompilerPlugin
{
    FormatOperator( Compiler& compiler )
    : CompilerPlugin( "Tutorial Plugin", compiler )
    {}

    virtual bool TryCompilation( CIBinaryOp&  ciBinaryOp )
    {
        // check if it is not us
        if(     ciBinaryOp.Operator != ASTR("{}")
            || !ciBinaryOp.ArgsBegin->IsSameType( Types::String ) )
            return false;

        // set debug info
        ALIB_DBG( ciBinaryOp.DbgCallBackName = "CBFormat";      )

        // all is const? We can do it at compile time!
        if( ciBinaryOp.LhsIsConst && ciBinaryOp.RhsIsConst )
        {
            ciBinaryOp.TypeOrValue= lib::expressions::plugins::CBFormat(ciBinaryOp.CompileTimeScope,
                                                                        ciBinaryOp.ArgsBegin,
                                                                        ciBinaryOp.ArgsEnd         );
            return true;
        }

        // set callback
        ciBinaryOp.Callback     = lib::expressions::plugins::CBFormat;
        ciBinaryOp.TypeOrValue  = Types::String;
        return true;
    }
};
//! [DOX_ALIB_EXPR_OPS_Plugin]


} // anonymous namespace
// #################################################################################################
// #### File filer sample code
// #################################################################################################
namespace{

//######################################### STEP 1 #################################################
//! [DOX_ALIB_EXPR_TUT_FF_FilterSkeleton]
namespace step1 {
struct FileFilter
{
    // Constructor
    FileFilter(const String& expressionString)
    {
        (void) expressionString;
    }

    // Filter function. Takes a directory entry and returns 'true' if the entry is included.
    bool Includes(const fs::directory_entry& directoryEntry)
    {
        (void) directoryEntry;
        return true;
    }
};
} // namespace step1
//! [DOX_ALIB_EXPR_TUT_FF_FilterSkeleton]


//######################################### STEP 2 #################################################
//! [DOX_ALIB_EXPR_TUT_FF_FilterGeneric]
namespace step2 {
struct FileFilter
{
    Compiler           compiler;
    expressions::Scope scope;
    SPExpression       expression;


    // Constructor
    FileFilter( const String& expressionString )
    : compiler()
    , scope( compiler.CfgFormatter )
    {
        compiler.SetupDefaults();

        expression= compiler.Compile( expressionString );
    }

    // Filter function. Takes a directory entry and returns 'true' if the entry is included.
    bool Includes(const fs::directory_entry& directoryEntry)
    {
        (void) directoryEntry;
        return expression->Evaluate( scope ).Unbox<bool>();
    }
};
} // namespace step2
//! [DOX_ALIB_EXPR_TUT_FF_FilterGeneric]

//######################################### STEP 3 #################################################
namespace step3 {
struct FileFilterCheckingX
{
    Compiler           compiler;
    expressions::Scope scope;
    SPExpression       expression;

FileFilterCheckingX( const String& expressionString )
: compiler()
, scope( compiler.CfgFormatter )
{
    compiler.SetupDefaults();
    expression= compiler.Compile( expressionString );
}
//! [DOX_ALIB_EXPR_TUT_FF_FilterCheckResultType]
bool Includes(const fs::directory_entry& directoryEntry)
{
    (void) directoryEntry;

    Box result= expression->Evaluate( scope );
    if( result.IsType<bool>() )
        return result.Unbox<bool>();

    // trow exception
    throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
}
//! [DOX_ALIB_EXPR_TUT_FF_FilterCheckResultType]
};

#define FileFilter FileFilterChecking2
struct FileFilter
{
    Compiler           compiler;
    expressions::Scope scope;
    SPExpression       expression;

//! [DOX_ALIB_EXPR_TUT_FF_FilterCheckResultType2]
FileFilter( const String& expressionString )
: compiler()
, scope( compiler.CfgFormatter )
{
    compiler.SetupDefaults();

    expression= compiler.Compile( expressionString );

    // check result type of the expression
    if( !expression->ResultType().IsType<bool>() )
        throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
}

bool Includes(const fs::directory_entry& directoryEntry)
{
    (void) directoryEntry;

    // no result type check needed: It is asserted that Evaluate() returns a boxed boolean value.
    return expression->Evaluate( scope ).Unbox<bool>();
}
//! [DOX_ALIB_EXPR_TUT_FF_FilterCheckResultType2]
};
} // namespace step3
#undef FileFilter

//###################################### STEP 4:  Scope ############################################
//! [DOX_ALIB_EXPR_TUT_FF_Scope]
namespace step4 {
struct FFScope : public expressions::Scope
{
    // the current directory entry
    const fs::directory_entry*    directoryEntry;

    // expose parent constructor
    using Scope::Scope;
};
} // namespace step4
//! [DOX_ALIB_EXPR_TUT_FF_Scope]

//! [DOX_ALIB_EXPR_TUT_FF_ScopeFF]
namespace step4 {
struct FileFilter
{
    Compiler        compiler;
    FFScope         scope;      // CHANGE 1: we use FFScope now
    SPExpression    expression;

    FileFilter( const String& expressionString )
    : compiler()
    , scope( compiler.CfgFormatter )
    {
        compiler.SetupDefaults();
        expression= compiler.Compile( expressionString );
        if( !expression->ResultType().IsType<bool>() )
            throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
    }

    bool Includes(const fs::directory_entry& directoryEntry)
    {
        // CHANGE 2: Store the given entry in our scope singleton which is then passed into
        //           Evaluated() as it was already before.
        scope.directoryEntry= &directoryEntry;

        return expression->Evaluate( scope ).Unbox<bool>();
    }
};
} // namespace step4
//! [DOX_ALIB_EXPR_TUT_FF_ScopeFF]


//###################################### STEP 5:  CompilerPlugin ############################################
#define FFCompilerPlugin FFCompilerPluginSkeleton

//! [DOX_ALIB_EXPR_TUT_FF_PluginSkeleton]
namespace step5 {
struct FFCompilerPlugin : public CompilerPlugin
{
    FFCompilerPlugin( Compiler& compiler )
    : CompilerPlugin( "FF Plug-in", compiler )
    {}

};
} // namespace step5
//! [DOX_ALIB_EXPR_TUT_FF_PluginSkeleton]

#undef FFCompilerPlugin

namespace step5 {
struct FFScope : public expressions::Scope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};


//! [DOX_ALIB_EXPR_TUT_FF_PluginCallback]
Box getName( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    // Create a copy of the string using the scope string allocator. This is done with utility
    // class ScopeString, which when returned, right away is boxed as a usual string,
    // aka char[]. Therefore, no intermediate string objects to be stored, neither the
    // std::string returned by "generic_u8string", nor the string.
    // Note, that this must be done within the call, because the object returned by
    // generic_u8string() will be instantly deleted and thus the buffer thrown away.
    return ScopeString( scope, 0,
                        dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().generic_u8string()   );
}
//! [DOX_ALIB_EXPR_TUT_FF_PluginCallback]

} // namespace step5

//! [DOX_ALIB_EXPR_TUT_FF_PluginOverride]
namespace step5 {
struct FFCompilerPlugin : public CompilerPlugin
{
    FFCompilerPlugin( Compiler& compiler )
    : CompilerPlugin( "FF Plug-in", compiler )
    {}

    // implement "TryCompilation" for functions
    virtual bool TryCompilation( CIFunction&  ciFunction )
    {
        // Is parameterless and function name equals "Name"?
        if(       ciFunction.QtyArgs() == 0
             &&   ciFunction.Name.Equals<Case::Ignore>( ASTR("Name") )    )
        {
            // set callback function, its return type and indicate success
            ciFunction.Callback     = getName;
            ciFunction.TypeOrValue  = Types::String;
            return true;
        }

        // For anything else, we are not responsible
        return false;
    }

};
} // namespace step5
//! [DOX_ALIB_EXPR_TUT_FF_PluginOverride]



//! [DOX_ALIB_EXPR_TUT_FF_PluginSkeleton2]
namespace step5 {
struct FileFilter
{
    Compiler            compiler;
    FFScope             scope;
    SPExpression        expression;
    FFCompilerPlugin    ffPlugin;     // CHANGE 1: We own an instance of our custom plug-in.

    FileFilter( const String& expressionString )
    : compiler()
    , scope( compiler.CfgFormatter )
    , ffPlugin( compiler )            // CHANGE 1: Initialize the plug-in with the compiler.
    {
        compiler.SetupDefaults();

        // CHANGE 2: Add our custom plug-in to the compiler prior to compiling the expression
        compiler.InsertPlugin( & ffPlugin, CompilePriorities::Custom );

        expression= compiler.Compile( expressionString );
        if( !expression->ResultType().IsType<bool>() )
            throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
    }

    bool Includes(const fs::directory_entry& directoryEntry)
    {
        scope.directoryEntry= &directoryEntry;
        return expression->Evaluate( scope ).Unbox<bool>();
    }
};
}; // namespace step5
//! [DOX_ALIB_EXPR_TUT_FF_PluginSkeleton2]

//###################################### STEP 6:  Calculus  ########################################

namespace step6 {
struct FFScope : public expressions::Scope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return ScopeString( scope, 0, dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().generic_u8string() );
}

//! [DOX_ALIB_EXPR_TUT_FF_PluginCalculus]
struct FFCompilerPlugin : public Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        Functions=
        {
            {  { ASTR("Name"), Case::Ignore, 0 }, // the function name, letter case min. abbreviation
                nullptr                         , // no arguments
                CALCULUS_CALLBACK(getName)      , // the callback, in debug mode, the name of the callback
                Types::String                   , // return type
                ETI                               // evaluation time invokeable only
            },
        };
    }
};
//! [DOX_ALIB_EXPR_TUT_FF_PluginCalculus]

struct FileFilter
{
    Compiler            compiler;
    FFScope             scope;
    SPExpression        expression;
    FFCompilerPlugin    ffPlugin;

    FileFilter( const String& expressionString )
    : compiler()
    , scope( compiler.CfgFormatter )
    , ffPlugin( compiler )
    {
        compiler.SetupDefaults();

        // CHANGE 2: Add our custom plug-in to the compiler prior to compiling the expression
        compiler.InsertPlugin( & ffPlugin, CompilePriorities::Custom );

        expression= compiler.Compile( expressionString );
        if( !expression->ResultType().IsType<bool>() )
            throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
    }

    bool Includes(const fs::directory_entry& directoryEntry)
    {
        scope.directoryEntry= &directoryEntry;
        return expression->Evaluate( scope ).Unbox<bool>();
    }
};
}; // namespace step6

//############################### STEP 7: Adding Identifiers  ######################################
namespace step7 {
struct FFScope : public expressions::Scope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return ScopeString( scope, 0, dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().generic_u8string() );
}

//! [DOX_ALIB_EXPR_TUT_FF_MoreIdentifiers]
Box isFolder( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? 0
            : static_cast<boxed_int>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    return DateTime::FromEpochSeconds( decltype(fsTime)::clock::to_time_t(fsTime) );
}

Box getPerm ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return static_cast<boxed_int>( dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions() );
}
//! [DOX_ALIB_EXPR_TUT_FF_MoreIdentifiers]

//! [DOX_ALIB_EXPR_TUT_FF_MoreIdentifiersConstants]
Box constOwnRead  = static_cast<boxed_int>( EnumValue( fs::perms::owner_read  ) );
Box constOwnWrite = static_cast<boxed_int>( EnumValue( fs::perms::owner_write ) );
Box constOwnExec  = static_cast<boxed_int>( EnumValue( fs::perms::owner_exec  ) );
Box constGrpRead  = static_cast<boxed_int>( EnumValue( fs::perms::group_read  ) );
Box constGrpWrite = static_cast<boxed_int>( EnumValue( fs::perms::group_write ) );
Box constGrpExec  = static_cast<boxed_int>( EnumValue( fs::perms::group_exec  ) );
Box constOthRead  = static_cast<boxed_int>( EnumValue( fs::perms::others_read ) );
Box constOthWrite = static_cast<boxed_int>( EnumValue( fs::perms::others_write) );
Box constOthExec  = static_cast<boxed_int>( EnumValue( fs::perms::others_exec ) );
//! [DOX_ALIB_EXPR_TUT_FF_MoreIdentifiersConstants]

//! [DOX_ALIB_EXPR_TUT_FF_MoreIdentifiersPlugin]
struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        ConstantIdentifiers=
        {
          { { ASTR("OwnerRead")    , Case::Ignore, 6} , constOwnRead  },
          { { ASTR("OwnerWrite")   , Case::Ignore, 6} , constOwnWrite },
          { { ASTR("OwnerExecute") , Case::Ignore, 6} , constOwnExec  },
          { { ASTR("GroupRead")    , Case::Ignore, 6} , constGrpRead  },
          { { ASTR("GroupWrite")   , Case::Ignore, 6} , constGrpWrite },
          { { ASTR("GroupExecute") , Case::Ignore, 6} , constGrpExec  },
          { { ASTR("OthersRead")   , Case::Ignore, 7} , constOthRead  },
          { { ASTR("OthersWrite")  , Case::Ignore, 7} , constOthWrite },
          { { ASTR("OthersExecute"), Case::Ignore, 7} , constOthExec  },
        };

        Functions=
        {
          { {ASTR("Name")         , Case::Ignore, 0}, nullptr, CALCULUS_CALLBACK(getName ), Types::String   , ETI },
          { {ASTR("IsDirectory")  , Case::Ignore, 3}, nullptr, CALCULUS_CALLBACK(isFolder), Types::Boolean  , ETI },
          { {ASTR("Size")         , Case::Ignore, 0}, nullptr, CALCULUS_CALLBACK(getSize ), Types::Integer  , ETI },
          { {ASTR("Date")         , Case::Ignore, 0}, nullptr, CALCULUS_CALLBACK(getDate ), Types::DateTime, ETI },
          { {ASTR("Permissions")  , Case::Ignore, 4}, nullptr, CALCULUS_CALLBACK(getPerm ), Types::Integer  , ETI },
        };
    }
};
//! [DOX_ALIB_EXPR_TUT_FF_MoreIdentifiersPlugin]

struct FileFilter
{
    Compiler            compiler;
    FFScope             scope;
    SPExpression        expression;
    FFCompilerPlugin    ffPlugin;

    FileFilter( const String& expressionString )
    : compiler()
    , scope( compiler.CfgFormatter )
    , ffPlugin( compiler )
    {
        compiler.SetupDefaults();
        compiler.InsertPlugin( & ffPlugin, CompilePriorities::Custom );
        expression= compiler.Compile( expressionString );
        if( !expression->ResultType().IsType<bool>() )
            throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
    }

    bool Includes(const fs::directory_entry& directoryEntry)
    {
        scope.directoryEntry= &directoryEntry;
        return expression->Evaluate( scope ).Unbox<bool>();
    }
};
}; // namespace step7

//############################### STEP 8: Adding Functions  ######################################
namespace step8 {
struct FFScope : public expressions::Scope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return ScopeString( scope, 0, dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().generic_u8string() );
}

Box isFolder( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? 0
            : static_cast<boxed_int>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    return DateTime::FromEpochSeconds( decltype(fsTime)::clock::to_time_t(fsTime) );
}

Box getPerm ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return static_cast<boxed_int>( dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions() );
}

//! [DOX_ALIB_EXPR_TUT_FF_Functions]
Box kiloBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024;
}
Box megaBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024 * 1024;
}
Box gigaBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024 * 1024 * 1024;
}
//! [DOX_ALIB_EXPR_TUT_FF_Functions]

Box constOwnRead  = static_cast<boxed_int>( EnumValue( fs::perms::owner_read  ) );
Box constOwnWrite = static_cast<boxed_int>( EnumValue( fs::perms::owner_write ) );
Box constOwnExec  = static_cast<boxed_int>( EnumValue( fs::perms::owner_exec  ) );
Box constGrpRead  = static_cast<boxed_int>( EnumValue( fs::perms::group_read  ) );
Box constGrpWrite = static_cast<boxed_int>( EnumValue( fs::perms::group_write ) );
Box constGrpExec  = static_cast<boxed_int>( EnumValue( fs::perms::group_exec  ) );
Box constOthRead  = static_cast<boxed_int>( EnumValue( fs::perms::others_read ) );
Box constOthWrite = static_cast<boxed_int>( EnumValue( fs::perms::others_write) );
Box constOthExec  = static_cast<boxed_int>( EnumValue( fs::perms::others_exec ) );

//! [DOX_ALIB_EXPR_TUT_FF_FunctionSignature]
std::vector<Box>  SIG_INT;
//! [DOX_ALIB_EXPR_TUT_FF_FunctionSignature]


//! [DOX_ALIB_EXPR_TUT_FF_FunctionsPlugin]
struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        ConstantIdentifiers=
        {
          { {ASTR("OwnerRead")    , Case::Ignore, 6}, constOwnRead  },
          { {ASTR("OwnerWrite")   , Case::Ignore, 6}, constOwnWrite },
          { {ASTR("OwnerExecute") , Case::Ignore, 6}, constOwnExec  },
          { {ASTR("GroupRead")    , Case::Ignore, 6}, constGrpRead  },
          { {ASTR("GroupWrite")   , Case::Ignore, 6}, constGrpWrite },
          { {ASTR("GroupExecute") , Case::Ignore, 6}, constGrpExec  },
          { {ASTR("OthersRead")   , Case::Ignore, 7}, constOthRead  },
          { {ASTR("OthersWrite")  , Case::Ignore, 7}, constOthWrite },
          { {ASTR("OthersExecute"), Case::Ignore, 7}, constOthExec  },
        };

        // initialize the function signature vector
        SIG_INT= { Types::Integer   };

        Functions=
        {
          { {ASTR("Name")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getName   ), Types::String   , ETI },
          { {ASTR("IsDirectory")  , Case::Ignore, 3}, nullptr , CALCULUS_CALLBACK(isFolder  ), Types::Boolean  , ETI },
          { {ASTR("Size")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getSize   ), Types::Integer  , ETI },
          { {ASTR("Date")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getDate   ), Types::DateTime, ETI },
          { {ASTR("Permissions")  , Case::Ignore, 4}, nullptr , CALCULUS_CALLBACK(getPerm   ), Types::Integer  , ETI },

          // the new functions:
          { {ASTR("Kilobytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(kiloBytes ), Types::Integer  , CTI },
          { {ASTR("Megabytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(megaBytes ), Types::Integer  , CTI },
          { {ASTR("Gigabytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(gigaBytes ), Types::Integer  , CTI },
        };
    }
};
//! [DOX_ALIB_EXPR_TUT_FF_FunctionsPlugin]

struct FileFilter
{
    Compiler            compiler;
    FFScope             scope;
    SPExpression        expression;
    FFCompilerPlugin    ffPlugin;

    FileFilter( const String& expressionString )
    : compiler()
    , scope( compiler.CfgFormatter )
    , ffPlugin( compiler )
    {
        compiler.SetupDefaults();
        compiler.InsertPlugin( & ffPlugin, CompilePriorities::Custom );
        expression= compiler.Compile( expressionString );
        if( !expression->ResultType().IsType<bool>() )
            throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
    }

    bool Includes(const fs::directory_entry& directoryEntry)
    {
        scope.directoryEntry= &directoryEntry;
        return expression->Evaluate( scope ).Unbox<bool>();
    }
};
}; // namespace step8

//############################### STEP 9: adding custom types  ######################################
namespace step9 {
struct FFScope : public expressions::Scope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return ScopeString( scope, 0, dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().generic_u8string() );
}

Box isFolder( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? 0
            : static_cast<boxed_int>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    return DateTime::FromEpochSeconds( decltype(fsTime)::clock::to_time_t(fsTime) );
}

Box kiloBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024;
}
Box megaBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024 * 1024;
}
Box gigaBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024 * 1024 * 1024;
}
//! [DOX_ALIB_EXPR_TUT_FF_PermType]
Box getPerm ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions();
}

Box constOwnRead  =  fs::perms::owner_read;
Box constOwnWrite =  fs::perms::owner_write;
Box constOwnExec  =  fs::perms::owner_exec;
Box constGrpRead  =  fs::perms::group_read;
Box constGrpWrite =  fs::perms::group_write;
Box constGrpExec  =  fs::perms::group_exec;
Box constOthRead  =  fs::perms::others_read;
Box constOthWrite =  fs::perms::others_write;
Box constOthExec  =  fs::perms::others_exec;
//! [DOX_ALIB_EXPR_TUT_FF_PermType]

//! [DOX_ALIB_EXPR_TUT_FF_PermTypeSampleBox]
Box TypePermission = fs::perms::owner_read;   // ...could be any other enum element as well!
//! [DOX_ALIB_EXPR_TUT_FF_PermTypeSampleBox]

std::vector<Box>  SIG_INT;


//! [DOX_ALIB_EXPR_TUT_FF_PermTypePlugin]
struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        ConstantIdentifiers=
        {
          { {ASTR("OwnerRead")    , Case::Ignore, 6}, constOwnRead  },
          { {ASTR("OwnerWrite")   , Case::Ignore, 6}, constOwnWrite },
          { {ASTR("OwnerExecute") , Case::Ignore, 6}, constOwnExec  },
          { {ASTR("GroupRead")    , Case::Ignore, 6}, constGrpRead  },
          { {ASTR("GroupWrite")   , Case::Ignore, 6}, constGrpWrite },
          { {ASTR("GroupExecute") , Case::Ignore, 6}, constGrpExec  },
          { {ASTR("OthersRead")   , Case::Ignore, 7}, constOthRead  },
          { {ASTR("OthersWrite")  , Case::Ignore, 7}, constOthWrite },
          { {ASTR("OthersExecute"), Case::Ignore, 7}, constOthExec  },
        };

        // initialize the function signature vector
        SIG_INT= { Types::Integer   };

        Functions=
        {
          { {ASTR("Name")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getName   ), Types::String   , ETI },
          { {ASTR("IsDirectory")  , Case::Ignore, 3}, nullptr , CALCULUS_CALLBACK(isFolder  ), Types::Boolean  , ETI },
          { {ASTR("Size")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getSize   ), Types::Integer  , ETI },
          { {ASTR("Date")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getDate   ), Types::DateTime, ETI },

          // change return type to TypePermission
          { {ASTR("Permissions")  , Case::Ignore, 4}, nullptr , CALCULUS_CALLBACK(getPerm   ), TypePermission  , ETI },

          { {ASTR("Kilobytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(kiloBytes ), Types::Integer  , CTI },
          { {ASTR("Megabytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(megaBytes ), Types::Integer  , CTI },
          { {ASTR("Gigabytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(gigaBytes ), Types::Integer  , CTI },
        };
    }
};
//! [DOX_ALIB_EXPR_TUT_FF_PermTypePlugin]

struct FileFilter
{
    Compiler            compiler;
    FFScope             scope;
    SPExpression        expression;
    FFCompilerPlugin    ffPlugin;

    FileFilter( const String& expressionString )
    : compiler()
    , scope( compiler.CfgFormatter )
    , ffPlugin( compiler )
    {
        compiler.SetupDefaults();
        compiler.InsertPlugin( & ffPlugin, CompilePriorities::Custom );
        expression= compiler.Compile( expressionString );
        if( !expression->ResultType().IsType<bool>() )
            throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
    }

    bool Includes(const fs::directory_entry& directoryEntry)
    {
        scope.directoryEntry= &directoryEntry;
        return expression->Evaluate( scope ).Unbox<bool>();
    }
};
}; // namespace step9


//############################### STEP 10: Announcing custom types ##################################
namespace step10 {
struct FFScope : public expressions::Scope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return ScopeString( scope, 0, dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().generic_u8string() );
}

Box isFolder( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? 0
            : static_cast<boxed_int>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    return DateTime::FromEpochSeconds( decltype(fsTime)::clock::to_time_t(fsTime) );
}

Box kiloBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024;
}
Box megaBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024 * 1024;
}
Box gigaBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024 * 1024 * 1024;
}
Box getPerm ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions();
}

Box constOwnRead  =  fs::perms::owner_read;
Box constOwnWrite =  fs::perms::owner_write;
Box constOwnExec  =  fs::perms::owner_exec;
Box constGrpRead  =  fs::perms::group_read;
Box constGrpWrite =  fs::perms::group_write;
Box constGrpExec  =  fs::perms::group_exec;
Box constOthRead  =  fs::perms::others_read;
Box constOthWrite =  fs::perms::others_write;
Box constOthExec  =  fs::perms::others_exec;

Box TypePermission = fs::perms::owner_read;

std::vector<Box>  SIG_INT;


struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
//! [DOX_ALIB_EXPR_TUT_FF_PermTypeAddTypeDef]
compiler.AddType( TypePermission, "Permission" );
//! [DOX_ALIB_EXPR_TUT_FF_PermTypeAddTypeDef]

        ConstantIdentifiers=
        {
          { {ASTR("OwnerRead")    , Case::Ignore, 6}, constOwnRead  },
          { {ASTR("OwnerWrite")   , Case::Ignore, 6}, constOwnWrite },
          { {ASTR("OwnerExecute") , Case::Ignore, 6}, constOwnExec  },
          { {ASTR("GroupRead")    , Case::Ignore, 6}, constGrpRead  },
          { {ASTR("GroupWrite")   , Case::Ignore, 6}, constGrpWrite },
          { {ASTR("GroupExecute") , Case::Ignore, 6}, constGrpExec  },
          { {ASTR("OthersRead")   , Case::Ignore, 7}, constOthRead  },
          { {ASTR("OthersWrite")  , Case::Ignore, 7}, constOthWrite },
          { {ASTR("OthersExecute"), Case::Ignore, 7}, constOthExec  },
        };

        // initialize the function signature vector
        SIG_INT= { Types::Integer   };

        Functions=
        {
          { {ASTR("Name")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getName   ), Types::String   , ETI },
          { {ASTR("IsDirectory")  , Case::Ignore, 3}, nullptr , CALCULUS_CALLBACK(isFolder  ), Types::Boolean  , ETI },
          { {ASTR("Size")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getSize   ), Types::Integer  , ETI },
          { {ASTR("Date")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getDate   ), Types::DateTime, ETI },
          { {ASTR("Permissions")  , Case::Ignore, 4}, nullptr , CALCULUS_CALLBACK(getPerm   ), TypePermission  , ETI },
          { {ASTR("Kilobytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(kiloBytes ), Types::Integer  , CTI },
          { {ASTR("Megabytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(megaBytes ), Types::Integer  , CTI },
          { {ASTR("Gigabytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(gigaBytes ), Types::Integer  , CTI },
        };
    }
};

struct FileFilter
{
    Compiler            compiler;
    FFScope             scope;
    SPExpression        expression;
    FFCompilerPlugin    ffPlugin;

    FileFilter( const String& expressionString )
    : compiler()
    , scope( compiler.CfgFormatter )
    , ffPlugin( compiler )
    {
        compiler.SetupDefaults();
        compiler.InsertPlugin( & ffPlugin, CompilePriorities::Custom );
        expression= compiler.Compile( expressionString );
        if( !expression->ResultType().IsType<bool>() )
            throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
    }

    bool Includes(const fs::directory_entry& directoryEntry)
    {
        scope.directoryEntry= &directoryEntry;
        return expression->Evaluate( scope ).Unbox<bool>();
    }
};
}; // namespace step10

//############################### STEP 11: custom operators ##################################
namespace step11 {
struct FFScope : public expressions::Scope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return ScopeString( scope, 0, dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().generic_u8string() );
}

Box isFolder( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? 0
            : static_cast<boxed_int>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    return DateTime::FromEpochSeconds( decltype(fsTime)::clock::to_time_t(fsTime) );
}

Box kiloBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024;
}
Box megaBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024 * 1024;
}
Box gigaBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024 * 1024 * 1024;
}
Box getPerm ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions();
}
//! [DOX_ALIB_EXPR_TUT_FF_PermTypeOperatorCallbacks]
Box opPermAnd( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return    argsBegin     ->Unbox<std::experimental::filesystem::v1::perms>()
           & (argsBegin + 1)->Unbox<std::experimental::filesystem::v1::perms>();
}
Box opPermOr ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return    argsBegin     ->Unbox<std::experimental::filesystem::v1::perms>()
           | (argsBegin + 1)->Unbox<std::experimental::filesystem::v1::perms>();
}
Box opPermXOr( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return    argsBegin     ->Unbox<std::experimental::filesystem::v1::perms>()
           ^ (argsBegin + 1)->Unbox<std::experimental::filesystem::v1::perms>();
}
Box opPermEq ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return    argsBegin     ->Unbox<std::experimental::filesystem::v1::perms>()
           ==(argsBegin + 1)->Unbox<std::experimental::filesystem::v1::perms>();
}
Box opPermNEq( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return    argsBegin     ->Unbox<std::experimental::filesystem::v1::perms>()
           !=(argsBegin + 1)->Unbox<std::experimental::filesystem::v1::perms>();
}
//! [DOX_ALIB_EXPR_TUT_FF_PermTypeOperatorCallbacks]

Box constOwnRead  =  fs::perms::owner_read;
Box constOwnWrite =  fs::perms::owner_write;
Box constOwnExec  =  fs::perms::owner_exec;
Box constGrpRead  =  fs::perms::group_read;
Box constGrpWrite =  fs::perms::group_write;
Box constGrpExec  =  fs::perms::group_exec;
Box constOthRead  =  fs::perms::others_read;
Box constOthWrite =  fs::perms::others_write;
Box constOthExec  =  fs::perms::others_exec;

Box TypePermission = fs::perms::owner_read;

std::vector<Box>  SIG_INT;

//! [DOX_ALIB_EXPR_TUT_FF_PermTypeOperatorTable]
#if !defined(_MSC_VER)
constexpr   // This leads to a linker error on MSC.
            // Does not find ALIB_API symbol Types::Boolean when this table is constexpr. Strange!
#endif
aworx::lib::expressions::plugins::Calculus::BinaryOpTableEntry  binaryOpTable[] =
{
    { ASTR("&") , TypePermission, TypePermission, CALCULUS_CALLBACK( opPermAnd ), TypePermission , Calculus::CTI },
    { ASTR("|") , TypePermission, TypePermission, CALCULUS_CALLBACK( opPermOr  ), TypePermission , Calculus::CTI },
    { ASTR("^") , TypePermission, TypePermission, CALCULUS_CALLBACK( opPermXOr ), TypePermission , Calculus::CTI },
    { ASTR("=="), TypePermission, TypePermission, CALCULUS_CALLBACK( opPermEq  ), Types::Boolean , Calculus::CTI },
    { ASTR("!="), TypePermission, TypePermission, CALCULUS_CALLBACK( opPermNEq ), Types::Boolean , Calculus::CTI },
};
//! [DOX_ALIB_EXPR_TUT_FF_PermTypeOperatorTable]

struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        compiler.AddType( TypePermission, "Permission" );

        ConstantIdentifiers=
        {
          { {ASTR("OwnerRead")    , Case::Ignore, 6} , constOwnRead  },
          { {ASTR("OwnerWrite")   , Case::Ignore, 6} , constOwnWrite },
          { {ASTR("OwnerExecute") , Case::Ignore, 6} , constOwnExec  },
          { {ASTR("GroupRead")    , Case::Ignore, 6} , constGrpRead  },
          { {ASTR("GroupWrite")   , Case::Ignore, 6} , constGrpWrite },
          { {ASTR("GroupExecute") , Case::Ignore, 6} , constGrpExec  },
          { {ASTR("OthersRead")   , Case::Ignore, 7} , constOthRead  },
          { {ASTR("OthersWrite")  , Case::Ignore, 7} , constOthWrite },
          { {ASTR("OthersExecute"), Case::Ignore, 7} , constOthExec  },
        };

        SIG_INT= { Types::Integer   };

        Functions=
        {
          { {ASTR("Name")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getName   ), Types::String   , Calculus::ETI },
          { {ASTR("IsDirectory")  , Case::Ignore, 3}, nullptr , CALCULUS_CALLBACK(isFolder  ), Types::Boolean  , Calculus::ETI },
          { {ASTR("Size")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getSize   ), Types::Integer  , Calculus::ETI },
          { {ASTR("Date")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getDate   ), Types::DateTime, Calculus::ETI },
          { {ASTR("Permissions")  , Case::Ignore, 4}, nullptr , CALCULUS_CALLBACK(getPerm   ), TypePermission  , Calculus::ETI },
          { {ASTR("Kilobytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(kiloBytes ), Types::Integer  , Calculus::CTI },
          { {ASTR("Megabytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(megaBytes ), Types::Integer  , Calculus::CTI },
          { {ASTR("Gigabytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(gigaBytes ), Types::Integer  , Calculus::CTI },
        };

//! [DOX_ALIB_EXPR_TUT_FF_PermTypeFeedTable]
AddBinaryOps( binaryOpTable );
//! [DOX_ALIB_EXPR_TUT_FF_PermTypeFeedTable]
    }
};

struct FileFilter
{
    Compiler            compiler;
    FFScope             scope;
    SPExpression        expression;
    FFCompilerPlugin    ffPlugin;

    FileFilter( const String& expressionString )
    : compiler()
    , scope( compiler.CfgFormatter )
    , ffPlugin( compiler )
    {
        compiler.SetupDefaults();
        compiler.InsertPlugin( & ffPlugin, CompilePriorities::Custom );
        expression= compiler.Compile( expressionString );
        if( !expression->ResultType().IsType<bool>() )
            throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
    }

    bool Includes(const fs::directory_entry& directoryEntry)
    {
        scope.directoryEntry= &directoryEntry;
        return expression->Evaluate( scope ).Unbox<bool>();
    }
};
}; // namespace step11

//############################### STEP 12: auto cast ##################################
namespace step12 {
struct FFScope : public expressions::Scope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return ScopeString( scope, 0, dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().generic_u8string() );
}

Box isFolder( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? 0
            : static_cast<boxed_int>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    return DateTime::FromEpochSeconds( decltype(fsTime)::clock::to_time_t(fsTime) );
}

Box kiloBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024;
}
Box megaBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024 * 1024;
}
Box gigaBytes ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<boxed_int>() * 1024 * 1024 * 1024;
}
Box getPerm ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions();
}

//! [DOX_ALIB_EXPR_TUT_FF_AutoCastCallback]
Box perm2Int ( expressions::Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return static_cast<boxed_int>( argsBegin->Unbox<fs::perms>() );
}
//! [DOX_ALIB_EXPR_TUT_FF_AutoCastCallback]


Box constOwnRead  =  fs::perms::owner_read;
Box constOwnWrite =  fs::perms::owner_write;
Box constOwnExec  =  fs::perms::owner_exec;
Box constGrpRead  =  fs::perms::group_read;
Box constGrpWrite =  fs::perms::group_write;
Box constGrpExec  =  fs::perms::group_exec;
Box constOthRead  =  fs::perms::others_read;
Box constOthWrite =  fs::perms::others_write;
Box constOthExec  =  fs::perms::others_exec;

Box TypePermission = fs::perms::owner_read;

std::vector<Box>  SIG_INT;

struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        compiler.AddType( TypePermission, "Permission" );

        ConstantIdentifiers=
        {
          { {ASTR("OwnerRead")    , Case::Ignore, 6} , constOwnRead  },
          { {ASTR("OwnerWrite")   , Case::Ignore, 6} , constOwnWrite },
          { {ASTR("OwnerExecute") , Case::Ignore, 6} , constOwnExec  },
          { {ASTR("GroupRead")    , Case::Ignore, 6} , constGrpRead  },
          { {ASTR("GroupWrite")   , Case::Ignore, 6} , constGrpWrite },
          { {ASTR("GroupExecute") , Case::Ignore, 6} , constGrpExec  },
          { {ASTR("OthersRead")   , Case::Ignore, 7} , constOthRead  },
          { {ASTR("OthersWrite")  , Case::Ignore, 7} , constOthWrite },
          { {ASTR("OthersExecute"), Case::Ignore, 7} , constOthExec  },
        };

        SIG_INT= { Types::Integer   };

        Functions=
        {
          { {ASTR("Name")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getName   ), Types::String   , Calculus::ETI },
          { {ASTR("IsDirectory")  , Case::Ignore, 3}, nullptr , CALCULUS_CALLBACK(isFolder  ), Types::Boolean  , Calculus::ETI },
          { {ASTR("Size")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getSize   ), Types::Integer  , Calculus::ETI },
          { {ASTR("Date")         , Case::Ignore, 0}, nullptr , CALCULUS_CALLBACK(getDate   ), Types::DateTime, Calculus::ETI },
          { {ASTR("Permissions")  , Case::Ignore, 4}, nullptr , CALCULUS_CALLBACK(getPerm   ), TypePermission  , Calculus::ETI },
          { {ASTR("Kilobytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(kiloBytes ), Types::Integer  , Calculus::CTI },
          { {ASTR("Megabytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(megaBytes ), Types::Integer  , Calculus::CTI },
          { {ASTR("Gigabytes")    , Case::Ignore, 4}, &SIG_INT, CALCULUS_CALLBACK(gigaBytes ), Types::Integer  , Calculus::CTI },
        };

    }
//! [DOX_ALIB_EXPR_TUT_FF_AutoCastCompilation]
virtual bool TryCompilation( CIBinaryAutoCast& ciAutoCast )    override
{
    // we don't cast for conditional operator "Q ? T : F"
    if( ciAutoCast.BinaryOp.IsNull() )
        return false;

    // cast LHS
    if( ciAutoCast.ArgsBegin->IsType<fs::perms>() )
    {
        if( ciAutoCast.LhsIsConst )
        {
            // compile-time invocation
            ciAutoCast.TypeOrValue= perm2Int( ciAutoCast.CompileTimeScope,
                                              ciAutoCast.ArgsBegin,
                                              ciAutoCast.ArgsEnd           );
        }
        else
        {
            ciAutoCast.Callback    = perm2Int;
            ciAutoCast.TypeOrValue = Types::Integer;
            ALIB_DBG( ciAutoCast.DbgCallBackName= "perm2Int"; )
        }
    }

    // cast RHS
    if( (ciAutoCast.ArgsBegin + 1)->IsType<fs::perms>() )
    {
        if( ciAutoCast.RhsIsConst )
        {
            // compile-time invocation
            ciAutoCast.TypeOrValueRhs= perm2Int( ciAutoCast.CompileTimeScope,
                                                 ciAutoCast.ArgsBegin + 1,
                                                 ciAutoCast.ArgsEnd           );
        }
        else
        {
            ciAutoCast.CallbackRhs    = perm2Int;
            ciAutoCast.TypeOrValueRhs = Types::Integer;
            ALIB_DBG( ciAutoCast.DbgCallBackNameRhs= "perm2Int"; )
        }
    }

    return true;
}

//! [DOX_ALIB_EXPR_TUT_FF_AutoCastCompilation]
};

struct FileFilter
{
    Compiler            compiler;
    FFScope             scope;
    SPExpression        expression;
    FFCompilerPlugin    ffPlugin;

    FileFilter( const String& expressionString )
    : compiler()
    , scope( compiler.CfgFormatter )
    , ffPlugin( compiler )
    {
        compiler.SetupDefaults();
        compiler.InsertPlugin( & ffPlugin, CompilePriorities::Custom );
        expression= compiler.Compile( expressionString );
        if( !expression->ResultType().IsType<bool>() )
            throw std::runtime_error( "Expression result type mismatch: expecting boolean result!" );
    }

    bool Includes(const fs::directory_entry& directoryEntry)
    {
        scope.directoryEntry= &directoryEntry;
        return expression->Evaluate( scope ).Unbox<bool>();
    }
};
}; // namespace step12


} // anonymous namespace

ALIB_WARNINGS_RESTORE



// #################################################################################################
// #### Unit test executing tutorial code
// #################################################################################################

namespace ut_aworx {

// #################################################################################################
// ### Test method used by the test below, with two macros. One for expressions understood by
// ### C++, one for those that are not compatible.
// #################################################################################################
//#define   CCOMP_CONSTEXPR(expr)               testExpression( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), expr  , 1 );
//#define   CONSTEXPR(expr, result)             testExpression( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), result, 1 );
#define   EXPRESSION(expr, result,expProgLen) testExpression( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), result, expProgLen );

extern
SPExpression testExpression( const NTString& file, int line, const NTString& func,
                             AWorxUnitTesting&     ut,
                             Compiler&             compiler,
                             expressions::Scope&   scope,
                             const String&         expressionString,
                             const Box&            expected,
                             integer               programLength                               );

//#if !ALIB_DEBUG
//    #define      PRINTPRGRM(expr)
//#else
//    #define      PRINTPRGRM(expr) printProgram( ALIB_CALLER, ut, compiler, ALIB_STRINGIFY(expr) );
//
//extern
//SPExpression printProgram( const NTString& file, int line, const NTString& func,
//                           AWorxUnitTesting&  ut,
//                           Compiler&          compiler,
//                           const String&      expressionString         );
//#endif



UT_CLASS()


UT_METHOD( FileSystemIntro )
{
    UT_INIT();
    {
        #if defined( _WIN32 )
const char* sourceDir= "../../../../src/alib/expressions/";

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    cout << directoryEntry.path().filename().generic_u8string() << endl;
        #else
//! [DOX_ALIB_EXPR_TUT_FF_1]
const char* sourceDir= "../../../../../src/alib/expressions/";

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    cout << directoryEntry.path().filename().generic_u8string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_1]
        #endif

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_INTRO-1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


//! [DOX_ALIB_EXPR_TUT_FF_FilterLoop]
step1::FileFilter filter(ASTR("expression string syntax not defined yet"));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( filter.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().generic_u8string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_FilterLoop]
    testOutputStreamN.str("");


//! [DOX_ALIB_EXPR_TUT_FF_TRUE]
cout << "--- Files using expression {true}: ---" << endl;

step2::FileFilter trueFilter(ASTR("true"));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( trueFilter.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().generic_u8string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_TRUE]

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_TRUE.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

//! [DOX_ALIB_EXPR_TUT_FF_FALSE]
cout << "--- Files using expression {false}: ---" << endl;

step2::FileFilter falseFilter(ASTR("false"));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( falseFilter.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().generic_u8string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_FALSE]

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_FALSE.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


//! [DOX_ALIB_EXPR_TUT_FF_Name1]
cout << "--- Files using expression {name == \"compiler.hpp\"}: ---" << endl;

step5::FileFilter filter1(ASTR("name == \"compiler.hpp\""));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( filter1.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().generic_u8string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_Name1]

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Name1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");



//! [DOX_ALIB_EXPR_TUT_FF_Name2]
cout << "--- Files using expression {WildcardMatch(name, \"*.hpp\"}: ---" << endl;

step5::FileFilter filter2(ASTR("WildcardMatch(name, \"*.hpp\")"));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( filter2.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().generic_u8string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_Name2]

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Name2.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

//! [DOX_ALIB_EXPR_TUT_FF_Name3]
cout << "--- Files using expression {name * \"*.cpp\"}: ---" << endl;

step5::FileFilter filter3(ASTR("name * \"*.cpp\""));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( filter3.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().generic_u8string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_Name3]

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Name3.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


    // test if step6 implementation works
    int cnt= 0;
    step6::FileFilter filter61(ASTR("name * \"compiler.hpp\""));
    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter61.Includes( directoryEntry ) )
            cnt++;
    UT_EQ(1, cnt);

    cnt= 0;
    step6::FileFilter filter62(ASTR("name * \"*.cpp\""));
    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter62.Includes( directoryEntry ) )
            cnt++;
    UT_EQ(3, cnt);

    cnt= 0;
    step6::FileFilter filter63(ASTR("name * \"*.hpp\""));
    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter63.Includes( directoryEntry ) )
            cnt++;
    UT_EQ(5, cnt);

    //---------------- samples after more  functionality was added ------------------
    cout << "--- Filter Expression {IsDirectory}: ---" << endl;
    step7::FileFilter filter71(ASTR("IsDirectory"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter71.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().generic_u8string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_More-1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    cout << "--- Filter Expression {!IsDirectory && size < 20000}: ---" << endl;
    step7::FileFilter filter72(ASTR("!IsDirectory && size < 20000"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter72.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().generic_u8string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_More-2.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    cout << "--- Filter Expression {date > DateTime(2018,2,5)}: ---" << endl;
    step7::FileFilter filter73(ASTR("date > DateTime(2018,2,5)"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter73.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().generic_u8string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_More-3.txt", testOutputStreamN.str()) ;
    testOutputStreamN.str("");

    cout << "--- Filter Expression {(permissions & OwnerExecute) != 0}: ---" << endl;
    step7::FileFilter filter74(ASTR("(permissions & OwnerExecute) != 0"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter74.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().generic_u8string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_More-4.txt", testOutputStreamN.str()) ;
    testOutputStreamN.str("");

    cout << "--- Filter Expression {size > 81920}: ---" << endl;
    step7::FileFilter filter75(ASTR("size > 81920"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter75.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().generic_u8string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_More-5.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


    //---------------- samples after more  functionality was added ------------------
    cout << "--- Filter Expression {size > kilobytes(80)}: ---" << endl;
    step8::FileFilter filter81(ASTR("size > kilobytes(80)"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter81.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().generic_u8string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Func-1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    //---------------- samples  added fs::permissions type ------------------
    cout << "--- Filter Expression {(permissions & OwnerExecute) != 0}: ---" << endl;
    try
    {
        step9::FileFilter filter91(ASTR("(permissions & OwnerExecute) != 0"));
    }
    catch(Exception& e)
    {
        SimpleText simpleText;
        simpleText.AddException( e );
        ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Operators-1.txt", simpleText.Text );
    }

    //---------------- samples  added fs::permissions type: better exception ------------------
    cout << "--- Filter Expression {(permissions & OwnerExecute) != 0}: ---" << endl;
    try
    {
        step10::FileFilter filter101(ASTR("(permissions & OwnerExecute) != 0"));
    }
    catch(Exception& e)
    {
        SimpleText simpleText;
        simpleText.AddException( e );
        ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Operators-2.txt", simpleText.Text );
    }

    //---------------- samples after adding operators ------------------
    testOutputStreamN.str("");
    cout << "--- Filter Expression {(permissions & OwnerExecute) == OwnerExecute}: ---" << endl;
    step11::FileFilter filter111(ASTR("(permissions & OwnerExecute) == OwnerExecute"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter111.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().generic_u8string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Func-3.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    //---------------- samples after adding auto casts ------------------
    testOutputStreamN.str("");
    cout << "--- Filter Expression {(permissions & OwnerExecute) == OwnerExecute}: ---" << endl;
    step12::FileFilter filter121(ASTR("(permissions & OwnerExecute) == OwnerExecute"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter121.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().generic_u8string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Func-4.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    cout << "--- Filter Expression {(permissions & 64) != 0}: ---" << endl;
    step12::FileFilter filter122(ASTR("(permissions & 64) != 0"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter122.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().generic_u8string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Func-5.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");
}

    {
//! [DOX_ALIB_EXPR_EmptyCompiler]
Compiler compiler;
compiler.CfgBuiltInPlugins= Compiler::BuiltInPlugins::NONE;
compiler.SetupDefaults();
//! [DOX_ALIB_EXPR_EmptyCompiler]

    }
    {
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope(compiler.CfgFormatter);
EXPRESSION(
//! [DOX_ALIB_EXPR_PROS_NASTY]
Format("Result: {}", GetDayOfWeek( today + Years(42) ) * int( remainder( PI * exp( sin( E ) ), 1.2345) * random ) % 7 ) != ""
//! [DOX_ALIB_EXPR_PROS_NASTY]
, true, 15 )
    }
}

// #################################################################################################
// ### Operators
// #################################################################################################
UT_METHOD( Operators )
{
    UT_INIT();

    testOutputStreamN.str("");

    //----------------- Ops 1 --------------------
try {
//! [DOX_ALIB_EXPR_OPS_1]
Compiler compiler;
compiler.SetupDefaults();

lib::expressions::Scope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile( ASTR(R"( "Hexadecimal: 0x{:x}" {} 42"  )") );
//! [DOX_ALIB_EXPR_OPS_1]
}
    catch(Exception& e)
    {
        SimpleText simpleText;
        simpleText.AddException( e );
        ut.WriteResultFile( "DOX_ALIB_EXPR_OPS_1.txt", simpleText.Text );
        testOutputStreamN.str("");
    }

    //----------------- Ops 2 --------------------
try {
//! [DOX_ALIB_EXPR_OPS_2]
Compiler compiler;
compiler.SetupDefaults();

compiler.AddBinaryOperator( ASTR("{}") , 900);

lib::expressions::Scope scope( compiler.CfgFormatter );
SPExpression expression= compiler.Compile( ASTR(R"( "Hexadecimal: 0x{:x}" {} 42  )") );
//! [DOX_ALIB_EXPR_OPS_2]
}
    catch(Exception& e)
    {
        SimpleText simpleText;
        simpleText.AddException( e );
        ut.WriteResultFile( "DOX_ALIB_EXPR_OPS_2.txt", simpleText.Text );
        testOutputStreamN.str("");
    }


    //----------------- Ops 3 --------------------
try {
//! [DOX_ALIB_EXPR_OPS_3]
Compiler compiler;
compiler.SetupDefaults();
compiler.AddBinaryOperator( ASTR("{}") , 900);

FormatOperator plugin( compiler );
compiler.InsertPlugin( &plugin, CompilePriorities::Custom );

lib::expressions::Scope scope( compiler.CfgFormatter );
SPExpression expression= compiler.Compile( ASTR(R"( "Hexadecimal: 0x{:x}" {} 42  )") );

cout << expression->Evaluate( scope )  << endl;
//! [DOX_ALIB_EXPR_OPS_3]
    ut.WriteResultFile( "DOX_ALIB_EXPR_OPS_3.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");
}
    catch(Exception& e)
    {
        SimpleText simpleText;
        simpleText.AddException( e );
        ut.WriteResultFile( "DOX_ALIB_EXPR_OPS_3.txt", simpleText.Text );
    }


#if !ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER
    //----------------- Verbal --------------------
    {
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );
EXPRESSION(
//! [DOX_ALIB_EXPR_OPS_verbal_1]
GetYear(Today) equals 2017 and GetDayOfWeek(Today) not_equals Monday
//! [DOX_ALIB_EXPR_OPS_verbal_1]
, false, 9 )
EXPRESSION(
//! [DOX_ALIB_EXPR_OPS_verbal_2]
GetYear(Today) == 2017 && GetDayOfWeek(Today) != Monday
//! [DOX_ALIB_EXPR_OPS_verbal_2]
, false, 9 )
    }
#endif
} // test method operators



// #################################################################################################
// ### Nested
// #################################################################################################
UT_METHOD( Nested )
{
    UT_INIT();

    testOutputStreamN.str("");
    try
    {
//! [DOX_ALIB_EXPR_NESTED_OP_1]
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(ASTR(R"(  *MyNestedExpression  )"));

cout << "Result: " << expression->Evaluate( scope );
//! [DOX_ALIB_EXPR_NESTED_OP_1]
    }
    catch(Exception& e)
    {
        SimpleText simpleText;
        simpleText.AddException( e );
        ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_OP_1.txt", simpleText.Text );
    }


    testOutputStreamN.str("");
    try
    {
//! [DOX_ALIB_EXPR_NESTED_OP_2]
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );

compiler.AddNamed( ASTR("MyNestedExpression"), ASTR("6 * 7") );

SPExpression expression= compiler.Compile(ASTR(R"(  *MyNestedExpression  )"));

cout << "Result: " << expression->Evaluate( scope ) << endl;
//! [DOX_ALIB_EXPR_NESTED_OP_2]
    }
    catch(Exception& )
    {
        assert(0);
    }
    ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_OP_2.txt", testOutputStreamN.str() );

    testOutputStreamN.str("");
    try
    {
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );

compiler.AddNamed( ASTR("MyNestedExpression"), ASTR("6 * 7") );

//! [DOX_ALIB_EXPR_NESTED_OP_3]
SPExpression expression= compiler.Compile(ASTR(R"(   2 * *MyNestedExpression    )"));
//! [DOX_ALIB_EXPR_NESTED_OP_3]

cout << "Result: " << expression->Evaluate( scope ) << endl;
    }
    catch(Exception& )
    {
        assert(0);
    }
    ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_OP_3.txt", testOutputStreamN.str() );

    testOutputStreamN.str("");
    try
    {
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );

compiler.AddNamed( ASTR("MyNestedExpression"), ASTR("6 * 7") );

//! [DOX_ALIB_EXPR_NESTED_OP_4]
SPExpression expression= compiler.Compile( ASTR(R"(   *("MyNested" + "Expression")   )") );
//! [DOX_ALIB_EXPR_NESTED_OP_4]

cout << "Result: " << expression->Evaluate( scope ) << endl;
    }
    catch(Exception& )
    {
        assert(0);
    }
    ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_OP_4.txt", testOutputStreamN.str() );


    testOutputStreamN.str("");
    try
    {
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );

compiler.AddNamed( ASTR("MyNestedExpression"), ASTR("6 * 7") );

//! [DOX_ALIB_EXPR_NESTED_OP_5]
SPExpression expression= compiler.Compile(ASTR(R"(    *("MyNested" + ( random >= 0.0 ? "Expression" : "" ))     )") );
//! [DOX_ALIB_EXPR_NESTED_OP_5]

    }
    catch(Exception& e)
    {
        SimpleText simpleText;
        simpleText.AddException( e );
        ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_OP_5.txt", simpleText.Text );
    }

    //----------------------------- function --------------------------------
    testOutputStreamN.str("");
    try
    {
//! [DOX_ALIB_EXPR_NESTED_FUNC_1]
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(ASTR(R"(   Expression( "MyNestedExpression", -1 )   )"));

cout << "Result: " << expression->Evaluate( scope ) << endl;
//! [DOX_ALIB_EXPR_NESTED_FUNC_1]
    }
    catch(Exception& )
    {
        assert(0);
    }
    ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_FUNC_1.txt", testOutputStreamN.str() );


    testOutputStreamN.str("");
    try
    {
//! [DOX_ALIB_EXPR_NESTED_FUNC_2]
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(ASTR(R"(   Expression( "MyNestedExpression", -1 )   )"));

compiler.AddNamed( ASTR("MyNestedExpression"), ASTR("3 * 3") );

cout << "Result: " << expression->Evaluate( scope ) << endl;
//! [DOX_ALIB_EXPR_NESTED_FUNC_2]
    }
    catch(Exception& )
    {
        assert(0);
    }
    ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_FUNC_2.txt", testOutputStreamN.str() );


    testOutputStreamN.str("");
    try
    {
//! [DOX_ALIB_EXPR_NESTED_FUNC_3]
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(ASTR(R"(   Expression( "MyNestedExpression", -1 )   )"));

compiler.AddNamed( ASTR("MyNestedExpression"), ASTR("3 * 3") );
cout << "Result1: " << expression->Evaluate( scope ) << endl;

compiler.AddNamed( ASTR("MyNestedExpression"), ASTR("4 * 4") );
cout << "Result2: " << expression->Evaluate( scope ) << endl;
//! [DOX_ALIB_EXPR_NESTED_FUNC_3]
    }
    catch(Exception& )
    {
        assert(0);
    }
    ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_FUNC_3.txt", testOutputStreamN.str() );



    testOutputStreamN.str("");
    try
    {
//! [DOX_ALIB_EXPR_NESTED_FUNC_4]
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(ASTR(R"(   Expression( "MyNestedExpression", -1 )   )"));

compiler.AddNamed( ASTR("MyNestedExpression"), ASTR(R"(  "Hello"   )"));
//! [DOX_ALIB_EXPR_NESTED_FUNC_4]
    }
    catch(Exception& )
    {
        assert(0);
    }


    testOutputStreamN.str("");
    try
    {
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(ASTR(R"(   Expression( "MyNestedExpression", -1 )   )"));

compiler.AddNamed( ASTR("MyNestedExpression"), ASTR(R"(  "Hello"   )"));

//! [DOX_ALIB_EXPR_NESTED_FUNC_5]
cout << "Result: " << expression->Evaluate( scope ) << endl;
//! [DOX_ALIB_EXPR_NESTED_FUNC_5]
    }
    catch(Exception& e)
    {
        SimpleText simpleText;
        simpleText.AddException( e );
        ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_FUNC_5.txt", simpleText.Text );
    }

    testOutputStreamN.str("");
    try
    {
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );
{
//! [DOX_ALIB_EXPR_NESTED_FUNC_6]
SPExpression expression= compiler.Compile(ASTR(R"(   Expression( MyNestedExpression, -1 )   )"));
//! [DOX_ALIB_EXPR_NESTED_FUNC_6]
}
{
//! [DOX_ALIB_EXPR_NESTED_FUNC_7]
SPExpression expression= compiler.Compile(ASTR(R"(   Expression( ("MyNested" + ( random >= 0.0 ? "Expression" : "" )), -1 )   )"));
//! [DOX_ALIB_EXPR_NESTED_FUNC_7]
}
    }
    catch(Exception& )
    {
        assert(0);
    }


    //----------------------------- function --------------------------------
    testOutputStreamN.str("");
    try
    {
//! [DOX_ALIB_EXPR_NESTED_FUNC_41]
Compiler compiler;
compiler.SetupDefaults();
expressions::Scope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(ASTR(R"(   Expression( "MyNestedExpression", -1, throw )   )"));

cout << "Result: " << expression->Evaluate( scope ) << endl;
//! [DOX_ALIB_EXPR_NESTED_FUNC_41]
    }
    catch(Exception& e)
    {
        SimpleText simpleText;
        simpleText.AddException( e );
        ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_FUNC_41.txt", simpleText.Text );
    }

} // test method Nested

// #################################################################################################
// ### VirtualMachine Listings
// #################################################################################################
#if ALIB_DEBUG
void printProgram( AWorxUnitTesting& ut, const String& expressionString, const NString& outputfilename,
                   bool dontOptimize= true)
{
    Compiler compiler;
    compiler.SetupDefaults();
    expressions::Scope scope( compiler.CfgFormatter );

    if( dontOptimize )
        compiler.CfgCompilation+= Compilation::NoOptimization;

    compiler.AddNamed(ASTR("nested"), ASTR("5"));

    //---------------------- Compile -------------------
    try
    {
        SPExpression expression= compiler.Compile( expressionString );
        AString listing=  expressions::detail::VirtualMachine::DbgList(*dynamic_cast<expressions::detail::Program*>( expression->GetProgram() ) );
        ut.WriteResultFile( outputfilename, listing );

    }
    catch( Exception e )
    {
        SimpleText text;
        text.AddException( e );
        ut.WriteResultFile( outputfilename, text.Text );
        assert(!ut.AssertOnFailure);
    }
}

UT_METHOD( VMListings )
{
    UT_INIT();

    printProgram( ut, ASTR("42"),                                               "DOX_ALIB_EXPR_VM_-1.txt", false  );
    printProgram( ut, ASTR("42 * 2"),                                           "DOX_ALIB_EXPR_VM_-2.txt", false  );
    printProgram( ut, ASTR("42 * 2"),                                           "DOX_ALIB_EXPR_VM_-3.txt"  );
    printProgram( ut, ASTR("(((42 * 2) / 5) * (2 + 3) ) * 7"),                  "DOX_ALIB_EXPR_VM_-4.txt"  );
    printProgram( ut, ASTR("Format( \"Result of: {}\", \"2 * 3\", 2 * 3 )") ,   "DOX_ALIB_EXPR_VM_-5.txt"  );
    printProgram( ut, ASTR("true ? 1 : 2")                                  ,   "DOX_ALIB_EXPR_VM_-6.txt"  );
    printProgram( ut, ASTR("true ? 1 : 2")                                  ,   "DOX_ALIB_EXPR_VM_-6opt.txt", false  );
    printProgram( ut, ASTR("*nested")                                       ,   "DOX_ALIB_EXPR_VM_-7.txt"  );
    printProgram( ut, ASTR("Expression(nested,-1,throw)")                   ,   "DOX_ALIB_EXPR_VM_-8.txt"  );

}
#endif


UT_CLASS_END

} //namespace

