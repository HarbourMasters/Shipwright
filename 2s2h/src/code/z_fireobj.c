#include "z_fireobj.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "objects/gameplay_keep/gameplay_keep.h"

ColliderCylinderInit sFireObjCollisionInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK4,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000820, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 18, 67, 0, { 0, 0, 0 } },
};

FireObjColors sFireObjColors[] = {
    { { 255, 255, 0, 255 }, 128, { 255, 0, 0 } },
};

FireObjLightParams sFireObjLightParams[] = {
    { 200, { 128, 128, 0 }, { 127, 127, 0 } },
};

void FireObj_InitWithParams(PlayState* play, FireObj* fire, FireObjInitParams* init) {
    fire->size = init->size;
    fire->sizeInv = 1.0f / init->size;
    fire->dynamicSizeStep = init->dynamicSizeStep;
    fire->state = init->state;
    fire->sizeGrowsCos2 = init->sizeGrowsCos2;
    fire->colorsIndex = init->colorsIndex;
    fire->flags = init->flags;
    fire->xScale = 0.0f;
    fire->yScale = 0.0f;
    fire->dynamicSize = 0.0f;
    fire->timer = Rand_ZeroOne() * 20.0f;
    fire->ignitionDelay = -1;
}

void FireObj_SetState(FireObj* fire, f32 dynamicSizeStep, u8 newState) {
    fire->state = newState;
    if (fire->state == FIRE_STATE_NOT_LIT) {
        fire->yScale = 0.0f;
        fire->xScale = 0.0f;
        fire->dynamicSize = 0.0f;
    } else if (fire->state == FIRE_STATE_FULLY_LIT) {
        fire->xScale = fire->yScale = fire->size;
        fire->dynamicSize = 1.0f;
    }
    fire->dynamicSizeStep = dynamicSizeStep;
}

void FireObj_SetPosition(FireObj* fire, Vec3f* pos) {
    Math_Vec3f_Copy(&fire->position, pos);
}

void FireObj_StepSize(FireObj* fire) {
    if (fire->state == FIRE_STATE_GROWING) {
        if (Math_StepToF(&fire->dynamicSize, 1.0f, fire->dynamicSizeStep)) {
            FireObj_SetState(fire, fire->dynamicSizeStep, FIRE_STATE_FULLY_LIT);
        }
    } else if ((fire->state == FIRE_STATE_SHRINKING) && Math_StepToF(&fire->dynamicSize, 0.0f, fire->dynamicSizeStep)) {
        FireObj_SetState(fire, fire->dynamicSizeStep, FIRE_STATE_NOT_LIT);
    }
    if (fire->sizeGrowsCos2 == 1) {
        if ((fire->state == FIRE_STATE_GROWING) || (fire->state == FIRE_STATE_SHRINKING)) {
            fire->xScale = (1.0f - Math_CosS(SQ(fire->dynamicSize) * 0x4000)) * fire->size;
            fire->yScale = fire->dynamicSize * fire->size;
        } else {
            fire->yScale = fire->dynamicSize * fire->size;
            fire->xScale = fire->dynamicSize * fire->size;
        }
        return;
    } else {
        fire->yScale = fire->dynamicSize * fire->size;
    }
    fire->xScale = fire->dynamicSize * fire->size;
}

void FireObj_UpdateStateTransitions(PlayState* play, FireObj* fire) {
    Player* player = GET_PLAYER(play);
    WaterBox* waterBox;
    f32 waterY;
    s32 sp40 = false;
    u8 nextState;
    Vec3f dist;

    FireObj_StepSize(fire);
    fire->timer++;
    if (fire->ignitionDelay > 0) {
        fire->ignitionDelay--;
    } else if (fire->ignitionDelay == 0) {
        fire->ignitionDelay = -1;
        if ((fire->state == FIRE_STATE_NOT_LIT) || (fire->state == FIRE_STATE_SHRINKING)) {
            nextState = FIRE_STATE_GROWING;
        } else {
            nextState = FIRE_STATE_SHRINKING;
        }
        FireObj_SetState(fire, fire->dynamicSizeStep, nextState);
    }

    if ((fire->flags & FIRE_FLAG_WATER_EXTINGUISHABLE) && (fire->state != FIRE_STATE_NOT_LIT) &&
        WaterBox_GetSurface1_2(play, &play->colCtx, fire->position.x, fire->position.z, &waterY, &waterBox) &&
        ((waterY - fire->position.y) > (6500.0f * fire->yScale))) {
        FireObj_SetState(fire, fire->dynamicSizeStep, FIRE_STATE_NOT_LIT);
    }
    if ((fire->flags & FIRE_FLAG_INTERACT_STICK) && (player->heldItemAction == PLAYER_IA_DEKU_STICK)) {
        Math_Vec3f_Diff(&player->meleeWeaponInfo[0].tip, &fire->position, &dist);
        if (Math3D_Vec3fMagnitudeSq(&dist) < SQ(20.0f)) {
            sp40 = true;
        }
    }

    if (sp40) {
        if (fire->state == FIRE_STATE_NOT_LIT) {
            if (player->unk_B28 > 0) {
                FireObj_SetState(fire, fire->dynamicSizeStep, FIRE_STATE_GROWING);
            }
        } else if (player->unk_B28 == 0) {
            player->unk_B28 = 210;
            SoundSource_PlaySfxAtFixedWorldPos(play, &fire->position, 20, NA_SE_EV_FLAME_IGNITION);
        } else if (player->unk_B28 < 200) {
            player->unk_B28 = 200;
        }
    }
}

void FireObj_Draw(PlayState* play, FireObj* fire) {
    s32 pad;
    FireObjColors* fireColors = &sFireObjColors[fire->colorsIndex];

    if (fire->state != FIRE_STATE_NOT_LIT) {
        Vec3s vec;

        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, (fire->timer * -20) % 512U, 32, 128, 0,
                                      0, 0, -20));

        gDPSetPrimColor(POLY_XLU_DISP++, 0, fireColors->lod, fireColors->primColor.r, fireColors->primColor.g,
                        fireColors->primColor.b, fireColors->primColor.a);

        gDPSetEnvColor(POLY_XLU_DISP++, fireColors->envColor.r, fireColors->envColor.g, fireColors->envColor.b, 0);

        vec.x = 0;
        vec.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000;
        vec.z = 0;
        Matrix_SetTranslateRotateYXZ(fire->position.x, fire->position.y, fire->position.z, &vec);
        Matrix_Scale(fire->xScale, fire->yScale, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void FireObj_InitLight(PlayState* play, FireObjLight* light, u8* paramsIndex, Vec3f* pos) {
    FireObjLightParams* objectParams = &sFireObjLightParams[*paramsIndex];

    Lights_PointGlowSetInfo(&light->lightInfo, pos->x, pos->y, pos->z, objectParams->color.r, objectParams->color.g,
                            objectParams->color.b, objectParams->radius);
    light->light = LightContext_InsertLight(play, &play->lightCtx, &light->lightInfo);
    light->lightParamsIndex = *paramsIndex;
}

void FireObj_DestroyLight(PlayState* play, FireObjLight* light) {
    LightContext_RemoveLight(play, &play->lightCtx, light->light);
}

void FireObj_UpdateLight(PlayState* play, FireObjLight* light, FireObj* fire) {
    FireObjLightParams* lightParams = &sFireObjLightParams[light->lightParamsIndex];
    s16 radius;

    if (fire->state == FIRE_STATE_NOT_LIT) {
        Lights_PointSetColorAndRadius(&light->lightInfo, 0, 0, 0, -1);
    } else {
        radius = (fire->yScale * 140.0f * fire->sizeInv) + 60.0f;

        Lights_PointGlowSetInfo(&light->lightInfo, fire->position.x, (fire->position.y + (fire->yScale * 6500.0f)),
                                fire->position.z,
                                ((s32)(Rand_ZeroOne() * lightParams->maxColorAdj.r) + lightParams->color.r),
                                ((s32)(Rand_ZeroOne() * lightParams->maxColorAdj.g) + lightParams->color.g),
                                ((s32)(Rand_ZeroOne() * lightParams->maxColorAdj.b) + lightParams->color.b), radius);
    }
}

void FireObj_Init(PlayState* play, FireObj* fire, FireObjInitParams* init, Actor* actor) {
    FireObj* fire2 = fire;

    FireObj_InitWithParams(play, fire, init);
    Collider_InitCylinder(play, &fire->collision);
    Collider_SetCylinder(play, &fire->collision, actor, &sFireObjCollisionInit);

    fire2->collision.dim.radius = (fire->size * 4000.0f) + 2.5f;
    fire2->collision.dim.height = fire->size * 16000.0f;
    fire->collision.dim.yShift = fire->size * -1728.0f;
    FireObj_InitLight(play, &fire->light, &init->lightParamsIndex, &fire->position);
}

void FireObj_Destroy(PlayState* play, FireObj* fire) {
    Collider_DestroyCylinder(play, &fire->collision);
    FireObj_DestroyLight(play, &fire->light);
}

void FireObj_SetState2(FireObj* fire, f32 dynamicSizeStep, u8 newState) {
    FireObj_SetState(fire, dynamicSizeStep, newState);
}

void FireObj_Update(PlayState* play, FireObj* fire, Actor* actor) {
    s32 pad; // Gamestate cast?
    EnArrow* arrow = (EnArrow*)fire->collision.base.ac;

    FireObj_UpdateStateTransitions(play, fire);
    if (fire->state == FIRE_STATE_NOT_LIT) {
        if ((fire->collision.base.acFlags & AC_HIT) &&
            (fire->collision.elem.acHitElem->atDmgInfo.dmgFlags & DMG_FIRE_ARROW)) {
            FireObj_SetState(fire, fire->dynamicSizeStep, FIRE_STATE_GROWING);
        }
    } else if ((fire->collision.base.acFlags & AC_HIT) && (arrow->actor.update != NULL) &&
               (arrow->actor.id == ACTOR_EN_ARROW)) {
        arrow->actor.params = 0;
        arrow->collider.elem.atDmgInfo.dmgFlags = DMG_FIRE_ARROW;
    }
    fire->collision.dim.pos.x = fire->position.x;
    fire->collision.dim.pos.y = fire->position.y;
    fire->collision.dim.pos.z = fire->position.z;
    CollisionCheck_SetAC(play, &play->colChkCtx, &fire->collision.base);
    FireObj_UpdateLight(play, &fire->light, fire);
}
