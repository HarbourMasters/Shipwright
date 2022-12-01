/*
 * File: z_bg_spot09_obj.c
 * Overlay: ovl_Bg_Spot09_Obj
 * Description:
 */

#include "z_bg_spot09_obj.h"
#include "objects/object_spot09_obj/object_spot09_obj.h"

#define FLAGS 0

void BgSpot09Obj_Init(Actor* thisx, PlayState* play);
void BgSpot09Obj_Destroy(Actor* thisx, PlayState* play);
void BgSpot09Obj_Update(Actor* thisx, PlayState* play);
void BgSpot09Obj_Draw(Actor* thisx, PlayState* play);

s32 func_808B1AE0(BgSpot09Obj* this, PlayState* play);
s32 func_808B1BA0(BgSpot09Obj* this, PlayState* play);
s32 func_808B1BEC(BgSpot09Obj* this, PlayState* play);

const ActorInit Bg_Spot09_Obj_InitVars = {
    ACTOR_BG_SPOT09_OBJ,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT09_OBJ,
    sizeof(BgSpot09Obj),
    (ActorFunc)BgSpot09Obj_Init,
    (ActorFunc)BgSpot09Obj_Destroy,
    (ActorFunc)BgSpot09Obj_Update,
    (ActorFunc)BgSpot09Obj_Draw,
    NULL,
};

static CollisionHeader* D_808B1F90[] = {
    NULL, &gValleyObjects1Col, &gValleyObjects2Col, &gValleyObjects3Col, &gValleyObjects4Col,
};

static s32 (*D_808B1FA4[])(BgSpot09Obj* this, PlayState* play) = {
    func_808B1BEC,
    func_808B1AE0,
    func_808B1BA0,
};

static InitChainEntry sInitChain1[] = {
    ICHAIN_F32(uncullZoneForward, 7200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 7200, ICHAIN_STOP),
};

static InitChainEntry sInitChain2[] = {
    ICHAIN_F32(uncullZoneForward, 7200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1500, ICHAIN_STOP),
};

static Gfx* sDLists[] = {
    gValleyBridgeSidesDL, gValleyBrokenBridgeDL, gValleyBridgeChildDL, gCarpentersTentDL, gValleyRepairedBridgeDL,
};

s32 func_808B1AE0(BgSpot09Obj* this, PlayState* play) {
    s32 carpentersRescued;

    if (gSaveContext.sceneSetupIndex >= 4) {
        return this->dyna.actor.params == 0;
    }

    carpentersRescued = (gSaveContext.eventChkInf[9] & 0xF) == 0xF;

    if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
        switch (this->dyna.actor.params) {
            case 0:
                return 0;
            case 1:
                return !carpentersRescued;
            case 4:
                return carpentersRescued;
            case 3:
                return 1;
        }
    } else {
        return this->dyna.actor.params == 2;
    }

    return 0;
}

s32 func_808B1BA0(BgSpot09Obj* this, PlayState* play) {
    if (this->dyna.actor.params == 3) {
        Actor_SetScale(&this->dyna.actor, 0.1f);
    } else {
        Actor_SetScale(&this->dyna.actor, 1.0f);
    }
    return 1;
}

s32 func_808B1BEC(BgSpot09Obj* this, PlayState* play) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2[2];

    if (D_808B1F90[this->dyna.actor.params] != NULL) {
        DynaPolyActor_Init(&this->dyna, DPM_UNK);
        CollisionHeader_GetVirtual(D_808B1F90[this->dyna.actor.params], &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    }
    return true;
}

s32 func_808B1C70(BgSpot09Obj* this, PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(D_808B1FA4); i++) {
        if (!D_808B1FA4[i](this, play)) {
            return false;
        }
    }
    return true;
}

s32 func_808B1CEC(BgSpot09Obj* this, PlayState* play) {
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain1);
    return true;
}

s32 func_808B1D18(BgSpot09Obj* this, PlayState* play) {
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain2);
    return true;
}

s32 func_808B1D44(BgSpot09Obj* this, PlayState* play) {
    if (this->dyna.actor.params == 3) {
        return func_808B1D18(this, play);
    } else {
        return func_808B1CEC(this, play);
    }
}

void BgSpot09Obj_Init(Actor* thisx, PlayState* play) {
    BgSpot09Obj* this = (BgSpot09Obj*)thisx;

    osSyncPrintf("Spot09 Object [arg_data : 0x%04x](大工救出フラグ 0x%x)\n", this->dyna.actor.params,
                 gSaveContext.eventChkInf[9] & 0xF);
    this->dyna.actor.params &= 0xFF;
    if ((this->dyna.actor.params < 0) || (this->dyna.actor.params >= 5)) {
        osSyncPrintf("Error : Spot 09 object の arg_data が判別出来ない(%s %d)(arg_data 0x%04x)\n",
                     __FILE__, __LINE__, this->dyna.actor.params);
    }

    if (!func_808B1C70(this, play)) {
        Actor_Kill(&this->dyna.actor);
    } else if (!func_808B1D44(this, play)) {
        Actor_Kill(&this->dyna.actor);
    }
}

void BgSpot09Obj_Destroy(Actor* thisx, PlayState* play) {
    DynaCollisionContext* dynaColCtx = &play->colCtx.dyna;
    BgSpot09Obj* this = (BgSpot09Obj*)thisx;

    if (this->dyna.actor.params != 0) {
        DynaPoly_DeleteBgActor(play, dynaColCtx, this->dyna.bgId);
    }
}

void BgSpot09Obj_Update(Actor* thisx, PlayState* play) {
}

void BgSpot09Obj_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, sDLists[thisx->params]);

    if (thisx->params == 3) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL_25Xlu(play->state.gfxCtx);

        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gCarpentersTentEntranceDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
