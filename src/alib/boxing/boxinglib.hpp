// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen


// set include guard
#ifndef HPP_ALIB_BOXING_LIB
#define HPP_ALIB_BOXING_LIB 1

#if !defined(ALIB_PROPER_INCLUSION)
    #error "include alib/alib.hpp instead of this header"
#endif


#if !defined(_STRING_H) && !defined(_INC_STRING)
    #include <string.h>
#endif
// #################################################################################################
// Init()
// #################################################################################################
namespace aworx { namespace lib { namespace boxing {

class Boxer;
class Box;
class Interface;

/** ************************************************************************************************
 * The library class for namespace \ref aworx::lib::boxing.
 **************************************************************************************************/
class Boxing
#if ALIB_MODULE_STRINGS
    : public lang::Library
#endif
{
    friend class Boxer;   ///< Friend class. Need access for generation of debug information.
    friend class Box;     ///< Friend class. Need access for generation of debug information.

    // #############################################################################################
    // internals
    // #############################################################################################
    protected:

    // #############################################################################################
    // Constructor
    // #############################################################################################
    public:
        inline  Boxing()
#if ALIB_MODULE_STRINGS
                : lang::Library( ALIB_VERSION, ALIB_REVISION, ASTR("ALIB_BOXING") )
#endif
        {}


    // #############################################################################################
    // Interface methods
    // #############################################################################################
    public:


    // #############################################################################################
    // Debug methods
    // #############################################################################################
    #if ALIB_DEBUG

    protected:
        /// Map of boxers. Available only in debug compilations.
        TypeMap<Boxer*>                     dbgKnownBoxers;

        /// Map of box interfaces. Available only in debug compilations.
        TypeMap<Interface*>                 dbgKnownInterfaces;

        /// Map of box interface implementations. Available only in debug compilations.
        TypeMap<Interface*>                 dbgKnownInterfaceImpl;

        /** Internal method doing checks in debug compilation version of the library.
         * @param boxer  The boxer to check.                                            */
        ALIB_API
        void                                dbgCheckNewBoxer( Boxer* boxer );

        /**
         * Internal method doing checks in debug compilation version of the library.
         * @param ifType The interface to check.
         */
        ALIB_API
        void                                debugCheckInterfaceExists( const std::type_info& ifType );


    public:
        /**
         * Collects all interface specializations available for the given boxed type.
         * Available only in debug compilations.
         * @param boxerType Type information on the boxer that represents the boxed type.
         * @return A vector containing type information structs.
         */
        ALIB_API
        std::vector<const std::type_info*>  DbgGetInterfaces( const std::type_info& boxerType );

        /**
         * Collects type information on used boxer singletons.
         * Available only in debug compilations.
         * @return A vector containing type information structs.
         */
        ALIB_API
        std::vector<const std::type_info*>  DbgGetKnownBoxers();

        /**
         * Collects type information on registered interfaces.
         * Available only in debug compilations.
         * @return A vector containing type information structs.
         */
        ALIB_API
        std::vector<const std::type_info*>  DbgGetKnownInterfaces();

        /**
         * Collects type information on registered default interfaces.
         * Available only in debug compilations.
         * @return A vector containing type information structs.
         */
        ALIB_API
        std::vector<const std::type_info*>  DbgGetDefaultInterfaces();

        /**
         * Collects type information on registered interface implementations.
         * Available only in debug compilations.
         * @return A vector containing type information structs.
         */
        ALIB_API
        std::vector<const std::type_info*>  DbgGetKnownInterfaceImpl();

    #endif


    // #############################################################################################
    // Internal methods
    // #############################################################################################
    protected:
    #if ALIB_MODULE_STRINGS
        /** ****************************************************************************************
         * Initializes the <b>%ALib %Boxing</b> system.
         *
         * The method performs the following initialization steps:
         * - Adds default interface class \ref aworx::lib::boxing::IEquals "IEquals"
         * - Adds default interface class \ref aworx::lib::boxing::IEquals "IIsNull"
         * - Adds default interface class \ref aworx::lib::boxing::IEquals "IIsEmpty"
         *
         * If code selection symbol #ALIB_FEAT_BOXING_FTYPES is \c 1, then in addition,
         * - implementations of interface \alib{boxing,IEquals} for fundamental
         *   types \c double, \c char, \c wchar_t and \ref aworx::strangeChar, and
         * - implementations of interface \alib{boxing,IIsNull} for fundamental
         *   types \c boxed_int, \c boxed_uint and \c double,
         *
         * are registered.
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void        init( Phases phase )                                           override;

        /** ****************************************************************************************
         * De-initializes <b>%ALib %Boxing</b> system. This is (as of today) not necessary, but
         * might be in the future.
         * Also, some valgrind warnings might disappear when this is invoked properly.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;
    #else
        // in single module distribution, no library class is available
        void        Init();
        void        TerminationCleanUp();

    #endif

}; // class boxing::Library

} // namespace aworx::lib[::boxing]

/** ************************************************************************************************
 * The static library singleton of namespace #aworx::lib::boxing.
 **************************************************************************************************/
ALIB_API
extern boxing::Boxing BOXING;
}} // namespace [aworx::lib]


/**
 * @addtogroup GrpALibCompilerSymbols
 * @{
 *  \def  ALIB_FEAT_BOXING_FTYPES_OFF
 *    This symbol may be passed to the compiler to define code selection symbol
 *    \ref ALIB_FEAT_BOXING_FTYPES to be \c 0.
 *
 *    See \ref ALIB_FEAT_BOXING_FTYPES for more information.
 *
 *  \def  ALIB_FEAT_BOXING_FTYPES_ON
 *    This symbol may be passed to the compiler to define code selection symbol
 *    \ref ALIB_FEAT_BOXING_FTYPES to be \c 1. However, as this is the default value, this symbol
 *    does not need to defined.
 *
 *    See \ref ALIB_FEAT_BOXING_FTYPES for more information.
 * @}
 *
 * @addtogroup GrpALibCodeSelectorSymbols
 * @{
 *  \def  ALIB_FEAT_BOXING_FTYPES
 *    Selects code for boxing fundamental types. When using <b>%ALib %Boxing</b> as a single module,
 *    built-in  fundamental type boxing may be disabled and replaced by a custom implementation.
 *    When using \alibmod_nolink_boxing in combination with \alibmod_strings, such disabling is not
 *    allowed as the formatter classes rely on the default implementation. The same is true for
 *    other parts of \alib.
 *
 *    Use \ref ALIB_FEAT_BOXING_FTYPES_OFF to set this symbol to \c 0 and disable default fundamental types
 *    boxing provided by \alib.
 *
 *    If built-in fundamental type boxing is suppressed, then either fundamental types are not boxable
 *    or boxing of fundamental types is implemented by the user of the library.
 *    The typical reason for providing a custom  implementation for boxing fundamental types is that
 *    the default implementation provided with \alib, is not \b injective in a mathematical sense.
 *
 *    \see
 *    Documentation of namespace \ref aworx::lib::boxing::ftypes for more information.
 * @}
 */
#if defined(DOX_PARSER)
    #define ALIB_FEAT_BOXING_FTYPES
    #define ALIB_FEAT_BOXING_FTYPES_ON
    #define ALIB_FEAT_BOXING_FTYPES_OFF
#else
    #if defined(ALIB_FEAT_BOXING_FTYPES)
        #error "ALIB_FEAT_BOXING_FTYPES must not be set from outside. Use ALIB_FEAT_BOXING_FTYPES_ON/OFF instead!"
    #endif

    #if defined(ALIB_FEAT_BOXING_FTYPES_ON) && defined(ALIB_FEAT_BOXING_FTYPES_OFF)
        #error "Contradiction in compiler symbols: ALIB_FEAT_BOXING_FTYPES_ON and ALIB_FEAT_BOXING_FTYPES_OFF set!"
    #endif

    #if defined(ALIB_FEAT_BOXING_FTYPES_OFF)
        #define ALIB_FEAT_BOXING_FTYPES   0
    #else
        #define ALIB_FEAT_BOXING_FTYPES   1
    #endif
#endif //DOX_PARSER


/**
 * @addtogroup GrpALibCompilerSymbols
 * @{ \def  ALIB_FEAT_BOXING_STD_VECTOR_OFF
 *  This symbol may be passed to the compiler to define code selection symbol
 *  \ref ALIB_FEAT_BOXING_STD_VECTOR to be \c 0.
 *
 *  See \ref ALIB_FEAT_BOXING_STD_VECTOR for more information.
 * @}
 *
 * @addtogroup GrpALibCompilerSymbols
 * @{ \def  ALIB_FEAT_BOXING_STD_VECTOR_ON
 *  This symbol may be passed to the compiler to define code selection symbol
 *  \ref ALIB_FEAT_BOXING_STD_VECTOR to be \c 1. However, as this is the default value, this symbol
 *  does not need to defined.
 *
 *  See \ref ALIB_FEAT_BOXING_STD_VECTOR for more information.
 * @}
 *
 * @addtogroup GrpALibCodeSelectorSymbols
 * @{ \def  ALIB_FEAT_BOXING_STD_VECTOR
 *  Selects code for boxing type <c>std::vector<T, std::allocator<T>></c> as plain
 *  C++ array types "<c>T[]</c>".<br>
 *  Use \ref ALIB_FEAT_BOXING_STD_VECTOR_OFF to set this symbol to \c 0 and disable this default
 *  behaviour. If switched off, the default boxing of custom types applies, which means that values
 *  and pointers of type \c std::vector  are boxed as pointers to \c std::vector object given.
 *
 *  \see
 *    Namespace documentation chapter \ref alib_namespace_boxing_types_arrays "4.1 Boxing Arrays and Vectors".
 *
 * @}
 */
#if defined(DOX_PARSER)
    #define ALIB_FEAT_BOXING_STD_VECTOR
    #define ALIB_FEAT_BOXING_STD_VECTOR_ON
    #define ALIB_FEAT_BOXING_STD_VECTOR_OFF
#else
    #if defined(ALIB_FEAT_BOXING_STD_VECTOR)
        #error "ALIB_FEAT_BOXING_STD_VECTOR must not be set from outside. Use ALIB_FEAT_BOXING_STD_VECTOR_ON/OFF instead!"
    #endif

    #if defined(ALIB_FEAT_BOXING_STD_VECTOR_ON) && defined(ALIB_FEAT_BOXING_STD_VECTOR_OFF)
        #error "Contradiction in compiler symbols: ALIB_FEAT_BOXING_STD_VECTOR_ON and ALIB_FEAT_BOXING_STD_VECTOR_OFF set!"
    #endif

    #if defined(ALIB_FEAT_BOXING_STD_VECTOR_OFF)
        #define ALIB_FEAT_BOXING_STD_VECTOR   0
    #else
        #define ALIB_FEAT_BOXING_STD_VECTOR   1
    #endif
#endif //DOX_PARSER


/**
 * @addtogroup GrpALibMacros
 * @{
 * @name Macros Supporting ALib Boxing
 * @{
 * \def  ALIB_BOXING_SPECIALIZE
 *
 * Macro used to specialize template struct \b %T_Boxing. Specializations
 * of the struct allow the customization of the way C++ types are mapped to boxed types.<br>
 * This version of the macro is used for custom types that do not need custom methods for
 * boxing and unboxing the value.
 * \see Variants \ref ALIB_BOXING_SPECIALIZE_CB, \ref ALIB_BOXING_SPECIALIZE_CUB and
 *      \ref ALIB_BOXING_SPECIALIZE_CB_CUB as well as default implementation of struct
 *      \ref aworx::lib::boxing::T_Boxing.
 *
 * @param TSrc               The C++ 'source' type to specialize struct aworx::lib::boxing::T_Boxing for.
 * @param TTarget            The target type to box values of type \p{TSrc} to. In case that macro
 *                           parameter \p{isArray} is true, this parameter denotes the element type of
 *                           the array.
 * @param isArray            Used as return value of \ref aworx::lib::boxing::T_Boxing::IsArray           "T_Boxing<TSrc>::IsArray".
 * @param isUnboxable        Used as return value of \ref aworx::lib::boxing::T_Boxing::IsUnboxable       "T_Boxing<TSrc>::IsUnboxable".
 *
 *
 * \def  ALIB_BOXING_SPECIALIZE_CB
 *
 * Variant of macro \ref ALIB_BOXING_SPECIALIZE used for custom types that needs a custom method for
 * boxing the value, but no custom method for unboxing.
 *
 * Method
 * \ref aworx::lib::boxing::T_Boxing::Boxing   "T_Boxing<TSrc>::Boxing" is declared and an
 * implementation has to be given when this macro is used.
 *
 *
 * @param TSrc               The C++ 'source' type to specialize struct aworx::lib::boxing::T_Boxing for.
 * @param TTarget            The target type to box values of type \p{TSrc} to. In case that macro
 *                           parameter \p{isArray} is true, this parameter denotes the element type of
 *                           the array.
 * @param isArray            Used as return value of \ref aworx::lib::boxing::T_Boxing::IsArray           "T_Boxing<TSrc>::IsArray".
 * @param isUnboxable        Used as return value of \ref aworx::lib::boxing::T_Boxing::IsUnboxable       "T_Boxing<TSrc>::IsUnboxable".
 *
 *
 * \def  ALIB_BOXING_SPECIALIZE_CUB
 *
 * Variant of macro \ref ALIB_BOXING_SPECIALIZE used for custom types that needs a custom method for
 * unboxing the value, but no custom method for boxing.
 *
 * Method
 * \ref aworx::lib::boxing::T_Boxing::Unboxing "T_Boxing<TSrc>::Unboxing" is declared and an
 * implementation has to be given when this macro is used.
 *
 *
 * @param TSrc               The C++ 'source' type to specialize struct aworx::lib::boxing::T_Boxing for.
 * @param TTarget            The target type to box values of type \p{TSrc} to. In case that macro
 *                           parameter \p{isArray} is true, this parameter denotes the element type of
 *                           the array.
 * @param isArray            Used as return value of \ref aworx::lib::boxing::T_Boxing::IsArray           "T_Boxing<TSrc>::IsArray".
 * @param isUnboxable        Used as return value of \ref aworx::lib::boxing::T_Boxing::IsUnboxable       "T_Boxing<TSrc>::IsUnboxable".
 *
 *
 * \def  ALIB_BOXING_SPECIALIZE_CB_CUB
 *
 * Variant of macro \ref ALIB_BOXING_SPECIALIZE used for custom types that needs custom methods for
 * boxing and unboxing the value.
 *
 * Methods
 * \ref aworx::lib::boxing::T_Boxing::Boxing   "T_Boxing<TSrc>::Boxing" and
 * \ref aworx::lib::boxing::T_Boxing::Unboxing "T_Boxing<TSrc>::Unboxing" are declared and an
 * implementation has to be given when this macro is used.
 *
 * @param TSrc               The C++ 'source' type to specialize struct aworx::lib::boxing::T_Boxing for.
 * @param TTarget            The target type to box values of type \p{TSrc} to. In case that macro
 *                           parameter \p{isArray} is true, this parameter denotes the element type of
 *                           the array.
 * @param isArray            Used as return value of \ref aworx::lib::boxing::T_Boxing::IsArray           "T_Boxing<TSrc>::IsArray".
 * @param isUnboxable        Used as return value of \ref aworx::lib::boxing::T_Boxing::IsUnboxable       "T_Boxing<TSrc>::IsUnboxable".
 *
 *
 * \def  ALIB_BOXING_DEFINE_IEQUALS_FOR_COMPARABLE_TYPE
 *
 * This macro creates a singleton object of templated boxing interface class
 * \alib{boxing,IEquals_TComparable} for the given type and passes it to namespace function
 * \ref aworx::lib::boxing::DefineInterface.
 *
 * The macro is to be placed in the bootstrap section of an application, for any type that
 * disposes of an implementation of C++ <c>operator==</c>. This operator will be used
 * for implementing boxing interface \alib{boxing,IEquals}.
 *
 * \note
 *   As by default, custom types get boxed as pointers, the type in question usually has to
 *   be provided here as pointer type, for example:
 *
 *          ALIB_BOXING_DEFINE_IEQUALS_FOR_COMPARABLE_TYPE( my_namespace::MyType* )
 *
 *
 * @param TComparable  The comparable, usually given as pointer type.
 *
 *
 * \def  ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE
 *
 * This macro creates a singleton object of templated boxing interface class
 * \alib{strings::boxing,IApply_TApplicable} for the given type and passes it to namespace function
 * \ref aworx::lib::boxing::DefineInterface.
 *
 * The macro is to be placed in the bootstrap section of an application, for any type that is
 * \ref aworx::lib::strings::T_Apply "applicable" to class \b %AString.
 *
 * This macro selects the \ref aworx::character version of \b %IApply_TApplicable. If a custom type
 * is explicitly applicable to both, \b %NAString and \b %WAString types, then macros
 * - \ref ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE_N and
 * - \ref ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE_W should be both used instead of this macro.
 *
 * \note
 *   As by default, custom types get boxed as pointers, the type in question usually has to be
 *   provided here as pointer type, for example:
 *
 *          ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE( my_namespace::MyType* )
 *
 *
 * \def  ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE_N
 *
 * This macro creates a singleton object of templated boxing interface class
 * \alib{strings::boxing,IApply_TApplicable} for the given type and passes it to namespace function
 * \ref aworx::lib::boxing::DefineInterface.
 *
 * The macro is to be placed in the bootstrap section of an application, for any type that is
 * \ref aworx::lib::strings::T_Apply "applicable" to class \b %AString.
 *
 * This macro selects the \c char version of \b %IApply_TApplicable.<br>
 * If a custom type is applicable to the general, compilation configuration dependent character
 * type \c aworx::character, then macro
 * \ref ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE should be used instead.<br>
 * If a custom type is applicable to both character types, then this macro should be used in
 * parallel to macro \ref ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE_W.
 *
 *
 * @param TApplicable  The applicable type, usually given as pointer.
 *
 *
 * \def  ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE_W
 *
 * This macro creates a singleton object of templated boxing interface class
 * \alib{strings::boxing,IApply_TApplicable} for the given type and passes it to namespace function
 * \ref aworx::lib::boxing::DefineInterface.
 *
 * The macro is to be placed in the bootstrap section of an application, for any type that is
 * \ref aworx::lib::strings::T_Apply "applicable" to class \b %AString.
 *
 * This macro selects the \c wchar_t version of \b %IApply_TApplicable.<br>
 * If a custom type is applicable to the general, compilation configuration dependent character
 * type \c aworx::character, then macro
 * \ref ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE should be used instead.<br>
 * If a custom type is applicable to both character types, then this macro should be used in
 * parallel to macro \ref ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE_N.
 *
 * @param TApplicable  The applicable type, usually given as pointer.
 *
 * @}
 * @}
 */
#define ALIB_BOXING_SPECIALIZE(TSrc, TTarget, isArray, isUnboxable)                                   \
template<>  struct T_Boxing<TSrc>    : std::true_type                                                 \
{                                                                                                     \
    using Type= TTarget;                                                                              \
    static constexpr bool IsArray()                             { return isArray;          }          \
    static constexpr bool IsUnboxable()                         { return isUnboxable;      }          \
    static constexpr bool HasCustomBoxing()                     { return false;            }          \
    static constexpr bool HasCustomUnboxing()                   { return false;            }          \
    static void           Boxing(Box&,  TSrc const&)            {}                                    \
    static TSrc&          Unboxing(const Box& )                 { return *reinterpret_cast<TSrc*>(1);}\
};

#define ALIB_BOXING_SPECIALIZE_CB(TSrc, TTarget, isArray, isUnboxable)                                \
template<>  struct T_Boxing<TSrc>    : std::true_type                                                 \
{                                                                                                     \
    using Type= TTarget;                                                                              \
    static constexpr bool IsArray()                             { return isArray;          }          \
    static constexpr bool IsUnboxable()                         { return isUnboxable;      }          \
    static constexpr bool HasCustomBoxing()                     { return true;             }          \
    static constexpr bool HasCustomUnboxing()                   { return false;            }          \
    static void           Boxing(Box& box,  TSrc const& value);                                       \
    static void           Unboxing(const Box& )                 {}                                    \
};

#define ALIB_BOXING_SPECIALIZE_CUB(TSrc, TTarget, isArray, isUnboxable)                               \
template<>  struct T_Boxing<TSrc>    : std::true_type                                                 \
{                                                                                                     \
    using Type= TTarget;                                                                              \
    static constexpr bool IsArray()                             { return isArray;          }          \
    static constexpr bool IsUnboxable()                         { return isUnboxable;      }          \
    static constexpr bool HasCustomBoxing()                     { return false;            }          \
    static constexpr bool HasCustomUnboxing()                   { return true;             }          \
    static void           Boxing(Box&,  TSrc const&)            {}                                    \
    static TSrc           Unboxing(const Box& box);                                                   \
};

#define ALIB_BOXING_SPECIALIZE_CB_CUB(TSrc, TTarget, isArray, isUnboxable)                            \
template<>  struct T_Boxing<TSrc>    : std::true_type                                                 \
{                                                                                                     \
    using Type= TTarget;                                                                              \
    static constexpr bool IsArray()                             { return isArray;          }          \
    static constexpr bool IsUnboxable()                         { return isUnboxable;      }          \
    static constexpr bool HasCustomBoxing()                     { return true;             }          \
    static constexpr bool HasCustomUnboxing()                   { return true;             }          \
    static void           Boxing(Box& box,  TSrc const& value);                                       \
    static TSrc           Unboxing(const Box& box);                                                   \
};

namespace aworx { namespace lib { namespace boxing  {

// #################################################################################################
// Forward declarations
// #################################################################################################
template<typename TBoxable>  inline  TBoxable      DefaultUnboxing( const Box& box );
template<typename TBoxable>  inline  void          DefaultBoxing(Box &, const TBoxable&);


// #################################################################################################
// Class BoxData
// #################################################################################################

/**
 * This is the type used for box data values.
 */
using boxvalue= uint64_t;


/**
 * This structs holds a boxed objects' value and length.
 */
struct BoxData
{
    /**
     * The value of the box. This is value might is fully type specific and set and retrieved
     *  using 'heavy' reinterpretation of its type. Mostly the value is accessed using
     *  template meta programming. Hence type #boxvalue can be considered as a union type.
     */
    boxvalue            Value;

    /// The length. Applicable for array types (and optionally custom types)
    integer             Length;

    /**
     * Trivial default constructor.
     */
    BoxData() = default;

    /**
     * Constructor providing both values.
     * @param value  The value.
     * @param length The array length. Defaults to 0.
     */
    inline constexpr
    BoxData( boxvalue value, integer length = 0 )
    : Value ( value  )
    , Length( length )
    {}
};


// #################################################################################################
// Template function to return a type-specific Boxer singleton (to be specialized for custom types).
// #################################################################################################


#if !defined(DOX_PARSER)
    ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING
#endif

/**
 * This template struct is used to define customized behaviour for boxing values of type
 * \p{TBoxable}.
 *
 * If this class is \b not specialized for template type \p{TBoxable}, then default
 * boxing applies. By default values and references of a type are boxed to pointers of the type,
 * while pointer types remain such pointer types.
 *
 * To specialize the class for a custom type, the following changes have to be made
 * - The specialization has to be derived from \c std::true_type.
 * - Method #IsArray has to be implemented to return \c true if the type should be boxed
 *   as an array type
 * - %Type definition #Type needs to use the boxed type. In case of array boxing, the
 *   array element type.
 * - Method #IsUnboxable has to return \c false if the type should not be unboxable.
 * - Methods #HasCustomBoxing and HasCustomUnboxing have to be implemented to return either
 *   \c true or false.
 * - Methods #Boxing and #Unboxing have to be declared. The also need to be implemented
 *   if #HasCustomBoxing, respectively #HasCustomUnboxing return \c true.
 *
 * \see Macro \ref ALIB_BOXING_SPECIALIZE which provides a simple method
 * to defined specializations of this class.
 *
 * @tparam TBoxable  The type to customize boxing for.
 */
template<typename TBoxable>  struct T_Boxing    : std::false_type
{
    /**
     * If \c true is returned, \p{TBoxable} is boxed as an array type. In this case alias type
     * #Type needs to be set to denote the element type of the array.
     * @return \c true if boxed type is an array type, \c false otherwise.
     */
    static constexpr bool IsArray()   { return false; }

    /**
     * This flag is asserted with keyword \c static_assert when invoking methods
     * \ref aworx::lib::boxing::Box::IsType "Box::IsType" and
     * \ref aworx::lib::boxing::Box::Unbox  "Box::Unbox". This way, types that are not meant to
     * be unboxed produce an error.
     *
     * @return \c true if boxed type can be unboxed, \c false otherwise.
     */
    static constexpr bool IsUnboxable() { return true; }


    /**
     * If returns \c true, the specialized method #Boxing will be invoked for boxing a value of
     * type \p{TBoxable}. Otherwise, default function
     * \ref aworx::lib::boxing::DefaultBoxing will be used and method #Boxing does not need
     * to be implemented.
     *
     * @return \c true if custom boxing implementation is to be used.
     */
    static constexpr bool HasCustomBoxing()  { return false; }

    /**
     * If returns \c true, the specialized method #Unboxing will be invoked for boxing a value of
     * type \p{TBoxable}. Otherwise, default function
     * \ref aworx::lib::boxing::DefaultUnboxing will be used and method #Unboxing does not need
     * to be implemented.
     *
     * @return \c true if custom implementation for unboxing is to be used.
     */
    static constexpr bool HasCustomUnboxing(){ return false; }

    /**
     * Optional custom implementation of boxing a value.
     * Fields
     * \ref aworx::lib::boxing::BoxData::Value  "BoxData::Value" and
     * \ref aworx::lib::boxing::BoxData::Length "BoxData::Length" have to be set, because for
     * efficiency reasons, both are not initialized otherwise.
     *
     * If this method is not specialized, template function
     * \ref aworx::lib::boxing::DefaultBoxing "DefaultBoxing" us used.
     *
     * @param box   The destination box
     * @param value The value to box
     */
    static void         Boxing(Box& box,  TBoxable const & value)
    {
        (void) box;
        (void) value;
        assert(0);
    }

    /**
     * Optional custom implementation of unboxing a value.
     *
     * \note
     *   Specializations of this class usually will return type \c TBoxable instead of
     *   \c TBoxable&. The reason why this method here is declared to return a reference, is of
     *   pure technical (TMP) nature.
     *
     * If method #HasCustomUnboxing is specialized to return \c false, only the declaration
     * of this method is needed, because namespace function
     * \ref aworx::lib::boxing::DefaultUnboxing "DefaultUnboxing" is used in this case.
     * However, while the declaration is still necessary, the implementation is not.
     *
     * @param box   The box object to unbox the data from
     * @return \c The unboxed object.
     */
    static TBoxable&     Unboxing(const Box& box)
    {
        (void) box;
        assert(0);
        return *((TBoxable*) 1);
    }

    /**
     * Needs to be using the mapped of template parameter \p{TBoxable}.
     */
    using Type= TBoxable;
};

/**
 * This function is invoked by the constructor of class
 * \ref aworx::lib::boxing::Box "Box" to determine the length of an array.
 *
 * Prior to the invocation, the length of the array provided, is inferred from the type using TMP
 * and passed as a parameter to this method.
 *
 * Normally, this method should not be specialized. The whole rational to do this exercise is to
 * prepare the right handling of literal string arrays. Those are including a terminating
 * \c 0 character (added by the compiler) and hence their boxing implementation might deduct
 * \c 1. As a sample, the standard implementation of boxing fundamental types, provided with
 * namespace #aworx::lib::boxing::ftypes (optionally replaced by a custom implementation of
 * fundamental types boxing) uses this feature. This way, other string types (e.g. of various widely used
 * libraries) can be boxed as a character array using their right string length.
 *
 * @tparam TPlain      The value type of the array (e.g. \c int, \c char or \c MyType) excluding
 *                     const/volatile and array extends.
 * @param arrayExtent  The array extent to set.
 * @returns The length as given by \p{arrayExtent}. Specializations of this method may return
 *          something different.
 */
template<typename TPlain>
inline constexpr   integer   T_SetArraySize( integer arrayExtent )
{
    return arrayExtent;
}


#if !defined(DOX_PARSER)
ALIB_WARNINGS_RESTORE
#endif


}}} // namespace lib::boxing


#include "alib/boxing/interface.hpp"
#include "alib/boxing/boxer.hpp"
#include "alib/boxing/box.hpp"
#include "alib/boxing/boxes.hpp"
#include "alib/boxing/interface_builtin.hpp"


// #################################################################################################
// Default implementation of T_Unboxing / T_Boxing
// #################################################################################################

namespace aworx { namespace lib { namespace boxing {

#if !defined(DOX_PARSER)
    ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING
#endif
/**
 * This template function provides the default implementation used to store values in struct
 * \ref aworx::lib::boxing::BoxData "BoxData" when they are placed in a
 * \ref aworx::lib::boxing::Box "Box".
 *
 * To customize boxing for a type, specialize struct
 * \ref aworx::lib::boxing::T_Boxing (with the help of macro \ref ALIB_BOXING_SPECIALIZE) with
 * implementing  methods
 * \ref aworx::lib::boxing::T_Boxing::HasCustomBoxing  "T_Boxing::HasCustomBoxing" to return
 * \c true and having
 * \ref aworx::lib::boxing::T_Boxing::Boxing  "T_Boxing::Boxing" to do the boxing.
 *
 * Fields
 * \ref aworx::lib::boxing::BoxData::Value  "BoxData::Value" and
 * \ref aworx::lib::boxing::BoxData::Length "BoxData::Length" have to be set, because for
 * efficiency reasons, both are not initialized otherwise.
 *
 * However, specializing this method for custom boxable types is seldom necessary, because
 * complex custom types (structs and classes) usually get announced to the \alib boxing system as
 * pointer types.
 * The default implementation of this function, as well as corresponding function
 * \ref aworx::lib::boxing::DefaultUnboxing "DefaultUnboxing", are using \c memcpy for reading
 *  and writing the value. This is necessary to avoid de-referencing type-punned pointers which
 * breaks the strict-aliasing rule when compiling the code with higher optimization levels.
 *
 * Typically, a custom version of boxing is needed for fundamental types (value types), when a type
 * is stored as a different type in a box. For example, custom string types might be stored as
 * pointer to the array of characters and the string length.
 * The advantage is, that this way all custom string classes are  stored in the same way, and code
 * that is processing a \b %Box does not need to check for more than one type.
 *
 * See \ref aworx::lib::boxing "documentation of \b %%ALib %Boxing" for more information.
 *
 * @param box    The \b %Box to set the data vor
 * @param value  The 'native' data value
 */
template<typename TBoxable>  inline  void     DefaultBoxing(Box& box, const TBoxable& value)

{
    if( sizeof(TBoxable) < sizeof(boxvalue) ) // clear value if the data is smaller
        box.data.Value= 0;
    ALIB_ASSERT_ERROR(sizeof(value) <= sizeof(box.data.Value),
                 ASTR("Boxed type bigger than size of type boxvalue. Use custom boxing method?") );
    memcpy( &box.data.Value, &value, sizeof(value) );

    box.data.Length= 0;
}

/**
 * This template function provides the default implementation used to retrieve values from struct
 * \ref aworx::lib::boxing::BoxData "BoxData" from a
 * \ref aworx::lib::boxing::Box "Box".
 *
 * To customize unboxing for a type, specialize struct
 * \ref aworx::lib::boxing::T_Boxing (with the help of macro \ref ALIB_BOXING_SPECIALIZE) with
 * implementing  methods
 * \ref aworx::lib::boxing::T_Boxing::HasCustomUnboxing  "T_Boxing::HasCustomUnboxing" to return
 * \c true and having
 * \ref aworx::lib::boxing::T_Boxing::Unboxing  "T_Boxing::Unboxing" to do the boxing.
 *
 * See \ref aworx::lib::boxing::T_Boxing "T_Boxing" for more information.
 *
 * @param box  The \b %Box to get the data of.
 * @returns The value of the boxed data converted to template type \p{TBoxable}.
 */
template<typename TBoxable> inline  TBoxable     DefaultUnboxing( const Box& box )
{
    typename std::aligned_storage<sizeof(TBoxable), alignof(TBoxable)>::type result;
    memcpy( &result, &box.data.Value, sizeof(TBoxable) );
    return * (TBoxable*) &result;
}

// #################################################################################################
// class BoxedAs and its T_Boxing struct
// #################################################################################################

/**
 * This is a simple wrapper class storing value references of arbitrary types. For this class,
 * template struct \ref aworx::lib::boxing::T_Boxing is defined to store objects of this class
 * as references.
 *
 * The purpose of this class is to provide a mechanism to box a type "as is", hence to suppress
 * the conversion of types when they are boxed.
 *
 * As a sample, with the use of default, built-in
 * \ref alib_namespace_boxing_types_ftypes "boxing of C++ fundamental types", \c float values
 * are boxed as double. If this conversion should be suppressed, the value can be wrapped in this
 * class as shown as follows:
 *
 * \snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_BOXEDAS_2
 *
 * With method \c useBox defined like this:
 *
 * \snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_BOXEDAS
 *
 * The output will be:
 *
 * \snippet "DOX_ALIB_BOXING_SAMPLE_BOXEDAS.txt"     OUTPUT
 *
 *
 * \see For more information see %boxing manual chapter
 *      \ref alib_namespace_boxing_types_boxedas "4.5 Boxing Types As They Are".
 *
 * @tparam Type The type to wrap.
 */
template<typename Type>
class BoxedAs
{
    protected:
        /// The value stored
        Type&     value;

    public:
        /**
         * Constructor storing the data. Removes the \c const specifier from
         * the given parameter.
         * @param val The value of type \p{Type} to store
         */
        BoxedAs( const Type& val)  :value(const_cast<Type&>(val))
        {
        }

        /**
         * Constructor accepting a pointer. The pointer will be dereferenced, hence \c nullptr
         * must not be passed.
         *
         * @param val The value of type \p{Type}* to store. Must not be \c nullptr.
         */
        BoxedAs( Type* val)  :value(*val) {}


        /**
         * Returns the wrapped value
         * @return The value stored.
         */
        Type&     Get() {  return value; }
};


/**
 * Templated specialization of template struct \ref aworx::lib::boxing::T_Boxing "T_Boxing"
 * for any (templated) type
 * \ref aworx::lib::boxing::BoxedAs "BoxedAs<TBoxed>".
 *
 * The specialization of this struct (for all template types) changes the default behavior
 * of boxing pointer types to boxing copies of \b BoxedAs.
 */
template<typename TBoxed>
struct T_Boxing<BoxedAs<TBoxed>>    : std::true_type
{
    /// The boxed type.
    using Type= BoxedAs<TBoxed>;

    /**
     * Not an array.
     * @return Constant \c false.
     */

    static constexpr bool IsArray()                                { return false; }

    /**
     * Not unboxable. \b %AString* is unboxable instead.
     * @return Constant \c false.
     */
    static constexpr bool IsUnboxable()                            { return true; }

    /**
     * No custom boxing.
     * @return Constant \c false.
     */
    static constexpr bool HasCustomBoxing()                        { return false; }

    /**
     * No custom unboxing.
     * @return Constant \c false.
     */
    static constexpr bool HasCustomUnboxing()                      { return false; }

    /**
     * This method is never called, because \b HasCustomBoxing returns constant \c false.
     */
    static void           Boxing(Box&,  const BoxedAs<TBoxed> & )
    {
        // never called
        assert(0);
    }

    /** This method is never called, because \b %HasCustomUnboxing returns constant \c false.
     * @return Not implemented.
     */
    static BoxedAs<TBoxed>  Unboxing(const Box&)
    {
        // never called
        assert(0);
        return nullptr;
    }
};


// #################################################################################################
// namespace functions  DefineInterface/DefineDefaultInterface
// #################################################################################################

/**
 * This namespace function allows the definition of invokable interfaces for boxed types.
 * Three template parameters have to be given. The first, \p{TBoxed}, defines the
 * boxed type that an interface method should be 'registered' with. \p{IsArray}
 * denotes if \p{TBoxed} is an array type or not. Together both parameters are used to
 * identify the right singleton of a derived \b %Boxer class, on which method
 * \ref aworx::lib::boxing::Boxer::DefineInterface "Boxer::DefineInterface" is invoked.
 * The object that is passed as parameter is the result of static invocation
 * of a method named \b "GetSingleton()" on the third template type \p{TInterface}.
 *
 * @tparam  TBoxed     The boxed type which is to be equipped with an interface.
 * @tparam  IsArray    Denotes if \p{TBoxed} is an array type.
 * @tparam  TInterface Type derived from class \b %Interface which provides the implementation
 *                     of the interface for type \p{TBoxed}.
 */
template<typename TBoxed, bool IsArray, typename TInterface>
inline
void DefineInterface()
{
    ( IsArray ?  (Boxer*) ArrayBoxerT<TBoxed>::GetSingleton()
              :  (Boxer*)      BoxerT<TBoxed>::GetSingleton() )

    ->DefineInterface( TInterface::GetSingleton() );
}



/**
 * This namespace function defines default interfaces for boxed types that
 * are not equipped with a specialized interface.
 *
 * A static invocation of a method named \b GetDefaultSingleton() is invoked on type \p{TInterface}
 * to receive the singleton instance of the interface used as a default implementation when
 * invoked.
 *
 * @tparam  TInterface Type derived from class \b %Interface which provides the default
 *                     implementation for boxed types with no specific implementation.
 */
template<typename TInterface>
inline
void DefineDefaultInterface()
{
    Boxer::DefineDefaultInterface( TInterface::GetDefaultSingleton() );
}

// #################################################################################################
// Default implementation of std::vector<T>
// #################################################################################################

// This is now a little a awkward: we love doxygen, but it has a bug: It would add its own marker
// [ DOX_ALIB_BOXING_STD_VECTOR ]
// to the documentation of the class. Therefore, first, we have the real, compiling version
// which is also put as a snippet in the docs, below, a whole copy of it, just for the dox...

#if !defined(DOX_PARSER)

#if ALIB_FEAT_BOXING_STD_VECTOR
//! [DOX_ALIB_BOXING_STD_VECTOR]
/**
 * Specialization of struct \b %T_Boxing for template type <c>std::vector<T, std::allocator<T>></c>
 * Instead of boxing a pointer to the vector object, a boxed array is stored, hence a pointer
 * to the first element contents and the array length.
 *
 * To suppress this default boxing behavior for type \c std::vector, use compilation
 * symbol \ref ALIB_FEAT_BOXING_STD_VECTOR_OFF.
 */
template<typename TElem>
struct T_Boxing<std::vector<TElem> > : std::true_type
{
    /** The vector type becomes boxed array type. */
    using Type= TElem;

    /** Boxing as array.
     *  @return Constant \c true */
    static constexpr bool IsArray()           { return true;  }

    /** The type is not unboxable.
     *  @return Constant \c false */
    static constexpr bool IsUnboxable()       { return false; }

    /** Denotes that default boxing is  overwritten.
     *  @return Constant \c true */
    static constexpr bool HasCustomBoxing()   { return true;  }

    /** Denotes that default Unboxing is not overwritten
     *  @return Constant \c false */
    static constexpr bool HasCustomUnboxing() { return false; }

    /**
     * Implementation of custom boxing for template class std::vector
     * @param box   The target box.
     * @param value The vector value to box.
     */
    static void           Boxing(Box& box, const std::vector<TElem>& value);

    /**
     * Declaration of unboxing. Needed for TMP compilation, while no implementation is given.
     * @param box   Never used, as never invoked.
     * @return Never invoked.
     */
    static std::vector<TElem> Unboxing(const Box& box);
};


template<typename TElem>
void T_Boxing<std::vector<TElem> >::Boxing( Box& box, std::vector<TElem> const& value )
{
    // initialize memory. (It is unexpected to be executed on any supported platform)
    if( sizeof(TElem*) < sizeof( boxvalue) )
        box.data.Value= static_cast<boxvalue >( 0 );

    // copy the pointer using memcpy (must not be done by casting!)
    const TElem* pData= value.data();
    memcpy( &box.data.Value, &pData, sizeof(TElem*) );

    // set the length
    box.data.Length=  (integer) value.size();
}
//! [DOX_ALIB_BOXING_STD_VECTOR]
#endif


// The same once more for soley for the doxygen parser!
#else
    /**
     * Specialization of struct \b %T_Boxing for template type <c>std::vector<T, std::allocator<T>></c>
     * Instead of boxing a pointer to the vector object, a boxed array is stored, hence a pointer
     * to the first element contents and the array length.
     *
     * To suppress this default boxing behavior for type \c std::vector, use compilation
     * symbol \ref ALIB_FEAT_BOXING_STD_VECTOR_OFF.
     */
    template<typename TElem>
    struct T_Boxing<std::vector<TElem> > : std::true_type
    {
        /// The vector type becomes boxed array type.
        using Type= TElem;

        /** Boxing as array.
         *  @return Constant \c true */
        static constexpr bool IsArray()           { return true;  }

        /** The type is not unboxable.
         *  @return Constant \c false */
        static constexpr bool IsUnboxable()       { return false; }

        /** Denotes that default boxing is  overwritten.
         *  @return Constant \c true */
        static constexpr bool HasCustomBoxing()   { return true;  }

        /** Denotes that default Unboxing is not overwritten
         *  @return Constant \c false */
        static constexpr bool HasCustomUnboxing() { return false; }

        /**
         * Implementation of custom boxing for template class std::vector
         * @param box   The target box.
         * @param value The vector value to box.
         */
        static void           Boxing(Box& box, const std::vector<TElem>& value);

        /**
         * Declaration of unboxing. Needed for TMP compilation, while no implementation is given.
         * @param box   Never used, as never invoked.
         * @return Never invoked.
         */
        static std::vector<TElem> Unboxing(const Box& box);
    };


    template<typename TElem>
    void T_Boxing<std::vector<TElem> >::Boxing( Box& box, std::vector<TElem> const& value )
    {
        // initialize memory. (It is unexpected to be executed on any supported platform)
        if( sizeof(TElem*) < sizeof( boxvalue) )
            box.data.Value= static_cast<boxvalue >( 0 );

        // copy the pointer using memcpy (must not be done by casting!)
        const TElem* pData= value.data();
        memcpy( &box.data.Value, &pData, sizeof(TElem*) );

        // set the length
        box.data.Length=  (integer) value.size();
    }
#endif

#if !defined(DOX_PARSER)
  ALIB_WARNINGS_RESTORE
#endif



}}}// namespace [aworx::lib::boxing]

// #################################################################################################
// Include built-in fundamental types boxing
// #################################################################################################
#if ALIB_FEAT_BOXING_FTYPES
    #include "alib/boxing/ftypes.hpp"
#endif



namespace aworx {



/// Type alias in namespace #aworx.
template<typename Type>
using     BoxedAs=     aworx::lib::boxing::BoxedAs<Type>;



} // namespace aworx


#endif // HPP_ALIB_BOXING_LIB
