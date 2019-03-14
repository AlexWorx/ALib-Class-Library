// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_EXPRESSIONS_EXPRESSIONS)
#   include "alib/expressions/expressions.hpp"
#endif

#if !defined (HPP_ALIB_RESULTS_RESULTS)
#    include "alib/results/results.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_VIRTUAL_MACHINE)
#   include "alib/expressions/detail/virtualmachine.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCE_TUPLE_LOADER)
#   include "alib/resources/resourcedtupleloader.hpp"
#endif

#if !defined (HPP_ALIB_LIB_ALIBMODULES)
#    include "alib/lib/alibmodules.hpp"
#endif

#if ALIB_MODULE_SYSTEM && !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_DATEANDTIME)
#   include "alib/expressions/plugins/dateandtime.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#    include "alib/resources/resources.hpp"
#endif

ALIB_BOXING_VTABLE_DEFINE( aworx::lib::expressions::Exceptions                              , vt_expressions_exceptions )
ALIB_BOXING_VTABLE_DEFINE( aworx::lib::expressions::detail::VirtualMachine::Command::OpCodes, vt_expressions_vmopcodes )

namespace aworx { namespace lib {

expressions::Expressions EXPRESSIONS;



/** ************************************************************************************************
 * This is the reference documentation of sub-namespace \b expressions of the \aliblink which
 * holds types of library module \alibmod_expressions.
 *
 * Extensive documentation for this module is provided with
 * \ref alib_mod_expressions "ALib Module Expressions - Programmer's Manual".
 **************************************************************************************************/
namespace expressions {}}}



// ##########################################################################################
// ### Resource loaders
// ##########################################################################################
using namespace aworx::lib::expressions;
ALIB_RESOURCED_IN_MODULE                ( DefaultUnaryOperators                 , aworx::lib::EXPRESSIONS, "UO"  )
ALIB_RESOURCED_IN_MODULE                ( DefaultBinaryOperators                , aworx::lib::EXPRESSIONS, "BO"  )
ALIB_RESOURCED_IN_MODULE                ( DefaultAlphabeticUnaryOperatorAliases , aworx::lib::EXPRESSIONS, "UOA" )
ALIB_RESOURCED_IN_MODULE                ( DefaultAlphabeticBinaryOperatorAliases, aworx::lib::EXPRESSIONS, "BOA" )
ALIB_ENUM_META_DATA_RESOURCE_LOADER( DefaultAlphabeticUnaryOperatorAliases  )
ALIB_ENUM_META_DATA_RESOURCE_LOADER( DefaultAlphabeticBinaryOperatorAliases )


namespace aworx { namespace lib { namespace expressions {

// ##########################################################################################
// ### Static sample objects and signatures
// ##########################################################################################
Box Types::Boolean = false;
Box Types::Integer = static_cast<integer>(0);
Box Types::Float   = static_cast<double >(0.0);
Box Types::String  = A_CHAR("");

#if ALIB_MODULE_SYSTEM
Box Types::DateTime= time::DateTime::FromRaw(0);
Box Types::Duration= time::DateTime::Duration::FromNanoseconds(0);
#endif

Box* Signatures::Var [1]  = { nullptr                                                     };
Box* Signatures::B   [1]  = { &Types::Boolean                                             };
Box* Signatures::BB  [2]  = { &Types::Boolean    ,  &Types::Boolean                       };
Box* Signatures::I   [1]  = { &Types::Integer                                             };
Box* Signatures::II  [2]  = { &Types::Integer    ,  &Types::Integer                       };
Box* Signatures::IVar[2]  = { &Types::Integer    ,  nullptr                               };
Box* Signatures::F   [1]  = { &Types::Float                                               };
Box* Signatures::FF  [2]  = { &Types::Float      ,  &Types::Float                         };
Box* Signatures::S   [1]  = { &Types::String                                              };
Box* Signatures::SVar[2]  = { &Types::String     ,  nullptr                               };
Box* Signatures::SS  [2]  = { &Types::String     ,  &Types::String                        };
Box* Signatures::SI  [2]  = { &Types::String     ,  &Types::Integer                       };
Box* Signatures::SSI [3]  = { &Types::String     ,  &Types::String    ,  &Types::Integer  };
Box* Signatures::SII [3]  = { &Types::String     ,  &Types::Integer   ,  &Types::Integer  };
Box* Signatures::SSB [3]  = { &Types::String     ,  &Types::String    ,  &Types::Boolean  };
Box* Signatures::SSS [3]  = { &Types::String     ,  &Types::String    ,  &Types::String   };
#if ALIB_MODULE_SYSTEM
Box* Signatures::D   [1]  = { &Types::DateTime                                            };
Box* Signatures::Dur [1]  = { &Types::Duration                                            };
Box* Signatures::DDur[2]  = { &Types::DateTime   ,  &Types::Duration                      };
#endif

//! @cond NO_DOX
#define EOS ,
//! @endcond

// ##########################################################################################
// ### Library class implementation
// ##########################################################################################

Expressions::Expressions()
: Module( ALIB_VERSION, ALIB_REVISION, "ALIB_EXPR" )
{
    ALIB_ASSERT_ERROR( this == &EXPRESSIONS,
       "Instances of class Expressions must not be created. Use singleton aworx::lib::EXPRESSIONS" )

    Dependencies.emplace_back( &lib::RESULTS );

    #if ALIB_MODULE_CONFIGURATION
        Dependencies.emplace_back( &lib::CONFIG );
    #else
        #if ALIB_MODULE_SYSTEM
            dependencies.emplace_back( &lib::system::System::GetSingleton() );
        #endif
    #endif

}


void Expressions::init( InitLevels level, int, const char**, const wchar_t** )
{
    if( level == InitLevels::PrepareResources )
    {
        ALIB.CheckDistribution();

        ALIB_BOXING_VTABLE_REGISTER( vt_expressions_exceptions )
        ALIB_BOXING_VTABLE_REGISTER( vt_expressions_vmopcodes  )
        ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( aworx::lib::expressions::Exceptions )
        #if ALIB_DEBUG
           ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( aworx::lib::expressions::detail::VirtualMachine::Command::OpCodes )
        #endif

        #if ALIB_MODULE_SYSTEM
            plugins::DateAndTime::Init();
        #endif

        // add resources
        Resources->AddBulk( ResourceCategory,

        // unary ops
        "UO0" , A_CHAR( "0"   ","     "NONE"     )  ,
        "UO1" , A_CHAR( "1"   ","     "+"        )  ,
        "UO2" , A_CHAR( "2"   ","     "-"        )  ,
        "UO3" , A_CHAR( "3"   ","     "!"        )  ,
        "UO4" , A_CHAR( "4"   ","     "~"        )  ,
        "UO5" , A_CHAR( "5"   ","     "*"        )  ,


        // Binary ops
         "BO0" , A_CHAR( "0"   ","     "NONE"    ","      "0" ) ,
         "BO1" , A_CHAR( "1"   ","     "[]"      ","      "0" ) ,
         "BO2" , A_CHAR( "2"   ","     "*"       ","    "900" ) ,
         "BO3" , A_CHAR( "3"   ","     "/"       ","    "900" ) ,
         "BO4" , A_CHAR( "4"   ","     "%"       ","    "900" ) ,
         "BO5" , A_CHAR( "5"   ","     "+"       ","    "800" ) ,
         "BO6" , A_CHAR( "6"   ","     "-"       ","    "800" ) ,
         "BO7" , A_CHAR( "7"   ","     "<<"      ","    "700" ) ,
         "BO8" , A_CHAR( "8"   ","     ">>"      ","    "700" ) ,
         "BO9" , A_CHAR( "9"   ","     "<"       ","    "600" ) ,
        "BO10" , A_CHAR("10"   ","     "<="      ","    "600" ) ,
        "BO11" , A_CHAR("11"   ","     ">"       ","    "600" ) ,
        "BO12" , A_CHAR("12"   ","     ">="      ","    "600" ) ,
        "BO13" , A_CHAR("13"   ","     "=="      ","    "500" ) ,
        "BO14" , A_CHAR("14"   ","     "!="      ","    "500" ) ,
        "BO15" , A_CHAR("15"   ","     "&"       ","    "470" ) ,
        "BO16" , A_CHAR("16"   ","     "^"       ","    "460" ) ,
        "BO17" , A_CHAR("17"   ","     "|"       ","    "450" ) ,
        "BO18" , A_CHAR("18"   ","     "&&"      ","    "440" ) ,
        "BO19" , A_CHAR("19"   ","     "||"      ","    "430" ) ,
        "BO20" , A_CHAR("20"   ","     "="       ","    "300" ) ,
        "BO21" , A_CHAR("21"   ","     "?:"      ","    "200" ) ,

        // verbal operator aliases
        "UOA0" , A_CHAR( "0"   ","     "Not"              ","   "!" )  ,
        "BOA0" , A_CHAR( "0"   ","     "And"              ","   "&&")  ,
        "BOA1" , A_CHAR( "1"   ","     "Or"               ","   "||")  ,
        "BOA2" , A_CHAR( "2"   ","     "Sm"               ","   "<" )  ,
        "BOA3" , A_CHAR( "3"   ","     "Smaller"          ","   "<" )  ,
        "BOA4" , A_CHAR( "4"   ","     "Smeq"             ","   "<=")  ,
        "BOA5" , A_CHAR( "5"   ","     "Smaller_Or_Equal" ","   "<=")  ,
        "BOA6" , A_CHAR( "6"   ","     "Gt"               ","   ">" )  ,
        "BOA7" , A_CHAR( "7"   ","     "Greater"          ","   ">" )  ,
        "BOA8" , A_CHAR( "8"   ","     "Gteq"             ","   ">=")  ,
        "BOA9" , A_CHAR( "9"   ","     "Greater_Or_Equal" ","   ">=")  ,
        "BOA10", A_CHAR("10"   ","     "Eq"               ","   "==")  ,
        "BOA11", A_CHAR("11"   ","     "Equals"           ","   "==")  ,
        "BOA12", A_CHAR("12"   ","     "Neq"              ","   "!=")  ,
        "BOA13", A_CHAR("13"   ","     "Not_equals"       ","   "!=")  ,

        // expression function and "throw" keyword
        "EF" ,     A_CHAR("Expression"  " I 4"),
        "EFT",     A_CHAR("throw"),

            "ExceptionsPrefix" , A_CHAR("expressions::") ,
            "ExceptionsPostfix", A_CHAR("")              ,

                             // parse errors
            "Exceptions",
            A_CHAR(    "1,SyntaxError"                        ",Exc1,"
                    "-1,ExpressionInfo"                     ",Exc-1,"
                    "-2,StdExceptionInfo"                   ",Exc-2,"
                     "2,EmptyExpressionString"              ",Exc2,"
                     "5,SyntaxErrorExpectation"             ",Exc5,"
                     "6,UnknownUnaryOperatorSymbol"         ",Exc6,"
                     "7,UnknownBinaryOperatorSymbol"        ",Exc7,"

                     // compile errors
                     "14,UnknownIdentifier"                  ",Exc14,"
                     "15,UnknownFunction"                    ",Exc15,"
                    "-15,FunctionHint"                       ",Exc-15,"
                     "16,UnaryOperatorNotDefined"            ",Exc16,"
                     "17,BinaryOperatorNotDefined"           ",Exc17,"

                    "22,MissingFunctionParentheses"         ",Exc22,"
                    "23,IdentifierWithFunctionParentheses"  ",Exc23,"

                    "25,IncompatibleTypesInConditional"     ",Exc25,"

                    "30,ExceptionInPlugin"                  ",Exc30,"
                    "40,ExceptionInCallback"                ",Exc40,"

                     // nested expressions
                    "50,NamedExpressionNotConstant"         ",Exc50,"
                    "51,NamedExpressionNotFound"            ",Exc51,"
                    "52,NestedExpressionNotFoundCT"         ",Exc52,"
                    "53,NestedExpressionCallArgumentMismatch"",Exc53,"
                    "54,NestedExpressionNotFoundET"         ",Exc54,"
                   "-54,WhenEvaluatingNestedExpression"     ",Exc-54,"
                    "55,NestedExpressionResultTypeError"    ",Exc55,"
                    "56,CircularNestedExpressions"          ",Exc56,"
                   "-56,CircularNestedExpressionsInfo"      ",Exc-56,"
                  "-500,InExpressionFormatter"              ",Exc-500"  ),

             "Exc1",    A_CHAR("Syntax error parsing expression.")
            ,"Exc-1",   A_CHAR("Expression: {{{}}}\\n"
                                   "             {!F}^->"  )
            ,"Exc-2",   A_CHAR("std::exception thrown: {!Q}.")

             ,"Exc2",   A_CHAR("Trying to parse empty expression string.")

             ,"Exc5",   A_CHAR("Syntax error while parsing expression{}")
             ,"Exc6",   A_CHAR("Unknown unary operator '{}' found.")
             ,"Exc7",   A_CHAR("Unknown binary operator '{}' found.")


            ,"Exc14",   A_CHAR("Unknown identifier {!Q}.")
            ,"Exc15",   A_CHAR("Unknown function '{}{}'.")
           ,"Exc-15",   A_CHAR("Found function {!Q}. Arguments not applicable.")

            ,"Exc16",   A_CHAR("Operator '{}' not defined for type {!Q}.")
            ,"Exc17",   A_CHAR("Operator '{}' not defined for types {!Q} and {!Q}.")


            ,"Exc22",   A_CHAR("Missing pair of empty parentheses '()' with call to function '{}'.")
            ,"Exc23",   A_CHAR("Function parentheses '()' not allowed identifier '{}'.")

            ,"Exc25",   A_CHAR("Incompatible types for conditional expression operator <{}> : <{}>.")

            ,"Exc30",   A_CHAR("Exception caught in 'callback function' while evaluating expression {!Q}.")

            ,"Exc40",   A_CHAR("Exception caught in 'callback function' while evaluating expression {!Q}.")

            ,"Exc50",   A_CHAR("Expression name has to be constant, if no expression return type is given.")
            ,"Exc51",   A_CHAR("Named expression {!Q} not found.")
            ,"Exc52",   A_CHAR("Compile-time defined nested expression {!Q} not found.")
            ,"Exc53",   A_CHAR("Erroneous arguments given with nested expression function {!Q}.")
            ,"Exc54",   A_CHAR("Evaluation-time defined nested expression {!Q} not found.")
           ,"Exc-54",   A_CHAR("Exception evaluating nested expression {!Q}.")
            ,"Exc55",   A_CHAR("Nested expression {!Q} returned wrong result type.\\n"
                                   "Type expected: {}\\n"
                                   "Type returned: {}"    )
            ,"Exc56",   A_CHAR("Circular nested expressions detected. Circular evaluation stack follows.")
           ,"Exc-56",   A_CHAR("Expression {!Q} contains nested expression {!Q}.")
          ,"Exc-500",   A_CHAR("Occured in ExpressionFormatter expression #{}.\\n"
                                   "of formatter string: {!Q}"    )

            EOS

            // exception "expectation" strings
            "ExcExp0",  A_CHAR(".")                                                ,
            "ExcExp1",  A_CHAR(": Closing brace ')' expected.") ,
            "ExcExp2",  A_CHAR(": Closing function parameter brace ')' expected.") ,
            "ExcExp3",  A_CHAR(": Closing subscript brace ']' expected.") ,
            "ExcExp4",  A_CHAR(": String literal closing quote ('\"') expected." ) ,
            "ExcExp5",  A_CHAR(": Operator expected.")                             ,
            "ExcExp6",  A_CHAR(": Colon ':' of conditional term expected.")        ,
            "ExcExp7",  A_CHAR(": Right-hand side binary operand expected.")       ,
            "ExcExp20", A_CHAR(": Unexpected end of expression.") ,
            "ExcExp21", A_CHAR(": Misplaced closing bracket.") ,
            "ExcExp22", A_CHAR(": Misplaced subscript operator symbol.") ,
            "ExcExp23", A_CHAR(": Misplaced comma.") ,



"ANON_EXPR_NAME",      A_CHAR("ANONYMOUS"),

#if ALIB_DEBUG
"VM_CMD_OPCODES",      A_CHAR("5,Subroutine"   ","
                            "4,Jump"         ","
                            "3,JumpIfFalse"  ","
                            "2,Function"     ","
                            "1,Constant"         ) EOS

"ProgListHeader",      A_CHAR("@HL-"
                            "ALib Expression Compiler\\n"
                            "(c) 2019 AWorx GmbH. Published under MIT License (Open Source).\\n"
                            "More Info: https://github.com/AlexWorx/ALib-Class-Library.\\n"
                            "@HL-"
                            "Expression name: {}\\n"
                            "     Normalized: {{{}}}\\n"
                            "\\n")
                 EOS

"ProgListHdl0",  A_CHAR("PC")            ,
"ProgListHdl1",  A_CHAR("ResultType")    ,
"ProgListHdl2",  A_CHAR("Command")       ,
"ProgListHdl3",  A_CHAR("Param")         ,
"ProgListHdl4",  A_CHAR("Stack")         ,
"ProgListHdl5",  A_CHAR("Description")   ,
"ProgListHdl6",  A_CHAR("ArgNo{Start..End}")   ,

"ProgListLine",  A_CHAR("{:<02} | {!AW} | {!AW} | {!AW} | {!AW} | {!AW} | {!AW} |{!Fill}{}\\n")

                 EOS


"ProgListFooter",  A_CHAR("@HL-")
                 EOS
#endif


        // Identifier/Function names
        "Arithmethics"  , A_CHAR("true"     " I 4,"
                               "false"    " I 5,"
                               "yes"      " I 3,"
                               "no"       " I 2,"
                               "on"       " I 2,"
                               "off"      " I 3,"
                               "Boolean"  " I 4,"
                               "Integer"  " I 3,"
                               "Float"    " I 5"     )EOS

        "Arithmethics2" , A_CHAR("Length"   " I 3"    )EOS

        "Math"          , A_CHAR("PI"         " I 2,"    "E"       " I 1,"
                               "abs"        " I 3,"    "ceil"    " I 4,"     "floor"  " I 5,"       "trunc"  " I 5,"
                               "round"      " I 5,"    "rint"    " I 4,"     "max"    " I 3,"       "min"    " I 3,"
                               "remainder"  " I 6,"    "random"  " I 4,"     "sin"    " I 3,"       "asin"   " I 4,"
                               "cos"        " I 3,"    "acos"    " I 4,"     "tan"    " I 3,"       "atan"   " I 4,"
                               "sinh"       " I 4,"    "asinh"   " I 5,"     "cosh"   " I 4,"       "acosh"  " I 5,"
                               "tanh"       " I 4,"    "atanh"   " I 5,"     "exp"    " I 3,"       "exp2"   " I 4,"
                               "exp10"      " I 5,"    "log"     " I 3,"     "log2"   " I 4,"       "log10"  " I 5,"
                               "pow"        " I 3,"    "pow10"   " I 5,"     "sqrt"   " I 4,"       "cbrt"   " I 4,"  )EOS

        "Strings",        A_CHAR("NewLine"  " I 1 1,"    "Tabulator"  " I 3," "String"  " I 3,"      "WildCardMatch"  " I 1 1 1,")
#if ALIB_FEAT_BOOST_REGEX
                          A_CHAR("RegExMatch"  " I 1 1 1,")
#endif
                          A_CHAR("Format"      " I 6,"     "ToUpper"   " I 1 1,"   "ToLower"    " I 1 1,"   "Compare"      " I 4,"
                               "StartsWith"  " I 1 1,"   "EndsWith"  " I 1 1,"   "Substring"  " I 4,"     "IndexOf"      " I 3 1,"
                               "Count"       " I 5,"     "Trim"      " I 4,"     "TrimStart"  " I 4 1,"   "TrimEnd"      " I 4 1,"
                               "Integer"     " I 3,"     "Float"     " I 5,"     "Token"      " I 3,"     "Hexadecimal"  " I 3,"
                               "Octal"       " I 3,"     "Binary"    " I 3,"     "Replace"    " I 4,"     "Repeat"       " I 6,"     )EOS

#if ALIB_MODULE_SYSTEM

        "DateAndTime"  , A_CHAR("January"  " I 3," "February" " I 3,"  "March"     " I 3,"  "April"   " I 3,"   "May"      " I 3,"  "June"     " I 3,"
                               "July"     " I 3," "August"   " I 3,"  "September" " I 3,"  "October" " I 3,"   "November" " I 3,"  "December" " I 3,"
                               "Sunday"   " I 3," "Monday"   " I 3,"  "Tuesday"   " I 3,"   "Wednesday"  " I 3,"
                               "Thursday" " I 3," "Friday"   " I 3,"  "Saturday"  " I 3,"

                               "NanoSecondS"  " I 1 1 0," "MicroSecondS"  " I 3 1 0," "MilliSecondS"  " I 1 1 0,"  "SecondS"  " I 3 0," "MinuteS"  " I 3 0,"
                               "Hours"        " I 4,"     "Days"          " I 3,"     "Weeks"         " I 4,"      "Months"   " I 3,"   "Years"    " I 4,"

                               "InDayS"          " I 2 1 0,"   "InHourS"         " I 2 1 0,"    "InMinuteS"     " I 2 3 0,"   "InSecondS"  " I 2 3 0,"
                               "InMilliSecondS"  " I 2 1 1 0," "InMicroSecondS"  " I 2 3 1 0,"  "InNanoSecondS" " I 2 1 1 0," "InHertZ"    " I 2 1 1,"

                               "DateTime"     " I 1 1,"
                               "UtcDateTime"  " I 1 1 1,"
                               "Now"          " I 3,"
                               "Today"        " I 5,"
                               "UtcToday"     " I 1 5,"
                               "Age"          " I 3,"
                               "IsOlderThan"  " I 2 1 1,"

                               "GetYear"    " I 1 1,"   "GetMonth"     " I 1 3,"   "GetDay"            " I 1 1,"    "GetDayOfWeek"    " I 1 1 1 1,"
                               "GetHour"    " I 1 1,"   "GetMinute"    " I 1 3,"   "GetMilliSecond"    " I 1 1 1,"

                               "GetUtcYear" " I 1 1 1," "GetUtcMonth"  " I 1 1 3," "GetUtcDay"         " I 1 1 1,"  "GetUtcDayOfWeek" " I 1 1 1 1 1,"
                               "GetUtcHour" " I 1 1 1," "GetUtcMinute" " I 1 1 3," "GetUtcMilliSecond" " I 1 1 1 1"
                            )EOS
#endif

        // end of AddBulk()
        nullptr);

    }
    else if( level == InitLevels::PrepareConfig )
    {
        EnumMetaData<expressions::DefaultUnaryOperators                 >::GetSingleton().CheckLoad();
        EnumMetaData<expressions::DefaultBinaryOperators                >::GetSingleton().CheckLoad();
        EnumMetaData<expressions::DefaultAlphabeticUnaryOperatorAliases >::GetSingleton().CheckLoad();
        EnumMetaData<expressions::DefaultAlphabeticBinaryOperatorAliases>::GetSingleton().CheckLoad();
    }
}



void Expressions::terminationCleanUp()
{
}


}}} // namespace [aworx::lib::expressions]


#undef EOS
