#include "resource/ResourceManager.h"
#include "Skeleton.h"
#include "soh/OTRGlobals.h"
#include "libultraship/libultraship.h"
#include <variables.h>
#include <macros.h>
#include <soh_assets.h>
#include <objects/object_link_boy/object_link_boy.h>

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


void SkeletonPatcher::RegisterSkeleton(std::string& path, SkelAnime* skelAnime, Actor* actor) {
    SkeletonPatchInfo info;

    info.skelAnime = skelAnime;
    info.actor = actor;

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
    for (auto skel : skeletons) 
    {
        Skeleton* newSkel =
            (Skeleton*)LUS::Context::GetInstance()->GetResourceManager()
                ->LoadResource((isHD ? LUS::IResource::gAltAssetPrefix : "") + skel.vanillaSkeletonPath, true)
                .get();

        if (newSkel != nullptr)
            skel.skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
    }
}

void SkeletonPatcher::UpdateTunicSkeletons() {
    bool isHD = CVarGetInteger("gAltAssets", 0);
    for (auto skel : skeletons) {
        if (skel.actor != nullptr) // is there actually an actor to check?
        {
            if (skel.actor->id == 0) // is this Link?
            {
                Skeleton* newSkel;
                if (LINK_IS_ADULT && CUR_EQUIP_VALUE(EQUIP_TUNIC) - 1 == PLAYER_TUNIC_KOKIRI &&
                    ResourceGetIsCustomByName(gLinkKokiriSkel)) {
                    newSkel = (Skeleton*)LUS::Context::GetInstance()
                                  ->GetResourceManager()
                                  ->LoadResource((isHD ? LUS::IResource::gAltAssetPrefix : "") + gLinkKokiriSkel, true)
                                  .get();
                } else if (LINK_IS_ADULT && CUR_EQUIP_VALUE(EQUIP_TUNIC) - 1 == PLAYER_TUNIC_GORON &&
                           ResourceGetIsCustomByName(gLinkGoronSkel)) {
                    newSkel = (Skeleton*)LUS::Context::GetInstance()
                                  ->GetResourceManager()
                                  ->LoadResource((isHD ? LUS::IResource::gAltAssetPrefix : "") + gLinkGoronSkel, true)
                                  .get();
                } else if (LINK_IS_ADULT && CUR_EQUIP_VALUE(EQUIP_TUNIC) - 1 == PLAYER_TUNIC_ZORA &&
                           ResourceGetIsCustomByName(gLinkZoraSkel)) {
                    newSkel = (Skeleton*)LUS::Context::GetInstance()
                                  ->GetResourceManager()
                                  ->LoadResource((isHD ? LUS::IResource::gAltAssetPrefix : "") + gLinkZoraSkel, true)
                                  .get();
                } else { // child link, no model available
                    newSkel = (Skeleton*)LUS::Context::GetInstance()
                                  ->GetResourceManager()
                                  ->LoadResource(
                                      (isHD ? LUS::IResource::gAltAssetPrefix : "") + skel.vanillaSkeletonPath, true)
                                  .get();
                }

                if (newSkel != nullptr)
                    skel.skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
            }
        }
    }
}
} // namespace LUS
