//==================================================================================================
/// \file
/// This header-file is part of module \alib_variables of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace variables {

//==================================================================================================
/// #"alib_enums_records;ALib Enum Record" associated with enumeration #"variables::Priority".
//==================================================================================================
struct ERPriority : enumrecords::ERSerializable {
    /// The precedence of an operator in respect to other binary operators.
    int         Priority;

    /// Required default constructor leaving the record undefined
    /// (Requirement is documented with #"EnumRecordPrototype".)
    ERPriority()                                                                  noexcept =default;

    using ERSerializable::ERSerializable;

    /// Implementation of #"EnumRecordPrototype::Parse;*".
    ALIB_DLL
    void Parse();
};

} // namespace alib::[config]

/// Type alias in namespace #"%alib".
using     Priority=       variables::Priority;

} // namespace [alib]

//##################################################################################################
// Macros
//##################################################################################################
ALIB_BOXING_VTABLE_DECLARE(   alib::variables::Priority, vt_config_priorities )
ALIB_ENUMS_MAKE_ARITHMETICAL( alib::variables::Priority  )
ALIB_ENUMS_ASSIGN_RECORD(     alib::variables::Priority, alib::variables::ERPriority  )
