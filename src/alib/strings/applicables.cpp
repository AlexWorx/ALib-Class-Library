// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
    #include "alib/strings/numberformat.hpp"
#endif



namespace aworx { namespace lib { namespace strings {

// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::applyto to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)

/**
 * \attention
 *   This is a non-existing namespace! It is exclusively defined for the
 *   [documentation parser](http://www.stack.nl/~dimitri/doxygen).
 *
 * \attention
 *   In this <b>"documentation namespace"</b>, you will find specializations of templated namespace
 *   functions and types of namespace
 *   \ref aworx::lib::strings. This is of-course wrong in respect to the C++ language
 *   definition, which requires such specializations to be placed in the original namespace.<br>
 *   It was decided to 'trick' the documentation parser to
 *   show these specialization in this namespace, to keep namespace \b %aworx::lib::strings
 *   clean and to have all specializations in one place.<br>
 *   In other words: All types and functions described in this namespace are residing in namespace
 *   #aworx::lib::strings.
 */
namespace applyto {
#endif

// #################################################################################################
// bool, std::type_info
// #################################################################################################
template<typename TChar>
integer T_Apply<bool,TChar>::Apply( AStringBase<TChar>& target, bool& b )
{
    target.template _<false>( lib::STRINGS.Get( b ? ASTR("APPLY_T") : ASTR("APPLY_F") ));
    return 1;
}

#if ALIB_DEBUG
template<typename TChar>
integer T_Apply<std::type_info, TChar>::Apply( AStringBase<TChar>& target, const std::type_info& type )
{
    target.template _<false>( debug::TypeDemangler( type ).Get() );
    return 1;
}
#endif

// #################################################################################################
// Tab()
// #################################################################################################
template<typename TChar>
integer T_Apply<typename FormatBase<TChar>::Tab, TChar>::Apply( AStringBase<TChar>& target, const typename FormatBase<TChar>::Tab& tab)
{
    integer reference= tab.reference;
    if (reference < 0 )
    {
        // search backwards
        reference= target.template LastIndexOfAny<Inclusion::Include>( StringConstants<TChar>::NewLine, target.Length() -1 );
        if ( reference < 0 )
            reference= 0;
        else
        {
            // if new line has more than one character (windows) we have to now search the first
            // character that is not in newline
            reference= target.template IndexOfAny<Inclusion::Exclude, false>( StringConstants<TChar>::NewLine, reference );
            if (reference < 0 )
                reference= target.Length();

        }
    }
    integer length=   target.Length();
    integer qtyChars= tab.minPad > 0 ? tab.minPad : 0;

    if ( tab.tabSize > 1 )
        qtyChars+= (tab.tabSize - ( (length + qtyChars - reference) % tab.tabSize ) ) % tab.tabSize;

    if ( qtyChars <= 0 )
        return 0;

    target.template InsertChars<false>( tab.tabChar, qtyChars );
    return qtyChars;
}


// #################################################################################################
// Field()
// #################################################################################################
template<typename TChar>
integer T_Apply<typename FormatBase<TChar>::Field, TChar>::Apply( AStringBase<TChar>& target, const typename FormatBase<TChar>::Field& field)
{

#if ALIB_MODULE_BOXING
    String theContent;

    // buffer apply given box into (if none string)
    String128 noneStringArgBuf;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE(noneStringArgBuf,  ReplaceExternalBuffer );

    // string type box given?
    if( field.theContent.template IsType<String>() )
        theContent= field.theContent.template Unbox<String>();
    else
    {
        // write box into local buffer
        noneStringArgBuf << field.theContent;
        theContent= noneStringArgBuf;
    }

#else
    StringBase<TChar> theContent= field.theContent;
#endif

    integer width=    field.theWidth;
    integer padSize=  field.theWidth
                       - theContent.WStringLength();

    // check pad field.width
    if (padSize <= 0 || field.theAlignment == Alignment::Left )
    {
                                target.template _          <false>( theContent );
        if (padSize > 0 )       target.template InsertChars<false>( field.padChar, padSize );
        return width;
    }

    // align Right
    if ( field.theAlignment == Alignment::Right )
    {
        if( padSize > 0 )
            target.template InsertChars<false>( field.padChar, padSize );
        target.template Apply<false>( theContent );
        return width;
    }

    // align Center
    integer leftPadding= padSize / 2;
    if( leftPadding > 0 )
        target.template InsertChars<false> ( field.padChar, leftPadding  );
    target.template Apply<false> ( theContent );
    if( padSize > leftPadding ) target.template InsertChars<false> ( field.padChar, padSize - leftPadding );

    return width;
}

// #################################################################################################
// Escape()
// #################################################################################################
template<typename TChar>
integer T_Apply<typename FormatBase<TChar>::Escape, TChar>::Apply( AStringBase<TChar>& target, const typename FormatBase<TChar>::Escape& escape)
{
    if( target.AdjustRegion( const_cast<typename FormatBase<TChar>::Escape&>(escape).startIdx,
                             const_cast<typename FormatBase<TChar>::Escape&>(escape).length   ) )
        return 0;

    integer regionEnd= escape.startIdx + escape.length;

    //
    // To escape sequences
    //
    if (escape.pSwitch == Switch::On)
    {
        for( integer idx= escape.startIdx; idx < regionEnd ; ++idx )
        {
            TChar c= target.CharAt(idx);

            TChar resultChar= '\0';
            switch(c)
            {
                case '\\' : resultChar= '\\'; break;
                case '\r' : resultChar= 'r' ; break;
                case '\n' : resultChar= 'n' ; break;
                case '\t' : resultChar= 't' ; break;
                case '\a' : resultChar= 'a' ; break;
                case '\b' : resultChar= 'b' ; break;
                case '\v' : resultChar= 'v' ; break;
                case '\f' : resultChar= 'f' ; break;
             // case '\e' : resultChar= 'e' ; break; Not C++ standard
                case '"'  : resultChar= '"' ; break;

                default   :                   break;
            }

            if( resultChar != '\0')
            {
                target.template InsertChars<false>('\\', 1, idx);
                target[++idx]= resultChar;
                regionEnd++;
            }
        }
    }

    //
    // Un-escape escape sequences
    //
    else
    {
        regionEnd--; // we can go 1 over it!
        for( integer idx= escape.startIdx; idx < regionEnd ; ++idx )
        {
            TChar c= target.CharAt(idx);
            if( c != '\\' )
                continue;

            c= target.CharAt(idx + 1);

            TChar resultChar= '\0';
            switch(c)
            {
                case '\\' : resultChar= '\\'; break;
                case 'r'  : resultChar= '\r' ; break;
                case 'n'  : resultChar= '\n' ; break;
                case 't'  : resultChar= '\t' ; break;
                case 'a'  : resultChar= '\a' ; break;
                case 'b'  : resultChar= '\b' ; break;
                case 'v'  : resultChar= '\v' ; break;
                case 'f'  : resultChar= '\f' ; break;
             // case 'e'  : resultChar= '\e' ; break; Not C++ standard
                case '"'  : resultChar= '"' ; break;

                default   :                   break;
            }

            if( resultChar != '\0')
            {
                target.Delete( idx, 1);
                target[idx]= resultChar;
                regionEnd--;
            }
        }
    }


    return 1;
}

// #################################################################################################
// Integers
// #################################################################################################
template<typename TChar>
integer T_Apply<FormatBase<TChar>, TChar>::Apply( AStringBase<TChar>& target, const FormatBase<TChar>& fmt )
{

    const NumberFormatBase<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &NumberFormatBase<TChar>::Computational;

    target.EnsureRemainingCapacity( fmt.valueType== 3 ?  48  //float: 2x15 + '.' + ',' + sign + fear
                                                      :  28  //int:   20 digits, grouping symbol, sign and what have you
                                  );

    integer length=      target.Length();
    integer oldLength=   length;


    length=  fmt.valueType == 1 ?  nf->WriteDecSigned  ( fmt.v.sInt, target.VBuffer(), length, fmt.theWidth  ) :
             fmt.valueType == 2 ?  nf->WriteDecUnsigned( fmt.v.uInt, target.VBuffer(), length, fmt.theWidth  ) :
                                   nf->WriteFloat      ( fmt.v.fp,   target.VBuffer(), length, fmt.theWidth  );

    target.template SetLength<false>( length );

    return length-oldLength;
}



template<typename TChar>
integer T_Apply<typename FormatBase<TChar>::Bin, TChar>::Apply( AStringBase<TChar>& target,  const typename FormatBase<TChar>::Bin& fmt )
{
    NumberFormatBase<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &NumberFormatBase<TChar>::Computational;

    target.EnsureRemainingCapacity( 80 );

    integer length=      target.Length();
    integer oldLength=   length;

    length= nf->WriteBin( fmt.theValue, target.VBuffer(), length,  fmt.theWidth );

    target.template SetLength<false>( length );

    return length-oldLength;
}

template<typename TChar>
integer T_Apply<typename FormatBase<TChar>::Hex, TChar>::Apply( AStringBase<TChar>& target,  const typename FormatBase<TChar>::Hex& fmt )
{
    NumberFormatBase<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &NumberFormatBase<TChar>::Computational;

    target.EnsureRemainingCapacity( 25 );

    integer length=      target.Length();
    integer oldLength=   length;

    length= nf->WriteHex( fmt.theValue, target.VBuffer(), length,  fmt.theWidth );

    target.template SetLength<false>( length );

    return length-oldLength;
}

template<typename TChar>
integer T_Apply<typename FormatBase<TChar>::Oct, TChar>::Apply( AStringBase<TChar>& target,  const typename FormatBase<TChar>::Oct& fmt )
{
    NumberFormatBase<TChar>* nf= fmt.nf;
    if( nf == nullptr )
        nf= &NumberFormatBase<TChar>::Computational;

    target.EnsureRemainingCapacity( 30 );

    integer length=      target.Length();
    integer oldLength=   length;

    length= nf->WriteOct( fmt.theValue, target.VBuffer(), length,  fmt.theWidth );

    target.template SetLength<false>( length );

    return length-oldLength;
}


// #################################################################################################
// NAString instantiations
// #################################################################################################
template integer T_Apply<bool                    , char>::Apply( AStringBase<char>&,       bool&                     );
template integer T_Apply<FormatBase<char>        , char>::Apply( AStringBase<char>&, const FormatBase<char>&         );
template integer T_Apply<FormatBase<char>::Tab   , char>::Apply( AStringBase<char>&, const FormatBase<char>::Tab&    );
template integer T_Apply<FormatBase<char>::Field , char>::Apply( AStringBase<char>&, const FormatBase<char>::Field&  );
template integer T_Apply<FormatBase<char>::Escape, char>::Apply( AStringBase<char>&, const FormatBase<char>::Escape& );
template integer T_Apply<FormatBase<char>::Bin   , char>::Apply( AStringBase<char>&, const FormatBase<char>::Bin&    );
template integer T_Apply<FormatBase<char>::Hex   , char>::Apply( AStringBase<char>&, const FormatBase<char>::Hex&    );
template integer T_Apply<FormatBase<char>::Oct   , char>::Apply( AStringBase<char>&, const FormatBase<char>::Oct&    );
#if ALIB_DEBUG
template integer T_Apply<std::type_info          , char>::Apply( AStringBase<char>&, const std::type_info&           );
#endif


// #################################################################################################
// WAString instantiations
// #################################################################################################
template integer T_Apply<bool                       , wchar_t>::Apply( AStringBase<wchar_t>&,       bool&                        );
template integer T_Apply<FormatBase<wchar_t>        , wchar_t>::Apply( AStringBase<wchar_t>&, const FormatBase<wchar_t>&         );
template integer T_Apply<FormatBase<wchar_t>::Tab   , wchar_t>::Apply( AStringBase<wchar_t>&, const FormatBase<wchar_t>::Tab&    );
template integer T_Apply<FormatBase<wchar_t>::Field , wchar_t>::Apply( AStringBase<wchar_t>&, const FormatBase<wchar_t>::Field&  );
template integer T_Apply<FormatBase<wchar_t>::Escape, wchar_t>::Apply( AStringBase<wchar_t>&, const FormatBase<wchar_t>::Escape& );
template integer T_Apply<FormatBase<wchar_t>::Bin   , wchar_t>::Apply( AStringBase<wchar_t>&, const FormatBase<wchar_t>::Bin&    );
template integer T_Apply<FormatBase<wchar_t>::Hex   , wchar_t>::Apply( AStringBase<wchar_t>&, const FormatBase<wchar_t>::Hex&    );
template integer T_Apply<FormatBase<wchar_t>::Oct   , wchar_t>::Apply( AStringBase<wchar_t>&, const FormatBase<wchar_t>::Oct&    );
#if ALIB_DEBUG
template integer T_Apply<std::type_info             , wchar_t>::Apply( AStringBase<wchar_t>&, const std::type_info&              );
#endif


// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::applyto to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
}
#endif


}}}// namespace [aworx::lib::strings]
