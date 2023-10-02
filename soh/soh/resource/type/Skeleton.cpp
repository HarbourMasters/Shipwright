#include "resource/ResourceManager.h"
#include "Skeleton.h"
#include "soh/OTRGlobals.h"
#include "libultraship/libultraship.h"

namespace LUS {
SkeletonData* Skeleton::GetPointer() {
    return &skeletonData;
}

size_t Skeleton::GetPointerSize() {
    switch(type) {
        case SkeletonType::Normal:
            return sizeof(skeletonData.skeletonHeader);
        case SkeletonType::Flex:
            return sizeof(skeletonData.flexSkeletonHeader);
        case SkeletonType::Curve:
            return sizeof(skeletonData.skelCurveLimbList);
        default:
            return 0;
    }
}

std::vector<SkeletonPatchInfo> SkeletonPatcher::skeletons;


void SkeletonPatcher::RegisterSkeleton(std::string& path, SkelAnime* skelAnime) {
    SkeletonPatchInfo info;

    info.skelAnime = skelAnime;

    static const std::string sOtr = "__OTR__";

    if (path.starts_with(sOtr)) {
        path = path.substr(sOtr.length());
    }

    // Determine if we're using an alternate skeleton
    if (path.starts_with(LUS::IResource::gAltAssetPrefix)) {
        info.vanillaSkeletonPath = path.substr(LUS::IResource::gAltAssetPrefix.length(),
                                               path.size() - LUS::IResource::gAltAssetPrefix.length());
    } else {
        info.vanillaSkeletonPath = path;
    }

    skeletons.push_back(info);
}

void SkeletonPatcher::UnregisterSkeleton(SkelAnime* skelAnime) {

    // TODO: Should probably just use a dictionary here...
    for (int i = 0; i < skeletons.size(); i++) 
    {
        auto skel = skeletons[i];

        if (skel.skelAnime == skelAnime) {
            skeletons.erase(skeletons.begin() + i);
            break;
        }
    }
}
void SkeletonPatcher::ClearSkeletons() 
{
    skeletons.clear();
}

void SkeletonPatcher::UpdateSkeletons() {
    bool isHD = CVarGetInteger("gAltAssets", 0);
    for (auto skel : skeletons) {
        Skeleton* newSkel =
            (Skeleton*)LUS::Context::GetInstance()->GetResourceManager()
                ->LoadResource((isHD ? LUS::IResource::gAltAssetPrefix : "") + skel.vanillaSkeletonPath, true)
                .get();

        if (newSkel != nullptr) {
            skel.skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
            uintptr_t skelPtr = (uintptr_t)newSkel->GetPointer();
            memcpy(&skel.skelAnime->skeletonHeader, &skelPtr, sizeof(uintptr_t)); // Dumb thing that needs to be done because cast is not cooperating
        }
    }
}
} // namespace LUS
