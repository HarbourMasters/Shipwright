/*
 * File: z_en_test2.c
 * Overlay: ovl_En_Test2
 * Description: Objects affected by the Lens of Truth
 */

#include "z_en_test2.h"
#include "objects/object_dekucity_ana_obj/object_dekucity_ana_obj.h"
#include "objects/object_sichitai_obj/object_sichitai_obj.h"
#include "objects/object_yukimura_obj/object_yukimura_obj.h"
#include "objects/object_hakugin_obj/object_hakugin_obj.h"
#include "objects/object_meganeana_obj/object_meganeana_obj.h"
#include "objects/object_haka_obj/object_haka_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_REACT_TO_LENS)

void EnTest2_Init(Actor* thisx, PlayState* play);
void EnTest2_Update(Actor* thisx, PlayState* play);
void EnTest2_UpdateForLens(Actor* thisx, PlayState* play);
void EnTest2_Draw(Actor* thisx, PlayState* play);

typedef struct EnTest2ModelInfo {
    /* 0x0 */ Gfx* dList1;
    /* 0x4 */ Gfx* dList2;
    /* 0x8 */ AnimatedMaterial* animMat;
} EnTest2ModelInfo; // size = 0xC

ActorProfile En_Test2_Profile = {
    /**/ ACTOR_EN_TEST2,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnTest2),
    /**/ EnTest2_Init,
    /**/ Actor_Noop,
    /**/ EnTest2_Update,
    /**/ NULL,
};

static EnTest2ModelInfo sModelInfo[EN_TEST2_TYPE_MAX] = {
    { object_dekucity_ana_obj_DL_000040, NULL, NULL },                               // EN_TEST2_TYPE_0
    { object_sichitai_obj_DL_001820, NULL, NULL },                                   // EN_TEST2_TYPE_1
    { object_yukimura_obj_DL_0008C0, NULL, NULL },                                   // EN_TEST2_TYPE_2
    { object_hakugin_obj_DL_0016D8, NULL, object_hakugin_obj_Matanimheader_0017A8 }, // EN_TEST2_TYPE_3
    { object_hakugin_obj_DL_002018, NULL, object_hakugin_obj_Matanimheader_0020E8 }, // EN_TEST2_TYPE_4
    { object_hakugin_obj_DL_005268, NULL, object_hakugin_obj_Matanimheader_005338 }, // EN_TEST2_TYPE_5
    { object_meganeana_obj_DL_000110, object_meganeana_obj_DL_000080, NULL },        // EN_TEST2_TYPE_6
    { object_haka_obj_DL_000F70, NULL, NULL },                                       // EN_TEST2_TYPE_7
    { object_haka_obj_DL_001200, NULL, NULL },                                       // EN_TEST2_TYPE_8
    { object_hakugin_obj_DL_004928, NULL, object_hakugin_obj_Matanimheader_0049F8 }, // EN_TEST2_TYPE_9
    { object_hakugin_obj_DL_002978, NULL, object_hakugin_obj_Matanimheader_002A58 }, // EN_TEST2_TYPE_10
    { object_hakugin_obj_DL_000D38, NULL, object_hakugin_obj_Matanimheader_000E48 }, // EN_TEST2_TYPE_11
    { object_hakugin_obj_DL_0035A8, NULL, object_hakugin_obj_Matanimheader_003888 }, // EN_TEST2_TYPE_12
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 8000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2500, ICHAIN_STOP),
};

static s16 sObjectIds[EN_TEST2_TYPE_MAX] = {
    OBJECT_DEKUCITY_ANA_OBJ, // EN_TEST2_TYPE_0
    OBJECT_SICHITAI_OBJ,     // EN_TEST2_TYPE_1
    OBJECT_YUKIMURA_OBJ,     // EN_TEST2_TYPE_2
    OBJECT_HAKUGIN_OBJ,      // EN_TEST2_TYPE_3
    OBJECT_HAKUGIN_OBJ,      // EN_TEST2_TYPE_4
    OBJECT_HAKUGIN_OBJ,      // EN_TEST2_TYPE_5
    OBJECT_MEGANEANA_OBJ,    // EN_TEST2_TYPE_6
    OBJECT_HAKA_OBJ,         // EN_TEST2_TYPE_7
    OBJECT_HAKA_OBJ,         // EN_TEST2_TYPE_8
    OBJECT_HAKUGIN_OBJ,      // EN_TEST2_TYPE_9
    OBJECT_HAKUGIN_OBJ,      // EN_TEST2_TYPE_10
    OBJECT_HAKUGIN_OBJ,      // EN_TEST2_TYPE_11
    OBJECT_HAKUGIN_OBJ,      // EN_TEST2_TYPE_12
};

void EnTest2_Init(Actor* thisx, PlayState* play) {
    EnTest2* this = (EnTest2*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    if ((ENTEST2_GET_TYPE(&this->actor) == EN_TEST2_TYPE_11) || (ENTEST2_GET_TYPE(&this->actor) == EN_TEST2_TYPE_12)) {
        this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
    }
}

void EnTest2_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    s32 objectSlot;
    EnTest2ModelInfo* modelInfo;
    EnTest2* this = (EnTest2*)thisx;

    objectSlot = Object_GetSlot(&play->objectCtx, sObjectIds[ENTEST2_GET_TYPE(&this->actor)]);
    if (objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    if (Object_IsLoaded(&play->objectCtx, objectSlot)) {
        modelInfo = &sModelInfo[ENTEST2_GET_TYPE(&this->actor)];
        this->actor.objectSlot = objectSlot;
        this->actor.draw = EnTest2_Draw;

        if (modelInfo->animMat != NULL) {
            Actor_SetObjectDependency(play, &this->actor);
            this->animMat = Lib_SegmentedToVirtual(modelInfo->animMat);
        }

        if (play->roomCtx.curRoom.lensMode != LENS_MODE_SHOW_ACTORS) {
            this->actor.update = EnTest2_UpdateForLens;
        } else {
            this->actor.update = Actor_Noop;
        }
    }
}

void EnTest2_UpdateForLens(Actor* thisx, PlayState* play) {
    EnTest2* this = (EnTest2*)thisx;

    if (play->actorCtx.lensMaskSize == LENS_MASK_ACTIVE_SIZE) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
    }
}

void EnTest2_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTest2* this = (EnTest2*)thisx;
    Gfx* dList = sModelInfo[ENTEST2_GET_TYPE(&this->actor)].dList2;

    if (this->animMat != NULL) {
        AnimatedMat_Draw(play, this->animMat);
    }

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

        if (dList != NULL) {
            gSPDisplayList(POLY_XLU_DISP++, dList);
        }
        gSPDisplayList(POLY_XLU_DISP++, sModelInfo[ENTEST2_GET_TYPE(&this->actor)].dList1);

        CLOSE_DISPS(play->state.gfxCtx);
    } else {
        if (dList != NULL) {
            Gfx_DrawDListXlu(play, dList);
        }
        Gfx_DrawDListOpa(play, sModelInfo[ENTEST2_GET_TYPE(&this->actor)].dList1);
    }
}
