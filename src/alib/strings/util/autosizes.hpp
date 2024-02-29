/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_strings of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_STRINGS_UTIL_AUTOSIZES
#define HPP_ALIB_STRINGS_UTIL_AUTOSIZES 1

#if !defined (HPP_ALIB_STRINGS_STRING)
#   include "alib/strings/string.hpp"
#endif



// #################################################################################################
// includes
// #################################################################################################
#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif

namespace alib {  namespace strings { namespace util  {

/** ************************************************************************************************
 * This class stores and manages tabulator positions and field sizes. The class supports a
 * simple session handling, by storing each value once for the actual output session and a
 * second time for a future session. The motivation for writing this class came from the
 * requirements of logging library \alib_alox. The goals here are:
 *
 * - During a logging session, log output should be as much tabular as possible.
 * - On the same time, the log output should be as "narrow" as possible.
 *
 * If used correctly, this class helps to achieve the following:
 * - A new log-output session increases tab stops and field widths during execution as needed.
 * - If values need to be increased, a certain amount of "extra padding" might be
 *   added to avoid too many small increments.
 * - Once all tab stops or fields have been logged with values of their maximum size, the log output
 *   will not vary in respect to tab stops and autosizes any more.
 * - If a subsequent session contains the very same log-output (aka the same requested tab positions
 *   and field width), all extra space is removed and the log output is 100% tabular
 *   beginning with the session start.
 * - If a subsequent session contains smaller values, then this session is formatted with the
 *   (still larger) width of the previous session. After that, the next session will use the smaller
 *   sizes.
 *
 * This approach very well guarantees stable log output widths across sessions. Only if the
 * execution path of a software changes (or logging verbosity setting is changed), adjustments
 * are performed.
 *
 * To preserve the information across <em>sessions</em>, this class provides methods to transform
 * it's information from and to string representations which can be stored in configuration files.
 *
 * As the use case of this class is not restricted to log output, this class is exposed
 * as a general utility class of \alib_strings_nl.
 **************************************************************************************************/
class AutoSizes
{

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /** The entry type, tab stop or field width. */
        enum class Types
        {
            Tabstop, ///< denotes a tab stop entry.
            Field,   ///< denotes a field width entry.
        };

        /**
         * The actual index requested by #Next.
         * This field is reset to 0 with every invocation of #Start. The field is public and
         * may be read and manipulated, which is considered "expert use".
         */
        size_t                      ActualIndex;



    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /** Actual and current session entries of tab stop indexes, respectively field widths. */
        struct Entry
        {
            Types       type;         ///< The type of entry.
            integer     actual;       ///< The actually used tab stop or size
            integer     session;      ///< The maximum requested size in actual session.

            /**
             * Constructor.
             * @param t Value for field #type.
             * @param v Value for field #actual.
             * @param s Value for field #session.
             */
            Entry( Types t, integer v, integer s )
            : type(t), actual(v), session(s)
            {}
        };

        /** The current and measured sizes. */
        std::vector<Entry>          data;

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Resets the all values, current ones and the once of the currently measured session.
         ******************************************************************************************/
        void        Reset ()                 {   data.clear(); Start();  }

        /** ****************************************************************************************
         * Consolidates the values. This method is usually not invoked directly.
         * Instead, it is invoked with method #Import.
         *
         * The method loops through all values and copies the session values to the actual ones.
         * The difference of both values is summed up during the loop and entries of type
         * \alib{strings::util::AutoSizes,Types::Tabstop} are adjusted by that difference.
         * As a result, the new values represent the smallest output format that fits all
         * rows, if the same output is performed as in the previous "session".
         ******************************************************************************************/
        ALIB_API
        void        Consolidate ();

        /** ****************************************************************************************
         * Initializes a new query sequence, which is a series of invocations of method #Next.
         ******************************************************************************************/
        void        Start ()                 {   ActualIndex=   0; }

        /** ****************************************************************************************
         * Returns the actual auto value stored, respectively, if the given requested size is
         * higher than what is stored, stores and returns the requested size.
         *
         * In the latter case, the given extra growth is added to the requested size, but only if
         * the value was set at least once before. In other words, the extra size is added only with
         * the second growth and each subsequent one.
         *
         * The requested size in addition replaces the current "session" value if it is higher than
         * the currently stored value. To this value, the growth padding is not added.
         *
         * @param type           The type of entry.
         * @param requestedSize  The minimum size that is requested.
         * @param growthPadding  Added to the new size, if the requested size is greater than
         *                       the stored size and if the stored size does not equal \c -1.
         *
         * @return The (new) size of the auto field.
         ******************************************************************************************/
        ALIB_API
        integer     Actual( Types type, integer requestedSize, integer growthPadding );

        /** ****************************************************************************************
         * Invokes #Actual and then increases the internal position counter.
         *
         * @param type            The type of entry.
         * @param requestedSize   The minimum size that is requested.
         * @param growthPadding   Added to the new size if the requested size is greater than
         *                        the stored size and if the stored size does not equal \c -1.
         *
         * @return The (new) size of the auto field.
         ******************************************************************************************/
        integer     Next  ( Types type, integer requestedSize, integer growthPadding )
        {
            auto result= Actual( type, requestedSize, growthPadding );
            ++ActualIndex;
            return result;
        }

        /** ****************************************************************************************
         * Imports values from the given \alib{strings,TString,String} by parsing it.
         * Usually, string data are used for importing values was previously generated
         * with method #Export.
         *
         * If parameter \p{session} equals \alib{lang::CurrentData,Clear} (which is the default),
         * then after the import, method #Consolidate is invoked.
         *
         * \note
         *   Parsing is 100% error tolerant. If the given string does not contain what is expected
         *   (see method #Export), then only a part or just nothing is imported.
         *   This is due to the purpose of this class, which is to allow nicer, tabbed output.
         *   If this is not possible due to import problems, the system should work normally still.<br>
         *   With debug builds, an \ref ALIB_WARNING is written.
         *
         *
         * @param source    The \b %String that is parsed for the exported data.
         * @param session   Denotes if #Consolidate is to be invoked after import.
         *                  Defaults to \alib{lang::CurrentData,Clear}.
         ******************************************************************************************/
        ALIB_API
        void        Import( const String& source, lang::CurrentData session= lang::CurrentData::Clear );

        /** ****************************************************************************************
         * Exports the current session values by converting the stored values to a string
         * representation and appending them to the given
         * \alib{strings,TAString,AString} object.
         *
         * The format of the entries written into the string is <c>"(T|F,Actual,Measured)</c>
         * which should be self-explanatory.
         *
         * @param target       The \b %AString to receive the our values
         ******************************************************************************************/
        ALIB_API
        void        Export( AString& target );
};


}} // namespace alib[::strings::util]


/// Type alias in namespace \b alib.
using     AutoSizes=            strings::util::AutoSizes;

} // namespace [alib]

#endif // HPP_ALIB_STRINGS_UTIL_AUTOSIZES
