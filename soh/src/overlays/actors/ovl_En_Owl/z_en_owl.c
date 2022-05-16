/*
 * File: z_en_owl.c
 * Overlay: ovl_En_Owl
 * Description: Owl
 */

#include "z_en_owl.h"
#include "objects/object_owl/object_owl.h"
#include "scenes/overworld/spot06/spot06_scene.h"
#include "scenes/overworld/spot16/spot16_scene.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnOwl_Init(Actor* thisx, GlobalContext* globalCtx);
void EnOwl_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnOwl_Update(Actor* thisx, GlobalContext* globalCtx);
void EnOwl_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnOwl_ChangeMode(EnOwl* this, EnOwlActionFunc, OwlFunc, SkelAnime*, AnimationHeader*, f32);
void EnOwl_WaitDefault(EnOwl* this, GlobalContext* globalCtx);
void func_80ACC540(EnOwl* this);
void EnOwl_WaitOutsideKokiri(EnOwl* this, GlobalContext* globalCtx);
void EnOwl_WaitHyruleCastle(EnOwl* this, GlobalContext* globalCtx);
void EnOwl_WaitKakariko(EnOwl* this, GlobalContext* globalCtx);
void EnOwl_WaitGerudo(EnOwl* this, GlobalContext* globalCtx);
void EnOwl_WaitLakeHylia(EnOwl* this, GlobalContext* globalCtx);
void EnOwl_WaitZoraRiver(EnOwl* this, GlobalContext* globalCtx);
void EnOwl_WaitHyliaShortcut(EnOwl* this, GlobalContext* globalCtx);
void EnOwl_WaitDeathMountainShortcut(EnOwl* this, GlobalContext* globalCtx);
void func_80ACB3E0(EnOwl* this, GlobalContext* globalCtx);
void EnOwl_WaitLWPreSaria(EnOwl* this, GlobalContext* globalCtx);
void EnOwl_WaitLWPostSaria(EnOwl* this, GlobalContext* globalCtx);
void func_80ACD4D4(EnOwl* this, GlobalContext* globalCtx);
void func_80ACD130(EnOwl* this, GlobalContext* globalCtx, s32 arg2);
void func_80ACBAB8(EnOwl* this, GlobalContext* globalCtx);
void func_80ACD2CC(EnOwl* this, GlobalContext* globalCtx);
void func_80ACAA54(EnOwl* this, GlobalContext* globalCtx);
void func_80ACAC6C(EnOwl* this, GlobalContext* globalCtx);
void func_80ACADF0(EnOwl* this, GlobalContext* globalCtx);
void func_80ACAF74(EnOwl* this, GlobalContext* globalCtx);
void func_80ACC30C(EnOwl* this, GlobalContext* globalCtx);
void func_80ACB4FC(EnOwl* this, GlobalContext* globalCtx);
void func_80ACB680(EnOwl* this, GlobalContext* globalCtx);
void func_80ACC460(EnOwl* this);
void func_80ACBEA0(EnOwl*, GlobalContext*);

typedef enum {
    /* 0x00 */ OWL_DEFAULT,
    /* 0x01 */ OWL_OUTSIDE_KOKIRI,
    /* 0x02 */ OWL_HYRULE_CASTLE,
    /* 0x03 */ OWL_KAKARIKO,
    /* 0x04 */ OWL_HYLIA_GERUDO,
    /* 0x05 */ OWL_LAKE_HYLIA,
    /* 0x06 */ OWL_ZORA_RIVER,
    /* 0x07 */ OWL_HYLIA_SHORTCUT,
    /* 0x08 */ OWL_DEATH_MOUNTAIN,
    /* 0x09 */ OWL_DEATH_MOUNTAIN2,
    /* 0x0A */ OWL_DESSERT_COLOSSUS,
    /* 0x0B */ OWL_LOST_WOODS_PRESARIA,
    /* 0x0C */ OWL_LOST_WOODS_POSTSARIA
} EnOwlType;

typedef enum {
    /* 0x00 */ OWL_REPEAT,
    /* 0x01 */ OWL_OK
} EnOwlMessageChoice;

const ActorInit En_Owl_InitVars = {
    ACTOR_EN_OWL,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_OWL,
    sizeof(EnOwl),
    (ActorFunc)EnOwl_Init,
    (ActorFunc)EnOwl_Destroy,
    (ActorFunc)EnOwl_Update,
    (ActorFunc)EnOwl_Draw,
    NULL,
};

static ColliderCylinderInit sOwlCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 25, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 2400, ICHAIN_STOP),
};

void EnOwl_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnOwl* this = (EnOwl*)thisx;
    ColliderCylinder* collider;
    s32 owlType;
    s32 switchFlag;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gOwlFlyingSkel, &gOwlFlyAnim, this->jointTable, this->morphTable,
                       21);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime2, &gOwlPerchingSkel, &gOwlPerchAnim, this->jointTable2,
                       this->morphTable2, 16);
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sOwlCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.minVelocityY = -10.0f;
    this->actor.targetArrowOffset = 500.0f;
    EnOwl_ChangeMode(this, EnOwl_WaitDefault, func_80ACC540, &this->skelAnime2, &gOwlPerchAnim, 0.0f);
    this->actionFlags = this->unk_406 = this->unk_409 = 0;
    this->unk_405 = 4;
    this->unk_404 = this->unk_407 = 0;
    this->unk_408 = 4;
    owlType = (this->actor.params & 0xFC0) >> 6;
    switchFlag = (this->actor.params & 0x3F);
    if (this->actor.params == 0xFFF) {
        owlType = OWL_OUTSIDE_KOKIRI;
        switchFlag = 0x20;
    }
    // "conversation owl %4x no = %d, sv = %d"
    osSyncPrintf(VT_FGCOL(CYAN) " 会話フクロウ %4x no = %d, sv = %d\n" VT_RST, this->actor.params, owlType, switchFlag);

    if ((owlType != OWL_DEFAULT) && (switchFlag < 0x20) && Flags_GetSwitch(globalCtx, switchFlag)) {
        osSyncPrintf("savebitでフクロウ退避\n"); // "Save owl with savebit"
        Actor_Kill(&this->actor);
        return;
    }

    this->unk_3EE = 0;
    this->unk_400 = this->actor.world.rot.y;

    switch (owlType) {
        case OWL_DEFAULT:
            this->actionFunc = EnOwl_WaitDefault;
            this->actor.uncullZoneForward = 4000.0f;
            this->unk_40A = 0;
            break;
        case OWL_OUTSIDE_KOKIRI:
            this->actionFunc = EnOwl_WaitOutsideKokiri;
            break;
        case OWL_HYRULE_CASTLE:
            this->actionFlags |= 2;
            this->unk_3EE = 0x20;
            this->actionFunc = EnOwl_WaitHyruleCastle;
            break;
        case OWL_KAKARIKO:
            if (gSaveContext.eventChkInf[4] & 1) {
                // has zelda's letter
                osSyncPrintf("フクロウ退避\n"); // "Owl evacuation"
                Actor_Kill(&this->actor);
                return;
            }

            this->actionFunc = EnOwl_WaitKakariko;
            break;
        case OWL_HYLIA_GERUDO:
            if (gSaveContext.eventChkInf[4] & 8) {
                // has ocarina of time
                osSyncPrintf("フクロウ退避\n"); // "Owl evacuation"
                Actor_Kill(&this->actor);
                return;
            }
            this->actionFunc = EnOwl_WaitGerudo;
            break;
        case OWL_LAKE_HYLIA:
            this->actionFunc = EnOwl_WaitLakeHylia;
            break;
        case OWL_ZORA_RIVER:
            if ((gSaveContext.eventChkInf[3] & 0x200) || !(gSaveContext.eventChkInf[4] & 1)) {
                // opened zora's domain or has zelda's letter
                osSyncPrintf("フクロウ退避\n"); // "Owl evacuation"
                Actor_Kill(&this->actor);
                return;
            }

            this->actionFunc = EnOwl_WaitZoraRiver;
            break;
        case OWL_HYLIA_SHORTCUT:
            this->actionFunc = EnOwl_WaitHyliaShortcut;
            Flags_UnsetSwitch(globalCtx, 0x23);
            return;
        case OWL_DEATH_MOUNTAIN:
            this->actionFunc = EnOwl_WaitDeathMountainShortcut;
            break;
        case OWL_DEATH_MOUNTAIN2:
            this->actionFunc = EnOwl_WaitDeathMountainShortcut;
            break;
        case OWL_DESSERT_COLOSSUS:
            this->actionFunc = func_80ACB3E0;
            break;
        case OWL_LOST_WOODS_PRESARIA:
            if (!CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
                osSyncPrintf("フクロウ退避\n"); // "Owl evacuation"
                Actor_Kill(&this->actor);
                return;
            }
            this->actionFunc = EnOwl_WaitLWPreSaria;
            break;
        case OWL_LOST_WOODS_POSTSARIA:
            if (!CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) {
                osSyncPrintf("フクロウ退避\n"); // "Owl evacuation"
                Actor_Kill(&this->actor);
                return;
            }
            this->actionFunc = EnOwl_WaitLWPostSaria;
            break;
        default:
            // Outside kokiri forest
            osSyncPrintf(VT_FGCOL(CYAN));
            osSyncPrintf("no = %d  \n", owlType);
            // "Unfinished owl unfinished owl unfinished owl"
            osSyncPrintf("未完成のフクロウ未完成のフクロウ未完成のフクロウ\n");
            osSyncPrintf(VT_RST);
            this->actionFlags |= 2;
            this->unk_3EE = 0x20;
            this->actionFunc = EnOwl_WaitOutsideKokiri;
            break;
    }
}

void EnOwl_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnOwl* this = (EnOwl*)thisx;

    Collider_DestroyCylinder(globalCtx, &this->collider);
}

/**
 * Rotates this to the player instance
 */
void EnOwl_LookAtLink(EnOwl* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    this->actor.shape.rot.y = this->actor.world.rot.y =
        Math_Vec3f_Yaw(&this->actor.world.pos, &player->actor.world.pos);
}

/**
 * Checks if link is within `targetDist` units, initalize the camera for the owl.
 * returns 0 if the link is not within `targetDistance`, returns 1 once link is within
 * the distance, and the camera has been initalized.
 */
s32 EnOwl_CheckInitTalk(EnOwl* this, GlobalContext* globalCtx, u16 textId, f32 targetDist, u16 flags) {
    s32 timer;
    f32 distCheck;

    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        if (this->actor.params == 0xFFF) {
            this->actionFlags |= 0x40;
            timer = -100;
        } else {
            if (Rand_ZeroOne() < 0.5f) {
                timer = (flags & 1) ? -97 : -99;
                this->actionFlags |= 0x40;
            } else {
                timer = (flags & 1) ? -96 : -98;
                this->actionFlags &= ~0x40;
            }
        }
        this->cameraIdx = OnePointCutscene_Init(globalCtx, 8700, timer, &this->actor, MAIN_CAM);
        return true;
    } else {
        this->actor.textId = textId;
        distCheck = (flags & 2) ? 200.0f : 1000.0f;
        if (this->actor.xzDistToPlayer < targetDist) {
            this->actor.flags |= ACTOR_FLAG_16;
            func_8002F1C4(&this->actor, globalCtx, targetDist, distCheck, 0);
        }
        return false;
    }
}

s32 func_80ACA558(EnOwl* this, GlobalContext* globalCtx, u16 textId) {
    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        return true;
    } else {
        this->actor.textId = textId;
        if (this->actor.xzDistToPlayer < 120.0f) {
            func_8002F1C4(&this->actor, globalCtx, 350.0f, 1000.0f, 0);
        }
        return false;
    }
}

void func_80ACA5C8(EnOwl* this) {
    EnOwl_ChangeMode(this, func_80ACBEA0, func_80ACC540, &this->skelAnime, &gOwlUnfoldWingsAnim, 0.0f);
    this->eyeTexIndex = 0;
    this->blinkTimer = Rand_S16Offset(60, 60);
}

void func_80ACA62C(EnOwl* this, GlobalContext* globalCtx) {
    s32 switchFlag = this->actor.params & 0x3F;

    if (switchFlag < 0x20) {
        Flags_SetSwitch(globalCtx, switchFlag);
        osSyncPrintf(VT_FGCOL(CYAN) " Actor_Environment_sw = %d\n" VT_RST, Flags_GetSwitch(globalCtx, switchFlag));
    }
    func_80ACA5C8(this);
}

void func_80ACA690(EnOwl* this, GlobalContext* globalCtx) {
    if ((this->unk_3EE & 0x3F) == 0) {
        func_80ACA62C(this, globalCtx);
    }
}

void func_80ACA6C0(EnOwl* this) {
    if (Rand_CenteredFloat(1.0f) < 0.0f) {
        this->actionFlags |= 0x20;
    } else {
        this->actionFlags &= ~0x20;
    }
}

void func_80ACA71C(EnOwl* this) {
    func_80ACA6C0(this);
    this->unk_3F2 = 0;
    this->actionFlags |= 0x10;
    this->unk_408 = 4;
    this->unk_404 = 0;
    this->unk_406 = 0;
    this->unk_405 = 4;
    this->unk_407 = this->unk_3F2;
}

void func_80ACA76C(EnOwl* this, GlobalContext* globalCtx) {
    func_8002DF54(globalCtx, &this->actor, 8);

    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_FANFARE << 24 | 0xFF);
        func_80ACA62C(this, globalCtx);
        this->actor.flags &= ~ACTOR_FLAG_16;
    }
}

void func_80ACA7E0(EnOwl* this, GlobalContext* globalCtx) {
    func_8002DF54(globalCtx, &this->actor, 8);

    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_FANFARE << 24 | 0xFF);
        if ((this->unk_3EE & 0x3F) == 0) {
            func_80ACA62C(this, globalCtx);
        } else {
            this->actionFlags &= ~2;
            func_80ACA71C(this);
            this->actionFunc = func_80ACA690;
        }
        this->actor.flags &= ~ACTOR_FLAG_16;
    }
}

void EnOwl_ConfirmKokiriMessage(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        uint8_t index = CVar_GetS32("gBetterOwl", 0) == 0 ? globalCtx->msgCtx.choiceIndex : (1 - globalCtx->msgCtx.choiceIndex);
        switch (index) {
            case OWL_REPEAT:
                Message_ContinueTextbox(globalCtx, 0x2065);
                break;
            case OWL_OK:
                Message_ContinueTextbox(globalCtx, 0x2067);
                this->actionFunc = func_80ACA76C;
                break;
        }
    }
}

void EnOwl_WaitOutsideKokiri(EnOwl* this, GlobalContext* globalCtx) {
    EnOwl_LookAtLink(this, globalCtx);

    if (EnOwl_CheckInitTalk(this, globalCtx, 0x2064, 360.0f, 0)) {
        // Sets BGM
        Audio_PlayFanfare(NA_BGM_OWL);

        this->actionFunc = EnOwl_ConfirmKokiriMessage;
        // spoke to owl by lost woods
        gSaveContext.eventChkInf[6] |= 0x8000;
    }
}

void func_80ACA998(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        uint8_t index = CVar_GetS32("gBetterOwl", 0) == 0 ? globalCtx->msgCtx.choiceIndex : (1 - globalCtx->msgCtx.choiceIndex);
        switch (index) {
            case OWL_REPEAT:
                Message_ContinueTextbox(globalCtx, 0x2069);
                this->actionFunc = func_80ACAA54;
                break;
            case OWL_OK:
                Message_ContinueTextbox(globalCtx, 0x206B);
                this->actionFunc = func_80ACA7E0;
                break;
        }
        this->actionFlags &= ~2;
        func_80ACA71C(this);
    }
}

void func_80ACAA54(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x206A);
        this->actionFunc = func_80ACA998;
        this->actionFlags |= 2;
        func_80ACA71C(this);
    }
}

void func_80ACAAC0(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x2069);
        this->actionFunc = func_80ACAA54;
        this->actionFlags &= ~2;
        func_80ACA71C(this);
    }
}

void EnOwl_WaitHyruleCastle(EnOwl* this, GlobalContext* globalCtx) {
    EnOwl_LookAtLink(this, globalCtx);

    if (EnOwl_CheckInitTalk(this, globalCtx, 0x2068, 540.0f, 0)) {
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFunc = func_80ACAAC0;
    }
}

void func_80ACAB88(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        uint8_t index = CVar_GetS32("gBetterOwl", 0) == 0 ? globalCtx->msgCtx.choiceIndex : (1 - globalCtx->msgCtx.choiceIndex);
        switch (index) {
            case OWL_REPEAT:
                // obtained zelda's letter
                if (gSaveContext.eventChkInf[4] & 1) {
                    Message_ContinueTextbox(globalCtx, 0x206D);
                } else {
                    Message_ContinueTextbox(globalCtx, 0x206C);
                }
                this->actionFunc = func_80ACAC6C;
                break;
            case OWL_OK:
                Message_ContinueTextbox(globalCtx, 0x206E);
                this->actionFunc = func_80ACA7E0;
                break;
        }

        this->actionFlags &= ~2;
        func_80ACA71C(this);
    }
}

void func_80ACAC6C(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x206A);
        this->actionFunc = func_80ACAB88;
        this->actionFlags |= 2;
        func_80ACA71C(this);
    }
}

void EnOwl_WaitKakariko(EnOwl* this, GlobalContext* globalCtx) {
    EnOwl_LookAtLink(this, globalCtx);

    if (EnOwl_CheckInitTalk(this, globalCtx, 0x206C, 480.0f, 0)) {
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFunc = func_80ACAC6C;
    }
}

void func_80ACAD34(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        uint8_t index = CVar_GetS32("gBetterOwl", 0) == 0 ? globalCtx->msgCtx.choiceIndex : (1 - globalCtx->msgCtx.choiceIndex);
        switch (index) {
            case OWL_REPEAT:
                Message_ContinueTextbox(globalCtx, 0x206F);
                this->actionFunc = func_80ACADF0;
                break;
            case OWL_OK:
                Message_ContinueTextbox(globalCtx, 0x2070);
                this->actionFunc = func_80ACA7E0;
                break;
        }

        this->actionFlags &= ~2;
        func_80ACA71C(this);
    }
}

void func_80ACADF0(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x206A);
        this->actionFunc = func_80ACAD34;
        this->actionFlags |= 2;
        func_80ACA71C(this);
    }
}

void EnOwl_WaitGerudo(EnOwl* this, GlobalContext* globalCtx) {
    EnOwl_LookAtLink(this, globalCtx);

    if (EnOwl_CheckInitTalk(this, globalCtx, 0x206F, 360.0f, 0)) {
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFunc = func_80ACADF0;
    }
}

void func_80ACAEB8(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        uint8_t index = CVar_GetS32("gBetterOwl", 0) == 0 ? globalCtx->msgCtx.choiceIndex : (1 - globalCtx->msgCtx.choiceIndex);
        switch (index) {
            case OWL_REPEAT:
                Message_ContinueTextbox(globalCtx, 0x2071);
                this->actionFunc = func_80ACAF74;
                break;
            case OWL_OK:
                Message_ContinueTextbox(globalCtx, 0x2072);
                this->actionFunc = func_80ACA7E0;
                break;
        }

        this->actionFlags &= ~2;
        func_80ACA71C(this);
    }
}

void func_80ACAF74(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x206A);
        this->actionFunc = func_80ACAEB8;
        this->actionFlags |= 2;
        func_80ACA71C(this);
    }
}

void EnOwl_WaitLakeHylia(EnOwl* this, GlobalContext* globalCtx) {
    EnOwl_LookAtLink(this, globalCtx);

    if (EnOwl_CheckInitTalk(this, globalCtx, 0x2071, 360.0f, 0)) {
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFunc = func_80ACAF74;
    }
}

void func_80ACB03C(EnOwl* this, GlobalContext* globalCtx) {
    func_8002DF54(globalCtx, &this->actor, 8);

    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_FANFARE << 24 | 0xFF);
        func_80ACA62C(this, globalCtx);
        this->actor.flags &= ~ACTOR_FLAG_16;
    }
}

void EnOwl_WaitZoraRiver(EnOwl* this, GlobalContext* globalCtx) {
    u16 textId;

    EnOwl_LookAtLink(this, globalCtx);

    if (CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
            textId = 0x4031;
        } else {
            textId = 0x4017;
        }
    } else {
        textId = 0x4002;
    }

    if (EnOwl_CheckInitTalk(this, globalCtx, textId, 360.0f, 0)) {
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFunc = func_80ACB03C;
    }
}

void func_80ACB148(EnOwl* this, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_FANFARE << 24 | 0xFF);
        func_80ACA5C8(this);
        this->actionFunc = func_80ACC30C;
        Flags_SetSwitch(globalCtx, 0x23);
    }
}

void EnOwl_WaitHyliaShortcut(EnOwl* this, GlobalContext* globalCtx) {
    u16 textId = (gSaveContext.infTable[25] & 0x20) ? 0x4004 : 0x4003;

    // Spoke to Owl in Lake Hylia
    EnOwl_LookAtLink(this, globalCtx);
    if (func_80ACA558(this, globalCtx, textId)) {
        gSaveContext.infTable[25] |= 0x20;
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFunc = func_80ACB148;
    }
}

void func_80ACB22C(EnOwl* this, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_FANFARE << 24 | 0xFF);
        func_80ACA5C8(this);
        this->actionFunc = func_80ACC30C;
    }
}

void func_80ACB274(EnOwl* this, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_FANFARE << 24 | 0xFF);
        this->actionFunc = EnOwl_WaitDeathMountainShortcut;
    }
}

void EnOwl_WaitDeathMountainShortcut(EnOwl* this, GlobalContext* globalCtx) {
    EnOwl_LookAtLink(this, globalCtx);

    if (!gSaveContext.magicAcquired) {
        if (func_80ACA558(this, globalCtx, 0x3062)) {
            Audio_PlayFanfare(NA_BGM_OWL);
            this->actionFunc = func_80ACB274;
            return;
        }
    } else {
        if (func_80ACA558(this, globalCtx, 0x3063)) {
            Audio_PlayFanfare(NA_BGM_OWL);
            this->actionFunc = func_80ACB22C;
        }
    }
}

void func_80ACB344(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        uint8_t index = CVar_GetS32("gBetterOwl", 0) == 0 ? globalCtx->msgCtx.choiceIndex : (1 - globalCtx->msgCtx.choiceIndex);
        switch (index) {
            case OWL_REPEAT:
                Message_ContinueTextbox(globalCtx, 0x607A);
                break;
            case OWL_OK:
                Message_ContinueTextbox(globalCtx, 0x607C);
                this->actionFunc = func_80ACA7E0;
                break;
        }
    }
}

void func_80ACB3E0(EnOwl* this, GlobalContext* globalCtx) {
    EnOwl_LookAtLink(this, globalCtx);

    if (EnOwl_CheckInitTalk(this, globalCtx, 0x6079, 360.0f, 2)) {
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFunc = func_80ACB344;
    }
}

void func_80ACB440(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        uint8_t index = CVar_GetS32("gBetterOwl", 0) == 0 ? globalCtx->msgCtx.choiceIndex : (1 - globalCtx->msgCtx.choiceIndex);
        switch (index) {
            case OWL_REPEAT:
                Message_ContinueTextbox(globalCtx, 0x10C1);
                this->actionFunc = func_80ACB4FC;
                break;
            case OWL_OK:
                Message_ContinueTextbox(globalCtx, 0x10C3);
                this->actionFunc = func_80ACA7E0;
        }

        this->actionFlags &= ~2;
        func_80ACA71C(this);
    }
}

void func_80ACB4FC(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x10C2);
        this->actionFunc = func_80ACB440;
        this->actionFlags |= 2;
        func_80ACA71C(this);
    }
}

void EnOwl_WaitLWPreSaria(EnOwl* this, GlobalContext* globalCtx) {
    EnOwl_LookAtLink(this, globalCtx);

    if (EnOwl_CheckInitTalk(this, globalCtx, 0x10C0, 190.0f, 0)) {
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFunc = func_80ACB4FC;
    }
}

void func_80ACB5C4(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        uint8_t index = CVar_GetS32("gBetterOwl", 0) == 0 ? globalCtx->msgCtx.choiceIndex : (1 - globalCtx->msgCtx.choiceIndex);
        switch (index) {
            case OWL_REPEAT:
                Message_ContinueTextbox(globalCtx, 0x10C5);
                this->actionFunc = func_80ACB680;
                break;
            case OWL_OK:
                Message_ContinueTextbox(globalCtx, 0x10C7);
                this->actionFunc = func_80ACA7E0;
                break;
        }

        this->actionFlags &= ~2;
        func_80ACA71C(this);
    }
}

void func_80ACB680(EnOwl* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x10C6);
        this->actionFunc = func_80ACB5C4;
        this->actionFlags |= 2;
        func_80ACA71C(this);
    }
}

void EnOwl_WaitLWPostSaria(EnOwl* this, GlobalContext* globalCtx) {
    EnOwl_LookAtLink(this, globalCtx);

    if (EnOwl_CheckInitTalk(this, globalCtx, 0x10C4, 360.0f, 0)) {
        Audio_PlayFanfare(NA_BGM_OWL);
        this->actionFunc = func_80ACB680;
    }
}

void func_80ACB748(EnOwl* this, GlobalContext* globalCtx) {
    static Vec3f D_80ACD62C = { 0.0f, 0.0f, 0.0f };
    f32 dist;
    f32 weight;
    s32 owlType = (this->actor.params & 0xFC0) >> 6;

    dist = Math3D_Vec3f_DistXYZ(&this->eye, &globalCtx->view.eye) / 45.0f;
    this->eye.x = globalCtx->view.eye.x;
    this->eye.y = globalCtx->view.eye.y;
    this->eye.z = globalCtx->view.eye.z;

    weight = dist;
    if (weight > 1.0f) {
        weight = 1.0f;
    }

    switch (owlType) {
        case 7:
            func_800F436C(&D_80ACD62C, NA_SE_EV_FLYING_AIR - SFX_FLAG, weight * 2.0f);
            if ((globalCtx->csCtx.frames > 324) ||
                ((globalCtx->csCtx.frames >= 142 && (globalCtx->csCtx.frames <= 266)))) {
                func_800F4414(&D_80ACD62C, NA_SE_EN_OWL_FLUTTER, weight * 2.0f);
            }
            if (globalCtx->csCtx.frames == 85) {
                func_800F436C(&D_80ACD62C, NA_SE_EV_PASS_AIR, weight * 2.0f);
            }
            break;
        case 8:
        case 9:
            func_800F436C(&D_80ACD62C, NA_SE_EV_FLYING_AIR - SFX_FLAG, weight * 2.0f);
            if ((globalCtx->csCtx.frames >= 420) ||
                ((0xC1 < globalCtx->csCtx.frames && (globalCtx->csCtx.frames <= 280)))) {
                func_800F4414(&D_80ACD62C, NA_SE_EN_OWL_FLUTTER, weight * 2.0f);
            }
            if (globalCtx->csCtx.frames == 217) {
                func_800F436C(&D_80ACD62C, NA_SE_EV_PASS_AIR, weight * 2.0f);
            }
            break;
    }
}

void func_80ACB904(EnOwl* this, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE && (globalCtx->csCtx.npcActions[7] != NULL)) {
        if (this->unk_40A != globalCtx->csCtx.npcActions[7]->action) {
            func_80ACD130(this, globalCtx, 7);
            func_80ACBAB8(this, globalCtx);
        }
        func_80ACD2CC(this, globalCtx);
    }

    if (this->actionFlags & 0x80) {
        func_80ACB748(this, globalCtx);
    }
}

void func_80ACB994(EnOwl* this, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE && (globalCtx->csCtx.npcActions[7] != NULL)) {
        if (this->unk_40A != globalCtx->csCtx.npcActions[7]->action) {
            func_80ACD130(this, globalCtx, 7);
            func_80ACBAB8(this, globalCtx);
        }
        func_80ACD4D4(this, globalCtx);
    }

    if (this->actionFlags & 0x80) {
        func_80ACB748(this, globalCtx);
    }
}

void EnOwl_WaitDefault(EnOwl* this, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE && (globalCtx->csCtx.npcActions[7] != NULL)) {
        if (this->unk_40A != globalCtx->csCtx.npcActions[7]->action) {
            this->actionFlags |= 4;
            func_80ACD130(this, globalCtx, 7);
            func_80ACBAB8(this, globalCtx);
        } else {
            this->actor.world.rot.z = globalCtx->csCtx.npcActions[7]->urot.y;
        }
    }

    if (this->actionFlags & 0x80) {
        func_80ACB748(this, globalCtx);
    }
}

void func_80ACBAB8(EnOwl* this, GlobalContext* globalCtx) {
    switch (globalCtx->csCtx.npcActions[7]->action - 1) {
        case 0:
            EnOwl_ChangeMode(this, func_80ACB904, func_80ACC540, &this->skelAnime, &gOwlFlyAnim, 0.0f);
            break;
        case 1:
            this->actor.draw = EnOwl_Draw;
            EnOwl_ChangeMode(this, EnOwl_WaitDefault, func_80ACC540, &this->skelAnime, &gOwlPerchAnim, 0.0f);
            break;
        case 2:
            this->actor.draw = EnOwl_Draw;
            EnOwl_ChangeMode(this, func_80ACB994, func_80ACC540, &this->skelAnime, &gOwlFlyAnim, 0.0f);
            break;
        case 3:
            this->actor.draw = NULL;
            this->actionFunc = EnOwl_WaitDefault;
            break;
        case 4:
            Actor_Kill(&this->actor);
            break;
    }

    this->unk_40A = globalCtx->csCtx.npcActions[7]->action;
}

void func_80ACBC0C(EnOwl* this, GlobalContext* globalCtx) {
    this->actor.flags |= ACTOR_FLAG_5;

    if (this->actor.xzDistToPlayer > 6000.0f && !(this->actionFlags & 0x80)) {
        Actor_Kill(&this->actor);
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_400, 2, 0x80, 0x40);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->actor.speedXZ < 16.0f) {
        this->actor.speedXZ += 0.5f;
    }

    if ((this->unk_3F8 + 1000.0f) < this->actor.world.pos.y) {
        if (this->actor.velocity.y > 0.0f) {
            this->actor.velocity.y -= 0.4f;
        }
    } else if (this->actor.velocity.y < 4.0f) {
        this->actor.velocity.y += 0.2f;
    }

    this->actionFlags |= 8;
}

void func_80ACBD4C(EnOwl* this, GlobalContext* globalCtx) {
    if (this->skelAnime.curFrame > 10.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_400, 2, 0x400, 0x40);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if (this->skelAnime.curFrame > 45.0f) {
        this->actor.velocity.y = 2.0f;
        this->actor.gravity = 0.0f;
        this->actor.speedXZ = 8.0f;
    } else if (this->skelAnime.curFrame > 17.0f) {
        this->actor.velocity.y = 6.0f;
        this->actor.gravity = 0.0f;
        this->actor.speedXZ = 4.0f;
    }

    if (this->actionFlags & 1) {
        EnOwl_ChangeMode(this, func_80ACBC0C, func_80ACC460, &this->skelAnime, &gOwlFlyAnim, 0.0f);
        this->unk_3FE = 6;
        if (this->actionFlags & 0x40) {
            this->unk_400 += 0x2000;
        } else {
            this->unk_400 -= 0x2000;
        }
    }

    this->actionFlags |= 8;
}

void func_80ACBEA0(EnOwl* this, GlobalContext* GlobalContext) {
    if (this->actionFlags & 1) {
        this->unk_3FE = 3;
        EnOwl_ChangeMode(this, func_80ACBD4C, func_80ACC540, &this->skelAnime, &gOwlTakeoffAnim, 0.0f);
        this->unk_3F8 = this->actor.world.pos.y;
        this->actor.velocity.y = 2.0f;
        if (this->actionFlags & 0x40) {
            this->unk_400 = this->actor.world.rot.y + 0x4000;
        } else {
            this->unk_400 = this->actor.world.rot.y - 0x4000;
        }
    }

    this->actionFlags |= 8;
}

void func_80ACBF50(EnOwl* this, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_400, 2, 0x384, 0x258);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->actionFlags & 1) {
        EnOwl_ChangeMode(this, func_80ACBC0C, func_80ACC460, &this->skelAnime, &gOwlFlyAnim, 0.0f);
        this->unk_3FE = 6;
        this->actor.velocity.y = 2.0f;
        this->actor.gravity = 0.0f;
        this->actor.speedXZ = 4.0f;
    }
    this->actionFlags |= 8;
}

void func_80ACC00C(EnOwl* this, GlobalContext* globalCtx) {
    s32 owlType;
    s32 temp_v0;
    s32 temp_v0_2;

    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_400, 2, 0x384, 0x258);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->actor.xzDistToPlayer < 50.0f) {
        if (!Gameplay_InCsMode(globalCtx)) {
            owlType = (this->actor.params & 0xFC0) >> 6;
            osSyncPrintf(VT_FGCOL(CYAN));
            osSyncPrintf("%dのフクロウ\n", owlType); // "%d owl"
            osSyncPrintf(VT_RST);
            switch (owlType) {
                case 7:
                    osSyncPrintf(VT_FGCOL(CYAN));
                    osSyncPrintf("SPOT 06 の デモがはしった\n"); // "Demo of SPOT 06 has been completed"
                    osSyncPrintf(VT_RST);
                    globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gLakeHyliaOwlCs);
                    this->actor.draw = NULL;
                    break;
                case 8:
                case 9:
                    globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gDMTOwlCs);
                    this->actor.draw = NULL;
                    break;
                default:
                    ASSERT(0, "0", "../z_en_owl.c", 1693);
                    break;
            }

            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            gSaveContext.cutsceneTrigger = 1;
            func_800F44EC(0x14, 0xA);
            this->actionFunc = EnOwl_WaitDefault;
            this->unk_40A = 0;
            this->actionFlags |= 0x80;
            gTimeIncrement = 0;
        }
    }

    if (this->skelAnime.curFrame >= 37.0f) {
        if (this->unk_3FE > 0) {
            this->skelAnime.curFrame = 21.0f;
            this->unk_3FE--;
        } else {
            this->actionFunc = func_80ACBF50;
        }
    }

    this->actionFlags |= 8;
}

void func_80ACC23C(EnOwl* this, GlobalContext* globalCtx) {
    if (this->skelAnime.curFrame < 20.0f) {
        this->actor.speedXZ = 1.5f;
    } else {
        this->actor.speedXZ = 0.0f;
        Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_400, 2, 0x384, 0x258);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if (this->skelAnime.curFrame >= 37.0f) {
        this->skelAnime.curFrame = 21.0f;
        this->actionFunc = func_80ACC00C;
        this->unk_3FE = 5;
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
        this->actor.speedXZ = 0.0f;
    }

    this->actionFlags |= 8;
}

void func_80ACC30C(EnOwl* this, GlobalContext* globalCtx) {
    if (this->actionFlags & 1) {
        this->unk_3FE = 3;
        EnOwl_ChangeMode(this, func_80ACC23C, func_80ACC540, &this->skelAnime, &gOwlTakeoffAnim, 0.0f);
        this->unk_3F8 = this->actor.world.pos.y;
        this->actor.velocity.y = 0.2f;
    }

    this->actionFlags |= 8;
}

void func_80ACC390(EnOwl* this) {
    SkelAnime_Update(this->curSkelAnime);

    if (this->unk_3FE > 0) {
        this->unk_3FE--;
        this->actor.shape.rot.z = Math_SinS(this->unk_3FE * 0x333) * 1000.0f;
    } else {
        this->unk_410 = func_80ACC460;
        this->unk_3FE = 6;
        Animation_Change(this->curSkelAnime, &gOwlFlyAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gOwlFlyAnim), 2, 5.0f);
    }
}

void func_80ACC460(EnOwl* this) {
    if (SkelAnime_Update(this->curSkelAnime)) {
        if (this->unk_3FE > 0) {
            this->unk_3FE--;
            Animation_Change(this->curSkelAnime, this->curSkelAnime->animation, 1.0f, 0.0f,
                             Animation_GetLastFrame(this->curSkelAnime->animation), ANIMMODE_ONCE, 0.0f);
        } else {
            this->unk_3FE = 0xA0;
            this->unk_410 = func_80ACC390;
            Animation_Change(this->curSkelAnime, &gOwlGlideAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gOwlGlideAnim), 0,
                             5.0f);
        }
    }
}

void func_80ACC540(EnOwl* this) {
    if (SkelAnime_Update(this->curSkelAnime)) {
        Animation_Change(this->curSkelAnime, this->curSkelAnime->animation, 1.0f, 0.0f,
                         Animation_GetLastFrame(this->curSkelAnime->animation), ANIMMODE_ONCE, 0.0f);
        this->actionFlags |= 1;
    } else {
        this->actionFlags &= ~1;
    }
}

s32 func_80ACC5CC(EnOwl* this) {
    s32 phi_v1 = (this->actionFlags & 2) ? 0x20 : 0;

    if (phi_v1 == (this->unk_3EE & 0x3F)) {
        return true;
    } else {
        if (this->actionFlags & 0x20) {
            this->unk_3EE += 4;
        } else {
            this->unk_3EE -= 4;
        }

        return false;
    }
}

s32 func_80ACC624(EnOwl* this, GlobalContext* globalCtx) {
    s32 switchFlag = (this->actor.params & 0xFC0) >> 6;

    if (globalCtx->sceneNum != SCENE_SPOT11) {
        return true;
    } else if (switchFlag == 0xA) {
        return true;
    } else if (globalCtx->csCtx.frames >= 300 && globalCtx->csCtx.frames <= 430) {
        return true;
    } else if (globalCtx->csCtx.frames >= 1080 && globalCtx->csCtx.frames <= 1170) {
        return true;
    } else {
        return false;
    }
}

void EnOwl_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnOwl* this = (EnOwl*)thisx;
    s16 phi_a1;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 10.0f, 10.0f, 10.0f, 5);
    this->unk_410(this);
    this->actionFlags &= ~8;
    this->actionFunc(this, globalCtx);
    if (this->actor.update == NULL) {
        // "Owl disappears"
        osSyncPrintf("フクロウ消滅!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        return;
    }

    if (!(this->actionFlags & 0x80) && func_80ACC624(this, globalCtx)) {
        if ((this->skelAnime.animation == &gOwlTakeoffAnim &&
             (this->skelAnime.curFrame == 2.0f || this->skelAnime.curFrame == 9.0f ||
              this->skelAnime.curFrame == 23.0f || this->skelAnime.curFrame == 40.0f ||
              this->skelAnime.curFrame == 58.0f)) ||
            (this->skelAnime.animation == &gOwlFlyAnim && this->skelAnime.curFrame == 4.0f)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_OWL_FLUTTER);
        }
    }

    if (this->actor.draw != NULL) {
        Actor_MoveForward(&this->actor);
    }

    if (this->actionFlags & 2) {
        this->eyeTexIndex = 2;
    } else {
        if (DECR(this->blinkTimer) == 0) {
            this->blinkTimer = Rand_S16Offset(60, 60);
        }

        this->eyeTexIndex = this->blinkTimer;

        if (this->eyeTexIndex >= 3) {
            this->eyeTexIndex = 0;
        }
    }

    if (!(this->actionFlags & 8)) {
        phi_a1 = 0;
        if (this->actionFlags & 0x10) {
            switch (this->unk_404) {
                case 0:
                    this->unk_404 = 1;
                    this->unk_405 = 6;
                    break;
                case 1:
                    this->unk_405--;

                    if (this->unk_405 != 0) {
                        phi_a1 = Math_CosS(this->unk_405 * 8192) * 4096.0f;
                    } else {
                        if (this->actionFlags & 2) {
                            this->unk_3EE = 0;
                        } else {
                            this->unk_3EE = 0x20;
                        }

                        if (this->actionFlags & 0x20) {
                            this->unk_3EE -= 4;
                        } else {
                            this->unk_3EE += 4;
                        }
                        this->unk_404++;
                    }

                    if (this->actionFlags & 0x20) {
                        phi_a1 = -phi_a1;
                    }
                    break;
                case 2:
                    if (func_80ACC5CC(this)) {
                        this->actionFlags &= ~0x10;
                        this->unk_406 = (s32)Rand_ZeroFloat(20.0f) + 0x3C;
                        this->unk_404 = 0;
                        func_80ACA6C0(this);
                    }
                    break;
                default:
                    break;
            }
        } else {
            if (this->unk_406 > 0) {
                this->unk_406--;
            } else {
                if (this->unk_404 == 0) {
                    if (Rand_ZeroOne() < 0.3f) {
                        this->unk_404 = 4;
                        this->unk_405 = 0xC;
                    } else {
                        this->unk_404 = 1;
                        this->unk_405 = 4;
                    }
                }

                this->unk_405--;

                switch (this->unk_404) {
                    case 1:
                        phi_a1 = Math_SinS((-this->unk_405 * 4096) + 0x4000) * 5000.0f;
                        if (this->unk_405 <= 0) {
                            this->unk_405 = (s32)(Rand_ZeroFloat(15.0f) + 5.0f);
                            this->unk_404 = 2;
                        }
                        break;
                    case 2:
                        phi_a1 = 0x1388;
                        if (this->unk_405 <= 0) {
                            this->unk_404 = 3;
                            this->unk_405 = 4;
                        }
                        break;
                    case 3:
                        phi_a1 = Math_SinS(this->unk_405 * 4096) * 5000.0f;
                        if (this->unk_405 <= 0) {
                            this->unk_406 = (s32)Rand_ZeroFloat(20.0f) + 0x3C;
                            this->unk_404 = 0;
                            func_80ACA6C0(this);
                        }
                        break;
                    case 4:
                        phi_a1 = Math_SinS(this->unk_405 * 8192) * 5000.0f;
                        if (this->unk_405 <= 0) {
                            this->unk_406 = (s32)Rand_ZeroFloat(20.0f) + 0x3C;
                            this->unk_404 = 0;
                            func_80ACA6C0(this);
                        }
                        break;
                    default:
                        break;
                }

                if (this->actionFlags & 0x20) {
                    phi_a1 = -phi_a1;
                }
            }

            if (this->unk_409 > 0) {
                this->unk_409--;
            } else {
                this->unk_408--;
                switch (this->unk_407) {
                    case 0:
                        this->unk_3F2 = (-this->unk_408 * 0x5DC) + 0x1770;
                        if (this->unk_408 <= 0) {
                            this->unk_407 = 1;
                            this->unk_408 = (s32)(Rand_ZeroFloat(15.0f) + 5.0f);
                        }
                        break;
                    case 1:
                        this->unk_3F2 = 0x1770;
                        if (this->unk_408 <= 0) {
                            this->unk_407 = 2;
                            this->unk_408 = 4;
                        }
                        break;
                    case 2:
                        this->unk_3F2 = this->unk_408 * 0x5DC;
                        if (this->unk_408 <= 0) {
                            this->unk_407 = 0;
                            this->unk_408 = 4;
                            this->unk_409 = (s32)Rand_ZeroFloat(40.0f) + 0xA0;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        if (phi_a1) {}
        this->unk_3F0 = (u16)((this->unk_3EE << 2) << 8) + phi_a1;
        this->unk_3EC = ABS(this->unk_3F0) >> 3;
    } else {
        this->unk_3F2 = 0;
        if (this->actionFlags & 2) {
            this->unk_3F0 = -0x8000;
        } else {
            this->unk_3F0 = 0;
        }

        this->unk_3EC = ABS(this->unk_3F0) >> 3;
    }
}

s32 EnOwl_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** gfx, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnOwl* this = (EnOwl*)thisx;

    switch (limbIndex) {
        case 3:
            rot->x += this->unk_3F0;
            rot->z += this->unk_3EC;
            rot->z -= this->unk_3F2;
            break;
        case 2:
            rot->z += this->unk_3F2;
            break;
        case 4:
            if (!(this->actionFlags & 8)) {
                rot->y -= (s16)(this->unk_3EC * 1.5f);
            }
            break;
        case 5:
            if (!(this->actionFlags & 8)) {
                rot->y += (s16)(this->unk_3EC * 1.5f);
            }
            break;
        default:
            break;
    }
    return false;
}

void EnOwl_PostLimbUpdate(GlobalContext* globalCtx, s32 limbIndex, Gfx** gfx, Vec3s* rot, void* thisx) {
    EnOwl* this = (EnOwl*)thisx;
    Vec3f vec;

    vec.z = 0.0f;
    if (this->actionFlags & 2) {
        vec.x = 700.0f;
        vec.y = 400.0f;
    } else {
        vec.y = 0.0f;
        vec.x = 1400.0f;
    }
    if (limbIndex == 3) {
        Matrix_MultVec3f(&vec, &this->actor.focus.pos);
    }
}

void EnOwl_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* eyeTextures[] = { gObjOwlEyeOpenTex, gObjOwlEyeHalfTex, gObjOwlEyeClosedTex };
    EnOwl* this = (EnOwl*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_owl.c", 2247);

    func_800943C8(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 8, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawFlexOpa(globalCtx, this->curSkelAnime->skeleton, this->curSkelAnime->jointTable,
                          this->curSkelAnime->dListCount, EnOwl_OverrideLimbDraw, EnOwl_PostLimbUpdate, this);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_owl.c", 2264);
}

void EnOwl_ChangeMode(EnOwl* this, EnOwlActionFunc actionFunc, OwlFunc arg2, SkelAnime* skelAnime,
                      AnimationHeader* animation, f32 morphFrames) {
    this->curSkelAnime = skelAnime;
    Animation_Change(this->curSkelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE,
                     morphFrames);
    this->actionFunc = actionFunc;
    this->unk_410 = arg2;
}

void func_80ACD130(EnOwl* this, GlobalContext* globalCtx, s32 idx) {
    Vec3f startPos;

    startPos.x = globalCtx->csCtx.npcActions[idx]->startPos.x;
    startPos.y = globalCtx->csCtx.npcActions[idx]->startPos.y;
    startPos.z = globalCtx->csCtx.npcActions[idx]->startPos.z;
    this->actor.world.pos = startPos;
    this->actor.world.rot.y = this->actor.shape.rot.y = globalCtx->csCtx.npcActions[idx]->rot.y;
    this->actor.shape.rot.z = globalCtx->csCtx.npcActions[idx]->urot.z;
}

f32 func_80ACD1C4(GlobalContext* globalCtx, s32 idx) {
    f32 ret = Environment_LerpWeight(globalCtx->csCtx.npcActions[idx]->endFrame,
                                     globalCtx->csCtx.npcActions[idx]->startFrame, globalCtx->csCtx.frames);

    ret = CLAMP_MAX(ret, 1.0f);
    return ret;
}

void func_80ACD220(EnOwl* this, Vec3f* arg1, f32 arg2) {
    Vec3f rpy;

    rpy.x = (arg1->x - this->actor.world.pos.x) * arg2;
    rpy.y = (arg1->y - this->actor.world.pos.y) * arg2;
    rpy.z = (arg1->z - this->actor.world.pos.z) * arg2;

    Math_StepToF(&this->actor.velocity.y, rpy.y, 1.0f);
    this->actor.speedXZ = sqrtf(SQ(rpy.x) + SQ(rpy.z));
    this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, arg1);
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void func_80ACD2CC(EnOwl* this, GlobalContext* globalCtx) {
    Vec3f pos;
    s32 angle;
    f32 t = func_80ACD1C4(globalCtx, 7);

    pos.x = globalCtx->csCtx.npcActions[7]->startPos.x;
    pos.y = globalCtx->csCtx.npcActions[7]->startPos.y;
    pos.z = globalCtx->csCtx.npcActions[7]->startPos.z;
    angle = (s16)globalCtx->csCtx.npcActions[7]->rot.y - this->actor.world.rot.z;
    if (angle < 0) {
        angle += 0x10000;
    }
    angle = (s16)((t * angle) + this->actor.world.rot.z);
    angle = (u16)angle;
    if (this->actionFlags & 4) {
        f32 phi_f2 = globalCtx->csCtx.npcActions[7]->urot.x;

        phi_f2 *= 10.0f * (360.0f / 0x10000);
        if (phi_f2 < 0.0f) {
            phi_f2 += 360.0f;
        }
        pos.x -= Math_SinS(angle) * phi_f2;
        pos.z += Math_CosS(angle) * phi_f2;
        this->unk_3F8 = phi_f2;
        this->actor.world.pos = pos;
        this->actor.draw = EnOwl_Draw;
        this->actionFlags &= ~4;
        this->actor.speedXZ = 0.0f;
    } else {
        pos.x -= Math_SinS(angle) * this->unk_3F8;
        pos.z += Math_CosS(angle) * this->unk_3F8;
        func_80ACD220(this, &pos, 1.0f);
    }
}

void func_80ACD4D4(EnOwl* this, GlobalContext* globalCtx) {
    Vec3f pos;
    Vec3f endPosf;
    f32 temp_ret = func_80ACD1C4(globalCtx, 7);

    pos.x = globalCtx->csCtx.npcActions[7]->startPos.x;
    pos.y = globalCtx->csCtx.npcActions[7]->startPos.y;
    pos.z = globalCtx->csCtx.npcActions[7]->startPos.z;
    endPosf.x = globalCtx->csCtx.npcActions[7]->endPos.x;
    endPosf.y = globalCtx->csCtx.npcActions[7]->endPos.y;
    endPosf.z = globalCtx->csCtx.npcActions[7]->endPos.z;
    pos.x = (endPosf.x - pos.x) * temp_ret + pos.x;
    pos.y = (endPosf.y - pos.y) * temp_ret + pos.y;
    pos.z = (endPosf.z - pos.z) * temp_ret + pos.z;
    func_80ACD220(this, &pos, 1.0f);
}
