//==================================================================================================
/// \file
/// This header file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_STRINGS_UTIL_AUTOSIZES
#define HPP_ALIB_STRINGS_UTIL_AUTOSIZES 1
#pragma once
#include "alib/strings/string.hpp"
#include <vector>

namespace alib {  namespace strings { namespace util  {

//==================================================================================================
/// This class stores and manages tabulator positions and field sizes. The class supports a
/// simple session handling, by storing each value once for the actual output session and a
/// second time for a future session. The motivation for writing this class came from the
/// requirements of logging library \alib_alox. The goals here are:
///
/// - During a logging session, log output should be as much tabular as possible.
/// - On the same time, the log output should be as "narrow" as possible.
///
/// If used correctly, this class helps to achieve the following:
/// - A new log-output session increases tab stops and field widths during execution as needed.
/// - If values need to be increased, a certain amount of "extra padding" might be
///   added to avoid too many small increments.
/// - Once all tab stops or fields have been logged with values of their maximum size, the log output
///   will not vary in respect to tab stops and auto-sizes anymore.
/// - If a subsequent session contains the very same log-output (aka the same maximum of requested
///   tab positions and field width), all extra space is removed and the log output is 100% tabular
///   beginning with the session start.
/// - If a subsequent session contains smaller values, then this session is formatted with the
///   (still larger) width of the previous session. After that, the next session will use the smaller
///   sizes.
///
/// This approach very well guarantees stable log output widths across sessions. Only if the
/// execution path of software changes (or logging verbosity setting is changed), adjustments
/// are performed.
///
/// To preserve the information across <em>sessions</em>, this class provides methods to transform
/// its information from and to string representations which can be stored in configuration files.
///
/// As the use case of this class is not restricted to log output, but can be used in general when
/// working with strings, it became a utility class of module \alib_strings_nl.
///
/// @see
/// - The use of this class with field \alib{lang::format;FormatterPythonStyle::Sizes}.
/// - The use of this class within \alox configuration variable
///   \alib{lox::textlogger;FormatAutoSizes}.
//==================================================================================================
class AutoSizes
{

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /// The entry type, tab stop or field width.
        enum class Types
        {
            Tabstop, ///< denotes a tab stop entry.
            Field,   ///< denotes a field width entry.
        };

        /// The actual index requested by #Next.
        /// This field is reset to 0 with every invocation of #Start. The field is public and
        /// may be read and manipulated, which is considered "expert use".
        unsigned int    ActualIndex;

    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /// Actual and current session entries of tab stop indexes, respectively field widths.
        struct Entry
        {
            Types   type;         ///< The type of entry.
            integer actual;       ///< The actually used value.
            integer session;      ///< The maximum value requested in the actual session.

            /// Constructor.
            /// @param t Value for field #type.
            /// @param v Value for field #actual.
            /// @param s Value for field #session.
            Entry( Types t, integer v, integer s )
            : type(t), actual(v), session(s)
            {}
        }; // struct Entry

        /// The current and measured sizes.
        std::vector<Entry>          data;

        /// Determines whether any value was changed sincel last #Reset.
        bool                        dirty;

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /// If set, method #Actual will not update the values, but instead return the requested
        /// value.<br>
        /// Defaults to \c false.
        bool                        WriteProtected                                          = false;

        /// Resets all values, the current ones and the ones of the currently measured session, to
        /// \c 0 and invokes #Restart().
        void    Reset ()                                   { data.clear(); Restart(); dirty= true; }

        /// Determines whether any value was changed since construction, or the last invocation of
        /// methods #Import, #Export or #SetUnchanged.
        /// @return \c true if any entry was changed, \c false otherwise.
        bool        IsChanged()                                              const { return dirty; }

        /// Same as #IsChanged(), but clears the internal flag.
        /// @return \c true if any entry was changed, \c false otherwise.
        bool        SetUnchanged()              { bool result= dirty; dirty= false; return result; }

        //==========================================================================================
        /// Consolidates the values. This method is usually not invoked directly.
        /// Instead, it is invoked with method #Import.
        /// The method loops through all values and copies the current session values to the actual
        /// ones. The difference of both values is summed up during the loop and entries of type
        /// \alib{strings::util::AutoSizes;Types::Tabstop} are adjusted by that difference.
        /// As a result, the new values represent the smallest output format that fits all
        /// rows, if the same output is performed as in the previous "session".
        //==========================================================================================
        ALIB_API
        void    Consolidate();

        //==========================================================================================
        /// Initializes a new query sequence, which is a series of invocations of method #Next.
        /// @param startIdx The tab stop or field size position to continue with.
        //==========================================================================================
        void    Restart( unsigned int startIdx= 0 )                       { ActualIndex= startIdx; }

        //==========================================================================================
        /// Returns the actual auto value stored, respectively, if the given requested size is
        /// higher than what is stored, stores and returns the requested size.
        ///
        /// In the latter case, the given extra growth is added to the requested size, but only if
        /// the value was set at least once before. In other words, the extra size is added only with
        /// the second growth and each subsequent one.
        ///
        /// The requested size in addition replaces the current "session" value if it is higher than
        /// the currently stored value. To this value, the growth padding is not added.
        ///
        /// The whole mechanism can be disabled by setting flag #WriteProtected. If so, this
        /// method just returns \p{requestedSize}
        ///
        /// If any of the values are changed, a call to #IsChanged will return \c true.
        ///
        /// @param type           The type of entry.
        /// @param requestedSize  The minimum size that is requested.
        /// @param growthPadding  Added to the new size, if the requested size is greater than
        ///                       the stored size and if the stored size does not equal \c -1.
        ///
        /// @return The (new) size of the auto field.
        //==========================================================================================
        ALIB_API
        integer Actual( Types type, integer requestedSize, integer growthPadding );

        //==========================================================================================
        /// Invokes #Actual and then increases the internal position counter.
        ///
        /// @param type            The type of entry.
        /// @param requestedSize   The minimum size that is requested.
        /// @param growthPadding   Added to the new size if the requested size is greater than
        ///                        the stored size and if the stored size does not equal \c -1.
        ///
        /// @return The (new) size of the auto field.
        //==========================================================================================
        integer Next  ( Types type, integer requestedSize, integer growthPadding )
        {
            auto result= Actual( type, requestedSize, growthPadding );
            ++ActualIndex;
            return result;
        }

        //==========================================================================================
        /// Imports values from the given \alib{strings;TString;String} by parsing it.
        /// Usually, string data are used for importing values was previously generated
        /// with method #Export.
        ///
        /// If parameter \p{session} equals \alib{lang::CurrentData;Clear} (which is the default),
        /// then after the import, method #Consolidate is invoked.
        ///
        /// \note
        ///   Parsing is 100% error-tolerant. If the given string does not contain what is expected
        ///   (see method #Export), then only a part or just nothing is imported.
        ///   This is due to the purpose of this class, which is to allow nicer, tabbed output.
        ///   If this is not possible due to import problems, the system should work normally still.
        ///   With debug-builds, an \ref ALIB_WARNING is written in this case.
        ///
        /// This method clears the internal flag that tracks changes, hence after an invocation,
        /// #IsChanged returns \c false.
        ///
        /// @param source    The \b %String that is parsed for the exported data.
        /// @param session   Denotes if #Consolidate is to be invoked after import.
        ///                  Defaults to \alib{lang::CurrentData;Clear}, which performs
        ///                  consolidation.
        //==========================================================================================
        ALIB_API
        void    Import( const String& source, lang::CurrentData session= lang::CurrentData::Clear );

        //==========================================================================================
        /// Exports the current session values by serializing the stored values into to the given
        /// \alib{strings;TAString;AString} object.
        ///
        /// For each current entry in this object, <c>"T|F Actual[,session]</c> is written,
        /// separated by a forward slash <c>'/'</c>. The session value is written only in case
        /// it differs from the actual value.
        /// Furthermore, if field #WriteProtected is set, an exclamation mark <c>'!'</c> will be
        /// written as a first character. With that, the complete syntax is:
        ///
        ///          [!] [ T|F Actual[,session] ]   [/ T|F Actual[,session] ]
        ///
        /// This method clears the internal flag that tracks changes, hence after an invocation,
        /// #IsChanged returns \c false.
        ///
        /// @param target       The \b %AString to receive the our values
        //==========================================================================================
        ALIB_API
        void    Export( AString& target );
};


}} // namespace alib[::strings::util]


/// Type alias in namespace \b alib.
using     AutoSizes=            strings::util::AutoSizes;

} // namespace [alib]

#endif // HPP_ALIB_STRINGS_UTIL_AUTOSIZES

