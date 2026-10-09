/*
 * File: z_en_ot.c
 * Overlay: ovl_En_Ot
 * Description: Seahorse
 */

#include "prevent_bss_reordering.h"
#include "z_en_ot.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnOt_Init(Actor* thisx, PlayState* play);
void EnOt_Destroy(Actor* thisx, PlayState* play);
void EnOt_Update(Actor* thisx, PlayState* play);
void EnOt_Draw(Actor* thisx, PlayState* play);
void EnOt_Reset(void);

void func_80B5BDA8(EnOt* this, PlayState* play);
void func_80B5BE04(EnOt* this, PlayState* play);
void func_80B5BE88(EnOt* this, PlayState* play);
void func_80B5BED4(EnOt* this, PlayState* play);
void func_80B5BF60(EnOt* this, PlayState* play);
void func_80B5BFB8(EnOt* this, PlayState* play);
void func_80B5C1CC(EnOt* this, PlayState* play);
void func_80B5C244(EnOt* this, PlayState* play);
void func_80B5C25C(EnOt* this, PlayState* play);
void func_80B5C3B8(EnOt* this, PlayState* play);
void func_80B5C3D8(EnOt* this, PlayState* play);
void func_80B5C634(EnOt* this, PlayState* play);
void func_80B5C64C(EnOt* this, PlayState* play);
void func_80B5C684(EnOt* this, PlayState* play);
void func_80B5C6DC(EnOt* this, PlayState* play);
void func_80B5C910(EnOt* this, PlayState* play);
void func_80B5C950(EnOt* this, PlayState* play);
void func_80B5C9A8(EnOt* this, PlayState* play);
void func_80B5C9C0(EnOt* this, PlayState* play);
void func_80B5C9D0(EnOt* this, PlayState* play);
void func_80B5CA30(EnOt* this, PlayState* play);
void func_80B5CAD0(EnOt* this, PlayState* play);
void func_80B5CB0C(EnOt* this, PlayState* play);
void func_80B5CBEC(EnOt* this, PlayState* play);
void func_80B5CC88(EnOt* this, PlayState* play);
void func_80B5CCA0(EnOt* this, PlayState* play);
void func_80B5CCF4(EnOt* this, PlayState* play);
void func_80B5CD40(EnOt* this, PlayState* play);
void func_80B5CE6C(EnOt* this, PlayState* play);
void func_80B5CEC8(EnOt* this, PlayState* play);
void func_80B5D114(EnOt* this, PlayState* play);
void func_80B5D160(EnOt* this, PlayState* play);
void func_80B5D648(EnOt* this, PlayState* play);
void func_80B5D750(EnOt* this, PlayState* play);
void func_80B5DAEC(Actor* thisx, PlayState* play);
void func_80B5DB6C(Actor* thisx, PlayState* play);
void EnOt_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx);
EnOtUnkStruct* func_80B5DF58(EnOtUnkStruct* arg0, u8 arg1, Vec3f* arg2, Vec3s* arg3, s32 arg4);
void func_80B5E078(PlayState* play, EnOtUnkStruct* arg1, s32 arg2);
void func_80B5E1D8(PlayState* play, EnOtUnkStruct* arg1, s32 arg2);

EnOt* D_80B5E880;
EnOt* D_80B5E884;
EnOt* D_80B5E888;

ActorProfile En_Ot_Profile = {
    /**/ ACTOR_EN_OT,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OT,
    /**/ sizeof(EnOt),
    /**/ EnOt_Init,
    /**/ EnOt_Destroy,
    /**/ EnOt_Update,
    /**/ EnOt_Draw,
    /**/ EnOt_Reset,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 5, 33, -20, { 0, 0, 0 } },
};

typedef enum SeahorseAnimation {
    /* 0 */ SEAHORSE_ANIM_0,
    /* 1 */ SEAHORSE_ANIM_1,
    /* 2 */ SEAHORSE_ANIM_2,
    /* 3 */ SEAHORSE_ANIM_MAX
} SeahorseAnimation;

static AnimationSpeedInfo sAnimationSpeedInfo[SEAHORSE_ANIM_MAX] = {
    { &object_ot_Anim_004B30, 1.0f, ANIMMODE_LOOP, -5.0f }, // SEAHORSE_ANIM_0
    { &object_ot_Anim_0008D8, 1.0f, ANIMMODE_LOOP, -5.0f }, // SEAHORSE_ANIM_1
    { &object_ot_Anim_000420, 1.0f, ANIMMODE_LOOP, 0.0f },  // SEAHORSE_ANIM_2
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 80, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 80, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_STOP),
};

void func_80B5B2E0(PlayState* play, Vec3f* pos, s16 pathIndex, Vec3f* vec, s32* index) {
    s32 i;
    Path* path = &play->setupPathList[pathIndex];
    f32 dist;
    Vec3f sp58;
    Vec3f sp4C;
    f32 minDist = FLT_MAX;

    for (i = 0; i < path->count; i++) {
        Math_Vec3s_ToVec3f(&sp58, &((Vec3s*)Lib_SegmentedToVirtual(path->points))[i]);
        dist = Math_Vec3f_DistXYZ(pos, &sp58);
        if (minDist > dist) {
            minDist = dist;
            Math_Vec3f_Copy(&sp4C, &sp58);
            *index = i;
        }
    }

    Math_Vec3f_Copy(vec, &sp4C);
}

void EnOt_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnOt* this = (EnOt*)thisx;
    s32 bgId;
    s32 pad2;
    Vec3f sp64;
    Vec3f sp58;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->unk_388 = 0;
    this->type = SEAHORSE_GET_TYPE(&this->actor);
    if (this->type == SEAHORSE_TYPE_0) {
        D_80B5E880 = this;
        this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.update = func_80B5DB6C;
        this->actor.draw = NULL;
        return;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_ot_Skel_004800, &object_ot_Anim_0008D8, this->jointTable,
                       this->morphTable, OBJECT_OT_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Animation_Change(&this->skelAnime, sAnimationSpeedInfo[SEAHORSE_ANIM_0].animation, 1.0f,
                     Animation_GetLastFrame(&sAnimationSpeedInfo[SEAHORSE_ANIM_0].animation->common) * Rand_ZeroOne(),
                     Animation_GetLastFrame(&sAnimationSpeedInfo[SEAHORSE_ANIM_0].animation->common),
                     sAnimationSpeedInfo[SEAHORSE_ANIM_0].mode, sAnimationSpeedInfo[SEAHORSE_ANIM_0].morphFrames);
    this->pathIndex = SEAHORSE_GET_PATH_INDEX(&this->actor);
    this->unk_344 = this->actor.world.rot.z;
    this->actor.world.rot.z = 0;
    this->actor.shape.rot.z = 0;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.gravity = 0.0f;
    SubS_FillCutscenesList(&this->actor, this->csIdList, ARRAY_COUNT(this->csIdList));
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SEAHORSE_ANIM_0, &this->animIndex);
    this->skelAnime.curFrame = Rand_ZeroOne() * this->skelAnime.endFrame;
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    this->unk_744.r = 255;
    this->unk_744.g = 200;
    this->unk_744.b = 80;

    switch (SEAHORSE_GET_TYPE(&this->actor)) {
        case SEAHORSE_TYPE_1:
            D_80B5E884 = this;
            Actor_SetScale(&this->actor, 1.3f * 0.01f);

            switch (this->unk_344) {
                case 0:
                    this->actor.world.pos.y = BgCheck_EntityRaycastFloor3(&play->colCtx, &this->actor.floorPoly, &bgId,
                                                                          &this->actor.world.pos) +
                                              50.0f;
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_84_10)) {
                        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
                        Matrix_MultVecZ(52.519997f, &sp64);
                        Math_Vec3f_Sum(&this->actor.world.pos, &sp64, &sp64);
                        this->unk_360 = (EnOt*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_OT, sp64.x, sp64.y, sp64.z,
                                                           0, BINANG_ROT180(this->actor.shape.rot.y), 1,
                                                           SEAHORSE_PARAMS_PARTNER(&this->actor, SEAHORSE_TYPE_2));
                        if (this->unk_360 != NULL) {
                            this->unk_360->unk_360 = this;
                            this->unk_3A0 = 0;
                            this->unk_360->unk_3A0 = BINANG_ROT180(this->unk_3A0);
                            this->unk_394.x = (this->actor.world.pos.x + this->unk_360->actor.world.pos.x) * 0.5f;
                            this->unk_394.y = this->actor.world.pos.y;
                            this->unk_394.z = (this->actor.world.pos.z + this->unk_360->actor.world.pos.z) * 0.5f;
                            Math_Vec3f_Copy(&this->unk_360->unk_394, &this->unk_394);
                            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SEAHORSE_HEART_PIECE)) {
                                func_80B5C244(this, play);
                            } else {
                                func_80B5C684(this, play);
                            }
                        } else {
                            Actor_Kill(&this->actor);
                        }
                    } else if ((D_80B5E888 != NULL) && (D_80B5E888->unk_32C & 1)) {
                        this->unk_360 = D_80B5E888;
                        this->unk_360->unk_360 = this;
                        Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_NEW);
                        Matrix_MultVecZ(800.0f, &sp58);
                        Math_Vec3f_Sum(&this->actor.world.pos, &sp58, &sp58);
                        Math_Vec3f_Copy(&this->unk_360->actor.world.pos, &sp58);
                        Math_Vec3f_Copy(&this->unk_360->actor.prevPos, &sp58);
                        func_80B5BDA8(this, play);
                        func_80B5BE88(this->unk_360, play);
                    } else {
                        func_80B5CE6C(this, play);
                    }
                    break;

                case 1:
                    Actor_Kill(&this->actor);
                    break;

                default:
                    break;
            }
            break;

        case SEAHORSE_TYPE_2:
            D_80B5E888 = this;

            switch (this->unk_344) {
                case 0:
                    Actor_SetScale(&this->actor, 0.0f);
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_13_01)) {
                        Actor_SetScale(&this->actor, 0.0f);
                        func_80B5C910(this, play);
                    } else {
                        Player* player = GET_PLAYER(play);

                        if (SurfaceType_IsHorseBlocked(&play->colCtx, player->actor.floorPoly,
                                                       player->actor.floorBgId)) {
                            Actor_SetScale(&this->actor, 0.0f);
                            func_80B5C910(this, play);
                        } else {
                            this->unk_360 = D_80B5E884;
                            this->unk_360->unk_360 = this;
                            func_80B5C9D0(this, play);
                        }
                    }
                    break;

                case 1:
                    Actor_SetScale(&this->actor, 1.3f * 0.01f);
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_84_10)) {
                        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SEAHORSE_HEART_PIECE)) {
                            func_80B5C244(this, play);
                        } else {
                            func_80B5C684(this, play);
                        }
                    } else {
                        func_80B5CE6C(this, play);
                    }
                    break;

                default:
                    break;
            }
            break;

        case SEAHORSE_TYPE_3:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_26_08)) {
                this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
                this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
                Actor_SetScale(&this->actor, 0.0064999997f);
                this->collider.dim.radius *= 0.5f;
                this->collider.dim.height *= 0.5f;
                this->collider.dim.yShift *= 0.5f;
                this->actor.update = func_80B5DAEC;
                func_80B5C634(this, play);
            } else {
                Actor_Kill(&this->actor);
            }
            break;

        default:
            break;
    }
}

void EnOt_Destroy(Actor* thisx, PlayState* play) {
    EnOt* this = (EnOt*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
}

void func_80B5BAAC(LightInfo* lightInfo, Vec3f* arg1, Color_RGB8* arg2, s16 radius) {
    Lights_PointNoGlowSetInfo(lightInfo, arg1->x, arg1->y, arg1->z, arg2->r, arg2->g, arg2->b, radius);
}

void EnOt_LerpColor(Color_RGB8* arg0, Color_RGB8* arg1, f32 lerp) {
    f32 rand = Rand_ZeroOne();

    arg0->r = (arg1->r * lerp) + (arg1->r * (1.0f - lerp) * rand);
    arg0->g = (arg1->g * lerp) + (arg1->g * (1.0f - lerp) * rand);
    arg0->b = (arg1->b * lerp) + (arg1->b * (1.0f - lerp) * rand);
}

void func_80B5BDA8(EnOt* this, PlayState* play) {
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SEAHORSE_ANIM_1, &this->animIndex);
    SubS_FillCutscenesList(&this->actor, this->csIdList, ARRAY_COUNT(this->csIdList));
    this->actionFunc = func_80B5BE04;
}

void func_80B5BE04(EnOt* this, PlayState* play) {
    switch (this->unk_388) {
        case 0:
            if (SubS_StartCutscene(&this->actor, this->csIdList[2], CS_ID_NONE, SUBS_CUTSCENE_WITH_PLAYER)) {
                func_80B5BF60(this, play);
            }
            break;

        case 1:
            if (SubS_StartCutscene(&this->actor, this->csIdList[3], CS_ID_NONE, SUBS_CUTSCENE_WITH_PLAYER)) {
                func_80B5BF60(this, play);
            }
            break;

        default:
            break;
    }
}

void func_80B5BE88(EnOt* this, PlayState* play) {
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SEAHORSE_ANIM_1, &this->animIndex);
    this->actionFunc = func_80B5BED4;
}

void func_80B5BED4(EnOt* this, PlayState* play) {
    func_800BE33C(&this->actor.world.pos, &this->unk_360->actor.world.pos, &this->actor.world.rot, false);
    Math_SmoothStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardActor(&this->actor, &this->unk_360->actor), 3,
                       0xE38, 0x38E);
    this->actor.speed = 3.5f;
    this->actor.world.pos.y = this->unk_360->actor.world.pos.y;
    Actor_MoveWithoutGravityReverse(&this->actor);
}

void func_80B5BF60(EnOt* this, PlayState* play) {
    this->unk_32C |= 0x40;
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SEAHORSE_ANIM_0, &this->animIndex);
    this->actionFunc = func_80B5BFB8;
}

void func_80B5BFB8(EnOt* this, PlayState* play) {
    Vec3f sp34;
    f32 temp_f0 = this->actor.floorHeight + 50.0f;

    if (temp_f0 <= this->actor.world.pos.y) {
        Math_StepToF(&this->actor.world.pos.y, temp_f0, 2.0f);
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardActor(&this->actor, &this->unk_360->actor), 3,
                       0xE38, 0x38E);

    if (Actor_WorldDistXYZToActor(&this->actor, &this->unk_360->actor) <= 52.519997f) {
        this->unk_73C = 50;
        Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_NEW);
        Matrix_MultVecZ(52.519997f, &sp34);
        this->unk_360->actor.world.pos.x = this->actor.world.pos.x + sp34.x;
        this->unk_360->actor.world.pos.y = this->actor.world.pos.y + sp34.y;
        this->unk_360->actor.world.pos.z = this->actor.world.pos.z + sp34.z;

        this->unk_394.x = (this->unk_360->actor.world.pos.x + this->actor.world.pos.x) * 0.5f;
        this->unk_394.y = (this->unk_360->actor.world.pos.y + this->actor.world.pos.y) * 0.5f;
        this->unk_394.z = (this->unk_360->actor.world.pos.z + this->actor.world.pos.z) * 0.5f;

        this->unk_360->unk_394.x = this->unk_394.x;
        this->unk_360->unk_394.y = this->unk_394.y;
        this->unk_360->unk_394.z = this->unk_394.z;

        func_80B5C684(this->unk_360, play);
        func_80B5C684(this, play);
    } else {
        Actor_MoveWithoutGravityReverse(&this->actor);
    }
}

void func_80B5C154(EnOt* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SEAHORSE_HEART_PIECE)) {
        this->getItemId = GI_RUPEE_RED;
    } else {
        this->getItemId = GI_HEART_PIECE;
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SEAHORSE_HEART_PIECE);
    }

    Actor_OfferGetItem(&this->actor, play, this->getItemId, this->actor.xzDistToPlayer, this->actor.playerHeightRel);
    this->actionFunc = func_80B5C1CC;
}

void func_80B5C1CC(EnOt* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        func_80B5C244(this, play);
        func_80B5C244(this->unk_360, play);
    } else {
        Actor_OfferGetItem(&this->actor, play, this->getItemId, this->actor.xzDistToPlayer,
                           this->actor.playerHeightRel);
    }
}

void func_80B5C244(EnOt* this, PlayState* play) {
    this->actionFunc = func_80B5C25C;
}

void func_80B5C25C(EnOt* this, PlayState* play) {
    this->unk_390 = Actor_WorldYawTowardActor(&this->actor, &this->unk_360->actor);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->unk_390, 3, 0xE38, 0x38E);
    if (BINANG_SUB(BINANG_ROT180(this->unk_360->actor.shape.rot.y), this->actor.shape.rot.y) < 0x38E) {
        this->unk_32C |= 0x80;
    }

    if ((this->type == SEAHORSE_TYPE_2) && (this->unk_32C & 0x80) && (this->unk_360->unk_32C & 0x80)) {
        this->unk_32C |= 0x100;
        this->unk_360->unk_32C |= 0x100;
        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SEAHORSE_ANIM_2, &this->animIndex);
        SubS_ChangeAnimationBySpeedInfo(&this->unk_360->skelAnime, sAnimationSpeedInfo, SEAHORSE_ANIM_2,
                                        &this->unk_360->animIndex);
        this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->unk_360->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
        this->unk_360->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        func_80B5C9A8(this->unk_360, play);
        func_80B5C3B8(this, play);
    }
}

void func_80B5C3B8(EnOt* this, PlayState* play) {
    this->unk_740 = 0.0f;
    this->actionFunc = func_80B5C3D8;
}

void func_80B5C3D8(EnOt* this, PlayState* play) {
    static Color_RGB8 D_80B5E408 = { 255, 200, 80 };
    static Color_RGB8 D_80B5E40C = { 255, 60, 200 };
    s16 temp;
    s32 pad;
    Vec3f sp5C;
    Vec3f sp50;
    Vec3f sp44;
    Vec3f sp38;

    this->unk_3A0 += 0x2D8;
    Matrix_RotateYS(this->unk_3A0, MTXMODE_NEW);
    Matrix_MultVecZ(26.259998f, &sp5C);
    Matrix_MultVecZ(-26.259998f, &sp50);
    this->unk_348.x = this->unk_394.x + sp5C.x;
    this->unk_348.y = this->unk_394.y;
    this->unk_348.z = this->unk_394.z + sp5C.z;

    this->unk_360->unk_348.x = this->unk_394.x + sp50.x;
    this->unk_360->unk_348.y = this->unk_394.y;
    this->unk_360->unk_348.z = this->unk_394.z + sp50.z;

    Math_Vec3f_Copy(&this->actor.world.pos, &this->unk_348);
    Math_Vec3f_Copy(&this->unk_360->actor.world.pos, &this->unk_360->unk_348);
    temp = Actor_WorldYawTowardActor(&this->actor, &this->unk_360->actor);
    this->actor.shape.rot.y = temp;
    this->unk_360->actor.shape.rot.y = BINANG_ROT180(temp);

    if (this->unk_740 < 1.0f) {
        Color_RGB8_Lerp(&D_80B5E408, &D_80B5E40C, this->unk_740, &this->unk_744);
        Color_RGB8_Lerp(&D_80B5E408, &D_80B5E40C, this->unk_740, &this->unk_360->unk_744);
        Math_StepToF(&this->unk_740, 1.0f, 0.05f);
    }

    if (Animation_OnFrame(&this->skelAnime, 12.0f)) {
        Matrix_RotateYS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)), MTXMODE_NEW);
        sp38.x = 1.0f;
        sp38.y = 8.1f;
        sp38.z = 0.0f;
        Matrix_MultVec3f(&sp38, &sp44);
        Math_Vec3f_Sum(&this->unk_74C, &sp44, &sp44);
        func_80B5DF58(this->unk_3A4, 1, &sp44, &this->actor.shape.rot, 10);
        sp38.x = -1.0f;
        sp38.y = 8.1f;
        sp38.z = 0.0f;
        Matrix_MultVec3f(&sp38, &sp44);
        Math_Vec3f_Sum(&this->unk_74C, &sp44, &sp44);
        func_80B5DF58(this->unk_3A4, 2, &sp44, &this->actor.shape.rot, 10);
    }
}

void func_80B5C634(EnOt* this, PlayState* play) {
    this->actionFunc = func_80B5C64C;
}

void func_80B5C64C(EnOt* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_26_08)) {
        Actor_Kill(&this->actor);
    }
}

void func_80B5C684(EnOt* this, PlayState* play) {
    this->actor.speed = 0.0f;
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SEAHORSE_ANIM_0, &this->animIndex);
    this->actionFunc = func_80B5C6DC;
}

void func_80B5C6DC(EnOt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 sp3E;
    Vec3f sp30;

    sp3E = Actor_WorldYawTowardPoint(&player->actor, &this->unk_394);
    Matrix_RotateYS(BINANG_ADD(sp3E, 0x4000), MTXMODE_NEW);
    if (this->type == SEAHORSE_TYPE_2) {
        Matrix_MultVecZ(26.259998f, &sp30);
    } else {
        if (this->unk_73C == 0) {
            SET_WEEKEVENTREG(WEEKEVENTREG_84_10);
            switch (this->unk_388) {
                case 0:
                    CutsceneManager_Stop(this->csIdList[2]);
                    break;

                case 1:
                    CutsceneManager_Stop(this->csIdList[3]);
                    break;

                default:
                    break;
            }
            this->unk_73C = -1;
        } else {
            this->unk_73C--;
        }
        Matrix_MultVecZ(-26.259998f, &sp30);
    }
    this->unk_348.x = this->unk_394.x + sp30.x;
    this->unk_348.y = this->unk_394.y;
    this->unk_348.z = this->unk_394.z + sp30.z;
    Math_SmoothStepToF(&this->actor.world.pos.x, this->unk_348.x, 1.0f, 2.0f, 0.01f);
    Math_SmoothStepToF(&this->actor.world.pos.z, this->unk_348.z, 1.0f, 2.0f, 0.01f);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0xE38, 0x38E);
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_84_10) && (this->type == SEAHORSE_TYPE_1)) {
        this->actor.textId = 0;
        this->unk_384 = 1;
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            this->unk_3A0 = BINANG_ADD(sp3E, 0x4000);
            this->unk_360->unk_3A0 = this->unk_3A0;
            func_80B5C9A8(this, play);
            func_80B5D114(this, play);
        } else if ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && !func_801242B4(player) &&
                   (this->actor.xzDistToPlayer < 130.0f)) {
            Actor_OfferTalk(&this->actor, play, 130.0f);
        }
    }
}

void func_80B5C910(EnOt* this, PlayState* play) {
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    // Set to itself
    this->actor.world.rot.x = this->actor.world.rot.x;
    this->actor.world.rot.y = this->actor.world.rot.y;
    this->actor.world.rot.z = this->actor.world.rot.z;
    this->actionFunc = func_80B5C950;
}

void func_80B5C950(EnOt* this, PlayState* play) {
    if (this->unk_32C & 8) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_SEAHORSE_OUT_BOTTLE);
        SET_WEEKEVENTREG(WEEKEVENTREG_25_04);
        func_80B5CAD0(this, play);
    }
}

void func_80B5C9A8(EnOt* this, PlayState* play) {
    this->actionFunc = func_80B5C9C0;
}

void func_80B5C9C0(EnOt* this, PlayState* play) {
}

void func_80B5C9D0(EnOt* this, PlayState* play) {
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    // Set to itself
    this->actor.world.rot.x = this->actor.world.rot.x;
    this->actor.world.rot.y = this->actor.world.rot.y;
    this->actor.world.rot.z = this->actor.world.rot.z;
    Actor_PlaySfx(&this->actor, NA_SE_EV_SEAHORSE_OUT_BOTTLE);
    this->actionFunc = func_80B5CA30;
}

void func_80B5CA30(EnOt* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.scale.x, 1.3f * 0.01f, 0.7f, 0.0001f, 0.01f);
    Actor_SetScale(&this->actor, this->actor.scale.x);
    if (this->actor.scale.x == 1.3f * 0.01f) {
        this->unk_360->unk_32C |= 0x1000;
        this->unk_360->unk_360 = this;
        func_80B5C9A8(this, play);
    }
}

void func_80B5CAD0(EnOt* this, PlayState* play) {
    SubS_FillCutscenesList(&this->actor, this->csIdList, ARRAY_COUNT(this->csIdList) / 2);
    this->actionFunc = func_80B5CB0C;
}

void func_80B5CB0C(EnOt* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.scale.x, 1.3f * 0.01f, 0.7f, 0.0001f, 0.01f);
    Actor_SetScale(&this->actor, this->actor.scale.x);
    if (this->actor.scale.x == 1.3f * 0.01f) {
        this->unk_32C |= 0x800;
        func_80B5CE6C(this, play);
    }
}

void func_80B5CBA0(EnOt* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    Actor_OfferTalkExchange(&this->actor, play, this->actor.xzDistToPlayer, this->actor.playerHeightRel,
                            PLAYER_IA_NONE);
    this->actionFunc = func_80B5CBEC;
}

void func_80B5CBEC(EnOt* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        func_80B5CC88(this, play);
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0xE38, 0x38E);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        Actor_OfferTalkExchange(&this->actor, play, this->actor.xzDistToPlayer, this->actor.playerHeightRel,
                                PLAYER_IA_NONE);
    }
}

void func_80B5CC88(EnOt* this, PlayState* play) {
    this->actionFunc = func_80B5CCA0;
}

void func_80B5CCA0(EnOt* this, PlayState* play) {
    if (SubS_StartCutscene(&this->actor, this->csIdList[0], CS_ID_GLOBAL_TALK, SUBS_CUTSCENE_NORMAL)) {
        Player* player = GET_PLAYER(play);

        player->stateFlags2 |= PLAYER_STATE2_20000000;
        func_80B5CCF4(this, play);
    }
}

void func_80B5CCF4(EnOt* this, PlayState* play) {
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SEAHORSE_ANIM_0, &this->animIndex);
    this->actionFunc = func_80B5CD40;
}

void func_80B5CD40(EnOt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 temp;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
            temp = Math_SmoothStepToS(&this->actor.shape.rot.y,
                                      BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))), 3, 0xE38, 0x38E);
            this->actor.world.rot.y = this->actor.shape.rot.y;

            //! FAKE:
            if (1) {}

            if (!temp) {
                SET_WEEKEVENTREG(WEEKEVENTREG_23_10);
                Message_StartTextbox(play, 0x1069, NULL);
            }
            break;

        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
            break;

        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play) && (play->msgCtx.currentTextId == 0x1069)) {
                this->unk_32C |= 4;
                CutsceneManager_Stop(this->csIdList[0]);
                player->stateFlags2 &= ~PLAYER_STATE2_20000000;
                func_80B5CE6C(this, play);
            }
            break;

        default:
            break;
    }
}

void func_80B5CE6C(EnOt* this, PlayState* play) {
    this->unk_384 = 0;
    this->unk_32C |= 0x20;
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SEAHORSE_ANIM_0, &this->animIndex);
    this->actionFunc = func_80B5CEC8;
}

void func_80B5CEC8(EnOt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;

    this->actor.textId = 0;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80B5D114(this, play);
        return;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0xE38, 0x38E);

    if (this->unk_32C & 0x800) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalkExchange(&this->actor, play, this->actor.xzDistToPlayer, this->actor.playerHeightRel,
                                PLAYER_IA_NONE);
    } else {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        if ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && !func_801242B4(player) &&
            (this->actor.xzDistToPlayer < 130.0f)) {
            Actor_OfferTalk(&this->actor, play, 130.0f);
        }
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_84_10) && (SEAHORSE_GET_TYPE(&this->actor) == SEAHORSE_TYPE_1)) {
        if ((fabsf(this->actor.xzDistToPlayer) <= 130.0f) && (fabsf(this->actor.playerHeightRel) <= 130.0f)) {
            player->unk_B2B = 29;
        }

        if ((D_80B5E888 != NULL) && (D_80B5E888->unk_32C & 1)) {
            Vec3f sp2C;

            this->unk_360 = D_80B5E888;
            this->unk_360->unk_360 = this;
            Matrix_RotateYS(this->actor.home.rot.y, MTXMODE_NEW);
            Matrix_MultVecZ(800.0f, &sp2C);
            Math_Vec3f_Sum(&this->actor.world.pos, &sp2C, &this->unk_360->actor.world.pos);
            Math_Vec3f_Copy(&this->unk_360->actor.prevPos, &this->unk_360->actor.world.pos);
            this->unk_32C &= ~0x800;
            this->unk_360->unk_32C &= ~0x800;
            func_80B5BDA8(this, play);
            func_80B5BE88(this->unk_360, play);
            return;
        }
    }

    if (this->unk_32C & 0x1000) {
        this->unk_388 = 1;
        func_80B5BDA8(this, play);
        func_80B5BE88(this->unk_360, play);
    }
}

void func_80B5D114(EnOt* this, PlayState* play) {
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SEAHORSE_ANIM_0, &this->animIndex);
    this->actionFunc = func_80B5D160;
}

void func_80B5D160(EnOt* this, PlayState* play) {
    u16 phi_a1;
    s32 temp;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
            temp = Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0xE38, 0x38E);
            this->actor.world.rot.y = this->actor.shape.rot.y;
            if (!temp) {
                switch (this->unk_384) {
                    case 0:
                        if ((this->type != SEAHORSE_TYPE_1) && (this->type == SEAHORSE_TYPE_2)) {
                            if (this->unk_32C & 1) {
                                if (this->unk_32C & 4) {
                                    phi_a1 = 0x106A;
                                } else {
                                    phi_a1 = 0x1069;
                                }
                            } else if (Flags_GetSwitch(play, SEAHORSE_GET_SWITCH_FLAG(&this->actor))) {
                                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_23_10)) {
                                    phi_a1 = 0x106C;
                                } else {
                                    phi_a1 = 0x106B;
                                }
                            } else {
                                phi_a1 = 0x1068;
                            }
                        } else {
                            phi_a1 = 0x10A5;
                        }
                        Message_StartTextbox(play, phi_a1, &this->actor);
                        func_80B5D114(this, play);
                        break;

                    case 1:
                        Message_StartTextbox(play, 0x106D, &this->actor);
                        func_80B5D114(this, play);
                        break;

                    default:
                        break;
                }
            }
            break;

        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
            break;

        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    default:
                    case 0x1068:
                    case 0x106B:
                    case 0x106C:
                        func_80B5D648(this, play);
                        break;

                    case 0x1069:
                        this->unk_32C |= 4;

                    case 0x106A:
                        func_80B5CE6C(this, play);
                        break;

                    case 0x10A5:
                        Message_CloseTextbox(play);
                        func_80B5CE6C(this, play);
                        break;

                    case 0x106D:
                        Message_CloseTextbox(play);
                        func_80B5C154(this, play);
                        break;
                }
            }
            break;

        default:
            break;
    }
}

s32 EnOt_ActorPathing_Move(PlayState* play, ActorPathing* actorPath) {
    Actor* thisx = actorPath->actor;
    EnOt* this = (EnOt*)thisx;
    f32 sp24 = Math_CosS(-thisx->world.rot.x) * thisx->speed;
    f32 sp20 = gFramerateDivisorHalf;

    thisx->velocity.x = Math_SinS(thisx->world.rot.y) * sp24;
    thisx->velocity.y = Math_SinS(-thisx->world.rot.x) * thisx->speed;
    thisx->velocity.z = Math_CosS(thisx->world.rot.y) * sp24;

    this->unk_330.x += (thisx->velocity.x * sp20) + thisx->colChkInfo.displacement.x;
    this->unk_330.y += (thisx->velocity.y * sp20) + thisx->colChkInfo.displacement.y;
    this->unk_330.z += (thisx->velocity.z * sp20) + thisx->colChkInfo.displacement.z;

    return false;
}

s32 EnOt_ActorPathing_UpdateActorInfo(PlayState* play, ActorPathing* actorPath) {
    Actor* thisx = actorPath->actor;
    s32 ret = false;
    s32 pad;
    Vec3f sp50;
    Vec3f sp44;
    f32 temp;
    s32 sp2C;
    s32 sp30;
    s32 sp34;

    thisx->gravity = 0.0f;
    Math_SmoothStepToF(&thisx->speed, 10.0f, 0.8f, 2.0f, 0.0f);

    sp50.x = actorPath->curPoint.x - thisx->world.pos.x;
    sp50.y = actorPath->curPoint.y - thisx->world.pos.y;
    sp50.z = actorPath->curPoint.z - thisx->world.pos.z;

    sp44.x = actorPath->curPoint.x - actorPath->prevPoint.x;
    sp44.y = actorPath->curPoint.y - actorPath->prevPoint.y;
    sp44.z = actorPath->curPoint.z - actorPath->prevPoint.z;

    temp = Math3D_Cos(&sp50, &sp44);
    if ((actorPath->distSqToCurPointXZ < SQ(thisx->speed)) || (temp <= 0.0f)) {
        ret = true;
    } else {
        temp = SQ(thisx->speed) / actorPath->distSqToCurPoint;
        sp34 = ABS(actorPath->rotToCurPoint.x - thisx->world.rot.x);
        sp2C = (s32)(sp34 * temp) + 0xAAA;

        sp34 = ABS(actorPath->rotToCurPoint.y - thisx->world.rot.y);

        Math_SmoothStepToS(&thisx->world.rot.x, actorPath->rotToCurPoint.x, 1, sp2C, 0);
        sp2C = (s32)(sp34 * temp) + 0xAAA;
        Math_SmoothStepToS(&thisx->world.rot.y, actorPath->rotToCurPoint.y, 1, sp2C, 0);
        Math_SmoothStepToS(&thisx->shape.rot.y, thisx->world.rot.y, 2, sp2C, 0);
    }

    return ret;
}

void func_80B5D648(EnOt* this, PlayState* play) {
    func_80B5B2E0(play, &this->actor.world.pos, this->pathIndex, &this->unk_348, &this->unk_340);
    Math_Vec3f_Copy(&this->unk_330, &this->actor.world.pos);
    SubS_ActorPathing_Init(play, &this->unk_330, &this->actor, &this->actorPath, play->setupPathList, this->pathIndex,
                           0, 0, this->unk_340, 0);
    this->unk_32C = 0;
    this->actorPath.pointOffset.x = 0.0f;
    this->actorPath.pointOffset.y = 0.0f;
    this->actorPath.pointOffset.z = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.speed = 0.0f;
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SEAHORSE_ANIM_1, &this->animIndex);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    Flags_SetSwitch(play, SEAHORSE_GET_SWITCH_FLAG(&this->actor));
    this->actionFunc = func_80B5D750;
}

void func_80B5D750(EnOt* this, PlayState* play) {
    if (!(this->unk_32C & 1) && !(this->unk_32C & 2)) {
        SubS_ActorPathing_Update(play, &this->actorPath, SubS_ActorPathing_ComputePointInfo,
                                 EnOt_ActorPathing_UpdateActorInfo, EnOt_ActorPathing_Move,
                                 SubS_ActorPathing_SetNextPoint);
    }

    Math_Vec3f_Copy(&this->actor.world.pos, &this->unk_330);

    if (this->actorPath.flags & ACTOR_PATHING_REACHED_POINT_TEMPORARY) {
        this->unk_32C |= 2;
    }

    if (this->actorPath.flags & ACTOR_PATHING_REACHED_END_TEMPORARY) {
        this->unk_32C |= 1;
    }

    if ((this->unk_32C & 2) && (this->actor.xyzDistToPlayerSq <= SQ(120.0f))) {
        this->unk_32C &= ~2;
    }

    if ((this->unk_32C & 1) && (this->actor.xzDistToPlayer <= 180.0f)) {
        this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        if (D_80B5E884 != NULL) {
            func_80B5C9A8(this, play);
        } else {
            func_80B5CBA0(this, play);
        }
    }
}

void EnOt_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnOt* this = (EnOt*)thisx;

    if ((this->animIndex == SEAHORSE_ANIM_1) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_SEAHORSE_SWIM);
    }

    this->actionFunc(this, play);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
        if (DECR(this->unk_354) == 0) {
            if (this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
                s32 i;

                for (i = 0; i < 2; i++) {
                    EffectSsBubble_Spawn(play, &this->actor.world.pos, 0.0f, 20.0f, 5.0f, 0.1f);
                }
                this->unk_354 = (Rand_ZeroOne() * 10.0f) + 10.0f;
            }
        }
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 10.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    if (this->actor.world.pos.y <= this->actor.floorHeight + 50.0f) {
        this->actor.world.pos.y = this->actor.floorHeight + 50.0f;
        this->actor.prevPos.y = this->actor.floorHeight + 50.0f;
    }

    Actor_SetFocus(&this->actor, 12.0f);
    SkelAnime_Update(&this->skelAnime);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    EnOt_LerpColor(&this->unk_747, &this->unk_744, 0.7f);

    if (this->unk_32C & 0x400) {
        func_80B5BAAC(&this->lightInfo, &this->unk_378, &this->unk_747, 210);
    }

    func_80B5E078(play, this->unk_3A4, 10);
}

void func_80B5DAEC(Actor* thisx, PlayState* play) {
    s32 pad;
    EnOt* this = (EnOt*)thisx;

    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, 12.0f);
    SkelAnime_Update(&this->skelAnime);
    EnOt_LerpColor(&this->unk_747, &this->unk_744, 0.7f);
    if (this->unk_32C & 0x400) {
        func_80B5BAAC(&this->lightInfo, &this->unk_378, &this->unk_747, 210);
    }
}

void func_80B5DB6C(Actor* thisx, PlayState* play) {
    s32 pad;
    EnOt* this = (EnOt*)thisx;
    Player* player = GET_PLAYER(play);

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_84_10) && !(this->unk_32C & 8)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_25_04)) {
            Vec3f sp50;

            func_80B5B2E0(play, &this->actor.world.pos, SEAHORSE_GET_PATH_INDEX(&this->actor), &sp50, &this->unk_340);
            if (Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_OT, sp50.x, sp50.y, sp50.z, 0,
                                              this->actor.shape.rot.y, 1,
                                              SEAHORSE_PARAMS_PARTNER(&this->actor, SEAHORSE_TYPE_2), this->actor.csId,
                                              this->actor.halfDaysBits, NULL) != NULL) {
                this->unk_32C |= 8;
            }
        } else if (D_80B5E888 != NULL) {
            s32 sp4C = false;

            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_13_01)) {
                if (!SurfaceType_IsHorseBlocked(&play->colCtx, player->actor.floorPoly, player->actor.floorBgId)) {
                    sp4C = true;
                }
            }

            if (!sp4C) {
                EnOt* temp = D_80B5E888;

                temp->unk_32C |= 8;
                temp->pathIndex = SEAHORSE_GET_PATH_INDEX(&this->actor);
                temp->actor.params = this->actor.params;
                temp->actor.csId = this->actor.csId;
                this->unk_32C |= 8;
            }
        } else if (SurfaceType_IsHorseBlocked(&play->colCtx, player->actor.floorPoly, player->actor.floorBgId)) {
            player->unk_B2B = 29;
        }
    }
}

void EnOt_Draw(Actor* thisx, PlayState* play) {
    s32 pad[2];
    EnOt* this = (EnOt*)thisx;
    Gfx* gfx;

    Matrix_Push();
    func_80B5E1D8(play, this->unk_3A4, ARRAY_COUNT(this->unk_3A4));
    Matrix_Pop();

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP, SETUPDL_25);
    POLY_XLU_DISP = Gfx_SetupDL71(POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);

    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_ot_Matanimheader_0005F8));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          EnOt_PostLimbDraw, &this->actor);
    Matrix_Translate(this->unk_378.x, this->unk_378.y, this->unk_378.z, MTXMODE_NEW);
    Matrix_Scale(0.0882f, 0.0882f, 0.0882f, MTXMODE_APPLY);

    OPEN_DISPS(play->state.gfxCtx);

    gfx = Gfx_SetupDL65_NoCD(POLY_XLU_DISP);

    gDPSetDither(&gfx[0], G_CD_NOISE);
    gDPSetCombineLERP(&gfx[1], 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                      0);
    gSPDisplayList(&gfx[2], gameplay_keep_DL_029CB0);
    gDPSetPrimColor(&gfx[3], 0, 0, this->unk_747.r, this->unk_747.g, this->unk_747.b, 50);
    MATRIX_FINALIZE_AND_LOAD(&gfx[4], play->state.gfxCtx);
    gSPDisplayList(&gfx[5], gameplay_keep_DL_029CF0);

    POLY_XLU_DISP = &gfx[6];

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnOt_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80B5E410 = { 400.0f, 600.0f, 0.0f };
    EnOt* this = (EnOt*)thisx;

    if (limbIndex == OBJECT_OT_LIMB_04) {
        OPEN_DISPS(play->state.gfxCtx);
        Gfx* gfx = POLY_OPA_DISP;

        gSPDisplayList(&gfx[0], object_ot_DL_0004A0);
        POLY_OPA_DISP = &gfx[1];
        Matrix_MultVec3f(&D_80B5E410, &this->unk_74C);

        CLOSE_DISPS(play->state.gfxCtx);
    } else if (limbIndex == OBJECT_OT_LIMB_01) {
        Matrix_MultZero(&this->unk_378);
        this->unk_32C |= 0x400;
    }
}

EnOtUnkStruct* func_80B5DF58(EnOtUnkStruct* arg0, u8 arg1, Vec3f* arg2, Vec3s* arg3, s32 arg4) {
    s32 i = 0;

    while ((i < arg4) && arg0->unk_00) {
        i++;
        arg0++;
    }

    if ((i < arg4) && ((arg1 == 1) || (arg1 == 2))) {
        arg0->unk_00 = arg1;
        Math_Vec3f_Copy(&arg0->unk_0C, arg2);
        Math_Vec3f_Copy(&arg0->unk_50, arg2);
        arg0->unk_04 = 0.009f;
        arg0->unk_30 = 0.0f;
        arg0->unk_34 = 0.0f;
        arg0->unk_38 = 0.3f;
        arg0->unk_3C = 1.4f;
        arg0->unk_40 = -0.05f;
        arg0->unk_4C = 40;

        if (arg0->unk_00 == 2) {
            arg0->unk_30 = -arg0->unk_30;
            arg0->unk_38 = -arg0->unk_38;
        }
    }

    return arg0;
}

void func_80B5E078(PlayState* play, EnOtUnkStruct* arg1, s32 arg2) {
    Vec3f sp54;
    s32 i;
    s16 temp = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));

    for (i = 0; i < arg2; i++, arg1++) {
        if ((arg1->unk_00 == 1) || (arg1->unk_00 == 2)) {
            if (DECR(arg1->unk_4C) == 0) {
                arg1->unk_00 = 0;
            }

            arg1->unk_30 += arg1->unk_38;
            arg1->unk_34 += arg1->unk_3C;
            arg1->unk_3C += arg1->unk_40;
            sp54.x = arg1->unk_30;
            sp54.y = arg1->unk_34;
            sp54.z = 0.0f;
            Matrix_RotateYS(temp, MTXMODE_NEW);
            Matrix_MultVec3f(&sp54, &arg1->unk_0C);
            Math_Vec3f_Sum(&arg1->unk_0C, &arg1->unk_50, &arg1->unk_0C);
            arg1->unk_4C--;
        }
    }
}

void func_80B5E1D8(PlayState* play, EnOtUnkStruct* arg1, s32 arg2) {
    s32 i;
    s32 flag = false;

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);

    for (i = 0; i < arg2; i++, arg1++) {
        if (arg1->unk_00 == 0) {
            continue;
        }

        if (!flag) {
            gSPDisplayList(POLY_OPA_DISP++, object_ot_DL_000040);
            flag = true;
        }

        Matrix_Translate(arg1->unk_0C.x, arg1->unk_0C.y, arg1->unk_0C.z, MTXMODE_NEW);
        Matrix_RotateYS(BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))), MTXMODE_APPLY);
        Matrix_Scale(arg1->unk_04, arg1->unk_04, arg1->unk_04, MTXMODE_APPLY);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(gDropRecoveryHeartTex));
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_ot_DL_000078);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnOt_Reset(void) {
    D_80B5E880 = NULL;
    D_80B5E884 = NULL;
    D_80B5E888 = NULL;
}
