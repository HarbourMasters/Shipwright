/*
 * File: z_bg_hidan_fslift.c
 * Overlay: ovl_Bg_Hidan_Fslift
 * Description: Hookshot Elevator
 */

#include "z_bg_hidan_fslift.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgHidanFslift_Init(Actor* thisx, PlayState* play);
void BgHidanFslift_Destroy(Actor* thisx, PlayState* play);
void BgHidanFslift_Update(Actor* thisx, PlayState* play);
void BgHidanFslift_Draw(Actor* thisx, PlayState* play);

void func_80886FCC(BgHidanFslift* this, PlayState* play);
void func_8088706C(BgHidanFslift* this, PlayState* play);
void func_808870D8(BgHidanFslift* this, PlayState* play);

const ActorInit Bg_Hidan_Fslift_InitVars = {
    ACTOR_BG_HIDAN_FSLIFT,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanFslift),
    (ActorFunc)BgHidanFslift_Init,
    (ActorFunc)BgHidanFslift_Destroy,
    (ActorFunc)BgHidanFslift_Update,
    (ActorFunc)BgHidanFslift_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 350, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_STOP),
};

void BgHidanFslift_Init(Actor* thisx, PlayState* play) {
    s32 pad1;
    BgHidanFslift* this = (BgHidanFslift*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(&gFireTempleHookshotElevatorCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
    if (Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_OBJ_HSBLOCK,
                           this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y + 40.0f,
                           this->dyna.actor.world.pos.z + -28.0f, 0, 0, 0, 2) == NULL) {
        Actor_Kill(&this->dyna.actor);
        return;
    }
    this->actionFunc = func_80886FCC;
}

void func_80886F24(BgHidanFslift* this) {
    if (this->dyna.actor.child != NULL && this->dyna.actor.child->update != NULL) {
        this->dyna.actor.child->world.pos.x = this->dyna.actor.world.pos.x;
        this->dyna.actor.child->world.pos.y = this->dyna.actor.world.pos.y + 40.0f;
        this->dyna.actor.child->world.pos.z = this->dyna.actor.world.pos.z + -28.0f;
    } else {
        this->dyna.actor.child = NULL;
    }
}

void BgHidanFslift_Destroy(Actor* thisx, PlayState* play) {
    BgHidanFslift* this = (BgHidanFslift*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80886FB4(BgHidanFslift* this) {
    this->timer = 40;
    this->actionFunc = func_80886FCC;
}

void func_80886FCC(BgHidanFslift* this, PlayState* play) {
    s32 heightBool;

    if (this->timer) {
        this->timer--;
    }

    if (this->timer == 0) {
        heightBool = false;
        if ((this->dyna.actor.world.pos.y - this->dyna.actor.home.pos.y) < 0.5f) {
            heightBool = true;
        }
        if (func_80043590(&this->dyna) && (heightBool)) {
            this->actionFunc = func_808870D8;
        } else if (!heightBool) {
            this->actionFunc = func_8088706C;
        }
    }
}

void func_8088706C(BgHidanFslift* this, PlayState* play) {
    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 4.0f)) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        func_80886FB4(this);
    } else {
        func_8002F974(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
    func_80886F24(this);
}

void func_808870D8(BgHidanFslift* this, PlayState* play) {
    if (func_80043590(&this->dyna)) {
        if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 790.0f, 4.0f)) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
            func_80886FB4(this);
        } else {
            func_8002F974(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
        }
    } else {
        func_80886FB4(this);
    }
    func_80886F24(this);
}

void BgHidanFslift_Update(Actor* thisx, PlayState* play) {
    BgHidanFslift* this = (BgHidanFslift*)thisx;

    this->actionFunc(this, play);
    if (func_8004356C(&this->dyna)) {
        if (this->unk_16A == 0) {
            this->unk_16A = 3;
        }
        Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_FIRE_PLATFORM);
    } else if (!func_8004356C(&this->dyna)) {
        if (this->unk_16A != 0) {
            Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
        }
        this->unk_16A = 0;
    }
}

void BgHidanFslift_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gFireTempleHookshotElevatorDL);
}
