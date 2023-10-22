#include "resource/ResourceManager.h"
#include "Skeleton.h"
#include "soh/OTRGlobals.h"
#include "libultraship/libultraship.h"
#include <variables.h>
#include <macros.h>
#include <soh_assets.h>
#include <objects/object_link_child/object_link_child.h>
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


void SkeletonPatcher::RegisterSkeleton(std::string& path, SkelAnime* skelAnime) {
    SkeletonPatchInfo info;

    info.skelAnime = skelAnime;

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
    bool isAlt = CVarGetInteger("gAltAssets", 0);
    for (auto skel : skeletons) 
    {
        Skeleton* newSkel =
            (Skeleton*)LUS::Context::GetInstance()->GetResourceManager()
                ->LoadResource((isAlt ? LUS::IResource::gAltAssetPrefix : "") + skel.vanillaSkeletonPath, true)
                .get();

        if (newSkel != nullptr) {
            skel.skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
            uintptr_t skelPtr = (uintptr_t)newSkel->GetPointer();
            memcpy(&skel.skelAnime->skeletonHeader, &skelPtr, sizeof(uintptr_t)); // Dumb thing that needs to be done because cast is not cooperating
        }
    }
}

void SkeletonPatcher::UpdateCustomSkeletons() {
    for (auto skel : skeletons) {
        UpdateTunicSkeletons(skel);
    }
}

void SkeletonPatcher::UpdateTunicSkeletons(SkeletonPatchInfo& skel) {
    std::string skeletonPath = "";

    // Check if this is one of Link's skeletons
    if (sOtr + skel.vanillaSkeletonPath == std::string(gLinkAdultSkel)) {
        // Check what Link's current tunic is
        switch (TUNIC_EQUIP_TO_PLAYER(CUR_EQUIP_VALUE(EQUIP_TUNIC))) {
            case PLAYER_TUNIC_KOKIRI:
                skeletonPath = std::string(gLinkAdultKokiriTunicSkel).substr(sOtr.length());
                break;
            case PLAYER_TUNIC_GORON:
                skeletonPath = std::string(gLinkAdultGoronTunicSkel).substr(sOtr.length());
                break;
            case PLAYER_TUNIC_ZORA:
                skeletonPath = std::string(gLinkAdultZoraTunicSkel).substr(sOtr.length());
                break;
            default:
                return;
        }

        UpdateCustomSkeletonFromPath(skeletonPath, skel);
    } else if (sOtr + skel.vanillaSkeletonPath == std::string(gLinkChildSkel)) {
        // Check what Link's current tunic is
        switch (TUNIC_EQUIP_TO_PLAYER(CUR_EQUIP_VALUE(EQUIP_TUNIC))) {
            case PLAYER_TUNIC_KOKIRI:
                skeletonPath = std::string(gLinkChildKokiriTunicSkel).substr(sOtr.length());
                break;
            case PLAYER_TUNIC_GORON:
                skeletonPath = std::string(gLinkChildGoronTunicSkel).substr(sOtr.length());
                break;
            case PLAYER_TUNIC_ZORA:
                skeletonPath = std::string(gLinkChildZoraTunicSkel).substr(sOtr.length());
                break;
            default:
                return;
        }

        UpdateCustomSkeletonFromPath(skeletonPath, skel);
    }
}

void SkeletonPatcher::UpdateCustomSkeletonFromPath(const std::string& skeletonPath, SkeletonPatchInfo& skel) {
    bool isAlt = CVarGetInteger("gAltAssets", 0);
    Skeleton* newSkel = nullptr;
    Skeleton* altSkel = nullptr;
    // Load new skeleton based on the custom model if it exists
    newSkel = 
        (Skeleton*)LUS::Context::GetInstance()
            ->GetResourceManager()
            ->LoadResource((isAlt ? LUS::IResource::gAltAssetPrefix : "") + skeletonPath, true)
            .get();

    // Change back to the original skeleton if no skeleton's were found
    if (newSkel == nullptr) {
        newSkel = 
            (Skeleton*)LUS::Context::GetInstance()
                ->GetResourceManager()
                ->LoadResource((isAlt ? LUS::IResource::gAltAssetPrefix : "") + skel.vanillaSkeletonPath, true)
                .get();
    }
    
    if (newSkel != nullptr) {
        skel.skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
        uintptr_t skelPtr = (uintptr_t)newSkel->GetPointer();
        memcpy(&skel.skelAnime->skeletonHeader, &skelPtr, sizeof(uintptr_t));
    }
}
} // namespace LUS
