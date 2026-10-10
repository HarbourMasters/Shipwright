/**
 * @file z_elf_message.c
 *
 * This file provides quest hints through Tatl.
 *
 * In Ocarina of Time, this was a more elaborate system that determined
 * which hint to give based on a script.
 * That system has been replaced with a single function that hardcodes the checks.
 */
#include "global.h"

/**
 * Gets the relevant text ID for Tatl hints in first cycle.
 */
u16 QuestHint_GetTatlTextId(PlayState* play) {
    if (INV_CONTENT(ITEM_OCARINA_OF_TIME) == ITEM_OCARINA_OF_TIME) {
        return 0;
    }

    if (CURRENT_DAY <= 0) {
        return 0;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_88_20)) {
        return 0;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_79_10)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLOCK_TOWER_OPENED)) {
            return 0;
        }

        return 0x224;
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_09_01)) {
            return 0x21E;
        }

        if (play->sceneId == SCENE_YOUSEI_IZUMI) {
            return 0;
        }

        return 0x21D;
    }

    if (gSaveContext.save.saveInfo.playerData.isMagicAcquired != true) {
        return 0x21F;
    }

    if (INV_CONTENT(ITEM_DEED_LAND) == ITEM_DEED_LAND) {
        if (play->sceneId != SCENE_OKUJOU) {
            return 0x244;
        }

        return 0;
    }

    if (INV_CONTENT(ITEM_MOONS_TEAR) == ITEM_MOONS_TEAR) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_86_04)) {
            return 0x242;
        }

        return 0x243;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_74_20)) {
        return 0x223;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_80)) {
        return 0x222;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_20)) {
        return 0x221;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_02)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10)) {
            return 0x240;
        }

        return 0x241;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_86_02) || CHECK_WEEKEVENTREG(WEEKEVENTREG_73_40)) {
        return 0x23F;
    }

    return 0x220;
}
