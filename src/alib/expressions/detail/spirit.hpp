// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

#ifndef HPP_ALIB_EXPRESSIONS_DETAIL_SPIRIT
#define HPP_ALIB_EXPRESSIONS_DETAIL_SPIRIT


#if ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER


namespace aworx { namespace lib { namespace expressions { namespace detail {

struct Parser;

/**
 * This namespace namespace function creates an (otherwise hidden) parser which uses
 * [boost::spirit](http://www.boost.org/doc/libs/1_66_0/libs/spirit/doc/html/index.html).
 *
 * @param increaseAssignOpPrecedence    If \c true, operator <c>=</c> becomes the same precedence
 *                                      as <c>==</c>.
 * @param supportArraySubscriptOperator If \c true, operator <c>=[]</c> is supported.
 *
 * @return An implementation of class \b %Parser using boost::spirit.
 */
ALIB_API
extern  Parser* CreateBoostSpiritParser( bool increaseAssignOpPrecedence,
                                         bool supportArraySubscriptOperator );


}}}}; // namespace [aworx::lib::expressions::detail]


#endif // ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER
#endif // HPP_ALIB_EXPRESSIONS_DETAIL_SPIRIT
