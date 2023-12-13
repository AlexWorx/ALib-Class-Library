// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_RESULTS_RESULTS)
#      include "alib/results/results.hpp"
#   endif
#   if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_VIRTUAL_MACHINE)
#      include "alib/expressions/detail/virtualmachine.hpp"
#   endif
#   if ALIB_SYSTEM && !defined(HPP_ALIB_SYSTEM_ERRORS)
#       include "alib/system/systemerrors.hpp"
#   endif
#   if ALIB_SYSTEM && !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_DATEANDTIME)
#      include "alib/expressions/plugins/dateandtime.hpp"
#   endif
#   if !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif
#   if ALIB_CONFIGURATION && !defined (HPP_ALIB_CONFIG_CONFIG)
#      include "alib/config/config.hpp"
#   endif
#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#       include "alib/enums/serialization.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#      include "alib/enums/recordbootstrap.hpp"
#   endif
#   if ALIB_SYSTEM && !defined( HPP_ALIB_TIME_DATETIME)
#      include "alib/time/datetime.hpp"
#   endif
#endif // !defined(ALIB_DOX)

ALIB_BOXING_VTABLE_DEFINE( aworx::lib::expressions::Exceptions                              , vt_expressions_exceptions )
ALIB_BOXING_VTABLE_DEFINE( aworx::lib::expressions::detail::VirtualMachine::Command::OpCodes, vt_expressions_vmopcodes )

namespace aworx { namespace lib {

expressions::Expressions EXPRESSIONS;

/** ************************************************************************************************
 * This is the reference documentation of sub-namespace \c expressions of the \aliblink, which
 * holds types of library module \alib_expressions.
 *
 * Extensive documentation for this module is provided with
 * \ref alib_mod_expressions "ALib Module Expressions - Programmer's Manual".
 **************************************************************************************************/
namespace expressions {




// ##########################################################################################
// ### Static sample objects and signatures
// ##########################################################################################
Box Types::Void    = nullptr;
Box Types::Boolean = false;
Box Types::Integer = static_cast<integer>(0);
Box Types::Float   = static_cast<double >(0.0);
Box Types::String  = A_CHAR("");

#if ALIB_SYSTEM
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
#if ALIB_SYSTEM
Box* Signatures::D   [1]  = { &Types::DateTime                                            };
Box* Signatures::Dur [1]  = { &Types::Duration                                            };
Box* Signatures::DDur[2]  = { &Types::DateTime   ,  &Types::Duration                      };
#endif

#if !defined(ALIB_DOX)
#   define EOS ,
#endif

// ##########################################################################################
// ### Module class implementation
// ##########################################################################################

Expressions::Expressions()
: Module( ALIB_VERSION, ALIB_REVISION, "EXPR" )
{
    ALIB_ASSERT_ERROR( this == &EXPRESSIONS, "EXPR",
       "Instances of class Expressions must not be created. Use singleton aworx::lib::EXPRESSIONS" )
}


void Expressions::bootstrap( BootstrapPhases phase, int, const char**, const wchar_t** )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        ALIB.CheckDistribution();

        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_expressions_exceptions )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_expressions_vmopcodes  )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( aworx::lib::expressions::Exceptions )
ALIB_DBG(ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( aworx::lib::expressions::detail::VirtualMachine::Command::OpCodes ) )

        ALIB_IF_SYSTEM( plugins::DateAndTime::Bootstrap(); )

        // add resources
#if !ALIB_RESOURCES_OMIT_DEFAULTS
        resourcePool->BootstrapBulk( ResourceCategory,

    // Type names

    "T_VOID"    ,      A_CHAR( "Void"     )  ,
    "T_BOOL"    ,      A_CHAR( "Boolean"  )  ,
    "T_INT"     ,      A_CHAR( "Integer"  )  ,
    "T_FLOAT"   ,      A_CHAR( "Float"    )  ,
    "T_STR"     ,      A_CHAR( "String"   )  ,
    "T_DATE"    ,      A_CHAR( "DateTime" )  ,
    "T_DUR"     ,      A_CHAR( "Duration" )  ,

    // unary ops
    "UO0" , A_CHAR( "0"   ","     "NONE"     )  ,
    "UO1" , A_CHAR( "1"   ","     "+"        )  ,
    "UO2" , A_CHAR( "2"   ","     "-"        )  ,
    "UO3" , A_CHAR( "3"   ","     "!"        )  ,
    "UO4" , A_CHAR( "4"   ","     "~"        )  ,
    "UO5" , A_CHAR( "5"   ","     "*"        )  ,


    // Binary ops
    "BO0"  , A_CHAR( "0"   ","     "NONE"    ","      "0" ) ,
    "BO1"  , A_CHAR( "1"   ","     "[]"      ","    "100" ) ,
    "BO2"  , A_CHAR( "2"   ","     "*"       ","    "900" ) ,
    "BO3"  , A_CHAR( "3"   ","     "/"       ","    "900" ) ,
    "BO4"  , A_CHAR( "4"   ","     "%"       ","    "900" ) ,
    "BO5"  , A_CHAR( "5"   ","     "+"       ","    "800" ) ,
    "BO6"  , A_CHAR( "6"   ","     "-"       ","    "800" ) ,
    "BO7"  , A_CHAR( "7"   ","     "<<"      ","    "700" ) ,
    "BO8"  , A_CHAR( "8"   ","     ">>"      ","    "700" ) ,
    "BO9"  , A_CHAR( "9"   ","     "<"       ","    "600" ) ,
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
    "EF"   , A_CHAR("Expression"  " I 4"),
    "EFT"  , A_CHAR("throw"),

    "E<"   , A_CHAR("expressions::") ,

    // parse errors
    "E0" , A_CHAR( "1,SyntaxError"                             ",ED1"   ),
    "E1" , A_CHAR( "-1,ExpressionInfo"                         ",ED-1"  ),
    "E2" , A_CHAR( "-2,StdExceptionInfo"                       ",ED-2"  ),
    "E3" , A_CHAR(  "2,EmptyExpressionString"                  ",ED2"   ),
    "E4" , A_CHAR(  "5,SyntaxErrorExpectation"                 ",ED5"   ),
    "E5" , A_CHAR(  "6,UnknownUnaryOperatorSymbol"             ",ED6"   ),
    "E6" , A_CHAR(  "7,UnknownBinaryOperatorSymbol"            ",ED7"   ),

    // compile errors
    "E7" , A_CHAR(  "14,UnknownIdentifier"                     ",ED14"  ),
    "E8" , A_CHAR(  "15,UnknownFunction"                       ",ED15"  ),
    "E9" , A_CHAR( "-15,FunctionHint"                          ",ED-15" ),
    "E10", A_CHAR(  "16,UnaryOperatorNotDefined"               ",ED16"  ),
    "E11", A_CHAR(  "17,BinaryOperatorNotDefined"              ",ED17"  ),

    "E12", A_CHAR(  "22,MissingFunctionParentheses"            ",ED22"  ),
    "E13", A_CHAR(  "23,IdentifierWithFunctionParentheses"     ",ED23"  ),

    "E14", A_CHAR(  "25,IncompatibleTypesInConditional"        ",ED25"  ),

    "E15", A_CHAR(  "30,ExceptionInPlugin"                     ",ED30"  ),
    "E16", A_CHAR(  "40,ExceptionInCallback"                   ",ED40"  ),


    // nested expressions
    "E17", A_CHAR(  "50,NamedExpressionNotConstant"            ",ED50"  ),
    "E18", A_CHAR(  "51,NamedExpressionNotFound"               ",ED51"  ),
    "E19", A_CHAR(  "52,NestedExpressionNotFoundCT"            ",ED52"  ),
    "E20", A_CHAR(  "53,NestedExpressionCallArgumentMismatch"  ",ED53"  ),
    "E21", A_CHAR(  "54,NestedExpressionNotFoundET"            ",ED54"  ),
    "E22", A_CHAR( "-54,WhenEvaluatingNestedExpression"        ",ED-54" ),
    "E23", A_CHAR(  "55,NestedExpressionResultTypeError"       ",ED55"  ),
    "E24", A_CHAR(  "56,CircularNestedExpressions"             ",ED56"  ),
    "E25", A_CHAR( "-56,CircularNestedExpressionsInfo"         ",ED-56" ),
    "E26", A_CHAR("-500,InExpressionFormatter"                 ",ED-500" ),

    "ED1",    A_CHAR( "Syntax error parsing expression."                                           ),
    "ED-1",   A_CHAR( "Expression: {{{}}}\n"
                      "             {!F}^->"                                                       ),
    "ED-2",   A_CHAR( "std::exception thrown: {!Q}."                                               ),

    "ED2",    A_CHAR( "Trying to parse empty expression string."                                   ),

    "ED5",    A_CHAR( "Syntax error while parsing expression{}"                                    ),
    "ED6",    A_CHAR( "Unknown unary operator {!Q'} found."                                        ),
    "ED7",    A_CHAR( "Unknown binary operator {!Q'} found."                                       ),


    "ED14",   A_CHAR( "Unknown identifier {!Q}."                                                   ),
    "ED15",   A_CHAR( "Unknown function '{}{}'."                                                   ),
    "ED-15",  A_CHAR( "Found function {!Q}. Arguments not applicable."                             ),

    "ED16",   A_CHAR( "Operator {!Q'} not defined for type {!Q}."                                  ),
    "ED17",   A_CHAR( "Operator {!Q'} not defined for types {!Q} and {!Q}."                        ),


    "ED22",   A_CHAR( "Missing function parentheses '()' with call to function {!Q'}."             ),
    "ED23",   A_CHAR( "Function parentheses '()' are not allowed on identifier {!Q'}."             ),

    "ED25",   A_CHAR( "Incompatible types for conditional expression operator {!Q<>} : {!Q<>}."    ),

    "ED30",   A_CHAR( "Exception caught in 'callback function' while evaluating expression {!Q}."  ),

    "ED40",   A_CHAR( "Exception caught in 'callback function' while evaluating expression {!Q}."  ),

    "ED50",   A_CHAR( "Expression name has to be constant, if no expression return type is given." ),
    "ED51",   A_CHAR( "Named expression {!Q} not found."                                           ),
    "ED52",   A_CHAR( "Compile-time defined nested expression {!Q} not found."                     ),
    "ED53",   A_CHAR( "Erroneous arguments given with nested expression function {!Q}."            ),
    "ED54",   A_CHAR( "Evaluation-time defined nested expression {!Q} not found."                  ),
    "ED-54",  A_CHAR( "Exception evaluating nested expression {!Q}."                               ),
    "ED55",   A_CHAR( "Nested expression {!Q} returned wrong result type.\n"
                      "Type expected: {}\n"
                      "Type returned: {}"                                                          ),
    "ED56",   A_CHAR( "Circular nested expressions detected. Circular evaluation stack follows."   ),
    "ED-56",  A_CHAR( "Expression {!Q} contains nested expression {!Q}."                           ),
    "ED-500", A_CHAR( "Occurred in ExpressionFormatter expression #{}.\n"
                      "of formatter string: {!Q}"                                                  ) EOS

    // exception "expectation" strings
    "EE1",    A_CHAR(": Closing brace ')' expected."                     ),
    "EE2",    A_CHAR(": Closing function parameter brace ')' expected."  ),
    "EE3",    A_CHAR(": Closing subscript brace ']' expected."           ),
    "EE4",    A_CHAR(": String literal closing quote ('\"') expected."   ),
    "EE5",    A_CHAR(": Operator expected."                              ),
    "EE6",    A_CHAR(": Colon ':' of conditional term expected."         ),
    "EE7",    A_CHAR(": Right-hand side binary operand expected."        ),
    "EE20",   A_CHAR(": Unexpected end of expression."                   ),
    "EE21",   A_CHAR(": Misplaced closing bracket."                      ),
    "EE22",   A_CHAR(": Misplaced subscript operator symbol."            ),
    "EE23",   A_CHAR(": Misplaced comma."                                ),

    "ANON_EXPR_NAME",      A_CHAR( "ANONYMOUS" ) EOS

#if ALIB_DEBUG
    "VM_CMD_OPCODES",      A_CHAR( "5,Subroutine"   ",1,"
                                   "3,JumpIfFalse"  ",5,"
                                   "4,Jump"         ",1,"
                                   "2,Function"     ",1,"
                                   "1,Constant"     ",1"    ) EOS

    "ProgListHeader",      A_CHAR( "@HL-"
                                   "ALib Expression Compiler\n"
                                   "(c) 2023 AWorx GmbH. Published under MIT License (Open Source).\n"
                                   "More Info: https://alib.dev\n"
                                   "@HL-"
                                   "Expression name: {}\n"
                                   "     Normalized: {{{}}}\n"
                                   "\n"                            ) EOS


    "ProgListHdl0",  A_CHAR("PC"                ),
    "ProgListHdl1",  A_CHAR("ResultType"        ),
    "ProgListHdl2",  A_CHAR("Command"           ),
    "ProgListHdl3",  A_CHAR("Param"             ),
    "ProgListHdl4",  A_CHAR("Stack"             ),
    "ProgListHdl5",  A_CHAR("Description"       ),
    "ProgListHdl6",  A_CHAR("ArgNo{Start..End}" ),

    "ProgListLine",  A_CHAR("{:<02} | {!AW} | {!AW} | {!AW} | {!AW} | {!AW} | {!AW} |{!Fill}{}\n")     EOS


    "ProgListFooter",A_CHAR("@HL-")        EOS
#endif


    // Identifier/Function names
     "CPA0"   , A_CHAR( "true"       " I 4" ),
     "CPA1"   , A_CHAR( "false"      " I 5" ),
     "CPA2"   , A_CHAR( "yes"        " I 3" ),
     "CPA3"   , A_CHAR( "no"         " I 2" ),
     "CPA4"   , A_CHAR( "on"         " I 2" ),
     "CPA5"   , A_CHAR( "off"        " I 3" ),
     "CPA6"   , A_CHAR( "Boolean"    " I 4" ),
     "CPA7"   , A_CHAR( "Integer"    " I 3" ),
     "CPA8"   , A_CHAR( "Float"      " I 5"  ) EOS

    "CPALen"  , A_CHAR("Length"      " I 3"  ) EOS

     "CPM0"   , A_CHAR( "PI"         " I 2" ),
     "CPM1"   , A_CHAR( "E"          " I 1" ),
     "CPM2"   , A_CHAR( "abs"        " I 3" ),
     "CPM3"   , A_CHAR( "ceil"       " I 4" ),
     "CPM4"   , A_CHAR( "floor"      " I 5" ),
     "CPM5"   , A_CHAR( "trunc"      " I 5" ),
     "CPM6"   , A_CHAR( "round"      " I 5" ),
     "CPM7"   , A_CHAR( "rint"       " I 4" ),
     "CPM8"   , A_CHAR( "max"        " I 3" ),
     "CPM9"   , A_CHAR( "min"        " I 3" ),
    "CPM10"   , A_CHAR( "remainder"  " I 6" ),
    "CPM11"   , A_CHAR( "random"     " I 4" ),
    "CPM12"   , A_CHAR( "sin"        " I 3" ),
    "CPM13"   , A_CHAR( "asin"       " I 4" ),
    "CPM14"   , A_CHAR( "cos"        " I 3" ),
    "CPM15"   , A_CHAR( "acos"       " I 4" ),
    "CPM16"   , A_CHAR( "tan"        " I 3" ),
    "CPM17"   , A_CHAR( "atan"       " I 4" ),
    "CPM18"   , A_CHAR( "sinh"       " I 4" ),
    "CPM19"   , A_CHAR( "asinh"      " I 5" ),
    "CPM20"   , A_CHAR( "cosh"       " I 4" ),
    "CPM21"   , A_CHAR( "acosh"      " I 5" ),
    "CPM22"   , A_CHAR( "tanh"       " I 4" ),
    "CPM23"   , A_CHAR( "atanh"      " I 5" ),
    "CPM24"   , A_CHAR( "exp"        " I 3" ),
    "CPM25"   , A_CHAR( "exp2"       " I 4" ),
    "CPM26"   , A_CHAR( "exp10"      " I 5" ),
    "CPM27"   , A_CHAR( "log"        " I 3" ),
    "CPM28"   , A_CHAR( "log2"       " I 4" ),
    "CPM29"   , A_CHAR( "log10"      " I 5" ),
    "CPM30"   , A_CHAR( "pow"        " I 3" ),
    "CPM31"   , A_CHAR( "pow10"      " I 5" ),
    "CPM32"   , A_CHAR( "sqrt"       " I 4" ),
    "CPM33"   , A_CHAR( "cbrt"       " I 4" ) EOS



     "CPS0"   , A_CHAR( "NewLine"        " I 1 1"  ),
     "CPS1"   , A_CHAR( "Tabulator"      " I 3"    ),
     "CPS2"   , A_CHAR( "String"         " I 3"    ),
     "CPS3"   , A_CHAR( "WildCardMatch"  " I 1 1 1"),
     "CPS4"   , A_CHAR( "Format"         " I 6"    ),
     "CPS5"   , A_CHAR( "ToUpper"        " I 1 1"  ),
     "CPS6"   , A_CHAR( "ToLower"        " I 1 1"  ),
     "CPS7"   , A_CHAR( "Compare"        " I 4"    ),
     "CPS8"   , A_CHAR( "StartsWith"     " I 1 1"  ),
     "CPS9"   , A_CHAR( "EndsWith"       " I 1 1"  ),
    "CPS10"   , A_CHAR( "Substring"      " I 4"    ),
    "CPS11"   , A_CHAR( "IndexOf"        " I 3 1"  ),
    "CPS12"   , A_CHAR( "Count"          " I 5"    ),
    "CPS13"   , A_CHAR( "Trim"           " I 4"    ),
    "CPS14"   , A_CHAR( "TrimStart"      " I 4 1"  ),
    "CPS15"   , A_CHAR( "TrimEnd"        " I 4 1"  ),
    "CPS16"   , A_CHAR( "Integer"        " I 3"    ),
    "CPS17"   , A_CHAR( "Float"          " I 5"    ),
    "CPS18"   , A_CHAR( "Token"          " I 3"    ),
    "CPS19"   , A_CHAR( "Hexadecimal"    " I 3"    ),
    "CPS20"   , A_CHAR( "Octal"          " I 3"    ),
    "CPS21"   , A_CHAR( "Binary"         " I 3"    ),
    "CPS22"   , A_CHAR( "Replace"        " I 4"    ),
    "CPS23"   , A_CHAR( "Repeat"         " I 6"    )
#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
  , "CPS24"   , A_CHAR( "RegExMatch"     " I 1 1 1")
#endif
                                                      EOS


#if ALIB_SYSTEM
     "CPD0"   , A_CHAR( "January"               " I 3,"         ),
     "CPD1"   , A_CHAR( "February"              " I 3,"         ),
     "CPD2"   , A_CHAR( "March"                 " I 3,"         ),
     "CPD3"   , A_CHAR( "April"                 " I 3,"         ),
     "CPD4"   , A_CHAR( "May"                   " I 3,"         ),
     "CPD5"   , A_CHAR( "June"                  " I 3,"         ),
     "CPD6"   , A_CHAR( "July"                  " I 3,"         ),
     "CPD7"   , A_CHAR( "August"                " I 3,"         ),
     "CPD8"   , A_CHAR( "September"             " I 3,"         ),
     "CPD9"   , A_CHAR( "October"               " I 3,"         ),
    "CPD10"   , A_CHAR( "November"              " I 3,"         ),
    "CPD11"   , A_CHAR( "December"              " I 3,"         ),
    "CPD12"   , A_CHAR( "Sunday"                " I 3,"         ),
    "CPD13"   , A_CHAR( "Monday"                " I 3,"         ),
    "CPD14"   , A_CHAR( "Tuesday"               " I 3,"         ),
    "CPD15"   , A_CHAR( "Wednesday"             " I 3,"         ),
    "CPD16"   , A_CHAR( "Thursday"              " I 3,"         ),
    "CPD17"   , A_CHAR( "Friday"                " I 3,"         ),
    "CPD18"   , A_CHAR( "Saturday"              " I 3,"         ),
    "CPD19"   , A_CHAR( "NanoSecondS"           " I 1 1 0,"     ),
    "CPD20"   , A_CHAR( "MicroSecondS"          " I 3 1 0,"     ),
    "CPD21"   , A_CHAR( "MilliSecondS"          " I 1 1 0,"     ),
    "CPD22"   , A_CHAR( "SecondS"               " I 3 0,"       ),
    "CPD23"   , A_CHAR( "MinuteS"               " I 3 0,"       ),
    "CPD24"   , A_CHAR( "Hours"                 " I 4,"         ),
    "CPD25"   , A_CHAR( "Days"                  " I 3,"         ),
    "CPD26"   , A_CHAR( "Weeks"                 " I 4,"         ),
    "CPD27"   , A_CHAR( "Months"                " I 3,"         ),
    "CPD28"   , A_CHAR( "Years"                 " I 4,"         ),
    "CPD29"   , A_CHAR( "InDayS"                " I 2 1 0,"     ),
    "CPD30"   , A_CHAR( "InHourS"               " I 2 1 0,"     ),
    "CPD31"   , A_CHAR( "InMinuteS"             " I 2 3 0,"     ),
    "CPD32"   , A_CHAR( "InSecondS"             " I 2 3 0,"     ),
    "CPD33"   , A_CHAR( "InMilliSecondS"        " I 2 1 1 0,"   ),
    "CPD34"   , A_CHAR( "InMicroSecondS"        " I 2 3 1 0,"   ),
    "CPD35"   , A_CHAR( "InNanoSecondS"         " I 2 1 1 0,"   ),
    "CPD36"   , A_CHAR( "InHertZ"               " I 2 1 1,"     ),
    "CPD37"   , A_CHAR( "DateTime"              " I 1 1,"       ),
    "CPD38"   , A_CHAR( "UtcDateTime"           " I 1 1 1,"     ),
    "CPD39"   , A_CHAR( "Now"                   " I 3,"         ),
    "CPD40"   , A_CHAR( "Today"                 " I 5,"         ),
    "CPD41"   , A_CHAR( "UtcToday"              " I 1 5,"       ),
    "CPD42"   , A_CHAR( "Age"                   " I 3,"         ),
    "CPD43"   , A_CHAR( "IsOlderThan"           " I 2 1 1,"     ),
    "CPD44"   , A_CHAR( "GetYear"               " I 1 1,"       ),
    "CPD45"   , A_CHAR( "GetMonth"              " I 1 3,"       ),
    "CPD46"   , A_CHAR( "GetDay"                " I 1 1,"       ),
    "CPD47"   , A_CHAR( "GetDayOfWeek"          " I 1 1 1 1,"   ),
    "CPD48"   , A_CHAR( "GetHour"               " I 1 1,"       ),
    "CPD49"   , A_CHAR( "GetMinute"             " I 1 3,"       ),
    "CPD50"   , A_CHAR( "GetMilliSecond"        " I 1 1 1,"     ),
    "CPD51"   , A_CHAR( "GetUtcYear"            " I 1 1 1,"     ),
    "CPD52"   , A_CHAR( "GetUtcMonth"           " I 1 1 3,"     ),
    "CPD53"   , A_CHAR( "GetUtcDay"             " I 1 1 1,"     ),
    "CPD54"   , A_CHAR( "GetUtcDayOfWeek"       " I 1 1 1 1 1," ),
    "CPD55"   , A_CHAR( "GetUtcHour"            " I 1 1 1,"     ),
    "CPD56"   , A_CHAR( "GetUtcMinute"          " I 1 1 3,"     ),
    "CPD57"   , A_CHAR( "GetUtcMilliSecond"     " I 1 1 1 1"    ) EOS
#endif

            // end of BootstrapBulk()
            nullptr);
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS

    }

    else if( phase == BootstrapPhases::PrepareConfig )
    {
        EnumRecords<Exceptions                              >::Bootstrap();
        EnumRecords<DefaultUnaryOperators                   >::Bootstrap( *this, "UO"  );
        EnumRecords<DefaultBinaryOperators                  >::Bootstrap( *this, "BO"  );
        EnumRecords<DefaultAlphabeticUnaryOperatorAliases   >::Bootstrap( *this, "UOA" );
        EnumRecords<DefaultAlphabeticBinaryOperatorAliases  >::Bootstrap( *this, "BOA" );
        #if ALIB_DEBUG
        EnumRecords<detail::VirtualMachine::Command::OpCodes>::Bootstrap( *this, "VM_CMD_OPCODES" );
        #endif
    }
}



// #################################################################################################
// Implementation of parsing methods of built-in record types.
// #################################################################################################
void ERBinaryOperator::Parse()
{
    enums::EnumRecordParser::Get( Symbol      );
    enums::EnumRecordParser::Get( Precedence  , true );
}

void EROperatorAlias::Parse()
{
    enums::EnumRecordParser::Get( Symbol      );
    enums::EnumRecordParser::Get( Replacement , true );
}


}}} // namespace [aworx::lib::expressions]


#undef EOS
