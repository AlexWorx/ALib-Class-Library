// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_LANG_RESOURCE_TUPLE_LOADER)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_LANG_RESOURCE_TUPLE_LOADER
//! @cond NO_DOX
#define HPP_ALIB_LANG_RESOURCE_TUPLE_LOADER 1
//! @endcond


#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
    #include "alib/strings/util/tokenizer.hpp"
#endif

#if !defined (_GLIBCXX_MAP) && !defined(_MAP_)
    #include <map>
#endif


#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
    #include "alib/strings/substring.hpp"
#endif



namespace aworx { namespace lib { namespace lang {

/** ************************************************************************************************
 * Static helper class which loads vectors of arbitrary \c std::tuples (with supported element types)
 * from \alib{lang,Resources}.
 *
 * This class and static method #LoadTable is used by implementations of
 * \alib{lang,EnumMetaData::CheckLoad} to load
 * \alib{lang,T_EnumMetaDataDecl,enum meta data} from resource strings.
 **************************************************************************************************/
class ResourcedTupleLoader
{
    protected:
    // #############################################################################################
    // internal methods
    // #############################################################################################
        /**
         * Reads an \c int value.
         * @param[out] e       The target reference object
         * @param      src  A substring to consume the target value from.
         */
        static void readElement( character&  e, Substring& src )
        {
            e= src.ConsumeChar();
        }

        /**
         * Reads an \c integer value.
         * @param[out] e       The target reference object
         * @param      src  A substring to consume the target value from.
         */
        template<typename TInteger,
                 typename TEnableIf= typename std::enable_if<std::is_integral<TInteger>::value>::type>
        static void readElement( TInteger&             e, Substring& src )
        {
            src.ConsumeInt( e );
            ALIB_ASSERT_ERROR( src.IsEmpty(),    ASTR("Error in tuple resource data. Integer expected.") )
        }

        /**
         * Reads an \c String value.
         * @param[out] e       The target reference object
         * @param      src  A substring to consume the target value from.
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
         * @tparam I        The actual (compile time) tuple element.
         * @tparam TTuple   The tuple to read.
         * @return Ignored.
         */
        template<std::size_t    I       = 0,
                 typename       TTuple       >
        inline
        typename std::enable_if<I == std::tuple_size<TTuple>::value>::type
        static readEachElement( TTuple&, Tokenizer& )
        {}

        /**
         * Specialization of TMP function that recursively compiles. In each compilation recursion
         * step, one of the overloaded  #readElement functions is inserted into the code.
         *
         * @param  tuple    The tuple to read.
         * @param  tknzr    The tokenizer to read the elements from.
         * @tparam I        The actual (compile time) tuple element.
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
         *
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
         *   Read using singleton \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational} which
         *   supports decimal, binary, octal and hexadecimal formats.
         *
         * \note
         *   While all other types are simply ignored and default-constructed, still a separation
         *   character has to exist in the resource string.
         *
         * @param  tuples       The vector of tuples to fill
         * @param  library      The library object to read from.
         * @param  name         The name of the resource string in \p{resources}.
         * @param  delim        The delimiter of the values.
         *                      Defaults to \b ','.
         * @tparam TTuple       The tuple type. Deducted by the compiler from parameter \p{tuples}.
         ******************************************************************************************/
        template<typename TTuple>
        static void LoadTable( std::vector<TTuple>&    tuples,
                               Library &               library,
                               const String&           name,
                               char                    delim= ','     )
        {
            // get category table and search entry
            String tupleRes= library.GetTry( name );
            if( tupleRes.IsNotNull() )
            {
                Tokenizer tknzr( tupleRes, delim );
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
                String64 nameNumbered(name);
                int nr= 0;
                for(;;)
                {
                    tupleRes=  library.GetTry( nameNumbered._(nr) );
                    if( tupleRes.IsNull() )
                        break;
                    Tokenizer tknzr( tupleRes, delim );
                    TTuple tuple;
                    readEachElement( tuple, tknzr );
                    tuples.emplace_back( tuple );
                    nameNumbered.SetLength( name.Length() );
                    nr++;
                }

            }
            #if ALIB_MODULE_CORE //Report
                ALIB_ASSERT_ERROR( tuples.size() > 0,
                                   ASTR("No tuple resource \"")  , name,
                                   ASTR("\" found in category: "), library.ResourceCategory );
            #else
                #if ALIB_DEBUG
                    if( tuples.size() == 0 )
                    {
                        String64 tName    ( name );
                        String64 tCategory( library.ResourceCategory );
                        ALIB_ERROR( ASTR("No tuple resource \"")  , tName.ToCString(),
                                    ASTR("\" found in category: "), tCategory.ToCString() );
                    }
                #endif
            #endif
        }



}; // class ResourcedTupleLoader

}} // namespace lib::lang


}  // namespace aworx

#endif // HPP_ALIB_LANG_RESOURCE_TUPLE_LOADER
