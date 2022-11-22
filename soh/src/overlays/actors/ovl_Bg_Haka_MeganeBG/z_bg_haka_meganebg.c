/*
 * File: z_bg_haka_meganebg.c
 * Overlay: ovl_Bg_Haka_MeganeBG
 * Description:
 */

#include "z_bg_haka_meganebg.h"
#include "objects/object_haka_objects/object_haka_objects.h"

#define FLAGS 0

void BgHakaMeganeBG_Init(Actor* thisx, PlayState* play);
void BgHakaMeganeBG_Destroy(Actor* thisx, PlayState* play);
void BgHakaMeganeBG_Update(Actor* thisx, PlayState* play);
void BgHakaMeganeBG_Draw(Actor* thisx, PlayState* play);

void func_8087DFF8(BgHakaMeganeBG* this, PlayState* play);
void func_8087E040(BgHakaMeganeBG* this, PlayState* play);
void func_8087E10C(BgHakaMeganeBG* this, PlayState* play);
void func_8087E1E0(BgHakaMeganeBG* this, PlayState* play);
void func_8087E258(BgHakaMeganeBG* this, PlayState* play);
void func_8087E288(BgHakaMeganeBG* this, PlayState* play);
void func_8087E2D8(BgHakaMeganeBG* this, PlayState* play);
void func_8087E34C(BgHakaMeganeBG* this, PlayState* play);

const ActorInit Bg_Haka_MeganeBG_InitVars = {
    ACTOR_BG_HAKA_MEGANEBG,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HAKA_OBJECTS,
    sizeof(BgHakaMeganeBG),
    (ActorFunc)BgHakaMeganeBG_Init,
    (ActorFunc)BgHakaMeganeBG_Destroy,
    (ActorFunc)BgHakaMeganeBG_Update,
    (ActorFunc)BgHakaMeganeBG_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

// Unused
static u32 D_8087E3FC[] = {
    0x00000000, 0x00000000, 0x00000000, 0xC8C800FF, 0xFF0000FF,
};

static Gfx* D_8087E410[] = {
    object_haka_objects_DL_008EB0,
    object_haka_objects_DL_00A1A0,
    object_haka_objects_DL_005000,
    object_haka_objects_DL_000040,
};

void BgHakaMeganeBG_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakaMeganeBG* this = (BgHakaMeganeBG*)thisx;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->unk_168 = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;

    if (thisx->params == 2) {
        DynaPolyActor_Init(&this->dyna, DPM_UNK3);
        thisx->flags |= ACTOR_FLAG_4;
        CollisionHeader_GetVirtual(&object_haka_objects_Col_005334, &colHeader);
        this->actionFunc = func_8087E258;
    } else {
        DynaPolyActor_Init(&this->dyna, DPM_PLAYER);

        if (thisx->params == 0) {
            CollisionHeader_GetVirtual(&object_haka_objects_Col_009168, &colHeader);
            thisx->flags |= ACTOR_FLAG_7;
            this->unk_16A = 20;
            this->actionFunc = func_8087DFF8;
        } else if (thisx->params == 3) {
            CollisionHeader_GetVirtual(&object_haka_objects_Col_000118, &colHeader);
            thisx->home.pos.y += 100.0f;

            if (Flags_GetSwitch(play, this->unk_168)) {
                this->actionFunc = func_8087E34C;
                thisx->world.pos.y = thisx->home.pos.y;
            } else {
                thisx->flags |= ACTOR_FLAG_4;
                this->actionFunc = func_8087E288;
            }
        } else {
            CollisionHeader_GetVirtual(&object_haka_objects_Col_00A7F4, &colHeader);
            this->unk_16A = 80;
            this->actionFunc = func_8087E10C;
            thisx->uncullZoneScale = 3000.0f;
            thisx->uncullZoneDownward = 3000.0f;
        }
    }

    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void BgHakaMeganeBG_Destroy(Actor* thisx, PlayState* play) {
    BgHakaMeganeBG* this = (BgHakaMeganeBG*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_8087DFF8(BgHakaMeganeBG* this, PlayState* play) {
    if (this->unk_16A != 0) {
        this->unk_16A--;
    }

    if (this->unk_16A == 0) {
        this->unk_16A = 40;
        this->dyna.actor.world.rot.y += 0x8000;
        this->actionFunc = func_8087E040;
    }
}

void func_8087E040(BgHakaMeganeBG* this, PlayState* play) {
    f32 xSub;

    if (this->unk_16A != 0) {
        this->unk_16A--;
    }

    xSub = (sinf(((this->unk_16A * 0.025f) + 0.5f) * M_PI) + 1.0f) * 160.0f;

    if (this->dyna.actor.world.rot.y != this->dyna.actor.shape.rot.y) {
        xSub = 320.0f - xSub;
    }

    this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x - xSub;

    if (this->unk_16A == 0) {
        this->unk_16A = 20;
        this->actionFunc = func_8087DFF8;
    }
}

void func_8087E10C(BgHakaMeganeBG* this, PlayState* play) {
    this->dyna.actor.velocity.y += 1.0f;

    if (this->dyna.actor.velocity.y > 20.0f) {
        this->dyna.actor.velocity.y = 20.0f;
    } else {
        this->dyna.actor.velocity.y = this->dyna.actor.velocity.y;
    }

    if (this->unk_16A != 0) {
        this->unk_16A--;
    }

    if (!Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 640.0f,
                      this->dyna.actor.velocity.y)) {
        func_8002F974(&this->dyna.actor, NA_SE_EV_CHINETRAP_DOWN - SFX_FLAG);
    }

    if (this->unk_16A == 0) {
        this->unk_16A = 120;
        this->actionFunc = func_8087E1E0;
        this->dyna.actor.velocity.y = 0.0f;
    }
}

void func_8087E1E0(BgHakaMeganeBG* this, PlayState* play) {
    Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 16.0f / 3.0f);
    func_8002F974(&this->dyna.actor, NA_SE_EV_BRIDGE_CLOSE - SFX_FLAG);

    if (this->unk_16A != 0) {
        this->unk_16A--;
    }

    if (this->unk_16A == 0) {
        this->unk_16A = 80;
        this->actionFunc = func_8087E10C;
    }
}

void func_8087E258(BgHakaMeganeBG* this, PlayState* play) {
    this->dyna.actor.shape.rot.y += 0x180;
    func_8002F974(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
}

void func_8087E288(BgHakaMeganeBG* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->unk_168)) {
        OnePointCutscene_Attention(play, &this->dyna.actor);
        this->actionFunc = func_8087E2D8;
    }
}

void func_8087E2D8(BgHakaMeganeBG* this, PlayState* play) {
    Math_StepToF(&this->dyna.actor.speedXZ, 30.0f, 2.0f);

    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, this->dyna.actor.speedXZ)) {
        Actor_SetFocus(&this->dyna.actor, 50.0f);
        this->actionFunc = func_8087E34C;
    } else {
        func_8002F974(&this->dyna.actor, NA_SE_EV_METALDOOR_OPEN);
    }
}

void func_8087E34C(BgHakaMeganeBG* this, PlayState* play) {
}

void BgHakaMeganeBG_Update(Actor* thisx, PlayState* play) {
    BgHakaMeganeBG* this = (BgHakaMeganeBG*)thisx;

    this->actionFunc(this, play);
}

void BgHakaMeganeBG_Draw(Actor* thisx, PlayState* play) {
    BgHakaMeganeBG* this = (BgHakaMeganeBG*)thisx;
    s16 params = this->dyna.actor.params;

    if (params == 0) {
        Gfx_DrawDListXlu(play, object_haka_objects_DL_008EB0);
    } else {
        Gfx_DrawDListOpa(play, D_8087E410[params]);
    }
}
