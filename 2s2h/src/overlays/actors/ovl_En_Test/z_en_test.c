/*
 * File: z_en_test.c
 * Overlay: ovl_En_Test
 * Description: Various crater marks (e.g., Goron Link punch/pound, Moon's Tear crash, etc.)
 */

#include "z_en_test.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnTest_Init(Actor* thisx, PlayState* play2);
void EnTest_Destroy(Actor* thisx, PlayState* play);
void EnTest_Update(Actor* thisx, PlayState* play);
void EnTest_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Test_Profile = {
    /**/ ACTOR_EN_TEST,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnTest),
    /**/ EnTest_Init,
    /**/ EnTest_Destroy,
    /**/ EnTest_Update,
    /**/ EnTest_Draw,
};

void func_80862B70(EnTestStruct* arg0) {
    s32 i;

    for (i = 0; i < 20; i++) {
        EnTestStruct* ptr = &arg0[i];

        ptr->unk_00 = false;
        ptr->unk_04 = 0;
        ptr->unk_08.x = 0.0f;
        ptr->unk_08.y = 0.0f;
        ptr->unk_08.z = 0.0f;
        ptr->unk_14.x = 0.0f;
        ptr->unk_14.y = 0.0f;
        ptr->unk_14.z = 0.0f;
        ptr->unk_20.x = 0.0f;
        ptr->unk_20.y = 0.0f;
        ptr->unk_20.z = 0.0f;
        ptr->unk_2C = 0.001f;
        ptr->unk_30.x = 0;
        ptr->unk_30.y = 0;
        ptr->unk_30.z = 0;
        ptr->unk_36.x = 0;
        ptr->unk_36.y = 0;
        ptr->unk_36.z = 0;
    }
}

void func_80862CBC(EnTestStruct* arg0, Vec3f* arg1) {
    EnTestStruct* ptr;
    s32 i;

    for (i = 0; i < 20; i++) {
        ptr = &arg0[i];

        if (!ptr->unk_00) {
            s16 sp26 = Rand_ZeroOne() * 0xFFFF;
            f32 sp20;

            ptr->unk_00 = true;

            ptr->unk_08.x = (Math_CosS(sp26) * 20.0f) + arg1->x;
            ptr->unk_08.y = arg1->y;
            ptr->unk_08.z = (Math_SinS(sp26) * 20.0f) + arg1->z;

            sp20 = Rand_ZeroOne();

            ptr->unk_14.x = Math_CosS(sp26) * 13.0f * sp20;
            ptr->unk_14.y = (Rand_ZeroOne() * 5.0f) + 8.0f;
            ptr->unk_14.z = (Math_SinS(sp26) * 13.0f) * sp20;

            ptr->unk_20.x = 0.0f;
            ptr->unk_20.z = 0.0f;
            ptr->unk_20.y = -2.0f;

            ptr->unk_2C = (Rand_ZeroOne() * 0.0004f) + 0.0004f;

            ptr->unk_30.x = Rand_ZeroOne() * 0x7FFE;
            ptr->unk_30.y = Rand_ZeroOne() * 0x7FFE;
            ptr->unk_30.z = Rand_ZeroOne() * 0x7FFE;

            ptr->unk_36.x = Rand_ZeroOne() * 0x2000;
            ptr->unk_36.y = Rand_ZeroOne() * 0x2000;
            ptr->unk_36.z = Rand_ZeroOne() * 0x2000;

            ptr->unk_04 = 10;
            break;
        }
    }
}

void func_80862EDC(EnTestStruct* arg0) {
    s32 i;
    EnTestStruct* ptr;

    for (i = 0; i < 20; i++) {
        ptr = &arg0[i];

        if (ptr->unk_00) {
            ptr->unk_04--;
            if (ptr->unk_04 < 0) {
                ptr->unk_00 = false;
            }

            ptr->unk_08.x += ptr->unk_14.x;
            ptr->unk_08.y += ptr->unk_14.y;
            ptr->unk_08.z += ptr->unk_14.z;

            ptr->unk_14.x += ptr->unk_20.x;
            ptr->unk_14.y += ptr->unk_20.y;
            ptr->unk_14.z += ptr->unk_20.z;

            ptr->unk_30.x += ptr->unk_36.x;
            ptr->unk_30.y += ptr->unk_36.y;
            ptr->unk_30.z += ptr->unk_36.z;
        }
    }
}

void func_80863048(PlayState* play, EnTestStruct* arg1) {
    EnTestStruct* ptr;
    s32 i;
    Mtx* mtx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);

    for (i = 0; i < 20; i++) {
        ptr = &arg1[i];

        if (!ptr->unk_00) {
            continue;
        }

        Matrix_Translate(ptr->unk_08.x, ptr->unk_08.y, ptr->unk_08.z, MTXMODE_NEW);
        Matrix_RotateZYX(ptr->unk_30.x, ptr->unk_30.y, ptr->unk_30.z, MTXMODE_APPLY);
        Matrix_Scale(ptr->unk_2C, ptr->unk_2C, ptr->unk_2C, MTXMODE_APPLY);

        mtx = Matrix_Finalize(play->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gameplay_keep_DL_06AB30);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnTest_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTest* this = (EnTest*)thisx;
    MtxF sp38;
    s32 bgId;

    this->unk_209 = 0;
    this->unk_174 = 0;

    if (thisx->params > 0) {
        Actor_SetScale(thisx, thisx->params / 100000.0f);
        this->surfaceMaterial = SURFACE_MATERIAL_DIRT;
    } else {
        thisx->floorPoly = NULL;
        thisx->world.pos.y += 10.0f;
        thisx->floorHeight = BgCheck_EntityRaycastFloor3(&play->colCtx, &thisx->floorPoly, &bgId, &thisx->world.pos);

        if ((thisx->floorPoly == NULL) || (thisx->floorHeight == BGCHECK_Y_MIN)) {
            Actor_Kill(thisx);
            return;
        }

        thisx->world.pos.y = thisx->floorHeight;
        func_800C0094(thisx->floorPoly, thisx->world.pos.x, thisx->floorHeight, thisx->world.pos.z, &sp38);
        Matrix_MtxFToYXZRot(&sp38, &thisx->shape.rot, true);
        thisx->world.rot = thisx->shape.rot;
        this->surfaceMaterial = SurfaceType_GetMaterial(&play->colCtx, thisx->floorPoly, bgId);
    }

    Keyframe_InitFlex(&this->kfSkelAnime, gGameplayKeepKFSkel_6EB70, gGameplayKeepKFAnim_6BB0C, this->jointTable,
                      this->morphTable, NULL);
    Keyframe_FlexPlayOnce(&this->kfSkelAnime, gGameplayKeepKFAnim_6BB0C);
    this->kfSkelAnime.frameCtrl.curTime = 9.0f;
    func_80862B70(this->unk_20C);
}

void EnTest_Destroy(Actor* thisx, PlayState* play) {
    EnTest* this = (EnTest*)thisx;

    Keyframe_DestroyFlex(&this->kfSkelAnime);
}

void EnTest_Update(Actor* thisx, PlayState* play) {
    EnTest* this = (EnTest*)thisx;
    s32 i;

    this->unk_208 = this->kfSkelAnime.frameCtrl.curTime;

    if (Keyframe_UpdateFlex(&this->kfSkelAnime) && (this->actor.parent == NULL) && (this->actor.params != -1)) {
        this->unk_209++;
        if (this->unk_209 > 20) {
            Actor_Kill(&this->actor);
        }
    }

    if ((this->actor.params != -1) && (this->unk_208 >= 2)) {
        if (!(this->unk_174 & 1)) {
            this->unk_174 |= 1;

            for (i = 0; i < ARRAY_COUNT(this->unk_20C); i++) {
                func_80862CBC(this->unk_20C, &this->actor.world.pos);
            }
        }
    }
    func_80862EDC(this->unk_20C);
}

s32 EnTest_OverrideLimbDraw(PlayState* play, KFSkelAnimeFlex* kfSkelAnime, s32 limbIndex, Gfx** dList, u8* flags,
                            void* thisx, Vec3f* scale, Vec3s* rot, Vec3f* pos) {
    EnTest* this = (EnTest*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if ((this->actor.params == -1) && ((limbIndex == 1) || (limbIndex == 2))) {
        *dList = NULL;
    }

    if ((this->actor.params == -2) && (limbIndex == 3)) {
        *dList = NULL;
    }

    if (limbIndex == 3) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 0, 0, 0, ((20 - this->unk_209) * 255.0f) / 20.0f);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, ((20 - this->unk_209) * 255.0f) / 20.0f);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    return true;
}

void EnTest_Draw(Actor* thisx, PlayState* play) {
    EnTest* this = (EnTest*)thisx;
    Mtx* mtxStack;
    s32 sp2C = this->unk_208 - 1;

    if (sp2C >= 29) {
        sp2C = 29;
    }

    //! @bug Checks for non-existent SURFACE_MATERIAL_MAX material
    if ((this->surfaceMaterial == SURFACE_MATERIAL_MAX) || (this->surfaceMaterial == SURFACE_MATERIAL_SNOW)) {
        AnimatedMat_DrawStep(play, Lib_SegmentedToVirtual(gameplay_keep_Matanimheader_06B730), sp2C);
    } else {
        AnimatedMat_DrawStep(play, Lib_SegmentedToVirtual(gameplay_keep_Matanimheader_06B6A0), sp2C);
    }

    mtxStack = GRAPH_ALLOC(play->state.gfxCtx, this->kfSkelAnime.skeleton->dListCount * sizeof(Mtx));

    if (mtxStack != NULL) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Keyframe_DrawFlex(play, &this->kfSkelAnime, mtxStack, EnTest_OverrideLimbDraw, NULL, thisx);
        func_80863048(play, this->unk_20C);
    }
}
