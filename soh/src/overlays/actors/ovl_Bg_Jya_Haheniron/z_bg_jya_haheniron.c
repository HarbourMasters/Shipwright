/*
 * File: z_bg_jya_haheniron
 * Overlay: ovl_Bg_Jya_Haheniron
 * Description: Chunks of Iron Knucle Chair and Pillar
 */

#include "z_bg_jya_haheniron.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/object_jya_iron/object_jya_iron.h"

#define FLAGS ACTOR_FLAG_4

void BgJyaHaheniron_Init(Actor* thisx, PlayState* play);
void BgJyaHaheniron_Destroy(Actor* thisx, PlayState* play);
void BgJyaHaheniron_Update(Actor* thisx, PlayState* play);
void BgJyaHaheniron_Draw(Actor* thisx, PlayState* play);

void BgJyaHaheniron_SetupChairCrumble(BgJyaHaheniron* this);
void BgJyaHaheniron_ChairCrumble(BgJyaHaheniron* this, PlayState* play);
void BgJyaHaheniron_SetupPillarCrumble(BgJyaHaheniron* this);
void BgJyaHaheniron_PillarCrumble(BgJyaHaheniron* this, PlayState* play);
void BgJyaHaheniron_SetupRubbleCollide(BgJyaHaheniron* this);
void BgJyaHaheniron_RubbleCollide(BgJyaHaheniron* this, PlayState* play);

const ActorInit Bg_Jya_Haheniron_InitVars = {
    ACTOR_BG_JYA_HAHENIRON,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_JYA_IRON,
    sizeof(BgJyaHaheniron),
    (ActorFunc)BgJyaHaheniron_Init,
    (ActorFunc)BgJyaHaheniron_Destroy,
    (ActorFunc)BgJyaHaheniron_Update,
    (ActorFunc)BgJyaHaheniron_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static s16 sKakeraScales[] = { 5, 8, 11, 14, 17 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),  ICHAIN_F32_DIV1000(minVelocityY, -15000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE), ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static f32 D_80898794[] = { 0.13f, 0.1f, 0.1f };

static Vec3f D_808987A0[] = { 0.0f, 14.0f, 0.0f };

static Vec3f D_808987AC[] = { 0.0f, 8.0f, 0.0f };

void BgJyaHaheniron_ColliderInit(BgJyaHaheniron* this, PlayState* play) {
    s32 pad;

    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderItems);
}

void BgJyaHaheniron_SpawnFragments(PlayState* play, Vec3f* vec1, Vec3f* vec2) {
    Vec3f vel;
    Vec3f pos;
    s16 arg5;
    s32 angle;
    s32 i;
    f32 rand1;

    for (angle = 0, i = 0; i < ARRAY_COUNT(sKakeraScales); i++) {
        rand1 = Rand_ZeroOne() * 10.0f;
        vel.x = (Math_SinS(angle) * rand1) + vec2->x;
        vel.y = (Rand_ZeroOne() * 10.0f) + vec2->y;
        vel.z = (Math_CosS(angle) * rand1) + vec2->z;

        rand1 = Rand_ZeroOne();
        if (rand1 < 0.2f) {
            arg5 = 96;
        } else if (rand1 < 0.8f) {
            arg5 = 64;
        } else {
            arg5 = 32;
        }

        EffectSsKakera_Spawn(play, vec1, &vel, vec1, -350, arg5, 40, 4, 0, sKakeraScales[i], 0, 20, 40,
                             KAKERA_COLOR_NONE, OBJECT_JYA_IRON, gObjectJyaIronDL_000880);
        angle += 0x3333;
    }
    pos.x = vec1->x + (vec2->x * 5.0f);
    pos.y = vec1->y + (vec2->y * 5.0f);
    pos.z = vec1->z + (vec2->z * 5.0f);
    func_80033480(play, &pos, 100.0f, 4, 100, 160, 1);
}

void BgJyaHaheniron_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgJyaHaheniron* this = (BgJyaHaheniron*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Actor_SetScale(&this->actor, D_80898794[this->actor.params]);
    if (this->actor.params == 0) {
        BgJyaHaheniron_ColliderInit(this, play);
        this->actor.shape.rot.z = (Rand_ZeroOne() * 65535.0f);
        BgJyaHaheniron_SetupChairCrumble(this);
    } else if (this->actor.params == 1) {
        BgJyaHaheniron_SetupPillarCrumble(this);
    } else if (this->actor.params == 2) {
        BgJyaHaheniron_SetupRubbleCollide(this);
    }
}

void BgJyaHaheniron_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    BgJyaHaheniron* this = (BgJyaHaheniron*)thisx;

    if (this->actor.params == 0) {
        Collider_DestroyJntSph(play, &this->collider);
    }
}

void BgJyaHaheniron_SetupChairCrumble(BgJyaHaheniron* this) {
    this->actionFunc = BgJyaHaheniron_ChairCrumble;
}

void BgJyaHaheniron_ChairCrumble(BgJyaHaheniron* this, PlayState* play) {
    Vec3f vec;

    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 5.0f, 8.0f, 0.0f, 0x85);
    if ((this->actor.bgCheckFlags & 9) || ((this->collider.base.atFlags & AT_HIT) && (this->collider.base.at != NULL) &&
                                           (this->collider.base.at->category == ACTORCAT_PLAYER))) {
        vec.x = -Rand_ZeroOne() * this->actor.velocity.x;
        vec.y = -Rand_ZeroOne() * this->actor.velocity.y;
        vec.z = -Rand_ZeroOne() * this->actor.velocity.z;
        BgJyaHaheniron_SpawnFragments(play, &this->actor.world.pos, &vec);
        Actor_Kill(&this->actor);
    } else if (this->timer > 60) {
        Actor_Kill(&this->actor);
    } else {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }
    this->actor.shape.rot.y += 0x4B0;
    this->actor.shape.rot.x += 0xFA0;
}

void BgJyaHaheniron_SetupPillarCrumble(BgJyaHaheniron* this) {
    this->actionFunc = BgJyaHaheniron_PillarCrumble;
}

void BgJyaHaheniron_PillarCrumble(BgJyaHaheniron* this, PlayState* play) {
    if (this->timer >= 8) {
        Actor_MoveForward(&this->actor);
    } else if (this->timer >= 17) {
        BgJyaHaheniron_SpawnFragments(play, &this->actor.world.pos, D_808987A0);
        Actor_Kill(&this->actor);
    }
    this->actor.shape.rot.y += 0x258;
    this->actor.shape.rot.x += 0x3E8;
}

void BgJyaHaheniron_SetupRubbleCollide(BgJyaHaheniron* this) {
    this->actionFunc = BgJyaHaheniron_RubbleCollide;
}

void BgJyaHaheniron_RubbleCollide(BgJyaHaheniron* this, PlayState* play) {
    if (this->timer >= 17) {
        BgJyaHaheniron_SpawnFragments(play, &this->actor.world.pos, D_808987AC);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 80, NA_SE_EN_IRONNACK_BREAK_PILLAR2);
        Actor_Kill(&this->actor);
    }
}

void BgJyaHaheniron_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    BgJyaHaheniron* this = (BgJyaHaheniron*)thisx;

    this->timer++;
    this->actionFunc(this, play);
}

void BgJyaHaheniron_Draw(Actor* thisx, PlayState* play) {
    static Gfx* dLists[] = {
        gObjectJyaIronDL_000880,
        gObjectJyaIronDL_000AE0,
        gObjectJyaIronDL_000600,
    };
    s32 pad;
    BgJyaHaheniron* this = (BgJyaHaheniron*)thisx;

    if (this->actor.params == 0) {
        Collider_UpdateSpheres(0, &this->collider);
    }
    Gfx_DrawDListOpa(play, dLists[this->actor.params]);
}
