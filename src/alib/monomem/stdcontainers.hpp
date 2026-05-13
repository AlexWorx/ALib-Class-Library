//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {
    /// Type alias in namespace #"%alib".
    template<typename T>
    using  StdVectorMA=     std::vector<T, StdMA<T>>;

    /// Type alias in namespace #"%alib".
    template<typename T>
    using  StdVectorPA=     std::vector<T, StdPA<T>>;

    /// Type alias in namespace #"%alib".
    template<typename T>
    using  StdListMA  =     std::list  <T, StdMA<T>>;

    /// Type alias in namespace #"%alib".
    template<typename T>
    using  StdListPA  =     std::list  <T, StdPA<T>>;

    /// Type alias in namespace #"%alib".
    template<typename T>
    using  StdDequeMA  =    std::deque <T, StdMA<T>>;

    /// Type alias in namespace #"%alib".
    template<typename T>
    using  StdDequePA  =    std::deque <T, StdPA<T>>;

} // namespace [alib]
