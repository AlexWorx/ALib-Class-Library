// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_RESOURCES_TUPLE_LOADER
#define HPP_ALIB_RESOURCES_TUPLE_LOADER 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_MODULE(RESOURCES)

#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#   include "alib/strings/util/tokenizer.hpp"
#endif

#if ALIB_MODULE_RESULTS
#   if !defined(HPP_ALIB_RESULTS_REPORT)
#      include "alib/results/report.hpp"
#   endif
#else
#   if !defined(HPP_ALIB_LIB_ASSERT)
#      include "alib/lib/assert.hpp"
#   endif
#endif


namespace aworx { namespace lib { namespace resources {


/** ************************************************************************************************
 * Static helper class which loads vectors of arbitrary \c std::tuples (with supported element
 * types) from \alib{resources,Resources}.
 *
 * This class and static method #LoadTable is used by implementations of
 * \alib{resources,EnumMetaData::CheckLoad} to load
 * \alib{resources,T_EnumMetaDataDecl,enum meta data} from resource strings.
 **************************************************************************************************/
class ResourcedTupleLoader
{
    // #############################################################################################
    // internal methods
    // #############################################################################################
    protected:
        /**
         * Reads an \c int value.
         * @param[out] e    The target reference object
         * @param      src  A sub-string to consume the target value from.
         */
        static void readElement( character&  e, Substring& src )
        {
            e= src.ConsumeChar();
        }

        /**
         * Reads an \c integer value.
         * @param[out] e    The target reference object
         * @param      src  A sub-string to consume the target value from.
         */
        template<typename TInteger,
                 typename TEnableIf= typename std::enable_if<std::is_integral<TInteger>::value>::type>
        static void readElement( TInteger& e, Substring& src )
        {
            src.ConsumeInt( e );
            #if ALIB_MODULE_RESULTS
                ALIB_ASSERT_ERROR(  src.IsEmpty(),
                                    "Error in tuple resource data. Integer expected.\\n"
                                    "  Rest of Resource string: {!Q}", src )
            #else
                #if ALIB_DEBUG
                    if( src.IsNotEmpty() )
                    {
                        ALIB_ERROR( "Error in tuple resource data. Integer expected.\n"
                                    "  Rest of Resource string: \"", NString1K( src ),
                                    "\""  )
                    }
                #endif
            #endif

        }

        /**
         * Reads an \c String value.
         * @param[out] e    The target reference object
         * @param      src  A sub-string to consume the target value from.
         */
        static void readElement( String& e, Substring& src )
        {
            e= src;
        }

        /**
         * Does not read (do) anything. This is provided to skip all other types.
         */
        template<typename T,
                 typename TEnableIf= typename std::enable_if<!std::is_integral<T>::value>::type>

        static void readElement( T    , Substring& )
        {
        }



        /**
         * Empty TMP function that recursively compiles trough its specialization.
         * @tparam I        The actual (compile-time) tuple element.
         * @tparam TTuple   The tuple to read.
         * @return Ignored.
         */
        template<std::size_t    I       = 0,
                 typename       TTuple       >
        inline static
        typename std::enable_if<I == std::tuple_size<TTuple>::value>::type
        readEachElement( TTuple&, Tokenizer& )
        {}

        /**
         * Specialization of TMP function that recursively compiles. In each compilation recursion
         * step, one of the overloaded  #readElement functions is inserted into the code.
         *
         * @param  tuple    The tuple to read.
         * @param  tknzr    The tokenizer to read the elements from.
         * @tparam I        The actual (compile-time) tuple element.
         * @tparam TTuple   The tuple to read.
         * @return Ignored.
         */
        template<std::size_t    I       = 0,
                 typename       TTuple       >
        inline
        typename std::enable_if<I != std::tuple_size<TTuple>::value>::type
        static readEachElement( TTuple& tuple, Tokenizer& tknzr)
        {
            readElement( std::get<I>(tuple), tknzr.Next(Whitespaces::Keep) );
            readEachElement<I + 1, TTuple>(tuple, tknzr);  // compiler recursion
        }


    // #############################################################################################
    // Interface.
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Loads a vector of \c std::tuple objects from the resources.
         * The entries are either all read from resource variable named \p{name}, or, if this is
         * not found, this methods searches for all resource variables named \p{nameN} with \p{N}
         * starting at \c 0 and increasing until resource string \p{nameN} is not found.
         *
         * The types of elements supported are:
         * - \c aworx::String:<br>
         *   Read 'as is' from the resource string.
         * - \c char:<br>
         *   Read 'as is' from the resource string.
         * - Integer types:<br>
         *   Read using singleton
         *   \alib{strings,TNumberFormat::Computational,NumberFormat::Computational} which
         *   supports decimal, binary, octal and hexadecimal formats.
         *
         * \note
         *   While all other types are simply ignored and default-constructed, still a separation
         *   character has to exist in the resource string.
         *
         * @param  tuples       The vector of tuples to fill
         * @param  resources    Reference to the resource object used.
         * @param  category     The resource category.
         * @param  name         The name of the resource string in \p{resources}.
         * @param  delim        The delimiter of the values.
         *                      Defaults to \b ','.
         * @tparam TTuple       The tuple type. Deducted by the compiler from parameter \p{tuples}.
         ******************************************************************************************/
        template<typename TTuple>
        static void LoadTable( std::vector<TTuple>&    tuples,
                               Resources&              resources,
                               const NString&          category,
                               const NString&          name,
                               nchar                   delim= ','     )
        {
            // get category table and search entry
            String tupleRes=   resources.Get( category, name  ALIB_DBG(, false) );
            if( tupleRes.IsNotNull() )
            {
                Tokenizer tknzr( tupleRes, static_cast<character>(delim) );
                while( tknzr.HasNext() )
                {
                    TTuple tuple;
                    readEachElement( tuple, tknzr );
                    tuples.emplace_back( tuple );
                }
            }
            // entry not found? Try to load numbered entries.
            else
            {
                NString64 nameNumbered(name);
                int nr= 0;
                for(;;)
                {
                    tupleRes=  resources.Get( category, nameNumbered._(nr)  ALIB_DBG(, false) );
                    if( tupleRes.IsNull() )
                        break;
                    Tokenizer tknzr( tupleRes, static_cast<character>(delim) );
                    TTuple tuple;
                    readEachElement( tuple, tknzr );
                    tuples.emplace_back( tuple );
                    nameNumbered.ShortenTo( name.Length() );
                    nr++;
                }

            }
            #if ALIB_MODULE_RESULTS
                ALIB_ASSERT_ERROR( tuples.size() > 0,
                                   "No tuple resource \""  , name,
                                   "\" found in category: ", category )
            #else
                #if ALIB_DEBUG
                    if( tuples.size() == 0 )
                    {
                        ALIB_ERROR( "No tuple resource \""  , NString64( name     ),
                                    "\" found in category: ", NString64( category )  )
                    }
                #endif
            #endif
        }



}; // class ResourcedTupleLoader

}}} // namespace [aworx::lib::resources]

#endif // HPP_ALIB_RESOURCES_TUPLE_LOADER
