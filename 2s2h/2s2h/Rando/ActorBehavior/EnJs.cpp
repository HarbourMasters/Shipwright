#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Js/z_en_js.h"
#include "objects/object_stk/object_stk.h"
s32 func_80968DD0(EnJs* enJs, PlayState* play);
void func_8096971C(EnJs* enJs, PlayState* play);
extern Vec3f D_8096AC30;
}

void EnJs_PromptForDialog(EnJs* enJs, PlayState* play);

void OverrideSubJsText(u16* textId, bool* loadFromMessageTable) {
    Player* player = GET_PLAYER(gPlayState);
    if (player->talkActor == NULL) {
        return;
    }
    s32 jsType = ENJS_GET_TYPE(player->talkActor);
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

    switch (*textId) {
        // Do you want to play? Yes/No
        case 0x2215: {
            switch (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS]) {
                case RO_ACCESS_TRIALS_20_MASKS: {
                    u8 requiredMasks = 0;
                    switch (jsType) {
                        case 1:
                            requiredMasks = 2;
                            break;
                        case 2:
                            requiredMasks = 6;
                            break;
                        case 3:
                            requiredMasks = 12;
                            break;
                        case 4:
                            requiredMasks = 20;
                            break;
                    }

                    if (Rando::Logic::MoonMaskCount() < requiredMasks) {
                        entry.msg =
                            "You need more masks to play... Come back when you have at least {{requiredMasks}}...";
                        entry.nextMessageID = 0x2216;
                        CustomMessage::Replace(&entry.msg, "{{requiredMasks}}", std::to_string(requiredMasks));
                    }
                    break;
                }
                case RO_ACCESS_TRIALS_REMAINS: {
                    QuestItem questItem = QUEST_17; // Unused
                    RandoItemId itemId = RI_NONE;
                    switch (jsType) {
                        case 1:
                            questItem = QUEST_REMAINS_ODOLWA;
                            itemId = RI_REMAINS_ODOLWA;
                            break;
                        case 2:
                            questItem = QUEST_REMAINS_GOHT;
                            itemId = RI_REMAINS_GOHT;
                            break;
                        case 3:
                            questItem = QUEST_REMAINS_GYORG;
                            itemId = RI_REMAINS_GYORG;
                            break;
                        case 4:
                            questItem = QUEST_REMAINS_TWINMOLD;
                            itemId = RI_REMAINS_TWINMOLD;
                            break;
                    }
                    if (questItem != QUEST_17 && !CHECK_QUEST_ITEM(questItem)) {
                        entry.msg = "You need to find {{item}} before you can play...";
                        entry.nextMessageID = 0x2216;
                        CustomMessage::Replace(&entry.msg, "{{item}}", Rando::StaticData::GetItemName(itemId));
                    }
                    break;
                }
                case RO_ACCESS_TRIALS_FORMS:
                    if (jsType == 1 && !HAS_ITEM(ITEM_MASK_DEKU)) {
                        entry.msg = "You need to find the Deku Mask before you can play...";
                        entry.nextMessageID = 0x2216;
                    } else if (jsType == 2 && !HAS_ITEM(ITEM_MASK_GORON)) {
                        entry.msg = "You need to find the Goron Mask before you can play...";
                        entry.nextMessageID = 0x2216;
                    } else if (jsType == 3 && !HAS_ITEM(ITEM_MASK_ZORA)) {
                        entry.msg = "You need to find the Zora Mask before you can play...";
                        entry.nextMessageID = 0x2216;
                    }
                    break;
            }

            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
            break;
        }
        // You don't want to play? oh...
        case 0x2216: {
            auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
            entry.msg = "...\xE0";
            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
            return;
        }
    }
}

void OverrideMainJsText(u16* textId, bool* loadFromMessageTable) {
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    bool override = false;

    switch (*textId) {
        // Will you play...with me? Yes/No
        case 0x21FC: {
            if (!RANDO_SAVE_CHECKS[RC_MOON_FIERCE_DEITY_MASK].cycleObtained && Rando::Logic::MoonMaskCount() >= 20) {
                RANDO_SAVE_CHECKS[RC_MOON_FIERCE_DEITY_MASK].eligible = true;
                entry.msg = "You... You seem strong... I have a gift for you...";
                entry.nextMessageID = 0x21FD;
                override = true;
            }

            if (Rando::Logic::MoonMaskCount() < RANDO_SAVE_OPTIONS[RO_ACCESS_MAJORA_MASKS_COUNT] ||
                Rando::Logic::RemainsCount() < RANDO_SAVE_OPTIONS[RO_ACCESS_MAJORA_REMAINS_COUNT]) {
                entry.msg = "You are not strong enough to play with me...";
                entry.nextMessageID = 0x21FD;
                override = true;
            }
            break;
        }
        // Skip having to say yes twice
        // "You only have weak masks...So...you'll play?"
        case 0x21FE: {
            *textId = 0x2200;
            break;
        }
        // You're a boring kid.
        case 0x21FD: {
            entry.msg = "...\xE0";
            override = true;
            break;
        }
    }
    if (override) {
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    }
}

void EnJs_PostLimbDraw_LinkMask(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    if (limbIndex == MOONCHILD_LIMB_HEAD) {
        Matrix_MultVec3f(&D_8096AC30, &thisx->focus.pos);
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_RotateZYX(0x8000, 0x8000, -0x4000, MTXMODE_APPLY);
        Matrix_Translate(-128.0, -256.0, 0.0, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gSkullKidLinkMask1DL);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnJs_Draw_LinkMask(Actor* thisx, PlayState* play) {
    EnJs* enJs = (EnJs*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, enJs->skelAnime.skeleton, enJs->skelAnime.jointTable, enJs->skelAnime.dListCount, NULL,
                          EnJs_PostLimbDraw_LinkMask, &enJs->actor);
}

void EnJs_ResetTime(EnJs* enJs, PlayState* play) {
    func_8096971C(enJs, play);
    if (play->msgCtx.ocarinaMode == OCARINA_MODE_APPLY_SOT) {
        play->nextEntrance = ENTRANCE(CUTSCENE, 0);
        gSaveContext.nextCutsceneIndex = 0xFFF7;
        play->transitionTrigger = TRANS_TRIGGER_START;
        enJs->actionFunc = func_8096971C;
    } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play) &&
               play->msgCtx.choiceIndex == 1) {
        enJs->actionFunc = EnJs_PromptForDialog;
    }
}

void EnJs_PromptForDialog(EnJs* enJs, PlayState* play) {
    func_8096971C(enJs, play);
    if (Actor_TalkOfferAccepted(&enJs->actor, &play->state)) {
        enJs->actionFunc = EnJs_ResetTime;
        Message_StartTextbox(play, 0x1B8A, &enJs->actor);
        play->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_SOT;
    } else if (func_80968DD0(enJs, play)) {
        Actor_OfferTalk(&enJs->actor, play, 120.0f);
    }
}

void EnJs_SpawnResetNpc(float rotY) {
    u32 params = 8;
    Actor* actor = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_JS, -150, 40, 50, 0, rotY, 0, params);
    actor->draw = EnJs_Draw_LinkMask;
    ((EnJs*)actor)->actionFunc = EnJs_PromptForDialog;
}

void Rando::ActorBehavior::InitEnJsBehavior() {
    bool shouldOverrideTrialsAccess = IS_RANDO && RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] != RO_ACCESS_TRIALS_VANILLA;

    COND_VB_SHOULD(VB_JS_CONSIDER_ELIGIBLE_FOR_DEITY, IS_RANDO, { *should = false; });

    COND_VB_SHOULD(VB_JS_OVERRIDE_MASK_CHECK, shouldOverrideTrialsAccess, {
        s32* jsType = va_arg(args, s32*);
        bool* result = va_arg(args, bool*);

        switch (*jsType) {
            case 1:
            case 2:
            case 3:
            case 4:
                *should = true;
                *result = true;
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                if (gPlayState->sceneId != SCENE_SOUGEN) {
                    *should = true;
                    *result = false;
                }
                break;
            default:
                break;
        }
    });

    COND_ID_HOOK(OnSceneInit, SCENE_SOUGEN, IS_RANDO, [](s8 sceneId, s8 spawnNum) { EnJs_SpawnResetNpc(-15000); });
    COND_ID_HOOK(OnSceneInit, SCENE_OKUJOU, IS_RANDO, [](s8 sceneId, s8 spawnNum) { EnJs_SpawnResetNpc(7000); });

    // Need to disable this for spawning on the Clock Tower roof
    COND_VB_SHOULD(VB_ENABLE_OBJECT_DEPENDENCY, IS_RANDO, {
        ObjectId objectId = (ObjectId)va_arg(args, int);
        if (objectId == OBJECT_OB) {
            *should = false;
        }
    });

    COND_ID_HOOK(OnActorInit, ACTOR_EN_JS, IS_RANDO, [](Actor* actor) {
        if (actor->draw == EnJs_Draw_LinkMask) {
            ((EnJs*)actor)->actionFunc = EnJs_PromptForDialog;
            actor->world.pos.y = 0;
        }
    });

    COND_ID_HOOK(OnOpenText, 0x2215, shouldOverrideTrialsAccess, OverrideSubJsText);
    COND_ID_HOOK(OnOpenText, 0x2216, shouldOverrideTrialsAccess, OverrideSubJsText);
    COND_ID_HOOK(OnOpenText, 0x21FC, IS_RANDO, OverrideMainJsText);
    COND_ID_HOOK(OnOpenText, 0x21FE, IS_RANDO, OverrideMainJsText);
    COND_ID_HOOK(OnOpenText, 0x21FD, IS_RANDO, OverrideMainJsText);
}
