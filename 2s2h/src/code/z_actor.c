/*
 * File: z_actor.c
 * Description:
 */

#include "z64actor.h"
#include "z64door.h"

#include "prevent_bss_reordering.h"
#include "fault.h"
#include "sys_cfb.h"
#include "loadfragment.h"
#include "z64horse.h"
#include "z64malloc.h"
#include "z64quake.h"
#include "z64rumble.h"

#include "code/fbdemo_circle/fbdemo_circle.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"
#include "overlays/actors/ovl_En_Part/z_en_part.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "objects/object_bdoor/object_bdoor.h"

#include <string.h>
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenPort.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/ObjectExtension/ObjectExtension.h"
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/BenGui/CosmeticEditor.h"

// bss
// FaultClient sActorFaultClient; // 2 funcs

CollisionPoly* D_801ED8B0; // 1 func
s32 D_801ED8B4;            // 2 funcs

struct Actor* sNearestAttentionActor;
struct Actor* sPrioritizedAttentionActor;
struct Actor* sNearestCameraDriftActor;
struct Actor* sPrioritizedCameraDriftActor;

f32 sNearestAttentionActorDistSq;
f32 sBgmEnemyDistSq;
f32 sNearestCameraDriftActorDistSq;
s32 sHighestAttentionPriority;
s32 sHighestCameraDriftPriority;
s16 sAttentionPlayerRotY;

Mtx sActorHiliteMtx;

struct Actor* gCameraDriftActor;

#define ACTOR_AUDIO_FLAG_SFX_ACTOR_POS_1 (1 << 0)
#define ACTOR_AUDIO_FLAG_SFX_ACTOR_POS_2 (1 << 1) // identical behavior to ACTOR_AUDIO_FLAG_SFX_ACTOR_POS_1
#define ACTOR_AUDIO_FLAG_SFX_CENTERED_1 (1 << 2)
#define ACTOR_AUDIO_FLAG_SFX_CENTERED_2 (1 << 3) // identical behavior to ACTOR_AUDIO_FLAG_SFX_CENTERED_1
#define ACTOR_AUDIO_FLAG_SFX_TIMER (1 << 4)
#define ACTOR_AUDIO_FLAG_SEQ_KAMARO_DANCE (1 << 5)
#define ACTOR_AUDIO_FLAG_SEQ_MUSIC_BOX_HOUSE (1 << 6)

#define ACTOR_AUDIO_FLAG_SFX_ALL                                                                      \
    (ACTOR_AUDIO_FLAG_SFX_TIMER | ACTOR_AUDIO_FLAG_SFX_CENTERED_2 | ACTOR_AUDIO_FLAG_SFX_CENTERED_1 | \
     ACTOR_AUDIO_FLAG_SFX_ACTOR_POS_2 | ACTOR_AUDIO_FLAG_SFX_ACTOR_POS_1)
#define ACTOR_AUDIO_FLAG_SEQ_ALL (ACTOR_AUDIO_FLAG_SEQ_MUSIC_BOX_HOUSE | ACTOR_AUDIO_FLAG_SEQ_KAMARO_DANCE)
#define ACTOR_AUDIO_FLAG_ALL (ACTOR_AUDIO_FLAG_SFX_ALL | ACTOR_AUDIO_FLAG_SEQ_ALL)

void Actor_KillAllOnHalfDayChange(PlayState* play, ActorContext* actorCtx);
Actor* Actor_SpawnEntry(ActorContext* actorCtx, ActorEntry* actorEntry, PlayState* play);
Actor* Actor_Delete(ActorContext* actorCtx, Actor* actor, PlayState* play);
void Attention_FindActor(PlayState* play, ActorContext* actorCtx, Actor** attentionActorP, Actor** cameraDriftActorP,
                         Player* player);
s32 Actor_CullingVolumeTest(PlayState* play, Actor* actor, Vec3f* projPos, f32 projW);
void Actor_AddToCategory(ActorContext* actorCtx, Actor* actor, u8 actorCategory);
Actor* Actor_RemoveFromCategory(PlayState* play, ActorContext* actorCtx, Actor* actorToRemove);

void Actor_PrintLists(ActorContext* actorCtx) {
#if 0
    ActorListEntry* actorList = &actorCtx->actorLists[0];
    Actor* actor;
    s32 category;

    FaultDrawer_SetCharPad(-2, 0);
    FaultDrawer_Printf("actor\n", gMaxActorId);
    FaultDrawer_Printf("No. Actor   Name Part SegName\n");

    for (category = 0; category < ACTORCAT_MAX; category++) {
        actor = actorList[category].first;

        while (actor != NULL) {
            FaultDrawer_Printf("%3d %08x %04x %3d %s\n", category, actor, actor->id, actor->category, "");
            actor = actor->next;
        }
    }
#endif
}

void ActorShape_Init(ActorShape* actorShape, f32 yOffset, ActorShadowFunc shadowDraw, f32 shadowScale) {
    actorShape->yOffset = yOffset;
    actorShape->shadowDraw = shadowDraw;
    actorShape->shadowScale = shadowScale;
    actorShape->shadowAlpha = 255;
}

void ActorShadow_Draw(Actor* actor, Lights* lights, PlayState* play, Gfx* dlist, Color_RGBA8* color) {
    if (actor->floorPoly != NULL) {
        f32 dy = actor->world.pos.y - actor->floorHeight;

        if (dy >= -50.0f && dy < 500.0f) {
            f32 shadowScale;
            MtxF mtx;

            OPEN_DISPS(play->state.gfxCtx);

            POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP, SETUPDL_44);

            gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0, 0,
                              COMBINED);

            dy = CLAMP(dy, 0.0f, 150.0f);
            shadowScale = 1.0f - (dy * (1.0f / 350.0f));
            if ((dy * (1.0f / 350.0f)) > 1.0f) {
                shadowScale = 0.0f;
            }

            if (color != NULL) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, color->r, color->g, color->b,
                                (u8)(actor->shape.shadowAlpha * shadowScale));
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, (u8)(actor->shape.shadowAlpha * shadowScale));
            }

            func_800C0094(actor->floorPoly, actor->world.pos.x, actor->floorHeight, actor->world.pos.z, &mtx);
            Matrix_Put(&mtx);

            if ((dlist != gCircleShadowDL) || (actor->scale.x != actor->scale.z)) {
                Matrix_RotateYS(actor->shape.rot.y, MTXMODE_APPLY);
            } else {
                // Mark non-rotating shadows to ignore the actor mtx prevents interpolation glitches when actor moves
                FrameInterpolation_IgnoreActorMtx();
            }

            shadowScale *= actor->shape.shadowScale;
            Matrix_Scale(actor->scale.x * shadowScale, 1.0f, actor->scale.z * shadowScale, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, dlist);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

void ActorShadow_DrawCircle(Actor* actor, Lights* lights, PlayState* play) {
    if (actor->bgCheckFlags & BGCHECKFLAG_PLAYER_400) {
        func_800B4AEC(play, actor, 50.0f);
    }

    ActorShadow_Draw(actor, lights, play, gCircleShadowDL, NULL);
}

void ActorShadow_DrawSquare(Actor* actor, Lights* lights, PlayState* play) {
    if (actor->bgCheckFlags & BGCHECKFLAG_PLAYER_400) {
        func_800B4AEC(play, actor, 50.0f);
    }

    ActorShadow_Draw(actor, lights, play, gSquareShadowDL, NULL);
}

void ActorShadow_DrawWhiteCircle(Actor* actor, Lights* lights, PlayState* play) {
    static Color_RGBA8 sColor = { 255, 255, 255, 255 };

    ActorShadow_Draw(actor, lights, play, gCircleShadowDL, &sColor);
}

void ActorShadow_DrawHorse(Actor* actor, Lights* lights, PlayState* play) {
    ActorShadow_Draw(actor, lights, play, gHorseShadowDL, NULL);
}

void ActorShadow_DrawFoot(PlayState* play, Light* light, MtxF* arg2, s32 lightNum, f32 shadowAlpha, f32 shadowScaleX,
                          f32 shadowScaleZ) {
    s32 pad;
    s16 sp58;
    f32 dir2;
    f32 dir0;

    OPEN_DISPS(play->state.gfxCtx);

    // Ignore the players rotation prevents feet shadow from glitching when turning abruptly
    FrameInterpolation_IgnoreActorMtx();

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, (u8)(CLAMP_MAX(lightNum * 1.3e-05f, 1.0f) * shadowAlpha));

    dir0 = light->l.dir[0];
    dir2 = light->l.dir[2];
    sp58 = Math_Atan2S_XY(dir2, dir0);
    shadowScaleZ *= (4.5f - (light->l.dir[1] * 0.035f));
    shadowScaleZ = CLAMP_MIN(shadowScaleZ, 1.0f);
    Matrix_Put(arg2);
    Matrix_RotateYS(sp58, MTXMODE_APPLY);
    Matrix_Scale(shadowScaleX, 1.0f, shadowScaleX * shadowScaleZ, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gFootShadowDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void ActorShadow_DrawFeet(Actor* actor, Lights* mapper, PlayState* play) {
    f32 distToFloor = actor->world.pos.y - actor->floorHeight;

    if (distToFloor > 0.0f) {
        f32 shadowScale = actor->shape.shadowScale;
        u8 shadowAlpha = actor->shape.shadowAlpha;
        f32 alphaRatio;

        if ((actor->id == ACTOR_PLAYER) && (((Player*)actor)->stateFlags3 & PLAYER_STATE3_8000)) {
            f32 prevScaleZ = actor->scale.z;

            actor->scale.z += 0.03f * fabsf(Math_CosS(((Player*)actor)->unk_AAA));
            actor->shape.shadowScale *= 0.2f;
            alphaRatio = distToFloor * 0.03f;
            actor->shape.shadowAlpha = actor->shape.shadowAlpha * CLAMP_MAX(alphaRatio, 1.0f);
            ActorShadow_Draw(actor, mapper, play, gCircleShadowDL, NULL);
            actor->scale.z = prevScaleZ;
        } else {
            actor->shape.shadowScale *= 0.3f;
            alphaRatio = (distToFloor - 20.0f) * 0.02f;
            actor->shape.shadowAlpha = actor->shape.shadowAlpha * CLAMP_MAX(alphaRatio, 1.0f);
            ActorShadow_DrawCircle(actor, mapper, play);
        }

        actor->shape.shadowScale = shadowScale;
        actor->shape.shadowAlpha = shadowAlpha;
    }

    if (distToFloor < 200.0f) {
        MtxF sp13C;
        MtxF spFC;
        CollisionPoly* poly;
        s32 bgId;
        f32 floorHeight[2];
        Light* firstLight = &mapper->l.l[0];
        f32 shadowAlpha;
        f32 shadowScaleX;
        f32 shadowScaleZ;
        Light* lightPtr;
        s32 lightNumMax;
        s32 i;
        s32 j;
        s32 lightNum;
        Vec3f* feetPosPtr;
        s32 numLights;
        f32* floorHeightPtr;
        s32 spB8;

        numLights = mapper->numLights - 2;
        feetPosPtr = actor->shape.feetPos;
        floorHeightPtr = floorHeight;

        OPEN_DISPS(play->state.gfxCtx);

        POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP, SETUPDL_44);
        actor->shape.feetFloorFlags = 0;
        spB8 = 2;

        for (i = 0; i < ARRAY_COUNT(floorHeight); i++, spB8 >>= 1) {
            feetPosPtr->y += 50.0f;
            *floorHeightPtr = Play_GetFloorSurfaceImpl(play, &sp13C, &poly, &bgId, feetPosPtr);
            feetPosPtr->y -= 50.0f;

            distToFloor = feetPosPtr->y - *floorHeightPtr;
            if ((distToFloor >= -1.0f) && (distToFloor < 500.0f)) {
                lightNumMax = 0;

                if (distToFloor <= 10.0f) {
                    actor->shape.feetFloorFlags |= spB8;

                    if ((actor->depthInWater < 0.0f) && (bgId == BGCHECK_SCENE) && (actor->shape.unk_17 & spB8)) {
                        if (SurfaceType_HasMaterialProperty(&play->colCtx, poly, bgId,
                                                            MATERIAL_PROPERTY_SOFT_IMPRINT)) {
                            SkinMatrix_MtxFCopy(&sp13C, &spFC);
                            SkinMatrix_MulYRotation(&spFC, actor->shape.rot.y);
                            EffFootmark_Add(play, &spFC, actor, i, feetPosPtr, (actor->shape.shadowScale * 0.3f),
                                            IREG(88) + 80, IREG(89) + 60, IREG(90) + 40, 30000, 200, 60);
                        }
                        actor->shape.unk_17 &= ~spB8;
                    }
                }

                if (distToFloor > 30.0f) {
                    distToFloor = 30.0f;
                }

                shadowAlpha = actor->shape.shadowAlpha * (1.0f - (distToFloor * (1 / 30.0f)));
                shadowScaleZ = 1.0f - (distToFloor * (1.0f / 70.0f));
                shadowScaleX = actor->shape.shadowScale * shadowScaleZ * actor->scale.x;

                lightPtr = firstLight;
                for (j = 0; j < numLights; j++) {
                    if (lightPtr->l.dir[1] > 0) {
                        lightNum = (lightPtr->l.col[0] + lightPtr->l.col[1] + lightPtr->l.col[2]) *
                                   ABS_ALT(lightPtr->l.dir[1]);

                        if (lightNum > 0) {
                            lightNumMax += lightNum;
                            ActorShadow_DrawFoot(play, lightPtr, &sp13C, lightNum, shadowAlpha, shadowScaleX,
                                                 shadowScaleZ);
                        }
                    }
                    lightPtr++;
                }

                for (j = 0; j < 2; j++) {
                    if (lightPtr->l.dir[1] > 0) {
                        lightNum = ((lightPtr->l.col[0] + lightPtr->l.col[1] + lightPtr->l.col[2]) *
                                    ABS_ALT(lightPtr->l.dir[1])) -
                                   (lightNumMax * 8);
                        if (lightNum > 0) {
                            ActorShadow_DrawFoot(play, lightPtr, &sp13C, lightNum, shadowAlpha, shadowScaleX,
                                                 shadowScaleZ);
                        }
                    }
                    lightPtr++;
                }
            }
            feetPosPtr++;
            floorHeightPtr++;
        }

        if (!(actor->bgCheckFlags & BGCHECKFLAG_GROUND)) {
            actor->shape.feetFloorFlags = 0;
        } else if (actor->shape.feetFloorFlags == 3) {
            f32 footDistY = actor->shape.feetPos[FOOT_LEFT].y - actor->shape.feetPos[FOOT_RIGHT].y;

            if ((floorHeight[0] + footDistY) < (floorHeight[1] - footDistY)) {
                actor->shape.feetFloorFlags = 2;
            } else {
                actor->shape.feetFloorFlags = 1;
            }
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void Actor_SetFeetPos(Actor* actor, s32 limbIndex, s32 leftFootIndex, Vec3f* leftFootPos, s32 rightFootIndex,
                      Vec3f* rightFootPos) {
    if (limbIndex == leftFootIndex) {
        Matrix_MultVec3f(leftFootPos, &actor->shape.feetPos[FOOT_LEFT]);
    } else if (limbIndex == rightFootIndex) {
        Matrix_MultVec3f(rightFootPos, &actor->shape.feetPos[FOOT_RIGHT]);
    }
}

void func_800B4AEC(PlayState* play, Actor* actor, f32 y) {
    s32 floorBgId;
    f32 yPos = actor->world.pos.y;

    actor->world.pos.y += y;
    actor->floorHeight =
        BgCheck_EntityRaycastFloor5_2(play, &play->colCtx, &actor->floorPoly, &floorBgId, actor, &actor->world.pos);
    actor->floorBgId = floorBgId;
    actor->world.pos.y = yPos;
}

void func_800B4B50(Actor* actor, Lights* mapper, PlayState* play) {
    f32 spEC;
    f32 temp_f12;
    f32 shadowScaleZ;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp_f8;
    s32 lightNum;
    MtxF sp94;
    s32 numLights;
    s8 phi_v1;
    u8 temp_v0;
    Light* phi_s0;
    s32 lightNumMax;

    if (actor->bgCheckFlags & BGCHECKFLAG_PLAYER_400) {
        func_800B4AEC(play, actor, 50.0f);
    }

    if (actor->floorPoly != NULL) {
        s32 j;

        spEC = actor->world.pos.y - actor->floorHeight;
        if (spEC > 20.0f) {
            f32 temp_f20 = actor->shape.shadowScale;

            temp_v0 = actor->shape.shadowAlpha;
            actor->shape.shadowScale *= 0.3f;
            temp_f12 = (spEC - 20.0f) * 0.02f;
            actor->shape.shadowAlpha = CLAMP_MAX(temp_f12, 1.0f) * actor->shape.shadowAlpha;
            ActorShadow_DrawCircle(actor, mapper, play);
            actor->shape.shadowScale = temp_f20;
            actor->shape.shadowAlpha = temp_v0;
        } else if (spEC >= -1.0f) {
            numLights = mapper->numLights - 2;

            OPEN_DISPS(play->state.gfxCtx);

            POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP, SETUPDL_44);

            func_800C0094(actor->floorPoly, actor->world.pos.x, actor->floorHeight, actor->world.pos.z, &sp94);
            temp_f22 = (f32)actor->shape.shadowAlpha * (1.0f - (spEC * (1.0f / 30.0f)));
            phi_s0 = mapper->l.l;
            shadowScaleZ = 1.0f - (spEC * (1.0f / 70.0f));
            temp_f24 = actor->shape.shadowScale * shadowScaleZ * actor->scale.x;

            lightNumMax = 0;
            for (j = 0; j < numLights; j++, phi_s0++) {
                if (phi_s0->l.dir[1] > 0) {
                    lightNum = (phi_s0->l.col[0] + phi_s0->l.col[1] + phi_s0->l.col[2]) * ABS_ALT(phi_s0->l.dir[1]);
                    if (lightNum > 0) {
                        lightNumMax += lightNum;
                        ActorShadow_DrawFoot(play, phi_s0, &sp94, lightNum, temp_f22, temp_f24, shadowScaleZ);
                    }
                }
            }

            for (j = 0; j < 2; j++, phi_s0++) {
                if (phi_s0->l.dir[1] > 0) {
                    lightNum = ((phi_s0->l.col[0] + phi_s0->l.col[1] + phi_s0->l.col[2]) * ABS_ALT(phi_s0->l.dir[1])) -
                               (8 * lightNumMax);
                    if (lightNum > 0) {
                        ActorShadow_DrawFoot(play, phi_s0, &sp94, lightNum, temp_f22, temp_f24, shadowScaleZ);
                    }
                }
            }
            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

void Actor_GetProjectedPos(PlayState* play, Vec3f* worldPos, Vec3f* projectedPos, f32* invW) {
    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, worldPos, projectedPos, invW);
    *invW = (*invW < 1.0f) ? 1.0f : (1.0f / *invW);
}

typedef struct AttentionColor {
    /* 0x0 */ Color_RGBA8 primary;   // Used for Tatl's inner color, lock-on arrow, and lock-on reticle
    /* 0x4 */ Color_RGBA8 secondary; // Used for Tatl's outer color
} AttentionColor;                    // size = 0x8

AttentionColor sAttentionColors[ACTORCAT_MAX + 1] = {
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_SWITCH
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_BG
    { { 255, 255, 230, 255 }, { 220, 160, 80, 0 } },  // ACTORCAT_PLAYER
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_EXPLOSIVES
    { { 150, 150, 255, 255 }, { 150, 150, 255, 0 } }, // ACTORCAT_NPC
    { { 255, 255, 0, 255 }, { 200, 155, 0, 0 } },     // ACTORCAT_ENEMY
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_PROP
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_ITEMACTION
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_MISC
    { { 255, 255, 0, 255 }, { 200, 155, 0, 0 } },     // ACTORCAT_BOSS
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_DOOR
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_CHEST
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // unused extra entry
};

void Attention_SetReticlePos(Attention* attention, s32 reticleNum, f32 x, f32 y, f32 z) {
    attention->lockOnReticles[reticleNum].pos.x = x;
    attention->lockOnReticles[reticleNum].pos.y = y;
    attention->lockOnReticles[reticleNum].pos.z = z;

    attention->lockOnReticles[reticleNum].radius = attention->reticleRadius;
}

void Attention_InitReticle(Attention* attention, ActorType actorCategory, PlayState* play) {
    LockOnReticle* reticle;
    AttentionColor* attentionColor = &sAttentionColors[actorCategory];
    s32 i;

    Math_Vec3f_Copy(&attention->reticlePos, &play->view.eye);

    attention->reticleRadius = 500.0f; // radius starts wide to zoom in on the actor
    attention->reticleFadeAlphaControl = 256;

    reticle = &attention->lockOnReticles[0];

    for (i = 0; i < ARRAY_COUNT(attention->lockOnReticles); i++, reticle++) {
        Attention_SetReticlePos(attention, i, 0.0f, 0.0f, 0.0f);

        reticle->color.r = attentionColor->primary.r;
        reticle->color.g = attentionColor->primary.g;
        reticle->color.b = attentionColor->primary.b;
    }
}

void Attention_SetTatlState(Attention* attention, Actor* actor, ActorType actorCategory, PlayState* play) {
    AttentionColor* attentionColor = &sAttentionColors[actorCategory];

    attention->tatlHoverPos.x = actor->focus.pos.x;
    attention->tatlHoverPos.y = actor->focus.pos.y + (actor->lockOnArrowOffset * actor->scale.y);
    attention->tatlHoverPos.z = actor->focus.pos.z;

    attention->tatlInnerColor.r = attentionColor->primary.r;
    attention->tatlInnerColor.g = attentionColor->primary.g;
    attention->tatlInnerColor.b = attentionColor->primary.b;
    attention->tatlInnerColor.a = attentionColor->primary.a;

    attention->tatlOuterColor.r = attentionColor->secondary.r;
    attention->tatlOuterColor.g = attentionColor->secondary.g;
    attention->tatlOuterColor.b = attentionColor->secondary.b;
    attention->tatlOuterColor.a = attentionColor->secondary.a;
}

void Attention_Init(Attention* attention, Actor* actor, PlayState* play) {
    attention->tatlHoverActor = attention->reticleActor = attention->forcedLockOnActor = attention->bgmEnemy = NULL;

    attention->reticleSpinCounter = 0;
    attention->curReticle = 0;
    attention->tatlMoveProgressFactor = 0.0f;

    Attention_SetTatlState(attention, actor, actor->category, play);
    Attention_InitReticle(attention, actor->category, play);
}

void Attention_Draw(Attention* attention, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* actor; // used for both the reticle actor and arrow hover actor

    if (player->stateFlags1 & (PLAYER_STATE1_2 | PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_200 |
                               PLAYER_STATE1_400 | PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000)) {
        return;
    }

    actor = attention->reticleActor;

    OPEN_DISPS(play->state.gfxCtx);

    if (attention->reticleFadeAlphaControl != 0) {
        LockOnReticle* reticle;
        s16 alpha = 255;
        f32 projectedPosScale = 1.0f;
        Vec3f projectedPos;
        s32 numReticles;
        f32 invW;
        s32 i;
        s32 curReticle;
        f32 lockOnScaleX;

        FrameInterpolation_RecordOpenChild(actor, 0);

        if (attention->reticleSpinCounter != 0) {
            // Reticle is spinning so it is active, only need to draw one
            numReticles = 1;
        } else {
            // Use multiple reticles for the motion blur effect from the reticle
            // quickly zooming in on an actor from off screen
            numReticles = ARRAY_COUNT(attention->lockOnReticles);
        }

        if (actor != NULL) {
            Math_Vec3f_Copy(&attention->reticlePos, &actor->focus.pos);
            projectedPosScale = (500.0f - attention->reticleRadius) / 420.0f;
        } else {
            // Not locked on, start fading out
            attention->reticleFadeAlphaControl -= 120;

            if (attention->reticleFadeAlphaControl < 0) {
                attention->reticleFadeAlphaControl = 0;
            }

            // `reticleFadeAlphaControl` is only used as an alpha when fading out.
            // Otherwise it defaults to 255, set above.
            alpha = attention->reticleFadeAlphaControl;
        }

        Actor_GetProjectedPos(play, &attention->reticlePos, &projectedPos, &invW);

        projectedPos.x = ((SCREEN_WIDTH / 2) * (projectedPos.x * invW)) * projectedPosScale;
        projectedPos.x = CLAMP(projectedPos.x, -SCREEN_WIDTH, SCREEN_WIDTH);

        projectedPos.y = ((SCREEN_HEIGHT / 2) * (projectedPos.y * invW)) * projectedPosScale;
        projectedPos.y = CLAMP(projectedPos.y, -SCREEN_HEIGHT, SCREEN_HEIGHT);

        projectedPos.z *= projectedPosScale;

        attention->curReticle--;

        if (attention->curReticle < 0) {
            attention->curReticle = ARRAY_COUNT(attention->lockOnReticles) - 1;
        }

        Attention_SetReticlePos(attention, attention->curReticle, projectedPos.x, projectedPos.y, projectedPos.z);

        if (!(player->stateFlags1 & PLAYER_STATE1_TALKING) || (actor != player->focusActor)) {
            OVERLAY_DISP = Gfx_SetupDL(OVERLAY_DISP, SETUPDL_57);

            for (i = 0, curReticle = attention->curReticle; i < numReticles;
                 i++, curReticle = (curReticle + 1) % ARRAY_COUNT(attention->lockOnReticles)) {
                reticle = &attention->lockOnReticles[curReticle];

                if (reticle->radius < 500.0f) {
                    s32 triangleIndex;

                    if (reticle->radius <= 120.0f) {
                        lockOnScaleX = 0.15f;
                    } else {
                        lockOnScaleX = ((reticle->radius - 120.0f) * 0.001f) + 0.15f;
                    }

                    f32 reticleScale = CVarGetFloat(CVAR_COSMETIC("Silly.LockOnReticleScale"), 1.0f);
                    f32 reticleSpin = CVarGetFloat(CVAR_COSMETIC("Silly.LockOnReticleSpin"), 1.0f);

                    Matrix_Translate(reticle->pos.x, reticle->pos.y, 0.0f, MTXMODE_NEW);
                    Matrix_Scale(lockOnScaleX * reticleScale, 0.15f * reticleScale, 1.0f, MTXMODE_APPLY);

                    gDPSetPrimColorOverride(OVERLAY_DISP++, 0, 0, reticle->color.r, reticle->color.g, reticle->color.b,
                                            (u8)alpha, COSMETIC_ID("HUD.TargetReticle"));

                    Matrix_RotateZS((s16)(attention->reticleSpinCounter * 0x200 * reticleSpin), MTXMODE_APPLY);

                    // Draw the 4 triangles that make up the reticle
                    for (triangleIndex = 0; triangleIndex < 4; triangleIndex++) {
                        Matrix_RotateZS(0x10000 / 4, MTXMODE_APPLY);
                        Matrix_Push();
                        Matrix_Translate(reticle->radius, reticle->radius, 0.0f, MTXMODE_APPLY);
                        MATRIX_FINALIZE_AND_LOAD(OVERLAY_DISP++, play->state.gfxCtx);
                        gSPDisplayList(OVERLAY_DISP++, gLockOnReticleTriangleDL);
                        Matrix_Pop();
                    }
                }

                alpha -= 255 / ARRAY_COUNT(attention->lockOnReticles);

                if (alpha < 0) {
                    alpha = 0;
                }
            }
        }
        FrameInterpolation_RecordCloseChild();
    }

    actor = attention->arrowHoverActor;

    if ((actor != NULL) && !(actor->flags & ACTOR_FLAG_LOCK_ON_DISABLED)) {
        AttentionColor* attentionColor = &sAttentionColors[actor->category];
        FrameInterpolation_RecordOpenChild(actor, 0);

        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_7);

        Matrix_Translate(actor->focus.pos.x, actor->focus.pos.y + (actor->lockOnArrowOffset * actor->scale.y) + 17.0f,
                         actor->focus.pos.z, MTXMODE_NEW);
        Matrix_RotateYS(play->gameplayFrames * 0xBB8, MTXMODE_APPLY);
        Matrix_Scale((iREG(27) + 35) / 1000.0f, (iREG(28) + 60) / 1000.0f, (iREG(29) + 50) / 1000.0f, MTXMODE_APPLY);
        if (GameInteractor_Should(VB_DRAW_LOCK_ON_ARROW, true, actor)) {
            gDPSetPrimColorOverride(POLY_XLU_DISP++, 0, 0, attentionColor->primary.r, attentionColor->primary.g,
                                    attentionColor->primary.b, 255, COSMETIC_ID("HUD.TargetArrow"));
        }
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gLockOnArrowDL);
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Attention_Update(Attention* attention, Player* player, Actor* playerFocusActor, PlayState* play) {
    s32 pad;
    Actor* actor; // used for both the Tatl hover actor and reticle actor
    s32 category;
    Vec3f projectedPos;
    f32 invW;

    actor = NULL;

    if ((player->focusActor != NULL) &&
        (player->controlStickDirections[player->controlStickDataIndex] == PLAYER_STICK_DIR_BACKWARD)) {
        // Holding backward on the control stick prevents an arrow appearing over the next lock-on actor.
        // This helps escape a lock-on loop when using Switch Targeting, but note that this still works for
        // Hold Targeting as well.
        attention->arrowHoverActor = NULL;
    } else {
        // Find the next attention actor so Tatl and an arrow can hover over it (if applicable)
        Attention_FindActor(play, &play->actorCtx, &actor, &gCameraDriftActor, player);
        attention->arrowHoverActor = actor;
    }

    if (attention->forcedLockOnActor != NULL) {
        // This lock-on actor takes precedence over anything else
        // (this feature is never used in practice)
        actor = attention->forcedLockOnActor;
        attention->forcedLockOnActor = NULL;
    } else if (playerFocusActor != NULL) {
        // Stay locked-on to the same actor, if there is one.
        // This also makes Tatl fly over to the current focus actor, if there is one.
        actor = playerFocusActor;
    }

    if (actor != NULL) {
        category = actor->category;
    } else {
        category = player->actor.category;
    }

    if ((actor != attention->tatlHoverActor) || (category != attention->tatlHoverActorCategory)) {
        // Set Tatl to hover over a new actor
        attention->tatlHoverActor = actor;
        attention->tatlHoverActorCategory = category;
        attention->tatlMoveProgressFactor = 1.0f;
    }

    if (actor == NULL) {
        // Setting the actor to Player will make Tatl return to him
        actor = &player->actor;
    }

    if (!Math_StepToF(&attention->tatlMoveProgressFactor, 0.0f, 0.25f)) {
        f32 moveScale = 0.25f / attention->tatlMoveProgressFactor;
        f32 x = actor->focus.pos.x - attention->tatlHoverPos.x;
        f32 y = (actor->focus.pos.y + (actor->lockOnArrowOffset * actor->scale.y)) - attention->tatlHoverPos.y;
        f32 z = actor->focus.pos.z - attention->tatlHoverPos.z;

        attention->tatlHoverPos.x += x * moveScale;
        attention->tatlHoverPos.y += y * moveScale;
        attention->tatlHoverPos.z += z * moveScale;
    } else {
        // Set Tatl pos and color after reaching destination
        Attention_SetTatlState(attention, actor, category, play);
    }

    if ((playerFocusActor != NULL) && (attention->reticleSpinCounter == 0)) {
        Actor_GetProjectedPos(play, &playerFocusActor->focus.pos, &projectedPos, &invW);

        if ((projectedPos.z <= 0.0f) || (fabsf(projectedPos.x * invW) >= 1.0f) ||
            (fabsf(projectedPos.y * invW) >= 1.0f)) {
            // Release the reticle if the actor is off screen.
            // It is possible to move far enough away from an actor that it goes off screen, despite being
            // locked onto it. In this case the reticle will release, but the lock-on will remain
            // because Player is still updating focusActor.
            // It is unclear if this is intentional, or if it is a bug and the lock-on as a whole is supposed
            // to release.
            playerFocusActor = NULL;
        }
    }

    if (playerFocusActor != NULL) {
        if (playerFocusActor != attention->reticleActor) {
            s32 lockOnSfxId;

            // Set up a new reticle
            Attention_InitReticle(attention, playerFocusActor->category, play);
            attention->reticleActor = playerFocusActor;

            if (playerFocusActor->id == ACTOR_EN_BOOM) {
                // Don't draw the reticle when locked onto the zora fin boomerang.
                // Note that it isn't possible to lock onto the boomerang, so this code doesn't do anything.
                // This implies that the boomerang camera lock may have been implemented with Z-Targeting at one point,
                // but was eventually implemented as its own camera mode instead.
                attention->reticleFadeAlphaControl = 0;
            }

            lockOnSfxId = CHECK_FLAG_ALL(playerFocusActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)
                              ? NA_SE_SY_LOCK_ON
                              : NA_SE_SY_LOCK_ON_HUMAN;
            Audio_PlaySfx(lockOnSfxId);
        }

        // Update reticle

        attention->reticlePos.x = playerFocusActor->world.pos.x;
        attention->reticlePos.y =
            playerFocusActor->world.pos.y - (playerFocusActor->shape.yOffset * playerFocusActor->scale.y);
        attention->reticlePos.z = playerFocusActor->world.pos.z;

        if (attention->reticleSpinCounter == 0) {
            f32 step = (500.0f - attention->reticleRadius) * 3.0f;
            f32 reticleZoomStep = CLAMP(step, 30.0f, 100.0f);

            if (Math_StepToF(&attention->reticleRadius, 80.0f, reticleZoomStep)) {
                // Non-zero counter indicates the reticle is done zooming in
                attention->reticleSpinCounter++;
            }
        } else {
            // Finished zooming in, spin the reticle around the lock-on actor

            // 0x80 is or'd to avoid a value of zero.
            // This rotation value gets multiplied by 0x200, which multiplied by 0x80 gives a full turn (0x10000)
            attention->reticleSpinCounter = (attention->reticleSpinCounter + 3) | 0x80;
            attention->reticleRadius = 120.0f;
        }
    } else {
        // Expand the radius quickly as the reticle is released
        attention->reticleActor = NULL;
        Math_StepToF(&attention->reticleRadius, 500.0f, 80.0f);
    }
}

/* Start of Flags section */

/**
 * Tests if a current scene switch flag is set.
 */
s32 Flags_GetSwitch(PlayState* play, s32 flag) {
    if ((flag > SWITCH_FLAG_NONE) && (flag < 0x80)) {
        return play->actorCtx.sceneFlags.switches[(flag & ~0x1F) >> 5] & (1 << (flag & 0x1F));
    }
    return 0;
}

/**
 * Sets a current scene switch flag.
 */
void Flags_SetSwitch(PlayState* play, s32 flag) {
    if ((flag > SWITCH_FLAG_NONE) && (flag < 0x80)) {
        u8 previouslyOff = !Flags_GetSwitch(play, flag);
        play->actorCtx.sceneFlags.switches[(flag & ~0x1F) >> 5] |= 1 << (flag & 0x1F);
        if (previouslyOff) {
            GameInteractor_ExecuteOnSceneFlagSet(play->sceneId, FLAG_CYCL_SCENE_SWITCH, flag);
        }
    }
}

/**
 * Unsets a current scene switch flag.
 */
void Flags_UnsetSwitch(PlayState* play, s32 flag) {
    if ((flag > SWITCH_FLAG_NONE) && (flag < 0x80)) {
        u8 previouslyOn = Flags_GetSwitch(play, flag);
        play->actorCtx.sceneFlags.switches[(flag & ~0x1F) >> 5] &= ~(1 << (flag & 0x1F));
        if (previouslyOn) {
            GameInteractor_ExecuteOnSceneFlagUnset(play->sceneId, FLAG_CYCL_SCENE_SWITCH, flag);
        }
    }
}

/**
 * Tests if a current scene chest flag is set.
 */
s32 Flags_GetTreasure(PlayState* play, s32 flag) {
    return play->actorCtx.sceneFlags.chest & (1 << flag);
}

/**
 * Sets a current scene chest flag.
 */
void Flags_SetTreasure(PlayState* play, s32 flag) {
    u8 previouslyOff = !Flags_GetTreasure(play, flag);
    play->actorCtx.sceneFlags.chest |= (1 << flag);
    if (previouslyOff) {
        GameInteractor_ExecuteOnSceneFlagSet(play->sceneId, FLAG_CYCL_SCENE_CHEST, flag);
    }
}

/**
 * Overrides all the current scene chest flags.
 */
void Flags_SetAllTreasure(PlayState* play, s32 flag) {
    play->actorCtx.sceneFlags.chest = flag;
}

/**
 * Returns all the current scene chest flags.
 */
s32 Flags_GetAllTreasure(PlayState* play) {
    return play->actorCtx.sceneFlags.chest;
}

/**
 * Tests if a current scene clear flag is set.
 */
s32 Flags_GetClear(PlayState* play, s32 roomNumber) {
    return play->actorCtx.sceneFlags.clearedRoom & (1 << roomNumber);
}

/**
 * Sets a current scene clear flag.
 */
void Flags_SetClear(PlayState* play, s32 roomNumber) {
    u8 previouslyOff = !Flags_GetClear(play, roomNumber);
    play->actorCtx.sceneFlags.clearedRoom |= (1 << roomNumber);
    if (previouslyOff) {
        GameInteractor_ExecuteOnSceneFlagSet(play->sceneId, FLAG_CYCL_SCENE_CLEARED_ROOM, roomNumber);
    }
}

/**
 * Unsets a current scene clear flag.
 */
void Flags_UnsetClear(PlayState* play, s32 roomNumber) {
    u8 previouslyOn = Flags_GetClear(play, roomNumber);
    play->actorCtx.sceneFlags.clearedRoom &= ~(1 << roomNumber);
    if (previouslyOn) {
        GameInteractor_ExecuteOnSceneFlagUnset(play->sceneId, FLAG_CYCL_SCENE_CLEARED_ROOM, roomNumber);
    }
}

/**
 * Tests if a current scene temp clear flag is set.
 */
s32 Flags_GetClearTemp(PlayState* play, s32 roomNumber) {
    return play->actorCtx.sceneFlags.clearedRoomTemp & (1 << roomNumber);
}

/**
 * Sets a current scene temp clear flag.
 */
void Flags_SetClearTemp(PlayState* play, s32 roomNumber) {
    play->actorCtx.sceneFlags.clearedRoomTemp |= (1 << roomNumber);
}

/**
 * Unsets a current scene temp clear flag.
 */
void Flags_UnsetClearTemp(PlayState* play, s32 roomNumber) {
    play->actorCtx.sceneFlags.clearedRoomTemp &= ~(1 << roomNumber);
}

/**
 * Tests if a current scene collectible flag is set.
 */
s32 Flags_GetCollectible(PlayState* play, s32 flag) {
    if ((flag > 0) && (flag < 0x80)) {
        return play->actorCtx.sceneFlags.collectible[(flag & ~0x1F) >> 5] & (1 << (flag & 0x1F));
    }
    return 0;
}

/**
 * Sets a current scene collectible flag.
 */
void Flags_SetCollectible(PlayState* play, s32 flag) {
    if ((flag > 0) && (flag < 0x80)) {
        u8 previouslyOff = !Flags_GetCollectible(play, flag);
        play->actorCtx.sceneFlags.collectible[(flag & ~0x1F) >> 5] |= 1 << (flag & 0x1F);
        if (previouslyOff) {
            GameInteractor_ExecuteOnSceneFlagSet(play->sceneId, FLAG_CYCL_SCENE_COLLECTIBLE, flag);
        }
    }
}

// #region 2S2H Originally these flags were all set with macros, for the port we want them to be in functions so we can
// hook into them
void Flags_SetWeekEventReg(s32 flag) {
    u8 previouslyOff = !CHECK_WEEKEVENTREG(flag);
    WEEKEVENTREG((flag) >> 8) = GET_WEEKEVENTREG((flag) >> 8) | ((flag)&0xFF);
    if (previouslyOff) {
        GameInteractor_ExecuteOnFlagSet(FLAG_WEEK_EVENT_REG, flag);
    }
}

void Flags_ClearWeekEventReg(s32 flag) {
    u8 previouslyOn = CHECK_WEEKEVENTREG(flag);
    WEEKEVENTREG((flag) >> 8) = GET_WEEKEVENTREG((flag) >> 8) & (u8) ~((flag)&0xFF);
    if (previouslyOn) {
        GameInteractor_ExecuteOnFlagUnset(FLAG_WEEK_EVENT_REG, flag);
    }
}

void Flags_SetWeekEventRegHorseRace(u8 state) {
    u8 previousState = GET_WEEKEVENTREG_HORSE_RACE_STATE;
    WEEKEVENTREG(92) &= (u8)~WEEKEVENTREG_HORSE_RACE_STATE_MASK;
    WEEKEVENTREG(92) = WEEKEVENTREG(92) | (u8)((WEEKEVENTREG(92) & ~WEEKEVENTREG_HORSE_RACE_STATE_MASK) | (state));
    if (previousState != state) {
        GameInteractor_ExecuteOnFlagSet(FLAG_WEEK_EVENT_REG_HORSE_RACE, state);
    }
}

void Flags_SetEventInf(s32 flag) {
    u8 previouslyOff = !CHECK_EVENTINF(flag);
    gSaveContext.eventInf[(flag) >> 4] |= (1 << ((flag)&0xF));
    if (previouslyOff) {
        GameInteractor_ExecuteOnFlagSet(FLAG_EVENT_INF, flag);
    }
}

void Flags_ClearEventInf(s32 flag) {
    u8 previouslyOn = CHECK_EVENTINF(flag);
    gSaveContext.eventInf[(flag) >> 4] &= (u8) ~(1 << ((flag)&0xF));
    if (previouslyOn) {
        GameInteractor_ExecuteOnFlagUnset(FLAG_EVENT_INF, flag);
    }
}
// #endregion

// #region 2S2H Our rando_inf flags
s32 Flags_GetRandoInf(s32 flag) {
    return gSaveContext.save.shipSaveInfo.rando.randoInf[(flag) >> 4] & (1 << ((flag)&0xF));
}

void Flags_SetRandoInf(s32 flag) {
    u8 previouslyOff = !Flags_GetRandoInf(flag);
    gSaveContext.save.shipSaveInfo.rando.randoInf[flag >> 4] |= (1 << (flag & 0xF));
    if (previouslyOff) {
        GameInteractor_ExecuteOnFlagSet(FLAG_RANDO_INF, flag);
    }
}

void Flags_ClearRandoInf(s32 flag) {
    u8 previouslyOn = Flags_GetRandoInf(flag);
    gSaveContext.save.shipSaveInfo.rando.randoInf[flag >> 4] &= ~(1 << (flag & 0xF));
    if (previouslyOn) {
        GameInteractor_ExecuteOnFlagUnset(FLAG_RANDO_INF, flag);
    }
}
// #endregion

/* End of Flags section */

/* Start of TitleCard section */

void TitleCard_ContextInit(GameState* gameState, TitleCardContext* titleCtx) {
    titleCtx->durationTimer = 0;
    titleCtx->delayTimer = 0;
    titleCtx->intensity = 0;
    titleCtx->alpha = 0;
}

void TitleCard_InitBossName(GameState* gameState, TitleCardContext* titleCtx, TexturePtr texture, s16 x, s16 y,
                            u8 width, u8 height) {
    titleCtx->texture = texture;
    titleCtx->x = x;
    titleCtx->y = y;
    titleCtx->width = width;
    titleCtx->height = height;
    titleCtx->durationTimer = 80;
    titleCtx->delayTimer = 0;
}

void TitleCard_InitPlaceName(GameState* gameState, TitleCardContext* titleCtx, TexturePtr texture, s32 x, s32 y,
                             s32 width, s32 height, s32 delay) {
}

void TitleCard_Update(GameState* gameState, TitleCardContext* titleCtx) {
    if (DECR(titleCtx->delayTimer) == 0) {
        if (DECR(titleCtx->durationTimer) == 0) {
            Math_StepToS(&titleCtx->alpha, 0, 30);
            Math_StepToS(&titleCtx->intensity, 0, 70);
        } else {
            Math_StepToS(&titleCtx->alpha, 255, 10);
            Math_StepToS(&titleCtx->intensity, 255, 20);
        }
    }
}

void TitleCard_Draw(GameState* gameState, TitleCardContext* titleCtx) {
    if (titleCtx->alpha != 0) {
        s32 width = titleCtx->width;
        s32 height = titleCtx->height;
        s32 doubleWidth = width * 2;
        s32 titleX = (titleCtx->x * 4) - doubleWidth;
        s32 doubleHeight = height * 2;
        s32 titleY = (titleCtx->y * 4) - doubleHeight;
        s32 titleSecondY;
        s32 textureLanguageOffset;

        OPEN_DISPS(gameState->gfxCtx);

        if (width * height > TMEM_SIZE) {
            // 2S2H [HD Textures] Commenting out the below so that we can render the full texture in one rectangle,
            // as we are not restricted to the console TMEM limit
            // height = TMEM_SIZE / width;
        }

        titleSecondY = titleY + (height * 4);

        OVERLAY_DISP = Gfx_SetupDL52_NoCD(OVERLAY_DISP);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, (u8)titleCtx->intensity, (u8)titleCtx->intensity, (u8)titleCtx->intensity,
                        (u8)titleCtx->alpha);

        gDPLoadTextureBlock(OVERLAY_DISP++, titleCtx->texture, G_IM_FMT_IA, G_IM_SIZ_8b, width, height, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        gSPTextureRectangle(OVERLAY_DISP++, titleX, titleY, ((doubleWidth * 2) + titleX) - 4, titleY + (height * 4) - 1,
                            G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

        height = titleCtx->height - height;

        // If texture is bigger than 0x1000, display the rest
        if (height > 0) {
            gDPLoadTextureBlock(OVERLAY_DISP++, (uintptr_t)titleCtx->texture + 0x1000, G_IM_FMT_IA, G_IM_SIZ_8b, width,
                                height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSPTextureRectangle(OVERLAY_DISP++, titleX, titleSecondY, ((doubleWidth * 2) + titleX) - 4,
                                titleSecondY + (height * 4) - 1, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
        }

        CLOSE_DISPS(gameState->gfxCtx);
    }
}

/* End of TitleCard section */

// unused
s32 func_800B6434(PlayState* play, TitleCardContext* titleCtx) {
    if ((play->actorCtx.titleCtx.delayTimer != 0) || (play->actorCtx.titleCtx.alpha != 0)) {
        titleCtx->durationTimer = 0;
        titleCtx->delayTimer = 0;
        return false;
    }
    return true;
}

void Actor_InitPlayerImpact(PlayState* play) {
    play->actorCtx.playerImpact.timer = 0;
}

void Actor_UpdatePlayerImpact(PlayState* play) {
    DECR(play->actorCtx.playerImpact.timer);
}

s32 Actor_SetPlayerImpact(PlayState* play, PlayerImpactType type, s32 timer, f32 dist, Vec3f* pos) {
    if ((play->actorCtx.playerImpact.timer != 0) && (dist < play->actorCtx.playerImpact.dist)) {
        return false;
    }

    play->actorCtx.playerImpact.type = type;
    play->actorCtx.playerImpact.timer = timer;
    play->actorCtx.playerImpact.dist = dist;
    Math_Vec3f_Copy(&play->actorCtx.playerImpact.pos, pos);

    return true;
}

f32 Actor_GetPlayerImpact(PlayState* play, f32 range, Vec3f* pos, PlayerImpactType* type) {
    f32 dist;

    if ((play->actorCtx.playerImpact.timer == 0) || (range == 0.0f)) {
        return -1.0f;
    }

    dist = Math_Vec3f_DistXYZ(&play->actorCtx.playerImpact.pos, pos) / range;
    *type = play->actorCtx.playerImpact.type;

    return play->actorCtx.playerImpact.dist - dist;
}

/**
 * Initializes an element of the `play->actorCtx.actorSharedMemory` array to the `ptr` pointer, or allocates one using
 * the `size` argument in case `ptr` is NULL. This element is associated to an `id`.
 *
 * This allows allows different actors the ability to access the varible, and thus communicate with each other by
 * reading/setting the value.
 *
 * In success: returns the allocated pointer if `ptr` was NULL or the `ptr` pointer otherwise.
 * In failure (There's no space left in `play->actorCtx.actorSharedMemory` or an allocation error happened): returns
 * NULL.
 *
 * Note there are no duplicated id checks.
 *
 * Used only by EnLiftNuts.
 */
void* Actor_AddSharedMemoryEntry(PlayState* play, s16 id, void* ptr, size_t size) {
    ActorSharedMemoryEntry* entry = play->actorCtx.actorSharedMemory;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(play->actorCtx.actorSharedMemory); i++) {
        if (entry->id == 0) {
            if (ptr == NULL) {
                ptr = ZeldaArena_Malloc(size);
                if (ptr == NULL) {
                    return NULL;
                }
                entry->isDynamicallyInitialised = true;
            }

            entry->id = id;
            entry->ptr = ptr;
            return ptr;
        }

        entry++;
    }

    return NULL;
}

/**
 * Frees the first element of `play->actorCtx.actorSharedMemory` with id `id`.
 *
 * If success, the free'd pointer is returned.
 * If failure, NULL is returned.
 *
 * Used only by EnLiftNuts.
 */
void* Actor_FreeSharedMemoryEntry(PlayState* play, s16 id) {
    ActorSharedMemoryEntry* entry = play->actorCtx.actorSharedMemory;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(play->actorCtx.actorSharedMemory); i++) {
        if (id == entry->id) {
            entry->id = 0;
            if (entry->isDynamicallyInitialised) {
                ZeldaArena_Free(entry->ptr);
                entry->isDynamicallyInitialised = false;
            }
            return entry->ptr;
        }

        entry++;
    }

    return NULL;
}

/**
 * Retrieves the first pointer stored with the id `id` from `play->actorCtx.actorSharedMemory`.
 * If there's no pointer stored with that id, NULL is returned.
 *
 * Used only by EnGamelupy.
 */
void* Actor_FindSharedMemoryEntry(PlayState* play, s16 id) {
    ActorSharedMemoryEntry* entry = play->actorCtx.actorSharedMemory;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(play->actorCtx.actorSharedMemory); i++) {
        if (id == entry->id) {
            return entry->ptr;
        }

        entry++;
    }

    return NULL;
}

void Actor_Kill(Actor* actor) {
    GameInteractor_ExecuteOnActorKill(actor);
    actor->draw = NULL;
    actor->update = NULL;
    actor->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void Actor_SetWorldToHome(Actor* actor) {
    actor->world = actor->home;
}

void Actor_SetFocus(Actor* actor, f32 height) {
    actor->focus.pos.x = actor->world.pos.x;
    actor->focus.pos.y = actor->world.pos.y + height;
    actor->focus.pos.z = actor->world.pos.z;

    actor->focus.rot.x = actor->world.rot.x;
    actor->focus.rot.y = actor->world.rot.y;
    actor->focus.rot.z = actor->world.rot.z;
}

void Actor_SetWorldRotToShape(Actor* actor) {
    actor->world.rot = actor->shape.rot;
}

void Actor_SetShapeRotToWorld(Actor* actor) {
    actor->shape.rot = actor->world.rot;
}

void Actor_SetScale(Actor* actor, f32 scale) {
    actor->scale.z = scale;
    actor->scale.y = scale;
    actor->scale.x = scale;
}

void Actor_SetObjectDependency(PlayState* play, Actor* actor) {
    gSegments[0x06] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[actor->objectSlot].segment);
}

void Actor_Init(Actor* actor, PlayState* play) {
    Actor_SetWorldToHome(actor);
    Actor_SetShapeRotToWorld(actor);
    Actor_SetFocus(actor, 0.0f);
    Math_Vec3f_Copy(&actor->prevPos, &actor->world.pos);
    Actor_SetScale(actor, 0.01f);
    actor->attentionRangeType = ATTENTION_RANGE_3;
    actor->terminalVelocity = -20.0f;

    actor->xyzDistToPlayerSq = FLT_MAX;
    actor->cullingVolumeDistance = 1000.0f;
    actor->cullingVolumeScale = 350.0f;
    actor->cullingVolumeDownward = 700.0f;

    actor->hintId = TATL_HINT_ID_NONE;

    CollisionCheck_InitInfo(&actor->colChkInfo);
    actor->floorBgId = BGCHECK_SCENE;

    ActorShape_Init(&actor->shape, 0.0f, NULL, 0.0f);
    if (Object_IsLoaded(&play->objectCtx, actor->objectSlot)) {
        Actor_SetObjectDependency(play, actor);

        if (GameInteractor_ShouldActorInit(actor)) {
            actor->init(actor, play);
            actor->init = NULL;
            GameInteractor_ExecuteOnActorInit(actor);
        } else {
            actor->init = NULL;
            Actor_Kill(actor);
        }
    }
}

void Actor_Destroy(Actor* actor, PlayState* play) {
    if (actor->init == NULL) {
        if (actor->destroy != NULL) {
            actor->destroy(actor, play);
            actor->destroy = NULL;
        }
    }
}

f32 sActorMovementScale = 1.0f;

void Actor_SetMovementScale(s32 scale) {
    sActorMovementScale = scale * 0.5f;
}

/**
 * Update actor position using velocity and any push from z_collision_check.
 */
void Actor_UpdatePos(Actor* actor) {
    f32 speedRate = sActorMovementScale;

    actor->world.pos.x += (actor->velocity.x * speedRate) + actor->colChkInfo.displacement.x;
    actor->world.pos.y += (actor->velocity.y * speedRate) + actor->colChkInfo.displacement.y;
    actor->world.pos.z += (actor->velocity.z * speedRate) + actor->colChkInfo.displacement.z;
}

/**
 * Updates actor's velocity accounting for gravity (without exceeding terminal velocity)
 * The operation is performed in cylindrical coordinates
 *
 * It is recommended to not call this function directly and use `Actor_MoveWithGravity` instead
 */
void Actor_UpdateVelocityWithGravity(Actor* actor) {
    actor->velocity.x = actor->speed * Math_SinS(actor->world.rot.y);
    actor->velocity.z = actor->speed * Math_CosS(actor->world.rot.y);

    actor->velocity.y += actor->gravity;
    if (actor->velocity.y < actor->terminalVelocity) {
        actor->velocity.y = actor->terminalVelocity;
    }
}

/**
 * Moves actor accounting for its current velocity and applying gravity
 * The operation is performed in cylindrical coordinates
 */
void Actor_MoveWithGravity(Actor* actor) {
    Actor_UpdateVelocityWithGravity(actor);
    Actor_UpdatePos(actor);
}

/**
 * Updates actor's velocity, ignoring gravity
 * The operation is performed in spherical coordinates
 *
 * It is recommended to not call this function directly and use `Actor_MoveWithoutGravity` instead
 */
void Actor_UpdateVelocityWithoutGravity(Actor* actor) {
    f32 speedXZ = Math_CosS(actor->world.rot.x) * actor->speed;

    actor->velocity.x = Math_SinS(actor->world.rot.y) * speedXZ;
    actor->velocity.y = Math_SinS(actor->world.rot.x) * actor->speed;
    actor->velocity.z = Math_CosS(actor->world.rot.y) * speedXZ;
}

/**
 * Moves actor accounting for its current velocity, without applying gravity
 * The operation is performed in spherical coordinates
 *
 * Useful for flying or swimming actors
 */
void Actor_MoveWithoutGravity(Actor* actor) {
    Actor_UpdateVelocityWithoutGravity(actor);
    Actor_UpdatePos(actor);
}

/**
 * Like `Actor_UpdateVelocityWithoutGravity`, but the actor is moved backwards instead of forwards
 *
 * It is recommended to not call this function directly and use `Actor_MoveWithoutGravityReverse` instead
 */
void Actor_UpdateVelocityWithoutGravityReverse(Actor* actor) {
    f32 speedXZ = Math_CosS(-actor->world.rot.x) * actor->speed;

    actor->velocity.x = Math_SinS(actor->world.rot.y) * speedXZ;
    actor->velocity.y = Math_SinS(-actor->world.rot.x) * actor->speed;
    actor->velocity.z = Math_CosS(actor->world.rot.y) * speedXZ;
}

/**
 * Like `Actor_MoveWithoutGravity`, but the actor is moved backwards instead of forwards
 */
void Actor_MoveWithoutGravityReverse(Actor* actor) {
    Actor_UpdateVelocityWithoutGravityReverse(actor);
    Actor_UpdatePos(actor);
}

/**
 * Sets horizontal speed and Y velocity using the `speed` argument and current pitch
 */
void Actor_SetSpeeds(Actor* actor, f32 speed) {
    actor->speed = Math_CosS(actor->world.rot.x) * speed;
    actor->velocity.y = -Math_SinS(actor->world.rot.x) * speed;
}

// unused
void Actor_UpdatePosFromSkelAnime(Actor* actor, SkelAnime* skelAnime) {
    Vec3f pos;

    SkelAnime_UpdateTranslation(skelAnime, &pos, actor->shape.rot.y);
    actor->world.pos.x += pos.x * actor->scale.x;
    actor->world.pos.y += pos.y * actor->scale.y;
    actor->world.pos.z += pos.z * actor->scale.z;
}

s16 Actor_WorldYawTowardActor(Actor* actorA, Actor* actorB) {
    return Math_Vec3f_Yaw(&actorA->world.pos, &actorB->world.pos);
}

s16 Actor_FocusYawTowardActor(Actor* actorA, Actor* actorB) {
    return Math_Vec3f_Yaw(&actorA->focus.pos, &actorB->focus.pos);
}

s16 Actor_WorldYawTowardPoint(Actor* actor, Vec3f* refPoint) {
    return Math_Vec3f_Yaw(&actor->world.pos, refPoint);
}

s16 Actor_WorldPitchTowardActor(Actor* actorA, Actor* actorB) {
    return Math_Vec3f_Pitch(&actorA->world.pos, &actorB->world.pos);
}

s16 Actor_FocusPitchTowardActor(Actor* actorA, Actor* actorB) {
    return Math_Vec3f_Pitch(&actorA->focus.pos, &actorB->focus.pos);
}

s16 Actor_WorldPitchTowardPoint(Actor* actor, Vec3f* refPoint) {
    return Math_Vec3f_Pitch(&actor->world.pos, refPoint);
}

f32 Actor_WorldDistXYZToActor(Actor* actorA, Actor* actorB) {
    return Math_Vec3f_DistXYZ(&actorA->world.pos, &actorB->world.pos);
}

f32 Actor_WorldDistXYZToPoint(Actor* actor, Vec3f* refPoint) {
    return Math_Vec3f_DistXYZ(&actor->world.pos, refPoint);
}

f32 Actor_WorldDistXZToActor(Actor* actorA, Actor* actorB) {
    return Math_Vec3f_DistXZ(&actorA->world.pos, &actorB->world.pos);
}

f32 Actor_WorldDistXZToPoint(Actor* actor, Vec3f* refPoint) {
    return Math_Vec3f_DistXZ(&actor->world.pos, refPoint);
}

/**
 * Find the offset of a point from an actor in that actor's own coordinates (origin at the actor's
 * world.pos, z-axis is facing angle, i.e. shape.rot.y)
 *
 * @param[in]  actor  The actor whose coordinate system to transform to.
 * @param[out] offset The transformed coordinates.
 * @param[in]  point  The point to transform to actor coordinates.
 */
void Actor_WorldToActorCoords(Actor* actor, Vec3f* offset, Vec3f* point) {
    f32 cos = Math_CosS(actor->shape.rot.y);
    f32 sin = Math_SinS(actor->shape.rot.y);
    f32 diffX;
    f32 diffZ;

    // Shift X,Z to actor coordinates origin
    diffX = point->x - actor->world.pos.x;
    diffZ = point->z - actor->world.pos.z;

    // Rotate X and Z offsets to align Z to actor's shape.rot.y
    offset->x = ((diffX * cos) - (diffZ * sin));
    offset->z = ((diffZ * cos) + (diffX * sin));

    // Shift Y to origin
    offset->y = point->y - actor->world.pos.y;
}

f32 Actor_HeightDiff(Actor* actor1, Actor* actor2) {
    return actor2->world.pos.y - actor1->world.pos.y;
}

/**
 * Calculates and sets the control stick x/y values and writes these to input.
 */
void Actor_SetControlStickData(PlayState* play, Input* input, f32 controlStickMagnitude, s16 controlStickAngle) {
    s16 relativeAngle = controlStickAngle - Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));

    input->cur.stick_x = -Math_SinS(relativeAngle) * controlStickMagnitude;
    // 2S2H [Enhancement] Allow inverting the X axis with GI, primarily for mirror mode,
    // otherwise link moves in the opposite direction and likely get soft locked, and
    // kafei turns the wrong direction as he paths.
    input->cur.stick_x *= GameInteractor_InvertControl(GI_INVERT_MOVEMENT_X);
    input->rel.stick_x = input->cur.stick_x;
    input->cur.stick_y = Math_CosS(relativeAngle) * controlStickMagnitude;
    input->rel.stick_y = input->cur.stick_y;
}

f32 Player_GetHeight(Player* player) {
    f32 extraHeight;

    if (player->stateFlags1 & PLAYER_STATE1_800000) {
        extraHeight = 32.0f;
    } else {
        extraHeight = 0.0f;
    }

    switch (player->transformation) {
        default:
        case PLAYER_FORM_FIERCE_DEITY:
            return extraHeight + 124.0f;

        case PLAYER_FORM_GORON:
            return extraHeight + ((player->stateFlags3 & PLAYER_STATE3_1000) ? 34.0f : 80.0f);

        case PLAYER_FORM_ZORA:
            return extraHeight + 68.0f;

        case PLAYER_FORM_DEKU:
            return extraHeight + 36.0f;

        case PLAYER_FORM_HUMAN:
            return extraHeight + 44.0f;
    }
}

f32 Player_GetRunSpeedLimit(Player* player) {
    if (player->stateFlags1 & PLAYER_STATE1_800000) {
        return 15.0f;
    } else if (player->stateFlags1 & PLAYER_STATE1_8000000) {
        return (R_RUN_SPEED_LIMIT / 100.0f) * 0.6f;
    } else {
        return R_RUN_SPEED_LIMIT / 100.0f;
    }
}

bool func_800B7118(Player* player) {
    return player->stateFlags1 & PLAYER_STATE1_8;
}

bool func_800B7128(Player* player) {
    return func_800B7118(player) && (player->unk_ACC != 0);
}

bool func_800B715C(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->stateFlags2 & PLAYER_STATE2_8;
}

void Player_SetCameraHorseSetting(PlayState* play, Player* player) {
    if ((play->roomCtx.curRoom.type != ROOM_TYPE_4) && (player->actor.id == ACTOR_PLAYER)) {
        EnHorse* rideActor = (EnHorse*)player->rideActor;

        if ((rideActor != NULL) && !(rideActor->unk_1EC & 0x10)) {
            Camera_ChangeSetting(Play_GetCamera(play, CAM_ID_MAIN), CAM_SET_HORSE);
        }
    }
}

void Player_MountHorse(PlayState* play, Player* player, Actor* horse) {
    player->rideActor = horse;
    player->stateFlags1 |= PLAYER_STATE1_800000;
    horse->child = &player->actor;
}

bool func_800B7200(Player* player) {
    return (player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_20000000)) ||
           (player->csAction != PLAYER_CSACTION_NONE);
}

void Player_SpawnHorse(PlayState* play, Player* player) {
    Horse_Spawn(play, player);
}

/**
 * Sets a Player Cutscene Action specified by `csAction`.
 *
 * `haltActorsDuringCsAction` being set to false in this function means that all actors will
 * be able to update while Player is performing the cutscene action.
 *
 * Note: due to how player implements initializing the cutscene action state, `haltActorsDuringCsAction`
 * will only be considered the first time player starts a `csAction`.
 * Player must leave the cutscene action state and enter it again before halting actors can be toggled.
 */
s32 Player_SetCsAction(PlayState* play, Actor* csActor, u8 csAction) {
    if (!GameInteractor_Should(VB_PLAYER_CUTSCENE_ACTION, true, csActor)) {
        return false;
    }

    Player* player = GET_PLAYER(play);

    if ((player->csAction == PLAYER_CSACTION_5) ||
        ((csAction == PLAYER_CSACTION_END) && (player->csAction == PLAYER_CSACTION_NONE))) {
        return false;
    }

    player->csAction = csAction;
    player->csActor = csActor;
    player->cv.haltActorsDuringCsAction = false;
    return true;
}

/**
 * Sets a Player Cutscene Action specified by `csAction`.
 *
 * `haltActorsDuringCsAction` being set to true in this function means that eventually `PLAYER_STATE1_20000000` will be
 * set. This makes it so actors belonging to categories `ACTORCAT_ENEMY` and `ACTORCAT_MISC` will not update while
 * Player is performing the cutscene action.
 *
 * Note: due to how player implements initializing the cutscene action state, `haltActorsDuringCsAction`
 * will only be considered the first time player starts a `csAction`.
 * Player must leave the cutscene action state and enter it again before halting actors can be toggled.
 */
s32 Player_SetCsActionWithHaltedActors(PlayState* play, Actor* csActor, u8 csAction) {
    Player* player = GET_PLAYER(play);

    if (Player_SetCsAction(play, csActor, csAction)) {
        player->cv.haltActorsDuringCsAction = true;
        return true;
    }
    return false;
}

// Unused
void func_800B72E0(DynaPolyActor* dyna) {
    dyna->unk14C = 0.0f;
    dyna->pushForce = 0.0f;
}

void func_800B72F8(DynaPolyActor* dyna, f32 extraPushForce, s16 yRotation) {
    dyna->yRotation = yRotation;
    dyna->pushForce += extraPushForce;
}

/**
 * Check if the player is facing the specified actor.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngleDiff`.
 */
s32 Player_IsFacingActor(Actor* actor, s16 maxAngleDiff, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff = BINANG_ADD(actor->yawTowardsPlayer, 0x8000) - player->actor.shape.rot.y;

    if (ABS_ALT(yawDiff) < maxAngleDiff) {
        return true;
    }
    return false;
}

/**
 * Check if `actorB` is facing `actorA`.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 *
 * This function is unused in the original game.
 */
s32 Actor_ActorBIsFacingActorA(Actor* actorA, Actor* actorB, s16 maxAngleDiff) {
    s16 angle = BINANG_ROT180(Actor_WorldYawTowardActor(actorA, actorB));
    s16 dist = angle - actorB->shape.rot.y;

    if (ABS_ALT(dist) < maxAngleDiff) {
        return true;
    }

    return false;
}

/**
 * Check if the specified actor is facing the player.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngleDiff`.
 */
s32 Actor_IsFacingPlayer(Actor* actor, s16 angle) {
    s16 dist = actor->yawTowardsPlayer - actor->shape.rot.y;

    if (ABS_ALT(dist) < angle) {
        return true;
    }
    return false;
}

/**
 * Check if `actorA` is facing `actorB`.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngleDiff`.
 */
s32 Actor_ActorAIsFacingActorB(Actor* actorA, Actor* actorB, s16 maxAngleDiff) {
    s16 dist = Actor_WorldYawTowardActor(actorA, actorB) - actorA->shape.rot.y;

    if (ABS_ALT(dist) < maxAngleDiff) {
        return true;
    }
    return false;
}

/**
 * Check if the specified actor is facing the player and is nearby.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngleDiff`.
 * The maximum distance that qualifies as "nearby" is specified by `range`.
 */
s32 Actor_IsFacingAndNearPlayer(Actor* actor, f32 range, s16 maxAngleDiff) {
    s16 yaw = actor->yawTowardsPlayer - actor->shape.rot.y;

    if (ABS_ALT(yaw) < maxAngleDiff) {
        s16 pad;

        if (sqrtf(SQ(actor->xzDistToPlayer) + SQ(actor->playerHeightRel)) < range) {
            return true;
        }
    }

    return false;
}

/**
 * Check if `actorA` is facing `actorB` and is nearby.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngleDiff`.
 * The maximum distance that qualifies as "nearby" is specified by `range`.
 */
s32 Actor_ActorAIsFacingAndNearActorB(Actor* actorA, Actor* actorB, f32 range, s16 maxAngleDiff) {
    if (Actor_WorldDistXYZToActor(actorA, actorB) < range) {
        s16 dist = Actor_WorldYawTowardActor(actorA, actorB) - actorA->shape.rot.y;

        if (ABS_ALT(dist) < maxAngleDiff) {
            return true;
        }
    }
    return false;
}

/* Start of BgCheck related section */

void Actor_GetSlopeDirection(CollisionPoly* floorPoly, Vec3f* slopeNormal, s16* downwardSlopeYaw) {
    slopeNormal->x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
    slopeNormal->y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
    slopeNormal->z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
    *downwardSlopeYaw = Math_Atan2S_XY(slopeNormal->z, slopeNormal->x);
}

s32 func_800B761C(Actor* actor, f32 arg1, s32 updBgCheckInfoFlags) {
    if (actor->bgCheckFlags & BGCHECKFLAG_GROUND) {
        actor->bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        actor->bgCheckFlags |= BGCHECKFLAG_GROUND_LEAVE;

        if ((actor->velocity.y < 0.0f) && (updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_10)) {
            actor->velocity.y = 0.0f;
        }
        return false;
    }

    return true;
}

s32 func_800B7678(PlayState* play, Actor* actor, Vec3f* pos, s32 updBgCheckInfoFlags) {
    f32 distToFloor;
    s32 bgId;

    pos->y += (updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_800) ? 10.0f : 50.0f;

    actor->floorHeight = BgCheck_EntityRaycastFloor5_2(play, &play->colCtx, &actor->floorPoly, &bgId, actor, pos);
    actor->bgCheckFlags &= ~(BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_GROUND_LEAVE | BGCHECKFLAG_GROUND_STRICT);
    if (actor->floorHeight <= BGCHECK_Y_MIN) {
        return func_800B761C(actor, BGCHECK_Y_MIN, updBgCheckInfoFlags);
    }

    distToFloor = actor->floorHeight - actor->world.pos.y;
    actor->floorBgId = bgId;
    if ((distToFloor >= 0.0f) ||
        (((actor->bgCheckFlags & BGCHECKFLAG_GROUND)) && !(actor->bgCheckFlags & BGCHECKFLAG_PLAYER_800) &&
         (distToFloor >= -11.0f) && (actor->velocity.y < 0.0f))) {
        actor->bgCheckFlags |= BGCHECKFLAG_GROUND_STRICT;

        if (actor->bgCheckFlags & BGCHECKFLAG_CEILING) {
            if (bgId != D_801ED8B4) {
                if (distToFloor > 15.0f) {
                    actor->bgCheckFlags |= BGCHECKFLAG_CRUSHED;
                }
            } else {
                actor->world.pos.x = actor->prevPos.x;
                actor->world.pos.z = actor->prevPos.z;
            }
        }

        actor->world.pos.y = actor->floorHeight;

        if (actor->velocity.y <= 0.0f) {
            if (!(actor->bgCheckFlags & BGCHECKFLAG_GROUND)) {
                actor->bgCheckFlags |= BGCHECKFLAG_GROUND_TOUCH;
            } else if ((updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_8) && (actor->gravity < 0.0f)) {
                actor->velocity.y = -4.0f;
            } else if (!(updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_100)) {
                actor->velocity.y = 0.0f;
            }

            actor->bgCheckFlags |= BGCHECKFLAG_GROUND;
            DynaPolyActor_AttachCarriedActor(&play->colCtx, actor, actor->floorBgId);
        }
    } else {
        return func_800B761C(actor, distToFloor, updBgCheckInfoFlags);
    }

    return true;
}

void Actor_UpdateBgCheckInfo(PlayState* play, Actor* actor, f32 wallCheckHeight, f32 wallCheckRadius,
                             f32 ceilingCheckHeight, u32 updBgCheckInfoFlags) {
    f32 sp94 = actor->world.pos.y - actor->prevPos.y;
    s32 pad;
    Vec3f pos;

    if ((actor->floorBgId != BGCHECK_SCENE) && (actor->bgCheckFlags & BGCHECKFLAG_GROUND)) {
        DynaPolyActor_TransformCarriedActor(&play->colCtx, actor->floorBgId, actor);
    }

    if (updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_1) {
        s32 bgId;

        actor->bgCheckFlags &= ~BGCHECKFLAG_PLAYER_1000;
        if ((!(updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_80) &&
             (BgCheck_EntitySphVsWall3(&play->colCtx, &pos, &actor->world.pos, &actor->prevPos, wallCheckRadius,
                                       &actor->wallPoly, &bgId, actor, wallCheckHeight))) ||
            ((updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_80) &&
             (BgCheck_EntitySphVsWall4(&play->colCtx, &pos, &actor->world.pos, &actor->prevPos, wallCheckRadius,
                                       &actor->wallPoly, &bgId, actor, wallCheckHeight)))) {
            CollisionPoly* sp7C = actor->wallPoly;

            actor->bgCheckFlags |= BGCHECKFLAG_WALL;
            if ((updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_200) && (actor->bgCheckFlags & BGCHECKFLAG_PLAYER_1000) &&
                ((s32)sp7C->normal.y > 0) && (sqrtf(SQXYZ(actor->colChkInfo.displacement)) < 10.0f)) {
                actor->bgCheckFlags &= ~BGCHECKFLAG_WALL;
            } else if (actor->bgCheckFlags & BGCHECKFLAG_WALL) {
                Math_Vec3f_Copy(&actor->world.pos, &pos);
            }

            actor->wallYaw = Math_Atan2S_XY(sp7C->normal.z, sp7C->normal.x);
            actor->wallBgId = bgId;
        } else {
            actor->bgCheckFlags &= ~BGCHECKFLAG_WALL;
        }
    }

    pos.x = actor->world.pos.x;
    pos.z = actor->world.pos.z;
    if (updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_2) {
        f32 y;

        pos.y = actor->prevPos.y + 4.0f;
        if (BgCheck_EntityCheckCeiling(&play->colCtx, &y, &pos, (ceilingCheckHeight + sp94) - 4.0f, &D_801ED8B0,
                                       &D_801ED8B4, actor)) {
            actor->bgCheckFlags |= BGCHECKFLAG_CEILING;
            actor->world.pos.y = (y + sp94) - 4.0f;
        } else {
            actor->bgCheckFlags &= ~BGCHECKFLAG_CEILING;
        }
    }
    if (updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_4) {
        WaterBox* waterbox;
        f32 y;

        pos.y = actor->prevPos.y;
        func_800B7678(play, actor, &pos, updBgCheckInfoFlags);
        y = actor->world.pos.y;

        if (WaterBox_GetSurface1(play, &play->colCtx, actor->world.pos.x, actor->world.pos.z, &y, &waterbox)) {
            actor->depthInWater = y - actor->world.pos.y;
            if (actor->depthInWater <= 0.0f) {
                actor->bgCheckFlags &= ~(BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
            } else if (!(actor->bgCheckFlags & BGCHECKFLAG_WATER)) {
                actor->bgCheckFlags |= (BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
                if (!(updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_40)) {
                    Vec3f sp64;

                    sp64.x = actor->world.pos.x;
                    sp64.y = y;
                    sp64.z = actor->world.pos.z;

                    EffectSsGRipple_Spawn(play, &sp64, 100, 500, 0);
                    EffectSsGRipple_Spawn(play, &sp64, 100, 500, 4);
                    EffectSsGRipple_Spawn(play, &sp64, 100, 500, 8);
                }
            } else {
                actor->bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
            }
        } else {
            actor->bgCheckFlags &= ~(BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
            actor->depthInWater = BGCHECK_Y_MIN;
        }
    }

    if (updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_400) {
        WaterBox* waterbox;
        f32 y = actor->world.pos.y;

        if (WaterBox_GetSurface1(play, &play->colCtx, actor->world.pos.x, actor->world.pos.z, &y, &waterbox)) {
            actor->depthInWater = y - actor->world.pos.y;

            if (actor->depthInWater < 0.0f) {
                actor->bgCheckFlags &= ~(BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
            } else if (!(actor->bgCheckFlags & BGCHECKFLAG_WATER)) {
                actor->bgCheckFlags |= (BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
                if (!(updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_40)) {
                    Vec3f sp50;

                    sp50.x = actor->world.pos.x;
                    sp50.y = y;
                    sp50.z = actor->world.pos.z;

                    EffectSsGRipple_Spawn(play, &sp50, 100, 500, 0);
                    EffectSsGRipple_Spawn(play, &sp50, 100, 500, 4);
                    EffectSsGRipple_Spawn(play, &sp50, 100, 500, 8);
                }
            } else {
                actor->bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
            }
        } else {
            actor->bgCheckFlags &= ~(BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
            actor->depthInWater = BGCHECK_Y_MIN;
        }
    }
}

Gfx* Hilite_Draw(Vec3f* object, Vec3f* eye, Vec3f* lightDir, GraphicsContext* gfxCtx, Gfx* dl, Hilite** hiliteP) {
    LookAt* lookAt = GRAPH_ALLOC(gfxCtx, sizeof(LookAt));
    f32 correctedEyeX = (eye->x == object->x) && (eye->z == object->z) ? eye->x + 0.001f : eye->x;

    *hiliteP = GRAPH_ALLOC(gfxCtx, sizeof(Hilite));

    guLookAtHilite(&sActorHiliteMtx, lookAt, *hiliteP, correctedEyeX, eye->y, eye->z, object->x, object->y, object->z,
                   0.0f, 1.0f, 0.0f, lightDir->x, lightDir->y, lightDir->z, lightDir->x, lightDir->y, lightDir->z, 0x10,
                   0x10);

    gSPLookAt(dl++, lookAt);
    gDPSetHilite1Tile(dl++, 1, *hiliteP, 0x10, 0x10);

    return dl;
}

Hilite* Hilite_DrawOpa(Vec3f* object, Vec3f* eye, Vec3f* lightDir, GraphicsContext* gfxCtx) {
    Hilite* hilite;

    OPEN_DISPS(gfxCtx);

    POLY_OPA_DISP = Hilite_Draw(object, eye, lightDir, gfxCtx, POLY_OPA_DISP, &hilite);

    CLOSE_DISPS(gfxCtx);

    return hilite;
}

Hilite* Hilite_DrawXlu(Vec3f* object, Vec3f* eye, Vec3f* lightDir, GraphicsContext* gfxCtx) {
    Hilite* hilite;

    OPEN_DISPS(gfxCtx);

    POLY_XLU_DISP = Hilite_Draw(object, eye, lightDir, gfxCtx, POLY_XLU_DISP, &hilite);

    CLOSE_DISPS(gfxCtx);

    return hilite;
}

void func_800B8050(Actor* actor, PlayState* play, s32 flag) {
    Hilite* hilite = func_800BCBF4(&actor->world.pos, play);

    if (flag != 0) {
        Gfx* gfxHead = GRAPH_ALLOC(play->state.gfxCtx, 2 * sizeof(Gfx));
        Gfx* gfx = gfxHead;

        OPEN_DISPS(play->state.gfxCtx);

        gDPSetHilite1Tile(gfx++, 1, hilite, 0x10, 0x10);
        gSPEndDisplayList(gfx++);
        gSPSegment(POLY_OPA_DISP++, 0x07, gfxHead);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void func_800B8118(Actor* actor, PlayState* play, s32 flag) {
    Hilite* hilite = func_800BCC68(&actor->world.pos, play);

    if (flag != 0) {
        Gfx* gfxHead = GRAPH_ALLOC(play->state.gfxCtx, 2 * sizeof(Gfx));
        Gfx* gfx = gfxHead;

        OPEN_DISPS(play->state.gfxCtx);

        gDPSetHilite1Tile(gfx++, 1, hilite, 0x10, 0x10);
        gSPEndDisplayList(gfx++);
        gSPSegment(POLY_XLU_DISP++, 0x07, gfxHead);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

PosRot Actor_GetFocus(Actor* actor) {
    return actor->focus;
}

PosRot Actor_GetWorld(Actor* actor) {
    return actor->world;
}

PosRot Actor_GetWorldPosShapeRot(Actor* actor) {
    PosRot sp1C;

    Math_Vec3f_Copy(&sp1C.pos, &actor->world.pos);
    if (actor->id == ACTOR_PLAYER) {
        Player* player = (Player*)actor;

        sp1C.pos.y += player->unk_AC0 * actor->scale.y;
    }
    sp1C.rot = actor->shape.rot;

    return sp1C;
}

/**
 * Returns the squared xyz distance from the actor to Player.
 * This distance will be weighted if Player is already locked onto another actor.
 */
f32 Attention_WeightedDistToPlayerSq(Actor* actor, Player* player, s16 playerShapeYaw) {
    f32 adjDistSq;
    s16 yawDiffAbs = ABS_ALT(BINANG_SUB(BINANG_SUB(actor->yawTowardsPlayer, 0x8000), playerShapeYaw));

    if (player->focusActor != NULL) {
        if ((yawDiffAbs > 0x4000) || (actor->flags & ACTOR_FLAG_LOCK_ON_DISABLED)) {
            return FLT_MAX;
        }

        // The distance returned is scaled down as the player faces more toward the actor.
        // At 90 degrees, 100% of the original distance will be returned.
        // This scales down linearly to 60% when facing 0 degrees away.
        adjDistSq =
            actor->xyzDistToPlayerSq - ((actor->xyzDistToPlayerSq * 0.8f) * ((0x4000 - yawDiffAbs) * (1.0f / 0x8000)));

        return adjDistSq;
    }

    // Player has to be facing less than ~60 degrees away from the actor
    if (yawDiffAbs > (0x10000 / 6)) {
        return FLT_MAX;
    }

    // Unweighted distSq
    return actor->xyzDistToPlayerSq;
}

#define ATTENTION_RANGES(range, lockOnLeashRange) \
    { SQ(range), (f32)(range) / (lockOnLeashRange) }

AttentionRangeParams gAttentionRanges[ATTENTION_RANGE_MAX] = {
    ATTENTION_RANGES(70, 140),        // ATTENTION_RANGE_0
    ATTENTION_RANGES(170, 255),       // ATTENTION_RANGE_1
    ATTENTION_RANGES(280, 5600),      // ATTENTION_RANGE_2
    ATTENTION_RANGES(350, 525),       // ATTENTION_RANGE_3
    ATTENTION_RANGES(700, 1050),      // ATTENTION_RANGE_4
    ATTENTION_RANGES(1000, 1500),     // ATTENTION_RANGE_5
    ATTENTION_RANGES(100, 105.36842), // ATTENTION_RANGE_6
    ATTENTION_RANGES(140, 163.33333), // ATTENTION_RANGE_7
    ATTENTION_RANGES(240, 576),       // ATTENTION_RANGE_8
    ATTENTION_RANGES(280, 280000),    // ATTENTION_RANGE_9
    ATTENTION_RANGES(2500, 3750),     // ATTENTION_RANGE_10
};

/**
 * Checks if an actor at `distSq` is inside the range specified by its `attentionRangeType`.
 *
 * Note that this gets used for both the attention range check and for the lock-on leash range check.
 * Despite how the data is presented in `gAttentionRanges`, the leash range is stored as a scale factor value.
 * When checking the leash range, this scale factor is applied to the input distance and checked against
 * the base `attentionRangeSq` value, which was used to initiate the lock-on in the first place.
 */
s32 Attention_ActorIsInRange(Actor* actor, f32 distSq) {
    return distSq < gAttentionRanges[actor->attentionRangeType].attentionRangeSq;
}

/**
 * Returns true if an actor lock-on should be released.
 * This function does not actually release the lock-on, as that is Player's responsibility.
 *
 * If an actor's update function is NULL or `ACTOR_FLAG_ATTENTION_ENABLED` is unset, the lock-on should be released.
 *
 * There is also a check for Player exceeding the lock-on leash distance.
 * Note that this check will be ignored if `ignoreLeash` is true.
 *
 */
s32 Attention_ShouldReleaseLockOn(Actor* actor, Player* player, s32 ignoreLeash) {
    if ((actor->update == NULL) || !(actor->flags & ACTOR_FLAG_ATTENTION_ENABLED) ||
        (actor->flags & ACTOR_FLAG_LOCK_ON_DISABLED)) {
        return true;
    }

    if (!ignoreLeash) {
        s16 yawDiffAbs = ABS_ALT(BINANG_SUB(BINANG_SUB(actor->yawTowardsPlayer, 0x8000), player->actor.shape.rot.y));
        f32 distSq;

        if ((player->focusActor == NULL) && (yawDiffAbs > (0x10000 / 6))) {
            // This function is only called (and is only relevant) when `player->focusActor != NULL`.
            // This is unreachable.
            distSq = FLT_MAX;
        } else {
            distSq = actor->xyzDistToPlayerSq;
        }

        return !Attention_ActorIsInRange(actor, gAttentionRanges[actor->attentionRangeType].lockOnLeashScale * distSq);
    }

    return false;
}

s16 D_801AED48[] = {
    HALFDAYBIT_DAY0_NIGHT | HALFDAYBIT_DAY4_NIGHT,
    HALFDAYBIT_DAY0_NIGHT | HALFDAYBIT_DAY1_NIGHT | HALFDAYBIT_DAY4_NIGHT,
    HALFDAYBIT_DAY0_NIGHT | HALFDAYBIT_DAY2_NIGHT | HALFDAYBIT_DAY4_NIGHT,
    HALFDAYBIT_DAY0_NIGHT | HALFDAYBIT_DAY1_NIGHT | HALFDAYBIT_DAY2_NIGHT | HALFDAYBIT_DAY4_NIGHT,
    HALFDAYBIT_DAY0_NIGHT | HALFDAYBIT_DAY3_NIGHT | HALFDAYBIT_DAY4_NIGHT,
    HALFDAYBIT_DAY0_NIGHT | HALFDAYBIT_DAY1_NIGHT | HALFDAYBIT_DAY3_NIGHT | HALFDAYBIT_DAY4_NIGHT,
    HALFDAYBIT_DAY0_NIGHT | HALFDAYBIT_DAY2_NIGHT | HALFDAYBIT_DAY3_NIGHT | HALFDAYBIT_DAY4_NIGHT,
    HALFDAYBIT_DAY0_NIGHT | HALFDAYBIT_DAY1_NIGHT | HALFDAYBIT_DAY2_NIGHT | HALFDAYBIT_DAY3_NIGHT |
        HALFDAYBIT_DAY4_NIGHT,
};

/**
 * When a given talk offer is accepted, Player will set `ACTOR_FLAG_TALK` for that actor.
 * This function serves to acknowledge that the offer was accepted by Player, and notifies the actor
 * that it should proceed with its own internal processes for handling dialogue.
 *
 * @return  true if the talk offer was accepted, false otherwise
 */
s32 Actor_TalkOfferAccepted(Actor* actor, GameState* gameState) {
    if (actor->flags & ACTOR_FLAG_TALK) {
        actor->flags &= ~ACTOR_FLAG_TALK;
        return true;
    }

    return false;
}

/**
 * This function covers offering the ability to `Talk` with the player.
 * Passing an exchangeItemAction (see `PlayerItemAction`) allows the player to also use the item to initiate the
 * conversation.
 *
 * This function carries a talk exchange request to the player actor if context allows it (e.g. the player is in range
 * and not busy with certain things). The player actor performs the requested action itself.
 *
 * The following description of what the `exchangeItemAction` values can do is provided here for completeness, but these
 * behaviors are entirely out of the scope of this function. All behavior is defined by the player actor.
 *
 * - Positive values (`PLAYER_IA_NONE < exchangeItemAction < PLAYER_IA_MAX`):
 *    Offers the ability to initiate the conversation with an item from the player.
 *    Not all positive values are implemented properly for this to work.
 *    Working ones are PLAYER_IA_PICTOGRAPH_BOX and PLAYER_IA_BOTTLE_MIN <= exchangeItemAction < PLAYER_IA_MASK_MIN
 *    Note: While PLAYER_IA_MAGIC_BEANS works, it is special cased to just plant the bean with no talking.
 * - `PLAYER_IA_NONE`:
 *    Allows the player to speak to or check the actor (by pressing A).
 * - `PLAYER_IA_MINUS1`:
 *    Used by actors/player to continue the current conversation after a textbox is closed.
 *
 * @return true If the player actor is capable of accepting the offer.
 *
 * Note: There is only one instance of using this for actually using an item to start the conversation with the player.
 * Every other instance is to either offer to speak, or continue the current conversation.
 */
s32 Actor_OfferTalkExchange(Actor* actor, PlayState* play, f32 xzRange, f32 yRange,
                            PlayerItemAction exchangeItemAction) {
    Player* player = GET_PLAYER(play);

    if ((player->actor.flags & ACTOR_FLAG_TALK) || ((exchangeItemAction > PLAYER_IA_NONE) && Player_InCsMode(play)) ||
        (!actor->isLockedOn &&
         ((fabsf(actor->playerHeightRel) > fabsf(yRange)) || (actor->xzDistToPlayer > player->talkActorDistance) ||
          (xzRange < actor->xzDistToPlayer)))) {
        return false;
    }

    player->talkActor = actor;
    player->talkActorDistance = actor->xzDistToPlayer;
    player->exchangeItemAction = exchangeItemAction;

    CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
    return true;
}

/**
 * Offers a talk exchange request within an equilateral cylinder with the radius specified.
 */
s32 Actor_OfferTalkExchangeEquiCylinder(Actor* actor, PlayState* play, f32 radius,
                                        PlayerItemAction exchangeItemAction) {
    return Actor_OfferTalkExchange(actor, play, radius, radius, exchangeItemAction);
}

/**
 * Offers a talk request within an equilateral cylinder with the radius specified.
 */
s32 Actor_OfferTalk(Actor* actor, PlayState* play, f32 radius) {
    return Actor_OfferTalkExchangeEquiCylinder(actor, play, radius, PLAYER_IA_NONE);
}

/**
 * Offers a talk request within an equilateral cylinder whose radius is determined by the actor's collision check
 * cylinder's radius.
 */
s32 Actor_OfferTalkNearColChkInfoCylinder(Actor* actor, PlayState* play) {
    f32 cylRadius = actor->colChkInfo.cylRadius + 50.0f;

    return Actor_OfferTalk(actor, play, cylRadius);
}

s32 Actor_TextboxIsClosing(Actor* actor, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        actor->flags &= ~ACTOR_FLAG_TALK;
        return true;
    }

    return false;
}

/**
 * Changes the actor the Player is focussing on
 * Fails if Player is not already focussing on an actor or in a talking state
 */
s32 Actor_ChangeFocus(Actor* actor1, PlayState* play, Actor* actor2) {
    Actor* talkActor;
    Player* player = GET_PLAYER(play);

    talkActor = player->talkActor;

    if ((player->actor.flags & ACTOR_FLAG_TALK) && (talkActor != NULL)) {
        player->talkActor = actor2;
        player->focusActor = actor2;
        return true;
    }

    return false;
}

PlayerItemAction Player_GetExchangeItemAction(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->exchangeItemAction;
}

/**
 * When a given ocarina interaction offer is accepted, Player will set `ACTOR_FLAG_OCARINA_INTERACTION` for that actor.
 * An exception is made for EN_ZOT, see `Player_ActionHandler_13`.
 * This function serves to acknowledge that the offer was accepted by Player, and notifies the actor
 * that it should proceed with its own internal processes for handling further interactions.
 *
 * @return  true if the ocarina interaction offer was accepted, false otherwise
 */
s32 Actor_OcarinaInteractionAccepted(Actor* actor, GameState* gameState) {
    if (actor->flags & ACTOR_FLAG_OCARINA_INTERACTION) {
        actor->flags &= ~ACTOR_FLAG_OCARINA_INTERACTION;
        return true;
    }

    return false;
}

s32 Actor_OfferOcarinaInteraction(Actor* actor, PlayState* play, f32 xzRange, f32 yRange) {
    Player* player = GET_PLAYER(play);

    if ((player->actor.flags & ACTOR_FLAG_OCARINA_INTERACTION) || Player_InCsMode(play) ||
        (yRange < fabsf(actor->playerHeightRel)) || ((player->ocarinaInteractionDistance < actor->xzDistToPlayer)) ||
        (xzRange < actor->xzDistToPlayer)) {
        return false;
    }

    player->ocarinaInteractionActor = actor;
    player->ocarinaInteractionDistance = actor->xzDistToPlayer;
    return true;
}

s32 Actor_OfferOcarinaInteractionNearby(Actor* actor, PlayState* play, f32 xzRange) {
    return Actor_OfferOcarinaInteraction(actor, play, xzRange, 20.0f);
}

s32 Actor_OfferOcarinaInteractionColChkInfoCylinder(Actor* actor, PlayState* play) {
    f32 cylRadius = actor->colChkInfo.cylRadius + 50.0f;

    return Actor_OfferOcarinaInteractionNearby(actor, play, cylRadius);
}

s32 Actor_NoOcarinaInteraction(Actor* actor, PlayState* play) {
    if (!(GET_PLAYER(play)->actor.flags & ACTOR_FLAG_OCARINA_INTERACTION)) {
        return true;
    }

    return false;
}

void Actor_GetScreenPos(PlayState* play, Actor* actor, s16* posX, s16* posY) {
    Vec3f projectedPos;
    f32 invW;

    Actor_GetProjectedPos(play, &actor->focus.pos, &projectedPos, &invW);

    *posX = PROJECTED_TO_SCREEN_X(projectedPos, invW);
    *posY = PROJECTED_TO_SCREEN_Y(projectedPos, invW);
}

bool Actor_OnScreen(PlayState* play, Actor* actor) {
    Vec3f projectedPos;
    f32 invW;
    s32 pad[2];

    Actor_GetProjectedPos(play, &actor->focus.pos, &projectedPos, &invW);

    return (projectedPos.x * invW >= -1.0f) && (projectedPos.x * invW <= 1.0f) && (projectedPos.y * invW >= -1.0f) &&
           (projectedPos.y * invW <= 1.0f);
}

s32 Actor_HasParent(Actor* actor, PlayState* play) {
    if (actor->parent != NULL) {
        return true;
    } else {
        return false;
    }
}

/**
 * This function covers various interactions with the player actor, using Get Item IDs (see `GetItemID` enum).
 * It is typically used to give items to the player, but also has other purposes.
 *
 * This function carries a get item request to the player actor if context allows it (e.g. the player is in range and
 * not busy with certain things). The player actor performs the requested action itself.
 *
 * The following description of what the `getItemId` values can do is provided here for completeness, but these
 * behaviors are entirely out of the scope of this function. All behavior is defined by the player actor.
 *
 * - Positive values (`GI_NONE < getItemId < GI_MAX`):
 *    Give an item to the player. The player may not get it immediately (for example if diving), but is expected to
 *    in the near future.
 * - Negative values (`-GI_MAX < getItemId < GI_NONE`):
 *    Used by treasure chests to indicate the chest can be opened (by pressing A).
 *    The item gotten corresponds to the positive Get Item ID `abs(getItemId)`.
 * - `GI_NONE`:
 *    Allows the player to pick up the actor (by pressing A), to carry it around.
 * - `GI_MAX`:
 *    Allows the player to catch specific actors in a bottle.
 *
 * @return true If the player actor is capable of accepting the offer.
 */
s32 Actor_OfferGetItem(Actor* actor, PlayState* play, GetItemId getItemId, f32 xzRange, f32 yRange) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 &
          (PLAYER_STATE1_DEAD | PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 |
           PLAYER_STATE1_40000 | PLAYER_STATE1_80000 | PLAYER_STATE1_100000 | PLAYER_STATE1_200000)) &&
        (Player_GetExplosiveHeld(player) <= PLAYER_EXPLOSIVE_NONE)) {
        if ((actor->xzDistToPlayer <= xzRange) && (fabsf(actor->playerHeightRel) <= fabsf(yRange))) {
            if (((getItemId == GI_MASK_CIRCUS_LEADER) || (getItemId == GI_PENDANT_OF_MEMORIES) ||
                 (getItemId == GI_DEED_LAND) ||
                 (((player->heldActor != NULL) || (actor == player->talkActor)) &&
                  ((getItemId > GI_NONE) && (getItemId < GI_MAX)))) ||
                !(player->stateFlags1 & (PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_20000000))) {
                s16 yawDiff = actor->yawTowardsPlayer - player->actor.shape.rot.y;
                s32 absYawDiff = ABS_ALT(yawDiff);

                if (GameInteractor_Should(VB_GIVE_ITEM_FROM_OFFER,
                                          ((getItemId != GI_NONE) || (player->getItemDirection < absYawDiff)),
                                          &getItemId, actor)) {
                    player->getItemId = getItemId;
                    player->interactRangeActor = actor;
                    player->getItemDirection = absYawDiff;

                    if ((getItemId > GI_NONE) && (getItemId < GI_MAX)) {
                        CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_ITEM_GET]);
                    }

                    return true;
                }
            }
        }
    }

    return false;
}

s32 Actor_OfferGetItemNearby(Actor* actor, PlayState* play, GetItemId getItemId) {
    return Actor_OfferGetItem(actor, play, getItemId, 50.0f, 10.0f);
}

s32 Actor_OfferCarry(Actor* actor, PlayState* play) {
    return Actor_OfferGetItemNearby(actor, play, GI_NONE);
}

s32 Actor_OfferGetItemFar(Actor* actor, PlayState* play, GetItemId getItemId) {
    return Actor_OfferGetItem(actor, play, getItemId, 9999.9f, 9999.9f);
}

s32 Actor_HasNoParent(Actor* actor, PlayState* play) {
    if (actor->parent == NULL) {
        return true;
    }

    return false;
}

void func_800B8C20(Actor* actorA, Actor* actorB, PlayState* play) {
    Actor* parent = actorA->parent;

    if (parent->id == ACTOR_PLAYER) {
        Player* player = (Player*)parent;

        player->heldActor = actorB;
        player->interactRangeActor = actorB;
    }

    parent->child = actorB;
    actorB->parent = parent;
    actorA->parent = NULL;
}

/**
 * Sets closest secret distance to the distance to the actor. Calling this function on `actor` is the way to make it a
 * 'secret' for that update cycle, i.e. something that the controller will rumble for.
 */
void Actor_SetClosestSecretDistance(Actor* actor, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (actor->xyzDistToPlayerSq < player->closestSecretDistSq) {
        player->closestSecretDistSq = actor->xyzDistToPlayerSq;
    }
}

s32 Actor_HasRider(PlayState* play, Actor* horse) {
    if (horse->child != NULL) {
        return true;
    }

    return false;
}

s32 Actor_SetRideActor(PlayState* play, Actor* horse, s32 mountSide) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 &
          (PLAYER_STATE1_DEAD | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_2000 |
           PLAYER_STATE1_4000 | PLAYER_STATE1_40000 | PLAYER_STATE1_80000 | PLAYER_STATE1_100000 |
           PLAYER_STATE1_200000))) {
        player->rideActor = horse;
        player->mountSide = mountSide;
        CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
        return true;
    }

    return false;
}

s32 Actor_HasNoRider(PlayState* play, Actor* horse) {
    if (horse->child == NULL) {
        return true;
    }

    return false;
}

void func_800B8D10(PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4, s32 arg5, u32 arg6) {
    Player* player = GET_PLAYER(play);

    player->unk_B74 = arg6;
    player->unk_B75 = arg5;
    player->unk_B78 = arg2;
    player->unk_B76 = arg3;
    player->unk_B7C = arg4;
}

void func_800B8D50(PlayState* play, Actor* actor, f32 arg2, s16 yaw, f32 arg4, u32 arg5) {
    func_800B8D10(play, actor, arg2, yaw, arg4, 3, arg5);
}

void func_800B8D98(PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4) {
    func_800B8D50(play, actor, arg2, arg3, arg4, 0);
}

void func_800B8DD4(PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4, u32 arg5) {
    func_800B8D10(play, actor, arg2, arg3, arg4, 2, arg5);
}

void func_800B8E1C(PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4) {
    func_800B8DD4(play, actor, arg2, arg3, arg4, 0);
}

/**
 * Play a sound effect at the player's position
 */
void Player_PlaySfx(Player* player, u16 sfxId) {
    if (player->currentMask == PLAYER_MASK_GIANT) {
        Audio_PlaySfx_AtPosWithPresetLowFreqAndHighReverb(&player->actor.projectedPos, sfxId);
    } else {
        if (GameInteractor_Should(VB_LINK_VOICE_PITCH_MULTIPLIER, true, &sfxId)) {
            AudioSfx_PlaySfx(sfxId, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }
    }
}

/**
 * Play a sound effect at the actor's position
 */
void Actor_PlaySfx(Actor* actor, u16 sfxId) {
    Audio_PlaySfx_AtPos(&actor->projectedPos, sfxId);
}

void Actor_PlaySfx_SurfaceBomb(PlayState* play, Actor* actor) {
    SurfaceSfxOffset surfaceSfxOffset;

    if (actor->bgCheckFlags & BGCHECKFLAG_WATER) {
        if (actor->depthInWater < 20.0f) {
            surfaceSfxOffset = SURFACE_SFX_OFFSET_WATER_SHALLOW;
        } else {
            surfaceSfxOffset = SURFACE_SFX_OFFSET_WATER_DEEP;
        }
    } else {
        surfaceSfxOffset = SurfaceType_GetSfxOffset(&play->colCtx, actor->floorPoly, actor->floorBgId);
    }

    Audio_PlaySfx_AtPos(&actor->projectedPos, NA_SE_EV_BOMB_BOUND);
    Audio_PlaySfx_AtPos(&actor->projectedPos, NA_SE_PL_WALK_GROUND + surfaceSfxOffset);
}

/**
 * Play a sfx at the actor's position using the shared audioFlag system
 */
void Actor_PlaySfx_Flagged2(Actor* actor, u16 sfxId) {
    actor->sfxId = sfxId;
    actor->audioFlags &= ~ACTOR_AUDIO_FLAG_SFX_ALL;
    actor->audioFlags |= ACTOR_AUDIO_FLAG_SFX_ACTOR_POS_2;
}

/**
 * Play a sfx at the center of the screen using the shared audioFlag system
 */
void Actor_PlaySfx_FlaggedCentered1(Actor* actor, u16 sfxId) {
    actor->sfxId = sfxId;
    actor->audioFlags &= ~ACTOR_AUDIO_FLAG_SFX_ALL;
    actor->audioFlags |= ACTOR_AUDIO_FLAG_SFX_CENTERED_1;
}

/**
 * Play a sfx at the center of the screen using the shared audioFlag system
 */
void Actor_PlaySfx_FlaggedCentered2(Actor* actor, u16 sfxId) {
    actor->sfxId = sfxId;
    actor->audioFlags &= ~ACTOR_AUDIO_FLAG_SFX_ALL;
    actor->audioFlags |= ACTOR_AUDIO_FLAG_SFX_CENTERED_2;
}

/**
 * Play a sfx at the actor's position using the shared audioFlag system
 */
void Actor_PlaySfx_Flagged(Actor* actor, u16 sfxId) {
    actor->sfxId = sfxId;
    actor->audioFlags &= ~ACTOR_AUDIO_FLAG_SFX_ALL;
    actor->audioFlags |= ACTOR_AUDIO_FLAG_SFX_ACTOR_POS_1;
}

void Actor_PlaySfx_FlaggedTimer(Actor* actor, s32 timer) {
    actor->audioFlags &= ~ACTOR_AUDIO_FLAG_SFX_ALL;
    actor->audioFlags |= ACTOR_AUDIO_FLAG_SFX_TIMER;

    // The sfxId here are not actually sound effects, but instead this is data that gets sent into
    // the io ports of the music macro language (Audio_PlaySfx_AtPosWithChannelIO / Audio_PlaySfxAtPosWithSoundScriptIO
    // is the function that it's used for)
    if (timer < 40) {
        actor->sfxId = 3;
    } else if (timer < 100) {
        actor->sfxId = 2;
    } else {
        actor->sfxId = 1;
    }
}

void Actor_PlaySeq_FlaggedKamaroDance(Actor* actor) {
    actor->audioFlags |= ACTOR_AUDIO_FLAG_SEQ_KAMARO_DANCE;
}

void Actor_PlaySeq_FlaggedMusicBoxHouse(Actor* actor) {
    actor->audioFlags |= ACTOR_AUDIO_FLAG_SEQ_MUSIC_BOX_HOUSE;
}

s32 func_800B90AC(PlayState* play, Actor* actor, CollisionPoly* polygon, s32 bgId, Vec3f* arg4) {
    if (SurfaceType_GetFloorType(&play->colCtx, polygon, bgId) == FLOOR_TYPE_8) {
        return true;
    }

    return false;
}

void Actor_DeactivateLens(PlayState* play) {
    if (play->actorCtx.lensActive) {
        play->actorCtx.lensActive = false;
        Magic_Reset(play);
    }
}

void Actor_InitHalfDaysBit(ActorContext* actorCtx) {
    s32 halfDayCount = CURRENT_DAY * 2;

    if ((CURRENT_TIME < CLOCK_TIME(6, 0)) || (CURRENT_TIME > CLOCK_TIME(18, 0))) {
        halfDayCount++;
    }

    actorCtx->halfDaysBit = HALFDAYBIT_DAY0_DAWN >> halfDayCount;
}

void Actor_InitContext(PlayState* play, ActorContext* actorCtx, ActorEntry* actorEntry) {
    ActorOverlay* overlayEntry;
    CycleSceneFlags* cycleFlags;
    s32 i;

    SET_WEEKEVENTREG(WEEKEVENTREG_92_80);
    cycleFlags = &gSaveContext.cycleSceneFlags[Play_GetOriginalSceneId(play->sceneId)];

    memset(actorCtx, 0, sizeof(ActorContext));
    ActorOverlayTable_Init();
    Matrix_MtxFCopy(&play->billboardMtxF, &gIdentityMtxF);
    Matrix_MtxFCopy(&play->viewProjectionMtxF, &gIdentityMtxF);

    overlayEntry = gActorOverlayTable;
    for (i = 0; i < ARRAY_COUNT(gActorOverlayTable); i++) {
        overlayEntry->loadedRamAddr = NULL;
        overlayEntry->numLoaded = 0;
        overlayEntry++;
    }

    actorCtx->sceneFlags.chest = cycleFlags->chest;
    actorCtx->sceneFlags.switches[0] = cycleFlags->switch0;
    actorCtx->sceneFlags.switches[1] = cycleFlags->switch1;
    if (play->sceneId == SCENE_INISIE_R) {
        cycleFlags = &gSaveContext.cycleSceneFlags[play->sceneId];
    }
    actorCtx->sceneFlags.collectible[0] = cycleFlags->collectible;
    actorCtx->sceneFlags.clearedRoom = cycleFlags->clearedRoom;

    TitleCard_ContextInit(&play->state, &actorCtx->titleCtx);
    Actor_InitPlayerImpact(play);

    actorCtx->absoluteSpace = NULL;

    Actor_SpawnEntry(actorCtx, actorEntry, play);
    Attention_Init(&actorCtx->attention, actorCtx->actorLists[ACTORCAT_PLAYER].first, play);
    Actor_InitHalfDaysBit(actorCtx);
    // Fault_AddClient(&sActorFaultClient, (void*)Actor_PrintLists, actorCtx, NULL);
    Player_SpawnHorse(play, (Player*)actorCtx->actorLists[ACTORCAT_PLAYER].first);
}

/**
 * Spawns the actors in the current setup (of the current scene/setup/room triple)
 * Only spawns actors based on the time flags embedded in their rotation parameters
 */
void Actor_SpawnSetupActors(PlayState* play, ActorContext* actorCtx) {
    if (play->numSetupActors > 0) {
        ActorEntry* actorEntry = play->setupActorList;
        s32 prevHalfDaysBitValue = actorCtx->halfDaysBit;
        s32 shiftedHalfDaysBit;
        s32 actorEntryHalfDayBit;
        s32 i;

        Actor_InitHalfDaysBit(actorCtx);
        Actor_KillAllOnHalfDayChange(play, &play->actorCtx);

        // Shift to previous halfDay bit, but ignoring DAY0_NIGHT.
        // In other words, if the current halfDay is DAY1_DAY then this logic is ignored and this variable is zero
        shiftedHalfDaysBit = (actorCtx->halfDaysBit << 1) & (HALFDAYBIT_ALL & ~HALFDAYBIT_DAY0_NIGHT);

        for (i = 0; i < play->numSetupActors; i++) {
            // 2S2H [ObjectExtension] store the actor's index to be used for identification
            currentActorListIndex = i;

            actorEntryHalfDayBit = ((actorEntry->rot.x & 7) << 7) | (actorEntry->rot.z & 0x7F);
            if (actorEntryHalfDayBit == 0) {
                actorEntryHalfDayBit = HALFDAYBIT_ALL;
            }

            if (!(actorEntryHalfDayBit & prevHalfDaysBitValue) && (actorEntryHalfDayBit & actorCtx->halfDaysBit) &&
                (!CHECK_EVENTINF(EVENTINF_17) || !(actorEntryHalfDayBit & shiftedHalfDaysBit) ||
                 !(actorEntry->id & 0x800))) {
                Actor_SpawnEntry(&play->actorCtx, actorEntry, play);
            }
            actorEntry++;
        }
        // 2S2H [ObjectExtension] Reset the currentActorListIndex
        currentActorListIndex = -1;

        // Prevents re-spawning the setup actors
        play->numSetupActors = -play->numSetupActors;
    }
}

typedef struct {
    /* 0x00 */ PlayState* play;
    /* 0x04 */ Actor* actor;
    /* 0x08 */ u32 freezeExceptionFlag;
    /* 0x0C */ u32 canFreezeCategory;
    /* 0x10 */ Actor* talkActor;
    /* 0x14 */ Player* player;
    /* 0x18 */ u32 updateActorFlagsMask; // Actor will update only if at least 1 actor flag is set in this bitmask
} UpdateActor_Params;                    // size = 0x1C

Actor* Actor_UpdateActor(UpdateActor_Params* params) {
    PlayState* play = params->play;
    Actor* actor = params->actor;
    Actor* nextActor;

    if (actor->world.pos.y < -25000.0f) {
        actor->world.pos.y = -25000.0f;
    }

    actor->sfxId = 0;
    actor->audioFlags &= ~ACTOR_AUDIO_FLAG_ALL;

    if (actor->init != NULL) {
        if (Object_IsLoaded(&play->objectCtx, actor->objectSlot)) {
            Actor_SetObjectDependency(play, actor);

            if (GameInteractor_ShouldActorInit(actor)) {
                actor->init(actor, play);
                actor->init = NULL;
                GameInteractor_ExecuteOnActorInit(actor);
            } else {
                actor->init = NULL;
                Actor_Kill(actor);
            }
        }
        nextActor = actor->next;
    } else if (actor->update == NULL) {
        if (!actor->isDrawn) {
            nextActor = Actor_Delete(&play->actorCtx, actor, play);
        } else {
            Actor_Destroy(actor, play);
            nextActor = actor->next;
        }
    } else {
        if (!Object_IsLoaded(&play->objectCtx, actor->objectSlot)) {
            Actor_Kill(actor);
        } else if (((params->freezeExceptionFlag != 0) && !(actor->flags & params->freezeExceptionFlag)) ||
                   (((!params->freezeExceptionFlag) != 0) &&
                    (!(actor->flags & ACTOR_FLAG_FREEZE_EXCEPTION) ||
                     ((actor->category == ACTORCAT_EXPLOSIVES) && (params->player->stateFlags1 & PLAYER_STATE1_200))) &&
                    params->canFreezeCategory && (actor != params->talkActor) && (actor != params->player->heldActor) &&
                    (actor->parent != &params->player->actor))) {
            CollisionCheck_ResetDamage(&actor->colChkInfo);
        } else {
            Math_Vec3f_Copy(&actor->prevPos, &actor->world.pos);
            actor->xzDistToPlayer = Actor_WorldDistXZToActor(actor, &params->player->actor);
            actor->playerHeightRel = Actor_HeightDiff(actor, &params->player->actor);
            actor->xyzDistToPlayerSq = SQ(actor->xzDistToPlayer) + SQ(actor->playerHeightRel);

            actor->yawTowardsPlayer = Actor_WorldYawTowardActor(actor, &params->player->actor);
            actor->flags &= ~ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;

            if ((DECR(actor->freezeTimer) == 0) && (actor->flags & params->updateActorFlagsMask)) {
                if (actor == params->player->focusActor) {
                    actor->isLockedOn = true;
                } else {
                    actor->isLockedOn = false;
                }

                if ((actor->targetPriority != 0) && (params->player->focusActor == NULL)) {
                    actor->targetPriority = 0;
                }

                Actor_SetObjectDependency(play, actor);

                if (actor->colorFilterTimer != 0) {
                    actor->colorFilterTimer--;
                }

                if (GameInteractor_ShouldActorUpdate(actor)) {
                    actor->update(actor, play);
                    GameInteractor_ExecuteOnActorUpdate(actor);
                }
                DynaPoly_UnsetAllInteractFlags(play, &play->colCtx.dyna, actor);
            }

            CollisionCheck_ResetDamage(&actor->colChkInfo);
        }
        nextActor = actor->next;
    }
    return nextActor;
}

u32 sCategoryFreezeMasks[ACTORCAT_MAX] = {
    /* ACTORCAT_SWITCH */
    PLAYER_STATE1_2 | PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_200 | PLAYER_STATE1_10000000,
    /* ACTORCAT_BG */
    PLAYER_STATE1_2 | PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_200 | PLAYER_STATE1_10000000,
    /* ACTORCAT_PLAYER */
    PLAYER_STATE1_200,
    /* ACTORCAT_EXPLOSIVES */
    PLAYER_STATE1_2 | PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_200 | PLAYER_STATE1_400 |
        PLAYER_STATE1_10000000,
    /* ACTORCAT_NPC */
    PLAYER_STATE1_2 | PLAYER_STATE1_DEAD | PLAYER_STATE1_200,
    /* ACTORCAT_ENEMY */
    PLAYER_STATE1_2 | PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_200 | PLAYER_STATE1_10000000 |
        PLAYER_STATE1_20000000,
    /* ACTORCAT_PROP */
    PLAYER_STATE1_2 | PLAYER_STATE1_DEAD | PLAYER_STATE1_200 | PLAYER_STATE1_10000000,
    /* ACTORCAT_ITEMACTION */
    PLAYER_STATE1_2,
    /* ACTORCAT_MISC */
    PLAYER_STATE1_2 | PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_200 | PLAYER_STATE1_10000000 |
        PLAYER_STATE1_20000000,
    /* ACTORCAT_BOSS */
    PLAYER_STATE1_2 | PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_200 | PLAYER_STATE1_400 |
        PLAYER_STATE1_10000000,
    /* ACTORCAT_DOOR */
    PLAYER_STATE1_2,
    /* ACTORCAT_CHEST */
    PLAYER_STATE1_2 | PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_200 | PLAYER_STATE1_10000000,
};

void Actor_UpdateAll(PlayState* play, ActorContext* actorCtx) {
    s32 category;
    Actor* actor;
    Player* player = GET_PLAYER(play);
    u32* categoryFreezeMaskP;
    s32 newCategory;
    Actor* next;
    ActorListEntry* entry;
    UpdateActor_Params params;

    params.player = player;
    params.play = play;

    if (play->soaringCsOrSoTCsPlaying) {
        params.updateActorFlagsMask = ACTOR_FLAG_UPDATE_DURING_SOARING_AND_SOT_CS;
    } else {
        params.updateActorFlagsMask = ACTOR_FLAG_UPDATE_DURING_SOARING_AND_SOT_CS | ACTOR_FLAG_INSIDE_CULLING_VOLUME |
                                      ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }

    Actor_SpawnSetupActors(play, actorCtx);

    if (actorCtx->unk2 != 0) {
        actorCtx->unk2--;
    }

    categoryFreezeMaskP = sCategoryFreezeMasks;

    if (player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) {
        params.freezeExceptionFlag = ACTOR_FLAG_UPDATE_DURING_OCARINA;
    } else {
        params.freezeExceptionFlag = 0;
    }

    if ((player->stateFlags1 & PLAYER_STATE1_TALKING) && ((player->actor.textId & 0xFF00) != 0x1900)) {
        params.talkActor = player->talkActor;
    } else {
        params.talkActor = NULL;
    }

    for (category = 0, entry = actorCtx->actorLists; category < ACTORCAT_MAX;
         entry++, categoryFreezeMaskP++, category++) {
        params.canFreezeCategory = *categoryFreezeMaskP & player->stateFlags1;
        params.actor = entry->first;

        while (params.actor != NULL) {
            params.actor = Actor_UpdateActor(&params);
        }

        if (category == ACTORCAT_BG) {
            DynaPoly_UpdateContext(play, &play->colCtx.dyna);
        }
    }

    // Move actors to a different actorList if it has changed categories.
    for (category = 0, entry = actorCtx->actorLists; category < ACTORCAT_MAX; entry++, category++) {
        if (entry->categoryChanged) {
            actor = entry->first;

            while (actor != NULL) {
                if (actor->category == category) {
                    // The actor category matches the list category. No change needed.
                    actor = actor->next;
                    continue;
                }

                // The actor category does not match the list category and needs to be moved.
                next = actor->next;
                newCategory = actor->category;
                actor->category = category;
                Actor_RemoveFromCategory(play, actorCtx, actor);
                Actor_AddToCategory(actorCtx, actor, newCategory);
                actor = next;
            }
            entry->categoryChanged = false;
        }
    }

    actor = player->focusActor;
    if ((actor != NULL) && (actor->update == NULL)) {
        actor = NULL;
        Player_ReleaseLockOn(player);
    }

    if ((actor == NULL) || (player->zTargetActiveTimer < 5)) {
        actor = NULL;
        if (actorCtx->attention.reticleSpinCounter != 0) {
            actorCtx->attention.reticleSpinCounter = 0;
            Audio_PlaySfx(NA_SE_SY_LOCK_OFF);
        }
    }

    if (!(player->stateFlags1 & PLAYER_STATE1_2)) {
        Attention_Update(&actorCtx->attention, player, actor, play);
    }

    TitleCard_Update(&play->state, &actorCtx->titleCtx);
    Actor_UpdatePlayerImpact(play);
    DynaPoly_UpdateBgActorTransforms(play, &play->colCtx.dyna);
}

// 2S2H [Port] Extern these for use below in interpolation checks
extern void Player_Action_93(Player* this, PlayState* play);
extern void Player_Action_95(Player* this, PlayState* play);

void Actor_Draw(PlayState* play, Actor* actor) {
    Lights* light;

    FrameInterpolation_RecordOpenChild(actor, 0);
    OPEN_DISPS(play->state.gfxCtx);

    light = LightContext_NewLights(&play->lightCtx, play->state.gfxCtx);
    if ((actor->flags & ACTOR_FLAG_UCODE_POINT_LIGHT_ENABLED) &&
        (play->roomCtx.curRoom.enablePosLights || (MREG(93) != 0))) {
        light->enablePosLights = true;
    }

    Lights_BindAll(light, play->lightCtx.listHead,
                   (actor->flags & (ACTOR_FLAG_UCODE_POINT_LIGHT_ENABLED | ACTOR_FLAG_IGNORE_LEGACY_POINT_LIGHTS))
                       ? NULL
                       : &actor->world.pos,
                   play);
    Lights_Draw(light, play->state.gfxCtx);

    // If the player is performing a Deku spin or entering a Deku flower, set it so that interpolation allows for >90
    // angle changes to be interpolated smoothly
    if (actor->id == ACTOR_PLAYER &&
        (((Player*)actor)->actionFunc == Player_Action_93 || ((Player*)actor)->actionFunc == Player_Action_95)) {
        FrameInterpolation_InterpolateWiderAngles();
    }

    FrameInterpolation_RecordActorPosRotMatrix();
    if (actor->flags & ACTOR_FLAG_IGNORE_QUAKE) {
        Matrix_SetTranslateRotateYXZ(actor->world.pos.x + play->mainCamera.quakeOffset.x,
                                     actor->world.pos.y +
                                         ((actor->shape.yOffset * actor->scale.y) + play->mainCamera.quakeOffset.y),
                                     actor->world.pos.z + play->mainCamera.quakeOffset.z, &actor->shape.rot);
    } else {
        Matrix_SetTranslateRotateYXZ(actor->world.pos.x, actor->world.pos.y + (actor->shape.yOffset * actor->scale.y),
                                     actor->world.pos.z, &actor->shape.rot);
    }

    Matrix_Scale(actor->scale.x, actor->scale.y, actor->scale.z, MTXMODE_APPLY);
    Actor_SetObjectDependency(play, actor);

    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[actor->objectSlot].segment);
    gSPSegment(POLY_XLU_DISP++, 0x06, play->objectCtx.slots[actor->objectSlot].segment);

    if (actor->colorFilterTimer != 0) {
        s32 colorFlag = COLORFILTER_GET_COLORFLAG(actor->colorFilterParams);
        Color_RGBA8 actorDefaultHitColor = { 0, 0, 0, 255 };

        if (colorFlag == COLORFILTER_COLORFLAG_GRAY) {
            actorDefaultHitColor.r = actorDefaultHitColor.g = actorDefaultHitColor.b =
                COLORFILTER_GET_COLORINTENSITY(actor->colorFilterParams) | 7;
        } else if (colorFlag == COLORFILTER_COLORFLAG_RED) {
            actorDefaultHitColor.r = COLORFILTER_GET_COLORINTENSITY(actor->colorFilterParams) | 7;
        } else if (colorFlag == COLORFILTER_COLORFLAG_NONE) {
            actorDefaultHitColor.b = actorDefaultHitColor.g = actorDefaultHitColor.r = 0;
        } else {
            actorDefaultHitColor.b = COLORFILTER_GET_COLORINTENSITY(actor->colorFilterParams) | 7;
        }

        if (actor->colorFilterParams & COLORFILTER_BUFFLAG_XLU) {
            func_800AE778(play, &actorDefaultHitColor, actor->colorFilterTimer,
                          COLORFILTER_GET_DURATION(actor->colorFilterParams));
        } else {
            func_800AE434(play, &actorDefaultHitColor, actor->colorFilterTimer,
                          COLORFILTER_GET_DURATION(actor->colorFilterParams));
        }
    }

    if (GameInteractor_ShouldActorDraw(actor)) {
        actor->draw(actor, play);
        GameInteractor_ExecuteOnActorDraw(actor);
    }

    if (actor->colorFilterTimer != 0) {
        if (actor->colorFilterParams & COLORFILTER_BUFFLAG_XLU) {
            func_800AE8EC(play);
        } else {
            func_800AE5A0(play);
        }
    }

    if (actor->shape.shadowDraw != NULL) {
        actor->shape.shadowDraw(actor, light, play);
    }
    actor->isDrawn = true;

    CLOSE_DISPS(play->state.gfxCtx);
    FrameInterpolation_RecordCloseChild();
}

void Actor_UpdateFlaggedAudio(Actor* actor) {
    s32 sfxId = actor->sfxId;

    if (sfxId != NA_SE_NONE) {
        if (actor->audioFlags & ACTOR_AUDIO_FLAG_SFX_ACTOR_POS_2) {
            AudioSfx_PlaySfx(sfxId, &actor->projectedPos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultReverb);
        } else if (actor->audioFlags & ACTOR_AUDIO_FLAG_SFX_CENTERED_1) {
            Audio_PlaySfx(sfxId);
        } else if (actor->audioFlags & ACTOR_AUDIO_FLAG_SFX_CENTERED_2) {
            Audio_PlaySfx_2(sfxId);
        } else if (actor->audioFlags & ACTOR_AUDIO_FLAG_SFX_TIMER) {
            Audio_PlaySfx_AtPosWithChannelIO(&gSfxDefaultPos, NA_SE_SY_TIMER - SFX_FLAG, (sfxId - 1));
        } else if (actor->audioFlags & ACTOR_AUDIO_FLAG_SFX_ACTOR_POS_1) {
            Audio_PlaySfx_AtPos(&actor->projectedPos, sfxId);
        }
    }

    //! FAKE:
    if (sfxId != NA_SE_NONE) {}

    if (actor->audioFlags & ACTOR_AUDIO_FLAG_SEQ_MUSIC_BOX_HOUSE) {
        Audio_PlaySequenceAtPos(SEQ_PLAYER_BGM_SUB, &actor->projectedPos, NA_BGM_MUSIC_BOX_HOUSE, 1500.0f);
    }

    if (actor->audioFlags & ACTOR_AUDIO_FLAG_SEQ_KAMARO_DANCE) {
        Audio_PlaySequenceAtPos(SEQ_PLAYER_BGM_MAIN, &actor->projectedPos, NA_BGM_KAMARO_DANCE, 900.0f);
    }
}

void Actor_ResetLensActors(PlayState* play) {
    play->actorCtx.numLensActors = 0;
    play->actorCtx.lensActorsDrawn = false;
}

s32 Actor_AddToLensActors(PlayState* play, Actor* actor) {
    if (play->actorCtx.numLensActors >= LENS_ACTOR_MAX) {
        return false;
    }

    play->actorCtx.lensActors[play->actorCtx.numLensActors] = actor;
    play->actorCtx.numLensActors++;
    return true;
}

void Actor_DrawLensOverlay(Gfx** gfxP, s32 lensMaskSize) {
    TransitionCircle_LoadAndSetTexture(gfxP, gCircleTex, G_IM_FMT_I, 0, 6, 6,
                                       ((LENS_MASK_ACTIVE_SIZE - lensMaskSize) * 0.003f) + 1.0f);
}

void Actor_DrawLensActors(PlayState* play, s32 numLensActors, Actor** lensActors) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    Actor** lensActor;
    s32 i;
    Gfx* gfx;
    Gfx* gfxTemp;
    void* zbuffer;
    void* spA4;
    s32 dbgVar1;
    s32 dbgVar2;
    s32 pad[2];

    // Remnant of debug
    dbgVar1 = true;
    // BENTODO: Disabling the framebufer effect for lens actors until we can restore effect for real
    dbgVar2 = false;

    if (dbgVar1) {
        dbgVar1 = (numLensActors > 0);
    }

    OPEN_DISPS(gfxCtx);

    if (dbgVar1) {
        gfx = POLY_XLU_DISP;
        zbuffer = gfxCtx->zbuffer;
        spA4 = play->unk_18E68;

        if (dbgVar2) {
            PreRender_SetValues(&play->pauseBgPreRender, gCfbWidth, gCfbHeight, gfxCtx->curFrameBuffer, zbuffer);

            gfxTemp = gfx;
            func_80170200(&play->pauseBgPreRender, &gfxTemp, zbuffer, spA4);
            gfx = gfxTemp;
        }

        gDPPipeSync(gfx++);
        gDPSetPrimDepth(gfx++, 0, 0);

        gDPSetOtherMode(gfx++,
                        G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                        G_AC_THRESHOLD | G_ZS_PRIM | Z_UPD | IM_RD | CVG_DST_SAVE | ZMODE_OPA | FORCE_BL |
                            GBL_c1(G_BL_CLR_BL, G_BL_0, G_BL_CLR_MEM, G_BL_1MA) |
                            GBL_c2(G_BL_CLR_BL, G_BL_0, G_BL_CLR_MEM, G_BL_1MA));

        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, 255);

        if (play->roomCtx.curRoom.lensMode == LENS_MODE_SHOW_ACTORS) {
            gDPSetCombineLERP(gfx++, 1, TEXEL0, PRIMITIVE, 0, 1, TEXEL0, PRIMITIVE, 0, 1, TEXEL0, PRIMITIVE, 0, 1,
                              TEXEL0, PRIMITIVE, 0);
        } else {
            gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        }

        gfxTemp = gfx;
        // BENTODO: Disabling actor masking until we can implement the framebuffer effect for real
        // Actor_DrawLensOverlay(&gfxTemp, play->actorCtx.lensMaskSize);
        gfx = Play_SetFog(play, gfxTemp);

        for (i = 0, lensActor = lensActors; i < numLensActors; i++, lensActor++) {
            POLY_XLU_DISP = gfx;
            // BENTODO: Since actor masking is diabled for actors that would normally
            // be "hidden", we are opting to just not render them at all
            if (play->roomCtx.curRoom.lensMode == LENS_MODE_SHOW_ACTORS) {
                Actor_Draw(play, *lensActor);
            }
            gfx = POLY_XLU_DISP;
        }

        if (dbgVar2) {
            gDPPipeSync(gfx++);

            gDPSetOtherMode(gfx++,
                            G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                                G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                            G_AC_THRESHOLD | G_ZS_PRIM | AA_EN | IM_RD | CVG_DST_WRAP | ZMODE_OPA | CVG_X_ALPHA |
                                ALPHA_CVG_SEL | FORCE_BL | GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_MEM, G_BL_1) |
                                GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_MEM, G_BL_1));

            gDPSetBlendColor(gfx++, 255, 255, 255, 0);
            gDPSetPrimColor(gfx++, 0, 0xFF, 0, 0, 0, 32);

            if (play->roomCtx.curRoom.lensMode == LENS_MODE_SHOW_ACTORS) {
                gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            } else {
                gDPSetCombineLERP(gfx++, 1, TEXEL0, PRIMITIVE, 0, 1, TEXEL0, PRIMITIVE, 0, 1, TEXEL0, PRIMITIVE, 0, 1,
                                  TEXEL0, PRIMITIVE, 0);
            }

            gDPSetColorImage(gfx++, G_IM_FMT_RGBA, G_IM_SIZ_16b, play->pauseBgPreRender.width, spA4);

            gfxTemp = gfx;
            Actor_DrawLensOverlay(&gfxTemp, play->actorCtx.lensMaskSize);
            gfx = gfxTemp;

            gDPPipeSync(gfx++);
            gDPSetBlendColor(gfx++, 255, 255, 255, 8);

            gDPSetColorImage(gfx++, G_IM_FMT_RGBA, G_IM_SIZ_16b, play->pauseBgPreRender.width,
                             play->pauseBgPreRender.fbuf);

            gfxTemp = gfx;
            PreRender_CopyImage(&play->pauseBgPreRender, &gfxTemp, spA4, zbuffer, true);
            gfx = gfxTemp;
        }

        POLY_XLU_DISP = gfx;
    }

    gfx = OVERLAY_DISP;

    gDPPipeSync(gfx++);

    gDPSetOtherMode(gfx++,
                    G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                    G_AC_THRESHOLD | G_ZS_PRIM | G_RM_CLD_SURF | G_RM_CLD_SURF2);

    gDPSetCombineLERP(gfx++, 1, TEXEL0, PRIMITIVE, 0, 1, TEXEL0, PRIMITIVE, 0, 1, TEXEL0, PRIMITIVE, 0, 1, TEXEL0,
                      PRIMITIVE, 0);
    gDPSetPrimColorOverride(gfx++, 0, 0, 74, 0, 0, 74, COSMETIC_ID("HUD.LensOverlay"));

    gfxTemp = gfx;
    Actor_DrawLensOverlay(&gfxTemp, play->actorCtx.lensMaskSize);
    OVERLAY_DISP = gfxTemp;

    CLOSE_DISPS(gfxCtx);
}

/**
 * Checks if an actor should be culled or not, by seeing if it is contained within its own culling volume.
 * For more details on the culling test, see `Actor_CullingVolumeTest`.
 *
 * Returns true if the actor is inside its culling volume. In other words, it should not cull.
 *
 * "Culling" in this context refers to the removal of something for the sake of improving performance.
 * For actors, being culled means that their Update and Draw processes are halted.
 * While halted, an Actor's update state is frozen and it will not draw, making it invisible.
 *
 * Actors that are within the bounds of their culling volume may update and draw, while actors that are
 * out of bounds of its culling volume may be excluded from updating and drawing until they are within bounds.
 *
 * It is possible for actors to opt out of update culling or draw culling.
 * This is set per-actor with `ACTOR_FLAG_UPDATE_CULLING_DISABLED` and `ACTOR_FLAG_DRAW_CULLING_DISABLED`.
 *
 * Note: Even if either `ACTOR_FLAG_UPDATE_CULLING_DISABLED` or `ACTOR_FLAG_DRAW_CULLING_DISABLED` are set, the actor
 * will still undergo the culling test and set `ACTOR_FLAG_INSIDE_CULLING_VOLUME` accordingly.
 * So, `ACTOR_FLAG_INSIDE_CULLING_VOLUME` cannot be used on it own to determine if an actor is actually culled.
 * It simply says whether or not they are physically located within the bounds of the culling volume.
 */
s32 Actor_CullingCheck(PlayState* play, Actor* actor) {
    return Actor_CullingVolumeTest(play, actor, &actor->projectedPos, actor->projectedW);
}

/**
 * Tests if an actor is currently within the bounds of its own culling volume.
 *
 * The culling volume is a 3D shape composed of a frustum with a box attached to the end of it. The frustum sits at the
 * camera's position and projects forward, encompassing the player's current view; the box extrudes behind the camera,
 * allowing actors in the immediate vicinity behind and to the sides of the camera to be detected.
 *
 * This function returns true if the actor is within bounds, false if not.
 * The comparison is done in projected space against the actor's projected position as the viewing frustum
 * in world space transforms to a box in projected space, making the calculation easy.
 *
 * Every actor can set properties for their own culling volume, changing its dimensions to suit the needs of
 * it and its environment. These properties are in units of projected space (i.e. compared to the actor's position
 * after perspective projection is applied) are therefore not directly comparable to world units.
 * These depend on the current view parameters (aspect, scale, znear, zfar).
 * The default parameters considered are (4/3, 1.0, 10, 12800).
 *
 *    cullingVolumeDistance: Configures how far forward the far plane of the frustum should extend.
 *                           This along with cullingVolumeScale determines the maximum distance from
 *                           the camera eye that the actor can be detected at. This quantity is related
 *                           to world units by a factor of
 *                                   (znear - zfar) / ((znear + zfar) * scale).
 *                           For default view parameters, increasing this property by 1 increases the
 *                           distance by ~0.995 world units.
 *
 *    cullingVolumeScale: Scales the entire culling volume in all directions except the downward
 *                        direction. Both the frustum and the box will scale in size. This quantity is
 *                        related to world units by different factors based on direction:
 *                         - For the forward and backward directions, they are related in the same way
 *                           as above. For default view parameters, increasing this property by 1 increases
 *                           the forward and backward scales by ~0.995 world units.
 *                         - For the sideways directions, the relation to world units is
 *                                   (aspect / scale) * sqrt(3)/3
 *                           For default view parameters, increasing this property by 1 increases the
 *                           sideways scales by ~0.77 world units.
 *                         - For the upward direction, the relation to world units is
 *                                   (1 / scale) * sqrt(3)/3
 *                           For default view parameters, increasing this property by 1 increases the
 *                           scale by ~0.58 world units.
 *
 *    cullingVolumeDownward: Sets the height of the culling volume in the downward direction. Increasing
 *                           this value will make actors below the camera more easily detected. This
 *                           quantity is related to world units by the same factor as the upward scale.
 *                           For default view parameters, increasing this property by 1 increases the
 *                           downward height by ~0.58 world units.
 *
 * This interactive 3D graph visualizes the shape of the culling volume and has sliders for the 3 properties mentioned
 * above: https://www.desmos.com/3d/4ztkxqky2a.
 */
s32 Actor_CullingVolumeTest(PlayState* play, Actor* actor, Vec3f* projPos, f32 projW) {
    if ((projPos->z > -actor->cullingVolumeScale) &&
        (projPos->z < (actor->cullingVolumeDistance + actor->cullingVolumeScale))) {
        f32 invW;
        f32 cullingVolumeScaleX;
        f32 cullingVolumeScaleY;
        f32 cullingVolumeDownward;

        // Clamping `projW` affects points behind the camera, so that the culling volume has
        // a frustum shape in front of the camera and a box shape behind the camera.
        invW = CLAMP_MIN(projW, 1.0f);

        if (play->view.fovy != 60.0f) {
            // If the fov isn't 60 degrees, make the cull parameters behave as if it were 60 degrees.
            // To do this, multiply by the ratios of the x and y diagonal elements of the projection matrix.
            // The x diagonal element is cot(0.5 * fov) / aspect and the y diagonal element is just cot(0.5 * fov).
            // When the fov is 60 degrees, cot(0.5 * 60 degrees) = sqrt(3) so the x element is 3sqrt(3)/4 and the y
            // element is sqrt(3). The current diagonal element divided by (or multiplied by their inverse) gives
            // the ratio.

            cullingVolumeScaleX = actor->cullingVolumeScale * play->projectionMtxFDiagonal.x *
                                  0.76980036f; // sqrt(16/27) = aspect / cot(0.5 * f) = (4/3) / sqrt(3)

            cullingVolumeDownward = play->projectionMtxFDiagonal.y * 0.57735026f; // 1 / sqrt(3) = 1 / cot(0.5 * f)
            cullingVolumeScaleY = actor->cullingVolumeScale * cullingVolumeDownward;
            cullingVolumeDownward *= actor->cullingVolumeDownward;
        } else {
            cullingVolumeScaleY = cullingVolumeScaleX = actor->cullingVolumeScale;
            cullingVolumeDownward = actor->cullingVolumeDownward;
        }

        if (((fabsf(projPos->x) - cullingVolumeScaleX) < invW) && ((-invW < (projPos->y + cullingVolumeDownward))) &&
            ((projPos->y - cullingVolumeScaleY) < invW)) {
            return true;
        }
    }

    return false;
}

// #region 2S2H [Enhancements] Allows us to increase the draw and update distance independently, mostly a modified
// version of the function above
s32 Ship_CalcShouldDrawAndUpdate(PlayState* play, Actor* actor, Vec3f* projectedPos, f32 projectedW, bool* shouldDraw,
                                 bool* shouldUpdate) {
    // Check if the actor passes its original/vanilla culling requirements
    if (Actor_CullingVolumeTest(play, actor, projectedPos, projectedW)) {
        *shouldUpdate = true;
        *shouldDraw = true;
        return true;
    }

    s32 distMultiplier = CVarGetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance", 1);
    distMultiplier = MAX(distMultiplier, 1);

    // Apply distance scale to forward cullzone check
    bool isWithingForwardCullZone =
        (-actor->cullingVolumeScale < projectedPos->z) &&
        (projectedPos->z < ((actor->cullingVolumeDistance + actor->cullingVolumeScale) * distMultiplier));

    if (isWithingForwardCullZone) {
        // Ensure the projected W value is at least 1.0
        f32 clampedProjectedW = CLAMP_MIN(projectedW, 1.0f);
        f32 aspectMultiplier = 1.0f;
        f32 cullingVolumeScaleDiagonal;
        f32 cullingVolumeScaleVertical;
        f32 cullingVolumeDownwardAdjusted;

        // Adjust calculations if the field of view is not the default 60 degrees
        if (play->view.fovy != 60.0f) {
            cullingVolumeScaleDiagonal =
                actor->cullingVolumeScale * play->projectionMtxFDiagonal.x * 0.76980036f; // sqrt(16/27)

            cullingVolumeScaleVertical = play->projectionMtxFDiagonal.y * 0.57735026f; // 1 / sqrt(3)
            cullingVolumeDownwardAdjusted = actor->cullingVolumeScale * cullingVolumeScaleVertical;
            cullingVolumeScaleVertical *= actor->cullingVolumeDownward;
        } else {
            cullingVolumeDownwardAdjusted = cullingVolumeScaleDiagonal = actor->cullingVolumeScale;
            cullingVolumeScaleVertical = actor->cullingVolumeDownward;
        }

        if (CVarGetInteger("gEnhancements.Graphics.ActorCullingAccountsForWidescreen", 0)) {
            aspectMultiplier = Ship_GetExtendedAspectRatioMultiplier();
        }

        // Apply adjsuted aspect ratio to just the horizontal cullzone check
        bool isWithinHorizontalCullZone =
            ((fabsf(projectedPos->x) - cullingVolumeScaleDiagonal) < (clampedProjectedW * aspectMultiplier));
        bool isAboveBottomOfCullZone = ((-clampedProjectedW < (projectedPos->y + cullingVolumeScaleVertical)));
        bool isBelowTopOfCullZone = ((projectedPos->y - cullingVolumeDownwardAdjusted) < clampedProjectedW);

        if (isWithinHorizontalCullZone && isAboveBottomOfCullZone && isBelowTopOfCullZone) {
            // Add additional overries here for glitch useful actors when those are reported

            *shouldDraw = true;
            *shouldUpdate = true;
            return true;
        }
    }

    return false;
}
// #endregion

void Actor_DrawAll(PlayState* play, ActorContext* actorCtx) {
    s32 pad[2];
    Gfx* ref2;
    Gfx* tmp2;
    s32 pad2;
    Gfx* sp58;
    ActorListEntry* actorEntry;
    Actor* actor;
    s32 drawActorFlagsMask;
    s32 category;

    if (play->soaringCsOrSoTCsPlaying) {
        drawActorFlagsMask = ACTOR_FLAG_UPDATE_DURING_SOARING_AND_SOT_CS;
    } else {
        drawActorFlagsMask = ACTOR_FLAG_UPDATE_DURING_SOARING_AND_SOT_CS | ACTOR_FLAG_INSIDE_CULLING_VOLUME |
                             ACTOR_FLAG_DRAW_CULLING_DISABLED;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Actor_ResetLensActors(play);

    sp58 = POLY_XLU_DISP;
    POLY_XLU_DISP = &sp58[1];

    for (category = 0, actorEntry = actorCtx->actorLists; category < ACTORCAT_MAX; category++, actorEntry++) {
        actor = actorEntry->first;

        while (actor != NULL) {
            SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &actor->world.pos, &actor->projectedPos,
                                         &actor->projectedW);

            if (actor->audioFlags & ACTOR_AUDIO_FLAG_ALL) {
                Actor_UpdateFlaggedAudio(actor);
            }

            // #region 2S2H [Enhancement] Extended culling updates
            bool shipShouldDraw = false;
            bool shipShouldUpdate = false;
            if (CVarGetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance", 1) > 1 ||
                CVarGetInteger("gEnhancements.Graphics.ActorCullingAccountsForWidescreen", 0)) {
                Ship_CalcShouldDrawAndUpdate(play, actor, &actor->projectedPos, actor->projectedW, &shipShouldDraw,
                                             &shipShouldUpdate);

                if (shipShouldUpdate) {
                    actor->flags |= ACTOR_FLAG_INSIDE_CULLING_VOLUME;
                } else {
                    actor->flags &= ~ACTOR_FLAG_INSIDE_CULLING_VOLUME;
                }
            } else {
                if (Actor_CullingCheck(play, actor)) {
                    actor->flags |= ACTOR_FLAG_INSIDE_CULLING_VOLUME;
                } else {
                    actor->flags &= ~ACTOR_FLAG_INSIDE_CULLING_VOLUME;
                }
            }

            // Copied flags so we can set the "is active" flag for the draw check below without modifying the actor.
            // This ensures that overrides for song of soaring or song of time cutscenes still hide actors.
            s32 shipActorFlagsCopy = actor->flags;
            if (shipShouldDraw) {
                shipActorFlagsCopy |= ACTOR_FLAG_INSIDE_CULLING_VOLUME;
            }

            actor->isDrawn = false;
            if ((actor->init == NULL) && (actor->draw != NULL) && (shipActorFlagsCopy & drawActorFlagsMask)) {
                // #endregion
                if ((actor->flags & ACTOR_FLAG_REACT_TO_LENS) &&
                    ((play->roomCtx.curRoom.lensMode == LENS_MODE_SHOW_ACTORS) ||
                     (play->actorCtx.lensMaskSize == LENS_MASK_ACTIVE_SIZE) ||
                     (actor->room != play->roomCtx.curRoom.num))) {
                    if (Actor_AddToLensActors(play, actor)) {}
                } else {
                    Actor_Draw(play, actor);
                }
            }

            actor = actor->next;
        }
    }

    Effect_DrawAll(play->state.gfxCtx);
    EffectSs_DrawAll(play);
    EffFootmark_Draw(play);

    ref2 = POLY_XLU_DISP;
    gSPDisplayList(sp58, &ref2[1]);
    POLY_XLU_DISP = &ref2[1];

    if (play->actorCtx.lensActive) {
        Math_StepToC(&play->actorCtx.lensMaskSize, LENS_MASK_ACTIVE_SIZE, 20);
        if (GET_PLAYER(play)->stateFlags2 & PLAYER_STATE2_USING_OCARINA) {
            Actor_DeactivateLens(play);
        }
    } else {
        Math_StepToC(&play->actorCtx.lensMaskSize, 0, 10);
    }

    if (play->actorCtx.lensMaskSize != 0) {
        play->actorCtx.lensActorsDrawn = true;
        Actor_DrawLensActors(play, play->actorCtx.numLensActors, play->actorCtx.lensActors);
    }

    tmp2 = POLY_XLU_DISP;
    gSPEndDisplayList(&tmp2[0]);
    gSPBranchList(ref2, &tmp2[1]);
    POLY_XLU_DISP = &tmp2[1];

    if (!play->soaringCsOrSoTCsPlaying) {
        Lights_DrawGlow(play);
    }

    TitleCard_Draw(&play->state, &actorCtx->titleCtx);

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Kill every actor which depends on an object that is not loaded.
 */
void Actor_KillAllWithMissingObject(PlayState* play, ActorContext* actorCtx) {
    Actor* actor;
    s32 category;

    for (category = 0; category < ACTORCAT_MAX; category++) {
        actor = actorCtx->actorLists[category].first;

        while (actor != NULL) {
            if (!Object_IsLoaded(&play->objectCtx, actor->objectSlot)) {
                Actor_Kill(actor);
            }

            actor = actor->next;
        }
    }
}

/**
 * Kill actors on room change and update flags accordingly
 */
void func_800BA798(PlayState* play, ActorContext* actorCtx) {
    Actor* actor;
    s32 category;

    for (category = 0; category < ACTORCAT_MAX; category++) {
        actor = actorCtx->actorLists[category].first;

        while (actor != NULL) {
            if ((actor->room >= 0) && (actor->room != play->roomCtx.curRoom.num) &&
                (actor->room != play->roomCtx.prevRoom.num)) {
                if (!actor->isDrawn) {
                    actor = Actor_Delete(actorCtx, actor, play);
                } else {
                    Actor_Kill(actor);
                    Actor_Destroy(actor, play);
                    actor = actor->next;
                }
            } else {
                actor = actor->next;
            }
        }
    }

    CollisionCheck_ClearContext(play, &play->colChkCtx);
    actorCtx->sceneFlags.clearedRoomTemp = 0;
    actorCtx->sceneFlags.switches[3] = 0;
    actorCtx->sceneFlags.collectible[3] = 0;
    play->msgCtx.unk_12030 = 0;
}

/**
 * Kill every actor which does not have the current halfDayBit enabled
 */
void Actor_KillAllOnHalfDayChange(PlayState* play, ActorContext* actorCtx) {
    s32 category;

    for (category = 0; category < ACTORCAT_MAX; category++) {
        Actor* actor = actorCtx->actorLists[category].first;

        while (actor != NULL) {
            if (!(actor->halfDaysBits & actorCtx->halfDaysBit)) {
                func_80123590(play, actor);

                if (!actor->isDrawn) {
                    actor = Actor_Delete(actorCtx, actor, play);
                } else {
                    Actor_Kill(actor);
                    Actor_Destroy(actor, play);
                    actor = actor->next;
                }
            } else {
                actor = actor->next;
            }
        }
    }

    CollisionCheck_ClearContext(play, &play->colChkCtx);
    play->msgCtx.unk_12030 = 0;
}

void Actor_CleanupContext(ActorContext* actorCtx, PlayState* play) {
    s32 category;

    // Fault_RemoveClient(&sActorFaultClient);

    for (category = 0; category < ACTORCAT_MAX; category++) {
        if (category != ACTORCAT_PLAYER) {
            Actor* actor = actorCtx->actorLists[category].first;

            while (actor != NULL) {
                Actor_Delete(actorCtx, actor, play);
                actor = actorCtx->actorLists[category].first;
            }
        }
    }

    while (actorCtx->actorLists[ACTORCAT_PLAYER].first != NULL) {
        Actor_Delete(actorCtx, actorCtx->actorLists[ACTORCAT_PLAYER].first, play);
    }

    if (actorCtx->absoluteSpace != NULL) {
        ZeldaArena_Free(actorCtx->absoluteSpace);
        actorCtx->absoluteSpace = NULL;
    }

    Play_SaveCycleSceneFlags(play);
    ActorOverlayTable_Cleanup();
}

/**
 * Adds a given actor instance at the front of the actor list of the specified category.
 * Also sets the actor instance as being of that category.
 */
void Actor_AddToCategory(ActorContext* actorCtx, Actor* actor, u8 actorCategory) {
    Actor* actorAux;
    Actor* lastActor;

    actor->category = actorCategory;

    actorCtx->totalLoadedActors++;
    actorCtx->actorLists[actorCategory].length++;
    lastActor = actorCtx->actorLists[actorCategory].first;

    if (lastActor == NULL) {
        actorCtx->actorLists[actorCategory].first = actor;
        return;
    }

    actorAux = lastActor->next;
    while (actorAux != NULL) {
        lastActor = actorAux;
        actorAux = actorAux->next;
    }

    lastActor->next = actor;
    actor->prev = lastActor;
}

/**
 * Removes a given actor instance from its actor list.
 * Also sets the temp clear flag of the current room if the actor removed was the last enemy loaded.
 */
Actor* Actor_RemoveFromCategory(PlayState* play, ActorContext* actorCtx, Actor* actorToRemove) {
    Actor* newHead;

    actorCtx->totalLoadedActors--;
    actorCtx->actorLists[actorToRemove->category].length--;

    if (actorToRemove->prev != NULL) {
        actorToRemove->prev->next = actorToRemove->next;
    } else {
        actorCtx->actorLists[actorToRemove->category].first = actorToRemove->next;
    }

    newHead = actorToRemove->next;

    if (newHead != NULL) {
        newHead->prev = actorToRemove->prev;
    }

    actorToRemove->next = NULL;
    actorToRemove->prev = NULL;

    if ((actorToRemove->room == play->roomCtx.curRoom.num) && (actorToRemove->category == ACTORCAT_ENEMY) &&
        (actorCtx->actorLists[ACTORCAT_ENEMY].length == 0)) {
        Flags_SetClearTemp(play, play->roomCtx.curRoom.num);
    }

    return newHead;
}

void Actor_FreeOverlay(ActorOverlay* entry) {
    if (entry->numLoaded == 0) {
        // #region 2S2H [Port] Added reset function is called when loaded count is 0
        // to clean up static variables
        if (entry->profile->reset != NULL) {
            entry->profile->reset();
        }
        // #endregion

        // 2S2H [Port] ramAddr will always be NULL for the port, so this block will never run
        void* ramAddr = entry->loadedRamAddr;

        if (ramAddr != NULL) {
            if (!(entry->allocType & ALLOCTYPE_PERMANENT)) {
                if (entry->allocType & ALLOCTYPE_ABSOLUTE) {
                    entry->loadedRamAddr = NULL;
                } else {
                    ZeldaArena_Free(ramAddr);
                    entry->loadedRamAddr = NULL;
                }
            }
        }
    }
}

Actor* Actor_Spawn(ActorContext* actorCtx, PlayState* play, s16 actorId, f32 posX, f32 posY, f32 posZ, s16 rotX,
                   s16 rotY, s16 rotZ, s32 params) {
    return Actor_SpawnAsChildAndCutscene(actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params,
                                         CS_ID_NONE, HALFDAYBIT_ALL, NULL);
}

ActorProfile* Actor_LoadOverlay(ActorContext* actorCtx, s16 index) {
    size_t overlaySize;
    ActorOverlay* overlayEntry = &gActorOverlayTable[index];
    ActorProfile* profile;

    overlaySize = (uintptr_t)overlayEntry->vramEnd - (uintptr_t)overlayEntry->vramStart;

    // 2S2H [Port] vramStart will always be NULL in the port, so the else block is never run
    if (overlayEntry->vramStart == NULL) {
        profile = overlayEntry->profile;
    } else {
        if (overlayEntry->loadedRamAddr == NULL) {
            if (overlayEntry->allocType & ALLOCTYPE_ABSOLUTE) {
                if (actorCtx->absoluteSpace == NULL) {
                    actorCtx->absoluteSpace = ZeldaArena_MallocR(0xFFFFFF);
                }
                gActorOverlayTable[index].loadedRamAddr = actorCtx->absoluteSpace;
            } else if (overlayEntry->allocType & ALLOCTYPE_PERMANENT) {
                gActorOverlayTable[index].loadedRamAddr = ZeldaArena_MallocR(overlaySize);
            } else {
                gActorOverlayTable[index].loadedRamAddr = ZeldaArena_Malloc(overlaySize);
            }

            if (overlayEntry->loadedRamAddr == NULL) {
                return NULL;
            }

            Overlay_Load(overlayEntry->vromStart, overlayEntry->vromEnd, overlayEntry->vramStart, overlayEntry->vramEnd,
                         overlayEntry->loadedRamAddr);
            overlayEntry->numLoaded = 0;
        }

        profile = (void*)(uintptr_t)((overlayEntry->profile != NULL)
                                         ? (void*)((uintptr_t)overlayEntry->profile -
                                                   (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                              (uintptr_t)overlayEntry->loadedRamAddr))
                                         : NULL);
    }

    return profile;
}

Actor* Actor_SpawnAsChildAndCutscene(ActorContext* actorCtx, PlayState* play, s16 index, f32 x, f32 y, f32 z, s16 rotX,
                                     s16 rotY, s16 rotZ, s32 params, u32 csId, u32 halfDaysBits, Actor* parent) {
    s32 pad;
    Actor* actor;
    ActorProfile* profile;
    s32 objectSlot;
    ActorOverlay* overlayEntry;

    if (actorCtx->totalLoadedActors >= 255) {
        return NULL;
    }

    profile = Actor_LoadOverlay(actorCtx, index);
    if (profile == NULL) {
        return NULL;
    }

    objectSlot = Object_GetSlot(&play->objectCtx, profile->objectId);
    if ((objectSlot <= OBJECT_SLOT_NONE) ||
        ((profile->type == ACTORCAT_ENEMY) && Flags_GetClear(play, play->roomCtx.curRoom.num) &&
         (profile->id != ACTOR_BOSS_05))) {
        Actor_FreeOverlay(&gActorOverlayTable[index]);
        return NULL;
    }

    actor = ZeldaArena_Malloc(profile->instanceSize);
    if (actor == NULL) {
        Actor_FreeOverlay(&gActorOverlayTable[index]);
        return NULL;
    }

    // #region 2S2H [ObjectExtension]
    SetActorListIndex(actor, currentActorListIndex);
    currentActorListIndex = -1;
    // #endregion

    overlayEntry = &gActorOverlayTable[index];
    // #region 2S2H [Port] Our actors are always loaded and have no vramStart
    // but we want to simulate them being unloaded to execute our actor reset funcs
    // for static variable cleanup
    // if (overlayEntry->vramStart != NULL) {
    if (true) {
        // #endregion
        overlayEntry->numLoaded++;
    }

    memset(actor, 0, profile->instanceSize);
    actor->overlayEntry = overlayEntry;
    actor->id = profile->id;
    actor->flags = profile->flags;

    if (profile->id == ACTOR_EN_PART) {
        actor->objectSlot = rotZ;
        rotZ = 0;
    } else {
        actor->objectSlot = objectSlot;
    }

    actor->init = profile->init;
    actor->destroy = profile->destroy;
    actor->update = profile->update;
    actor->draw = profile->draw;

    if (parent != NULL) {
        actor->room = parent->room;
        actor->parent = parent;
        parent->child = actor;
    } else {
        actor->room = play->roomCtx.curRoom.num;
    }

    actor->home.pos.x = x;
    actor->home.pos.y = y;
    actor->home.pos.z = z;
    actor->home.rot.x = rotX;
    actor->home.rot.y = rotY;
    actor->home.rot.z = rotZ;
    actor->params = params & 0xFFFF;
    actor->csId = csId & 0x7F;

    if (actor->csId == 0x7F) {
        actor->csId = CS_ID_NONE;
    }

    if (halfDaysBits != 0) {
        actor->halfDaysBits = halfDaysBits;
    } else {
        actor->halfDaysBits = HALFDAYBIT_ALL;
    }

    Actor_AddToCategory(actorCtx, actor, profile->type);

    {
        uintptr_t sp20 = gSegments[6];

        Actor_Init(actor, play);
        gSegments[6] = sp20;
    }

    return actor;
}

Actor* Actor_SpawnAsChild(ActorContext* actorCtx, Actor* parent, PlayState* play, s16 actorId, f32 posX, f32 posY,
                          f32 posZ, s16 rotX, s16 rotY, s16 rotZ, s32 params) {
    return Actor_SpawnAsChildAndCutscene(actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params,
                                         CS_ID_NONE, parent->halfDaysBits, parent);
}

void Actor_SpawnTransitionActors(PlayState* play, ActorContext* actorCtx) {
    TransitionActorEntry* transitionActorList = play->transitionActors.list;
    s32 i;
    s16 numTransitionActors = play->transitionActors.count;

    for (i = 0; i < numTransitionActors; transitionActorList++, i++) {
        if (transitionActorList->id >= 0) {
            if (((transitionActorList->sides[0].room >= 0) &&
                 ((play->roomCtx.curRoom.num == transitionActorList->sides[0].room) ||
                  (play->roomCtx.prevRoom.num == transitionActorList->sides[0].room))) ||
                ((transitionActorList->sides[1].room >= 0) &&
                 ((play->roomCtx.curRoom.num == transitionActorList->sides[1].room) ||
                  (play->roomCtx.prevRoom.num == transitionActorList->sides[1].room)))) {
                s16 rotY = DEG_TO_BINANG((transitionActorList->rotY >> 7) & 0x1FF);

                if (Actor_SpawnAsChildAndCutscene(actorCtx, play, transitionActorList->id & 0x1FFF,
                                                  transitionActorList->pos.x, transitionActorList->pos.y,
                                                  transitionActorList->pos.z, 0, rotY, 0,
                                                  TRANSITION_ACTOR_PARAMS(i, transitionActorList->params),
                                                  transitionActorList->rotY & 0x7F, HALFDAYBIT_ALL, 0) != NULL) {
                    transitionActorList->id = -transitionActorList->id;
                }
                numTransitionActors = play->transitionActors.count;
            }
        }
    }
}

Actor* Actor_SpawnEntry(ActorContext* actorCtx, ActorEntry* actorEntry, PlayState* play) {
    s16 rotX = (actorEntry->rot.x >> 7) & 0x1FF;
    s16 rotY = (actorEntry->rot.y >> 7) & 0x1FF;
    s16 rotZ = (actorEntry->rot.z >> 7) & 0x1FF;

    if (!(actorEntry->id & 0x8000)) {
        rotY = DEG_TO_BINANG(rotY);
    } else if (rotY > 180) {
        rotY -= 360;
    }

    if (!(actorEntry->id & 0x4000)) {
        rotX = DEG_TO_BINANG(rotX);
    } else if (rotX > 180) {
        rotX -= 360;
    }

    if (!(actorEntry->id & 0x2000)) {
        rotZ = DEG_TO_BINANG(rotZ);
    } else if (rotZ > 180) {
        rotZ -= 360;
    }

    return Actor_SpawnAsChildAndCutscene(actorCtx, play, actorEntry->id & 0x1FFF, actorEntry->pos.x, actorEntry->pos.y,
                                         actorEntry->pos.z, rotX, rotY, rotZ, actorEntry->params & 0xFFFF,
                                         actorEntry->rot.y & 0x7F,
                                         ((actorEntry->rot.x & 7) << 7) | (actorEntry->rot.z & 0x7F), NULL);
}

Actor* Actor_Delete(ActorContext* actorCtx, Actor* actor, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Actor* newHead;
    ActorOverlay* overlayEntry = actor->overlayEntry;

    // Execute before actor memory is freed
    GameInteractor_ExecuteOnActorDestroy(actor);

    if ((player != NULL) && (actor == player->focusActor)) {
        Player_ReleaseLockOn(player);
        Camera_ChangeMode(Play_GetCamera(play, Play_GetActiveCamId(play)), CAM_MODE_NORMAL);
    }

    if (actor == actorCtx->attention.tatlHoverActor) {
        actorCtx->attention.tatlHoverActor = NULL;
    }

    if (actor == actorCtx->attention.forcedLockOnActor) {
        actorCtx->attention.forcedLockOnActor = NULL;
    }

    if (actor == actorCtx->attention.bgmEnemy) {
        actorCtx->attention.bgmEnemy = NULL;
    }

    AudioSfx_StopByPos(&actor->projectedPos);
    Actor_Destroy(actor, play);

    newHead = Actor_RemoveFromCategory(play, actorCtx, actor);

    // #region 2S2H [ObjectExtension]
    ObjectExtension_Free(actor);
    // #endregion

    ZeldaArena_Free(actor);

    // #region 2S2H [Port] Our actors are always loaded and have no vramStart
    // but we want to simulate them being unloaded to execute our actor reset funcs
    // for static variable cleanup
    // if (overlayEntry->vramStart != NULL) {
    if (true) {
        // #endregion
        overlayEntry->numLoaded--;
        Actor_FreeOverlay(overlayEntry);
    }

    return newHead;
}

/**
 * Checks that an actor is on-screen enough to be considered an attention actor.
 *
 * Note that the screen bounds checks are larger than the actual screen region
 * to give room for error.
 */
bool Attention_ActorOnScreen(PlayState* play, Actor* actor) {
    s16 x;
    s16 y;

    Actor_GetScreenPos(play, actor, &x, &y);

#define X_LEEWAY 20
#define Y_LEEWAY 160

    return (x > (0 - X_LEEWAY)) && (x < (gScreenWidth + X_LEEWAY)) && (y > (0 - Y_LEEWAY)) &&
           (y < (gScreenHeight + Y_LEEWAY));
}

/**
 * Search for attention actors or camera drift actors within the specified category.
 *
 * To be considered an attention actor the actor needs to:
 * - Have a non-NULL update function (still active)
 * - Not be player (this is technically a redundant check because the PLAYER category is never searched)
 * - Have `ACTOR_FLAG_ATTENTION_ENABLED` or `ACTOR_FLAG_FOCUS_ACTOR_REFINDABLE` set
 * - Not be the current focus actor unless `ACTOR_FLAG_FOCUS_ACTOR_REFINDABLE` is set
 * - Be the closest attention actor found so far
 * - Be within range, specified by attentionRangeType
 * - Be roughly on-screen
 * - Not be blocked by a surface
 *
 * If an actor has a priority value set and the value is the lowest found so far, it will be set as the prioritized
 * attention actor. Otherwise, it is set as the nearest attention actor or camera drift actor.
 *
 * This function is expected to be called with almost every actor category in each cycle. On a new cycle its global
 * variables must be reset by the caller, otherwise the information of the previous cycle will be retained.
 */
void Attention_FindActorInCategory(PlayState* play, ActorContext* actorCtx, Player* player, ActorType actorCategory) {
    f32 distSq;
    Actor* actor = actorCtx->actorLists[actorCategory].first;
    Actor* playerFocusActor = player->focusActor;
    s32 isNearestAttentionActor;
    s32 isNearestCameraDriftActor;

    for (; actor != NULL; actor = actor->next) {
        if ((actor->update == NULL) || ((Player*)actor == player)) {
            continue;
        }

        if (!(actor->flags & (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_CAMERA_DRIFT_ENABLED))) {
            continue;
        }

        // Determine the closest enemy actor to player within a range. Used for playing enemy background music.
        if ((actorCategory == ACTORCAT_ENEMY) &&
            CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)) {
            if ((actor->xyzDistToPlayerSq < SQ(500.0f)) && (actor->xyzDistToPlayerSq < sBgmEnemyDistSq)) {
                actorCtx->attention.bgmEnemy = actor;
                sBgmEnemyDistSq = actor->xyzDistToPlayerSq;
            }
        }

        // Ignore the current focus actor unless it is refindable
        if ((actor == playerFocusActor) && !(actor->flags & ACTOR_FLAG_FOCUS_ACTOR_REFINDABLE)) {
            continue;
        }

        distSq = Attention_WeightedDistToPlayerSq(actor, player, sAttentionPlayerRotY);

        isNearestAttentionActor =
            (actor->flags & ACTOR_FLAG_ATTENTION_ENABLED) && (distSq < sNearestAttentionActorDistSq);

        isNearestCameraDriftActor =
            (actor->flags & ACTOR_FLAG_CAMERA_DRIFT_ENABLED) && (distSq < sNearestCameraDriftActorDistSq);

        if (!isNearestAttentionActor && !isNearestCameraDriftActor) {
            continue;
        }

        if (Attention_ActorIsInRange(actor, distSq) && Attention_ActorOnScreen(play, actor)) {
            CollisionPoly* poly;
            s32 bgId;
            Vec3f lineTestResultPos;

            if (BgCheck_CameraLineTest1(&play->colCtx, &player->actor.focus.pos, &actor->focus.pos, &lineTestResultPos,
                                        &poly, true, true, true, true, &bgId)) {
                if (!SurfaceType_IsIgnoredByProjectiles(&play->colCtx, poly, bgId)) {
                    continue;
                }
            }

            if (actor->targetPriority != 0) {
                if (isNearestAttentionActor && (actor->targetPriority < sHighestAttentionPriority)) {
                    sPrioritizedAttentionActor = actor;
                    sHighestAttentionPriority = actor->targetPriority;
                }
                if (isNearestCameraDriftActor && (actor->targetPriority < sHighestCameraDriftPriority)) {
                    sPrioritizedCameraDriftActor = actor;
                    sHighestCameraDriftPriority = actor->targetPriority;
                }
            } else {
                if (isNearestAttentionActor) {
                    sNearestAttentionActor = actor;
                    sNearestAttentionActorDistSq = distSq;
                }
                if (isNearestCameraDriftActor) {
                    sNearestCameraDriftActor = actor;
                    sNearestCameraDriftActorDistSq = distSq;
                }
            }
        }
    }
}

u8 sAttentionCategorySearchOrder[] = {
    ACTORCAT_BOSS,  ACTORCAT_ENEMY,  ACTORCAT_BG,   ACTORCAT_EXPLOSIVES, ACTORCAT_NPC,  ACTORCAT_ITEMACTION,
    ACTORCAT_CHEST, ACTORCAT_SWITCH, ACTORCAT_PROP, ACTORCAT_MISC,       ACTORCAT_DOOR, ACTORCAT_SWITCH,
};

/**
 * Search for the nearest attention actor and camera drift actor by iterating through most actor categories.
 * See `Attention_FindActorInCategory` for more details on search criteria.
 *
 * The attention actor found is stored in the `attentionActorP` parameter, while the camera drift actor is stored in
 * `cameraDriftActorP` They may be NULL if no actor that fulfills the criteria is found.
 */
void Attention_FindActor(PlayState* play, ActorContext* actorCtx, Actor** attentionActorP, Actor** cameraDriftActorP,
                         Player* player) {
    u8* category;
    s32 i;

    sNearestAttentionActor = sPrioritizedAttentionActor = sNearestCameraDriftActor = sPrioritizedCameraDriftActor =
        NULL;
    sNearestAttentionActorDistSq = sNearestCameraDriftActorDistSq = sBgmEnemyDistSq = FLT_MAX;
    sHighestAttentionPriority = sHighestCameraDriftPriority = INT32_MAX;

    actorCtx->attention.bgmEnemy = NULL;
    sAttentionPlayerRotY = player->actor.shape.rot.y;

    category = sAttentionCategorySearchOrder;

    // Search the first 3 actor categories first for an attention actor
    // These are Boss, Enemy, and Bg, in order.
    for (i = 0; i < 3; i++) {
        Attention_FindActorInCategory(play, actorCtx, player, *category);
        category++;
    }

    // If no actor in the above categories was found, then try searching in the remaining categories
    if (sNearestAttentionActor == NULL) {
        for (; i < ARRAY_COUNT(sAttentionCategorySearchOrder); i++) {
            Attention_FindActorInCategory(play, actorCtx, player, *category);
            category++;
        }
    }

    if (sNearestAttentionActor == NULL) {
        *attentionActorP = sPrioritizedAttentionActor;
    } else {
        *attentionActorP = sNearestAttentionActor;
    }

    if (sNearestCameraDriftActor == NULL) {
        *cameraDriftActorP = sPrioritizedCameraDriftActor;
    } else {
        *cameraDriftActorP = sNearestCameraDriftActor;
    }
}

/**
 * Play the death sound effect and flash the screen white for 4 frames.
 * While the screen flashes, the game freezes.
 */
void Enemy_StartFinishingBlow(PlayState* play, Actor* actor) {
    play->actorCtx.freezeFlashTimer = 5;
    SoundSource_PlaySfxAtFixedWorldPos(play, &actor->world.pos, 20, NA_SE_EN_LAST_DAMAGE);
}

/**
 * Updates `FaceChange` data for a blinking pattern.
 * This system expects that the actor using the system has defined 3 faces in this exact order:
 * "eyes open", "eyes half open", "eyes closed".
 *
 * @param faceChange  pointer to an actor's faceChange data
 * @param blinkIntervalBase  The base number of frames between blinks
 * @param blinkIntervalRandRange  The range for a random number of frames that can be added to `blinkIntervalBase`
 * @param blinkDuration  The number of frames it takes for a single blink to occur
 */
s16 FaceChange_UpdateBlinking(FaceChange* faceChange, s16 blinkIntervalBase, s16 blinkIntervalRandRange,
                              s16 blinkDuration) {
    if (DECR(faceChange->timer) == 0) {
        faceChange->timer = Rand_S16Offset(blinkIntervalBase, blinkIntervalRandRange);
    }

    if (faceChange->timer - blinkDuration > 0) {
        // `timer - duration` is positive so this is the default state: "eyes open" face
        faceChange->face = 0;
    } else if ((faceChange->timer - blinkDuration >= -1) || (faceChange->timer < 2)) {
        // This condition aims to catch both cases where the "eyes half open" face is needed.
        // Note that the comparison assumes the duration of the "eyes half open" phase is 2 frames, irrespective of the
        // value of `blinkDuration`. The duration for the "eyes closed" phase is `blinkDuration - 4`.
        // For Player's use case `blinkDuration` is 6, so the "eyes closed" phase happens to have
        // the same duration as each "eyes half open" phase.
        faceChange->face = 1;
    } else {
        // If both conditions above fail, the only possibility left is the "eyes closed" face
        faceChange->face = 2;
    }

    return faceChange->face;
}

/**
 * Updates `FaceChange` data for a blinking pattern.
 * This system expects that the actor using the system has defined 3 faces in this exact order:
 * "eyes open", "eyes half open", "eyes closed".
 *
 * @param faceChange  pointer to an actor's faceChange data
 * @param blinkIntervalBase  The base number of frames between blinks
 * @param blinkIntervalRandRange  The range for a random number of frames that can be added to `blinkIntervalBase`
 * @param blinkDuration  The number of frames it takes for a single blink to occur
 */
s16 FaceChange_UpdateBlinkingNonHuman(FaceChange* faceChange, s16 blinkIntervalBase, s16 blinkIntervalRandRange,
                                      s16 blinkDuration) {
    if (DECR(faceChange->timer) == 0) {
        faceChange->timer = Rand_S16Offset(blinkIntervalBase, blinkIntervalRandRange);
    }

    if (faceChange->timer - blinkDuration > 0) {
        // `timer - duration` is positive so this is the default state: "eyes open" face
        faceChange->face = 0;
    } else if (faceChange->timer - blinkDuration == 0) {
        faceChange->face = 1;
    } else {
        // If both conditions above fail, the only possibility left is the "eyes closed" face
        faceChange->face = 2;
    }

    return faceChange->face;
}

/**
 * Updates `FaceChange` data for randomly selected face sets.
 * Each set contains 3 faces. After the timer runs out, the next face in the set is used.
 * After the third face in a set is used, a new face set is randomly chosen.
 *
 * @param faceChange  pointer to an actor's faceChange data
 * @param changeTimerBase  The base number of frames between each face change
 * @param changeTimerRandRange  The range for a random number of frames that can be added to `changeTimerBase`
 * @param faceSetRange  The max number of face sets that will be chosen from
 */
s16 FaceChange_UpdateRandomSet(FaceChange* faceChange, s16 changeTimerBase, s16 changeTimerRandRange,
                               s16 faceSetRange) {
    if (DECR(faceChange->timer) == 0) {
        faceChange->timer = Rand_S16Offset(changeTimerBase, changeTimerRandRange);
        faceChange->face++;
        if ((faceChange->face % 3) == 0) {
            faceChange->face = (s32)(Rand_ZeroOne() * faceSetRange) * 3;
        }
    }

    return faceChange->face;
}

void Actor_SpawnBodyParts(Actor* actor, PlayState* play, s32 partParams, Gfx** dList) {
    if (*dList != NULL) {
        EnPart* part;
        Actor* spawnedPart;
        MtxF* currentMatrix = Matrix_GetCurrent();

        spawnedPart =
            Actor_SpawnAsChild(&play->actorCtx, actor, play, ACTOR_EN_PART, currentMatrix->mf[3][0],
                               currentMatrix->mf[3][1], currentMatrix->mf[3][2], 0, 0, actor->objectSlot, partParams);

        if (spawnedPart != NULL) {
            part = (EnPart*)spawnedPart;

            Matrix_MtxFToYXZRot(currentMatrix, &part->actor.shape.rot, false);
            part->dList = *dList;
            Math_Vec3f_Copy(&part->actor.scale, &actor->scale);
        }
    }
}

void Actor_SpawnFloorDustRing(PlayState* play, Actor* actor, Vec3f* posXZ, f32 radius, s32 countMinusOne,
                              f32 randAccelWeight, s16 scale, s16 scaleStep, u8 useLighting) {
    Vec3f pos;
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    s32 pad[2];
    f32 angle;
    s32 i;

    angle = (Rand_ZeroOne() - 0.5f) * (2.0f * 3.14f);
    pos.y = actor->floorHeight;
    accel.y += (Rand_ZeroOne() - 0.5f) * 0.2f;

    for (i = countMinusOne; i >= 0; i--) {
        pos.x = (Math_SinF(angle) * radius) + posXZ->x;
        pos.z = (Math_CosF(angle) * radius) + posXZ->z;
        accel.x = (Rand_ZeroOne() - 0.5f) * randAccelWeight;
        accel.z = (Rand_ZeroOne() - 0.5f) * randAccelWeight;

        if (scale == 0) {
            func_800B10C0(play, &pos, &gZeroVec3f, &accel);
        } else if (useLighting) {
            func_800B1210(play, &pos, &gZeroVec3f, &accel, scale, scaleStep);
        } else {
            func_800B11A0(play, &pos, &gZeroVec3f, &accel, scale, scaleStep);
        }
        angle += (2.0f * 3.14f) / (countMinusOne + 1.0f);
    }
}

void func_800BBFB0(PlayState* play, Vec3f* position, f32 arg2, s32 arg3, s16 arg4, s16 scaleStep, u8 arg6) {
    Vec3f pos;
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    s32 i;

    for (i = arg3; i >= 0; i--) {
        s16 scale;

        pos.x = ((Rand_ZeroOne() - 0.5f) * arg2) + position->x;
        pos.y = ((Rand_ZeroOne() - 0.5f) * arg2) + position->y;
        pos.z = ((Rand_ZeroOne() - 0.5f) * arg2) + position->z;

        scale = (s32)(Rand_ZeroOne() * arg4 * 0.2f);
        scale += arg4;

        if (arg6) {
            func_800B1210(play, &pos, &gZeroVec3f, &accel, scale, scaleStep);
        } else {
            func_800B11A0(play, &pos, &gZeroVec3f, &accel, scale, scaleStep);
        }
    }
}

void Actor_ChangeCategory(PlayState* play, ActorContext* actorCtx, Actor* actor, u8 actorCategory) {
    actorCtx->actorLists[actor->category].categoryChanged = true;
    actor->category = actorCategory;
}

// Damage flags for EnArrow
u32 sArrowDmgFlags[] = {
    DMG_FIRE_ARROW,   // ARROW_TYPE_NORMAL_LIT
    DMG_NORMAL_ARROW, // ARROW_TYPE_NORMAL_HORSE
    DMG_NORMAL_ARROW, // ARROW_TYPE_NORMAL
    DMG_FIRE_ARROW,   // ARROW_TYPE_FIRE
    DMG_ICE_ARROW,    // ARROW_TYPE_ICE
    DMG_LIGHT_ARROW,  // ARROW_TYPE_LIGHT
    DMG_DEKU_NUT,     // ARROW_TYPE_SLINGSHOT
    DMG_DEKU_BUBBLE,  // ARROW_TYPE_DEKU_BUBBLE
    DMG_DEKU_NUT,     // ARROW_TYPE_DEKU_NUT
};

u32 Actor_GetArrowDmgFlags(s32 params) {
    if ((params < 0) || (params >= ARRAY_COUNT(sArrowDmgFlags))) {
        return 0;
    }

    return sArrowDmgFlags[params];
}

s32 func_800BC1B4(Actor* actor, Actor* projectile, f32 distance, f32 speed) {
    if ((speed > 0.0f) && (Actor_WorldDistXYZToActor(projectile, actor) < ((speed * 2.5f) + distance))) {
        s16 temp_v1 = BINANG_SUB(Actor_WorldYawTowardActor(projectile, actor), projectile->world.rot.y);

        if (ABS_ALT(temp_v1) < 0x1400) {
            return true;
        }
    }

    return false;
}

Actor* func_800BC270(PlayState* play, Actor* actor, f32 distance, u32 dmgFlags) {
    Actor* itemAction = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;

    while (itemAction != NULL) {
        if (((itemAction->id == ACTOR_ARMS_HOOK) && (dmgFlags & DMG_HOOKSHOT)) ||
            ((itemAction->id == ACTOR_EN_BOOM) && (dmgFlags & DMG_ZORA_BOOMERANG)) ||
            ((itemAction->id == ACTOR_EN_ARROW) && (Actor_GetArrowDmgFlags(itemAction->params) & dmgFlags))) {
            f32 speed;

            if ((itemAction->speed <= 0.0f) && (GET_PLAYER(play)->unk_D57 != 0)) {
                if (itemAction->id == ACTOR_ARMS_HOOK) {
                    speed = 20.0f;
                } else if (itemAction->id == ACTOR_EN_BOOM) {
                    speed = 12.0f;
                } else {
                    u32 arrowDmgFlags = Actor_GetArrowDmgFlags(itemAction->params);

                    if (arrowDmgFlags == DMG_DEKU_NUT) {
                        speed = 80.0f;
                    } else if (arrowDmgFlags == DMG_DEKU_BUBBLE) {
                        speed = 60.0f;
                    } else {
                        speed = 150.0f;
                    }
                }
            } else {
                speed = itemAction->speed;
            }

            if (func_800BC1B4(actor, itemAction, distance, speed)) {
                break;
            }
        }

        itemAction = itemAction->next;
    }

    return itemAction;
}

Actor* func_800BC444(PlayState* play, Actor* actor, f32 distance) {
    Actor* explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;

    while (explosive != NULL) {
        if (((explosive->id == ACTOR_EN_BOM) || (explosive->id == ACTOR_EN_BOM_CHU) ||
             (explosive->id == ACTOR_EN_BOMBF))) {
            if (func_800BC1B4(actor, explosive, distance, explosive->speed)) {
                break;
            }
        }
        explosive = explosive->next;
    }

    return explosive;
}

/**
 * Checks if a given actor will be standing on the ground after being translated
 * by the provided distance and angle.
 *
 * Returns true if the actor will be standing on ground.
 */
s16 Actor_TestFloorInDirection(Actor* actor, PlayState* play, f32 distance, s16 angle) {
    s16 ret;
    u16 bgCheckFlags;
    f32 dx;
    f32 dz;
    Vec3f actorPos;

    Math_Vec3f_Copy(&actorPos, &actor->world.pos);

    bgCheckFlags = actor->bgCheckFlags;

    dx = Math_SinS(angle) * distance;
    dz = Math_CosS(angle) * distance;

    actor->world.pos.x += dx;
    actor->world.pos.z += dz;

    Actor_UpdateBgCheckInfo(play, actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    Math_Vec3f_Copy(&actor->world.pos, &actorPos);

    ret = actor->bgCheckFlags & BGCHECKFLAG_GROUND;

    actor->bgCheckFlags = bgCheckFlags;

    return ret;
}

/**
 * Returns true if the player is targeting the provided actor
 */
s32 Actor_IsTargeted(PlayState* play, Actor* actor) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags3 & PLAYER_STATE3_HOSTILE_LOCK_ON) && actor->isLockedOn) {
        return true;
    }

    return false;
}

/**
 * Returns true if the player is targeting an actor other than the provided actor
 */
s32 Actor_OtherIsTargeted(PlayState* play, Actor* actor) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags3 & PLAYER_STATE3_HOSTILE_LOCK_ON) && !actor->isLockedOn) {
        return true;
    }

    return false;
}

void func_800BC620(Vec3f* pos, Vec3f* scale, u8 alpha, PlayState* play) {
    MtxF mf;
    f32 yIntersect;
    Vec3f adjustedPos;
    CollisionPoly* poly;

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP, SETUPDL_44);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, alpha);

    adjustedPos.x = pos->x;
    adjustedPos.y = pos->y + 1.0f;
    adjustedPos.z = pos->z;

    yIntersect = BgCheck_EntityRaycastFloor2(play, &play->colCtx, &poly, &adjustedPos);
    if (poly != NULL) {
        func_800C0094(poly, pos->x, yIntersect, pos->z, &mf);
        Matrix_Put(&mf);
    } else {
        Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);
    }

    Matrix_Scale(scale->x, 1.0f, scale->z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gCircleShadowDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Actor_RequestQuake(PlayState* play, s16 y, s16 duration) {
    s16 quakeIndex = Quake_Request(&play->mainCamera, QUAKE_TYPE_3);

    Quake_SetSpeed(quakeIndex, 20000);
    Quake_SetPerturbations(quakeIndex, y, 0, 0, 0);
    Quake_SetDuration(quakeIndex, duration);
}

void Actor_RequestQuakeWithSpeed(PlayState* play, s16 y, s16 duration, s16 speed) {
    s16 quakeIndex = Quake_Request(&play->mainCamera, QUAKE_TYPE_3);

    Quake_SetSpeed(quakeIndex, speed);
    Quake_SetPerturbations(quakeIndex, y, 0, 0, 0);
    Quake_SetDuration(quakeIndex, duration);
}

void Actor_RequestQuakeAndRumble(Actor* actor, PlayState* play, s16 quakeY, s16 quakeDuration) {
    if (quakeY >= 5) {
        Rumble_Request(actor->xyzDistToPlayerSq, 255, 20, 150);
    } else {
        Rumble_Request(actor->xyzDistToPlayerSq, 180, 20, 100);
    }
    Actor_RequestQuake(play, quakeY, quakeDuration);
}

typedef struct {
    /* 0x00 */ f32 chainAngle;
    /* 0x04 */ f32 chainLength;
    /* 0x08 */ f32 yShift;
    /* 0x0C */ f32 chainsScale;
    /* 0x10 */ f32 chainsRotZInit;
    /* 0x14 */ Gfx* chainDL;
    /* 0x18 */ Gfx* lockDL;
} DoorLockInfo; // size = 0x1C

DoorLockInfo sDoorLocksInfo[DOORLOCK_MAX] = {
    /* DOORLOCK_NORMAL */ { 0.54f, 6000.0f, 5000.0f, 1.0f, 0.0f, gDoorChainDL, gDoorLockDL },
    /* DOORLOCK_BOSS */ { 0.644f, 12000.0f, 8000.0f, 1.0f, 0.0f, gBossDoorChainDL, gBossDoorLockDL },
    /* DOORLOCK_2 */ { 0.6400000453f, 8500.0f, 8000.0f, 1.75f, 0.1f, gDoorChainDL, gDoorLockDL },
};

/**
 * Draws chains and lock of a locked door, of the specified `type` (see `DoorLockType`).
 * `frame` can be 0 to 10, where 0 is "open" and 10 is "closed", the chains slide accordingly.
 */
void Actor_DrawDoorLock(PlayState* play, s32 frame, s32 type) {
    s32 pad[2];
    MtxF baseMtxF;
    s32 i;
    f32 chainsTranslateX;
    f32 chainsTranslateY;
    DoorLockInfo* entry = &sDoorLocksInfo[type];
    f32 chainRotZ = entry->chainsRotZInit;
    f32 rotZStep;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(0.0f, entry->yShift, 500.0f, MTXMODE_APPLY);
    Matrix_Get(&baseMtxF);

    chainsTranslateX = sinf(entry->chainAngle - chainRotZ) * -(10 - frame) * 0.1f * entry->chainLength;
    chainsTranslateY = cosf(entry->chainAngle - chainRotZ) * (10 - frame) * 0.1f * entry->chainLength;

    for (i = 0; i < 4; i++) {
        Matrix_Put(&baseMtxF);
        Matrix_RotateZF(chainRotZ, MTXMODE_APPLY);
        Matrix_Translate(chainsTranslateX, chainsTranslateY, 0.0f, MTXMODE_APPLY);
        if (entry->chainsScale != 1.0f) {
            Matrix_Scale(entry->chainsScale, entry->chainsScale, entry->chainsScale, MTXMODE_APPLY);
        }

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, entry->chainDL);

        if ((i % 2) != 0) {
            rotZStep = 2.0f * entry->chainAngle;
        } else {
            rotZStep = M_PIf - (2.0f * entry->chainAngle);
        }

        chainRotZ += rotZStep;
    }

    Matrix_Put(&baseMtxF);
    Matrix_Scale(frame * 0.1f, frame * 0.1f, frame * 0.1f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, entry->lockDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Actor_SpawnShieldParticlesMetal(PlayState* play, Vec3f* pos) {
    CollisionCheck_SpawnShieldParticlesMetal(play, pos);
}

void Actor_SetColorFilter(Actor* actor, u16 colorFlag, u16 colorIntensityMax, u16 bufFlag, u16 duration) {
    if ((colorFlag == COLORFILTER_COLORFLAG_GRAY) && !(colorIntensityMax & COLORFILTER_INTENSITY_FLAG)) {
        Actor_PlaySfx(actor, NA_SE_EN_LIGHT_ARROW_HIT);
    }

    actor->colorFilterParams = colorFlag | bufFlag | ((colorIntensityMax & 0xF8) << 5) | duration;
    actor->colorFilterTimer = duration;
}

Hilite* func_800BCBF4(Vec3f* arg0, PlayState* play) {
    Vec3f lightDir;

    lightDir.x = play->envCtx.dirLight1.params.dir.x;
    lightDir.y = play->envCtx.dirLight1.params.dir.y;
    lightDir.z = play->envCtx.dirLight1.params.dir.z;

    return Hilite_DrawOpa(arg0, &play->view.eye, &lightDir, play->state.gfxCtx);
}

Hilite* func_800BCC68(Vec3f* arg0, PlayState* play) {
    Vec3f lightDir;

    lightDir.x = play->envCtx.dirLight1.params.dir.x;
    lightDir.y = play->envCtx.dirLight1.params.dir.y;
    lightDir.z = play->envCtx.dirLight1.params.dir.z;

    return Hilite_DrawXlu(arg0, &play->view.eye, &lightDir, play->state.gfxCtx);
}

/**
 * Calculates the closest position `dstPos` to the input position `srcPos` along the path given by `points`/`numPoints`
 * Whether the points provided forms a closed-loop path is indicated by `isPathLoop`
 */
void Actor_GetClosestPosOnPath(Vec3s* points, s32 numPoints, Vec3f* srcPos, Vec3f* dstPos, s32 isPathLoop) {
    s32 pointIndex;
    s32 closestPointIndex;
    s32 useAdjacentLines[2] = {
        false, // determines whether to use line connecting to previous point in calculations
        false, // determines whether to use line connecting to next point in calculations
    };
    s32 isRightSideOfAdjacentLines[2] = {
        false, // determines whether srcPos is on the right side of the line from prev to curr point
        false, // determines whether srcPos is on the right side of the line from curr to next point
    };
    Vec3f closestPoint;
    Vec3f closestPos[2];
    Vec3f closestPointNext;
    Vec3f closestPointPrev;
    f32 distSq; // First used as distSq to closest point, then used as distSq to closest position
    f32 closestPointDistSq;
    f32 loopDistSq[2];
    s32 i;

    closestPointIndex = 0;
    closestPointDistSq = SQ(40000.0f);

    // Find the point closest to srcPos
    for (pointIndex = 0; pointIndex < numPoints; pointIndex++) {
        distSq = Math3D_Dist2DSq(srcPos->x, srcPos->z, points[pointIndex].x, points[pointIndex].z);
        if (distSq < closestPointDistSq) {
            closestPointDistSq = distSq;
            closestPointIndex = pointIndex;
        }
    }

    closestPoint.x = (points + closestPointIndex)->x;
    closestPoint.z = (points + closestPointIndex)->z;
    dstPos->y = (points + closestPointIndex)->y;

    // Analyze point on path immediately previous to the closest point
    if (closestPointIndex != 0) {
        // The point previous to the closest point
        closestPointPrev.x = (points + closestPointIndex - 1)->x;
        closestPointPrev.z = (points + closestPointIndex - 1)->z;
    } else if (isPathLoop) {
        // Closest point is the first point in the path list
        // Set the previous point to loop around to the the final point on the path
        closestPointPrev.x = (points + numPoints - 1)->x;
        closestPointPrev.z = (points + numPoints - 1)->z;
    }
    if ((closestPointIndex != 0) || isPathLoop) {
        // Use the adjacent line
        useAdjacentLines[0] =
            Math3D_PointDistSqToLine2DImpl(srcPos->x, srcPos->z, closestPointPrev.x, closestPointPrev.z, closestPoint.x,
                                           closestPoint.z, &closestPos[0].x, &closestPos[0].z, &distSq);
    }

    // Analyze point on path immediately next to the closest point
    if (closestPointIndex + 1 != numPoints) {
        // The point next to the closest point
        closestPointNext.x = (points + closestPointIndex + 1)->x;
        closestPointNext.z = (points + closestPointIndex + 1)->z;
    } else if (isPathLoop) {
        // Closest point is the final point in the path list
        // Set the next point to loop around to the the first point on the path
        closestPointNext.x = (points + 0)->x;
        closestPointNext.z = (points + 0)->z;
    }
    if ((closestPointIndex + 1 != numPoints) || isPathLoop) {
        useAdjacentLines[1] =
            Math3D_PointDistSqToLine2DImpl(srcPos->x, srcPos->z, closestPoint.x, closestPoint.z, closestPointNext.x,
                                           closestPointNext.z, &closestPos[1].x, &closestPos[1].z, &distSq);
    }

    /**
     * For close-looped paths, they must be defined in a clockwise orientation looking from the top down.
     * Therefore, `srcPos` being interior of the loop will lead to both lines of `isRightSideOfAdjacentLines`
     * returning true.
     */
    if (isPathLoop) {
        isRightSideOfAdjacentLines[0] = ((closestPointPrev.x - srcPos->x) * (closestPoint.z - srcPos->z)) <
                                        ((closestPointPrev.z - srcPos->z) * (closestPoint.x - srcPos->x));

        isRightSideOfAdjacentLines[1] = ((closestPointNext.z - srcPos->z) * (closestPoint.x - srcPos->x)) <
                                        ((closestPoint.z - srcPos->z) * (closestPointNext.x - srcPos->x));

        for (i = 0; i < ARRAY_COUNT(loopDistSq); i++) {
            if (useAdjacentLines[i]) {
                // Get distSq from srcPos to closestPos
                loopDistSq[i] = Math3D_Dist2DSq(srcPos->x, srcPos->z, closestPos[i].x, closestPos[i].z);
            } else {
                // The closest Pos is not contained within the line-segment
                loopDistSq[i] = SQ(40000.0f);
            }
        }
    }

    // Calculate closest position along path
    if (isPathLoop && ((isRightSideOfAdjacentLines[0] && isRightSideOfAdjacentLines[1]) ||
                       (isRightSideOfAdjacentLines[0] && useAdjacentLines[0] && (loopDistSq[0] < loopDistSq[1])) ||
                       (isRightSideOfAdjacentLines[1] && useAdjacentLines[1] && (loopDistSq[1] < loopDistSq[0])))) {
        // srcPos is contained within the closed loop
        dstPos->x = srcPos->x;
        dstPos->z = srcPos->z;
    } else if (useAdjacentLines[0] && useAdjacentLines[1]) {
        // srcPos is somewhere within the bend of the path
        if (!isRightSideOfAdjacentLines[0] && !isRightSideOfAdjacentLines[1]) {
            // srcPos is not inside a loop
            if (!Math3D_PointDistSqToLine2DImpl(srcPos->x, srcPos->z, closestPos[0].x, closestPos[0].z, closestPos[1].x,
                                                closestPos[1].z, &dstPos->x, &dstPos->z, &distSq)) {
                // The dstPos calculated in Math3D_PointDistSqToLine2DImpl was not valid.
                // Take the midpoint of the two closest ponits instead
                dstPos->x = (closestPos[1].x + closestPos[0].x) * 0.5f;
                dstPos->z = (closestPos[1].z + closestPos[0].z) * 0.5f;
            }
        } else if (loopDistSq[1] < loopDistSq[0]) {
            // Use closest position along the line in the loop connecting the closest point and the next point
            dstPos->x = closestPos[1].x;
            dstPos->z = closestPos[1].z;
        } else {
            // Use closest position along the ling in the loop connecting the closest point and the prev point
            dstPos->x = closestPos[0].x;
            dstPos->z = closestPos[0].z;
        }
    } else if (useAdjacentLines[0]) {
        // Use closest position along line segment connecting the closest point and the prev point
        dstPos->x = closestPos[0].x;
        dstPos->z = closestPos[0].z;
    } else if (useAdjacentLines[1]) {
        // Use closest position along line segment connecting the closest point and the next point
        dstPos->x = closestPos[1].x;
        dstPos->z = closestPos[1].z;
    } else if (isPathLoop && ((((closestPointPrev.x - srcPos->x) * (closestPointNext.z - srcPos->z)) <
                               ((closestPointPrev.z - srcPos->z) * (closestPointNext.x - srcPos->x))))) {
        // Inside the line that directly connects the previous point to the next point (inside the bend of a corner)
        dstPos->x = srcPos->x;
        dstPos->z = srcPos->z;
    } else {
        // The closest point and the closest position are the same (srcPos is near the outer region of a corner)
        dstPos->x = closestPoint.x;
        dstPos->z = closestPoint.z;
    }
}

/**
 * Updates NPC talking state. Checks for a talk request and updates
 * the talkState parameter when a dialog is ongoing. Otherwise checks if
 * the actor is onscreen, advertises the interaction in a range and sets
 * the current text id if necessary.
 *
 * The talk state values are defined in the NpcTalkState enum.
 *
 * @see NpcTalkState
 *
 * @param[in,out] talkState Talk state
 * @param interactRange The interact (talking) range for the actor
 * @param getTextId Callback for getting the next text id
 * @param updateTalkState Callback for getting the next talkState value
 * @return True if a new dialog was started (player talked to the actor). False otherwise.
 */
s32 Npc_UpdateTalking(PlayState* play, Actor* actor, s16* talkState, f32 interactRange, NpcGetTextIdFunc getTextId,
                      NpcUpdateTalkStateFunc updateTalkState) {
    if (Actor_TalkOfferAccepted(actor, &play->state)) {
        *talkState = NPC_TALK_STATE_TALKING;
        return true;
    }

    if (*talkState != NPC_TALK_STATE_IDLE) {
        *talkState = updateTalkState(play, actor);
        return false;
    }

    if (!Actor_OnScreen(play, actor)) {
        return false;
    }

    if (!Actor_OfferTalk(actor, play, interactRange)) {
        return false;
    }

    actor->textId = getTextId(play, actor);

    return false;
}

typedef struct {
    /* 0x0 */ s16 maxHeadYaw;
    /* 0x2 */ s16 minHeadPitch;
    /* 0x4 */ s16 maxHeadPitch;
    /* 0x6 */ s16 maxTorsoYaw;
    /* 0x8 */ s16 minTorsoPitch;
    /* 0xA */ s16 maxTorsoPitch;
    /* 0xC */ u8 rotateYaw;
} NpcTrackingRotLimits; // size = 0x10

typedef struct {
    /* 0x00 */ NpcTrackingRotLimits rotLimits;
    // Fields specific to NPC_TRACKING_PLAYER_AUTO_TURN mode
    /* 0x10 */ f32 autoTurnDistanceRange;   // Max distance to player to enable tracking and auto-turn
    /* 0x14 */ s16 maxYawForPlayerTracking; // Player is tracked if within this yaw
} NpcTrackingParams;                        // size = 0x18

/**
 * Npc tracking angle limit presets to use with Npc_TrackPoint.
 *
 * @see Npc_TrackPoint
 */
NpcTrackingParams sNpcTrackingPresets[] = {
    { { 0x1C20, 0xE390, 0x1C70, 0x1554, 0x0000, 0x0000, false }, 170.0f, 0x3FFC },
    { { 0x2AA8, 0xEAAC, 0x1554, 0x1554, 0xF8E4, 0x0E38, true }, 170.0f, 0x3FFC },
    { { 0x31C4, 0xE390, 0x0E38, 0x0E38, 0xF1C8, 0x071C, true }, 170.0f, 0x3FFC },
    { { 0x1554, 0xF1C8, 0x0000, 0x071C, 0xF8E4, 0x0000, true }, 170.0f, 0x3FFC },
    { { 0x2AA8, 0xF8E4, 0x071C, 0x0E38, 0xD558, 0x2AA8, true }, 170.0f, 0x3FFC },
    { { 0x0000, 0xE390, 0x2AA8, 0x3FFC, 0xF1C8, 0x0E38, true }, 170.0f, 0x3FFC },
    { { 0x2AA8, 0xF1C8, 0x0E38, 0x0E38, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
    { { 0x2AA8, 0xF1C8, 0x0000, 0x0E38, 0x0000, 0x1C70, true }, 0.0f, 0x0000 },
    { { 0x2AA8, 0xF1C8, 0xF1C8, 0x0000, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
    { { 0x071C, 0xF1C8, 0x0E38, 0x1C70, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
    { { 0x0E38, 0xF1C8, 0x0000, 0x1C70, 0x0000, 0x0E38, true }, 0.0f, 0x0000 },
    { { 0x2AA8, 0xE390, 0x1C70, 0x0E38, 0xF1C8, 0x0E38, true }, 0.0f, 0x0000 },
    { { 0x18E2, 0xF1C8, 0x0E38, 0x0E38, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
    { { 0x2A6C, 0xE390, 0x1C70, 0x1554, 0x0000, 0x0000, false }, 170.0f, 0x3FFC },
};

/**
 * Smoothly turns the actor's whole body and updates torso and head rotations in
 * NpcInteractInfo so that the actor tracks the point specified in NpcInteractInfo.trackPos.
 * Rotations are limited to specified angles.
 *
 * Head and torso rotation angles are determined by calculating the pitch and yaw
 * from the actor position to the given target position.
 *
 * The y position of the actor is offset by NpcInteractInfo.yOffset
 * before calculating the angles. It can be used to configure the height difference
 * between the actor and the target.
 *
 * @param maxHeadYaw maximum head yaw difference from neutral position
 * @param maxHeadPitch maximum head pitch angle
 * @param minHeadPitch minimum head pitch angle
 * @param maxTorsoYaw maximum torso yaw difference from neutral position
 * @param maxTorsoPitch maximum torso pitch angle
 * @param minTorsoPitch minimum torso pitch angle
 * @param rotateYaw if true, the actor's yaw (shape.rot.y) is updated to turn the actor's whole body
 */
void Npc_TrackPointWithLimits(Actor* actor, NpcInteractInfo* interactInfo, s16 maxHeadYaw, s16 maxHeadPitch,
                              s16 minHeadPitch, s16 maxTorsoYaw, s16 maxTorsoPitch, s16 minTorsoPitch, u8 rotateYaw) {
    s16 pitchTowardsTarget;
    s16 yawTowardsTarget;
    s16 torsoPitch;
    s16 bodyYawDiff;
    s16 temp;
    Vec3f offsetActorPos;

    offsetActorPos.x = actor->world.pos.x;
    offsetActorPos.y = actor->world.pos.y + interactInfo->yOffset;
    offsetActorPos.z = actor->world.pos.z;

    pitchTowardsTarget = Math_Vec3f_Pitch(&offsetActorPos, &interactInfo->trackPos);
    yawTowardsTarget = Math_Vec3f_Yaw(&offsetActorPos, &interactInfo->trackPos);
    bodyYawDiff = Math_Vec3f_Yaw(&actor->world.pos, &interactInfo->trackPos) - actor->shape.rot.y;

    temp = CLAMP(bodyYawDiff, -maxHeadYaw, maxHeadYaw);
    Math_SmoothStepToS(&interactInfo->headRot.y, temp, 6, 0x7D0, 1);

    temp = (ABS_ALT(bodyYawDiff) >= 0x8000) ? 0 : ABS_ALT(bodyYawDiff);
    interactInfo->headRot.y = CLAMP(interactInfo->headRot.y, -temp, temp);

    bodyYawDiff -= interactInfo->headRot.y;

    temp = CLAMP(bodyYawDiff, -maxTorsoYaw, maxTorsoYaw);
    Math_SmoothStepToS(&interactInfo->torsoRot.y, temp, 6, 0x7D0, 1);

    temp = (ABS_ALT(bodyYawDiff) >= 0x8000) ? 0 : ABS_ALT(bodyYawDiff);
    interactInfo->torsoRot.y = CLAMP(interactInfo->torsoRot.y, -temp, temp);

    if (rotateYaw) {
        Math_SmoothStepToS(&actor->shape.rot.y, yawTowardsTarget, 6, 0x7D0, 1);
    }

    temp = CLAMP(pitchTowardsTarget, minHeadPitch, (s16)(u16)maxHeadPitch);
    Math_SmoothStepToS(&interactInfo->headRot.x, temp, 6, 0x7D0, 1);

    torsoPitch = pitchTowardsTarget - interactInfo->headRot.x;

    temp = CLAMP(torsoPitch, minTorsoPitch, maxTorsoPitch);
    Math_SmoothStepToS(&interactInfo->torsoRot.x, temp, 6, 0x7D0, 1);
}

// unused
s16 Npc_GetTrackingPresetMaxPlayerYaw(s16 presetIndex) {
    return sNpcTrackingPresets[presetIndex].maxYawForPlayerTracking;
}

/**
 * Handles NPC tracking modes and auto-turning towards the player when
 * NPC_TRACKING_PLAYER_AUTO_TURN tracking mode is used.
 *
 * Returns a tracking mode that will determine which actor limbs
 * will be rotated towards the target.
 *
 * When the player is behind the actor (i.e. not in the yaw range in front of the actor
 * defined by maxYawForPlayerTracking), the actor will start an auto-turn sequence:
 *   - look forward for 30-60 frames
 *   - turn head to look at the player for 10-20 frames
 *   - look forward for 30-60 frames
 *   - turn the entire body to face the player
 *
 * @param distanceRange Max distance to player that tracking and auto-turning will be active for
 * @param maxYawForPlayerTracking Maximum angle for tracking the player.
 * @param trackingMode The tracking mode selected by the actor. If this is not
 *        NPC_TRACKING_PLAYER_AUTO_TURN this function does nothing
 *
 * @return The tracking mode (NpcTrackingMode) to use for the current frame.
 */
s16 Npc_UpdateAutoTurn(Actor* actor, NpcInteractInfo* interactInfo, f32 distanceRange, s16 maxYawForPlayerTracking,
                       s16 trackingMode) {
    s32 pad;
    s16 yaw;
    s16 yawDiff;

    if (trackingMode != NPC_TRACKING_PLAYER_AUTO_TURN) {
        return trackingMode;
    }

    if (interactInfo->talkState != NPC_TALK_STATE_IDLE) {
        // When talking, always fully turn to face the player
        return NPC_TRACKING_FULL_BODY;
    }

    if (distanceRange < Math_Vec3f_DistXYZ(&actor->world.pos, &interactInfo->trackPos)) {
        // Player is too far away, do not track
        interactInfo->autoTurnTimer = 0;
        interactInfo->autoTurnState = 0;
        return NPC_TRACKING_NONE;
    }

    yaw = Math_Vec3f_Yaw(&actor->world.pos, &interactInfo->trackPos);
    yawDiff = ABS_ALT(BINANG_SUB(yaw, actor->shape.rot.y));

    if (maxYawForPlayerTracking >= yawDiff) {
        // Player is in front of the actor, track with the head and the torso
        interactInfo->autoTurnTimer = 0;
        interactInfo->autoTurnState = 0;
        return NPC_TRACKING_HEAD_AND_TORSO;
    }

    // Player is behind the actor, run the auto-turn sequence.

    if (DECR(interactInfo->autoTurnTimer) != 0) {
        // While the timer is still running, return the previous tracking mode
        return interactInfo->trackingMode;
    }

    switch (interactInfo->autoTurnState) {
        case 0:
        case 2:
            // Just stand still, not tracking the player
            interactInfo->autoTurnTimer = Rand_S16Offset(30, 30);
            interactInfo->autoTurnState++;
            return NPC_TRACKING_NONE;

        case 1:
            // Glance at the player by only turning the head
            interactInfo->autoTurnTimer = Rand_S16Offset(10, 10);
            interactInfo->autoTurnState++;
            return NPC_TRACKING_HEAD;

        default:
            // Auto-turn sequence complete, turn towards the player
            return NPC_TRACKING_FULL_BODY;
    }
}

/**
 * Rotates the actor's whole body, torso and head tracking the point specified in NpcInteractInfo.trackPos.
 * Uses angle limits from a preset selected from from sNpcTrackingPresets.
 *
 * The trackingMode parameter controls whether the head and torso are turned towards the target.
 * If not, they are smoothly turned towards zero. Setting the parameter to NPC_TRACKING_FULL_BODY
 * causes the actor's whole body to be rotated to face the target.
 *
 * If NPC_TRACKING_PLAYER_AUTO_TURN is used, the actor will track the player with its head and torso as long
 * as the player is in front of the actor (within a yaw angle specified in the option preset).
 * If the player is outside of this angle, the actor will turn to face the player after a while.
 *
 * @see Npc_UpdateAutoTurn
 * @see sNpcTrackingPresets
 * @see NpcTrackingMode
 *
 * @param presetIndex The index to a preset in sNpcTrackingPresets
 * @param trackingMode A value from NpcTrackingMode enum
 */
void Npc_TrackPoint(Actor* actor, NpcInteractInfo* interactInfo, s16 presetIndex, s16 trackingMode) {
    NpcTrackingRotLimits rotLimits;

    interactInfo->trackingMode =
        Npc_UpdateAutoTurn(actor, interactInfo, sNpcTrackingPresets[presetIndex].autoTurnDistanceRange,
                           sNpcTrackingPresets[presetIndex].maxYawForPlayerTracking, trackingMode);

    rotLimits = sNpcTrackingPresets[presetIndex].rotLimits;

    switch (interactInfo->trackingMode) {
        case NPC_TRACKING_NONE:
            rotLimits.maxHeadYaw = 0;
            rotLimits.maxHeadPitch = 0;
            rotLimits.minHeadPitch = 0;
            // fallthrough
        case NPC_TRACKING_HEAD:
            rotLimits.maxTorsoYaw = 0;
            rotLimits.maxTorsoPitch = 0;
            rotLimits.minTorsoPitch = 0;
            // fallthrough
        case NPC_TRACKING_HEAD_AND_TORSO:
            rotLimits.rotateYaw = false;
            break;

        default:
            break;
    }

    Npc_TrackPointWithLimits(actor, interactInfo, rotLimits.maxHeadYaw, rotLimits.maxHeadPitch, rotLimits.minHeadPitch,
                             rotLimits.maxTorsoYaw, rotLimits.maxTorsoPitch, rotLimits.minTorsoPitch,
                             rotLimits.rotateYaw);
}

// #region 2S2H - These DisplayLists are now extracted
// Gfx D_801AEF88[] = {
//     gsDPSetRenderMode(AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
//                           G_RM_FOG_SHADE_A,
//                       AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
//                           GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
//     gsDPSetAlphaCompare(G_AC_THRESHOLD),
//     gsSPEndDisplayList(),
// };

// Gfx D_801AEFA0[] = {
//     gsSPEndDisplayList(),
// };
// #endregion

Gfx* func_800BD9A0(GraphicsContext* gfxCtx) {
    Gfx* gfxHead = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));
    Gfx* gfx = gfxHead;

    gDPSetRenderMode(
        gfx++, AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL | G_RM_FOG_SHADE_A,
        AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
            GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA));
    gSPEndDisplayList(gfx++);

    return gfxHead;
}

// unused
void func_800BD9E0(PlayState* play, SkelAnime* skelAnime, OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw,
                   Actor* actor, s16 alpha) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, alpha);
    gSPSegment(POLY_OPA_DISP++, 0x0C, gEmptyDL);

    POLY_OPA_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       overrideLimbDraw, postLimbDraw, actor, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_800BDAA0(PlayState* play, SkelAnime* skelAnime, OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw,
                   Actor* actor, s16 alpha) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, func_800BD9A0(play->state.gfxCtx));

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       overrideLimbDraw, postLimbDraw, actor, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

// Unused
s16 func_800BDB6C(Actor* actor, PlayState* play, s16 arg2, f32 arg3) {
    Player* player = GET_PLAYER(play);
    f32 phi_f2;

    if ((play->csCtx.state != CS_STATE_IDLE) || gDbgCamEnabled) {
        phi_f2 = Math_Vec3f_DistXYZ(&actor->world.pos, &play->view.eye) * 0.25f;
    } else {
        phi_f2 = Math_Vec3f_DistXYZ(&actor->world.pos, &player->actor.world.pos);
    }

    if (arg3 < phi_f2) {
        actor->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        Math_SmoothStepToS(&arg2, 0, 6, 0x14, 1);
    } else {
        actor->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        Math_SmoothStepToS(&arg2, 0xFF, 6, 0x14, 1);
    }

    return arg2;
}

void Actor_ChangeAnimationByInfo(SkelAnime* skelAnime, AnimationInfo* animInfo, s32 animIndex) {
    f32 endFrame;

    animInfo += animIndex;

    if (animInfo->frameCount > 0.0f) {
        endFrame = animInfo->frameCount;
    } else {
        endFrame = Animation_GetLastFrame(&animInfo->animation->common);
    }

    Animation_Change(skelAnime, animInfo->animation, animInfo->playSpeed, animInfo->startFrame, endFrame,
                     animInfo->mode, animInfo->morphFrames);
}

/**
 * Fills two tables with rotation angles that can be used to simulate idle animations.
 *
 * The rotation angles are dependent on the current frame, so should be updated regularly, generally every frame.
 *
 * This is done for the desired limb by taking either the `sin` of the yTable value or the `cos` of the zTable value,
 * multiplying by some scale factor (generally 200), and adding that to the already existing rotation.
 *
 * Note: With the common scale factor of 200, this effect is practically unnoticeable if the current animation already
 * has motion involved.
 *
 * Note: This function goes unused in favor of `SubS_UpdateFidgetTables`.
 */
void Actor_UpdateFidgetTables(PlayState* play, s16* fidgetTableY, s16* fidgetTableZ, s32 tableLen) {
    s32 frames = play->gameplayFrames;
    s32 i;

    for (i = 0; i < tableLen; i++) {
        fidgetTableY[i] = (i * 50 + 0x814) * frames;
        fidgetTableZ[i] = (i * 50 + 0x940) * frames;
    }
}

void Actor_Noop(Actor* actor, PlayState* play) {
}

/**
 * Draws a display list to the opaque display buffer
 */
void Gfx_DrawDListOpa(PlayState* play, Gfx* dlist) {
    Gfx* dl;

    OPEN_DISPS(play->state.gfxCtx);

    dl = POLY_OPA_DISP;

    gSPDisplayList(&dl[0], gSetupDLs[SETUPDL_25]);
    MATRIX_FINALIZE_AND_LOAD(&dl[1], play->state.gfxCtx);
    gSPDisplayList(&dl[2], dlist);

    POLY_OPA_DISP = &dl[3];

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Draws a display list to the translucent display buffer
 */
void Gfx_DrawDListXlu(PlayState* play, Gfx* dlist) {
    Gfx* dl;

    OPEN_DISPS(play->state.gfxCtx);

    dl = POLY_XLU_DISP;

    gSPDisplayList(&dl[0], gSetupDLs[SETUPDL_25]);
    MATRIX_FINALIZE_AND_LOAD(&dl[1], play->state.gfxCtx);
    gSPDisplayList(&dl[2], dlist);

    POLY_XLU_DISP = &dl[3];

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Finds the first actor instance of a specified Id and category within a given range from
 * an actor if there is one. If the Id provided is -1, this will look for any actor of the
 * specified category rather than a specific Id.
 */
Actor* Actor_FindNearby(PlayState* play, Actor* inActor, s16 actorId, u8 actorCategory, f32 distance) {
    Actor* actor = play->actorCtx.actorLists[actorCategory].first;

    while (actor != NULL) {
        if ((actor == inActor) || ((actorId != -1) && (actorId != actor->id))) {
            actor = actor->next;
            continue;
        }

        if (Actor_WorldDistXYZToActor(inActor, actor) <= distance) {
            return actor;
        }

        actor = actor->next;
    }

    return NULL;
}

s32 func_800BE184(PlayState* play, Actor* actor, f32 xzDist, s16 arg3, s16 arg4, s16 arg5) {
    Player* player = GET_PLAYER(play);
    s16 phi_v0 = BINANG_SUB(BINANG_ROT180(actor->yawTowardsPlayer), player->actor.shape.rot.y);
    s16 temp_t0 = actor->yawTowardsPlayer - arg5;

    if ((actor->xzDistToPlayer <= xzDist) && (player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0)) {
        if ((arg4 >= ABS_ALT(phi_v0)) && (arg3 >= ABS_ALT(temp_t0))) {
            return true;
        }
    }

    return false;
}

u8 Actor_ApplyDamage(Actor* actor) {
    if (actor->colChkInfo.damage >= actor->colChkInfo.health) {
        actor->colChkInfo.health = 0;
    } else {
        actor->colChkInfo.health -= actor->colChkInfo.damage;
    }
    return actor->colChkInfo.health;
}

void Actor_SetDropFlag(Actor* actor, ColliderElement* elem) {
    ColliderElement* acHitElem = elem->acHitElem;

    if (acHitElem == NULL) {
        actor->dropFlag = DROPFLAG_NONE;
    } else if (acHitElem->atDmgInfo.dmgFlags & DMG_FIRE_ARROW) {
        actor->dropFlag = DROPFLAG_1;
    } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ICE_ARROW) {
        actor->dropFlag = DROPFLAG_2;
    } else if (acHitElem->atDmgInfo.dmgFlags & DMG_LIGHT_ARROW) {
        actor->dropFlag = DROPFLAG_20;
    } else {
        actor->dropFlag = DROPFLAG_NONE;
    }
}

void Actor_SetDropFlagJntSph(Actor* actor, ColliderJntSph* jntSph) {
    s32 i;
    ColliderJntSphElement* jntSphElem;
    ColliderElement* acHitElem;
    s32 flag;

    actor->dropFlag = DROPFLAG_NONE;

    for (i = jntSph->count - 1; i >= 0; i--) {
        jntSphElem = &jntSph->elements[i];
        acHitElem = jntSphElem->base.acHitElem;

        if (acHitElem == NULL) {
            flag = DROPFLAG_NONE;
        } else {
            s32 dmgFlags = acHitElem->atDmgInfo.dmgFlags;

            if (dmgFlags & DMG_FIRE_ARROW) {
                flag = DROPFLAG_1;
            } else if (dmgFlags & DMG_ICE_ARROW) {
                flag = DROPFLAG_2;
            } else {
                flag = (dmgFlags & DMG_LIGHT_ARROW) ? DROPFLAG_20 : DROPFLAG_NONE;
            }
        }

        actor->dropFlag |= flag;
    }
}

void func_800BE33C(Vec3f* arg0, Vec3f* arg1, Vec3s* dst, s32 arg3) {
    f32 xDiff = arg1->x - arg0->x;
    f32 zDiff = arg1->z - arg0->z;
    f32 yDiff = arg3 ? (arg1->y - arg0->y) : (arg0->y - arg1->y);

    dst->y = Math_Atan2S_XY(zDiff, xDiff);
    dst->x = Math_Atan2S_XY(sqrtf(SQ(xDiff) + SQ(zDiff)), yDiff);
}

void func_800BE3D0(Actor* actor, s16 angle, Vec3s* arg2) {
    f32 floorPolyNormalX;
    f32 floorPolyNormalY;
    f32 floorPolyNormalZ;
    f32 sp38;
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    s32 pad[3];

    if (actor->floorPoly != NULL) {
        CollisionPoly* floorPoly = actor->floorPoly;

        floorPolyNormalX = COLPOLY_GET_NORMAL(floorPoly->normal.x);
        floorPolyNormalY = COLPOLY_GET_NORMAL(floorPoly->normal.y);
        floorPolyNormalZ = COLPOLY_GET_NORMAL(floorPoly->normal.z);

        sp38 = Math_SinS(angle);
        sp34 = Math_CosS(angle);
        arg2->x = (s16)-Math_Atan2S((-(floorPolyNormalX * sp38) - (floorPolyNormalZ * sp34)) * floorPolyNormalY, 1.0f);

        sp2C = Math_SinS(angle - 0x3FF7);
        sp30 = Math_CosS(angle - 0x3FF7);
        arg2->z = (s16)-Math_Atan2S((-(floorPolyNormalX * sp2C) - (floorPolyNormalZ * sp30)) * floorPolyNormalY, 1.0f);
    }
}

void func_800BE504(Actor* actor, ColliderCylinder* cyl) {
    // Checks if was hit by either DMG_NORMAL_ARROW, DMG_FIRE_ARROW, DMG_ICE_ARROW, DMG_LIGHT_ARROW or DMG_DEKU_BUBBLE
    if ((cyl->elem.acHitElem->atDmgInfo.dmgFlags & (0x10000 | 0x2000 | 0x1000 | 0x800 | 0x20))) {
        actor->world.rot.y = cyl->base.ac->shape.rot.y;
    } else {
        actor->world.rot.y = Actor_WorldYawTowardActor(cyl->base.ac, actor);
    }
}

void func_800BE568(Actor* actor, ColliderSphere* sph) {
    if (sph->elem.acHitElem->atDmgInfo.dmgFlags & (0x10000 | 0x2000 | 0x1000 | 0x800 | 0x20)) {
        actor->world.rot.y = sph->base.ac->shape.rot.y;
    } else {
        actor->world.rot.y = Actor_WorldYawTowardActor(sph->base.ac, actor);
    }
}

void func_800BE5CC(Actor* actor, ColliderJntSph* jntSph, s32 elemIndex) {
    if (jntSph->elements[elemIndex].base.acHitElem->atDmgInfo.dmgFlags & (0x10000 | 0x2000 | 0x1000 | 0x800 | 0x20)) {
        actor->world.rot.y = jntSph->base.ac->shape.rot.y;
    } else {
        actor->world.rot.y = Actor_WorldYawTowardActor(jntSph->base.ac, actor);
    }
}

s32 Actor_IsSmallChest(struct EnBox* chest) {
    if (chest->type == ENBOX_TYPE_SMALL || chest->type == ENBOX_TYPE_SMALL_INVISIBLE ||
        chest->type == ENBOX_TYPE_SMALL_ROOM_CLEAR || chest->type == ENBOX_TYPE_SMALL_SWITCH_FLAG_FALL ||
        chest->type == ENBOX_TYPE_SMALL_SWITCH_FLAG) {
        return true;
    }
    return false;
}

TexturePtr sElectricSparkTextures[] = {
    gElectricSpark1Tex,
    gElectricSpark2Tex,
    gElectricSpark3Tex,
    gElectricSpark4Tex,
};

/**
 * Draw common damage effects applied to each body part provided in bodyPartsPos
 */
void Actor_DrawDamageEffects(PlayState* play, Actor* actor, Vec3f bodyPartsPos[], s16 bodyPartsCount, f32 effectScale,
                             f32 frozenSteamScale, f32 effectAlpha, u8 type) {
    if (!GameInteractor_Should(VB_DRAW_DAMAGE_EFFECT, true, actor, &type)) {
        return;
    }

    if (effectAlpha > 0.001f) {
        s32 twoTexScrollParam;
        s16 bodyPartIndex;
        MtxF* currentMatrix;
        f32 alpha;
        f32 frozenScale;
        f32 lightOrbsScale;
        f32 electricSparksScale;
        f32 steamScale;
        Vec3f* bodyPartsPosStart = bodyPartsPos;
        u32 gameplayFrames = play->gameplayFrames;
        f32 effectAlphaScaled;

        currentMatrix = Matrix_GetCurrent();

        // Apply sfx along with damage effect
        if ((actor != NULL) && (effectAlpha > 0.05f) && (play->gameOverCtx.state == GAMEOVER_INACTIVE)) {
            if (type == ACTOR_DRAW_DMGEFF_FIRE) {
                Actor_PlaySfx(actor, NA_SE_EV_BURN_OUT - SFX_FLAG);
            } else if (type == ACTOR_DRAW_DMGEFF_BLUE_FIRE) {
                Actor_PlaySfx(actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
            } else if (type == ACTOR_DRAW_DMGEFF_FROZEN_SFX) {
                Actor_PlaySfx(actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
            } else if ((type == ACTOR_DRAW_DMGEFF_LIGHT_ORBS) || (type == ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS)) {
                Actor_PlaySfx(actor, NA_SE_EN_COMMON_DEADLIGHT - SFX_FLAG);
            }
        }

        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        switch (type) {
            case ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX:
            case ACTOR_DRAW_DMGEFF_FROZEN_SFX:
                frozenScale = ((KREG(19) * 0.01f) + 2.3f) * effectScale;
                steamScale = ((KREG(28) * 0.0001f) + 0.035f) * frozenSteamScale;
                func_800BCC68(bodyPartsPos, play);

                // Setup to draw ice over frozen actor

                gSPSegment(POLY_XLU_DISP++, 0x08,
                           Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, gameplayFrames & 0xFF, 32, 16, 1, 0,
                                              (gameplayFrames * 2) & 0xFF, 64, 32, 0, 1, 0, 2));
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, 255);
                gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment2MaterialDL);

                effectAlphaScaled = effectAlpha * 255.0f;

                // Apply and draw ice over each body part of frozen actor
                for (bodyPartIndex = 0; bodyPartIndex < bodyPartsCount; bodyPartIndex++, bodyPartsPos++) {
                    FrameInterpolation_RecordOpenChild(bodyPartsPos, type);
                    alpha = bodyPartIndex & 3;
                    alpha = effectAlphaScaled - (30.0f * alpha);
                    if (effectAlphaScaled < (30.0f * (bodyPartIndex & 3))) {
                        alpha = 0.0f;
                    }
                    if (alpha > 255.0f) {
                        alpha = 255.0f;
                    }

                    gDPSetEnvColor(POLY_XLU_DISP++, KREG(20) + 200, KREG(21) + 200, KREG(22) + 255, (u8)alpha);

                    Matrix_Translate(bodyPartsPos->x, bodyPartsPos->y, bodyPartsPos->z, MTXMODE_NEW);
                    Matrix_Scale(frozenScale, frozenScale, frozenScale, MTXMODE_APPLY);

                    if (bodyPartIndex & 1) {
                        Matrix_RotateYF(M_PIf, MTXMODE_APPLY);
                    }

                    if (bodyPartIndex & 2) {
                        Matrix_RotateZF(M_PIf, MTXMODE_APPLY);
                    }

                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

                    gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment2ModelDL);
                    FrameInterpolation_RecordCloseChild();
                }

                bodyPartsPos = bodyPartsPosStart; // reset bodyPartsPos

                // Setup to draw steam over frozen actor

                gDPSetColorDither(POLY_XLU_DISP++, G_CD_BAYER);
                gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_PATTERN);
                gSPDisplayList(POLY_XLU_DISP++, gFrozenSteamMaterialDL);

                alpha = effectAlpha * 100.0f;
                if (alpha > 100.0f) {
                    alpha = 100.0f;
                }

                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (u8)alpha);

                // Apply and draw steam over each body part of frozen actor
                for (bodyPartIndex = 0; bodyPartIndex < bodyPartsCount; bodyPartIndex++, bodyPartsPos++) {
                    FrameInterpolation_RecordOpenChild(bodyPartsPos, type);
                    twoTexScrollParam = ((bodyPartIndex * 3) + gameplayFrames);
                    gSPSegment(POLY_XLU_DISP++, 0x08,
                               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, twoTexScrollParam * 3, twoTexScrollParam * -12,
                                                  32, 64, 1, 0, 0, 32, 32, 3, -12, 0, 0));

                    Matrix_Translate(bodyPartsPos->x, bodyPartsPos->y, bodyPartsPos->z, MTXMODE_NEW);
                    Matrix_ReplaceRotation(&play->billboardMtxF);
                    Matrix_Scale(steamScale, steamScale, 1.0f, MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

                    gSPDisplayList(POLY_XLU_DISP++, gFrozenSteamModelDL);
                    FrameInterpolation_RecordCloseChild();
                }
                break;

            case ACTOR_DRAW_DMGEFF_FIRE:
            case ACTOR_DRAW_DMGEFF_BLUE_FIRE:
                if (type == ACTOR_DRAW_DMGEFF_FIRE) {
                    gDPSetEnvColor(POLY_XLU_DISP++, 255, 10, 0, 0);
                } else {
                    gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, 0);
                    // Reuse type for blue primitive color
                    type = 255;
                }

                Matrix_Put(&play->billboardMtxF);
                Matrix_Scale((effectScale * 0.005f) * 1.35f, (effectScale * 0.005f), (effectScale * 0.005f) * 1.35f,
                             MTXMODE_APPLY);

                effectAlphaScaled = effectAlpha * 255.0f;

                // Apply and draw fire on every body part
                for (bodyPartIndex = 0; bodyPartIndex < bodyPartsCount; bodyPartIndex++, bodyPartsPos++) {
                    FrameInterpolation_RecordOpenChild(bodyPartsPos, type);
                    alpha = bodyPartIndex & 3;
                    alpha = effectAlphaScaled - 30.0f * alpha;
                    if (effectAlphaScaled < (30.0f * (bodyPartIndex & 3))) {
                        alpha = 0.0f;
                    }
                    if (alpha > 255.0f) {
                        alpha = 255.0f;
                    }

                    // Use type for blue primitive color
                    // = 0 for ACTOR_DRAW_DMGEFF_FIRE
                    // = 255 for ACTOR_DRAW_DMGEFF_BLUE_FIRE
                    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, type, (u8)alpha);

                    gSPSegment(POLY_XLU_DISP++, 0x08,
                               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                                  ((bodyPartIndex * 10 + gameplayFrames) * -20) & 0x1FF, 32, 128, 0, 0,
                                                  0, -20));

                    Matrix_RotateYF(M_PIf, MTXMODE_APPLY);
                    currentMatrix->mf[3][0] = bodyPartsPos->x;
                    currentMatrix->mf[3][1] = bodyPartsPos->y;
                    currentMatrix->mf[3][2] = bodyPartsPos->z;

                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

                    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
                    FrameInterpolation_RecordCloseChild();
                }
                break;

            case ACTOR_DRAW_DMGEFF_LIGHT_ORBS:
            case ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS:

                // Setup to draw light orbs on actor

                lightOrbsScale = ((KREG(19) * 0.01f) + 4.0f) * effectScale;

                gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);

                alpha = effectAlpha * 255.0f;
                if (alpha > 255.0f) {
                    alpha = 255.0f;
                }

                if (type == ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS) {
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (u8)(sREG(16) + 255), (u8)(sREG(17) + 255),
                                    (u8)(sREG(18) + 255), (u8)alpha);

                    gDPSetEnvColor(POLY_XLU_DISP++, (u8)sREG(19), (u8)(sREG(20) + 255), (u8)(sREG(21) + 255), 128);
                } else {
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, (u8)alpha);

                    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 100, 128);
                }

                Matrix_Put(&play->billboardMtxF);
                Matrix_Scale(lightOrbsScale, lightOrbsScale, 1.0f, MTXMODE_APPLY);

                // Apply and draw a light orb over each body part of frozen actor
                for (bodyPartIndex = 0; bodyPartIndex < bodyPartsCount; bodyPartIndex++, bodyPartsPos++) {
                    FrameInterpolation_RecordOpenChild(bodyPartsPos, type);
                    Matrix_RotateZF(Rand_CenteredFloat(2 * M_PIf), MTXMODE_APPLY);
                    currentMatrix->mf[3][0] = bodyPartsPos->x;
                    currentMatrix->mf[3][1] = bodyPartsPos->y;
                    currentMatrix->mf[3][2] = bodyPartsPos->z;

                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

                    gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
                    FrameInterpolation_RecordCloseChild();
                }
                break;

            case ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL:
            case ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_MEDIUM:
            case ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE:
                if (type == ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL) {
                    electricSparksScale = (KREG(19) * 0.01f + 1.0f) * effectScale;
                } else if (type == ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_MEDIUM) {
                    electricSparksScale = (KREG(19) * 0.01f + 1.5f) * effectScale;
                } else {
                    electricSparksScale = (KREG(19) * 0.01f + 2.0f) * effectScale;
                }

                gSPSegment(POLY_XLU_DISP++, 0x08,
                           Lib_SegmentedToVirtual(sElectricSparkTextures[play->gameplayFrames % 4]));

                gSPDisplayList(POLY_XLU_DISP++, gElectricSparkMaterialDL);

                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (u8)(sREG(16) + 255), (u8)(sREG(17) + 255), (u8)(sREG(18) + 150),
                                (u8)(sREG(19) + 255));

                gDPSetEnvColor(POLY_XLU_DISP++, (u8)(sREG(20) + 255), (u8)(sREG(21) + 255), (u8)sREG(22), (u8)sREG(23));

                Matrix_Put(&play->billboardMtxF);
                Matrix_Scale(electricSparksScale, electricSparksScale, electricSparksScale, MTXMODE_APPLY);

                // Every body part draws two electric sparks at random orientations
                for (bodyPartIndex = 0; bodyPartIndex < bodyPartsCount; bodyPartIndex++, bodyPartsPos++) {
                    FrameInterpolation_RecordOpenChild(bodyPartsPos, type);
                    // first electric spark
                    Matrix_RotateXFApply(Rand_ZeroFloat(2 * M_PIf));
                    Matrix_RotateZF(Rand_ZeroFloat(2 * M_PIf), MTXMODE_APPLY);
                    currentMatrix->mf[3][0] = Rand_CenteredFloat((f32)sREG(24) + 30.0f) + bodyPartsPos->x;
                    currentMatrix->mf[3][1] = Rand_CenteredFloat((f32)sREG(24) + 30.0f) + bodyPartsPos->y;
                    currentMatrix->mf[3][2] = Rand_CenteredFloat((f32)sREG(24) + 30.0f) + bodyPartsPos->z;

                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

                    gSPDisplayList(POLY_XLU_DISP++, gElectricSparkModelDL);

                    // second electric spark
                    Matrix_RotateXFApply(Rand_ZeroFloat(2 * M_PIf));
                    Matrix_RotateZF(Rand_ZeroFloat(2 * M_PIf), MTXMODE_APPLY);
                    currentMatrix->mf[3][0] = Rand_CenteredFloat((f32)sREG(24) + 30.0f) + bodyPartsPos->x;
                    currentMatrix->mf[3][1] = Rand_CenteredFloat((f32)sREG(24) + 30.0f) + bodyPartsPos->y;
                    currentMatrix->mf[3][2] = Rand_CenteredFloat((f32)sREG(24) + 30.0f) + bodyPartsPos->z;

                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

                    gSPDisplayList(POLY_XLU_DISP++, gElectricSparkModelDL);
                    FrameInterpolation_RecordCloseChild();
                }

                break;
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void Actor_SpawnIceEffects(PlayState* play, Actor* actor, Vec3f bodyPartsPos[], s32 bodyPartsCount,
                           s32 effectsPerBodyPart, f32 scale, f32 scaleRange) {
    static Color_RGBA8 sPrimColor = { 170, 255, 255, 255 };
    static Color_RGBA8 sEnvColor = { 200, 200, 255, 255 };
    static Vec3f sAccel = { 0.0f, -1.0f, 0.0f };
    s32 i;
    s32 pad;
    Vec3f velocity;
    s16 randomYaw;
    s16 yaw;
    s32 j;

    SoundSource_PlaySfxAtFixedWorldPos(play, &actor->world.pos, 30, NA_SE_EV_ICE_BROKEN);

    for (i = 0; i < bodyPartsCount; i++) {
        yaw = Actor_WorldYawTowardPoint(actor, bodyPartsPos);

        for (j = 0; j < effectsPerBodyPart; j++) {
            randomYaw = ((s32)Rand_Next() >> 0x13) + yaw;

            velocity.z = Rand_ZeroFloat(5.0f);

            velocity.x = Math_SinS(randomYaw) * velocity.z;
            velocity.y = Rand_ZeroFloat(4.0f) + 8.0f;
            velocity.z *= Math_CosS(randomYaw);

            EffectSsEnIce_Spawn(play, bodyPartsPos, Rand_ZeroFloat(scaleRange) + scale, &velocity, &sAccel, &sPrimColor,
                                &sEnvColor, 30);
        }

        bodyPartsPos++;
    }
}
