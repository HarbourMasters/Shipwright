/*
 * File: z_bg_iknin_susceil.c
 * Overlay: ovl_Bg_Iknin_Susceil
 * Description: Ikana Castle - Hot Checkered Ceiling
 */

#include "z_bg_iknin_susceil.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "objects/object_ikninside_obj/object_ikninside_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgIkninSusceil_Init(Actor* thisx, PlayState* play);
void BgIkninSusceil_Destroy(Actor* thisx, PlayState* play);
void BgIkninSusceil_Update(Actor* thisx, PlayState* play);
void BgIkninSusceil_Draw(Actor* thisx, PlayState* play);

void func_80C0AB44(BgIkninSusceil* this, PlayState* play);
void func_80C0AB88(BgIkninSusceil* this);
void func_80C0ABA8(BgIkninSusceil* this, PlayState* play);
void func_80C0AC74(BgIkninSusceil* this);
void func_80C0AC90(BgIkninSusceil* this, PlayState* play);
void func_80C0ACD4(BgIkninSusceil* this);
void func_80C0ACE8(BgIkninSusceil* this, PlayState* play);
void func_80C0AD44(BgIkninSusceil* this);
void func_80C0AD64(BgIkninSusceil* this, PlayState* play);
void func_80C0AE5C(BgIkninSusceil* this, PlayState* play);

ActorProfile Bg_Iknin_Susceil_Profile = {
    /**/ ACTOR_BG_IKNIN_SUSCEIL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_IKNINSIDE_OBJ,
    /**/ sizeof(BgIkninSusceil),
    /**/ BgIkninSusceil_Init,
    /**/ BgIkninSusceil_Destroy,
    /**/ BgIkninSusceil_Update,
    /**/ BgIkninSusceil_Draw,
};

static s32 sPad = 0;
static f32 D_80C0B0E4 = 960.0f;
static Vec2f D_80C0B0E8 = { -320.0f, 0.0f };
static s8 D_80C0B0F0[] = { 0x00, 0x00, 0x07, 0x0A, 0x0A, 0x0B, 0x0B, 0x00 };
static s8 D_80C0B0F8[] = { 0x01, 0x02, 0x00, 0x01, 0x02, 0x01, 0x02, 0x00 };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

bool func_80C0A740(BgIkninSusceil* this, PlayState* play) {
    s32 pad2[2];
    Vec3f offset;
    Player* player = GET_PLAYER(play);

    Actor_WorldToActorCoords(&this->dyna.actor, &offset, &player->actor.world.pos);

    return (D_80C0B0E8.x < offset.z) && (offset.z < D_80C0B0E8.z) && (offset.x > -240.0f) && (offset.x < D_80C0B0E4);
}

void func_80C0A804(BgIkninSusceil* this, PlayState* play) {
    DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80C0A838(BgIkninSusceil* this, PlayState* play) {
    DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgIkninSusceil_RequestQuakeAndRumble(BgIkninSusceil* this, PlayState* play, s16 quakeY, s16 quakeDuration,
                                          s32 rumbleType) {
    s32 pad;
    s16 quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

    Quake_SetSpeed(quakeIndex, 31536);
    Quake_SetPerturbations(quakeIndex, quakeY, 0, 0, 0);
    Quake_SetDuration(quakeIndex, quakeDuration);

    if (rumbleType == 1) {
        Rumble_Request(SQ(100.0f), 255, 20, 150);
    } else if (rumbleType == 2) {
        Rumble_Request(SQ(100.0f), 180, 20, 100);
    } else if (rumbleType == 3) {
        Rumble_Request(SQ(100.0f), 120, 20, 10);
    }
}

s32 func_80C0A95C(BgIkninSusceil* this, PlayState* play) {
    s32 phi_t0 = true;
    s32 i;
    f32 new_var;
    Player* player = GET_PLAYER(play);
    Vec3f offset;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp4;

    Actor_WorldToActorCoords(&this->dyna.actor, &offset, &player->actor.world.pos);
    for (i = 0; i < 7; i++) {
        temp3 = (D_80C0B0F0[i] * 80.0f) + 0.5f;
        temp4 = (D_80C0B0F0[i] * 80.0f) + 79.5f;
        if (1) {}
        new_var = D_80C0B0F8[i] * -80.0f; // temp seems to actually be needed for matching
        temp1 = new_var - 79.5f;
        temp2 = new_var - 0.5f;
        if ((temp1 < offset.z) && (offset.z < temp2) && (temp3 < offset.x) && (offset.x < temp4)) {
            phi_t0 = false;
            break;
        }
    }

    return phi_t0;
}

void BgIkninSusceil_Init(Actor* thisx, PlayState* play) {
    BgIkninSusceil* this = (BgIkninSusceil*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_ikninside_obj_Colheader_00CBAC);
    this->animatedTexture = Lib_SegmentedToVirtual(object_ikninside_obj_Matanimheader_00C670);
    func_80C0AC74(this);
}

void BgIkninSusceil_Destroy(Actor* thisx, PlayState* play) {
    BgIkninSusceil* this = (BgIkninSusceil*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80C0AB14(BgIkninSusceil* this) {
    this->timer = 110;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 365.0f;
    this->actionFunc = func_80C0AB44;
}

void func_80C0AB44(BgIkninSusceil* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer -= 1;
        return;
    }

    if (func_80C0A740(this, play)) {
        func_80C0AB88(this);
    }
}

void func_80C0AB88(BgIkninSusceil* this) {
    this->actionFunc = func_80C0ABA8;
    this->dyna.actor.velocity.y = -10.0f;
}

void func_80C0ABA8(BgIkninSusceil* this, PlayState* play) {
    this->dyna.actor.velocity.y += -0.7f;
    this->dyna.actor.velocity.y *= 0.93f;
    this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;
    if (this->dyna.actor.world.pos.y <= this->dyna.actor.home.pos.y) {
        BgIkninSusceil_RequestQuakeAndRumble(this, play, 4, 14, 1);
        Flags_UnsetSwitch(play, SUSCEIL_GET_SWITCH_FLAG(&this->dyna.actor));
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BIGWALL_BOUND);
        func_80C0AC74(this);
    } else {
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_ICE_PILLAR_FALL - SFX_FLAG);
    }
}

void func_80C0AC74(BgIkninSusceil* this) {
    this->actionFunc = func_80C0AC90;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
}

void func_80C0AC90(BgIkninSusceil* this, PlayState* play) {
    if (Flags_GetSwitch(play, SUSCEIL_GET_SWITCH_FLAG(&this->dyna.actor))) {
        func_80C0ACD4(this);
    }
}

void func_80C0ACD4(BgIkninSusceil* this) {
    this->actionFunc = func_80C0ACE8;
}

void func_80C0ACE8(BgIkninSusceil* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        func_80C0AD44(this);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void func_80C0AD44(BgIkninSusceil* this) {
    this->actionFunc = func_80C0AD64;
    this->dyna.actor.velocity.y = 5.0f;
}

void func_80C0AD64(BgIkninSusceil* this, PlayState* play) {
    this->dyna.actor.velocity.y += 0.46f;
    this->dyna.actor.velocity.y *= 0.98f;
    if (Math_SmoothStepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 365.0f, 0.5f,
                           this->dyna.actor.velocity.y, 1.0f) < 0.1f) {
        BgIkninSusceil_RequestQuakeAndRumble(this, play, 1, 14, 3);
        CutsceneManager_Stop(this->dyna.actor.csId);
        func_80C0AB14(this);
    } else {
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_ICE_PILLAR_RISING - SFX_FLAG);
    }
}

void func_80C0AE3C(BgIkninSusceil* this) {
    this->actionFunc = func_80C0AE5C;
    this->dyna.actor.velocity.y = 30.0f;
}

void func_80C0AE5C(BgIkninSusceil* this, PlayState* play) {
    this->dyna.actor.velocity.y += -0.1f;
    this->dyna.actor.velocity.y *= 0.95f;
    this->dyna.actor.velocity.y = CLAMP_MIN(this->dyna.actor.velocity.y, 1.0f);
    this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;
    if ((this->dyna.actor.home.pos.y + 365.0f) < this->dyna.actor.world.pos.y) {
        BgIkninSusceil_RequestQuakeAndRumble(this, play, 3, 14, 2);
        func_80C0AB14(this);
    }
}

void BgIkninSusceil_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    BgIkninSusceil* this = (BgIkninSusceil*)thisx;
    Player* player = GET_PLAYER(play);

    if ((this->unk168 == 0) && (this->unk166 > 0) && (player->stateFlags3 & PLAYER_STATE3_100) &&
        (player->unk_B48 > 1000.0f)) {
        this->unk168 = 2;
        if ((func_80C0A95C(this, play) != 0) && (this->actionFunc != func_80C0AE5C)) {
            Player_PlaySfx(player, NA_SE_PL_BODY_HIT);
            func_80C0AE3C(this);
        }
    }

    if (this->unk168 > 0) {
        this->unk168--;
    }

    this->actionFunc(this, play);

    if ((this->dyna.actor.home.pos.y + 70.0f) < this->dyna.actor.world.pos.y) {
        this->unk166 = 0;
    } else if (player->stateFlags3 & PLAYER_STATE3_100) {
        this->unk166 = 3;
    } else {
        if (this->unk166 > 0) {
            this->unk166--;
        }
    }
    if (this->unk166 > 0) {
        func_80C0A838(this, play);
    } else {
        func_80C0A804(this, play);
    }
}

void BgIkninSusceil_Draw(Actor* thisx, PlayState* play) {
    BgIkninSusceil* this = (BgIkninSusceil*)thisx;

    AnimatedMat_Draw(play, this->animatedTexture);
    Gfx_DrawDListOpa(play, object_ikninside_obj_DL_00C308);
}
