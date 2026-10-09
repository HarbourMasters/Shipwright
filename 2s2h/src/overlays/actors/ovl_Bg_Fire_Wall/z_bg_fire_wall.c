/*
 * File: z_bg_fire_wall.c
 * Overlay: ovl_Bg_Fire_Wall
 * Description: Wall of fire spawned by ovl_Bg_Spout_Fire
 */

#include "z_bg_fire_wall.h"
#include "objects/object_fwall/object_fwall.h"
#include "overlays/actors/ovl_En_Encount4/z_en_encount4.h"

#define FLAGS 0x00000000

void BgFireWall_Init(Actor* thisx, PlayState* play);
void BgFireWall_Destroy(Actor* thisx, PlayState* play);
void BgFireWall_Update(Actor* thisx, PlayState* play2);
void BgFireWall_Draw(Actor* thisx, PlayState* play);

void func_809AC638(BgFireWall* this, PlayState* play);
void func_809AC68C(BgFireWall* this, PlayState* play);
void func_809AC6C0(BgFireWall* this, PlayState* play);

ActorProfile Bg_Fire_Wall_Profile = {
    /**/ ACTOR_BG_FIRE_WALL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_FWALL,
    /**/ sizeof(BgFireWall),
    /**/ BgFireWall_Init,
    /**/ BgFireWall_Destroy,
    /**/ BgFireWall_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 34, 85, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 80, 100, MASS_IMMOVABLE };

static TexturePtr sFlameTextures[] = {
    gFwallFireball0Tex, gFwallFireball1Tex, gFwallFireball2Tex, gFwallFireball3Tex,
    gFwallFireball4Tex, gFwallFireball5Tex, gFwallFireball6Tex, gFwallFireball7Tex,
};

void BgFireWall_Init(Actor* thisx, PlayState* play) {
    BgFireWall* this = (BgFireWall*)thisx;

    this->unk_14C = this->actor.params;
    this->actor.scale.y = 0.005f;
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    this->actor.scale.z = 0.12f;
    this->actor.scale.x = 0.12f;
    this->unk_15C = 0.09f;
    this->unk_158 = 0.1f;
    this->unk_160 = 300.0f;
    this->texIndex = Rand_S16Offset(0, ARRAY_COUNT(sFlameTextures) - 1);
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->actionFunc = func_809AC638;
}

void BgFireWall_Destroy(Actor* thisx, PlayState* play) {
    BgFireWall* this = (BgFireWall*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 func_809AC5C0(BgFireWall* thisx, PlayState* play) {
    BgFireWall* this = (BgFireWall*)thisx;
    Player* player = GET_PLAYER(play);
    Vec3f sp1C;

    Actor_WorldToActorCoords(&this->actor, &sp1C, &player->actor.world.pos);
    if ((fabsf(sp1C.x) < this->unk_160) && (fabsf(sp1C.z) < (this->unk_160 + 20.0f))) {
        return true;
    }
    return false;
}

void func_809AC638(BgFireWall* this, PlayState* play) {
    if ((this->unk_14C != 0) || (func_809AC5C0(this, play))) {
        this->actor.draw = BgFireWall_Draw;
        this->timer = 5;
        this->actionFunc = func_809AC68C;
    }
}

void func_809AC68C(BgFireWall* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        this->actionFunc = func_809AC6C0;
    }
}

void func_809AC6C0(BgFireWall* this, PlayState* play) {
    if ((this->unk_14C != 0) || (func_809AC5C0(this, play))) {
        this->unk_15C = 0.09f;
        Math_StepToF(&this->actor.scale.y, this->unk_15C + this->unk_154, this->unk_158);
    } else if (Math_StepToF(&this->actor.scale.y, 0.005f, this->unk_158)) {
        this->actionFunc = func_809AC638;
    } else {
        this->timer = 0;
    }
}

void func_809AC760(BgFireWall* this, PlayState* play) {
    s16 phi_a3;

    if (Actor_IsFacingPlayer(&this->actor, 0x4000)) {
        phi_a3 = this->actor.shape.rot.y;
    } else {
        phi_a3 = (this->actor.shape.rot.y + 0x8000);
    }
    func_800B8D98(play, &this->actor, BREG(48) + 10.0f, phi_a3, BREG(49) + 5.0f);
}

void func_809AC7F8(BgFireWall* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp38;
    f32 val = 25.0f;
    f32 sin;
    f32 cos;

    Actor_WorldToActorCoords(&this->actor, &sp38, &player->actor.world.pos);
    sp38.x = CLAMP(sp38.x, -80.0f, 80.0f);

    if (this->step == 0) {
        if (sp38.z > 0.0f) {
            sp38.z = -val;
            this->step = -1.0f;
        } else {
            sp38.z = val;
            this->step = 1.0f;
        }
    } else {
        sp38.z = this->step * 25.0f;
    }

    sin = Math_SinS(this->actor.shape.rot.y);
    cos = Math_CosS(this->actor.shape.rot.y);
    this->collider.dim.pos.x = this->actor.world.pos.x + (sp38.x * cos) + (sp38.z * sin);
    this->collider.dim.pos.z = this->actor.world.pos.z - (sp38.x * sin) + (sp38.z * cos);
}

void func_809AC970(BgFireWall* this, PlayState* play) {
    if (Math_StepToF(&this->actor.scale.y, 0.005f, this->unk_158)) {
        Actor_Kill(&this->actor);
    }
}

void BgFireWall_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgFireWall* this = (BgFireWall*)thisx;

    this->actionFunc(this, play);
    if ((this->unk_14C == 0) || ((this->unk_14C != 0) && (this->actor.xzDistToPlayer < 240.0f))) {
        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            func_809AC760(this, play);
        }
    }
    if (this->actionFunc == func_809AC6C0) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_FIRE_PLATE - SFX_FLAG);
        if ((this->unk_14C == 0) || ((this->unk_14C != 0) && (this->actor.xzDistToPlayer < 240.0f))) {
            func_809AC7F8(this, play);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
    }
    this->texIndex = (this->texIndex + 1) % ARRAY_COUNT(sFlameTextures);
    if (this->actionFunc != func_809AC970) {
        if (this->actor.parent != NULL) {
            EnEncount4* parent = (EnEncount4*)this->actor.parent;
            if ((parent->actor.update != NULL) && (parent->timer != 0)) {
                this->actionFunc = func_809AC970;
            }
        }
    }
}

void BgFireWall_Draw(Actor* thisx, PlayState* play) {
    BgFireWall* this = (BgFireWall*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_20);
    gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(sFlameTextures[this->texIndex]));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x01, 255, 255, 0, 150);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 255);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, object_fwall_DL_000040);

    CLOSE_DISPS(play->state.gfxCtx);
}
