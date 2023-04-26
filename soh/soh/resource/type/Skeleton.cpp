#include "resource/ResourceMgr.h"
#include "Skeleton.h"
#include "soh/OTRGlobals.h"

namespace Ship {
void* Skeleton::GetPointer() {
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


void SkeletonPatcher::RegisterSkeleton(std::string path, SkelAnime* skelAnime) {
    SkeletonPatchInfo info;

    info.skelAnime = skelAnime;

    if (path.substr(0, 7) == "__OTR__")
        path = path.substr(7, path.size() - 7);

    // Determine if we're using an alternate skeleton
    if (path.substr(0, 3) == Ship::Resource::gAltAssetPrefix) {
        info.vanillaSkeletonPath = path.substr(3, path.size() - 3);
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

void SkeletonPatcher::UpdateSkeletons(bool isHD) {
    for (auto skel : skeletons) 
    {
        Skeleton* newSkel = nullptr;

        if (isHD)
            newSkel = (Skeleton*)OTRGlobals::Instance->context->GetResourceManager()
                          ->LoadResource(Ship::Resource::gAltAssetPrefix + skel.vanillaSkeletonPath)
                          .get();
        else
            newSkel = (Skeleton*)OTRGlobals::Instance->context->GetResourceManager()->LoadResource(skel.vanillaSkeletonPath).get();

        if (newSkel != nullptr)
            skel.skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
    }
}
} // namespace Ship
