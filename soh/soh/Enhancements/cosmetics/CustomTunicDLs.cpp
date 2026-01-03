#include "soh/Enhancements/cosmetics/CustomTunicDLs.h"

#include "soh/ResourceManagerHelpers.h"

#include "variables.h"
#include "z64item.h"

#include <string>
#include <string_view>
#include <unordered_map>

namespace {
constexpr std::string_view kOtrPrefix = "__OTR__";

enum class TunicType {
    None,
    Goron,
    Zora,
};

TunicType GetTunicType() {
    s32 tunicEquip = (gSaveContext.equips.equipment & gEquipMasks[EQUIP_TYPE_TUNIC]) >> gEquipShifts[EQUIP_TYPE_TUNIC];
    switch (tunicEquip) {
        case EQUIP_VALUE_TUNIC_GORON:
            return TunicType::Goron;
        case EQUIP_VALUE_TUNIC_ZORA:
            return TunicType::Zora;
        default:
            return TunicType::None;
    }
}

struct RemapEntry {
    std::string_view base;
    std::string_view goron;
    std::string_view zora;
};

constexpr RemapEntry kRemapEntries[] = {
    { "objects/object_link_boy/gLinkAdultLeftHandClosedNearDL",
      "objects/object_link_boy_goron/gLinkAdultLeftHandClosedNearDL",
      "objects/object_link_boy_zora/gLinkAdultLeftHandClosedNearDL" },
    { "objects/object_link_boy/gLinkAdultLeftHandNearDL", "objects/object_link_boy_goron/gLinkAdultLeftHandNearDL",
      "objects/object_link_boy_zora/gLinkAdultLeftHandNearDL" },
    { "objects/object_link_boy/gLinkAdultLeftHandOutNearDL",
      "objects/object_link_boy_goron/gLinkAdultLeftHandOutNearDL",
      "objects/object_link_boy_zora/gLinkAdultLeftHandOutNearDL" },
    { "objects/object_link_boy/gLinkAdultRightHandClosedNearDL",
      "objects/object_link_boy_goron/gLinkAdultRightHandClosedNearDL",
      "objects/object_link_boy_zora/gLinkAdultRightHandClosedNearDL" },
    { "objects/object_link_boy/gLinkAdultRightHandHoldingOotNearDL",
      "objects/object_link_boy_goron/gLinkAdultRightHandHoldingOotNearDL",
      "objects/object_link_boy_zora/gLinkAdultRightHandHoldingOotNearDL" },
    { "objects/object_link_boy/gLinkAdultRightHandNearDL", "objects/object_link_boy_goron/gLinkAdultRightHandNearDL",
      "objects/object_link_boy_zora/gLinkAdultRightHandNearDL" },
    { "objects/object_link_boy/gLinkAdultRightHandOutNearDL",
      "objects/object_link_boy_goron/gLinkAdultRightHandOutNearDL",
      "objects/object_link_boy_zora/gLinkAdultRightHandOutNearDL" },
    { "objects/object_link_child/gLinkChildLeftFistNearDL", "objects/object_link_child_goron/gLinkChildLeftFistNearDL",
      "objects/object_link_child_zora/gLinkChildLeftFistNearDL" },
    { "objects/object_link_child/gLinkChildLeftHandHoldingMasterSwordDL",
      "objects/object_link_child_goron/gLinkChildLeftHandHoldingMasterSwordDL",
      "objects/object_link_child_zora/gLinkChildLeftHandHoldingMasterSwordDL" },
    { "objects/object_link_child/gLinkChildLeftHandNearDL", "objects/object_link_child_goron/gLinkChildLeftHandNearDL",
      "objects/object_link_child_zora/gLinkChildLeftHandNearDL" },
    { "objects/object_link_child/gLinkChildLeftHandUpNearDL",
      "objects/object_link_child_goron/gLinkChildLeftHandUpNearDL",
      "objects/object_link_child_zora/gLinkChildLeftHandUpNearDL" },
    { "objects/object_link_child/gLinkChildRightHandClosedNearDL",
      "objects/object_link_child_goron/gLinkChildRightHandClosedNearDL",
      "objects/object_link_child_zora/gLinkChildRightHandClosedNearDL" },
    { "objects/object_link_child/gLinkChildRightHandNearDL",
      "objects/object_link_child_goron/gLinkChildRightHandNearDL",
      "objects/object_link_child_zora/gLinkChildRightHandNearDL" },
    { "objects/object_link_child/gLinkChildRightHandHoldingFairyOcarinaNearDL",
      "objects/object_link_child_goron/gLinkChildRightHandHoldingFairyOcarinaNearDL",
      "objects/object_link_child_zora/gLinkChildRightHandHoldingFairyOcarinaNearDL" },
    { "objects/object_link_child/gLinkChildRightHandAndOotNearDL",
      "objects/object_link_child_goron/gLinkChildRightHandAndOotNearDL",
      "objects/object_link_child_zora/gLinkChildRightHandAndOotNearDL" },
    { "objects/object_custom_equip/gCustomAdultFPSHandDL", "objects/object_custom_equip/gCustomAdultGoronFPSHandDL",
      "objects/object_custom_equip/gCustomAdultZoraFPSHandDL" },
    { "objects/object_custom_equip/gCustomChildFPSHandDL", "objects/object_custom_equip/gCustomChildGoronFPSHandDL",
      "objects/object_custom_equip/gCustomChildZoraFPSHandDL" },
    { "objects/object_link_boy/gLinkAdultLeftIronBootDL", "objects/object_link_boy_goron/gLinkAdultLeftIronBootDL",
      "objects/object_link_boy_zora/gLinkAdultLeftIronBootDL" },
    { "objects/object_link_boy/gLinkAdultRightIronBootDL", "objects/object_link_boy_goron/gLinkAdultRightIronBootDL",
      "objects/object_link_boy_zora/gLinkAdultRightIronBootDL" },
    { "objects/object_link_boy/gLinkAdultLeftHoverBootDL", "objects/object_link_boy_goron/gLinkAdultLeftHoverBootDL",
      "objects/object_link_boy_zora/gLinkAdultLeftHoverBootDL" },
    { "objects/object_link_boy/gLinkAdultRightHoverBootDL", "objects/object_link_boy_goron/gLinkAdultRightHoverBootDL",
      "objects/object_link_boy_zora/gLinkAdultRightHoverBootDL" },
};

std::string_view StripOtrPrefix(std::string_view path) {
    if (path.starts_with(kOtrPrefix)) {
        return path.substr(kOtrPrefix.size());
    }
    return path;
}

bool TargetExists(std::string_view path) {
    return ResourceMgr_FileExists(path.data()) ||
           (ResourceMgr_IsAltAssetsEnabled() && ResourceMgr_FileAltExists(path.data()));
}

std::string_view GetRemapTarget(std::string_view view, TunicType tunicType) {
    if (tunicType == TunicType::None) {
        return {};
    }

    for (const auto& entry : kRemapEntries) {
        if (view == entry.base) {
            return (tunicType == TunicType::Goron) ? entry.goron : entry.zora;
        }
    }

    return {};
}

const char* MakeRemappedPath(std::string_view target, bool hasOtrPrefix) {
    std::string fullPath;
    if (hasOtrPrefix) {
        fullPath.reserve(kOtrPrefix.size() + target.size());
        fullPath.append(kOtrPrefix);
        fullPath.append(target);
    } else {
        fullPath.assign(target);
    }

    static thread_local std::unordered_map<std::string, std::string> remappedCache;
    auto [it, inserted] = remappedCache.emplace(fullPath, fullPath);
    return it->second.c_str();
}
} // namespace

const char* CustomTunicDLs_RemapPath(const char* path) {
    if (path == nullptr) {
        return path;
    }

    std::string_view view(path);
    bool hasOtrPrefix = view.starts_with(kOtrPrefix);
    view = StripOtrPrefix(view);

    TunicType tunicType = GetTunicType();
    if (tunicType == TunicType::None) {
        return path;
    }

    std::string_view target = GetRemapTarget(view, tunicType);
    if (target.empty()) {
        return path;
    }

    if (!TargetExists(target)) {
        return path;
    }

    return MakeRemappedPath(target, hasOtrPrefix);
}
