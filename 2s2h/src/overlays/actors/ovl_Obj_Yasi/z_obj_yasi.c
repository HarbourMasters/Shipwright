/*
 * File: z_obj_yasi.c
 * Overlay: ovl_Obj_Yasi
 * Description: Palm Tree
 */

#include "z_obj_yasi.h"
#include "objects/object_obj_yasi/object_obj_yasi.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS 0x00000000

#define CAN_DROP_NUT(thisx) (thisx->params < 0)

void ObjYasi_Init(Actor* thisx, PlayState* play);
void ObjYasi_Destroy(Actor* thisx, PlayState* play);
void ObjYasi_Update(Actor* thisx, PlayState* play);
void ObjYasi_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Yasi_Profile = {
    /**/ ACTOR_OBJ_YASI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_OBJ_YASI,
    /**/ sizeof(ObjYasi),
    /**/ ObjYasi_Init,
    /**/ ObjYasi_Destroy,
    /**/ ObjYasi_Update,
    /**/ ObjYasi_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
};

void ObjYasi_Init(Actor* thisx, PlayState* play) {
    ObjYasi* this = (ObjYasi*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gPalmTreeCol);

    this->dyna.actor.home.rot.y = 0;

    if (PALM_TREE_IS_WIDE(thisx)) {
        this->dyna.actor.scale.x = 0.2f;
        this->dyna.actor.scale.z = 0.2f;
    }
}

void ObjYasi_Destroy(Actor* thisx, PlayState* play) {
    ObjYasi* this = (ObjYasi*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjYasi_Update(Actor* thisx, PlayState* play) {
    ObjYasi* this = (ObjYasi*)thisx;
    s16 temp;
    Vec3f dropPos;

    if (this->dyna.actor.home.rot.z != 0) {
        if (GameInteractor_Should(VB_TREE_DROP_COLLECTIBLE, CAN_DROP_NUT(thisx), thisx)) {
            if (Rand_ZeroOne() < 0.5f) {
                dropPos.x = this->dyna.actor.world.pos.x;
                dropPos.y = this->dyna.actor.world.pos.y + 280.0f;
                dropPos.z = this->dyna.actor.world.pos.z;
                Item_DropCollectible(play, &dropPos, ITEM00_DEKU_NUTS_1);
            }
        }
        this->dyna.actor.home.rot.y = GET_PLAYER(play)->actor.shape.rot.y;
        this->dyna.actor.home.rot.x = 400;
        this->dyna.actor.home.rot.z = 0;
    }
    temp = TRUNCF_BINANG(this->dyna.actor.shape.rot.x * 0.1f);
    this->dyna.actor.home.rot.x -= temp;
    this->dyna.actor.shape.rot.x +=
        BINANG_SUB(this->dyna.actor.home.rot.x, TRUNCF_BINANG(this->dyna.actor.shape.rot.x * 0.08f));
}

void ObjYasi_Draw(Actor* thisx, PlayState* play) {
    ObjYasi* this = (ObjYasi*)thisx;

    Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);

    if (this->dyna.actor.shape.rot.x != 0) {
        Matrix_RotateYS(this->dyna.actor.home.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->dyna.actor.shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateYS(BINANG_SUB(this->dyna.actor.shape.rot.y, this->dyna.actor.home.rot.y), MTXMODE_APPLY);
    } else {
        Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_APPLY);
    }

    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Gfx_DrawDListOpa(play, gPalmTreeDL);
}
