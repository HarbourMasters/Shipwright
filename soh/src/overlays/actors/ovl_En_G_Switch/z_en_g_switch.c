/*
 * File: z_en_g_switch.c
 * Overlay: ovl_En_G_Switch
 * Description: Silver rupees, shooting gallery targets, and horseback archery pots
 */

#include "z_en_g_switch.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Syateki_Itm/z_en_syateki_itm.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "overlays/effects/ovl_Effect_Ss_HitMark/z_eff_ss_hitmark.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_tsubo/object_tsubo.h"
#include "objects/object_gi_rupy/object_gi_rupy.h"
#include "soh/frame_interpolation.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /* 0 */ MOVE_TARGET,
    /* 1 */ MOVE_HOME
} GSwitchMoveState;

void EnGSwitch_Init(Actor* thisx, PlayState* play);
void EnGSwitch_Destroy(Actor* thisx, PlayState* play);
void EnGSwitch_Update(Actor* thisx, PlayState* play);
void EnGSwitch_DrawRupee(Actor* thisx, PlayState* play);
void EnGSwitch_DrawPot(Actor* thisx, PlayState* play);

void EnGSwitch_SilverRupeeTracker(EnGSwitch* this, PlayState* play);
void EnGSwitch_SilverRupeeIdle(EnGSwitch* this, PlayState* play);
void EnGSwitch_WaitForObject(EnGSwitch* this, PlayState* play);
void EnGSwitch_SilverRupeeCollected(EnGSwitch* this, PlayState* play);
void EnGSwitch_GalleryRupee(EnGSwitch* this, PlayState* play);
void EnGSwitch_ArcheryPot(EnGSwitch* this, PlayState* play);
void EnGSwitch_Kill(EnGSwitch* this, PlayState* play);

void EnGSwitch_SpawnEffects(EnGSwitch* this, Vec3f* pos, s16 scale, s16 colorIdx);
void EnGSwitch_UpdateEffects(EnGSwitch* this, PlayState* play);
void EnGSwitch_DrawEffects(EnGSwitch* this, PlayState* play);

static s16 sCollectedCount = 0;

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 13, 40, 0, { 0, 0, 0 } },
};

// Unused, but probably intended to be this
static s16 sRupeeTypes[] = {
    ITEM00_RUPEE_GREEN, ITEM00_RUPEE_BLUE, ITEM00_RUPEE_RED, ITEM00_RUPEE_ORANGE, ITEM00_RUPEE_PURPLE,
};

const ActorInit En_G_Switch_InitVars = {
    ACTOR_EN_G_SWITCH,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnGSwitch),
    (ActorFunc)EnGSwitch_Init,
    (ActorFunc)EnGSwitch_Destroy,
    (ActorFunc)EnGSwitch_Update,
    NULL,
    NULL,
};

void EnGSwitch_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGSwitch* this = (EnGSwitch*)thisx;

    this->type = (this->actor.params >> 0xC) & 0xF;
    this->switchFlag = this->actor.params & 0x3F;
    this->numEffects = ARRAY_COUNT(this->effects);
    // "index"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ インデックス ☆☆☆☆☆ %x\n" VT_RST, this->type);
    // "save"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ セーブ\t     ☆☆☆☆☆ %x\n" VT_RST, this->switchFlag);
    switch (this->type) {
        case ENGSWITCH_SILVER_TRACKER:
            osSyncPrintf("\n\n");
            // "parent switch spawn"
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 親スイッチ発生 ☆☆☆☆☆ %x\n" VT_RST, this->actor.params);
            sCollectedCount = 0;
            this->silverCount = this->actor.params >> 6;
            this->silverCount &= 0x3F;
            // "maximum number of checks"
            osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 最大チェック数 ☆☆☆☆☆ %d\n" VT_RST, this->silverCount);
            osSyncPrintf("\n\n");
            if (Flags_GetSwitch(play, this->switchFlag)) {
                // This is a reference to Hokuto no Ken
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ Ｙｏｕ ａｒｅ Ｓｈｏｃｋ！  ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
                Actor_Kill(&this->actor);
            } else {
                this->actionFunc = EnGSwitch_SilverRupeeTracker;
            }
            break;
        case ENGSWITCH_SILVER_RUPEE:
            osSyncPrintf("\n\n");
            // "child switch spawn"
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 子スイッチ発生 ☆☆☆☆☆ %x\n" VT_RST, this->actor.params);
            this->colorIdx = 5;
            this->numEffects = 20;
            Collider_InitCylinder(play, &this->collider);
            Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
            this->actor.draw = EnGSwitch_DrawRupee;
            this->actor.shape.yOffset = 700.0f;

            if (CVar_GetS32("gNewDrops", 0) !=0) {
                this->actor.shape.yOffset = 35.0f;
            } else {
                this->actor.shape.yOffset = 700.0f;
            }

            if (Flags_GetSwitch(play, this->switchFlag)) {
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ Ｙｏｕ ａｒｅ Ｓｈｏｃｋ！  ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
                Actor_Kill(&this->actor);
            } else {
                if (CVar_GetS32("gNewDrops", 0) !=0) {
                    Actor_SetScale(&this->actor, 0.6f);
                } else {
                    Actor_SetScale(&this->actor, 0.03f);
                }
                this->actionFunc = EnGSwitch_SilverRupeeIdle;
            }
            break;
        case ENGSWITCH_ARCHERY_POT:
            osSyncPrintf("\n\n");
            // "Horseback archery destructible pot"
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ やぶさめぶち抜き壷 ☆☆☆☆☆ \n" VT_RST);
            this->actor.gravity = -3.0f;
            this->colorIdx = Rand_ZeroFloat(2.99f);
            Collider_InitCylinder(play, &this->collider);
            Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
            this->actor.scale.x = 0.25f;
            this->actor.scale.y = 0.45f;
            this->actor.scale.z = 0.25f;
            this->collider.info.bumper.dmgFlags = 0x1F820;
            this->objId = OBJECT_TSUBO;
            this->objIndex = Object_GetIndex(&play->objectCtx, this->objId);
            if (this->objIndex < 0) {
                Actor_Kill(&this->actor);
                // "what?"
                osSyncPrintf(VT_FGCOL(PURPLE) " なにみの？ %d\n" VT_RST "\n", this->objIndex);
                // "bank is funny"
                osSyncPrintf(VT_FGCOL(CYAN) " バンクおかしいしぞ！%d\n" VT_RST "\n", this->actor.params);
            }
            this->collider.dim.radius = 24;
            this->collider.dim.height = 74;
            this->collider.dim.yShift = 0;
            this->actionFunc = EnGSwitch_WaitForObject;
            break;
        case ENGSWITCH_TARGET_RUPEE:
            if (CVar_GetS32("gNewDrops", 0) !=0) {
                this->actor.shape.yOffset = 35.0f;
                Actor_SetScale(&this->actor, 0.9f);
            } else {
                this->actor.shape.yOffset = 700.0f;
                Actor_SetScale(&this->actor, 0.05f);
            }
            
            Collider_InitCylinder(play, &this->collider);
            Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
            this->actor.draw = EnGSwitch_DrawRupee;
            this->collider.dim.radius = 20;
            this->collider.dim.height = 60;
            this->collider.dim.yShift = 5;
            this->actionFunc = EnGSwitch_GalleryRupee;
            break;
    }
}

void EnGSwitch_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGSwitch* this = (EnGSwitch*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnGSwitch_Break(EnGSwitch* this, PlayState* play) {
    Vec3f randPos;
    Vec3f hitPos;
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    s32 i;

    randPos.x = this->actor.world.pos.x + Rand_CenteredFloat(40.0f);
    randPos.y = this->actor.world.pos.y + 30.0f + Rand_CenteredFloat(35.0f);
    randPos.z = this->actor.world.pos.z + Rand_CenteredFloat(40.0f);
    hitPos.x = this->collider.info.bumper.hitPos.x;
    hitPos.y = this->collider.info.bumper.hitPos.y;
    hitPos.z = this->collider.info.bumper.hitPos.z;
    EffectSsHitMark_SpawnCustomScale(play, EFFECT_HITMARK_WHITE, 700, &hitPos);
    if (this->type == ENGSWITCH_ARCHERY_POT) {
        velocity.y = 15.0f;
        EffectSsExtra_Spawn(play, &hitPos, &velocity, &accel, 5, 2);
    }
    if (this->type == ENGSWITCH_TARGET_RUPEE) {
        for (i = 0; i < this->numEffects; i++) {
            EnGSwitch_SpawnEffects(this, &randPos, 100, this->colorIdx);
        }
    }
}

void EnGSwitch_WaitForObject(EnGSwitch* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objIndex)) {
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->objIndex].segment);
        this->actor.objBankIndex = this->objIndex;
        this->actor.draw = EnGSwitch_DrawPot;
        this->actionFunc = EnGSwitch_ArcheryPot;
    }
}

void EnGSwitch_SilverRupeeTracker(EnGSwitch* this, PlayState* play) {
    static s8 majorScale[] = { 0, 2, 4, 5, 7 };

    if (this->noteIndex < sCollectedCount) {
        if (sCollectedCount < 5) {
            // "sound?"
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 音？ ☆☆☆☆☆ %d\n" VT_RST, this->noteIndex);
            Audio_PlaySoundTransposed(&D_801333D4, NA_SE_EV_FIVE_COUNT_LUPY, majorScale[this->noteIndex]);
            this->noteIndex = sCollectedCount;
        }
    }
    if (sCollectedCount >= this->silverCount) {
        // "It is now the end of the century."
        // This another reference to Hokuto no Ken.
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 時はまさに世紀末〜  ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
        // "Last!"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ らすとぉ！          ☆☆☆☆☆ \n" VT_RST);
        if ((play->sceneNum == SCENE_MEN) && (this->actor.room == 2)) {
            Flags_SetTempClear(play, this->actor.room);
        } else {
            func_80078884(NA_SE_SY_CORRECT_CHIME);
            Flags_SetSwitch(play, this->switchFlag);
        }
        func_80078884(NA_SE_SY_GET_RUPY);
        Actor_Kill(&this->actor);
    }
}

void EnGSwitch_SilverRupeeIdle(EnGSwitch* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.shape.rot.y += 0x800;
    if (this->actor.xyzDistToPlayerSq < 900.0f) {
        Rupees_ChangeBy(5);
        sCollectedCount++;
        func_80078884(NA_SE_SY_GET_RUPY);
        this->actor.world.pos = player->actor.world.pos;
        this->actor.world.pos.y += 40.0f;
        if (LINK_IS_ADULT) {
            this->actor.world.pos.y += 20.0f;
        }
        this->actor.gravity = 0.0f;
        this->killTimer = 15;
        this->actionFunc = EnGSwitch_SilverRupeeCollected;
    }
}

void EnGSwitch_SilverRupeeCollected(EnGSwitch* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.shape.rot.y += 0x3C0;
    if (this->killTimer == 0) {
        Actor_Kill(&this->actor);
        return;
    }
    this->actor.world.pos = player->actor.world.pos;
    this->actor.world.pos.y =
        player->actor.world.pos.y + 40.0f + (this->killTimer * 0.3f) * Math_SinS(this->killTimer * 0x3A98);
    if (LINK_IS_ADULT) {
        this->actor.world.pos.y += 20.0f;
    }
}

void EnGSwitch_GalleryRupee(EnGSwitch* this, PlayState* play) {
    EnSyatekiItm* gallery;

    this->actor.shape.rot.y += 0x3C0;
    if (this->delayTimer == 0) {
        switch (this->moveMode) {
            case GSWITCH_THROW:
                Actor_MoveForward(&this->actor);
                if ((this->actor.velocity.y < 0.0f) && (this->actor.world.pos.y < (this->actor.home.pos.y - 50.0f))) {
                    gallery = ((EnSyatekiItm*)this->actor.parent);
                    this->actor.velocity.y = 0.0f;
                    this->actor.gravity = 0.0f;
                    if (gallery->actor.update != NULL) {
                        gallery->targetState[this->index] = ENSYATEKIHIT_MISS;
                    }
                    Actor_Kill(&this->actor);
                }
                break;
            case GSWITCH_LEFT:
                func_8002D7EC(&this->actor);
                if ((this->actor.velocity.x < 0.0f) && (this->actor.world.pos.x < this->targetPos.x)) {
                    gallery = ((EnSyatekiItm*)this->actor.parent);
                    if (gallery->actor.update != NULL) {
                        gallery->targetState[this->index] = ENSYATEKIHIT_MISS;
                    }
                    Actor_Kill(&this->actor);
                }
                break;
            case GSWITCH_RIGHT:
                func_8002D7EC(&this->actor);
                if (this->actor.world.pos.x > this->targetPos.x) {
                    gallery = ((EnSyatekiItm*)this->actor.parent);
                    if (gallery->actor.update != NULL) {
                        gallery->targetState[this->index] = ENSYATEKIHIT_MISS;
                    }
                    Actor_Kill(&this->actor);
                }
                break;
            default:
                switch (this->moveState) {
                    case MOVE_TARGET:
                        if ((fabsf(this->actor.world.pos.x - this->targetPos.x) > 5.0f) ||
                            (fabsf(this->actor.world.pos.y - this->targetPos.y) > 5.0f)) {
                            Math_ApproachF(&this->actor.world.pos.x, this->targetPos.x, 0.3f, 30.0f);
                            Math_ApproachF(&this->actor.world.pos.y, this->targetPos.y, 0.3f, 30.0f);
                        } else {
                            this->moveState = MOVE_HOME;
                            this->waitTimer = 60;
                        }
                        break;
                    case MOVE_HOME:
                        if (this->waitTimer == 0) {
                            if ((fabsf(this->actor.world.pos.x - this->actor.home.pos.x) > 5.0f) ||
                                (fabsf(this->actor.world.pos.y - this->actor.home.pos.y) > 5.0f)) {
                                Math_ApproachF(&this->actor.world.pos.x, this->actor.home.pos.x, 0.3f, 30.0f);
                                Math_ApproachF(&this->actor.world.pos.y, this->actor.home.pos.y, 0.3f, 30.0f);
                            } else {
                                gallery = ((EnSyatekiItm*)this->actor.parent);
                                if (gallery->actor.update != NULL) {
                                    gallery->targetState[this->index] = ENSYATEKIHIT_MISS;
                                }
                                Actor_Kill(&this->actor);
                            }
                        }
                        break;
                }
                break;
        }
        if ((this->collider.base.acFlags & AC_HIT) || BREG(8)) {
            gallery = ((EnSyatekiItm*)this->actor.parent);
            this->collider.base.acFlags &= ~AC_HIT;
            if (gallery->actor.update != NULL) {
                gallery->hitCount++;
                gallery->targetState[this->index] = ENSYATEKIHIT_HIT;
                func_80078884(NA_SE_EV_HIT_SOUND);
                func_80078884(NA_SE_SY_GET_RUPY);
                // "Yeah !"
                osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ いぇぇーす！ＨＩＴ！！ ☆☆☆☆☆ %d\n" VT_RST, gallery->hitCount);
                EnGSwitch_Break(this, play);
                this->killTimer = 50;
                this->broken = true;
                this->actionFunc = EnGSwitch_Kill;
            }
        }
    }
}

void EnGSwitch_ArcheryPot(EnGSwitch* this, PlayState* play) {
    s32 i;
    s16 angle;
    Vec3f* thisPos = &this->actor.world.pos;

    this->actor.shape.rot.y += 0x3C0;
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        for (i = 0, angle = 0; i < 30; i++, angle += 0x4E20) {
            Vec3f pos;
            Vec3f vel;
            f32 sn = Math_SinS(angle);
            f32 cs = Math_CosS(angle);
            f32 rand;
            s32 phi_s0;
            s32 scale;
            s32 pad;

            pos.x = sn * 8.0f;
            pos.y = 10.0f + Rand_CenteredFloat(5.0f);
            pos.z = cs * 8.0f;

            vel.x = pos.x / 2.0f;
            vel.y = 10.0f + Rand_ZeroOne() * 15.0f;
            vel.z = pos.z / 2.0f;

            pos.x += thisPos->x;
            pos.y += thisPos->y;
            pos.z += thisPos->z;

            rand = Rand_ZeroOne();
            if (rand < 0.2f) {
                phi_s0 = 0x60;
            } else if (rand < 0.6f) {
                phi_s0 = 0x40;
            } else {
                phi_s0 = 0x20;
            }

            scale = 30.0f + Rand_ZeroOne() * 130.0f;

            EffectSsKakera_Spawn(play, &pos, &vel, thisPos, -240, phi_s0, 10, 10, 0, scale, 0, 0x20, 60,
                                 KAKERA_COLOR_NONE, OBJECT_TSUBO, object_tsubo_DL_001960);
        }
        func_80033480(play, thisPos, 30.0f, 4, 20, 50, 0);
        SoundSource_PlaySfxAtFixedWorldPos(play, thisPos, 40, NA_SE_EV_POT_BROKEN);
        EnGSwitch_Break(this, play);
        this->killTimer = 50;
        this->broken = true;
        this->actionFunc = EnGSwitch_Kill;
    }
}

void EnGSwitch_Kill(EnGSwitch* this, PlayState* play) {
    if (this->killTimer == 0) {
        Actor_Kill(&this->actor);
    }
}

void EnGSwitch_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGSwitch* this = (EnGSwitch*)thisx;

    this->actionFunc(this, play);
    if (this->killTimer != 0) {
        this->killTimer--;
    }
    if (this->waitTimer != 0) {
        this->waitTimer--;
    }
    if (this->delayTimer != 0) {
        this->delayTimer--;
    }
    if ((this->type != ENGSWITCH_SILVER_TRACKER) && (this->type != ENGSWITCH_SILVER_RUPEE) &&
        (this->type != ENGSWITCH_TARGET_RUPEE)) {
        Actor_MoveForward(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 50.0f, 100.0f, 0x1C);
    }
    if (this->actor.draw != NULL) {
        if (this->type == ENGSWITCH_TARGET_RUPEE) {
            EnGSwitch_UpdateEffects(this, play);
        }
        if ((this->actionFunc != EnGSwitch_Kill) && (this->actionFunc != EnGSwitch_SilverRupeeIdle)) {
            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
    if (BREG(0) && (this->type == ENGSWITCH_SILVER_TRACKER)) {
        DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }
}

void EnGSwitch_DrawPot(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGSwitch* this = (EnGSwitch*)thisx;

    if (!this->broken) {
        OPEN_DISPS(play->state.gfxCtx);
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, object_tsubo_DL_0017C0);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

static void* sRupeeTextures[] = {
    gRupeeGreenTex, gRupeeBlueTex, gRupeeRedTex, gRupeePinkTex, gRupeeOrangeTex, gRupeeSilverTex,
};
static void* sRupeeTexturesNew[] = {
    GID_RUPEE_GREEN, GID_RUPEE_BLUE, GID_RUPEE_RED, GID_RUPEE_PURPLE, GID_RUPEE_GOLD,
};
void EnGSwitch_DrawRupee(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGSwitch* this = (EnGSwitch*)thisx;

    if (!this->broken) {
        OPEN_DISPS(play->state.gfxCtx);
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        func_8002EBCC(&this->actor, play, 0);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (CVar_GetS32("gNewDrops", 0) !=0) {
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
            if (this->type == ENGSWITCH_TARGET_RUPEE) {
                GetItem_Draw(play, sRupeeTexturesNew[this->colorIdx]);
            } else {
                gDPSetGrayscaleColor(POLY_OPA_DISP++, 255, 255, 255, 255);
                gSPGrayscale(POLY_OPA_DISP++, true);
                Gfx_SetupDL_25Opa(play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gGiRupeeInnerDL);
                gSPDisplayList(POLY_OPA_DISP++, gGiGoldRupeeInnerColorDL);
                Gfx_SetupDL_25Xlu(play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gGiRupeeOuterDL);
                gSPDisplayList(POLY_OPA_DISP++, gGiGoldRupeeOuterColorDL);
                gSPGrayscale(POLY_OPA_DISP++, false);
            }
        } else {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sRupeeTextures[this->colorIdx]));
            gSPDisplayList(POLY_OPA_DISP++, gRupeeDL);
        }
        CLOSE_DISPS(play->state.gfxCtx);
    }
    if (this->type == ENGSWITCH_TARGET_RUPEE) {
        EnGSwitch_DrawEffects(this, play);
    }
}

void EnGSwitch_SpawnEffects(EnGSwitch* this, Vec3f* pos, s16 scale, s16 colorIdx) {
    EnGSwitchEffect* effect = this->effects;
    s16 i;

    for (i = 0; i < this->numEffects; i++, effect++) {
        if (!effect->flag) {
            Vec3f baseVel;
            f32 pitch;
            f32 yaw;

            effect->epoch++;
            effect->pos = *pos;
            effect->scale = scale;
            effect->colorIdx = colorIdx;
            effect->timer = 30;
            effect->rot.x = effect->rot.y = effect->rot.z = 0.0f;
            pitch = Rand_CenteredFloat(1000.0f) - 13000.0f;
            yaw = Rand_CenteredFloat(65535.0f);
            Matrix_RotateY(yaw, MTXMODE_NEW);
            Matrix_RotateX(pitch, MTXMODE_APPLY);
            baseVel.x = baseVel.y = 0.0f;
            baseVel.z = 20.0f;
            Matrix_MultVec3f(&baseVel, &effect->velocity);
            effect->flag = true;
            return;
        }
    }
}

void EnGSwitch_UpdateEffects(EnGSwitch* this, PlayState* play) {
    Vec3f temp;
    s16 i;
    EnGSwitchEffect* effect = this->effects;

    for (i = 0; i < this->numEffects; i++, effect++) {
        if (effect->flag) {
            effect->rot.x += Rand_ZeroOne() * 10.0f + 15.0f;
            effect->rot.y += Rand_ZeroOne() * 10.0f + 15.0f;
            effect->rot.z += Rand_ZeroOne() * 10.0f + 15.0f;
            temp.x = effect->pos.x + effect->velocity.x;
            temp.y = effect->pos.y + effect->velocity.y;
            temp.z = effect->pos.z + effect->velocity.z;
            Math_ApproachF(&effect->pos.x, temp.x, 0.3f, 30.0f);
            Math_ApproachF(&effect->pos.y, temp.y, 0.8f, 250.0f);
            Math_ApproachF(&effect->pos.z, temp.z, 0.3f, 30.0f);
            Math_ApproachF(&effect->velocity.y, -20.0f, 0.9f, 1.0f);
            if (effect->timer != 0) {
                effect->timer--;
            } else if (effect->scale < 10) {
                effect->flag = false;
            } else {
                effect->scale -= 2;
            }
        }
    }
}

void EnGSwitch_DrawEffects(EnGSwitch* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    EnGSwitchEffect* effect = this->effects;
    s16 i;
    f32 scale;
    s32 pad;

    OPEN_DISPS(gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    for (i = 0; i < this->numEffects; i++, effect++) {
        if (effect->flag) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            scale = effect->scale / 10000.0f;
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            Matrix_RotateX(effect->rot.x, MTXMODE_APPLY);
            Matrix_RotateY(effect->rot.y, MTXMODE_APPLY);
            Matrix_RotateZ(effect->rot.z, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sRupeeTextures[effect->colorIdx]));
            gSPDisplayList(POLY_OPA_DISP++, gRupeeDL);
            FrameInterpolation_RecordCloseChild();
        }
    }
    CLOSE_DISPS(gfxCtx);
}
