namespace alib {

/// This is the reference documentation module \alib_resources.<br>
/// Extensive documentation for this namespace is provided with the
/// #"alib_mod_resources;Programmer's Manual" of that module.
namespace resources {

#if ALIB_DEBUG_RESOURCES
std::ostream* LocalResourcePool::DbgResourceLoadObserver= nullptr;
#endif

bool LocalResourcePool::BootstrapAddOrReplace( const NString& category,
                                               const NString& name,
                                               const  String& resource        ) {

#if !ALIB_DEBUG_RESOURCES
    auto it= data.InsertOrAssign( detail::Key {category, name},   resource      );
#else
    auto it= data.InsertOrAssign( detail::Key {category, name}, { resource, 0 } );
    if( DbgResourceLoadObserver ) {
        (*DbgResourceLoadObserver) << (it.second ? "Adding Resource: " : "Replacing Resource: " )
                                   << category
                                   << "/"              << name << "=" << resource << std::endl;
    }
#endif

    return !it.second;
}

void LocalResourcePool::BootstrapBulk( const nchar* category, ... ) {
    // find / create category
    detail::Key key {category, nullptr};

    va_list args;
    va_start(args, category);
    for(;;) {
        key.Name= NString( va_arg( args, const nchar* ) );
        if( key.Name.IsNull() )
            break;

        String val = va_arg( args, const character* );
        ALIB_ASSERT_ERROR( key.Name.IndexOf(' ') < 0, "RESOURCES",
                   "Resource key name contains spaces: {} / {} = val", category, key.Name, val )
        ALIB_ASSERT_ERROR( key.Name.IndexOf(' ') <= 50, "RESOURCES",
                           "Resource key length > 50: {} / {} = val",  category, key.Name, val )
        #if ALIB_DEBUG_RESOURCES
            if( DbgResourceLoadObserver )
                (*DbgResourceLoadObserver) << "Bulk Resource: " << category
                                           << "/" << key.Name << "=" << val << std::endl;
        #endif

        #if ALIB_DEBUG_RESOURCES
            auto it= data.Find(key);
            if (it!=data.end())
                    ALIB_MESSAGE( "RESOURCES", "Replacing resource \"{}\" in category \"{}\".\n"
                                             "  Old value: \"{}\"\n"
                                             "  New value: \"{}\"",
                                        key.Name, category,  it->second, val )
        #endif
        #if !ALIB_DEBUG_RESOURCES
            data.EmplaceOrAssign( key, val );
        #else
            data.EmplaceOrAssign( key, std::make_pair(val,0) );
        #endif
    }
    va_end(args);
}


const String& LocalResourcePool::Get( const NString& category, const NString& name
                                      ALIB_DBG(, bool dbgAssert ) ) {
    // search main map
    auto dataIt=  data.Find( detail::Key { category, name } );
    if( dataIt != data.end() ) {
#if !ALIB_DEBUG_RESOURCES
        return dataIt.Mapped();
#else
               dataIt.Mapped().second++;
        return dataIt.Mapped().first;
#endif
    }
    ALIB_ASSERT_ERROR( !dbgAssert, "RESOURCES",
       "Missing resource \"{}\" in category: \"{}\"", name, category )
    return NULL_STRING;

}


#if ALIB_DEBUG_RESOURCES

std::vector<std::tuple<NString, NString, String, integer>>
ResourcePool::DbgGetList() {
    ALIB_WARNING( "STRINGS",
             "ResourcePool::DbgGetList was not overridden by the ResourcePool type set. "
             "Note that type built-in ALib type LocalResourcePool does provide an implementation." )

    return std::vector<std::tuple<NString, NString, String, integer>>();
}

std::vector<std::pair<NString, integer>>
ResourcePool::DbgGetCategories() {
    ALIB_WARNING( "STRINGS",
             "ResourcePool::DbgGetCategories was not overridden by the ResourcePool type set. "
             "Note that type built-in ALib type LocalResourcePool does provide an implementation." )

    return std::vector<std::pair<NString, integer>>();
}

std::vector<std::tuple<NString, NString, String, integer>>
LocalResourcePool::DbgGetList() {
    std::vector<std::tuple<NString, NString, String, integer>> result;

    result.reserve( size_t( data.Size() ) );
    for( auto& it : data ) {
        result.emplace_back(
                             it.first.Category,
                             it.first.Name,
                             it.second.first,
                             it.second.second   );
    }

    std::sort( result.begin(), result.end(),
               [] (const std::tuple<NString, NString, String, integer>& a,
                   const std::tuple<NString, NString, String, integer>& b )
               {

                    auto comp= std::get<0>(a).template CompareTo<CHK, lang::Case::Ignore>( std::get<0>(b) );
                    if( comp != 0 )
                        return comp < 0;

                    return     std::get<1>(a).template CompareTo<CHK, lang::Case::Ignore>( std::get<1>(b) ) < 0;
               }
    );
    return result;
}

std::vector<std::pair<NString, integer>>
LocalResourcePool::DbgGetCategories() {
    std::vector<std::pair<NString, integer>> result;

    auto list= DbgGetList();
    NString lastCat= nullptr;
    for( auto& entry : list ) {
        if( !lastCat.Equals( std::get<0>(entry)  ) ) {
            lastCat=  std::get<0>(entry);
            result.push_back( { std::get<0>(entry), 0 } );
        }
        ++result.back().second;
    }

    return result;
}

#endif // ALIB_DEBUG_RESOURCES

}} // namespace [alib::resources]
