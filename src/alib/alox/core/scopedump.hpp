// #################################################################################################
//  aworx::lib::lox::core - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include ALox main header first...
#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

// then, set include guard
#ifndef HPP_ALOX_CORE_SCOPEDUMP
#define HPP_ALOX_CORE_SCOPEDUMP 1


// #################################################################################################
// includes
// #################################################################################################
#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif

#include "scopestore.hpp"


namespace aworx { namespace lib { namespace lox { namespace core {

/** ************************************************************************************************
 * This class is an internal helper to export current scope values.
 * It was extracted from class \b %Lox to keep that class clean from this somehow 'cluttered' code.
 *
 * The current purpose of this class is to write information into an AString buffer. This is used
 * by method \ref aworx::lib::lox::Lox::State "Lox::State". In the future, other export formats,
 * like JSON might be implemented here.
 *
 * \note As this is a pure internal helper class. Documentation may be limited.
 **************************************************************************************************/
class ScopeDump
{
    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /// The target to write to
        AString&                        targetBuffer;

        /// String to identify global keys.
        const String                    noKey;

        /// The maximum length of a key. Adjusts (increases) over lifecycle.
        integer                         maximumKeyLength;

        /// User-defined threads names.
        ScopeInfo::ThreadDictionary&    threadDict;

    // #############################################################################################
    // Public interface
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Constructor
         * @param threadDictionary User-defined threads names.
         * @param target           Target string to write to.
         * @param noKeyHashKey     String to identify global keys.
         * @param maxKeyLength     The start value for the maximum length of a key.
         ******************************************************************************************/
        ScopeDump( ScopeInfo::ThreadDictionary& threadDictionary, const String noKeyHashKey,
                   AString& target, int maxKeyLength= 10 )
        : targetBuffer(target)
        , noKey(noKeyHashKey)
        , maximumKeyLength(maxKeyLength)
        , threadDict(threadDictionary)
        {
        }

        /** ****************************************************************************************
         * Writes hash tables stored in a ScopeStore. Keys are AStrings.
         * Value types currently supported are LogData* and int.
         * @param store The store to use.
         * @return The total number of hash table entries written.
         ******************************************************************************************/
        template<typename T>
        ALIB_API
        int writeStoreMap( ScopeStore<T>* store );


        /** ****************************************************************************************
         * Writes the contents of the ScopeStore used for <em>Scope Domains</em>.
         * @param store         The store to use
         * @param indentSpaces  The number of spaces to add at the beginning of each line.
         * @return The total number of <em>Scope Domains</em> written.
         ******************************************************************************************/
        template<typename T>
        ALIB_API
        int writeStore( ScopeStore<T>* store, int indentSpaces );

    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Helper method to #writeStoreMap.
         * @param map       The map to use
         * @param prefix    A prefix string for each line
         * @return The number of objects written
         ******************************************************************************************/
        template<typename T>
        ALIB_API
        integer writeStoreMapHelper( std::map<AString, T>& map, const String& prefix );

        /** ****************************************************************************************
         * Helper method to write thread information.
         * @param threadID  The thread id.
         * @return The target to allow concatenated calls.
         ******************************************************************************************/
        ALIB_API
        AString& storeThreadToScope( int threadID );

        /** ****************************************************************************************
         * Helper method to write a StringTrees' key as scope information.
         * @param key  The key used for the \b %StringTree.
         * @return The target to allow concatenated calls.
         ******************************************************************************************/
        ALIB_API
        AString& storeKeyToScope( String key );

}; // ScopeDump


//! @cond NO_DOX
// needed to suppress pedantic warnings with clang
extern template   int ScopeDump::writeStore   ( ScopeStore<NAString*               >* store, int indentSpaces );
extern template   int ScopeDump::writeStore   ( ScopeStore<Box*                    >* store, int indentSpaces );
extern template   int ScopeDump::writeStoreMap( ScopeStore<std::map<AString, int>* >* store );
extern template   int ScopeDump::writeStoreMap( ScopeStore<std::map<AString, Box>* >* store );
//! @endcond


}}}}// namespace [aworx::lib::lox::core]



#endif // HPP_ALOX_CORE_SCOPEDUMP
