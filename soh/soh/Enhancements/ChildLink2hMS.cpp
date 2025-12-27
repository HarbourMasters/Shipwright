#include "soh/Enhancements/ChildLink2hMS.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"

extern "C" u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);

extern "C" {
#include "global.h"
#include "z64player.h"
}

static bool ChildLink2hMS_IsEnabledInternal() {
    // Allow either the generated randomizer option or the backing cvars to enable the feature.
    // Enhancement cvar lets non-rando saves opt in; randomizer setting forces it on when enabled.
    return (IS_RANDO && Randomizer_GetSettingValue(RSK_CHILD_LINK_2H_MS) != RO_GENERIC_OFF) ||
           CVarGetInteger(CVAR_RANDOMIZER_SETTING("ChildMasterSword"), 0) ||
           CVarGetInteger(CVAR_ENHANCEMENT("ChildMasterSword"), 0);
}

extern "C" s32 ChildLink2hMS_IsEnabled(void) {
    return ChildLink2hMS_IsEnabledInternal();
}

static s16 ChildLink2hMS_SelectChildSwordInternal(s16 preferredSword) {
    s32 hasKokiriSword = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_KOKIRI);
    s32 hasMasterSword = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER);

    // Keep the last equipped sword if still owned; otherwise fall back to the other owned sword, else none.
    if ((preferredSword == ITEM_SWORD_MASTER) && hasMasterSword) {
        return ITEM_SWORD_MASTER;
    }

    if ((preferredSword == ITEM_SWORD_KOKIRI) && hasKokiriSword) {
        return ITEM_SWORD_KOKIRI;
    }

    if (hasKokiriSword) {
        return ITEM_SWORD_KOKIRI;
    }

    if (hasMasterSword) {
        return ITEM_SWORD_MASTER;
    }

    return ITEM_NONE;
}

// Centralize the conditions for allowing child Link to wield the Master Sword.
extern "C" s32 ChildLink2hMS_CanChildUseMasterSword(void) {
    return ChildLink2hMS_IsEnabledInternal() && (gSaveContext.linkAge == LINK_AGE_CHILD) &&
           CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER);
}

extern "C" s32 ChildLink2hMS_ShouldBypassAgeReq(s32 equipType, s32 equipValue) {
    return (equipType == EQUIP_TYPE_SWORD) && (equipValue == EQUIP_VALUE_SWORD_MASTER) &&
           ChildLink2hMS_CanChildUseMasterSword();
}

extern "C" s32 ChildLink2hMS_IsTwoHanded(Player* player) {
    if (player == NULL) {
        return 0;
    }

    return ChildLink2hMS_CanChildUseMasterSword() && (player->heldItemAction == PLAYER_IA_SWORD_MASTER);
}

extern "C" s32 ChildLink2hMS_ShouldAllowEquip(s32 equipType, s32 equipValue) {
    return ChildLink2hMS_ShouldBypassAgeReq(equipType, equipValue);
}

extern "C" s32 ChildLink2hMS_OverrideMeleeWeapon(s32 actionParam, s32 baseMeleeWeapon) {
    // Keep Master Sword hitbox/damage while still using two-handed animations/models.
    return baseMeleeWeapon;
}

extern "C" s16 ChildLink2hMS_SelectChildSword(s16 preferredSword) {
    if (!ChildLink2hMS_IsEnabledInternal()) {
        return preferredSword;
    }

    return ChildLink2hMS_SelectChildSwordInternal(preferredSword);
}

extern "C" void ChildLink2hMS_ApplyChildSwordEquip(s16 preferredSword) {
    if (!ChildLink2hMS_IsEnabledInternal()) {
        return;
    }

    s16 sword = ChildLink2hMS_SelectChildSwordInternal(preferredSword);

    gSaveContext.equips.buttonItems[0] = sword;
    gSaveContext.equips.equipment &= (u16) ~(0xF << (EQUIP_TYPE_SWORD * 4));

    if (sword == ITEM_SWORD_MASTER) {
        gSaveContext.equips.equipment |= (EQUIP_VALUE_SWORD_MASTER << (EQUIP_TYPE_SWORD * 4));
        Flags_UnsetInfTable(INFTABLE_SWORDLESS);
    } else if (sword == ITEM_SWORD_KOKIRI) {
        gSaveContext.equips.equipment |= (EQUIP_VALUE_SWORD_KOKIRI << (EQUIP_TYPE_SWORD * 4));
        Flags_UnsetInfTable(INFTABLE_SWORDLESS);
    } else {
        Flags_SetInfTable(INFTABLE_SWORDLESS);
    }
}
