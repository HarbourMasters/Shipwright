/*
 * File: z_bg_haka_huta.c
 * Overlay: ovl_Bg_Haka_Huta
 * Description: Coffin Lid
 */

#include "z_bg_haka_huta.h"
#include "objects/object_hakach_objects/object_hakach_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgHakaHuta_Init(Actor* thisx, PlayState* play);
void BgHakaHuta_Destroy(Actor* thisx, PlayState* play);
void BgHakaHuta_Update(Actor* thisx, PlayState* play);
void BgHakaHuta_Draw(Actor* thisx, PlayState* play);

void BgHakaHuta_SpawnDust(BgHakaHuta* this, PlayState* play);
void BgHakaHuta_PlaySound(BgHakaHuta* this, PlayState* play, u16 sfx);
void BgHakaHuta_SpawnEnemies(BgHakaHuta* this, PlayState* play);
void BgHakaHuta_Open(BgHakaHuta* this, PlayState* play);
void BgHakaHuta_SlideOpen(BgHakaHuta* this, PlayState* play);
void func_8087D720(BgHakaHuta* this, PlayState* play);
void BgHakaHuta_DoNothing(BgHakaHuta* this, PlayState* play);

const ActorInit Bg_Haka_Huta_InitVars = {
    ACTOR_BG_HAKA_HUTA,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HAKACH_OBJECTS,
    sizeof(BgHakaHuta),
    (ActorFunc)BgHakaHuta_Init,
    (ActorFunc)BgHakaHuta_Destroy,
    (ActorFunc)BgHakaHuta_Update,
    (ActorFunc)BgHakaHuta_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaHuta_Init(Actor* thisx, PlayState* play) {
    BgHakaHuta* this = (BgHakaHuta*)thisx;
    s16 pad;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(&gBotwCoffinLidCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
    this->unk_16A = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;
    if (Flags_GetSwitch(play, thisx->params)) {
        this->counter = -1;
        this->actionFunc = func_8087D720;
    } else {
        this->actionFunc = BgHakaHuta_SpawnEnemies;
    }
}

void BgHakaHuta_Destroy(Actor* thisx, PlayState* play) {
    BgHakaHuta* this = (BgHakaHuta*)thisx;
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgHakaHuta_SpawnDust(BgHakaHuta* this, PlayState* play) {
    static Vec3f sEffectAccel[] = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 primColor = { 30, 20, 50, 255 };
    static Color_RGBA8 envColor = { 0, 0, 0, 255 };
    f32 scale;
    f32 phi_f20;
    Vec3f effectPos;
    Vec3f effectVel;
    s32 i;
    f32 new_Xpos;
    f32 xPosOffset;

    phi_f20 = (this->dyna.actor.world.rot.y == 0) ? 1.0f : -1.0f;
    effectVel.y = 0.0f;
    effectVel.z = 0.0f;
    effectVel.x = -0.5f * phi_f20;
    effectPos.y = this->dyna.actor.world.pos.y;
    effectPos.z = this->dyna.actor.world.pos.z;
    new_Xpos = 50 - ((this->dyna.actor.world.pos.x - this->dyna.actor.home.pos.x) * phi_f20);
    xPosOffset = new_Xpos * phi_f20;

    for (i = 0; i < 4; i++) {
        if (i == 2) {
            effectPos.z += 120.0f * phi_f20;
        }
        effectPos.x = this->dyna.actor.home.pos.x - (Rand_ZeroOne() * xPosOffset);
        scale = ((Rand_ZeroOne() * 10.0f) + 50.0f);
        func_8002829C(play, &effectPos, &effectVel, sEffectAccel, &primColor, &envColor, scale, 0xA);
    }
}

void BgHakaHuta_PlaySound(BgHakaHuta* this, PlayState* play, u16 sfx) {
    Vec3f pos;

    pos.z = (this->dyna.actor.shape.rot.y == 0) ? this->dyna.actor.world.pos.z + 120.0f
                                                : this->dyna.actor.world.pos.z - 120.0f;
    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y;
    SoundSource_PlaySfxAtFixedWorldPos(play, &pos, 30, sfx);
}

void BgHakaHuta_SpawnEnemies(BgHakaHuta* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->dyna.actor.params) && !Player_InCsMode(play)) {
        this->counter = 25;
        this->actionFunc = BgHakaHuta_Open;
        OnePointCutscene_Init(play, 6001, 999, &this->dyna.actor, MAIN_CAM);
        if (this->unk_16A == 2) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_FIREFLY,
                        (this->dyna.actor.world.pos.x + (-25.0f) * Math_CosS(this->dyna.actor.shape.rot.y) +
                         40.0f * Math_SinS(this->dyna.actor.shape.rot.y)),
                        this->dyna.actor.world.pos.y - 10.0f,
                        (this->dyna.actor.world.pos.z - (-25.0f) * Math_SinS(this->dyna.actor.shape.rot.y) +
                         Math_CosS(this->dyna.actor.shape.rot.y) * 40.0f),
                        0, this->dyna.actor.shape.rot.y + 0x8000, 0, 2, true);

            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_FIREFLY,
                        (this->dyna.actor.world.pos.x + (-25.0f) * (Math_CosS(this->dyna.actor.shape.rot.y)) +
                         Math_SinS(this->dyna.actor.shape.rot.y) * 80.0f),
                        this->dyna.actor.world.pos.y - 10.0f,
                        (this->dyna.actor.world.pos.z - (-25.0f) * (Math_SinS(this->dyna.actor.shape.rot.y)) +
                         Math_CosS(this->dyna.actor.shape.rot.y) * 80.0f),
                        0, this->dyna.actor.shape.rot.y, 0, 2, true);

        } else if (this->unk_16A == 1) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_RD,
                        (this->dyna.actor.home.pos.x + (-25.0f) * (Math_CosS(this->dyna.actor.shape.rot.y)) +
                         Math_SinS(this->dyna.actor.shape.rot.y) * 100.0f),
                        this->dyna.actor.home.pos.y - 40.0f,
                        (this->dyna.actor.home.pos.z - (-25.0f) * (Math_SinS(this->dyna.actor.shape.rot.y)) +
                         Math_CosS(this->dyna.actor.shape.rot.y) * 100.0f),
                        0, this->dyna.actor.shape.rot.y, 0, 0xFD, true);
        }
    }
}

void BgHakaHuta_Open(BgHakaHuta* this, PlayState* play) {
    f32 posOffset;

    if (this->counter != 0) {
        this->counter--;
    }
    posOffset = (this->dyna.actor.world.rot.y == 0) ? 4.0f : -4.0f;
    Math_StepToF(&this->dyna.actor.world.pos.x, this->dyna.actor.home.pos.x + posOffset, 2.0f);
    if (this->counter == 0) {
        this->counter = 37;
        BgHakaHuta_PlaySound(this, play, NA_SE_EV_COFFIN_CAP_OPEN);
        this->actionFunc = BgHakaHuta_SlideOpen;
    }
}

void BgHakaHuta_SlideOpen(BgHakaHuta* this, PlayState* play) {
    f32 posOffset;

    if (this->counter != 0) {
        this->counter--;
    }
    posOffset = (this->dyna.actor.world.rot.y == 0) ? 24.0f : -24.0f;
    if (!Math_StepToF(&this->dyna.actor.world.pos.x, this->dyna.actor.home.pos.x + posOffset, 0.5f)) {
        BgHakaHuta_SpawnDust(this, play);
    }
    if (this->counter == 0) {
        BgHakaHuta_PlaySound(this, play, NA_SE_EV_COFFIN_CAP_BOUND);
        this->actionFunc = func_8087D720;
    }
}

void func_8087D720(BgHakaHuta* this, PlayState* play) {
    static Vec3f D_8087D958 = { 30.0f, 0.0f, 0.0f };
    static Vec3f D_8087D964 = { 0.03258f, 0.3258f, -0.9449f };
    MtxF mtx;
    Vec3f vec;
    s32 quakeIndex;

    this->counter++;
    if (this->counter == 6) {
        this->actionFunc = BgHakaHuta_DoNothing;
        quakeIndex = Quake_Add(GET_ACTIVE_CAM(play), 3);
        Quake_SetSpeed(quakeIndex, 0x7530);
        Quake_SetQuakeValues(quakeIndex, 4, 0, 0, 0);
        Quake_SetCountdown(quakeIndex, 2);
    } else if (this->counter == 0) {
        this->counter = 6;
        this->actionFunc = BgHakaHuta_DoNothing;
    }

    D_8087D958.x = this->counter + 24.0f;
    if (D_8087D958.x > 30.0f) {
        D_8087D958.x = 30.0f;
    }
    Matrix_RotateY(this->dyna.actor.world.rot.y * (M_PI / 0x8000), MTXMODE_NEW);
    Matrix_RotateAxis(this->counter * (191 * M_PI / 3750), &D_8087D964, MTXMODE_APPLY);
    Matrix_MultVec3f(&D_8087D958, &vec);
    this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + vec.x;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + vec.y;
    this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + vec.z;
    Matrix_Get(&mtx);
    Matrix_MtxFToYXZRotS(&mtx, &this->dyna.actor.shape.rot, 0);
}

void BgHakaHuta_DoNothing(BgHakaHuta* this, PlayState* play) {
}

void BgHakaHuta_Update(Actor* thisx, PlayState* play) {
    BgHakaHuta* this = (BgHakaHuta*)thisx;

    this->actionFunc(this, play);
}

void BgHakaHuta_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gBotwCoffinLidDL);
}
