#include <ship/resource/ResourceManager.h>
#include "Skeleton.h"
#include "2s2h/BenPort.h"

namespace SOH {
SkeletonData* Skeleton::GetPointer() {
    return &skeletonData;
}

size_t Skeleton::GetPointerSize() {
    switch (type) {
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
    static const std::string sOtr = "__OTR__";

    if (path.starts_with(sOtr)) {
        path = path.substr(sOtr.length());
    }

    std::string vanillaSkeletonPath;

    // Determine if we're using an alternate skeleton
    if (path.starts_with(Ship::IResource::gAltAssetPrefix)) {
        vanillaSkeletonPath = path.substr(Ship::IResource::gAltAssetPrefix.length(),
                                          path.size() - Ship::IResource::gAltAssetPrefix.length());
    } else {
        vanillaSkeletonPath = path;
    }

    for (auto& skel : skeletons) {
        if (skel.skelAnime == skelAnime) {
            skel.vanillaSkeletonPath = vanillaSkeletonPath;
            return;
        }
    }

    SkeletonPatchInfo info;
    info.skelAnime = skelAnime;
    info.vanillaSkeletonPath = vanillaSkeletonPath;
    skeletons.push_back(info);
}

void SkeletonPatcher::UnregisterSkeleton(SkelAnime* skelAnime) {

    // TODO: Should probably just use a dictionary here...
    for (size_t i = 0; i < skeletons.size(); i++) {
        auto skel = skeletons[i];

        if (skel.skelAnime == skelAnime) {
            skeletons.erase(skeletons.begin() + i);
            i--;
        }
    }
}
void SkeletonPatcher::ClearSkeletons() {
    skeletons.clear();
}

void SkeletonPatcher::UpdateSkeletons() {
    auto resourceMgr = Ship::Context::GetRawInstance()->GetResourceManager();
    bool isAlt = resourceMgr->IsAltAssetsEnabled();

    for (const auto& skel : skeletons) {
        auto newSkel = std::static_pointer_cast<Skeleton>(resourceMgr->LoadResource(
            (isAlt ? Ship::IResource::gAltAssetPrefix : "") + skel.vanillaSkeletonPath, true));

        if (newSkel == nullptr || skel.skelAnime == nullptr) {
            continue;
        }

        switch (newSkel->type) {
            case SkeletonType::Flex:
                skel.skelAnime->skeleton = newSkel->skeletonData.flexSkeletonHeader.sh.segment;
                skel.skelAnime->dListCount = newSkel->skeletonData.flexSkeletonHeader.dListCount;
                break;
            case SkeletonType::Normal:
                skel.skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
                break;
            case SkeletonType::Curve:
                skel.skelAnime->skeleton = reinterpret_cast<void**>(newSkel->skeletonData.skelCurveLimbList.limbs);
                break;
        }
    }
}
} // namespace SOH
