/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_enums of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ENUMS_RECORDBOOTSTRAP
#define HPP_ALIB_ENUMS_RECORDBOOTSTRAP 1

#if !defined(HPP_ALIB_SINGLETONS_SINGLETON)
#   include "alib/singletons/singleton.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)

#if !defined (HPP_ALIB_ENUMS_RECORDS)
#   include "alib/enums/records.hpp"
#endif
#if ALIB_STRINGS && !defined (HPP_ALIB_ENUMS_RECORDPARSER)
#   include "alib/enums/recordparser.hpp"
#endif

#if ALIB_STRINGS && !defined(HPP_ALIB_STRINGS_SUBSTRING)
#   include "alib/strings/substring.hpp"
#endif

#if ALIB_RESOURCES && !defined(HPP_ALIB_RESOURCES_RESOURCES)
#   include "alib/resources/resources.hpp"
#endif

#if ALIB_STRINGS && ALIB_FILESET_MODULES && !defined(HPP_ALIB_FS_MODULES_MODULE)
#   include "alib/lib/fs_modules/module.hpp"
#endif

#if ALIB_MONOMEM && !defined (HPP_ALIB_MONOMEM_MONOALLOCATOR)
#   include "alib/monomem/monoallocator.hpp"
#endif

#include <initializer_list>


namespace aworx { namespace lib { namespace enums {

#if !defined(ALIB_DOX)

template<typename TEnum, typename TEnableIf>
template <typename... TArgs>
inline
void EnumRecords<TEnum, TEnableIf>::Bootstrap( TEnum element, TArgs&&... args)              noexcept
{
    auto&  records= detail::EnumRecordHook<TEnum>::GetSingleton();
    auto** lastP  = records.getPointerToLast();
    #if ALIB_MONOMEM
        *lastP= monomem::GlobalAllocator.Emplace<typename detail::EnumRecordHook<TEnum>::Node>(
    #else
        *lastP= new typename detail::EnumRecordHook<TEnum>::Node(
    #endif
                     element,  std::forward<TArgs>(args)... );

    detail::setEnumRecord( typeid(TEnum),
                           UnderlyingIntegral(element),
                           &(*lastP)->record  );
    (*lastP)->next= nullptr;
}

template<typename TEnum, typename TEnableIf>
inline
void EnumRecords<TEnum, TEnableIf>::Bootstrap( std::initializer_list<EnumRecords::Initializer> definitions )
{
    auto* table= definitions.begin();
    auto&  records= detail::EnumRecordHook<TEnum>::GetSingleton();
    auto** lastP  = records.getPointerToLast();
    for( size_t i= 0; i!= definitions.size(); ++i)
    {
        #if ALIB_MONOMEM
            *lastP= monomem::GlobalAllocator.Emplace<typename detail::EnumRecordHook<TEnum>::Node>(
        #else
            *lastP= new typename detail::EnumRecordHook<TEnum>::Node(
        #endif
                                table[i].element, table[i].record );
        detail::setEnumRecord( typeid(TEnum),
                               UnderlyingIntegral(table[i].element),
                               &(*lastP)->record  );
        lastP= &(*lastP)->next;
    }

    (*lastP)= nullptr;
}

#if ALIB_STRINGS

    template<typename TEnum, typename TEnableIf>
    inline
    void EnumRecords<TEnum, TEnableIf>::Bootstrap( const String& input,
                                              character     innerDelim,
                                              character     outerDelim    )
    {
        EnumRecordParser::Initialize(input, innerDelim, outerDelim, NullNString(), NullNString() );

        auto& records= detail::EnumRecordHook<TEnum>::GetSingleton();
        auto** lastP = records.getPointerToLast();

        for(;;)
        {
            #if ALIB_MONOMEM
                auto* element=  (*lastP= monomem::GlobalAllocator.Emplace<typename detail::EnumRecordHook<TEnum>::Node>() );
            #else
                auto* element=  (*lastP= new typename detail::EnumRecordHook<TEnum>::Node() );
            #endif

            EnumRecordParser::Get( element->integral );
            element->record.Parse();

            detail::setEnumRecord( typeid(TEnum),  element->integral,
                                                  &element->record     );

            // next?
            lastP= &element->next;
            if( EnumRecordParser::Input.IsEmpty() )
                break;
            EnumRecordParser::OuterDelim();
        }
        EnumRecordParser::assertEndOfInput();
        (*lastP)= nullptr;
    }
#endif

#if ALIB_RESOURCES
    template<typename TEnum, typename TEnableIf>
    inline
    void EnumRecords<TEnum, TEnableIf>::Bootstrap( ResourcePool&   pool,
                                              const NString&  category,
                                              const NString&  name,
                                              character       innerDelim,
                                              character       outerDelim     )
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

            auto* element=  (*lastP= monomem::GlobalAllocator.Emplace<typename detail::EnumRecordHook<TEnum>::Node>() );

            EnumRecordParser::Get( element->integral );
            element->record.Parse();

            detail::setEnumRecord( typeid(TEnum),  element->integral,
                                                  &element->record     );

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
                    ALIB_ERROR( NString128()
                       << "Detected a \"gap\" in numbering of enum records for type <"
                       << DbgTypeDemangler( typeid(TEnum)).Get() << ">: From index "
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

    #if ALIB_FILESET_MODULES
        template<typename TEnum, typename TEnableIf>
        inline
        void EnumRecords<TEnum, TEnableIf>::Bootstrap( Module&        module,
                                                  const NString& name,
                                                  character      innerDelim,
                                                  character      outerDelim   )
        {
            Bootstrap( module.GetResourcePool(), module.ResourceCategory, name, innerDelim, outerDelim );
        }
    #endif
#endif

#endif // !defined(ALIB_DOX)

}}} // namespace [aworx::lib::enums]


#endif // HPP_ALIB_ENUMS_RECORDBOOTSTRAP

