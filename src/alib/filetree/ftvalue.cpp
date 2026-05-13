
using namespace alib::system;
namespace alib::filetree {

void   FTValue::SetLinkTarget(FTree& tree, const PathString& target, const PathString& realTarget) {
    EISymLinkFile& ei= *static_cast<EISymLinkFile*>(extendedInfo);
    auto& pool= tree.Pool;

    // delete old values
    if( ei.RealTarget.Length() && ei.RealTarget.Buffer() != ei.Target.Buffer() )
        ei.RealTarget.Free(pool);
    ei.Target.Free(pool);

    // allocate target
    ei.Target.Allocate(pool, target);

    // real target is same?
    if( realTarget.Equals(target) )
        ei.RealTarget= ei.Target;
    else
        ei.RealTarget.Allocate(pool, realTarget);
}

const NString  OwnerAndGroupResolver::GetOwnerName( const FileStatus& fInfo )                const {
    #if !defined ( _WIN32)
        auto key= fInfo.Owner();
        auto resultPair= ownerCache.Try( key );
        if( !resultPair.first ) {
            auto* result= getpwuid(key);
            resultPair.second.Construct( key, NString(result ? result->pw_name : "?"));
        }
        return resultPair.second.Mapped();
    #else
        (void) fInfo;
        return "?";
    #endif
}

const NString  OwnerAndGroupResolver::GetGroupName( const FileStatus& fInfo )                const {
    #if !defined ( _WIN32)
        auto key= fInfo.Group();
        auto resultPair= groupCache.Try( key );
        if( !resultPair.first ) {
            auto* result= getgrgid(key);
            resultPair.second.Construct( key, NString(result ? result->gr_name : "?"));
        }
        return resultPair.second.Mapped();
    #else
        (void) fInfo;
        return "?";
    #endif
}
} // namespace [alib::filetree]
