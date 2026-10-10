/*
 * File: z_obj_syokudai.c
 * Overlay: ovl_Obj_Syokudai
 * Description: Torch
 */

#include "z_obj_syokudai.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "objects/object_syokudai/object_syokudai.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void ObjSyokudai_Init(Actor* thisx, PlayState* play);
void ObjSyokudai_Destroy(Actor* thisx, PlayState* play);
void ObjSyokudai_Update(Actor* thisx, PlayState* play2);
void ObjSyokudai_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Syokudai_Profile = {
    /**/ ACTOR_OBJ_SYOKUDAI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SYOKUDAI,
    /**/ sizeof(ObjSyokudai),
    /**/ ObjSyokudai_Init,
    /**/ ObjSyokudai_Destroy,
    /**/ ObjSyokudai_Update,
    /**/ ObjSyokudai_Draw,
};

static ColliderCylinderInit sStandColliderInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0xF6CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 12, 45, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sFlameColliderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000820, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 15, 45, 45, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
};

static u8 sColMaterials[] = { COL_MATERIAL_METAL, COL_MATERIAL_WOOD, COL_MATERIAL_WOOD };

static Gfx* sDLists[] = {
    gObjectSyokudaiTypeSwitchCausesFlameDL,
    gObjectSyokudaiTypeFlameCausesSwitchDL,
    gObjectSyokudaiTypeNoSwitchDL,
};

static s32 sNumLitTorchesInGroup;

void ObjSyokudai_Init(Actor* thisx, PlayState* play) {
    ObjSyokudai* this = (ObjSyokudai*)thisx;
    s32 pad;
    s32 type = OBJ_SYOKUDAI_GET_TYPE(thisx);
    s32 switchFlag = OBJ_SYOKUDAI_GET_SWITCH_FLAG(thisx);

    Actor_ProcessInitChain(thisx, sInitChain);
    func_800B4AEC(play, thisx, 50.0f);
    ActorShape_Init(&thisx->shape, 0.0f, func_800B4B50, 1.0f);
    Collider_InitAndSetCylinder(play, &this->standCollider, thisx, &sStandColliderInit);
    this->standCollider.base.colMaterial = sColMaterials[OBJ_SYOKUDAI_GET_TYPE(thisx)];
    Collider_InitAndSetCylinder(play, &this->flameCollider, thisx, &sFlameColliderInit);
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    Lights_PointGlowSetInfo(&this->lightInfo, thisx->world.pos.x, thisx->world.pos.y + OBJ_SYOKUDAI_GLOW_HEIGHT,
                            thisx->world.pos.z, 0xFF, 0xFF, 0xB4, -1);
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);

    if (OBJ_SYOKUDAI_GET_START_LIT(thisx) ||
        (((type != OBJ_SYOKUDAI_TYPE_NO_SWITCH) || (switchFlag != OBJ_SYOKUDAI_SWITCH_FLAG_NONE)) &&
         Flags_GetSwitch(play, switchFlag))) {
        s32 groupSize = OBJ_SYOKUDAI_GET_GROUP_SIZE(thisx);

        this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
        if (groupSize != 0) {
            sNumLitTorchesInGroup = groupSize;
        }
    } else {
        sNumLitTorchesInGroup = 0;
    }
    this->flameTexScroll = (u32)(Rand_ZeroOne() * OBJ_SYOKUDAI_SNUFF_DEFAULT);
    Actor_SetFocus(thisx, 60.0f);
}

void ObjSyokudai_Destroy(Actor* thisx, PlayState* play) {
    ObjSyokudai* this = (ObjSyokudai*)thisx;

    Collider_DestroyCylinder(play, &this->standCollider);
    Collider_DestroyCylinder(play, &this->flameCollider);
    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
}

void ObjSyokudai_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjSyokudai* this = (ObjSyokudai*)thisx;
    s32 groupSize = OBJ_SYOKUDAI_GET_GROUP_SIZE(thisx);
    s32 switchFlag = OBJ_SYOKUDAI_GET_SWITCH_FLAG(thisx);
    s32 type = OBJ_SYOKUDAI_GET_TYPE(thisx);
    s32 pad0;
    WaterBox* waterBox;
    f32 waterSurface;
    s32 lightRadius = -1;
    u8 lightIntensity = 0;
    Player* player;
    s32 pad1;

    if (this->pendingAction != OBJ_SYOKUDAI_PENDING_ACTION_NONE) {
        if (CutsceneManager_GetCurrentCsId() != thisx->csId) {
            if (CutsceneManager_IsNext(thisx->csId)) {
                CutsceneManager_StartWithPlayerCs(thisx->csId, thisx);
                if (this->pendingAction >= OBJ_SYOKUDAI_PENDING_ACTION_CUTSCENE_AND_SWITCH) {
                    Flags_SetSwitch(play, switchFlag);
                }
            } else {
                CutsceneManager_Queue(thisx->csId);
            }
        } else if (func_800F22C4(thisx->csId, thisx) != 0) {
            this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
            this->pendingAction = OBJ_SYOKUDAI_PENDING_ACTION_NONE;
        }
    } else {
        if (WaterBox_GetSurface1_2(play, &play->colCtx, thisx->world.pos.x, thisx->world.pos.z, &waterSurface,
                                   &waterBox) &&
            ((waterSurface - thisx->world.pos.y) > OBJ_SYOKUDAI_FLAME_HEIGHT)) {

            this->snuffTimer = OBJ_SYOKUDAI_SNUFF_OUT;
            if (type == OBJ_SYOKUDAI_TYPE_FLAME_CAUSES_SWITCH) {
                Flags_UnsetSwitch(play, switchFlag);
                if (groupSize != 0) {
                    this->snuffTimer = OBJ_SYOKUDAI_SNUFF_GROUP_BY_WATER;
                }
            }
        } else {
            s32 interaction = OBJ_SYOKUDAI_INTERACTION_NONE;
            u32 flameColliderACDmgFlags = 0;

            player = GET_PLAYER(play);

            if (OBJ_SYOKUDAI_GET_START_LIT(thisx)) {
                this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
            }
            if (groupSize != 0) {
                if (Flags_GetSwitch(play, switchFlag)) {
                    if (this->snuffTimer == OBJ_SYOKUDAI_SNUFF_OUT) {
                        if (type != OBJ_SYOKUDAI_TYPE_SWITCH_CAUSES_FLAME) {
                            this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
                        } else {
                            this->pendingAction = OBJ_SYOKUDAI_PENDING_ACTION_CUTSCENE_NO_SWITCH;
                        }
                    } else if (this->snuffTimer > OBJ_SYOKUDAI_SNUFF_OUT) {
                        this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
                    }
                } else if (this->snuffTimer <= OBJ_SYOKUDAI_SNUFF_NEVER) {
                    this->snuffTimer = OBJ_SYOKUDAI_SNUFF_DEFAULT;
                }
            }
            if (this->flameCollider.base.acFlags & AC_HIT) {
                flameColliderACDmgFlags = this->flameCollider.elem.acHitElem->atDmgInfo.dmgFlags;
                if (this->flameCollider.elem.acHitElem->atDmgInfo.dmgFlags & 0x820) {
                    interaction = OBJ_SYOKUDAI_INTERACTION_ARROW_FA;
                }
            } else if (player->heldItemAction == PLAYER_IA_DEKU_STICK) {
                Vec3f stickTipSeparationVec;

                Math_Vec3f_Diff(&player->meleeWeaponInfo[0].tip, &thisx->world.pos, &stickTipSeparationVec);
                stickTipSeparationVec.y -= OBJ_SYOKUDAI_STICK_IGNITION_HEIGHT;
                if (SQXYZ(stickTipSeparationVec) < SQ(OBJ_SYOKUDAI_STICK_IGNITION_RADIUS)) {
                    interaction = OBJ_SYOKUDAI_INTERACTION_STICK;
                }
            }
            if (interaction != OBJ_SYOKUDAI_INTERACTION_NONE) {
                if (this->snuffTimer != OBJ_SYOKUDAI_SNUFF_OUT) {
                    if (interaction <= OBJ_SYOKUDAI_INTERACTION_STICK) {
                        if (player->unk_B28 == 0) {
                            player->unk_B28 = 0xD2;
                            Audio_PlaySfx_AtPos(&thisx->projectedPos, NA_SE_EV_FLAME_IGNITION);
                        } else if (player->unk_B28 < 0xC8) {
                            player->unk_B28 = 0xC8;
                        }
                    } else if (flameColliderACDmgFlags & 0x20) {
                        Actor* flameColliderACActor = this->flameCollider.base.ac;

                        if ((flameColliderACActor->update != NULL) && (flameColliderACActor->id == ACTOR_EN_ARROW)) {

                            flameColliderACActor->params = ARROW_TYPE_NORMAL_LIT;
                            ((EnArrow*)flameColliderACActor)->collider.elem.atDmgInfo.dmgFlags = 0x800;
                        }
                    }
                    if ((this->snuffTimer > OBJ_SYOKUDAI_SNUFF_NEVER) &&
                        (this->snuffTimer < (OBJ_SYOKUDAI_SNUFF_TIMER_INITIAL(groupSize))) &&
                        (type != OBJ_SYOKUDAI_TYPE_SWITCH_CAUSES_FLAME)) {

                        this->snuffTimer = OBJ_SYOKUDAI_SNUFF_TIMER_INITIAL(groupSize);
                    }
                } else if ((type != OBJ_SYOKUDAI_TYPE_SWITCH_CAUSES_FLAME) &&
                           (((interaction >= OBJ_SYOKUDAI_INTERACTION_ARROW_FA) && (flameColliderACDmgFlags & 0x800)) ||
                            ((interaction <= OBJ_SYOKUDAI_INTERACTION_STICK) && (player->unk_B28 != 0)))) {
                    if ((interaction < OBJ_SYOKUDAI_INTERACTION_NONE) && (player->unk_B28 < 0xC8)) {
                        player->unk_B28 = 0xC8;
                    }
                    if (groupSize == 0) {
                        if ((type == OBJ_SYOKUDAI_TYPE_NO_SWITCH) && (switchFlag == OBJ_SYOKUDAI_SWITCH_FLAG_NONE)) {
                            this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
                        } else if (thisx->csId > CS_ID_NONE) {
                            this->pendingAction = OBJ_SYOKUDAI_PENDING_ACTION_CUTSCENE_AND_SWITCH;
                        } else {
                            Flags_SetSwitch(play, switchFlag);
                            this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
                        }
                    } else {
                        sNumLitTorchesInGroup++;
                        if (sNumLitTorchesInGroup >= groupSize) {
                            this->pendingAction = OBJ_SYOKUDAI_PENDING_ACTION_CUTSCENE_AND_SWITCH;
                        } else {
                            this->snuffTimer =
                                OBJ_SYOKUDAI_SNUFF_TIMER_INITIAL(groupSize) + OBJ_SYOKUDAI_SNUFF_TIMER_JUST_LIT_BONUS;
                        }
                    }
                    AudioSfx_PlaySfx(NA_SE_EV_FLAME_IGNITION, &thisx->projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                }
            }
        }
    }
    Collider_UpdateCylinder(thisx, &this->standCollider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->standCollider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->standCollider.base);
    Collider_UpdateCylinder(thisx, &this->flameCollider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->flameCollider.base);
    if (this->snuffTimer > OBJ_SYOKUDAI_SNUFF_OUT) {
        this->snuffTimer--;
        if ((this->snuffTimer == OBJ_SYOKUDAI_SNUFF_OUT) && (type != OBJ_SYOKUDAI_TYPE_SWITCH_CAUSES_FLAME)) {
            sNumLitTorchesInGroup--;
        }
    }
    if (this->snuffTimer != OBJ_SYOKUDAI_SNUFF_OUT) {
        s32 pad2;

        if ((this->snuffTimer <= OBJ_SYOKUDAI_SNUFF_NEVER) || (this->snuffTimer >= OBJ_SYOKUDAI_SNUFF_DEFAULT)) {
            lightRadius = OBJ_SYOKUDAI_LIGHT_RADIUS_MAX;
        } else {
            lightRadius = (f32)this->snuffTimer * OBJ_SYOKUDAI_LIGHT_RADIUS_MAX / OBJ_SYOKUDAI_SNUFF_DEFAULT;
        }
        lightIntensity = Rand_ZeroOne() * 127;
        lightIntensity += 128;
        Actor_PlaySfx_Flagged(thisx, NA_SE_EV_TORCH - SFX_FLAG);
    }
    Lights_PointSetColorAndRadius(&this->lightInfo, lightIntensity, lightIntensity * 0.7f, 0, lightRadius);
    this->flameTexScroll++;
}

void ObjSyokudai_Draw(Actor* thisx, PlayState* play) {
    ObjSyokudai* this = (ObjSyokudai*)thisx;
    s32 pad;
    s32 groupSize = OBJ_SYOKUDAI_GET_GROUP_SIZE(thisx);
    f32 flameScale;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, sDLists[OBJ_SYOKUDAI_GET_TYPE(thisx)]);
    if (this->snuffTimer != OBJ_SYOKUDAI_SNUFF_OUT) {
        s32 snuffTimerInitial = OBJ_SYOKUDAI_SNUFF_TIMER_INITIAL(groupSize);

        flameScale = 1.0f;
        if (snuffTimerInitial < this->snuffTimer) {
            flameScale = (f32)(snuffTimerInitial - this->snuffTimer + OBJ_SYOKUDAI_SNUFF_TIMER_JUST_LIT_BONUS) /
                         OBJ_SYOKUDAI_SNUFF_TIMER_JUST_LIT_BONUS;
        } else if ((this->snuffTimer > OBJ_SYOKUDAI_SNUFF_OUT) && (this->snuffTimer < OBJ_SYOKUDAI_SNUFF_DEFAULT)) {
            flameScale = (f32)this->snuffTimer / OBJ_SYOKUDAI_SNUFF_DEFAULT;
        }
        flameScale *= 0.0027f;
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                      (this->flameTexScroll * -OBJ_SYOKUDAI_SNUFF_DEFAULT) & 0x1FF, 0x20, 0x80, 0, 0, 0,
                                      -OBJ_SYOKUDAI_SNUFF_DEFAULT));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
        Matrix_Translate(0.0f, OBJ_SYOKUDAI_FLAME_HEIGHT, 0.0f, MTXMODE_APPLY);
        Matrix_RotateYS(BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - thisx->shape.rot.y), MTXMODE_APPLY);
        Matrix_Scale(flameScale, flameScale, flameScale, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
