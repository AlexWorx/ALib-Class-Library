// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

//  set include guard
#ifndef HPP_ALIB_STRINGS_BOXING_DEBUG
#define HPP_ALIB_STRINGS_BOXING_DEBUG 1

// #################################################################################################
// General debug methods for %ALib %Boxing
// #################################################################################################
#if ALIB_DEBUG  && ALIB_MODULE_BOXING

// Fix string construction warning raised by use with tuples
ALIB_STRING_CONSTRUCTOR_FIX( std::tuple<String ALIB_COMMA const std::type_info *>, char )

namespace aworx { namespace lib { namespace debug {

/** ************************************************************************************************
 * This class is only available in debug compilations of \alib and furthermore only if
 * <b>%ALib %Boxing</b> is used in an \alib module distribution that incorporates
 * module <b>%ALib Strings</b>.
 *
 * It uses the plain debug methods of classes \b %Box and \b Boxer that accept and return type
 * information structs \c std::typeinfo and converts them to strings and sorted lists of strings.
 **************************************************************************************************/
class Boxing
{
    public:

        /// If \c true, %ALib namespace names are removed from the strings.
        bool            removeNamespaces;

        /// A string that is used to collect and return information.
        AString         target;

        /// A string that is used to collect and return information.
        AString         dump;

        /// The return type of various methods of this class
        using           StringVector   =   std::vector<aworx::AString>;

        /// A vector of strings that is used to collect and return information.
        StringVector    strings;

    /** ********************************************************************************************
     * Constructor
     *
     * @param removeALibNamespaces   If \c true, %ALib namespace names are removed from the strings.
     **********************************************************************************************/
    ALIB_API
    Boxing( bool removeALibNamespaces = true )
    : removeNamespaces( removeALibNamespaces )
    {}

    /**
     * Writes information about the template type \p{TBoxable} in respect to how boxing of
     * objects (values) of this type are performed.
     *
     * \note
     *   Does not work for type \c void*, which is boxed and unboxed as identical type \c void*.
     *
     * @return A string containing the requested information.
     */
    template<typename TBoxable>
    AString&  GetBoxingInfo()
    {
        using TGiven= typename std::remove_const    <
                      typename std::remove_reference<TBoxable>::type>::type;

        target._()      << "Boxing information for type: " << typeid(TGiven) << NewLine;

        if( std::is_same<TGiven, decltype(nullptr)>::value )
            return target  << "  'decltype(nullptr)' not customizable" << NewLine;

        if( std::is_array<TGiven>::value )
        {
            using TAElem= typename std::remove_extent<TGiven>::type;
            return target   << "  Array Type (not customizable, boxed as array of "
                            << typeid( TAElem ) << ')' << NewLine;
        }

        if( !lib::boxing::T_Boxing<TGiven>::value )
        {
            if( std::is_pointer<TGiven>::value )
            {
                using TNP= typename std::remove_pointer<TGiven>::type;
                if( !lib::boxing::T_Boxing<TNP>::value )
                {
                    target  << "  Customized:      No" << NewLine
                            << "  Target Type:     "  << "Ident (" << typeid(TGiven) << ')' << NewLine
                            << "  Unboxable:       Yes";
                }
                else
                {
                    target  << "  Customized:      Yes" << NewLine
                            << "  Target Type:     " << typeid(typename lib::boxing::T_Boxing<TNP>::Type);
                            if( lib::boxing::T_Boxing<TNP>::IsArray() )
                                target << "[]";
                    target  << NewLine
                            << "  Custom Boxing:   "  << (lib::boxing::T_Boxing<TNP>::HasCustomBoxing() ? "Yes" : "No" )  << NewLine
                            << "  Unboxable:       No";
                }

            }
            else
            {
                using TP= TGiven*;
                if( !lib::boxing::T_Boxing<TP>::value )
                {
                    target  << "  Customized:      No" << NewLine
                            << "  Target Type:     "  << "As pointer type (" << typeid(TP) << ')' << NewLine
                            << "  Unboxable:       No (pointer type only)";
                }
                else
                {
                    target      << "  Customized:      Yes" << NewLine
                                << "  Target Type:     " << typeid(typename lib::boxing::T_Boxing<TP>::Type);
                    if( lib::boxing::T_Boxing<TP>::IsArray() )
                        target << "[]";

                    target      << NewLine
                                << "  Custom Boxing:   " << (lib::boxing::T_Boxing<TP>::HasCustomBoxing() ? "Yes" : "No" )
                                << NewLine
                                << "  Unboxable:       No";
                }
            }
        }
        else
        {
            target      << "  Customized:      Yes" << NewLine;
            target      << "  Target Type:     " << typeid(typename lib::boxing::T_Boxing<TGiven>::Type);
            if( lib::boxing::T_Boxing<TGiven>::IsArray() )
                target << "[]";

            target      << NewLine
                        << "  Custom Boxing:   " << (lib::boxing::T_Boxing<TGiven>::HasCustomBoxing() ? "Yes" : "No" )
                        << NewLine
                        << "  Unboxable:       ";

            if( lib::boxing::T_Boxing<TGiven>::IsUnboxable() )
            {
                target  << "Yes" << NewLine
                        << "  Custom Unboxing: " << (lib::boxing::T_Boxing<TGiven>::HasCustomUnboxing() ? "Yes" : "No" );
            }
            else
                target  << "No";
        }

        return target << NewLine;
    }


    /**
     * Writes the (demangled) string representation of the given type.
     * It is tried to identify a boxer type and infer the boxed type information from this
     * type.
     *
     * @param type            The type.
     * @return A string containing the requested information.
     */
    ALIB_API
    AString&      GetTypeName        ( const std::type_info &type );

    /**
     * Writes the type of the boxed value of \p{box}.
     *
     * @param box           The box object to get the type from.
     * @return A string containing the requested information.
     */
    static ALIB_API
    String64      GetType            ( const Box &box );

    /**
     * Returns a \b %StringBlock with a list of types received with
     * \alib{boxing,Boxing::DbgGetKnownBoxers}.
     * The uses #GetTypeName to tries to infer the boxed value types from the boxer types.)
     *
     * @return A list of demangled type names.
     */
    ALIB_API
    StringVector&  GetKnownBoxTypes                 ();

    /**
     * Returns a \b %StringBlock with a list of types received by
     * \alib{boxing,Boxing::DbgGetInterfaces} providing
     * the type information received with
     * \alib{boxing,Boxing::DbgGetInterfaces}.
     *
     * @param box                    The box object to get the type from.
     * @return A list of demangled type names.
     */
    ALIB_API
    StringVector&  GetInterfaces                    ( const Box &box );

    /**
     * Returns a \b %StringBlock with a list of types received with
     * \alib{boxing,Boxing::DbgGetKnownInterfaces}.
     *
     * @return A list of demangled type names.
     */
    ALIB_API
    StringVector&  GetKnownInterfaces               ();

    /**
     * Returns a \b %StringBlock with a list of types received with
     * \alib{boxing,Boxing::DbgGetDefaultInterfaces}.
     *
     * @return A list of demangled type names.
     */
    ALIB_API
    StringVector&  GetKnownInterfaceDefaults        ();

    /**
     * Returns a \b %StringBlock with a list of types received with
     * \alib{boxing,Boxing::DbgGetKnownInterfaceImpl}.
     *
     * @return A list of demangled type names.
     */
    ALIB_API
    StringVector&  GetKnownInterfaceImplementations ();


    /**
     * Uses methods #GetKnownBoxTypes, #GetInterfaces, #GetKnownInterfaces,
     * #GetKnownInterfaceDefaults and #GetKnownInterfaceImplementations to write
     * the current state of <b>%ALib %Boxing</b> to the given \p{target}.
     *
     * @return A string containing the dump.
     */
    ALIB_API
    AString&   Dump                            ();

    protected:
    /**
     * Internal helper to get human readable type name from boxer types.
     *
     * @param list     A list of types.
     */
    void  typesToStrings(const std::vector<const std::type_info *> &list);

    /** Internal helper method of #Dump.
     *
     * @param list      A list of types.
     * @param headline  The headline to write.
     * @param indent    Spaces to write prior to each entry.
     */
    void  dumpHelper    (const std::vector<const std::type_info *> &list, const String &headline, const String &indent );


}; // class


}}} // namespace [aworx::lib::debug]
#endif

#endif // HPP_ALIB_STRINGS_BOXING_DEBUG
