#include "resource/ResourceManager.h"
#include "Skeleton.h"
#include "soh/OTRGlobals.h"
#include "libultraship/libultraship.h"
#include <variables.h>
#include <macros.h>
#include <soh_assets.h>
#include <objects/object_link_boy/object_link_boy.h>

#define OTR_OFFSET 7

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
    bool isAlt = CVarGetInteger("gAltAssets", 0);
    for (auto skel : skeletons) 
    {
        Skeleton* newSkel =
            (Skeleton*)LUS::Context::GetInstance()->GetResourceManager()
                ->LoadResource((isAlt ? LUS::IResource::gAltAssetPrefix : "") + skel.vanillaSkeletonPath, true)
                .get();

        if (newSkel != nullptr)
            skel.skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
    }
}

void SkeletonPatcher::UpdateTunicSkeletons() {
    bool isAlt = CVarGetInteger("gAltAssets", 0);
    const char* altPrefix = isAlt ? LUS::IResource::gAltAssetPrefix.c_str() : "";
    for (auto skel : skeletons) {
        // Check if this is Link's skeleton
        if (strcmp(skel.vanillaSkeletonPath.c_str(), &gLinkAdultSkel[OTR_OFFSET]) == 0) {
            Skeleton* newSkel = nullptr;
            Skeleton* altSkel = nullptr;
            std::string skeletonPath = "";
            // Check what Link's current tunic is
            switch (CUR_EQUIP_VALUE(EQUIP_TUNIC) - 1) {
                case PLAYER_TUNIC_KOKIRI:
                    skeletonPath = gLinkKokiriTunicSkel;
                    break;
                case PLAYER_TUNIC_GORON:
                    skeletonPath = gLinkGoronTunicSkel;
                    break;
                case PLAYER_TUNIC_ZORA:
                    skeletonPath = gLinkZoraTunicSkel;
                    break;
            }
            // Load new skeleton based on Link's tunic if it exists
            newSkel = 
                (Skeleton*)LUS::Context::GetInstance()
                    ->GetResourceManager()
                    ->LoadResource(skeletonPath, true)
                    .get();

            // If alt assets are on, look for alt tagged skeletons
            if (isAlt) {
                altSkel = 
                    (Skeleton*)LUS::Context::GetInstance()
                        ->GetResourceManager()
                        ->LoadResource(LUS::IResource::gAltAssetPrefix + skeletonPath, true)
                        .get();
            }

            // Override non-alt skeleton if necessary
            if (altSkel != nullptr && isAlt) {
                newSkel = altSkel;
            }

            // Change back to the original Link skeleton if no skeleton's were found
            if (newSkel == nullptr) {
                newSkel = 
                    (Skeleton*)LUS::Context::GetInstance()
                        ->GetResourceManager()
                        ->LoadResource(skel.vanillaSkeletonPath, true)
                        .get();
            }
            
            if (newSkel != nullptr) {
                skel.skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
            }
        }
    }
}
} // namespace LUS
