//==================================================================================================
/// \file
/// This header-file is part of module \alib_variables of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace variables {

//==================================================================================================
/// \ref alib_enums_records "ALib Enum Record" associated with enumeration \alib{variables;Priority}.
//==================================================================================================
struct ERPriority : enumrecords::ERSerializable
{
    /// The precedence of an operator in respect to other binary operators.
    int         Priority;

    /// Required default constructor leaving the record undefined
    /// (Requirement is documented with\alib{enumrecords::EnumRecordPrototype}.)
    ERPriority()                                                                  noexcept =default;

    using ERSerializable::ERSerializable;

    /// Implementation of \alib{enumrecords;EnumRecordPrototype::Parse}.
    ALIB_DLL
    void Parse();
};

} // namespace alib::[config]

/// Type alias in namespace \b alib.
using     Priority=       alib::variables::Priority;

} // namespace [alib]

//##################################################################################################
// Macros
//##################################################################################################
ALIB_BOXING_VTABLE_DECLARE(   alib::variables::Priority, vt_config_priorities )
ALIB_ENUMS_MAKE_ARITHMETICAL( alib::variables::Priority  )
ALIB_ENUMS_ASSIGN_RECORD(     alib::variables::Priority, alib::variables::ERPriority  )
