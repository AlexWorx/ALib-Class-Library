//==================================================================================================
/// \file
/// This header file is part of module \alib_enums of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_ENUMS_RECORDBOOTSTRAP
#define HPP_ALIB_ENUMS_RECORDBOOTSTRAP 1
#pragma once
#include "alib/singletons/singleton.hpp"

ALIB_ASSERT_MODULE(ENUMS)

#include "alib/enums/records.hpp"
#include "alib/enums/recordparser.hpp"
#include "alib/strings/substring.hpp"
#if ALIB_CAMP
#   include "alib/lang/resources/resources.hpp"
#   include "alib/lang/basecamp/camp.hpp"
#endif
#if ALIB_MONOMEM
#   include "alib/monomem/globalallocator.hpp"
#endif

#include <initializer_list>

#include "alib/lang/callerinfo_functions.hpp"

namespace alib {  namespace enums {

#if !DOXYGEN

template<typename TEnum, typename TEnableIf>
template <typename... TArgs>
inline
void EnumRecords<TEnum, TEnableIf>::Bootstrap( TEnum element, TArgs&&... args)              noexcept
{
    auto&  records= detail::EnumRecordHook<TEnum>::GetSingleton();
    auto** lastP  = records.getPointerToLast();
    #if ALIB_MONOMEM
        *lastP= monomem::GLOBAL_ALLOCATOR()
    #else
        *lastP= HeapAllocator()()
    #endif
            .New<typename detail::EnumRecordHook<TEnum>::Node>(element,  std::forward<TArgs>(args)... );

    detail::setEnumRecord( typeid(TEnum), integer(element), &(*lastP)->record  );
    (*lastP)->next= nullptr;
}

template<typename TEnum, typename TEnableIf>
inline
void EnumRecords<TEnum, TEnableIf>::Bootstrap( std::initializer_list<EnumRecords::Initializer> definitions )
{
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    auto*  table  = definitions.begin();
    auto&  records= detail::EnumRecordHook<TEnum>::GetSingleton();
    auto** lastP  = records.getPointerToLast();
    for( size_t i= 0; i!= definitions.size(); ++i)
    {
        #if ALIB_MONOMEM
            *lastP= monomem::GLOBAL_ALLOCATOR()
        #else
            *lastP= HeapAllocator()()
        #endif
            .New<typename detail::EnumRecordHook<TEnum>::Node>(table[i].element, table[i].record );

        detail::setEnumRecord( typeid(TEnum), integer(table[i].element),  &(*lastP)->record  );
        lastP= &(*lastP)->next;
    }

    (*lastP)= nullptr;
    ALIB_WARNINGS_RESTORE
}

    template<typename TEnum, typename TEnableIf>
    inline
    void EnumRecords<TEnum, TEnableIf>::Bootstrap( const String& input,
                                                   character     innerDelim,
                                                   character     outerDelim    )
    {
        EnumRecordParser::Initialize(input, innerDelim, outerDelim, NULL_NSTRING, NULL_NSTRING );

        auto& records= detail::EnumRecordHook<TEnum>::GetSingleton();
        auto** lastP = records.getPointerToLast();

        for(;;)
        {
            #if ALIB_MONOMEM
                auto* element=  (*lastP= monomem::GLOBAL_ALLOCATOR()
            #else
                auto* element=  (*lastP= HeapAllocator()()
            #endif
                .New<typename detail::EnumRecordHook<TEnum>::Node>() );
            EnumRecordParser::Get( element->integral );
            element->record.Parse();

            detail::setEnumRecord( typeid(TEnum),  integer(element->integral), &element->record );

            // next?
            lastP= &element->next;
            if( EnumRecordParser::Input.IsEmpty() )
                break;
            EnumRecordParser::OuterDelim();
        }
        EnumRecordParser::assertEndOfInput();
        (*lastP)= nullptr;
    }

#if ALIB_CAMP
    template<typename TEnum, typename TEnableIf>
    inline
    void EnumRecords<TEnum, TEnableIf>::Bootstrap( lang::resources::ResourcePool& pool,
                                                   const NString&                 category,
                                                   const NString&                 name,
                                                   character                      innerDelim,
                                                   character                      outerDelim     )
    {
        // resources given in the standard, non-indexed way?
        String input= pool.Get( category, name  ALIB_DBG(, false) );
        if( input.IsNotNull() )
        {
            // Note:
            //  The parser is initialized here already. The "inner" call to Bootstrap() will not have
            //  the resource information otherwise.
            //  Double initialization is checked inside the parser's initialize method.
            //  (A little crude but OK!)
            EnumRecordParser::Initialize(input, innerDelim, outerDelim, category, name );
            Bootstrap( input, innerDelim, outerDelim );
            return;
        }

        // resources given as name0, name1, name2...
        NString64 nameNr( name);
        int nr= 0;
        auto&  records= detail::EnumRecordHook<TEnum>::GetSingleton();
        auto** lastP  = records.getPointerToLast();
        while(  (input= pool.Get( category, nameNr.Reset( name)._(nr) ALIB_DBG(, false))).IsNotNull()
               || nr== 0 )
        {
            EnumRecordParser::Initialize(input, innerDelim, outerDelim, category, nameNr );

            auto* element=  (*lastP=  monomem::GLOBAL_ALLOCATOR().New<typename detail::EnumRecordHook<TEnum>::Node>());

            EnumRecordParser::Get( element->integral );
            element->record.Parse();

            detail::setEnumRecord( typeid(TEnum), integer(element->integral), &element->record );

            EnumRecordParser::assertEndOfInput();
            // next
            lastP= &element->next;
            ++nr;
        }
        (*lastP)= nullptr;

        // check if there are more coming (a gap in numbered definition)
        #if ALIB_DEBUG
            for( int i= 0 ; i < 35 ; ++i )
            {
                ++nr;
                if( pool.Get( category, nameNr.Reset( name)._( nr) ALIB_DBG(, false)).IsNotNull() )
                {
                    ALIB_ERROR( "ENUMS", NString128()
                       << "Detected a \"gap\" in numbering of enum records for type <"
                       << lang::DbgTypeDemangler( typeid(TEnum)).Get() << ">: From index "
                       << nr - i - 1 << " to " << nr - 1 << ".\n"
                          " Resource category/name: " << category << '/' << name << "." )
                }
            }
        #endif

    }

    template<typename TEnum, typename TEnableIf>
    inline
    void EnumRecords<TEnum, TEnableIf>::Bootstrap( character innerDelim, character outerDelim )
    {
        static_assert( T_Resourced<TEnum>::value,
                       "No specialization for T_Resourced<TEnum> given. Method not applicable." );

        Bootstrap( *T_Resourced<TEnum>::Pool(),
                    T_Resourced<TEnum>::Category(),
                    T_Resourced<TEnum>::Name    (),
                    innerDelim, outerDelim                         );
    }

    template<typename TEnum, typename TEnableIf>
    inline
    void EnumRecords<TEnum, TEnableIf>::Bootstrap( lang::Camp&    module,
                                                   const NString&       name,
                                                   character            innerDelim,
                                                   character            outerDelim   )
    {
        Bootstrap( module.GetResourcePool(), module.ResourceCategory, name, innerDelim, outerDelim );
    }
#endif // ALIB_CAMP

#endif // !DOXYGEN

}} // namespace [alib::enums]

#include "alib/lang/callerinfo_methods.hpp"

#endif // HPP_ALIB_ENUMS_RECORDBOOTSTRAP

