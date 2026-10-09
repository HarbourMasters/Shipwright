#include "z_bg_lotus.h"
#include "objects/object_lotus/object_lotus.h"

#define FLAGS 0x00000000

void BgLotus_Init(Actor* thisx, PlayState* play);
void BgLotus_Destroy(Actor* thisx, PlayState* play);
void BgLotus_Update(Actor* thisx, PlayState* play);
void BgLotus_Draw(Actor* thisx, PlayState* play);

void func_80AD68DC(BgLotus* this, PlayState* play);
void func_80AD6A88(BgLotus* this, PlayState* play);
void func_80AD6B68(BgLotus* this, PlayState* play);

ActorProfile Bg_Lotus_Profile = {
    /**/ ACTOR_BG_LOTUS,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_LOTUS,
    /**/ sizeof(BgLotus),
    /**/ BgLotus_Init,
    /**/ BgLotus_Destroy,
    /**/ BgLotus_Update,
    /**/ BgLotus_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgLotus_Init(Actor* thisx, PlayState* play) {
    BgLotus* this = (BgLotus*)thisx;
    s32 pad;
    s32 bgId;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gLilyPadCol);
    this->dyna.actor.floorHeight = BgCheck_EntityRaycastFloor5(&play->colCtx, &this->dyna.actor.floorPoly, &bgId,
                                                               &this->dyna.actor, &this->dyna.actor.world.pos);
    this->unk168 = 0x60;
    this->dyna.actor.world.rot.y = (s32)Rand_Next() >> 0x10;
    this->actionFunc = func_80AD68DC;
}

void BgLotus_Destroy(Actor* thisx, PlayState* play) {
    BgLotus* this = (BgLotus*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80AD6830(BgLotus* this) {
    f32 temp_fv1;

    if (this->dyna.actor.params == 0) {
        temp_fv1 = Math_SinF(this->unk166 * (M_PIf / 4)) * ((0.014f * (this->unk166 / (80.0f * 0.1f))) + 0.01f);
        this->dyna.actor.scale.x = (1.0f + temp_fv1) * 0.1f;
        this->dyna.actor.scale.z = (1.0f - temp_fv1) * 0.1f;
    }
}

void func_80AD68DC(BgLotus* this, PlayState* play) {
    f32 sp34;

    this->unk168--;
    sp34 = Math_SinF(this->unk168 * (M_PIf / 48)) * 6.0f;

    if (this->dyna.actor.params == 0) {
        this->dyna.actor.world.pos.x = (Math_SinS(this->dyna.actor.world.rot.y) * sp34) + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = (Math_CosS(this->dyna.actor.world.rot.y) * sp34) + this->dyna.actor.home.pos.z;
        if (this->unk168 == 0) {
            this->unk168 = 0x60;
            this->dyna.actor.world.rot.y += (s16)((s32)Rand_Next() >> 0x12);
        }
    }
    if (this->unk160 < this->dyna.actor.floorHeight) {
        this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
    } else {
        this->dyna.actor.world.pos.y = this->unk160;
        if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
            if (!this->unk164) {
                EffectSsGRipple_Spawn(play, &this->dyna.actor.world.pos, 0x3E8, 0x578, 0);
                EffectSsGRipple_Spawn(play, &this->dyna.actor.world.pos, 0x3E8, 0x578, 8);
                this->unk166 = 40;
            }
            if (gSaveContext.save.playerForm != PLAYER_FORM_DEKU) {
                this->unk166 = 40;
                this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
                this->actionFunc = func_80AD6A88;
                return;
            }
            this->unk164 = true;
        } else {
            this->unk164 = false;
        }
    }
    if (this->unk166 > 0) {
        this->unk166--;
    }
    func_80AD6830(this);
}

void func_80AD6A88(BgLotus* this, PlayState* play) {
    if (this->dyna.actor.world.pos.y > this->unk160) {
        this->dyna.actor.world.pos.y = this->unk160;
    }
    this->dyna.actor.world.pos.y -= 1.0f;
    if (this->dyna.actor.world.pos.y <= this->dyna.actor.floorHeight) {
        this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
        this->unk166 = 0;
    }
    if (this->unk166 > 0) {
        this->unk166--;
        func_80AD6830(this);
    } else {
        if (Math_StepToF(&this->dyna.actor.scale.x, 0.0f, 5.0f * 0.001f)) {
            this->dyna.actor.draw = NULL;
            this->unk166 = 100;
            DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
            this->actionFunc = func_80AD6B68;
        }
        this->dyna.actor.scale.z = this->dyna.actor.scale.x;
    }
}

void func_80AD6B68(BgLotus* this, PlayState* play) {
    if (this->unk166 > 0) {
        this->unk166--;
    } else if ((this->dyna.actor.xzDistToPlayer > 100.0f) && (this->dyna.actor.projectedPos.z < 0.0f)) {
        this->dyna.actor.draw = BgLotus_Draw;
        DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        Actor_SetScale(&this->dyna.actor, 0.1f);
        if (this->unk160 < this->dyna.actor.floorHeight) {
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
        } else {
            this->dyna.actor.world.pos.y = this->unk160;
        }
        this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->unk168 = 0x60;
        this->actionFunc = func_80AD68DC;
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;
    }
}

void BgLotus_Update(Actor* thisx, PlayState* play) {
    BgLotus* this = (BgLotus*)thisx;
    s32 pad;
    WaterBox* waterBox;

    WaterBox_GetSurface1_2(play, &play->colCtx, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.z,
                           &this->unk160, &waterBox);
    this->actionFunc(this, play);
}

void BgLotus_Draw(Actor* thisx, PlayState* play) {
    BgLotus* this = (BgLotus*)thisx;

    Gfx_DrawDListOpa(play, gLilyPadDL);
}
