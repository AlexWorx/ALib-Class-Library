// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(HPP_ALIB_EXPRESSIONS_DETAIL_SPIRIT)
#   include "alib/expressions/detail/spirit.hpp"
#endif

#if ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER

#if ALIB_MODULE_THREADS && !defined(HPP_ALIB_THREADS_THREADLOCKNR)
#   include "alib/threads/threadlocknr.hpp"
#endif


//! @cond NO_DOX

#if !defined (HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM)
#   include "alib/compatibility/std_strings_iostream.hpp"
#endif
#if !defined(HPP_ALIB_EXPRESSIONS_DETAIL_PARSER)
#   include "alib/expressions/detail/parser.hpp"
#endif
#if !defined(HPP_ALIB_EXPRESSIONS_DETAIL_AST)
#   include "alib/expressions/detail/ast.hpp"
#endif
#if !defined(BOOST_SPIRIT_INCLUDE_QI)
#   include <boost/spirit/include/qi.hpp>
#endif
#if !defined(BOOST_SPIRIT_INCLUDE_PHOENIX)
#   include <boost/spirit/include/phoenix.hpp>
#endif

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#pragma clang diagnostic ignored "-Wunused-member-function"
#endif

namespace qi = boost::spirit::qi;

using namespace aworx::lib::expressions::detail;


namespace aworx { namespace lib { namespace expressions { namespace detail { namespace  {
// #################################################################################################
// BSAST
// #################################################################################################
//namespace  {

// BSAST node types
struct BSASTIdentifier;
struct BSASTUnaryOp;
struct BSASTBinaryOp;
struct BSASTConditional;
struct BSASTFunction;

/**
 * Variant of abstract syntax tree types used by boost::spirit for parsing \alib expression strings.
 */
using BSAST=  boost::variant
<
                              integer           ,
                              double            ,
                              AString           ,
                              BSASTIdentifier   ,
    boost::recursive_wrapper< BSASTUnaryOp     >,
    boost::recursive_wrapper< BSASTBinaryOp    >,
    boost::recursive_wrapper< BSASTConditional   >,
    boost::recursive_wrapper< BSASTFunction    >
>;

/**
 * Abstract syntax tree node representing identifiers.
 */
struct BSASTIdentifier
{
    AString     Name;   ///< The name of the identifier as parsed from the expression string.
};

/**
 * Abstract syntax tree node representing a function call.
 */
struct BSASTFunction
{
    AString                              Name;      ///< Enum element denoting the operator.
    boost::optional<std::vector<BSAST>>  Arguments; ///< The argument node.

    /**
     * Constructor providing all fields.
     * @param identifier  The identifier ast. We just take its name
     * @param arguments   The argument of the operation.
     */
    explicit
    BSASTFunction(const BSASTIdentifier identifier, const boost::optional<std::vector<BSAST>>& arguments )
    : Name(identifier.Name), Arguments(arguments)
    {}
};


/**
 * Abstract syntax tree node representing unary operations.
 */
struct BSASTUnaryOp
{
    DefaultUnaryOperators    Operator; ///< Enum element denoting the operator.
    BSAST         Argument; ///< The argument node.

    /**
     * Constructor providing all fields.
     * @param argument The argument of the operation.
     * @param op       The operator
     */
    explicit
    BSASTUnaryOp(const BSAST& argument, DefaultUnaryOperators op)
    : Operator(op), Argument(argument)
    {}
};

/**
 * Abstract syntax tree node representing binary operations.
 */
struct BSASTBinaryOp
{
    DefaultBinaryOperators   Operator;  ///< Enum element denoting the operator.
    BSAST         Lhs;       ///< The left-hand-side expression node.
    BSAST         Rhs;       ///< The right-hand-side expression node.
    /**
     * Constructor providing all fields.
     * @param lhs Left-hand side node.
     * @param rhs Right-hand side node.
     * @param op  The operator
     */
    explicit
    BSASTBinaryOp(const BSAST& lhs, const BSAST& rhs, DefaultBinaryOperators op )
    : Operator(op), Lhs(lhs), Rhs(rhs)
    {}
};

/**
 * Abstract syntax tree node representing binary operations.
 */
struct BSASTConditional
{
    BSAST         Q;   ///< The question.
    BSAST         T;   ///< The true-result.
    BSAST         F;   ///< The false-result.

    /**
     * Constructor providing all fields.
     * @param q The question.
     * @param t The true-result.
     * @param f The false-result.
     */
    explicit
    BSASTConditional(const BSAST& q, const BSAST& t, const BSAST& f )
    : Q(q), T(t), F(f)
    {}
};


// We need to tell fusion about our structs to make them a first-class fusion citizen
}}}}}
BOOST_FUSION_ADAPT_STRUCT(
    BSASTIdentifier,
    ( aworx::AString, Name )
)
namespace aworx { namespace lib { namespace expressions { namespace detail { namespace  {

// #################################################################################################
// This "Fold" stuff ist taken (and not completely understood 8-) from
//     https://stackoverflow.com/questions/39820298/boostspirit-optimizing-an-expression-parser
// Thank you 'jv_"!
// #################################################################################################

struct Fold
{
    BOOST_SPIRIT_IS_TAG()
};

template <typename TExposed, typename TExpression>
boost::spirit::stateful_tag_type<TExpression, Fold, TExposed>
fold(TExpression const& expr)
{
    return boost::spirit::stateful_tag_type<TExpression, Fold, TExposed>(expr);
}

}}}}} // namespace [aworx::lib::expressions::detail::anonymous]


//specialize boost::spirit::use_directive
namespace boost { namespace spirit {

    template <typename TExpression, typename TExposed>
    struct use_directive<qi::domain, tag::stateful_tag<TExpression, Fold, TExposed> >
      : mpl::true_
    {};
}} //namespace [boost::spirit]

namespace aworx { namespace lib { namespace expressions { namespace detail { namespace {

template <typename Exposed, typename InitialParser, typename RepeatingParser>
struct FoldDirective
{
    InitialParser    initialParser;
    RepeatingParser  repeatingParser;

    FoldDirective(InitialParser const& initial, RepeatingParser const& repeating)
    :initialParser(initial)
    ,repeatingParser(repeating)
    {}

    template <typename Context, typename Iterator>
    struct attribute
    {
        //This works in this case but is not generic
        using type= typename boost::spirit::traits::attribute_of<InitialParser,Context,Iterator>::type ;
    };

    template <typename Iterator, typename Context , typename Skipper, typename Attribute>
    bool parse(Iterator& first, Iterator const& last, Context& context, Skipper const& skipper, Attribute& attr_) const
    {
        Iterator start = first;

        typename boost::spirit::traits::attribute_of<InitialParser,Context,Iterator>::type attrInitial;


        if (!initialParser.parse(first, last, context, skipper, attrInitial))
        {
            first=start;
            return false;
        }

        typename boost::spirit::traits::attribute_of<RepeatingParser,Context,Iterator>::type attrRepeating;

        if(!repeatingParser.parse(first, last, context, skipper, attrRepeating))
        {
            boost::spirit::traits::assign_to(attrInitial, attr_);
            return true;
        }

        Exposed attrCurrent(attrInitial,
                            boost::phoenix::at_c<1>(attrRepeating)(),
                            boost::phoenix::at_c<0>(attrRepeating)()   );

        while(repeatingParser.parse(first, last, context, skipper, attrRepeating))
        {
            boost::spirit::traits::assign_to( Exposed(attrCurrent,
                                                      boost::phoenix::at_c<1>(attrRepeating)(),
                                                      boost::phoenix::at_c<0>(attrRepeating)() ),
                                              attrCurrent );
        }
        boost::spirit::traits::assign_to(attrCurrent,attr_);
        return true;
    }

    template <typename Context>
    boost::spirit::info what(Context& ) const
    {
        return boost::spirit::info("fold");
    }
};

}}}}} // namespace [aworx::lib::expressions::detail::anonymous]


//specialize boost::spirit::qi::make_directive
namespace boost { namespace spirit { namespace qi {

template <typename TExpression, typename TExposed, typename TSubject, typename TModifiers>
struct make_directive<tag::stateful_tag<TExpression, Fold, TExposed>, TSubject, TModifiers>
{
    using result_type= FoldDirective<TExposed, TExpression, TSubject>;

    template <typename Terminal>
    result_type operator()(Terminal const& term, TSubject const& subject, TModifiers const&) const
    {
        using tag_type = tag::stateful_tag<TExpression, Fold, TExposed> ;
        using spirit::detail::get_stateful_data;

        return result_type(get_stateful_data<tag_type>::call(term), subject);
    }
};

}}} //namespace [boost::spirit::qi]



namespace aworx { namespace lib { namespace expressions { namespace detail { namespace  {

// #################################################################################################
// The ALib expression parser
// #################################################################################################
struct ALibExpressionParserBoostSpirit : qi::grammar<aworx::String::ConstIterator, BSAST(), qi::space_type>
{
    private:
    using TIter= aworx::String::ConstIterator;

    qi::rule<TIter, integer               > literalInt;
    qi::rule<TIter, double                > literalFloat;
    qi::rule<TIter, AString()             > literalString;
    qi::rule<TIter, BSASTIdentifier()     > identifier;

    qi::rule<TIter, BSAST() , qi::space_type> simple,start,

                                            // operators groups in ascending precedence order
                                            conditional,
                                            assign,
                                            boolOr , boolAnd, bitOr, bitXor, bitAnd,

                                            eqNeq, comp, shift,
                                            addSub, mulDivMod,

                                            unPre,

                                            // others
                                            function, subscript;

    qi::rule<TIter, std::vector<BSAST>() , qi::space_type> paramList;


    qi::symbols<char, DefaultBinaryOperators>  assignOp,
                                  boolOrOp,          boolAndOp,
                                  bitOrOp, bitXorOp, bitAndOp,

                                  addSubOp,
                                  mulDivModOp,

                                  eqNeqOp,
                                  compOp,
                                  shiftOp;

    qi::symbols<char, DefaultUnaryOperators>   unPreOp;

    public:

    ALibExpressionParserBoostSpirit( bool  aliasEqualsOperatorWithAssignOperator,
                                     bool  supportArraySubscriptOperator          )
    : ALibExpressionParserBoostSpirit::base_type(start)
    {
        using namespace qi;
        namespace phx= boost::phoenix;

        // operator symbols
        unPreOp       .add( "~" ,  DefaultUnaryOperators::BitNot            )
                          ( "!" ,  DefaultUnaryOperators::BoolNot           )
                          ( "+" ,  DefaultUnaryOperators::Positive          )
                          ( "-" ,  DefaultUnaryOperators::Negative          )
                          ( "*" ,  DefaultUnaryOperators::Indirection       );


        boolOrOp      .add( "||",  DefaultBinaryOperators::BoolOr           );
        boolAndOp     .add( "&&",  DefaultBinaryOperators::BoolAnd          );
        bitOrOp       .add( "|" ,  DefaultBinaryOperators::BitOr            );
        bitXorOp      .add( "^" ,  DefaultBinaryOperators::BitXOr           );
        bitAndOp      .add( "&" ,  DefaultBinaryOperators::BitAnd           );

        addSubOp      .add( "+" ,  DefaultBinaryOperators::Add              )
                          ( "-" ,  DefaultBinaryOperators::Subtract         );

        mulDivModOp   .add( "*" ,  DefaultBinaryOperators::Multiply         )
                          ( "/" ,  DefaultBinaryOperators::Divide           )
                          ( "%" ,  DefaultBinaryOperators::Modulo           );

        eqNeqOp       .add( "==",  DefaultBinaryOperators::Equal            )
                          ( "!=",  DefaultBinaryOperators::NotEqual         );

        if( aliasEqualsOperatorWithAssignOperator )
            eqNeqOp   .add( "=",   DefaultBinaryOperators::Assign           );
        else
            assignOp  .add( "=" ,  DefaultBinaryOperators::Assign           );


        compOp        .add( "<" ,  DefaultBinaryOperators::Smaller          )
                          ( "<=",  DefaultBinaryOperators::SmallerOrEqual   )
                          ( ">" ,  DefaultBinaryOperators::Greater          )
                          ( ">=",  DefaultBinaryOperators::GreaterOrEqual   );

        shiftOp.add       ( "<<",  DefaultBinaryOperators::ShiftLeft        )
                          ( ">>",  DefaultBinaryOperators::ShiftRight       );




        //#### operators in ascending precedence ###
        start  =  conditional.alias();   // start

        // conditional ? :
        conditional =   ( assign  >> char_('?') >> conditional >> char_(':')  >> conditional   )
                        [_val =boost::phoenix::construct<BSASTConditional>(_1,_3,_5)]

                      | ( assign  >> char_('?')                >> char_(':')  >> conditional   )
                        [_val =boost::phoenix::construct<BSASTConditional>(_1, boost::phoenix::construct<BSASTIdentifier>() ,_4)]

                      | ( assign                                                               )
                        [_val =_1]
                      ;

        // binary operators (starting with assign)
        #define BINOP(OP,NEXT)   OP= fold<BSASTBinaryOp>(NEXT.alias()) [OP##Op >> NEXT ];

            BINOP(assign    , boolOr  )
            BINOP(boolOr    , boolAnd  )
            BINOP(boolAnd   , bitOr    )
            BINOP(bitOr     , bitXor   )
            BINOP(bitXor    , bitAnd   )

            BINOP(bitAnd    , eqNeq    )

            BINOP(eqNeq     , comp     )
            BINOP(comp      , shift    )

            BINOP(shift     , addSub   )

            BINOP(addSub    , mulDivMod)
            BINOP(mulDivMod , unPre    )

        #undef BINOP

        // prefixed unary operators
if( supportArraySubscriptOperator )
{
        unPre  =   subscript                            [_val = _1]
                | ( unPreOp  >> unPre    )              [_val = boost::phoenix::construct<BSASTUnaryOp>(_2,_1)];
}
else
{
        unPre  =   simple                               [_val = _1]
                | ( unPreOp  >> unPre    )              [_val = boost::phoenix::construct<BSASTUnaryOp>(_2,_1)];
}

        // array subscript []
        subscript=    ( simple >> ('[' > start > ']') ) [_val = boost::phoenix::construct<BSASTBinaryOp>(_1,_2,DefaultBinaryOperators::Subscript)]
                   |  simple                            [_val = _1];

        // recursion
        simple =     function
                   | literalString
                   | literalFloat
                   | literalInt
                   | ( '(' > start > ')')   ;

        // terminals
        qi::real_parser<double, qi::strict_real_policies<double> > strict_double;
        literalInt    = ulong_long;
        literalFloat  = strict_double;
        literalString = lexeme['"' >> *(ascii::char_ - '"') >> '"']                    [ _val << _1  ];

        // functions
        identifier    = lexeme[ascii::char_("a-zA-Z_") >> *ascii::char_("a-zA-Z_0-9")] [ phx::at_c<0>(_val) << phx::at_c<0>(_1) << phx::at_c<1>(_1)  ];
        paramList=   start >> *(',' >> start);

        function =   ( identifier >>( '(' > -paramList > ')')) [_val = boost::phoenix::construct<BSASTFunction>(_1,_2)]
                   | identifier                                [_val = _1];


        BOOST_SPIRIT_DEBUG_NODES(
            (start)(simple)
            (conditional)
            (boolOr)(boolAnd)            (boolOrOp)(boolAndOp)
            (bitOr)(bitXor)(bitAnd)      (bitOrOp)(bitXorOp)(bitAndOp)
            (addSub)(mulDivMod)          (addSubOp)(mulDivModOp)
            (eqNeq)(comp)                (eqNeqOp)(compOp)
            (shift)                      (shiftOp)
            (unPre)                      (unPreOp)
        );
    }
};

// #################################################################################################
// BSASTTranslator
// #################################################################################################
struct BSASTTranslator : boost::static_visitor<AST*>
{

    EnumMetaData<expressions::DefaultUnaryOperators>*    metaInfoUnaryOps;
    EnumMetaData<expressions::DefaultBinaryOperators>*   metaInfoBinaryOps;

    BSASTTranslator()
    {
       metaInfoUnaryOps = &EnumMetaData<expressions::DefaultUnaryOperators >::GetSingleton();
       metaInfoBinaryOps= &EnumMetaData<expressions::DefaultBinaryOperators>::GetSingleton();
    }

    AST* operator() (const integer           literal ) { return new ASTLiteral   ( literal   ,0 );  }
    AST* operator() (const double            literal ) { return new ASTLiteral   ( literal   ,0 );  }
    AST* operator() (const AString&          literal ) { return new ASTLiteral   ( literal   ,0 );  }
    AST* operator() (const BSASTIdentifier&  node    ) { return new ASTIdentifier( node.Name ,0 );  }

    AST* operator() (const BSASTFunction&  node )
    {
        auto function= new ASTFunction( node.Name ,0);
        if( node.Arguments )
            for( auto& param: *node.Arguments )
                function->Arguments.emplace_back( boost::apply_visitor(*this, param ) );
        return function;
    }

    AST* operator() (const BSASTUnaryOp&       node )
    {
        return new ASTUnaryOp( std::get<1>(*metaInfoUnaryOps->Get(node.Operator) ),
                               boost::apply_visitor(*this, node.Argument ) ,0 );
    }

    AST* operator() ( const BSASTBinaryOp&      node )
    {
        return new ASTBinaryOp( std::get<1>(*metaInfoBinaryOps->Get( node.Operator ) ),
                                boost::apply_visitor(*this, node.Lhs),
                                boost::apply_visitor(*this, node.Rhs)   ,0);
    }

    AST* operator() (const BSASTConditional&  node )
    {
        // Check if is "Elvis operator" (A ?: B, hence the true-result is missing).
        // In this case, our parser adds an ASTIdentifier without a name as node.T.
        // (Unnamed identifiers are otherwise not parsable an thus is used to recognize elvis here!)
        ALIB_ASSERT_ERROR(  static_cast<BSAST>(BSASTIdentifier()).which() == 3,
                             "The index BSTAST changed! Correct this here."     )
        if(    node.T.which() == 3
            && boost::get<BSASTIdentifier>( node.T ).Name.IsEmpty() )
        {
            return new ASTBinaryOp(  std::get<1>(*metaInfoBinaryOps->Get( DefaultBinaryOperators::Elvis )),
                                     boost::apply_visitor(*this, node.Q),
                                     boost::apply_visitor(*this, node.F) ,0  );
        }
        return new ASTConditional( boost::apply_visitor(*this, node.Q),
                                   boost::apply_visitor(*this, node.T),
                                   boost::apply_visitor(*this, node.F)  ,0,0);
    }

}; // struct BSASTTranslator

struct ParserBoostSpirit : public Parser
{
    ALibExpressionParserBoostSpirit spiritParser;
#if ALIB_MODULE_THREADS
    ThreadLockNR         parserLock;
#endif

    ParserBoostSpirit( bool increaseAssignOpPrecedence,
                       bool supportArraySubscriptOperator )
    : spiritParser(increaseAssignOpPrecedence, supportArraySubscriptOperator )
    {}

    virtual ~ParserBoostSpirit()                                                            override
    {}


    virtual AST* Parse( const String& expressionString, NumberFormat*  )                    override
    {
        ALIB_LOCK_WITH( parserLock )
        String::ConstIterator   act=         expressionString.begin();
        String::ConstIterator   end=         expressionString.end();

        BSAST* bsast=       new BSAST();
        try
        {
            if(    !qi::phrase_parse(act,end, spiritParser, qi::space, *bsast)   ||  act != end )
            {
                delete bsast;
                Exception   e( ALIB_CALLER_NULLED, Exceptions::SyntaxError );
                e.Add        ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                               expressionString, act - expressionString.begin() + 1  );
                throw e;
            }
        }
        catch (const qi::expectation_failure<decltype(act)>& )
        {
            delete bsast;
            Exception e  ( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation,
                           EXPRESSIONS.GetResource("ExcExp0")      );
            e.Add        ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                           expressionString, act - expressionString.begin() + 1    );
            throw e;
        }
        catch (std::runtime_error& e )
        {
            ALIB_ERROR( "Unknown spirit exception thrown" )
            throw e;
        }

        BSASTTranslator visitor;
        return  boost::apply_visitor( visitor, *bsast );
    }
};

}; // anonymous namespace

// creation function
Parser* CreateBoostSpiritParser( bool increaseAssignOpPrecedence,
                                 bool supportArraySubscriptOperator )
{
    return new ParserBoostSpirit( increaseAssignOpPrecedence, supportArraySubscriptOperator );
}

}}}} // namespace [aworx::lib::expressions::detail]


#endif // ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER
//! @endcond




