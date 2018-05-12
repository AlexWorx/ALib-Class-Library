// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// Include guard
#ifndef HPP_ALIB_STRINGS_UTIL_STRINGMAP
#define HPP_ALIB_STRINGS_UTIL_STRINGMAP 1

namespace aworx { namespace lib { namespace strings { namespace util  {

/** ************************************************************************************************
 * Implements a hash functor for class \ref aworx::String, usable with types found in
 * namespace \b std.<br>
 * Instead of implementing \b std::hash inside namespace \b std, this struct can be
 * provided as template parameter, e.g. to \b std::unordered_map, for which a templated type
 * definition is provided for with \ref aworx::UnorderedStringMap.
 **************************************************************************************************/
struct std_StringHash
{
    /**
     * Calculates the hash code for class \b NString.
     * @param src The string object to hash.
     * @return The hash code.
     */
    size_t operator()(const String& src) const
    {
        integer result= 0xc70f6907UL;

        for (int i = 0; i < src.Length(); i++)
            result = 31*result + src.CharAt<false>(i++);

        return static_cast<size_t>( result );
    }
};

/** ************************************************************************************************
 * Implements a comparison functor for objects of class \ref aworx::String, usable with types
 * found in namespace \b std.<br>
 * Instead of implementing the operator inside namespace \b std, this struct can be
 * provided as template parameter, e.g. to \b std::unordered_map., for which a templated type
 * definition is provided for with \ref aworx::UnorderedStringMap.
 **************************************************************************************************/
struct std_StringEquals
{
    /**
     * Invokes \alib{strings,StringBase::Equals,String::Equals} on \p{lhs}, passing
     * \p{rhs}.
     * @param lhs The first string object.
     * @param rhs The second string object.
     * @return The hash code.
     */
    bool operator()(const String& lhs, const String& rhs) const
    {
        return lhs.Equals( rhs );
    }
};

/** ************************************************************************************************
 * Implements a case insensitive hash functor for class \ref aworx::String, usable with types
 * found in namespace \b std.<br>
 * Instead of implementing \b std::hash inside namespace \b std, this struct can be
 * provided as template parameter, e.g. to \b std::unordered_map, for which a templated type
 * definition is provided for with \ref aworx::UnorderedStringMapIgnoreCase.
 **************************************************************************************************/
struct std_StringHashIgnoreCase
{
    /**
     * Calculates the hash code for class \b String.
     * @param src The string object to hash.
     * @return The hash code.
     */
    size_t operator()(const String& src) const
    {
        integer result= 0xc70f6907UL;

        for (int i = 0; i < src.Length(); i++)
            result = 31*result + toupper(src.CharAt<false>(i++));

        return static_cast<size_t>( result );
    }
};

/** ************************************************************************************************
 * Implements a case insensitive comparison functor for objects of class \ref aworx::String,
 * usable with types found in namespace \b std.<br>
 * Instead of implementing the operator inside namespace \b std, this struct can be
 * provided as template parameter, e.g. to \b std::unordered_map., for which a templated type
 * definition is provided for with \ref aworx::UnorderedStringMapIgnoreCase.
 **************************************************************************************************/
struct std_StringEqualsIgnoreCase
{
    /**
     * Invokes \alib{strings,StringBase::Equals,String::Equals} on \p{lhs}, passing
     * \p{rhs}.
     * @param lhs The first string object.
     * @param rhs The second string object.
     * @return The hash code.
     */
    bool operator()(const String& lhs, const String& rhs) const
    {
        return lhs.Equals<Case::Ignore>( rhs );
    }
};

/** ************************************************************************************************
 * Implements a hash functor for class \ref aworx::String, usable with types found in
 * namespace \b std.<br>
 * Instead of implementing \b std::hash inside namespace \b std, this struct can be
 * provided as template parameter, e.g. to \b std::unordered_map, for which a templated type
 * definition is provided for with \ref aworx::UnorderedStringMap.
 **************************************************************************************************/
struct std_AStringHash
{
    /**
     * Calculates the hash code for class \b String.
     * @param src The string object to hash.
     * @return The hash code.
     */
    size_t operator()(const String& src) const
    {
        integer result= 0xc70f6907UL;

        for (int i = 0; i < src.Length(); i++)
            result = 31*result + src.CharAt<false>(i++);

        return static_cast<size_t>( result );
    }
};

/** ************************************************************************************************
 * Implements a comparison functor for objects of class \ref aworx::String, usable with types
 * found in namespace \b std.<br>
 * Instead of implementing the operator inside namespace \b std, this struct can be
 * provided as template parameter, e.g. to \b std::unordered_map., for which a templated type
 * definition is provided for with \ref aworx::UnorderedStringMap.
 **************************************************************************************************/
struct std_AStringEquals
{
    /**
     * Invokes \alib{strings,StringBase::Equals,String::Equals} on \p{lhs}, passing
     * \p{rhs}.
     * @param lhs The first string object.
     * @param rhs The second string object.
     * @return The hash code.
     */
    bool operator()(const String& lhs, const String& rhs) const
    {
        return lhs.Equals( rhs );
    }
};

/** ************************************************************************************************
 * Implements a case insensitive hash functor for class \ref aworx::String, usable with types
 * found in namespace \b std.<br>
 * Instead of implementing \b std::hash inside namespace \b std, this struct can be
 * provided as template parameter, e.g. to \b std::unordered_map, for which a templated type
 * definition is provided for with \ref aworx::UnorderedStringMapIgnoreCase.
 **************************************************************************************************/
struct std_AStringHashIgnoreCase
{
    /**
     * Calculates the hash code for class \b String.
     * @param src The string object to hash.
     * @return The hash code.
     */
    size_t operator()(const String& src) const
    {
        integer result= 0xc70f6907UL;

        for (int i = 0; i < src.Length(); i++)
            result = 31*result + toupper(src.CharAt<false>(i++));

        return static_cast<size_t>( result );
    }
};

/** ************************************************************************************************
 * Implements a case insensitive comparison functor for objects of class \ref aworx::String,
 * usable with types found in namespace \b std.<br>
 * Instead of implementing the operator inside namespace \b std, this struct can be
 * provided as template parameter, e.g. to \b std::unordered_map., for which a templated type
 * definition is provided for with \ref aworx::UnorderedStringMapIgnoreCase.
 **************************************************************************************************/
struct std_AStringEqualsIgnoreCase
{
    /**
     * Invokes \alib{strings,StringBase::Equals,String::Equals} on \p{lhs}, passing
     * \p{rhs}.
     * @param lhs The first string object.
     * @param rhs The second string object.
     * @return The hash code.
     */
    bool operator()(const String& lhs, const String& rhs) const
    {
        return lhs.Equals<Case::Ignore>( rhs );
    }
};

}}} // namespace aworx::[lib::strings::util]


/// Unordered map using a key of type \ref aworx::String.
template<typename TValue>
using     UnorderedStringMap           =   std::unordered_map<String, TValue,
                                                              lib::strings::util::std_StringHash,
                                                              lib::strings::util::std_StringEquals>;

/// Unordered map using a case insensitive key of type \ref aworx::String.
template<typename TValue>
using     UnorderedStringMapIgnoreCase =   std::unordered_map<String, TValue,
                                                             lib::strings::util::std_StringHashIgnoreCase,
                                                             lib::strings::util::std_StringEqualsIgnoreCase>;

/// Unordered map using a key of type \ref aworx::String.
template<typename TValue>
using     UnorderedAStringMap          =   std::unordered_map<AString, TValue,
                                                              lib::strings::util::std_AStringHash,
                                                              lib::strings::util::std_AStringEquals>;

/// Unordered map using a case insensitive key of type \ref aworx::String.
template<typename TValue>
using     UnorderedAStringMapIgnoreCase=   std::unordered_map<AString, TValue,
                                                             lib::strings::util::std_AStringHashIgnoreCase,
                                                             lib::strings::util::std_AStringEqualsIgnoreCase>;

} // namespace aworx

#endif // HPP_ALIB_STRINGS_UTIL_STRINGMAP
