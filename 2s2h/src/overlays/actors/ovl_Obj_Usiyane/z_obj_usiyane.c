/*
 * File: z_obj_usiyane.c
 * Overlay: ovl_Obj_Usiyane
 * Description: Roof of Cow Barn
 */

#include "z_obj_usiyane.h"
#include "objects/object_obj_usiyane/object_obj_usiyane.h"

#define FLAGS (ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjUsiyane_Init(Actor* thisx, PlayState* play);
void ObjUsiyane_Destroy(Actor* thisx, PlayState* play);
void ObjUsiyane_Update(Actor* thisx, PlayState* play);
void ObjUsiyane_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Usiyane_Profile = {
    /**/ ACTOR_OBJ_USIYANE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_OBJ_USIYANE,
    /**/ sizeof(ObjUsiyane),
    /**/ ObjUsiyane_Init,
    /**/ ObjUsiyane_Destroy,
    /**/ ObjUsiyane_Update,
    /**/ ObjUsiyane_Draw,
};

PosRot D_80C08660[] = {
    { { 800.0f, -940.0f, 2000.0f }, { 0, 0, 0x2AF8 } },
    { { 560.0f, -790.0f, -2000.0f }, { 0, 0, 0x2EE0 } },
    { { -480.0f, -780.0f, -1990.0f }, { 0, 0, -0x2710 } },
    { { -470.0f, -850.0f, 2000.0f }, { 0, 0, -0x2710 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 900, ICHAIN_STOP),
};

s32 ObjUsiyane_GetAlienSpawnTime(s32 alienIndex) {
    return CLOCK_TIME(2, 30) + ALIEN_GET_SPAWN_TIME_OFFSET(alienIndex);
}

s32 func_80C07CD0(void) {
    if (CURRENT_DAY < 1) {
        return false;
    }

    if (CURRENT_DAY == 1) {
        s32 time = CURRENT_TIME;
        s32 i;

        if ((time < CLOCK_TIME(2, 30)) || (time >= CLOCK_TIME(6, 0))) {
            return false;
        }

        if (time < CLOCK_TIME(5, 15)) {
            s32 firstSpawn = CLOCK_TIME(5, 15);
            s32 spawnTime;

            for (i = 0; i < 8; i++) {
                spawnTime = ObjUsiyane_GetAlienSpawnTime(i);
                firstSpawn = CLAMP_MAX(firstSpawn, spawnTime);
            }

            if (time < (firstSpawn + (80 * CLOCK_TIME_MINUTE) + 1)) {
                return false;
            }
        }
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
        return false;
    }
    return true;
}

void func_80C07DC4(ObjUsiyane* this, PlayState* play) {
    if (func_80C07CD0()) {
        Actor_Kill(&this->dyna.actor);
    }
}

void func_80C07DFC(Vec3f* arg0, Vec3s* arg1, Vec3f* arg2, Vec3s* arg3, s32 arg4, s32 arg5, Vec3f* arg6, Vec3s* arg7) {
    f32 temp_fv0 = (f32)arg4 / arg5;

    // VEC3F_LERPIMPDST
    arg6->x = ((arg2->x - arg0->x) * temp_fv0) + arg0->x;
    arg6->y = ((arg2->y - arg0->y) * temp_fv0) + arg0->y;
    arg6->z = ((arg2->z - arg0->z) * temp_fv0) + arg0->z;
    arg7->x = (((f32)arg3->x - (f32)arg1->x) * temp_fv0) + arg1->x;
    arg7->y = (((f32)arg3->y - (f32)arg1->y) * temp_fv0) + arg1->y;
    arg7->z = (((f32)arg3->z - (f32)arg1->z) * temp_fv0) + arg1->z;
}

void func_80C07F30(ObjUsiyane* this, PlayState* play) {
    s32 i;
    s32 j;
    Vec3f sp94;

    for (i = 0; i < ARRAY_COUNT(this->unk_168[0]); i++) {
        for (j = 0; j < ARRAY_COUNT(this->unk_168); j++) {
            if (i != (ARRAY_COUNT(this->unk_168[0]) - 1)) {
                func_80C07DFC(&this->unk_710[i], &D_80C08660[i].rot, &this->unk_710[i + 1], &D_80C08660[i + 1].rot, j,
                              10, &this->unk_168[j][i].unk_00, &this->unk_168[j][i].unk_18);
            } else {
                func_80C07DFC(&this->unk_710[i], &D_80C08660[i].rot, &this->unk_710[0], &D_80C08660[0].rot, j, 10,
                              &this->unk_168[j][i].unk_00, &this->unk_168[j][i].unk_18);
            }
            func_800B12F0(play, &this->unk_168[j][i].unk_00, &gZeroVec3f, &gZeroVec3f, 100, 30, 7);

            this->unk_168[j][i].unk_18.y = (s32)Rand_Next() >> 0x10;

            sp94.x = this->unk_168[j][i].unk_00.x - this->unk_708;
            sp94.y = 0.0f;
            sp94.z = this->unk_168[j][i].unk_00.z - this->unk_70C;
            Math3D_Normalize(&sp94);

            sp94.x *= 5.0f;
            sp94.y = 19.0f;
            sp94.z *= 5.0f;

            this->unk_168[j][i].unk_0C = sp94;

            this->unk_168[j][i].unk_1E.x = Rand_Centered() * 4000.0f;
            this->unk_168[j][i].unk_1E.y = Rand_Centered() * 4000.0f;
            this->unk_168[j][i].unk_1E.z = Rand_Centered() * 4000.0f;
        }
    }

    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_HOUSE_BROKEN);
    this->unk_740 = 0;
    this->unk_744 |= 4;
}

void func_80C081C8(ObjUsiyane* this, PlayState* play) {
    s32 i;
    s32 j;

    this->unk_744 |= 1;

    if (this->unk_740 <= 70) {
        for (i = 0; i < ARRAY_COUNT(this->unk_168[0]); i++) {
            for (j = 0; j < ARRAY_COUNT(this->unk_168); j++) {
                this->unk_168[j][i].unk_0C.y += -0.8f;

                this->unk_168[j][i].unk_00.x += this->unk_168[j][i].unk_0C.x;
                this->unk_168[j][i].unk_00.y += this->unk_168[j][i].unk_0C.y;
                this->unk_168[j][i].unk_00.z += this->unk_168[j][i].unk_0C.z;

                this->unk_168[j][i].unk_18.x += this->unk_168[j][i].unk_1E.x;
                this->unk_168[j][i].unk_18.y += this->unk_168[j][i].unk_1E.y;
                this->unk_168[j][i].unk_18.z += this->unk_168[j][i].unk_1E.z;
            }
        }

    } else if (this->unk_740 >= 70) {
        Actor_Kill(&this->dyna.actor);
    }
    this->unk_740++;
}

void func_80C082CC(ObjUsiyane* this, PlayState* play) {
    this->cueId = -1;
}

void func_80C082E0(ObjUsiyane* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_552)) {
        this->cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_552);
        cue = play->csCtx.actorCues[this->cueChannel];
        if (this->cueId != cue->id) {
            this->cueId = cue->id;
            if (this->cueId == 2) {
                func_80C07F30(this, play);
                this->actionFunc = func_80C081C8;
            }
        }
    }
}

void ObjUsiyane_Init(Actor* thisx, PlayState* play) {
    ObjUsiyane* this = (ObjUsiyane*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    Actor_SetScale(&this->dyna.actor, 0.1f);

    this->unk_744 = 0;

    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_obj_usiyane_Colheader_0022AC);

    switch (OBJUSIYANE_GET_F(&this->dyna.actor)) {
        case OBJUSIYANE_F_0:
            this->actionFunc = func_80C07DC4;
            break;

        case OBJUSIYANE_F_1:
            func_80C082CC(this, play);
            this->actionFunc = func_80C082E0;
            break;

        default:
            Actor_Kill(&this->dyna.actor);
            break;
    }
}

void ObjUsiyane_Destroy(Actor* thisx, PlayState* play) {
    ObjUsiyane* this = (ObjUsiyane*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjUsiyane_Update(Actor* thisx, PlayState* play) {
    ObjUsiyane* this = (ObjUsiyane*)thisx;

    this->actionFunc(this, play);
}

void ObjUsiyane_Draw(Actor* thisx, PlayState* play) {
    ObjUsiyane* this = (ObjUsiyane*)thisx;
    MtxF mf;

    if (!(this->unk_744 & 1)) {
        Gfx_DrawDListOpa(play, object_obj_usiyane_DL_000838);
    }

    if (!(this->unk_744 & 2)) {
        s32 i;

        for (i = 0; i < ARRAY_COUNT(D_80C08660); i++) {
            Vec3f sp74;

            Matrix_MultVec3f(&D_80C08660[i].pos, &this->unk_710[i]);
            Matrix_MultVec3f(&gZeroVec3f, &sp74);
            this->unk_708 = sp74.x;
            this->unk_70C = sp74.z;
        }
        this->unk_744 |= 2;
    }

    if ((this->unk_744 & 4) && !(this->unk_744 & 8)) {
        s32 i;
        s32 j;

        for (i = 0; i < ARRAY_COUNT(this->unk_168[0]); i++) {
            for (j = 0; j < ARRAY_COUNT(this->unk_168); j++) {
                Matrix_Push();
                SkinMatrix_SetScaleRotateRPYTranslate(&mf, 0.1f, 0.1f, 0.1f, this->unk_168[j][i].unk_18.x,
                                                      this->unk_168[j][i].unk_18.y, this->unk_168[j][i].unk_18.z,
                                                      this->unk_168[j][i].unk_00.x, this->unk_168[j][i].unk_00.y,
                                                      this->unk_168[j][i].unk_00.z);
                Matrix_Put(&mf);
                Gfx_DrawDListOpa(play, object_obj_usiyane_DL_000098);
                Matrix_Pop();
            }
        }
    }
}
