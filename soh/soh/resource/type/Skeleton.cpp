#include <ship/resource/ResourceManager.h>
#include <array>
#include <string>
#include <string_view>
#include "Skeleton.h"
#include "soh/OTRGlobals.h"
#include "libultraship/libultraship.h"
#include "soh/ResourceManagerHelpers.h"
#include <soh_assets.h>
#include <objects/object_link_child/object_link_child.h>
#include <objects/object_link_boy/object_link_boy.h>
#include "macros.h"

extern "C" {
#include "variables.h"
#include "z64.h"
#include "z64player.h"
extern PlayState* gPlayState;
extern Gfx* gPlayerLeftHandBgsDLs[8];
extern Gfx* gPlayerLeftHandOpenDLs[4];
extern Gfx* gPlayerLeftHandClosedDLs[4];
extern Gfx* sPlayerLeftHandSwordDLs2[4];
extern Gfx* sPlayerLeftHandSwordDLs[4];
extern Gfx* sPlayerRightHandOpenDLs[4];
extern Gfx* sPlayerRightHandClosedDLs[4];
extern Gfx* sPlayerRightHandShieldDLs[PLAYER_SHIELD_MAX * 4];
extern Gfx* sPlayerRightHandBowSlingshotDLs[4];
extern Gfx* sPlayerRightHandBowSlingshotDLs2[4];
extern Gfx* sPlayerRightHandOcarinaDLs[4];
extern Gfx* sPlayerRightHandOotDLs[4];
extern Gfx* sPlayerRightHandHookshotDLs[4];
extern Gfx* sPlayerLeftHandHammerDLs[4];
extern Gfx* gPlayerLeftHandBoomerangDLs[4];
extern Gfx* sPlayerLeftHandBottleDLs[4];
extern Gfx* sFirstPersonLeftForearmDLs[2];
extern Gfx* sFirstPersonLeftHandDLs[2];
extern Gfx* sFirstPersonForearmDLs[2];
extern Gfx* sFirstPersonRightHandHoldingWeaponDLs[2];
}

extern "C" SaveContext gSaveContext;
extern "C" u16 gEquipMasks[4];
extern "C" u8 gEquipShifts[4];

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

namespace {
constexpr std::string_view sAdultLinkObjectPath = "__OTR__objects/object_link_boy/";
constexpr std::string_view sChildLinkObjectPath = "__OTR__objects/object_link_child/";
constexpr std::string_view sAdultGoronLinkObjectPath = "__OTR__objects/object_link_boy_goron/";
constexpr std::string_view sChildGoronLinkObjectPath = "__OTR__objects/object_link_child_goron/";
constexpr std::string_view sAdultZoraLinkObjectPath = "__OTR__objects/object_link_boy_zora/";
constexpr std::string_view sChildZoraLinkObjectPath = "__OTR__objects/object_link_child_zora/";

bool TunicHandDisplayListExists(const char* path) {
    if (path == nullptr) {
        return false;
    }

    return ResourceMgr_FileExists(path) || ResourceGetIsCustomByName(path) ||
           (ResourceMgr_IsAltAssetsEnabled() && ResourceMgr_FileAltExists(path));
}

std::string_view GetDisplayListName(std::string_view path) {
    return path.substr(path.find_last_of('/') + 1);
}

constexpr std::string_view GetTunicObjectPath(std::string_view vanillaPath, s8 currentTunic) {
    const bool isAdult = vanillaPath.starts_with(sAdultLinkObjectPath);
    const bool isChild = vanillaPath.starts_with(sChildLinkObjectPath);
    if (!isAdult && !isChild) {
        return {};
    }

    switch (currentTunic) {
        case PLAYER_TUNIC_GORON:
            return isAdult ? sAdultGoronLinkObjectPath : sChildGoronLinkObjectPath;
        case PLAYER_TUNIC_ZORA:
            return isAdult ? sAdultZoraLinkObjectPath : sChildZoraLinkObjectPath;
        default:
            return {};
    }
}

template <auto& Slots> void UpdateTunicHandDisplayListGroup(s8 currentTunic) {
    constexpr size_t N = std::size(Slots);

    static bool initialized = false;
    static std::array<const char*, N> vanillaPaths{};
    static std::array<std::string, N> tunicPaths{};

    if (!initialized) {
        for (size_t i = 0; i < N; i++) {
            vanillaPaths[i] = reinterpret_cast<const char*>(Slots[i]);
        }
        initialized = true;
    }

    for (size_t i = 0; i < N; i++) {
        const char* resolvedPath = vanillaPaths[i];
        const std::string_view vanillaPath = vanillaPaths[i] != nullptr ? vanillaPaths[i] : "";
        const std::string_view tunicObjectPath = GetTunicObjectPath(vanillaPath, currentTunic);

        if (!tunicObjectPath.empty()) {
            tunicPaths[i] = std::string(tunicObjectPath) + std::string(GetDisplayListName(vanillaPath));
            if (TunicHandDisplayListExists(tunicPaths[i].c_str())) {
                resolvedPath = tunicPaths[i].c_str();
            }
        }

        Slots[i] = reinterpret_cast<Gfx*>(const_cast<char*>(resolvedPath));
    }
}
} // namespace

bool SkeletonPatcher::IsLinkSkeletonPath(const std::string& path) {
    return (sOtr + path == std::string(gLinkAdultSkel)) || (sOtr + path == std::string(gLinkChildSkel));
}

bool SkeletonPatcher::IsLocalPlayerSkelAnime(SkelAnime* skelAnime) {
    if (gPlayState == nullptr) {
        return false;
    }

    Player* player = GET_PLAYER(gPlayState);

    if (player == nullptr) {
        return false;
    }

    PauseContext* pauseCtx = &gPlayState->pauseCtx;

    return (skelAnime == &player->skelAnime) || (skelAnime == &player->upperSkelAnime) ||
           (skelAnime == &pauseCtx->playerSkelAnime);
}

void SkeletonPatcher::RegisterSkeleton(std::string& path, SkelAnime* skelAnime) {
    SkeletonPatchInfo info;

    info.skelAnime = skelAnime;
    info.isLocalPlayer = false;

    if (path.starts_with(sOtr)) {
        path = path.substr(sOtr.length());
    }

    // Determine if we're using an alternate skeleton
    if (path.starts_with(Ship::IResource::gAltAssetPrefix)) {
        info.vanillaSkeletonPath = path.substr(Ship::IResource::gAltAssetPrefix.length(),
                                               path.size() - Ship::IResource::gAltAssetPrefix.length());
    } else {
        info.vanillaSkeletonPath = path;
    }

    if (IsLinkSkeletonPath(info.vanillaSkeletonPath)) {
        info.isLocalPlayer = IsLocalPlayerSkelAnime(skelAnime);

        // Skip registering skeletons that do not belong to the local player (e.g. Anchor dummy actors)
        if (!info.isLocalPlayer) {
            return;
        }
    }

    skeletons.push_back(info);
}

void SkeletonPatcher::UnregisterSkeleton(SkelAnime* skelAnime) {

    // TODO: Should probably just use a dictionary here...
    for (size_t i = 0; i < skeletons.size(); i++) {
        auto skel = skeletons[i];

        if (skel.skelAnime == skelAnime) {
            skeletons.erase(skeletons.begin() + i);
            break;
        }
    }
}
void SkeletonPatcher::ClearSkeletons() {
    skeletons.clear();
}

void SkeletonPatcher::UpdateSkeletons() {
    auto resourceMgr = Ship::Context::GetInstance()->GetResourceManager();
    bool isAlt = resourceMgr->IsAltAssetsEnabled();
    for (auto skel : skeletons) {
        Skeleton* newSkel =
            (Skeleton*)resourceMgr
                ->LoadResource((isAlt ? Ship::IResource::gAltAssetPrefix : "") + skel.vanillaSkeletonPath, true)
                .get();

        if (newSkel != nullptr) {
            skel.skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
            uintptr_t skelPtr = (uintptr_t)newSkel->GetPointer();
            memcpy(&skel.skelAnime->skeletonHeader, &skelPtr,
                   sizeof(uintptr_t)); // Dumb thing that needs to be done because cast is not cooperating
        }
    }
}

void SkeletonPatcher::UpdateCustomSkeletons() {
    UpdateTunicHandDisplayLists();

    for (auto skel : skeletons) {
        if (!skel.isLocalPlayer) {
            continue;
        }

        UpdateTunicSkeletons(skel);
    }
}

void SkeletonPatcher::UpdateTunicHandDisplayLists() {
    if (gPlayState == nullptr) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return;
    }

    const s8 currentTunic = TUNIC_EQUIP_TO_PLAYER(CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC));
    UpdateTunicHandDisplayListGroup<gPlayerLeftHandBgsDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<gPlayerLeftHandOpenDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<gPlayerLeftHandClosedDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerLeftHandSwordDLs2>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerLeftHandSwordDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerRightHandOpenDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerRightHandClosedDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerRightHandShieldDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerRightHandBowSlingshotDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerRightHandBowSlingshotDLs2>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerRightHandOcarinaDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerRightHandOotDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerRightHandHookshotDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerLeftHandHammerDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<gPlayerLeftHandBoomerangDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sPlayerLeftHandBottleDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sFirstPersonLeftForearmDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sFirstPersonLeftHandDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sFirstPersonForearmDLs>(currentTunic);
    UpdateTunicHandDisplayListGroup<sFirstPersonRightHandHoldingWeaponDLs>(currentTunic);
}

void SkeletonPatcher::UpdateTunicSkeletons(SkeletonPatchInfo& skel) {
    std::string skeletonPath = "";

    // Check if this is one of Link's skeletons
    if (sOtr + skel.vanillaSkeletonPath == std::string(gLinkAdultSkel)) {
        // Check what Link's current tunic is
        switch (TUNIC_EQUIP_TO_PLAYER(CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC))) {
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
        switch (TUNIC_EQUIP_TO_PLAYER(CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC))) {
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
    Skeleton* newSkel = nullptr;
    Skeleton* altSkel = nullptr;
    auto resourceMgr = Ship::Context::GetInstance()->GetResourceManager();
    bool isAlt = resourceMgr->IsAltAssetsEnabled();

    // If alt assets are on, look for alt tagged skeletons
    if (isAlt) {
        altSkel = (Skeleton*)Ship::Context::GetInstance()
                      ->GetResourceManager()
                      ->LoadResource(Ship::IResource::gAltAssetPrefix + skeletonPath, true)
                      .get();

        // Override non-alt skeleton if necessary
        if (altSkel != nullptr) {
            newSkel = altSkel;
        }
    }

    // Load new skeleton based on the custom model if it exists
    if (altSkel == nullptr) {
        newSkel = (Skeleton*)Ship::Context::GetInstance()->GetResourceManager()->LoadResource(skeletonPath, true).get();
    }

    // Change back to the original skeleton if no skeleton's were found
    if (newSkel == nullptr && skeletonPath != skel.vanillaSkeletonPath) {
        UpdateCustomSkeletonFromPath(skel.vanillaSkeletonPath, skel);
        return;
    }

    if (newSkel != nullptr) {
        skel.skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
        uintptr_t skelPtr = (uintptr_t)newSkel->GetPointer();
        memcpy(&skel.skelAnime->skeletonHeader, &skelPtr, sizeof(uintptr_t));
    }
}
} // namespace SOH
