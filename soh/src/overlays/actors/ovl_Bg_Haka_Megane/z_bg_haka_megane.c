/*
 * File: z_bg_haka_megane.c
 * Overlay: ovl_Bg_Haka_Megane
 * Description: Shadow Temple Fake Walls
 */

#include "z_bg_haka_megane.h"
#include "objects/object_hakach_objects/object_hakach_objects.h"
#include "objects/object_haka_objects/object_haka_objects.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_7)

void BgHakaMegane_Init(Actor* thisx, PlayState* play);
void BgHakaMegane_Destroy(Actor* thisx, PlayState* play);
void BgHakaMegane_Update(Actor* thisx, PlayState* play);
void BgHakaMegane_Draw(Actor* thisx, PlayState* play);

void func_8087DB24(BgHakaMegane* this, PlayState* play);
void func_8087DBF0(BgHakaMegane* this, PlayState* play);
void BgHakaMegane_DoNothing(BgHakaMegane* this, PlayState* play);

const ActorInit Bg_Haka_Megane_InitVars = {
    ACTOR_BG_HAKA_MEGANE,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(BgHakaMegane),
    (ActorFunc)BgHakaMegane_Init,
    (ActorFunc)BgHakaMegane_Destroy,
    (ActorFunc)BgHakaMegane_Update,
    NULL,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static CollisionHeader* sCollisionHeaders[] = {
    &gBotw1Col,
    &gBotw2Col,
    NULL,
    &object_haka_objects_Col_004330,
    &object_haka_objects_Col_0044D0,
    NULL,
    &object_haka_objects_Col_004780,
    &object_haka_objects_Col_004940,
    NULL,
    &object_haka_objects_Col_004B00,
    NULL,
    &object_haka_objects_Col_004CC0,
    NULL,
};

static Gfx* sDLists[] = {
    gBotwFakeWallsAndFloorsDL,     gBotwThreeFakeFloorsDL,        gBotwHoleTrap2DL,
    object_haka_objects_DL_0040F0, object_haka_objects_DL_0043B0, object_haka_objects_DL_001120,
    object_haka_objects_DL_0045A0, object_haka_objects_DL_0047F0, object_haka_objects_DL_0018F0,
    object_haka_objects_DL_0049B0, object_haka_objects_DL_003CF0, object_haka_objects_DL_004B70,
    object_haka_objects_DL_002ED0,
};

void BgHakaMegane_Init(Actor* thisx, PlayState* play) {
    BgHakaMegane* this = (BgHakaMegane*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);

    if (thisx->params < 3) {
        this->objBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_HAKACH_OBJECTS);
    } else {
        this->objBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_HAKA_OBJECTS);
    }

    if (this->objBankIndex < 0) {
        Actor_Kill(thisx);
    } else {
        this->actionFunc = func_8087DB24;
    }
}

void BgHakaMegane_Destroy(Actor* thisx, PlayState* play) {
    BgHakaMegane* this = (BgHakaMegane*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_8087DB24(BgHakaMegane* this, PlayState* play) {
    CollisionHeader* colHeader;
    CollisionHeader* collision;

    if (Object_IsLoaded(&play->objectCtx, this->objBankIndex)) {
        this->dyna.actor.objBankIndex = this->objBankIndex;
        this->dyna.actor.draw = BgHakaMegane_Draw;
        Actor_SetObjectDependency(play, &this->dyna.actor);
        if (play->roomCtx.curRoom.lensMode != LENS_MODE_HIDE_ACTORS) {
            this->actionFunc = func_8087DBF0;
            collision = sCollisionHeaders[this->dyna.actor.params];
            if (collision != NULL) {
                CollisionHeader_GetVirtual(collision, &colHeader);
                this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            }
        } else {
            this->actionFunc = BgHakaMegane_DoNothing;
        }
    }
}

void func_8087DBF0(BgHakaMegane* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (play->actorCtx.lensActive) {
        thisx->flags |= ACTOR_FLAG_7;
        func_8003EBF8(play, &play->colCtx.dyna, this->dyna.bgId);
    } else {
        thisx->flags &= ~ACTOR_FLAG_7;
        func_8003EC50(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void BgHakaMegane_DoNothing(BgHakaMegane* this, PlayState* play) {
}

void BgHakaMegane_Update(Actor* thisx, PlayState* play) {
    BgHakaMegane* this = (BgHakaMegane*)thisx;

    this->actionFunc(this, play);
}

void BgHakaMegane_Draw(Actor* thisx, PlayState* play) {
    BgHakaMegane* this = (BgHakaMegane*)thisx;

    if (CHECK_FLAG_ALL(thisx->flags, ACTOR_FLAG_7)) {
        Gfx_DrawDListXlu(play, sDLists[thisx->params]);
    } else {
        Gfx_DrawDListOpa(play, sDLists[thisx->params]);
    }

    if (thisx->params == 0) {
        Gfx_DrawDListXlu(play, gBotwBloodSplatterDL);
    }
}
