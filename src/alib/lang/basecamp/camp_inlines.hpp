/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_basecamp of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_CAMP_INLINES
#define HPP_ALIB_LANG_CAMP_INLINES 1

#if !defined(HPP_ALIB_LANG_CAMP)
#   include "alib/lang/basecamp/camp.hpp"
#endif

#if !defined (HPP_ALIB_LANG_RESOURCES_RESOURCES)
#   include "alib/lang/resources/resources.hpp"
#endif

namespace alib::lang {
inline void          Camp::BootstrapResource( const NString& name, const String& data ) {  resourcePool->Bootstrap( ResourceCategory, name, data ); }
inline const String& Camp::GetResource      ( const NString& name                     ) {  return resourcePool->Get( ResourceCategory, name   ALIB_DBG(, true)  ); }
inline const String& Camp::TryResource      ( const NString& name                     ) {  return resourcePool->Get( ResourceCategory, name  ALIB_DBG(, false)  ); }
} // namespace [alib::lang]

#endif // HPP_ALIB_LANG_CAMP_INLINES
