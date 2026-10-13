/*
 * File: z_obj_rotlift.c
 * Overlay: ovl_Obj_Rotlift
 * Description: Deku Moon Dungeon - Spiked Rotating Platforms
 */

#include "z_obj_rotlift.h"
#include "objects/object_rotlift/object_rotlift.h"
#include "overlays/actors/ovl_Obj_Etcetera/z_obj_etcetera.h"

#define FLAGS 0x00000000

void ObjRotlift_Init(Actor* thisx, PlayState* play2);
void ObjRotlift_Destroy(Actor* thisx, PlayState* play);
void ObjRotlift_Update(Actor* thisx, PlayState* play);
void ObjRotlift_Draw(Actor* thisx, PlayState* play);

void ObjRotlift_MoveDekuFlowers(ObjRotlift* this);

typedef struct ObjRotliftModelInfo {
    /* 0x0 */ Gfx* dList;
    /* 0x4 */ AnimatedMaterial* animMat;
    /* 0x8 */ CollisionHeader* colHeader;
} ObjRotliftModelInfo; // size = 0xC

ActorProfile Obj_Rotlift_Profile = {
    /**/ ACTOR_OBJ_ROTLIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_ROTLIFT,
    /**/ sizeof(ObjRotlift),
    /**/ ObjRotlift_Init,
    /**/ ObjRotlift_Destroy,
    /**/ ObjRotlift_Update,
    /**/ ObjRotlift_Draw,
};

struct ObjRotliftModelInfo sModelInfo[] = {
    {
        gDekuMoonDungeonRotatingPlatformsDL,
        gDekuMoonDungeonRotatingPlatformsUnusedTexAnim,
        &gDekuMoonDungeonRotatingPlatformsCol,
    },
    {
        gDekuMoonDungeonRotatingSpikesDL,
        gDekuMoonDungeonRotatingSpikesUnusedTexAnim,
        &gDekuMoonDungeonRotatingSpikesCol,
    },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
};

void ObjRotlift_MoveDekuFlowers(ObjRotlift* this) {
    ObjEtcetera** dekuFlower = this->dekuFlowers;
    ObjEtcetera* curDekuFlower;
    f32 posOffset = 300.0f;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->dekuFlowers); i++, dekuFlower++) {
        curDekuFlower = *dekuFlower;

        if (curDekuFlower->dyna.actor.update == NULL) {
            *dekuFlower = NULL;
        } else {
            curDekuFlower->dyna.actor.world.pos.x =
                this->dyna.actor.world.pos.x + posOffset * Math_SinS(this->dyna.actor.shape.rot.y);
            curDekuFlower->dyna.actor.world.pos.y = this->dyna.actor.world.pos.y + 380.0f * this->dyna.actor.scale.y;
            curDekuFlower->dyna.actor.world.pos.z =
                this->dyna.actor.world.pos.z + posOffset * Math_CosS(this->dyna.actor.shape.rot.y);
            curDekuFlower->dyna.actor.shape.rot.y = this->dyna.actor.shape.rot.y;
        }

        posOffset -= 600.0f;
    }
}

void ObjRotlift_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjRotlift* this = (ObjRotlift*)thisx;
    s32 type = OBJROTLIFT_GET_TYPE(&this->dyna.actor);
    s32 dekuFlowerParams;
    s32 i;
    ObjRotliftModelInfo* modelInfo;
    ObjEtcetera** dekuFlowers;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    if (type == OBJROTLIFT_TYPE_PLATFORMS) {
        for (dekuFlowers = this->dekuFlowers, i = 0; i < ARRAY_COUNT(this->dekuFlowers); i++, dekuFlowers++) {
            // Depending on the params, the platforms can be configured in one of two ways:
            // 1. Pink Deku Flower, Pink Deku Flower
            // 2. Gold Deku Flower, Pink Deku Flower
            if (!OBJROTLIFT_FIRST_DEKU_FLOWER_IS_GOLD(thisx) || (i != 0)) {
                dekuFlowerParams = DEKU_FLOWER_PARAMS(DEKU_FLOWER_TYPE_PINK);
            } else {
                dekuFlowerParams = DEKU_FLOWER_PARAMS(DEKU_FLOWER_TYPE_GOLD);
            }

            *dekuFlowers = (ObjEtcetera*)Actor_SpawnAsChild(
                &play->actorCtx, &this->dyna.actor, play, ACTOR_OBJ_ETCETERA, this->dyna.actor.world.pos.x,
                this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, this->dyna.actor.shape.rot.x,
                this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z, dekuFlowerParams);
        }

        ObjRotlift_MoveDekuFlowers(this);
    }

    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);

    modelInfo = &sModelInfo[type];
    DynaPolyActor_LoadMesh(play, &this->dyna, modelInfo->colHeader);
    modelInfo->animMat = Lib_SegmentedToVirtual(modelInfo->animMat);
}

void ObjRotlift_Destroy(Actor* thisx, PlayState* play) {
    ObjRotlift* this = (ObjRotlift*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjRotlift_Update(Actor* thisx, PlayState* play) {
    ObjRotlift* this = (ObjRotlift*)thisx;
    s16 angShift;
    s32 angVelocity;

    if (OBJROTLIFT_GET_TYPE(&this->dyna.actor) == OBJROTLIFT_TYPE_PLATFORMS) {
        ObjRotlift_MoveDekuFlowers(this);
    }

    if (thisx->params >= 0) {
        angVelocity = -0xC8;
    } else {
        angVelocity = 0xC8;
    }

    angShift = angVelocity;
    this->dyna.actor.shape.rot.y += angShift;
}

void ObjRotlift_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjRotlift* this = (ObjRotlift*)thisx;
    ObjRotliftModelInfo* modelInfo = &sModelInfo[OBJROTLIFT_GET_TYPE(&this->dyna.actor)];

    // Neither of the displaylists reference other segments, so this call is ultimately pointless.
    AnimatedMat_Draw(play, modelInfo->animMat);
    Gfx_DrawDListOpa(play, modelInfo->dList);
}
