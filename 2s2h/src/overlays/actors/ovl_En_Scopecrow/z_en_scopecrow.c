/*
 * File: z_en_scopecrow.c
 * Overlay: ovl_En_Scopecrow
 * Description: Astral Observatory - Guay in Telescope
 */

#include "z_en_scopecrow.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnScopecrow_Init(Actor* thisx, PlayState* play);
void EnScopecrow_Destroy(Actor* thisx, PlayState* play);
void EnScopecrow_Update(Actor* thisx, PlayState* play);
void EnScopecrow_Draw(Actor* thisx, PlayState* play);

void func_80BCD590(EnScopecrow* this, PlayState* play);
void func_80BCD640(EnScopecrow* this, PlayState* play);

ActorProfile En_Scopecrow_Profile = {
    /**/ ACTOR_EN_SCOPECROW,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_CROW,
    /**/ sizeof(EnScopecrow),
    /**/ EnScopecrow_Init,
    /**/ EnScopecrow_Destroy,
    /**/ EnScopecrow_Update,
    /**/ EnScopecrow_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 60, 0 }, 50 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

void func_80BCD000(EnScopecrow* this, PlayState* play) {
    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y =
        sJntSphInit.elements[0].dim.modelSphere.center.y + this->actor.world.pos.y;
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

s32 func_80BCD09C(s16 arg0) {
    switch (arg0) {
        case 0:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_53_04)) {
                return true;
            } else {
                return false;
            }

        case 1:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_53_80)) {
                return true;
            } else {
                return false;
            }

        case 2:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_54_01)) {
                return true;
            } else {
                return false;
            }

        case 3:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_54_02)) {
                return true;
            } else {
                return false;
            }

        case 4:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_54_04)) {
                return true;
            } else {
                return false;
            }

        case 5:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_54_08)) {
                return true;
            } else {
                return false;
            }
    }

    return false;
}

s32 func_80BCD1AC(s16 arg0) {
    switch (arg0) {
        case 0:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_53_04)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_53_04);
                return true;
            }
            break;

        case 1:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_53_80)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_53_80);
                return true;
            }
            break;

        case 2:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_54_01)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_54_01);
                return true;
            }
            break;

        case 3:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_54_02)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_54_02);
                return true;
            }
            break;

        case 4:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_54_04)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_54_04);
                return true;
            }
            break;

        case 5:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_54_08)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_54_08);
                return true;
            }
            break;
    }

    return false;
}

void func_80BCD2BC(EnScopecrow* this, PlayState* play) {
    Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_SC_RUPPE, this->actor.world.pos.x,
                                  this->actor.world.pos.y, this->actor.world.pos.z, this->actor.shape.rot.x,
                                  this->actor.shape.rot.y, this->actor.shape.rot.z, this->actor.params,
                                  this->actor.csId, this->actor.halfDaysBits, NULL);
}

s32 EnScopecrow_HasReachedPoint(EnScopecrow* this, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * this->actor.world.pos.x) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

f32 func_80BCD4D0(Path* path, s32 count, Vec3f* arg2, Vec3s* arg3) {
    Vec3s* temp;
    Vec3f sp20;
    Vec3s* points;

    if (path != NULL) {
        temp = Lib_SegmentedToVirtual(path->points);
        points = temp + count;

        sp20.x = points->x;
        sp20.y = points->y;
        sp20.z = points->z;
    }

    arg3->y = Math_Vec3f_Yaw(arg2, &sp20);
    arg3->x = Math_Vec3f_Pitch(arg2, &sp20);
    return sp20.y - arg2->y;
}

void func_80BCD590(EnScopecrow* this, PlayState* play) {
    Vec3f screenPos;

    Play_GetScreenPos(play, &this->actor.world.pos, &screenPos);

    if ((screenPos.x >= 130.0f) && (screenPos.x < (SCREEN_WIDTH - 130.0f)) && (screenPos.y >= 90.0f) &&
        (screenPos.y < (SCREEN_HEIGHT - 90.0f))) {
        this->actor.draw = EnScopecrow_Draw;
        this->actionFunc = func_80BCD640;
    }
}

void func_80BCD640(EnScopecrow* this, PlayState* play) {
    Vec3s sp30;

    if (this->path != NULL) {
        func_80BCD4D0(this->path, this->unk_1FC, &this->actor.world.pos, &sp30);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            sp30.y = this->actor.wallYaw;
        }

        Math_SmoothStepToS(&this->actor.world.rot.y, sp30.y, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        Math_SmoothStepToS(&this->actor.world.rot.x, -sp30.x, 4, 0x3E8, 1);

        if (EnScopecrow_HasReachedPoint(this, this->path, this->unk_1FC)) {
            if ((this->unk_1FC == this->unk_262) && func_80BCD1AC(this->unk_260)) {
                func_80BCD2BC(this, play);
            }

            if (this->unk_1FC >= (this->path->count - 1)) {
                Actor_Kill(&this->actor);
            } else {
                this->unk_1FC++;
            }
        }
    }

    Math_ApproachF(&this->actor.speed, 6.0f, 0.2f, 1.0f);
    Actor_MoveWithoutGravity(&this->actor);
    this->unk_264 += 0x1000;
    this->actor.shape.yOffset = Math_SinS(this->unk_264) * 500.0f;
}

void EnScopecrow_Init(Actor* thisx, PlayState* play) {
    EnScopecrow* this = (EnScopecrow*)thisx;
    Vec3s* temp;
    CollisionPoly* sp4C;
    Vec3s* points;
    Vec3f sp3C;

    this->unk_260 = ENSCOPECROW_GET_1F(&this->actor);
    if ((this->unk_260 < 0) || (this->unk_260 >= 6)) {
        this->unk_260 = 0;
    }

    if (func_80BCD09C(this->unk_260)) {
        this->path = SubS_GetPathByIndex(play, ENSCOPECROW_GET_PATH_INDEX(&this->actor), ENSCOPECROW_PATH_INDEX_NONE);
        this->unk_262 = ENSCOPECROW_GET_3E0(&this->actor);

        if (this->path != NULL) {
            if ((this->unk_262 <= 0) || ((this->path->count - 1) < this->unk_262)) {
                this->unk_262 = this->path->count - 1;
            }

            temp = Lib_SegmentedToVirtual(this->path->points);
            points = temp + this->unk_262;

            sp3C.x = points->x;
            sp3C.y = points->y;
            sp3C.z = points->z;

            this->actor.world.pos = sp3C;
            this->actor.world.pos.y = BgCheck_EntityRaycastFloor1(&play->colCtx, &sp4C, &sp3C);
            if (this->actor.world.pos.y == BGCHECK_Y_MIN) {
                Actor_Kill(&this->actor);
            }

            func_80BCD2BC(this, play);
            Actor_Kill(&this->actor);
            return;
        }

        Actor_Kill(&this->actor);
        return;
    }

    if (play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) {
        SkelAnime_InitFlex(play, &this->skelAnime, &gGuaySkel, &gGuayFlyAnim, this->jointTable, this->morphTable,
                           OBJECT_CROW_LIMB_MAX);
        ActorShape_Init(&this->actor.shape, 2000.0f, ActorShadow_DrawCircle, 20.0f);

        Collider_InitJntSph(play, &this->collider);
        Collider_InitAndSetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);
        this->collider.elements[0].dim.worldSphere.radius = sJntSphInit.elements[0].dim.modelSphere.radius;

        Actor_SetScale(&this->actor, 0.03f);
        this->path = SubS_GetPathByIndex(play, ENSCOPECROW_GET_PATH_INDEX(&this->actor), ENSCOPECROW_PATH_INDEX_NONE);
        this->unk_262 = ENSCOPECROW_GET_3E0(&this->actor);

        if (this->path != NULL) {
            if ((this->unk_262 <= 0) || ((this->path->count - 1) < this->unk_262)) {
                this->unk_262 = this->path->count - 1;
            }
            this->actor.draw = NULL;
            this->actor.gravity = 0.0f;
            this->actionFunc = func_80BCD590;
            return;
        }

        Actor_Kill(&this->actor);
        return;
    }

    Actor_Kill(&this->actor);
}

void EnScopecrow_Destroy(Actor* thisx, PlayState* play) {
    EnScopecrow* this = (EnScopecrow*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnScopecrow_Update(Actor* thisx, PlayState* play) {
    EnScopecrow* this = (EnScopecrow*)thisx;

    this->actionFunc(this, play);

    SkelAnime_Update(&this->skelAnime);
    func_80BCD000(this, play);
}

void EnScopecrow_Draw(Actor* thisx, PlayState* play) {
    EnScopecrow* this = (EnScopecrow*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);
}
