/*
 * File: z_obj_ending.c
 * Overlay: ovl_Obj_Ending
 * Description: The stump and lighting at the end of the credits
 */

#include "z_obj_ending.h"
#include "objects/object_ending_obj/object_ending_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjEnding_Init(Actor* thisx, PlayState* play);
void ObjEnding_Update(Actor* thisx, PlayState* play);
void ObjEnding_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Ending_Profile = {
    /**/ ACTOR_OBJ_ENDING,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_ENDING_OBJ,
    /**/ sizeof(ObjEnding),
    /**/ ObjEnding_Init,
    /**/ Actor_Noop,
    /**/ ObjEnding_Update,
    /**/ ObjEnding_Draw,
};

static ObjEndingModelInfo sModelInfo[] = {
    { { object_ending_obj_DL_003440, object_ending_obj_DL_0031A0 }, NULL },
    { { NULL, object_ending_obj_DL_0003D0 }, object_ending_obj_Matanimheader_001FF8 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void ObjEnding_Init(Actor* thisx, PlayState* play) {
    ObjEnding* this = (ObjEnding*)thisx;
    AnimatedMaterial* animMat;

    Actor_ProcessInitChain(thisx, sInitChain);
    this->modelInfo = &sModelInfo[thisx->params];
    animMat = this->modelInfo->animMat;
    if (animMat != NULL) {
        this->animMat = Lib_SegmentedToVirtual(animMat);
    }
}

void ObjEnding_Update(Actor* thisx, PlayState* play) {
}

void ObjEnding_Draw(Actor* thisx, PlayState* play) {
    ObjEnding* this = (ObjEnding*)thisx;
    Gfx* dl1;
    Gfx* dl2;

    if (this->animMat != NULL) {
        AnimatedMat_Draw(play, this->animMat);
    }
    dl1 = this->modelInfo->dLists[0];
    if (dl1 != NULL) {
        Gfx_DrawDListOpa(play, dl1);
    }
    dl2 = this->modelInfo->dLists[1];
    if (dl2 != NULL) {
        Gfx_DrawDListXlu(play, dl2);
    }
}
