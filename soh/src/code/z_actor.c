#include "global.h"
#include "vt.h"

#include "overlays/actors/ovl_Arms_Hook/z_arms_hook.h"
#include "overlays/actors/ovl_En_Part/z_en_part.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "objects/object_bdoor/object_bdoor.h"
#include "soh/frame_interpolation.h"
#include "soh/Enhancements/enemyrandomizer.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/nametag.h"

#include "soh/ActorDB.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#if defined(_MSC_VER) || defined(__GNUC__)
#include "textures/place_title_cards/g_pn_49.h"
#include "textures/place_title_cards/g_pn_01.h"
#include "textures/place_title_cards/g_pn_02.h"
#include "textures/place_title_cards/g_pn_03.h"
#include "textures/place_title_cards/g_pn_04.h"
#include "textures/place_title_cards/g_pn_05.h"
#include "textures/place_title_cards/g_pn_06.h"
#include "textures/place_title_cards/g_pn_07.h"
#include "textures/place_title_cards/g_pn_08.h"
#include "textures/place_title_cards/g_pn_09.h"
#include "textures/place_title_cards/g_pn_10.h"
#include "textures/place_title_cards/g_pn_11.h"
#include "textures/place_title_cards/g_pn_12.h"
#include "textures/place_title_cards/g_pn_13.h"
#include "textures/place_title_cards/g_pn_14.h"
#include "textures/place_title_cards/g_pn_15.h"
#include "textures/place_title_cards/g_pn_16.h"
#include "textures/place_title_cards/g_pn_17.h"
#include "textures/place_title_cards/g_pn_18.h"
#include "textures/place_title_cards/g_pn_19.h"
#include "textures/place_title_cards/g_pn_20.h"
#include "textures/place_title_cards/g_pn_21.h"
#include "textures/place_title_cards/g_pn_22.h"
#include "textures/place_title_cards/g_pn_23.h"
#include "textures/place_title_cards/g_pn_24.h"
#include "textures/place_title_cards/g_pn_25.h"
#include "textures/place_title_cards/g_pn_26.h"
#include "textures/place_title_cards/g_pn_27.h"
#include "textures/place_title_cards/g_pn_28.h"
#include "textures/place_title_cards/g_pn_29.h"
#include "textures/place_title_cards/g_pn_30.h"
#include "textures/place_title_cards/g_pn_31.h"
#include "textures/place_title_cards/g_pn_32.h"
#include "textures/place_title_cards/g_pn_33.h"
#include "textures/place_title_cards/g_pn_34.h"
#include "textures/place_title_cards/g_pn_35.h"
#include "textures/place_title_cards/g_pn_36.h"
#include "textures/place_title_cards/g_pn_37.h"
#include "textures/place_title_cards/g_pn_38.h"
#include "textures/place_title_cards/g_pn_39.h"
#include "textures/place_title_cards/g_pn_40.h"
#include "textures/place_title_cards/g_pn_41.h"
#include "textures/place_title_cards/g_pn_42.h"
#include "textures/place_title_cards/g_pn_43.h"
#include "textures/place_title_cards/g_pn_44.h"
#include "textures/place_title_cards/g_pn_45.h"
#include "textures/place_title_cards/g_pn_46.h"
#include "textures/place_title_cards/g_pn_47.h"
#include "textures/place_title_cards/g_pn_48.h"
#include "textures/place_title_cards/g_pn_50.h"
#include "textures/place_title_cards/g_pn_51.h"
#include "textures/place_title_cards/g_pn_52.h"
#include "textures/place_title_cards/g_pn_53.h"
#include "textures/place_title_cards/g_pn_54.h"
#include "textures/place_title_cards/g_pn_55.h"
#include "textures/place_title_cards/g_pn_56.h"
#include "textures/place_title_cards/g_pn_57.h"
#endif

static CollisionPoly* sCurCeilingPoly;
static s32 sCurCeilingBgId;

// Used for animating the ice trap on the "Get Item" model.
f32 iceTrapScale;

// For Link's voice pitch SFX modifier
static f32 freqMultiplier = 1;

void ActorShape_Init(ActorShape* shape, f32 yOffset, ActorShadowFunc shadowDraw, f32 shadowScale) {
    shape->yOffset = yOffset;
    shape->shadowDraw = shadowDraw;
    shape->shadowScale = shadowScale;
    shape->shadowAlpha = 255;
}

void ActorShadow_Draw(Actor* actor, Lights* lights, PlayState* play, Gfx* dlist, Color_RGBA8* color) {
    f32 temp1;
    f32 temp2;
    MtxF sp60;

    if (actor->floorPoly != NULL) {
        temp1 = actor->world.pos.y - actor->floorHeight;

        if (temp1 >= -50.0f && temp1 < 500.0f) {
            OPEN_DISPS(play->state.gfxCtx);

            POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP, 0x2C);

            gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0, 0,
                              COMBINED);

            temp1 = (temp1 < 0.0f) ? 0.0f : ((temp1 > 150.0f) ? 150.0f : temp1);
            temp2 = 1.0f - (temp1 * (1.0f / 350));

            if (color != NULL) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, color->r, color->g, color->b,
                                (u32)(actor->shape.shadowAlpha * temp2) & 0xFF);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, (u32)(actor->shape.shadowAlpha * temp2) & 0xFF);
            }

            func_80038A28(actor->floorPoly, actor->world.pos.x, actor->floorHeight, actor->world.pos.z, &sp60);
            Matrix_Put(&sp60);

            if (dlist != gCircleShadowDL) {
                Matrix_RotateY(actor->shape.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
            }

            temp2 = (1.0f - (temp1 * (1.0f / 350))) * actor->shape.shadowScale;
            Matrix_Scale(actor->scale.x * temp2, 1.0f, actor->scale.z * temp2, MTXMODE_APPLY);

            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_MODELVIEW | G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, dlist);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

void ActorShadow_DrawCircle(Actor* actor, Lights* lights, PlayState* play) {
    ActorShadow_Draw(actor, lights, play, gCircleShadowDL, NULL);
}

void ActorShadow_DrawWhiteCircle(Actor* actor, Lights* lights, PlayState* play) {
    static Color_RGBA8 white = { 255, 255, 255, 255 };

    ActorShadow_Draw(actor, lights, play, gCircleShadowDL, &white);
}

void ActorShadow_DrawHorse(Actor* actor, Lights* lights, PlayState* play) {
    ActorShadow_Draw(actor, lights, play, gHorseShadowDL, NULL);
}

void ActorShadow_DrawFoot(PlayState* play, Light* light, MtxF* arg2, s32 arg3, f32 arg4, f32 arg5, f32 arg6) {
    s32 pad1;
    f32 sp58;
    s32 pad2[2];

    OPEN_DISPS(play->state.gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0,
                    (u32)(((arg3 * 0.00005f) > 1.0f ? 1.0f : (arg3 * 0.00005f)) * arg4) & 0xFF);

    sp58 = Math_FAtan2F(light->l.dir[0], light->l.dir[2]);
    arg6 *= (4.5f - (light->l.dir[1] * 0.035f));
    arg6 = (arg6 < 1.0f) ? 1.0f : arg6;
    Matrix_Put(arg2);
    Matrix_RotateY(sp58, MTXMODE_APPLY);
    Matrix_Scale(arg5, 1.0f, arg5 * arg6, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, gFootShadowDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void ActorShadow_DrawFeet(Actor* actor, Lights* lights, PlayState* play) {
    f32 distToFloor = actor->world.pos.y - actor->floorHeight;

    if (distToFloor > 20.0f) {
        f32 shadowScale = actor->shape.shadowScale;
        u8 shadowAlpha = actor->shape.shadowAlpha;
        f32 alphaRatio;

        actor->shape.shadowScale *= 0.3f;
        alphaRatio = (distToFloor - 20.0f) * 0.02f;
        actor->shape.shadowAlpha = (f32)actor->shape.shadowAlpha * CLAMP_MAX(alphaRatio, 1.0f);
        ActorShadow_DrawCircle(actor, lights, play);
        actor->shape.shadowScale = shadowScale;
        actor->shape.shadowAlpha = shadowAlpha;
    }

    if (distToFloor < 200.0f) {
        MtxF floorMtx;
        f32 floorHeight[2]; // One for each foot
        f32 distToFloor;
        f32 shadowAlpha;
        f32 shadowScaleX;
        f32 shadowScaleZ;
        Light* lightPtr;
        s32 lightNum;
        s32 lightNumMax;
        s32 i;
        s32 j;
        s32 numLights = lights->numLights - 2;
        Light* firstLightPtr = &lights->l.l[0];
        Vec3f* feetPosPtr = actor->shape.feetPos;
        f32* floorHeightPtr = floorHeight;

        OPEN_DISPS(play->state.gfxCtx);

        POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP, 0x2C);

        actor->shape.feetFloorFlags = 0;

        for (i = 0; i < 2; i++) {
            feetPosPtr->y += 50.0f;
            *floorHeightPtr = func_800BFCB8(play, &floorMtx, feetPosPtr);
            feetPosPtr->y -= 50.0f;
            actor->shape.feetFloorFlags <<= 1;
            distToFloor = feetPosPtr->y - *floorHeightPtr;

            if ((-1.0f <= distToFloor) && (distToFloor < 500.0f)) {
                if (distToFloor <= 0.0f) {
                    actor->shape.feetFloorFlags++;
                }
                distToFloor = CLAMP_MAX(distToFloor, 30.0f);
                shadowAlpha = (f32)actor->shape.shadowAlpha * (1.0f - (distToFloor * (1.0f / 30.0f)));
                distToFloor = CLAMP_MAX(distToFloor, 30.0f);
                shadowScaleZ = 1.0f - (distToFloor * (1.0f / (30.0f + 40.0f)));
                shadowScaleX = shadowScaleZ * actor->shape.shadowScale * actor->scale.x;
                lightNumMax = 0;
                lightPtr = firstLightPtr;

                for (j = 0; j < numLights; j++) {
                    if (lightPtr->l.dir[1] > 0) {
                        lightNum =
                            (lightPtr->l.col[0] + lightPtr->l.col[1] + lightPtr->l.col[2]) * ABS(lightPtr->l.dir[1]);
                        if (lightNum > 0) {
                            lightNumMax += lightNum;
                            ActorShadow_DrawFoot(play, lightPtr, &floorMtx, lightNum, shadowAlpha, shadowScaleX,
                                                 shadowScaleZ);
                        }
                    }
                    lightPtr++;
                }

                for (j = 0; j < 2; j++) {
                    if (lightPtr->l.dir[1] > 0) {
                        lightNum =
                            ((lightPtr->l.col[0] + lightPtr->l.col[1] + lightPtr->l.col[2]) * ABS(lightPtr->l.dir[1])) -
                            (lightNumMax * 8);
                        if (lightNum > 0) {
                            ActorShadow_DrawFoot(play, lightPtr, &floorMtx, lightNum, shadowAlpha, shadowScaleX,
                                                 shadowScaleZ);
                        }
                    }
                    lightPtr++;
                }
            }
            feetPosPtr++;
            floorHeightPtr++;
        }

        if (!(actor->bgCheckFlags & 1)) {
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

void func_8002BE04(PlayState* play, Vec3f* arg1, Vec3f* arg2, f32* arg3) {
    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, arg1, arg2, arg3);
    *arg3 = (*arg3 < 1.0f) ? 1.0f : (1.0f / *arg3);
}

typedef struct {
    /* 0x00 */ Color_RGBA8 inner;
    /* 0x04 */ Color_RGBA8 outer;
} NaviColor; // size = 0x8

NaviColor sNaviColorList[] = {
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },
    { { 255, 255, 255, 255 }, { 0, 0, 255, 0 } },     { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },
    { { 150, 150, 255, 255 }, { 150, 150, 255, 0 } }, { { 255, 255, 0, 255 }, { 200, 155, 0, 0 } },
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         { { 255, 255, 0, 255 }, { 200, 155, 0, 0 } },
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },
};

static Color_RGBA8 defaultIdlePrimaryColor = { 255, 255, 255, 255 };
static Color_RGBA8 defaultIdleSecondaryColor = { 0, 0, 255, 0 };
static Color_RGBA8 defaultNPCPrimaryColor = { 150, 150, 255, 255 };
static Color_RGBA8 defaultNPCSecondaryColor = { 150, 150, 255, 0 };
static Color_RGBA8 defaultEnemyPrimaryColor = { 255, 255, 0, 255 };
static Color_RGBA8 defaultEnemySecondaryColor = { 200, 155, 0, 0 };
static Color_RGBA8 defaultPropsPrimaryColor = { 0, 255, 0, 255 };
static Color_RGBA8 defaultPropsSecondaryColor = { 0, 255, 0, 0 };

// unused
Gfx D_80115FF0[] = {
    gsSPEndDisplayList(),
};

void func_8002BE64(TargetContext* targetCtx, s32 index, f32 arg2, f32 arg3, f32 arg4) {
    targetCtx->arr_50[index].pos.x = arg2;
    targetCtx->arr_50[index].pos.y = arg3;
    targetCtx->arr_50[index].pos.z = arg4;
    targetCtx->arr_50[index].unk_0C = targetCtx->unk_44;
}

void func_8002BE98(TargetContext* targetCtx, s32 actorCategory, PlayState* play) {
    TargetContextEntry* entry;
    NaviColor* naviColor;
    s32 i;

    Math_Vec3f_Copy(&targetCtx->targetCenterPos, &play->view.eye);
    targetCtx->unk_44 = 500.0f;
    targetCtx->unk_48 = 0x100;

    naviColor = &sNaviColorList[actorCategory];

    entry = &targetCtx->arr_50[0];
    for (i = 0; i < ARRAY_COUNT(targetCtx->arr_50); i++) {
        func_8002BE64(targetCtx, i, 0.0f, 0.0f, 0.0f);
        entry->color.r = naviColor->inner.r;
        entry->color.g = naviColor->inner.g;
        entry->color.b = naviColor->inner.b;
        entry++;
    }
}

void func_8002BF60(TargetContext* targetCtx, Actor* actor, s32 actorCategory, PlayState* play) {
    if (CVarGetInteger("gCosmetics.Navi_IdlePrimary.Changed", 0)) {
        sNaviColorList[ACTORCAT_PLAYER].inner = CVarGetColor("gCosmetics.Navi_IdlePrimary.Value", defaultIdlePrimaryColor);
    } else {
        sNaviColorList[ACTORCAT_PLAYER].inner = defaultIdlePrimaryColor;
    }
    if (CVarGetInteger("gCosmetics.Navi_IdleSecondary.Changed", 0)) {
        sNaviColorList[ACTORCAT_PLAYER].outer = CVarGetColor("gCosmetics.Navi_IdleSecondary.Value", defaultIdleSecondaryColor);
    } else {
        sNaviColorList[ACTORCAT_PLAYER].outer = defaultIdleSecondaryColor;
    }
    
    if (CVarGetInteger("gCosmetics.Navi_NPCPrimary.Changed", 0)) {
        sNaviColorList[ACTORCAT_NPC].inner = CVarGetColor("gCosmetics.Navi_NPCPrimary.Value", defaultNPCPrimaryColor);
    } else {
        sNaviColorList[ACTORCAT_NPC].inner = defaultNPCPrimaryColor;
    }
    if (CVarGetInteger("gCosmetics.Navi_NPCSecondary.Changed", 0)) {
        sNaviColorList[ACTORCAT_NPC].outer = CVarGetColor("gCosmetics.Navi_NPCSecondary.Value", defaultNPCSecondaryColor);
    } else {
        sNaviColorList[ACTORCAT_NPC].outer = defaultNPCSecondaryColor;
    }

    if (CVarGetInteger("gCosmetics.Navi_EnemyPrimary.Changed", 0)) {
        sNaviColorList[ACTORCAT_ENEMY].inner = CVarGetColor("gCosmetics.Navi_EnemyPrimary.Value", defaultEnemyPrimaryColor);
        sNaviColorList[ACTORCAT_BOSS].inner = CVarGetColor("gCosmetics.Navi_EnemyPrimary.Value", defaultEnemyPrimaryColor);
    } else {
        sNaviColorList[ACTORCAT_ENEMY].inner = defaultEnemyPrimaryColor;
        sNaviColorList[ACTORCAT_BOSS].inner = defaultEnemyPrimaryColor;
    }
    if (CVarGetInteger("gCosmetics.Navi_EnemySecondary.Changed", 0)) {
        sNaviColorList[ACTORCAT_ENEMY].outer = CVarGetColor("gCosmetics.Navi_EnemySecondary.Value", defaultEnemySecondaryColor);
        sNaviColorList[ACTORCAT_BOSS].outer = CVarGetColor("gCosmetics.Navi_EnemySecondary.Value", defaultEnemySecondaryColor);
    } else {
        sNaviColorList[ACTORCAT_ENEMY].outer = defaultEnemySecondaryColor;
        sNaviColorList[ACTORCAT_BOSS].outer = defaultEnemySecondaryColor;
    }

    if (CVarGetInteger("gCosmetics.Navi_PropsPrimary.Changed", 0)) {
        sNaviColorList[ACTORCAT_PROP].inner = CVarGetColor("gCosmetics.Navi_PropsPrimary.Value", defaultPropsPrimaryColor);
    } else {
        sNaviColorList[ACTORCAT_PROP].inner = defaultPropsPrimaryColor;
    }
    if (CVarGetInteger("gCosmetics.Navi_PropsSecondary.Changed", 0)) {
        sNaviColorList[ACTORCAT_PROP].outer = CVarGetColor("gCosmetics.Navi_PropsSecondary.Value", defaultPropsSecondaryColor);
    } else {
        sNaviColorList[ACTORCAT_PROP].outer = defaultPropsSecondaryColor;
    }

    NaviColor* naviColor = &sNaviColorList[actorCategory];
    targetCtx->naviRefPos.x = actor->focus.pos.x;
    targetCtx->naviRefPos.y = actor->focus.pos.y + (actor->targetArrowOffset * actor->scale.y);
    targetCtx->naviRefPos.z = actor->focus.pos.z;
    targetCtx->naviInner.r = naviColor->inner.r;
    targetCtx->naviInner.g = naviColor->inner.g;
    targetCtx->naviInner.b = naviColor->inner.b;
    targetCtx->naviInner.a = naviColor->inner.a;
    targetCtx->naviOuter.r = naviColor->outer.r;
    targetCtx->naviOuter.g = naviColor->outer.g;
    targetCtx->naviOuter.b = naviColor->outer.b;
    targetCtx->naviOuter.a = naviColor->outer.a;
}

void func_8002C0C0(TargetContext* targetCtx, Actor* actor, PlayState* play) {
    targetCtx->arrowPointedActor = NULL;
    targetCtx->targetedActor = NULL;
    targetCtx->unk_40 = 0.0f;
    targetCtx->unk_8C = NULL;
    targetCtx->bgmEnemy = NULL;
    targetCtx->unk_4B = 0;
    targetCtx->unk_4C = 0;
    func_8002BF60(targetCtx, actor, actor->category, play);
    func_8002BE98(targetCtx, actor->category, play);
}

void func_8002C124(TargetContext* targetCtx, PlayState* play) {
    Actor* actor = targetCtx->targetedActor;

    OPEN_DISPS(play->state.gfxCtx);

    if (targetCtx->unk_48 != 0) {
        TargetContextEntry* entry;
        Player* player;
        s16 spCE;
        f32 temp1;
        Vec3f spBC;
        s32 spB8;
        f32 spB4;
        s32 spB0;
        s32 spAC;
        f32 var1;
        f32 var2;
        s32 i;

        FrameInterpolation_RecordOpenChild(actor, 0);
        player = GET_PLAYER(play);

        spCE = 0xFF;
        var1 = 1.0f;

        if (targetCtx->unk_4B != 0) {
            spB8 = 1;
        } else {
            spB8 = 3;
        }

        if (actor != NULL) {
            Math_Vec3f_Copy(&targetCtx->targetCenterPos, &actor->focus.pos);
            var1 = (500.0f - targetCtx->unk_44) / 420.0f;
        } else {
            targetCtx->unk_48 -= 120;
            if (targetCtx->unk_48 < 0) {
                targetCtx->unk_48 = 0;
            }
            spCE = targetCtx->unk_48;
        }

        func_8002BE04(play, &targetCtx->targetCenterPos, &spBC, &spB4);

        spBC.x = (160 * (spBC.x * spB4)) * var1;
        spBC.x = CLAMP(spBC.x, -320.0f, 320.0f);

        spBC.y = (120 * (spBC.y * spB4)) * var1;
        spBC.y = CLAMP(spBC.y, -240.0f, 240.0f);

        spBC.z = spBC.z * var1;

        targetCtx->unk_4C--;
        if (targetCtx->unk_4C < 0) {
            targetCtx->unk_4C = 2;
        }

        func_8002BE64(targetCtx, targetCtx->unk_4C, spBC.x, spBC.y, spBC.z);

        if ((!(player->stateFlags1 & 0x40)) || (actor != player->unk_664)) {
            OVERLAY_DISP = Gfx_SetupDL(OVERLAY_DISP, 0x39);

            for (spB0 = 0, spAC = targetCtx->unk_4C; spB0 < spB8; spB0++, spAC = (spAC + 1) % 3) {
                entry = &targetCtx->arr_50[spAC];

                if (entry->unk_0C < 500.0f) {
                    if (entry->unk_0C <= 120.0f) {
                        var2 = 0.15f;
                    } else {
                        var2 = ((entry->unk_0C - 120.0f) * 0.001f) + 0.15f;
                    }

                    Matrix_Translate(entry->pos.x, entry->pos.y, 0.0f, MTXMODE_NEW);
                    Matrix_Scale(var2, 0.15f, 1.0f, MTXMODE_APPLY);

                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, entry->color.r, entry->color.g, entry->color.b, (u8)spCE);

                    Matrix_RotateZ((targetCtx->unk_4B & 0x7F) * (M_PI / 64), MTXMODE_APPLY);

                    for (i = 0; i < 4; i++) {
                        Matrix_RotateZ(M_PI / 2, MTXMODE_APPLY);
                        Matrix_Push();
                        Matrix_Translate(entry->unk_0C, entry->unk_0C, 0.0f, MTXMODE_APPLY);
                        gSPMatrix(OVERLAY_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                                  G_MTX_MODELVIEW | G_MTX_LOAD);
                        gSPDisplayList(OVERLAY_DISP++, gZTargetLockOnTriangleDL);
                        Matrix_Pop();
                    }
                }

                spCE -= 0xFF / 3;
                if (spCE < 0) {
                    spCE = 0;
                }
            }
        }
        FrameInterpolation_RecordCloseChild();
    }

    actor = targetCtx->unk_94;
    if ((actor != NULL) && !(actor->flags & ACTOR_FLAG_NO_LOCKON)) {
        FrameInterpolation_RecordOpenChild(actor, 1);
        NaviColor* naviColor = &sNaviColorList[actor->category];

        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 0x7);

        Matrix_Translate(actor->focus.pos.x, actor->focus.pos.y + (actor->targetArrowOffset * actor->scale.y) + 17.0f,
                         actor->focus.pos.z, MTXMODE_NEW);
        Matrix_RotateY((f32)((u16)(play->gameplayFrames * 3000)) * (M_PI / 0x8000), MTXMODE_APPLY);
        Matrix_Scale((iREG(27) + 35) / 1000.0f, (iREG(28) + 60) / 1000.0f, (iREG(29) + 50) / 1000.0f, MTXMODE_APPLY);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, naviColor->inner.r, naviColor->inner.g, naviColor->inner.b, 255);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPDisplayList(POLY_XLU_DISP++, gZTargetArrowDL);
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_8002C7BC(TargetContext* targetCtx, Player* player, Actor* actorArg, PlayState* play) {
    s32 pad;
    Actor* unkActor;
    s32 actorCategory;
    Vec3f sp50;
    f32 sp4C;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp4;
    f32 temp5;
    f32 temp6;
    s32 lockOnSfxId;

    unkActor = NULL;

    if ((player->unk_664 != NULL) && (player->unk_84B[player->unk_846] == 2)) {
        targetCtx->unk_94 = NULL;
    } else {
        func_80032AF0(play, &play->actorCtx, &unkActor, player);
        targetCtx->unk_94 = unkActor;
    }

    if (targetCtx->unk_8C != NULL) {
        unkActor = targetCtx->unk_8C;
        targetCtx->unk_8C = NULL;
    } else if (actorArg != NULL) {
        unkActor = actorArg;
    }

    if (unkActor != NULL) {
        actorCategory = unkActor->category;
    } else {
        actorCategory = player->actor.category;
    }

    if ((unkActor != targetCtx->arrowPointedActor) || (actorCategory != targetCtx->activeCategory)) {
        targetCtx->arrowPointedActor = unkActor;
        targetCtx->activeCategory = actorCategory;
        targetCtx->unk_40 = 1.0f;
    }

    if (unkActor == NULL) {
        unkActor = &player->actor;
    }

    if (Math_StepToF(&targetCtx->unk_40, 0.0f, 0.25f) == 0) {
        temp1 = 0.25f / targetCtx->unk_40;
        temp2 = unkActor->world.pos.x - targetCtx->naviRefPos.x;
        temp3 = (unkActor->world.pos.y + (unkActor->targetArrowOffset * unkActor->scale.y)) - targetCtx->naviRefPos.y;
        temp4 = unkActor->world.pos.z - targetCtx->naviRefPos.z;
        targetCtx->naviRefPos.x += temp2 * temp1;
        targetCtx->naviRefPos.y += temp3 * temp1;
        targetCtx->naviRefPos.z += temp4 * temp1;
    } else {
        func_8002BF60(targetCtx, unkActor, actorCategory, play);
    }

    if ((actorArg != NULL) && (targetCtx->unk_4B == 0)) {
        func_8002BE04(play, &actorArg->focus.pos, &sp50, &sp4C);
        if (((sp50.z <= 0.0f) || (1.0f <= fabsf(sp50.x * sp4C))) || (1.0f <= fabsf(sp50.y * sp4C))) {
            actorArg = NULL;
        }
    }

    if (actorArg != NULL) {
        if (actorArg != targetCtx->targetedActor) {
            func_8002BE98(targetCtx, actorArg->category, play);
            targetCtx->targetedActor = actorArg;

            if (actorArg->id == ACTOR_EN_BOOM) {
                targetCtx->unk_48 = 0;
            }

            lockOnSfxId = CHECK_FLAG_ALL(actorArg->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE) ? NA_SE_SY_LOCK_ON
                                                                                       : NA_SE_SY_LOCK_ON_HUMAN;
            func_80078884(lockOnSfxId);
        }

        targetCtx->targetCenterPos.x = actorArg->world.pos.x;
        targetCtx->targetCenterPos.y = actorArg->world.pos.y - (actorArg->shape.yOffset * actorArg->scale.y);
        targetCtx->targetCenterPos.z = actorArg->world.pos.z;

        if (targetCtx->unk_4B == 0) {
            temp5 = (500.0f - targetCtx->unk_44) * 3.0f;
            temp6 = (temp5 < 30.0f) ? 30.0f : ((100.0f < temp5) ? 100.0f : temp5);
            if (Math_StepToF(&targetCtx->unk_44, 80.0f, temp6) != 0) {
                targetCtx->unk_4B++;
            }
        } else {
            targetCtx->unk_4B = (targetCtx->unk_4B + 3) | 0x80;
            targetCtx->unk_44 = 120.0f;
        }
    } else {
        targetCtx->targetedActor = NULL;
        Math_StepToF(&targetCtx->unk_44, 500.0f, 80.0f);
    }
}

/**
 * Tests if current scene switch flag is set.
 */
s32 Flags_GetSwitch(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        return play->actorCtx.flags.swch & (1 << flag);
    } else {
        return play->actorCtx.flags.tempSwch & (1 << (flag - 0x20));
    }
}

/**
 * Sets current scene switch flag.
 */
void Flags_SetSwitch(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        play->actorCtx.flags.swch |= (1 << flag);
    } else {
        play->actorCtx.flags.tempSwch |= (1 << (flag - 0x20));
    }
}

/**
 * Unsets current scene switch flag.
 */
void Flags_UnsetSwitch(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        play->actorCtx.flags.swch &= ~(1 << flag);
    } else {
        play->actorCtx.flags.tempSwch &= ~(1 << (flag - 0x20));
    }
}

/**
 * Tests if unknown flag is set.
 */
s32 Flags_GetUnknown(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        return play->actorCtx.flags.unk0 & (1 << flag);
    } else {
        return play->actorCtx.flags.unk1 & (1 << (flag - 0x20));
    }
}

/**
 * Sets unknown flag.
 */
void Flags_SetUnknown(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        play->actorCtx.flags.unk0 |= (1 << flag);
    } else {
        play->actorCtx.flags.unk1 |= (1 << (flag - 0x20));
    }
}

/**
 * Unsets unknown flag.
 */
void Flags_UnsetUnknown(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        play->actorCtx.flags.unk0 &= ~(1 << flag);
    } else {
        play->actorCtx.flags.unk1 &= ~(1 << (flag - 0x20));
    }
}

/**
 * Tests if current scene chest flag is set.
 */
s32 Flags_GetTreasure(PlayState* play, s32 flag) {
    return play->actorCtx.flags.chest & (1 << flag);
}

/**
 * Sets current scene chest flag.
 */
void Flags_SetTreasure(PlayState* play, s32 flag) {
    play->actorCtx.flags.chest |= (1 << flag);
}

/**
 * Tests if current scene clear flag is set.
 */
s32 Flags_GetClear(PlayState* play, s32 flag) {
    return play->actorCtx.flags.clear & (1 << flag);
}

/**
 * Sets current scene clear flag.
 */
void Flags_SetClear(PlayState* play, s32 flag) {
    play->actorCtx.flags.clear |= (1 << flag);
}

/**
 * Unsets current scene clear flag.
 */
void Flags_UnsetClear(PlayState* play, s32 flag) {
    play->actorCtx.flags.clear &= ~(1 << flag);
}

/**
 * Tests if current scene temp clear flag is set.
 */
s32 Flags_GetTempClear(PlayState* play, s32 flag) {
    return play->actorCtx.flags.tempClear & (1 << flag);
}

/**
 * Sets current scene temp clear flag.
 */
void Flags_SetTempClear(PlayState* play, s32 flag) {
    play->actorCtx.flags.tempClear |= (1 << flag);
}

/**
 * Unsets current scene temp clear flag.
 */
void Flags_UnsetTempClear(PlayState* play, s32 flag) {
    play->actorCtx.flags.tempClear &= ~(1 << flag);
}

/**
 * Tests if current scene collectible flag is set.
 */
s32 Flags_GetCollectible(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        return play->actorCtx.flags.collect & (1 << flag);
    } else {
        return play->actorCtx.flags.tempCollect & (1 << (flag - 0x20));
    }
}

/**
 * Sets current scene collectible flag.
 */
void Flags_SetCollectible(PlayState* play, s32 flag) {
    if (flag != 0) {
        if (flag < 0x20) {
            play->actorCtx.flags.collect |= (1 << flag);
        } else {
            play->actorCtx.flags.tempCollect |= (1 << (flag - 0x20));
        }
    }
}

void func_8002CDE4(PlayState* play, TitleCardContext* titleCtx) {
    titleCtx->durationTimer = titleCtx->delayTimer = titleCtx->intensityR = titleCtx->alpha = 0;
    titleCtx->durationTimer = titleCtx->delayTimer = titleCtx->intensityG = titleCtx->alpha = 0;
    titleCtx->durationTimer = titleCtx->delayTimer = titleCtx->intensityB = titleCtx->alpha = 0;
}

void TitleCard_InitBossName(PlayState* play, TitleCardContext* titleCtx, void* texture, s16 x, s16 y, u8 width,
                            u8 height, s16 hasTranslation) {
    static char newName[512];
    
    if (gSaveContext.language != LANGUAGE_ENG) {
        size_t length = strlen(texture);
        strcpy(newName, texture);
        if (gSaveContext.language == LANGUAGE_FRA) {
            newName[length - 6] = 'F';
            newName[length - 5] = 'R';
            newName[length - 4] = 'A';
        } else if (gSaveContext.language == LANGUAGE_GER) {
            newName[length - 6] = 'G';
            newName[length - 5] = 'E';
            newName[length - 4] = 'R';
        }
        texture = newName;
    }

    titleCtx->texture = texture;
    titleCtx->isBossCard = true;
    titleCtx->hasTranslation = hasTranslation;
    titleCtx->x = x;
    titleCtx->y = y;
    titleCtx->width = width;
    titleCtx->height = height;
    titleCtx->durationTimer = 80;
    titleCtx->delayTimer = 0;
}

void TitleCard_InitPlaceName(PlayState* play, TitleCardContext* titleCtx, void* texture, s32 x, s32 y,
                             s32 width, s32 height, s32 delay) {
    SceneTableEntry* loadedScene = play->loadedScene;
  //  size_t size = loadedScene->titleFile.vromEnd - loadedScene->titleFile.vromStart;
    switch (play->sceneNum) {
        case SCENE_YDAN:
            texture = gDekuTreeTitleCardENGTex;
            break;
        case SCENE_DDAN:
            texture = gDodongosCavernTitleCardENGTex;
            break;
        case SCENE_BDAN:
            texture = gJabuJabuTitleCardENGTex;
            break;
        case SCENE_BMORI1:
            texture = gForestTempleTitleCardENGTex;
            break;
        case SCENE_HIDAN:
            texture = gFireTempleTitleCardENGTex;
            break;
        case SCENE_MIZUSIN:
            texture = gWaterTempleTitleCardENGTex;
            break;
        case SCENE_JYASINZOU:
            texture = gSpiritTempleTitleCardENGTex;
            break;
        case SCENE_HAKADAN:
            texture = gShadowTempleTitleCardENGTex;
            break;
        case SCENE_HAKADANCH:
            texture = gBottomOfTheWellTitleCardENGTex;
            break;
        case SCENE_ICE_DOUKUTO:
            texture = gIceCavernTitleCardENGTex;
            break;
        case SCENE_MEN:
            texture = gGERudoTrainingGroundTitleCardENGTex;
            break;
        case SCENE_GERUDOWAY:
            texture = gThievesHideoutTitleCardENGTex;
            break;
        case SCENE_GANON_TOU:
            texture = gGanonsCastleTitleCardENGTex;
            break;
        case SCENE_GANONTIKA:
            texture = gInsideGanonsCastleTitleCardENGTex;
            break;
        case SCENE_TAKARAYA:
            texture = gTreasureBoxShopTitleCardENGTex;
            break;
        case SCENE_MARKET_ALLEY:
        case SCENE_MARKET_ALLEY_N:
            texture = gBackAlleyTitleCardENGTex;
            break;
        case SCENE_MARKET_DAY:
        case SCENE_MARKET_NIGHT:
        case SCENE_MARKET_RUINS:
            texture = gMarketTitleCardENGTex;
            break;
        case SCENE_SHOP1:
            texture = gBazaarTitleCardENGTex;
            break;
        case SCENE_KOKIRI_SHOP:
            texture = gKokiriShopTitleCardENGTex;
            break;
        case SCENE_GOLON:
            texture = gGoronShopTitleCardENGTex;
            break;
        case SCENE_ZOORA:
            texture = gZoraShopTitleCardENGTex;
            break;
        case SCENE_NIGHT_SHOP:
            texture = gBombchuShopTitleCardENGTex;
            break;
        case SCENE_DRAG:
        case SCENE_MAHOUYA:
        case SCENE_ALLEY_SHOP:
            texture = gPotionShopTitleCardENGTex;
            break;
        case SCENE_FACE_SHOP:
            texture = gHappyMaskShopTitleCardENGTex;
            break;
        case SCENE_MALON_STABLE:
            texture = gStableTitleCardENGTex;
            break;
        case SCENE_HYLIA_LABO:
            texture = gLakesideLaboratoryTitleCardENGTex;
            break;
        case SCENE_HUT:
            texture = gGravekeepersHutTitleCardENGTex;
            break;
        case SCENE_DAIYOUSEI_IZUMI:
        case SCENE_YOUSEI_IZUMI_YOKO:
            texture = gGreatFairysFountainTitleCardENGTex;
            break;
        case SCENE_YOUSEI_IZUMI_TATE:
            texture = gFairysFountainTitleCardENGTex;
            break;
        case SCENE_HAKAANA_OUKE:
            texture = gRoyalFamilysTombTitleCardENGTex;
            break;
        case SCENE_SYATEKIJYOU:
            texture = gShootingGalleryTitleCardENGTex;
            break;
        case SCENE_TOKINOMA:
            texture = gTempleOfTimeTitleCardENGTex;
            break;
        case SCENE_KENJYANOMA:
            texture = gChamberOfTheSagesTitleCardENGTex;
            break;
        case SCENE_HAIRAL_NIWA:
        case SCENE_HAIRAL_NIWA_N:
        case SCENE_NAKANIWA:
        case SCENE_HAIRAL_NIWA2:
            texture = gCastleCourtyardTitleCardENGTex;
            break;
        case SCENE_HAKASITARELAY:
            texture = gQuestionMarkTitleCardENGTex;
            break;
        case SCENE_TURIBORI:
            texture = gFishingPondTitleCardENGTex;
            break;
        case SCENE_BOWLING:
            texture = gBombchuBowlingAlleyCardENGTex;
            break;
        case SCENE_KINSUTA:
            texture = gHouseOfSkulltulaTitleCardENGTex;
            break;
        case SCENE_SPOT00:
            texture = gHyruleFieldTitleCardENGTex;
            break;
        case SCENE_SPOT01:
            texture = gKakarikoVillageTitleCardENGTex;
            break;
        case SCENE_SPOT02:
            texture = gGraveyardTitleCardENGTex;
            break;
        case SCENE_SPOT03:
            texture = gZorasRiverTitleCardENGTex;
            break;
        case SCENE_SPOT04:
            texture = gKokiriForestTitleCardENGTex;
            break;
        case SCENE_SPOT05:
            texture = gSacredForestMeadowTitleCardENGTex;
            break;
        case SCENE_SPOT06:
            texture = gLakeHyliaTitleCardENGTex;
            break;
        case SCENE_SPOT07:
            texture = gZorasDomainTitleCardENGTex;
            break;
        case SCENE_SPOT08:
            texture = gZorasFountainTitleCardENGTex;
            break;
        case SCENE_SPOT09:
            texture = gGERudoValleyTitleCardENGTex;
            break;
        case SCENE_SPOT10:
            texture = gLostWoodsTitleCardENGTex;
            break;
        case SCENE_SPOT11:
            texture = gDesertColossusTitleCardENGTex;
            break;
        case SCENE_SPOT12:
            texture = gGERudosFortressTitleCardENGTex;
            break;
        case SCENE_SPOT13:
            texture = gHauntedWastelandTitleCardENGTex;
            break;
        case SCENE_SPOT15:
            texture = gHyruleCastleTitleCardENGTex;
            break;
        case SCENE_SPOT16:
            texture = gDeathMountainTrailTitleCardENGTex;
            break;
        case SCENE_SPOT17:
            texture = gDeathMountainCraterTitleCardENGTex;
            break;
        case SCENE_SPOT18:
            texture = gGoronCityTitleCardENGTex;
            break;
        case SCENE_SPOT20:
            texture = gLonLonRanchTitleCardENGTex;
            break;
        default:
            titleCtx->texture = NULL;
            return;

    }

    static char newName[512];

    if (gSaveContext.language != LANGUAGE_ENG) {
        size_t length = strlen(texture);
        strcpy(newName, texture);
        if (gSaveContext.language == LANGUAGE_FRA) {
            newName[length - 6] = 'F';
            newName[length - 5] = 'R';
            newName[length - 4] = 'A';
        }
        else if (gSaveContext.language == LANGUAGE_GER) {
            newName[length - 6] = 'G';
            newName[length - 5] = 'E';
            newName[length - 4] = 'R';
        }
        texture = newName;
    }

    titleCtx->texture = texture;
    titleCtx->isBossCard = false;
    titleCtx->hasTranslation = false;
    titleCtx->x = x;
    titleCtx->y = y;
    titleCtx->width = width;
    titleCtx->height = height;
    titleCtx->durationTimer = 80;
    titleCtx->delayTimer = delay;
}

void TitleCard_Update(PlayState* play, TitleCardContext* titleCtx) {
    const Color_RGB8 TitleCard_Colors_ori = {255,255,255};
    Color_RGB8 TitleCard_Colors = {255,255,255};
    if (titleCtx->isBossCard && CVarGetInteger("gHudColors", 1) == 2) {//Bosses cards.
        TitleCard_Colors = CVarGetColor24("gCCTC_B_U_Prim", TitleCard_Colors_ori);
    } else if (!titleCtx->isBossCard && CVarGetInteger("gHudColors", 1) == 2) {
        TitleCard_Colors = CVarGetColor24("gCCTC_OW_U_Prim", TitleCard_Colors_ori);
    } else {
        TitleCard_Colors = TitleCard_Colors_ori;
    }

    if (DECR(titleCtx->delayTimer) == 0) {
        if (titleCtx->durationTimer == 80) {
            GameInteractor_ExecuteOnPresentTitleCard();
        }
        
        if (DECR(titleCtx->durationTimer) == 0) {
            Math_StepToS(&titleCtx->alpha, 0, 30);
            Math_StepToS(&titleCtx->intensityR, 0, 70);
            Math_StepToS(&titleCtx->intensityG, 0, 70);
            Math_StepToS(&titleCtx->intensityB, 0, 70);
        } else {
            Math_StepToS(&titleCtx->alpha, 255, 10);
            Math_StepToS(&titleCtx->intensityR, TitleCard_Colors.r, 20);
            Math_StepToS(&titleCtx->intensityG, TitleCard_Colors.g, 20);
            Math_StepToS(&titleCtx->intensityB, TitleCard_Colors.b, 20);
        }
    }
}

void TitleCard_Draw(PlayState* play, TitleCardContext* titleCtx) {
    s32 width;
    s32 height;
    s32 titleX;
    s32 doubleWidth;
    s32 titleY;

    if (titleCtx->alpha != 0) {
        width = titleCtx->width;
        height = titleCtx->height;
        s16 TitleCard_PosX_Modifier = (titleCtx->isBossCard ? CVarGetInteger("gTCBPosX", 0) : CVarGetInteger("gTCMPosX", 0));
        s16 TitleCard_PosY_Modifier = (titleCtx->isBossCard ? CVarGetInteger("gTCBPosY", 0) : CVarGetInteger("gTCMPosY", 0));
        s16 TitleCard_PosType_Checker = (titleCtx->isBossCard ? CVarGetInteger("gTCBPosType", 0) : CVarGetInteger("gTCMPosType", 0));
        s16 TitleCard_Margin_Checker = (titleCtx->isBossCard ? CVarGetInteger("gTCBUseMargins", 0) : CVarGetInteger("gTCMUseMargins", 0));
        s16 TitleCard_MarginX = 0;
        s16 TitleCard_PosX = titleCtx->x;
        s16 TitleCard_PosY = titleCtx->y;
        if (TitleCard_PosType_Checker != 0) {
            TitleCard_PosY = TitleCard_PosY_Modifier;
            if (TitleCard_PosType_Checker == 1) {//Anchor Left
                if (TitleCard_Margin_Checker != 0) {TitleCard_MarginX = CVarGetInteger("gHUDMargin_L", 0)*-1;};
                TitleCard_PosX = OTRGetDimensionFromLeftEdge(TitleCard_PosX_Modifier+TitleCard_MarginX)-11;            
            } else if (TitleCard_PosType_Checker == 2) {//Anchor Right
                if (TitleCard_Margin_Checker != 0) {TitleCard_MarginX = CVarGetInteger("gHUDMargin_R", 0);};
                TitleCard_PosX = OTRGetDimensionFromRightEdge(TitleCard_PosX_Modifier+TitleCard_MarginX);
            } else if (TitleCard_PosType_Checker == 3) {//Anchor None
                TitleCard_PosX = TitleCard_PosX_Modifier;
            } else if (TitleCard_PosType_Checker == 4) {//Hidden
                TitleCard_PosX = -9999;
            }
        }

        titleX = (TitleCard_PosX * 4) - (width * 2);
        titleY = (TitleCard_PosY * 4) - (height * 2);
        doubleWidth = width * 2;

        OPEN_DISPS(play->state.gfxCtx);

        // WORLD_OVERLAY_DISP Goes over POLY_XLU_DISP but under POLY_KAL_DISP
        WORLD_OVERLAY_DISP = Gfx_SetupDL_52NoCD(WORLD_OVERLAY_DISP);

        gDPSetPrimColor(WORLD_OVERLAY_DISP++, 0, 0, (u8)titleCtx->intensityR, (u8)titleCtx->intensityG, (u8)titleCtx->intensityB,
                        (u8)titleCtx->alpha);

        gDPLoadTextureBlock(WORLD_OVERLAY_DISP++, (uintptr_t)titleCtx->texture, G_IM_FMT_IA,
                            G_IM_SIZ_8b,
                            width, height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPWideTextureRectangle(WORLD_OVERLAY_DISP++, titleX, titleY, ((doubleWidth * 2) + titleX) - 4, titleY + (height * 4),
                            G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

        height = titleCtx->height - height;

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

s32 func_8002D53C(PlayState* play, TitleCardContext* titleCtx) {
    if ((play->actorCtx.titleCtx.delayTimer != 0) || (play->actorCtx.titleCtx.alpha != 0)) {
        titleCtx->durationTimer = 0;
        titleCtx->delayTimer = 0;
        return false;
    }

    return true;
}

void Actor_Kill(Actor* actor) {
    actor->draw = NULL;
    actor->update = NULL;
    actor->flags &= ~ACTOR_FLAG_TARGETABLE;
}

void Actor_SetWorldToHome(Actor* actor) {
    actor->world = actor->home;
}

void Actor_SetFocus(Actor* actor, f32 yOffset) {
    actor->focus.pos.x = actor->world.pos.x;
    actor->focus.pos.y = actor->world.pos.y + yOffset;
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
    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[actor->objBankIndex].segment);
}

void Actor_Init(Actor* actor, PlayState* play) {
    Actor_SetWorldToHome(actor);
    Actor_SetShapeRotToWorld(actor);
    Actor_SetFocus(actor, 0.0f);
    Math_Vec3f_Copy(&actor->prevPos, &actor->world.pos);
    Actor_SetScale(actor, 0.01f);
    actor->targetMode = 3;
    actor->minVelocityY = -20.0f;
    actor->xyzDistToPlayerSq = FLT_MAX;
    actor->naviEnemyId = 0xFF;
    actor->uncullZoneForward = 1000.0f;
    actor->uncullZoneScale = 350.0f;
    actor->uncullZoneDownward = 700.0f;
    if (CVarGetInteger("gDisableDrawDistance", 0) != 0 && actor->id != ACTOR_EN_TORCH2 && actor->id != ACTOR_EN_BLKOBJ // Extra check for Dark Link and his room 
        && actor->id != ACTOR_EN_HORSE // Check for Epona, else if we call her she will spawn at the other side of the  map + we can hear her during the title screen sequence
        && actor->id != ACTOR_EN_HORSE_GANON && actor->id != ACTOR_EN_HORSE_ZELDA  // check for Zelda's and Ganondorf's horses that will always be scene during cinematic whith camera paning
        && (play->sceneNum != SCENE_DDAN && actor->id != ACTOR_EN_ZF)) { // Check for DC and Lizalfos for the case where the miniboss music would still play under certains conditions and changing room
        actor->uncullZoneForward = 32767.0f;
        actor->uncullZoneScale = 32767.0f;
        actor->uncullZoneDownward = 32767.0f;
    }
    CollisionCheck_InitInfo(&actor->colChkInfo);
    actor->floorBgId = BGCHECK_SCENE;
    ActorShape_Init(&actor->shape, 0.0f, NULL, 0.0f);
    //if (Object_IsLoaded(&play->objectCtx, actor->objBankIndex))
    {
        //Actor_SetObjectDependency(play, actor);
        actor->init(actor, play);
        actor->init = NULL;

        GameInteractor_ExecuteOnActorInit(actor);

        // For enemy health bar we need to know the max health during init
        if (actor->category == ACTORCAT_ENEMY) {
            actor->maximumHealth = actor->colChkInfo.health;
        }
    }
}

void Actor_Destroy(Actor* actor, PlayState* play) {
    if (actor->destroy != NULL) {
        actor->destroy(actor, play);
        actor->destroy = NULL;
    } else {
        // "No Actor class destruct [%s]"
        osSyncPrintf("Ａｃｔｏｒクラス デストラクトがありません [%s]\n" VT_RST, ActorDB_Retrieve(actor->id)->name);
    }

    NameTag_RemoveAllForActor(actor);
}

void func_8002D7EC(Actor* actor) {
    f32 speedRate = R_UPDATE_RATE * 0.5f;

    actor->world.pos.x += (actor->velocity.x * speedRate) + actor->colChkInfo.displacement.x;
    actor->world.pos.y += (actor->velocity.y * speedRate) + actor->colChkInfo.displacement.y;
    actor->world.pos.z += (actor->velocity.z * speedRate) + actor->colChkInfo.displacement.z;
}

void func_8002D868(Actor* actor) {
    actor->velocity.x = Math_SinS(actor->world.rot.y) * actor->speedXZ;
    actor->velocity.z = Math_CosS(actor->world.rot.y) * actor->speedXZ;

    actor->velocity.y += actor->gravity;
    if (actor->velocity.y < actor->minVelocityY) {
        actor->velocity.y = actor->minVelocityY;
    }
}

void Actor_MoveForward(Actor* actor) {
    func_8002D868(actor);
    func_8002D7EC(actor);
}

void func_8002D908(Actor* actor) {
    f32 sp24 = Math_CosS(actor->world.rot.x) * actor->speedXZ;

    actor->velocity.x = Math_SinS(actor->world.rot.y) * sp24;
    actor->velocity.y = Math_SinS(actor->world.rot.x) * actor->speedXZ;
    actor->velocity.z = Math_CosS(actor->world.rot.y) * sp24;
}

void func_8002D97C(Actor* actor) {
    func_8002D908(actor);
    func_8002D7EC(actor);
}

void func_8002D9A4(Actor* actor, f32 arg1) {
    actor->speedXZ = Math_CosS(actor->world.rot.x) * arg1;
    actor->velocity.y = -Math_SinS(actor->world.rot.x) * arg1;
}

void func_8002D9F8(Actor* actor, SkelAnime* skelAnime) {
    Vec3f sp1C;

    SkelAnime_UpdateTranslation(skelAnime, &sp1C, actor->shape.rot.y);
    actor->world.pos.x += sp1C.x * actor->scale.x;
    actor->world.pos.y += sp1C.y * actor->scale.y;
    actor->world.pos.z += sp1C.z * actor->scale.z;
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

void func_8002DBD0(Actor* actor, Vec3f* result, Vec3f* arg2) {
    f32 cosRot2Y;
    f32 sinRot2Y;
    f32 deltaX;
    f32 deltaZ;

    cosRot2Y = Math_CosS(actor->shape.rot.y);
    sinRot2Y = Math_SinS(actor->shape.rot.y);
    deltaX = arg2->x - actor->world.pos.x;
    deltaZ = arg2->z - actor->world.pos.z;

    result->x = (deltaX * cosRot2Y) - (deltaZ * sinRot2Y);
    result->z = (deltaX * sinRot2Y) + (deltaZ * cosRot2Y);
    result->y = arg2->y - actor->world.pos.y;
}

f32 Actor_HeightDiff(Actor* actorA, Actor* actorB) {
    return actorB->world.pos.y - actorA->world.pos.y;
}

f32 Player_GetHeight(Player* player) {
    f32 offset = (player->stateFlags1 & 0x800000) ? 32.0f : 0.0f;

    if (LINK_IS_ADULT) {
        return offset + 68.0f;
    } else {
        return offset + 44.0f;
    }
}

f32 func_8002DCE4(Player* player) {
    if (player->stateFlags1 & 0x800000) {
        return 8.0f;
    } else if (player->stateFlags1 & 0x8000000) {
        return (R_RUN_SPEED_LIMIT / 100.0f) * 0.6f;
    } else {
        return R_RUN_SPEED_LIMIT / 100.0f;
    }
}

s32 func_8002DD6C(Player* player) {
    return player->stateFlags1 & 0x8;
}

s32 func_8002DD78(Player* player) {
    return func_8002DD6C(player) && player->unk_834;
}

s32 func_8002DDA8(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return (player->stateFlags1 & 0x800) || func_8002DD78(player);
}

s32 func_8002DDE4(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->stateFlags2 & 0x8;
}

s32 func_8002DDF4(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->stateFlags2 & 0x1000;
}

void func_8002DE04(PlayState* play, Actor* actorA, Actor* actorB) {
    ArmsHook* hookshot = (ArmsHook*)Actor_Find(&play->actorCtx, ACTOR_ARMS_HOOK, ACTORCAT_ITEMACTION);

    hookshot->grabbed = actorB;
    hookshot->grabbedDistDiff.x = 0.0f;
    hookshot->grabbedDistDiff.y = 0.0f;
    hookshot->grabbedDistDiff.z = 0.0f;
    actorB->flags |= ACTOR_FLAG_HOOKSHOT_ATTACHED;
    actorA->flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
}

void func_8002DE74(PlayState* play, Player* player) {
    if ((play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_4) && func_800C0CB8(play)) {
        Camera_ChangeSetting(Play_GetCamera(play, MAIN_CAM), CAM_SET_HORSE);
    }
}

void Actor_MountHorse(PlayState* play, Player* player, Actor* horse) {
    player->rideActor = horse;
    player->stateFlags1 |= 0x800000;
    horse->child = &player->actor;
}

s32 func_8002DEEC(Player* player) {
    return (player->stateFlags1 & 0x20000080) || (player->csMode != 0);
}

void func_8002DF18(PlayState* play, Player* player) {
    func_8006DC68(play, player);
}

s32 func_8002DF38(PlayState* play, Actor* actor, u8 csMode) {
    Player* player = GET_PLAYER(play);

    player->csMode = csMode;
    player->unk_448 = actor;
    player->doorBgCamIndex = 0;

    return true;
}

s32 func_8002DF54(PlayState* play, Actor* actor, u8 csMode) {
    Player* player = GET_PLAYER(play);

    func_8002DF38(play, actor, csMode);
    player->doorBgCamIndex = 1;

    return true;
}

void func_8002DF90(DynaPolyActor* dynaActor) {
    dynaActor->unk_154 = 0.0f;
    dynaActor->unk_150 = 0.0f;
}

void func_8002DFA4(DynaPolyActor* dynaActor, f32 arg1, s16 arg2) {
    dynaActor->unk_150 += arg1;
    dynaActor->unk_158 = arg2;
}

/**
 * Chcek if the player is facing the specified actor.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 */
s32 Player_IsFacingActor(Actor* actor, s16 maxAngle, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff = (s16)(actor->yawTowardsPlayer + 0x8000) - player->actor.shape.rot.y;

    if (ABS(yawDiff) < maxAngle) {
        return true;
    }

    return false;
}

/**
 * Chcek if `actorB` is facing `actorA`.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 *
 * This function is unused in the original game.
 */
s32 Actor_ActorBIsFacingActorA(Actor* actorA, Actor* actorB, s16 maxAngle) {
    s16 yawDiff = (s16)(Actor_WorldYawTowardActor(actorA, actorB) + 0x8000) - actorB->shape.rot.y;

    if (ABS(yawDiff) < maxAngle) {
        return true;
    }

    return false;
}

/**
 * Chcek if the specified actor is facing the player.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 */
s32 Actor_IsFacingPlayer(Actor* actor, s16 maxAngle) {
    s16 yawDiff = actor->yawTowardsPlayer - actor->shape.rot.y;

    if (ABS(yawDiff) < maxAngle) {
        return true;
    }

    return false;
}

/**
 * Chcek if `actorA` is facing `actorB`.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 *
 * This function is unused in the original game.
 */
s32 Actor_ActorAIsFacingActorB(Actor* actorA, Actor* actorB, s16 maxAngle) {
    s16 yawDiff = Actor_WorldYawTowardActor(actorA, actorB) - actorA->shape.rot.y;

    if (ABS(yawDiff) < maxAngle) {
        return true;
    }

    return false;
}

/**
 * Chcek if the specified actor is facing the player and is nearby.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 * The minimum distance that qualifies as "nearby" is specified by `range`.
 */
s32 Actor_IsFacingAndNearPlayer(Actor* actor, f32 range, s16 maxAngle) {
    s16 yawDiff = actor->yawTowardsPlayer - actor->shape.rot.y;

    if (ABS(yawDiff) < maxAngle) {
        f32 xyzDistanceFromLink = sqrtf(SQ(actor->xzDistToPlayer) + SQ(actor->yDistToPlayer));

        if (xyzDistanceFromLink < range) {
            return true;
        }
    }

    return false;
}

/**
 * Chcek if `actorA` is facing `actorB` and is nearby.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 * The minimum distance that qualifies as "nearby" is specified by `range`.
 */
s32 Actor_ActorAIsFacingAndNearActorB(Actor* actorA, Actor* actorB, f32 range, s16 maxAngle) {
    if (Actor_WorldDistXYZToActor(actorA, actorB) < range) {
        s16 yawDiff = Actor_WorldYawTowardActor(actorA, actorB) - actorA->shape.rot.y;

        if (ABS(yawDiff) < maxAngle) {
            return true;
        }
    }

    return false;
}

s32 func_8002E234(Actor* actor, f32 arg1, s32 arg2) {
    if ((actor->bgCheckFlags & 0x1) && (arg1 < -11.0f)) {
        actor->bgCheckFlags &= ~0x1;
        actor->bgCheckFlags |= 0x4;

        if ((actor->velocity.y < 0.0f) && (arg2 & 0x10)) {
            actor->velocity.y = 0.0f;
        }

        return false;
    }

    return true;
}

s32 func_8002E2AC(PlayState* play, Actor* actor, Vec3f* arg2, s32 arg3) {
    f32 floorHeightDiff;
    s32 floorBgId;

    arg2->y += 50.0f;

    actor->floorHeight =
        BgCheck_EntityRaycastFloor5(play, &play->colCtx, &actor->floorPoly, &floorBgId, actor, arg2);
    actor->bgCheckFlags &= ~0x0086;

    if (actor->floorHeight <= BGCHECK_Y_MIN) {
        return func_8002E234(actor, BGCHECK_Y_MIN, arg3);
    }

    floorHeightDiff = actor->floorHeight - actor->world.pos.y;
    actor->floorBgId = floorBgId;

    if (floorHeightDiff >= 0.0f) { // actor is on or below the ground
        actor->bgCheckFlags |= 0x80;

        if (actor->bgCheckFlags & 0x10) {
            if (floorBgId != sCurCeilingBgId) {
                if (floorHeightDiff > 15.0f) {
                    actor->bgCheckFlags |= 0x100;
                }
            } else {
                actor->world.pos.x = actor->prevPos.x;
                actor->world.pos.z = actor->prevPos.z;
            }
        }

        actor->world.pos.y = actor->floorHeight;

        if (actor->velocity.y <= 0.0f) {
            if (!(actor->bgCheckFlags & 0x1)) {
                actor->bgCheckFlags |= 0x2;
            } else if ((arg3 & 0x8) && (actor->gravity < 0.0f)) {
                actor->velocity.y = -4.0f;
            } else {
                actor->velocity.y = 0.0f;
            }

            actor->bgCheckFlags |= 0x1;
            func_80043334(&play->colCtx, actor, actor->floorBgId);
        }
    } else { // actor is above ground
        if ((actor->bgCheckFlags & 0x1) && (floorHeightDiff >= -11.0f)) {
            func_80043334(&play->colCtx, actor, actor->floorBgId);
        }

        return func_8002E234(actor, floorHeightDiff, arg3);
    }

    return true;
}

void Actor_UpdateBgCheckInfo(PlayState* play, Actor* actor, f32 wallCheckHeight, f32 wallCheckRadius,
                             f32 ceilingCheckHeight, s32 flags) {
    f32 sp74;
    s32 pad;
    Vec3f sp64;
    s32 bgId;
    CollisionPoly* wallPoly;
    f32 sp58;
    WaterBox* waterBox;
    f32 waterBoxYSurface;
    Vec3f ripplePos;

    sp74 = actor->world.pos.y - actor->prevPos.y;

    if ((actor->floorBgId != BGCHECK_SCENE) && (actor->bgCheckFlags & 1)) {
        func_800433A4(&play->colCtx, actor->floorBgId, actor);
    }

    if (flags & 1) {
        if ((!(flags & 0x80) &&
             BgCheck_EntitySphVsWall3(&play->colCtx, &sp64, &actor->world.pos, &actor->prevPos, wallCheckRadius,
                                      &actor->wallPoly, &bgId, actor, wallCheckHeight)) ||
            ((flags & 0x80) &&
             BgCheck_EntitySphVsWall4(&play->colCtx, &sp64, &actor->world.pos, &actor->prevPos, wallCheckRadius,
                                      &actor->wallPoly, &bgId, actor, wallCheckHeight))) {
            wallPoly = actor->wallPoly;
            Math_Vec3f_Copy(&actor->world.pos, &sp64);
            actor->wallYaw = Math_Atan2S(wallPoly->normal.z, wallPoly->normal.x);
            actor->bgCheckFlags |= 8;
            actor->wallBgId = bgId;
        } else {
            actor->bgCheckFlags &= ~8;
        }
    }

    sp64.x = actor->world.pos.x;
    sp64.z = actor->world.pos.z;

    if (flags & 2) {
        sp64.y = actor->prevPos.y + 10.0f;
        if (BgCheck_EntityCheckCeiling(&play->colCtx, &sp58, &sp64, (ceilingCheckHeight + sp74) - 10.0f,
                                       &sCurCeilingPoly, &sCurCeilingBgId, actor)) {
            actor->bgCheckFlags |= 0x10;
            actor->world.pos.y = (sp58 + sp74) - 10.0f;
        } else {
            actor->bgCheckFlags &= ~0x10;
        }
    }

    if (flags & 4) {
        sp64.y = actor->prevPos.y;
        func_8002E2AC(play, actor, &sp64, flags);
        waterBoxYSurface = actor->world.pos.y;
        if (WaterBox_GetSurface1(play, &play->colCtx, actor->world.pos.x, actor->world.pos.z,
                                 &waterBoxYSurface, &waterBox)) {
            actor->yDistToWater = waterBoxYSurface - actor->world.pos.y;
            if (actor->yDistToWater < 0.0f) {
                actor->bgCheckFlags &= ~0x60;
            } else {
                if (!(actor->bgCheckFlags & 0x20)) {
                    actor->bgCheckFlags |= 0x40;
                    if (!(flags & 0x40)) {
                        ripplePos.x = actor->world.pos.x;
                        ripplePos.y = waterBoxYSurface;
                        ripplePos.z = actor->world.pos.z;
                        EffectSsGRipple_Spawn(play, &ripplePos, 100, 500, 0);
                        EffectSsGRipple_Spawn(play, &ripplePos, 100, 500, 4);
                        EffectSsGRipple_Spawn(play, &ripplePos, 100, 500, 8);
                    }
                }
                actor->bgCheckFlags |= 0x20;
            }
        } else {
            actor->bgCheckFlags &= ~0x60;
            actor->yDistToWater = BGCHECK_Y_MIN;
        }
    }
}

Mtx D_8015BBA8;

Gfx* func_8002E830(Vec3f* object, Vec3f* eye, Vec3f* lightDir, GraphicsContext* gfxCtx, Gfx* gfx, Hilite** hilite) {
    LookAt* lookAt;
    f32 correctedEyeX;

    lookAt = Graph_Alloc(gfxCtx, sizeof(LookAt));

    correctedEyeX = (eye->x == object->x) && (eye->z == object->z) ? eye->x + 0.001f : eye->x;

    *hilite = Graph_Alloc(gfxCtx, sizeof(Hilite));

    if (HREG(80) == 6) {
        osSyncPrintf("z_actor.c 3529 eye=[%f(%f) %f %f] object=[%f %f %f] light_direction=[%f %f %f]\n", correctedEyeX,
                     eye->x, eye->y, eye->z, object->x, object->y, object->z, lightDir->x, lightDir->y, lightDir->z);
    }

    func_800ABE74(correctedEyeX, eye->y, eye->z);
    guLookAtHilite(&D_8015BBA8, lookAt, *hilite, correctedEyeX, eye->y, eye->z, object->x, object->y, object->z, 0.0f,
                   1.0f, 0.0f, lightDir->x, lightDir->y, lightDir->z, lightDir->x, lightDir->y, lightDir->z, 0x10,
                   0x10);

    gSPLookAt(gfx++, lookAt);
    gDPSetHilite1Tile(gfx++, 1, *hilite, 0x10, 0x10);

    return gfx;
}

Hilite* func_8002EABC(Vec3f* object, Vec3f* eye, Vec3f* lightDir, GraphicsContext* gfxCtx) {
    Hilite* hilite;

    OPEN_DISPS(gfxCtx);

    POLY_OPA_DISP = func_8002E830(object, eye, lightDir, gfxCtx, POLY_OPA_DISP, &hilite);

    CLOSE_DISPS(gfxCtx);

    return hilite;
}

Hilite* func_8002EB44(Vec3f* object, Vec3f* eye, Vec3f* lightDir, GraphicsContext* gfxCtx) {
    Hilite* hilite;

    OPEN_DISPS(gfxCtx);

    POLY_XLU_DISP = func_8002E830(object, eye, lightDir, gfxCtx, POLY_XLU_DISP, &hilite);

    CLOSE_DISPS(gfxCtx);

    return hilite;
}

void func_8002EBCC(Actor* actor, PlayState* play, s32 flag) {
    Hilite* hilite;
    Vec3f lightDir;
    Gfx* displayListHead;
    Gfx* displayList;

    lightDir.x = play->envCtx.dirLight1.params.dir.x;
    lightDir.y = play->envCtx.dirLight1.params.dir.y;
    lightDir.z = play->envCtx.dirLight1.params.dir.z;

    if (HREG(80) == 6) {
        osSyncPrintf("z_actor.c 3637 game_play->view.eye=[%f(%f) %f %f]\n", play->view.eye.x,
                     play->view.eye.y, play->view.eye.z);
    }

    hilite = func_8002EABC(&actor->world.pos, &play->view.eye, &lightDir, play->state.gfxCtx);

    if (flag != 0) {
        displayList = Graph_Alloc(play->state.gfxCtx, 2 * sizeof(Gfx));
        displayListHead = displayList;

        OPEN_DISPS(play->state.gfxCtx);

        gDPSetHilite1Tile(displayListHead++, 1, hilite, 0x10, 0x10);
        gSPEndDisplayList(displayListHead);
        gSPSegment(POLY_OPA_DISP++, 0x07, displayList);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void func_8002ED80(Actor* actor, PlayState* play, s32 flag) {
    Hilite* hilite;
    Vec3f lightDir;
    Gfx* displayListHead;
    Gfx* displayList;

    lightDir.x = play->envCtx.dirLight1.params.dir.x;
    lightDir.y = play->envCtx.dirLight1.params.dir.y;
    lightDir.z = play->envCtx.dirLight1.params.dir.z;

    hilite = func_8002EB44(&actor->world.pos, &play->view.eye, &lightDir, play->state.gfxCtx);

    if (flag != 0) {
        displayList = Graph_Alloc(play->state.gfxCtx, 2 * sizeof(Gfx));
        displayListHead = displayList;

        OPEN_DISPS(play->state.gfxCtx);

        gDPSetHilite1Tile(displayListHead++, 1, hilite, 0x10, 0x10);
        gSPEndDisplayList(displayListHead);
        gSPSegment(POLY_XLU_DISP++, 0x07, displayList);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

PosRot* Actor_GetFocus(PosRot* dest, Actor* actor) {
    *dest = actor->focus;

    return dest;
}

PosRot* Actor_GetWorld(PosRot* dest, Actor* actor) {
    *dest = actor->world;

    return dest;
}

PosRot* Actor_GetWorldPosShapeRot(PosRot* arg0, Actor* actor) {
    PosRot sp1C;

    Math_Vec3f_Copy(&sp1C.pos, &actor->world.pos);
    sp1C.rot = actor->shape.rot;
    *arg0 = sp1C;

    return arg0;
}

f32 func_8002EFC0(Actor* actor, Player* player, s16 arg2) {
    s16 yawTemp = (s16)(actor->yawTowardsPlayer - 0x8000) - arg2;
    s16 yawTempAbs = ABS(yawTemp);

    if (player->unk_664 != NULL) {
        if ((yawTempAbs > 0x4000) || (actor->flags & ACTOR_FLAG_NO_LOCKON)) {
            return FLT_MAX;
        } else {
            f32 ret =
                actor->xyzDistToPlayerSq - actor->xyzDistToPlayerSq * 0.8f * ((0x4000 - yawTempAbs) * (1.0f / 0x8000));

            return ret;
        }
    }

    if (yawTempAbs > 0x2AAA) {
        return FLT_MAX;
    }

    return actor->xyzDistToPlayerSq;
}

typedef struct {
    /* 0x0 */ f32 rangeSq;
    /* 0x4 */ f32 leashScale;
} TargetRangeParams; // size = 0x8

#define TARGET_RANGE(range, leash) \
    { SQ(range), (f32)range / leash }

TargetRangeParams D_80115FF8[] = {
    TARGET_RANGE(70, 140),   TARGET_RANGE(170, 255),    TARGET_RANGE(280, 5600),      TARGET_RANGE(350, 525),
    TARGET_RANGE(700, 1050), TARGET_RANGE(1000, 1500),  TARGET_RANGE(100, 105.36842), TARGET_RANGE(140, 163.33333),
    TARGET_RANGE(240, 576),  TARGET_RANGE(280, 280000),
};

u32 func_8002F090(Actor* actor, f32 arg1) {
    return arg1 < D_80115FF8[actor->targetMode].rangeSq;
}

s32 func_8002F0C8(Actor* actor, Player* player, s32 flag) {
    if ((actor->update == NULL) || !(actor->flags & ACTOR_FLAG_TARGETABLE)) {
        return true;
    }

    if (!flag) {
        s16 var = (s16)(actor->yawTowardsPlayer - 0x8000) - player->actor.shape.rot.y;
        s16 abs_var = ABS(var);
        f32 dist;

        if ((player->unk_664 == NULL) && (abs_var > 0x2AAA)) {
            dist = FLT_MAX;
        } else {
            dist = actor->xyzDistToPlayerSq;
        }

        return !func_8002F090(actor, D_80115FF8[actor->targetMode].leashScale * dist);
    }

    return false;
}

u32 Actor_ProcessTalkRequest(Actor* actor, PlayState* play) {
    if (actor->flags & ACTOR_FLAG_PLAYER_TALKED_TO) {
        actor->flags &= ~ACTOR_FLAG_PLAYER_TALKED_TO;
        return true;
    }

    return false;
}

s32 func_8002F1C4(Actor* actor, PlayState* play, f32 arg2, f32 arg3, u32 exchangeItemId) {
    Player* player = GET_PLAYER(play);

    // This is convoluted but it seems like it must be a single if statement to match
    if ((player->actor.flags & ACTOR_FLAG_PLAYER_TALKED_TO) || ((exchangeItemId != EXCH_ITEM_NONE) && Player_InCsMode(play)) ||
        (!actor->isTargeted &&
         ((arg3 < fabsf(actor->yDistToPlayer)) || (player->targetActorDistance < actor->xzDistToPlayer) ||
          (arg2 < actor->xzDistToPlayer)))) {
        return false;
    }

    player->targetActor = actor;
    player->targetActorDistance = actor->xzDistToPlayer;
    player->exchangeItemId = exchangeItemId;

    return true;
}

s32 func_8002F298(Actor* actor, PlayState* play, f32 arg2, u32 exchangeItemId) {
    return func_8002F1C4(actor, play, arg2, arg2, exchangeItemId);
}

s32 func_8002F2CC(Actor* actor, PlayState* play, f32 arg2) {
    return func_8002F298(actor, play, arg2, EXCH_ITEM_NONE);
}

s32 func_8002F2F4(Actor* actor, PlayState* play) {
    f32 var1 = 50.0f + actor->colChkInfo.cylRadius;

    return func_8002F2CC(actor, play, var1);
}

u32 Actor_TextboxIsClosing(Actor* actor, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return true;
    } else {
        return false;
    }
}

s8 func_8002F368(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->exchangeItemId;
}

void Actor_GetScreenPos(PlayState* play, Actor* actor, s16* x, s16* y) {
    Vec3f projectedPos;
    f32 w;

    func_8002BE04(play, &actor->focus.pos, &projectedPos, &w);
    *x = projectedPos.x * w * (SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 2);
    *y = projectedPos.y * w * -(SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT / 2);
}

u32 Actor_HasParent(Actor* actor, PlayState* play) {
    if (actor->parent != NULL) {
        return true;
    } else {
        return false;
    }
}

/**
 * Uses the given `GetItemEntry` to prepare the player to receive an item via the animation
 * where Link holds an item over his head. This function does not require an actor for giving
 * the player an item, instead setting the player as their own interactRangeActor and getItemDirection.
 * 
 * \param play the Global Context
 * \param getItemEntry the GetItemEntry for the item you want the player to receive.
 * \return true if the player can receive an item, false if not.
 */
s32 GiveItemEntryWithoutActor(PlayState* play, GetItemEntry getItemEntry) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & 0x3C7080) && Player_GetExplosiveHeld(player) < 0) {
        if (((player->heldActor != NULL) && ((getItemEntry.getItemId > GI_NONE) && (getItemEntry.getItemId < GI_MAX)) || 
            (gSaveContext.n64ddFlag && (getItemEntry.getItemId > RG_NONE) && (getItemEntry.getItemId < RG_MAX))) ||
            (!(player->stateFlags1 & 0x20000800))) {
            if ((getItemEntry.getItemId != GI_NONE)) {
                player->getItemEntry = getItemEntry;
                player->getItemId = getItemEntry.getItemId;
                player->interactRangeActor = &player->actor;
                player->getItemDirection = player->actor.shape.rot.y;
                return true;
            }
        }
    }

    return false;
}

/**
 * Uses the given `GetItemEntry` to prepare the player to receive an item via the animation
 * where Link holds an item over his head. This uses data from the actor link is receiving
 * the item from to set the player's interactRangeActor and getItemDirection. It also checks
 * a range from which Link must be from said actor in order to receive the item.
 *
 * \param actor the actor link is receiving an item from. Will usually be a chest but can also
 * be an npc.
 * \param play the Global Context
 * \param getItemEntry the GetItemEntry for the item you want the player to receive.
 * \param xzRange the distance on the x and z axes that the player can be from the target
 * actor to receive the item.
 * \param yRange the distance on the y axis that the player can be from the target actor
 * to receive the item.
 * \return true if the player can receive an item, false if not.
 */
s32 GiveItemEntryFromActor(Actor* actor, PlayState* play, GetItemEntry getItemEntry, f32 xzRange, f32 yRange) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & 0x3C7080) && Player_GetExplosiveHeld(player) < 0) {
        if ((((player->heldActor != NULL) || (actor == player->targetActor)) && 
            ((!gSaveContext.n64ddFlag && ((getItemEntry.getItemId > GI_NONE) && (getItemEntry.getItemId < GI_MAX))) || 
                (gSaveContext.n64ddFlag && ((getItemEntry.getItemId > RG_NONE) && (getItemEntry.getItemId < RG_MAX))))) ||
                    (!(player->stateFlags1 & 0x20000800))) {
            if ((actor->xzDistToPlayer < xzRange) && (fabsf(actor->yDistToPlayer) < yRange)) {
                s16 yawDiff = actor->yawTowardsPlayer - player->actor.shape.rot.y;
                s32 absYawDiff = ABS(yawDiff);

                if ((getItemEntry.getItemId != GI_NONE) || (player->getItemDirection < absYawDiff)) {
                    iceTrapScale = 0.0f;
                    player->getItemEntry = getItemEntry;
                    player->getItemId = getItemEntry.getItemId;
                    player->interactRangeActor = actor;
                    player->getItemDirection = absYawDiff;
                    return true;
                }
            }
        }
    }

    return false;
}

/**
 * Uses the given `GetItemEntry` to prepare the player to receive an item via the animation
 * where Link holds an item over his head. This is a wrapper function around `GiveItemEntryFromActor`
 * that supplies a fixed xzRange of 50.0f and a fixed yRange of 10.0f.
 *
 * \param actor the target actor that link is receiving an item from.
 * \param play the Global Context
 * \param getItemEntry the GetItemEntry for the item you want the player to receive.
 */
void GiveItemEntryFromActorWithFixedRange(Actor* actor, PlayState* play, GetItemEntry getItemEntry) {
    GiveItemEntryFromActor(actor, play, getItemEntry, 50.0f, 10.0f);
}

// TODO: Rename to GiveItemIdFromActor or similar
// If you're doing something for randomizer, you're probably looking for GiveItemEntryFromActor
s32 func_8002F434(Actor* actor, PlayState* play, s32 getItemId, f32 xzRange, f32 yRange) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & 0x3C7080) && Player_GetExplosiveHeld(player) < 0) {
        if ((((player->heldActor != NULL) || (actor == player->targetActor)) && 
            ((!gSaveContext.n64ddFlag && ((getItemId > GI_NONE) && (getItemId < GI_MAX))) || (gSaveContext.n64ddFlag && ((getItemId > RG_NONE) && (getItemId < RG_MAX))))) ||
            (!(player->stateFlags1 & 0x20000800))) {
            if ((actor->xzDistToPlayer < xzRange) && (fabsf(actor->yDistToPlayer) < yRange)) {
                s16 yawDiff = actor->yawTowardsPlayer - player->actor.shape.rot.y;
                s32 absYawDiff = ABS(yawDiff);

                if ((getItemId != GI_NONE) || (player->getItemDirection < absYawDiff)) {
                    player->getItemId = getItemId;
                    player->interactRangeActor = actor;
                    player->getItemDirection = absYawDiff;
                    return true;
                }
            }
        }
    }

    return false;
}

// TODO: Rename to GiveItemIdFromActorWithFixedRange or similar
// If you're doing something for randomizer, you're probably looking for GiveItemEntryFromActorWithFixedRange
void func_8002F554(Actor* actor, PlayState* play, s32 getItemId) {
    func_8002F434(actor, play, getItemId, 50.0f, 10.0f);
}

void func_8002F580(Actor* actor, PlayState* play) {
    func_8002F554(actor, play, GI_NONE);
}

u32 Actor_HasNoParent(Actor* actor, PlayState* play) {
    if (actor->parent == NULL) {
        return true;
    } else {
        return false;
    }
}

void func_8002F5C4(Actor* actorA, Actor* actorB, PlayState* play) {
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

void func_8002F5F0(Actor* actor, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (actor->xyzDistToPlayerSq < player->unk_6A4) {
        player->unk_6A4 = actor->xyzDistToPlayerSq;
    }
}

s32 Actor_IsMounted(PlayState* play, Actor* horse) {
    if (horse->child != NULL) {
        return true;
    } else {
        return false;
    }
}

u32 Actor_SetRideActor(PlayState* play, Actor* horse, s32 mountSide) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & 0x003C7880)) {
        player->rideActor = horse;
        player->mountSide = mountSide;
        return true;
    }

    return false;
}

s32 Actor_NotMounted(PlayState* play, Actor* horse) {
    if (horse->child == NULL) {
        return true;
    } else {
        return false;
    }
}

void func_8002F698(PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4, u32 arg5, u32 arg6) {
    Player* player = GET_PLAYER(play);

    player->unk_8A0 = arg6;
    player->unk_8A1 = arg5;
    player->unk_8A2 = arg3;
    player->unk_8A4 = arg2;
    player->unk_8A8 = arg4;
}

void func_8002F6D4(PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4, u32 arg5) {
    func_8002F698(play, actor, arg2, arg3, arg4, 2, arg5);
}

void func_8002F71C(PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4) {
    func_8002F6D4(play, actor, arg2, arg3, arg4, 0);
}

void func_8002F758(PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4, u32 arg5) {
    func_8002F698(play, actor, arg2, arg3, arg4, 1, arg5);
}

void func_8002F7A0(PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4) {
    func_8002F758(play, actor, arg2, arg3, arg4, 0);
}

void func_8002F7DC(Actor* actor, u16 sfxId) {
    if (actor->id != ACTOR_PLAYER || sfxId < NA_SE_VO_LI_SWORD_N || sfxId > NA_SE_VO_LI_ELECTRIC_SHOCK_LV_KID) {
        Audio_PlaySoundGeneral(sfxId, &actor->projectedPos, 4, &D_801333E0 , &D_801333E0, &D_801333E8);
    } else {
        freqMultiplier = CVarGetFloat("gLinkVoiceFreqMultiplier", 1.0);
        if (freqMultiplier <= 0) { 
            freqMultiplier = 1;
        }
        // Authentic behavior uses D_801333E0 for both freqScale and a4
        // Audio_PlaySoundGeneral(sfxId, &actor->projectedPos, 4, &D_801333E0 , &D_801333E0, &D_801333E8);
        Audio_PlaySoundGeneral(sfxId, &actor->projectedPos, 4, &freqMultiplier, &D_801333E0, &D_801333E8);
    }
}

void Audio_PlayActorSound2(Actor* actor, u16 sfxId) {
    func_80078914(&actor->projectedPos, sfxId);
}

void func_8002F850(PlayState* play, Actor* actor) {
    s32 sfxId;

    if (actor->bgCheckFlags & 0x20) {
        if (actor->yDistToWater < 20.0f) {
            sfxId = NA_SE_PL_WALK_WATER0 - SFX_FLAG;
        } else {
            sfxId = NA_SE_PL_WALK_WATER1 - SFX_FLAG;
        }
    } else {
        sfxId = SurfaceType_GetSfx(&play->colCtx, actor->floorPoly, actor->floorBgId);
    }

    func_80078914(&actor->projectedPos, NA_SE_EV_BOMB_BOUND);
    func_80078914(&actor->projectedPos, sfxId + SFX_FLAG);
}

void func_8002F8F0(Actor* actor, u16 sfxId) {
    actor->sfx = sfxId;
    actor->flags |= ACTOR_FLAG_SFX_AT_POS;
    actor->flags &= ~(ACTOR_FLAG_SFX_AT_CENTER | ACTOR_FLAG_SFX_AT_CENTER2 | ACTOR_FLAG_SFX_AS_TIMER);
}

void func_8002F91C(Actor* actor, u16 sfxId) {
    actor->sfx = sfxId;
    actor->flags |= ACTOR_FLAG_SFX_AT_CENTER;
    actor->flags &= ~(ACTOR_FLAG_SFX_AT_POS | ACTOR_FLAG_SFX_AT_CENTER2 | ACTOR_FLAG_SFX_AS_TIMER);
}

void func_8002F948(Actor* actor, u16 sfxId) {
    actor->sfx = sfxId;
    actor->flags |= ACTOR_FLAG_SFX_AT_CENTER2;
    actor->flags &= ~(ACTOR_FLAG_SFX_AT_POS | ACTOR_FLAG_SFX_AT_CENTER | ACTOR_FLAG_SFX_AS_TIMER);
}

void func_8002F974(Actor* actor, u16 sfxId) {
    actor->flags &= ~(ACTOR_FLAG_SFX_AT_POS | ACTOR_FLAG_SFX_AT_CENTER | ACTOR_FLAG_SFX_AT_CENTER2 | ACTOR_FLAG_SFX_AS_TIMER);
    actor->sfx = sfxId;
}

void func_8002F994(Actor* actor, s32 arg1) {
    actor->flags |= ACTOR_FLAG_SFX_AS_TIMER;
    actor->flags &= ~(ACTOR_FLAG_SFX_AT_POS | ACTOR_FLAG_SFX_AT_CENTER | ACTOR_FLAG_SFX_AT_CENTER2);
    if (arg1 < 40) {
        actor->sfx = NA_SE_PL_WALK_DIRT - SFX_FLAG;
    } else if (arg1 < 100) {
        actor->sfx = NA_SE_PL_WALK_CONCRETE - SFX_FLAG;
    } else {
        actor->sfx = NA_SE_PL_WALK_SAND - SFX_FLAG;
    }
}

// Tests if something hit Jabu Jabu surface, displaying hit splash and playing sfx if true
s32 func_8002F9EC(PlayState* play, Actor* actor, CollisionPoly* poly, s32 bgId, Vec3f* pos) {
    if (func_80041D4C(&play->colCtx, poly, bgId) == 8) {
        play->roomCtx.unk_74[0] = 1;
        CollisionCheck_BlueBlood(play, NULL, pos);
        Audio_PlayActorSound2(actor, NA_SE_IT_WALL_HIT_BUYO);
        return true;
    }

    return false;
}

// Local data used for Farore's Wind light (stored in BSS, possibly a struct?)
LightInfo D_8015BC00;
LightNode* D_8015BC10;
s32 D_8015BC14;
f32 D_8015BC18;

void func_8002FA60(PlayState* play) {
    Vec3f lightPos;

    if (gSaveContext.fw.set) {
        gSaveContext.respawn[RESPAWN_MODE_TOP].data = 0x28;
        gSaveContext.respawn[RESPAWN_MODE_TOP].pos.x = gSaveContext.fw.pos.x;
        gSaveContext.respawn[RESPAWN_MODE_TOP].pos.y = gSaveContext.fw.pos.y;
        gSaveContext.respawn[RESPAWN_MODE_TOP].pos.z = gSaveContext.fw.pos.z;
        gSaveContext.respawn[RESPAWN_MODE_TOP].yaw = gSaveContext.fw.yaw;
        gSaveContext.respawn[RESPAWN_MODE_TOP].playerParams = gSaveContext.fw.playerParams;
        gSaveContext.respawn[RESPAWN_MODE_TOP].entranceIndex = gSaveContext.fw.entranceIndex;
        gSaveContext.respawn[RESPAWN_MODE_TOP].roomIndex = gSaveContext.fw.roomIndex;
        gSaveContext.respawn[RESPAWN_MODE_TOP].tempSwchFlags = gSaveContext.fw.tempSwchFlags;
        gSaveContext.respawn[RESPAWN_MODE_TOP].tempCollectFlags = gSaveContext.fw.tempCollectFlags;
    } else {
        gSaveContext.respawn[RESPAWN_MODE_TOP].data = 0;
        gSaveContext.respawn[RESPAWN_MODE_TOP].pos.x = 0.0f;
        gSaveContext.respawn[RESPAWN_MODE_TOP].pos.y = 0.0f;
        gSaveContext.respawn[RESPAWN_MODE_TOP].pos.z = 0.0f;
    }

    lightPos.x = gSaveContext.respawn[RESPAWN_MODE_TOP].pos.x;
    lightPos.y = gSaveContext.respawn[RESPAWN_MODE_TOP].pos.y + 80.0f;
    lightPos.z = gSaveContext.respawn[RESPAWN_MODE_TOP].pos.z;

    Lights_PointNoGlowSetInfo(&D_8015BC00, lightPos.x, lightPos.y, lightPos.z, 0xFF, 0xFF, 0xFF, -1);

    D_8015BC10 = LightContext_InsertLight(play, &play->lightCtx, &D_8015BC00);
    D_8015BC14 = 0;
    D_8015BC18 = 0.0f;
}

void Actor_DrawFaroresWindPointer(PlayState* play) {
    s32 lightRadius = -1;
    s32 params;

    OPEN_DISPS(play->state.gfxCtx);

    params = gSaveContext.respawn[RESPAWN_MODE_TOP].data;

    if (params) {
        f32 yOffset = LINK_IS_ADULT ? 80.0f : 60.0f;
        f32 ratio = 1.0f;
        s32 alpha = 255;
        s32 temp = params - 40;

        if (temp < 0) {
            gSaveContext.respawn[RESPAWN_MODE_TOP].data = ++params;
            ratio = ABS(params) * 0.025f;
            D_8015BC14 = 60;
            D_8015BC18 = 1.0f;
        } else if (D_8015BC14) {
            D_8015BC14-= CVarGetInteger("gFastFarores", 0) ? 5 : 1;
        } else if (D_8015BC18 > 0.0f) {
            static Vec3f effectVel = { 0.0f, -0.05f, 0.0f };
            static Vec3f effectAccel = { 0.0f, -0.025f, 0.0f };
            static Color_RGBA8 effectPrimCol = { 255, 255, 255, 0 };
            static Color_RGBA8 effectEnvCol = { 100, 200, 0, 0 };
            Vec3f* curPos = &gSaveContext.respawn[RESPAWN_MODE_TOP].pos;
            Vec3f* nextPos = &gSaveContext.respawn[RESPAWN_MODE_DOWN].pos;
            f32 prevNum = D_8015BC18;
            Vec3f dist;
            f32 diff = Math_Vec3f_DistXYZAndStoreDiff(nextPos, curPos, &dist);
            Vec3f effectPos;
            f32 factor;
            f32 length;
            f32 dx;
            f32 speed;

            if (diff < 20.0f) {
                D_8015BC18 = 0.0f;
                Math_Vec3f_Copy(curPos, nextPos);
            } else {
                length = diff * (1.0f / D_8015BC18);
                speed = 20.0f / length;
                speed = CLAMP_MIN(speed, 0.05f);
                Math_StepToF(&D_8015BC18, 0.0f, speed);
                factor = (diff * (D_8015BC18 / prevNum)) / diff;
                curPos->x = nextPos->x + (dist.x * factor);
                curPos->y = nextPos->y + (dist.y * factor);
                curPos->z = nextPos->z + (dist.z * factor);
                length *= 0.5f;
                dx = diff - length;
                yOffset += sqrtf(SQ(length) - SQ(dx)) * 0.2f;
                osSyncPrintf("-------- DISPLAY Y=%f\n", yOffset);
            }

            effectPos.x = curPos->x + Rand_CenteredFloat(6.0f);
            effectPos.y = curPos->y + 80.0f + (6.0f * Rand_ZeroOne());
            effectPos.z = curPos->z + Rand_CenteredFloat(6.0f);

            EffectSsKiraKira_SpawnDispersed(play, &effectPos, &effectVel, &effectAccel, &effectPrimCol,
                                            &effectEnvCol, 1000, 16);

            if (D_8015BC18 == 0.0f) {
                gSaveContext.respawn[RESPAWN_MODE_TOP] = gSaveContext.respawn[RESPAWN_MODE_DOWN];
                gSaveContext.respawn[RESPAWN_MODE_TOP].playerParams = 0x06FF;
                gSaveContext.respawn[RESPAWN_MODE_TOP].data = 40;
            }

            gSaveContext.respawn[RESPAWN_MODE_TOP].pos = *curPos;
        } else if (temp > 0) {
            Vec3f* curPos = &gSaveContext.respawn[RESPAWN_MODE_TOP].pos;
            f32 nextRatio = 1.0f - temp * 0.1f;
            f32 curRatio = 1.0f - (f32)(temp - 1) * 0.1f;
            Vec3f eye;
            Vec3f dist;
            f32 diff;

            if (nextRatio > 0.0f) {
                eye.x = play->view.eye.x;
                eye.y = play->view.eye.y - yOffset;
                eye.z = play->view.eye.z;
                diff = Math_Vec3f_DistXYZAndStoreDiff(&eye, curPos, &dist);
                diff = (diff * (nextRatio / curRatio)) / diff;
                curPos->x = eye.x + (dist.x * diff);
                curPos->y = eye.y + (dist.y * diff);
                curPos->z = eye.z + (dist.z * diff);
                gSaveContext.respawn[RESPAWN_MODE_TOP].pos = *curPos;
            }

            alpha = 255 - (temp * 30);

            if (alpha < 0) {
                gSaveContext.fw.set = 0;
                gSaveContext.respawn[RESPAWN_MODE_TOP].data = 0;
                alpha = 0;
            } else {
                gSaveContext.respawn[RESPAWN_MODE_TOP].data = ++params;
            }

            ratio = 1.0f + ((f32)temp * 0.2); // required to match
        }

        lightRadius = 500.0f * ratio;

        if ((play->csCtx.state == CS_STATE_IDLE) &&
            (((void)0, gSaveContext.respawn[RESPAWN_MODE_TOP].entranceIndex) ==
             ((void)0, gSaveContext.entranceIndex)) &&
            (((void)0, gSaveContext.respawn[RESPAWN_MODE_TOP].roomIndex) == play->roomCtx.curRoom.num)) {
            f32 scale = 0.025f * ratio;

            POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 0x19);

            Matrix_Translate(((void)0, gSaveContext.respawn[RESPAWN_MODE_TOP].pos.x),
                             ((void)0, gSaveContext.respawn[RESPAWN_MODE_TOP].pos.y) + yOffset,
                             ((void)0, gSaveContext.respawn[RESPAWN_MODE_TOP].pos.z), MTXMODE_NEW);
            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
            Matrix_Push();

            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 128, 128, 255, 255, 200, alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 200, 0, 255);

            Matrix_RotateZ(((play->gameplayFrames * 1500) & 0xFFFF) * M_PI / 32768.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);

            Matrix_Pop();
            Matrix_RotateZ(~((play->gameplayFrames * 1200) & 0xFFFF) * M_PI / 32768.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
        }

        Lights_PointNoGlowSetInfo(&D_8015BC00, ((void)0, gSaveContext.respawn[RESPAWN_MODE_TOP].pos.x),
                                  ((void)0, gSaveContext.respawn[RESPAWN_MODE_TOP].pos.y) + yOffset,
                                  ((void)0, gSaveContext.respawn[RESPAWN_MODE_TOP].pos.z), 255, 255, 255, lightRadius);

    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80030488(PlayState* play) {
    LightContext_RemoveLight(play, &play->lightCtx, D_8015BC10);
}

void Actor_DisableLens(PlayState* play) {
    if (play->actorCtx.lensActive) {
        play->actorCtx.lensActive = false;
        func_800876C8(play);
    }
}

// Actor_InitContext
void func_800304DC(PlayState* play, ActorContext* actorCtx, ActorEntry* actorEntry) {
    SavedSceneFlags* savedSceneFlags;
    s32 i;

    savedSceneFlags = &gSaveContext.sceneFlags[play->sceneNum];

    memset(actorCtx, 0, sizeof(*actorCtx));

    Matrix_MtxFCopy(&play->billboardMtxF, &gMtxFClear);
    Matrix_MtxFCopy(&play->viewProjectionMtxF, &gMtxFClear);

    actorCtx->flags.chest = savedSceneFlags->chest;
    actorCtx->flags.swch = savedSceneFlags->swch;
    actorCtx->flags.clear = savedSceneFlags->clear;
    actorCtx->flags.collect = savedSceneFlags->collect;

    func_8002CDE4(play, &actorCtx->titleCtx);

    actorCtx->absoluteSpace = NULL;

    Actor_SpawnEntry(actorCtx, actorEntry, play);
    func_8002C0C0(&actorCtx->targetCtx, actorCtx->actorLists[ACTORCAT_PLAYER].head, play);
    func_8002FA60(play);
}

u32 D_80116068[ACTORCAT_MAX] = {
    0x100000C0, 0x100000C0, 0x00000000, 0x100004C0, 0x00000080, 0x300000C0,
    0x10000080, 0x00000000, 0x300000C0, 0x100004C0, 0x00000000, 0x100000C0,
};

void Actor_UpdateAll(PlayState* play, ActorContext* actorCtx) {
    Actor* refActor;
    Actor* actor;
    Player* player;
    u32* sp80;
    u32 unkFlag;
    u32 unkCondition;
    Actor* sp74;
    ActorEntry* actorEntry;
    s32 i;

    player = GET_PLAYER(play);

    sp74 = NULL;
    unkFlag = 0;

    if (play->numSetupActors != 0) {
        actorEntry = &play->setupActorList[0];
        for (i = 0; i < play->numSetupActors; i++) {
            Actor_SpawnEntry(&play->actorCtx, actorEntry++, play);
        }
        play->numSetupActors = 0;
        GameInteractor_ExecuteOnSceneSpawnActors();
    }

    if (actorCtx->unk_02 != 0) {
        actorCtx->unk_02--;
    }

    if (KREG(0) == -100) {
        refActor = &GET_PLAYER(play)->actor;
        KREG(0) = 0;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, refActor->world.pos.x,
                    refActor->world.pos.y + 100.0f, refActor->world.pos.z, 0, 0, 0, 1, true);
    }

    sp80 = &D_80116068[0];

    if (player->stateFlags2 & 0x8000000) {
        unkFlag = ACTOR_FLAG_NO_FREEZE_OCARINA;
    }

    if ((player->stateFlags1 & 0x40) && ((player->actor.textId & 0xFF00) != 0x600)) {
        sp74 = player->targetActor;
    }

    for (i = 0; i < ARRAY_COUNT(actorCtx->actorLists); i++, sp80++) {
        unkCondition = (*sp80 & player->stateFlags1);

        actor = actorCtx->actorLists[i].head;
        while (actor != NULL) {
            if (actor->world.pos.y < -25000.0f) {
                actor->world.pos.y = -25000.0f;
            }

            actor->sfx = 0;

            if (actor->init != NULL) {
                if (Object_IsLoaded(&play->objectCtx, actor->objBankIndex))
                {
                    Actor_SetObjectDependency(play, actor);
                    actor->init(actor, play);
                    actor->init = NULL;
                }
                actor = actor->next;
            } else if (!Object_IsLoaded(&play->objectCtx, actor->objBankIndex)) {
                Actor_Kill(actor);
                actor = actor->next;
            } else if ((unkFlag && !(actor->flags & unkFlag)) ||
                       (!unkFlag && unkCondition && (sp74 != actor) && (actor != player->naviActor) &&
                        (actor != player->heldActor) && (&player->actor != actor->parent))) {
                CollisionCheck_ResetDamage(&actor->colChkInfo);
                actor = actor->next;
            } else if (actor->update == NULL) {
                if (!actor->isDrawn) {
                    actor = Actor_Delete(&play->actorCtx, actor, play);
                } else {
                    Actor_Destroy(actor, play);
                    actor = actor->next;
                }
            } else {
                Math_Vec3f_Copy(&actor->prevPos, &actor->world.pos);
                actor->xzDistToPlayer = Actor_WorldDistXZToActor(actor, &player->actor);
                actor->yDistToPlayer = Actor_HeightDiff(actor, &player->actor);
                actor->xyzDistToPlayerSq = SQ(actor->xzDistToPlayer) + SQ(actor->yDistToPlayer);

                actor->yawTowardsPlayer = Actor_WorldYawTowardActor(actor, &player->actor);
                actor->flags &= ~ACTOR_FLAG_PLAY_HIT_SFX;

                if ((DECR(actor->freezeTimer) == 0) && (actor->flags & (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_ACTIVE))) {
                    if (actor == player->unk_664) {
                        actor->isTargeted = true;
                    } else {
                        actor->isTargeted = false;
                    }

                    if ((actor->targetPriority != 0) && (player->unk_664 == NULL)) {
                        actor->targetPriority = 0;
                    }

                    Actor_SetObjectDependency(play, actor);
                    if (actor->colorFilterTimer != 0) {
                        actor->colorFilterTimer--;
                    }
                    actor->update(actor, play);
                    GameInteractor_ExecuteOnActorUpdate(actor);
                    func_8003F8EC(play, &play->colCtx.dyna, actor);
                }

                CollisionCheck_ResetDamage(&actor->colChkInfo);

                actor = actor->next;
            }
        }

        if (i == ACTORCAT_BG) {
            DynaPoly_Setup(play, &play->colCtx.dyna);
        }
    }

    actor = player->unk_664;

    if ((actor != NULL) && (actor->update == NULL)) {
        actor = NULL;
        func_8008EDF0(player);
    }

    if ((actor == NULL) || (player->unk_66C < 5)) {
        actor = NULL;
        if (actorCtx->targetCtx.unk_4B != 0) {
            actorCtx->targetCtx.unk_4B = 0;
            func_80078884(NA_SE_SY_LOCK_OFF);
        }
    }

    func_8002C7BC(&actorCtx->targetCtx, player, actor, play);
    TitleCard_Update(play, &actorCtx->titleCtx);
    DynaPoly_UpdateBgActorTransforms(play, &play->colCtx.dyna);
}

void Actor_FaultPrint(Actor* actor, char* command) {
    char* name;

    if (actor == NULL) {
        FaultDrawer_SetCursor(48, 24);
        FaultDrawer_Printf("ACTOR NAME is NULL");
        return;
    }

    name = ActorDB_Retrieve(actor->id)->name;

    osSyncPrintf("アクターの名前(%08x:%s)\n", actor, name); // "Actor name (%08x:%s)"

    if (command != NULL) {
        osSyncPrintf("コメント:%s\n", command); // "Command:%s"
    }

    FaultDrawer_SetCursor(48, 24);
    FaultDrawer_Printf("ACTOR NAME %08x:%s", actor, name);
}

void Actor_Draw(PlayState* play, Actor* actor) {
    FaultClient faultClient;
    Lights* lights;

    Fault_AddClient(&faultClient, Actor_FaultPrint, actor, "Actor_draw");

    FrameInterpolation_RecordOpenChild(actor, 0);
    OPEN_DISPS(play->state.gfxCtx);

    lights = LightContext_NewLights(&play->lightCtx, play->state.gfxCtx);

    Lights_BindAll(lights, play->lightCtx.listHead, (actor->flags & ACTOR_FLAG_IGNORE_POINTLIGHTS) ? NULL : &actor->world.pos);
    Lights_Draw(lights, play->state.gfxCtx);

    FrameInterpolation_RecordActorPosRotMatrix();
    if (actor->flags & ACTOR_FLAG_IGNORE_QUAKE) {
        Matrix_SetTranslateRotateYXZ(
            actor->world.pos.x + play->mainCamera.skyboxOffset.x,
            actor->world.pos.y + (f32)((actor->shape.yOffset * actor->scale.y) + play->mainCamera.skyboxOffset.y),
            actor->world.pos.z + play->mainCamera.skyboxOffset.z, &actor->shape.rot);
    } else {
        Matrix_SetTranslateRotateYXZ(actor->world.pos.x, actor->world.pos.y + (actor->shape.yOffset * actor->scale.y),
                                     actor->world.pos.z, &actor->shape.rot);
    }

    Matrix_Scale(actor->scale.x, actor->scale.y, actor->scale.z, MTXMODE_APPLY);
    Actor_SetObjectDependency(play, actor);

    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[actor->objBankIndex].segment);
    gSPSegment(POLY_XLU_DISP++, 0x06, play->objectCtx.status[actor->objBankIndex].segment);

    if (actor->colorFilterTimer != 0) {
        Color_RGBA8 color = { 0, 0, 0, 255 };

        if (actor->colorFilterParams & 0x8000) {
            color.r = color.g = color.b = ((actor->colorFilterParams & 0x1F00) >> 5) | 7;
        } else if (actor->colorFilterParams & 0x4000) {
            color.r = ((actor->colorFilterParams & 0x1F00) >> 5) | 7;
        } else {
            color.b = ((actor->colorFilterParams & 0x1F00) >> 5) | 7;
        }

        if (actor->colorFilterParams & 0x2000) {
            func_80026860(play, &color, actor->colorFilterTimer, actor->colorFilterParams & 0xFF);
        } else {
            func_80026400(play, &color, actor->colorFilterTimer, actor->colorFilterParams & 0xFF);
        }
    }

    actor->draw(actor, play);

    if (actor->colorFilterTimer != 0) {
        if (actor->colorFilterParams & 0x2000) {
            func_80026A6C(play);
        } else {
            func_80026608(play);
        }
    }

    if (actor->shape.shadowDraw != NULL) {
        actor->shape.shadowDraw(actor, lights, play);
    }

    CLOSE_DISPS(play->state.gfxCtx);
    FrameInterpolation_RecordCloseChild();

    Fault_RemoveClient(&faultClient);
}

void func_80030ED8(Actor* actor) {
    if (actor->flags & ACTOR_FLAG_SFX_AT_POS) {
        Audio_PlaySoundGeneral(actor->sfx, &actor->projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (actor->flags & ACTOR_FLAG_SFX_AT_CENTER) {
        func_80078884(actor->sfx);
    } else if (actor->flags & ACTOR_FLAG_SFX_AT_CENTER2) {
        func_800788CC(actor->sfx);
    } else if (actor->flags & ACTOR_FLAG_SFX_AS_TIMER) {
        func_800F4C58(&D_801333D4, NA_SE_SY_TIMER - SFX_FLAG, (s8)(actor->sfx - 1));
    } else {
        func_80078914(&actor->projectedPos, actor->sfx);
    }
}

#define LENS_MASK_WIDTH 64
#define LENS_MASK_HEIGHT 64
// 26 and 6 are for padding between the mask texture and the screen borders
#define LENS_MASK_OFFSET_S ((SCREEN_WIDTH / 2 - LENS_MASK_WIDTH) - 26)
#define LENS_MASK_OFFSET_T ((SCREEN_HEIGHT / 2 - LENS_MASK_HEIGHT) - 6)

void Actor_DrawLensOverlay(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gDPLoadTextureBlock(POLY_XLU_DISP++, gLensOfTruthMaskTex, G_IM_FMT_I, G_IM_SIZ_8b, LENS_MASK_WIDTH,
                        LENS_MASK_HEIGHT, 0, G_TX_MIRROR | G_TX_CLAMP, G_TX_MIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD,
                        G_TX_NOLOD);

    s32 x = OTRGetRectDimensionFromLeftEdge(0) << 2;
    s32 w = OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH) << 2;

    gDPSetTileSize(POLY_XLU_DISP++, G_TX_RENDERTILE, (SCREEN_WIDTH / 2 - LENS_MASK_WIDTH) << 2,
                   (SCREEN_HEIGHT / 2 - LENS_MASK_HEIGHT) << 2, (SCREEN_WIDTH / 2 + LENS_MASK_WIDTH - 1) << 2,
                   (SCREEN_HEIGHT / 2 + LENS_MASK_HEIGHT - 1) << 2);
    gSPWideTextureRectangle(POLY_XLU_DISP++, x, 0, x + abs(x), SCREEN_HEIGHT << 2, G_TX_RENDERTILE, 0, 0, 0, 0);
    gSPWideTextureRectangle(POLY_XLU_DISP++, 0, 0, w, SCREEN_HEIGHT << 2, G_TX_RENDERTILE,
                        LENS_MASK_OFFSET_S << 5, LENS_MASK_OFFSET_T << 5,
                        (1 << 10) * (SCREEN_WIDTH - 2 * LENS_MASK_OFFSET_S) / SCREEN_WIDTH,
                        (1 << 10) * (SCREEN_HEIGHT - 2 * LENS_MASK_OFFSET_T) / SCREEN_HEIGHT);
    gDPPipeSync(POLY_XLU_DISP++);
    CLOSE_DISPS(gfxCtx);
}

void Actor_DrawLensActors(PlayState* play, s32 numInvisibleActors, Actor** invisibleActors) {
    Actor** invisibleActor;
    GraphicsContext* gfxCtx;
    s32 i;

    gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    gDPNoOpString(POLY_OPA_DISP++, "魔法のメガネ START", 0); // "Magic lens START"

    gDPPipeSync(POLY_XLU_DISP++);

    if (play->roomCtx.curRoom.lensMode == LENS_MODE_HIDE_ACTORS) {
        // Update both the color frame buffer and the z-buffer
        gDPSetOtherMode(POLY_XLU_DISP++,
                        G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                        G_AC_THRESHOLD | G_ZS_PRIM | Z_UPD | G_RM_CLD_SURF | G_RM_CLD_SURF2);

        gDPSetCombineMode(POLY_XLU_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 0, 0, 255);

        // the z-buffer will later only allow drawing inside the lens circle
    } else {
        // Update the z-buffer but not the color frame buffer
        gDPSetOtherMode(POLY_XLU_DISP++,
                        G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                        G_AC_THRESHOLD | G_ZS_PRIM | Z_UPD | IM_RD | CVG_DST_SAVE | ZMODE_OPA | FORCE_BL |
                            GBL_c1(G_BL_CLR_BL, G_BL_0, G_BL_CLR_MEM, G_BL_1MA) |
                            GBL_c2(G_BL_CLR_BL, G_BL_0, G_BL_CLR_MEM, G_BL_1MA));

        // inverts the mask image, which initially is 0 inner and 74 outer,
        // by setting the combiner to draw 74 - image instead of the image
        gDPSetCombineLERP(POLY_XLU_DISP++, PRIMITIVE, TEXEL0, PRIM_LOD_FRAC, 0, PRIMITIVE, TEXEL0, PRIM_LOD_FRAC, 0,
                          PRIMITIVE, TEXEL0, PRIM_LOD_FRAC, 0, PRIMITIVE, TEXEL0, PRIM_LOD_FRAC, 0);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0xFF, 74, 74, 74, 74);

        // the z-buffer will later only allow drawing outside the lens circle
    }

    // Together with the depth source set above, this sets the depth to the closest.
    // For a pixel with such a depth value, the z-buffer will reject drawing to that pixel.
    gDPSetPrimDepth(POLY_XLU_DISP++, 0, 0);

    // The z-buffer will be updated where the mask is not fully transparent.
    Actor_DrawLensOverlay(gfxCtx);

    // "Magic lens invisible Actor display START"
    gDPNoOpString(POLY_OPA_DISP++, "魔法のメガネ 見えないＡcｔｏｒ表示 START", numInvisibleActors);

    invisibleActor = &invisibleActors[0];
    for (i = 0; i < numInvisibleActors; i++) {
        // "Magic lens invisible Actor display"
        gDPNoOpString(POLY_OPA_DISP++, "魔法のメガネ 見えないＡcｔｏｒ表示", i);
        Actor_Draw(play, *(invisibleActor++));
    }

    // "Magic lens invisible Actor display END"
    gDPNoOpString(POLY_OPA_DISP++, "魔法のメガネ 見えないＡcｔｏｒ表示 END", numInvisibleActors);

    if (play->roomCtx.curRoom.lensMode != LENS_MODE_HIDE_ACTORS) {
        // Draw the lens overlay to the color frame buffer

        gDPNoOpString(POLY_OPA_DISP++, "青い眼鏡(外側)", 0); // "Blue spectacles (exterior)"

        gDPPipeSync(POLY_XLU_DISP++);

        gDPSetOtherMode(POLY_XLU_DISP++,
                        G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                        G_AC_THRESHOLD | G_ZS_PRIM | G_RM_CLD_SURF | G_RM_CLD_SURF2);
        gDPSetCombineMode(POLY_XLU_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 0, 0, 255);

        Actor_DrawLensOverlay(gfxCtx);

        gDPNoOpString(POLY_OPA_DISP++, "青い眼鏡(外側)", 1); // "Blue spectacles (exterior)"
    }

    gDPNoOpString(POLY_OPA_DISP++, "魔法のメガネ END", 0); // "Magic lens END"

    CLOSE_DISPS(gfxCtx);
}

s32 func_800314B0(PlayState* play, Actor* actor) {
    return func_800314D4(play, actor, &actor->projectedPos, actor->projectedW);
}

s32 func_800314D4(PlayState* play, Actor* actor, Vec3f* arg2, f32 arg3) {
    f32 var;

    if (CVarGetInteger("gDisableDrawDistance", 0) != 0 && actor->id != ACTOR_EN_TORCH2 && actor->id != ACTOR_EN_BLKOBJ // Extra check for Dark Link and his room 
        && actor->id != ACTOR_EN_HORSE // Check for Epona, else if we call her she will spawn at the other side of the  map + we can hear her during the title screen sequence
        && actor->id != ACTOR_EN_HORSE_GANON && actor->id != ACTOR_EN_HORSE_ZELDA  // check for Zelda's and Ganondorf's horses that will always be scene during cinematic whith camera paning
        && (play->sceneNum != SCENE_DDAN && actor->id != ACTOR_EN_ZF)) { // Check for DC and Lizalfos for the case where the miniboss music would still play under certains conditions and changing room
        return true;
    }

    if ((arg2->z > -actor->uncullZoneScale) && (arg2->z < (actor->uncullZoneForward + actor->uncullZoneScale))) {
        var = (arg3 < 1.0f) ? 1.0f : 1.0f / arg3;

        if ((((fabsf(arg2->x) - actor->uncullZoneScale) * var) < 2.0f) &&
            (((arg2->y + actor->uncullZoneDownward) * var) > -2.0f) &&
            (((arg2->y - actor->uncullZoneScale) * var) < 2.0f)) {
            return true;
        }
    }

    return false;
}

void func_800315AC(PlayState* play, ActorContext* actorCtx) {
    s32 invisibleActorCounter;
    Actor* invisibleActors[INVISIBLE_ACTOR_MAX];
    ActorListEntry* actorListEntry;
    Actor* actor;
    s32 i;

    invisibleActorCounter = 0;

    OPEN_DISPS(play->state.gfxCtx);

    actorListEntry = &actorCtx->actorLists[0];

    for (i = 0; i < ARRAY_COUNT(actorCtx->actorLists); i++, actorListEntry++) {
        actor = actorListEntry->head;

        while (actor != NULL) {
            char* actorName = ActorDB_Retrieve(actor->id)->name;

            gDPNoOpString(POLY_OPA_DISP++, actorName, i);
            gDPNoOpString(POLY_XLU_DISP++, actorName, i);

            HREG(66) = i;

            if ((HREG(64) != 1) || ((HREG(65) != -1) && (HREG(65) != HREG(66))) || (HREG(68) == 0)) {
                SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &actor->world.pos, &actor->projectedPos,
                                             &actor->projectedW);
            }

            if ((HREG(64) != 1) || ((HREG(65) != -1) && (HREG(65) != HREG(66))) || (HREG(69) == 0)) {
                if (actor->sfx != 0) {
                    func_80030ED8(actor);
                }
            }

            if ((HREG(64) != 1) || ((HREG(65) != -1) && (HREG(65) != HREG(66))) || (HREG(70) == 0)) {
                if (func_800314B0(play, actor)) {
                    actor->flags |= ACTOR_FLAG_ACTIVE;
                } else {
                    actor->flags &= ~ACTOR_FLAG_ACTIVE;
                }
            }

            actor->isDrawn = false;

            if ((HREG(64) != 1) || ((HREG(65) != -1) && (HREG(65) != HREG(66))) || (HREG(71) == 0)) {
                if ((actor->init == NULL) && (actor->draw != NULL) && (actor->flags & (ACTOR_FLAG_DRAW_WHILE_CULLED | ACTOR_FLAG_ACTIVE))) {
                    if ((actor->flags & ACTOR_FLAG_LENS) &&
                        ((play->roomCtx.curRoom.lensMode == LENS_MODE_HIDE_ACTORS) ||
                         play->actorCtx.lensActive || (actor->room != play->roomCtx.curRoom.num))) {
                        assert(invisibleActorCounter < INVISIBLE_ACTOR_MAX);
                        invisibleActors[invisibleActorCounter] = actor;
                        invisibleActorCounter++;
                    } else {
                        if ((HREG(64) != 1) || ((HREG(65) != -1) && (HREG(65) != HREG(66))) || (HREG(72) == 0)) {
                            Actor_Draw(play, actor);
                            actor->isDrawn = true;
                        }
                    }
                }
            }

            actor = actor->next;
        }
    }

    if ((HREG(64) != 1) || (HREG(73) != 0)) {
        Effect_DrawAll(play->state.gfxCtx);
    }

    if ((HREG(64) != 1) || (HREG(74) != 0)) {
        EffectSs_DrawAll(play);
    }

    if ((HREG(64) != 1) || (HREG(72) != 0)) {
        if (play->actorCtx.lensActive) {
            Actor_DrawLensActors(play, invisibleActorCounter, invisibleActors);
            if ((play->csCtx.state != CS_STATE_IDLE) || Player_InCsMode(play)) {
                Actor_DisableLens(play);
            }
        }
    }

    Actor_DrawFaroresWindPointer(play);

    if (IREG(32) == 0) {
        Lights_DrawGlow(play);
    }

    if ((HREG(64) != 1) || (HREG(75) != 0)) {
        TitleCard_Draw(play, &actorCtx->titleCtx);
    }

    if ((HREG(64) != 1) || (HREG(76) != 0)) {
        CollisionCheck_DrawCollision(play, &play->colChkCtx);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80031A28(PlayState* play, ActorContext* actorCtx) {
    Actor* actor;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(actorCtx->actorLists); i++) {
        actor = actorCtx->actorLists[i].head;
        while (actor != NULL) {
            if (!Object_IsLoaded(&play->objectCtx, actor->objBankIndex)) {
                Actor_Kill(actor);
            }
            actor = actor->next;
        }
    }
}

u8 sEnemyActorCategories[] = { ACTORCAT_ENEMY, ACTORCAT_BOSS };

void Actor_FreezeAllEnemies(PlayState* play, ActorContext* actorCtx, s32 duration) {
    Actor* actor;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sEnemyActorCategories); i++) {
        actor = actorCtx->actorLists[sEnemyActorCategories[i]].head;
        while (actor != NULL) {
            actor->freezeTimer = duration;
            actor = actor->next;
        }
    }
}

void func_80031B14(PlayState* play, ActorContext* actorCtx) {
    Actor* actor;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(actorCtx->actorLists); i++) {
        actor = actorCtx->actorLists[i].head;
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
    actorCtx->flags.tempClear = 0;
    actorCtx->flags.tempSwch &= 0xFFFFFF;
    play->msgCtx.unk_E3F4 = 0;
}

// Actor_CleanupContext
void func_80031C3C(ActorContext* actorCtx, PlayState* play) {
    Actor* actor;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(actorCtx->actorLists); i++) {
        actor = actorCtx->actorLists[i].head;
        while (actor != NULL) {
            Actor_Delete(actorCtx, actor, play);
            actor = actorCtx->actorLists[i].head;
        }
    }

    if (HREG(20) != 0) {
        osSyncPrintf("絶対魔法領域解放\n"); // "Absolute magic field deallocation"
    }

    if (actorCtx->absoluteSpace != NULL) {
        ZELDA_ARENA_FREE_DEBUG(actorCtx->absoluteSpace);
        actorCtx->absoluteSpace = NULL;
    }

    Play_SaveSceneFlags(play);
    func_80030488(play);
}

/**
 * Adds a given actor instance at the front of the actor list of the specified category.
 * Also sets the actor instance as being of that category.
 */
void Actor_AddToCategory(ActorContext* actorCtx, Actor* actorToAdd, u8 actorCategory) {
    Actor* prevHead;

    actorToAdd->category = actorCategory;

    actorCtx->total++;
    actorCtx->actorLists[actorCategory].length++;
    prevHead = actorCtx->actorLists[actorCategory].head;

    if (prevHead != NULL) {
        prevHead->prev = actorToAdd;
    }

    actorCtx->actorLists[actorCategory].head = actorToAdd;
    actorToAdd->next = prevHead;
}

/**
 * Removes a given actor instance from its actor list.
 * Also sets the temp clear flag of the current room if the actor removed was the last enemy loaded.
 */
Actor* Actor_RemoveFromCategory(PlayState* play, ActorContext* actorCtx, Actor* actorToRemove) {
    Actor* newHead;

    actorCtx->total--;
    actorCtx->actorLists[actorToRemove->category].length--;

    if (actorToRemove->prev != NULL) {
        actorToRemove->prev->next = actorToRemove->next;
    } else {
        actorCtx->actorLists[actorToRemove->category].head = actorToRemove->next;
    }

    newHead = actorToRemove->next;

    if (newHead != NULL) {
        newHead->prev = actorToRemove->prev;
    }

    actorToRemove->next = NULL;
    actorToRemove->prev = NULL;

    if ((actorToRemove->room == play->roomCtx.curRoom.num) && (actorToRemove->category == ACTORCAT_ENEMY) &&
        (actorCtx->actorLists[ACTORCAT_ENEMY].length == 0)) {
        Flags_SetTempClear(play, play->roomCtx.curRoom.num);
    }

    return newHead;
}

void Actor_FreeOverlay(ActorDBEntry* dbEntry) {
    osSyncPrintf(VT_FGCOL(CYAN));

    if (dbEntry->numLoaded == 0) {

        if (dbEntry->reset != NULL) {
            dbEntry->reset();
        }

        if (HREG(20) != 0) {
            osSyncPrintf("アクタークライアントが０になりました\n"); // "Actor client is now 0"
        }
    } else if (HREG(20) != 0) {
        // "%d of actor client remains"
        osSyncPrintf("アクタークライアントはあと %d 残っています\n", dbEntry->numLoaded);
    }

    osSyncPrintf(VT_RST);
}

int gMapLoading = 0;

Actor* Actor_Spawn(ActorContext* actorCtx, PlayState* play, s16 actorId, f32 posX, f32 posY, f32 posZ,
                   s16 rotX, s16 rotY, s16 rotZ, s16 params, s16 canRandomize) {

    uint8_t tryRandomizeEnemy = CVarGetInteger("gRandomizedEnemies", 0) && gSaveContext.fileNum >= 0 && gSaveContext.fileNum <= 2 && canRandomize;

    if (tryRandomizeEnemy) {
        if (!GetRandomizedEnemy(play, &actorId, &posX, &posY, &posZ, &rotX, &rotY, &rotZ, &params)) {
            return NULL;
        }
    }

    Actor* actor;
    s32 objBankIndex;
    u32 temp;

    ActorDBEntry* dbEntry = ActorDB_Retrieve(actorId);

    assert(dbEntry->valid);

    if (HREG(20) != 0) {
        // "Actor class addition [%d:%s]"
        osSyncPrintf("アクタークラス追加 [%d:%s]\n", actorId, dbEntry->name);
    }

    if (actorCtx->total > ACTOR_NUMBER_MAX) {
        // "Ａｃｔｏｒ set number exceeded"
        osSyncPrintf(VT_COL(YELLOW, BLACK) "Ａｃｔｏｒセット数オーバー\n" VT_RST);
        return NULL;
    }

    objBankIndex = Object_GetIndex(&gPlayState->objectCtx, dbEntry->objectId);

    if (objBankIndex < 0 && (!gMapLoading || CVarGetInteger("gRandomizedEnemies", 0))) {
        objBankIndex = 0;
    }

    if ((objBankIndex < 0) ||
        ((dbEntry->category == ACTORCAT_ENEMY) && Flags_GetClear(play, play->roomCtx.curRoom.num))) {
        // "No data bank!! <data bank＝%d> (profilep->bank=%d)"
        osSyncPrintf(VT_COL(RED, WHITE) "データバンク無し！！<データバンク＝%d>(profilep->bank=%d)\n" VT_RST,
                     objBankIndex, dbEntry->objectId);
        Actor_FreeOverlay(dbEntry);
        return NULL;
    }

    actor = ZELDA_ARENA_MALLOC_DEBUG(dbEntry->instanceSize);

    if (actor == NULL) {
        // "Actor class cannot be reserved! %s <size＝%d bytes>"
        osSyncPrintf(VT_COL(RED, WHITE) "Ａｃｔｏｒクラス確保できません！ %s <サイズ＝%dバイト>\n", VT_RST,
                     dbEntry->name, dbEntry->instanceSize);
        Actor_FreeOverlay(dbEntry);
        return NULL;
    }

    assert(dbEntry->numLoaded < 255);

    dbEntry->numLoaded++;

    if (HREG(20) != 0) {
        // "Actor client No. %d"
        osSyncPrintf("アクタークライアントは %d 個目です\n", dbEntry->numLoaded);
    }

    memset((u8*)actor, 0, dbEntry->instanceSize);
    actor->id = dbEntry->id;
    actor->flags = dbEntry->flags;

    if (dbEntry->id == ACTOR_EN_PART) {
        actor->objBankIndex = rotZ;
        rotZ = 0;
    } else {
        actor->objBankIndex = objBankIndex;
    }

    actor->init = dbEntry->init;
    actor->destroy = dbEntry->destroy;
    actor->update = dbEntry->update;
    actor->draw = dbEntry->draw;
    actor->room = play->roomCtx.curRoom.num;
    actor->home.pos.x = posX;
    actor->home.pos.y = posY;
    actor->home.pos.z = posZ;
    actor->home.rot.x = rotX;
    actor->home.rot.y = rotY;
    actor->home.rot.z = rotZ;
    actor->params = params;

    Actor_AddToCategory(actorCtx, actor, dbEntry->category);

    temp = gSegments[6];
    Actor_Init(actor, play);
    gSegments[6] = temp;

    return actor;
}

Actor* Actor_SpawnAsChild(ActorContext* actorCtx, Actor* parent, PlayState* play, s16 actorId, f32 posX,
                          f32 posY, f32 posZ, s16 rotX, s16 rotY, s16 rotZ, s16 params) {
    Actor* spawnedActor = Actor_Spawn(actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params, true);

    if (spawnedActor == NULL) {
        return NULL;
    }

    // The following enemies break when the parent actor isn't the same as what would happen in authentic gameplay.
    // As such, don't assign a parent to them at all when spawned with Enemy Randomizer.
    // Gohma (z_boss_goma.c), the Stalchildren spawner (z_en_encount1.c) and the falling platform spawning Stalfos in
    // Forest Temple (z_bg_mori_bigst.c) that normally rely on this behaviour are changed when
    // Enemy Rando is on so they still work properly even without assigning a parent.
    if (CVarGetInteger("gRandomizedEnemies", 0) && (spawnedActor->id == ACTOR_EN_FLOORMAS || spawnedActor->id == ACTOR_EN_PEEHAT)) {
        return spawnedActor;
    }

    parent->child = spawnedActor;
    spawnedActor->parent = parent;

    if (spawnedActor->room >= 0) {
        spawnedActor->room = parent->room;
    }

    return spawnedActor;
}

void Actor_SpawnTransitionActors(PlayState* play, ActorContext* actorCtx) {
    TransitionActorEntry* transitionActor;
    u8 numActors;
    s32 i;

    transitionActor = play->transiActorCtx.list;
    numActors = play->transiActorCtx.numActors;

    for (i = 0; i < numActors; i++) {
        if (transitionActor->id >= 0) {
            if (((transitionActor->sides[0].room >= 0) &&
                 ((transitionActor->sides[0].room == play->roomCtx.curRoom.num) ||
                  (transitionActor->sides[0].room == play->roomCtx.prevRoom.num))) ||
                ((transitionActor->sides[1].room >= 0) &&
                 ((transitionActor->sides[1].room == play->roomCtx.curRoom.num) ||
                  (transitionActor->sides[1].room == play->roomCtx.prevRoom.num)))) {
                Actor_Spawn(actorCtx, play, (s16)(transitionActor->id & 0x1FFF), transitionActor->pos.x,
                            transitionActor->pos.y, transitionActor->pos.z, 0, transitionActor->rotY, 0,
                            (i << 0xA) + transitionActor->params, true);

                transitionActor->id = -transitionActor->id;
                numActors = play->transiActorCtx.numActors;
            }
        }
        transitionActor++;
    }
}

Actor* Actor_SpawnEntry(ActorContext* actorCtx, ActorEntry* actorEntry, PlayState* play) {
    gMapLoading = 1;
    Actor* ret = Actor_Spawn(actorCtx, play, actorEntry->id, actorEntry->pos.x, actorEntry->pos.y, actorEntry->pos.z,
                       actorEntry->rot.x, actorEntry->rot.y, actorEntry->rot.z, actorEntry->params, true);
    gMapLoading = 0;

    return ret;
}

Actor* Actor_Delete(ActorContext* actorCtx, Actor* actor, PlayState* play) {
    Player* player;
    Actor* newHead;
    ActorDBEntry* dbEntry;

    player = GET_PLAYER(play);

    dbEntry = ActorDB_Retrieve(actor->id);

    if (HREG(20) != 0) {
        osSyncPrintf("アクタークラス削除 [%s]\n", dbEntry->name); // "Actor class deleted [%s]"
    }

    if ((player != NULL) && (actor == player->unk_664)) {
        func_8008EDF0(player);
        Camera_ChangeMode(Play_GetCamera(play, Play_GetActiveCamId(play)), 0);
    }

    if (actor == actorCtx->targetCtx.arrowPointedActor) {
        actorCtx->targetCtx.arrowPointedActor = NULL;
    }

    if (actor == actorCtx->targetCtx.unk_8C) {
        actorCtx->targetCtx.unk_8C = NULL;
    }

    if (actor == actorCtx->targetCtx.bgmEnemy) {
        actorCtx->targetCtx.bgmEnemy = NULL;
    }

    Audio_StopSfxByPos(&actor->projectedPos);
    Actor_Destroy(actor, play);

    newHead = Actor_RemoveFromCategory(play, actorCtx, actor);

    ZELDA_ARENA_FREE_DEBUG(actor);

    dbEntry->numLoaded--;
    Actor_FreeOverlay(dbEntry);

    return newHead;
}

s32 func_80032880(PlayState* play, Actor* actor) {
    s16 sp1E;
    s16 sp1C;

    Actor_GetScreenPos(play, actor, &sp1E, &sp1C);

    return (sp1E > -20) && (sp1E < 340) && (sp1C > -160) && (sp1C < 400);
}

Actor* D_8015BBE8;
Actor* D_8015BBEC;
f32 D_8015BBF0;
f32 sbgmEnemyDistSq;
s32 D_8015BBF8;
s16 D_8015BBFC;

void func_800328D4(PlayState* play, ActorContext* actorCtx, Player* player, u32 actorCategory) {
    f32 var;
    Actor* actor;
    Actor* sp84;
    CollisionPoly* sp80;
    s32 sp7C;
    Vec3f sp70;

    actor = actorCtx->actorLists[actorCategory].head;
    sp84 = player->unk_664;

    while (actor != NULL) {
        if ((actor->update != NULL) && ((Player*)actor != player) && CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_TARGETABLE)) {

            // This block below is for determining the closest actor to player in determining the volume
            // used while playing enemy bgm music
            if ((actorCategory == ACTORCAT_ENEMY) && CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE) &&
                (actor->xyzDistToPlayerSq < SQ(500.0f)) && (actor->xyzDistToPlayerSq < sbgmEnemyDistSq)) {
                actorCtx->targetCtx.bgmEnemy = actor;
                sbgmEnemyDistSq = actor->xyzDistToPlayerSq;
            }

            if (actor != sp84) {
                var = func_8002EFC0(actor, player, D_8015BBFC);
                if ((var < D_8015BBF0) && func_8002F090(actor, var) && func_80032880(play, actor) &&
                    (!BgCheck_CameraLineTest1(&play->colCtx, &player->actor.focus.pos, &actor->focus.pos, &sp70,
                                              &sp80, 1, 1, 1, 1, &sp7C) ||
                     SurfaceType_IsIgnoredByProjectiles(&play->colCtx, sp80, sp7C))) {
                    if (actor->targetPriority != 0) {
                        if (actor->targetPriority < D_8015BBF8) {
                            D_8015BBEC = actor;
                            D_8015BBF8 = actor->targetPriority;
                        }
                    } else {
                        D_8015BBE8 = actor;
                        D_8015BBF0 = var;
                    }
                }
            }
        }

        actor = actor->next;
    }
}

u8 D_801160A0[] = {
    ACTORCAT_BOSS,  ACTORCAT_ENEMY,  ACTORCAT_BG,   ACTORCAT_EXPLOSIVE, ACTORCAT_NPC,  ACTORCAT_ITEMACTION,
    ACTORCAT_CHEST, ACTORCAT_SWITCH, ACTORCAT_PROP, ACTORCAT_MISC,      ACTORCAT_DOOR, ACTORCAT_SWITCH,
};

Actor* func_80032AF0(PlayState* play, ActorContext* actorCtx, Actor** actorPtr, Player* player) {
    s32 i;
    u8* entry;

    D_8015BBE8 = D_8015BBEC = NULL;
    D_8015BBF0 = sbgmEnemyDistSq = FLT_MAX;
    D_8015BBF8 = 0x7FFFFFFF;

    if (!Player_InCsMode(play)) {
        entry = &D_801160A0[0];

        actorCtx->targetCtx.bgmEnemy = NULL;
        D_8015BBFC = player->actor.shape.rot.y;

        for (i = 0; i < 3; i++) {
            func_800328D4(play, actorCtx, player, *entry);
            entry++;
        }

        if (D_8015BBE8 == NULL) {
            for (; i < ARRAY_COUNT(D_801160A0); i++) {
                func_800328D4(play, actorCtx, player, *entry);
                entry++;
            }
        }
    }

    if (D_8015BBE8 == 0) {
        *actorPtr = D_8015BBEC;
    } else {
        *actorPtr = D_8015BBE8;
    }

    return *actorPtr;
}

/**
 * Finds the first actor instance of a specified ID and category if there is one.
 */
Actor* Actor_Find(ActorContext* actorCtx, s32 actorId, s32 actorCategory) {
    Actor* actor = actorCtx->actorLists[actorCategory].head;

    while (actor != NULL) {
        if (actorId == actor->id) {
            return actor;
        }
        actor = actor->next;
    }

    return NULL;
}

/**
 * Play the death sound effect and flash the screen white for 4 frames.
 * While the screen flashes, the game freezes.
 */
void Enemy_StartFinishingBlow(PlayState* play, Actor* actor) {
    play->actorCtx.freezeFlashTimer = 5;
    SoundSource_PlaySfxAtFixedWorldPos(play, &actor->world.pos, 20, NA_SE_EN_LAST_DAMAGE);
}

s16 func_80032CB4(s16* arg0, s16 arg1, s16 arg2, s16 arg3) {
    if (DECR(arg0[1]) == 0) {
        arg0[1] = Rand_S16Offset(arg1, arg2);
    }

    if ((arg0[1] - arg3) > 0) {
        arg0[0] = 0;
    } else if (((arg0[1] - arg3) > -2) || (arg0[1] < 2)) {
        arg0[0] = 1;
    } else {
        arg0[0] = 2;
    }

    return arg0[0];
}

s16 func_80032D60(s16* arg0, s16 arg1, s16 arg2, s16 arg3) {
    if (DECR(arg0[1]) == 0) {
        arg0[1] = Rand_S16Offset(arg1, arg2);
        arg0[0]++;

        if ((arg0[0] % 3) == 0) {
            arg0[0] = (s32)(Rand_ZeroOne() * arg3) * 3;
        }
    }

    return arg0[0];
}

void BodyBreak_Alloc(BodyBreak* bodyBreak, s32 count, PlayState* play) {
    u32 matricesSize;
    u32 dListsSize;
    u32 objectIdsSize;

    matricesSize = (count + 1) * sizeof(*bodyBreak->matrices);
    bodyBreak->matrices = ZELDA_ARENA_MALLOC_DEBUG(matricesSize);

    if (bodyBreak->matrices != NULL) {
        dListsSize = (count + 1) * sizeof(*bodyBreak->dLists);
        bodyBreak->dLists = ZELDA_ARENA_MALLOC_DEBUG(dListsSize);

        if (bodyBreak->dLists != NULL) {
            objectIdsSize = (count + 1) * sizeof(*bodyBreak->objectIds);
            bodyBreak->objectIds = ZELDA_ARENA_MALLOC_DEBUG(objectIdsSize);

            if (bodyBreak->objectIds != NULL) {
                memset((u8*)bodyBreak->matrices,0, matricesSize);
                memset((u8*)bodyBreak->dLists, 0, dListsSize);
                memset((u8*)bodyBreak->objectIds, 0, objectIdsSize);
                bodyBreak->val = 1;
                return;
            }
        }
    }

    if (bodyBreak->matrices != NULL) {
        ZELDA_ARENA_FREE_DEBUG(bodyBreak->matrices);
    }

    if (bodyBreak->dLists != NULL) {
        ZELDA_ARENA_FREE_DEBUG(bodyBreak->dLists);
    }

    if (bodyBreak->objectIds != NULL) {
        ZELDA_ARENA_FREE_DEBUG(bodyBreak->objectIds);
    }
}

void BodyBreak_SetInfo(BodyBreak* bodyBreak, s32 limbIndex, s32 minLimbIndex, s32 maxLimbIndex, u32 count, Gfx** dList,
                       s16 objectId) {
    PlayState* play = Effect_GetPlayState();

    if ((play->actorCtx.freezeFlashTimer == 0) && (bodyBreak->val > 0)) {
        if ((limbIndex >= minLimbIndex) && (limbIndex <= maxLimbIndex) && (*dList != NULL)) {
            bodyBreak->dLists[bodyBreak->val] = *dList;
            Matrix_Get(&bodyBreak->matrices[bodyBreak->val]);
            bodyBreak->objectIds[bodyBreak->val] = objectId;
            bodyBreak->val++;
        }

        if (limbIndex != bodyBreak->prevLimbIndex) {
            bodyBreak->count++;
        }

        if ((u32)bodyBreak->count >= count) {
            bodyBreak->count = bodyBreak->val - 1;
            bodyBreak->val = BODYBREAK_STATUS_READY;
        }
    }

    bodyBreak->prevLimbIndex = limbIndex;
}

s32 BodyBreak_SpawnParts(Actor* actor, BodyBreak* bodyBreak, PlayState* play, s16 type) {
    EnPart* spawnedEnPart;
    MtxF* mtx;
    s16 objBankIndex;

    if (bodyBreak->val != BODYBREAK_STATUS_READY) {
        return false;
    }

    while (bodyBreak->count > 0) {
        Matrix_Put(&bodyBreak->matrices[bodyBreak->count]);
        Matrix_Scale(1.0f / actor->scale.x, 1.0f / actor->scale.y, 1.0f / actor->scale.z, MTXMODE_APPLY);
        Matrix_Get(&bodyBreak->matrices[bodyBreak->count]);

        if (1) {
            if (bodyBreak->objectIds[bodyBreak->count] >= 0) {
                objBankIndex = bodyBreak->objectIds[bodyBreak->count];
            } else {
                objBankIndex = actor->objBankIndex;
            }
        }

        mtx = &bodyBreak->matrices[bodyBreak->count];

        spawnedEnPart = (EnPart*)Actor_SpawnAsChild(&play->actorCtx, actor, play, ACTOR_EN_PART, mtx->xw,
                                                    mtx->yw, mtx->zw, 0, 0, objBankIndex, type);

        if (spawnedEnPart != NULL) {
            Matrix_MtxFToYXZRotS(&bodyBreak->matrices[bodyBreak->count], &spawnedEnPart->actor.shape.rot, 0);
            spawnedEnPart->displayList = bodyBreak->dLists[bodyBreak->count];
            spawnedEnPart->actor.scale = actor->scale;
        }

        bodyBreak->count--;
    }

    bodyBreak->val = BODYBREAK_STATUS_FINISHED;

    ZELDA_ARENA_FREE_DEBUG(bodyBreak->matrices);
    ZELDA_ARENA_FREE_DEBUG(bodyBreak->dLists);
    ZELDA_ARENA_FREE_DEBUG(bodyBreak->objectIds);

    return true;
}

void Actor_SpawnFloorDustRing(PlayState* play, Actor* actor, Vec3f* posXZ, f32 radius, s32 amountMinusOne,
                              f32 randAccelWeight, s16 scale, s16 scaleStep, u8 useLighting) {
    Vec3f pos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    f32 angle;
    s32 i;

    angle = (Rand_ZeroOne() - 0.5f) * (2.0f * 3.14f);
    pos.y = actor->floorHeight;
    accel.y += (Rand_ZeroOne() - 0.5f) * 0.2f;

    for (i = amountMinusOne; i >= 0; i--) {
        pos.x = Math_SinF(angle) * radius + posXZ->x;
        pos.z = Math_CosF(angle) * radius + posXZ->z;
        accel.x = (Rand_ZeroOne() - 0.5f) * randAccelWeight;
        accel.z = (Rand_ZeroOne() - 0.5f) * randAccelWeight;

        if (scale == 0) {
            func_8002857C(play, &pos, &velocity, &accel);
        } else {
            if (useLighting) {
                func_800286CC(play, &pos, &velocity, &accel, scale, scaleStep);
            } else {
                func_8002865C(play, &pos, &velocity, &accel, scale, scaleStep);
            }
        }

        angle += (2.0f * 3.14f) / (amountMinusOne + 1.0f);
    }
}

void func_80033480(PlayState* play, Vec3f* posBase, f32 randRangeDiameter, s32 amountMinusOne, s16 scaleBase,
                   s16 scaleStep, u8 arg6) {
    Vec3f pos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    s16 scale;
    u32 var2;
    s32 i;

    for (i = amountMinusOne; i >= 0; i--) {
        pos.x = posBase->x + ((Rand_ZeroOne() - 0.5f) * randRangeDiameter);
        pos.y = posBase->y + ((Rand_ZeroOne() - 0.5f) * randRangeDiameter);
        pos.z = posBase->z + ((Rand_ZeroOne() - 0.5f) * randRangeDiameter);

        scale = (s16)((Rand_ZeroOne() * scaleBase) * 0.2f) + scaleBase;
        var2 = arg6;

        if (var2 != 0) {
            func_800286CC(play, &pos, &velocity, &accel, scale, scaleStep);
        } else {
            func_8002865C(play, &pos, &velocity, &accel, scale, scaleStep);
        }
    }
}

Actor* Actor_GetCollidedExplosive(PlayState* play, Collider* collider) {
    if ((collider->acFlags & AC_HIT) && (collider->ac->category == ACTORCAT_EXPLOSIVE)) {
        collider->acFlags &= ~AC_HIT;
        return collider->ac;
    }

    return NULL;
}

Actor* func_80033684(PlayState* play, Actor* explosiveActor) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

    while (actor != NULL) {
        if ((actor == explosiveActor) || (actor->params != 1)) {
            actor = actor->next;
        } else {
            if (Actor_WorldDistXYZToActor(explosiveActor, actor) <= (actor->shape.rot.z * 10) + 80.0f) {
                return actor;
            } else {
                actor = actor->next;
            }
        }
    }

    return NULL;
}

/**
 * Dynamically changes the category of a given actor instance.
 * This is done by moving it to the corresponding category list and setting its category variable accordingly.
 */
void Actor_ChangeCategory(PlayState* play, ActorContext* actorCtx, Actor* actor, u8 actorCategory) {
    Actor_RemoveFromCategory(play, actorCtx, actor);
    Actor_AddToCategory(actorCtx, actor, actorCategory);
}

/**
 * Checks if a hookshot or arrow actor is going to collide with the cylinder denoted by the
 * actor's `cylRadius` and `cylHeight`.
 * The check is only peformed if the projectile actor is within the provided sphere radius.
 *
 * Returns the actor if there will be collision, NULL otherwise.
 */
Actor* Actor_GetProjectileActor(PlayState* play, Actor* refActor, f32 radius) {
    Actor* actor;
    Vec3f spA8;
    f32 deltaX;
    f32 deltaY;
    f32 deltaZ;
    Vec3f sp90;
    Vec3f sp84;

    actor = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
    while (actor != NULL) {
        if (((actor->id != ACTOR_ARMS_HOOK) && (actor->id != ACTOR_EN_ARROW)) || (actor == refActor)) {
            actor = actor->next;
        } else {
            //! @bug The projectile actor gets unsafely casted to a hookshot to check its timer, even though
            //  it can also be an arrow.
            //  Luckily, the field at the same offset in the arrow actor is the x component of a vector
            //  which will rarely ever be 0. So it's very unlikely for this bug to cause an issue.
            if ((Math_Vec3f_DistXYZ(&refActor->world.pos, &actor->world.pos) > radius) ||
                (((ArmsHook*)actor)->timer == 0)) {
                actor = actor->next;
            } else {
                deltaX = Math_SinS(actor->world.rot.y) * (actor->speedXZ * 10.0f);
                deltaY = actor->velocity.y + (actor->gravity * 10.0f);
                deltaZ = Math_CosS(actor->world.rot.y) * (actor->speedXZ * 10.0f);

                spA8.x = actor->world.pos.x + deltaX;
                spA8.y = actor->world.pos.y + deltaY;
                spA8.z = actor->world.pos.z + deltaZ;

                if (CollisionCheck_CylSideVsLineSeg(refActor->colChkInfo.cylRadius, refActor->colChkInfo.cylHeight,
                                                    0.0f, &refActor->world.pos, &actor->world.pos, &spA8, &sp90,
                                                    &sp84)) {
                    return actor;
                } else {
                    actor = actor->next;
                }
            }
        }
    }

    return NULL;
}

/**
 * Sets the actor's text id with a dynamic prefix based on the current scene.
 */
void Actor_SetTextWithPrefix(PlayState* play, Actor* actor, s16 baseTextId) {
    s16 prefix;

    switch (play->sceneNum) {
        case SCENE_YDAN:
        case SCENE_YDAN_BOSS:
        case SCENE_MORIBOSSROOM:
        case SCENE_KOKIRI_HOME:
        case SCENE_KOKIRI_HOME3:
        case SCENE_KOKIRI_HOME4:
        case SCENE_KOKIRI_HOME5:
        case SCENE_KOKIRI_SHOP:
        case SCENE_LINK_HOME:
        case SCENE_SPOT04:
        case SCENE_SPOT05:
        case SCENE_SPOT10:
        case 112:
            prefix = 0x1000;
            break;
        case SCENE_MALON_STABLE:
        case SCENE_SPOT00:
        case SCENE_SPOT20:
            prefix = 0x2000;
            break;
        case SCENE_HIDAN:
        case SCENE_DDAN_BOSS:
        case SCENE_FIRE_BS:
        case SCENE_SPOT16:
        case SCENE_SPOT17:
        case SCENE_SPOT18:
            prefix = 0x3000;
            break;
        case SCENE_BDAN:
        case SCENE_BDAN_BOSS:
        case SCENE_SPOT03:
        case SCENE_SPOT07:
        case SCENE_SPOT08:
            prefix = 0x4000;
            break;
        case SCENE_HAKADAN:
        case SCENE_HAKADAN_BS:
        case SCENE_KAKARIKO:
        case SCENE_KAKARIKO3:
        case SCENE_IMPA:
        case SCENE_HUT:
        case SCENE_HAKAANA:
        case SCENE_HAKASITARELAY:
        case SCENE_SPOT01:
        case SCENE_SPOT02:
            prefix = 0x5000;
            break;
        case SCENE_JYASINZOU:
        case SCENE_JYASINBOSS:
        case SCENE_LABO:
        case SCENE_TENT:
        case SCENE_SPOT06:
        case SCENE_SPOT09:
        case SCENE_SPOT11:
            prefix = 0x6000;
            break;
        case SCENE_ENTRA:
        case SCENE_MARKET_ALLEY:
        case SCENE_MARKET_ALLEY_N:
        case SCENE_MARKET_DAY:
        case SCENE_MARKET_NIGHT:
        case SCENE_MARKET_RUINS:
        case SCENE_SPOT15:
            prefix = 0x7000;
            break;
        default:
            prefix = 0x0000;
            break;
    }

    actor->textId = prefix | baseTextId;
}

/**
 * Checks if a given actor will be standing on the ground after being translated
 * by the provided distance and angle.
 *
 * Returns true if the actor will be standing on ground.
 */
s16 Actor_TestFloorInDirection(Actor* actor, PlayState* play, f32 distance, s16 angle) {
    s16 ret;
    s16 prevBgCheckFlags;
    f32 dx;
    f32 dz;
    Vec3f prevActorPos;

    Math_Vec3f_Copy(&prevActorPos, &actor->world.pos);
    prevBgCheckFlags = actor->bgCheckFlags;

    dx = Math_SinS(angle) * distance;
    dz = Math_CosS(angle) * distance;
    actor->world.pos.x += dx;
    actor->world.pos.z += dz;

    Actor_UpdateBgCheckInfo(play, actor, 0.0f, 0.0f, 0.0f, 4);

    Math_Vec3f_Copy(&actor->world.pos, &prevActorPos);

    ret = actor->bgCheckFlags & 1;
    actor->bgCheckFlags = prevBgCheckFlags;

    return ret;
}

/**
 * Returns true if the player is targeting the provided actor
 */
s32 Actor_IsTargeted(PlayState* play, Actor* actor) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 & 0x10) && actor->isTargeted) {
        return true;
    } else {
        return false;
    }
}

/**
 * Returns true if the player is targeting an actor other than the provided actor
 */
s32 Actor_OtherIsTargeted(PlayState* play, Actor* actor) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 & 0x10) && !actor->isTargeted) {
        return true;
    } else {
        return false;
    }
}

f32 func_80033AEC(Vec3f* arg0, Vec3f* arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    f32 ret = 0.0f;

    if (arg4 <= Math_Vec3f_DistXYZ(arg0, arg1)) {
        ret = Math_SmoothStepToF(&arg1->x, arg0->x, arg2, arg3, 0.0f);
        ret += Math_SmoothStepToF(&arg1->y, arg0->y, arg2, arg3, 0.0f);
        ret += Math_SmoothStepToF(&arg1->z, arg0->z, arg2, arg3, 0.0f);
    } else if (arg5 < Math_Vec3f_DistXYZ(arg0, arg1)) {
        ret = Math_SmoothStepToF(&arg1->x, arg0->x, arg2, arg3, 0.0f);
        ret += Math_SmoothStepToF(&arg1->y, arg0->y, arg2, arg3, 0.0f);
        ret += Math_SmoothStepToF(&arg1->z, arg0->z, arg2, arg3, 0.0f);
    }

    return ret;
}

void func_80033C30(Vec3f* arg0, Vec3f* arg1, u8 alpha, PlayState* play) {
    MtxF sp60;
    f32 var;
    Vec3f sp50;
    CollisionPoly* sp4C;

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP, 0x2C);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, alpha);

    sp50.x = arg0->x;
    sp50.y = arg0->y + 1.0f;
    sp50.z = arg0->z;

    var = BgCheck_EntityRaycastFloor2(play, &play->colCtx, &sp4C, &sp50);

    if (sp4C != NULL) {
        func_80038A28(sp4C, arg0->x, var, arg0->z, &sp60);
        Matrix_Put(&sp60);
    } else {
        Matrix_Translate(arg0->x, arg0->y, arg0->z, MTXMODE_NEW);
    }

    Matrix_Scale(arg1->x, 1.0f, arg1->z, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, gCircleShadowDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80033DB8(PlayState* play, s16 arg1, s16 arg2) {
    s16 var = Quake_Add(&play->mainCamera, 3);

    Quake_SetSpeed(var, 20000);
    Quake_SetQuakeValues(var, arg1, 0, 0, 0);
    Quake_SetCountdown(var, arg2);
}

void func_80033E1C(PlayState* play, s16 arg1, s16 arg2, s16 arg3) {
    s16 var = Quake_Add(&play->mainCamera, 3);

    Quake_SetSpeed(var, arg3);
    Quake_SetQuakeValues(var, arg1, 0, 0, 0);
    Quake_SetCountdown(var, arg2);
}

void func_80033E88(Actor* actor, PlayState* play, s16 arg2, s16 arg3) {
    if (arg2 >= 5) {
        func_800AA000(actor->xyzDistToPlayerSq, 0xFF, 0x14, 0x96);
    } else {
        func_800AA000(actor->xyzDistToPlayerSq, 0xB4, 0x14, 0x64);
    }

    func_80033DB8(play, arg2, arg3);
}

f32 Rand_ZeroFloat(f32 f) {
    return Rand_ZeroOne() * f;
}

f32 Rand_CenteredFloat(f32 f) {
    return (Rand_ZeroOne() - 0.5f) * f;
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

static DoorLockInfo sDoorLocksInfo[] = {
    /* DOORLOCK_NORMAL */ { 0.54f, 6000.0f, 5000.0f, 1.0f, 0.0f, gDoorChainsDL, gDoorLockDL },
    /* DOORLOCK_BOSS */ { 0.644f, 12000.0f, 8000.0f, 1.0f, 0.0f, object_bdoor_DL_001530, object_bdoor_DL_001400 },
    /* DOORLOCK_NORMAL_SPIRIT */ { 0.64000005f, 8500.0f, 8000.0f, 1.75f, 0.1f, gDoorChainsDL, gDoorLockDL },
};

/**
 * Draws chains and lock of a locked door, of the specified `type` (see `DoorLockType`).
 * `frame` can be 0 to 10, where 0 is "open" and 10 is "closed", the chains slide accordingly.
 */
void Actor_DrawDoorLock(PlayState* play, s32 frame, s32 type) {
    DoorLockInfo* entry;
    s32 i;
    MtxF baseMtxF;
    f32 chainRotZ;
    f32 chainsTranslateX;
    f32 chainsTranslateY;
    f32 rotZStep;

    entry = &sDoorLocksInfo[type];
    chainRotZ = entry->chainsRotZInit;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(0.0f, entry->yShift, 500.0f, MTXMODE_APPLY);
    Matrix_Get(&baseMtxF);

    chainsTranslateX = sinf(entry->chainAngle - chainRotZ) * -(10 - frame) * 0.1f * entry->chainLength;
    chainsTranslateY = cosf(entry->chainAngle - chainRotZ) * (10 - frame) * 0.1f * entry->chainLength;

    for (i = 0; i < 4; i++) {
        FrameInterpolation_RecordOpenChild(entry, i);
        Matrix_Put(&baseMtxF);
        Matrix_RotateZ(chainRotZ, MTXMODE_APPLY);
        Matrix_Translate(chainsTranslateX, chainsTranslateY, 0.0f, MTXMODE_APPLY);

        if (entry->chainsScale != 1.0f) {
            Matrix_Scale(entry->chainsScale, entry->chainsScale, entry->chainsScale, MTXMODE_APPLY);
        }

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPDisplayList(POLY_OPA_DISP++, entry->chainDL);

        if (i % 2) {
            rotZStep = 2.0f * entry->chainAngle;
        } else {
            rotZStep = M_PI - (2.0f * entry->chainAngle);
        }

        chainRotZ += rotZStep;
        FrameInterpolation_RecordCloseChild();
    }

    Matrix_Put(&baseMtxF);
    Matrix_Scale(frame * 0.1f, frame * 0.1f, frame * 0.1f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, entry->lockDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_8003424C(PlayState* play, Vec3f* arg1) {
    CollisionCheck_SpawnShieldParticlesMetal(play, arg1);
}

void Actor_SetColorFilter(Actor* actor, s16 colorFlag, s16 colorIntensityMax, s16 xluFlag, s16 duration) {
    if ((colorFlag == 0x8000) && !(colorIntensityMax & 0x8000)) {
        Audio_PlayActorSound2(actor, NA_SE_EN_LIGHT_ARROW_HIT);
    }

    actor->colorFilterParams = colorFlag | xluFlag | ((colorIntensityMax & 0xF8) << 5) | duration;
    actor->colorFilterTimer = duration;
}

Hilite* func_800342EC(Vec3f* object, PlayState* play) {
    Vec3f lightDir;

    lightDir.x = play->envCtx.dirLight1.params.dir.x;
    lightDir.y = play->envCtx.dirLight1.params.dir.y;
    lightDir.z = play->envCtx.dirLight1.params.dir.z;

    return func_8002EABC(object, &play->view.eye, &lightDir, play->state.gfxCtx);
}

Hilite* func_8003435C(Vec3f* object, PlayState* play) {
    Vec3f lightDir;

    lightDir.x = play->envCtx.dirLight1.params.dir.x;
    lightDir.y = play->envCtx.dirLight1.params.dir.y;
    lightDir.z = play->envCtx.dirLight1.params.dir.z;

    return func_8002EB44(object, &play->view.eye, &lightDir, play->state.gfxCtx);
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
    s16 x;
    s16 y;

    if (Actor_ProcessTalkRequest(actor, play)) {
        *talkState = NPC_TALK_STATE_TALKING;
        return true;
    }

    if (*talkState != NPC_TALK_STATE_IDLE) {
        *talkState = updateTalkState(play, actor);
        return false;
    }

    Actor_GetScreenPos(play, actor, &x, &y);

    if ((x < 0) || (x > SCREEN_WIDTH) || (y < 0) || (y > SCREEN_HEIGHT)) {
        // Actor is offscreen
        return false;
    }

    if (!func_8002F2CC(actor, play, interactRange)) {
        return false;
    }

    actor->textId = getTextId(play, actor);

    return false;
}

typedef struct {
    /* 0x00 */ s16 maxHeadYaw;
    /* 0x02 */ s16 minHeadPitch;
    /* 0x04 */ s16 maxHeadPitch;
    /* 0x06 */ s16 maxTorsoYaw;
    /* 0x08 */ s16 minTorsoPitch;
    /* 0x0A */ s16 maxTorsoPitch;
    /* 0x0C */ u8 rotateYaw;
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
static NpcTrackingParams sNpcTrackingPresets[] = {
    { { 0x2AA8, -0x0E38, 0x18E2, 0x1554, 0x0000, 0x0000, true }, 170.0f, 0x3FFC },
    { { 0x2AA8, -0x1554, 0x1554, 0x1554, -0x071C, 0x0E38, true }, 170.0f, 0x3FFC },
    { { 0x31C4, -0x1C70, 0x0E38, 0x0E38, -0x0E38, 0x071C, true }, 170.0f, 0x3FFC },
    { { 0x1554, -0x0E38, 0x0000, 0x071C, -0x071C, 0x0000, true }, 170.0f, 0x3FFC },
    { { 0x2AA8, -0x071C, 0x071C, 0x0E38, -0x2AA8, 0x2AA8, true }, 170.0f, 0x3FFC },
    { { 0x0000, -0x1C70, 0x2AA8, 0x3FFC, -0x0E38, 0x0E38, true }, 170.0f, 0x3FFC },
    { { 0x2AA8, -0x0E38, 0x0E38, 0x0E38, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
    { { 0x2AA8, -0x0E38, 0x0000, 0x0E38, 0x0000, 0x1C70, true }, 0.0f, 0x0000 },
    { { 0x2AA8, -0x0E38, -0x0E38, 0x0000, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
    { { 0x071C, -0x0E38, 0x0E38, 0x1C70, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
    { { 0x0E38, -0x0E38, 0x0000, 0x1C70, 0x0000, 0x0E38, true }, 0.0f, 0x0000 },
    { { 0x2AA8, -0x1C70, 0x1C70, 0x0E38, -0x0E38, 0x0E38, true }, 0.0f, 0x0000 },
    { { 0x18E2, -0x0E38, 0x0E38, 0x0E38, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
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
    Math_SmoothStepToS(&interactInfo->headRot.y, temp, 6, 2000, 1);

    temp = (ABS(bodyYawDiff) >= 0x8000) ? 0 : ABS(bodyYawDiff);
    interactInfo->headRot.y = CLAMP(interactInfo->headRot.y, -temp, temp);

    bodyYawDiff -= interactInfo->headRot.y;

    temp = CLAMP(bodyYawDiff, -maxTorsoYaw, maxTorsoYaw);
    Math_SmoothStepToS(&interactInfo->torsoRot.y, temp, 6, 2000, 1);

    temp = (ABS(bodyYawDiff) >= 0x8000) ? 0 : ABS(bodyYawDiff);
    interactInfo->torsoRot.y = CLAMP(interactInfo->torsoRot.y, -temp, temp);

    if (rotateYaw) {
        Math_SmoothStepToS(&actor->shape.rot.y, yawTowardsTarget, 6, 2000, 1);
    }

    temp = CLAMP(pitchTowardsTarget, minHeadPitch, (s16)(u16)maxHeadPitch);
    Math_SmoothStepToS(&interactInfo->headRot.x, temp, 6, 2000, 1);

    torsoPitch = pitchTowardsTarget - interactInfo->headRot.x;

    temp = CLAMP(torsoPitch, minTorsoPitch, maxTorsoPitch);
    Math_SmoothStepToS(&interactInfo->torsoRot.x, temp, 6, 2000, 1);
}

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
    yawDiff = ABS((s16)((f32)yaw - actor->shape.rot.y));
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
    }

    // Auto-turn sequence complete, turn towards the player
    return NPC_TRACKING_FULL_BODY;
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
        case NPC_TRACKING_HEAD:
            rotLimits.maxTorsoYaw = 0;
            rotLimits.maxTorsoPitch = 0;
            rotLimits.minTorsoPitch = 0;
        case NPC_TRACKING_HEAD_AND_TORSO:
            rotLimits.rotateYaw = false;
    }

    Npc_TrackPointWithLimits(actor, interactInfo, rotLimits.maxHeadYaw, rotLimits.maxHeadPitch, rotLimits.minHeadPitch,
                             rotLimits.maxTorsoYaw, rotLimits.maxTorsoPitch, rotLimits.minTorsoPitch,
                             rotLimits.rotateYaw);
}

Gfx* func_80034B28(GraphicsContext* gfxCtx) {
    Gfx* displayList;

    displayList = Graph_Alloc(gfxCtx, sizeof(Gfx));
    gSPEndDisplayList(displayList);

    return displayList;
}

Gfx* func_80034B54(GraphicsContext* gfxCtx) {
    Gfx* displayListHead;
    Gfx* displayList;

    displayList = displayListHead = Graph_Alloc(gfxCtx, 2 * sizeof(Gfx));

    gDPSetRenderMode(displayListHead++, G_RM_FOG_SHADE_A,
                     AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                         GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA));

    gSPEndDisplayList(displayListHead++);

    return displayList;
}

void func_80034BA0(PlayState* play, SkelAnime* skelAnime, OverrideLimbDraw overrideLimbDraw,
                   PostLimbDraw postLimbDraw, Actor* actor, s16 alpha) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, alpha);
    gDPPipeSync(POLY_OPA_DISP++);
    gSPSegment(POLY_OPA_DISP++, 0x0C, func_80034B28(play->state.gfxCtx));

    POLY_OPA_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       overrideLimbDraw, postLimbDraw, actor, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80034CC4(PlayState* play, SkelAnime* skelAnime, OverrideLimbDraw overrideLimbDraw,
                   PostLimbDraw postLimbDraw, Actor* actor, s16 alpha) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, func_80034B54(play->state.gfxCtx));

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       overrideLimbDraw, postLimbDraw, actor, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

s16 func_80034DD4(Actor* actor, PlayState* play, s16 arg2, f32 arg3) {
    Player* player = GET_PLAYER(play);
    f32 var;

    if ((play->csCtx.state != CS_STATE_IDLE) || (gDbgCamEnabled)) {
        var = Math_Vec3f_DistXYZ(&actor->world.pos, &play->view.eye) * 0.25f;
    } else {
        var = Math_Vec3f_DistXYZ(&actor->world.pos, &player->actor.world.pos);
    }

    if (arg3 < var) {
        actor->flags &= ~ACTOR_FLAG_TARGETABLE;
        Math_SmoothStepToS(&arg2, 0, 6, 0x14, 1);
    } else {
        actor->flags |= ACTOR_FLAG_TARGETABLE;
        Math_SmoothStepToS(&arg2, 0xFF, 6, 0x14, 1);
    }

    return arg2;
}

void Animation_ChangeByInfo(SkelAnime* skelAnime, AnimationInfo* animationInfo, s32 index) {
    f32 frameCount;

    animationInfo += index;

    if (animationInfo->frameCount > 0.0f) {
        frameCount = animationInfo->frameCount;
    } else {
        frameCount = Animation_GetLastFrame(animationInfo->animation);
    }

    Animation_Change(skelAnime, animationInfo->animation, animationInfo->playSpeed, animationInfo->startFrame,
                     frameCount, animationInfo->mode, animationInfo->morphFrames);
}

void func_80034F54(PlayState* play, s16* arg1, s16* arg2, s32 arg3) {
    u32 frames = play->gameplayFrames;
    s32 i;

    for (i = 0; i < arg3; i++) {
        arg1[i] = (0x814 + 50 * i) * frames;
        arg2[i] = (0x940 + 50 * i) * frames;
    }
}

void Actor_Noop(Actor* actor, PlayState* play) {
}

s32 func_80035124(Actor* actor, PlayState* play) {
    s32 ret = 0;

    switch (actor->params) {
        case 0:
            if (Actor_HasParent(actor, play)) {
                actor->params = 1;
            } else if (!(actor->bgCheckFlags & 1)) {
                Actor_MoveForward(actor);
                Math_SmoothStepToF(&actor->speedXZ, 0.0f, 1.0f, 0.1f, 0.0f);
            } else if ((actor->bgCheckFlags & 2) && (actor->velocity.y < -4.0f)) {
                ret = 1;
            } else {
                actor->shape.rot.x = actor->shape.rot.z = 0;
                func_8002F580(actor, play);
            }
            break;
        case 1:
            if (Actor_HasNoParent(actor, play)) {
                actor->params = 0;
            }
            break;
    }

    Actor_UpdateBgCheckInfo(play, actor, actor->colChkInfo.cylHeight, actor->colChkInfo.cylRadius,
                            actor->colChkInfo.cylRadius, 0x1D);

    return ret;
}

u8 func_800353E8(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->unk_845;
}

/**
 * Finds the first actor instance of a specified ID and category within a given range from
 * an actor if there is one. If the ID provided is -1, this will look for any actor of the
 * specified category rather than a specific ID.
 */
Actor* Actor_FindNearby(PlayState* play, Actor* refActor, s16 actorId, u8 actorCategory, f32 range) {
    Actor* actor = play->actorCtx.actorLists[actorCategory].head;

    while (actor != NULL) {
        if (actor == refActor || ((actorId != -1) && (actorId != actor->id))) {
            actor = actor->next;
        } else {
            if (Actor_WorldDistXYZToActor(refActor, actor) <= range) {
                return actor;
            } else {
                actor = actor->next;
            }
        }
    }

    return NULL;
}

s32 func_800354B4(PlayState* play, Actor* actor, f32 range, s16 arg3, s16 arg4, s16 arg5) {
    Player* player = GET_PLAYER(play);
    s16 var1;
    s16 var2;

    var1 = (s16)(actor->yawTowardsPlayer + 0x8000) - player->actor.shape.rot.y;
    var2 = actor->yawTowardsPlayer - arg5;

    if ((actor->xzDistToPlayer <= range) && (player->swordState != 0) && (arg4 >= ABS(var1)) && (arg3 >= ABS(var2))) {
        return true;
    } else {
        return false;
    }
}

void func_8003555C(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    Color_RGBA8 color1;
    Color_RGBA8 color2;

    color1.r = 200;
    color1.g = 160;
    color1.b = 120;

    color2.r = 130;
    color2.g = 90;
    color2.b = 50;

    //! @bug color1 and color2 alpha components not set before being passed on
    EffectSsKiraKira_SpawnSmall(play, pos, velocity, accel, &color1, &color2);
}

Vec3f D_80116268 = { 0.0f, -1.5f, 0.0f };
Vec3f D_80116274 = { 0.0f, -0.2f, 0.0f };

Gfx D_80116280[] = {
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU |
                                            FORCE_BL | GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
    gsDPSetAlphaCompare(G_AC_THRESHOLD),
    gsSPEndDisplayList(),
};

void func_800355B8(PlayState* play, Vec3f* pos) {
    func_8003555C(play, pos, &D_80116268, &D_80116274);
}

u8 func_800355E4(PlayState* play, Collider* collider) {
    Player* player = GET_PLAYER(play);

    if ((collider->acFlags & AC_TYPE_PLAYER) && (player->swordState != 0) && (player->meleeWeaponAnimation == 0x16)) {
        return true;
    } else {
        return false;
    }
}

u8 Actor_ApplyDamage(Actor* actor) {
    if (actor->colChkInfo.damage >= actor->colChkInfo.health) {
        actor->colChkInfo.health = 0;
    } else {
        actor->colChkInfo.health -= actor->colChkInfo.damage;
    }

    return actor->colChkInfo.health;
}

void Actor_SetDropFlag(Actor* actor, ColliderInfo* colInfo, s32 freezeFlag) {
    if (colInfo->acHitInfo == NULL) {
        actor->dropFlag = 0x00;
    } else if (freezeFlag && (colInfo->acHitInfo->toucher.dmgFlags & 0x10060000)) {
        actor->freezeTimer = colInfo->acHitInfo->toucher.damage;
        actor->dropFlag = 0x00;
    } else if (colInfo->acHitInfo->toucher.dmgFlags & 0x0800) {
        actor->dropFlag = 0x01;
    } else if (colInfo->acHitInfo->toucher.dmgFlags & 0x1000) {
        actor->dropFlag = 0x02;
    } else if (colInfo->acHitInfo->toucher.dmgFlags & 0x4000) {
        actor->dropFlag = 0x04;
    } else if (colInfo->acHitInfo->toucher.dmgFlags & 0x8000) {
        actor->dropFlag = 0x08;
    } else if (colInfo->acHitInfo->toucher.dmgFlags & 0x10000) {
        actor->dropFlag = 0x10;
    } else if (colInfo->acHitInfo->toucher.dmgFlags & 0x2000) {
        actor->dropFlag = 0x20;
    } else if (colInfo->acHitInfo->toucher.dmgFlags & 0x80000) {
        if (freezeFlag) {
            actor->freezeTimer = colInfo->acHitInfo->toucher.damage;
        }
        actor->dropFlag = 0x40;
    } else {
        actor->dropFlag = 0x00;
    }
}

void Actor_SetDropFlagJntSph(Actor* actor, ColliderJntSph* jntSph, s32 freezeFlag) {
    ColliderInfo* curColInfo;
    s32 flag;
    s32 i;

    actor->dropFlag = 0x00;

    for (i = jntSph->count - 1; i >= 0; i--) {
        curColInfo = &jntSph->elements[i].info;
        if (curColInfo->acHitInfo == NULL) {
            flag = 0x00;
        } else if (freezeFlag && (curColInfo->acHitInfo->toucher.dmgFlags & 0x10060000)) {
            actor->freezeTimer = curColInfo->acHitInfo->toucher.damage;
            flag = 0x00;
        } else if (curColInfo->acHitInfo->toucher.dmgFlags & 0x0800) {
            flag = 0x01;
        } else if (curColInfo->acHitInfo->toucher.dmgFlags & 0x1000) {
            flag = 0x02;
        } else if (curColInfo->acHitInfo->toucher.dmgFlags & 0x4000) {
            flag = 0x04;
        } else if (curColInfo->acHitInfo->toucher.dmgFlags & 0x8000) {
            flag = 0x08;
        } else if (curColInfo->acHitInfo->toucher.dmgFlags & 0x10000) {
            flag = 0x10;
        } else if (curColInfo->acHitInfo->toucher.dmgFlags & 0x2000) {
            flag = 0x20;
        } else if (curColInfo->acHitInfo->toucher.dmgFlags & 0x80000) {
            if (freezeFlag) {
                actor->freezeTimer = curColInfo->acHitInfo->toucher.damage;
            }
            flag = 0x40;
        } else {
            flag = 0x00;
        }
        actor->dropFlag |= flag;
    }
}

void func_80035844(Vec3f* arg0, Vec3f* arg1, Vec3s* arg2, s32 arg3) {
    f32 dx = arg1->x - arg0->x;
    f32 dz = arg1->z - arg0->z;
    f32 dy = arg3 ? (arg1->y - arg0->y) : (arg0->y - arg1->y);

    arg2->y = Math_Atan2S(dz, dx);
    arg2->x = Math_Atan2S(sqrtf(SQ(dx) + SQ(dz)), dy);
}

/**
 * Spawns En_Part (Dissipating Flames) actor as a child of the given actor.
 */
Actor* func_800358DC(Actor* actor, Vec3f* spawnPos, Vec3s* spawnRot, f32* arg3, s32 timer, s16* unused,
                     PlayState* play, s16 params, s32 arg8) {
    EnPart* spawnedEnPart;

    spawnedEnPart =
        (EnPart*)Actor_SpawnAsChild(&play->actorCtx, actor, play, ACTOR_EN_PART, spawnPos->x, spawnPos->y,
                                    spawnPos->z, spawnRot->x, spawnRot->y, actor->objBankIndex, params);
    if (spawnedEnPart != NULL) {
        spawnedEnPart->actor.scale = actor->scale;
        spawnedEnPart->actor.speedXZ = arg3[0];
        spawnedEnPart->displayList = arg8;
        spawnedEnPart->action = 2;
        spawnedEnPart->timer = timer;
        spawnedEnPart->rotZ = arg3[1];
        spawnedEnPart->rotZSpeed = arg3[2];
        return &spawnedEnPart->actor;
    }

    return NULL;
}

void func_800359B8(Actor* actor, s16 arg1, Vec3s* arg2) {
    f32 sp44;
    f32 sp40;
    f32 sp3C;
    f32 sp38;
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    f32 sp28;
    f32 sp24;
    CollisionPoly* floorPoly;
    s32 pad;

    if (actor->floorPoly != NULL) {
        floorPoly = actor->floorPoly;
        sp44 = COLPOLY_GET_NORMAL(floorPoly->normal.x);
        sp40 = COLPOLY_GET_NORMAL(floorPoly->normal.y);
        sp3C = COLPOLY_GET_NORMAL(floorPoly->normal.z);

        sp38 = Math_SinS(arg1);
        sp34 = Math_CosS(arg1);
        sp28 = (-(sp44 * sp38) - (sp3C * sp34));
        arg2->x = -(s16)(Math_FAtan2F(sp28 * sp40, 1.0f) * (32768 / M_PI));

        sp2C = Math_SinS(arg1 - 16375);
        sp30 = Math_CosS(arg1 - 16375);
        sp24 = (-(sp44 * sp2C) - (sp3C * sp30));
        arg2->z = -(s16)(Math_FAtan2F(sp24 * sp40, 1.0f) * (32768 / M_PI));
    }
}

void func_80035B18(PlayState* play, Actor* actor, u16 textId) {
    Message_ContinueTextbox(play, textId);
    actor->textId = textId;
}

/**
 * Tests if "eventChkInf" flag is set.
 */
s32 Flags_GetEventChkInf(s32 flag) {
    return gSaveContext.eventChkInf[flag >> 4] & (1 << (flag & 0xF));
}

/**
 * Sets "eventChkInf" flag.
 */
void Flags_SetEventChkInf(s32 flag) {
    gSaveContext.eventChkInf[flag >> 4] |= (1 << (flag & 0xF));
}

/**
 * Unsets "eventChkInf" flag.
 */
void Flags_UnsetEventChkInf(s32 flag) {
    gSaveContext.eventChkInf[flag >> 4] &= ~(1 << (flag & 0xF));
}

/**
 * Tests if "itemGetInf" flag is set.
 */
s32 Flags_GetItemGetInf(s32 flag) {
    return gSaveContext.itemGetInf[flag >> 4] & (1 << (flag & 0xF));
}

/**
 * Sets "itemGetInf" flag.
 */
void Flags_SetItemGetInf(s32 flag) {
    gSaveContext.itemGetInf[flag >> 4] |= (1 << (flag & 0xF));
}

/**
 * Unsets "itemGetInf" flag.
 */
void Flags_UnsetItemGetInf(s32 flag) {
    gSaveContext.itemGetInf[flag >> 4] &= ~(1 << (flag & 0xF));
}

/**
 * Tests if "infTable" flag is set.
 */
s32 Flags_GetInfTable(s32 flag) {
    return gSaveContext.infTable[flag >> 4] & (1 << (flag & 0xF));
}

/**
 * Sets "infTable" flag.
 */
void Flags_SetInfTable(s32 flag) {
    gSaveContext.infTable[flag >> 4] |= (1 << (flag & 0xF));
}

/**
 * Unsets "infTable" flag.
 */
void Flags_UnsetInfTable(s32 flag) {
    gSaveContext.infTable[flag >> 4] &= ~(1 << (flag & 0xF));
}

/**
 * Tests if "eventInf" flag is set.
 */
s32 Flags_GetEventInf(s32 flag) {
    return gSaveContext.eventInf[flag >> 4] & (1 << (flag & 0xF));
}

/**
 * Sets "eventInf" flag.
 */
void Flags_SetEventInf(s32 flag) {
    gSaveContext.eventInf[flag >> 4] |= (1 << (flag & 0xF));
}

/**
 * Unsets "eventInf" flag.
 */
void Flags_UnsetEventInf(s32 flag) {
    gSaveContext.eventInf[flag >> 4] &= ~(1 << (flag & 0xF));
}

/**
 * Tests if "randomizerInf" flag is set.
 */
s32 Flags_GetRandomizerInf(RandomizerInf flag) {
    return gSaveContext.randomizerInf[flag >> 4] & (1 << (flag & 0xF));
}

/**
 * Sets "randomizerInf" flag.
 */
void Flags_SetRandomizerInf(RandomizerInf flag) {
    gSaveContext.randomizerInf[flag >> 4] |= (1 << (flag & 0xF));
}

/**
 * Unsets "randomizerInf" flag.
 */
void Flags_UnsetRandomizerInf(RandomizerInf flag) {
    gSaveContext.randomizerInf[flag >> 4] &= ~(1 << (flag & 0xF));
}

u32 func_80035BFC(PlayState* play, s16 arg1) {
    u16 retTextId = 0;

    switch (arg1) {
        case 0:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                if (Flags_GetInfTable(INFTABLE_05)) {
                    retTextId = 0x1048;
                } else {
                    retTextId = 0x1047;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_FIRST_SPOKE_TO_MIDO)) {
                    if (Flags_GetInfTable(INFTABLE_03)) {
                        retTextId = 0x1032;
                    } else {
                        retTextId = 0x1031;
                    }
                } else {
                    if (Flags_GetInfTable(INFTABLE_GREETED_BY_SARIA)) {
                        if (Flags_GetInfTable(INFTABLE_01)) {
                            retTextId = 0x1003;
                        } else {
                            retTextId = 0x1002;
                        }
                    } else {
                        retTextId = 0x1001;
                    }
                }
            }
            break;
        case 1:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    if (Flags_GetInfTable(INFTABLE_10)) {
                        retTextId = 0x1046;
                    } else {
                        retTextId = 0x1045;
                    }
                } else {
                    if (Flags_GetEventChkInf(EVENTCHKINF_COMPLAINED_ABOUT_MIDO)) {
                        if (Flags_GetInfTable(INFTABLE_0E)) {
                            retTextId = 0x1034;
                        } else {
                            retTextId = 0x1033;
                        }
                    } else {
                        if (Flags_GetInfTable(INFTABLE_0C)) {
                            retTextId = 0x1030;
                        } else {
                            retTextId = 0x102F;
                        }
                    }
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    if (Flags_GetInfTable(INFTABLE_19)) {
                        retTextId = 0x1071;
                    } else {
                        retTextId = 0x1070;
                    }
                } else {
                    if (Flags_GetEventChkInf(EVENTCHKINF_0B)) {
                        if (Flags_GetInfTable(INFTABLE_17)) {
                            retTextId = 0x1068;
                        } else {
                            retTextId = 0x1067;
                        }
                    } else {
                        if (Flags_GetInfTable(INFTABLE_15)) {
                            retTextId = 0x1061;
                        } else {
                            retTextId = 0x1060;
                        }
                    }
                }
            }
            break;
        case 2:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x1042;
                } else {
                    retTextId = 0x1004;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1072;
                } else if (Flags_GetInfTable(INFTABLE_41)) {
                    retTextId = 0x1055;
                } else {
                    retTextId = 0x1056;
                }
            }
            break;
        case 3:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x1043;
                } else {
                    if (Flags_GetInfTable(INFTABLE_1E)) {
                        retTextId = 0x1006;
                    } else {
                        retTextId = 0x1005;
                    }
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1073;
                } else {
                    retTextId = 0x105A;
                }
            }
            break;
        case 4:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x1042;
                } else {
                    retTextId = 0x1007;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1072;
                } else if (Flags_GetInfTable(INFTABLE_47)) {
                    retTextId = 0x105E;
                } else {
                    retTextId = 0x105D;
                }
            }
            break;
        case 5:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x1044;
                } else if (Flags_GetInfTable(INFTABLE_22)) {
                    retTextId = 0x1009;
                } else {
                    retTextId = 0x1008;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1075;
                } else {
                    retTextId = 0x105B;
                }
            }
            break;
        case 6:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x1042;
                } else if (Flags_GetInfTable(INFTABLE_24)) {
                    retTextId = 0x100B;
                } else {
                    retTextId = 0x100A;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1056;
                } else {
                    retTextId = 0x105F;
                }
            }
            break;
        case 7:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x1043;
                } else if (Flags_GetInfTable(INFTABLE_26)) {
                    retTextId = 0x100D;
                } else {
                    retTextId = 0x100C;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1057;
                } else {
                    retTextId = 0x1057;
                }
            }
            break;
        case 8:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x1043;
                } else if (Flags_GetInfTable(INFTABLE_28)) {
                    retTextId = 0x1019;
                } else {
                    retTextId = 0x100E;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1077;
                } else if (Flags_GetInfTable(INFTABLE_51)) {
                    retTextId = 0x1058;
                } else {
                    retTextId = 0x1059;
                }
            }
            break;
        case 9:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x1049;
                } else {
                    retTextId = 0x1035;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1079;
                } else {
                    retTextId = 0x104E;
                }
            }
            break;
        case 10:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x104A;
                } else {
                    retTextId = 0x1038;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x1079;
                } else if (Flags_GetInfTable(INFTABLE_59)) {
                    retTextId = 0x1050;
                } else {
                    retTextId = 0x104F;
                }
            }
            break;
        case 11:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x104B;
                } else {
                    retTextId = 0x103C;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x107B;
                } else {
                    retTextId = 0x1051;
                }
            }
            break;
        case 12:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x104C;
                } else {
                    retTextId = 0x103D;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x107C;
                } else {
                    retTextId = 0x1052;
                }
            }
            break;
        case 13:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
                    retTextId = 0x104D;
                } else {
                    retTextId = 0x103E;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                    retTextId = 0x106E;
                } else if (Flags_GetInfTable(INFTABLE_61)) {
                    retTextId = 0x1053;
                } else {
                    retTextId = 0x1054;
                }
            }
            break;
        case 15:
            if (Flags_GetEventChkInf(EVENTCHKINF_5C)) {
                retTextId = 0x1078;
            } else if (Flags_GetInfTable(INFTABLE_66)) {
                retTextId = 0x1066;
            } else {
                retTextId = 0x1062;
            }
            break;
        case 16:
            if (play->sceneNum == SCENE_SPOT15) {
                retTextId = 0x7002;
            } else if (Flags_GetInfTable(INFTABLE_6A)) {
                retTextId = 0x7004;
            } else if ((gSaveContext.dayTime >= 0x4000) && (gSaveContext.dayTime < 0xC556)) {
                retTextId = 0x7002;
            } else {
                retTextId = 0x7003;
            }
            break;
        case 17:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                if (Flags_GetInfTable(INFTABLE_6C)) {
                    retTextId = 0x7008;
                } else {
                    retTextId = 0x7007;
                }
            } else {
                retTextId = 0;
            }
            break;
        case 19:
            retTextId = 0x702D;
            break;
        case 18:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x7006;
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_POCKET_EGG)) {
                    if (Flags_GetInfTable(INFTABLE_71)) {
                        retTextId = 0x7072;
                    } else {
                        retTextId = 0x7071;
                    }
                } else {
                    retTextId = 0x7029;
                }
            }
            break;
        case 20:
        case 21:
            if (Flags_GetEventChkInf(EVENTCHKINF_42)) {
                retTextId = 0x2012;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_41)) {
                if (Flags_GetInfTable(INFTABLE_SHOWED_ZELDAS_LETTER_TO_GATE_GUARD)) {
                    retTextId = 0x2011;
                } else {
                    retTextId = 0x2010;
                }
            } else if (Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_ZELDAS_LETTER)) {
                retTextId = 0x200F;
            } else {
                retTextId = 0x200E;
            }
            break;
        case 24:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x7044;
            } else {
                retTextId = 0x7015;
            }
            break;
        case 25:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x7045;
            } else {
                Flags_GetInfTable(INFTABLE_C2);
                retTextId = 0x7016;
            }
            break;
        case 26:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x7046;
            } else {
                Flags_GetInfTable(INFTABLE_C2);
                retTextId = 0x7018;
            }
            break;
        case 27:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x7047;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                retTextId = 0x701A;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_INGO_AT_RANCH_BEFORE_TALON_RETURNS)) {
                if (Flags_GetInfTable(INFTABLE_C6)) {
                    retTextId = 0x701C;
                } else {
                    retTextId = 0x701B;
                }
            } else {
                retTextId = 0x701A;
            }
            break;
        case 28:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x7048;
            } else {
                Flags_GetInfTable(INFTABLE_CA);
                retTextId = 0x701D;
            }
            break;
        case 29:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x7049;
            } else {
                Flags_GetInfTable(INFTABLE_CC);
                retTextId = 0x701F;
            }
            break;
        case 30:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x704A;
            } else {
                Flags_GetInfTable(INFTABLE_CE);
                retTextId = 0x7021;
            }
            break;
        case 31:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x704B;
            } else {
                Flags_GetInfTable(INFTABLE_D0);
                retTextId = 0x7023;
            }
            break;
        case 32:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x704C;
            } else {
                Flags_GetInfTable(INFTABLE_D2);
                retTextId = 0x7025;
            }
            break;
        case 33:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP) && Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x704D;
            } else {
                Flags_GetInfTable(INFTABLE_D4);
                retTextId = 0x7027;
            }
            break;
        case 34:
            Flags_GetInfTable(INFTABLE_D6);
            retTextId = 0x403C;
            break;
        case 35:
            if (Flags_GetInfTable(INFTABLE_D8)) {
                retTextId = 0x5029;
            } else {
                retTextId = 0x5028;
            }
            break;
        case 37:
            retTextId = 0x5002;
            break;
        case 38:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) {
                    retTextId = 0x3027;
                } else if (Flags_GetEventChkInf(EVENTCHKINF_BOMBED_DODONGOS_CAVERN_ENTRANCE)) {
                    retTextId = 0x3021;
                } else if (Flags_GetInfTable(INFTABLE_E0)) {
                    retTextId = 0x302A;
                } else {
                    retTextId = 0x3008;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_20)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 39:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) {
                    retTextId = 0x3027;
                } else if (Flags_GetEventChkInf(EVENTCHKINF_BOMBED_DODONGOS_CAVERN_ENTRANCE)) {
                    retTextId = 0x3026;
                } else {
                    retTextId = 0x3009;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_2A)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 40:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) {
                    retTextId = 0x3027;
                } else if (Flags_GetEventChkInf(EVENTCHKINF_BOMBED_DODONGOS_CAVERN_ENTRANCE)) {
                    retTextId = 0x3026;
                } else if (Flags_GetInfTable(INFTABLE_EB)) {
                    retTextId = 0x302B;
                } else {
                    retTextId = 0x300A;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_2B)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 41:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) {
                    retTextId = 0x3027;
                } else if (Flags_GetInfTable(INFTABLE_F0)) {
                    retTextId = 0x3015;
                } else {
                    retTextId = 0x3014;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_2C)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 42:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) {
                    retTextId = 0x3027;
                } else if (Flags_GetInfTable(INFTABLE_F4)) {
                    retTextId = 0x3017;
                } else {
                    retTextId = 0x3016;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_2C)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 43:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) {
                    retTextId = 0x3027;
                } else if (Flags_GetInfTable(INFTABLE_F8)) {
                    retTextId = 0x3019;
                } else {
                    retTextId = 0x3018;
                }
            } else {
                if (Flags_GetEventChkInf(EVENTCHKINF_2D)) {
                    retTextId = 0x4043;
                } else {
                    retTextId = 0x302A;
                }
            }
            break;
        case 48:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) {
                retTextId = 0x3029;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_20) && Flags_GetEventChkInf(EVENTCHKINF_21)) {
                retTextId = 0x301B;
            } else {
                retTextId = 0x301A;
            }
            break;
        case 49:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x402D;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_A_ZORA)) {
                retTextId = 0x4007;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 50:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x402E;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_A_ZORA)) {
                if (Flags_GetInfTable(INFTABLE_124)) {
                    retTextId = 0x4009;
                } else {
                    retTextId = 0x4008;
                }
            } else {
                retTextId = 0x4006;
            }
            break;
        case 51:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x402D;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_RUTOS_LETTER)) {
                if (Flags_GetInfTable(INFTABLE_12A)) {
                    retTextId = 0x400B;
                } else {
                    retTextId = 0x402F;
                }
            } else if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_A_ZORA)) {
                retTextId = 0x400A;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 52:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x402E;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_A_ZORA)) {
                retTextId = 0x400C;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 53:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x402D;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)) {
                retTextId = 0x4010;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_A_ZORA)) {
                retTextId = 0x400F;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 54:
            if (Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                retTextId = 0x402E;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_A_ZORA)) {
                retTextId = 0x4011;
            } else {
                retTextId = 0x4006;
            }
            break;
        case 55:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                    retTextId = 0x402B;
                } else if (Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_RUTOS_LETTER)) {
                    if (Flags_GetInfTable(INFTABLE_138)) {
                        retTextId = 0x401C;
                    } else {
                        retTextId = 0x401B;
                    }
                } else {
                    retTextId = 0x401A;
                }
            } else {
                retTextId = 0;
            }
            break;
        case 58:
            retTextId = 0x500F;
            break;
        case 59:
            retTextId = 0x5010;
            break;
        case 60:
            retTextId = 0x5012;
            break;
        case 61:
            if (Flags_GetInfTable(INFTABLE_166)) {
                retTextId = 0x5001;
            } else {
                retTextId = 0x5000;
            }
            break;
        case 62:
            retTextId = 0x5012;
            break;
        case 63:
            if (Flags_GetInfTable(INFTABLE_16A)) {
                retTextId = 0x5001;
            } else {
                retTextId = 0x5000;
            }
            break;
        case 71:
            if (Flags_GetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON)) {
                retTextId = 0x2049;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_RANCH)) {
                retTextId = 0x2048;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                retTextId = 0x2047;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_POCKET_EGG) && !Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                retTextId = 0x2044;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_CASTLE_OR_MARKET)) {
                if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_INGO_AT_RANCH_BEFORE_TALON_RETURNS)) {
                    retTextId = 0x2043;
                } else {
                    retTextId = 0x2042;
                }
            } else {
                retTextId = 0x2041;
            }
            break;
        case 72:
            if (!LINK_IS_ADULT) {
                if (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                    retTextId = 0x2040;
                } else if (Flags_GetInfTable(INFTABLE_94)) {
                    retTextId = 0x2040;
                } else {
                    retTextId = 0x203F;
                }
            } else {
                if (!Flags_GetEventChkInf(EVENTCHKINF_EPONA_OBTAINED)) {
                    if (!IS_DAY) {
                        retTextId = 0x204E;
                    } else if (Flags_GetInfTable(INFTABLE_SPOKE_TO_INGO_ONCE_AS_ADULT)) {
                        retTextId = 0x2031;
                    } else {
                        retTextId = 0x2030;
                    }
                } else {
                    retTextId = 0;
                }
            }
            break;
    }

    if (retTextId == 0) {
        retTextId = 1;
    }

    return retTextId;
}

void func_80036E50(u16 textId, s16 arg1) {
    switch (arg1) {
        case 0:
            switch (textId) {
                case 0x1001:
                    Flags_SetInfTable(INFTABLE_GREETED_BY_SARIA);
                    return;
                case 0x1002:
                    Flags_SetInfTable(INFTABLE_01);
                    return;
                case 0x1031:
                    Flags_SetEventChkInf(EVENTCHKINF_COMPLAINED_ABOUT_MIDO);
                    Flags_SetInfTable(INFTABLE_03);
                    return;
                case 0x1047:
                    Flags_SetInfTable(INFTABLE_05);
                    return;
            }
            return;
        case 1:
            switch (textId) {
                case 0x102F:
                    Flags_SetEventChkInf(EVENTCHKINF_FIRST_SPOKE_TO_MIDO);
                    Flags_SetInfTable(INFTABLE_0C);
                    return;
                case 0x1033:
                    Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    Flags_SetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD);
                    Flags_SetInfTable(INFTABLE_0E);
                    return;
                case 0x1045:
                    Flags_SetInfTable(INFTABLE_10);
                    return;
                case 0x1060:
                    Flags_SetInfTable(INFTABLE_15);
                    return;
                case 0x1067:
                    Flags_SetEventChkInf(EVENTCHKINF_PLAYED_SARIAS_SONG_FOR_MIDO_AS_ADULT);
                    Flags_SetInfTable(INFTABLE_17);
                    return;
                case 0x1070:
                    Flags_SetInfTable(INFTABLE_19);
                    return;
            }
            return;
        case 2:
            if (textId == 0x1056) {
                Flags_SetInfTable(INFTABLE_41);
            }
            return;
        case 3:
            if (textId == 0x1005) {
                Flags_SetInfTable(INFTABLE_1E);
            }
            return;
        case 4:
            if (textId == 0x105D) {
                Flags_SetInfTable(INFTABLE_47);
            }
            return;
        case 5:
            if (textId == 0x1008) {
                Flags_SetInfTable(INFTABLE_22);
            }
            return;
        case 6:
            if (textId == 0x100A) {
                Flags_SetInfTable(INFTABLE_24);
            }
            return;
        case 7:
            if (textId == 0x100C) {
                Flags_SetInfTable(INFTABLE_26);
            }
            return;
        case 8:
            if (textId == 0x100E) {
                Flags_SetInfTable(INFTABLE_28);
            }
            if (textId == 0x1059) {
                Flags_SetInfTable(INFTABLE_51);
            }
            return;
        case 10:
            if (textId == 0x104F) {
                Flags_SetInfTable(INFTABLE_59);
            }
            return;
        case 13:
            if (textId == 0x1054) {
                Flags_SetInfTable(INFTABLE_61);
            }
            return;
        case 15:
            if (textId == 0x1062) {
                Flags_SetInfTable(INFTABLE_66);
            }
            return;
        case 16:
            if (textId == 0x7002) {
                Flags_SetInfTable(INFTABLE_6A);
            }
            if (textId == 0x7003) {
                Flags_SetInfTable(INFTABLE_6A);
            }
            return;
        case 17:
            if (textId == 0x7007) {
                Flags_SetInfTable(INFTABLE_6C);
            }
            return;
        case 18:
            if (textId == 0x7071) {
                Flags_SetInfTable(INFTABLE_71);
            }
            return;
        case 20:
        case 21:
            if (textId == 0x2010) {
                Flags_SetInfTable(INFTABLE_SHOWED_ZELDAS_LETTER_TO_GATE_GUARD);
            }
            return;
        case 25:
            if (textId == 0x7016) {
                Flags_SetInfTable(INFTABLE_C2);
            }
            return;
        case 26:
            if (textId == 0x7018) {
                Flags_SetInfTable(INFTABLE_C4);
            }
            return;
        case 28:
            if (textId == 0x701D) {
                Flags_SetInfTable(INFTABLE_CA);
            }
            return;
        case 29:
            if (textId == 0x701F) {
                Flags_SetInfTable(INFTABLE_CC);
            }
            return;
        case 30:
            if (textId == 0x7021) {
                Flags_SetInfTable(INFTABLE_CE);
            }
            return;
        case 31:
            if (textId == 0x7023) {
                Flags_SetInfTable(INFTABLE_D0);
            }
            return;
        case 32:
            if (textId == 0x7025) {
                Flags_SetInfTable(INFTABLE_D2);
            }
            return;
        case 33:
            if (textId == 0x7027) {
                Flags_SetInfTable(INFTABLE_D4);
            }
            return;
        case 34:
            if (textId == 0x403C) {
                Flags_SetInfTable(INFTABLE_D6);
            }
            return;
        case 35:
            if (textId == 0x5028) {
                Flags_SetInfTable(INFTABLE_D8);
            }
            return;
        case 38:
            if (textId == 0x3008) {
                Flags_SetInfTable(INFTABLE_E0);
            }
            return;
        case 40:
            if (textId == 0x300B) {
                Flags_SetInfTable(INFTABLE_EB);
            }
            return;
        case 41:
            if (textId == 0x3014) {
                Flags_SetInfTable(INFTABLE_F0);
            }
            return;
        case 42:
            if (textId == 0x3016) {
                Flags_SetInfTable(INFTABLE_F4);
            }
            return;
        case 43:
            if (textId == 0x3018) {
                Flags_SetEventChkInf(EVENTCHKINF_20);
                Flags_SetInfTable(INFTABLE_F8);
            }
            return;
        case 48:
            if (textId == 0x3020) {
                Flags_SetEventChkInf(EVENTCHKINF_22);
                Flags_SetInfTable(INFTABLE_113);
            }
            return;
        case 49:
        case 52:
        case 53:
        case 54:
            if (textId == 0x4006) {
                Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_A_ZORA);
            }
            return;
        case 50:
            if (textId == 0x4006) {
                Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_A_ZORA);
            }
            if (textId == 0x4008) {
                Flags_SetInfTable(INFTABLE_124);
            }
            return;
        case 51:
            if (textId == 0x4006) {
                Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_A_ZORA);
            }
            if (textId == 0x400A) {
                Flags_SetEventChkInf(EVENTCHKINF_32);
            }
            if (textId == 0x402F) {
                Flags_SetInfTable(INFTABLE_12A);
            }
            return;
        case 55:
            if (textId == 0x401B) {
                Flags_SetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED);
                Flags_SetInfTable(INFTABLE_138);
            }
            return;
        case 61:
            if (textId == 0x5000) {
                Flags_SetInfTable(INFTABLE_166);
            }
            return;
        case 63:
            if (textId == 0x5013) {
                Flags_SetInfTable(INFTABLE_16A);
            }
            return;
        case 71:
            if (textId == 0x2041) {
                Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_CASTLE_OR_MARKET);
            }
            if (textId == 0x2044) {
                Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_POCKET_EGG);
            }
            if (textId == 0x2047) {
                Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_RANCH);
            }
            if (textId == 0x2048) {
                Flags_SetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON);
            }
            return;
        case 72:
            return;
    }
}

s32 func_800374E0(PlayState* play, Actor* actor, u16 textId) {
    MessageContext* msgCtx = &play->msgCtx;
    s32 ret = 1;

    switch (textId) {
        case 0x1035:
            if (msgCtx->choiceIndex == 0) {
                if (Flags_GetInfTable(INFTABLE_2A)) {
                    func_80035B18(play, actor, 0x1036);
                } else {
                    func_80035B18(play, actor, 0x1041);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                if (Flags_GetInfTable(INFTABLE_2B)) {
                    func_80035B18(play, actor, 0x1037);
                } else {
                    func_80035B18(play, actor, 0x1041);
                }
            }
            ret = 0;
            break;
        case 0x1038:
            if (msgCtx->choiceIndex == 0) {
                if (Flags_GetInfTable(INFTABLE_2E)) {
                    func_80035B18(play, actor, 0x1039);
                } else {
                    func_80035B18(play, actor, 0x1041);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                if (Flags_GetInfTable(INFTABLE_2F)) {
                    func_80035B18(play, actor, 0x103A);
                } else {
                    func_80035B18(play, actor, 0x1041);
                }
            }
            if (msgCtx->choiceIndex == 2) {
                if (Flags_GetInfTable(INFTABLE_30)) {
                    func_80035B18(play, actor, 0x103B);
                } else {
                    func_80035B18(play, actor, 0x1041);
                }
            }
            ret = 0;
            break;
        case 0x103E:
            if (msgCtx->choiceIndex == 0) {
                func_80035B18(play, actor, 0x103F);
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x1040);
            }
            ret = 0;
            break;
        case 0x1041:
            if (msgCtx->choiceTextId == 0x1035) {
                if (msgCtx->choiceIndex == 0) {
                    func_80035B18(play, actor, 0x1036);
                    Flags_SetInfTable(INFTABLE_2A);
                }
                if (msgCtx->choiceIndex == 1) {
                    func_80035B18(play, actor, 0x1037);
                    Flags_SetInfTable(INFTABLE_2B);
                }
            }
            if (msgCtx->choiceTextId == 0x1038) {
                if (msgCtx->choiceIndex == 0) {
                    func_80035B18(play, actor, 0x1039);
                    Flags_SetInfTable(INFTABLE_2E);
                }
                if (msgCtx->choiceIndex == 1) {
                    func_80035B18(play, actor, 0x103A);
                    Flags_SetInfTable(INFTABLE_2F);
                }
                if (msgCtx->choiceIndex == 2) {
                    func_80035B18(play, actor, 0x103B);
                    Flags_SetInfTable(INFTABLE_30);
                }
            }
            ret = 0;
            break;
        case 0x1062:
            if (msgCtx->choiceIndex == 0) {
                func_80035B18(play, actor, 0x1063);
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x1064);
            }
            ret = 0;
            break;
        case 0x2030:
        case 0x2031:
            if (msgCtx->choiceIndex == 0) {
                if (gSaveContext.rupees >= 10) {
                    func_80035B18(play, actor, 0x2034);
                    Rupees_ChangeBy(-10);
                } else {
                    func_80035B18(play, actor, 0x2032);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x2032);
            }
            Flags_SetInfTable(INFTABLE_SPOKE_TO_INGO_ONCE_AS_ADULT);
            ret = 0;
            break;
        case 0x2036:
        case 0x2037:
            if (msgCtx->choiceIndex == 0) {
                func_80035B18(play, actor, 0x201F);
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x205A);
            }
            ret = 0;
            break;
        case 0x2038:
            if (msgCtx->choiceIndex == 0) {
                break;
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x205A);
            }
            ret = 0;
            break;
        case 0x2034:
            if (msgCtx->choiceIndex != 0) {
                break;
            }
            func_80035B18(play, actor, 0x2035);
            ret = 0;
            break;
        case 0x2043:
            if (Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_POCKET_EGG)) {
                break;
            }
            func_80035B18(play, actor, 0x2044);
            ret = 0;
            break;
        case 0x205A:
            break;
        case 0x300A:
            if (msgCtx->choiceIndex == 0) {
                if (Flags_GetEventChkInf(EVENTCHKINF_22)) {
                    func_80035B18(play, actor, 0x300B);
                } else {
                    func_80035B18(play, actor, 0x300C);
                }
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x300D);
            }
            ret = 0;
            break;
        case 0x301B:
            if (msgCtx->choiceIndex == 0) {
                func_80035B18(play, actor, 0x301D);
            }
            if (msgCtx->choiceIndex == 1) {
                if (Flags_GetInfTable(INFTABLE_113)) {
                    func_80035B18(play, actor, 0x301F);
                } else {
                    func_80035B18(play, actor, 0x301E);
                }
            }
            ret = 0;
            break;
        case 0x301E:
            func_80035B18(play, actor, 0x3020);
            ret = 0;
            break;
        case 0x400C:
            if (msgCtx->choiceIndex == 0) {
                func_80035B18(play, actor, 0x400D);
            }
            if (msgCtx->choiceIndex == 1) {
                func_80035B18(play, actor, 0x400E);
            }
            ret = 0;
            break;
        case 0x7007:
            func_80035B18(play, actor, 0x703E);
            ret = 0;
            break;
        case 0x703E:
            func_80035B18(play, actor, 0x703F);
            ret = 0;
            break;
        case 0x703F:
            func_80035B18(play, actor, 0x7042);
            ret = 0;
            break;
    }

    return ret;
}

u16 func_80037C30(PlayState* play, s16 arg1) {
    return func_80035BFC(play, arg1);
}

s32 func_80037C5C(PlayState* play, s16 arg1, u16 textId) {
    func_80036E50(textId, arg1);
    return false;
}

s32 func_80037C94(PlayState* play, Actor* actor, s32 arg2) {
    return func_800374E0(play, actor, actor->textId);
}

s32 func_80037CB8(PlayState* play, Actor* actor, s16 arg2) {
    MessageContext* msgCtx = &play->msgCtx;
    s32 ret = false;

    switch (Message_GetState(msgCtx)) {
        case TEXT_STATE_CLOSING:
            func_80037C5C(play, arg2, actor->textId);
            ret = true;
            break;
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play) && func_80037C94(play, actor, arg2)) {
                Audio_PlaySoundGeneral(NA_SE_SY_CANCEL, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                msgCtx->msgMode = MSGMODE_TEXT_CLOSING;
                ret = true;
            }
            break;
    }

    return ret;
}

s32 func_80037D98(PlayState* play, Actor* actor, s16 arg2, s32* arg3) {
    s16 var;
    s16 sp2C;
    s16 sp2A;
    s16 abs_var;

    if (Actor_ProcessTalkRequest(actor, play)) {
        *arg3 = 1;
        return true;
    }

    if (*arg3 == 1) {
        if (func_80037CB8(play, actor, arg2)) {
            *arg3 = 0;
        }
        return false;
    }

    Actor_GetScreenPos(play, actor, &sp2C, &sp2A);

    if ((sp2C < 0) || (sp2C > SCREEN_WIDTH) || (sp2A < 0) || (sp2A > SCREEN_HEIGHT)) {
        return false;
    }

    var = actor->yawTowardsPlayer - actor->shape.rot.y;
    abs_var = ABS(var);

    if (abs_var >= 0x4300) {
        return false;
    }

    if ((actor->xyzDistToPlayerSq > SQ(160.0f)) && !actor->isTargeted) {
        return false;
    }

    if (actor->xyzDistToPlayerSq <= SQ(80.0f)) {
        if (func_8002F2CC(actor, play, 80.0f)) {
            actor->textId = func_80037C30(play, arg2);
        }
    } else {
        if (func_8002F2F4(actor, play)) {
            actor->textId = func_80037C30(play, arg2);
        }
    }

    return false;
}

s32 func_80037F30(Vec3s* arg0, Vec3s* arg1) {
    Math_SmoothStepToS(&arg0->y, 0, 6, 6200, 100);
    Math_SmoothStepToS(&arg0->x, 0, 6, 6200, 100);
    Math_SmoothStepToS(&arg1->y, 0, 6, 6200, 100);
    Math_SmoothStepToS(&arg1->x, 0, 6, 6200, 100);
    return true;
}

s32 func_80037FC8(Actor* actor, Vec3f* arg1, Vec3s* arg2, Vec3s* arg3) {
    s16 sp36;
    s16 sp34;
    s16 var;

    sp36 = Math_Vec3f_Pitch(&actor->focus.pos, arg1);
    sp34 = Math_Vec3f_Yaw(&actor->focus.pos, arg1) - actor->world.rot.y;

    Math_SmoothStepToS(&arg2->x, sp36, 6, 2000, 1);
    arg2->x = (arg2->x < -6000) ? -6000 : ((arg2->x > 6000) ? 6000 : arg2->x);

    var = Math_SmoothStepToS(&arg2->y, sp34, 6, 2000, 1);
    arg2->y = (arg2->y < -8000) ? -8000 : ((arg2->y > 8000) ? 8000 : arg2->y);

    if (var && (ABS(arg2->y) < 8000)) {
        return false;
    }

    Math_SmoothStepToS(&arg3->y, sp34 - arg2->y, 4, 2000, 1);
    arg3->y = (arg3->y < -12000) ? -12000 : ((arg3->y > 12000) ? 12000 : arg3->y);

    return true;
}

s32 func_80038154(PlayState* play, Actor* actor, Vec3s* arg2, Vec3s* arg3, f32 arg4) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    Vec3f sp2C;
    s16 var;
    s16 abs_var;

    actor->focus.pos = actor->world.pos;
    actor->focus.pos.y += arg4;

    if (!(((play->csCtx.state != CS_STATE_IDLE) || (gDbgCamEnabled)) && (gSaveContext.entranceIndex == 0x00EE))) {
        var = actor->yawTowardsPlayer - actor->shape.rot.y;
        abs_var = ABS(var);
        if (abs_var >= 0x4300) {
            func_80037F30(arg2, arg3);
            return false;
        }
    }

    if (((play->csCtx.state != CS_STATE_IDLE) || (gDbgCamEnabled)) && (gSaveContext.entranceIndex == 0x00EE)) {
        sp2C = play->view.eye;
    } else {
        sp2C = player->actor.focus.pos;
    }

    func_80037FC8(actor, &sp2C, arg2, arg3);

    return true;
}

s32 func_80038290(PlayState* play, Actor* actor, Vec3s* arg2, Vec3s* arg3, Vec3f arg4) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    Vec3f sp24;
    s16 var;
    s16 abs_var;

    actor->focus.pos = arg4;

    if (!(((play->csCtx.state != CS_STATE_IDLE) || (gDbgCamEnabled)) && (gSaveContext.entranceIndex == 0x00EE))) {
        var = actor->yawTowardsPlayer - actor->shape.rot.y;
        abs_var = ABS(var);
        if (abs_var >= 0x4300) {
            func_80037F30(arg2, arg3);
            return false;
        }
    }

    if (((play->csCtx.state != CS_STATE_IDLE) || (gDbgCamEnabled)) && (gSaveContext.entranceIndex == 0x00EE)) {
        sp24 = play->view.eye;
    } else {
        sp24 = player->actor.focus.pos;
    }

    func_80037FC8(actor, &sp24, arg2, arg3);

    return true;
}

GetItemEntry GetChestGameRandoGetItem(s8 room, s16 ogDrawId, PlayState* play) {
    if (Randomizer_GetSettingValue(RSK_SHUFFLE_CHEST_MINIGAME)) {
        // RANDOTODO update this logic when we implement keysanity
        // because 3drando replaces the keys not the rupees
        if (ogDrawId == GID_RUPEE_GREEN ||
            ogDrawId == GID_RUPEE_BLUE ||
            ogDrawId == GID_RUPEE_RED)
        {
            switch(room) {
                case 1:
                    if(!Flags_GetCollectible(play, 0x1B)) {
                        return Randomizer_GetItemFromKnownCheck(RC_MARKET_TREASURE_CHEST_GAME_ITEM_1, GI_RUPEE_GREEN);
                    }
                    break;
                case 2:
                    if(!Flags_GetCollectible(play, 0x1C)) {
                        return Randomizer_GetItemFromKnownCheck(RC_MARKET_TREASURE_CHEST_GAME_ITEM_2, GI_RUPEE_GREEN);
                    }
                    break;
                case 3:
                    if(!Flags_GetCollectible(play, 0x1D)) {
                        return Randomizer_GetItemFromKnownCheck(RC_MARKET_TREASURE_CHEST_GAME_ITEM_3, GI_RUPEE_BLUE);
                    }
                    break;
                case 4:
                    if(!Flags_GetCollectible(play, 0x1E)) {
                        return Randomizer_GetItemFromKnownCheck(RC_MARKET_TREASURE_CHEST_GAME_ITEM_4, GI_RUPEE_BLUE);
                    }
                    break;
                case 5:
                    if(!Flags_GetCollectible(play, 0x1F)) {
                        return Randomizer_GetItemFromKnownCheck(RC_MARKET_TREASURE_CHEST_GAME_ITEM_5, GI_RUPEE_RED);
                    }
                    break;
            }
        }
    }

    if(ogDrawId == GID_HEART_PIECE) {
        return Randomizer_GetItemFromKnownCheck(RC_MARKET_TREASURE_CHEST_GAME_REWARD, GI_HEART_PIECE);
    }

    return (GetItemEntry)GET_ITEM_NONE;
}

s16 GetChestGameRandoGiDrawId(s8 room, s16 ogDrawId, PlayState* play) {
    GetItemEntry randoGetItem = GetChestGameRandoGetItem(room, ogDrawId, play);

    if (randoGetItem.itemId != ITEM_NONE) {
        return randoGetItem.gid;
    }

    return ogDrawId;
}
