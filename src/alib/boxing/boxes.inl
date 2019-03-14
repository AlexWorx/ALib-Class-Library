// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#if !defined(HPP_ALIB_BOXING_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif

namespace aworx { namespace lib { namespace boxing {


/** ************************************************************************************************
 * A vector of objects of type \alib{boxing,Box}.
 * Specializes class \c std::vector<Box> (publically) with a constructor and methods to add a
 * variable amount of arbitrary values with one invocation.
 *
 * \see
 *   Chapter \ref alib_boxing_boxes "11. Class Boxes and Variadic Function Arguments"
 *   of the Programmer's Manual of module \alibmod_boxing.
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
         * Empty method. Needed to allow adding empty variadic template parameter packs.
         *****************************************************************************************/
        inline void Add()
        {}

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

        #if ALIB_DOCUMENTATION_PARSER
        /** ****************************************************************************************
         * Template method that boxes and adds one value to this vector of boxes.
         *
         * If the given value is an instance of this class, then, instead of "boxing" the list,
         * the values of that list are each added. The latter also works if the given value
         * is a boxed instance of this class.<br>
         * Furthermore, boxes and boxed arrays of boxes are "flattened" in the same fashion.
         *
         *
         * @param src The value to be boxed and added.
         ******************************************************************************************/
        template <typename TBoxable>
        inline void Add( TBoxable&& src );
        #else

        // adding a box: flatten, if the box contains Boxes or an array of boxes
        template <typename TBoxable>
        inline
        ATMP_VOID_IF( ATMP_EQ( Box, ATMP_RCVR(TBoxable)) )
        Add( TBoxable&& box )
        {
            if( box.template IsType<Boxes*>() )
            {
                auto* boxes= box.template Unbox<Boxes*>();
                reserve( size() + boxes->size() );
                for( Boxes::const_iterator it= boxes->begin() ; it != boxes->end() ; ++it )
                    push_back( *it );
                return;
            }

            if( box.template IsArrayOf<Box>() )
            {
                for( integer i= 0; i < box.UnboxLength() ; ++i )
                    push_back( box.template UnboxElement<Box>(i) );
                return;
            }

            // add a single object
            emplace_back( std::forward<TBoxable>(box) );
        }

        // adding a Boxes object: flatten
        template <typename TBoxable>
        inline
        ATMP_VOID_IF( ATMP_EQ( Boxes, ATMP_RCVR(TBoxable)) )
        Add( TBoxable&& boxes )
        {
            reserve( size() + boxes.size() );
            for( auto it= boxes.begin() ; it != boxes.end() ; ++it )
                push_back( *it );
        }

        // adding an array of boxes: flatten
        template <typename TBoxable>
        inline
        ATMP_VOID_IF(    ATMP_IS_ARR(ATMP_RCVR(TBoxable))
                      && ATMP_EQ( Box, ATMP_RECVP(TBoxable)) )
        Add( TBoxable&& boxArray )
        {
            constexpr size_t extent= std::extent<ATMP_RCVR(TBoxable)>::value;
            reserve( size() + extent );
            for( size_t i= 0; i < extent; ++i )
                push_back( boxArray[i] );
        }

        // ...any other type
        template <typename TBoxable>
        inline
        ATMP_VOID_IF(    !ATMP_EQ( Box   , ATMP_RCVR(TBoxable))
                      && !ATMP_EQ( Boxes , ATMP_RCVR(TBoxable))
                      && !(   ATMP_IS_ARR(ATMP_RCVR(TBoxable))
                           && ATMP_EQ( Box, ATMP_RECVP(TBoxable)) )
                     )
        Add( TBoxable&& src )
        {
            emplace_back( std::forward<TBoxable>(src) );
        }
        #endif // ALIB_DOCUMENTATION_PARSER


        /** ****************************************************************************************
         * Inline operator that simply aliases method #Add.
         *
         * @param src The value to be boxed and added.
         * @return Returns a mutable reference to \c this.
         *****************************************************************************************/
        template <typename TBoxable>
        inline Boxes& operator+=( TBoxable&& src )
        {
            Add( src );
            return *this;
        }

        /** ****************************************************************************************
         * Inline operator that simply aliases method #Add.
         *
         * @param src The value to be boxed and added.
         * @return Returns a mutable reference to \c this.
         *****************************************************************************************/
        template <typename TBoxable>
        inline Boxes& operator<<( TBoxable&& src )
        {
            Add( src );
            return *this;
        }

        /** ****************************************************************************************
         * Invokes \alib{boxing,Box::Call} with each box in this list.
         * The result of the invocations of the box-functions is ignored.
         *
         * @tparam TFDecl  The function type to call.
         *                 Needs to be specified with invocations of this method.
         * @tparam TArgs   Types of the variadic arguments \p{args}.
         *                 Do not need to be specified.
         * @param  args    Variadic arguments forwarded to the functions.
         ******************************************************************************************/
        template <typename TFDecl,  typename... TArgs>
        void CallAll(TArgs&&... args)                                                          const
        {
            for( auto& box : *this )
                box.Call<TFDecl>( std::forward<TArgs>(args)... );
        }

        /** ****************************************************************************************
         * Non-constant version of method #CallAll, which likewise chooses the non-constant version
         * of \alib{boxing,Box::Call} and hence this method is usable with functions that only
         * accept mutable (aka not constant) boxes.<br>
         * Technically, the only difference between this method and \b CallAll is that the latter
         * is declared \c const.
         *
         * @tparam TFDecl  The function type to call.
         *                 Needs to be specified with invocations of this method.
         * @tparam TArgs   Types of the variadic arguments \p{args} .
         * @param  args    Variadic arguments forwarded to the functions.
         ******************************************************************************************/
        template <typename TFDecl,  typename... TArgs>
        void CallAll(TArgs&&... args)
        {
            for( auto& box : *this )
                box.Call<TFDecl>( std::forward<TArgs>(args)... );
        }

    #if ALIB_MODULE_MEMORY
        /** ****************************************************************************************
         * Same as \ref CallAll "CallAll<FClone>", but uses method \alib{boxing,Box::Clone},
         * which internally invokes \alib{boxing,FClone}.
         *
         * Using this version leads to shorter code, because method \b Box::Clone is not inlined.
         *
         * #### Availability ####
         * This method is available only if module \alibmod_memory is included in the \alibdist.
         *
         * @param memory  A block allocator used for storing cloned data.
         ******************************************************************************************/
        ALIB_API
        void CloneAll(memory::MemoryBlocks& memory);
    #endif

}; // class Boxes

}}} // namespace [aworx::lib::boxing]


