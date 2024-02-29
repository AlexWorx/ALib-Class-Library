// #################################################################################################
//  AWorx ALib Unit Tests
//  Documentation sample for ALib Expressions: Calculator
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"

#if ALIB_UT_DOCS && ALIB_UT_EXPRESSIONS

#include "alib/alox.hpp"

// Fix the method name of logging (needed for unity builds)
ALIB_WARNINGS_IGNORE_UNUSED_MACRO
#undef  ALIB_CALLER
#if defined( __GNUC__ )
#   define ALIB_CALLER    __FILE__, __LINE__, __func__
#else
#   define ALIB_CALLER    __FILE__, __LINE__, __FUNCTION__
#endif

// Preparations to fake std::cout, main(), etc.
#include <sstream>
#include <iostream>
#include <iomanip>
#include <assert.h>

#if defined(_MSC_VER)
    ALIB_WARNINGS_RESERVED_MACRO_NAME_OFF
    ALIB_WARNINGS_MACRO_NOT_USED_OFF
        #define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING // MSVC to20do
    ALIB_WARNINGS_RESTORE
    ALIB_WARNINGS_RESTORE
#endif

#include <chrono>

#include <filesystem>
namespace fs = std::filesystem;

// Note: MacOS is currently (as of 231210) missing C++ 20 library features in the area of std::clock
#if ALIB_CPP_STANDARD == 17 || defined(__APPLE__)  || defined(__ANDROID_NDK__)
namespace
{
    template <typename TP>
    std::time_t to_time_t(TP tp)
    {
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp - TP::clock::now()
                        + std::chrono::system_clock::now());
        return std::chrono::system_clock::to_time_t(sctp);
    }
}
#endif



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

#include "alib/monomem/mastring.hpp"
#include "alib/expressions/compiler.hpp"
#include "alib/expressions/scope.hpp"
#include "alib/expressions/compilerplugin.hpp"
#include "alib/expressions/plugins/strings.hpp"
#include "alib/expressions/plugins/calculus.hpp"
#include "alib/expressions/detail/program.hpp"
#include "alib/expressions/detail/virtualmachine.hpp"
#include "alib/compatibility/std_characters.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"
#include "alib/lang/format/paragraphs.hpp"
#include "alib/time/datetime.hpp"
#include <sys/stat.h>

using namespace std;
using namespace alib;
using namespace alib::expressions;



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

    virtual bool TryCompilation( CIBinaryOp&  ciBinaryOp )                                  override
    {
        // check if it is not us
        if(     ciBinaryOp.Operator != A_CHAR("{}")
            || !ciBinaryOp.ArgsBegin->IsSameType( Types::String ) )
            return false;

        // set debug info
        ALIB_DBG( ciBinaryOp.DbgCallbackName = "CBFormat";      )

        // all is const? We can do it at compile-time!
        if( ciBinaryOp.LhsIsConst && ciBinaryOp.RhsIsConst )
        {
            ciBinaryOp.TypeOrValue= expressions::plugins::CBFormat(ciBinaryOp.CompileTimeScope,
                                                                        ciBinaryOp.ArgsBegin,
                                                                        ciBinaryOp.ArgsEnd         );
            return true;
        }

        // set callback
        ciBinaryOp.Callback     = expressions::plugins::CBFormat;
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
    // Constructor.
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
    Compiler        compiler;
    ExpressionScope scope;
    SPExpression    expression;


    // Constructor. Compiles the expression
    FileFilter( const String& expressionString )
    : compiler()
    , scope( compiler.CfgFormatter )
    {
        compiler.SetupDefaults();

        expression= compiler.Compile( expressionString );
    }

    // Filter function. Evaluates the expression.
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
    Compiler        compiler;
    ExpressionScope scope;
    SPExpression    expression;

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
    Compiler        compiler;
    ExpressionScope scope;
    SPExpression    expression;

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
struct FFScope : public ExpressionScope
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
        //           Evaluate().
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
struct FFScope : public ExpressionScope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};


//! [DOX_ALIB_EXPR_TUT_FF_PluginCallback]
Box getName( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    // Create a copy of the string using the scope string allocator. This is done by using
    // class MAString, which, when returned, right away is boxed as a usual string,
    // aka char[]. Therefore, no intermediate string objects need to be stored, neither the
    // std::string returned by "string()", nor the string.
    return MAString( scope.Allocator,
                      dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().string(),
                      0 );
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
    virtual bool TryCompilation( CIFunction&  ciFunction )                                  override
    {
        // Is parameterless and function name equals "Name"?
        if(       ciFunction.QtyArgs() == 0
             &&   ciFunction.Name.Equals<false, lang::Case::Ignore>( A_CHAR("Name") )    )
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
    , ffPlugin( compiler )            // CHANGE 2: Initialize the plug-in with the compiler.
    {
        compiler.SetupDefaults();

        // CHANGE 3: Add our custom plug-in to the compiler prior to compiling the expression
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
} // namespace step5
//! [DOX_ALIB_EXPR_TUT_FF_PluginSkeleton2]

//###################################### STEP 6:  Calculus  ########################################

namespace step6 {
struct FFScope : public ExpressionScope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return MAString( scope.Allocator,
                      dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().string(),
                      0 );
}

//! [DOX_ALIB_EXPR_TUT_FF_PluginCalculus]
struct FFCompilerPlugin : public Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        Functions=
        {
            {  { A_CHAR("Name"), lang::Case::Ignore, 4 }, // The function name, letter case min. abbreviation (using class strings::util::Token).
                nullptr, 0                         ,      // No arguments (otherwise an array of sample boxes defining expected argument types).
                CALCULUS_CALLBACK(getName)         ,      // The callback function. In debug mode, also the name of the callback.
                &Types::String                     ,      // The return type of the callback function, given as pointer to sample box.
                ETI                                       // Denotes "evaluation time invokable only". Alternative is "CTI".
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
} // namespace step6

//############################### STEP 7: Adding Identifiers  ######################################
namespace step7 {
struct FFScope : public ExpressionScope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};


Box getName( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return MAString( scope.Allocator,
                      dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().string(),
                      0   );
}

//! [DOX_ALIB_EXPR_TUT_FF_MoreIdentifiers]
namespace
{
    Box isFolder( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
    {
        return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
                == fs::file_type::directory;
    }

    Box getSize( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
    {
        return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
                == fs::file_type::directory )
                ? 0
                : static_cast<integer>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
    }

    Box getDate( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
    {
        auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
        #if ALIB_CPP_STANDARD == 17  || defined(__APPLE__)  || defined(__ANDROID_NDK__)
            return DateTime::FromEpochSeconds( to_time_t( fsTime ) );
        #else
            return DateTime::FromEpochSeconds( chrono::system_clock::to_time_t(
                                               chrono::clock_cast<chrono::system_clock>(fsTime) ) );
        #endif
    }

    Box getPerm( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
    {
        return static_cast<integer>( dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions() );
    }
}
//! [DOX_ALIB_EXPR_TUT_FF_MoreIdentifiers]

//! [DOX_ALIB_EXPR_TUT_FF_MoreIdentifiersConstants]
namespace
{
    Box constOwnRead  = static_cast<integer>( UnderlyingIntegral( fs::perms::owner_read  ) );
    Box constOwnWrite = static_cast<integer>( UnderlyingIntegral( fs::perms::owner_write ) );
    Box constOwnExec  = static_cast<integer>( UnderlyingIntegral( fs::perms::owner_exec  ) );
    Box constGrpRead  = static_cast<integer>( UnderlyingIntegral( fs::perms::group_read  ) );
    Box constGrpWrite = static_cast<integer>( UnderlyingIntegral( fs::perms::group_write ) );
    Box constGrpExec  = static_cast<integer>( UnderlyingIntegral( fs::perms::group_exec  ) );
    Box constOthRead  = static_cast<integer>( UnderlyingIntegral( fs::perms::others_read ) );
    Box constOthWrite = static_cast<integer>( UnderlyingIntegral( fs::perms::others_write) );
    Box constOthExec  = static_cast<integer>( UnderlyingIntegral( fs::perms::others_exec ) );
}
//! [DOX_ALIB_EXPR_TUT_FF_MoreIdentifiersConstants]


//! [DOX_ALIB_EXPR_TUT_FF_MoreIdentifiersPlugin]
struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        ConstantIdentifiers=
        {
          // Parameters: "1, 1" denote the minimum abbreviation of each "camel hump"
          { { A_CHAR("OwnerRead")    , lang::Case::Ignore, 1, 1}, constOwnRead  },
          { { A_CHAR("OwnerWrite")   , lang::Case::Ignore, 1, 1}, constOwnWrite },
          { { A_CHAR("OwnerExecute") , lang::Case::Ignore, 1, 1}, constOwnExec  },
          { { A_CHAR("GroupRead")    , lang::Case::Ignore, 1, 1}, constGrpRead  },
          { { A_CHAR("GroupWrite")   , lang::Case::Ignore, 1, 1}, constGrpWrite },
          { { A_CHAR("GroupExecute") , lang::Case::Ignore, 1, 1}, constGrpExec  },
          { { A_CHAR("OthersRead")   , lang::Case::Ignore, 1, 1}, constOthRead  },
          { { A_CHAR("OthersWrite")  , lang::Case::Ignore, 1, 1}, constOthWrite },
          { { A_CHAR("OthersExecute"), lang::Case::Ignore, 1, 1}, constOthExec  },
        };

        Functions=
        {
          { {A_CHAR("Name")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(getName ), &Types::String  , ETI },
          { {A_CHAR("IsDirectory")  , lang::Case::Ignore, 2, 3}, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(isFolder), &Types::Boolean , ETI },
          { {A_CHAR("Size")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(getSize ), &Types::Integer , ETI },
          { {A_CHAR("Date")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(getDate ), &Types::DateTime, ETI },
          { {A_CHAR("Permissions")  , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(getPerm ), &Types::Integer , ETI },
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
} // namespace step7

//############################### STEP 8: Adding Functions  ######################################
namespace step8 {
struct FFScope : public ExpressionScope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return MAString( scope.Allocator,
                      dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().string(),
                      0 );
}

Box isFolder( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? 0
            : static_cast<integer>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    #if ALIB_CPP_STANDARD == 17 || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        return DateTime::FromEpochSeconds( to_time_t( fsTime ) );
    #else
            return DateTime::FromEpochSeconds( chrono::system_clock::to_time_t(
                                               chrono::clock_cast<chrono::system_clock>(fsTime) ) );
    #endif
}

Box getPerm( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return static_cast<integer>( dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions() );
}

//! [DOX_ALIB_EXPR_TUT_FF_Functions]
Box kiloBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024;
}
Box megaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024;
}
Box gigaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024 * 1024;
}
//! [DOX_ALIB_EXPR_TUT_FF_Functions]

Box constOwnRead  = static_cast<integer>( UnderlyingIntegral( fs::perms::owner_read  ) );
Box constOwnWrite = static_cast<integer>( UnderlyingIntegral( fs::perms::owner_write ) );
Box constOwnExec  = static_cast<integer>( UnderlyingIntegral( fs::perms::owner_exec  ) );
Box constGrpRead  = static_cast<integer>( UnderlyingIntegral( fs::perms::group_read  ) );
Box constGrpWrite = static_cast<integer>( UnderlyingIntegral( fs::perms::group_write ) );
Box constGrpExec  = static_cast<integer>( UnderlyingIntegral( fs::perms::group_exec  ) );
Box constOthRead  = static_cast<integer>( UnderlyingIntegral( fs::perms::others_read ) );
Box constOthWrite = static_cast<integer>( UnderlyingIntegral( fs::perms::others_write) );
Box constOthExec  = static_cast<integer>( UnderlyingIntegral( fs::perms::others_exec ) );


//! [DOX_ALIB_EXPR_TUT_FF_FunctionSignature]
Box*  OneInt[1]= { &Types::Integer  };
//! [DOX_ALIB_EXPR_TUT_FF_FunctionSignature]


//! [DOX_ALIB_EXPR_TUT_FF_FunctionsPlugin]
struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        ConstantIdentifiers=
        {
          { {A_CHAR("OwnerRead")    , lang::Case::Ignore, 1, 1}, constOwnRead  },
          { {A_CHAR("OwnerWrite")   , lang::Case::Ignore, 1, 1}, constOwnWrite },
          { {A_CHAR("OwnerExecute") , lang::Case::Ignore, 1, 1}, constOwnExec  },
          { {A_CHAR("GroupRead")    , lang::Case::Ignore, 1, 1}, constGrpRead  },
          { {A_CHAR("GroupWrite")   , lang::Case::Ignore, 1, 1}, constGrpWrite },
          { {A_CHAR("GroupExecute") , lang::Case::Ignore, 1, 1}, constGrpExec  },
          { {A_CHAR("OthersRead")   , lang::Case::Ignore, 1, 1}, constOthRead  },
          { {A_CHAR("OthersWrite")  , lang::Case::Ignore, 1, 1}, constOthWrite },
          { {A_CHAR("OthersExecute"), lang::Case::Ignore, 1, 1}, constOthExec  },
        };

        Functions=
        {
          { {A_CHAR("Name")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(getName  ), &Types::String  , ETI },
          { {A_CHAR("IsDirectory")  , lang::Case::Ignore, 2, 3}, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(isFolder ), &Types::Boolean , ETI },
          { {A_CHAR("Size")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(getSize  ), &Types::Integer , ETI },
          { {A_CHAR("Date")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(getDate  ), &Types::DateTime, ETI },
          { {A_CHAR("Permissions")  , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(getPerm  ), &Types::Integer , ETI },

          // the new functions:
          { {A_CHAR("KiloBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(OneInt ), CALCULUS_CALLBACK(kiloBytes), &Types::Integer , CTI },
          { {A_CHAR("MegaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(OneInt ), CALCULUS_CALLBACK(megaBytes), &Types::Integer , CTI },
          { {A_CHAR("GigaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(OneInt ), CALCULUS_CALLBACK(gigaBytes), &Types::Integer , CTI },
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
} // namespace step8

//############################### STEP 9: adding custom types  ######################################
namespace step9 {
struct FFScope : public ExpressionScope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return MAString( scope.Allocator,
                      dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().string(),
                      0 );
}

Box isFolder( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? 0
            : static_cast<integer>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    #if ALIB_CPP_STANDARD == 17 || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        return DateTime::FromEpochSeconds( to_time_t( fsTime ) );
    #else
            return DateTime::FromEpochSeconds( chrono::system_clock::to_time_t(
                                               chrono::clock_cast<chrono::system_clock>(fsTime) ) );
    #endif
}

Box kiloBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024;
}
Box megaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024;
}
Box gigaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024 * 1024;
}
//! [DOX_ALIB_EXPR_TUT_FF_PermType]
Box getPerm( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions();
}
//! [DOX_ALIB_EXPR_TUT_FF_PermType]

Box constOwnRead  ;
Box constOwnWrite ;
Box constOwnExec  ;
Box constGrpRead  ;
Box constGrpWrite ;
Box constGrpExec  ;
Box constOthRead  ;
Box constOthWrite ;
Box constOthExec  ;

Box TypePermission;


//! [DOX_ALIB_EXPR_TUT_FF_PermTypePlugin]
struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        // Initializations of constant values. This now must not be done with their definition
        // anymore, because now type "fs::perms" is boxed instead of type "integer"
        constOwnRead  = UnderlyingIntegral( fs::perms::owner_read  );
        constOwnWrite = UnderlyingIntegral( fs::perms::owner_write );
        constOwnExec  = UnderlyingIntegral( fs::perms::owner_exec  );
        constGrpRead  = UnderlyingIntegral( fs::perms::group_read  );
        constGrpWrite = UnderlyingIntegral( fs::perms::group_write );
        constGrpExec  = UnderlyingIntegral( fs::perms::group_exec  );
        constOthRead  = UnderlyingIntegral( fs::perms::others_read );
        constOthWrite = UnderlyingIntegral( fs::perms::others_write);
        constOthExec  = UnderlyingIntegral( fs::perms::others_exec );

        // A sample box for the new type "fs::perm"
        TypePermission = fs::perms::owner_read;   // ...could be any other enum element as well!

        ConstantIdentifiers=
        {
          { {A_CHAR("OwnerRead")    , lang::Case::Ignore, 1, 1}, constOwnRead  },
          { {A_CHAR("OwnerWrite")   , lang::Case::Ignore, 1, 1}, constOwnWrite },
          { {A_CHAR("OwnerExecute") , lang::Case::Ignore, 1, 1}, constOwnExec  },
          { {A_CHAR("GroupRead")    , lang::Case::Ignore, 1, 1}, constGrpRead  },
          { {A_CHAR("GroupWrite")   , lang::Case::Ignore, 1, 1}, constGrpWrite },
          { {A_CHAR("GroupExecute") , lang::Case::Ignore, 1, 1}, constGrpExec  },
          { {A_CHAR("OthersRead")   , lang::Case::Ignore, 1, 1}, constOthRead  },
          { {A_CHAR("OthersWrite")  , lang::Case::Ignore, 1, 1}, constOthWrite },
          { {A_CHAR("OthersExecute"), lang::Case::Ignore, 1, 1}, constOthExec  },
        };

        Functions=
        {
          { {A_CHAR("Name")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getName  ), &Types::String  , ETI },
          { {A_CHAR("IsDirectory")  , lang::Case::Ignore, 2, 3}, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(isFolder ), &Types::Boolean , ETI },
          { {A_CHAR("Size")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getSize  ), &Types::Integer , ETI },
          { {A_CHAR("Date")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getDate  ), &Types::DateTime, ETI },

          // change return type to TypePermission
          { {A_CHAR("Permissions")  , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getPerm  ), &TypePermission , ETI },

          { {A_CHAR("KiloBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(kiloBytes), &Types::Integer , CTI },
          { {A_CHAR("MegaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(megaBytes), &Types::Integer , CTI },
          { {A_CHAR("GigaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(gigaBytes), &Types::Integer , CTI },
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
} // namespace step9


//############################### STEP 10: Announcing custom types ##################################
namespace step10 {
struct FFScope : public ExpressionScope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return MAString( scope.Allocator,
                      dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().string(),
                      0 );
}

Box isFolder( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? static_cast<integer>( 0 )
            : static_cast<integer>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    #if ALIB_CPP_STANDARD == 17 || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        return DateTime::FromEpochSeconds( to_time_t( fsTime ) );
    #else
        return DateTime::FromEpochSeconds( chrono::system_clock::to_time_t(
                                           chrono::clock_cast<chrono::system_clock>(fsTime) ) );
    #endif
}

Box kiloBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024;
}
Box megaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024;
}
Box gigaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024 * 1024;
}
Box getPerm( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions();
}

Box constOwnRead  ;
Box constOwnWrite ;
Box constOwnExec  ;
Box constGrpRead  ;
Box constGrpWrite ;
Box constGrpExec  ;
Box constOthRead  ;
Box constOthWrite ;
Box constOthExec  ;

Box TypePermission;

struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        constOwnRead  =  fs::perms::owner_read;
        constOwnWrite =  fs::perms::owner_write;
        constOwnExec  =  fs::perms::owner_exec;
        constGrpRead  =  fs::perms::group_read;
        constGrpWrite =  fs::perms::group_write;
        constGrpExec  =  fs::perms::group_exec;
        constOthRead  =  fs::perms::others_read;
        constOthWrite =  fs::perms::others_write;
        constOthExec  =  fs::perms::others_exec;

        TypePermission = fs::perms::owner_read;

//! [DOX_ALIB_EXPR_TUT_FF_PermTypeAddTypeDef]
        // Announce our custom type to the compiler
        compiler.AddType( TypePermission, "Permission" );
        //...
//! [DOX_ALIB_EXPR_TUT_FF_PermTypeAddTypeDef]

        ConstantIdentifiers=
        {
          { {A_CHAR("OwnerRead")    , lang::Case::Ignore, 1, 1}, constOwnRead  },
          { {A_CHAR("OwnerWrite")   , lang::Case::Ignore, 1, 1}, constOwnWrite },
          { {A_CHAR("OwnerExecute") , lang::Case::Ignore, 1, 1}, constOwnExec  },
          { {A_CHAR("GroupRead")    , lang::Case::Ignore, 1, 1}, constGrpRead  },
          { {A_CHAR("GroupWrite")   , lang::Case::Ignore, 1, 1}, constGrpWrite },
          { {A_CHAR("GroupExecute") , lang::Case::Ignore, 1, 1}, constGrpExec  },
          { {A_CHAR("OthersRead")   , lang::Case::Ignore, 1, 1}, constOthRead  },
          { {A_CHAR("OthersWrite")  , lang::Case::Ignore, 1, 1}, constOthWrite },
          { {A_CHAR("OthersExecute"), lang::Case::Ignore, 1, 1}, constOthExec  },
        };

        Functions=
        {
          { {A_CHAR("Name")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getName  ), &Types::String  , ETI },
          { {A_CHAR("IsDirectory")  , lang::Case::Ignore, 2, 3}, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(isFolder ), &Types::Boolean , ETI },
          { {A_CHAR("Size")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getSize  ), &Types::Integer , ETI },
          { {A_CHAR("Date")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getDate  ), &Types::DateTime, ETI },
          { {A_CHAR("Permissions")  , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getPerm  ), &TypePermission , ETI },
          { {A_CHAR("KiloBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(kiloBytes), &Types::Integer , CTI },
          { {A_CHAR("MegaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(megaBytes), &Types::Integer , CTI },
          { {A_CHAR("GigaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(gigaBytes), &Types::Integer , CTI },
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
} // namespace step10

//############################### STEP 11: custom operators ##################################
namespace step11 {
struct FFScope : public ExpressionScope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return MAString( scope.Allocator,
                      dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().string(),
                      0 );
}

Box isFolder( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? static_cast<integer>(0)
            : static_cast<integer>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    #if ALIB_CPP_STANDARD == 17 || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        return DateTime::FromEpochSeconds( to_time_t( fsTime ) );
    #else
        return DateTime::FromEpochSeconds( chrono::system_clock::to_time_t(
                                           chrono::clock_cast<chrono::system_clock>(fsTime) ) );
    #endif
}

Box kiloBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024;
}
Box megaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024;
}
Box gigaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024 * 1024;
}
Box getPerm( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions();
}
//! [DOX_ALIB_EXPR_TUT_FF_PermTypeOperatorCallbacks]
Box opPermAnd( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return    argsBegin     ->Unbox<fs::perms>()
           & (argsBegin + 1)->Unbox<fs::perms>();
}
Box opPermOr( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return    argsBegin     ->Unbox<fs::perms>()
           | (argsBegin + 1)->Unbox<fs::perms>();
}
Box opPermXOr( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return    argsBegin     ->Unbox<fs::perms>()
           ^ (argsBegin + 1)->Unbox<fs::perms>();
}
Box opPermEq( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return    argsBegin     ->Unbox<fs::perms>()
           ==(argsBegin + 1)->Unbox<fs::perms>();
}
Box opPermNEq( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return    argsBegin     ->Unbox<fs::perms>()
           !=(argsBegin + 1)->Unbox<fs::perms>();
}
//! [DOX_ALIB_EXPR_TUT_FF_PermTypeOperatorCallbacks]


Box constOwnRead  ;
Box constOwnWrite ;
Box constOwnExec  ;
Box constGrpRead  ;
Box constGrpWrite ;
Box constGrpExec  ;
Box constOthRead  ;
Box constOthWrite ;
Box constOthExec  ;

Box TypePermission;

//! [DOX_ALIB_EXPR_TUT_FF_PermTypeOperatorTable]
alib::expressions::plugins::Calculus::OperatorTableEntry  binaryOpTable[] =
{
    { A_CHAR("&") , TypePermission, TypePermission, CALCULUS_CALLBACK( opPermAnd ), TypePermission , Calculus::CTI },
    { A_CHAR("|") , TypePermission, TypePermission, CALCULUS_CALLBACK( opPermOr  ), TypePermission , Calculus::CTI },
    { A_CHAR("^") , TypePermission, TypePermission, CALCULUS_CALLBACK( opPermXOr ), TypePermission , Calculus::CTI },
    { A_CHAR("=="), TypePermission, TypePermission, CALCULUS_CALLBACK( opPermEq  ), Types::Boolean , Calculus::CTI },
    { A_CHAR("!="), TypePermission, TypePermission, CALCULUS_CALLBACK( opPermNEq ), Types::Boolean , Calculus::CTI },
};
//! [DOX_ALIB_EXPR_TUT_FF_PermTypeOperatorTable]


struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        constOwnRead  =  fs::perms::owner_read;
        constOwnWrite =  fs::perms::owner_write;
        constOwnExec  =  fs::perms::owner_exec;
        constGrpRead  =  fs::perms::group_read;
        constGrpWrite =  fs::perms::group_write;
        constGrpExec  =  fs::perms::group_exec;
        constOthRead  =  fs::perms::others_read;
        constOthWrite =  fs::perms::others_write;
        constOthExec  =  fs::perms::others_exec;

        TypePermission = fs::perms::owner_read;

        compiler.AddType( TypePermission, "Permission" );

        ConstantIdentifiers=
        {
          { {A_CHAR("OwnerRead")    , lang::Case::Ignore, 1, 1}, constOwnRead  },
          { {A_CHAR("OwnerWrite")   , lang::Case::Ignore, 1, 1}, constOwnWrite },
          { {A_CHAR("OwnerExecute") , lang::Case::Ignore, 1, 1}, constOwnExec  },
          { {A_CHAR("GroupRead")    , lang::Case::Ignore, 1, 1}, constGrpRead  },
          { {A_CHAR("GroupWrite")   , lang::Case::Ignore, 1, 1}, constGrpWrite },
          { {A_CHAR("GroupExecute") , lang::Case::Ignore, 1, 1}, constGrpExec  },
          { {A_CHAR("OthersRead")   , lang::Case::Ignore, 1, 1}, constOthRead  },
          { {A_CHAR("OthersWrite")  , lang::Case::Ignore, 1, 1}, constOthWrite },
          { {A_CHAR("OthersExecute"), lang::Case::Ignore, 1, 1}, constOthExec  },
        };

        Functions=
        {
          { {A_CHAR("Name")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getName  ), &Types::String  , ETI },
          { {A_CHAR("IsDirectory")  , lang::Case::Ignore, 2, 3}, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(isFolder ), &Types::Boolean , ETI },
          { {A_CHAR("Size")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getSize  ), &Types::Integer , ETI },
          { {A_CHAR("Date")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getDate  ), &Types::DateTime, ETI },
          { {A_CHAR("Permissions")  , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getPerm  ), &TypePermission , ETI },
          { {A_CHAR("KiloBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(kiloBytes), &Types::Integer , CTI },
          { {A_CHAR("MegaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(megaBytes), &Types::Integer , CTI },
          { {A_CHAR("GigaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(gigaBytes), &Types::Integer , CTI },
        };

//! [DOX_ALIB_EXPR_TUT_FF_PermTypeFeedTable]
AddOperators( binaryOpTable );
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
} // namespace step11

//############################### STEP 12: auto cast ##################################
namespace step12 {
struct FFScope : public ExpressionScope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return MAString( scope.Allocator,
                      dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().string(),
                      0 );
}

Box isFolder( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? static_cast<integer>(0)
            : static_cast<integer>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    #if ALIB_CPP_STANDARD == 17 || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        return DateTime::FromEpochSeconds( to_time_t( fsTime ) );
    #else
        return DateTime::FromEpochSeconds( chrono::system_clock::to_time_t(
                                           chrono::clock_cast<chrono::system_clock>(fsTime) ) );
    #endif
}

Box kiloBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024;
}
Box megaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024;
}
Box gigaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024 * 1024;
}
Box getPerm( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions();
}

//! [DOX_ALIB_EXPR_TUT_FF_AutoCastCallback]
Box perm2Int( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return static_cast<integer>( argsBegin->Unbox<fs::perms>() );
}
//! [DOX_ALIB_EXPR_TUT_FF_AutoCastCallback]

Box constOwnRead  ;
Box constOwnWrite ;
Box constOwnExec  ;
Box constGrpRead  ;
Box constGrpWrite ;
Box constGrpExec  ;
Box constOthRead  ;
Box constOthWrite ;
Box constOthExec  ;

Box TypePermission;



struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        constOwnRead  =  fs::perms::owner_read;
        constOwnWrite =  fs::perms::owner_write;
        constOwnExec  =  fs::perms::owner_exec;
        constGrpRead  =  fs::perms::group_read;
        constGrpWrite =  fs::perms::group_write;
        constGrpExec  =  fs::perms::group_exec;
        constOthRead  =  fs::perms::others_read;
        constOthWrite =  fs::perms::others_write;
        constOthExec  =  fs::perms::others_exec;

        TypePermission = fs::perms::owner_read;
        compiler.AddType( TypePermission, "Permission" );

        ConstantIdentifiers=
        {
          { {A_CHAR("OwnerRead")    , lang::Case::Ignore, 1, 1}, constOwnRead  },
          { {A_CHAR("OwnerWrite")   , lang::Case::Ignore, 1, 1}, constOwnWrite },
          { {A_CHAR("OwnerExecute") , lang::Case::Ignore, 1, 1}, constOwnExec  },
          { {A_CHAR("GroupRead")    , lang::Case::Ignore, 1, 1}, constGrpRead  },
          { {A_CHAR("GroupWrite")   , lang::Case::Ignore, 1, 1}, constGrpWrite },
          { {A_CHAR("GroupExecute") , lang::Case::Ignore, 1, 1}, constGrpExec  },
          { {A_CHAR("OthersRead")   , lang::Case::Ignore, 1, 1}, constOthRead  },
          { {A_CHAR("OthersWrite")  , lang::Case::Ignore, 1, 1}, constOthWrite },
          { {A_CHAR("OthersExecute"), lang::Case::Ignore, 1, 1}, constOthExec  },
        };

        Functions=
        {
          { {A_CHAR("Name")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getName  ), &Types::String  , ETI },
          { {A_CHAR("IsDirectory")  , lang::Case::Ignore, 2, 3}, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(isFolder ), &Types::Boolean , ETI },
          { {A_CHAR("Size")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getSize  ), &Types::Integer , ETI },
          { {A_CHAR("Date")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getDate  ), &Types::DateTime, ETI },
          { {A_CHAR("Permissions")  , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getPerm  ), &TypePermission , ETI },
          { {A_CHAR("KiloBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(kiloBytes), &Types::Integer , CTI },
          { {A_CHAR("MegaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(megaBytes), &Types::Integer , CTI },
          { {A_CHAR("GigaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(gigaBytes), &Types::Integer , CTI },
        };

    }
//! [DOX_ALIB_EXPR_TUT_FF_AutoCastCompilation]
virtual bool TryCompilation( CIAutoCast& ciAutoCast )    override
{
    // we don't cast for conditional operator "Q ? T : F"
    // Note: It is usually a good practice to also cast for this operator.
    //       This code is just a sample to demonstrate how to omit casting for certain operator(s).
    if( ciAutoCast.Operator.Equals<false>( A_CHAR("Q?T:F") ) )
        return false;

    bool result= false;

    // cast first argument (lhs, if binary op)
    if( ciAutoCast.ArgsBegin->IsType<fs::perms>() )
    {
        result= true;
        if( ciAutoCast.IsConst )
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
            ALIB_DBG( ciAutoCast.DbgCallbackName= "perm2Int"; )
        }
    }

    // cast RHS, if given
    if(      ciAutoCast.ArgsBegin + 1 < ciAutoCast.ArgsEnd
         && (ciAutoCast.ArgsBegin + 1)->IsType<fs::perms>() )
    {
        result= true;
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
            ALIB_DBG( ciAutoCast.DbgCallbackNameRhs= "perm2Int"; )
        }
    }

    return result;
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
} // namespace step12


//############################### STEP 13: auto cast with calculus ##################################
namespace step13 {
struct FFScope : public ExpressionScope
{
    const fs::directory_entry*    directoryEntry;
    using Scope::Scope;
};

Box getName( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return MAString( scope.Allocator,
                      dynamic_cast<FFScope&>( scope ).directoryEntry->path().filename().string(),
                      0 );
}

Box isFolder( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return     dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory;
}

Box getSize( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return (  dynamic_cast<FFScope&>( scope ).directoryEntry->status().type()
            == fs::file_type::directory )
            ? static_cast<integer>(0)
            : static_cast<integer>(fs::file_size( *dynamic_cast<FFScope&>( scope ).directoryEntry ));
}

Box getDate( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    auto fsTime = fs::last_write_time(  *dynamic_cast<FFScope&>( scope ).directoryEntry );
    #if ALIB_CPP_STANDARD == 17 || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        return DateTime::FromEpochSeconds( to_time_t( fsTime ) );
    #else
        return DateTime::FromEpochSeconds( chrono::system_clock::to_time_t(
                                           chrono::clock_cast<chrono::system_clock>(fsTime) ) );
    #endif
}

Box kiloBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024;
}
Box megaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024;
}
Box gigaBytes( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return argsBegin->Unbox<integer>() * 1024 * 1024 * 1024;
}
Box getPerm( ExpressionScope& scope, ArgIterator argsBegin, ArgIterator argsEnd )
{
    return dynamic_cast<FFScope&>( scope ).directoryEntry->status().permissions();
}

Box constOwnRead  ;
Box constOwnWrite ;
Box constOwnExec  ;
Box constGrpRead  ;
Box constGrpWrite ;
Box constGrpExec  ;
Box constOthRead  ;
Box constOthWrite ;
Box constOthExec  ;

Box TypePermission;

struct FFCompilerPlugin : public plugins::Calculus
{
    FFCompilerPlugin( Compiler& compiler )
    : Calculus( "FF Plug-in", compiler )
    {
        constOwnRead  =  static_cast<integer>( fs::perms::owner_read   );
        constOwnWrite =  static_cast<integer>( fs::perms::owner_write  );
        constOwnExec  =  static_cast<integer>( fs::perms::owner_exec   );
        constGrpRead  =  static_cast<integer>( fs::perms::group_read   );
        constGrpWrite =  static_cast<integer>( fs::perms::group_write  );
        constGrpExec  =  static_cast<integer>( fs::perms::group_exec   );
        constOthRead  =  static_cast<integer>( fs::perms::others_read  );
        constOthWrite =  static_cast<integer>( fs::perms::others_write );
        constOthExec  =  static_cast<integer>( fs::perms::others_exec  );

        TypePermission =                       fs::perms::owner_read    ;

        compiler.AddType( TypePermission, "Permission" );

        ConstantIdentifiers=
        {
          { {A_CHAR("OwnerRead")    , lang::Case::Ignore, 1, 1}, constOwnRead  },
          { {A_CHAR("OwnerWrite")   , lang::Case::Ignore, 1, 1}, constOwnWrite },
          { {A_CHAR("OwnerExecute") , lang::Case::Ignore, 1, 1}, constOwnExec  },
          { {A_CHAR("GroupRead")    , lang::Case::Ignore, 1, 1}, constGrpRead  },
          { {A_CHAR("GroupWrite")   , lang::Case::Ignore, 1, 1}, constGrpWrite },
          { {A_CHAR("GroupExecute") , lang::Case::Ignore, 1, 1}, constGrpExec  },
          { {A_CHAR("OthersRead")   , lang::Case::Ignore, 1, 1}, constOthRead  },
          { {A_CHAR("OthersWrite")  , lang::Case::Ignore, 1, 1}, constOthWrite },
          { {A_CHAR("OthersExecute"), lang::Case::Ignore, 1, 1}, constOthExec  },
        };

        Functions=
        {
          { {A_CHAR("Name")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getName  ), &Types::String  , ETI },
          { {A_CHAR("IsDirectory")  , lang::Case::Ignore, 2, 3}, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(isFolder ), &Types::Boolean , ETI },
          { {A_CHAR("Size")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getSize  ), &Types::Integer , ETI },
          { {A_CHAR("Date")         , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getDate  ), &Types::DateTime, ETI },
          { {A_CHAR("Permissions")  , lang::Case::Ignore, 4   }, CALCULUS_SIGNATURE(nullptr      ), CALCULUS_CALLBACK(getPerm  ), &TypePermission , ETI },
          { {A_CHAR("KiloBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(kiloBytes), &Types::Integer , CTI },
          { {A_CHAR("MegaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(megaBytes), &Types::Integer , CTI },
          { {A_CHAR("GigaBytes")    , lang::Case::Ignore, 1, 1}, CALCULUS_SIGNATURE(Signatures::I), CALCULUS_CALLBACK(gigaBytes), &Types::Integer , CTI },
        };

//! [DOX_ALIB_EXPR_TUT_FF_AutoCastCalculus]
AutoCasts=
{
    { TypePermission, nullptr, nullptr, CALCULUS_DEFAULT_AUTOCAST, nullptr, nullptr },
};
//! [DOX_ALIB_EXPR_TUT_FF_AutoCastCalculus]
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
} // namespace step13


} // anonymous namespace

ALIB_WARNINGS_RESTORE



// #################################################################################################
// #### Unit test executing tutorial code
// #################################################################################################

#define TESTCLASSNAME       CPP_ALib_Dox_Expr_Tutorial
#include "unittests/aworx_unittests.hpp"

namespace ut_aworx {

// #################################################################################################
// ### Test method used by the test below, with two macros. One for expressions understood by
// ### C++, one for those that are not compatible.
// #################################################################################################
//#define   CCOMP_CONSTEXPR(expr)               testExpression( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), expr  , 1 );
//#define   CONSTEXPR(expr, result)             testExpression( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), result, 1 );
#define   EXPRESSION(expr, result,expProgLen) testExpression( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), result, expProgLen );

extern
SPExpression testExpression( const NCString& file, int line, const NCString& func,
                             AWorxUnitTesting&     ut,
                             Compiler&             compiler,
                             ExpressionScope&   scope,
                             const String&         expressionString,
                             Box                   expected,
                             integer               programLength                               );

//#if !ALIB_DEBUG
//    #define      PRINTPRGRM(expr)
//#else
//    #define      PRINTPRGRM(expr) printProgram( ALIB_CALLER, ut, compiler, ALIB_STRINGIFY(expr) );
//
//extern
//SPExpression printProgram( const NCString& file, int line, const NCString& func,
//                           AWorxUnitTesting&  ut,
//                           Compiler&          compiler,
//                           const String&      expressionString         );
//#endif



UT_CLASS


UT_METHOD( FileSystemIntro )
{
    UT_INIT()
    {
//! [DOX_ALIB_EXPR_TUT_FF_1]
// search source path from current
auto sourceDir = fs::path(ALIB_BASE_DIR);
sourceDir+= "/src/alib/expressions";
ALIB_ASSERT_ERROR( fs::exists(sourceDir), "UNITTESTS", String512("Test directory not found: ") << sourceDir.c_str() )

// list files
for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    cout << directoryEntry.path().filename().string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_1]

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_INTRO-1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


//! [DOX_ALIB_EXPR_TUT_FF_FilterLoop]
step1::FileFilter filter(A_CHAR("expression string syntax not defined yet"));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( filter.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_FilterLoop]
    testOutputStreamN.str("");


//! [DOX_ALIB_EXPR_TUT_FF_TRUE]
cout << "--- Files using expression {true}: ---" << endl;

step2::FileFilter trueFilter(A_CHAR("true"));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( trueFilter.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_TRUE]

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_TRUE.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

//! [DOX_ALIB_EXPR_TUT_FF_FALSE]
cout << "--- Files using expression {false}: ---" << endl;

step2::FileFilter falseFilter(A_CHAR("false"));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( falseFilter.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_FALSE]

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_FALSE.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


//! [DOX_ALIB_EXPR_TUT_FF_Name1]
cout << "--- Files using expression {name == \"compiler.hpp\"}: ---" << endl;

step5::FileFilter filter1(A_CHAR("name == \"compiler.hpp\""));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( filter1.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_Name1]

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Name1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");



//! [DOX_ALIB_EXPR_TUT_FF_Name2]
cout << "--- Files using expression {WildcardMatch(name, \"*.hpp\"}: ---" << endl;

step5::FileFilter filter2(A_CHAR("WildcardMatch(name, \"*.hpp\")"));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( filter2.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_Name2]

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Name2.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

//! [DOX_ALIB_EXPR_TUT_FF_Name3]
cout << "--- Files using expression {name * \"*.cpp\"}: ---" << endl;

step5::FileFilter filter3(A_CHAR("name * \"*.cpp\""));

for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
    if( filter3.Includes( directoryEntry ) )
        cout << directoryEntry.path().filename().string() << endl;
//! [DOX_ALIB_EXPR_TUT_FF_Name3]

    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Name3.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


    // test if step6 implementation works
    int cnt= 0;
    step6::FileFilter filter61(A_CHAR("name * \"compiler.hpp\""));
    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter61.Includes( directoryEntry ) )
            ++cnt;
    UT_EQ(1, cnt)

    cnt= 0;
    step6::FileFilter filter62(A_CHAR("name * \"*.cpp\""));
    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter62.Includes( directoryEntry ) )
            ++cnt;
    UT_EQ(4, cnt)

    cnt= 0;
    step6::FileFilter filter63(A_CHAR("name * \"*.hpp\""));
    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter63.Includes( directoryEntry ) )
            ++cnt;
    UT_EQ(6, cnt)

    //---------------- samples after more  functionality was added ------------------
    cout << "--- Filter Expression {IsDirectory}: ---" << endl;
    step7::FileFilter filter71(A_CHAR("IsDirectory"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter71.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_More-1.txt", testOutputStreamN.str() ,"");
    testOutputStreamN.str("");

    cout << "--- Filter Expression {!IsDirectory && size < 20000}: ---" << endl;
    step7::FileFilter filter72(A_CHAR("!IsDirectory && size < 20000"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter72.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_More-2.txt", testOutputStreamN.str() ,"");
    testOutputStreamN.str("");

    cout << "--- Filter Expression {date > DateTime(2019,2,5)}: ---" << endl;
    step7::FileFilter filter73(A_CHAR("date > DateTime(2019,2,5)"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter73.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_More-3.txt", testOutputStreamN.str() ,"") ;
    testOutputStreamN.str("");

    cout << "--- Filter Expression {(permissions & OwnerExecute) != 0}: ---" << endl;
    step7::FileFilter filter74(A_CHAR("(permissions & OwnerExecute) != 0"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter74.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_More-4.txt", testOutputStreamN.str() ,"") ;
    testOutputStreamN.str("");

    cout << "--- Filter Expression {size > 20480}: ---" << endl;
    step7::FileFilter filter75(A_CHAR("size > 20480"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter75.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_More-5.txt", testOutputStreamN.str() ,"");
    testOutputStreamN.str("");


    //---------------- samples after more  functionality was added ------------------
    cout << "--- Filter Expression {size > kilobytes(20)}: ---" << endl;
    step8::FileFilter filter81(A_CHAR("size > kilobytes(20)"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter81.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Func-1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    //---------------- samples  added fs::permissions type ------------------
    cout << "--- Filter Expression {(permissions & OwnerExecute) != 0}: ---" << endl;
    try
    {
        step9::FileFilter filter91(A_CHAR("(permissions & OwnerExecute) != 0"));
    }
    catch(Exception& e)
    {
        ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Operators-1.txt", e.Format() );
    }

    //---------------- samples  added fs::permissions type: better exception ------------------
    cout << "--- Filter Expression {(permissions & OwnerExecute) != 0}: ---" << endl;
    try
    {
        step10::FileFilter filter101(A_CHAR("(permissions & OwnerExecute) != 0"));
    }
    catch(Exception& e)
    {
        ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Operators-2.txt", e.Format() );
    }

    //---------------- samples after adding operators ------------------
    testOutputStreamN.str("");
    cout << "--- Filter Expression {(permissions & OwnerExecute) == OwnerExecute}: ---" << endl;
    step11::FileFilter filter111(A_CHAR("(permissions & OwnerExecute) == OwnerExecute"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter111.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Func-3.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    //---------------- samples after adding auto casts ------------------
    testOutputStreamN.str("");
    cout << "--- Filter Expression {(permissions & OwnerExecute) == OwnerExecute}: ---" << endl;
    step12::FileFilter filter121(A_CHAR("(permissions & OwnerExecute) == OwnerExecute"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter121.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Func-4.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    cout << "--- Filter Expression {(permissions & 64) != 0}: ---" << endl;
    step12::FileFilter filter122(A_CHAR("(permissions & 64) != 0"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter122.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Func-5.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


    cout << "--- Filter Expression {(permissions & OwnerExecute) == OwnerExecute}: ---" << endl;
    step13::FileFilter filter13(A_CHAR("(permissions & OwnerExecute) == OwnerExecute"));

    for( auto& directoryEntry : fs::directory_iterator( sourceDir ) )
        if( filter13.Includes( directoryEntry ) )
            cout << directoryEntry.path().filename().string() << endl;
    ut.WriteResultFile( "DOX_ALIB_EXPR_TUT_FF_Func-6.txt", testOutputStreamN.str() );
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
ExpressionScope scope(compiler.CfgFormatter);
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
    UT_INIT()

    testOutputStreamN.str("");

    //----------------- Ops 1 --------------------
try {
//! [DOX_ALIB_EXPR_OPS_1]
Compiler compiler;
compiler.SetupDefaults();

ExpressionScope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile( A_CHAR("\"Hexadecimal: 0x{:x}\" {} 42") );
//! [DOX_ALIB_EXPR_OPS_1]
}
    catch(Exception& e)
    {
        ut.WriteResultFile( "DOX_ALIB_EXPR_OPS_1.txt", e.Format() );
        testOutputStreamN.str("");
    }

    //----------------- Ops 2 --------------------
try {
//! [DOX_ALIB_EXPR_OPS_2]
Compiler compiler;
compiler.SetupDefaults();

compiler.AddBinaryOperator( A_CHAR("{}") , 900);

ExpressionScope scope( compiler.CfgFormatter );
SPExpression expression= compiler.Compile( A_CHAR("\"Hexadecimal: 0x{:x}\" {} 42") );
//! [DOX_ALIB_EXPR_OPS_2]
}
    catch(Exception& e)
    {
        ut.WriteResultFile( "DOX_ALIB_EXPR_OPS_2.txt", e.Format() );
        testOutputStreamN.str("");
    }


    //----------------- Ops 3 --------------------
try {
//! [DOX_ALIB_EXPR_OPS_3]
Compiler compiler;
compiler.SetupDefaults();
compiler.AddBinaryOperator( A_CHAR("{}") , 900);

FormatOperator plugin( compiler );
compiler.InsertPlugin( &plugin, CompilePriorities::Custom );

ExpressionScope scope( compiler.CfgFormatter );
SPExpression expression= compiler.Compile( A_CHAR("\"Hexadecimal: 0x{:x}\" {} 42") );

cout << expression->Evaluate( scope )  << endl;
//! [DOX_ALIB_EXPR_OPS_3]
    ut.WriteResultFile( "DOX_ALIB_EXPR_OPS_3.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");
}
    catch(Exception& e)
    {
        ut.WriteResultFile( "DOX_ALIB_EXPR_OPS_3.txt", e.Format() );
    }


    //----------------- Verbal --------------------
    {
Compiler compiler;
compiler.SetupDefaults();
ExpressionScope scope( compiler.CfgFormatter );

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
} // test method operators



// #################################################################################################
// ### Nested
// #################################################################################################
UT_METHOD( Nested )
{
    UT_INIT()

    testOutputStreamN.str("");
    try
    {
//! [DOX_ALIB_EXPR_NESTED_OP_1]
Compiler compiler;
compiler.SetupDefaults();
ExpressionScope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(A_CHAR(R"(  *MyNestedExpression  )"));

cout << "Result: " << expression->Evaluate( scope );
//! [DOX_ALIB_EXPR_NESTED_OP_1]
    }
    catch(Exception& e)
    {
        ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_OP_1.txt", e.Format() );
    }


    testOutputStreamN.str("");
    try
    {
//! [DOX_ALIB_EXPR_NESTED_OP_2]
Compiler compiler;
compiler.SetupDefaults();
ExpressionScope scope( compiler.CfgFormatter );

compiler.AddNamed( A_CHAR("MyNestedExpression"), A_CHAR("6 * 7") );

SPExpression expression= compiler.Compile(A_CHAR(R"(  *MyNestedExpression  )"));

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
ExpressionScope scope( compiler.CfgFormatter );

compiler.AddNamed( A_CHAR("MyNestedExpression"), A_CHAR("6 * 7") );

//! [DOX_ALIB_EXPR_NESTED_OP_3]
SPExpression expression= compiler.Compile(A_CHAR(R"(   2 * *MyNestedExpression    )"));
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
ExpressionScope scope( compiler.CfgFormatter );

compiler.AddNamed( A_CHAR("MyNestedExpression"), A_CHAR("6 * 7") );

//! [DOX_ALIB_EXPR_NESTED_OP_4]
SPExpression expression= compiler.Compile( A_CHAR(R"(   *("MyNested" + "Expression")   )") );
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
ExpressionScope scope( compiler.CfgFormatter );

compiler.AddNamed( A_CHAR("MyNestedExpression"), A_CHAR("6 * 7") );

//! [DOX_ALIB_EXPR_NESTED_OP_5]
SPExpression expression= compiler.Compile(A_CHAR(R"(    *("MyNested" + ( random >= 0.0 ? "Expression" : "" ))     )") );
//! [DOX_ALIB_EXPR_NESTED_OP_5]

    }
    catch(Exception& e)
    {
        ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_OP_5.txt", e.Format() );
    }

    //----------------------------- function --------------------------------
    testOutputStreamN.str("");
    try
    {
//! [DOX_ALIB_EXPR_NESTED_FUNC_1]
Compiler compiler;
compiler.SetupDefaults();
ExpressionScope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(A_CHAR(R"(   Expression( "MyNestedExpression", -1 )   )"));

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
ExpressionScope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(A_CHAR(R"(   Expression( "MyNestedExpression", -1 )   )"));

compiler.AddNamed( A_CHAR("MyNestedExpression"), A_CHAR("3 * 3") );

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
ExpressionScope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(A_CHAR(R"(   Expression( "MyNestedExpression", -1 )   )"));

compiler.AddNamed( A_CHAR("MyNestedExpression"), A_CHAR("3 * 3") );
cout << "Result1: " << expression->Evaluate( scope ) << endl;

compiler.AddNamed( A_CHAR("MyNestedExpression"), A_CHAR("4 * 4") );
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
ExpressionScope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(A_CHAR(R"(   Expression( "MyNestedExpression", -1 )   )"));

compiler.AddNamed( A_CHAR("MyNestedExpression"), A_CHAR(R"(  "Hello"   )"));
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
ExpressionScope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(A_CHAR(R"(   Expression( "MyNestedExpression", -1 )   )"));

compiler.AddNamed( A_CHAR("MyNestedExpression"), A_CHAR(R"(  "Hello"   )"));

//! [DOX_ALIB_EXPR_NESTED_FUNC_5]
cout << "Result: " << expression->Evaluate( scope ) << endl;
//! [DOX_ALIB_EXPR_NESTED_FUNC_5]
    }
    catch(Exception& e)
    {
        ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_FUNC_5.txt", e.Format() );
    }

    testOutputStreamN.str("");
    try
    {
Compiler compiler;
compiler.SetupDefaults();
ExpressionScope scope( compiler.CfgFormatter );
{
//! [DOX_ALIB_EXPR_NESTED_FUNC_6]
SPExpression expression= compiler.Compile(A_CHAR(R"(   Expression( MyNestedExpression, -1 )   )"));
//! [DOX_ALIB_EXPR_NESTED_FUNC_6]
}
{
//! [DOX_ALIB_EXPR_NESTED_FUNC_7]
SPExpression expression= compiler.Compile(A_CHAR(R"(   Expression( ("MyNested" + ( random >= 0.0 ? "Expression" : "" )), -1 )   )"));
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
ExpressionScope scope( compiler.CfgFormatter );

SPExpression expression= compiler.Compile(A_CHAR(R"(   Expression( "MyNestedExpression", -1, throw )   )"));

cout << "Result: " << expression->Evaluate( scope ) << endl;
//! [DOX_ALIB_EXPR_NESTED_FUNC_41]
    }
    catch(Exception& e)
    {
        ut.WriteResultFile( "DOX_ALIB_EXPR_NESTED_FUNC_41.txt", e.Format() );
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
    ExpressionScope scope( compiler.CfgFormatter );

    if( dontOptimize )
        compiler.CfgCompilation+= Compilation::NoOptimization;

    compiler.AddNamed(A_CHAR("nested"), A_CHAR("5"));

    //---------------------- Compile -------------------
    try
    {
        SPExpression expression= compiler.Compile( expressionString );
        AString listing=  expressions::detail::VirtualMachine::DbgList(
            *dynamic_cast<expressions::detail::Program*>( expression->GetProgram() ) );
        ut.WriteResultFile( outputfilename, listing , "" ); // , "" -> all \verbinclude

    }
    catch( Exception& e )
    {
        ut.WriteResultFile( outputfilename, e.Format() );
        assert(!ut.AssertOnFailure);
    }
}

UT_METHOD( VMListings )
{
    UT_INIT()

    printProgram( ut, A_CHAR("42"),                                               "DOX_ALIB_EXPR_VM_-1.txt", false  );
    printProgram( ut, A_CHAR("42 * 2"),                                           "DOX_ALIB_EXPR_VM_-2.txt", false  );
    printProgram( ut, A_CHAR("42 * 2"),                                           "DOX_ALIB_EXPR_VM_-3.txt"  );
    printProgram( ut, A_CHAR("(((42 * 2) / 5) * (2 + 3) ) * 7"),                  "DOX_ALIB_EXPR_VM_-4.txt"  );
    printProgram( ut, A_CHAR("Format( \"Result of: {}\", \"2 * 3\", 2 * 3 )") ,   "DOX_ALIB_EXPR_VM_-5.txt"  );
    printProgram( ut, A_CHAR("true ? 1 : 2")                                  ,   "DOX_ALIB_EXPR_VM_-6.txt"  );
    printProgram( ut, A_CHAR("true ? 1 : 2")                                  ,   "DOX_ALIB_EXPR_VM_-6opt.txt", false  );
    printProgram( ut, A_CHAR("*nested")                                       ,   "DOX_ALIB_EXPR_VM_-7.txt"  );
    printProgram( ut, A_CHAR("Expression(nested,-1,throw)")                   ,   "DOX_ALIB_EXPR_VM_-8.txt"  );
}
#endif


#include "unittests/aworx_unittests_end.hpp"

} //namespace

ALIB_WARNINGS_RESTORE

#endif //  ALIB_UT_DOCS && ALIB_EXPRESSIONS
