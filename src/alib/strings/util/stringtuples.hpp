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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_STRINGS_UTIL_STRINGTUPLES)
    #error "Header already included"
#endif


// then, set include guard
#ifndef HPP_ALIB_STRINGS_UTIL_STRINGTUPLES
//! @cond NO_DOX
#define HPP_ALIB_STRINGS_UTIL_STRINGTUPLES 1
//! @endcond

#if !defined (HPP_ALIB_LANG_MEMORYBLOCKS)
#   include "alib/util/memoryblocks.hpp"
#endif

namespace aworx { namespace lib { namespace strings { namespace util  {

/**
 * Finds a tuple in a vector of those by performing a comparison of a given string with
 * string data provided in one of the tuple elements.
 *
 * The first template parameter T_StringColumn denotes
 * the index of the tuple elements that provides the string to search parameter \p{needle} in.
 *
 * The other template types are fetched with the second, variadic template parameter
 * \p{T_TupleTypes} and and denote the types of the tuples. These template parameters do not
 * need to be specified with invocations of the function, as they are inferred by the compiler
 * from parameter \p{haystack}.
 *
 *
 * @param  haystack         Vector of tuples to search in.
 * @param  needle           The string to search.
 * @tparam sensitivity      Character case sensitivity of the comparison.
 *                          Defaults to \b Case::Sensitive.
 * @tparam T_StringColumn   The tuple element number of the comparison string.
 * @tparam T_TupleTypes     The template type of the tuple. (Inferred by the compiler.)
 *
 * @return A pointer to the tuple found. \b nullptr on failure.
 */
template< Case        sensitivity,
          int         T_StringColumn,
          typename... T_TupleTypes >
std::tuple<T_TupleTypes...> const * FindStringInTupleVector( const std::vector<std::tuple<T_TupleTypes...>>& haystack,
                                                             const String&                                   needle    )
{

    for( auto& entry : haystack )
    {
        if ( std::get<T_StringColumn>(entry). template Equals<sensitivity>( needle )  )
            return  &entry;
    }
    return nullptr;
}

/**
 * Finds a tuple in a vector of those by performing a comparison of a given string with
 * string data provided in one of the tuple elements.
 *
 * The function has two integer template parameters. The first, \p{T_StringColumn} denotes
 * the index of the tuple elements that provides the string to search method parameter \p{needle} in.
 *
 * The second, \p{T_MinCharIndex},give the index of an element of the tuple that
 * holds an \c int value. This value is used to determine possible
 * "abbreviations" in the search. For example, if this template parameter is set to \c 1, then
 * the second column of the table is interpreted as this minimum value. If then a value of
 * for example \c 3 is found in an entry, then a match is returned if just the first three
 * characters of the string are provided with method parameter \p{needle}.
 * This allows to search for identifiers in a table which are abbreviated up to a mimimum length.
 *
 * The other template types are fetched with the second, variadic template parameter
 * \p{T_TupleTypes} and and denote the types of the tuples. These template parameters do not
 * need to be specified with invocations of the function, as they are inferred by the compiler
 * from parameter \p{haystack}.
 *
 * @param  haystack         Vector of tuples to search in.
 * @param  needle           The string to search.
 *
 * @tparam sensitivity      Character case sensitivity of the comparison.
 *                          Defaults to \b Case::Sensitive.
 * @tparam T_StringColumn   The tuple element number of the comparison string.
 * @tparam T_MinCharColumn  The element number of the column providing a minimum length
 *                          of the given search string \p{needle}.
 * @tparam T_TupleTypes     The template type of the tuple. (Inferred by the compiler.)
 *
 * @return A pointer to the tuple found. \b nullptr on failure.
 */
template< Case         sensitivity,
          int          T_StringColumn,
          size_t       T_MinCharColumn,
          typename...  T_TupleTypes >
std::tuple<T_TupleTypes...> const * FindStringStartInTupleVector( const std::vector<std::tuple<T_TupleTypes...>>& haystack,
                                                                  const String&                                   needle
                                                                   )
{
    for( auto& entry : haystack )
    {
        String identifier= std::get<T_StringColumn>(entry);
        int    minChars=   std::get<T_MinCharColumn>(entry);

        if ( Substring(needle).ConsumePartOf<sensitivity>(  identifier,  minChars ) == needle.Length() )
                return  &entry;
    }
    return nullptr;
}

/** ************************************************************************************************
 * Variadic Template class implementing a vector of tuples whose first element is of type
 * \c ref aworx::lib::strings::String "String" and the further elements are
 * the variadic types \p{TAssociatedTypes}.
 *
 * When new tuples are added with method #Add, then memory for copying the provided string is
 * allocated using an internal field of type
 * \c ref aworx::lib::lang::MemoryBlock "MemoryBlock".
 * This allows to add single strings, which are allocated in bigger memory chunks.
 *
 * Note that standard vector operations, including insertions and deletions are still allowed!
 * In other words, it is allowed to store string objects which are pointing to string data that
 * is \b not allocated in the internal chunks.
 *
 * The associated values are especially useful when the vector is sorted (e.g. using
 * \c std::sort), because such associations will be kept intact and allow referencing back
 * to whatever the strings represents.
 *
 * \note
 *   This class is new with \alib. It is not considered finished, optimized and stable in design.
 **************************************************************************************************/
template< typename... TAssociatedTypes >
class StringTable  : public std::vector<std::tuple<String, TAssociatedTypes...>>
{
    // #############################################################################################
    // Type definitions
    // #############################################################################################
    public:
        /**
         * This is a shortcut giving the type of the concrete
         * standard <c>std::tuple\<String, ...\></c>  that instantiations of this template class
         * store.
         */
        using ElementType= std::tuple<String, TAssociatedTypes...>;

        /**
         * This is a shortcut giving the type of the vector that this class implements.
         * Used internally to access inherited methods.
         */
        using VectorType = std::vector<ElementType>;


    // #############################################################################################
    // protected fields
    // #############################################################################################
    protected:
        /// The list of allocated memory blocks.
        MemoryBlocks                                 blocks;

   // #############################################################################################
   // Constructor / Destructor
   // #############################################################################################
   public:
        /** ****************************************************************************************
         * Constructor.
         * Accepts a value \c stdBlockSize to manipulate the standard size of allocated memory
         * chunks. (This value, is forwarded to the constructor of class
         * \alib{util,MemoryBlocks}.)
         *
         * @param stdBlockSize The standard size of memory blocks allocated.
         ******************************************************************************************/
        StringTable( size_t stdBlockSize = 8 * 1024 )
        : blocks( stdBlockSize )
        {
        }

   // #############################################################################################
   // Interface
   // #############################################################################################
   public:
        /** ****************************************************************************************
         * Adds a tuple. The contents of the string of the tuple (its first element) is copied
         * from the given string \p{src}. The memory for this string is allocated using
         * field #blocks.<br>
         * The other members of the tuple added are forwarded from variadic parameter block \p{args}.
         *
         * \note
         *   With some compilers (as the type of writing this with \b clang), a compilation error
         *   might occur. To avoid this, the tuple type used needs to be fixed with using macro
         *   \ref ALIB_STRING_CONSTRUCTOR_FIX "ALIB_STRING_CONSTRUCTOR_FIX( std::tuple<String ALIB_COMMA ...<yourtypes>... )"

         * @param src   The string to copy into the first member of the tuple.
         * @param args  Variadic arguments to fill the rest of the inserted tuple.
         *
         * @return A reference to the string that was added.
         ******************************************************************************************/
        String& Add( const String& src, TAssociatedTypes... args )
        {
            // on errors, see note above!
            VectorType::emplace_back( blocks.AllocAndCopy( src ), args...  );
            return std::get<0>(VectorType::back());
        }


        /** ****************************************************************************************
         * Clears this vector of tuples and frees the allocated block memory, either for reuse
         * or completely. (Parameter \c deallocate is forwarded to
         * \ref aworx::lib::lang::MemoryBlocks::Clear "MemoryBlocks::Clear".)
         *
         * @param deallocate If \b %CurrentData::Clear , all allocated memory chunks are freed.
         *                   If \b %CurrentData::Keep, which is the default, they
         *                   will be reused for future new strings that are added.
         ******************************************************************************************/
        void    Clear( CurrentData deallocate= CurrentData::Keep )
        {
            VectorType::clear();
            blocks.Clear( deallocate );
        }
};

}}} // namespace aworx[::lib::strings::util]

/// Type alias in namespace #aworx.
template< typename... TAssociatedTypes >
using     StringTable  =   aworx::lib::strings::util::StringTable<TAssociatedTypes...>;


} // namespace aworx

#endif // HPP_ALIB_STRINGS_UTIL_STRINGTUPLES
