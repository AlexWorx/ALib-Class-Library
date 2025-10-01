//==================================================================================================
/// \file
/// This header-file is part of module \alib_format of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace  format {

//==============================================================================================
/// This struct declares the signature of the \ref alib_boxing_functions "box-function"
/// \b FFormat. It can be implemented to write the contents of a boxed type the given
/// \b %AString instance, in accordance with a custom format specification.
///
/// \see
///   For more information about this class see chapter
///   \ref alib_format_custom_types of the Programmer's Manual of module \alib_format_nl.
//==============================================================================================
struct FFormat
{
    /// Signature of the invokable function.<br>
    /// Implementations write the content of \p{box} to the given \b %AString object \p{target} in
    /// accordance to the type-specific format specification \p{formatSpec}.
    ///
    /// @param self       The box that the function was invoked on.
    /// @param formatSpec The specification of the format (type specific). If empty, a default
    ///                   specification string might have to be chosen.
    /// @param nf         A copy of the number format of the formatter (allowed to be modified).
    /// @param target     The AString object receiving the formatted string.
    using Signature = void (*) ( const Box& self, const String& formatSpec, NumberFormat& nf,
                                 AString& target );
};


// forward declaration
class Formatter;

} // namespace alib[::format]

/// A shared pointer to instances of \alib{format;Formatter;standard formatters}.
/// \see Static member \alib{format;Formatter::Default}.
using   SPFormatter        =  containers::SharedPtr<format::Formatter>;

namespace  format {

//==================================================================================================
/// This is an abstract base class to implement an \alib string formatter. A string formatter
/// uses a "format string" to transform arguments into formatted text. The format string defines
/// how the arguments are transformed by offering a "placeholder syntax".
///
/// With this information, it becomes clear that different formatter types (derived types that offer
/// a concrete placeholder syntax) all can have the same interface methods.
/// This class defines this abstract interface.
///
/// Built-in formatters derived from this class, are
/// \alib{format;FormatterPythonStyle;FormatterPythonStyle} and
/// \alib{format;FormatterJavaStyle;FormatterJavaStyle}.
//==================================================================================================
class Formatter
#if ALIB_DEBUG_CRITICAL_SECTIONS
                 : public lang::DbgCriticalSections
#endif
{

    // #############################################################################################
    //  Internal fields
    // #############################################################################################
    protected:
        /// This allocator is (exclusively) used for field #boxes.
        MonoAllocator   allocator;

        /// A list of boxes. This is reset with every new invocation of variadic template method
        /// #Format
        BoxesMA         boxes;

        /// A buffer used for conversion of the next argument if it is not of a string-type.
        AString         formatStringBuffer;

        ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL

    // #############################################################################################
    //  public fields
    // #############################################################################################
    public:
        /// Stores default attributes for formatting numbers.
        /// Likewise #AlternativeNumberFormat, it usually is not used directly for formatting by
        /// descendants.
        /// Instead, at the beginning of parsing a next placeholder field, values are copied to
        /// a local copy. During the parsing process, values are then modified only in this local
        /// copy, probably taken from #AlternativeNumberFormat.
        ///
        /// This object is to be initialized in the constructor (of descendants) once to meet the
        /// formatting defaults of the corresponding specification.
        /// If after construction, attribute values of this object are changed, such changes are
        /// applied to all number formatting.
        NumberFormat    DefaultNumberFormat;

        /// This number format is used to store alternative attributes. Likewise #DefaultNumberFormat,
        /// it is never used directly for formatting.
        /// Instead, when processing the placeholder syntax, alternatives get copied from either this
        /// object or from #DefaultNumberFormat.
        ///
        /// This object is initialized in the constructor (of descendants) once to meet the
        /// formatting defaults of the corresponding specification.
        ///
        /// With the implementations of this class provided with \alib, not all fields in this object
        /// are used. The fields used are:
        /// <p>
        /// - Locale-specific versions of floating point separators:
        ///   - \alib{strings;TNumberFormat::DecimalPointChar;NumberFormat::DecimalPointChar}
        ///   - \alib{strings;TNumberFormat::ThousandsGroupChar;NumberFormat::ThousandsGroupChar}
        ///
        ///   These are retrieved according to the current locale once in the constructor. To
        ///   change the locale, these fields can be changed.
        ///
        /// - Lower case versions of floating point literals:
        ///   - \alib{strings;TNumberFormat::ExponentSeparator;NumberFormat::ExponentSeparator}
        ///   - \alib{strings;TNumberFormat::INFLiteral;NumberFormat::INFLiteral}
        ///   - \alib{strings;TNumberFormat::NANLiteral;NumberFormat::NANLiteral}
        ///
        /// - Lower case versions of prefix literals that indicate the base of integral values:
        ///   - \alib{strings;TNumberFormat::BinLiteralPrefix;BinLiteralPrefix}
        ///   - \alib{strings;TNumberFormat::HexLiteralPrefix;HexLiteralPrefix}
        ///   - \alib{strings;TNumberFormat::OctLiteralPrefix;OctLiteralPrefix}
        NumberFormat    AlternativeNumberFormat;

        ///  An optional, next formatter. If set, this formatter will be invoked for a format string
        ///  that does not contain recognized placeholders.
        ///  \attention
        ///    This field is public and not further maintained by this class. Setting the field lies
        ///    completely in the responsibility of the user. E.g., cyclic settings must be avoided.
        ///    Also, object life-cycle management is completely up to the user.
        SharedPtr<Formatter>  Next;

    // #############################################################################################
    //  Constructor/destructor
    // #############################################################################################
    public:
        /// Default Constructor.
        Formatter()
        :
        #if ALIB_DEBUG_CRITICAL_SECTIONS
          lang::DbgCriticalSections("Formatter")
        ,
        #endif
          allocator(ALIB_DBG("Formatter",) 1)
        , boxes( allocator )
        {}
        
        //==========================================================================================
        /// Destructs an object of this class.
        /// Note that concatenated formatters are not deleted automatically.
        //==========================================================================================
        virtual        ~Formatter()
        {}

    // #############################################################################################
    //  Interface
    // #############################################################################################

        //==========================================================================================
        /// Variadic template method that accepts a target \b %AString and a list of arguments.
        /// This is a convenience method to allow single-line format invocations.
        ///
        /// \note
        ///   This method uses <c>static_assert</c> to disallow the invocation with one
        ///   variadic argument of type \alib{boxing;TBoxes} or a derived type.
        ///   This is to ensure that for these box containers, the more efficient method #FormatArgs
        ///   is used.
        ///
        /// @tparam TArgs   Variadic template type list.
        /// @param target   An AString that takes the result.
        /// @param args     The variadic list of arguments to be used with formatters.
        /// @return A reference to this formatter to allow concatenated operations.
        //==========================================================================================
        template <typename... TArgs>
        Formatter&          Format( AString& target, TArgs&&... args )
        {
            // assert that this method is not used with Boxes containers.
            // Those are to be processed with FormatArgs
            constexpr  bool Argument_has_type_Boxes=
                  (sizeof...(TArgs) == 1)
               && (   std::is_base_of_v< BoxesHA, std::decay< std::tuple_element<0, std::tuple<TArgs...>>>>
                   || std::is_base_of_v< BoxesMA, std::decay< std::tuple_element<0, std::tuple<TArgs...>>>>
                   || std::is_base_of_v< BoxesPA, std::decay< std::tuple_element<0, std::tuple<TArgs...>>>> );
            static_assert( !Argument_has_type_Boxes,
                           "To pass a container of type Boxes to a formatter, use method FormatArgs." );

            // create argument objects using implicit constructor invocation
            boxes.clear();
            boxes.Add( std::forward<TArgs>( args )... );

            // invoke format
            formatLoop( target, boxes );
            return *this;
        }

        //==========================================================================================
        /// Formats the internal list of arguments that is returned by methods #GetArgContainer
        /// and #Reset.
        ///
        /// This method may be more efficient than using inlined variadic method \b Format
        /// and should be preferred if:
        /// - Format arguments cannot be collected in a single invocation, for example, if those
        ///   are to be collected in a loop.
        /// - A sequence of different format operations is to be performed.
        ///
        /// @param target     An AString that takes the result.
        /// @return A reference to this formatter to allow concatenated operations.
        //==========================================================================================
        inline
        Formatter&          FormatArgs( AString& target );  // implementation below


        //==========================================================================================
        /// Same as Format(AString&) but allows specifying an external list of arguments instead of
        /// the internally allocated object, which is returned by methods #GetArgContainer
        /// and #Reset.
        ///
        /// @param args       The arguments to be used with formatters.
        /// @param target     An AString that takes the result.
        /// @return A reference to this formatter to allow concatenated operations.
        //==========================================================================================
        template<typename TAllocator>
        Formatter&          FormatArgs( AString& target, const boxing::TBoxes<TAllocator>& args )
        {
            ALIB_DCS
            return formatLoop( target, args );
        }


        //==========================================================================================
        /// Clones and returns a copy of this formatter.
        ///
        /// If a formatter is attached to field \alib{format;Formatter::Next}, it is
        /// cloned as well (recursively).
        ///
        /// @returns An object of the same (derived) type and the same custom settings.
        //==========================================================================================
        virtual SPFormatter  Clone()                                                            = 0;

        //==========================================================================================
        /// Clones the settings from the given formatter.
        /// @param reference  The formatter to copy settings from.
        //==========================================================================================
        ALIB_DLL
        virtual void        CloneSettings( Formatter& reference );

        //==========================================================================================
        /// Virtual method used to reset internal states.
        ///
        /// It is formatter-depended, which state information is reset. As a sample, derived type
        /// \alib{format;FormatterPythonStyle;FormatterPythonStyle} clears its auto-tab
        /// and auto-width positions.
        ///
        /// The default implementation does nothing.
        /// @return An internally allocated container of boxes that may be used to collect
        ///         formatter arguments.
        //==========================================================================================
        virtual BoxesMA&    Reset()                         {ALIB_DCS boxes.clear(); return boxes; }

        //==========================================================================================
        /// Returns an empty vector of arguments, which can be passed to the format interface.<br>
        /// Note: This is nothing more than a recycling strategy.
        /// @return An internally allocated container of boxes that may be used to collect
        ///         formatter arguments.
        //==========================================================================================
        virtual BoxesMA&    GetArgContainer()               {ALIB_DCS boxes.clear(); return boxes; }


    // #############################################################################################
    // Static Interface methods (default formatter)
    // #############################################################################################
        /// A publicly accessible static singleton instance which is can be freely used.
        /// Racing conditions in multithreaded applications have to be avoided by locking mutex
        /// #DefaultLock.
        ALIB_DLL
        static SPFormatter              Default;

        #if !ALIB_SINGLE_THREADED
            /// The lock to be set with multithreaded use of #Default.
            /// If the compiler-symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS is set, this lock will be
            /// attached to the \alib{lang;DbgCriticalSections} instance in #Default during
            /// bootstrap.
            /// Thus, an assertion will be raised if #Default is used without locking this.
            ///
            /// \par Availability
            ///   This object is available only if symbol \ref ALIB_SINGLE_THREADED is not set.
            ///
            /// @see Chapter \ref alib_threads_intro_assert_locks of the Programmer's Manual of
            ///      module \alib_threads.
            ALIB_DLL
            static threads::RecursiveLock   DefaultLock;
        #endif
    
    // #############################################################################################
    //  Protected methods
    // #############################################################################################
    protected:
        //==========================================================================================
        /// Virtual method which is invoked with each invocation of #Format.
        /// The default implementation does nothing.
        //==========================================================================================
        virtual void        initializeFormat()                                                    {}

        //==========================================================================================
        /// The abstract format method that needs to be implemented by descendants.
        /// Note that parameter \p{startIdx} and the demanded return value together comprise the
        /// possibility to use more than one formatter in parallel and to perform multiple format
        /// operations on suitable argument lists. This demands the implementation of this method to
        /// \b not copy the format string to the \p{target} in the case that no 'escape sequence'
        /// was found. For further information, see the general documentation of this class.
        ///
        /// @param target        The %AString that takes the result.
        /// @param formatString  The format string.
        /// @param args          The objects to be used with formatters.
        /// @param startArgument The first object in \p{args} to convert.
        ///
        /// @return The number of args consumed.
        //==========================================================================================
        virtual int         format( AString&         target,
                                    const String&    formatString,
                                    const BoxesMA&   args,
                                    int              startArgument )         = 0;

        //==========================================================================================
        /// The format loop implementation. Searches format strings in \p{args} and tests
        /// if \c this or #Next is capable of processing it.
        ///
        /// @param target     An AString that takes the result.
        /// @param args       The objects to be used with formatters.
        /// @return A reference to this formatter to allow concatenated operations.
        //==========================================================================================
        template<typename TAllocator>
        Formatter&      formatLoop( AString& target, const boxing::TBoxes<TAllocator>& args );
};

#if !DOXYGEN
template<> ALIB_DLL Formatter& Formatter::formatLoop( AString& target, const boxing::TBoxes<lang::HeapAllocator>& args );
template<> ALIB_DLL Formatter& Formatter::formatLoop( AString& target, const boxing::TBoxes<MonoAllocator>& args );
template<> ALIB_DLL Formatter& Formatter::formatLoop( AString& target, const boxing::TBoxes<PoolAllocator>& args );
#endif // !DOXYGEN


        inline
        Formatter&      Formatter::FormatArgs( AString& target )
        {
            ALIB_DCS
            return formatLoop( target, boxes );
        }

} // namespace alib[::format]

// #################################################################################################
// Alias types in namespace #alib.
// #################################################################################################
/// Type alias in namespace \b alib.
using  Formatter           =   format::Formatter;

} // namespace [alib]
