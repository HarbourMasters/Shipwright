#include "BossRush.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh_assets.h"
#include "soh/frame_interpolation.h"

#include <array>
#include <string>
#include <vector>

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "src/overlays/actors/ovl_Boss_Goma/z_boss_goma.h"
#include "src/overlays/actors/ovl_Boss_Mo/z_boss_mo.h"
#include "src/overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "src/overlays/gamestates/ovl_file_choose/file_choose.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "textures/icon_item_nes_static/icon_item_nes_static.h"
#include "textures/icon_item_ger_static/icon_item_ger_static.h"
#include "textures/icon_item_fra_static/icon_item_fra_static.h"

extern PlayState* gPlayState;

Gfx* KaleidoScope_QuadTextureIA8(Gfx* gfx, void* texture, s16 width, s16 height, u16 point);
void FileChoose_UpdateStickDirectionPromptAnim(GameState* thisx);
void FileChoose_DrawTextRec(GraphicsContext* gfxCtx, s32 r, s32 g, s32 b, s32 a, f32 x, f32 y, f32 z, s32 s, s32 t,
                            f32 dx, f32 dy);
}

typedef enum {
    BR_CHOICE_BOSSES_ALL,
    BR_CHOICE_BOSSES_CHILD,
    BR_CHOICE_BOSSES_ADULT,
    BR_CHOICE_BOSSES_GANONDORF_GANON
} BossRushBossesChoices;

typedef enum {
    BR_CHOICE_HEARTS_10,
    BR_CHOICE_HEARTS_15,
    BR_CHOICE_HEARTS_20,
    BR_CHOICE_HEARTS_3,
    BR_CHOICE_HEARTS_5,
    BR_CHOICE_HEARTS_7
} BossRushHeartsChoices;

typedef enum {
    BR_CHOICE_AMMO_LIMITED,
    BR_CHOICE_AMMO_FULL,
    BR_CHOICE_AMMO_MAXED,
} BossRushAmmoChoices;

typedef enum {
    BR_CHOICE_HEAL_GANONDORF,
    BR_CHOICE_HEAL_EVERYBOSS,
    BR_CHOICE_HEAL_NEVER,
} BossRushHealChoices;

typedef enum {
    BR_CHOICE_MAGIC_SINGLE,
    BR_CHOICE_MAGIC_DOUBLE,
} BossRushMagicChoices;

typedef enum {
    BR_CHOICE_BGS_NO,
    BR_CHOICE_BGS_YES,
} BossRushBgsChoices;

typedef enum {
    BR_CHOICE_BOTTLE_NO,
    BR_CHOICE_BOTTLE_EMPTY,
    BR_CHOICE_BOTTLE_FAIRY,
    BR_CHOICE_BOTTLE_REDPOTION,
    BR_CHOICE_BOTTLE_GREENPOTION,
    BR_CHOICE_BOTTLE_BLUEPOTION
} BossRushBottleChoices;

typedef enum {
    BR_CHOICE_LONGSHOT_NO,
    BR_CHOICE_LONGSHOT_YES,
} BossRushLongshotChoices;

typedef enum {
    BR_CHOICE_HOVERBOOTS_NO,
    BR_CHOICE_HOVERBOOTS_YES,
} BossRushHoverBootsChoices;

typedef enum {
    BR_CHOICE_BUNNYHOOD_NO,
    BR_CHOICE_BUNNYHOOD_YES,
} BossRushBunnyHoodChoices;

typedef enum {
    BR_CHOICE_TIMER_YES,
    BR_CHOICE_TIMER_NO,
} BossRushTimerChoices;

typedef struct BossRushSetting {
    std::array<std::string, LANGUAGE_MAX> name;
    std::vector<std::array<std::string, LANGUAGE_MAX>> choices;
} BossRushSetting;

BossRushSetting BossRushOptions[BR_OPTIONS_MAX] = {
    { { "BOSSES:", "BOSSE:", "BOSS:" },
      {
          { "All", "Alle", "Tous" },
          { "Child", "Kind", "Enfant" },
          { "Adult", "Erwachsener", "Adulte" },
          { "Ganondorf & Ganon", "Ganondorf & Ganon", "Ganondorf & Ganon" },
      } },
    { { "HEARTS:", "HERZEN:", "COEURS:" },
      {
          { "10", "10", "10" },
          { "15", "15", "15" },
          { "20", "20", "20" },
          { "3", "3", "3" },
          { "5", "5", "5" },
          { "7", "7", "7" },
      } },
    { { "AMMO:", "MUNITION:", "MUNITIONS:" },
      {
          { "Limited", "Limitiert", "Limitées" },
          { "Full", "Voll", "Pleines" },
          { "Maxed", "Maximum", "Maximum" },
      } },
    { { "HEAL:", "REGENERATION:", "SOIN:" },
      {
          { "Before Ganondorf", "Vor Ganondorf", "Avant Ganondorf" },
          { "Every Boss", "Bei jedem Boss", "Tous les Boss" },
          { "Never", "Niemals", "Jamais" },
      } },
    { { "HYPER BOSSES:", "HYPER-BOSSE:", "HYPER BOSS:" },
      {
          { "No", "Nein", "Non" },
          { "Yes", "Ja", "Oui" },
      } },
    { { "MAGIC:", "MAGIE:", "MAGIE:" },
      {
          { "Single", "Einzel", "Simple" },
          { "Double", "Doppel", "Double" },
      } },
    { { "BIG. SWORD:", "BIG.-SCHWERT:", "EPÉE DE BIG.:" },
      {
          { "No", "Nein", "Non" },
          { "Yes", "Ja", "Oui" },
      } },
    { { "BOTTLE:", "FLASCHEN:", "BOUTEILLE:" },
      {
          { "No", "Nein", "Non" },
          { "Empty", "Leer", "Vide" },
          { "Fairy", "Fee", "Fée" },
          { "Red Potion", "Rotes Elixier", "Potion Rouge" },
          { "Green Potion", "Grünes Elixier", "Potion Verte" },
          { "Blue Potion", "Blaues Elixier", "Potion Bleue" },
      } },
    { { "LONGSHOT:", "ENTERHAKEN:", "SUPER GRAPPIN:" },
      {
          { "No", "Nein", "Non" },
          { "Yes", "Ja", "Oui" },
      } },
    { { "HOVER BOOTS:", "GLEITSTIEFEL:", "BOTTES DES AIRS:" },
      {
          { "No", "Nein", "Non" },
          { "Yes", "Ja", "Oui" },
      } },
    { { "BUNNY HOOD:", "HASENOHREN:", "MASQUE DU LAPIN:" },
      {
          { "No", "Nein", "Non" },
          { "Yes", "Ja", "Oui" },
      } },
    { { "TIMER:", "TIMER:", "TIMER:" },
      {
          { "Yes", "Ja", "Oui" },
          { "No", "Nein", "Non" },
      } }
};

const char* BossRush_GetSettingName(u8 optionIndex, u8 language) {
    return BossRushOptions[optionIndex].name[language].c_str();
}

const char* BossRush_GetSettingChoiceName(u8 optionIndex, u8 choiceIndex, u8 language) {
    return BossRushOptions[optionIndex].choices[choiceIndex][language].c_str();
}

u8 BossRush_GetSettingOptionsAmount(u8 optionIndex) {
    return static_cast<u8>(BossRushOptions[optionIndex].choices.size());
}

void FileChoose_UpdateBossRushMenu(GameState* gameState) {
    static s8 sLastBossRushOptionIndex = -1;
    static s8 sLastBossRushOptionValue = -1;

    FileChoose_UpdateStickDirectionPromptAnim(gameState);
    FileChooseContext* fileChooseContext = (FileChooseContext*)gameState;
    Input* input = &fileChooseContext->state.input[0];
    bool dpad = CVarGetInteger(CVAR_SETTING("DpadInText"), 0);

    // Fade in elements after opening Boss Rush options menu
    fileChooseContext->bossRushUIAlpha += 25;
    if (fileChooseContext->bossRushUIAlpha > 255) {
        fileChooseContext->bossRushUIAlpha = 255;
    }

    // Animate up/down arrows.
    fileChooseContext->bossRushArrowOffset += 1;
    if (fileChooseContext->bossRushArrowOffset >= 30) {
        fileChooseContext->bossRushArrowOffset = 0;
    }

    // Move menu selection up or down.
    if (ABS(fileChooseContext->stickRelY) > 30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN | BTN_DUP))) {
        // Move down
        if (fileChooseContext->stickRelY < -30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN))) {
            // When selecting past the last option, cycle back to the first option.
            if ((fileChooseContext->bossRushIndex + 1) > BR_OPTIONS_MAX - 1) {
                fileChooseContext->bossRushIndex = 0;
                fileChooseContext->bossRushOffset = 0;
            } else {
                fileChooseContext->bossRushIndex++;
                // When last visible option is selected when moving down, offset the list down by one.
                if (fileChooseContext->bossRushIndex - fileChooseContext->bossRushOffset >
                    BOSSRUSH_MAX_OPTIONS_ON_SCREEN - 1) {
                    fileChooseContext->bossRushOffset++;
                }
            }
        } else if (fileChooseContext->stickRelY > 30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DUP))) {
            // When selecting past the first option, cycle back to the last option and offset the list to view it
            // properly.
            if ((fileChooseContext->bossRushIndex - 1) < 0) {
                fileChooseContext->bossRushIndex = BR_OPTIONS_MAX - 1;
                fileChooseContext->bossRushOffset =
                    fileChooseContext->bossRushIndex - BOSSRUSH_MAX_OPTIONS_ON_SCREEN + 1;
            } else {
                // When first visible option is selected when moving up, offset the list up by one.
                if (fileChooseContext->bossRushIndex - fileChooseContext->bossRushOffset == 0) {
                    fileChooseContext->bossRushOffset--;
                }
                fileChooseContext->bossRushIndex--;
            }
        }

        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    // Cycle through choices for currently selected option.
    if (ABS(fileChooseContext->stickRelX) > 30 ||
        (dpad && CHECK_BTN_ANY(input->press.button, BTN_DLEFT | BTN_DRIGHT))) {
        if (fileChooseContext->stickRelX > 30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DRIGHT))) {
            // If exceeding the amount of choices for the selected option, cycle back to the first.
            if ((gSaveContext.ship.quest.data.bossRush.options[fileChooseContext->bossRushIndex] + 1) ==
                BossRush_GetSettingOptionsAmount(fileChooseContext->bossRushIndex)) {
                gSaveContext.ship.quest.data.bossRush.options[fileChooseContext->bossRushIndex] = 0;
            } else {
                gSaveContext.ship.quest.data.bossRush.options[fileChooseContext->bossRushIndex]++;
            }
        } else if (fileChooseContext->stickRelX < -30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DLEFT))) {
            // If cycling back when already at the first choice for the selected option, cycle back to the last choice.
            if ((gSaveContext.ship.quest.data.bossRush.options[fileChooseContext->bossRushIndex] - 1) < 0) {
                gSaveContext.ship.quest.data.bossRush.options[fileChooseContext->bossRushIndex] =
                    BossRush_GetSettingOptionsAmount(fileChooseContext->bossRushIndex) - 1;
            } else {
                gSaveContext.ship.quest.data.bossRush.options[fileChooseContext->bossRushIndex]--;
            }
        }

        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    if (sLastBossRushOptionIndex != fileChooseContext->bossRushIndex ||
        sLastBossRushOptionValue != gSaveContext.ship.quest.data.bossRush.options[fileChooseContext->bossRushIndex]) {
        GameInteractor_ExecuteOnUpdateFileBossRushOptionSelection(
            fileChooseContext->bossRushIndex,
            gSaveContext.ship.quest.data.bossRush.options[fileChooseContext->bossRushIndex]);
        sLastBossRushOptionIndex = fileChooseContext->bossRushIndex;
        sLastBossRushOptionValue = gSaveContext.ship.quest.data.bossRush.options[fileChooseContext->bossRushIndex];
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        fileChooseContext->configMode = CM_BOSS_RUSH_TO_QUEST;
        return;
    }

    // Load into the game.
    if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_A)) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        fileChooseContext->buttonIndex = 0xFE;
        fileChooseContext->menuMode = FS_MENU_MODE_SELECT;
        fileChooseContext->selectMode = SM_FADE_OUT;
        fileChooseContext->prevConfigMode = fileChooseContext->configMode;
        return;
    }
}

void FileChoose_DrawBossRushMenuWindowContents(FileChooseContext* fileChooseContext) {
    OPEN_DISPS(fileChooseContext->state.gfxCtx);

    uint8_t language = (gSaveContext.language == LANGUAGE_JPN) ? LANGUAGE_ENG : gSaveContext.language;
    uint8_t listOffset = fileChooseContext->bossRushOffset;
    uint8_t textAlpha = fileChooseContext->bossRushUIAlpha;

    // Draw arrows to indicate that the list can scroll up or down.
    // Arrow up
    if (listOffset > 0) {
        uint16_t arrowUpX = 140;
        uint16_t arrowUpY = 76 - (fileChooseContext->bossRushArrowOffset / 10);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gArrowUpTex, G_IM_FMT_IA, G_IM_SIZ_16b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSPWideTextureRectangle(POLY_OPA_DISP++, arrowUpX << 2, arrowUpY << 2, (arrowUpX + 8) << 2, (arrowUpY + 8) << 2,
                                G_TX_RENDERTILE, 0, 0, (1 << 11), (1 << 11));
    }
    // Arrow down
    if (BR_OPTIONS_MAX - listOffset > BOSSRUSH_MAX_OPTIONS_ON_SCREEN) {
        uint16_t arrowDownX = 140;
        uint16_t arrowDownY = 181 + (fileChooseContext->bossRushArrowOffset / 10);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gArrowDownTex, G_IM_FMT_IA, G_IM_SIZ_16b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSPWideTextureRectangle(POLY_OPA_DISP++, arrowDownX << 2, arrowDownY << 2, (arrowDownX + 8) << 2,
                                (arrowDownY + 8) << 2, G_TX_RENDERTILE, 0, 0, (1 << 11), (1 << 11));
    }

    // Draw options. There's more options than what fits on the screen, so the visible options
    // depend on the current offset of the list. Currently selected option pulses in
    // color and has arrows surrounding the option.
    for (uint8_t i = listOffset; i - listOffset < BOSSRUSH_MAX_OPTIONS_ON_SCREEN; i++) {
        uint16_t textYOffset = (i - listOffset) * 16;

        // Option name.
        Interface_DrawTextLine(fileChooseContext->state.gfxCtx, (char*)BossRush_GetSettingName(i, language), 65,
                               (87 + textYOffset), 255, 255, 80, textAlpha, 0.8f, true);

        // Selected choice for option.
        uint16_t finalKerning = Interface_DrawTextLine(
            fileChooseContext->state.gfxCtx,
            (char*)BossRush_GetSettingChoiceName(i, gSaveContext.ship.quest.data.bossRush.options[i], language), 165,
            (87 + textYOffset), 255, 255, 255, textAlpha, 0.8f, true);

        // Draw arrows around selected option.
        if (fileChooseContext->bossRushIndex == i) {
            Gfx_SetupDL_39Opa(fileChooseContext->state.gfxCtx);
            gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPLoadTextureBlock(POLY_OPA_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                                G_TX_NOLOD);
            FileChoose_DrawTextRec(fileChooseContext->state.gfxCtx, fileChooseContext->stickLeftPrompt.arrowColorR,
                                   fileChooseContext->stickLeftPrompt.arrowColorG,
                                   fileChooseContext->stickLeftPrompt.arrowColorB, textAlpha, 160, (92 + textYOffset),
                                   0.42f, 0, 0, -1.0f, 1.0f);
            FileChoose_DrawTextRec(fileChooseContext->state.gfxCtx, fileChooseContext->stickRightPrompt.arrowColorR,
                                   fileChooseContext->stickRightPrompt.arrowColorG,
                                   fileChooseContext->stickRightPrompt.arrowColorB, textAlpha, (171 + finalKerning),
                                   (92 + textYOffset), 0.42f, 0, 0, 1.0f, 1.0f);
        }
    }

    CLOSE_DISPS(fileChooseContext->state.gfxCtx);
}

void BossRush_SpawnBlueWarps(PlayState* play) {

    // Spawn blue warps in Chamber of Sages based on what bosses have been defeated.
    if (gSaveContext.linkAge == LINK_AGE_CHILD) {
        // Forest Medallion (Gohma)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -100, 6, -170, 0, 0, 0, -1, false);
        }
        // Fire Medallion (King Dodongo)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 100, 6, -170, 0, 0, 0, -1, false);
        }
        // Water Medallion (Barinade)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 199, 6, 0, 0, 0, 0, -1, false);
        }
    } else {
        // Light Medallion (Ganondorf)
        if (CheckDungeonCount() == 8) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -199, 6, 0, 0, 0, 0, -1, false);
        }
        // Forest Medallion (Phantom Ganondorf)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -100, 6, -170, 0, 0, 0, -1, false);
        }
        // Fire Medallion (Volvagia)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 100, 6, -170, 0, 0, 0, -1, false);
        }
        // Water Medallion (Morpha)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 199, 6, 0, 0, 0, 0, -1, false);
        }
        // Spirit Medallion (Twinrova)
        if (!Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 100, 6, 170, 0, 0, 0, -1, false);
        }
        // Shadow Medallion (Bongo Bongo)
        if (!Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -100, 6, 170, 0, 0, 0, -1, false);
        }
    }
}

void BossRush_SetEquipment(u8 linkAge) {
    std::array<u8, 8> brButtonItems;
    std::array<u8, 7> brCButtonSlots;

    // Set Child Equipment.
    if (linkAge == LINK_AGE_CHILD) {
        brButtonItems = {
            ITEM_SWORD_KOKIRI, ITEM_STICK, ITEM_NUT, ITEM_BOMB, ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE
        };

        brCButtonSlots = { SLOT_STICK, SLOT_NUT, SLOT_BOMB, SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE };

        Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_KOKIRI);
        Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_DEKU);
        // Set Adult equipment.
    } else {
        brButtonItems = { ITEM_SWORD_MASTER, ITEM_BOW,  ITEM_HAMMER, ITEM_BOMB,
                          ITEM_NONE,         ITEM_NONE, ITEM_NONE,   ITEM_NONE };

        brCButtonSlots = { SLOT_BOW, SLOT_HAMMER, SLOT_BOMB, SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE };

        Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_MASTER);
        Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_MIRROR);
        Inventory_ChangeEquipment(EQUIP_TYPE_TUNIC, EQUIP_VALUE_TUNIC_GORON);
    }

    // Button Items
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.equips.buttonItems); button++) {
        gSaveContext.equips.buttonItems[button] = brButtonItems[button];
    }

    // C buttons
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.equips.cButtonSlots); button++) {
        gSaveContext.equips.cButtonSlots[button] = brCButtonSlots[button];
    }
}

void BossRush_HandleBlueWarp(PlayState* play, f32 warpPosX, f32 warpPosZ) {
    // If warping from Chamber of Sages, choose the correct boss room to teleport to.
    if (play->sceneNum == SCENE_CHAMBER_OF_THE_SAGES) {
        // Gohma & Phantom Ganon
        if (warpPosX == -100 && warpPosZ == -170) {
            if (gSaveContext.linkAge == LINK_AGE_CHILD) {
                play->nextEntranceIndex = ENTR_DEKU_TREE_BOSS_ENTRANCE;
            } else {
                play->nextEntranceIndex = ENTR_FOREST_TEMPLE_BOSS_ENTRANCE;
            }
            // King Dodongo & Volvagia
        } else if (warpPosX == 100 && warpPosZ == -170) {
            if (gSaveContext.linkAge == LINK_AGE_CHILD) {
                play->nextEntranceIndex = ENTR_DODONGOS_CAVERN_BOSS_ENTRANCE;
            } else {
                play->nextEntranceIndex = ENTR_FIRE_TEMPLE_BOSS_ENTRANCE;
            }
            // Barinade & Morb
        } else if (warpPosX == 199 && warpPosZ == 0) {
            if (gSaveContext.linkAge == LINK_AGE_CHILD) {
                play->nextEntranceIndex = ENTR_JABU_JABU_BOSS_ENTRANCE;
            } else {
                play->nextEntranceIndex = ENTR_WATER_TEMPLE_BOSS_ENTRANCE;
            }
            // Twinrova
        } else if (warpPosX == 100 && warpPosZ == 170) {
            play->nextEntranceIndex = ENTR_SPIRIT_TEMPLE_BOSS_2;
            // Bongo Bongo
        } else if (warpPosX == -100 && warpPosZ == 170) {
            play->nextEntranceIndex = ENTR_SHADOW_TEMPLE_BOSS_ENTRANCE;
            // Ganondork
        } else if (warpPosX == -199 && warpPosZ == 0) {
            play->nextEntranceIndex = ENTR_GANONDORF_BOSS_0;
        } else {
            SPDLOG_ERROR("[BossRush]: Unknown blue warp in chamber of sages at position ({}, {}). Warping back to "
                         "chamber of sages.",
                         warpPosX, warpPosZ);
            play->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
        }
        // If coming from a boss room, teleport back to Chamber of Sages and set flag.
    } else {
        play->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;

        if (CheckDungeonCount() == 3) {
            play->linkAgeOnLoad = LINK_AGE_ADULT;
            gSaveContext.linkAge = LINK_AGE_ADULT;

            // Change to Adult Link.
            if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_ALL) {
                BossRush_SetEquipment(LINK_AGE_ADULT);
                // Warp to credits.
            } else if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_CHILD) {
                play->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
                gSaveContext.nextCutsceneIndex = 0xFFF2;
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->transitionType = TRANS_TYPE_FADE_WHITE;
            }
        }
    }

    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_FADE_WHITE;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE_SLOW;
}

void BossRush_HandleBlueWarpHeal(PlayState* play) {

    // This function gets called multiple times per blue warp, so only heal when player isn't at max HP.
    if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_HEAL] == BR_CHOICE_HEAL_EVERYBOSS &&
        gSaveContext.health != gSaveContext.healthCapacity) {
        Health_ChangeBy(play, 320);
    }
}

void BossRush_HandleCompleteBoss(PlayState* play) {
    gSaveContext.ship.quest.data.bossRush.isPaused = true;
    switch (play->sceneNum) {
        case SCENE_DEKU_TREE_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP);
            break;
        case SCENE_DODONGOS_CAVERN_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP);
            break;
        case SCENE_JABU_JABU_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP);
            break;
        case SCENE_FOREST_TEMPLE_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP);
            break;
        case SCENE_FIRE_TEMPLE_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP);
            break;
        case SCENE_WATER_TEMPLE_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP);
            break;
        case SCENE_SPIRIT_TEMPLE_BOSS:
            Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE);
            break;
        case SCENE_SHADOW_TEMPLE_BOSS:
            Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE);
            break;
        default:
            break;
    }

    // Fully heal the player after Ganondorf
    if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_HEAL] == BR_CHOICE_HEAL_EVERYBOSS &&
        play->sceneNum == SCENE_GANONDORF_BOSS) {
        Health_ChangeBy(play, 320);
    }

    if ((CheckDungeonCount() == 3 &&
         gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_CHILD) ||
        play->sceneNum == SCENE_GANON_BOSS) {
        gSaveContext.ship.stats.playTimer += 2;
        gSaveContext.ship.stats.gameComplete = 1;
        gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_BOSSRUSH_FINISH] =
            static_cast<uint32_t>(GAMEPLAYSTAT_TOTAL_TIME);
    }
}

extern "C" void BossRush_InitSave() {

    // Set player name to Lonk for the few textboxes that show up during Boss Rush. Player can't input their own name.
    std::array<char, 8> brPlayerName = { 21, 50, 49, 46, 62, 62, 62, 62 };
    for (int i = 0; i < ARRAY_COUNT(gSaveContext.playerName); i++) {
        gSaveContext.playerName[i] = brPlayerName[i];
    }

    gSaveContext.ship.quest.id = QUEST_BOSSRUSH;
    gSaveContext.ship.quest.data.bossRush.isPaused = true;
    gSaveContext.entranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
    gSaveContext.cutsceneIndex = 0x8000;
    gSaveContext.isMagicAcquired = 1;

    // Set magic
    if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_MAGIC] == BR_CHOICE_MAGIC_SINGLE) {
        gSaveContext.magicLevel = 1;
        gSaveContext.magic = 48;
    } else {
        gSaveContext.isDoubleMagicAcquired = 1;
        gSaveContext.magicLevel = 2;
        gSaveContext.magic = 96;
    }

    // Set health
    u16 health = 16;
    switch (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_HEARTS]) {
        case BR_CHOICE_HEARTS_7:
            health *= 7;
            break;
        case BR_CHOICE_HEARTS_10:
            health *= 10;
            break;
        case BR_CHOICE_HEARTS_15:
            health *= 15;
            break;
        case BR_CHOICE_HEARTS_20:
            health *= 20;
            break;
        case BR_CHOICE_HEARTS_3:
            health *= 3;
            break;
        case BR_CHOICE_HEARTS_5:
            health *= 5;
            break;
        default:
            break;
    }

    gSaveContext.healthCapacity = health;
    gSaveContext.health = health;

    // Skip boss cutscenes
    gSaveContext.eventChkInf[7] |= 1;    // gohma
    gSaveContext.eventChkInf[7] |= 2;    // dodongo
    gSaveContext.eventChkInf[7] |= 4;    // phantom ganon
    gSaveContext.eventChkInf[7] |= 8;    // volvagia
    gSaveContext.eventChkInf[7] |= 0x10; // morpha
    gSaveContext.eventChkInf[7] |= 0x20; // twinrova
    gSaveContext.eventChkInf[7] |= 0x40; // barinade
    gSaveContext.eventChkInf[7] |= 0x80; // bongo bongo

    // Sets all rando flags to false
    // Boss Rush currently uses 2 randomizer flags (RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE &
    // RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE)
    for (s32 i = 0; i < ARRAY_COUNT(gSaveContext.ship.randomizerInf); i++) {
        gSaveContext.ship.randomizerInf[i] = 0;
    }

    // Set items
    std::array<u8, 24> brItems = {
        ITEM_STICK,       ITEM_NUT,      ITEM_BOMB, ITEM_BOW,  ITEM_NONE,      ITEM_NONE, ITEM_SLINGSHOT, ITEM_NONE,
        ITEM_NONE,        ITEM_HOOKSHOT, ITEM_NONE, ITEM_NONE, ITEM_BOOMERANG, ITEM_LENS, ITEM_NONE,      ITEM_HAMMER,
        ITEM_ARROW_LIGHT, ITEM_NONE,     ITEM_NONE, ITEM_NONE, ITEM_NONE,      ITEM_NONE, ITEM_NONE,      ITEM_NONE,
    };

    if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_LONGSHOT] == BR_CHOICE_LONGSHOT_YES) {
        brItems[9] = ITEM_LONGSHOT;
    }

    switch (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_BOTTLE]) {
        case BR_CHOICE_BOTTLE_EMPTY:
            brItems[18] = ITEM_BOTTLE;
            break;
        case BR_CHOICE_BOTTLE_FAIRY:
            brItems[18] = ITEM_FAIRY;
            break;
        case BR_CHOICE_BOTTLE_REDPOTION:
            brItems[18] = ITEM_POTION_RED;
            break;
        case BR_CHOICE_BOTTLE_GREENPOTION:
            brItems[18] = ITEM_POTION_GREEN;
            break;
        case BR_CHOICE_BOTTLE_BLUEPOTION:
            brItems[18] = ITEM_POTION_BLUE;
            break;
        default:
            break;
    }

    if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_BUNNYHOOD] == BR_CHOICE_BUNNYHOOD_YES) {
        brItems[23] = ITEM_MASK_BUNNY;
    }

    for (int item = 0; item < ARRAY_COUNT(gSaveContext.inventory.items); item++) {
        gSaveContext.inventory.items[item] = brItems[item];
    }

    // Set consumable counts
    std::array<s8, 16> brAmmo = { 5, 5, 10, 10, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_AMMO] == BR_CHOICE_AMMO_FULL) {
        brAmmo = { 10, 20, 20, 30, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    } else if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_AMMO] == BR_CHOICE_AMMO_MAXED) {
        brAmmo = { 30, 40, 40, 50, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    }

    for (int ammo = 0; ammo < ARRAY_COUNT(gSaveContext.inventory.ammo); ammo++) {
        gSaveContext.inventory.ammo[ammo] = brAmmo[ammo];
    }

    // Equipment
    gSaveContext.inventory.equipment |= 1 << 0; // Kokiri Sword
    gSaveContext.inventory.equipment |= 1 << 1; // Master Sword
    gSaveContext.inventory.equipment |= 1 << 4; // Deku Shield
    gSaveContext.inventory.equipment |= 1 << 6; // Mirror Shield
    gSaveContext.inventory.equipment |= 1 << 9; // Goron Tunic
    if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_BGS] == BR_CHOICE_BGS_YES) {
        gSaveContext.inventory.equipment |= 1 << 2; // Biggoron Sword
        gSaveContext.bgsFlag = 1;
    }
    if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_HOVERBOOTS] == BR_CHOICE_HOVERBOOTS_YES) {
        gSaveContext.inventory.equipment |= 1 << 14; // Hover Boots
    }

    // Upgrades
    u8 upgradeLevel = 1;
    if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_AMMO] == BR_CHOICE_AMMO_MAXED) {
        upgradeLevel = 3;
    }
    Inventory_ChangeUpgrade(UPG_QUIVER, upgradeLevel);
    Inventory_ChangeUpgrade(UPG_BOMB_BAG, upgradeLevel);
    Inventory_ChangeUpgrade(UPG_BULLET_BAG, upgradeLevel);
    Inventory_ChangeUpgrade(UPG_STICKS, upgradeLevel);
    Inventory_ChangeUpgrade(UPG_NUTS, upgradeLevel);
    Inventory_ChangeUpgrade(UPG_STRENGTH, 1);

    // Set flags and Link's age based on chosen settings.
    if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_ADULT ||
        gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_GANONDORF_GANON) {
        Flags_SetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP);
        Flags_SetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP);
        Flags_SetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP);
        if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_GANONDORF_GANON) {
            Flags_SetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP);
            Flags_SetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP);
            Flags_SetEventChkInf(EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP);
            Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE);
            Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE);
        }
        gSaveContext.linkAge = LINK_AGE_ADULT;
        BossRush_SetEquipment(LINK_AGE_ADULT);
    } else {
        gSaveContext.linkAge = LINK_AGE_CHILD;
        BossRush_SetEquipment(LINK_AGE_CHILD);
    }
}

static void* sSavePromptNoChoiceTexs[] = {
    (void*)gPauseNoENGTex,
    (void*)gPauseNoGERTex,
    (void*)gPauseNoFRATex,
};

void BossRush_OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);

    switch (id) {
        // Allow not healing before ganon
        case VB_GANON_HEAL_BEFORE_FIGHT: {
            if (gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_HEAL] == BR_CHOICE_HEAL_NEVER) {
                *should = false;
            }
            break;
        }
        case VB_PLAY_BLUE_WARP_CS: {
            *should = false;
            break;
        }
        // Spawn clean blue warps (no ruto, adult animation, etc)
        case VB_SPAWN_BLUE_WARP: {
            switch (gPlayState->sceneNum) {
                case SCENE_DEKU_TREE_BOSS: {
                    BossGoma* bossGoma = va_arg(args, BossGoma*);
                    static Vec3f roomCenter = { -150.0f, 0.0f, -350.0f };
                    Vec3f childPos = roomCenter;

                    for (s32 i = 0; i < 10000; i++) {
                        if ((fabsf(childPos.x - GET_PLAYER(gPlayState)->actor.world.pos.x) < 100.0f &&
                             fabsf(childPos.z - GET_PLAYER(gPlayState)->actor.world.pos.z) < 100.0f) ||
                            (fabsf(childPos.x - bossGoma->actor.world.pos.x) < 150.0f &&
                             fabsf(childPos.z - bossGoma->actor.world.pos.z) < 150.0f)) {
                            childPos.x = Rand_CenteredFloat(400.0f) + -150.0f;
                            childPos.z = Rand_CenteredFloat(400.0f) + -350.0f;
                        } else {
                            break;
                        }
                    }
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, childPos.x,
                                bossGoma->actor.world.pos.y, childPos.z, 0, 0, 0, WARP_DUNGEON_ADULT, false);
                    break;
                }
                case SCENE_DODONGOS_CAVERN_BOSS: {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, -890.0f, -1523.76f, -3304.0f, 0, 0,
                                0, WARP_DUNGEON_ADULT, false);
                    break;
                }
                case SCENE_JABU_JABU_BOSS: {
                    static Vec3f sWarpPos[] = {
                        { 10.0f, 0.0f, 30.0f },
                        { 260.0f, 0.0f, -470.0f },
                        { -240.0f, 0.0f, -470.0f },
                    };

                    s32 sp7C = 2;
                    for (s32 i = 2; i > 0; i -= 1) {
                        if (Math_Vec3f_DistXYZ(&sWarpPos[i], &GET_PLAYER(gPlayState)->actor.world.pos) <
                            Math_Vec3f_DistXYZ(&sWarpPos[i - 1], &GET_PLAYER(gPlayState)->actor.world.pos)) {
                            sp7C = i - 1;
                        }
                    }

                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, sWarpPos[sp7C].x, sWarpPos[sp7C].y,
                                sWarpPos[sp7C].z, 0, 0, 0, WARP_DUNGEON_ADULT, false);
                    break;
                }
                case SCENE_FOREST_TEMPLE_BOSS: {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, 14.0f, -33.0f, -3315.0f, 0, 0, 0,
                                WARP_DUNGEON_ADULT, true);
                    break;
                }
                case SCENE_FIRE_TEMPLE_BOSS: {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, 0.0f, 100.0f, 0.0f, 0, 0, 0,
                                WARP_DUNGEON_ADULT, true);
                    break;
                }
                case SCENE_WATER_TEMPLE_BOSS: {
                    BossMo* bossMo = va_arg(args, BossMo*);
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, bossMo->actor.world.pos.x, -280.0f,
                                bossMo->actor.world.pos.z, 0, 0, 0, WARP_DUNGEON_ADULT, true);
                    break;
                }
                case SCENE_SPIRIT_TEMPLE_BOSS: {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, 600.0f, 230.0f, 0.0f, 0, 0, 0,
                                WARP_DUNGEON_ADULT, true);
                    break;
                }
                case SCENE_SHADOW_TEMPLE_BOSS: {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, -50.0f, 0.0f, 400.0f, 0, 0, 0,
                                WARP_DUNGEON_ADULT, true);
                    break;
                }
                default: {
                    SPDLOG_WARN("[BossRush]: Blue warp spawned in unhandled scene, ignoring");
                    return;
                }
            }
            *should = false;
            break;
        }
        // Skip past the "Save?" window when pressing B while paused and instead close the menu.
        case VB_CLOSE_PAUSE_MENU: {
            if (CHECK_BTN_ALL(gPlayState->state.input[0].press.button, BTN_B)) {
                *should = true;
            }
            break;
        }
        // Show "No" twice because the player can't continue.
        case VB_RENDER_YES_ON_CONTINUE_PROMPT: {
            Gfx** disp = va_arg(args, Gfx**);
            *disp = KaleidoScope_QuadTextureIA8(*disp, sSavePromptNoChoiceTexs[gSaveContext.language], 48, 16, 12);
            *should = false;
            break;
        }
        // Break the dodongo breakable floor immediately so the player can jump in the hole immediately.
        case VB_BG_BREAKWALL_BREAK: {
            *should = true;
            break;
        }
        // Skip past the "Save?" window when dying and go to the "Continue?" screen immediately.
        case VB_TRANSITION_TO_SAVE_SCREEN_ON_DEATH: {
            PauseContext* pauseCtx = va_arg(args, PauseContext*);
            pauseCtx->state = 0xF;
            *should = false;
            break;
        }
        // Handle the heal on blue warp
        case VB_BLUE_WARP_CONSIDER_ADULT_IN_RANGE: {
            if (*should) {
                BossRush_HandleBlueWarpHeal(gPlayState);
            }
            break;
        }
        case VB_SHOW_GAMEPLAY_TIMER: {
            *should |= gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_TIMER] == BR_CHOICE_TIMER_YES;
            break;
        }
        // Prevent saving
        case VB_BE_ABLE_TO_SAVE:
        // Disable doors so the player can't leave the boss rooms backwards.
        case VB_BE_ABLE_TO_OPEN_DOORS:
        // There's no heart containers in boss rush
        case VB_SPAWN_HEART_CONTAINER:
        // Rupees are useless in boss rush
        case VB_RENDER_RUPEE_COUNTER: {
            *should = false;
            break;
        }
        // Prevent warning spam
        default: {
            break;
        }
    }

    va_end(args);
}

void BossRush_OnSceneInitHandler(s16 sceneNum) {
    // Unpause the timer when the scene loaded isn't the Chamber of Sages.
    if (sceneNum != SCENE_CHAMBER_OF_THE_SAGES) {
        gSaveContext.ship.quest.data.bossRush.isPaused = false;
    }
}

void BossRush_OnBossDefeatHandler(void* refActor) {
    BossRush_HandleCompleteBoss(gPlayState);
}

void BossRush_OnBlueWarpUpdate(void* actor) {
    DoorWarp1* blueWarp = static_cast<DoorWarp1*>(actor);

    if (blueWarp->warpTimer > 160) {
        BossRush_HandleBlueWarp(gPlayState, blueWarp->actor.world.pos.x, blueWarp->actor.world.pos.z);
    }
}

void RegisterBossRush() {
    COND_HOOK(OnLoadGame, true, [](int32_t fileNum) {
        COND_ID_HOOK(OnActorInit, ACTOR_DEMO_SA, IS_BOSS_RUSH, [](void* actorPtr) {
            BossRush_SpawnBlueWarps(gPlayState);
            Actor_Kill((Actor*)actorPtr);
            GET_PLAYER(gPlayState)->actor.world.rot.y = 27306;
            GET_PLAYER(gPlayState)->actor.shape.rot.y = 27306;
        });

        // Remove bushes, used in Gohma's arena
        COND_ID_HOOK(OnActorInit, ACTOR_EN_KUSA, IS_BOSS_RUSH, [](void* actorPtr) { Actor_Kill((Actor*)actorPtr); });

        // Remove pots, used in Barinade's and Ganondorf's arenas
        COND_ID_HOOK(OnActorInit, ACTOR_OBJ_TSUBO, IS_BOSS_RUSH, [](void* actorPtr) { Actor_Kill((Actor*)actorPtr); });

        // Remove chests, mainly for the chest in King Dodongo's boss room
        COND_ID_HOOK(OnActorInit, ACTOR_EN_BOX, IS_BOSS_RUSH, [](void* actorPtr) { Actor_Kill((Actor*)actorPtr); });

        COND_HOOK(OnVanillaBehavior, IS_BOSS_RUSH, BossRush_OnVanillaBehaviorHandler);

        COND_HOOK(OnSceneInit, IS_BOSS_RUSH, BossRush_OnSceneInitHandler);

        COND_HOOK(OnBossDefeat, IS_BOSS_RUSH, BossRush_OnBossDefeatHandler);

        COND_ID_HOOK(OnActorUpdate, ACTOR_DOOR_WARP1, IS_BOSS_RUSH, BossRush_OnBlueWarpUpdate);
    });
}

static RegisterShipInitFunc initFunc(RegisterBossRush);