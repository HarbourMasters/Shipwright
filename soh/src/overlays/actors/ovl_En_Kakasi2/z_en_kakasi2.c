/*
 * File: z_en_kakasi2.c
 * Overlay: ovl_En_Kakasi2
 * Description: Pierre the Scarecrow Spawn
 */

#include "z_en_kakasi2.h"
#include "vt.h"
#include "objects/object_ka/object_ka.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_25 | ACTOR_FLAG_27)

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

void EnKakasi2_Init(Actor* thisx, GlobalContext* globalCtx);
void EnKakasi2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnKakasi2_Update(Actor* thisx, GlobalContext* globalCtx);
void func_80A90948(Actor* thisx, GlobalContext* globalCtx);

void func_80A9062C(EnKakasi2* this, GlobalContext* globalCtx);
void func_80A90264(EnKakasi2* this, GlobalContext* globalCtx);
void func_80A904D8(EnKakasi2* this, GlobalContext* globalCtx);
void func_80A90578(EnKakasi2* this, GlobalContext* globalCtx);
void func_80A906C4(EnKakasi2* this, GlobalContext* globalCtx);

const ActorInit En_Kakasi2_InitVars = {
    ACTOR_EN_KAKASI2,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_KA,
    sizeof(EnKakasi2),
    (ActorFunc)EnKakasi2_Init,
    (ActorFunc)EnKakasi2_Destroy,
    (ActorFunc)EnKakasi2_Update,
    NULL,
    NULL,
};

void EnKakasi2_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi2* this = (EnKakasi2*)thisx;
    s32 pad;
    f32 spawnRangeY;
    f32 spawnRangeXZ;

    osSyncPrintf("\n\n");
    // "Visit Umeda"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 梅田参号見参！ ☆☆☆☆☆ \n" VT_RST);

    this->switchFlag = this->actor.params & 0x3F;
    spawnRangeY = (this->actor.params >> 6) & 0xFF;
    spawnRangeXZ = this->actor.world.rot.z;
    if (this->switchFlag == 0x3F) {
        this->switchFlag = -1;
    }
    this->actor.targetMode = 4;
    this->maxSpawnDistance.x = (spawnRangeY * 40.0f) + 40.0f;
    this->maxSpawnDistance.y = (spawnRangeXZ * 40.0f) + 40.0f;

    // "Former? (Argument 0)"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 元？(引数０) ☆☆☆☆ %f\n" VT_RST, spawnRangeY);
    // "Former? (Z angle)"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 元？(Ｚアングル) ☆☆ %f\n" VT_RST, spawnRangeXZ);
    // "Correction coordinates X"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 補正座標Ｘ ☆☆☆☆☆ %f\n" VT_RST, this->maxSpawnDistance.x);
    // "Correction coordinates Y"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 補正座標Ｙ ☆☆☆☆☆ %f\n" VT_RST, this->maxSpawnDistance.y);
    // "Correction coordinates Z"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 補正座標Ｚ ☆☆☆☆☆ %f\n" VT_RST, this->maxSpawnDistance.z);
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ SAVE       ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
    osSyncPrintf("\n\n");

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->height = 60.0f;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.flags |= ACTOR_FLAG_10;
    this->unk_198 = this->actor.shape.rot.y;

    if (this->switchFlag >= 0 && Flags_GetSwitch(globalCtx, this->switchFlag)) {
        this->actor.draw = func_80A90948;
        Collider_InitCylinder(globalCtx, &this->collider);
        Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
        SkelAnime_InitFlex(globalCtx, &this->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);
        this->actionFunc = func_80A9062C;
    } else {
        this->actionFunc = func_80A90264;
        this->actor.shape.yOffset = -8000.0f;
    }
}

void EnKakasi2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi2* this = (EnKakasi2*)thisx;

    Collider_DestroyCylinder(globalCtx, &this->collider);
    SkelAnime_Free(&this->skelAnime, globalCtx); // OTR - Fixed this memory leak
    //! @bug SkelAnime_Free is not called
}

void func_80A90264(EnKakasi2* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    this->unk_194++;

    if (((BREG(1) != 0) && (this->actor.xzDistToPlayer < this->maxSpawnDistance.x) &&
            (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < this->maxSpawnDistance.y)) ||
            ((CVar_GetS32("gSkipScarecrow",0) != 0) && (globalCtx->msgCtx.ocarinaAction == OCARINA_ACTION_FREE_PLAY) &&
            gSaveContext.scarecrowSpawnSongSet)){
        this->actor.draw = func_80A90948;
        Collider_InitCylinder(globalCtx, &this->collider);
        Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
        SkelAnime_InitFlex(globalCtx, &this->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);
        OnePointCutscene_Attention(globalCtx, &this->actor);
        this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_27;

        func_80078884(NA_SE_SY_CORRECT_CHIME);
        if (this->switchFlag >= 0) {
            Flags_SetSwitch(globalCtx, this->switchFlag);
        }

        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ SAVE 終了 ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
        this->actionFunc = func_80A904D8;
    } else if ((this->actor.xzDistToPlayer < this->maxSpawnDistance.x) &&
               (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < this->maxSpawnDistance.y) &&
               (gSaveContext.eventChkInf[9] & 0x1000)) {

        this->unk_194 = 0;
        if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_0B) {
            if (this->switchFlag >= 0) {
                Flags_SetSwitch(globalCtx, this->switchFlag);
            }
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ SAVE 終了 ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
            globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
            this->actor.draw = func_80A90948;
            Collider_InitCylinder(globalCtx, &this->collider);
            Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
            SkelAnime_InitFlex(globalCtx, &this->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL,
                               0);
            OnePointCutscene_Attention(globalCtx, &this->actor);
            func_80078884(NA_SE_SY_CORRECT_CHIME);

            this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_27;
            this->actionFunc = func_80A904D8;
        }
    }
}

void func_80A904D8(EnKakasi2* this, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&object_ka_Anim_000214);

    Animation_Change(&this->skelAnime, &object_ka_Anim_000214, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EV_COME_UP_DEKU_JR);
    this->actionFunc = func_80A90578;
}

void func_80A90578(EnKakasi2* this, GlobalContext* globalCtx) {
    s16 currentFrame;

    SkelAnime_Update(&this->skelAnime);

    currentFrame = this->skelAnime.curFrame;
    if (currentFrame == 11 || currentFrame == 17) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_KAKASHI_SWING);
    }

    this->actor.shape.rot.y += 0x800;
    Math_ApproachZeroF(&this->actor.shape.yOffset, 0.5f, 500.0f);

    if (this->actor.shape.yOffset > -100.0f) {
        this->actionFunc = func_80A9062C;
        this->actor.shape.yOffset = 0.0f;
    }
}

void func_80A9062C(EnKakasi2* this, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&object_ka_Anim_000214);

    Animation_Change(&this->skelAnime, &object_ka_Anim_000214, 0.0f, 0.0f, (s16)frameCount, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = func_80A906C4;
}

void func_80A906C4(EnKakasi2* this, GlobalContext* globalCtx) {
    if (this->skelAnime.curFrame != 0) {
        Math_ApproachZeroF(&this->skelAnime.curFrame, 0.5f, 1.0f);
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->unk_198, 5, 0xBB8, 0);
    SkelAnime_Update(&this->skelAnime);
}

void EnKakasi2_Update(Actor* thisx, GlobalContext* globalCtx2) {
    EnKakasi2* this = (EnKakasi2*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    this->actor.world.rot = this->actor.shape.rot;
    Actor_SetFocus(&this->actor, this->height);
    this->actionFunc(this, globalCtx);
    Actor_MoveForward(&this->actor);

    if (this->actor.shape.yOffset == 0.0f) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    }
    if (BREG(0) != 0) {
        if (BREG(5) != 0) {
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ this->actor.player_distance ☆☆☆☆☆ %f\n" VT_RST,
                         this->actor.xzDistToPlayer);
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ this->hosei.x ☆☆☆☆☆ %f\n" VT_RST, this->maxSpawnDistance.x);
            osSyncPrintf("\n\n");
        }
        if (this->actor.draw == NULL) {
            if (this->unk_194 != 0) {
                if ((this->unk_194 % 2) == 0) {
                    DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                           this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z,
                                           1.0f, 1.0f, 1.0f, 70, 70, 70, 255, 4, globalCtx->state.gfxCtx);
                }
            } else {
                DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                       this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 0, 255, 255, 255, 4, globalCtx->state.gfxCtx);
            }
        }
    }
}

void func_80A90948(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi2* this = (EnKakasi2*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          NULL, NULL, this);
}
