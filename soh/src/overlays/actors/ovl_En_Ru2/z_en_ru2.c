/*
 * File: z_en_ru2.c
 * Overlay: En_Ru2
 * Description: Adult Ruto
 */

#include "z_en_ru2.h"
#include "objects/object_ru2/object_ru2.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void EnRu2_Init(Actor* thisx, GlobalContext* globalCtx);
void EnRu2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnRu2_Update(Actor* thisx, GlobalContext* globalCtx);
void EnRu2_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80AF2CB4(EnRu2* this, GlobalContext* globalCtx);
void func_80AF2CD4(EnRu2* this, GlobalContext* globalCtx);
void func_80AF2CF4(EnRu2* this, GlobalContext* globalCtx);
void func_80AF2D2C(EnRu2* this, GlobalContext* globalCtx);
void func_80AF2D6C(EnRu2* this, GlobalContext* globalCtx);
void func_80AF2DAC(EnRu2* this, GlobalContext* globalCtx);
void func_80AF2DEC(EnRu2* this, GlobalContext* globalCtx);
void func_80AF3144(EnRu2* this, GlobalContext* globalCtx);
void func_80AF3174(EnRu2* this, GlobalContext* globalCtx);
void func_80AF31C8(EnRu2* this, GlobalContext* globalCtx);
void func_80AF3604(EnRu2* this, GlobalContext* globalCtx);
void func_80AF3624(EnRu2* this, GlobalContext* globalCtx);
void func_80AF366C(EnRu2* this, GlobalContext* globalCtx);
void func_80AF36AC(EnRu2* this, GlobalContext* globalCtx);
void func_80AF3BC8(EnRu2* this, GlobalContext* globalCtx);
void func_80AF3C04(EnRu2* this, GlobalContext* globalCtx);
void func_80AF3C64(EnRu2* this, GlobalContext* globalCtx);
void func_80AF3CB8(EnRu2* this, GlobalContext* globalCtx);
void func_80AF3D0C(EnRu2* this, GlobalContext* globalCtx);
void func_80AF3D60(EnRu2* this, GlobalContext* globalCtx);

void func_80AF3F14(EnRu2* this, GlobalContext* globalCtx);
void func_80AF3F20(EnRu2* this, GlobalContext* globalCtx);
void func_80AF321C(EnRu2* this, GlobalContext* globalCtx);

void func_80AF2AB4(EnRu2* this, GlobalContext* globalCtx);

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0x00000080, 0x00, 0x00 }, 0x00, 0x01, 0x00 },
    { 30, 100, 0, { 0 } },
};

static void* sEyeTextures[] = {
    gAdultRutoEyeOpenTex,
    gAdultRutoEyeHalfTex,
    gAdultRutoEyeClosedTex,
};

static UNK_TYPE D_80AF4118 = 0;

#include "z_en_ru2_cutscene_data.c" EARLY

static EnRu2ActionFunc sActionFuncs[] = {
    func_80AF2CB4, func_80AF2CD4, func_80AF2CF4, func_80AF2D2C, func_80AF2D6C, func_80AF2DAC, func_80AF2DEC,
    func_80AF3144, func_80AF3174, func_80AF31C8, func_80AF3604, func_80AF3624, func_80AF366C, func_80AF36AC,
    func_80AF3BC8, func_80AF3C04, func_80AF3C64, func_80AF3CB8, func_80AF3D0C, func_80AF3D60,
};

static EnRu2DrawFunc sDrawFuncs[] = {
    func_80AF3F14,
    func_80AF3F20,
    func_80AF321C,
};

const ActorInit En_Ru2_InitVars = {
    ACTOR_EN_RU2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_RU2,
    sizeof(EnRu2),
    (ActorFunc)EnRu2_Init,
    (ActorFunc)EnRu2_Destroy,
    (ActorFunc)EnRu2_Update,
    (ActorFunc)EnRu2_Draw,
    NULL,
};

void func_80AF2550(Actor* thisx, GlobalContext* globalCtx) {
    EnRu2* this = (EnRu2*)thisx;

    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinderType1(globalCtx, &this->collider, &this->actor, &sCylinderInit);
}

void func_80AF259C(EnRu2* this, GlobalContext* globalCtx) {
    s32 pad[5];

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
}

void EnRu2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnRu2* this = (EnRu2*)thisx;
    D_80AF4118 = 0;
    Collider_DestroyCylinder(globalCtx, &this->collider);
}

void func_80AF2608(EnRu2* this) {
    s32 pad[3];
    s16* unk_2A6 = &this->unk_2A6;
    s16* unk_2A4 = &this->unk_2A4;

    if (!DECR(*unk_2A6)) {
        *unk_2A6 = Rand_S16Offset(0x3C, 0x3C);
    }

    *unk_2A4 = *unk_2A6;
    if (*unk_2A4 >= 3) {
        *unk_2A4 = 0;
    }
}

s32 func_80AF2690(EnRu2* this) {
    s32 params_shift = this->actor.params >> 8;

    return params_shift & 0xFF;
}

s32 func_80AF26A0(EnRu2* this) {
    s16 params = this->actor.params;

    return params & 0xFF;
}

void func_80AF26AC(EnRu2* this) {
    this->action = 7;
    this->drawConfig = 0;
    this->alpha = 0;
    this->unk_2B8 = 0;
    this->actor.shape.shadowAlpha = 0;
    this->unk_2B0 = 0.0f;
}

void func_80AF26D0(EnRu2* this, GlobalContext* globalCtx) {
    s32 one; // Needed to match

    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        if (D_80AF4118 != 0) {
            if (this->actor.params == 2) {
                func_80AF26AC(this);
            }
            D_80AF4118 = 0;
            return;
        }
    } else {
        one = 1;
        if (D_80AF4118 == 0) {
            D_80AF4118 = one;
        }
    }
}

void func_80AF2744(EnRu2* this, GlobalContext* globalCtx) {
    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 75.0f, 30.0f, 30.0f, 4);
}

s32 EnRu2_UpdateSkelAnime(EnRu2* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorAction* func_80AF27AC(GlobalContext* globalCtx, s32 npcActionIdx) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        return globalCtx->csCtx.npcActions[npcActionIdx];
    }
    return NULL;
}

s32 func_80AF27D0(EnRu2* this, GlobalContext* globalCtx, u16 arg2, s32 npcActionIdx) {
    CsCmdActorAction* csCmdActorAction = func_80AF27AC(globalCtx, npcActionIdx);

    if ((csCmdActorAction != NULL) && (csCmdActorAction->action == arg2)) {
        return true;
    }
    return false;
}

s32 func_80AF281C(EnRu2* this, GlobalContext* globalCtx, u16 arg2, s32 npcActionIdx) {
    CsCmdActorAction* csCmdNPCAction = func_80AF27AC(globalCtx, npcActionIdx);

    if ((csCmdNPCAction != NULL) && (csCmdNPCAction->action != arg2)) {
        return true;
    }
    return false;
}

void func_80AF2868(EnRu2* this, GlobalContext* globalCtx, u32 npcActionIdx) {
    CsCmdActorAction* csCmdNPCAction = func_80AF27AC(globalCtx, npcActionIdx);
    s16 newRotY;
    Actor* thisx = &this->actor;

    if (csCmdNPCAction != NULL) {
        thisx->world.pos.x = csCmdNPCAction->startPos.x;
        thisx->world.pos.y = csCmdNPCAction->startPos.y;
        thisx->world.pos.z = csCmdNPCAction->startPos.z;
        newRotY = csCmdNPCAction->rot.y;
        thisx->shape.rot.y = newRotY;
        thisx->world.rot.y = newRotY;
    }
}

void func_80AF28E8(EnRu2* this, AnimationHeader* animation, u8 arg2, f32 transitionRate, s32 arg4) {
    f32 frameCount = Animation_GetLastFrame(animation);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (arg4 == 0) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        unk0 = frameCount;
        fc = 0.0f;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&this->skelAnime, animation, playbackSpeed, unk0, fc, arg2, transitionRate);
}

void func_80AF2978(EnRu2* this, GlobalContext* globalCtx) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

void func_80AF2994(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF28E8(this, &gAdultRutoIdleAnim, 0, 0.0f, 0);
    this->actor.shape.yOffset = -10000.0f;
}

void func_80AF29DC(EnRu2* this, GlobalContext* globalCtx) {
    Actor* thisx = &this->actor;
    f32 posX = thisx->world.pos.x;
    f32 posY = thisx->world.pos.y;
    f32 posZ = thisx->world.pos.z;

    Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0,
                       WARP_SAGES);
}

void func_80AF2A38(EnRu2* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 50.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0, 10);
    Item_Give(globalCtx, ITEM_MEDALLION_WATER);
}

void func_80AF2AB4(EnRu2* this, GlobalContext* globalCtx) {
    s32 pad[2];
    Player* player;
    s16 temp;

    if ((gSaveContext.chamberCutsceneNum == 2) && (gSaveContext.sceneSetupIndex < 4)) {
        player = GET_PLAYER(globalCtx);
        this->action = 1;
        globalCtx->csCtx.segment = &D_80AF411C;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(globalCtx, ITEM_MEDALLION_WATER);
        temp = this->actor.world.rot.y + 0x8000;
        player->actor.shape.rot.y = temp;
        player->actor.world.rot.y = temp;
    }
}

void func_80AF2B44(EnRu2* this, GlobalContext* globalCtx) {
    CutsceneContext* csCtx = &globalCtx->csCtx;
    CsCmdActorAction* csCmdNPCAction;

    if (csCtx->state != CS_STATE_IDLE) {
        csCmdNPCAction = csCtx->npcActions[3];
        if ((csCmdNPCAction != NULL) && (csCmdNPCAction->action == 2)) {
            this->action = 2;
            this->drawConfig = 1;
            func_80AF29DC(this, globalCtx);
        }
    }
}

void func_80AF2B94(EnRu2* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = 3;
        this->actor.shape.yOffset = 0.0f;
    }
}

void func_80AF2BC0(EnRu2* this, GlobalContext* globalCtx) {
    AnimationHeader* animation = &gAdultRutoRaisingArmsUpAnim;
    CsCmdActorAction* csCmdNPCAction;

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        csCmdNPCAction = globalCtx->csCtx.npcActions[3];
        if ((csCmdNPCAction != NULL) && (csCmdNPCAction->action == 3)) {
            Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE,
                             0.0f);
            this->action = 4;
        }
    }
}

void func_80AF2C54(EnRu2* this, s32 arg1) {
    if (arg1 != 0) {
        this->action = 5;
    }
}

void func_80AF2C68(EnRu2* this, GlobalContext* globalCtx) {
    CsCmdActorAction* csCmdNPCAction;

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        csCmdNPCAction = globalCtx->csCtx.npcActions[6];
        if ((csCmdNPCAction != NULL) && (csCmdNPCAction->action == 2)) {
            this->action = 6;
            func_80AF2A38(this, globalCtx);
        }
    }
}

void func_80AF2CB4(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2AB4(this, globalCtx);
}

void func_80AF2CD4(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2B44(this, globalCtx);
}

void func_80AF2CF4(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2978(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    func_80AF2B94(this);
}

void func_80AF2D2C(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2744(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    func_80AF2BC0(this, globalCtx);
}

void func_80AF2D6C(EnRu2* this, GlobalContext* globalCtx) {
    s32 something;

    func_80AF2744(this, globalCtx);
    something = EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    func_80AF2C54(this, something);
}

void func_80AF2DAC(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2744(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    func_80AF2C68(this, globalCtx);
}

void func_80AF2DEC(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2744(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
}

void func_80AF2E1C(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF28E8(this, &gAdultRutoCrossingArmsAnim, 2, 0.0f, 0);
    this->action = 7;
    this->actor.shape.shadowAlpha = 0;
}

void func_80AF2E64() {
    func_800788CC(NA_SE_SY_WHITE_OUT_T);
}

void func_80AF2E84(EnRu2* this, GlobalContext* globalCtx) {
    Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(19) + 24.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 8);
}

void func_80AF2F04(EnRu2* this, GlobalContext* globalCtx) {
    if (func_80AF27D0(this, globalCtx, 4, 3)) {
        this->action = 8;
        this->drawConfig = 2;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_2B0 = 0.0f;
        func_80AF2E64();
    }
}

void func_80AF2F58(EnRu2* this, GlobalContext* globalCtx) {
    f32* unk_2B0 = &this->unk_2B0;
    s32 alpha;

    if (func_80AF27D0(this, globalCtx, 4, 3)) {
        *unk_2B0 += 1.0f;
        if (*unk_2B0 >= kREG(5) + 10.0f) {
            this->action = 9;
            this->drawConfig = 1;
            *unk_2B0 = kREG(5) + 10.0f;
            this->alpha = 255;
            this->actor.shape.shadowAlpha = 0xFF;
            return;
        }
    } else {
        *unk_2B0 -= 1.0f;
        if (*unk_2B0 <= 0.0f) {
            this->action = 7;
            this->drawConfig = 0;
            *unk_2B0 = 0.0f;
            this->alpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    alpha = (*unk_2B0 / (kREG(5) + 10.0f)) * 255.0f;
    this->alpha = alpha;
    this->actor.shape.shadowAlpha = alpha;
}

void func_80AF30AC(EnRu2* this, GlobalContext* globalCtx) {
    if (func_80AF281C(this, globalCtx, 4, 3)) {
        this->action = 8;
        this->drawConfig = 2;
        this->unk_2B0 = kREG(5) + 10.0f;
        this->alpha = 255;
        if (this->unk_2B8 == 0) {
            func_80AF2E84(this, globalCtx);
            this->unk_2B8 = 1;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80AF3144(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2F04(this, globalCtx);
    func_80AF26D0(this, globalCtx);
}

void func_80AF3174(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2744(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    func_80AF2F58(this, globalCtx);
    func_80AF26D0(this, globalCtx);
}

void func_80AF31C8(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2744(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    func_80AF30AC(this, globalCtx);
    func_80AF26D0(this, globalCtx);
}

void func_80AF321C(EnRu2* this, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 temp = this->unk_2A4;
    void* tex = sEyeTextures[temp];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       NULL, NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void func_80AF3394(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF28E8(this, &gAdultRutoIdleHandsOnHipsAnim, 0, 0.0f, 0);
    this->action = 10;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_80AF33E0(EnRu2* this) {
    f32* unk_2B0 = &this->unk_2B0;
    f32 temp_f0;
    s32 temp_f18;

    *unk_2B0 += 1.0f;

    temp_f0 = kREG(17) + 10.0f;
    if (temp_f0 <= *unk_2B0) {
        this->alpha = 255;
        this->actor.shape.shadowAlpha = 0xFF;
    } else {
        temp_f18 = (*unk_2B0 / temp_f0) * 255.0f;
        this->alpha = temp_f18;
        this->actor.shape.shadowAlpha = temp_f18;
    }
}

void func_80AF346C(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2868(this, globalCtx, 3);
    this->action = 11;
    this->drawConfig = 2;
}

void func_80AF34A4(EnRu2* this) {
    if (this->unk_2B0 >= kREG(17) + 10.0f) {
        this->action = 12;
        this->drawConfig = 1;
    }
}

void func_80AF34F0(EnRu2* this) {
    func_80AF28E8(this, &gAdultRutoHeadTurnDownLeftAnim, 2, 0.0f, 0);
    this->action = 13;
}

void func_80AF3530(EnRu2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80AF28E8(this, &gAdultRutoLookingDownLeftAnim, 0, 0.0f, 0);
    }
}

void func_80AF3564(EnRu2* this, GlobalContext* globalCtx) {
    CsCmdActorAction* csCmdNPCAction = func_80AF27AC(globalCtx, 3);
    s32 action;
    s32 unk_2BC;

    if (csCmdNPCAction != NULL) {
        action = csCmdNPCAction->action;
        unk_2BC = this->unk_2BC;
        if (action != unk_2BC) {
            switch (action) {
                case 7:
                    func_80AF346C(this, globalCtx);
                    break;
                case 8:
                    func_80AF34F0(this);
                    break;
                default:
                    // "There is no such action!"
                    osSyncPrintf("En_Ru2_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->unk_2BC = action;
        }
    }
}

void func_80AF3604(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF3564(this, globalCtx);
}

void func_80AF3624(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2744(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    func_80AF33E0(this);
    func_80AF34A4(this);
}

void func_80AF366C(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2744(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    func_80AF3564(this, globalCtx);
}

void func_80AF36AC(EnRu2* this, GlobalContext* globalCtx) {
    s32 something;

    func_80AF2744(this, globalCtx);
    something = EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    func_80AF3530(this, something);
}

void func_80AF36EC(EnRu2* this, GlobalContext* globalCtx) {
    Flags_SetSwitch(globalCtx, func_80AF2690(this));
}

s32 func_80AF3718(EnRu2* this, GlobalContext* globalCtx) {
    return Flags_GetSwitch(globalCtx, func_80AF2690(this));
}

void func_80AF3744(EnRu2* this, GlobalContext* globalCtx) {
    if (func_80AF3718(this, globalCtx)) {
        Actor_Kill(&this->actor);
    } else {
        func_80AF28E8(this, &gAdultRutoIdleAnim, 0, 0.0f, 0);
        this->action = 14;
        this->drawConfig = 1;
    }
}

void func_80AF37AC(void) {
    Audio_PlayFanfare(NA_BGM_APPEAR);
}

void func_80AF37CC(EnRu2* this) {
    f32 funcFloat;

    this->unk_2C0++;
    funcFloat = Environment_LerpWeightAccelDecel((kREG(2) + 0x96) & 0xFFFF, 0, this->unk_2C0, 8, 0);
    this->actor.world.pos.y = this->actor.home.pos.y + (300.0f * funcFloat);
}

s32 func_80AF383C(EnRu2* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 thisPosX = this->actor.world.pos.x;
    f32 playerPosX = player->actor.world.pos.x;

    if (playerPosX - thisPosX >= -202.0f) {
        return 1;
    }
    return 0;
}

void func_80AF3878(EnRu2* this, GlobalContext* globalCtx) {
    if (func_80AF383C(this, globalCtx) && !Gameplay_InCsMode(globalCtx)) {
        this->action = 16;
        this->subCamId = OnePointCutscene_Init(globalCtx, 3130, -99, &this->actor, MAIN_CAM);
    }
}

void func_80AF38D0(EnRu2* this, GlobalContext* globalCtx) {
    this->action = 16;
    this->subCamId = OnePointCutscene_Init(globalCtx, 3130, -99, &this->actor, MAIN_CAM);
}

void func_80AF390C(EnRu2* this, GlobalContext* globalCtx) {
    f32* unk_2C4 = &this->unk_2C4;

    *unk_2C4 += 1.0f;
    if (*unk_2C4 == kREG(6) + 40.0f) {
        func_80AF37AC();
    } else if (*unk_2C4 > kREG(4) + 50.0f) {
        this->actor.textId = 0x403E;
        Message_StartTextbox(globalCtx, this->actor.textId, NULL);
        this->action = 17;
    }
}

void func_80AF39DC(EnRu2* this, GlobalContext* globalCtx) {
    s32 pad;
    MessageContext* msgCtx;
    s32 pad2;
    u8 dialogState;
    Player* player;
    s32 pad3;

    msgCtx = &globalCtx->msgCtx;
    dialogState = Message_GetState(msgCtx);

    if (dialogState == TEXT_STATE_DONE_FADING) {
        if (this->unk_2C3 != TEXT_STATE_DONE_FADING) {
            // "I'm Komatsu!" (cinema scene dev)
            osSyncPrintf("おれが小松だ！ \n");
            this->unk_2C2++;
            if (this->unk_2C2 % 6 == 3) {
                player = GET_PLAYER(globalCtx);
                // "uorya-!" (screeming sound)
                osSyncPrintf("うおりゃー！ \n");
                func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
                player->actor.world.pos.x = 820.0f;
                player->actor.world.pos.y = 0.0f;
                player->actor.world.pos.z = 180.0f;
            }
        }
    }

    this->unk_2C3 = dialogState;
    if (Message_GetState(msgCtx) == TEXT_STATE_CLOSING) {
        this->action = 18;
        func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
    }
}

void func_80AF3ADC(EnRu2* this, GlobalContext* globalCtx) {
    this->unk_2C4 += 1.0f;
    if (this->unk_2C4 > kREG(5) + 100.0f) {
        func_80AF28E8(this, &gAdultRutoSwimmingUpAnim, 0, -12.0f, 0);
        this->action = 19;
        func_80AF36EC(this, globalCtx);
    }
}

void func_80AF3B74(EnRu2* this, GlobalContext* globalCtx) {
    if (this->unk_2C0 > ((((u16)(kREG(3) + 0x28)) + ((u16)(kREG(2) + 0x96))) & 0xFFFF)) {
        Actor_Kill(&this->actor);
        OnePointCutscene_EndCutscene(globalCtx, this->subCamId);
    }
}

void func_80AF3BC8(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF3878(this, globalCtx);
    Actor_SetFocus(&this->actor, 50.0f);
    func_80AF259C(this, globalCtx);
}

void func_80AF3C04(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2744(this, globalCtx);
    func_80AF259C(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    Actor_SetFocus(&this->actor, 50.0f);
    func_80AF38D0(this, globalCtx);
}

void func_80AF3C64(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2744(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    Actor_SetFocus(&this->actor, 50.0f);
    func_80AF390C(this, globalCtx);
}

void func_80AF3CB8(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2744(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    Actor_SetFocus(&this->actor, 50.0f);
    func_80AF39DC(this, globalCtx);
}

void func_80AF3D0C(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF2744(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    Actor_SetFocus(&this->actor, 50.0f);
    func_80AF3ADC(this, globalCtx);
}

void func_80AF3D60(EnRu2* this, GlobalContext* globalCtx) {
    func_80AF37CC(this);
    func_80AF2744(this, globalCtx);
    EnRu2_UpdateSkelAnime(this);
    func_80AF2608(this);
    Actor_SetFocus(&this->actor, 50.0f);
    func_80AF3B74(this, globalCtx);
}

void EnRu2_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnRu2* this = (EnRu2*)thisx;

    if ((this->action < 0) || (this->action >= ARRAY_COUNT(sActionFuncs)) || (sActionFuncs[this->action] == NULL)) {
        // "Main Mode is improper!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, globalCtx);
}

void EnRu2_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnRu2* this = (EnRu2*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    func_80AF2550(thisx, globalCtx);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gAdultRutoSkel, NULL, this->jointTable, this->morphTable, 23);

    switch (func_80AF26A0(this)) {
        case 2:
            func_80AF2E1C(this, globalCtx);
            break;
        case 3:
            func_80AF3394(this, globalCtx);
            break;
        case 4:
            func_80AF3744(this, globalCtx);
            break;
        default:
            func_80AF2994(this, globalCtx);
            break;
    }

    this->unk_2C2 = 0;
    this->unk_2C3 = TEXT_STATE_DONE_FADING;
    this->subCamId = 0;
}

void func_80AF3F14(EnRu2* this, GlobalContext* globalCtx) {
}

void func_80AF3F20(EnRu2* this, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 temp = this->unk_2A4;
    void* tex = sEyeTextures[temp];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL,
                          this);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void EnRu2_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnRu2* this = (EnRu2*)thisx;

    // FAST3D: This is a hack for the issue of both TEXEL0 and TEXEL1 using the same texture with different settings.
    // Ruto's earring uses both TEXEL0 and TEXEL1 to render. The issue is that it never loads anything into TEXEL1, so
    // it reuses whatever happens to be there, which is the water temple brick texture. It just so happens that the
    // earring texture loads into the same place in tmem as the brick texture, so when it comes to rendering, TEXEL1
    // uses the earring texture with diffrent clamp settings, and it displays without noticeable error. However, both
    // texel samplers are not intended to be used for the same texture with different settings, so this misuse confuses
    // our texture cache, and we load the wrong settings for the earrings texture. This patch is a hack that replaces
    // TEXEL1 with TEXEL0, which is most likely the original intention, and all is well.
    Gfx* gfx = ResourceMgr_LoadGfxByName(gAdultRutoHeadDL);
    Gfx patch = gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, TEXEL0, 0,
                                  PRIM_LOD_FRAC, COMBINED);
    gfx[0xA2] = patch;

    if ((this->drawConfig < 0) || (this->drawConfig >= ARRAY_COUNT(sDrawFuncs)) ||
        (sDrawFuncs[this->drawConfig] == 0)) {
        // "Draw Mode is improper!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawConfig](this, globalCtx);
}
