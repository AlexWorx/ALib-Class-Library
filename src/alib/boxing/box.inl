/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_boxing of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_BOXING_BOX
#define HPP_ALIB_BOXING_BOX 1

#if !defined(HPP_ALIB_BOXING_BOXING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if ALIB_DEBUG && !defined(HPP_ALIB_LANG_DBGTYPEDEMANGLER)
#   include "alib/lang/dbgtypedemangler.hpp"
#endif

namespace alib {  namespace boxing  {

/** ************************************************************************************************
 * This is the central class of \alib_boxing_nl . By using template meta programming, an object
 * of this class can be created by passing just any C++ type to the constructor. The passed
 * value will be "boxed" within the instance of this class.
 *
 * Then, the instances of this class support type checking, value extraction ("unboxing") and the
 * invocation of "virtual methods". All features are customizable in detail per "boxable type".
 *
 * A thorough introduction to and documentation of all aspects of \alib_boxing_nl  is given
 * with Programmer's Manual \alib_boxing.
 *
 * ## Functors In Namespace std ##
 * Functors <c>std::hash</c>, <c>std::equal_to</c> and <c>std::less</c> are specialized for
 * this type with the inclusion of header file \alibheader{compatibility/std_boxing_functional.hpp}
 * as documented with namespace #alib::compatibility::std.
 **************************************************************************************************/
class Box
{
    // #############################################################################################
    // protected fields and methods
    // #############################################################################################
    protected:

        /**
         * The singleton of a class derived from class \b %VTable which defines our type and
         * behavior.
         */
        detail::VTable*     vtable;

        /** The data that we encapsulate. */
        Placeholder         data;


        // #########################################################################################
        // Two local macro that check whether TBoxable is a valid type to box, respectively unbox.
        // For this, various different cases are (redundantly) checked to produce compiler
        // errors that provide all necessary information about why the operation can not be
        // performed.
        // Note: Using an empty templated method for this, produced the static assertion only after
        //       other compiler errors. Therefore, a macro is used.
        // #########################################################################################

#if !defined(ALIB_DOX)
        #define ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING(TBoxable, TClean)                      \
        using          TVal          = typename std::remove_pointer<ATMP_RCVR(TBoxable)>::type;    \
        using          TPtr          = TVal*;                                                      \
        constexpr bool valueBoxing   = !ATMP_IS_PTR(TClean);                                       \
        constexpr bool isVolatile    = std::is_volatile<ATMP_RP(TBoxable)>::value;                 \
        constexpr bool isPointer     = std::is_pointer<TBoxable>::value;                           \
        constexpr bool isValue       = !isPointer;                                                 \
        constexpr bool isCustomizedTV= TT_IsCustomized<TVal>::value;                               \
        constexpr bool isCustomizedTP= TT_IsCustomized<TPtr>::value;                               \
        constexpr bool isBlockedTV   = ATMP_EQ( detail::TNotBoxable, typename T_Boxer<TVal>::Mapping::Type );   \
        constexpr bool isBlockedTP   = ATMP_EQ( detail::TNotBoxable, typename T_Boxer<TPtr>::Mapping::Type );   \
                                                                                                   \
        ALIB_STATIC_DENY( GeneralBoxingRule1,  !valueBoxing && isVolatile,                         \
          "Types boxed as pointers can not be boxed if volatile."      );                          \
                                                                                                   \
        ALIB_STATIC_DENY( CustomBoxingRule3,  isBlockedTV  && isValue,                             \
          "Customized boxing forbids boxing this value type: "                                     \
          "'T_Boxer<T>::Type == NotBoxable'!"      );                                              \
                                                                                                   \
        ALIB_STATIC_DENY( CustomBoxingRule4,  isBlockedTP  && isPointer,                           \
          "Customized boxing forbids boxing this pointer type: "                                   \
          "'T_Boxer<T*>::Type == NotBoxable'!"     );                                              \
                                                                                                   \
        ALIB_STATIC_DENY( CustomBoxingRule5,  isBlockedTV  && !isCustomizedTP  &&  isPointer,      \
          "Customized boxing forbids boxing value type T (T_Boxer<T>::Type == NotBoxable), while " \
          "no customization for this pointer type T* was given."  );                               \
                                                                                                   \
        ALIB_STATIC_DENY( CustomBoxingRule6,  isBlockedTP  && !isCustomizedTV  &&  isValue,        \
          "Customized boxing forbids boxing pointer type T* "                                      \
          "(T_Boxer<T*>::Type == NotBoxable), while no customization for this value type T was "   \
          "given. "                                );                                              \
                                                                                                   \

        // check IsType/Unbox
        #define ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_UNBOXING(TUnboxable)                          \
        using          T              = ATMP_RCVR(TUnboxable);                                     \
        using          TVal           = typename std::remove_pointer<T>::type;                     \
        using          TPtr           = TVal*;                                                     \
        constexpr bool isConst        = std::is_const   <ATMP_RP(TUnboxable)>::value;              \
        constexpr bool isVolatile     = std::is_volatile<ATMP_RP(TUnboxable)>::value;              \
        constexpr bool isPointer      = std::is_pointer<TUnboxable>::value;                        \
        constexpr bool isValue        = !isPointer;                                                \
        constexpr bool valuesFit      =     sizeof(ATMP_IF_T_F(ATMP_EQ(void,TVal), void*,TVal))    \
                                        <=  sizeof(Placeholder);                                   \
        constexpr bool isConstructible= std::is_copy_constructible<TVal>::value;                   \
        constexpr bool isTriviallyDest= std::is_trivially_destructible<TVal>::value;               \
        constexpr bool isCustomizedTV = TT_IsCustomized<TVal>::value;                              \
        constexpr bool isCustomizedTP = TT_IsCustomized<TPtr>::value;                              \
        constexpr bool isDefault      = !(isCustomizedTV || isCustomizedTP);                       \
        constexpr bool isBlockedTV    = ATMP_EQ( detail::TNotBoxable,  typename T_Boxer<TVal>::Mapping::Type );   \
        constexpr bool isBlockedTP    = ATMP_EQ( detail::TNotBoxable,  typename T_Boxer<TPtr>::Mapping::Type );   \
        constexpr bool isLockedTV     = TT_IsLocked<TVal>::value;                                  \
        constexpr bool isLockedTP     = TT_IsLocked<TPtr>::value;                                  \
                                                                                                   \
        /* Redundant type qualifiers */                                                            \
        ALIB_STATIC_DENY( GeneralBoxingRule2, isConst,                                             \
          "Type qualifier 'const' not allowed with template type TUnboxable. Types boxed as values"\
          " are always unboxed mutable, types boxed as pointers are always unboxed constant." );   \
                                                                                                   \
        ALIB_STATIC_DENY( GeneralBoxingRule3, isVolatile,                                          \
          "Type qualifier 'volatile' not allowed with template type TUnboxable"               );   \
                                                                                                   \
        /* Default boxing */                                                                       \
        ALIB_STATIC_DENY( DefaultBoxingRule1, isDefault && isValue && !valuesFit,                  \
          "This type can not be unboxed by value: "                                                \
          "By default, values that do not fit into boxes are boxed as pointers."              );   \
                                                                                                   \
        ALIB_STATIC_DENY( DefaultBoxingRule2,                                                      \
                                 isDefault && isValue && (!isConstructible || !isTriviallyDest),   \
          "This type can not be unboxed by value: "                                                \
          "By default, types that are not copy constructible or not trivially destructible, "      \
          "are boxed as pointers."                                                            );   \
                                                                                                   \
        ALIB_STATIC_DENY( DefaultBoxingRule3,                                                      \
                      isDefault && isPointer && valuesFit && isConstructible && isTriviallyDest,   \
          "This type can not be unboxed as pointer: Default boxing of types that fit "             \
          "into boxes and are copy constructible and trivially destructible, "                     \
          "is performed by value."                                                            );   \
                                                                                                   \
                                                                                                   \
        /* Custom boxing */                                                                        \
        ALIB_STATIC_DENY( CustomBoxingRule1,  isCustomizedTV && !isCustomizedTP  &&  isPointer,    \
          "This pointer type T* can not be unboxed, because custom boxing is defined for "         \
          "value type T, while no custom boxing is defined for pointer type T*."             );    \
                                                                                                   \
        ALIB_STATIC_DENY( CustomBoxingRule2, !isCustomizedTV && isCustomizedTP  &&  isValue,       \
          "This value type T can not be unboxed, because custom boxing is defined for "            \
          "pointer type T*, while no custom boxing is defined for value type T."              );   \
                                                                                                   \
                                                                                                   \
        /* Boxing blocked */                                                                       \
        ALIB_STATIC_DENY( CustomBoxingRule3,  isBlockedTV  && isValue,                             \
          "Customized boxing forbids unboxing (and even boxing) this value type: "                 \
          "'T_Boxer<T>::Type == NotBoxable'!"       );                                             \
                                                                                                   \
        ALIB_STATIC_DENY( CustomBoxingRule4,  isBlockedTP  && isPointer,                           \
          "Customized boxing forbids unboxing (and even boxing) this pointer type: "               \
          "'T_Boxer<T*>::Type == NotBoxable'!"      );                                             \
                                                                                                   \
        ALIB_STATIC_DENY( CustomBoxingRule5,  isBlockedTV  && !isCustomizedTP  &&  isPointer,      \
          "Customized boxing forbids unboxing (and even boxing) value type T "                     \
          "(T_Boxer<T>::Type == NotBoxable), while no customization for this pointer type T* "     \
          "was given."                              );                                             \
                                                                                                   \
        ALIB_STATIC_DENY( CustomBoxingRule6,  isBlockedTP  && !isCustomizedTV  &&  isValue,        \
          "Customized boxing forbids unboxing (and even boxing) pointer type T* "                  \
          "(T_Boxer<T*>::Type == NotBoxable), while no customization for this value type T was"    \
          "given."                                  );                                             \
                                                                                                   \
        /* Unboxing locked */                                                                      \
        ALIB_STATIC_DENY( CustomBoxingRule7,  isLockedTV   && isValue,                             \
          "Customized boxing forbids unboxing this value type: "                                   \
          "'T_Boxer<T>::Read' returns a different type."                                      );   \
                                                                                                   \
        ALIB_STATIC_DENY( CustomBoxingRule8,  isLockedTP   && isPointer,                           \
          "Customized boxing forbids unboxing this pointer type: "                                 \
          "'T_Boxer<T*>::Read' returns a different type."                                     );   \
                                                                                                   \
        ALIB_STATIC_DENY( CustomBoxingRule9,  isLockedTV   && !isCustomizedTP  &&  isPointer,      \
          "Customized boxing forbids unboxing value type T "                                       \
          "('T_Boxer<T>::Read' returns a different type), while no customization for this pointer "\
          "type T* was given."                                                                );   \
                                                                                                   \
        ALIB_STATIC_DENY( CustomBoxingRule10,  isLockedTP   && !isCustomizedTV  &&  isValue,       \
          "Customized boxing forbids unboxing pointer type T* "                                    \
          "('T_Boxer<T*>::Read' returns a different type), while no customization for this value " \
          "type T was given."                                                                );    \

#endif // !defined(ALIB_DOX)

        #if defined(ALIB_DOX)
            /**
             * Shortcut inline method to retrieve the vtable singleton for the template type.
             * In debug-compilations, the received \e vtable is checked for being registered.
             *
             * @tparam TBoxable  The source type to receive the vtable for.
             * @return The vtable singleton.
             */
            template<typename TBoxable, bool NoStaticAsserts= false>
            static inline
            detail::VTable* getVTable();
        #else


        template<typename TBoxable>
        ALIB_FORCE_INLINE
        static
        constexpr
        detail::VTable* getVTable()
        {
            return detail::T_VTableFactory<
                   ATMP_IF_T_F( ATMP_EQ(detail::TDefaultBoxing, typename T_Boxer<TBoxable>::Mapping::Type),
                                TMappedTo<TBoxable>,
                                typename T_Boxer<TBoxable>::Mapping )
                   >::Get();
        }

        #endif // defined(ALIB_DOX)

    // #############################################################################################
    // Constructors
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Default constructor.<br>
         * After creation with this constructor, a call to #IsType<void> returns true.
         * To reset an instance previously used, assign keyword \c nullptr.
         ******************************************************************************************/
        constexpr
        Box()                           noexcept
        : vtable( nullptr )
        , data()
        {}


        /** ****************************************************************************************
         * Trivial default copy constructor.
         ******************************************************************************************/
        Box( const Box&  )              noexcept                                          = default;

        /** ****************************************************************************************
         * Trivial default move constructor.
         ******************************************************************************************/
        Box(       Box&& )              noexcept                                          = default;

        /** ****************************************************************************************
         * Trivial default copy assign operator.
         * @return A reference to \c this.
         ******************************************************************************************/
        Box& operator=( const Box&  )   noexcept                                          = default;

        /** ****************************************************************************************
         * Trivial default move assign operator.
         * @return A reference to \c this.
         ******************************************************************************************/
        Box& operator=(       Box&& )   noexcept                                          = default;

        /** ****************************************************************************************
         * Trivial default destructor.
         ******************************************************************************************/
        ~Box()                          noexcept                                          = default;



    #if defined(ALIB_DOX)
        /** ****************************************************************************************
         * Constructor using template meta programming to fetch any type of C++ value.<br>
         * Internally, this constructor is implemented using a set of different constructors
         * which are selected by the compiler using \c  std::enable_if conditions.
         *
         * Types derived from class \b %Box itself are boxed by coping the internal values
         * of the box. This means, that boxing objects of derived types is similar to
         * "downcasting" the object to class \b %Box.
         *
         * @tparam TBoxable  Any C++ type to be boxed.
         * @param  src       The src value or pointer type \c T.
         ******************************************************************************************/
        template <typename TBoxable>
        inline  constexpr Box(const  TBoxable& src ) noexcept;
    #else
        // ################################## Special types #######################################

        // Keyword 'nullptr'
        ATMP_SELECT_IF_1TP( typename TNullptr, std::is_same<std::nullptr_t, TNullptr>::value )
        constexpr
        Box(const TNullptr& )           noexcept
        : vtable(nullptr)
        , data  ()
        {}


        // C++ arrays
        ATMP_SELECT_IF_1TP( typename T, ATMP_IS_ARR(ATMP_RCV(T)) )
        constexpr
        Box( T& src )                   noexcept
        : vtable( detail::T_VTableFactory<TMappedToArrayOf<ATMP_RECVP(T)>>::Get() )
        , data  ( &src, static_cast<integer>(     std::is_same<ATMP_RECVP(T), char    >::value
                                               || std::is_same<ATMP_RECVP(T), wchar_t >::value
                                               || std::is_same<ATMP_RECVP(T), char16_t>::value
                                               || std::is_same<ATMP_RECVP(T), char32_t>::value
                                             ? std::extent<ATMP_RCVP(T)>::value - 1
                                             : std::extent<ATMP_RCVP(T)>::value             )      )
        {}


        // Derived Box value types (Box itself uses built-in copy constructor)
        //         note: references to Box are fetched with copy/move constructors
        ATMP_SELECT_IF_1TP( typename T,  !std::is_same   <Box, ATMP_RCV(T)>::value
                                      &&  std::is_base_of<Box, ATMP_RCV(T)>::value )
        constexpr
        Box(const T& src )              noexcept
        : vtable( src.vtable )
        , data  ( src.data   )
        {}

        // ########################     Boxing  with  T_Boxer  #####################################

        // 1) Value remains value
        // 1.1) Customized with constexpr T_Boxer::Write
        ATMP_SELECT_IF_1TP( typename T,
           !ATMP_IS_PTR(T)
        &&  ATMP_EQ(Placeholder, decltype( T_Boxer<ATMP_RCV(T)>::Write( std::declval<const ATMP_RCV(T)&>()  ))) )
        constexpr Box(const T& src ) noexcept
        : vtable( getVTable<ATMP_RCV(T)>() )
        , data  ( T_Boxer  <ATMP_RCV(T)>::Write( const_cast<ATMP_RCV(T) const &>( src ) ) )
        {}

        // 1.2) Default or customized with standard T_Boxer::Write
        ATMP_SELECT_IF_1TP( typename T,
            !ATMP_IS_PTR(T)
        && !ATMP_EQ(std::nullptr_t,T)
        && !ATMP_ISOF(T, Box)
        && (    (    !TT_IsCustomized<ATMP_RCVR(T)*>::value
                  &&  sizeof(Placeholder) >= sizeof(ATMP_RCVP(T))
                  &&  std::is_copy_constructible    <ATMP_RCVP(T)>::value
                  &&  std::is_trivially_destructible<ATMP_RCVP(T)>::value )
             || TT_IsCustomized<ATMP_RCVR(T) >::value )
        &&  ATMP_EQ(void, decltype( T_Boxer<ATMP_RCV(T)>::Write(data, std::declval<const ATMP_RCV(T)&>()) )))
        Box(const T& src )              noexcept
        : vtable( getVTable<ATMP_RCV(T)>() )
        , data  ()
        {
            ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING(T, ATMP_RCV(T))
            T_Boxer<ATMP_RCV(T)>::Write( data, const_cast<ATMP_RCV(T) const &>( src ) );
        }

        // 2) Value converted to pointer
        // 2.1) Default boxing as pointer (constexpr)
        ATMP_SELECT_IF_1TP( typename T,
           !ATMP_IS_PTR(T)
        && !ATMP_EQ(std::nullptr_t,T)
        && !ATMP_ISOF(T, Box)
        && !TT_IsCustomized  <ATMP_RCVR(T) >::value
        && !TT_IsCustomized  <ATMP_RCVR(T)*>::value
        && !ATMP_IS_ARR(T)
        &&  (    (sizeof(Placeholder) < sizeof(ATMP_RCVP(T)))
              || !std::is_copy_constructible    <ATMP_RCVP(T)>::value
              || !std::is_trivially_destructible<ATMP_RCVP(T)>::value  )   )
        constexpr
        Box(const T& src )              noexcept
        : vtable( getVTable<ATMP_RCV(T)*>() )
        , data  ( Placeholder( &src )                                                        )
        {}

        // 2.2) Customized pointer type with constexpr T_Boxer::Write
        ATMP_SELECT_IF_1TP( typename T,
            !ATMP_IS_PTR(T)
        && !TT_IsCustomized  <ATMP_RCVR(T) >::value
        &&  ATMP_EQ(Placeholder, decltype( T_Boxer<ATMP_RCV(T)*>::Write( std::declval<ATMP_RCV(T)* const &>()  ))) )
        constexpr Box(const T& src )   noexcept
        : vtable( getVTable<ATMP_RCV(T)*>() )
        , data  ( T_Boxer  <ATMP_RCV(T)*>::Write( const_cast<ATMP_RCV(T)* const&>( &src ) ) )
        {}

        // 2.3) Customized with constexpr T_Boxer::Write
        ATMP_SELECT_IF_1TP( typename T,
            !ATMP_IS_PTR(T)
        && !TT_IsCustomized  <ATMP_RCVR(T) >::value
        &&  TT_IsCustomized  <ATMP_RCVR(T)*>::value
        &&  ATMP_EQ(void, decltype( T_Boxer<ATMP_RCV(T)*>::Write(data, std::declval<ATMP_RCV(T)* const &>()) )))
        Box(const T& src )              noexcept
        : vtable( getVTable<ATMP_RCV(T)*>() )
        , data  ()
        {
            ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING(T, ATMP_RCV(T)*)
            T_Boxer<ATMP_RCV(T)*>::Write( data, const_cast<ATMP_RCV(T)*>( &src )  );
        }


        // 3) Pointer remains pointer

        // 3.1) No customization
        ATMP_SELECT_IF_1TP( typename T,
             ATMP_IS_PTR(T)
        && !ATMP_ISOF(ATMP_RCVP(T), Box)
        && !TT_IsCustomized<ATMP_RCVP(T)>::value
        && !TT_IsCustomized<ATMP_RCVR(T)>::value
        &&  (     (sizeof(Placeholder) < sizeof(ATMP_RCVP(T)))
              || !std::is_copy_constructible    <ATMP_RCVP(T)>::value
              || !std::is_trivially_destructible<ATMP_RCVP(T)>::value ) )
        constexpr Box(const T& srcP )   noexcept
        : vtable( getVTable<ATMP_RCVP(T)*>() )
        , data  ( Placeholder( srcP ) )
        {}

        // 3.2) Customized with constexpr T_Boxer::Write, mutable pointer type
        ATMP_SELECT_IF_1TP( typename T,
            ATMP_IS_PTR(T)
        && !std::is_const<T>::value
        && ATMP_EQ(Placeholder, decltype( T_Boxer<ATMP_RCVP(T)*>::Write( std::declval<ATMP_RCVP(T)* const&>()  ))) )
        constexpr   Box(const T& srcP ) noexcept
        : vtable( getVTable<ATMP_RCVP(T)*>() )
        , data  ( T_Boxer  <ATMP_RCVP(T)*>::Write( const_cast<ATMP_RCVP(T)* const &>( srcP ) ) )
        {}


        // 3.3) Customized with constexpr T_Boxer::Write, constant pointer type
        ATMP_SELECT_IF_1TP( typename T,
            ATMP_IS_PTR(T)
        && std::is_const<ATMP_RP(T)>::value
        && ATMP_EQ(Placeholder, decltype( T_Boxer<T>::Write( std::declval<T&>()  ))) )
        constexpr Box(const T& srcP )   noexcept
        : vtable( getVTable<T>() )
        , data  ( T_Boxer  <T>::Write( srcP ) )
        {}

        // 3.4) Customized with normal T_Boxer::Write
        ATMP_SELECT_IF_1TP( typename T,
            ATMP_IS_PTR(T)
        && TT_IsCustomized  <ATMP_RCVR(T)>::value
        && ATMP_EQ(void, decltype( T_Boxer<ATMP_RCVP(T)*>::Write(data, std::declval<ATMP_RCVP(T)* const&>()) )))
        Box(const T& srcP )             noexcept
        : vtable( getVTable<ATMP_RCVP(T)*>() )
        , data  ()
        {
            ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING(T, ATMP_RCVP(T)*)
            T_Boxer<ATMP_RCVP(T)*>::Write( data, reinterpret_cast<ATMP_RCVP(T)*>( const_cast<ATMP_RCVP(T)*>(srcP) ) );
        }



        // 4) Pointer dereferenced to value
        // 4.1) Customized with constexpr T_Boxer::Write
        ATMP_SELECT_IF_1TP( typename T,
             ATMP_IS_PTR(T)
        &&  (    !std::is_const<ATMP_RP(T)>::value
              || !ATMP_EQ(void, decltype( T_Boxer<ATMP_RCVP(T)*>::Write(data, std::declval<ATMP_RCVP(T)* const&>()) )) )
        &&  ATMP_EQ(Placeholder, decltype( T_Boxer<ATMP_RCVP(T)>::Write( std::declval<const ATMP_RCVP(T)&>()  ))) )
        constexpr Box(const T& srcP )   noexcept
        : vtable( getVTable<ATMP_RCVP(T)>() )
        , data  ( srcP ? T_Boxer  <ATMP_RCVP(T)>::Write( const_cast<ATMP_RCVP(T) const &>( *srcP ) )
                       : (sizeof(ATMP_RCVP(T)) <= sizeof(integer) )
                       ? Placeholder( 0 )
                       : Placeholder( static_cast<integer>(0), static_cast<integer>(0) ) )
        {}

        // 4.2) Default or customized with normal T_Boxer::Write
        ATMP_SELECT_IF_1TP( typename T,
             ATMP_IS_PTR(T)
        && !ATMP_ISOF(ATMP_RCVP(T), Box)
        && !TT_IsCustomized<ATMP_RCVR(T) >::value
        && (    TT_IsCustomized<ATMP_RCVP(T) >::value
             || (     sizeof(Placeholder) >= sizeof( ATMP_RCVP(T) )
                   && std::is_copy_constructible    <ATMP_RCVP(T)>::value
                   && std::is_trivially_destructible<ATMP_RCVP(T)>::value ) )
        &&  ATMP_EQ(void, decltype( T_Boxer<ATMP_RCVP(T)>::Write(data, std::declval<const ATMP_RCVP(T)&>()) )))
        Box(const T& srcP )             noexcept
        : vtable( getVTable<ATMP_RCVP(T)>() )
        , data  ()
        {
            ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING(T, ATMP_RCVP(T))

                                        // cast needed to remove volatile, as allowed with values
            if( srcP )  T_Boxer<ATMP_RCVP(T)>::Write( data, const_cast<ATMP_RCVP(T) const &>(*srcP)  );
            else        data.Clear<     ATMP_EQ( typename T_Boxer<ATMP_RCVP(T)>::Mapping::PlainOrArray, detail::TArrayOf        )
                                      ? sizeof(Placeholder)
                                      : T_SizeInPlaceholder< ATMP_IF_T_F(  ATMP_EQ( typename T_Boxer<ATMP_RCVP(T)>::Mapping::Type , detail::TDefaultBoxing )
                                                                           , ATMP_RCVP(T)
                                                                           , typename T_Boxer<ATMP_RCVP(T)>::Mapping::Type)
                                                           >::value
                                  >();
        }


        #undef ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING
        #undef ALIB_TM_IS_DEFAULT_BOXING

    #endif // defined(ALIB_DOX)

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        #if ALIB_DEBUG
            /**
             * Returns the \e vtable of this instance that is associated with the currently boxed
             * type.<br>
             * Available only with debug builds.
             *
             * \see
             *    Manual chapter \ref alib_boxing_more_debug of the Programmer's Manual.
             *
             * @return The \e vtable of this instance.
             */
            const detail::VTable*    DbgGetVTable()                                            const
            {
                return vtable;
            }

        #endif

        #if defined(ALIB_DOX)
        /** ****************************************************************************************
         * Checks if this box stores a value of type \p{TBoxable}.
         *
         * If template parameter \p{TBoxable} it is not unboxable, a compile-time assertion
         * is given, with specific guidance why the type must not be unboxed and for that
         * reason must not even be tested for.
         *
         * Special type \c void may be given to test if this box does contain a value at all.
         * A box does not contain a value, after
         * - default construction,
         * - construction with keyword \c nullptr, or
         * - assignment of  keyword \c nullptr.
         *
         * For more information on the "void state" of boxes, see manual chapter
         * \ref alib_boxing_more_void.
         *
         *
         * @tparam TBoxable  The boxable type guessed.
         * @return \c true if this box stores a value that is convertible to type \p{TBoxable},
         *         \c false otherwise.
         ******************************************************************************************/
        template<typename TBoxable>
        inline
        bool            IsType()                                                              const;

        #else

        template<typename TBoxable>
        ATMP_T_IF(bool, ATMP_EQ(TBoxable, void) )
        IsType()                                                                               const
        {
            return vtable == nullptr;
        }

        template<typename TBoxable>
        ATMP_T_IF(bool, !ATMP_EQ(TBoxable, void))
        IsType()                                                                               const
        {
            ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_UNBOXING(TBoxable)

            return vtable == getVTable<TBoxable>();
        }
        #endif

        #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS   || defined(ALIB_DOX)
            /** ************************************************************************************
             * Tests if this this box contains a signed integral type (one of the C++ fundamental
             * types of different sizes).
             *
             * With compilation that disables
             * \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS, this method will be inlined and
             * simply returns <c>IsType<integer>()</c>.<br>
             * Otherwise this method will not be inlined and tests for the five different
             * integer sizes (\c int8_t, \c int16_t, \c int32_t, \c int64_t and \alib{intGap_t}).
             *
             * @return \c true if this box contains a signed integral type, \c false otherwise.
             **************************************************************************************/
            bool        IsSignedIntegral()          const
            {
                return IsType<integer >();
            }

            /** ************************************************************************************
             * Tests if this this box contains an unsigned integral type (one of the C++ fundamental
             * type of different sizes).
             *
             * With default library compilation that disables
             * \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS, this method will be inlined and
             * simply returns <c>IsType<uinteger>()</c>.<br>
             * Otherwise this method will not be inlined and tests for the five different
             * integer sizes (\c uint8_t, \c uint16_t, \c uint32_t, \c uint64_t and
             * \alib{uintGap_t}).
             *
             * @return \c true if this box contains an unsigned integral type, \c false otherwise.
             **************************************************************************************/
            bool        IsUnsignedIntegral()        const
            {
                return IsType<uinteger>();
            }

            /** ************************************************************************************
             * Unboxes a signed integral.
             *
             * With default library compilation that disables
             * \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS, this method will be inlined and
             * simply returns <c>Unbox<integer>()</c>.<br>
             * Otherwise this method will not be inlined and tests for the five different
             * integer sizes (1, 2, 4 and 8 bytes size and the #alib::intGap_t) prior to
             * unboxing.
             *
             * @return The boxed signed integral value.
             **************************************************************************************/
            integer   UnboxSignedIntegral()       const
            {
                return Unbox<integer >();
            }

            /** ************************************************************************************
             * Unboxes an unsigned integral.
             *
             * With default library compilation that disables
             * \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS, this method will be inlined and
             * simply returns <c>Unbox<uinteger>()</c>.<br>
             * Otherwise this method will not be inlined and tests for the five different
             * integer sizes (1, 2, 4 and 8 bytes size and the #alib::uintGap_t) prior to
             * unboxing.
             *
             * @return The boxed unsigned integral value.
             **************************************************************************************/
            uinteger  UnboxUnsignedIntegral()     const
            {
                return Unbox<uinteger >();
            }
        #else
            ALIB_API bool            IsSignedIntegral()   const;
            ALIB_API bool          IsUnsignedIntegral()   const;
            ALIB_API integer      UnboxSignedIntegral()   const;
            ALIB_API uinteger   UnboxUnsignedIntegral()   const;
        #endif


        #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS  || defined(ALIB_DOX)
            /** ************************************************************************************
             * Tests if this this box contains one of types \c char, \c wchar_t, \c char16_t or
             * \c char32_t.
             *
             * With default library compilation that disables symbol
             * \ref ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS, this method will be inlined and
             * simply returns <c>IsType<wchar>()</c>.<br>
             * Otherwise this method will not be inlined and tests for all four different
             * character types.
             *
             * @return \c true if this box contains a character type, \c false otherwise.
             **************************************************************************************/
            bool            IsCharacter()                                                     const
            {
                return IsType<wchar>();
            }

            /** ************************************************************************************
             * Unboxes one of the types \c char, \c wchar_t, \c char16_t or \c char32_t
             * and converts it to \alib{characters,wchar}.
             *
             * With default library compilation that disables
             * \ref ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS, this method will be inlined and
             * simply returns <c>Unbox<wchar>()</c>.<br>
             * Otherwise this method will not be inlined and tests for the four different
             * character types prior to unboxing.
             *
             * @return The stored character.
             **************************************************************************************/
            wchar           UnboxCharacter()                                                  const
            {
                return Unbox<wchar>();
            }

        #else
            ALIB_API bool   IsCharacter()                                                     const;
            ALIB_API wchar  UnboxCharacter()                                                  const;
        #endif


        /** ****************************************************************************************
         * Tests if this this box contains a floating point type.
         *
         * \note
         *   If \ref ALIB_FEAT_BOXING_BIJECTIVE_FLOATS is not set, this method will
         *   test against \c double and <c>long double</c>. If it is set, in addition
         *   type \c float is tested.
         *
         * @return \c true if this box contains a floating point type, \c false otherwise.
         ******************************************************************************************/
        bool            IsFloatingPoint()                                                     const;

        /** ****************************************************************************************
         * Unboxes a floating point value as \c double.
         *
         * \note
         *   If \ref ALIB_FEAT_BOXING_BIJECTIVE_FLOATS is not set, this method will
         *   test against \c double and <c>long double</c> and convert the latter.
         *   If it is set, in addition type \c float is tested.
         *
         * @return \c true if this box contains a floating point type, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        double          UnboxFloatingPoint()                                                  const;

        /** ****************************************************************************************
         * Returns \c true if this box represents an array of objects.
         * In this case, method #UnboxLength (usually) will return the length of the array and
         * #UnboxElement may be used to access elements of the array.
         *
         * @return \c true if this box represents an array, \c false otherwise.
         ******************************************************************************************/
        bool            IsArray()                                                              const
        {
            return vtable && vtable->IsArray();
        }

        /** ****************************************************************************************
         * Returns \c true if this objects represents an array and the element type
         * equals template parameter \p{TElementType}.
         *
         * @tparam TElementType The array element type to compare our element type with.
         * @return \c true if this box represents an array of given type, \c false otherwise.
         ******************************************************************************************/
        template<typename TElementType>
        bool            IsArrayOf()                                                            const
        {
            return      vtable && typeid(TElementType) == vtable->ElementType;
        }

        /** ****************************************************************************************
         * Returns \c true if this box uses pointer-boxing, otherwise \c false.
         * The default boxing of pointers will store the pointer to the boxed object
         * in union \alib{boxing,Placeholder::Pointers}.
         *
         * @return \c true if this box contains an object boxed as pointer type, \c false otherwise.
         ******************************************************************************************/
        bool            IsPointer()                                                            const
        {
            return vtable && vtable->IsPointer();
        }

        /** ****************************************************************************************
         * Returns \c true if this box contains an element of a scoped or non-scoped enum type.
         * Enum element values are always boxed as type \alib{integer} stored in
         * union field \alib{boxing,Placeholder}.
         *
         * @return \c true if this box contains an object boxed as pointer type, \c false otherwise.
         ******************************************************************************************/
        bool            IsEnum()                                                               const
        {
            return vtable && vtable->IsEnum();
        }

        /** ****************************************************************************************
         * Returns \c true if \p{other} and this object share the same boxed type.
         * Note, if this box has void state (no value boxed), then this method returns
         * \c false, even if given \p{other} is void state as well.
         *
         * @param other The box to compare our type with.
         * @return \c true if this box has the same type like \p{other}, \c false otherwise.
         ******************************************************************************************/
        bool            IsSameType(const Box& other)                                           const
        {
            return  vtable && vtable == other.vtable;
        }

        #if defined(ALIB_DOX)
        /** ****************************************************************************************
         * Returns the contents of this box converted to type \p{TBoxable}.
         * By default this is done by invoking template method \alib{boxing,Placeholder::Read}
         * on field #data.
         * This behavior might be customized by specializing struct\alib{boxing,T_Boxer}".
         *
         * With debug builds, the actual type of this object is
         * \ref ALIB_ASSERT_ERROR "asserted" to equal the templated return type.
         *
         * \note
         *   With debug builds, it is \ref ALIB_ASSERT_ERROR "asserted" that given \p{TBoxable}
         *   is mapped to the type stored, so that #IsType returned \c true for \p{TBoxable}.
         *   In release compilations, no checks are performed!
         *
         * @tparam TUnboxable The type to unbox. If it is not unboxable, a compile-time
         *                    assertion is given.
         * @return The unboxed value of type \p{TUnboxable} .
         ******************************************************************************************/
        template<typename TUnboxable>
        inline
        const TUnboxable  Unbox()                                                             const;
        #else

        template <typename TUnboxable>
        ALIB_FORCE_INLINE
        ATMP_IF_T_F( !ATMP_IS_PTR(TUnboxable),  TUnboxable,
                                                ATMP_RP(TUnboxable) const * )
        Unbox()                                                                                const
        {
            ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_UNBOXING(TUnboxable)

            ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Unboxing is undefined behavior." )
            ALIB_ASSERT_ERROR( vtable == getVTable<TUnboxable>(),
                               "Can not unbox type <" , lang::DbgTypeDemangler(typeid(TUnboxable)).Get(),
                               "> from mapped type <" , lang::DbgTypeDemangler(vtable->Type    ).Get(),
                               ">." )

            detail::DbgCheckRegistration( vtable, true );
            return T_Boxer<TUnboxable>::Read(data);
        }

        #undef ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_UNBOXING
        #endif

        #if defined(ALIB_DOX)
        /** ****************************************************************************************
         * Convenient method to unbox types boxed as pointers, as a non-<c>const</c> pointer type.
         *
         * \see Refer to manual chapter \ref alib_boxing_classes_constant for more information.
         *
         * @tparam TUnboxable The type to unbox. If it is not unboxable, a compile-time
         *                    assertion is given.
         * @return The unboxed value of type \p{TUnboxable} casted to a non-<c>const</c> object.
         ******************************************************************************************/
        template<typename TUnboxable>
        inline
        TUnboxable  UnboxMutable()                                                            const;
        #else

        template <typename TUnboxable>
        ALIB_FORCE_INLINE
        TUnboxable
        UnboxMutable()                                                                         const
        {
            return const_cast<ATMP_RC(TUnboxable)>( Unbox<TUnboxable>() );
        }
        #endif

        /** ****************************************************************************************
         * Returns the "raw" placeholder of this box.
         *
         * In some special situations, this method might be used inspect into the boxed data and
         * "reinterpret" its contents in a custom way.
         *
         * @return The raw contents of this box.
         ******************************************************************************************/
        const Placeholder&  Data()                                                             const
        {
            ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Can nt access placeholder." )
            return data;
        }

        /** ****************************************************************************************
         * Non-constant variant of #Data, that allows write access to the internal
         * memory of this box.
         *
         * A use case for non-constant access could be the implementation of a
         * \ref alib_boxing_functions_mutable "non-constant box-function".
         * In fact, this is the only occasion where within any \alibmod_nl this method was
         * needed.
         *
         * @return The raw contents of this box.
         ******************************************************************************************/
        Placeholder&        Data()
        {
            ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Can't access placeholder." )
            return data;
        }

        /** ****************************************************************************************
         * Returns the number of relevant bytes used in the placeholder.
         *
         * This method is used with default implementations of box-functions \alib{boxing,FHashcode}
         * and \alib{boxing,FEquals}.
         *
         * \see
         *   The documentation of TMP struct \alib{boxing,T_SizeInPlaceholder} provides details on
         *   and rationals for the existence of this method.
         *
         * @return The raw contents of this box.
         ******************************************************************************************/
        unsigned int        GetPlaceholderUsageLength()                                        const
        {
            ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized." )
            return vtable->PlaceholderUsage;
        }

        /** ****************************************************************************************
         * Returns the \c std::type_info struct describing the boxed type.
         * To get the element type of boxed arrays, use #ElementTypeID.
         *
         * \note
         *   This method is provided for "completeness" and only be used in special situations.<br>
         *
         * \note
         *   If a box is not initialized (or has \c nullptr assigned, <c>typeid(void)</c> is
         *   returned.
         *
         * \note
         *   In case of arrays, the a \c std::type_info reference is returned that corresponds
         *   to an array of the element type of size \c 1. For example, if an array of type
         *   \c double of arbitrary size was boxed, then <c>typeid(double[1])</c>is returned.
         *
         * @return The \c std::type_info of the mapped type.
         ******************************************************************************************/
        const std::type_info&   TypeID()                                                       const
        {
            detail::DbgCheckRegistration( vtable, true );
            return  vtable ? vtable->Type : typeid(void);
        }

        /** ****************************************************************************************
         * Returns the \c std::type_info struct describing the element type of mapped array types.
         *
         * \note
         *   This method is provided for "completeness" and only be used in special situations.<br>
         *   In case this box is not of array type, <c>typeid(void)</c> is returned.
         *
         * @return The \c std::type_info of the mapped type.
         ******************************************************************************************/
        const std::type_info&   ElementTypeID()                                                const
        {
            ALIB_ASSERT_ERROR( vtable   , "BOXING", "Box not initialized. Can not get type information." )
            return  vtable->ElementType;
        }

        /** ****************************************************************************************
         * Returns the size in bytes of on element of the stored array.
         * For non-array types, \c 0 is returned.
         *
         * @return The size of elements in the array.
         ******************************************************************************************/
        size_t           ArrayElementSize()                                                    const
        {
            ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Unboxing is undefined behavior." )
            return vtable->Mapping > 0 ? static_cast<size_t>( vtable->Mapping )
                                       : 0;
        }

        /** ****************************************************************************************
         * Returns the pointer to the first array element.
         *
         * \note
         *   With debug builds, it is \ref ALIB_ASSERT_ERROR "asserted" that #IsArray
         *   returns \c true and the stored type is the same as requested.
         *   In release compilations, no checks are performed!
         *
         * @tparam TElementType The type of array elements
         * @return A pointer to the first array element.
         ******************************************************************************************/
        template <typename TElementType>
        TElementType*    UnboxArray()       const
        {
            ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Unboxing is undefined behavior." )
            ALIB_ASSERT_ERROR( IsArray(), "BOXING",
                               "Box::UnboxArray() invoked on box of non-array type <",
                               lang::DbgTypeDemangler(vtable->Type).Get(), ">." )

            ALIB_ASSERT_ERROR( typeid(TElementType) == vtable->ElementType,
                               "BOXING: Can not unbox array type<" , lang::DbgTypeDemangler(typeid(TElementType*)).Get(),
                               "[]> from mapped type<"             , lang::DbgTypeDemangler(vtable->ElementType  ).Get(),
                               "[]>." )

            detail::DbgCheckRegistration( vtable, true );

            return data.Pointer<TElementType>();
        }

        /** ****************************************************************************************
         * Returns the length of a boxed Array. While in theory, the length applies only to
         * arrays, in debug-compilations, \b no run-time type check is performed.
         * This way, mapped types that use the second "word" of the placeholder to store a
         * value of type \c integer, may also use this function.<br>
         * In the latter case, the name of this method might be misleading and therefore, it is
         * recommended to use <b>Data().integer[1]</b> to denote that a custom interpretation of the
         * placeholder is performed. The compilation result is the same.
         *
         * Some quick rational for why \alib is generally using signed types for array lengths,
         * is given \ref alib_strings_details_signedlength "here".
         *
         * @return The length of the boxed object.
         ******************************************************************************************/
        integer         UnboxLength()                                                          const
        {
            ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Can not access placeholder." )
            return data.Length();
        }

        /** ****************************************************************************************
         * Returns a reference to element \p{idx} of the boxed array.
         *
         * \note
         *   With debug builds, it is \ref ALIB_ASSERT_ERROR "asserted" that #IsArray returns
         *   \c true, that the stored type is the same as the requested type and the provided
         *   \p{idx} is between \c 0 and the length of the array.
         *   In release compilations, no checks are performed!
         *
         * @tparam TElementType The type of array elements
         * @param  idx      The index of the element to receive.
         * @return The value of the element at \p{idx}.
         ******************************************************************************************/
        template <typename TElementType>
        TElementType&    UnboxElement(integer idx)       const
        {
            ALIB_ASSERT_ERROR( vtable, "BOXING", "Box is void (no contents). Unboxing is undefined behavior." )
            ALIB_ASSERT_ERROR( IsArray(), "BOXING",
                               "Box::UnboxElement() invoked on box of non-array type <",
                               lang::DbgTypeDemangler(vtable->Type).Get(), ">." )

            ALIB_ASSERT_ERROR( typeid(TElementType) == vtable->ElementType,
                               "BOXING: Can not unbox array element type <" , lang::DbgTypeDemangler(typeid(TElementType)).Get(),
                               "> from mapped type <"                       , lang::DbgTypeDemangler(vtable->ElementType    ).Get(),
                               "[]>." )

            ALIB_ASSERT_ERROR( idx >= 0 && idx < UnboxLength(), "BOXING",
                               "Box::UnboxElement<", lang::DbgTypeDemangler(typeid(TElementType)).Get(),
                               ">(): Index out of bounds.")

            detail::DbgCheckRegistration( vtable, true );

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            return * ( data.Pointer<TElementType>()  + idx );
            ALIB_WARNINGS_RESTORE
        }

        #if defined(ALIB_DOX)
        /** ****************************************************************************************
         * Searches an implementation  of a box-function identified by template parameter
         * \p{TFDecl}, which has to be implemented according the rules of
         * \ref alib_boxing_functions_concepts_decl "function declarations".<br>
         * If found, a <em>non-nulled</em> function pointer is returned, otherwise a \e nulled one.
         *
         * On success, the function can be invoked by passing the returned pointer to method
         * #CallDirect.
         * This approach avoids further searches that are otherwise to be performed with multiple
         * invocations of method #Call.
         *
         * If parameter \p{defaults} equals \alib{lang,Reach::Local}, functions specific to the mapped
         * type of this box (registered using \alib{boxing,BootstrapRegister}) are searched.
         * If \alib{lang,Reach::Global} is given, then a defaulted function (registered using
         * \alib{boxing,BootstrapRegisterDefault}) is searched, if no specific function was found.
         *
         * \note
         *   \alib{lang,Reach::Local} can be used to detect specific behavior and to avoid the use
         *   of default functions. This can be useful if the default implementation of a function
         *   is just not applicable in a certain situation.
         *
         * \note
         *   A second use case of this method are situations where multiple invocations of the
         *   same function are to be done, on just one or on several boxes of the same mapped type:
         *
         *        assert( box1.IsSameType( box2 ) );
         *
         *        auto* func= box1.GetFunction<FMyFunc>( Reach::Global );
         *        if( func != nullptr )
         *            for( int i= 0; i< 10; ++i )
         *            {
         *                box1.CallDirect<FMyFunc>( func, i );
         *                box2.CallDirect<FMyFunc>( func, i );
         *            }
         *
         * @tparam TFDecl       The \ref alib_boxing_functions_concepts_decl "function declaration"
         *                      to search for.
         * @param  searchScope  \alib{lang,Reach::Local} chooses type-specific functions only, while.
         *                      \alib{lang,Reach::Global} includes default functions in the search.
         * @param  isInvocation Available only in debug compilations. If \c true, a counter
         *                      associated with an implementation found is increaed to provide
         *                      statistics. Defaults to false and should not be given.
         * @return The function implementation.
         *         \c nullptr in case that no function is available.
         ******************************************************************************************/
        template <typename TFDecl>
        inline
        typename TFDecl::Signature    GetFunction( Reach searchScope
                                                 , bool isInvocation = false  )               const;
        #else
        template <typename TFDecl>
        typename TFDecl::Signature    GetFunction( lang::Reach searchScope
                                       ALIB_DBG( , bool isInvocation = false)    )             const
        {
            if( !vtable )
                return nullptr;

             ALIB_DBG( ++vtable->DbgCntUsage );

            auto result= vtable->Functions.Get<TFDecl>( ALIB_DBG(isInvocation) );
            return    result
                    ? result
                    : searchScope == lang::Reach::Global
                    ? detail::DEFAULT_FUNCTIONS.Get<TFDecl>( ALIB_DBG(isInvocation) )
                    : nullptr;
        }
        #endif //ALIB_DOX



        /** ****************************************************************************************
         * Invokes a function registered for boxes of the mapped type.
         * The \ref alib_boxing_functions_concepts_decl "function declaration" is provided with the
         * first template parameter \p{TFDecl}.
         * The variadic further template parameters do not need to be specified.
         * They specify the types of the called function's parameters and are matched against
         * the function signature given with the declaration.
         * If the types of the given function arguments do not correspond to the types of
         * the box-function called, a compile-time error is raised.
         *
         * \note
         *   Precisely, the variadic template types denote the function arguments starting from the
         *   second, as the first argument is always a reference to the box that this method was
         *   invoked on.
         *
         * If no corresponding function \alib{boxing,BootstrapRegister,was registered} for the mapped
         * type, then a \alib{boxing,BootstrapRegisterDefault,default function}, that is applicable
         * to any mapped type is searched.
         * If neither is found, a default value of the return type of the function is returned.
         *
         * With debug builds, an assertion is raised if the function type is not known at all
         * to \alib_boxing_nl. This is true, if an implementation was neither registered with
         * any other mapped type, nor registered as a default.
         *
         * \see
         *   Description of method #GetFunction to implement two use cases:
         *   - Repetitive invocation of the same function.
         *   - Avoidance of default functions
         *
         * \see
         *   A non-constant overload exists, for the seldom case the reference to this box that is
         *   passed to the function, needs to be of non-constant type.
         *
         * @tparam TFDecl  The function type to call. Has to be explicitly specified.
         * @tparam TArgs   Types of the variadic arguments \p{args}. Do not need to be specified.
         * @param  args    Variadic arguments forwarded to the function.
         * @return Nothing in case that \p{TReturn} is void, otherwise the result of the invocation,
         *         respectively <c>TReturn()</c> if the requested function type was not found for
         *         this \b %Box.
         ******************************************************************************************/
        template <typename TFDecl,  typename... TArgs>
        decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(), std::declval<TArgs>()... ) )
        Call(TArgs&&... args)                                                                  const
        {
            auto* func= GetFunction<TFDecl>( lang::Reach::Global    ALIB_DBG(, true) );
            if( func != nullptr )
                return reinterpret_cast<typename TFDecl::Signature>(func)
                       ( *this, std::forward<TArgs>(args)... );


            return  decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(),
                                                                          std::declval<TArgs>()... )) ();
        }

        /** ****************************************************************************************
         * Alternative version of method #Call, which accepts the function's pointer as a first
         * argument. Such pointer can be received upfront with method #GetFunction.
         *
         * @tparam TFDecl    The function type to call. Has to be explicitly specified.
         * @tparam TArgs     Types of the variadic arguments \p{args}. Do not need to be specified.
         * @param  args      Variadic arguments forwarded to the function.
         * @param  function  The function to invoke.
         * @return Nothing in case that \p{TReturn} is void, otherwise the result of the invocation,
         *         respectively <c>TReturn()</c> if the requested function type was not found for
         *         this \b %Box.
         ******************************************************************************************/
        template <typename TFDecl,  typename... TArgs>
        decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(), std::declval<TArgs>()... ) )
        CallDirect(typename TFDecl::Signature function, TArgs&&... args)                  const
        {
            ALIB_ASSERT_ERROR( vtable, "BOXING",
                "Box not initialized (does not contain value). Function call not allowed." )
            return reinterpret_cast<typename TFDecl::Signature>(function)
                       ( *this, std::forward<TArgs>(args)... );
        }

        /** ****************************************************************************************
         * Same as method #Call, but usable with interfaces that only accept a mutable
         * (aka not constant) box. Technically, the only difference between this method and \b Call
         * is that the latter is declared \c const.
         *
         * \note
         *   The only built-in boxing function that requires a mutable reference
         *   to a box, is function \alib{boxing,FClone}. This modifies the contents
         *   of a box by performing deep copies, with the goal to
         *   \ref alib_boxing_more_iclone "extent the lifecylce of boxes".
         *
         * @tparam TFDecl  The function type to call. Has to be explicitly specified.
         * @tparam TArgs   Types of the variadic arguments \p{args}. Do not need to be specified.
         * @param  args    Variadic arguments forwarded to the function.
         * @return Nothing in case that \p{TReturn} is void, otherwise the result of the invocation,
         *         respectively <c>TReturn()</c> if the requested function type was not found for
         *         this \b %Box.
         ******************************************************************************************/
        template <typename TFDecl,  typename... TArgs>
        decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(), std::declval<TArgs>()... ) )
        Call(TArgs&&... args)
        {
            ALIB_ASSERT_ERROR( vtable, "BOXING",
                "Box not initialized (does not contain value). Function call not allowed." )
            auto* func= GetFunction<TFDecl>( lang::Reach::Global ALIB_DBG(, true));
            if( func != nullptr )
                return reinterpret_cast<typename TFDecl::Signature>(func)
                       ( *this, std::forward<TArgs>(args)... );

            return  decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(),
                                                                          std::declval<TArgs>()... )) ();
        }

        /** ****************************************************************************************
         * Alternative version of non-constant version of method #Call, which accepts the function's
         * pointer as a first argument.
         * Such pointer can be received upfront with method #GetFunction.
         *
         * @tparam TFDecl    The function type to call. Has to be explicitly specified.
         * @tparam TArgs     Types of the variadic arguments \p{args}. Do not need to be specified.
         * @param  args      Variadic arguments forwarded to the function.
         * @param  function  The function to invoke.
         * @return Nothing in case that \p{TReturn} is void, otherwise the result of the invocation,
         *         respectively <c>TReturn()</c> if the requested function type was not found for
         *         this \b %Box.
         ******************************************************************************************/
        template <typename TFDecl,  typename... TArgs>
        decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(), std::declval<TArgs>()... ) )
        CallDirect(typename TFDecl::Signature function, TArgs &&... args)
        {
            ALIB_ASSERT_ERROR( vtable, "BOXING",
                "Box not initialized (does not contain value). Function call not allowed." )
            return reinterpret_cast<typename TFDecl::Signature>( function )
                       ( *this, std::forward<TArgs>(args)... );
        }

        /** ****************************************************************************************
         * Comparison operator. Returns the result of invocation of built-in boxing function
         * \alib{boxing,FEquals}.
         *
         * @param rhs The right hand side argument of the comparison.
         * @return \c true if this object equals \p{rhs}, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        bool  operator==(Box const& rhs)                                                      const;

        /** ****************************************************************************************
         * Comparison operator. Returns the negated result of #operator==.
         *
         * @param rhs The right hand side argument of the comparison.
         * @return \c true if this object equals \p{rhs}, \c false otherwise.
         ******************************************************************************************/
        bool  operator!=(const Box& rhs)                                                       const
        {
            return  ! ((*this) == rhs);
        }

        /** ****************************************************************************************
         * Comparison operator. Returns the result of invocation of built-in box-function
         * \alib{boxing,FIsLess}.
         *
         * \see
         *   Sample code provided with documentation of box-function \alib{boxing,FIsLess}.
         *
         * @param rhs The right hand side argument of the comparison.
         * @return \c true if this object is smaller than \p{rhs}, otherwise \c false.
         ******************************************************************************************/
        ALIB_API
        bool operator< (Box const& rhs)                                                       const;

        /** ****************************************************************************************
         * Comparison operator. Uses a combination of \c operator< and \c operator==.
         *
         * @param rhs The right hand side argument of the comparison.
         * @return \c true if this object is smaller than \p{rhs}, otherwise \c false.
         ******************************************************************************************/
        ALIB_API
        bool operator<=(Box const& rhs)                                                       const;

        /** ****************************************************************************************
         * Comparison operator. Uses a combination of \c operator< and \c operator==.
         *
         * @param rhs The right hand side argument of the comparison.
         * @return \c true if this object is smaller than \p{rhs}, otherwise \c false.
         ******************************************************************************************/
        ALIB_API
        bool operator> (Box const& rhs)                                                       const;

        /** ****************************************************************************************
         * Comparison operator. Returns the negated result of \c operator<.
         *
         * @param rhs The right hand side argument of the comparison.
         * @return \c true if this object is smaller than \p{rhs}, otherwise \c false.
         ******************************************************************************************/
        bool operator>= (Box const& rhs)                                                       const
        {
            return   !( (*this) < rhs);
        }

        /** ****************************************************************************************
         * Explicit cast operator to \c bool. Returns the result of built-in box-function
         * \alib{boxing,FIsTrue}.
         *
         * @return \c true if the boxed value <em>represents value true</em>, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        explicit operator bool()                                                     const;


        /** ****************************************************************************************
         * Returns the result of a call to built-in boxing function \alib{boxing,FIsNotNull}.
         *
         * @return \c false if this object represents a \e nulled object, \c true otherwise.
         ******************************************************************************************/
        ALIB_API
        bool   IsNotNull()                                                           const;

        /** ****************************************************************************************
         * Returns the negated result of a call to built-in boxing function
         * \alib{boxing,FIsNotNull}.
         *
         * @return \c true if this object represents a \e nulled object, \c false otherwise.
         ******************************************************************************************/
        bool     IsNull()                                                               const
        {
            return  !IsNotNull();
        }

        /** ****************************************************************************************
         * Returns the result of invocation of built-in boxing function \alib{boxing,FHashcode}.
         *
         * @return A hashcode for the boxed type and value.
         ******************************************************************************************/
        ALIB_API size_t Hashcode()                                                            const;

    #if ALIB_MONOMEM
        /** ****************************************************************************************
         * Returns the result of invocation of built-in boxing function \alib{boxing,FHashcode}.
         *
         * #### Availability ####
         * This method is available only if module \alib_monomem is included in the \alibdist.
         * @param memory  A monotonic allocator used for storing cloned data.
         ******************************************************************************************/
        ALIB_API void   Clone( monomem::MonoAllocator& memory );
    #endif

}; // class Box

}} // namespace [alib::boxing]


// For documentation, all operators are faked into namespace alib::boxing
#if defined(ALIB_DOX)
namespace alib {  namespace boxing {
#endif


#if defined(ALIB_DOX)
}} // namespace [alib::boxing]
#endif

#endif // HPP_ALIB_BOXING_BOX
