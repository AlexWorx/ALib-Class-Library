// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen


#if !defined (HPP_ALIB_BOXING_LIB)
    #include "alib/boxing/boxing.hpp"
#endif

#include <vector>

//  set include guard
#ifndef HPP_ALIB_BOXING_BOXES
//! @cond NO_DOX
#define HPP_ALIB_BOXING_BOXES 1
//! @endcond


namespace aworx { namespace lib { namespace boxing
{


/** ************************************************************************************************
 * A vector of objects of class \ref aworx::lib::boxing::Box "Box".
 * Specializes class \c std::vector<Box> (publically) with a constructor and methods to add a
 * variable amount of arbitrary values with one invocation.
 *
 * \see Chapter
 *      \ref alib_namespace_boxing_variadic "7. Variadic Arguments"
 *      of the namespace documentation.
 **************************************************************************************************/
class Boxes : public std::vector<Box>
{
    public:
        /** ****************************************************************************************
         * Default constructor.
         *****************************************************************************************/
        Boxes() {} // "= default" does not seem to work with (current) MSVC, when Boxes is
                   // a field of a composed class;

        /** ****************************************************************************************
         * Constructor accepting variadic template arguments.
         * Forwards arguments to method #Add.
         *
         * @param boxables The list of values to be boxed and added.
         *****************************************************************************************/
        template <typename... TBoxables>
        inline Boxes( TBoxables&&... boxables )
        {
            Add( std::forward<TBoxables>(boxables)...);
        }

        /** ****************************************************************************************
         * Overloaded method 'extracting' the first argument and recursively calls itself for
         * the rest.
         *
         * @param first  The first argument.
         * @param rest   The remaining arguments.
         *****************************************************************************************/
        template <typename TFirstBoxable, typename... TBoxables>
        inline void Add( TFirstBoxable&& first, TBoxables&&... rest )
        {
            Add( std::forward<TFirstBoxable>(first)      );
            Add( std::forward<TBoxables>    (rest )...);
        }


        /** ****************************************************************************************
         * Template method that boxes and adds one value to this vector of boxes.
         *
         * If the given value is an instance of this class, then, instead of "boxing" the list,
         * the values of that list are each added. The latter also works if the given value
         * is a boxed instance of this class.
         *
         * @param src The value to be boxed and added.
         *****************************************************************************************/
        template <typename TBoxable>
        inline void Add( TBoxable&& src )
        {
            ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING

            // add another list of Boxes
            using TPlain= typename std::remove_const    <
                          typename std::remove_pointer  <
                          typename std::remove_reference<
                          typename std::remove_const    < TBoxable >::type>::type>::type>::type;

            //ALIB_TMP_SHOW_TYPE_IN_DEBUGGER(TPlain);

            if( std::is_same< Boxes, TPlain >::value )
            {
                Boxes* boxes=    std::is_pointer<TBoxable>::value  ?  * (Boxes **) &src
                                                                   :    (Boxes * ) &src;
                reserve( size() + boxes->size() );
                for( Boxes::iterator it= boxes->begin() ; it != boxes->end() ; ++it )
                    push_back( *it );
                return;
            }
            if( std::is_same< Box, TPlain >::value )
            {
                Box* box=    std::is_pointer<TBoxable>::value  ?  * (Box **) &src
                                                               :    (Box * ) &src;
                if( box->IsType<Boxes*>() )
                {
                    Boxes* boxes= box->Unbox<Boxes*>();
                    reserve( size() + boxes->size() );
                    for( Boxes::iterator it= boxes->begin() ; it != boxes->end() ; ++it )
                        push_back( *it );
                    return;
                }
            }

            // add a single object
            emplace_back( std::forward<TBoxable>(src) );

            ALIB_WARNINGS_RESTORE
        }

}; // class Boxes



}} // namespace lib::boxing

/// Type alias in namespace #aworx.
using     Boxes=         aworx::lib::boxing::Boxes;


} // namespace aworx

#endif // HPP_ALIB_BOXING_BOXES
