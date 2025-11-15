//==================================================================================================
/// \file
/// This header-file contributes to module \alib_containers but for technical reasons is part of
/// the module \alib_format.
/// is part of module \alib_format.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if ALIB_DEBUG_CONTAINERS
ALIB_EXPORT namespace alib::containers {

#include "ALib.Lang.CIFunctions.H"
/// Invokes method \alib{containers;DbgGetHashTableDistribution} and creates human-readable
/// output, ready to be logged or written to the console.
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
//    Furthermore the compiler-symbol \ref ALIB_DEBUG_CONTAINERS has to be set.
///
/// \see
///   Sibling namespace functions \alib{containers;DbgGetHashTableDistribution} and
///   \alib{containers;DbgDumpHashtable} provided for debugging and optimization.
///
/// @tparam THashtable          A specification  of templated type \alib{containers;HashTable}.
///                             Deduced by the compiler by given parameter \p{hashtable}.
/// @param  hashtable           The hashtable to investigate on.
/// @param  detailedBucketList  If \c true is given, for each bucket a line with its size value and
///                             a "size bar" is written.
///
/// @return A string containing human-readable information about the distribution of elements
///         in the hashtable.
template<typename THashtable>
inline
AString DbgDumpDistribution(const THashtable& hashtable, bool detailedBucketList )
{
    auto values = DbgGetHashTableDistribution( hashtable );
    AString result;
    double  loadFactor= std::get<0>( values );
    double  deviation = std::get<1>( values );
    integer minSize   = std::get<2>( values );
    integer maxSize   = std::get<3>( values );
    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    Formatter& formatter= *Formatter::Default;
    formatter.GetArgContainer();
    formatter.Format( result, "Size:        {}\n"
                               "#Buckets:    {}\n"
                               "Load Factor: {:.02}  (Base: {:.01}  Max: {:.01})\n"
                               "Deviation:   {:.02} (~{:%.1})\n"
                               "Minimum:     {}\n"
                               "Maximum:     {}\n",
                       hashtable.Size(),
                       hashtable.BucketCount(),
                       loadFactor, hashtable.BaseLoadFactor(), hashtable.MaxLoadFactor(),
                       deviation , ( hashtable.Size() != 0
                                                  ? deviation / loadFactor
                                                  : 0.0                                          ),
                       minSize, maxSize );


        // bucket filling statistics
        integer* bucketFills= new integer[size_t(maxSize + 1)];
        for( integer i= 0; i < maxSize ; ++i)
            bucketFills[i]= 0;

        for( uinteger i= 0; i < hashtable.BucketCount() ; ++i)
            ++bucketFills[hashtable.BucketSize(i)];

        formatter.Format( result, "Bucket Fills:  Size    #Buckets\n" );
        formatter.Format( result, "              -----------------\n" );
        for( integer i= 0; i < maxSize ; ++i)
            formatter.Format( result, "               {}      {}\n", i, bucketFills[i] );
        delete[] bucketFills;


        // detailed bucked list
        if( detailedBucketList )
        {
            formatter.Format(result, "\nDetailed Bucket List:\n");
            auto    qtyBuckets      = hashtable.BucketCount();
            for( uinteger i= 0 ; i < qtyBuckets ; ++i )
            {
                auto bucketSize= hashtable.BucketSize( i );
                formatter.Format(result, "{:3} ({:2}): {!FillCX}\n", i, bucketSize,
                                  int(bucketSize) );
            }

        }
    return result;
}

/// Dumps all values of the hash table sorted by buckets.
/// Besides other scenarios of usage, this method allows investigating into how the keys of
/// the table are distributed in the buckets, and thus learn something about the hash algorithm used.
///
/// Before invoking this method, specializations of \alib{strings;AppendableTraits} have to be made
/// and furthermore, boxed values of the type have to be <em>"made appendable"</em> to
/// instances of type \b %AString. The latter is rather simple, if done using
/// \ref ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE "this macro" during bootstrap.
///
/// \note
///   If the prerequisites for using this method seem to be too complicated and not worth the
///   effort for a "quick debug session", it is recommended to just copy the source code of this
///   inline function and adopt the \alib{format;Formatter::Format} statement to
///   suit a specific type stored in \p{hashtable}.
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
//    Furthermore the compiler-symbol \ref ALIB_DEBUG_CONTAINERS has to be set.
///
/// \see
///   Sibling namespace functions \alib{containers;DbgGetHashTableDistribution} and
///   \alib{containers;DbgDumpDistribution} provided for debugging and optimization.
///
/// @tparam THashtable          A specification  of templated type \alib{containers;HashTable}.
///                             Deduced by the compiler by given parameter \p{hashtable}.
/// @param  hashtable           The hashtable to investigate on.
/// @return A string containing a dump of the given hash table's contents.
template<typename THashtable>
inline
AString DbgDumpHashtable(const THashtable& hashtable )
{
    AString result;
    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    Formatter& formatter= *Formatter::Default;
    formatter.GetArgContainer();
    formatter.Format(result, "\nHashtable dump:\n");
    auto    qtyBuckets      = hashtable.BucketCount();
    for( uinteger i= 0 ; i < qtyBuckets ; ++i )
    {
        auto bucketSize= hashtable.BucketSize( i );
        formatter.Format(result, "{:3} ({:2}): ", i, bucketSize );

        int entryNo= 0;
        for( auto  bucketIt=   hashtable.begin(i)
                ;  bucketIt != hashtable.end  (i)
                ;++bucketIt )
        {
           if( entryNo!=0)
               result << "          ";
           formatter.Format(result, "{}: {}\n", entryNo, *bucketIt );
           ++entryNo;
        }
        if( bucketSize == 0)
            result << "---" << NEW_LINE;
    }

    return result;
}
} // namespace [alib::containers]
#include "ALib.Lang.CIMethods.H"
#endif //ALIB_DEBUG_CONTAINERS
